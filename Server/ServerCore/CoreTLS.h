#pragma once
#include <stack>
/*
TLS는 Thread Local Storage의 약자 이다. 
*/
extern thread_local uint32				LThreadId;
extern thread_local uint64				LEndTickCount;

extern thread_local std::stack<int32>	LLockStack;
extern thread_local SendBufferChunkRef	LSendBufferChunk;
extern thread_local class JobQueue* LCurrentJobQueue;//이 쓰레드가 현재 실행중인 쓰레드로컬의 잡큐 



