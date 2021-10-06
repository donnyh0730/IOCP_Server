#pragma once
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];
//서버패킷헨들러도 똑같이 정의 되어있다. 
//함수포인터의 배열을 쓰는 이유는 id값에 따라 헨들러를 부르는 switch case문법은 가독성도 안좋고
//관리하기가 어렵다는 측면이 있다. 반면, 이렇게 함수포인터의 배열을 미리 선언해둠으로써 빠르게 
//랜덤액세스로 접근하여 핸들러 호출을 가능하게 할 수 있다.
// TODO : 자동화
enum : uint16
{
	PKT_S_TEST = 1,
	PKT_S_LOGIN = 2,
};


// TODO : 자동화
// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_S_TEST(PacketSessionRef& session, Protocol::S_TEST& pkt);
//이 밑으로 주루룩 핸들러들이 자동 생성될거다.
class ClientPacketHandler
{
public:
	// TODO : 자동화
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;

		GPacketHandler[PKT_S_TEST] = [](PacketSessionRef& session, BYTE* buffer, int32 len) 
		{ 
			return HandlePacket<Protocol::S_TEST>(Handle_S_TEST, session, buffer, len); 
		};//1번 패킷에 대한 헨들러는 이 람다가 된다. 
		//이밑으로 주루룩 헨들러 등록이 자동화 되어야 한다. 
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}
};
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
//	PacketIterator<T, PacketList<T>> begin()
//	{
//		return PacketIterator<T, PacketList<T>>(*this, 0);
//	}
//
//	PacketIterator<T, PacketList<T>> end()
//	{
//		return PacketIterator<T, PacketList<T>>(*this, _count);
//	}
//
//private:
//	T* _data;
//	uint16		_count;
//};


