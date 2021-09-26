#pragma once

/*----------------
	IocpObject
-----------------*/
//���ǰ� ����� ������ ��ü��� ����ȴ�. �ᱹ completionport�� ������ �Ѱ��ֱ����� ������� Ŭ����.
class IocpObject : public enable_shared_from_this<IocpObject>//�ڱ��ڽ��� this�����͸� sharedptr�� �Ѱ� �� �� �ֵ��� �Ѵ�.
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) abstract;
};

/*--------------
	IocpCore : ���ø��� ��Ʈ�� ����ִ� ��ü�̴�. ���������� �ϳ� �ۿ� ���� Ȯ���� ����.
---------------*/

class IocpCore
{
public:
	IocpCore();
	~IocpCore();

	HANDLE		GetHandle() { return _iocpHandle; }

	bool		Register(IocpObjectRef iocpObject);
	bool		Dispatch(uint32 timeoutMs = INFINITE);//IOCP�� �ϰ��� �ִ��� üũ�ϴ� �Լ�.

private:
	HANDLE		_iocpHandle;
};
