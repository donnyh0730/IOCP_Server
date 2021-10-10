#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "ClientPacketHandler.h"
#include "Room.h"
void GameSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnected()
{
	GSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this()));

	//이 게임 세션을 소멸시키려면 이게임 세션에서 접속한 player를 가리키고있는 모든 객체들한테서 그 레퍼런스를 지워야한다.

	if (_currentPlayer)
	{
		if (auto room = _room.lock())
		{
			room->DoAsync(&Room::Leave, _currentPlayer);
		}
		//만약 room이외에도 맵이거나, 어딘가에서 이 player를 레퍼런싱 하고있다면 날리는 코드를 넣어주어야한다.
	}
	_currentPlayer = nullptr;
	_players.clear();//이렇게 하면 플레이어의 레퍼런스 카운트가 먼저 0이되어 소멸
	//->players가 전부 소멸되면GameSession을 레퍼런싱하고있는 포인터가 전부 사라지기때문에 이 GameSession도 정상 소멸된다. 
}

void GameSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	// TODO : packetId 대역 체크
	ClientPacketHandler::HandlePacket(session, buffer, len);
}

void GameSession::OnSend(int32 len)
{
	//cout << "OnSend Len = " << len << endl;
}