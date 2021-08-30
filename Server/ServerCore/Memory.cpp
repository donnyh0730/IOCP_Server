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
			//예를들면 32~63번까지는 2번째로 만들어진 32~64사이즈의 풀을 가리키게한다.
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
			//예를들면 1024~1152사이즈를 요청할경우 1024~1152사이즈풀을 테이블이 가리키게한다.
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
	const int32 allocSize = size + sizeof(MemoryHeader);//요청받은 사이즈에 헤더사이즈까지 더한다,

	if (allocSize >= MAX_ALLOC_SIZE)
	{
		//메모리풀의 최대크기를 벗어난 사이즈라면 일반 할당을해준다.
		header = reinterpret_cast<MemoryHeader*>(::malloc(allocSize));
	}
	else
	{
		header = _pooltable[allocSize]->Pop();//요청받은 사이즈의 메모리풀을 바로 찾아서 헤더주소를 넘겨준다.
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
