#pragma once
#include "Types.h"
/*
RWLock을 사용하는 이유.
일반적으로 스테틱한 데이터는 바뀌는 경우가 잘 없다. 예를 들면 아이템리스트의 데이터라든지 퀘스트 보상 데이터라든지
이런 데이터들을 read하는 것은 상호 베타적일 필요가 없다. 그러나 아~~주 가끔 상호배타적일 필요가 있는데, 
서버 런타임 중간에 데이터를 덮어씌우거나 바꾸어야 할 상황에서 락을 걸고 데이터 값을 바꾸어 주어야하는데,
이때 필요한 것이 RW락이다. 임계영역접근에 Read는 경합없이 효율적으로 자유롭게 할 수 있게 하자라고 고안된 것이
RWLock이다. 

[WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR]
W : WriteFlag (Exclusiive Lock Owner ThreadId)
R : ReadFlag (Shared Lock Count)
즉, 상위 16비트에는 wirte쓰레드의 쓰레드Id하나가 기입이될거고 (하나만 쓸 수 있으므로)
하위 16비트는 Read쓰레드가 몇개가 들어와잇는지를 알아보도록 16비트에 기입 할 거다.
RAII방식으로 함수 스택프레임 안에 선언해두고 쓰레드 에서 호출하면, 자동적으로 지워지도록 프로그래밍 하는 기법. 
생성자와 소멸자에서 각각 락과 언락을 쌍으로 호출한다?
상호배타성 정책
[W] -> [W]  O
[R] -> [R]  O
[R] -> [W]  X
[W] -> [R]  O
*/
class Lock//전역으로 떠있는 하나의 뮤텍스 객체처럼 활용 될거다.
{
	enum : uint32
	{
		ACQUIRE_TIMEOUT_TICK = 10000,
		MAX_SPINT_COUNT = 5000,
		WRITE_THREAD_MASK = 0xFFFF'0000,
		READ_COUNT_MASK = 0x0000'FFFF,
		EMPTY_FLAG = 0x00000000
	};

public:

	void WriteLock();
	void WriteUnlock();
	void ReadLock();
	void ReadUnlock();

private :
	Atomic<uint32> _lockFlag = EMPTY_FLAG;// 현재 쓰레드상황을 알려줄 32비트 플래그
	uint16 _writeCount = 0;
};

class ReadLockGuard
{
public:
	ReadLockGuard(Lock& lock) : _lock(lock)
	{
		_lock.ReadLock();//생성되면서 뺑뺑이를 돌든가 락경합을 성공 하든가
	}

	~ReadLockGuard()
	{
		_lock.ReadUnlock();
	}
private:
	Lock& _lock;
};

class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock) : _lock(lock)
	{
		_lock.WriteLock();//생성되면서 뺑뺑이를 돌든가 락경합을 성공 하든가
	}

	~WriteLockGuard()
	{
		_lock.WriteUnlock();
	}
private:
	Lock& _lock;
};