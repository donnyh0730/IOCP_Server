#pragma once

class Session;

enum class EventType : uint8
{
	Connect,
	Accept,
	//PreRecv,
	Recv,
	Send
};
//이벤트 타입에따라 해당이벤트에 필요한 추가인자가 필요할 수 도 있고 안할 수 도 
//있기 때문에 이벤트 타입별로 별도의 클래스를 만들어서 관리한다.
/*--------------
	IocpEvent
---------------*/
//OVERAPPED를 상속받게되면 클래스의 데이터필드에 옵셋이 첫번째위치로 오기때문에 캐스팅에 자유롭다. 
//만약 캐스팅하기위해 상속을 받았다면 가상함수를 사용하면 안된다. 클래스 옵셋이 바뀌기 때문이다.
class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	void		Init();
	EventType	GetType() { return _type; }

protected:
	EventType	_type;
};

/*----------------
	ConnectEvent
-----------------*/

class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) { }
};

/*----------------
	AcceptEvent
-----------------*/

class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::Accept) { }

	void		SetSession(Session* session) { _session = session; }
	Session* GetSession() { return _session; }

private:
	Session* _session = nullptr;
};

/*----------------
	RecvEvent
-----------------*/

class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) { }
};

/*----------------
	SendEvent
-----------------*/

class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::Send) { }
};