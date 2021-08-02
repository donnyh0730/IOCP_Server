#pragma once
#include<mutex>
template <typename T>
class LockStack
{
public:
	LockStack(){}

	LockStack(const LockStack&) = delete; // 복사생성 불가능
	LockStack& operator = (const LockStack&) = delete; //Copy Operator도 Delete

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
	lock_guard<mutex> lock(_mutex);//스택에 푸쉬할경우 자동적으로 뮤텍스락 기반 락이 걸리도록 함.
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
	_cv.wait(lock, [this] { return _stack.empty() == false; });//데이터가 있을때 까지 대기를 하도록 한다.
	//이코드에서 쓰레드가 람다식의 조건을 만족하지않는 경우 쓰레드를 쉬게한다. 다른 쓰레드에서 notify가 오게되면 쓰레드는 다시 동작한다.
	//CV기법을 사용할때 unique_lock이 필수인 이유 : 중복 락을 허용하지 않는 유니크 락으로 뮤텍스 락을 잡아야 100퍼센트 안전하게 임계영역을 열어줄 수 있기때문이다.
	//만약 lock_guard로 해서 여러개의 중복락 때문에 락이 소멸되지 않았다면 임계영역은 계속 접근 불가가 되었을 것이다.

	val = std::move(_stack.top());//그제서야 데이터를 꺼내온다.
	_stack.pop();
}