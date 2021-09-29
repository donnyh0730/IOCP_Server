#include "pch.h"
#include "DeadLockProfiler.h"

void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard guard(_lock);
	int32 lockId = 0;

	auto findit = _nameToId.find(name);//락을 사용하는 객체의 이름이 들어온다.
	if (findit == _nameToId.end())//새로운(객체) lock인 경우,
	{
		lockId = static_cast<int32>(_nameToId.size());//이 락에대한 호출을 한 객체의 이름이 관리되고 있는 map의 사이즈를 통해 넘버링을 한다. 
		_nameToId[name] = lockId;
		_IdToName[lockId] = name;
	}
	else
	{
		lockId = findit->second;
	}

	//잡고있는 락이 있다면,
	if (LLockStack.empty() == false)
	{
		//기존에 발견되지 않은 케이스 라면 데드락 여부를 다시 확인한다. 복수의 락이 복수의 쓰레드에서 동시에 잡혀버린 상황.
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

	_discoveredOrder[here] = _discoveredCount++;//오더 번호를 기록함.
	auto findit = _lockHistory.find(here);

	if (findit == _lockHistory.end())//이 노드는 지금 새로 추가된 노드 이므로 탐색할 리스트가 없다.
	{
		_finished[here] = true;
		return;
	}

	set<int32>& nextSet = findit->second;//타고 들어온 정점 간선 기록들을 찾는다.
	for (int32 there : nextSet)
	{
		if (_discoveredOrder[there] == -1)//아직 발견되지 않은 노드를 찾았다면 들어간다.
		{
			_parent[there] = here;
			Dfs(there);
			continue;
		}
		//here가 there보다 먼저 발견 되었다면, there는 here의 후손이다.(순방향 간선)
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
