///////////////////////////////////////////////////////////////////////////////
// Name:        table.cpp
// Purpose:     Implementation of the wxTable class.
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

/*
#ifdef _CONSOLE
	#include <iostream.h>
#endif
*/

#include  "wx/wxprec.h"

#ifdef    __BORLANDC__
  #pragma hdrstop
#endif  //__BORLANDC__

#ifndef WX_PRECOMP
  #include  <wx/wx.h>
#endif //WX_PRECOMP

#if wxUSE_ODBC

#include <wx/dbtable.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __WXUNIX__
// The HPUX preprocessor lines below were commented out on 8/20/97
// because macros.h currently redefines DEBUG and is unneeded.
// #  ifdef HPUX
// #    include <macros.h>
// #  endif
#  ifdef __WXLINUX__
#    include <sys/minmax.h>
#  endif
#endif

/********** wxTable::wxTable() **********/
wxTable::wxTable(wxDB *pwxDB, const char *tblName, const int nCols, const char *qryTblName)
{
	// Assign member variables
	pDb = pwxDB;									// Pointer to the wxDB object
	strcpy(tableName, tblName);				// Table Name
	if (qryTblName)								// Name of the table/view to query
		strcpy(queryTableName, qryTblName);
	else
		strcpy(queryTableName, tblName);

	noCols = nCols;								// No. of cols in the table
	where = 0;										// Where clause
	orderBy = 0;									// Order By clause
	selectForUpdate = FALSE;					// SELECT ... FOR UPDATE; Indicates whether to include the FOR UPDATE phrase

	// Grab the HENV and HDBC from the wxDB object
	henv = pDb->henv;
	hdbc = pDb->hdbc;

	// Allocate space for column definitions
	if (noCols)
		colDefs = new CcolDef[noCols];  // Points to the first column defintion
	else
		colDefs = 0;

	// Allocate statement handles for the table
	if (SQLAllocStmt(hdbc, &c0) != SQL_SUCCESS)
		pDb->DispAllErrors(henv, hdbc);
	if (SQLAllocStmt(hdbc, &c1) != SQL_SUCCESS)
		pDb->DispAllErrors(henv, hdbc);
	if (SQLAllocStmt(hdbc, &c2) != SQL_SUCCESS)
		pDb->DispAllErrors(henv, hdbc);
//	if (SQLAllocStmt(hdbc, &c3) != SQL_SUCCESS)
//		pDb->DispAllErrors(henv, hdbc);
//	if (SQLAllocStmt(hdbc, &c4) != SQL_SUCCESS)
//		pDb->DispAllErrors(henv, hdbc);
//	if (SQLAllocStmt(hdbc, &c5) != SQL_SUCCESS)
//		pDb->DispAllErrors(henv, hdbc);
	// Allocate a separate statement handle for performing inserts
	if (SQLAllocStmt(hdbc, &hstmtInsert) != SQL_SUCCESS)
		pDb->DispAllErrors(henv, hdbc);
	// Allocate a separate statement handle for performing deletes
	if (SQLAllocStmt(hdbc, &hstmtDelete) != SQL_SUCCESS)
		pDb->DispAllErrors(henv, hdbc);
	// Allocate a separate statement handle for performing updates
	if (SQLAllocStmt(hdbc, &hstmtUpdate) != SQL_SUCCESS)
		pDb->DispAllErrors(henv, hdbc);
	// Allocate a separate statement handle for performing count(*) function
	if (SQLAllocStmt(hdbc, &hstmtCount) != SQL_SUCCESS)
		pDb->DispAllErrors(henv, hdbc);

	// Set the cursor type for the statement handles
	UDWORD cursorType = SQL_CURSOR_STATIC;
	if (SQLSetStmtOption(c1, SQL_CURSOR_TYPE, cursorType) != SQL_SUCCESS)
	{
		// Check to see if cursor type is supported
		pDb->GetNextError(henv, hdbc, c1);
		if (! strcmp(pDb->sqlState, "01S02"))  // Option Value Changed
		{
			// Datasource does not support static cursors.  Driver
			// will substitute a cursor type.  Call SQLGetStmtOption()
			// to determine which cursor type was selected.
			if (SQLGetStmtOption(c1, SQL_CURSOR_TYPE, &cursorType) != SQL_SUCCESS)
				pDb->DispAllErrors(henv, hdbc, c1);
#ifdef _CONSOLE
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
			pDb->DispAllErrors(henv, hdbc, c1);
		}
	}
#ifdef _CONSOLE
	else
		cout << "Cursor Type set to STATIC" << endl << endl;
#endif

	if (SQLSetStmtOption(c0, SQL_CURSOR_TYPE, cursorType) != SQL_SUCCESS)
		pDb->DispAllErrors(henv, hdbc, c0);
	if (SQLSetStmtOption(c2, SQL_CURSOR_TYPE, cursorType) != SQL_SUCCESS)
		pDb->DispAllErrors(henv, hdbc, c2);
//	if (SQLSetStmtOption(c3, SQL_CURSOR_TYPE, cursorType) != SQL_SUCCESS)
//		pDb->DispAllErrors(henv, hdbc, c3);
//	if (SQLSetStmtOption(c4, SQL_CURSOR_TYPE, cursorType) != SQL_SUCCESS)
//		pDb->DispAllErrors(henv, hdbc, c4);
//	if (SQLSetStmtOption(c5, SQL_CURSOR_TYPE, cursorType) != SQL_SUCCESS)
//		pDb->DispAllErrors(henv, hdbc, c5);

	// Set the cursor type for the INSERT statement handle
	if (SQLSetStmtOption(hstmtInsert, SQL_CURSOR_TYPE, SQL_CURSOR_FORWARD_ONLY) != SQL_SUCCESS)
		pDb->DispAllErrors(henv, hdbc, hstmtInsert);
	// Set the cursor type for the DELETE statement handle
	if (SQLSetStmtOption(hstmtDelete, SQL_CURSOR_TYPE, SQL_CURSOR_FORWARD_ONLY) != SQL_SUCCESS)
		pDb->DispAllErrors(henv, hdbc, hstmtDelete);
	// Set the cursor type for the UPDATE statement handle
	if (SQLSetStmtOption(hstmtUpdate, SQL_CURSOR_TYPE, SQL_CURSOR_FORWARD_ONLY) != SQL_SUCCESS)
		pDb->DispAllErrors(henv, hdbc, hstmtUpdate);
	// Set the cursor type for the COUNT(*) statement handle
	if (SQLSetStmtOption(hstmtCount, SQL_CURSOR_TYPE, SQL_CURSOR_FORWARD_ONLY) != SQL_SUCCESS)
		pDb->DispAllErrors(henv, hdbc, hstmtCount);

	// Copy cursor 1 to the default cursor
	hstmt = c1;
	currCursorNo = DB_CURSOR1;

}  // wxTable::wxTable()

/********** wxTable::~wxTable() **********/
wxTable::~wxTable()
{
	// Delete memory allocated for column definitions
	if (colDefs)
		delete [] colDefs;

	// Free statement handles
	if (SQLFreeStmt(c0, SQL_DROP) != SQL_SUCCESS)
		pDb->DispAllErrors(henv, hdbc);
	if (SQLFreeStmt(c1, SQL_DROP) != SQL_SUCCESS)
		pDb->DispAllErrors(henv, hdbc);
	if (SQLFreeStmt(c2, SQL_DROP) != SQL_SUCCESS)
		pDb->DispAllErrors(henv, hdbc);
//	if (SQLFreeStmt(c3, SQL_DROP) != SQL_SUCCESS)
//		pDb->DispAllErrors(henv, hdbc);
//	if (SQLFreeStmt(c4, SQL_DROP) != SQL_SUCCESS)
//		pDb->DispAllErrors(henv, hdbc);
//	if (SQLFreeStmt(c5, SQL_DROP) != SQL_SUCCESS)
//		pDb->DispAllErrors(henv, hdbc);
	if (SQLFreeStmt(hstmtInsert, SQL_DROP) != SQL_SUCCESS)
		pDb->DispAllErrors(henv, hdbc);
	if (SQLFreeStmt(hstmtDelete, SQL_DROP) != SQL_SUCCESS)
		pDb->DispAllErrors(henv, hdbc);
	if (SQLFreeStmt(hstmtUpdate, SQL_DROP) != SQL_SUCCESS)
		pDb->DispAllErrors(henv, hdbc);
	if (SQLFreeStmt(hstmtCount, SQL_DROP) != SQL_SUCCESS)
		pDb->DispAllErrors(henv, hdbc);

}  // wxTable::~wxTable()

/********** wxTable::Open() **********/
bool wxTable::Open(void)
{
	int i;
	char sqlStmt[DB_MAX_STATEMENT_LEN];

	// Verify that the table exists in the database
	if (!pDb->TableExists(tableName))
	{
        wxString s;
		s.Printf("Error opening '%s', table/view does not exist in the database.", tableName);
		pDb->LogError(s);
		return(FALSE);
	}

	// Bind the member variables for field exchange between
	// the wxTable object and the ODBC record.
	if(! bindInsertParams())							// Inserts
		return(FALSE);
	if(! bindUpdateParams())							// Updates
		return(FALSE);
	if(! bindCols(c0))									// Selects
		return(FALSE);
	if(! bindCols(c1))
		return(FALSE);
	if(! bindCols(c2))
		return(FALSE);
//	if(! bindCols(c3))
//		return(FALSE);
//	if(! bindCols(c4))
//		return(FALSE);
//	if(! bindCols(c5))
//		return(FALSE);

	// Build an insert statement using parameter markers
	if (noCols > 0)
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
		GetSelectStmt(sqlStmt, queryType, distinct);	// so generate a select statement.

	// Make sure the cursor is closed first
	if (! CloseCursor(hstmt))
		return(FALSE);

	// Execute the SQL SELECT statement
	if (SQLExecDirect(hstmt, (UCHAR FAR *) (queryType == DB_SELECT_STATEMENT ? pSqlStmt : sqlStmt),
		               SQL_NTS) != SQL_SUCCESS)
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

	// Add the column list
	for (int i = 0; i < noCols; i++)
	{
		strcat(pSqlStmt, colDefs[i].ColName);
		if (i + 1 < noCols)
			strcat(pSqlStmt, ",");
	}

	// If the datasource supports ROWID, get this column as well.  Exception: Don't retrieve
	// the ROWID if querying distinct records.  The rowid will always be unique.
	if (!distinct && CanUpdByROWID())
		strcat(pSqlStmt, ",ROWID");

	// Append the FROM tablename portion
	strcat(pSqlStmt, " FROM ");
	strcat(pSqlStmt, queryTableName);

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

	// SELECT FOR UPDATE if told to do so and the datasource is capable
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
	if ((retcode = SQLFetch(hstmt)) != SQL_SUCCESS)
		if (retcode == SQL_NO_DATA_FOUND)
			return(FALSE);
		else
			return(pDb->DispAllErrors(henv, hdbc, hstmt));
#endif

	// Completed successfully
	return(TRUE);

}  // wxTable::getRec()

/********** wxTable::GetRowNum() **********/
UWORD wxTable::GetRowNum(void)
{
	UDWORD rowNum;

	if (SQLGetStmtOption(hstmt, SQL_ROW_NUMBER, &rowNum) != SQL_SUCCESS)
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
	SWORD   fSqlType;
	UDWORD  precision;
	SWORD   scale;

//glt CcolDef	*tColDef;

	// Bind each column (that can be inserted) of the table to a parameter marker
	for (int i = 0; i < noCols; i++)
	{
//glt tColDef = &colDefs[i];
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
		if (SQLBindParameter(hstmtInsert, i+1, SQL_PARAM_INPUT, colDefs[i].SqlCtype,
									fSqlType, precision, scale, colDefs[i].PtrDataObj, 
									precision+1,&colDefs[i].CbValue) != SQL_SUCCESS)
			return(pDb->DispAllErrors(henv, hdbc, hstmtInsert));
	}

	// Completed successfully
	return(TRUE);

}  // wxTable::bindInsertParams()

/********** wxTable::bindUpdateParams() **********/
bool wxTable::bindUpdateParams(void)
{
	SWORD   fSqlType;
	UDWORD  precision;
	SWORD   scale;
	
	// Bind each UPDATEABLE column of the table to a parameter marker
	for (int i = 0, colNo = 1; i < noCols; i++)
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
									fSqlType, precision, scale, colDefs[i].PtrDataObj, 
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
	for (int i = 0; i < noCols; i++)
	{
		if (SQLBindCol(cursor, i+1, colDefs[i].SqlCtype, colDefs[i].PtrDataObj,
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
bool wxTable::CreateTable(void)
{
	int i, j;
	char sqlStmt[DB_MAX_STATEMENT_LEN];

#ifdef _CONSOLE
	cout << "Creating Table " << tableName << "..." << endl;
#endif

	// Drop the table first
	sprintf(sqlStmt, "DROP TABLE %s", tableName);
	if (SQLExecDirect(hstmt, (UCHAR FAR *) sqlStmt, SQL_NTS) != SQL_SUCCESS)
	{
		// Check for sqlState = S0002, "Table or view not found".
		// Ignore this error, bomb out on any other error.
		// SQL Sybase Anwhere v5.5 returns an access violation error here
		// (sqlstate = 42000) rather than an S0002.
		pDb->GetNextError(henv, hdbc, hstmt);
		if (strcmp(pDb->sqlState, "S0002") && strcmp(pDb->sqlState, "42000"))
		{
			pDb->DispNextError();
			pDb->DispAllErrors(henv, hdbc, hstmt);
			pDb->RollbackTrans();
			CloseCursor(hstmt);
			return(FALSE);
		}
	}

	// Commit the transaction and close the cursor
	if (! pDb->CommitTrans())
		return(FALSE);
	if (! CloseCursor(hstmt))
		return(FALSE);

	// Create the table
#ifdef _CONSOLE
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
			// strcat(sqlStmt, "(");
			// strcat(sqlStmt, itoa(colDefs[i].SzDataObj, s, 10));
			// strcat(sqlStmt, ")");
            wxString s;
			s.Printf("(%d)", colDefs[i].SzDataObj);
			strcat(sqlStmt, s);
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
	if (j)	// Found a keyfield
	{
		strcat(sqlStmt, ",CONSTRAINT ");
		strcat(sqlStmt, tableName);
		strcat(sqlStmt, "_PIDX PRIMARY KEY (");
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

#ifdef _CONSOLE
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

/********** wxTable::CreateIndex() **********/
bool wxTable::CreateIndex(char * idxName, bool unique, int noIdxCols, CidxDef *pIdxDefs)
{
	char sqlStmt[DB_MAX_STATEMENT_LEN];

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
	for (int i = 0; i < noIdxCols; i++)
	{
		strcat(sqlStmt, pIdxDefs[i].ColName);
		if (pIdxDefs[i].Ascending)
			strcat(sqlStmt, " ASC");
		else
			strcat(sqlStmt, " DESC");
		if ((i + 1) < noIdxCols)
			strcat(sqlStmt, ",");
	}
	
	// Append closing parentheses
	strcat(sqlStmt, ")");

#ifdef _CONSOLE
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

/********** wxTable::Insert() **********/
int wxTable::Insert(void)
{
	// Insert the record by executing the already prepared insert statement
	if (SQLExecute(hstmtInsert) != SQL_SUCCESS)
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

	return(execUpdate(pSqlStmt));

}  // wxTable::Update(pSqlStmt)

/********** wxTable::Update() **********/
bool wxTable::Update(void)
{
	char sqlStmt[DB_MAX_STATEMENT_LEN];

	// Build the SQL UPDATE statement
	GetUpdateStmt(sqlStmt, DB_UPD_KEYFIELDS);

#ifdef _CONSOLE
	cout << endl << sqlStmt << endl << endl;
#endif

	// Execute the SQL UPDATE statement
	return(execUpdate(sqlStmt));

}  // wxTable::Update()

/********** wxTable::UpdateWhere() **********/
bool wxTable::UpdateWhere(char *pWhereClause)
{
	char sqlStmt[DB_MAX_STATEMENT_LEN];

	// Build the SQL UPDATE statement
	GetUpdateStmt(sqlStmt, DB_UPD_WHERE, pWhereClause);

#ifdef _CONSOLE
	cout << endl << sqlStmt << endl << endl;
#endif

	// Execute the SQL UPDATE statement
	return(execUpdate(sqlStmt));

}  // wxTable::UpdateWhere()

/********** wxTable::Delete() **********/
bool wxTable::Delete(void)
{
	char sqlStmt[DB_MAX_STATEMENT_LEN];

	// Build the SQL DELETE statement
	GetDeleteStmt(sqlStmt, DB_DEL_KEYFIELDS);

	// Execute the SQL DELETE statement
	return(execDelete(sqlStmt));

}  // wxTable::Delete()

/********** wxTable::DeleteWhere() **********/
bool wxTable::DeleteWhere(char *pWhereClause)
{
	char sqlStmt[DB_MAX_STATEMENT_LEN];

	// Build the SQL DELETE statement
	GetDeleteStmt(sqlStmt, DB_DEL_WHERE, pWhereClause);

	// Execute the SQL DELETE statement
	return(execDelete(sqlStmt));

}  // wxTable::DeleteWhere()

/********** wxTable::DeleteMatching() **********/
bool wxTable::DeleteMatching(void)
{
	char sqlStmt[DB_MAX_STATEMENT_LEN];

	// Build the SQL DELETE statement
	GetDeleteStmt(sqlStmt, DB_DEL_MATCHING);

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
	char whereClause[DB_MAX_WHERE_CLAUSE_LEN];
	bool firstColumn = TRUE;

	whereClause[0] = 0;
	sprintf(pSqlStmt, "UPDATE %s SET ", tableName);

	// Append a list of columns to be updated
	for (int i = 0; i < noCols; i++)
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
		// e.g. UPDATE PARTS SET C1 = ?, C2 = ? WHERE ROWID = '111.222.333'
		if (CanUpdByROWID())
		{
			SDWORD cb;
			char   rowid[ROWID_LEN];

			// Get the ROWID value.  If not successful retreiving the ROWID,
			// simply fall down through the code and build the WHERE clause
			// based on the key fields.
			if (SQLGetData(hstmt, noCols+1, SQL_C_CHAR, rowid, ROWID_LEN, &cb) == SQL_SUCCESS)
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
			if (SQLGetData(hstmt, noCols+1, SQL_C_CHAR, rowid, ROWID_LEN, &cb) == SQL_SUCCESS)
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

void wxTable::GetWhereClause(char *pWhereClause, int typeOfWhere)
{
	bool moreThanOneColumn = FALSE;
    wxString colValue;

	// Loop through the columns building a where clause as you go
	for (int i = 0; i < noCols; i++)
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
			strcat(pWhereClause, colDefs[i].ColName);
			strcat(pWhereClause, " = ");
			switch(colDefs[i].SqlCtype)
			{
			case SQL_C_CHAR:
				colValue.Printf("'%s'", (UCHAR FAR *) colDefs[i].PtrDataObj);
				break;
			case SQL_C_SSHORT:
				colValue.Printf("%hi", *((SWORD *) colDefs[i].PtrDataObj));
				break;
			case SQL_C_USHORT:
				colValue.Printf("%hu", *((UWORD *) colDefs[i].PtrDataObj));
				break;
			case SQL_C_SLONG:
				colValue.Printf("%li", *((SDWORD *) colDefs[i].PtrDataObj));
				break;
			case SQL_C_ULONG:
				colValue.Printf("%lu", *((UDWORD *) colDefs[i].PtrDataObj));
				break;
			case SQL_C_FLOAT:
				colValue.Printf("%.6f", *((SFLOAT *) colDefs[i].PtrDataObj));
				break;
			case SQL_C_DOUBLE:
				colValue.Printf("%.6f", *((SDOUBLE *) colDefs[i].PtrDataObj));
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
	if (pDb->dbInf.posStmts & SQL_PS_SELECT_FOR_UPDATE)
		return(TRUE);
	else
		return(FALSE);

}  // wxTable::CanSelectForUpdate()

/********** wxTable::CanUpdByROWID() **********/
bool wxTable::CanUpdByROWID(void)
{

//@@@@@@glt - returning FALSE for testing purposes, as the ROWID is not getting updated correctly
	return FALSE;

	if ((! strcmp(pDb->dbInf.dbmsName, "Oracle")) || (! strcmp(pDb->dbInf.dbmsName, "ORACLE")))
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
	for (int i = 0; i < noCols; i++)
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
	if (SQLSetStmtOption(c0, SQL_QUERY_TIMEOUT, nSeconds) != SQL_SUCCESS)
		return(pDb->DispAllErrors(henv, hdbc, c0));
	if (SQLSetStmtOption(c1, SQL_QUERY_TIMEOUT, nSeconds) != SQL_SUCCESS)
		return(pDb->DispAllErrors(henv, hdbc, c1));
	if (SQLSetStmtOption(c2, SQL_QUERY_TIMEOUT, nSeconds) != SQL_SUCCESS)
		return(pDb->DispAllErrors(henv, hdbc, c2));
//	if (SQLSetStmtOption(c3, SQL_QUERY_TIMEOUT, nSeconds) != SQL_SUCCESS)
//		return(pDb->DispAllErrors(henv, hdbc, c3));
//	if (SQLSetStmtOption(c4, SQL_QUERY_TIMEOUT, nSeconds) != SQL_SUCCESS)
//		return(pDb->DispAllErrors(henv, hdbc, c4));
//	if (SQLSetStmtOption(c5, SQL_QUERY_TIMEOUT, nSeconds) != SQL_SUCCESS)
//		return(pDb->DispAllErrors(henv, hdbc, c5));
	if (SQLSetStmtOption(hstmtInsert, SQL_QUERY_TIMEOUT, nSeconds) != SQL_SUCCESS)
		return(pDb->DispAllErrors(henv, hdbc, hstmtInsert));
	if (SQLSetStmtOption(hstmtUpdate, SQL_QUERY_TIMEOUT, nSeconds) != SQL_SUCCESS)
		return(pDb->DispAllErrors(henv, hdbc, hstmtUpdate));
	if (SQLSetStmtOption(hstmtDelete, SQL_QUERY_TIMEOUT, nSeconds) != SQL_SUCCESS)
		return(pDb->DispAllErrors(henv, hdbc, hstmtDelete));
	if (SQLSetStmtOption(hstmtCount, SQL_QUERY_TIMEOUT, nSeconds) != SQL_SUCCESS)
		return(pDb->DispAllErrors(henv, hdbc, hstmtCount));

	// Completed Successfully
	return(TRUE);

}  // wxTable::SetQueryTimeout()

/********** wxTable::SetColDefs() **********/
void wxTable::SetColDefs (int index, char *fieldName, int dataType, void *pData,
								 int cType, int size, bool keyField, bool upd,
								 bool insAllow, bool derivedCol)
{
	if (strlen(fieldName) > DB_MAX_COLUMN_NAME_LEN)  // glt 4/21/97
	{
		strncpy (colDefs[index].ColName, fieldName, DB_MAX_COLUMN_NAME_LEN);
		colDefs[index].ColName[DB_MAX_COLUMN_NAME_LEN] = 0;  // glt 10/23/97
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
	
}  // wxTable::SetColDefs()

/********** wxTable::SetCursor() **********/
bool wxTable::SetCursor(int cursorNo)
{
	switch(cursorNo)
	{
	case DB_CURSOR0:
		hstmt = c0;
		// currCursorNo doesn't change since Cursor0 is a temp cursor
		break;
	case DB_CURSOR1:
		hstmt = c1;
		currCursorNo = DB_CURSOR1;
		break;
	case DB_CURSOR2:
		hstmt = c2;
		currCursorNo = DB_CURSOR2;
		break;
//	case DB_CURSOR3:
//		hstmt = c3;
//		currCursorNo = DB_CURSOR3;
//		break;
//	case DB_CURSOR4:
//		hstmt = c4;
//		currCursorNo = DB_CURSOR4;
//		break;
//	case DB_CURSOR5:
//		hstmt = c5;
//		currCursorNo = DB_CURSOR5;
//		break;
	default:
		return(FALSE);
	}

	// Refresh the current record
#ifndef FWD_ONLY_CURSORS
	UDWORD  cRowsFetched;
	UWORD   rowStatus;
	SQLExtendedFetch(hstmt, SQL_FETCH_NEXT,  0, &cRowsFetched, &rowStatus);
	SQLExtendedFetch(hstmt, SQL_FETCH_PRIOR, 0, &cRowsFetched, &rowStatus);
#endif

	// Completed successfully
	return(TRUE);

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

	// Add the where clause if one is provided
	if (where && strlen(where))
	{
		strcat(sqlStmt, " WHERE ");
		strcat(sqlStmt, where);
	}

	// Execute the SQL statement
	if (SQLExecDirect(hstmtCount, (UCHAR FAR *) sqlStmt, SQL_NTS) != SQL_SUCCESS)
	{
		pDb->DispAllErrors(henv, hdbc, hstmtCount);
		return(0);
	}

	// Fetch the record
	if (SQLFetch(hstmtCount) != SQL_SUCCESS)
	{
		pDb->DispAllErrors(henv, hdbc, hstmtCount);
		return(0);
	}

	// Obtain the result
	if (SQLGetData(hstmtCount, 1, SQL_C_ULONG, &l, sizeof(l), &cb) != SQL_SUCCESS)
	{
		pDb->DispAllErrors(henv, hdbc, hstmtCount);
		return(0);
	}

	// Free the cursor
	if (SQLFreeStmt(hstmtCount, SQL_CLOSE) != SQL_SUCCESS)
		pDb->DispAllErrors(henv, hdbc, hstmtCount);

	// Return the record count
	return(l);

}  // wxTable::Count()

/********** wxTable::Refresh() **********/
bool wxTable::Refresh(void)
{
	bool result = TRUE;

	// Switch to cursor 0
	int cursorNo = GetCursor();
	if (!SetCursor())
		return(FALSE);

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
		if (SQLGetData(hstmt, noCols+1, SQL_C_CHAR, rowid, ROWID_LEN, &cb) == SQL_SUCCESS)
		{
			strcat(whereClause, "ROWID = '");
			strcat(whereClause, rowid);
			strcat(whereClause, "'");
		}
	}

	// If unable to use the ROWID, build a where clause from the keyfields
	if (strlen(whereClause) == 0)
		GetWhereClause(whereClause, DB_WHERE_KEYFIELDS);

	// Requery the record
	where = whereClause;
	orderBy = 0;
	if (!Query())
		result = FALSE;

	if (result && !GetNext())
		result = FALSE;

	// Switch back to original cursor
	if (!SetCursor(cursorNo))
		result = FALSE;

	// Restore the original where and order by clauses
	where	  = saveWhere;
	orderBy = saveOrderBy;

	return(result);

}  // wxTable::Refresh()

#endif
    // wxUSE_ODBC
