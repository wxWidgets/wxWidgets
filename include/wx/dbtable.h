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
// Notice:      This class library and its intellectual design are free of charge for use,
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

// Used to indicate end of a variable length list of
// column numbers passed to member functions
const int   wxDB_NO_MORE_COLUMN_NUMBERS = -1;

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
    wxChar  ColName[DB_MAX_COLUMN_NAME_LEN+1];  // Column Name
    int     DbDataType;                         // Logical Data Type; e.g. DB_DATA_TYPE_INTEGER
    SWORD   SqlCtype;                           // C data type; e.g. SQL_C_LONG
    void   *PtrDataObj;                         // Address of the data object
    int     SzDataObj;                          // Size, in bytes, of the data object
    bool    KeyField;                           // TRUE if this column is part of the PRIMARY KEY to the table; Date fields should NOT be KeyFields.
    bool    Updateable;                         // Specifies whether this column is updateable
    bool    InsertAllowed;                      // Specifies whether this column should be included in an INSERT statement
    bool    DerivedCol;                         // Specifies whether this column is a derived value
    SDWORD  CbValue;                            // Internal use only!!!
    bool    Null;                               // NOT FULLY IMPLEMENTED - Allows NULL values in Inserts and Updates

    wxDbColDef();

    bool    Initialize();
};  // wxDbColDef


class WXDLLEXPORT wxDbColDataPtr
{
public:
    void    *PtrDataObj;
    int      SzDataObj;
    SWORD    SqlCtype;
};  // wxDbColDataPtr


// This structure is used when creating secondary indexes.
class WXDLLEXPORT wxDbIdxDef
{
public:
    wxChar  ColName[DB_MAX_COLUMN_NAME_LEN+1];
    bool    Ascending;
};  // wxDbIdxDef


class WXDLLEXPORT wxDbTable
{
private:
    ULONG       tableID;  // Used for debugging.  This can help to match up mismatched constructors/destructors

    // Private member variables
    UDWORD      cursorType;
    bool        insertable;

    // Private member functions
    bool        initialize(wxDb *pwxDb, const wxString &tblName, const UWORD numColumns,
                       const wxString &qryTblName, bool qryOnly, const wxString &tblPath);
    void        cleanup();

    bool        bindParams(bool forUpdate);  // called by the other 'bind' functions
    bool        bindInsertParams(void);
    bool        bindUpdateParams(void);

    bool        bindCols(HSTMT cursor);
    bool        getRec(UWORD fetchType);
    bool        execDelete(const wxString &pSqlStmt);
    bool        execUpdate(const wxString &pSqlStmt);
    bool        query(int queryType, bool forUpdate, bool distinct, const wxString &pSqlStmt=wxEmptyString);

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
    wxString    tablePath;                                 // needed for dBase tables
    wxString    tableName;                                 // Table name
    wxString    queryTableName;                            // Query Table Name
    UWORD       noCols;                                    // # of columns in the table
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
    UWORD       noCols;                                    // # of columns in the table
    bool        queryOnly;                                 // Query Only, no inserts, updates or deletes

    // Column Definitions
    wxDbColDef *colDefs;         // Array of wxDbColDef structures
#endif
    // Public member functions
    wxDbTable(wxDb *pwxDb, const wxString &tblName, const UWORD numColumns,
              const wxString &qryTblName="", bool qryOnly = !wxDB_QUERY_ONLY, 
              const wxString &tblPath="");

    // DEPRECATED
    wxDbTable(wxDb *pwxDb, const wxString &tblName, const UWORD numColumns,
              const wxChar *qryTblName="", bool qryOnly = !wxDB_QUERY_ONLY, 
              const wxString &tblPath="");

    virtual ~wxDbTable();

    bool            Open(bool checkPrivileges=FALSE, bool checkTableExists=TRUE);
    bool            CreateTable(bool attemptDrop=TRUE);
    bool            DropTable(void);
    bool            CreateIndex(const wxString &idxName, bool unique, int noIdxCols, 
                                wxDbIdxDef *pIdxDefs, bool attemptDrop=TRUE);
    bool            DropIndex(const wxString &idxName);

    // Accessors

    // The member variables returned by these accessors are all
    // set when the wxDbTable instance is created and cannot be 
    // changed, hence there is no corresponding SetXxxx function
    wxDb           *GetDb()              { return pDb; }
    const wxString &GetTableName()       { return tableName; }
    const wxString &GetQueryTableName()  { return queryTableName; }
    const wxString &GetTablePath()       { return tablePath; }

    UWORD           GetNumberOfColumns() { return noCols; }  // number of "defined" columns for this wxDbTable instance

    const wxString &GetFromClause()      { return from; }
    const wxString &GetOrderByClause()   { return orderBy; }
    const wxString &GetWhereClause()     { return where; }

    bool            IsQueryOnly()        { return queryOnly; }
#if wxODBC_BACKWARD_COMPATABILITY
    void            SetFromClause(const char *From) { from = (char *)From; }
    void            SetOrderByClause(const char *OrderBy) { orderBy = (char *)OrderBy; }
    void            SetWhereClause(const char *Where) { where = (char *)Where; }
#else
    void            SetFromClause(const wxString &From) { from = From; }
    void            SetOrderByClause(const wxString &OrderBy) { orderBy = OrderBy; }
    bool            SetOrderByColNums(UWORD first, ...);
    void            SetWhereClause(const wxString &Where) { where = Where; }
    void            From(const wxString &From) { from = From; }
    void            OrderBy(const wxString &OrderBy) { orderBy = OrderBy; }
    void            Where(const wxString &Where) { where = Where; }
    const wxString &Where()   { return where; }
    const wxString &OrderBy() { return orderBy; }
    const wxString &From()    { return from; }
#endif
    int             Insert(void);
    bool            Update(void);
    bool            Update(const wxString &pSqlStmt);
    bool            UpdateWhere(const wxString &pWhereClause);
    bool            Delete(void);
    bool            DeleteWhere(const wxString &pWhereClause);
    bool            DeleteMatching(void);
    virtual bool    Query(bool forUpdate = FALSE, bool distinct = FALSE);
    bool            QueryBySqlStmt(const wxString &pSqlStmt);
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

    void            BuildSelectStmt(wxString &pSqlStmt, int typeOfSelect, bool distinct);
    void            BuildSelectStmt(wxChar *pSqlStmt, int typeOfSelect, bool distinct);

    void            BuildDeleteStmt(wxString &pSqlStmt, int typeOfDel, const wxString &pWhereClause="");
    void            BuildDeleteStmt(wxChar *pSqlStmt, int typeOfDel, const wxString &pWhereClause="");

    void            BuildUpdateStmt(wxString &pSqlStmt, int typeOfUpd, const wxString &pWhereClause="");
    void            BuildUpdateStmt(wxChar *pSqlStmt, int typeOfUpd, const wxString &pWhereClause="");

    void            BuildWhereClause(wxString &pWhereClause, int typeOfWhere, const wxString &qualTableName="", bool useLikeComparison=FALSE);
    void            BuildWhereClause(wxChar *pWhereClause, int typeOfWhere, const wxString &qualTableName="", bool useLikeComparison=FALSE);

#if wxODBC_BACKWARD_COMPATABILITY
// The following member functions are deprecated.  You should use the BuildXxxxxStmt functions (above)
    void            GetSelectStmt(char *pSqlStmt, int typeOfSelect, bool distinct)
                           { BuildSelectStmt(pSqlStmt,typeOfSelect,distinct); }
    void            GetDeleteStmt(char *pSqlStmt, int typeOfDel, const char *pWhereClause = NULL)
                           { BuildDeleteStmt(pSqlStmt,typeOfDel,pWhereClause); }
    void            GetUpdateStmt(char *pSqlStmt, int typeOfUpd, const char *pWhereClause = NULL)
                           { BuildUpdateStmt(pSqlStmt,typeOfUpd,pWhereClause); }
    void            GetWhereClause(char *pWhereClause, int typeOfWhere, 
                                   const char *qualTableName = NULL, bool useLikeComparison=FALSE)
                           { BuildWhereClause(pWhereClause,typeOfWhere,qualTableName,useLikeComparison); }
#endif
    bool            CanSelectForUpdate(void);
    bool            CanUpdByROWID(void);
    void            ClearMemberVar(UWORD colNo, bool setToNull=FALSE);
    void            ClearMemberVars(bool setToNull=FALSE);
    bool            SetQueryTimeout(UDWORD nSeconds);

    wxDbColDef     *GetColDefs() { return colDefs; }
    void            SetColDefs(int index, const wxString &fieldName, int dataType, 
                               void *pData, SWORD cType,
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

    ULONG           Count(const wxString &args="*");
    int             DB_STATUS(void) { return(pDb->DB_STATUS); }

    bool            IsColNull(UWORD colNo);
    bool            SetColNull(UWORD colNo, bool set=TRUE);
    bool            SetColNull(const wxString &colName, bool set=TRUE);
#if wxODBC_BACKWARD_COMPATABILITY
// The following member functions are deprecated.  You should use the SetColNull()
    bool            SetNull(int colNo, bool set=TRUE) { return (SetNull(colNo,set)); }
    bool            SetNull(const char *colName, bool set=TRUE) { return (SetNull(colName,set)); }
#endif
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
