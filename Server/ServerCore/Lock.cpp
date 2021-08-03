#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"
#include "DeadLockProfiler.h"

//���� �Լ����� ������ job���� ������ lockguard������ �ҷ����� �Լ����̴�. 
//���� ���Ͽ� �����ҽ� �ڿ�ȹ������(�����尡 ���θ޸𸮰��� ���ٰ���) ������ ���� ���� �۾��� �� �� �ְ� ���ֹǷ�,
//while()������ 5000�� ���ų� this_thread::yield()�� ��� ���� ������ ���̴�.
void Lock::WriteLock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif

	//������ �����尡 ������ ��� ������ ���������� ���� ���� �� �ְ� �Ѵ�.
	const uint32 lockThread = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;//���� �������� ������ ID
	if (LThreadId == lockThread)
	{
		_writeCount++;//������ �����尡 �� write�� �õ��ϸ� +1�� ���ش�.
		return;
	}
	//1. �ƹ��� ������ �����ϰ� ���� ���� ���� �����ؼ� �������� ��´�.
	const int64 beginTick = GetTickCount64();
	const uint32 newflag = ((LThreadId << 16) & WRITE_THREAD_MASK);
	//newflag�� [00000000][00000001][00000000][00000000] �̷������� ����� ���� �Ʒ��� CAS�� �����Ϸ��� �ݵ�� ReadMASK�� 
	//0x0000�̾�߸� ��, �а��ִ� �����尡��������� Write�� �� �ְ� �Ѵٴ� �Ŵ�.
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPINT_COUNT; spinCount++)
		{
			uint32 expected = EMPTY_FLAG;
			//�� _lockFlag�� EMPTY�����ΰ��(�������� �����尡 �����Ƿ�) Lockflag���� newflag�� �ٲٰ�  
			if (_lockFlag.compare_exchange_strong(OUT expected, newflag))//OUT���� �������� ���expected�� ���� �ٽ� _lockFlag�� ������ ���� ���� �� �� �ֱ� ������ OUT Ű���带 �������̴�.
			{
				//�̾����� ������ �Ӱ迵���� ȹ���Ѱ�,
				_writeCount++;
				return;
			}
		}
		if (GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");

		this_thread::yield();//���� ����
	}
}

void Lock::WriteUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif
	//ReadLock��Ǯ�� ������, WriteUnLock �Ұ���,
	if ((_lockFlag.load() & READ_COUNT_MASK) != 0)//�а� �ִ� ��������� ������,
		CRASH("INVALID_UNLOCK_ORDER");

	const int32 lockCount = --_writeCount;

	if (lockCount == 0)
		_lockFlag.store(EMPTY_FLAG);
}

void Lock::ReadLock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif
	//������ �����尡 �����ϰ� �ִٸ� �����Ǽ���.
	//���� �������� write������ ID
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId)//���������尡 write���߿� ȣ�⽺�ÿ��� �� read���� �ɷ����ϴ� ���, ��å�� ���� while������ �����ʰ� �ٷ� ȹ���Ѵ�.
	{
		_lockFlag.fetch_add(1);
		return;
	}
	//�ƹ��� �����ϰ� ���� �������� �����ؼ� ���� ī��Ʈ�� �ø���.
	const int64 beginTick = GetTickCount64();
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPINT_COUNT; spinCount++)
		{
			uint32 expected = (_lockFlag.load() & READ_COUNT_MASK);//���� lock�÷��׸� ��Ʈ�������� �鿩�� ������, 
			//Read�ϴ� �ֵ鸸�����ְ� ���� 16��Ʈ�� ���׸��� 0���� write�ϴ� �ֵ��� ���ٰ� ����ϰ� ���غ��� ��,
			if (_lockFlag.compare_exchange_strong(OUT expected, expected + 1))//����ߴ����� ���� �޸𸮰��� ��ġ�ϸ� ��!
				//write���� �����尡 ������ ! �ϰ� �ٷ� ���϶����� ���� ȣ�⽺���� ������.
				return;
		}
		if (GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");

		this_thread::yield();//���� ����
	}

}

void Lock::ReadUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif
	if ((_lockFlag.load() & READ_COUNT_MASK) == 0)//Read���� ���µ� Ǯ���� ������� ũ���ù߻���Ŵ
		CRASH("MULTIPLE_UNLOCK");

	_lockFlag.fetch_sub(1);
}
