#pragma once
#include "NetAddress.h"
#include "IocpCore.h"
#include "Listener.h"
#include <functional>

enum class ServiceType : uint8
{
	Server,
	Client
};


//SessionRef를 뱉어주는 함수의 이름을 SessionFactory라고 이름을 붙힌것. 
//GameServer쪽의 코드를 보면 MakeShared<Session>처럼 SessionRef를 리턴하는 함수자체를 인자로 받았다. 즉 세션을 만들어주는 외부 함수를 service객체에서 펑션포인터를 받아서 사용하겠다는 이야기.
using SessionFactory = function<SessionRef(void)>;
/*Service
이친구는 IocpCore라는 컴플리션포트와 Session의 Set을 가지고있다 즉, 클라이언트 세션들에 대한 관리
밑 각각클라이언트의 소켓을 통한 컴플리션포트에 들어오는 이벤트 타입에 따라 서버 서비스를 주체하므로
서버 어플리케이션의 본체 라고 할 수 있겠다. 
주의 : 서비스는 session을 상속받은 클래스가 아니다.
*/
class Service : public enable_shared_from_this<Service>
{
public:
	Service(ServiceType type, NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~Service();

	virtual bool		Start() abstract;
	bool				CanStart() { return _sessionFactory != nullptr; }

	virtual void		CloseService();
	void				SetSessionFactory(SessionFactory func) { _sessionFactory = func; }


	void				Broadcast(SendBufferRef sendBuffer);
	SessionRef			CreateSession();
	void				AddSession(SessionRef session);
	void				ReleaseSession(SessionRef session);
	int32				GetCurrentSessionCount() { return _sessionCount; }
	int32				GetMaxSessionCount() { return _maxSessionCount; }

public:
	ServiceType			GetServiceType() { return _type; }
	NetAddress			GetNetAddress() { return _netAddress; }
	IocpCoreRef&		GetIocpCore() { return _iocpCore; }

protected:
	USE_LOCK;
	ServiceType			_type;
	NetAddress			_netAddress = {};
	IocpCoreRef			_iocpCore;//거의 CompletionPort라고 보면됨.

	Set<SessionRef>		_sessions;//연결된 세션들
	int32				_sessionCount = 0;
	int32				_maxSessionCount = 0;
	SessionFactory		_sessionFactory;
};

/*-----------------
	ClientService
------------------*/

class ClientService : public Service
{
public:
	ClientService(NetAddress targetAddress, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~ClientService() {}

	virtual bool	Start() override;
};


/*-----------------
	ServerService
------------------*/

class ServerService : public Service
{
public:
	ServerService(NetAddress targetAddress, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~ServerService() {}

	virtual bool	Start() override;
	virtual void	CloseService() override;

private:
	ListenerRef		_listener = nullptr;//새로접속하는 클라를 받기위한 리스너 객체 
};