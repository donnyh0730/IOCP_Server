#pragma once
#include "DBConnection.h"
#include "DBModel.h"

/*--------------------
	DBSynchronizer
---------------------*/
//현재 DB와 갱신된 XML을 비교하여 DB를 최신화된 테이블 형식으로 동기화 해주는 클래스이다. 
class DBSynchronizer
{
	enum
	{
		PROCEDURE_MAX_LEN = 10000
	};

	enum UpdateStep : uint8
	{
		DropIndex,
		AlterColumn,
		AddColumn,
		CreateTable,
		DefaultConstraint,
		CreateIndex,
		DropColumn,
		DropTable,
		StoredProcecure,

		Max
	};

	enum ColumnFlag : uint8
	{
		Type = 1 << 0,
		Nullable = 1 << 1,
		Identity = 1 << 2,
		Default = 1 << 3,
		Length = 1 << 4,
	};

public:
	DBSynchronizer(DBConnection& conn) : _dbConn(conn) { }
	~DBSynchronizer();

	bool		Synchronize(const WCHAR* path);

private:
	void		ParseXmlDB(const WCHAR* path);
	bool		GatherDBTables();
	bool		GatherDBIndexes();
	bool		GatherDBStoredProcedures();

	void		CompareDBModel();
	void		CompareTables(DBModel::TableRef dbTable, DBModel::TableRef xmlTable);
	void		CompareColumns(DBModel::TableRef dbTable, DBModel::ColumnRef dbColumn, DBModel::ColumnRef xmlColumn);
	void		CompareStoredProcedures();

	void		ExecuteUpdateQueries();

private:
	DBConnection& _dbConn;

	Vector<DBModel::TableRef>			_xmlTables;
	Vector<DBModel::ProcedureRef>		_xmlProcedures;
	Set<String>							_xmlRemovedTables;

	Vector<DBModel::TableRef>			_dbTables;
	Vector<DBModel::ProcedureRef>		_dbProcedures;

private:
	Set<String>							_dependentIndexes;
	Vector<String>						_updateQueries[UpdateStep::Max];
};