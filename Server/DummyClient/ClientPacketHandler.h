#pragma once
//패킷의 종류를 나타내는 enum값. 
enum
{
	S_TEST = 1
};

class ClientPacketHandler
{
public:
	static void HandlePacket(BYTE* buffer, int32 len);

	static void Handle_S_TEST(BYTE* buffer, int32 len);
};


