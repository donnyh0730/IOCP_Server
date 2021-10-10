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
* ��Ŭ������ ���� ������ �ڵ忡�� �ϰ��� push�ϰų� pop���� �ʴ´�. 
* ������ �ڵ忡���� Jobť�� ��ӹ��� ��ü�� �ϰ��� �ο��ް� �ȴ�.
* 
*/
