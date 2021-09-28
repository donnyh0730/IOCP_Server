#include "pch.h"
#include "RecvBuffer.h"

/*--------------
	RecvBuffer
----------------*/

RecvBuffer::RecvBuffer(int32 bufferSize) : _bufferSize(bufferSize)
{
	_capacity = bufferSize * BUFFER_COUNT;
	_buffer.resize(_capacity);
}

RecvBuffer::~RecvBuffer()
{
}

void RecvBuffer::Clean()
{
	int32 dataSize = DataSize();
	if (dataSize == 0)
	{
		// 보통은 리시브된 데이터 사이즈만큼 write가되고 또 그만큼 바로 read해버리기 때문에, 
		// 즉 처리가 완료되기 때문에 이리로 들어온다.
		_readPos = _writePos = 0;
	}
	else
	{
		//그러나 정말 패킷의 트랜잭션량이 너무 많아져서 소켓의 버퍼에서 한번에 전송되지못하고 잘려서 오는 패킷의 경우에는 
		//요쪽으로 들어오게된다. 그래서 readoffset과 writeoffset이 위치가 차이가 나버리는 상황이 오는데,
		//하나의 패킷을 두번이상의 송수신으로 전송하는 경우가 누적되어 만약 버퍼의 끝부분 까지 rw옵셋이 같아지지 않았다면,
		//어쩔수 없이 데이터가 진짜로 처리되었는지 에는 관심이 없고 옵셋을 초기화 0번위치로 옮겨서 clean하게 한다. (확율 극악)
		if (FreeSize() < _bufferSize)
		{
			::memcpy(&_buffer[0], &_buffer[_readPos], dataSize);
			_readPos = 0;
			_writePos = dataSize;
		}
	}
}

bool RecvBuffer::OnRead(int32 numOfBytes)
{
	if (numOfBytes > DataSize())
		return false;

	_readPos += numOfBytes;
	return true;
}

bool RecvBuffer::OnWrite(int32 numOfBytes)
{
	if (numOfBytes > FreeSize())
		return false;

	_writePos += numOfBytes;//데이터를 받았으니 writeoffset를 옮긴다. 
	return true;
}