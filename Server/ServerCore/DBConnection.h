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
	//데이터를 받아오는계열 쿼리가 아니라, 디비의 로우나 칼럼을 늘이거나 지우고 하는 조작 작업을 할때 Excute를 사용한다.

	bool			Fetch();//데이터를 긁어 올때 사용하는 함수.
	int32			GetRowCount();
	void			Unbind();

public:
	//데이터 행을 세팅할때,
	bool			BindParam(SQLUSMALLINT paramIndex, SQLSMALLINT cType, SQLSMALLINT sqlType, SQLULEN len, SQLPOINTER ptr, SQLLEN* index);
	bool			BindCol(SQLUSMALLINT columnIndex, SQLSMALLINT cType, SQLULEN len, SQLPOINTER value, SQLLEN* index);
	void			HandleError(SQLRETURN ret);

private:
	SQLHDBC			_connection = SQL_NULL_HANDLE;
	SQLHSTMT		_statement = SQL_NULL_HANDLE;
};

