#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"

void ClientPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br >> header;

	switch (header.id)
	{
	case S_TEST:
		Handle_S_TEST(buffer, len);
		break;
	}
}

#pragma pack(1)//[1]Serialization�� �ϰڴٴ� �����Ϸ� ��ɾ� 

// [ PKT_S_TEST ][BuffsListItem BuffsListItem BuffsListItem]
struct PKT_S_TEST
{
	struct BuffsListItem
	{
		uint64 buffId;
		float remainTime;
	};

	uint16 packetSize;// ���� ���
	uint16 packetId; // ���� ���
	uint64 id; // 8
	uint32 hp; // 4
	uint16 attack; // 2
	uint16 buffsOffset;
	uint16 buffsCount;

	bool IsValidePck()
	{
		uint32 size = 0;
		size += sizeof(PKT_S_TEST);
		size += buffsCount * sizeof(BuffsListItem);
		if (size != packetSize)
			return false;

		if (buffsOffset + buffsCount * sizeof(BuffsListItem) > packetSize)
			return false;

		return true;
	}
	using BuffsList = PacketList<PKT_S_TEST::BuffsListItem>;

	BuffsList GetBuffsList()
	{
		BYTE* data = reinterpret_cast<BYTE*>(this);
		data += buffsOffset;//18����Ʈ ��ŭ �̵� 
		return BuffsList(reinterpret_cast<PKT_S_TEST::BuffsListItem*>(data), buffsCount);
		//�� ��ġ���� ������ ĳ�������ϸ� ���������� �迭�� ���� �� �ִ�.
	}
};
#pragma pack()

void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	/*���־��� �߿��� ����*/
	PKT_S_TEST* pkt = reinterpret_cast<PKT_S_TEST*>(buffer);
	/*������ �ִ� ������ ���ø޸𸮿� �����ؼ� ����ϴ� ���̾ƴ϶� �����ּ��� ����Ʈ
	�����͸� �״�� Ÿ������ ĳ�����ؼ� ����Ѵٴ� �����̴�. ������������ ��� Packet�ȿ� �ִ� List�����͸�
	���� �� �ְ� �����ִ� ����Ŭ������ ����Ʈ ������ Ÿ��ĳ�������� �о� ������. ���簡 ��ǻ� Zero*/
	if (pkt->IsValidePck() == false)
		return;

	//cout << "ID: " << id << " HP : " << hp << " ATT : " << attack << endl;

	PKT_S_TEST::BuffsList buffs = pkt->GetBuffsList();

	cout << "BufCount : " << buffs.Count() << endl;
	for (int32 i = 0; i < buffs.Count(); i++)
	{
		cout << "BufInfo : " << buffs[i].buffId << " " << buffs[i].remainTime << endl;
	}

	for (auto it = buffs.begin(); it != buffs.end(); ++it)
	{
		cout << "BufInfo : " << it->buffId << " " << it->remainTime << endl;
	}

	for (auto& buff : buffs)
	{
		cout << "BufInfo : " << buff.buffId << " " << buff.remainTime << endl;
	}
}
