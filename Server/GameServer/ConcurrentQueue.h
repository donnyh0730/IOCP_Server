#pragma once
#include <mutex>
template <typename T>
class LockQueue
{
public:
	LockQueue() {}

	LockQueue(const LockQueue&) = delete;
	LockQueue& operator = (const LockQueue&) = delete;

	void Push(T& val);
	bool TryPop(T& val);	
	void WaitPop(T& val);

private:

	queue<T> _queue;
	mutex _mutex;
	condition_variable _cv;
};


template <typename T>
void LockQueue<T>::Push(T& val)
{
	lock_guard<mutex> lock(_mutex);
	_queue.push(std::move(val));
	_cv.notify_one();
}

template <typename T>
bool LockQueue<T>::TryPop(T& val)
{
	lock_guard<mutex> lock(_mutex);
	if (_queue.empty())
		return false;

	val = std::move(_queue.front());
	_queue.pop();

	return true;
}

template <typename T>
void LockQueue<T>::WaitPop(T& val)
{
	unique_lock<mutex> lock(_mutex);
	_cv.wait(lock, [this] {return !_queue.empty(); });// ���⼭ ���ٴ� �������� ����, 
	//�� notify�� �����ʴ��� �ڵ������� wakeupchecking�� �ȴٴ� �Ҹ���!
	//�ٽ� �����ڸ� que�� ������� ������ �����尡 �����.

	val = std::move(_queue.front());
	_queue.pop();
}