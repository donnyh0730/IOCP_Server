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

	unordered_map<const char*, int32>	_nameToId;//���� ȣ���� ��ü�� �̸��� ����
	unordered_map<int32, const char*>	_IdToName;

	stack<int32>						_lockStack;
	map<int32, set<int32>>				_lockHistory;//������ ���� ������ �������

	Mutex _lock;

private:
	vector<int32>	_discoveredOrder;//��尡 �߰ߵ� ������ ����ϴ� �迭.
	int32			_discoveredCount = 0;//���� �߰ߵ� ����� ����.
	vector<bool>	_finished;//DFS i��°�� �������� ����
	vector<int32>	_parent;
};

//lockstack�� ���״�� ȣ������� stack���� �̷���� �־���Ѵ�. 