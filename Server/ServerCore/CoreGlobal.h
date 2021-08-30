#pragma once
//extern키워드는 외부에 전역변수공간 어딘가에 클래스와 변수가 있을거야. 라고 컴파일러에게 알려줌.

extern class ThreadManager*			GThreadManager;
extern class Memory*				GMemory;
extern class DeadLockProfiler*		GDeadLockProfiler;