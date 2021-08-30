#include "pch.h"
#include "Memory.h"
#include "MemoryPool.h"

Memory::Memory()
{
	int32 size = 0;
	int32 tableIndex = 0;

	for (size = 32; size <= 1024; size += 32)
	{
		//32,64,96,128...
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (tableIndex <= size)
		{
			//������� 32~63�������� 2��°�� ������� 32~64�������� Ǯ�� ����Ű���Ѵ�.
			_pooltable[tableIndex] = pool;
			tableIndex++;
		}
	}
	for (size = 1024 ;size <= 2048; size += 128)
	{
		//1024,1152,1280...
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (tableIndex <= size)
		{
			//������� 1024~1152����� ��û�Ұ�� 1024~1152������Ǯ�� ���̺��� ����Ű���Ѵ�.
			_pooltable[tableIndex] = pool;
			tableIndex++;
		}
	}
	for (size = 2048; size <= 4096; size += 256)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (tableIndex <= size)
		{
			_pooltable[tableIndex] = pool;
			tableIndex++;
		}
	}
}

Memory::~Memory()
{
	for (MemoryPool* pool : _pools)
		delete pool;

	_pools.clear();
}

void* Memory::Allocate(int32 size)
{
	MemoryHeader* header = nullptr;
	const int32 allocSize = size + sizeof(MemoryHeader);//��û���� ����� ������������ ���Ѵ�,

	if (allocSize >= MAX_ALLOC_SIZE)
	{
		//�޸�Ǯ�� �ִ�ũ�⸦ ��� �������� �Ϲ� �Ҵ������ش�.
		header = reinterpret_cast<MemoryHeader*>(::malloc(allocSize));
	}
	else
	{
		header = _pooltable[allocSize]->Pop();//��û���� �������� �޸�Ǯ�� �ٷ� ã�Ƽ� ����ּҸ� �Ѱ��ش�.
	}
	return MemoryHeader::AttatchHeader(header, allocSize);
}

void Memory::Release(void* ptr)
{
	MemoryHeader* header = MemoryHeader::GetHeader(ptr);
	const int32 allocSize = header->allocSize;
	ASSERT_CRASH(allocSize > 0);

	if (allocSize > MAX_ALLOC_SIZE)
	{
		::free(header);
	}
	else
	{
		_pooltable[allocSize]->Push(header);
	}
}
