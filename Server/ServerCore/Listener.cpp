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
	SessionRef session = _service->CreateSession(); // Register IOCP

	acceptEvent->Init();
	acceptEvent->session = session;

	DWORD bytesReceived = 0;
	//기본적으로 accept()를 호출한 것이기 때문에 논블로킹 옵션을 걸지않는 이상 여기서 블로킹이 걸려있게 된다.
	if (false == SocketUtils::AcceptEx(
		_socket, 
		session->GetSocket(), 
		session->_recvBuffer.WritePos(), 
		0, 
		sizeof(SOCKADDR_IN) + 16, 
		sizeof(SOCKADDR_IN) + 16, 
		OUT & bytesReceived, 
		static_cast<LPOVERLAPPED>(acceptEvent)))
	{
		//보통 false 인데 여기로 들어오게되면 펜딩이 걸려있을 확율이 높다. 펜딩이 걸렸을경우도 false를 리턴하기 때문이다.
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)//에러중에 펜딩이 아닌경우는 진짜 문제가 있는경우이므로,
		{
			//재귀호출로 다시 Accept 걸어준다
			RegisterAccept(acceptEvent);
		}
	}
}
/*
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
	if (SOCKET_ERROR == ::getpeername(session->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeOfSockAddr))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	session->SetNetAddress(NetAddress(sockAddress));
	session->ProcessConnect();
	RegisterAccept(acceptEvent);
}