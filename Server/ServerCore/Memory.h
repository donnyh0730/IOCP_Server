#pragma once
#include "Allocator.h"

class MemoryPool;

class Memory
{
	enum {
		//1024까지 32단위,2048까지 128단위, ~4096까지 256단위로 늘어난다.
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256),
		MAX_ALLOC_SIZE = 4096
	};

public:
	Memory();
	~Memory();

	void* Allocate(int32 size);
	void Release(void* ptr);

private:
	vector<MemoryPool*> _pools;

	//O(1)메모리 풀 찾기
	//할당받으려는 메모리사이즈를 넣으면 어느풀에 할당해야할지를 바로 알수 있도록 이 배열에서 사이즈별로,
	//어디인지 바로 가리키게한다.
	MemoryPool* _pooltable[MAX_ALLOC_SIZE + 1];
};
template<typename Type>
Type* xnew()//인자를 받지 않는 기본생성 new
{
	//Type* memory = static_cast<Type*>(xalloc(sizeof(Type)));
	Type* memory = static_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));
	new(memory)Type();
	return memory;
}

template<typename Type, typename... Args>
Type* xnew(Args&&... args)//보편참조인자 파라미터가 1개이상인 모든 파라미터형태에 대해서 대응한다.
{
	//보편참조는 인자가없는 함수에대해서는 대응하지 않는다 컴파일 에러남...
	Type* memory = static_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));
	//placement new지정된 메모리 포인터위치에 생성자를 호출하라는 문법이다.
	new(memory)Type(std::forward<Args>(args)...);
	return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
	obj->~Type();
	PoolAllocator::Release(obj);

}

template<typename Type, typename... Args>
shared_ptr<Type> MakeShared(Args&&... args)
{
	return shared_ptr<Type>{ xnew<Type>(forward<Args>(args)...), xdelete<Type> };
}