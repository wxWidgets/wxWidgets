///////////////////////////////////////////////////////////////////////////////
// Name:        dbtable.h
// Purpose:     Declaration of the wxDbTable class.
// Author:      Doug Card
// Modified by: George Tasker
//              Bart Jourquin
//              Mark Johnson
// Created:     9.96
// RCS-ID:      $Id$
// Copyright:   (c) 1996 Remstar International, Inc.
// Licence:     wxWindows licence, plus:
// Notice:        This class library and its intellectual design are free of charge for use,
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

const int   wxDB_ROWID_LEN       = 24;  // 18 is the max, 24 is in case it gets larger
const int   wxDB_DEFAULT_CURSOR  = 0;
const bool  wxDB_QUERY_ONLY      = TRUE;
const bool  wxDB_DISABLE_VIEW    = TRUE;

// The following class is used to define a column of a table.
// The wxDbTable constructor will dynamically allocate as many of
// these as there are columns in the table.  The class derived
// from wxDbTable must initialize these column definitions in it's
// constructor.  These column definitions provide inf. to the
// wxDbTable class which allows it to create a table in the data
// source, exchange data between the data source and the C++
// object, and so on.
class WXDLLEXPORT wxDbColDef
{
public:
    char    ColName[DB_MAX_COLUMN_NAME_LEN+1];  // Column Name
    int     DbDataType;                         // Logical Data Type; e.g. DB_DATA_TYPE_INTEGER
    int     SqlCtype;                           // C data type; e.g. SQL_C_LONG
    void   *PtrDataObj;                         // Address of the data object
    int     SzDataObj;                          // Size, in bytes, of the data object
    bool    KeyField;                           // TRUE if this column is part of the PRIMARY KEY to the table; Date fields should NOT be KeyFields.
    bool    Updateable;                         // Specifies whether this column is updateable
    bool    InsertAllowed;                      // Specifies whether this column should be included in an INSERT statement
    bool    DerivedCol;                         // Specifies whether this column is a derived value
    SDWORD  CbValue;                            // Internal use only!!!
    bool    Null;                               // NOT FULLY IMPLEMENTED - Allows NULL values in Inserts and Updates
};  // wxDbColDef


class WXDLLEXPORT wxDbColDataPtr
{
public:
    void    *PtrDataObj;
    int      SzDataObj;
    int      SqlCtype;
};  // wxDbColDataPtr


// This structure is used when creating secondary indexes.
class WXDLLEXPORT wxDbIdxDef
{
public:
    char ColName[DB_MAX_COLUMN_NAME_LEN+1];
    bool Ascending;
};  // wxDbIdxDef


class WXDLLEXPORT wxDbTable
{
private:
    ULONG    tableID;  // Used for debugging.  This can help to match up mismatched constructors/destructors

    // Private member variables
    UDWORD cursorType;
    bool   insertable;

    // Private member functions
    bool bindInsertParams(void);
    bool bindUpdateParams(void);
    bool bindCols(HSTMT cursor);
    bool getRec(UWORD fetchType);
    bool execDelete(const char *pSqlStmt);
    bool execUpdate(const char *pSqlStmt);
    bool query(int queryType, bool forUpdate, bool distinct, const char *pSqlStmt = 0);

#if !wxODBC_BACKWARD_COMPATABILITY
// these were public
    // Where, Order By and From clauses
    wxString    where;               // Standard SQL where clause, minus the word WHERE
    wxString    orderBy;             // Standard SQL order by clause, minus the ORDER BY
    wxString    from;                // Allows for joins in a wxDbTable::Query().  Format: ",tbl,tbl..."

    // ODBC Handles
    HENV        henv;           // ODBC Environment handle
    HDBC        hdbc;           // ODBC DB Connection handle
    HSTMT       hstmt;          // ODBC Statement handle
    HSTMT      *hstmtDefault;   // Default cursor
    HSTMT       hstmtInsert;    // ODBC Statement handle used specifically for inserts
    HSTMT       hstmtDelete;    // ODBC Statement handle used specifically for deletes
    HSTMT       hstmtUpdate;    // ODBC Statement handle used specifically for updates
    HSTMT       hstmtInternal;  // ODBC Statement handle used internally only
    HSTMT      *hstmtCount;     // ODBC Statement handle used by Count() function (No binding of columns)

    // Flags
    bool        selectForUpdate;

    // Pointer to the database object this table belongs to
    wxDb       *pDb;

    // Table Inf.
    char        tablePath[wxDB_PATH_MAX];                  // needed for dBase tables
    char        tableName[DB_MAX_TABLE_NAME_LEN+1];        // Table name
    char        queryTableName[DB_MAX_TABLE_NAME_LEN+1];   // Query Table Name
    int         noCols;                                    // # of columns in the table
    bool        queryOnly;                                 // Query Only, no inserts, updates or deletes

    // Column Definitions
    wxDbColDef *colDefs;         // Array of wxDbColDef structures
#endif
public:
#if wxODBC_BACKWARD_COMPATABILITY
    // Where, Order By and From clauses
    char       *where;          // Standard SQL where clause, minus the word WHERE
    char       *orderBy;        // Standard SQL order by clause, minus the ORDER BY
    char       *from;           // Allows for joins in a wxDbTable::Query().  Format: ",tbl,tbl..."

    // ODBC Handles
    HENV        henv;           // ODBC Environment handle
    HDBC        hdbc;           // ODBC DB Connection handle
    HSTMT       hstmt;          // ODBC Statement handle
    HSTMT      *hstmtDefault;   // Default cursor
    HSTMT       hstmtInsert;    // ODBC Statement handle used specifically for inserts
    HSTMT       hstmtDelete;    // ODBC Statement handle used specifically for deletes
    HSTMT       hstmtUpdate;    // ODBC Statement handle used specifically for updates
    HSTMT       hstmtInternal;  // ODBC Statement handle used internally only
    HSTMT      *hstmtCount;     // ODBC Statement handle used by Count() function (No binding of columns)

    // Flags
    bool        selectForUpdate;

    // Pointer to the database object this table belongs to
    wxDb       *pDb;

    // Table Inf.
    char        tablePath[wxDB_PATH_MAX];                  // needed for dBase tables
    char        tableName[DB_MAX_TABLE_NAME_LEN+1];        // Table name
    char        queryTableName[DB_MAX_TABLE_NAME_LEN+1];   // Query Table Name
    int         noCols;                                    // # of columns in the table
    bool        queryOnly;                                 // Query Only, no inserts, updates or deletes

    // Column Definitions
    wxDbColDef *colDefs;         // Array of wxDbColDef structures
#endif
    // Public member functions
    wxDbTable(wxDb *pwxDb, const char *tblName, const int nCols,
              const char *qryTblName = 0, bool qryOnly = !wxDB_QUERY_ONLY, const char *tblPath=NULL);
    virtual ~wxDbTable();

    bool            Open(void);
    bool            CreateTable(bool attemptDrop=TRUE);
    bool            DropTable(void);
    bool            CreateIndex(const char * idxName, bool unique, int noIdxCols, wxDbIdxDef *pIdxDefs, bool attemptDrop=TRUE);
    bool            DropIndex(const char * idxName);

// Accessors

    // The member variables returned by these accessors are all
    // set when the wxDbTable instance is createand cannot be 
    // changed, hence there is no corresponding SetXxxx function
	 wxDb           *GetDb()              { return pDb; }
	 const char     *GetTableName()       { return tableName; }
	 const char     *GetQueryTableName()  { return queryTableName; }
	 const char     *GetTablePath()       { return tablePath; }

         int GetNumberOfColumns() { return noCols; }  // number of "defined" columns for this wxDbTable instance
       

	 const char     *GetFromClause()      { return from; }
	 const char     *GetOrderByClause()   { return orderBy; }
	 const char     *GetWhereClause()     { return where; }

	 bool            IsQueryOnly()        { return queryOnly; }
#if wxODBC_BACKWARD_COMPATABILITY
    void            SetFromClause(const char *From) { from = (char *)From; }
	 void            SetOrderByClause(const char *OrderBy) { orderBy = (char *)OrderBy; }
	 void            SetWhereClause(const char *Where) { where = (char *)Where; }
#else
	 void            SetFromClause(const wxString& From) { from = From; }
	 void            SetOrderByClause(const wxString& OrderBy) { orderBy = OrderBy; }
	 void            SetWhereClause(const wxString& Where) { where = Where; }
#endif
    int             Insert(void);
    bool            Update(void);
    bool            Update(const char *pSqlStmt);
    bool            UpdateWhere(const char *pWhereClause);
    bool            Delete(void);
    bool            DeleteWhere(const char *pWhereClause);
    bool            DeleteMatching(void);
    virtual bool    Query(bool forUpdate = FALSE, bool distinct = FALSE);
    bool            QueryBySqlStmt(const char *pSqlStmt);
    bool            QueryMatching(bool forUpdate = FALSE, bool distinct = FALSE);
    bool            QueryOnKeyFields(bool forUpdate = FALSE, bool distinct = FALSE);
    bool            Refresh(void);
    bool            GetNext(void)   { return(getRec(SQL_FETCH_NEXT));  }
    bool            operator++(int) { return(getRec(SQL_FETCH_NEXT));  }

    /***** These four functions only work with wxDb instances that are defined  *****
     ***** as not being FwdOnlyCursors                                          *****/
    bool            GetPrev(void);
    bool            operator--(int);
    bool            GetFirst(void);
    bool            GetLast(void);

    bool            IsCursorClosedOnCommit(void);
    UWORD           GetRowNum(void);

    void            BuildSelectStmt(char *pSqlStmt, int typeOfSelect, bool distinct);
    void            BuildDeleteStmt(char *pSqlStmt, int typeOfDel, const char *pWhereClause = 0);
    void            BuildUpdateStmt(char *pSqlStmt, int typeOfUpd, const char *pWhereClause = 0);
    void            BuildWhereClause(char *pWhereClause, int typeOfWhere, const char *qualTableName = 0, bool useLikeComparison=FALSE);
#if wxODBC_BACKWARD_COMPATABILITY
// The following member functions are deprecated.  You should use the BuildXxxxxStmt functions (above)
    void            GetSelectStmt(char *pSqlStmt, int typeOfSelect, bool distinct)
                           { BuildSelectStmt(pSqlStmt,typeOfSelect,distinct); }
    void            GetDeleteStmt(char *pSqlStmt, int typeOfDel, const char *pWhereClause = 0)
                           { BuildDeleteStmt(pSqlStmt,typeOfDel,pWhereClause); }
    void            GetUpdateStmt(char *pSqlStmt, int typeOfUpd, const char *pWhereClause = 0)
                           { BuildUpdateStmt(pSqlStmt,typeOfUpd,pWhereClause); }
    void            GetWhereClause(char *pWhereClause, int typeOfWhere, 
                                   const char *qualTableName = 0, bool useLikeComparison=FALSE)
                           { BuildWhereClause(pWhereClause,typeOfWhere,qualTableName,useLikeComparison); }
#endif
    bool            CanSelectForUpdate(void);
    bool            CanUpdByROWID(void);
    void            ClearMemberVars(void);
    bool            SetQueryTimeout(UDWORD nSeconds);

    wxDbColDef     *GetColDefs() { return colDefs; }
    void            SetColDefs(int index, const char *fieldName, int dataType, void *pData, int cType,
                               int size, bool keyField = FALSE, bool upd = TRUE,
                               bool insAllow = TRUE, bool derivedCol = FALSE);
    wxDbColDataPtr *SetColDefs(wxDbColInf *colInfs, ULONG numCols);

    bool            CloseCursor(HSTMT cursor);
    bool            DeleteCursor(HSTMT *hstmtDel);
    void            SetCursor(HSTMT *hstmtActivate = (void **) wxDB_DEFAULT_CURSOR);
    HSTMT           GetCursor(void) { return(hstmt); }
    HSTMT          *GetNewCursor(bool setCursor = FALSE, bool bindColumns = TRUE);
#if wxODBC_BACKWARD_COMPATABILITY
// The following member function is deprecated.  You should use the GetNewCursor
    HSTMT          *NewCursor(bool setCursor = FALSE, bool bindColumns = TRUE) {  return GetNewCursor(setCursor,bindColumns); }
#endif

    ULONG           Count(const char *args="*");
    int             DB_STATUS(void) { return(pDb->DB_STATUS); }

    bool            IsColNull(int colNo);
    bool            SetNull(int colNo);
    bool            SetNull(const char *colName);

#ifdef __WXDEBUG__
    ULONG           GetTableID() { return tableID; }
#endif

};  // wxDbTable


// Change this to 0 to remove use of all deprecated functions
#if wxODBC_BACKWARD_COMPATABILITY
//#################################################################################
//############### DEPRECATED functions for backward compatability #################
//#################################################################################

// Backward compability.  These will eventually go away
typedef wxDbTable       wxTable;
typedef wxDbIdxDef      wxIdxDef;
typedef wxDbIdxDef      CidxDef;
typedef wxDbColDef      wxColDef;
typedef wxDbColDef      CcolDef;
typedef wxDbColDataPtr  wxColDataPtr;
typedef wxDbColDataPtr  CcolDataPtr;

const int   ROWID           = wxDB_ROWID_LEN;
const int   DEFAULT_CURSOR  = wxDB_DEFAULT_CURSOR;
const bool  QUERY_ONLY      = wxDB_QUERY_ONLY;
const bool  DISABLE_VIEW    = wxDB_DISABLE_VIEW;
#endif

#endif
