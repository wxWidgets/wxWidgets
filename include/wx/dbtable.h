///////////////////////////////////////////////////////////////////////////////
// Name:        dbtable.h
// Purpose:     Declaration of the wxTable class.
// Author:      Doug Card
// Modified by:
// Created:     9.96
// RCS-ID:      $Id$
// Copyright:   (c) 1996 Remstar International, Inc.
// Licence:     wxWindows licence, plus:
// Notice:		This class library and its intellectual design are free of charge for use,
//              modification, enhancement, debugging under the following conditions:
//              1) These classes may only be used as part of the implementation of a
//                 wxWindows-based application
//              2) All enhancements and bug fixes are to be submitted back to the wxWindows
//                 user groups free of all charges for use with the wxWindows library.
//              3) These classes may not be distributed as part of any other class library,
//                 DLL, text (written or electronic), other than a complete distribution of
//                 the wxWindows GUI development toolkit.
///////////////////////////////////////////////////////////////////////////////

/*
// SYNOPSIS START
// SYNOPSIS STOP
*/

#ifndef DBTABLE_DOT_H
#define DBTABLE_DOT_H

// Use this line for wxWindows v1.x
//#include "wx_ver.h"
// Use this line for wxWindows v2.x
#include "wx/version.h"

#if wxMAJOR_VERSION == 2
	#ifdef __GNUG__
	#pragma interface "dbtable.h"
	#endif
#endif

#if wxMAJOR_VERSION == 2
	#include "wx/db.h"
#else
	#include "db.h"
#endif

const int	ROWID_LEN			= 24;  // 18 is the max, 24 is in case it gets larger
const int	DEFAULT_CURSOR		= 0;
const bool	QUERY_ONLY			= TRUE;
const bool  DISABLE_VIEW		= TRUE;

// The following class is used to define a column of a table.
// The wxTable constructor will dynamically allocate as many of
// these as there are columns in the table.  The class derived
// from wxTable must initialize these column definitions in it's
// constructor.  These column definitions provide inf. to the
// wxTable class which allows it to create a table in the data
// source, exchange data between the data source and the C++
// object, and so on.

class WXDLLEXPORT CcolDef
{
public:
	char    ColName[DB_MAX_COLUMN_NAME_LEN+1];	// Column Name
	int     DbDataType;									// Logical Data Type; e.g. DB_DATA_TYPE_INTEGER
	int     SqlCtype;										// C data type; e.g. SQL_C_LONG
	void   *PtrDataObj;									// Address of the data object
	int	  SzDataObj;									// Size, in bytes, of the data object
	bool	  KeyField;										// TRUE if this column is part of the PRIMARY KEY to the table; Date fields should NOT be KeyFields.
	bool	  Updateable;									// Specifies whether this column is updateable
	bool    InsertAllowed;								// Specifies whether this column should be included in an INSERT statement
	bool    DerivedCol;									// Specifies whether this column is a derived value
	SDWORD  CbValue;										// Internal use only!!!
	bool	  Null;											// NOT FULLY IMPLEMENTED - Allows NULL values in Inserts and Updates
};  // CcolDef

// This structure is used when creating secondary indexes.
class WXDLLEXPORT CidxDef
{
public:
	char ColName[DB_MAX_COLUMN_NAME_LEN+1];
	bool Ascending;
};  // CidxDef

class WXDLLEXPORT wxTable
{
private:

	ULONG	tableID;  // Used for debugging.  This can help to match up mismatched constructors/destructors

	// Private member variables
	UDWORD cursorType;

	// Private member functions
	bool bindInsertParams(void);
	bool bindUpdateParams(void);
	bool bindCols(HSTMT cursor);
	bool getRec(UWORD fetchType);
	bool execDelete(char *pSqlStmt);
	bool execUpdate(char *pSqlStmt);
	bool query(int queryType, bool forUpdate, bool distinct, char *pSqlStmt = 0);

public:

	// Pointer to the database object this table belongs to
	wxDB *pDb;

	// ODBC Handles
	HENV  henv;							// ODBC Environment handle
	HDBC  hdbc;							// ODBC DB Connection handle
	HSTMT hstmt;						// ODBC Statement handle
	HSTMT *hstmtDefault;				// Default cursor
	HSTMT hstmtInsert;				// ODBC Statement handle used specifically for inserts
	HSTMT hstmtDelete;				// ODBC Statement handle used specifically for deletes
	HSTMT hstmtUpdate;				// ODBC Statement handle used specifically for updates
	HSTMT hstmtInternal;				// ODBC Statement handle used internally only
	HSTMT *hstmtCount;				// ODBC Statement handle used by Count() function (No binding of columns)

	// Table Inf.
	char tableName[DB_MAX_TABLE_NAME_LEN+1];			// Table name
	char queryTableName[DB_MAX_TABLE_NAME_LEN+1];	// Query Table Name
	int  noCols;												// # of columns in the table
	bool queryOnly;											// Query Only, no inserts, updates or deletes

	char tablePath[DB_PATH_MAX];  // needed for dBase tables

	// Column Definitions
	CcolDef *colDefs;	// Array of CcolDef structures

	// Where, Order By and From clauses
	char *where;			// Standard SQL where clause, minus the word WHERE
	char *orderBy;			// Standard SQL order by clause, minus the ORDER BY
	char *from;				// Allows for joins in a wxTable::Query().  Format: ",tbl,tbl..."

	// Flags
	bool selectForUpdate;

	// Public member functions
	wxTable(wxDB *pwxDB, const char *tblName, const int nCols,
		const char *qryTblName = 0, bool qryOnly = !QUERY_ONLY, char *tblPath=NULL);
	virtual ~wxTable();
	bool	Open(void);
	bool	CreateTable(bool attemptDrop=TRUE);
	bool	DropTable(void);
	bool	CreateIndex(char * idxName, bool unique, int noIdxCols, CidxDef *pIdxDefs, bool attemptDrop=TRUE);
	bool	DropIndex(char * idxName);
	bool	CloseCursor(HSTMT cursor);
	int   Insert(void);
	bool	Update(void);
	bool	Update(char *pSqlStmt);
	bool	UpdateWhere(char *pWhereClause);
	bool	Delete(void);
	bool	DeleteWhere(char *pWhereClause);
	bool	DeleteMatching(void);
	virtual bool Query(bool forUpdate = FALSE, bool distinct = FALSE);
	bool	QueryBySqlStmt(char *pSqlStmt);
	bool	QueryMatching(bool forUpdate = FALSE, bool distinct = FALSE);
	bool	QueryOnKeyFields(bool forUpdate = FALSE, bool distinct = FALSE);
	bool	GetNext(void)   { return(getRec(SQL_FETCH_NEXT));  }
	bool	operator++(int) { return(getRec(SQL_FETCH_NEXT));  }
#if !wxODBC_FWD_ONLY_CURSORS
	bool	GetPrev(void)   { return(getRec(SQL_FETCH_PRIOR)); }
	bool	operator--(int) { return(getRec(SQL_FETCH_PRIOR)); }
	bool	GetFirst(void)  { return(getRec(SQL_FETCH_FIRST)); }
	bool	GetLast(void)   { return(getRec(SQL_FETCH_LAST));  }
#endif
	bool	IsCursorClosedOnCommit(void);
	bool	IsColNull(int colNo);
	UWORD GetRowNum(void);
	void  GetSelectStmt(char *pSqlStmt, int typeOfSelect, bool distinct);
	void  GetDeleteStmt(char *pSqlStmt, int typeOfDel, char *pWhereClause = 0);
	void  GetUpdateStmt(char *pSqlStmt, int typeOfUpd, char *pWhereClause = 0);
	void  GetWhereClause(char *pWhereClause, int typeOfWhere, char *qualTableName = 0);
	bool	CanSelectForUpdate(void);
	bool	CanUpdByROWID(void);
	void  ClearMemberVars(void);
	bool	SetQueryTimeout(UDWORD nSeconds);
	void  SetColDefs (int index, char *fieldName, int dataType, void *pData, int cType,
							int size, bool keyField = FALSE, bool upd = TRUE,
							bool insAllow = TRUE, bool derivedCol = FALSE);
	HSTMT *NewCursor(bool setCursor = FALSE, bool bindColumns = TRUE);
	bool	DeleteCursor(HSTMT *hstmtDel);
	void	SetCursor(HSTMT *hstmtActivate = (void **) DEFAULT_CURSOR);
	HSTMT GetCursor(void) { return(hstmt); }
	ULONG Count(void);
	int   DB_STATUS(void) { return(pDb->DB_STATUS); }
	bool	Refresh(void);
	bool	SetNull(int colNo);
	bool	SetNull(char *colName);

#ifdef __WXDEBUG__
	ULONG	GetTableID() { return tableID; };
#endif

};  // wxTable

#endif
