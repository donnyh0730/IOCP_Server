#pragma once
#include "Allocator.h"

class MemoryPool;

class Memory
{
	enum {
		//1024���� 32����,2048���� 128����, ~4096���� 256������ �þ��.
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

	//O(1)�޸� Ǯ ã��
	//�Ҵ�������� �޸𸮻���� ������ ���Ǯ�� �Ҵ��ؾ������� �ٷ� �˼� �ֵ��� �� �迭���� �������,
	//������� �ٷ� ����Ű���Ѵ�.
	MemoryPool* _pooltable[MAX_ALLOC_SIZE + 1];
};
template<typename Type>
Type* xnew()//���ڸ� ���� �ʴ� �⺻���� new
{
	//Type* memory = static_cast<Type*>(xalloc(sizeof(Type)));
	Type* memory = static_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));
	new(memory)Type();
	return memory;
}

template<typename Type, typename... Args>
Type* xnew(Args&&... args)//������������ �Ķ���Ͱ� 1���̻��� ��� �Ķ�������¿� ���ؼ� �����Ѵ�.
{
	//���������� ���ڰ����� �Լ������ؼ��� �������� �ʴ´� ������ ������...
	Type* memory = static_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));
	//placement new������ �޸� ��������ġ�� �����ڸ� ȣ���϶�� �����̴�.
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