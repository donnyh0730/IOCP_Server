#include "pch.h"
#include "JobTimer.h"
#include "JobQueue.h"
void JobTimer::Reserve(uint64 tickAfter, weak_ptr<JobQueue> owner, JobRef job)
{
	const uint64 executeTick = ::GetTickCount64() + tickAfter;
	JobData* jobdata = ObjectPool<JobData>::Pop(owner, job);

	WRITE_LOCK;

	_items.push(TimerItem{ executeTick,jobdata });

}

void JobTimer::Distribute(uint64 currentTime)
{
	//한번에 한 쓰레드만 통과
	if (_distributing.exchange(true) == true)
		return;

	Vector<TimerItem> items;
	{
		WRITE_LOCK;
		while (_items.empty() == false)
		{
			const TimerItem& timeritem = _items.top();
			if (currentTime < timeritem.executeTick)
				break;

			items.push_back(timeritem);
			_items.pop();
		}
	}

	for (TimerItem& item : items)
	{
		if (JobQueueRef owner = item.jobData->owner.lock())
			owner->Push(item.jobData->job);

		ObjectPool<JobData>::Push(item.jobData);
	}

	_distributing.store(false);
}

void JobTimer::Clear()
{
	WRITE_LOCK;
	while (_items.empty() == false)
	{
		const TimerItem& timerItem = _items.top();
		ObjectPool<JobData>::Push(timerItem.jobData);
		_items.pop();
	}
}
