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
	�⺻Allocator�̱�� �ϳ� ��ó���� ���ù������� �б��� �� �ִ�.
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
//Stl�����̳ʿ� Ŀ������ Allocator�� ����ϱ����ؼ� �������־���ϴ� Class�������̽�
template <typename T>
class StlAllocator
{
public:
	using value_type = T;

	StlAllocator() {}
	
	template<typename other>
	StlAllocator(const StlAllocator<other>&) {}//�ٸ�Ÿ���� �����̳ʿ��� �����Ϸ��Ҷ�(���� ����������)

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