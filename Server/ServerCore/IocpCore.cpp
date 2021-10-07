#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"

// TEMP
IocpCore GIocpCore;

/*--------------
	IocpCore
---------------*/

IocpCore::IocpCore()
{
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	//이렇게 기본적인 인자를 넣어주면 컴플리션 포트를 사용할 준비를 하게된다.HANDLE라는 객체를 리턴.
	ASSERT_CRASH(_iocpHandle != INVALID_HANDLE_VALUE);
}

IocpCore::~IocpCore()
{
	::CloseHandle(_iocpHandle);
}

bool IocpCore::Register(IocpObjectRef iocpObject)
{
	//첫번째 자리에 HANDLE이 Socket을 의미하기도 하기떄문에 Socket대신에 Handle을 넘겨도 상관없다. 
	//내부적으로 Handle이 Socket을 물고있기 때문이다. iocpObject->GetHandle()에서 _iocpHandle로 등록을 한다.
	return ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, /*key*/0, 0);
}

bool IocpCore::Dispatch(uint32 timeoutMs)
{
	DWORD numOfBytes = 0;
	ULONG_PTR key = 0;
	//계속 관찰하면서 컴플리션 포트에 일감이 들어올때까지 쓰레드를 쉬고있는다.
	//등록된 소켓에서 일감이 오면 위의 두개의 (iocpObject,iocpEvent)널포인터가 채워지면서 리턴하게된다.
	IocpEvent* iocpEvent = nullptr;//애초에 등록할때 event타입이 정해져서 큐에 들어가있으므로 밖으로 뽑기위한포인터.
	if (::GetQueuedCompletionStatus(_iocpHandle, OUT & numOfBytes, OUT & key, OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
	{
		IocpObjectRef iocpObject = iocpEvent->owner;//레지스터할때 이미 오너가 등록이되어 있다. 
		iocpObject->Dispatch(iocpEvent, numOfBytes);
	}
	else
	{
		int32 errCode = ::WSAGetLastError();
		switch (errCode)
		{
		case WAIT_TIMEOUT:
			return false;
		default:
			// TODO : 로그 찍기
			IocpObjectRef iocpObject = iocpEvent->owner;
			iocpObject->Dispatch(iocpEvent, numOfBytes);
			break;
		}
	}

	return true;
}
