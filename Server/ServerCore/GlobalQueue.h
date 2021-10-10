#pragma once


class GlobalQueue
{
public:

	GlobalQueue();
	~GlobalQueue();

	void			Push(JobQueueRef jobQueue);
	JobQueueRef		Pop();

private:

	LockQueue<JobQueueRef> _jobQueues;
};
/*
* 이클래스는 직접 컨텐츠 코드에서 일감을 push하거나 pop하지 않는다. 
* 컨텐츠 코드에서는 Job큐를 상속받은 객체가 일감을 부여받게 된다.
* 
*/
