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

		if (_queue.empty() == false)//pool에 여분이 있으면,
		{
			header = _queue.front();
			_queue.pop();
		}
	}
	//que에서 pop할 만들어진 메모리가 없을 경우에는,
	if (header == nullptr)
	{
		//새로 할당하여 만들어준다.
		header = reinterpret_cast<MemoryHeader*>(::malloc(_allocSize));
	}
	else//que에서 빼내온건데 사이즈가 0이면 문제가 있는 것임으로 크래쉬.
	{
		ASSERT_CRASH(header->allocSize == 0);
	}
	_allocCount.fetch_add(1);//이 풀에있는 객체의 카운트가 하나 늘어났다.

	return header;
}
