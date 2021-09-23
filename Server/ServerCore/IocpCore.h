#pragma once

/*----------------
	IocpObject
-----------------*/
//세션과 비슷한 느낌의 객체라고 보면된다. 결국 completionport에 정보를 넘겨주기위해 만들어진 클래스.
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
	bool		Dispatch(uint32 timeoutMs = INFINITE);//IOCP에 일감이 있는지 체크하는 함수.

private:
	HANDLE		_iocpHandle;
};

// TEMP
extern IocpCore GIocpCore;