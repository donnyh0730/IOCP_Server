#include "pch.h"
#include "CoreTLS.h"
//������ ���� ���丮���� �����ԵǴ� �޸� �����̴�. 
//�����带 �����ϸ� �Ʒ��� variable���� �����ȴ�.
thread_local uint32 LThreadId = 0;
thread_local std::stack<int32>	LLockStack;
thread_local SendBufferChunkRef	LSendBufferChunk;