#pragma once
#include <sql.h>
#include <sqlext.h>

/*----------------
	DBConnection
-----------------*/

class DBConnection
{
public:
	bool			Connect(SQLHENV henv, const WCHAR* connectionString);
	void			Clear();

	bool			Execute(const WCHAR* query);
	//�����͸� �޾ƿ��°迭 ������ �ƴ϶�, ����� �ο쳪 Į���� ���̰ų� ����� �ϴ� ���� �۾��� �Ҷ� Excute�� ����Ѵ�.

	bool			Fetch();//�����͸� �ܾ� �ö� ����ϴ� �Լ�.
	int32			GetRowCount();
	void			Unbind();

public:
	//������ ���� �����Ҷ�,
	bool			BindParam(SQLUSMALLINT paramIndex, SQLSMALLINT cType, SQLSMALLINT sqlType, SQLULEN len, SQLPOINTER ptr, SQLLEN* index);
	bool			BindCol(SQLUSMALLINT columnIndex, SQLSMALLINT cType, SQLULEN len, SQLPOINTER value, SQLLEN* index);
	void			HandleError(SQLRETURN ret);

private:
	SQLHDBC			_connection = SQL_NULL_HANDLE;
	SQLHSTMT		_statement = SQL_NULL_HANDLE;
};

