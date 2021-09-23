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
//�̺�Ʈ Ÿ�Կ����� �ش��̺�Ʈ�� �ʿ��� �߰����ڰ� �ʿ��� �� �� �ְ� ���� �� �� 
//�ֱ� ������ �̺�Ʈ Ÿ�Ժ��� ������ Ŭ������ ���� �����Ѵ�.
/*--------------
	IocpEvent
---------------*/
//OVERAPPED�� ��ӹްԵǸ� Ŭ������ �������ʵ忡 �ɼ��� ù��°��ġ�� ���⶧���� ĳ���ÿ� �����Ӵ�. 
//���� ĳ�����ϱ����� ����� �޾Ҵٸ� �����Լ��� ����ϸ� �ȵȴ�. Ŭ���� �ɼ��� �ٲ�� �����̴�.
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