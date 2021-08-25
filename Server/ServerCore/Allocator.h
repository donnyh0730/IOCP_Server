#pragma once
/*
	BaseAllocator
*/

class BaseAllocator
{
public:
	static void*	Alloc(int32 size);
	static void		Release(void* ptr);
};

/*
	StompAllocator
*/
class StompAllocator
{
	enum { PAGE_SIZE = 0x1000 };

public:
	static void*	Alloc(int32 size);
	static void		Release(void* ptr);
};

/*
	STL Allocator
*/

template <typename T>
class StlAllocator
{
public:
	using value_type = T;

	StlAllocator() {}
	
	template<typename other>
	StlAllocator(const StlAllocator<other>&) {}//다른타입을 가지는 벡터로 복사생성하려고 할때.

	T* allocate(size_t count)//벡터의 사이즈를 return하는 함수.
	{
		const int32 size = static_cast<int32>(count * sizeof(T));
		return static_cast<T*>(xalloc(size));
	}

	void deallocate(T* ptr, size_t count)
	{
		xrelease(ptr);
	}
};