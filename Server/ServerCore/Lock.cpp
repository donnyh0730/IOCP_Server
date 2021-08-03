#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"
#include "DeadLockProfiler.h"

//여기 함수들은 쓰레드 job스택 내에서 lockguard생성시 불려지는 함수들이다. 
//따라서 리턴에 성공할시 자원획득으로(쓰레드가 메인메모리값에 접근가능) 쓰레드 내의 남은 작업을 할 수 있게 해주므로,
//while()루프를 5000번 돌거나 this_thread::yield()의 경우 경합 실패인 것이다.
void Lock::WriteLock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif

	//동일한 쓰레드가 소유할 경우 무조건 성공적으로 락을 잡을 수 있게 한다.
	const uint32 lockThread = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;//현재 점유중인 쓰레드 ID
	if (LThreadId == lockThread)
	{
		_writeCount++;//동일한 쓰레드가 또 write를 시도하면 +1을 해준다.
		return;
	}
	//1. 아무도 소유및 공유하고 있지 않을 때는 경합해서 소유권을 얻는다.
	const int64 beginTick = GetTickCount64();
	const uint32 newflag = ((LThreadId << 16) & WRITE_THREAD_MASK);
	//newflag는 [00000000][00000001][00000000][00000000] 이런식으로 만들어 지고 아래의 CAS가 성공하려면 반드시 ReadMASK가 
	//0x0000이어야만 즉, 읽고있는 쓰레드가없어야지만 Write할 수 있게 한다는 거다.
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPINT_COUNT; spinCount++)
		{
			uint32 expected = EMPTY_FLAG;
			//즉 _lockFlag가 EMPTY상태인경우(소유중인 쓰레드가 없으므로) Lockflag값을 newflag로 바꾸고  
			if (_lockFlag.compare_exchange_strong(OUT expected, newflag))//OUT단지 실패했을 경우expected의 값이 다시 _lockFlag의 값으로 덮어 씌워 질 수 있기 때문에 OUT 키워드를 붙힌것이다.
			{
				//이안으로 들어오면 임계영역을 획득한것,
				_writeCount++;
				return;
			}
		}
		if (GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");

		this_thread::yield();//경합 실패
	}
}

void Lock::WriteUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif
	//ReadLock다풀기 전에는, WriteUnLock 불가능,
	if ((_lockFlag.load() & READ_COUNT_MASK) != 0)//읽고 있는 쓰레드들이 있으면,
		CRASH("INVALID_UNLOCK_ORDER");

	const int32 lockCount = --_writeCount;

	if (lockCount == 0)
		_lockFlag.store(EMPTY_FLAG);
}

void Lock::ReadLock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif
	//동일한 쓰레드가 소유하고 있다면 무조건성공.
	//현재 점유중인 write쓰레드 ID
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId)//같은쓰레드가 write락중에 호출스택에서 또 read락을 걸려고하는 경우, 정책에 의해 while루프를 돌지않고 바로 획득한다.
	{
		_lockFlag.fetch_add(1);
		return;
	}
	//아무도 소유하고 있지 않을때는 경합해서 공유 카운트를 늘린다.
	const int64 beginTick = GetTickCount64();
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPINT_COUNT; spinCount++)
		{
			uint32 expected = (_lockFlag.load() & READ_COUNT_MASK);//지금 lock플래그를 비트연산으로 들여다 봤을때, 
			//Read하는 애들만들어와있고 상위 16비트가 깡그리다 0으로 write하는 애들이 없다고 기대하고 비교해보는 것,
			if (_lockFlag.compare_exchange_strong(OUT expected, expected + 1))//기대했던값이 실제 메모리값과 일치하면 아!
				//write중인 쓰레드가 없구나 ! 하고 바로 리턴때려서 남은 호출스택을 실햄함.
				return;
		}
		if (GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");

		this_thread::yield();//경합 실패
	}

}

void Lock::ReadUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif
	if ((_lockFlag.load() & READ_COUNT_MASK) == 0)//Read락이 없는데 풀려고 했을경우 크래시발생시킴
		CRASH("MULTIPLE_UNLOCK");

	_lockFlag.fetch_sub(1);
}
