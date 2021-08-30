#pragma once

//사이즈가 다양한 메모리 풀 을 갖고 있게 할것이다.
//한 블럭의 사이즈가 정해져있는 메모리풀을 사용할 것이냐,[32][32][32][32]... 
//큰 부분에서 조금씩 잘라먹는 방법을 쓸것이냐 두가지가 있다.[    |       |      ...]

//일반적으로 객체가 힙에 할당될때 해당객체의 헤더부분이 만들어지는데 
//그 헤더에는 객체의 사이즈와 해당객체의 메모리가 끝나는 지점의 메모리주소 등을 가지고 있다. 
struct MemoryHeader//메모리풀 한칸마다 들어가게될 데이터
{
	MemoryHeader(int32 size) : allocSize(size) {}

	static void* AttatchHeader(MemoryHeader* ptr, int32 size)
	{
		new(ptr)MemoryHeader(size);//ptr위치에 MemoryHeader를 기입하고 보이드포인터 형으로 리턴한다
		return reinterpret_cast<void*>(++ptr);
	}

	static MemoryHeader* GetHeader(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}

	int32 allocSize;
};

class MemoryPool
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	void			Push(MemoryHeader* ptr);//다쓴 객체를 반납함.
	MemoryHeader*	Pop();//객체를 뽑아씀 (새로만들거나 반납되어있던것을 줌.)
private :

	USE_LOCK;

	int32 _allocSize = 0;
	atomic<int32> _allocCount = 0;

	queue<MemoryHeader*> _queue;//실질적인 객체가 할당되는 메모리공간
};

