#pragma once
#include <stack>
/*
TLS�� Thread Local Storage�� ���� �̴�. 
*/
extern thread_local uint32				LThreadId;
extern thread_local std::stack<int32>	LLockStack;
extern thread_local SendBufferChunkRef	LSendBufferChunk;
//1. �ٸ� ���Ͽ� �������� ����Ǿ��ִ� ����,
//2. ������ ���ú����̰� �������� ��ȣ�� 1~ ���������� ��Ÿ���� ��� 


