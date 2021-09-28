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
		// ������ ���ú�� ������ �����ŭ write���ǰ� �� �׸�ŭ �ٷ� read�ع����� ������, 
		// �� ó���� �Ϸ�Ǳ� ������ �̸��� ���´�.
		_readPos = _writePos = 0;
	}
	else
	{
		//�׷��� ���� ��Ŷ�� Ʈ����Ƿ��� �ʹ� �������� ������ ���ۿ��� �ѹ��� ���۵������ϰ� �߷��� ���� ��Ŷ�� ��쿡�� 
		//�������� �����Եȴ�. �׷��� readoffset�� writeoffset�� ��ġ�� ���̰� �������� ��Ȳ�� ���µ�,
		//�ϳ��� ��Ŷ�� �ι��̻��� �ۼ������� �����ϴ� ��찡 �����Ǿ� ���� ������ ���κ� ���� rw�ɼ��� �������� �ʾҴٸ�,
		//��¿�� ���� �����Ͱ� ��¥�� ó���Ǿ����� ���� ������ ���� �ɼ��� �ʱ�ȭ 0����ġ�� �Űܼ� clean�ϰ� �Ѵ�. (Ȯ�� �ؾ�)
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

	_writePos += numOfBytes;//�����͸� �޾����� writeoffset�� �ű��. 
	return true;
}