#include "pch.h"//미리 빌드된 헤더를 사용 한다. 매번 빌드시 다시 빌드되지 않으므로 성능상 유리하지만pch헤더의 내용이 바뀌는 경우 빌드 타임이 갑자기 늘어 날 수 있다.
#include "CorePch.h"//ServerCore에 있는 정적 러이브러리 API들을 인클루드 하기위해 사용한다.
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>
#include "ThreadManager.h"

#include "PlayerManager.h"
#include "AccountManager.h"

int main()
{
	GThreadManager->Launch([=] 
		{
			while (true)
			{
				cout << "player -> Account" << endl;
				GPlayerManager.PlayerthenAccount();
				this_thread::sleep_for(100ms);
			}
		});

	GThreadManager->Launch([=]
		{
			while (true)
			{
				cout << "Account -> player" << endl;
				GAccountManager.AccountThenAPlayer();
				this_thread::sleep_for(100ms);
			}
		});
	GThreadManager->Join();
}


/*아래로를 1단원 내용의 예제 소스를 주석으로 남겨둡니다.*/
/*쓰레드 예제코드 뭉치*/
/*
*
* void HelloTherad_2(int32 num)
{
	cout << num << endl;//cout은 멀티쓰레드 환경에서 순차성을 보장하지 않는다 즉 레이스컨디션에 놓인다.

	for (int32 i = 0; i < 10; ++i)
		cout << num * i << endl;
}
int main()
{
	vector<thread> v;
	thread t;//미리 생성해놓고 구동은 안시킬 수도 있다.
	for (int32 i = 0; i < 10; ++i)//for문의 중괄호도 함수스택처럼 끝나면 소멸되고 재생성된다. ㄷㄷ..
	{
		v.emplace_back(HelloTherad_2, i);
		cout << "Hello Main" << endl;

		//v[i].join();	//join() stops current thread until another one finishes. mutex stops current thread until mutex owner releases it or locks right away if it isn't locked. So these guys are quite different
		//즉 메인 호출스택에서 쓰레드들의 join을 호출하면 호출된 순간 메인쓰레드는 멈추게된다. 그리고 그 스레드가 끝날때 까지 기다리게된다. 즉 바꿔말하면 join이 호출된 시점에서 다른쓰레드의 작업이 완료되어 결과물이 리턴되길 기다려야 할때 사용될어 질 수 있는 함수이다. 이러한 기능을 가지므로 쓰레드 생성후 바로 조인을 불러주면 순차적으로 실행되므로 멀티쓰레드를 사용하는 의미가없다.
		//메인 쓰레드 생성이 모두 끝나고 join을 일괄적으로 걸어주는것이 성능을 최대로 뽑아 낼 수 있다.
	}
	for (int32 i = 0; i < 10; ++i)
	{
		if(v[i].joinable())
			v[i].join();
	}
	t = thread(HelloThread_2,7);//실제 쓰레드 동작 시점.
	//t.detach();//join과 반대 개념, 이 함수를 호출한 쓰레드가 t쓰레드가 끝날때 기다리는 행위를 하지않게 한다.(거의 사용하지 않음.)
}
*/
/*아토믹 예제코드 뭉치*/
/*
atomic<int32> sum = 0;
//int32 sum = 0;

void add()
{
	for (int32 i = 0; i < 1000000; i++)
	{
		//sum++;
		sum.fetch_add(1);//생각보다 연산이 느리므로 속도를 고려한다면 좋지 않을 수 있다.
	}
}

void sub()
{
	for (int32 i = 0; i < 1000000; i++)
	{
		//sum--;
		sum.fetch_sub(-1);
	}
}
int main()
{
	add();
	sub();

	std::thread t1(add);
	std::thread t2(sub);

	t1.join();
	t2.join();
	cout << sum << endl;

}
*/
/*Lock 예제코드 뭉치*/
/*
//[1][2][3] 벡터에 원소를 추가할때 공간이 부족하면 공간을 재할당하여 그곳에 값을 하나씩 다 복사하고 원래있던 메모리 공간은 지운다.
//[1][2][3][][][]
//벡터의 원소들은 힙에 할당받는다. 그렇다면 스택에 선언된 벡터 자체가 재할당 받을때는 스택에서 동적으로 재할당을 하는 것인가?
//스택프레임 내부에서 재할당이 가능한가? 아니라면 힙에 선언 됬다면 애초에 stl은 힙에 선언되고 스마트포인터로 래핑되어 있어야 한다는 말이 된다.
//vector<int32> v;
mutex m;
template <typename T>
class LockGuard
{
public:
	LockGuard(T& m)
	{
		_mutex = &m;
		_mutex->lock();
	}
	~LockGuard()
	{
		_mutex->unlock();
	}
private:
	T* _mutex;
};

void Push(shared_ptr<vector<int32>> v)
{
	for (int32 i = 0; i < 10000; ++i)
	{
		LockGuard<std::mutex> lockGuard(m);//생성되면서 락
		//std::lock_guard<std::mutex> lockguard(m);//위의 템플릿 클래스와 똑같은 표준 클래스.

		//std::unique_lock<std::mutex> uniquelock(m, std::defer_lock);//생성시 바로 락을 걸지 않을때 사용할 수 있다.
		//uniquelock.lock();
		//m.lock();//임계영역인 힙이나 데이터 영역에 다른 쓰레드가 접근하지못하도록 해두고 작업을 수행한다. 다른쓰레드는 자기 스레드 영역의 지역변수들에는 접근이 가능하지만
	 //임계영역은 칸막이 화장실과 같다. 만약 다른쓰레드가 락이걸려있는 임계영역에 접근하려고하면 락이 풀릴때까지 계속 기다린다. -> 이것을 교착상태라고 한다.
		v->push_back(i);
		//m.unlock();

		//if (i == 5000)
			//break;

			//LockGuard클래스는 프레임을 벗어나면서 자동으로 소멸자가 호출되므로 언락됨.
			//결국 객체의 스택영역에서의 생성과 소멸 이용하여 스마트 lock & unlock 방식을 만든것이라고 볼 수 있음.
	}
}

void process()
{
	shared_ptr<vector<int32>> v = make_shared<vector<int32>>();//힙에 할당 했을때도 뮤텍스 동작할까?->힙에 할당해도 동작한다. ㄷㄷ...
	//v->reserve(20000);
	thread t1(Push, v);
	thread t2(Push, v);

	t1.join();
	t2.join();

	cout << v->size();
}

*/
/*스핀락, CAS 예제코드 뭉치*/
/*
class Spinlock
{
public:
	void lock()
	{
		bool expectedValue = false;//
		bool newValue = true;//
		//CAS (Compare And Swap)

		//compare_exchange_strong내부 의사 코드.
		//스핀락 bool변수 (_locked의 값)이 내가 원하는 값(expectedValue)(false)과 같은 경우 bool변수(_locked)값을 새로운 값(newValue)으로 변경한다.
		//원래는 뮤텍스이전에 스핀락을 위해사용되던 코드이다. 많이 사용 하게 될지는 모르겠다...
		
		while (_locked.compare_exchange_strong(expectedValue, newValue) == false)//
		{
			expectedValue = false;

			//this_thread::sleep_for(std::chrono::milliseconds(100));
			this_thread::sleep_for(0ms);//100밀리세컨드 후에 쓰레드를 쓰레드풀에 반환한다.
			//this_thread::yield();//자신의 쓰레드를 멈추고 쓰레드 풀에 반환하므로 쓰레드 컨텍스트가 날아가버린다.
		}
		//compare_exchange_strong내부 구조.
		//if(locked == expected)//locked는 메인메모리의 값 expected는 쓰레드로컬의 값 기대했던 값과 서로 같으면, 원래 하려고했던 locked에 새로운 값을 store하고,
		//{
		//	locked = newValue;
		//	return true;
		//}
		//else //만약 아니라면 그사이에 다른쓰레드가 메인메모리의 값을 바꾼것임으로 메인메모리에 있는 값을 다시 기대값expected로 가지고와서 다시한번 같은지 검사한다.
		//{
		//	expectedValue = locked;
		//	return false;
		//}
		
	}
	void unlock()
	{
		_locked.store(false);
	}
private:
	atomic<bool> _locked = false;//volitile bool _locked = false;
	//c++ 에서 volatile키워드의 의미는 컴파일 최적화를 하지 말라고 컴파일러에게 표시해주는 기능
};
Spinlock spinlock;
mutex m;
int32 sum = 0;

void add()
{
	for (int32 i = 0; i < 1000000; i++)
	{
		lock_guard<Spinlock> guard(spinlock);
		sum++;
	}
}

void sub()
{
	for (int32 i = 0; i < 1000000; i++)
	{
		lock_guard<Spinlock> guard(spinlock);
		sum--;
	}
}
int main()
{
	add();
	sub();

	std::thread t1(add);
	std::thread t2(sub);

	t1.join();
	t2.join();
	cout << sum << endl;

}
*/
/*Sleep,Event,ConditionVariable 예제 코드*/
// mutex m;
//queue<int32> q;
//HANDLE handle;
//
////컨디션variable은 커널 오브젝트가 아님 유저레벨 오브젝트임. 프로세스 내에서 코드제어를 통해 쓰레드교통정리를 할 수 있도록 함.
//condition_variable cv;//현제는 시그널 방식을 사용하기 보다는 컨디션베리어블 방식을 일반적으로 사용하고 있다.
//
//void Producer()
//{
//	while (true)
//	{
//		//1)Lock을 잡고,
//		//2)공유변수 값을 수정,
//		//3)lock을 풀고
//		//4)조건 변수 값을 다른쓰레드에게 통지
//		{
//			unique_lock<mutex> lock(m);
//			q.push(100);
//			//중괄호가 끝나면서 뮤텍스락이 해제된다.
//		}
//
//		cv.notify_one();//wait중인 쓰레드가 있으면 딱 1개만 깨운다.
//
//		//::SetEvent(handle);//데이터 푸쉬가 끝나면 이벤트를 발생시켜 헨들을 구독중인 다른 쓰레드에게 알림.
//
//		//this_thread::sleep_for(100ms);
//	}
//}
//void Consumer()
//{
//	while (true)
//	{
//		//::WaitForSingleObject(handle, INFINITE);//운영체제가 시그널을 보낼 때까지 무한정 쓰레드를 쉬게된다.
//
//		unique_lock<mutex> lock(m);//유니크락도 경합을 통해 락을 잡으려 할 것이다.
//
//		cv.wait(lock, []() { return q.empty() == false; });//기다릴 것인데 predicate의 조건 까지만 기다린다. 즉 que가 비어있지 않을때 까지만 기다린다.
//		//1)lock을 잡고
//		//2)조건을 확인
//		//-만족 o ->빠져나와서 코드를 진행. 쓰레드를 쉬지 않음.
//		//-만족 x ->lock을 풀어주고 쓰레드를 대기상태. 즉,유니크락객체가 아직 소멸되면서 언락을 해주지 않았는데 언락이 되는 상황이다.
//		//또한 소멸자가 락을 풀기전에 중간에 락을 풀어주므로 lock의 자리에는 unique_lock객체만 사용할 수 있다. Lock_Guard는 사용할 수 없음.
//
//		//그런데 notify_one으로 깨어난 쓰레드는 어찌되었건 조건이 만족했기 때문에 nofity_one을 한것이라서 깨었으므로 굳이 람다식을 체크하지 않아도 되는 것이 아닐까?
//		//막상 노티파이로 쓰레드가 깨어나서 작업을 시작하려할래도 다른쓰레드가 그사이 또 공유데이터 내용을 바꿔두었을 수도 있기때문에 람다식을 체크하고나서 쓰레드를 진행 하는 것이 맞다.
//
//		//if (q.empty() == false)//이부분이 생략이 가능하다.
//		{
//			int32 data = q.front();
//			q.pop();
//			cout << q.size() << endl;
//			//cout << data << endl;
//		}
//	}
//}
//void ProduceAndConsume()
//{
//	//커널오브젝트 : 커널에서 관리를 할 수 있는 오브젝트이다. 어떠한 프로세스에서 커널오브젝트를 생성하면 커널에서 이값을 읽고 쓸 수 있다. 따라서 handle이라는 커널오브젝트로 커널이 발생시켜주는 이벤트의 true,false 값을 이 프로세스도 알 수 있게된다.
//	//커널 오브젝트에는 (Usage Count),(Signal / NonSignal ),(Auto / Manual) 세가지의 속성이 있다. 각각 사용되어 진 수, 시그널의 발생여부 -> 파란불/빨간불 ,커널모드가 자동적으로 제어하는지 프로세스가 코드로 제어하는지 등이 나타나있다.
//	//아래의 헨들도 커널 오브젝트인데, 시그널 발생후에 자동적으로 다시 시그널을 false상태로 바꿔둘지 아닐지 여부가 bManualReset변수이고 false일 경우 자동적으로 시그널 발생 후 false로 바꾼다.
//	handle = CreateEvent(NULL/*보안속성*/, FALSE/*bManualReset*/, FALSE/*bInitialState*/, NULL);
//
//	thread t1(Producer);
//	thread t2(Consumer);
//
//	t1.join();
//	t2.join();
//}
/*future예제 코드*/
//#include <future>
//int64 Calculate()
//{
//	int64 sum = 0;
//	for (int32 i = 0; i < 100000; i++)
//		sum += i;
//
//	return sum;
//}
//class Kinght
//{
//public:
//	int64 GetHP() { return 100; }
//};
//void PromiseWorker_Rvalueversion(std::promise<string>&& promise)//l밸류를 인자로 받게되면 값의 복사가 아니라 소유권이 넘어 오기때문에 불필요한 copy오버헤드를 줄일 수 있다.
//{
//	std::this_thread::sleep_for(100ms); 
//	promise.set_value("Hellow World");
//
//}
//void PromiseWorker_ptrversion(std::promise<string>* promise)
//{
//	std::this_thread::sleep_for(100ms);
//	if (promise != nullptr)//분명 pr이 있던 스택프레임은 삭제되었으나, 널포인터로 인식 하지 않는다. 
//		promise->set_value("Hellow World");
//	else
//		cout << "pr is deleted" << endl;
//}
//void TaskWorker(std::packaged_task<int64(void)>&& task)//만약 여기인자를 task벡터로 만들면 이쓰레드가 여러개의 작업을 하게 할 수 있음.
//{
//	task();//여기서 람다를 실행하는 순간 결과값이 task에도 저장된다. 즉 람다이면서 동시에 값을 가지고 있는것,
//	//람다로 들어온 task함수도 실행하고,future에도 값이 저장된다.
//	
//}
////예상
////template <typename T>
////class packaged_task
////{
////private:
////	std::future* _pf;
////	T (*task_ptr)(...) _tp;
////};
//void future_execute()
//{
//	//동기방식은 아래와 같다.
//	//int64 sum = Calculate();//시간이 엄청 오래걸리는 작업.
//	//cout << sum << endl;
//	{
//		//std::launch옵션 세가지
//		//1)deferred -> lazy Evaluation 지연해서 실행해주세요. 즉, get()을 한 시점에서야 실행하게된다. 별도의 쓰레드를 생성하지 않음()
//		//2)async -> 별도의 쓰레드에서 병렬 수행해주세요.
//		//3)deferred | async -> 둘 중 알아서 골라주세요.
//		std::future<int64> fu = std::async(std::launch::async, Calculate);
//		//위의 비동기방식의 코드를 설명:
//		//우선적으로 fu라는 future객체를 만들게된다. 
//		//그리고std::async라는 함수를 호출하게되면 자동적으로 쓰레드 를 생성해서 해당 job를 넘겨주게된다. 
//		//작업을 병렬로 수행하게 되므로 호출쓰레드에서 fu.get()을했을 경우 곧바로 값을 얻을 수도 있고 아닐 수도 있지만 보통 오래걸리는 작업인 경우 곧바로 값을 얻을 수 없다.
//		//std::future_status fs = fu.wait_for(1ms);
//
//		// TODO : 이 쓰레드는 동시에 다른 일을 할 수 있다. 
//
//		//fu.wait();//작업을 수행하고 있는 쓰레드가 완료될때 까지 호출쓰레드에서 기다린다.
//		fu.get();//위의 wait기능 까지 포함하는 get이다. 완료될때까지 기다렸다가 값을 리턴한다.
//
//		Kinght knight;
//		std::future<int64> future2 = std::async(std::launch::async, &Kinght::GetHP, knight);//이와 같이 클래스함수도 사용 할 수 있다.
//	}
//	thread t;
//	std::future<string> fu2;
//	{
//		std::promise<string> pr;//미래에 값이 정해실 future값이 들어있다.
//		fu2 = pr.get_future();//pr안에 있는 future의 포인터를 얻어왔다! 즉 값이저장될 위치를 알고 있으니 약속을 한 상태,
//
//		//t = std::thread(PromiseWorker_ptrversion, &pr/*std::move(pr)*/);//<- pr은 l밸류 이지만, std::move를 통해 r밸류 인것처럼 취급할 수 있다.
//		t = std::thread(PromiseWorker_Rvalueversion, std::move(pr));//이런식으로 Rvalue로 쓰레드워커에 넘겨주어야 멀티쓰레드 환경에서 안전하다.
//		//std::move를 할 경우 해당 lvalue에 대해서 소유권이 넘어가 버리기때문에 이 스택프레임 안에있는 pr은 empty가 된다. 근데 그냥힙에 할당해도 상관 없을듯 하다.
//		t.join();
//	}
//	//t.join();//만약 여기서 join했으면 스택 프레임에 있던 pr변수의 메모리 값이 날라가서 에러가 발생 한다. 
//	string message = fu2.get();
//	cout << message << endl;
//
//	{
//		//promise 랑 거의 같은데 람다(함수포인터)를 넣으라는 말이나 거의 같다. 
//		std::packaged_task<int64(void)> packaged_task(Calculate);
//		std::future<int64> future = packaged_task.get_future();
//
//		std::thread t(TaskWorker, std::move(packaged_task));
//		int64 sum = future.get();
//		cout << sum << endl;
//
//		t.join();
//	}
//}
/*스마트포인터 예제 연습*/
//#include <memory>
//unique_ptr<string> var = make_unique<string>("hello");
//unique_ptr<string> sptr = std::move(var);
//if (var == nullptr)
//	cout << "var is null ptr" << endl;
//cout << *sptr << endl;
//class testclass
//{
//public :
//	int32 num;
//	testclass(int32 _num)
//	{
//		num = _num;
//		cout << num <<" 생성 되었습니다." << endl;
//	}
//	~testclass()
//	{
//		cout << num <<" 소멸 되었습니다." << endl;
//	}
//};
//void vectorcleartest()
//{
//	vector<shared_ptr<testclass>> vec;
//	vec.reserve(100);
//	for (int32 i = 0; i < 10; i++)
//	{
//		shared_ptr<testclass> t = make_shared<testclass>(i);
//		vec.push_back(t);
//		this_thread::sleep_for(1s);
//	}
//	vec.clear();//레퍼런스 카운트를 줄여주므로 모든 객체들이 사라진다.
//	this_thread::sleep_for(3s);
//	cout << "메인 스택이 끝남" << endl;
//}
/*캐시 예제 코드*/
//int32 buffer[10000][10000];
//
//void Cache_expriment()
//{
//	memset(buffer, 0, sizeof(buffer));
//
//	{
//		uint64 start = GetTickCount64();
//		int64 sum = 0;
//		for (int32 i = 0; i < 10000; ++i)
//			for (int32 j = 0; j < 10000; ++j)
//				sum += buffer[i][j];
//
//		uint64 end = GetTickCount64();
//
//		cout << "elapsed Tick " << (end - start) << endl;
//	}
//	{
//		uint64 start = GetTickCount64();
//		int64 sum = 0;
//		for (int32 i = 0; i < 10000; ++i)
//			for (int32 j = 0; j < 10000; ++j)
//				sum += buffer[j][i];
//
//		uint64 end = GetTickCount64();
//
//		cout << "elapsed Tick " << (end - start) << endl;
//	}
//}
/*쓰레드 로컬 스토리지 예제 코드*/
//
////thread_local int32 LthreadId;//전역으로 선언해도 쓰레드 로컬키워드를 붙히면 쓰레드로컬로 사용 가능하다.
//
//void ThreadMain(int32 ThreadId)
//{
//	thread_local int32 LthreadId = ThreadId;
//	//LthreadId = ThreadId;
//	while (true)
//	{
//		cout << "hi I am thread " << LthreadId << endl;
//		this_thread::sleep_for(1s);
//	}
//}
//void ThreadLocalStorage()
//{
//	vector<thread> threads;
//
//	for (int32 i = 0; i < 10; ++i)
//	{
//		threads.push_back(thread(ThreadMain, i + 1));
//	}
//	for (thread& t : threads)
//	{
//		t.join();
//	}
//}

/*LockBasedQue,Stack*/
//#include "ConcurrentQueue.h"
//#include "ConcurrentStack.h"
//LockQueue<int32> q;
//LockStack<int32> s;
//
//void push()
//{
//	while (true)
//	{
//		int32 val = rand() % 100;
//		q.Push(val);
//
//		this_thread::sleep_for(10ms);
//	}
//}
//void consume()
//{
//	while (true)
//	{
//		int32 data = 0;
//		if (q.TryPop(OUT data))
//			cout << data << endl;
//	}
//}
//void LockBasedDataStruct()
//{
//	thread t1(push);
//	thread t2(consume);
//	thread t3(consume);
//	t1.join();
//	t2.join();
//	t3.join();
//}
//LockQueue<int32> que;
//
//void ThreadMain()
//{
//	while (true)
//	{
//		cout << "Hello!! I am thread " << LThreadId << endl;
//		this_thread::sleep_for(1s);
//	}
//}
//
//void Produce()
//{
//	while (true)
//	{
//		int32 data = rand() % 100;
//		que.Push(data);
//		this_thread::sleep_for(100ms);
//	}
//}
//void Consume()
//{
//	while (true)
//	{
//		int32 data;
//		bool val = que.TryPop(OUT data);
//		if (val)
//			cout << data << endl;
//		
//		//que.WaitPop(OUT data);
//	}
//}
//int main()
//{
//	//for (int32 i = 0; i < 5; ++i)
//	//{
//	//	GThreadManager->Launch(ThreadMain);
//	//}
//	GThreadManager->Launch(Produce);
//	GThreadManager->Launch(Consume);
//	GThreadManager->Launch(Consume);
//	GThreadManager->Join();
//}