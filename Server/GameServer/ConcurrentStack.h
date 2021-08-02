#pragma once
#include<mutex>
template <typename T>
class LockStack
{
public:
	LockStack(){}

	LockStack(const LockStack&) = delete; // ������� �Ұ���
	LockStack& operator = (const LockStack&) = delete; //Copy Operator�� Delete

	void Push(T val);

	bool TryPop(T& val);

	void WaitPop(T& val);

private:

	mutex _mutex;
	stack<T> _stack;
	condition_variable _cv;

};

template <typename T>
void LockStack<T>::Push(T val)
{
	lock_guard<mutex> lock(_mutex);//���ÿ� Ǫ���Ұ�� �ڵ������� ���ؽ��� ��� ���� �ɸ����� ��.
	_stack.push(std::move(val));
	_cv.notify_one();
}

template <typename T>
bool LockStack<T>::TryPop(T& val)
{
	lock_guard<mutex> lock(_mutex);
	if (_stack.empty())
		return false;

	val = std::move(_stack.top());
	_stack.pop();
	return true;
}

template <typename T>
void LockStack<T>::WaitPop(T& val)
{
	unique_lock<mutex> lock(_mutex);
	_cv.wait(lock, [this] { return _stack.empty() == false; });//�����Ͱ� ������ ���� ��⸦ �ϵ��� �Ѵ�.
	//���ڵ忡�� �����尡 ���ٽ��� ������ ���������ʴ� ��� �����带 �����Ѵ�. �ٸ� �����忡�� notify�� ���ԵǸ� ������� �ٽ� �����Ѵ�.
	//CV����� ����Ҷ� unique_lock�� �ʼ��� ���� : �ߺ� ���� ������� �ʴ� ����ũ ������ ���ؽ� ���� ��ƾ� 100�ۼ�Ʈ �����ϰ� �Ӱ迵���� ������ �� �ֱ⶧���̴�.
	//���� lock_guard�� �ؼ� �������� �ߺ��� ������ ���� �Ҹ���� �ʾҴٸ� �Ӱ迵���� ��� ���� �Ұ��� �Ǿ��� ���̴�.

	val = std::move(_stack.top());//�������� �����͸� �����´�.
	_stack.pop();
}