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

	//�� ���� ������ �Ҹ��Ű���� �̰��� ���ǿ��� ������ player�� ����Ű���ִ� ��� ��ü�����׼� �� ���۷����� �������Ѵ�.

	if (_currentPlayer)
	{
		if (auto room = _room.lock())
		{
			room->DoAsync(&Room::Leave, _currentPlayer);
		}
		//���� room�̿ܿ��� ���̰ų�, ��򰡿��� �� player�� ���۷��� �ϰ��ִٸ� ������ �ڵ带 �־��־���Ѵ�.
	}
	_currentPlayer = nullptr;
	_players.clear();//�̷��� �ϸ� �÷��̾��� ���۷��� ī��Ʈ�� ���� 0�̵Ǿ� �Ҹ�
	//->players�� ���� �Ҹ�Ǹ�GameSession�� ���۷����ϰ��ִ� �����Ͱ� ���� ������⶧���� �� GameSession�� ���� �Ҹ�ȴ�. 
}

void GameSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	// TODO : packetId �뿪 üũ
	ClientPacketHandler::HandlePacket(session, buffer, len);
}

void GameSession::OnSend(int32 len)
{
	//cout << "OnSend Len = " << len << endl;
}