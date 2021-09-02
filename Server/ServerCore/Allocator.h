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
	PoolAllocator
	기본Allocator이기는 하나 전처리기 지시문에의해 분기할 수 있다.
*/
class PoolAllocator
{
public:
	static void*	Alloc(int32 size);
	static void		Release(void* ptr);
};

/*
	STL Allocator
*/
//Stl컨테이너에 커스텀한 Allocator를 사용하기위해서 구현해주어야하는 Class인터페이스
template <typename T>
class StlAllocator
{
public:
	using value_type = T;

	StlAllocator() {}
	
	template<typename other>
	StlAllocator(const StlAllocator<other>&) {}//다른타입을 컨테이너에서 복사하려할때(거의 쓰이지않음)

	T* allocate(size_t count)
	{
		const int32 size = static_cast<int32>(count * sizeof(T));
		//return static_cast<T*>(xalloc(size));
		return static_cast<T*>(PoolAllocator::Alloc(size));
	}

	void deallocate(T* ptr, size_t count)
	{
		//xrelease(ptr);
		PoolAllocator::Release(ptr);
	}
};