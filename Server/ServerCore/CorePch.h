#pragma once
//ó�� ������ �׳�pch.h������ �ٸ� ������Ʈ���� ������ �Ұ����ϴ�.
//���� ServerCore �뵵�� �°� CorePch.h��� (�Ϲ��켭)������ ���� �ٸ� ������Ʈ�� ���� �� �� �ֵ��� �Ѵ�.
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