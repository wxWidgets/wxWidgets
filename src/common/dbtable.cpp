///////////////////////////////////////////////////////////////////////////////
// Name:        dbtable.cpp
// Purpose:     Implementation of the wxTable class.
// Author:      Doug Card
// Mods:			 April 1999
//						-Dynamic cursor support - Only one predefined cursor, as many others as
//							you need may be created on demand
//						-Reduced number of active cursors significantly 
//						-Query-Only wxTable objects 
// Created:     9.96
// RCS-ID:      $Id$
// Copyright:   (c) 1996 Remstar International, Inc.
// Licence:     wxWindows licence, plus:
// Notice:		 This class library and its intellectual design are free of charge for use,
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

// Use this line for wxWindows v1.x
//#include "wx_ver.h"
// Use this line for wxWindows v2.x
#include "wx/version.h"
#include  "wx/wxprec.h"

#if wxMAJOR_VERSION == 2
#	ifdef __GNUG__
#	pragma implementation "dbtable.h"
#	endif
#endif

#ifdef DBDEBUG_CONSOLE
#	include <iostream.h>
#endif

#ifdef    __BORLANDC__
  #pragma hdrstop
#endif  //__BORLANDC__

#if wxMAJOR_VERSION == 2
#	ifndef WX_PRECOMP
#		include  "wx/wx.h"
#	endif //WX_PRECOMP
#endif

#if wxMAJOR_VERSION == 1
#	if defined(wx_msw) || defined(wx_x)
#		ifdef WX_PRECOMP
#			include "wx_prec.h"
#		else
#			include "wx.h"
#		endif
#	endif
#	define wxUSE_ODBC 1
#endif

#if wxUSE_ODBC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#if   wxMAJOR_VERSION == 1
	#include "table.h"
#elif wxMAJOR_VERSION == 2
	#include "wx/dbtable.h"
#endif

#ifdef __UNIX__
// The HPUX preprocessor lines below were commented out on 8/20/97
// because macros.h currently redefines DEBUG and is unneeded.
// #  ifdef HPUX
// #    include <macros.h>
// #  endif
#  ifdef LINUX
#    include <sys/minmax.h>
#  endif
#endif

ULONG lastTableID = 0;


#if __WXDEBUG__ > 0
	wxList TablesInUse;
#endif


/********** wxTable::wxTable() **********/
wxTable::wxTable(wxDB *pwxDB, const char *tblName, const int nCols,
					const char *qryTblName, bool qryOnly, char *tblPath)
{
	pDb					= pwxDB;					// Pointer to the wxDB object
	henv					= 0;
	hdbc					= 0;
	hstmt					= 0;
	hstmtDefault		= 0;						// Initialized below
	hstmtCount			= 0;						// Initialized first time it is needed
	hstmtInsert			= 0;
	hstmtDelete			= 0;
	hstmtUpdate			= 0;
	hstmtInternal		= 0;
	colDefs				= 0;
	tableID				= 0;
	noCols				= nCols;					// No. of cols in the table
	where					= 0;						// Where clause
	orderBy				= 0;						// Order By clause
	from					= 0;						// From clause
	selectForUpdate	= FALSE;					// SELECT ... FOR UPDATE; Indicates whether to include the FOR UPDATE phrase
	queryOnly			= qryOnly;

	assert (tblName);

	strcpy(tableName, tblName);				// Table Name
	if (tblPath)
		strcpy(tablePath, tblPath);				// Table Path - used for dBase files

	if (qryTblName)								// Name of the table/view to query
		strcpy(queryTableName, qryTblName);
	else
		strcpy(queryTableName, tblName);

//	assert(pDb);  // Assert is placed after table name is assigned for error reporting reasons
	if (!pDb)
		return;

	pDb->nTables++;

	char s[200];
	tableID = ++lastTableID;
	sprintf(s, "wxTable constructor (%-20s) tableID:[%6lu] pDb:[%p]", tblName,tableID,pDb);

#if __WXDEBUG__ > 0
	CstructTablesInUse *tableInUse;
	tableInUse = new CstructTablesInUse();
	tableInUse->tableName = tblName;
	tableInUse->tableID = tableID;
	tableInUse->pDb = pDb;
	TablesInUse.Append(tableInUse);
#endif

	pDb->WriteSqlLog(s);

	// Grab the HENV and HDBC from the wxDB object
	henv = pDb->henv;
	hdbc = pDb->hdbc;

	// Allocate space for column definitions
	if (noCols)
		colDefs = new CcolDef[noCols];  // Points to the first column defintion

	// Allocate statement handles for the table
	if (!queryOnly)
	{
		// Allocate a separate statement handle for performing inserts
		if (SQLAllocStmt(hdbc, &hstmtInsert) != SQL_SUCCESS)
			pDb->DispAllErrors(henv, hdbc);
		// Allocate a separate statement handle for performing deletes
		if (SQLAllocStmt(hdbc, &hstmtDelete) != SQL_SUCCESS)
			pDb->DispAllErrors(henv, hdbc);
		// Allocate a separate statement handle for performing updates
		if (SQLAllocStmt(hdbc, &hstmtUpdate) != SQL_SUCCESS)
			pDb->DispAllErrors(henv, hdbc);
	}
	// Allocate a separate statement handle for internal use
	if (SQLAllocStmt(hdbc, &hstmtInternal) != SQL_SUCCESS)
		pDb->DispAllErrors(henv, hdbc);

	// Set the cursor type for the statement handles
	cursorType = SQL_CURSOR_STATIC;
	if (SQLSetStmtOption(hstmtInternal, SQL_CURSOR_TYPE, cursorType) != SQL_SUCCESS)
	{
		// Check to see if cursor type is supported
		pDb->GetNextError(henv, hdbc, hstmtInternal);
		if (! strcmp(pDb->sqlState, "01S02"))  // Option Value Changed
		{
			// Datasource does not support static cursors.  Driver
			// will substitute a cursor type.  Call SQLGetStmtOption()
			// to determine which cursor type was selected.
			if (SQLGetStmtOption(hstmtInternal, SQL_CURSOR_TYPE, &cursorType) != SQL_SUCCESS)
				pDb->DispAllErrors(henv, hdbc, hstmtInternal);
#ifdef DBDEBUG_CONSOLE
			cout << "Static cursor changed to: ";
			switch(cursorType)
			{
			case SQL_CURSOR_FORWARD_ONLY:
				cout << "Forward Only";		break;
			case SQL_CURSOR_STATIC:
				cout << "Static";				break;
			case SQL_CURSOR_KEYSET_DRIVEN:
				cout << "Keyset Driven";	break;
			case SQL_CURSOR_DYNAMIC:
				cout << "Dynamic";			break;
			}
			cout << endl << endl;
#endif
		}
		else
		{
			pDb->DispNextError();
			pDb->DispAllErrors(henv, hdbc, hstmtInternal);
		}
	}
#ifdef DBDEBUG_CONSOLE
	else
		cout << "Cursor Type set to STATIC" << endl << endl;
#endif

	if (!queryOnly)
	{
		// Set the cursor type for the INSERT statement handle
		if (SQLSetStmtOption(hstmtInsert, SQL_CURSOR_TYPE, SQL_CURSOR_FORWARD_ONLY) != SQL_SUCCESS)
			pDb->DispAllErrors(henv, hdbc, hstmtInsert);
		// Set the cursor type for the DELETE statement handle
		if (SQLSetStmtOption(hstmtDelete, SQL_CURSOR_TYPE, SQL_CURSOR_FORWARD_ONLY) != SQL_SUCCESS)
			pDb->DispAllErrors(henv, hdbc, hstmtDelete);
		// Set the cursor type for the UPDATE statement handle
		if (SQLSetStmtOption(hstmtUpdate, SQL_CURSOR_TYPE, SQL_CURSOR_FORWARD_ONLY) != SQL_SUCCESS)
			pDb->DispAllErrors(henv, hdbc, hstmtUpdate);
	}

	// Make the default cursor the active cursor
	hstmtDefault = NewCursor(FALSE,FALSE);
	assert(hstmtDefault);
	hstmt = *hstmtDefault;

}  // wxTable::wxTable()

/********** wxTable::~wxTable() **********/
wxTable::~wxTable()
{
	char s[80];
	if (pDb)
	{
		sprintf(s, "wxTable destructor (%-20s) tableID:[%6lu] pDb:[%p]", tableName,tableID,pDb);
		pDb->WriteSqlLog(s);
	}

#ifndef PROGRAM_FP4UPG
#if __WXDEBUG__ > 0
	if (tableID)
	{
		bool found = FALSE;
		wxNode *pNode;
		pNode = TablesInUse.First();
		while (pNode && !found)
		{
			if (((CstructTablesInUse *)pNode->Data())->tableID == tableID)
			{
				found = TRUE;
				if (!TablesInUse.DeleteNode(pNode))
					wxMessageBox (s,"Unable to delete node!");
			}
			else
				pNode = pNode->Next();
		}
		if (!found)
		{
			char msg[250];
			sprintf(msg,"Unable to find the tableID in the linked\nlist of tables in use.\n\n%s",s);
			wxMessageBox (msg,"NOTICE...");
		}
	}
#endif
#endif
	// Decrement the wxDB table count
	if (pDb)
		pDb->nTables--;

	// Delete memory allocated for column definitions
	if (colDefs)
		delete [] colDefs;

	// Free statement handles
	if (!queryOnly)
	{
		if (hstmtInsert)
			if (SQLFreeStmt(hstmtInsert, SQL_DROP) != SQL_SUCCESS)
				pDb->DispAllErrors(henv, hdbc);
		if (hstmtDelete)
			if (SQLFreeStmt(hstmtDelete, SQL_DROP) != SQL_SUCCESS)
				pDb->DispAllErrors(henv, hdbc);
		if (hstmtUpdate)
			if (SQLFreeStmt(hstmtUpdate, SQL_DROP) != SQL_SUCCESS)
				pDb->DispAllErrors(henv, hdbc);
	}
	if (hstmtInternal)
		if (SQLFreeStmt(hstmtInternal, SQL_DROP) != SQL_SUCCESS)
			pDb->DispAllErrors(henv, hdbc);

	// Delete dynamically allocated cursors
	if (hstmtDefault)
		DeleteCursor(hstmtDefault);
	if (hstmtCount)
		DeleteCursor(hstmtCount);

}  // wxTable::~wxTable()

/********** wxTable::Open() **********/
bool wxTable::Open(void)
{
	if (!pDb)
		return FALSE;

	int i;
	char sqlStmt[DB_MAX_STATEMENT_LEN];

	// Verify that the table exists in the database
	if (!pDb->TableExists(tableName,NULL,tablePath))
	{
		char s[128];
		sprintf(s, "Error opening '%s', table/view does not exist in the database.", tableName);
		pDb->LogError(s);
		return(FALSE);
	}

	// Bind the member variables for field exchange between
	// the wxTable object and the ODBC record.
	if (!queryOnly)
	{
		if (!bindInsertParams())					// Inserts
			return(FALSE);
		if (!bindUpdateParams())					// Updates
			return(FALSE);
	}
	if (!bindCols(*hstmtDefault))					// Selects
		return(FALSE);
	if (!bindCols(hstmtInternal))					// Internal use only
		return(FALSE);
	/*
	 * Do NOT bind the hstmtCount cursor!!!
	 */

	// Build an insert statement using parameter markers
	if (!queryOnly && noCols > 0)
	{
		bool needComma = FALSE;
		sprintf(sqlStmt, "INSERT INTO %s (", tableName);
		for (i = 0; i < noCols; i++)
		{
			if (! colDefs[i].InsertAllowed)
				continue;
			if (needComma)
				strcat(sqlStmt, ",");
			strcat(sqlStmt, colDefs[i].ColName);
			needComma = TRUE;
		}
		needComma = FALSE;
		strcat(sqlStmt, ") VALUES (");
		for (i = 0; i < noCols; i++)
		{
			if (! colDefs[i].InsertAllowed)
				continue;
			if (needComma)
				strcat(sqlStmt, ",");
			strcat(sqlStmt, "?");
			needComma = TRUE;
		}
		strcat(sqlStmt, ")");

//		pDb->WriteSqlLog(sqlStmt);

		// Prepare the insert statement for execution
		if (SQLPrepare(hstmtInsert, (UCHAR FAR *) sqlStmt, SQL_NTS) != SQL_SUCCESS)
			return(pDb->DispAllErrors(henv, hdbc, hstmtInsert));
	}
	
	// Completed successfully
	return(TRUE);

}  // wxTable::Open()

/********** wxTable::Query() **********/
bool wxTable::Query(bool forUpdate, bool distinct)
{

	return(query(DB_SELECT_WHERE, forUpdate, distinct));

}  // wxTable::Query()

/********** wxTable::QueryBySqlStmt() **********/
bool wxTable::QueryBySqlStmt(char *pSqlStmt)
{
	pDb->WriteSqlLog(pSqlStmt);

	return(query(DB_SELECT_STATEMENT, FALSE, FALSE, pSqlStmt));

}  // wxTable::QueryBySqlStmt()

/********** wxTable::QueryMatching() **********/
bool wxTable::QueryMatching(bool forUpdate, bool distinct)
{

	return(query(DB_SELECT_MATCHING, forUpdate, distinct));

}  // wxTable::QueryMatching()

/********** wxTable::QueryOnKeyFields() **********/
bool wxTable::QueryOnKeyFields(bool forUpdate, bool distinct)
{

	return(query(DB_SELECT_KEYFIELDS, forUpdate, distinct));

}  // wxTable::QueryOnKeyFields()

/********** wxTable::query() **********/
bool wxTable::query(int queryType, bool forUpdate, bool distinct, char *pSqlStmt)
{
	char sqlStmt[DB_MAX_STATEMENT_LEN];

	// Set the selectForUpdate member variable
	if (forUpdate)
		// The user may wish to select for update, but the DBMS may not be capable
		selectForUpdate = CanSelectForUpdate();
	else
		selectForUpdate = FALSE;

	// Set the SQL SELECT string
	if (queryType != DB_SELECT_STATEMENT)				// A select statement was not passed in,
	{																// so generate a select statement.
		GetSelectStmt(sqlStmt, queryType, distinct);
		pDb->WriteSqlLog(sqlStmt);
	}

	// Make sure the cursor is closed first
	if (! CloseCursor(hstmt))
		return(FALSE);

	// Execute the SQL SELECT statement
	int retcode;

	retcode = SQLExecDirect(hstmt, (UCHAR FAR *) (queryType == DB_SELECT_STATEMENT ? pSqlStmt : sqlStmt), SQL_NTS);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
		return(pDb->DispAllErrors(henv, hdbc, hstmt));

	// Completed successfully
	return(TRUE);

}  // wxTable::query()

/********** wxTable::GetSelectStmt() **********/
void wxTable::GetSelectStmt(char *pSqlStmt, int typeOfSelect, bool distinct)
{
	char whereClause[DB_MAX_WHERE_CLAUSE_LEN];

	whereClause[0] = 0;

	// Build a select statement to query the database
	strcpy(pSqlStmt, "SELECT ");

	// SELECT DISTINCT values only?
	if (distinct)
		strcat(pSqlStmt, "DISTINCT ");

	// Was a FROM clause specified to join tables to the base table?
	// Available for ::Query() only!!!
	bool appendFromClause = FALSE;
	if (typeOfSelect == DB_SELECT_WHERE && from && strlen(from))
		appendFromClause = TRUE;

	// Add the column list
	int i;
	for (i = 0; i < noCols; i++)
	{
		// If joining tables, the base table column names must be qualified to avoid ambiguity
		if (appendFromClause)
		{
			strcat(pSqlStmt, queryTableName);
			strcat(pSqlStmt, ".");
		}
		strcat(pSqlStmt, colDefs[i].ColName);
		if (i + 1 < noCols)
			strcat(pSqlStmt, ",");
	}

	// If the datasource supports ROWID, get this column as well.  Exception: Don't retrieve
	// the ROWID if querying distinct records.  The rowid will always be unique.
	if (!distinct && CanUpdByROWID())
	{
		// If joining tables, the base table column names must be qualified to avoid ambiguity
		if (appendFromClause)
		{
			strcat(pSqlStmt, ",");
			strcat(pSqlStmt, queryTableName);
			strcat(pSqlStmt, ".ROWID");
		}
		else
			strcat(pSqlStmt, ",ROWID");
	}

	// Append the FROM tablename portion
	strcat(pSqlStmt, " FROM ");
	strcat(pSqlStmt, queryTableName);

	// Sybase uses the HOLDLOCK keyword to lock a record during query.
	// The HOLDLOCK keyword follows the table name in the from clause.
	// Each table in the from clause must specify HOLDLOCK or
	// NOHOLDLOCK (the default).  Note: The "FOR UPDATE" clause
	// is parsed but ignored in SYBASE Transact-SQL.
	if (selectForUpdate && (pDb->Dbms() == dbmsSYBASE_ASA || pDb->Dbms() == dbmsSYBASE_ASE))
		strcat(pSqlStmt, " HOLDLOCK");

	if (appendFromClause)
		strcat(pSqlStmt, from);

	// Append the WHERE clause.  Either append the where clause for the class
	// or build a where clause.  The typeOfSelect determines this.
	switch(typeOfSelect)
	{
	case DB_SELECT_WHERE:
		if (where && strlen(where))	// May not want a where clause!!!
		{
			strcat(pSqlStmt, " WHERE ");
			strcat(pSqlStmt, where);
		}
		break;
	case DB_SELECT_KEYFIELDS:
		GetWhereClause(whereClause, DB_WHERE_KEYFIELDS);
		if (strlen(whereClause))
		{
			strcat(pSqlStmt, " WHERE ");
			strcat(pSqlStmt, whereClause);
		}
		break;
	case DB_SELECT_MATCHING:
		GetWhereClause(whereClause, DB_WHERE_MATCHING);
		if (strlen(whereClause))
		{
			strcat(pSqlStmt, " WHERE ");
			strcat(pSqlStmt, whereClause);
		}
		break;
	}

	// Append the ORDER BY clause
	if (orderBy && strlen(orderBy))
	{
		strcat(pSqlStmt, " ORDER BY ");
		strcat(pSqlStmt, orderBy);
	}

	// SELECT FOR UPDATE if told to do so and the datasource is capable.  Sybase
	// parses the FOR UPDATE clause but ignores it.  See the comment above on the
	// HOLDLOCK for Sybase.
	if (selectForUpdate && CanSelectForUpdate())
		strcat(pSqlStmt, " FOR UPDATE");

}  // wxTable::GetSelectStmt()

/********** wxTable::getRec() **********/
bool wxTable::getRec(UWORD fetchType)
{
	RETCODE retcode;

#ifndef FWD_ONLY_CURSORS
	// Fetch the NEXT, PREV, FIRST or LAST record, depending on fetchType
	UDWORD  cRowsFetched;
	UWORD   rowStatus;
	if ((retcode = SQLExtendedFetch(hstmt, fetchType, 0, &cRowsFetched, &rowStatus)) != SQL_SUCCESS)
		if (retcode == SQL_NO_DATA_FOUND)
			return(FALSE);
		else
			return(pDb->DispAllErrors(henv, hdbc, hstmt));
#else
	// Fetch the next record from the record set

	retcode = SQLFetch(hstmt);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
		if (retcode == SQL_NO_DATA_FOUND)
			return(FALSE);
		else
			return(pDb->DispAllErrors(henv, hdbc, hstmt));
	}
#endif

	// Completed successfully
	return(TRUE);

}  // wxTable::getRec()

/********** wxTable::GetRowNum() **********/
UWORD wxTable::GetRowNum(void)
{
	UDWORD rowNum;

	if (SQLGetStmtOption(hstmt, SQL_ROW_NUMBER, (UCHAR*) &rowNum) != SQL_SUCCESS)
	{
		pDb->DispAllErrors(henv, hdbc, hstmt);
		return(0);
	}

	// Completed successfully
	return((UWORD) rowNum);

}  // wxTable::GetRowNum()

/********** wxTable::bindInsertParams() **********/
bool wxTable::bindInsertParams(void)
{
	assert(!queryOnly);
	if (queryOnly)
		return(FALSE);

	SWORD   fSqlType	= 0;
	UDWORD  precision	= 0;
	SWORD   scale		= 0;

	// Bind each column (that can be inserted) of the table to a parameter marker
	int i;
	for (i = 0; i < noCols; i++)
	{
		if (! colDefs[i].InsertAllowed)
			continue;
		switch(colDefs[i].DbDataType)
		{
		case DB_DATA_TYPE_VARCHAR:
			fSqlType = pDb->typeInfVarchar.FsqlType;
			precision = colDefs[i].SzDataObj;
			scale = 0;
			colDefs[i].CbValue = SQL_NTS;
			break;
		case DB_DATA_TYPE_INTEGER:
			fSqlType = pDb->typeInfInteger.FsqlType;
			precision = pDb->typeInfInteger.Precision;
			scale = 0;
			colDefs[i].CbValue = 0;
			break;
		case DB_DATA_TYPE_FLOAT:
			fSqlType = pDb->typeInfFloat.FsqlType;
			precision = pDb->typeInfFloat.Precision;
			scale = pDb->typeInfFloat.MaximumScale;
			// SQL Sybase Anywhere v5.5 returned a negative number for the
			// MaxScale.  This caused ODBC to kick out an error on ibscale.
			// I check for this here and set the scale = precision.
			//if (scale < 0)
			//	scale = (short) precision;
			colDefs[i].CbValue = 0;
			break;
		case DB_DATA_TYPE_DATE:
			fSqlType = pDb->typeInfDate.FsqlType;
			precision = pDb->typeInfDate.Precision;
			scale = 0;
			colDefs[i].CbValue = 0;
			break;
		}
		// Null values
		if (colDefs[i].Null)
		{
			colDefs[i].CbValue = SQL_NULL_DATA;
			colDefs[i].Null = FALSE;
		}
		if (SQLBindParameter(hstmtInsert, i+1, SQL_PARAM_INPUT, colDefs[i].SqlCtype,
									fSqlType, precision, scale, (UCHAR*) colDefs[i].PtrDataObj, 
									precision+1,&colDefs[i].CbValue) != SQL_SUCCESS)
			return(pDb->DispAllErrors(henv, hdbc, hstmtInsert));
	}

	// Completed successfully
	return(TRUE);

}  // wxTable::bindInsertParams()

/********** wxTable::bindUpdateParams() **********/
bool wxTable::bindUpdateParams(void)
{
	assert(!queryOnly);
	if (queryOnly)
		return(FALSE);

	SWORD   fSqlType	= 0;
	UDWORD  precision	= 0;
	SWORD   scale		= 0;
	
	// Bind each UPDATEABLE column of the table to a parameter marker
	int i,colNo;
	for (i = 0, colNo = 1; i < noCols; i++)
	{
		if (! colDefs[i].Updateable)
			continue;
		switch(colDefs[i].DbDataType)
		{
		case DB_DATA_TYPE_VARCHAR:
			fSqlType = pDb->typeInfVarchar.FsqlType;
			precision = colDefs[i].SzDataObj;
			scale = 0;
			colDefs[i].CbValue = SQL_NTS;
			break;
		case DB_DATA_TYPE_INTEGER:
			fSqlType = pDb->typeInfInteger.FsqlType;
			precision = pDb->typeInfInteger.Precision;
			scale = 0;
			colDefs[i].CbValue = 0;
			break;
		case DB_DATA_TYPE_FLOAT:
			fSqlType = pDb->typeInfFloat.FsqlType;
			precision = pDb->typeInfFloat.Precision;
			scale = pDb->typeInfFloat.MaximumScale;
			// SQL Sybase Anywhere v5.5 returned a negative number for the
			// MaxScale.  This caused ODBC to kick out an error on ibscale.
			// I check for this here and set the scale = precision.
			//if (scale < 0)
			// scale = (short) precision;
			colDefs[i].CbValue = 0;
			break;
		case DB_DATA_TYPE_DATE:
			fSqlType = pDb->typeInfDate.FsqlType;
			precision = pDb->typeInfDate.Precision;
			scale = 0;
			colDefs[i].CbValue = 0;
			break;
		}
		if (SQLBindParameter(hstmtUpdate, colNo++, SQL_PARAM_INPUT, colDefs[i].SqlCtype,
									fSqlType, precision, scale, (UCHAR*) colDefs[i].PtrDataObj, 
									precision+1, &colDefs[i].CbValue) != SQL_SUCCESS)
			return(pDb->DispAllErrors(henv, hdbc, hstmtUpdate));
	}

	// Completed successfully
	return(TRUE);

}  // wxTable::bindUpdateParams()

/********** wxTable::bindCols() **********/
bool wxTable::bindCols(HSTMT cursor)
{
	static SDWORD  cb;
	
	// Bind each column of the table to a memory address for fetching data
	int i;
	for (i = 0; i < noCols; i++)
	{
		if (SQLBindCol(cursor, i+1, colDefs[i].SqlCtype, (UCHAR*) colDefs[i].PtrDataObj,
							colDefs[i].SzDataObj, &cb) != SQL_SUCCESS)
			return(pDb->DispAllErrors(henv, hdbc, cursor));
	}

	// Completed successfully
	return(TRUE);

}  // wxTable::bindCols()

/********** wxTable::CloseCursor() **********/
bool wxTable::CloseCursor(HSTMT cursor)
{
	if (SQLFreeStmt(cursor, SQL_CLOSE) != SQL_SUCCESS)
		return(pDb->DispAllErrors(henv, hdbc, cursor));

	// Completed successfully
	return(TRUE);

}  // wxTable::CloseCursor()

/********** wxTable::CreateTable() **********/
bool wxTable::CreateTable(bool attemptDrop)
{
	if (!pDb)
		return FALSE;

	int i, j;
	char sqlStmt[DB_MAX_STATEMENT_LEN];

#ifdef DBDEBUG_CONSOLE
	cout << "Creating Table " << tableName << "..." << endl;
#endif

	// Drop table first
	if (attemptDrop && !DropTable())
		return FALSE;

	// Create the table
#ifdef DBDEBUG_CONSOLE
	for (i = 0; i < noCols; i++)
	{
		// Exclude derived columns since they are NOT part of the base table
		if (colDefs[i].DerivedCol)
			continue;
		cout << i + 1 << ": " << colDefs[i].ColName << "; ";
		switch(colDefs[i].DbDataType)
		{
			case DB_DATA_TYPE_VARCHAR:
				cout << pDb->typeInfVarchar.TypeName << "(" << colDefs[i].SzDataObj << ")";
				break;
			case DB_DATA_TYPE_INTEGER:
				cout << pDb->typeInfInteger.TypeName;
				break;
			case DB_DATA_TYPE_FLOAT:
				cout << pDb->typeInfFloat.TypeName;
				break;
			case DB_DATA_TYPE_DATE:
				cout << pDb->typeInfDate.TypeName;
				break;
		}
		cout << endl;
	}
#endif

	// Build a CREATE TABLE string from the colDefs structure.
	bool needComma = FALSE;
	sprintf(sqlStmt, "CREATE TABLE %s (", tableName);
	for (i = 0; i < noCols; i++)
	{
		// Exclude derived columns since they are NOT part of the base table
		if (colDefs[i].DerivedCol)
			continue;
		// Comma Delimiter
		if (needComma)
		   strcat(sqlStmt, ",");
		// Column Name
		strcat(sqlStmt, colDefs[i].ColName);
		strcat(sqlStmt, " ");
		// Column Type
		switch(colDefs[i].DbDataType)
		{
			case DB_DATA_TYPE_VARCHAR:
				strcat(sqlStmt, pDb->typeInfVarchar.TypeName); break;
			case DB_DATA_TYPE_INTEGER:
				strcat(sqlStmt, pDb->typeInfInteger.TypeName); break;
			case DB_DATA_TYPE_FLOAT:
				strcat(sqlStmt, pDb->typeInfFloat.TypeName); break;
			case DB_DATA_TYPE_DATE:
				strcat(sqlStmt, pDb->typeInfDate.TypeName); break;
		}
		// For varchars, append the size of the string
		if (colDefs[i].DbDataType == DB_DATA_TYPE_VARCHAR)
		{
			char s[10];
			// strcat(sqlStmt, "(");
			// strcat(sqlStmt, itoa(colDefs[i].SzDataObj, s, 10));
			// strcat(sqlStmt, ")");
			sprintf(s, "(%d)", colDefs[i].SzDataObj);
			strcat(sqlStmt, s);
		}
 
		if (pDb->Dbms() == dbmsSYBASE_ASE || pDb->Dbms() == dbmsMY_SQL)
		{
			if (colDefs[i].KeyField)
			{
					  strcat(sqlStmt, " NOT NULL");
			}
		}
		
		needComma = TRUE;
	}
	// If there is a primary key defined, include it in the create statement
	for (i = j = 0; i < noCols; i++)
	{
		if (colDefs[i].KeyField)
		{
			j++;
			break;
		}
	}
	if (j && pDb->Dbms() != dbmsDBASE)	// Found a keyfield
	{
		if (pDb->Dbms() != dbmsMY_SQL)
		{
			strcat(sqlStmt, ",CONSTRAINT ");
			strcat(sqlStmt, tableName);
			strcat(sqlStmt, "_PIDX PRIMARY KEY (");
		}
		else
		{
			/* MySQL goes out on this one. We also declare the relevant key NON NULL above */
			strcat(sqlStmt, ", PRIMARY KEY (");
		}

		// List column name(s) of column(s) comprising the primary key
		for (i = j = 0; i < noCols; i++)
		{
			if (colDefs[i].KeyField)
			{
				if (j++) // Multi part key, comma separate names
					strcat(sqlStmt, ",");
				strcat(sqlStmt, colDefs[i].ColName);
			}
		}
	   strcat(sqlStmt, ")");
	}
	// Append the closing parentheses for the create table statement
   strcat(sqlStmt, ")");

	pDb->WriteSqlLog(sqlStmt);

#ifdef DBDEBUG_CONSOLE
	cout << endl << sqlStmt << endl;
#endif

	// Execute the CREATE TABLE statement
	if (SQLExecDirect(hstmt, (UCHAR FAR *) sqlStmt, SQL_NTS) != SQL_SUCCESS)
	{
		pDb->DispAllErrors(henv, hdbc, hstmt);
		pDb->RollbackTrans();
		CloseCursor(hstmt);
		return(FALSE);
	}

	// Commit the transaction and close the cursor
	if (! pDb->CommitTrans())
		return(FALSE);
	if (! CloseCursor(hstmt))
		return(FALSE);

	// Database table created successfully
	return(TRUE);

} // wxTable::CreateTable()

/********** wxTable::DropTable() **********/
bool wxTable::DropTable()
{
	// NOTE: This function returns TRUE if the Table does not exist, but
	//       only for identified databases.  Code will need to be added
	//			below for any other databases when those databases are defined
	//       to handle this situation consistently

	char sqlStmt[DB_MAX_STATEMENT_LEN];

	sprintf(sqlStmt, "DROP TABLE %s", tableName);

	pDb->WriteSqlLog(sqlStmt);

#ifdef DBDEBUG_CONSOLE
	cout << endl << sqlStmt << endl;
#endif

	if (SQLExecDirect(hstmt, (UCHAR FAR *) sqlStmt, SQL_NTS) != SQL_SUCCESS)
	{
		// Check for "Base table not found" error and ignore
		pDb->GetNextError(henv, hdbc, hstmt);
		if (strcmp(pDb->sqlState,"S0002"))  // "Base table not found"
		{
			// Check for product specific error codes
			if (!((pDb->Dbms() == dbmsSYBASE_ASA	&& !strcmp(pDb->sqlState,"42000"))	 ||  // 5.x (and lower?)
				   (pDb->Dbms() == dbmsMY_SQL			&& !strcmp(pDb->sqlState,"S1000"))	 ||  // untested
				   (pDb->Dbms() == dbmsPOSTGRES		&& !strcmp(pDb->sqlState,"08S01")))) 	  // untested
			{
				pDb->DispNextError();
				pDb->DispAllErrors(henv, hdbc, hstmt);
				pDb->RollbackTrans();
				CloseCursor(hstmt);
				return(FALSE);
			}
		}
	}

	// Commit the transaction and close the cursor
	if (! pDb->CommitTrans())
		return(FALSE);
	if (! CloseCursor(hstmt))
		return(FALSE);

	return(TRUE);
}  // wxTable::DropTable()

/********** wxTable::CreateIndex() **********/
bool wxTable::CreateIndex(char * idxName, bool unique, int noIdxCols, CidxDef *pIdxDefs, bool attemptDrop)
{
	char sqlStmt[DB_MAX_STATEMENT_LEN];

	// Drop the index first
	if (attemptDrop && !DropIndex(idxName))
		return (FALSE);

	// Build a CREATE INDEX statement
	strcpy(sqlStmt, "CREATE ");
	if (unique)
		strcat(sqlStmt, "UNIQUE ");

	strcat(sqlStmt, "INDEX ");
	strcat(sqlStmt, idxName);
	strcat(sqlStmt, " ON ");
	strcat(sqlStmt, tableName);
	strcat(sqlStmt, " (");

	// Append list of columns making up index
	int i;
	for (i = 0; i < noIdxCols; i++)
	{
		strcat(sqlStmt, pIdxDefs[i].ColName);
      /* Postgres doesn't cope with ASC */
		if (pDb->Dbms() != dbmsPOSTGRES)
		{
			if (pIdxDefs[i].Ascending)
				strcat(sqlStmt, " ASC");
			else
				strcat(sqlStmt, " DESC");
		}

		if ((i + 1) < noIdxCols)
			strcat(sqlStmt, ",");
	}
	
	// Append closing parentheses
	strcat(sqlStmt, ")");

	pDb->WriteSqlLog(sqlStmt);

#ifdef DBDEBUG_CONSOLE
	cout << endl << sqlStmt << endl << endl;
#endif

	// Execute the CREATE INDEX statement
	if (SQLExecDirect(hstmt, (UCHAR FAR *) sqlStmt, SQL_NTS) != SQL_SUCCESS)
	{
		pDb->DispAllErrors(henv, hdbc, hstmt);
		pDb->RollbackTrans();
		CloseCursor(hstmt);
		return(FALSE);
	}

	// Commit the transaction and close the cursor
	if (! pDb->CommitTrans())
		return(FALSE);
	if (! CloseCursor(hstmt))
		return(FALSE);

	// Index Created Successfully
	return(TRUE);

}  // wxTable::CreateIndex()

/********** wxTable::DropIndex() **********/
bool wxTable::DropIndex(char * idxName)
{
	// NOTE: This function returns TRUE if the Index does not exist, but
	//       only for identified databases.  Code will need to be added
	//			below for any other databases when those databases are defined
	//       to handle this situation consistently

	char sqlStmt[DB_MAX_STATEMENT_LEN];

	if (pDb->Dbms() == dbmsACCESS)
		sprintf(sqlStmt, "DROP INDEX %s ON %s",idxName,tableName);
	else if (pDb->Dbms() == dbmsSYBASE_ASE)
		sprintf(sqlStmt, "DROP INDEX %s.%s",tableName,idxName);
	else
		sprintf(sqlStmt, "DROP INDEX %s",idxName);

	pDb->WriteSqlLog(sqlStmt);

#ifdef DBDEBUG_CONSOLE
	cout << endl << sqlStmt << endl;
#endif

	if (SQLExecDirect(hstmt, (UCHAR FAR *) sqlStmt, SQL_NTS) != SQL_SUCCESS)
	{
		// Check for "Index not found" error and ignore
		pDb->GetNextError(henv, hdbc, hstmt);
		if (strcmp(pDb->sqlState,"S0012"))  // "Index not found"
		{
			// Check for product specific error codes
			if (!((pDb->Dbms() == dbmsSYBASE_ASA	&& !strcmp(pDb->sqlState,"42000"))   ||  // v5.x (and lower?)
				   (pDb->Dbms() == dbmsSYBASE_ASE	&& !strcmp(pDb->sqlState,"S0002"))   ||  // Base table not found
				   (pDb->Dbms() == dbmsMY_SQL			&& !strcmp(pDb->sqlState,"42S02"))       // untested
					))
			{
				pDb->DispNextError();
				pDb->DispAllErrors(henv, hdbc, hstmt);
				pDb->RollbackTrans();
				CloseCursor(hstmt);
				return(FALSE);
			}
		}
	}

	// Commit the transaction and close the cursor
	if (! pDb->CommitTrans())
		return(FALSE);
	if (! CloseCursor(hstmt))
		return(FALSE);

	return(TRUE);
}  // wxTable::DropIndex()

/********** wxTable::Insert() **********/
int wxTable::Insert(void)
{
	assert(!queryOnly);
	if (queryOnly)
		return(DB_FAILURE);

	bindInsertParams();

	// Insert the record by executing the already prepared insert statement
	RETCODE retcode;
	retcode=SQLExecute(hstmtInsert);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
		// Check to see if integrity constraint was violated
		pDb->GetNextError(henv, hdbc, hstmtInsert);
		if (! strcmp(pDb->sqlState, "23000"))  // Integrity constraint violated
			return(DB_ERR_INTEGRITY_CONSTRAINT_VIOL);
		else
		{
			pDb->DispNextError();
			pDb->DispAllErrors(henv, hdbc, hstmtInsert);
			return(DB_FAILURE);
		}
	}

	// Record inserted into the datasource successfully
	return(DB_SUCCESS);

}  // wxTable::Insert()

/********** wxTable::Update(pSqlStmt) **********/
bool wxTable::Update(char *pSqlStmt)
{
	assert(!queryOnly);
	if (queryOnly)
		return(FALSE);

	pDb->WriteSqlLog(pSqlStmt);

	return(execUpdate(pSqlStmt));

}  // wxTable::Update(pSqlStmt)

/********** wxTable::Update() **********/
bool wxTable::Update(void)
{
	assert(!queryOnly);
	if (queryOnly)
		return(FALSE);

	char sqlStmt[DB_MAX_STATEMENT_LEN];

	// Build the SQL UPDATE statement
	GetUpdateStmt(sqlStmt, DB_UPD_KEYFIELDS);

	pDb->WriteSqlLog(sqlStmt);

#ifdef DBDEBUG_CONSOLE
	cout << endl << sqlStmt << endl << endl;
#endif

	// Execute the SQL UPDATE statement
	return(execUpdate(sqlStmt));

}  // wxTable::Update()

/********** wxTable::UpdateWhere() **********/
bool wxTable::UpdateWhere(char *pWhereClause)
{
	assert(!queryOnly);
	if (queryOnly)
		return(FALSE);

	char sqlStmt[DB_MAX_STATEMENT_LEN];

	// Build the SQL UPDATE statement
	GetUpdateStmt(sqlStmt, DB_UPD_WHERE, pWhereClause);

	pDb->WriteSqlLog(sqlStmt);

#ifdef DBDEBUG_CONSOLE
	cout << endl << sqlStmt << endl << endl;
#endif

	// Execute the SQL UPDATE statement
	return(execUpdate(sqlStmt));

}  // wxTable::UpdateWhere()

/********** wxTable::Delete() **********/
bool wxTable::Delete(void)
{
	assert(!queryOnly);
	if (queryOnly)
		return(FALSE);

	char sqlStmt[DB_MAX_STATEMENT_LEN];

	// Build the SQL DELETE statement
	GetDeleteStmt(sqlStmt, DB_DEL_KEYFIELDS);

	pDb->WriteSqlLog(sqlStmt);

	// Execute the SQL DELETE statement
	return(execDelete(sqlStmt));

}  // wxTable::Delete()

/********** wxTable::DeleteWhere() **********/
bool wxTable::DeleteWhere(char *pWhereClause)
{
	assert(!queryOnly);
	if (queryOnly)
		return(FALSE);

	char sqlStmt[DB_MAX_STATEMENT_LEN];

	// Build the SQL DELETE statement
	GetDeleteStmt(sqlStmt, DB_DEL_WHERE, pWhereClause);

	pDb->WriteSqlLog(sqlStmt);

	// Execute the SQL DELETE statement
	return(execDelete(sqlStmt));

}  // wxTable::DeleteWhere()

/********** wxTable::DeleteMatching() **********/
bool wxTable::DeleteMatching(void)
{
	assert(!queryOnly);
	if (queryOnly)
		return(FALSE);

	char sqlStmt[DB_MAX_STATEMENT_LEN];

	// Build the SQL DELETE statement
	GetDeleteStmt(sqlStmt, DB_DEL_MATCHING);

	pDb->WriteSqlLog(sqlStmt);

	// Execute the SQL DELETE statement
	return(execDelete(sqlStmt));

}  // wxTable::DeleteMatching()

/********** wxTable::execDelete() **********/
bool wxTable::execDelete(char *pSqlStmt)
{
	// Execute the DELETE statement
	if (SQLExecDirect(hstmtDelete, (UCHAR FAR *) pSqlStmt, SQL_NTS) != SQL_SUCCESS)
		return(pDb->DispAllErrors(henv, hdbc, hstmtDelete));

	// Record deleted successfully
	return(TRUE);

}  // wxTable::execDelete()

/********** wxTable::execUpdate() **********/
bool wxTable::execUpdate(char *pSqlStmt)
{
	// Execute the UPDATE statement
	if (SQLExecDirect(hstmtUpdate, (UCHAR FAR *) pSqlStmt, SQL_NTS) != SQL_SUCCESS)
		return(pDb->DispAllErrors(henv, hdbc, hstmtUpdate));

	// Record deleted successfully
	return(TRUE);

}  // wxTable::execUpdate()

/********** wxTable::GetUpdateStmt() **********/
void wxTable::GetUpdateStmt(char *pSqlStmt, int typeOfUpd, char *pWhereClause)
{
	assert(!queryOnly);
	if (queryOnly)
		return;

	char whereClause[DB_MAX_WHERE_CLAUSE_LEN];
	bool firstColumn = TRUE;

	whereClause[0] = 0;
	sprintf(pSqlStmt, "UPDATE %s SET ", tableName);

	// Append a list of columns to be updated
	int i;
	for (i = 0; i < noCols; i++)
	{
		// Only append Updateable columns
		if (colDefs[i].Updateable)
		{
			if (! firstColumn)
				strcat(pSqlStmt, ",");
			else
				firstColumn = FALSE;
			strcat(pSqlStmt, colDefs[i].ColName);
			strcat(pSqlStmt, " = ?");
		}
	}

	// Append the WHERE clause to the SQL UPDATE statement
	strcat(pSqlStmt, " WHERE ");
	switch(typeOfUpd)
	{
	case DB_UPD_KEYFIELDS:
		// If the datasource supports the ROWID column, build
		// the where on ROWID for efficiency purposes.
		// e.g. UPDATE PARTS SET Col1 = ?, Col2 = ? WHERE ROWID = '111.222.333'
		if (CanUpdByROWID())
		{
			SDWORD cb;
			char   rowid[ROWID_LEN];

			// Get the ROWID value.  If not successful retreiving the ROWID,
			// simply fall down through the code and build the WHERE clause
			// based on the key fields.
			if (SQLGetData(hstmt, noCols+1, SQL_C_CHAR, (UCHAR*) rowid, ROWID_LEN, &cb) == SQL_SUCCESS)
			{
				strcat(pSqlStmt, "ROWID = '");
				strcat(pSqlStmt, rowid);
				strcat(pSqlStmt, "'");
				break;
			}
		}
		// Unable to delete by ROWID, so build a WHERE
		// clause based on the keyfields.
		GetWhereClause(whereClause, DB_WHERE_KEYFIELDS);
		strcat(pSqlStmt, whereClause);
		break;
	case DB_UPD_WHERE:
		strcat(pSqlStmt, pWhereClause);
		break;
	}

}  // GetUpdateStmt()

/********** wxTable::GetDeleteStmt() **********/
void wxTable::GetDeleteStmt(char *pSqlStmt, int typeOfDel, char *pWhereClause)
{
	assert(!queryOnly);
	if (queryOnly)
		return;

	char whereClause[DB_MAX_WHERE_CLAUSE_LEN];

	whereClause[0] = 0;

	// Handle the case of DeleteWhere() and the where clause is blank.  It should
	// delete all records from the database in this case.
	if (typeOfDel == DB_DEL_WHERE && (pWhereClause == 0 || strlen(pWhereClause) == 0))
	{
		sprintf(pSqlStmt, "DELETE FROM %s", tableName);
		return;
	}

	sprintf(pSqlStmt, "DELETE FROM %s WHERE ", tableName);

	// Append the WHERE clause to the SQL DELETE statement
	switch(typeOfDel)
	{
	case DB_DEL_KEYFIELDS:
		// If the datasource supports the ROWID column, build
		// the where on ROWID for efficiency purposes.
		// e.g. DELETE FROM PARTS WHERE ROWID = '111.222.333'
		if (CanUpdByROWID())
		{
			SDWORD cb;
			char   rowid[ROWID_LEN];

			// Get the ROWID value.  If not successful retreiving the ROWID,
			// simply fall down through the code and build the WHERE clause
			// based on the key fields.
			if (SQLGetData(hstmt, noCols+1, SQL_C_CHAR, (UCHAR*) rowid, ROWID_LEN, &cb) == SQL_SUCCESS)
			{
				strcat(pSqlStmt, "ROWID = '");
				strcat(pSqlStmt, rowid);
				strcat(pSqlStmt, "'");
				break;
			}
		}
		// Unable to delete by ROWID, so build a WHERE
		// clause based on the keyfields.
		GetWhereClause(whereClause, DB_WHERE_KEYFIELDS);
		strcat(pSqlStmt, whereClause);
		break;
	case DB_DEL_WHERE:
		strcat(pSqlStmt, pWhereClause);
		break;
	case DB_DEL_MATCHING:
		GetWhereClause(whereClause, DB_WHERE_MATCHING);
		strcat(pSqlStmt, whereClause);
		break;
	}

}  // GetDeleteStmt()

/********** wxTable::GetWhereClause() **********/
/*
 * Note: GetWhereClause() currently ignores timestamp columns.
 *       They are not included as part of the where clause.
 */

void wxTable::GetWhereClause(char *pWhereClause, int typeOfWhere, char *qualTableName)
{
	bool moreThanOneColumn = FALSE;
	char colValue[255];

	// Loop through the columns building a where clause as you go
	int i;
	for (i = 0; i < noCols; i++)
	{
		// Determine if this column should be included in the WHERE clause
		if ((typeOfWhere == DB_WHERE_KEYFIELDS && colDefs[i].KeyField) ||
			 (typeOfWhere == DB_WHERE_MATCHING  && (! IsColNull(i))))
		{
			// Skip over timestamp columns
			if (colDefs[i].SqlCtype == SQL_C_TIMESTAMP)
				continue;
			// If there is more than 1 column, join them with the keyword "AND"
			if (moreThanOneColumn)
				strcat(pWhereClause, " AND ");
			else
				moreThanOneColumn = TRUE;
			// Concatenate where phrase for the column
			if (qualTableName && strlen(qualTableName))
			{
				strcat(pWhereClause, qualTableName);
				strcat(pWhereClause, ".");
			}
			strcat(pWhereClause, colDefs[i].ColName);
			strcat(pWhereClause, " = ");
			switch(colDefs[i].SqlCtype)
			{
			case SQL_C_CHAR:
				sprintf(colValue, "'%s'", (UCHAR FAR *) colDefs[i].PtrDataObj);
				break;
			case SQL_C_SSHORT:
				sprintf(colValue, "%hi", *((SWORD *) colDefs[i].PtrDataObj));
				break;
			case SQL_C_USHORT:
				sprintf(colValue, "%hu", *((UWORD *) colDefs[i].PtrDataObj));
				break;
			case SQL_C_SLONG:
				sprintf(colValue, "%li", *((SDWORD *) colDefs[i].PtrDataObj));
				break;
			case SQL_C_ULONG:
				sprintf(colValue, "%lu", *((UDWORD *) colDefs[i].PtrDataObj));
				break;
			case SQL_C_FLOAT:
				sprintf(colValue, "%.6f", *((SFLOAT *) colDefs[i].PtrDataObj));
				break;
			case SQL_C_DOUBLE:
				sprintf(colValue, "%.6f", *((SDOUBLE *) colDefs[i].PtrDataObj));
				break;
			}
			strcat(pWhereClause, colValue);
		}
	}

}  // wxTable::GetWhereClause()

/********** wxTable::IsColNull() **********/
bool wxTable::IsColNull(int colNo)
{
	switch(colDefs[colNo].SqlCtype)
	{
	case SQL_C_CHAR:
		return(((UCHAR FAR *) colDefs[colNo].PtrDataObj)[0] == 0);
	case SQL_C_SSHORT:
		return((  *((SWORD *) colDefs[colNo].PtrDataObj))   == 0);
	case SQL_C_USHORT:
		return((   *((UWORD*) colDefs[colNo].PtrDataObj))   == 0);
	case SQL_C_SLONG:
		return(( *((SDWORD *) colDefs[colNo].PtrDataObj))   == 0);
	case SQL_C_ULONG:
		return(( *((UDWORD *) colDefs[colNo].PtrDataObj))   == 0);
	case SQL_C_FLOAT:
		return(( *((SFLOAT *) colDefs[colNo].PtrDataObj))   == 0);
	case SQL_C_DOUBLE:
		return((*((SDOUBLE *) colDefs[colNo].PtrDataObj))   == 0);
	case SQL_C_TIMESTAMP:
		TIMESTAMP_STRUCT *pDt;
		pDt = (TIMESTAMP_STRUCT *) colDefs[colNo].PtrDataObj;
		if (pDt->year == 0 && pDt->month == 0 && pDt->day == 0)
			return(TRUE);
		else
			return(FALSE);
	default:
		return(TRUE);
	}

}  // wxTable::IsColNull()

/********** wxTable::CanSelectForUpdate() **********/
bool wxTable::CanSelectForUpdate(void)
{
	if (pDb->Dbms() == dbmsMY_SQL)
		return FALSE;

	if (pDb->dbInf.posStmts & SQL_PS_SELECT_FOR_UPDATE)
		return(TRUE);
	else
		return(FALSE);

}  // wxTable::CanSelectForUpdate()

/********** wxTable::CanUpdByROWID() **********/
bool wxTable::CanUpdByROWID(void)
{

//NOTE: Returning FALSE for now until this can be debugged,
//	     as the ROWID is not getting updated correctly
	return FALSE;

	if (pDb->Dbms() == dbmsORACLE)
		return(TRUE);
	else
		return(FALSE);

}  // wxTable::CanUpdByROWID()

/********** wxTable::IsCursorClosedOnCommit() **********/
bool wxTable::IsCursorClosedOnCommit(void)
{
	if (pDb->dbInf.cursorCommitBehavior == SQL_CB_PRESERVE)
		return(FALSE);
	else
		return(TRUE);

}  // wxTable::IsCursorClosedOnCommit()

/********** wxTable::ClearMemberVars() **********/
void wxTable::ClearMemberVars(void)
{
	// Loop through the columns setting each member variable to zero
	int i;
	for (i = 0; i < noCols; i++)
	{
		switch(colDefs[i].SqlCtype)
		{
		case SQL_C_CHAR:
			((UCHAR FAR *) colDefs[i].PtrDataObj)[0]	= 0;
			break;
		case SQL_C_SSHORT:
			*((SWORD *) colDefs[i].PtrDataObj)			= 0;
			break;
		case SQL_C_USHORT:
			*((UWORD*) colDefs[i].PtrDataObj)			= 0;
			break;
		case SQL_C_SLONG:
			*((SDWORD *) colDefs[i].PtrDataObj)			= 0;
			break;
		case SQL_C_ULONG:
			*((UDWORD *) colDefs[i].PtrDataObj)			= 0;
			break;
		case SQL_C_FLOAT:
			*((SFLOAT *) colDefs[i].PtrDataObj)			= 0.0f;
			break;
		case SQL_C_DOUBLE:
			*((SDOUBLE *) colDefs[i].PtrDataObj)		= 0.0f;
			break;
		case SQL_C_TIMESTAMP:
			TIMESTAMP_STRUCT *pDt;
			pDt = (TIMESTAMP_STRUCT *) colDefs[i].PtrDataObj;
			pDt->year = 0;
			pDt->month = 0;
			pDt->day = 0;
			pDt->hour = 0;
			pDt->minute = 0;
			pDt->second = 0;
			pDt->fraction = 0;
			break;
		}
	}

}  // wxTable::ClearMemberVars()

/********** wxTable::SetQueryTimeout() **********/
bool wxTable::SetQueryTimeout(UDWORD nSeconds)
{
	if (SQLSetStmtOption(hstmtInsert, SQL_QUERY_TIMEOUT, nSeconds) != SQL_SUCCESS)
		return(pDb->DispAllErrors(henv, hdbc, hstmtInsert));
	if (SQLSetStmtOption(hstmtUpdate, SQL_QUERY_TIMEOUT, nSeconds) != SQL_SUCCESS)
		return(pDb->DispAllErrors(henv, hdbc, hstmtUpdate));
	if (SQLSetStmtOption(hstmtDelete, SQL_QUERY_TIMEOUT, nSeconds) != SQL_SUCCESS)
		return(pDb->DispAllErrors(henv, hdbc, hstmtDelete));
	if (SQLSetStmtOption(hstmtInternal, SQL_QUERY_TIMEOUT, nSeconds) != SQL_SUCCESS)
		return(pDb->DispAllErrors(henv, hdbc, hstmtInternal));

	// Completed Successfully
	return(TRUE);

}  // wxTable::SetQueryTimeout()

/********** wxTable::SetColDefs() **********/
void wxTable::SetColDefs (int index, char *fieldName, int dataType, void *pData,
								 int cType, int size, bool keyField, bool upd,
								 bool insAllow, bool derivedCol)
{
	if (!colDefs)  // May happen if the database connection fails
		return;

	if (strlen(fieldName) > (unsigned int) DB_MAX_COLUMN_NAME_LEN)
	{
		strncpy (colDefs[index].ColName, fieldName, DB_MAX_COLUMN_NAME_LEN);
		colDefs[index].ColName[DB_MAX_COLUMN_NAME_LEN] = 0;
	}
	else
		strcpy(colDefs[index].ColName, fieldName);

	colDefs[index].DbDataType		= dataType;
	colDefs[index].PtrDataObj		= pData;
	colDefs[index].SqlCtype			= cType;
	colDefs[index].SzDataObj		= size;
	colDefs[index].KeyField			= keyField;
	colDefs[index].DerivedCol		= derivedCol;
	// Derived columns by definition would NOT be "Insertable" or "Updateable"
	if (derivedCol)
	{
		colDefs[index].Updateable		= FALSE;
		colDefs[index].InsertAllowed	= FALSE;
	}
	else
	{
		colDefs[index].Updateable		= upd;
		colDefs[index].InsertAllowed	= insAllow;
	}

	colDefs[index].Null					= FALSE;
	
}  // wxTable::SetColDefs()

/********** wxTable::SetCursor() **********/
void wxTable::SetCursor(HSTMT *hstmtActivate)
{
	if (hstmtActivate == DEFAULT_CURSOR)
		hstmt = *hstmtDefault;
	else
		hstmt = *hstmtActivate;

}  // wxTable::SetCursor()

/********** wxTable::Count() **********/
ULONG wxTable::Count(void)
{
	ULONG l;
	char sqlStmt[DB_MAX_STATEMENT_LEN];
	SDWORD cb;

	// Build a "SELECT COUNT(*) FROM queryTableName [WHERE whereClause]" SQL Statement
	strcpy(sqlStmt, "SELECT COUNT(*) FROM ");
	strcat(sqlStmt, queryTableName);

	if (from && strlen(from))
		strcat(sqlStmt, from);

	// Add the where clause if one is provided
	if (where && strlen(where))
	{
		strcat(sqlStmt, " WHERE ");
		strcat(sqlStmt, where);
	}

	pDb->WriteSqlLog(sqlStmt);

	// Initialize the Count cursor if it's not already initialized
	if (!hstmtCount)
	{
		hstmtCount = NewCursor(FALSE,FALSE);
		assert(hstmtCount);
		if (!hstmtCount)
			return(0);
	}

	// Execute the SQL statement
	if (SQLExecDirect(*hstmtCount, (UCHAR FAR *) sqlStmt, SQL_NTS) != SQL_SUCCESS)
	{
		pDb->DispAllErrors(henv, hdbc, *hstmtCount);
		return(0);
	}

	// Fetch the record
	if (SQLFetch(*hstmtCount) != SQL_SUCCESS)
	{
		pDb->DispAllErrors(henv, hdbc, *hstmtCount);
		return(0);
	}

	// Obtain the result
	if (SQLGetData(*hstmtCount, 1, SQL_C_ULONG, &l, sizeof(l), &cb) != SQL_SUCCESS)
	{
		pDb->DispAllErrors(henv, hdbc, *hstmtCount);
		return(0);
	}

	// Free the cursor
	if (SQLFreeStmt(*hstmtCount, SQL_CLOSE) != SQL_SUCCESS)
		pDb->DispAllErrors(henv, hdbc, *hstmtCount);

	// Return the record count
	return(l);

}  // wxTable::Count()

/********** wxTable::Refresh() **********/
bool wxTable::Refresh(void)
{
	bool result = TRUE;

	// Switch to the internal cursor so any active cursors are not corrupted
	HSTMT currCursor = GetCursor();
	hstmt = hstmtInternal;

	// Save the where and order by clauses
	char *saveWhere	= where;
	char *saveOrderBy = orderBy;

	// Build a where clause to refetch the record with.  Try and use the
	// ROWID if it's available, ow use the key fields.
	char whereClause[DB_MAX_WHERE_CLAUSE_LEN+1];
	strcpy(whereClause, "");
	if (CanUpdByROWID())
	{
		SDWORD cb;
		char   rowid[ROWID_LEN+1];

		// Get the ROWID value.  If not successful retreiving the ROWID,
		// simply fall down through the code and build the WHERE clause
		// based on the key fields.
		if (SQLGetData(hstmt, noCols+1, SQL_C_CHAR, (UCHAR*) rowid, ROWID_LEN, &cb) == SQL_SUCCESS)
		{
			strcat(whereClause, queryTableName);
			strcat(whereClause, ".ROWID = '");
			strcat(whereClause, rowid);
			strcat(whereClause, "'");
		}
	}

	// If unable to use the ROWID, build a where clause from the keyfields
	if (strlen(whereClause) == 0)
		GetWhereClause(whereClause, DB_WHERE_KEYFIELDS, queryTableName);

	// Requery the record
	where = whereClause;
	orderBy = 0;
	if (!Query())
		result = FALSE;

	if (result && !GetNext())
		result = FALSE;

	// Switch back to original cursor
	SetCursor(&currCursor);

	// Free the internal cursor
	if (SQLFreeStmt(hstmtInternal, SQL_CLOSE) != SQL_SUCCESS)
		pDb->DispAllErrors(henv, hdbc, hstmtInternal);

	// Restore the original where and order by clauses
	where	  = saveWhere;
	orderBy = saveOrderBy;

	return(result);

}  // wxTable::Refresh()

/********** wxTable::SetNull(UINT colNo) **********/
bool wxTable::SetNull(int colNo)
{
	if (colNo < noCols)
		return(colDefs[colNo].Null = TRUE);
	else
		return(FALSE);

}  // wxTable::SetNull(UINT colNo)

/********** wxTable::SetNull(char *colName) **********/
bool wxTable::SetNull(char *colName)
{
	int i;
	for (i = 0; i < noCols; i++)
	{
		if (!stricmp(colName, colDefs[i].ColName))
			break;
	}

	if (i < noCols)
		return(colDefs[i].Null = TRUE);
	else
		return(FALSE);

}  // wxTable::SetNull(char *colName)

/********** wxTable::NewCursor() **********/
HSTMT *wxTable::NewCursor(bool setCursor, bool bindColumns)
{
	HSTMT *newHSTMT = new HSTMT;
	assert(newHSTMT);
	if (!newHSTMT)
		return(0);

	if (SQLAllocStmt(hdbc, newHSTMT) != SQL_SUCCESS)
	{
		pDb->DispAllErrors(henv, hdbc);
		delete newHSTMT;
		return(0);
	}

	if (SQLSetStmtOption(*newHSTMT, SQL_CURSOR_TYPE, cursorType) != SQL_SUCCESS)
	{
		pDb->DispAllErrors(henv, hdbc, *newHSTMT);
		delete newHSTMT;
		return(0);
	}

	if (bindColumns)
	{
		if(!bindCols(*newHSTMT))
		{
			delete newHSTMT;
			return(0);
		}
	}

	if (setCursor)
		SetCursor(newHSTMT);

	return(newHSTMT);

}   // wxTable::NewCursor()

/********** wxTable::DeleteCursor() **********/
bool wxTable::DeleteCursor(HSTMT *hstmtDel)
{
	bool result = TRUE;

	if (!hstmtDel)  // Cursor already deleted
		return(result);

	if (SQLFreeStmt(*hstmtDel, SQL_DROP) != SQL_SUCCESS)
	{
		pDb->DispAllErrors(henv, hdbc);
		result = FALSE;
	}

	delete hstmtDel;

	return(result);

}  // wxTable::DeleteCursor()

#endif  // wxUSE_ODBC

