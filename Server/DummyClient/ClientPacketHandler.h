#pragma once
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];
//������Ŷ��鷯�� �Ȱ��� ���� �Ǿ��ִ�. 
//�Լ��������� �迭�� ���� ������ id���� ���� ��鷯�� �θ��� switch case������ �������� ������
//�����ϱⰡ ��ƴٴ� ������ �ִ�. �ݸ�, �̷��� �Լ��������� �迭�� �̸� �����ص����ν� ������ 
//�����׼����� �����Ͽ� �ڵ鷯 ȣ���� �����ϰ� �� �� �ִ�.
// TODO : �ڵ�ȭ
enum : uint16
{
	PKT_S_TEST = 1,
	PKT_S_LOGIN = 2,
};


// TODO : �ڵ�ȭ
// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_S_TEST(PacketSessionRef& session, Protocol::S_TEST& pkt);
//�� ������ �ַ�� �ڵ鷯���� �ڵ� �����ɰŴ�.
class ClientPacketHandler
{
public:
	// TODO : �ڵ�ȭ
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;

		GPacketHandler[PKT_S_TEST] = [](PacketSessionRef& session, BYTE* buffer, int32 len) 
		{ 
			return HandlePacket<Protocol::S_TEST>(Handle_S_TEST, session, buffer, len); 
		};//1�� ��Ŷ�� ���� ��鷯�� �� ���ٰ� �ȴ�. 
		//�̹����� �ַ�� ��鷯 ����� �ڵ�ȭ �Ǿ�� �Ѵ�. 
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
//	// ranged-base for ����
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


