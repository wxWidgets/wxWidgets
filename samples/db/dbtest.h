///////////////////////////////////////////////////////////////////////////////
// Name:        dbtest.h
// Purpose:     wxWindows database demo app
// Author:      George Tasker
// Modified by:
// Created:     1998
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Remstar International, Inc.
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma interface "dbtest.h"
#endif

#include <wx/string.h>
#include <wx/db.h>
#include <wx/dbtable.h>

enum    DialogModes {mView,mCreate,mEdit,mSearch};

// ID for the menu quit command
#define FILE_CREATE           100
#define FILE_RECREATE_TABLE   110
#define FILE_RECREATE_INDEXES 120
#define FILE_EXIT             199
#define EDIT_PARAMETERS       200
#define ABOUT_DEMO            300

// this seems to be missing, Robert Roebling (?)
#ifndef MAX_PATH
    #if defined(__WXMAC__)
        #define MAX_PATH   260 /* max. length of full pathname */
    #else  /* _MAC */
        #define MAX_PATH   256 /* max. length of full pathname */
    #endif
#endif

// Name of the table to be created/opened
const wxChar     CONTACT_TABLE_NAME[]       = "contacts";

// Number of columns in the CONTACT table
const int        CONTACT_NO_COLS            = 12;        // 0-11

const wxChar     PARAM_FILENAME[]            = "dbtest.cfg";


enum Language {langENGLISH, langFRENCH, langGERMAN, langSPANISH, langOTHER};

// Forward class declarations
class CeditorDlg;
class CparameterDlg;

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
        TIMESTAMP_STRUCT   JoinDate;            // Date on which this person joined the wxWindows project
        Language           NativeLanguage;      // Enumerated type indicating person's native language
        bool               IsDeveloper;         // Is this person a developer for wxWindows, or just a subscriber
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
        bool                 CreateIndexes(void);

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

        void    OnCloseWindow(wxCloseEvent& event);
        void    OnCreate(wxCommandEvent& event);
        void    OnRecreateTable(wxCommandEvent& event);
        void    OnRecreateIndexes(wxCommandEvent& event);
        void    OnExit(wxCommandEvent& event);
        void    OnEditParameters(wxCommandEvent& event);
        void    OnAbout(wxCommandEvent& event);

        void    BuildEditorDialog();
        void    BuildParameterDialog(wxWindow *parent);

DECLARE_EVENT_TABLE()
};  // DatabaseDemoFrame


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
        wxTextCtrl      *pNameTxt,    *pAddress1Txt,  *pAddress2Txt,*pCityTxt,  *pStateTxt, *pCountryTxt,*pPostalCodeTxt;
        wxStaticText    *pNameMsg,    *pAddress1Msg,  *pAddress2Msg,*pCityMsg,  *pStateMsg, *pCountryMsg,*pPostalCodeMsg;
        wxTextCtrl      *pJoinDateTxt,*pContribTxt,   *pLinesTxt;
        wxStaticText    *pJoinDateMsg,*pContribMsg,   *pLinesMsg;
        wxRadioBox      *pDeveloperRadio;
        wxChoice        *pNativeLangChoice;
        wxStaticText    *pNativeLangMsg;

    public:
        // Indicates if the editor dialog has been initialized yet (used to
        // help trap if the Initialize() function failed to load all required
        // resources or not.
        bool             initialized;

        enum DialogModes mode;

        // Pointer to the wxDbTable instance that is used to manipulate
        // the data in memory and in the database
        Ccontact        *Contact;

        CeditorDlg(wxWindow *parent);

        void    OnCloseWindow(wxCloseEvent& event);
        void    OnButton( wxCommandEvent &event );
        void    OnCommand(wxWindow& win, wxCommandEvent& event);
        void    OnActivate(bool) {};  // necessary for hot keys

        bool    Initialize();

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
wxChar * const langQRY_EQ           = "column = column | value";
wxChar * const langQRY_LT           = "column < column | value";
wxChar * const langQRY_GT           = "column > column | value";
wxChar * const langQRY_LE           = "column <= column | value";
wxChar * const langQRY_GE           = "column >= column | value";
wxChar * const langQRY_BEGINS       = "columns that BEGIN with the string entered";
wxChar * const langQRY_CONTAINS     = "columns that CONTAIN the string entered";
wxChar * const langQRY_LIKE         = "% matches 0 or more of any char; _ matches 1 char";
wxChar * const langQRY_BETWEEN      = "column BETWEEN value AND value";


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

