#pragma once
#include "DBConnection.h"

/*
* �� Ŭ������ DBStatment ��ü�� ������ ���ε� �Ҷ� ���ϰ� ����ϵ��� �����Ǿ� �ִ�.
* ��� ������ �Ķ������ ������ŭ ���ε��� �Ϸ� �Ǿ�߸� excute�� ������ �� �ְԵǱ� ������
* ���������� ���� �� �ִ�. 
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
		//ó�� �������� �Ķ���� ī��Ʈ ������ ���δ� ���ε� �Ǿ������� �˻���.
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
	void BindParam(int32 idx, T& value)//���۷��� Ÿ���� ���� ���
	{
		_dbConnection.BindParam(idx + 1, &value, &_paramIndex[idx]);
		_paramFlag |= (1LL << idx);//idx��ŭ 1�� �̵���Ų ��  �÷��׿� or �������� ��Ʈ�÷��׸� ���ش�.
		//ex) 0000 0001 |= 0000 0100 -> 0000 0101 
	}

	void BindParam(int32 idx, const WCHAR* value)//���ڿ��� ���� ���
	{
		_dbConnection.BindParam(idx + 1, value, &_paramIndex[idx]);
		_paramFlag |= (1LL << idx);
	}

	template<typename T, int32 N>
	void BindParam(int32 idx, T(&value)[N])//�迭�� ���� ���
	{
		_dbConnection.BindParam(idx + 1, (const BYTE*)value, size32(T) * N, &_paramIndex[idx]);
		_paramFlag |= (1LL << idx);
	}

	template<typename T>
	void BindParam(int32 idx, T* value, int32 N)//������ Ÿ���� ���� ���
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
	//PramCount�� 0���� ū��? ������ ParamCount�� : �ƴϸ� 1�� 
	SQLLEN			_columnIndex[ParamCount > 0 ? ParamCount : 1];
	/*���� �� ������ ������ Ÿ�ӿ� �����Ǵµ� �����Ϸ��� ���׿������� ����������� 
	Ŭ������ ������ֱ⶧���� ������ �ڵ��̴�...*/
	uint64			_paramFlag;
	uint64			_columnFlag;
};

