#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"

/*--------------
	Session
---------------*/

Session::Session() :
	_recvBuffer(BUFFER_SIZE)
{
	_socket = SocketUtils::CreateSocket();
}

Session::~Session()
{
	SocketUtils::Close(_socket);
}


void Session::Send(SendBufferRef sendBuffer)//
{
	// 현재 RegisterSend가 걸리지 않은 상태라면, 걸어준다
	WRITE_LOCK;

	_sendQueue.push(sendBuffer);

	/*if (_sendRegistered == false)//현재 메인메모리의 값이 false가 맞으면
	{
		_sendRegistered = true;//이 메인메모리에 있는 값을 true로 바꾸겠다.
		//그냥 위와같은 방식으로 코드를 저렇게써버리면 각각의 쓰레드스택은 로컬스토리지의 값을 보려고 하기때문에,
		// 이런식으로 _sendRegistered.exchange(true) 아토믹 함수 (즉 메인메모리에 원자적으로 하나있고,)
		// (스레드 로컬캐쉬에 _sendRegister라는 값을 복사해두지않음)를 사용하여 값을 체크한다.
	}*/

	if (_sendRegistered.exchange(true) == false)
		RegisterSend();
}


bool Session::Connect()
{
	return RegisterConnect();
}

void Session::Disconnect(const WCHAR* cause)
{
	if (_connected.exchange(false) == false)
		return;

	// TEMP
	wcout << "Disconnect : " << cause << endl;

	RegisterDisconnect();
}

HANDLE Session::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	switch (iocpEvent->eventType)
	{
	case EventType::Connect:
		ProcessConnect();
		break;
	case EventType::Disconnect:
		ProcessDisconnect();
		break;
	case EventType::Recv:
		ProcessRecv(numOfBytes);
		break;
	case EventType::Send:
		ProcessSend(numOfBytes);
		break;
	default:
		break;
	}
}

bool Session::RegisterConnect()
{
	if (IsConnected())
		return false;

	if (GetService()->GetServiceType() != ServiceType::Client)
		return false;

	if (SocketUtils::SetReuseAddress(_socket, true) == false)
		return false;

	if (SocketUtils::BindAnyAddress(_socket, 0/*남는거*/) == false)
		return false;

	_connectEvent.Init();
	_connectEvent.owner = shared_from_this(); // ADD_REF

	DWORD numOfBytes = 0;
	SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();
	//sockAddr는 생성자에 넣어준 NetAddress(L"127.0.0.1", 7777),/*서버 주소*/를 가져온다. 
	if (false == SocketUtils::ConnectEx(_socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nullptr, 0, &numOfBytes, &_connectEvent))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_connectEvent.owner = nullptr; // RELEASE_REF
			return false;
		}
	}

	return true;
}

bool Session::RegisterDisconnect()
{
	_disconnectEvent.Init();
	_disconnectEvent.owner = shared_from_this(); // ADD_REF

	if (false == SocketUtils::DisconnectEx(_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_disconnectEvent.owner = nullptr; // RELEASE_REF
			return false;
		}
	}

	return true;
}

void Session::RegisterRecv()
{
	if (IsConnected() == false)
		return;

	_recvEvent.Init();
	_recvEvent.owner = shared_from_this(); // ADD_REF

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer.WritePos());
	wsaBuf.len = _recvBuffer.FreeSize();

	DWORD numOfBytes = 0;
	DWORD flags = 0;
	if (SOCKET_ERROR == ::WSARecv(_socket, &wsaBuf, 1, OUT & numOfBytes, OUT & flags, &_recvEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_recvEvent.owner = nullptr; // RELEASE_REF
		}
	}
}

void Session::RegisterSend()
{
	if (IsConnected() == false)
		return;

	_sendEvent.Init();
	_sendEvent.owner = shared_from_this();//Add_ref 및 오너 등록

	//세션의 샌드큐에 쌓인 데이터를 sendEvent에 벡터에 쑤셔넣는 작업.
	{
		WRITE_LOCK;
		int32 writeSize = 0;
		while (_sendQueue.empty() == false)
		{
			SendBufferRef sendBuffer = _sendQueue.front();
			writeSize += sendBuffer->WriteSize();
			//TODO : 사이즈가 너무 커지면 빠져나온다거 하는 예외처리 필요.
			_sendQueue.pop();
			_sendEvent.sendBuffers.push_back(sendBuffer);
		}

	}

	Vector<WSABUF> wsaBufs;
	wsaBufs.reserve(_sendEvent.sendBuffers.size());
	for (SendBufferRef sendBuffer : _sendEvent.sendBuffers)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
		wsaBuf.len = static_cast<LONG>(sendBuffer->WriteSize());
		wsaBufs.push_back(wsaBuf);
	}

	DWORD numOfBytes = 0;
	//IOCP큐에 이벤트타입, 데이터버퍼 사이즈들을 푸쉬해놓는다.
	if (SOCKET_ERROR == ::WSASend(_socket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), OUT & numOfBytes, 0, &_sendEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_sendEvent.owner = nullptr; // RELEASE_REF
			_sendEvent.sendBuffers.clear(); // RELEASE_REF
			_sendRegistered.store(false);
		}
	}
}
void Session::ProcessConnect()
{
	_connected.store(true);

	// 세션 등록
	GetService()->AddSession(GetSessionRef());
	//1. 서버에서 이코드로 들어온경우. 클라이언트세션을 자신이 관리하게끔 등록한다.
	//2. 클라에서 이코드로 들어온경우. 서버세션을 등록한다. 

	// 컨텐츠 코드에서 오버로딩
	OnConnected();

	// 수신 등록
	RegisterRecv();
}

void Session::ProcessDisconnect()
{
	_disconnectEvent.owner = nullptr; // RELEASE_REF

	OnDisconnected(); // 컨텐츠 코드에서 재정의
	GetService()->ReleaseSession(GetSessionRef());

}

void Session::ProcessRecv(int32 numOfBytes)
{
	_recvEvent.owner = nullptr; // RELEASE_REF

	if (numOfBytes == 0)
	{
		Disconnect(L"Recv 0");
		return;
	}

	if (_recvBuffer.OnWrite(numOfBytes) == false)
	{
		Disconnect(L"OnWirte Overflow");
		return;
	}
	int32 dataSize = _recvBuffer.DataSize();
	int32 processedLen = OnRecv(_recvBuffer.ReadPos(), dataSize);// SeverSession 컨텐츠 코드에서 재정의
	//보통은 OnRecv에서 한번에 들어온 데이터의 크기만큼 처리를 한번에 다 해주기때문에, 
	//processLen의 값이 dataSize와 똑같게 들어온다. 
	if (processedLen < 0 || dataSize < processedLen || _recvBuffer.OnRead(processedLen) == false)
	{
		Disconnect(L"OnRead Overflow");
		return;
	}
	// 옵셋 정리
	_recvBuffer.Clean();//보통 OnRecv에서 처리된 데이터사이즈와 처음에 들어온 RecvData Size가 보통 같기때문에 
	//버퍼의 ReadOffset과 WriteOffet이 0번위치로 돌아가 있게될 확율이 높다. 거의 99프로
	// 수신 등록
	RegisterRecv();
}

void Session::ProcessSend(int32 numOfBytes)
{
	//이미 iocp큐가 소켓샌드를 하고난 다음에 이부분이 실행될 것이다. 거의 정리작업.

	_sendEvent.owner = nullptr; // RELEASE_REF 
	_sendEvent.sendBuffers.clear();//RELASE_REF

	if (numOfBytes == 0)
	{
		Disconnect(L"Send 0");
		return;
	}

	// 컨텐츠 코드에서 재정의
	OnSend(numOfBytes);

	WRITE_LOCK;
	if (_sendQueue.empty())//보내고 난후에 que가 비어있다면 send할것등록여부 플래그를 false로 바꿔놓고 리턴한다.
		_sendRegistered.store(false);
	else
		RegisterSend();//멀티쓰레드 환경이므로, Dispatch도중에 또 que에 sendbuffer가 쌓일 수 있다. 다시IOCP에 등록,
}

void Session::HandleError(int32 errorCode)
{
	switch (errorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"HandleError");
		break;
	default:
		// TODO : Log
		cout << "Handle Error : " << errorCode << endl;
		break;
	}
}

/*-----------------
	PacketSession
------------------*/

PacketSession::PacketSession()
{
}

PacketSession::~PacketSession()
{
}

// [size(2)][id(2)][data....][size(2)][id(2)][data....]
int32 PacketSession::OnRecv(BYTE* buffer, int32 len)//len = 실제 받은 잘린경우도 있다.
{
	int32 processLen = 0;

	while (true)
	{
		int32 dataSize = len - processLen;
		if (dataSize < sizeof(PacketHeader))
			break;

		PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));
		// 헤더에 기록된 패킷 크기를 파싱할 수 있어야 한다
		if (dataSize < header.size)
			break;

		// 패킷 조립 성공
		OnRecvPacket(&buffer[processLen], header.size);

		processLen += header.size;
	}

	return processLen;
}