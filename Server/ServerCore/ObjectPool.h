#pragma once
#include "Types.h"
#include "MemoryPool.h"
template <typename Type>
class ObjectPool
{
public:

	template <typename...Args>
	static Type* Pop(Args&&... args)
	{
#ifdef _STOMP
		MemoryHeader* ptr = reinterpret_cast<MemoryHeader*>(StompAllocator::Alloc(s_allocSize));
		Type* memory = static_cast<Type*>(MemoryHeader::AttatchHeader(s_pool.Pop(), s_allocSize));
#else
		Type* memory = static_cast<Type*>(MemoryHeader::AttatchHeader(s_pool.Pop(), s_allocSize));
#endif
		
		new(memory)Type(std::forward<Args>(args)...);
		return memory;
	}

	static void Push(Type* obj)
	{
		obj->~Type();
#ifdef _STOMP
		StompAllocator::Release(MemoryHeader::DeatachHeader(obj));
#else
		s_pool.Push(MemoryHeader::DeatachHeader(obj));
#endif
		
	}
	template<typename... Args>
	static shared_ptr<Type> MakeShared(Args&&... args)
	{
		//쉐어드포인터의 생성자에 할당기와 딜리터를 넣어서 쉐어드 포인터를 만들 수 있다.
		shared_ptr<Type> ptr = { Pop(forward<Args>(args)...), Push };
		return ptr;
	}

private:
	//스태틱으로 선언 했지만 Type에 따라 별개의 클래스로 동작하기 때문에 Type별로 사이즈와 풀이 존재하게된다
	static int32 s_allocSize;
	static MemoryPool s_pool;
};

template <typename Type>
int32 ObjectPool<Type>::s_allocSize = sizeof(Type) + sizeof(MemoryHeader);

template <typename Type>
MemoryPool ObjectPool<Type>::s_pool{ s_allocSize };