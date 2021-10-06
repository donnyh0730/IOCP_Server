#pragma once
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
//std::function<리턴타입(파라미터타입)> 함수의 주소를 저장하는 포인터 변수타입을 지정한다.
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];//함수포인터 65535개 랜덤액세스로 빠르게 함수호출가능


enum : uint16
{
	// TODO : 자동화
	PKT_S_TEST = 1,
	PKT_S_LOGIN = 2,
};

// TODO : 자동화
// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
// TODO :자동화
bool Handle_S_TEST(PacketSessionRef& session, Protocol::S_TEST& pkt);

class ServerPacketHandler
{
public:
	
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)//처음에는 전부다 Invalide로초기화 바인딩.
			GPacketHandler[i] = Handle_INVALID;
		// TODO : 자동화
		GPacketHandler[PKT_S_TEST] = [](PacketSessionRef& session, BYTE* buffer, int32 len)
		{
			return HandlePacket<Protocol::S_TEST>(Handle_S_TEST, session, buffer, len);
			//람다로 익명함수를 만들어서 넣어줄 건데, HandlePacket이 또 템플릿 함수이다. 
		};
		
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}

	// TODO : 자동화
	static SendBufferRef MakeSendBuffer(Protocol::S_TEST& pkt)
	{
		return MakeSendBuffer(pkt, PKT_S_TEST);
	}

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;
		//buffer에서 pkt로 파싱하여 데이터를 넣어준다. 제대로 파싱이 안됬다면 false
		return func(session, pkt);//파싱후에 패킷타입에 맞게 넣어준 핸들러펑션을 실행한다.
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};


//
//template<typename T, typename C>
//class PacketIterator
//{
//public:
//	PacketIterator(C& container, uint16 index) : _container(container), _index(index) { }
//
//	bool				operator!=(const PacketIterator& other) const { return _index != other._index; }
//	const T& operator*() const { return _container[_index]; }
//	T& operator*() { return _container[_index]; }
//	T* operator->() { return &_container[_index]; }
//	PacketIterator& operator++() { _index++; return *this; }
//	PacketIterator		operator++(int32) { PacketIterator ret = *this; ++_index; return ret; }
//
//private:
//	C& _container;
//	uint16			_index;
//};
//
//template<typename T>
//class PacketList
//{
//public:
//	PacketList() : _data(nullptr), _count(0) { }
//	PacketList(T* data, uint16 count) : _data(data), _count(count) { }
//
//	T& operator[](uint16 index)
//	{
//		ASSERT_CRASH(index < _count);
//		return _data[index];
//	}
//
//	uint16 Count() { return _count; }
//
//	// ranged-base for 지원
//	PacketIterator<T, PacketList<T>> begin() { return PacketIterator<T, PacketList<T>>(*this, 0); }
//	PacketIterator<T, PacketList<T>> end() { return PacketIterator<T, PacketList<T>>(*this, _count); }
//
//private:
//	T* _data;
//	uint16		_count;
//};
//
//#pragma pack(1)
//
//// [ PKT_S_TEST ][BuffsListItem BuffsListItem BuffsListItem][victim victim][victim victim]
//struct PKT_S_TEST
//{
//	struct BuffsListItem
//	{
//		uint64 buffId;
//		float remainTime;
//
//		// Victim List
//		uint16 victimsOffset;
//		uint16 victimsCount;
//	};
//
//	uint16 packetSize; // 공용 헤더
//	uint16 packetId; // 공용 헤더
//	uint64 id; // 8
//	uint32 hp; // 4
//	uint16 attack; // 2
//	uint16 buffsOffset;
//	uint16 buffsCount;
//};
//
//// [ PKT_S_TEST ][BuffsListItem BuffsListItem BuffsListItem][victim victim]
//class PKT_S_TEST_WRITE
//{
//public:
//	using BuffsListItem = PKT_S_TEST::BuffsListItem;
//	using BuffsList = PacketList<PKT_S_TEST::BuffsListItem>;
//	using BuffsVictimsList = PacketList<uint64>;
//
//	PKT_S_TEST_WRITE(uint64 id, uint32 hp, uint16 attack)
//	{
//		_sendBuffer = GSendBufferManager->Open(4096);
//		_bw = BufferWriter(_sendBuffer->Buffer(), _sendBuffer->AllocSize());
//
//		_pkt = _bw.Reserve<PKT_S_TEST>();
//		_pkt->packetSize = 0; // To Fill
//		_pkt->packetId = S_TEST;
//		_pkt->id = id;
//		_pkt->hp = hp;
//		_pkt->attack = attack;
//		_pkt->buffsOffset = 0; // To Fill
//		_pkt->buffsCount = 0; // To Fill
//	}
//
//	BuffsList ReserveBuffsList(uint16 buffCount)
//	{
//		BuffsListItem* firstBuffsListItem = _bw.Reserve<BuffsListItem>(buffCount);
//		_pkt->buffsOffset = (uint64)firstBuffsListItem - (uint64)_pkt;
//		_pkt->buffsCount = buffCount;
//		return BuffsList(firstBuffsListItem, buffCount);
//	}
//
//	BuffsVictimsList ReserveBuffsVictimsList(BuffsListItem* buffsItem, uint16 victimsCount)
//	{
//		uint64* firstVictimsListItem = _bw.Reserve<uint64>(victimsCount);
//		buffsItem->victimsOffset = (uint64)firstVictimsListItem - (uint64)_pkt;
//		buffsItem->victimsCount = victimsCount;
//		return BuffsVictimsList(firstVictimsListItem, victimsCount);
//	}
//
//	SendBufferRef CloseAndReturn()
//	{
//		// 패킷 사이즈 계산
//		_pkt->packetSize = _bw.WriteSize();
//
//		_sendBuffer->Close(_bw.WriteSize());
//		return _sendBuffer;
//	}
//
//private:
//	PKT_S_TEST* _pkt = nullptr;
//	SendBufferRef _sendBuffer;
//	BufferWriter _bw;
//};
//
//
//#pragma pack()