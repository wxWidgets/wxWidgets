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
#include <wx/dbtable.h>

enum		DialogModes {mView,mCreate,mEdit,mSearch};

// ID for the menu quit command
#define FILE_CREATE			100
#define FILE_EXIT				199
#define EDIT_PARAMETERS		200
#define ABOUT_DEMO			300


// Name of the table to be created/opened
const char	CONTACT_TABLE_NAME[]		=	"CONTACTS";

// Nuber of columns in the above table
const int	CONTACT_NO_COLS			= 12;		// 0-11

// Global structure for holding ODBC connection information
struct DbStuff DbConnectInf;

enum Language {langENGLISH, langFRENCH, langGERMAN, langSPANISH, langOTHER};

// Forward class declarations
class CeditorDlg;
class CparameterDlg;

#ifdef __WXGTK__
const char paramFilename[] = "../database.cfg";
#else
const char paramFilename[] = "database.cfg";
#endif


/*
 * This class contains the actual data members that are used for transferring
 * data back and forth from the database to the program.  
 *
 * NOTE: The object described in this class is just for example purposes, and has no
 * real meaning other than to show each type of field being used by the database
 */
class CstructContact : public wxObject
{
	public:
		char					Name[ 50+1 ];		//	Contact's name
		char					Addr1[ 50+1 ];
		char					Addr2[ 50+1 ];
		char					City[ 25+1 ];
		char					State[ 25+1 ];
		char					PostalCode[ 15+1 ];
		char					Country[ 20+1 ];
		TIMESTAMP_STRUCT	JoinDate;			// Date on which this person joined the wxWindows project
		Language				NativeLanguage;	// Enumerated type indicating person's native language
		bool					IsDeveloper;		// Is this person a developer for wxWindows, or just a subscriber
		int					Contributions;		// Something to show off an integer field
		ULONG					LinesOfCode;		// Something to show off a 'long' field
};  // CstructContact


//
// NOTE: Ccontact inherits wxTable, which gives access to all the database functionality
//
class Ccontact : public wxTable, public CstructContact
{ 
	private:
		bool				 freeDbConn;
		void				 SetupColumns();

	public:
		wxString			 whereStr;
		wxString			 qryWhereStr;   // Where string returned from the query dialog

		Ccontact(wxDB *pwxDB=NULL);
		~Ccontact();

		void				 Initialize();
		bool				 CreateIndexes(void);
		bool				 FetchByName(char *name);

};  // Ccontact class definition


typedef struct Cparameters
{
	// The length of these strings were arbitrarily picked, and are
	// dependent on the OS and database engine you will be using.
	char	ODBCSource[100+1];
	char	UserName[25+1];
	char	Password[25+1];
} Cparameters;


// Define a new application type
class DatabaseDemoApp: public wxApp
{
	public:
		Cparameters  params;
		bool         OnInit();
};  // DatabaseDemoApp

DECLARE_APP(DatabaseDemoApp)

// Define a new frame type
class DatabaseDemoFrame: public wxFrame
{ 
	private:
		CeditorDlg		*pEditorDlg;
		CparameterDlg	*pParamDlg;

	public:
		DatabaseDemoFrame(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& sz);

		void    OnCloseWindow(wxCloseEvent& event);
        void    OnCreate(wxCommandEvent& event);
        void    OnExit(wxCommandEvent& event);
        void    OnEditParameters(wxCommandEvent& event);
        void    OnAbout(wxCommandEvent& event);

		void	CreateDataTable();
		void	BuildEditorDialog();
		void	BuildParameterDialog(wxWindow *parent);

DECLARE_EVENT_TABLE()
};  // DatabaseDemoFrame



// *************************** CeditorDlg ***************************

class CeditorDlg : public wxPanel
{
	private:
		bool						 widgetPtrsSet;
		wxString					 saveName;

		// Pointers to all widgets on the dialog
		wxButton		*pCreateBtn,  *pEditBtn,      *pDeleteBtn,  *pCopyBtn,  *pSaveBtn,  *pCancelBtn;
		wxButton		*pPrevBtn,    *pNextBtn,      *pQueryBtn,   *pResetBtn, *pDoneBtn,  *pHelpBtn;
		wxButton		*pNameListBtn;
		wxTextCtrl		*pNameTxt,    *pAddress1Txt,  *pAddress2Txt,*pCityTxt,  *pStateTxt, *pCountryTxt,*pPostalCodeTxt;
		wxStaticText	*pNameMsg,    *pAddress1Msg,  *pAddress2Msg,*pCityMsg,  *pStateMsg, *pCountryMsg,*pPostalCodeMsg;
		wxTextCtrl		*pJoinDateTxt,*pContribTxt,   *pLinesTxt;
		wxStaticText   *pJoinDateMsg,*pContribMsg,   *pLinesMsg;
		wxRadioBox	*pDeveloperRadio;
		wxChoice    *pNativeLangChoice;
		wxStaticText	*pNativeLangMsg;

	public:
		enum	DialogModes		 mode;
		Ccontact					*Contact;	// this is the table object that will be being manipulated

		CeditorDlg(wxWindow *parent);
		bool	OnClose(void);
		void    OnButton( wxCommandEvent &event );
		void	OnCommand(wxWindow& win, wxCommandEvent& event);
 		void	OnActivate(bool) {};  // necessary for hot keys

		void	FieldsEditable();
		void	SetMode(enum DialogModes m);
		bool	PutData();
		bool	GetData();
		bool	Save();
		bool	GetNextRec();
		bool	GetPrevRec();
		bool	GetRec(char *whereStr);
		
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
		bool						 widgetPtrsSet;
		enum	DialogModes		 mode;
		bool						 saved;
		Cparameters				 savedParamSettings;

		// Pointers to all widgets on the dialog
		wxStaticText	*pParamODBCSourceMsg;
		wxListBox	*pParamODBCSourceList;
		wxStaticText	*pParamUserNameMsg,		*pParamPasswordMsg;
		wxTextCtrl		*pParamUserNameTxt,		*pParamPasswordTxt;
		wxButton		*pParamSaveBtn,			*pParamCancelBtn;

	public:
		CparameterDlg(wxWindow *parent);
		bool	OnClose(void);
		void	OnCommand(wxWindow& win, wxCommandEvent& event);
 		void	OnActivate(bool) {};  // necessary for hot keys

		bool	PutData();
		bool	GetData();
		bool	Save();
		void	FillDataSourceList();

};  // CparameterDlg

#define PARAMETER_DIALOG                    400

// Parameter dialog control ids
#define PARAMETER_DIALOG_SOURCE_MSG         401
#define PARAMETER_DIALOG_SOURCE_LISTBOX     402
#define PARAMETER_DIALOG_NAME_MSG           403
#define PARAMETER_DIALOG_NAME_TEXT          404
#define PARAMETER_DIALOG_PASSWORD_MSG       405
#define PARAMETER_DIALOG_PASSWORD_TEXT      406
#define PARAMETER_DIALOG_SAVE               407
#define PARAMETER_DIALOG_CANCEL             408

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
char * const langQRY_EQ										= "column = column | value";
char * const langQRY_LT										= "column < column | value";
char * const langQRY_GT										= "column > column | value";
char * const langQRY_LE										= "column <= column | value";
char * const langQRY_GE										= "column >= column | value";
char * const langQRY_BEGINS								= "columns that BEGIN with the string entered";
char * const langQRY_CONTAINS								= "columns that CONTAIN the string entered";
char * const langQRY_LIKE									= "% matches 0 or more of any char; _ matches 1 char";
char * const langQRY_BETWEEN								= "column BETWEEN value AND value";


class CqueryDlg : public wxDialog
{
	private:
		CcolInf	*colInf;		// Column inf. returned by db->GetColumns()
		wxTable	*dbTable;
		char		*masterTableName;
		char		*pWhere;		// A pointer to the storage for the resulting where clause
		wxDB		*pDB;

	public:
		bool					widgetPtrsSet;

		// Widget pointers
		wxStaticText			*pQueryCol1Msg;
		wxChoice				*pQueryCol1Choice;
		wxStaticText			*pQueryNotMsg;
		wxCheckBox			*pQueryNotCheck;
		wxStaticText			*pQueryOperatorMsg;
		wxChoice				*pQueryOperatorChoice;
		wxStaticText			*pQueryCol2Msg;
		wxChoice				*pQueryCol2Choice;
		wxStaticText			*pQueryValue1Msg;
		wxTextCtrl				*pQueryValue1Txt;
		wxStaticText			*pQueryValue2Msg;
		wxTextCtrl				*pQueryValue2Txt;
		wxStaticText			*pQuerySqlWhereMsg;
		wxTextCtrl			*pQuerySqlWhereMtxt;
		wxButton				*pQueryAddBtn;
		wxButton				*pQueryAndBtn;
		wxButton				*pQueryOrBtn;
		wxButton				*pQueryLParenBtn;
		wxButton				*pQueryRParenBtn;
		wxButton				*pQueryDoneBtn;
		wxButton				*pQueryClearBtn;
		wxButton				*pQueryCountBtn;
		wxButton				*pQueryHelpBtn;
		wxStaticBox			*pQueryHintGrp;
		wxStaticText			*pQueryHintMsg;

		wxTextCtrl 				*pFocusTxt;

		CqueryDlg(wxWindow *parent, wxDB *pDb, char *tblName[], char *pWhereArg);

		void    OnButton( wxCommandEvent &event );
		void		OnCommand(wxWindow& win, wxCommandEvent& event);
		bool		OnClose();
		void		OnActivate(bool) {};  // necessary for hot keys

//		bool		SetWidgetPtrs();
		void		AppendToWhere(char *s);
		void		ProcessAddBtn();
		void		ProcessCountBtn();
		bool		ValidateWhereClause();

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

