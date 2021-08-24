#pragma once
#include <stack>
#include <map>
#include <vector>

class DeadLockProfiler
{
public:

	void PushLock(const char* name);
	void PopLock(const char* name);
	void CheckCycle();

private:
	void Dfs(int32 index);

private:

	unordered_map<const char*, int32>	_nameToId;//락을 호출한 객체의 이름이 담기는
	unordered_map<int32, const char*>	_IdToName;

	stack<int32>						_lockStack;
	map<int32, set<int32>>				_lockHistory;//락정점 노드들 각각의 순번기록

	Mutex _lock;

private:
	vector<int32>	_discoveredOrder;//노드가 발견된 순서를 기록하는 배열.
	int32			_discoveredCount = 0;//현재 발견된 노드의 갯수.
	vector<bool>	_finished;//DFS i번째가 끝났는지 여부
	vector<int32>	_parent;
};

//lockstack은 말그대로 호출순번이 stack으로 이루어져 있어야한다. 
