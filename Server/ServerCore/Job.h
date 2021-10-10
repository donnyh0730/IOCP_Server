#pragma once
#include <functional>

using CallbackType = std::function<void()>;
//job이란 개념은 일반적으로 펑터를 하나 들고 있는 클래스이며, 이 펑터를 비동기로 실행하기위한 객체이다.
class Job
{
public:
	Job(CallbackType&& callback) : _callback(std::move(callback))
	{
	}

	template<typename T, typename Ret, typename... Args>
	Job(shared_ptr<T> owner, Ret(T::* memFunc)(Args...), Args&&... args)
	{
		_callback = [owner, memFunc, args...]()
		{
			(owner.get()->*memFunc)(args...);
		};
	}

	void Execute()
	{
		_callback();
	}

private:
	CallbackType _callback;
};

//job을 생성해서 만드는 순간 람다가 만들어지는데 이람다의 인자로 넣어준 인자들을 
//캡처하여 내부적으로 파라미터(튜플타입)을 보관하고 있게된다. 
//owner를 쉐어드포인터로 받는 이유는 일반포인터이면 이 함수를 호출할 시점에 저 객체가 소멸되어 있을 수 있기 때문에 레퍼런스카운트를 최소한 여기서 유지시켜주기 위함이다.