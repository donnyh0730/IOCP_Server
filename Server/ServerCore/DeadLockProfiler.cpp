#include "pch.h"
#include "DeadLockProfiler.h"

void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard guard(_lock);
	int32 lockId = 0;

	auto findit = _nameToId.find(name);//���� ����ϴ� ��ü�� �̸��� ���´�.
	if (findit == _nameToId.end())//���ο�(��ü) lock�� ���,
	{
		lockId = static_cast<int32>(_nameToId.size());//�� �������� ȣ���� �� ��ü�� �̸��� �����ǰ� �ִ� map�� ����� ���� �ѹ����� �Ѵ�. 
		_nameToId[name] = lockId;
		_IdToName[lockId] = name;
	}
	else
	{
		lockId = findit->second;
	}

	//����ִ� ���� �ִٸ�,
	if (LLockStack.empty() == false)
	{
		//������ �߰ߵ��� ���� ���̽� ��� ����� ���θ� �ٽ� Ȯ���Ѵ�. ������ ���� ������ �����忡�� ���ÿ� �������� ��Ȳ.
		const int32 prevId = LLockStack.top();
		if (lockId != prevId)
		{
			set<int32>& history = _lockHistory[prevId];
			if (history.find(lockId) == history.end())
			{
				history.insert(lockId);
				CheckCycle();
			}
		}
	}

	LLockStack.push(lockId);
}

void DeadLockProfiler::PopLock(const char* name)
{
	LockGuard guard(_lock);

	if (LLockStack.empty())
		CRASH("MULTUPLE_UNLOCK");
	
	int32 lockId = _nameToId[name];
	if (LLockStack.top() != lockId)
		CRASH("INVALID_UNLOCK");

	LLockStack.pop();
}

void DeadLockProfiler::CheckCycle()
{
	const int32 lockCount = static_cast<int32>(_nameToId.size());
	_discoveredOrder = vector<int32>(lockCount, -1);
	_discoveredCount = 0;
	_finished = vector<bool>(lockCount, false);
	_parent = vector<int32>(lockCount, -1);
	
	for (int32 lockId = 0; lockId < lockCount; lockId++)
	{
		Dfs(lockId);
	}

	_discoveredOrder.clear();
	_finished.clear();
	_parent.clear();
}

void DeadLockProfiler::Dfs(int32 here)
{
	if (_discoveredOrder[here] != -1)
		return;

	_discoveredOrder[here] = _discoveredCount++;//���� ��ȣ�� �����.
	auto findit = _lockHistory.find(here);

	if (findit == _lockHistory.end())//�� ���� ���� ���� �߰��� ��� �̹Ƿ� Ž���� ����Ʈ�� ����.
	{
		_finished[here] = true;
		return;
	}

	set<int32>& nextSet = findit->second;//Ÿ�� ���� ���� ���� ��ϵ��� ã�´�.
	for (int32 there : nextSet)
	{
		if (_discoveredOrder[there] == -1)//���� �߰ߵ��� ���� ��带 ã�Ҵٸ� ����.
		{
			_parent[there] = here;
			Dfs(there);
			continue;
		}
		//here�� there���� ���� �߰� �Ǿ��ٸ�, there�� here�� �ļ��̴�.(������ ����)
		if (_discoveredOrder[here] < _discoveredOrder[there])
			continue;

		if (_finished[there] == false)
		{
			printf("%s -> %s\n", _IdToName[here], _IdToName[there]);

			int32 now = here;
			while (true)
			{
				printf("%s -> %s\n", _IdToName[_parent[now]], _IdToName[now]);
				now = _parent[now];
				if (now == there)
					break;
			}

			CRASH("DEADLOCK_DETECTED");
		}
	}
	_finished[here] = true;
}
