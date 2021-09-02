#pragma once

//����� �پ��� �޸� Ǯ �� ���� �ְ� �Ұ��̴�.
//�� ���� ����� �������ִ� �޸�Ǯ�� ����� ���̳�,[32][32][32][32]... 
//ū �κп��� ���ݾ� �߶�Դ� ����� �����̳� �ΰ����� �ִ�.[    |       |      ...]

//�Ϲ������� ��ü�� ���� �Ҵ�ɶ� �ش簴ü�� ����κ��� ��������µ� 
//�� ������� ��ü�� ������� �ش簴ü�� �޸𸮰� ������ ������ �޸��ּ� ���� ������ �ִ�. 
struct MemoryHeader//�޸�Ǯ ��ĭ���� ���Ե� ������
{
	MemoryHeader(int32 size) : allocSize(size) {}

	static void* AttatchHeader(MemoryHeader* ptr, int32 size)
	{
		new(ptr)MemoryHeader(size);//ptr��ġ�� MemoryHeader�� �����ϰ� ���̵������� ������ �����Ѵ�
		return reinterpret_cast<void*>(++ptr);
	}

	static MemoryHeader* DeatachHeader(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}

	int32 allocSize;
};

class MemoryPool
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	void			Push(MemoryHeader* ptr);//�پ� ��ü�� �ݳ���.
	MemoryHeader*	Pop();//��ü�� �̾ƾ� (���θ���ų� �ݳ��Ǿ��ִ����� ��.)
private :

	USE_LOCK;

	int32 _allocSize = 0;
	atomic<int32> _useCount = 0;
	atomic<int32> _reserveCount = 0;
	queue<MemoryHeader*> _queue;//�������� ��ü�� �Ҵ�Ǵ� �޸𸮰���
};

