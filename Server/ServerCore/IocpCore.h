#pragma once

/*----------------
	IocpObject
-----------------*/
//세션과 비슷한 느낌의 객체라고 보면된다. 결국 completionport에 정보를 넘겨주기위해 만들어진 클래스.
class IocpObject : public enable_shared_from_this<IocpObject>//자기자신의 this포인터를 sharedptr로 넘겨 줄 수 있도록 한다.
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) abstract;
};

/*--------------
	IocpCore : 컴플리션 포트를 들고있는 객체이다. 전역적으로 하나 밖에 없을 확율이 높다.
---------------*/

class IocpCore
{
public:
	IocpCore();
	~IocpCore();

	HANDLE		GetHandle() { return _iocpHandle; }

	bool		Register(IocpObjectRef iocpObject);
	bool		Dispatch(uint32 timeoutMs = INFINITE);//IOCP에 일감이 있는지 체크하는 함수.

private:
	HANDLE		_iocpHandle;
};
