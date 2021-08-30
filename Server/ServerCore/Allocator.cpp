#include "pch.h"
#include "Allocator.h"
#include "Memory.h"
/*
	BaseAllocator
*/
void* BaseAllocator::Alloc(int32 size)
{
	return ::malloc(size);
}

void BaseAllocator::Release(void* ptr)
{
	::free(ptr);
}

/*
	StompAllocator
*/
//여기서 일반 new와 delete와 다른점
//일반 new와 달리 읽기쓰기 권한 부여등의 옵션을 부여할 수 있다. 
//일반 delete는 컴파일러에 따라 다르지만,
//Virtualfree는 운영체제에게 포인터가 가리키고 있는 곳의 메모리를 즉각적인 해제를 요청한다. 
/*
*  예제코드 
	//[player] knight hp,mp ]
	Knight* knight = (Knight*)xnew<Player>();//강제형변환
	//강제로 캐스팅한 것 이기 때문에 오버플로우 문제가 발생한다.
	knight->hp = 100;//오버플로 오염된 메모리에 접근하려고함.

	xdelete(knight);
*/
void* StompAllocator::Alloc(int32 size)
{
	//ex) (4 + 4095) / 4096 = 1; (4096 + 4095)/ 4096 = 1;
	//즉 4바이트로 요청하던 4096바이트로 요청하던 결과는 몇개의 페이지를 만들거냐 라는 것이기때문에 이렇게 계산을 해준다. 
	//그리고 업캐스트에 의한 오버플로우를 감지하기위해서 페이지 단위의 앞쪽에 메모리를 할당하는 것이아니라,
	//페이지의 가장 뒤쪽에 할당하기위한 작업도 같이해준다.
	const int64 pageCount = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	const int64 dataOffset = pageCount * PAGE_SIZE - size;//[[size]|           ]->[           |[size]]

	void* baseAddress = ::VirtualAlloc(NULL, pageCount * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	return static_cast<void*>(static_cast<int8*>(baseAddress) + dataOffset);
	//static_cast<int8*>를 한 이유는 포인터주소를 연산할때에는 확실하게1바이트단위로 계산해주어야하기때문에 
	//size가 1bite인 int8*로 캐스팅하고나서 연산한 후에 만들어진 주소를 return하게 된다.
}

void StompAllocator::Release(void* ptr)
{
	const int64 address = reinterpret_cast<int64>(ptr);//[             ][       [   ]]
	const int64 OffsetfromBase = address % PAGE_SIZE;
	const int64 baseAddress = address - OffsetfromBase;
	//OffsetfromBase == [     /    ][     /     ][|잘려진 이부분|[   ]]페이지 사이즈로 메모리 주소번호를
	//나누고 남은 나머지부분
	::VirtualFree(reinterpret_cast<void*>(baseAddress), 0, MEM_RELEASE);
}

void* PoolAllocator::Alloc(int32 size)
{
	return GMemory->Allocate(size);
}

void PoolAllocator::Release(void* ptr)
{
	GMemory->Release(ptr);
}
