#include "pch.h"
#include "CoreTLS.h"
//여기다가는 쓰레드로컬로만 사영되게될 영역이 결정된다.
thread_local uint32 LThreadId = 0;