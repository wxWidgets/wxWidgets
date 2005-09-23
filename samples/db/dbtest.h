///////////////////////////////////////////////////////////////////////////////
// Name:        dbtest.h
// Purpose:     wxWidgets database demo app
// Author:      George Tasker
// Modified by:
// Created:     1998
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Remstar International, Inc.
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/string.h"
#include "wx/dbtable.h"

enum    DialogModes {mView,mCreate,mEdit,mSearch};

// ID for the menu quit command
#define FILE_CREATE_ID        100
#define FILE_RECREATE_TABLE   110
#define FILE_RECREATE_INDEXES 120

#if wxUSE_GRID
#define FILE_DBGRID_TABLE     130
#endif
#define FILE_EXIT             wxID_EXIT
#define EDIT_PARAMETERS       200
#define HELP_ABOUT            wxID_ABOUT

// this seems to be missing, Robert Roebling (?)
#ifndef MAX_PATH
    #if defined(__WXMAC__)
        #define MAX_PATH   260 /* max. length of full pathname */
    #else  /* _MAC */
        #define MAX_PATH   256 /* max. length of full pathname */
    #endif
#endif

// Name of the table to be created/opened
const wxString     CONTACT_TABLE_NAME       = wxT("CONTACTS");

#define wxODBC_BLOB_SUPPORT


// Number of columns in the CONTACT table
#ifdef wxODBC_BLOB_SUPPORT
    const int        CONTACT_NO_COLS        = 14;        // 0-13
    const int        MAX_PICTURE_SIZE       = 128000;    // in bytes
#else
    const int        CONTACT_NO_COLS        = 12;        // 0-11
#endif

const wxString       PARAM_FILENAME         = wxT("dbtest.cfg");

enum Language {langENGLISH, langFRENCH, langGERMAN, langSPANISH, langOTHER};

// Forward class declarations
class CeditorDlg;
class CparameterDlg;


// Used for displaying many of the database capabilites
// and usage statistics on a database connection
void DisplayDbDiagnostics(wxDb *pDb);


//
// This class contains the actual data members that are used for transferring
// data back and forth from the database to the program.
//
// NOTE: The object described in this class is just for example purposes, and has no
// real meaning other than to show each type of field being used by the database
//
class CstructContact : public wxObject
{
    public:
        wxChar             Name[50+1];          //    Contact's name
        wxChar             Addr1[50+1];
        wxChar             Addr2[50+1];
        wxChar             City[25+1];
        wxChar             State[25+1];
        wxChar             PostalCode[15+1];
        wxChar             Country[20+1];
        TIMESTAMP_STRUCT   JoinDate;            // Date on which this person joined the wxWidgets project
        Language           NativeLanguage;      // Enumerated type indicating person's native language
        ULONG              BlobSize;
        wxChar             Picture[MAX_PICTURE_SIZE];
        bool               IsDeveloper;         // Is this person a developer for wxWidgets, or just a subscriber
        UCHAR              Contributions;       // Something to show off an integer field
        ULONG              LinesOfCode;         // Something to show off a 'long' field
};  // CstructContact


//
// The Ccontact class derives from wxDbTable, so we have access to all
// of the database table functions and the local memory variables that
// the database classes will store the data into (and read the data from)
// all combined in this one class.
//
class Ccontact : public wxDbTable, public CstructContact
{
    private:
        // Used to keep track of whether this class had a wxDb instance
        // passed in to it or not.  If an existing wxDb instance was not
        // passed in at Ccontact creation time, then when the Ccontact
        // instance is deleted, the connection will be freed as Ccontact
        // created its own connection when it was created.
        bool                 freeDbConn;

        // Calls wxDbTable::SetColDefs() once for each column that is
        // to be associated with some member variable for use with
        // this database object.
        void                 SetupColumns();

    public:
        // Used in places where we need to construct a WHERE clause to
        // be passed to the SetWhereClause() function.  From example,
        // where building the WHERE clause requires using ::Printf()
        // to build the string.
        wxString             whereStr;

        // WHERE string returned from the query dialog
        wxString             qryWhereStr;

        Ccontact(wxDb *pwxDb=NULL);
        ~Ccontact();

        void                 Initialize();

        // Contains all the index definitions and calls to wxDbTable::CreateIndex()
        // required to create all the indexes we wish to define for this table.
        bool                 CreateIndexes(bool recreate);

        // Since we do not wish to have duplicate code blocks all over our program
        // for a common query/fetch that we will need to do in many places, we
        // include this member function that does it all for us in one place.
        bool                 FetchByName(const wxString &name);

};  // Ccontact class definition


typedef struct Cparameters
{
    wxChar    ODBCSource[SQL_MAX_DSN_LENGTH+1];
    wxChar    UserName[SQL_MAX_USER_NAME_LEN+1];
    wxChar    Password[SQL_MAX_AUTHSTR_LEN+1];
    wxChar    DirPath[MAX_PATH+1];
} Cparameters;


// Define a new frame type
class DatabaseDemoFrame: public wxFrame
{
    private:
        CeditorDlg      *pEditorDlg;
        CparameterDlg   *pParamDlg;

    public:
        DatabaseDemoFrame(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& sz);
        ~DatabaseDemoFrame();

        void    OnCloseWindow(wxCloseEvent& event);
        void    OnCreate(wxCommandEvent& event);
        void    OnRecreateTable(wxCommandEvent& event);
        void    OnRecreateIndexes(wxCommandEvent& event);
        void    OnExit(wxCommandEvent& event);
        void    OnEditParameters(wxCommandEvent& event);
        void    OnAbout(wxCommandEvent& event);
#if wxUSE_GRID
        void    OnDbGridTable( wxCommandEvent& );
#endif
        void    CreateDataTable(bool recreate);
        void    BuildEditorDialog();
        void    BuildParameterDialog(wxWindow *parent);

DECLARE_EVENT_TABLE()
};  // DatabaseDemoFrame


#if wxUSE_GRID

// *************************** DBGridFrame ***************************

class DbGridFrame : public wxFrame
{
public:
    bool     initialized;

    DbGridFrame(wxWindow *parent);

    void     OnCloseWindow(wxCloseEvent& event);
    bool     Initialize();

    DECLARE_EVENT_TABLE()
};

#endif

// Define a new application type
class DatabaseDemoApp: public wxApp
{
    public:
        // These are the parameters that are stored in the "PARAM_FILENAME" file
        // that are read in at startup of the program that indicate the connection
        // parameters to be used for connecting to the ODBC data source.
        Cparameters      params;

        // Pointer to the main frame used by the App
        DatabaseDemoFrame *DemoFrame;

        // Pointer to the main database connection used in the program.  This
        // pointer would normally be used for doing things as database lookups
        // for user login names and passwords, getting workstation settings, etc.
        //
        // ---> IMPORTANT <---
        //
        // For each database object created which uses this wxDb pointer
        // connection to the database, when a CommitTrans() or RollBackTrans()
        // will commit or rollback EVERY object which uses this wxDb pointer.
        //
        // To allow each table object (those derived from wxDbTable) to be
        // individually committed or rolled back, you MUST use a different
        // instance of wxDb in the constructor of the table.  Doing so creates
        // more overhead, and will use more database connections (some DBs have
        // connection limits...), so use connections sparringly.
        //
        // It is recommended that one "main" database connection be created for
        // the entire program to use for READ-ONLY database accesses, but for each
        // table object which will do a CommitTrans() or RollbackTrans() that a
        // new wxDb object be created and used for it.
        wxDb            *READONLY_DB;

        // Contains the ODBC connection information used by
        // all database connections
        wxDbConnectInf  *DbConnectInf;

        bool             OnInit();

        // Read/Write ODBC connection parameters to the "PARAM_FILENAME" file
        bool             ReadParamFile(Cparameters &params);
        bool             WriteParamFile(Cparameters &params);

        void             CreateDataTable(bool recreate);

        // Pointer to the wxDbTable instance that is used to manipulate
        // the data in memory and in the database
        Ccontact        *Contact;

};  // DatabaseDemoApp


DECLARE_APP(DatabaseDemoApp)


// *************************** CeditorDlg ***************************

class CeditorDlg : public wxPanel
{
    private:
        // Used to indicate whether all of the widget pointers (defined
        // below) have been initialized to point to the memory for
        // the named widget.  Used as a safeguard from using the widget
        // before it has been initialized.
        bool             widgetPtrsSet;

        // Used when the EDIT button has been pressed to maintain the
        // original name that was displayed in the editor before the
        // EDIT button was pressed, so that if CANCEL is pressed, a
        // FetchByName() can be done to retrieve the original data
        // to repopulate the dialog.
        wxString         saveName;

        // Pointers to all widgets on the dialog
        wxButton        *pCreateBtn,  *pEditBtn,      *pDeleteBtn,  *pCopyBtn,  *pSaveBtn,  *pCancelBtn;
        wxButton        *pPrevBtn,    *pNextBtn,      *pQueryBtn,   *pResetBtn, *pDoneBtn,  *pHelpBtn;
        wxButton        *pNameListBtn;
        wxButton        *pCatalogBtn, *pDataTypesBtn, *pDbDiagsBtn;
        wxTextCtrl      *pNameTxt,    *pAddress1Txt,  *pAddress2Txt,*pCityTxt,  *pStateTxt, *pCountryTxt,*pPostalCodeTxt;
        wxStaticText    *pNameMsg,    *pAddress1Msg,  *pAddress2Msg,*pCityMsg,  *pStateMsg, *pCountryMsg,*pPostalCodeMsg;
        wxTextCtrl      *pJoinDateTxt,*pContribTxt,   *pLinesTxt;
        wxStaticText    *pJoinDateMsg,*pContribMsg,   *pLinesMsg;
        wxRadioBox      *pDeveloperRadio;
        wxChoice        *pNativeLangChoice;
        wxStaticText    *pNativeLangMsg;
#ifdef wxODBC_BLOB_SUPPORT
        wxStaticText    *pPictureMsg, *pPictSizeMsg;
        wxButton        *pChooseImageBtn, *pShowImageBtn;
        wxTextCtrl      *pPictSizeTxt;
#endif

    public:
        // Indicates if the editor dialog has been initialized yet (used to
        // help trap if the Initialize() function failed to load all required
        // resources or not.
        bool             initialized;

        enum DialogModes mode;

        CeditorDlg(wxWindow *parent);

        void    OnCloseWindow(wxCloseEvent& event);
        void    OnButton( wxCommandEvent &event );
        void    OnCommand(wxWindow& win, wxCommandEvent& event);
        void    OnActivate(bool) {};  // necessary for hot keys

        bool    Initialize();

#ifdef wxODBC_BLOB_SUPPORT
        // Methods for reading image file into current table, and
        // also displaying the image.
        void    OnSelectPict();
        void    OnShowImage();
#endif

        // Sets wxStaticText fields to be editable or not depending
        // on the current value of 'mode'
        void    FieldsEditable();

        // Sets the editor mode, determining what state widgets
        // on the dialog are to be in based on the operation
        // being performed.
        void    SetMode(enum DialogModes m);

        // Update/Retrieve data from the widgets on the dialog
        bool    PutData();
        bool    GetData();

        // Inserts/updates the database with the current data
        // retrieved from the editor dialog
        bool    Save();

        // Database functions for changing the data that is to
        // be displayed on the dialog.  GetNextRec()/GetPrevRec()
        // provide database independent methods that do not require
        // backward scrolling cursors to obtain the record that
        // is prior to the current record in the search sequence.
        bool    GetNextRec();
        bool    GetPrevRec();
        bool    GetRec(const wxString &whereStr);

DECLARE_EVENT_TABLE()
};  // CeditorDlg

#define EDITOR_DIALOG                   199

// Editor dialog control ids
#define EDITOR_DIALOG_FN_GROUP          200
#define EDITOR_DIALOG_SEARCH_GROUP      201
#define EDITOR_DIALOG_CREATE            202
#define EDITOR_DIALOG_EDIT              203
#define EDITOR_DIALOG_DELETE            204
#define EDITOR_DIALOG_COPY              205
#define EDITOR_DIALOG_SAVE              206
#define EDITOR_DIALOG_CANCEL            207
#define EDITOR_DIALOG_PREV              208
#define EDITOR_DIALOG_NEXT              209
#define EDITOR_DIALOG_QUERY             211
#define EDITOR_DIALOG_RESET             212
#define EDITOR_DIALOG_NAME_MSG          213
#define EDITOR_DIALOG_NAME_TEXT         214
#define EDITOR_DIALOG_LOOKUP            215
#define EDITOR_DIALOG_ADDRESS1_MSG      216
#define EDITOR_DIALOG_ADDRESS1_TEXT     217
#define EDITOR_DIALOG_ADDRESS2_MSG      218
#define EDITOR_DIALOG_ADDRESS2_TEXT     219
#define EDITOR_DIALOG_CITY_MSG          220
#define EDITOR_DIALOG_CITY_TEXT         221
#define EDITOR_DIALOG_COUNTRY_MSG       222
#define EDITOR_DIALOG_COUNTRY_TEXT      223
#define EDITOR_DIALOG_POSTAL_MSG        224
#define EDITOR_DIALOG_POSTAL_TEXT       225
#define EDITOR_DIALOG_LANG_MSG          226
#define EDITOR_DIALOG_LANG_CHOICE       227
#define EDITOR_DIALOG_DATE_MSG          228
#define EDITOR_DIALOG_DATE_TEXT         229
#define EDITOR_DIALOG_CONTRIB_MSG       230
#define EDITOR_DIALOG_CONTRIB_TEXT      231
#define EDITOR_DIALOG_LINES_MSG         232
#define EDITOR_DIALOG_LINES_TEXT        233
#define EDITOR_DIALOG_STATE_MSG         234
#define EDITOR_DIALOG_STATE_TEXT        235
#define EDITOR_DIALOG_DEVELOPER         236
#define EDITOR_DIALOG_JOIN_MSG          237
#define EDITOR_DIALOG_JOIN_TEXT         238
#define EDITOR_DIALOG_CATALOG           240
#define EDITOR_DIALOG_DATATYPES         250
#define EDITOR_DIALOG_DB_DIAGS          260
#ifdef wxODBC_BLOB_SUPPORT
	#define EDITOR_DIALOG_PIC_MSG           270
	#define EDITOR_DIALOG_PICSIZE_MSG       271
	#define EDITOR_DIALOG_PIC_BROWSE        272
	#define EDITOR_DIALOG_PIC_SHOW          273
	#define EDITOR_DIALOG_PIC_SIZE_TEXT     274
#endif

// *************************** CparameterDlg ***************************

class CparameterDlg : public wxDialog
{
    private:
        // Used to indicate whether all of the widget pointers (defined
        // below) have been initialized to point to the memory for
        // the named widget.  Used as a safeguard from using the widget
        // before it has been initialized.
        bool                 widgetPtrsSet;

        enum DialogModes     mode;

        // Have the parameters been saved yet, or do they
        // need to be saved to update the params on disk
        bool                 saved;

        // Original params
        Cparameters          savedParamSettings;

        // Pointers to all widgets on the dialog
        wxStaticText        *pParamODBCSourceMsg;
        wxListBox           *pParamODBCSourceList;
        wxStaticText        *pParamUserNameMsg,        *pParamPasswordMsg,    *pParamDirPathMsg;
        wxTextCtrl          *pParamUserNameTxt,        *pParamPasswordTxt,    *pParamDirPathTxt;
        wxButton            *pParamSaveBtn,            *pParamCancelBtn;

    public:
        CparameterDlg(wxWindow *parent);

        void    OnCloseWindow(wxCloseEvent& event);
        void    OnButton( wxCommandEvent &event );
        void    OnCommand(wxWindow& win, wxCommandEvent& event);
        void    OnActivate(bool) {};  // necessary for hot keys

        // Update/Retrieve data from the widgets on the dialog
        bool    PutData();
        bool    GetData();

        // Stores the defined parameter for connecting to the selected ODBC
        // data source to the config file name in "PARAM_FILENAME"
        bool    Save();

        // Populates the 'pParamODBCSourceList' listbox with the names of all
        // ODBC datasource defined for use at the current workstation
        void    FillDataSourceList();

DECLARE_EVENT_TABLE()
};  // CparameterDlg

#define PARAMETER_DIALOG                    400

// Parameter dialog control ids
#define PARAMETER_DIALOG_SOURCE_MSG         401
#define PARAMETER_DIALOG_SOURCE_LISTBOX     402
#define PARAMETER_DIALOG_NAME_MSG           403
#define PARAMETER_DIALOG_NAME_TEXT          404
#define PARAMETER_DIALOG_PASSWORD_MSG       405
#define PARAMETER_DIALOG_PASSWORD_TEXT      406
#define PARAMETER_DIALOG_DIRPATH_MSG        407
#define PARAMETER_DIALOG_DIRPATH_TEXT       408
#define PARAMETER_DIALOG_SAVE               409
#define PARAMETER_DIALOG_CANCEL             410

// *************************** CqueryDlg ***************************


// QUERY DIALOG
enum qryOp
{
    qryOpEQ,
    qryOpLT,
    qryOpGT,
    qryOpLE,
    qryOpGE,
    qryOpBEGINS,
    qryOpCONTAINS,
    qryOpLIKE,
    qryOpBETWEEN
};


// Query strings
wxString const & langQRY_EQ           = wxT("column = column | value");
const wxString & langQRY_LT           = wxT("column < column | value");
const wxString & langQRY_GT           = wxT("column > column | value");
const wxString & langQRY_LE           = wxT("column <= column | value");
const wxString & langQRY_GE           = wxT("column >= column | value");
const wxString & langQRY_BEGINS       = wxT("columns that BEGIN with the string entered");
const wxString & langQRY_CONTAINS     = wxT("columns that CONTAIN the string entered");
const wxString & langQRY_LIKE         = wxT("% matches 0 or more of any char; _ matches 1 char");
const wxString & langQRY_BETWEEN      = wxT("column BETWEEN value AND value");


class CqueryDlg : public wxDialog
{
    private:
        wxDbColInf  *colInf;            // Column inf. returned by db->GetColumns()
        wxDbTable   *dbTable;           // generic wxDbTable object for attaching to the table to query
        wxChar      *masterTableName;   // Name of the table that 'dbTable' will be associated with
        wxString     pWhere;            // A pointer to the storage for the resulting where clause
        wxDb        *pDB;

    public:
        // Used to indicate whether all of the widget pointers (defined
        // below) have been initialized to point to the memory for
        // the named widget.  Used as a safeguard from using the widget
        // before it has been initialized.
        bool                     widgetPtrsSet;

        // Widget pointers
        wxStaticText            *pQueryCol1Msg;
        wxChoice                *pQueryCol1Choice;
        wxStaticText            *pQueryNotMsg;
        wxCheckBox              *pQueryNotCheck;
        wxStaticText            *pQueryOperatorMsg;
        wxChoice                *pQueryOperatorChoice;
        wxStaticText            *pQueryCol2Msg;
        wxChoice                *pQueryCol2Choice;
        wxStaticText            *pQueryValue1Msg;
        wxTextCtrl              *pQueryValue1Txt;
        wxStaticText            *pQueryValue2Msg;
        wxTextCtrl              *pQueryValue2Txt;
        wxStaticText            *pQuerySqlWhereMsg;
        wxTextCtrl              *pQuerySqlWhereMtxt;
        wxButton                *pQueryAddBtn;
        wxButton                *pQueryAndBtn;
        wxButton                *pQueryOrBtn;
        wxButton                *pQueryLParenBtn;
        wxButton                *pQueryRParenBtn;
        wxButton                *pQueryDoneBtn;
        wxButton                *pQueryClearBtn;
        wxButton                *pQueryCountBtn;
        wxButton                *pQueryHelpBtn;
        wxStaticBox             *pQueryHintGrp;
        wxStaticText            *pQueryHintMsg;

        wxTextCtrl              *pFocusTxt;

        CqueryDlg(wxWindow *parent, wxDb *pDb, wxChar *tblName[], const wxString &pWhereArg);
        ~CqueryDlg();

        void        OnButton( wxCommandEvent &event );
        void        OnCommand(wxWindow& win, wxCommandEvent& event);
        void        OnCloseWindow(wxCloseEvent& event);
        void        OnActivate(bool) {};  // necessary for hot keys

        void        AppendToWhere(wxChar *s);
        void        ProcessAddBtn();
        void        ProcessCountBtn();
        bool        ValidateWhereClause();

DECLARE_EVENT_TABLE()
};  // CqueryDlg

#define QUERY_DIALOG                    300

// Parameter dialog control ids
#define QUERY_DIALOG_COL_MSG            301
#define QUERY_DIALOG_COL_CHOICE         302
#define QUERY_DIALOG_NOT_MSG            303
#define QUERY_DIALOG_NOT_CHECKBOX       304
#define QUERY_DIALOG_OP_MSG             305
#define QUERY_DIALOG_OP_CHOICE          306
#define QUERY_DIALOG_COL2_MSG           307
#define QUERY_DIALOG_COL2_CHOICE        308
#define QUERY_DIALOG_WHERE_MSG          309
#define QUERY_DIALOG_WHERE_TEXT         310
#define QUERY_DIALOG_ADD                311
#define QUERY_DIALOG_AND                312
#define QUERY_DIALOG_OR                 313
#define QUERY_DIALOG_LPAREN             314
#define QUERY_DIALOG_RPAREN             315
#define QUERY_DIALOG_DONE               316
#define QUERY_DIALOG_CLEAR              317
#define QUERY_DIALOG_COUNT              318
#define QUERY_DIALOG_VALUE1_MSG         319
#define QUERY_DIALOG_VALUE1_TEXT        320
#define QUERY_DIALOG_VALUE2_MSG         321
#define QUERY_DIALOG_VALUE2_TEXT        322
#define QUERY_DIALOG_HINT_GROUP         323
#define QUERY_DIALOG_HINT_MSG           324

#ifdef wxODBC_BLOB_SUPPORT

class CimageDlg : public wxDialog
{
public:
    CimageDlg(wxWindow *parent, wxChar *pImageData, off_t iSize);
    ~CimageDlg();

    void OnCloseWindow(wxCloseEvent &event);

private:
    wxStaticBitmap *m_pDisplayBmp;
    wxBitmap       *m_pBmp;
    wxImage        *m_pImage;

protected:


DECLARE_EVENT_TABLE()
};  // CimageDlg

#define IMAGE_DIALOG            400

#define IMAGE_DIALOG_STATIC_BMP 401

#endif

wxChar * const langNO                        = wxT("No");
wxChar * const langYES                       = wxT("Yes");
wxChar * const langDBINF_DB_NAME             = wxT("Database Name = ");
wxChar * const langDBINF_DB_VER              = wxT("Database Version = ");
wxChar * const langDBINF_DRIVER_NAME         = wxT("Driver Name = ");
wxChar * const langDBINF_DRIVER_ODBC_VER     = wxT("Driver ODBC Version = ");
wxChar * const langDBINF_DRIVER_MGR_ODBC_VER = wxT("Driver Manager ODBC Version = ");
wxChar * const langDBINF_DRIVER_VER          = wxT("Driver Version = ");
wxChar * const langDBINF_SERVER_NAME         = wxT("Server Name = ");
wxChar * const langDBINF_FILENAME            = wxT("Filename = ");
wxChar * const langDBINF_OUTER_JOINS         = wxT("Outer Joins = ");
wxChar * const langDBINF_STORED_PROC         = wxT("Stored Procedures = ");
wxChar * const langDBINF_MAX_HDBC            = wxT("Max # of Db connections = ");
wxChar * const langDBINF_MAX_HSTMT           = wxT("Max # of cursors (per db connection) = ");
wxChar * const langDBINF_UNLIMITED           = wxT("Unlimited or Unknown");
wxChar * const langDBINF_API_LVL             = wxT("ODBC API conformance level = ");
wxChar * const langDBINF_CLI_LVL             = wxT("Client (SAG) conformance level = ");
wxChar * const langDBINF_SQL_LVL             = wxT("SQL conformance level = ");
wxChar * const langDBINF_COMMIT_BEHAVIOR     = wxT("Commit Behavior = ");
wxChar * const langDBINF_ROLLBACK_BEHAVIOR   = wxT("Rollback Behavior = ");
wxChar * const langDBINF_SUPP_NOT_NULL       = wxT("Support NOT NULL clause = ");
wxChar * const langDBINF_SUPP_IEF            = wxT("Support IEF = ");
wxChar * const langDBINF_TXN_ISOLATION       = wxT("Transaction Isolation Level (default) = ");
wxChar * const langDBINF_TXN_ISOLATION_CURR  = wxT("Transaction Isolation Level (current) = ");
wxChar * const langDBINF_TXN_ISOLATION_OPTS  = wxT("Transaction Isolation Options Available = ");
wxChar * const langDBINF_FETCH_DIRS          = wxT("Fetch Directions = ");
wxChar * const langDBINF_LOCK_TYPES          = wxT("Lock Types (SQLSetPos) = ");
wxChar * const langDBINF_POS_OPERS           = wxT("Position Operations (SQLSetPos) = ");
wxChar * const langDBINF_POS_STMTS           = wxT("Position Statements = ");
wxChar * const langDBINF_SCROLL_CONCURR      = wxT("Concurrency Options (scrollable cursors) = ");
wxChar * const langDBINF_SCROLL_OPTS         = wxT("Scroll Options (scrollable cursors) = ");
wxChar * const langDBINF_STATIC_SENS         = wxT("Static Sensitivity = ");
wxChar * const langDBINF_TXN_CAPABLE         = wxT("Transaction Support = ");
wxChar * const langDBINF_LOGIN_TIMEOUT       = wxT("Login Timeout = ");
wxChar * const langDBINF_NONE                = wxT("None");
wxChar * const langDBINF_LEVEL1              = wxT("Level 1");
wxChar * const langDBINF_LEVEL2              = wxT("Level 2");
wxChar * const langDBINF_NOT_COMPLIANT       = wxT("Not Compliant");
wxChar * const langDBINF_COMPLIANT           = wxT("Compliant");
wxChar * const langDBINF_MIN_GRAMMAR         = wxT("Minimum Grammer");
wxChar * const langDBINF_CORE_GRAMMAR        = wxT("Core Grammer");
wxChar * const langDBINF_EXT_GRAMMAR         = wxT("Extended Grammer");
wxChar * const langDBINF_DELETE_CURSORS      = wxT("Delete cursors");
wxChar * const langDBINF_CLOSE_CURSORS       = wxT("Close cursors");
wxChar * const langDBINF_PRESERVE_CURSORS    = wxT("Preserve cursors");
wxChar * const langDBINF_READ_UNCOMMITTED    = wxT("Read Uncommitted");
wxChar * const langDBINF_READ_COMMITTED      = wxT("Read Committed");
wxChar * const langDBINF_REPEATABLE_READ     = wxT("Repeatable Read");
wxChar * const langDBINF_SERIALIZABLE        = wxT("Serializable");
wxChar * const langDBINF_VERSIONING          = wxT("Versioning");
wxChar * const langDBINF_NEXT                = wxT("Next");
wxChar * const langDBINF_PREV                = wxT("Prev");
wxChar * const langDBINF_FIRST               = wxT("First");
wxChar * const langDBINF_LAST                = wxT("Last");
wxChar * const langDBINF_ABSOLUTE            = wxT("Absolute");
wxChar * const langDBINF_RELATIVE            = wxT("Relative");
wxChar * const langDBINF_RESUME              = wxT("Resume");
wxChar * const langDBINF_BOOKMARK            = wxT("Bookmark");
wxChar * const langDBINF_NO_CHANGE           = wxT("No Change");
wxChar * const langDBINF_EXCLUSIVE           = wxT("Exclusive");
wxChar * const langDBINF_UNLOCK              = wxT("Unlock");
wxChar * const langDBINF_POSITION            = wxT("Position");
wxChar * const langDBINF_REFRESH             = wxT("Refresh");
wxChar * const langDBINF_UPD                 = wxT("Upd");
wxChar * const langDBINF_DEL                 = wxT("Del");
wxChar * const langDBINF_ADD                 = wxT("Add");
wxChar * const langDBINF_POS_DEL             = wxT("Pos Delete");
wxChar * const langDBINF_POS_UPD             = wxT("Pos Update");
wxChar * const langDBINF_SELECT_FOR_UPD      = wxT("Select For Update");
wxChar * const langDBINF_READ_ONLY           = wxT("Read Only");
wxChar * const langDBINF_LOCK                = wxT("Lock");
wxChar * const langDBINF_OPT_ROWVER          = wxT("Opt. Rowver");
wxChar * const langDBINF_OPT_VALUES          = wxT("Opt. Values");
wxChar * const langDBINF_FWD_ONLY            = wxT("Fwd Only");
wxChar * const langDBINF_STATIC              = wxT("Static");
wxChar * const langDBINF_KEYSET_DRIVEN       = wxT("Keyset Driven");
wxChar * const langDBINF_DYNAMIC             = wxT("Dynamic");
wxChar * const langDBINF_MIXED               = wxT("Mixed");
wxChar * const langDBINF_ADDITIONS           = wxT("Additions");
wxChar * const langDBINF_DELETIONS           = wxT("Deletions");
wxChar * const langDBINF_UPDATES             = wxT("Updates");
wxChar * const langDBINF_DML_ONLY            = wxT("DML Only");
wxChar * const langDBINF_DDL_COMMIT          = wxT("DDL Commit");
wxChar * const langDBINF_DDL_IGNORE          = wxT("DDL Ignore");
wxChar * const langDBINF_DDL_AND_DML         = wxT("DDL and DML");
wxChar * const langDBINF_ORACLE_BANNER       = wxT(">>> ORACLE STATISTICS AND TUNING INFORMATION <<<");
wxChar * const langDBINF_DB_BLOCK_GETS       = wxT("DB block gets");
wxChar * const langDBINF_CONSISTENT_GETS     = wxT("Consistent gets");
wxChar * const langDBINF_PHYSICAL_READS      = wxT("Physical reads");
wxChar * const langDBINF_CACHE_HIT_RATIO     = wxT("Cache hit ratio");
wxChar * const langDBINF_TABLESPACE_IO       = wxT("TABLESPACE I/O LEVELS");
wxChar * const langDBINF_PHYSICAL_WRITES     = wxT("Physical writes");
