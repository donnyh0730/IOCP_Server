#pragma once
#include "Types.h"
/*
RWLock�� ����ϴ� ����.
�Ϲ������� ����ƽ�� �����ʹ� �ٲ�� ��찡 �� ����. ���� ��� �����۸���Ʈ�� �����Ͷ���� ����Ʈ ���� �����Ͷ����
�̷� �����͵��� read�ϴ� ���� ��ȣ ��Ÿ���� �ʿ䰡 ����. �׷��� ��~~�� ���� ��ȣ��Ÿ���� �ʿ䰡 �ִµ�, 
���� ��Ÿ�� �߰��� �����͸� �����ų� �ٲپ�� �� ��Ȳ���� ���� �ɰ� ������ ���� �ٲپ� �־���ϴµ�,
�̶� �ʿ��� ���� RW���̴�. �Ӱ迵�����ٿ� Read�� ���վ��� ȿ�������� �����Ӱ� �� �� �ְ� ���ڶ�� ��ȵ� ����
RWLock�̴�. 

[WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR]
W : WriteFlag (Exclusiive Lock Owner ThreadId)
R : ReadFlag (Shared Lock Count)
��, ���� 16��Ʈ���� wirte�������� ������Id�ϳ��� �����̵ɰŰ� (�ϳ��� �� �� �����Ƿ�)
���� 16��Ʈ�� Read�����尡 ��� �����մ����� �˾ƺ����� 16��Ʈ�� ���� �� �Ŵ�.
RAII������� �Լ� ���������� �ȿ� �����صΰ� ������ ���� ȣ���ϸ�, �ڵ������� ���������� ���α׷��� �ϴ� ���. 
�����ڿ� �Ҹ��ڿ��� ���� ���� ����� ������ ȣ���Ѵ�?
��ȣ��Ÿ�� ��å
[W] -> [W]  O
[R] -> [R]  O
[R] -> [W]  X
[W] -> [R]  O
*/
class Lock//�������� ���ִ� �ϳ��� ���ؽ� ��üó�� Ȱ�� �ɰŴ�.
{
	enum : uint32
	{
		ACQUIRE_TIMEOUT_TICK = 10000,
		MAX_SPINT_COUNT = 5000,
		WRITE_THREAD_MASK = 0xFFFF'0000,
		READ_COUNT_MASK = 0x0000'FFFF,
		EMPTY_FLAG = 0x00000000
	};

public:

	void WriteLock(const char* name);
	void WriteUnlock(const char* name);
	void ReadLock(const char* name);
	void ReadUnlock(const char* name);

private :
	Atomic<uint32> _lockFlag = EMPTY_FLAG;// ���� �������Ȳ�� �˷��� 32��Ʈ �÷���
	uint16 _writeCount = 0;
};

class ReadLockGuard
{
public:
	ReadLockGuard(Lock& lock, const char* name)
		: _lock(lock) , _name(name)
	{
		_lock.ReadLock(_name);//�����Ǹ鼭 �����̸� ���簡 �������� ���� �ϵ簡
	}

	~ReadLockGuard()
	{
		_lock.ReadUnlock(_name);
	}
private:
	Lock& _lock;
	const char* _name;
};

class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock, const char* name) 
		: _lock(lock), _name(name)
	{
		_lock.WriteLock(_name);//�����Ǹ鼭 �����̸� ���簡 �������� ���� �ϵ簡
	}

	~WriteLockGuard()
	{
		_lock.WriteUnlock(_name);
	}
private:
	Lock& _lock;
	const char* _name;
};