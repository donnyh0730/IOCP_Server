#pragma once
//처음 생성된 그냥pch.h파일은 다른 프로젝트에서 참조가 불가능하다.
//따라서 ServerCore 용도에 맞게 CorePch.h라는 (일반헤서)파일을 만들어서 다른 프로젝트가 참조 할 수 있도록 한다.
#include "Types.h"
#include "CoreGlobal.h"
#include "CoreTLS.h"
#include "CoreMacro.h"
#include "Container.h"
#include <windows.h>
#include <iostream>

using namespace std;

#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")

#include "Lock.h"
#include "ObjectPool.h"
#include "TypeCast.h"
#include "Memory.h"
#include "SendBuffer.h"