#pragma once
#include <stack>
/*
TLS는 Thread Local Storage의 약자 이다. 
*/
extern thread_local uint32				LThreadId;
extern thread_local std::stack<int32>	LLockStack;
extern thread_local SendBufferChunkRef	LSendBufferChunk;
//1. 다른 파일에 전역으로 선언되어있는 변수,
//2. 쓰레드 로컬변수이고 쓰레드의 번호를 1~ 순차적으로 나타내는 기능 


