#pragma once

/*----------------
	BufferWriter
-----------------*/
//Send Buffer에 데이터를 넣기 편하도록 << 연산자 오버로딩틀 통해 효율을 높혀주는 클래스.
class BufferWriter
{
public:
	BufferWriter();
	BufferWriter(BYTE* buffer, uint32 size, uint32 pos = 0);
	~BufferWriter();

	BYTE* Buffer() { return _buffer; }
	uint32			Size() { return _size; }
	uint32			WriteSize() { return _pos; }
	uint32			FreeSize() { return _size - _pos; }

	template<typename T>
	bool			Write(T* src) { return Write(src, sizeof(T)); }
	bool			Write(void* src, uint32 len);

	template<typename T>
	T* Reserve(uint16 count = 1);

	template<typename T>
	BufferWriter& operator<<(T&& src);

private:
	BYTE* _buffer = nullptr;
	uint32			_size = 0;
	uint32			_pos = 0;
};

template<typename T>
T* BufferWriter::Reserve(uint16 count)
{
	if (FreeSize() < (sizeof(T) * count))
		return nullptr;

	T* ret = reinterpret_cast<T*>(&_buffer[_pos]);
	_pos += (sizeof(T) * count);
	return ret;
}
//실제로 데이터가 기입(reverve)되는 것은아니고 버퍼의 현재 쓰여질 위치값을 받아서 
//그위치에 T타입으로 쓰여질 준비를한다. 

template<typename T>
BufferWriter& BufferWriter::operator<<(T&& src)
{
	using DataType = std::remove_reference_t<T>;
	*reinterpret_cast<DataType*>(&_buffer[_pos]) = std::forward<DataType>(src);
	_pos += sizeof(T);
	return *this;
}
//템플릿 을 적용한 함수에 인자를 T&& src 처럼 R밸류 래퍼런싱을 하겠다 라고 하면,
//이것은 "보편참조를 하겠다" 라는 말과 같아진다. 보편참조는 Lvalue, Rvalue 상관없이 모두 받는다.
//const uint64& 이런식으로 값복사가 일어남.