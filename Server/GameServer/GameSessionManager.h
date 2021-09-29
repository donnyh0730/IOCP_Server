#pragma once

class GameSession;
using GameSessionRef = shared_ptr<GameSession>;

//전체 세션들을 들고있는 클래스, 흡사 브로드캐스터라고 보면 된다.
class GameSessionManager
{
public:

	void Add(GameSessionRef session);
	void Remove(GameSessionRef session);
	void Broadcast(SendBufferRef sendBuffer);

private:
	USE_LOCK;
	Set<GameSessionRef> _sessions;
};

extern GameSessionManager GSessionManager;