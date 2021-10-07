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
	//�̷��� �⺻���� ���ڸ� �־��ָ� ���ø��� ��Ʈ�� ����� �غ� �ϰԵȴ�.HANDLE��� ��ü�� ����.
	ASSERT_CRASH(_iocpHandle != INVALID_HANDLE_VALUE);
}

IocpCore::~IocpCore()
{
	::CloseHandle(_iocpHandle);
}

bool IocpCore::Register(IocpObjectRef iocpObject)
{
	//ù��° �ڸ��� HANDLE�� Socket�� �ǹ��ϱ⵵ �ϱ⋚���� Socket��ſ� Handle�� �Ѱܵ� �������. 
	//���������� Handle�� Socket�� �����ֱ� �����̴�. iocpObject->GetHandle()���� _iocpHandle�� ����� �Ѵ�.
	return ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, /*key*/0, 0);
}

bool IocpCore::Dispatch(uint32 timeoutMs)
{
	DWORD numOfBytes = 0;
	ULONG_PTR key = 0;
	//��� �����ϸ鼭 ���ø��� ��Ʈ�� �ϰ��� ���ö����� �����带 �����ִ´�.
	//��ϵ� ���Ͽ��� �ϰ��� ���� ���� �ΰ��� (iocpObject,iocpEvent)�������Ͱ� ä�����鼭 �����ϰԵȴ�.
	IocpEvent* iocpEvent = nullptr;//���ʿ� ����Ҷ� eventŸ���� �������� ť�� �������Ƿ� ������ �̱�����������.
	if (::GetQueuedCompletionStatus(_iocpHandle, OUT & numOfBytes, OUT & key, OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
	{
		IocpObjectRef iocpObject = iocpEvent->owner;//���������Ҷ� �̹� ���ʰ� ����̵Ǿ� �ִ�. 
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
			// TODO : �α� ���
			IocpObjectRef iocpObject = iocpEvent->owner;
			iocpObject->Dispatch(iocpEvent, numOfBytes);
			break;
		}
	}

	return true;
}
