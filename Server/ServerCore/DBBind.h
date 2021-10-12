#pragma once
#include "DBConnection.h"

template<int32 C>
struct FullBits { enum { value = (1 << (C - 1)) | FullBits<C - 1>::value }; };

template<>
struct FullBits<1> { enum { value = 1 }; };

template<>
struct FullBits<0> { enum { value = 0 }; };

template<int32 ParamCount, int32 ColumnCount>
class DBBind
{
public:
	DBBind(DBConnection& dbConnection, const WCHAR* query)
		: _dbConnection(dbConnection), _query(query)
	{
		::memset(_paramIndex, 0, sizeof(_paramIndex));
		::memset(_columnIndex, 0, sizeof(_columnIndex));
		_paramFlag = 0;
		_columnFlag = 0;
		dbConnection.Unbind();
	}

	bool Validate()
	{
		return _paramFlag == FullBits<ParamCount>::value && _columnFlag == FullBits<ColumnCount>::value;
	}

	bool Execute()
	{
		ASSERT_CRASH(Validate());
		return _dbConnection.Execute(_query);
	}

	bool Fetch()
	{
		return _dbConnection.Fetch();
	}

public:
	template<typename T>
	void BindParam(int32 idx, T& value)//레퍼런스 타입이 들어온 경우
	{
		_dbConnection.BindParam(idx + 1, &value, &_paramIndex[idx]);
		_paramFlag |= (1LL << idx);//idx만큼 1을 이동시킨 후  플래그와 or 연산으로 비트플래그를 켜준다.
		//ex) 0000 0001 |= 0000 0100 -> 0000 0101 
	}

	void BindParam(int32 idx, const WCHAR* value)//문자열이 들어온 경우
	{
		_dbConnection.BindParam(idx + 1, value, &_paramIndex[idx]);
		_paramFlag |= (1LL << idx);
	}

	template<typename T, int32 N>
	void BindParam(int32 idx, T(&value)[N])//배열이 들어온 경우
	{
		_dbConnection.BindParam(idx + 1, (const BYTE*)value, size32(T) * N, &_paramIndex[idx]);
		_paramFlag |= (1LL << idx);
	}

	template<typename T>
	void BindParam(int32 idx, T* value, int32 N)//포인터 타입이 들어온 경우
	{
		_dbConnection.BindParam(idx + 1, (const BYTE*)value, size32(T) * N, &_paramIndex[idx]);
		_paramFlag |= (1LL << idx);
	}

	template<typename T>
	void BindCol(int32 idx, T& value)
	{
		_dbConnection.BindCol(idx + 1, &value, &_columnIndex[idx]);
		_columnFlag |= (1LL << idx);
	}

	template<int32 N>
	void BindCol(int32 idx, WCHAR(&value)[N])
	{
		_dbConnection.BindCol(idx + 1, value, N - 1, &_columnIndex[idx]);
		_columnFlag |= (1LL << idx);
	}

	void BindCol(int32 idx, WCHAR* value, int32 len)
	{
		_dbConnection.BindCol(idx + 1, value, len - 1, &_columnIndex[idx]);
		_columnFlag |= (1LL << idx);
	}

	template<typename T, int32 N>
	void BindCol(int32 idx, T(&value)[N])
	{
		_dbConnection.BindCol(idx + 1, value, size32(T) * N, &_columnIndex[idx]);
		_columnFlag |= (1LL << idx);
	}

protected:
	DBConnection& _dbConnection;
	const WCHAR* _query;
	SQLLEN			_paramIndex[ParamCount > 0 ? ParamCount : 1];
	SQLLEN			_columnIndex[ParamCount > 0 ? ParamCount : 1];
	uint64			_paramFlag;
	uint64			_columnFlag;
};

