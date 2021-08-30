#include "pch.h"
#include "MemoryPool.h"

MemoryPool::MemoryPool(int32 allocSize) : _allocSize(allocSize)
{

}

MemoryPool::~MemoryPool()
{
	while (_queue.empty())
	{
		MemoryHeader* ptr = _queue.front();
		_queue.pop();
		::free(ptr);
	}
}

void MemoryPool::Push(MemoryHeader* ptr)
{
	WRITE_LOCK;
	ptr->allocSize = 0;

	_queue.push(ptr);
	_allocCount.fetch_sub(1);
}

MemoryHeader* MemoryPool::Pop()
{
	MemoryHeader* header = nullptr;

	{
		WRITE_LOCK;

		if (_queue.empty() == false)//pool�� ������ ������,
		{
			header = _queue.front();
			_queue.pop();
		}
	}
	//que���� pop�� ������� �޸𸮰� ���� ��쿡��,
	if (header == nullptr)
	{
		//���� �Ҵ��Ͽ� ������ش�.
		header = reinterpret_cast<MemoryHeader*>(::malloc(_allocSize));
	}
	else//que���� �����°ǵ� ����� 0�̸� ������ �ִ� �������� ũ����.
	{
		ASSERT_CRASH(header->allocSize == 0);
	}
	_allocCount.fetch_add(1);//�� Ǯ���ִ� ��ü�� ī��Ʈ�� �ϳ� �þ��.

	return header;
}
