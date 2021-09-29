#include "pch.h"
#include "CoreTLS.h"
//쓰레드 로컬 스토리지에 잡히게되는 메모리 영역이다. 
//쓰레드를 생성하면 아래의 variable들이 생성된다.
thread_local uint32 LThreadId = 0;
thread_local std::stack<int32>	LLockStack;
thread_local SendBufferChunkRef	LSendBufferChunk;