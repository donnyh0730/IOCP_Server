#pragma once
#include "DBConnection.h"

/*
* 이 클래스는 DBStatment 객체에 변수를 바인딩 할때 편리하게 사용하도록 구현되어 있다.
* 모든 지정된 파라미터의 갯수만큼 바인딩이 완료 되어야만 excute를 실행할 수 있게되기 때문에
* 쿼리에러를 줄일 수 있다. 
*/

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
		//처음 지정해준 파라미터 카운트 갯수가 전부다 바인딩 되었는지를 검사함.
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
	//PramCount가 0보다 큰가? 맞으면 ParamCount로 : 아니면 1로 
	SQLLEN			_columnIndex[ParamCount > 0 ? ParamCount : 1];
	/*위의 두 변수는 컴파일 타임에 결정되는데 컴파일러가 삼항연산자의 결과를가지고 
	클래스를 만들어주기때문에 가능한 코드이다...*/
	uint64			_paramFlag;
	uint64			_columnFlag;
};

