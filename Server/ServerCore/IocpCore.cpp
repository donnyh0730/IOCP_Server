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

bool IocpCore::Register(IocpObject* iocpObject)
{
	//ù��° �ڸ��� HANDLE�� Socket�� �ǹ��ϱ⵵ �ϱ⋚���� Socket��ſ� Handle�� �Ѱܵ� �������. 
	//���������� Handle�� Socket�� �����ֱ� �����̴�. iocpObject->GetHandle()���� _iocpHandle�� ����� �Ѵ�.
	//IocpObject�� �ᱹ�� ������ ������ �ϱ⶧���� Ű���� IocpObject�� �ּҸ� ����Ѵ�.
	return ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, reinterpret_cast<ULONG_PTR>(iocpObject), 0);
}

bool IocpCore::Dispatch(uint32 timeoutMs)
{
	DWORD numOfBytes = 0;
	IocpObject* iocpObject = nullptr;
	IocpEvent* iocpEvent = nullptr;
	//��� �����ϸ鼭 ���ø��� ��Ʈ�� �ϰ��� ���ö����� �����带 �����ִ´�.
	//��ϵ� ���Ͽ��� �ϰ��� ���� ���� �ΰ��� (iocpObject,iocpEvent)�������Ͱ� ä�����鼭 �����ϰԵȴ�.
	if (::GetQueuedCompletionStatus(_iocpHandle, OUT & numOfBytes, OUT reinterpret_cast<PULONG_PTR>(&iocpObject), OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
	{
		iocpObject->Dispatch(iocpEvent, numOfBytes);//���� �ϰ��� ���������� ���Դٸ� �ش� �̺�Ʈ�� �����Ѵ�.
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
			iocpObject->Dispatch(iocpEvent, numOfBytes);
			break;
		}
	}

	return true;
}
