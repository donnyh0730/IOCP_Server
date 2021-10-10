#include "pch.h"
#include "ThreadManager.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"
#include "GlobalQueue.h"

ThreadManager::ThreadManager()
{
	//MainThread
	InitTLS();
}

ThreadManager::~ThreadManager()
{
	Join();
}

void ThreadManager::Launch(function<void(void)> callback)
{
	LockGuard guard(_lock);
	_threads.push_back(thread([=]() 
	{
			InitTLS();
			callback();
			DestroyTLS();
	}
	));
}

void ThreadManager::Join()
{
	for (thread& t : _threads)
	{
		if (t.joinable())
			t.join();
	}
	_threads.clear();
}

void ThreadManager::InitTLS()
{
	static Atomic<uint32> SThreadId = 1;
	LThreadId = SThreadId.fetch_add(1);
}

void ThreadManager::DestroyTLS()
{

}

void ThreadManager::DoGlobalQueueWork()
{
	while (true)
	{
		uint64 now = ::GetTickCount64();//현제시간을 체크한다음,
		if (now > LEndTickCount)//지정된 시간까지는 실행을 해도된다는 타임 아웃값이 있다. 지금은 64밀리세컨드.
			break;

		JobQueueRef jobQueue = GGlobalQueue->Pop();//64밀리세컨드까지 최대한 pop을 해서 일감을 처리하려고함.
		if (jobQueue == nullptr)//jobQueue가 empty여도 nullptr로 인식한다.
			break;

		jobQueue->Execute();
	}
}

void ThreadManager::DistributeReservedJobs()
{
	const uint64 now = ::GetTickCount64();
	GJobTimer->Distribute(now);
}
