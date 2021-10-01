#pragma once

class SendBufferChunk;

/*----------------
	SendBuffer
-----------------*/

class SendBuffer
{
public:
	SendBuffer(SendBufferChunkRef owner, BYTE* buffer, int32 allocSize);
	~SendBuffer();

	BYTE*		Buffer() { return _buffer; }
	uint32		AllocSize() { return _allocSize; }
	int32		WriteSize() { return _writeSize; }
	void		Close(uint32 writeSize);

private:
	BYTE* _buffer;
	uint32				_allocSize = 0;
	uint32				_writeSize = 0;
	SendBufferChunkRef	_owner;//샌드버퍼가 소멸할때 chunk의 레프카운트도 하나씩 줄게된다.
};

/*--------------------
	SendBufferChunk
--------------------*/

class SendBufferChunk : public enable_shared_from_this<SendBufferChunk>
{
	enum
	{
		SEND_BUFFER_CHUNK_SIZE = 6000
	};

public:
	SendBufferChunk();
	~SendBufferChunk();

	void				Reset();
	SendBufferRef		Open(uint32 allocSize);//청크를 오픈하여 메모리를 할당 받는다는 의미의 open
	void				Close(uint32 writeSize);

	bool				IsOpen() { return _open; }
	BYTE* Buffer() { return &_buffer[_usedSize]; }
	uint32				FreeSize() { return static_cast<uint32>(_buffer.size()) - _usedSize; }

private:
	Array<BYTE, SEND_BUFFER_CHUNK_SIZE>		_buffer = {};
	bool									_open = false;
	uint32									_usedSize = 0;
};

/*---------------------
	SendBufferManager
----------------------*/

class SendBufferManager
{
public:
	SendBufferRef		Open(uint32 size);

private:
	SendBufferChunkRef	Pop();
	void				Push(SendBufferChunkRef buffer);

	static void			PushGlobal(SendBufferChunk* buffer);

private:
	USE_LOCK;
	Vector<SendBufferChunkRef> _sendBufferChunks;
};
