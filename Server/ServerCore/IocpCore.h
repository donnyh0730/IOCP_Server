#pragma once

/*----------------
	IocpObject
-----------------*/
//���ǰ� ����� ������ ��ü��� ����ȴ�. �ᱹ completionport�� ������ �Ѱ��ֱ����� ������� Ŭ����.
class IocpObject
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) abstract;
};

/*--------------
	IocpCore
---------------*/

class IocpCore
{
public:
	IocpCore();
	~IocpCore();

	HANDLE		GetHandle() { return _iocpHandle; }

	bool		Register(class IocpObject* iocpObject);
	bool		Dispatch(uint32 timeoutMs = INFINITE);//IOCP�� �ϰ��� �ִ��� üũ�ϴ� �Լ�.

private:
	HANDLE		_iocpHandle;
};

// TEMP
extern IocpCore GIocpCore;