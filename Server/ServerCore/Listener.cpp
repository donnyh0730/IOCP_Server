#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "IocpEvent.h"
#include "Session.h"
#include "Service.h"

/*--------------
	Listener
---------------*/

Listener::~Listener()
{
	SocketUtils::Close(_socket);

	for (AcceptEvent* acceptEvent : _acceptEvents)
	{
		// TODO

		xdelete(acceptEvent);
	}
}

bool Listener::StartAccept(ServerServiceRef service)
{
	_service = service;
	if (_service == nullptr)
		return false;

	_socket = SocketUtils::CreateSocket();
	if (_socket == INVALID_SOCKET)
		return false;

	if (_service->GetIocpCore()->Register(shared_from_this()) == false)
		return false;

	if (SocketUtils::SetReuseAddress(_socket, true) == false)
		return false;

	if (SocketUtils::SetLinger(_socket, 0, 0) == false)
		return false;

	if (SocketUtils::Bind(_socket, _service->GetNetAddress()) == false)
		return false;

	if (SocketUtils::Listen(_socket) == false)
		return false;

	const int32 acceptCount = _service->GetMaxSessionCount();
	for (int32 i = 0; i < acceptCount; i++)
	{
		AcceptEvent* acceptEvent = xnew<AcceptEvent>();
		acceptEvent->owner = shared_from_this();//자기자신을 Shared_ptr로 받지만 레프카운트는 변경 없이 가져온다.
		_acceptEvents.push_back(acceptEvent);
		RegisterAccept(acceptEvent);
	}

	return true;
}

void Listener::CloseSocket()
{
	SocketUtils::Close(_socket);
}

HANDLE Listener::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Listener::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	ASSERT_CRASH(iocpEvent->eventType == EventType::Accept);
	AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
	ProcessAccept(acceptEvent);
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	//1.이 클라이언트 전용 세션,소켓을 만든다.
	SessionRef session = _service->CreateSession();//만들면서 동시에 IOCP에세션(소켓)을 등록도 해준다.

	acceptEvent->Init();
	acceptEvent->session = session;

	DWORD bytesReceived = 0;
	//기본적으로 WSAaccept()를 호출한 것이기 때문에 논블로킹 옵션을 걸지않는 이상 여기서 블로킹이 걸려있게 된다.
	if (false == SocketUtils::AcceptEx(
		_socket, //리스너 자기자신의 소켓
		session->GetSocket(), //만들어준 전용 세션에 바인딩 해주게될거다.
		session->_recvBuffer.WritePos(), 
		0, 
		sizeof(SOCKADDR_IN) + 16, 
		sizeof(SOCKADDR_IN) + 16, 
		OUT & bytesReceived, 
		static_cast<LPOVERLAPPED>(acceptEvent)))
	{
		//보통 여기로 잘 안들어오는데 들어오게되면 펜딩이 걸려있을 확율이 높다. 펜딩이 걸렸을경우도 false를 리턴하기 때문이다.
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)//에러중에 펜딩이 아닌경우는 진짜 문제가 있는경우이므로,
		{
			//재귀호출로 다시 Accept 걸어준다
			RegisterAccept(acceptEvent);
		}
	}
}
/* AcceptEx
BOOL
(PASCAL FAR * LPFN_ACCEPTEX)(
	_In_ SOCKET sListenSocket,
	_In_ SOCKET sAcceptSocket,
	_Out_writes_bytes_(dwReceiveDataLength+dwLocalAddressLength+dwRemoteAddressLength) PVOID lpOutputBuffer,
	_In_ DWORD dwReceiveDataLength,
	_In_ DWORD dwLocalAddressLength,
	_In_ DWORD dwRemoteAddressLength,
	_Out_ LPDWORD lpdwBytesReceived,
	_Inout_ LPOVERLAPPED lpOverlapped
	);
*/
void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
	SessionRef session = acceptEvent->session;

	if (false == SocketUtils::SetUpdateAcceptSocket(session->GetSocket(), _socket))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	SOCKADDR_IN sockAddress;
	int32 sizeOfSockAddr = sizeof(sockAddress);
	//sockaddress와 피어네임을 가져온다. 그런데 peername이 안가져와진다는건 버그상황
	if (SOCKET_ERROR == ::getpeername(session->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeOfSockAddr))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	session->SetNetAddress(NetAddress(sockAddress));
	session->ProcessConnect();//서버에있는 클라이언트세션

	RegisterAccept(acceptEvent);
	return;
}