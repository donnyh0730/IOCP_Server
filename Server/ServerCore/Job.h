#pragma once
#include <functional>

using CallbackType = std::function<void()>;
//job�̶� ������ �Ϲ������� ���͸� �ϳ� ��� �ִ� Ŭ�����̸�, �� ���͸� �񵿱�� �����ϱ����� ��ü�̴�.
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

//job�� �����ؼ� ����� ���� ���ٰ� ��������µ� �̶����� ���ڷ� �־��� ���ڵ��� 
//ĸó�Ͽ� ���������� �Ķ����(Ʃ��Ÿ��)�� �����ϰ� �ְԵȴ�. 
//owner�� ����������ͷ� �޴� ������ �Ϲ��������̸� �� �Լ��� ȣ���� ������ �� ��ü�� �Ҹ�Ǿ� ���� �� �ֱ� ������ ���۷���ī��Ʈ�� �ּ��� ���⼭ ���������ֱ� �����̴�.