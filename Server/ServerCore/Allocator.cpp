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
//���⼭ �Ϲ� new�� delete�� �ٸ���
//�Ϲ� new�� �޸� �б⾲�� ���� �ο����� �ɼ��� �ο��� �� �ִ�. 
//�Ϲ� delete�� �����Ϸ��� ���� �ٸ�����,
//Virtualfree�� �ü������ �����Ͱ� ����Ű�� �ִ� ���� �޸𸮸� �ﰢ���� ������ ��û�Ѵ�. 
/*
*  �����ڵ� 
	//[player] knight hp,mp ]
	Knight* knight = (Knight*)xnew<Player>();//��������ȯ
	//������ ĳ������ �� �̱� ������ �����÷ο� ������ �߻��Ѵ�.
	knight->hp = 100;//�����÷� ������ �޸𸮿� �����Ϸ�����.

	xdelete(knight);
*/
void* StompAllocator::Alloc(int32 size)
{
	//ex) (4 + 4095) / 4096 = 1; (4096 + 4095)/ 4096 = 1;
	//�� 4����Ʈ�� ��û�ϴ� 4096����Ʈ�� ��û�ϴ� ����� ��� �������� ����ų� ��� ���̱⶧���� �̷��� ����� ���ش�. 
	//�׸��� ��ĳ��Ʈ�� ���� �����÷ο츦 �����ϱ����ؼ� ������ ������ ���ʿ� �޸𸮸� �Ҵ��ϴ� ���̾ƴ϶�,
	//�������� ���� ���ʿ� �Ҵ��ϱ����� �۾��� �������ش�.
	const int64 pageCount = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	const int64 dataOffset = pageCount * PAGE_SIZE - size;//[[size]|           ]->[           |[size]]

	void* baseAddress = ::VirtualAlloc(NULL, pageCount * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	return static_cast<void*>(static_cast<int8*>(baseAddress) + dataOffset);
	//static_cast<int8*>�� �� ������ �������ּҸ� �����Ҷ����� Ȯ���ϰ�1����Ʈ������ ������־���ϱ⶧���� 
	//size�� 1bite�� int8*�� ĳ�����ϰ��� ������ �Ŀ� ������� �ּҸ� return�ϰ� �ȴ�.
}

void StompAllocator::Release(void* ptr)
{
	const int64 address = reinterpret_cast<int64>(ptr);//[             ][       [   ]]
	const int64 OffsetfromBase = address % PAGE_SIZE;
	const int64 baseAddress = address - OffsetfromBase;
	//OffsetfromBase == [     /    ][     /     ][|�߷��� �̺κ�|[   ]]������ ������� �޸� �ּҹ�ȣ��
	//������ ���� �������κ�
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
