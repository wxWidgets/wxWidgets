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

#pragma interface "dbtest.h"

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

const char paramFilename[] = "database.cfg";


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
		wxFrame		*OnInit(void);
};  // DatabaseDemoApp

DECLARE_APP(DatabaseDemoApp)

// Define a new frame type
class DatabaseDemoFrame: public wxFrame
{ 
	private:
		CeditorDlg		*pEditorDlg;
		CparameterDlg	*pParamDlg;

	public:
		DatabaseDemoFrame(wxFrame *frame, char *title, int x, int y, int w, int h);

		void	OnMenuCommand(int id);
		bool	OnClose(void);

		void	CreateDataTable();
		void	BuildEditorDialog();
		void	BuildParameterDialog(wxWindow *parent);
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
		wxText		*pNameTxt,    *pAddress1Txt,  *pAddress2Txt,*pCityTxt,  *pStateTxt, *pCountryTxt,*pPostalCodeTxt;
		wxMessage	*pNameMsg,    *pAddress1Msg,  *pAddress2Msg,*pCityMsg,  *pStateMsg, *pCountryMsg,*pPostalCodeMsg;
		wxText		*pJoinDateTxt,*pContribTxt,   *pLinesTxt;
		wxMessage   *pJoinDateMsg,*pContribMsg,   *pLinesMsg;
		wxRadioBox	*pDeveloperRadio;
		wxChoice    *pNativeLangChoice;
		wxMessage	*pNativeLangMsg;

	public:
		enum	DialogModes		 mode;
		Ccontact					*Contact;	// this is the table object that will be being manipulated

		CeditorDlg(wxWindow *parent);
		bool	OnClose(void);
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
};  // CeditorDlg


// *************************** CparameterDlg ***************************

class CparameterDlg : public wxDialogBox
{
	private:
		bool						 widgetPtrsSet;
		enum	DialogModes		 mode;
		bool						 saved;
		Cparameters				 savedParamSettings;

		// Pointers to all widgets on the dialog
		wxMessage	*pParamODBCSourceMsg;
		wxListBox	*pParamODBCSourceList;
		wxMessage	*pParamUserNameMsg,		*pParamPasswordMsg;
		wxText		*pParamUserNameTxt,		*pParamPasswordTxt;
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


class CqueryDlg : public wxDialogBox
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
		wxMessage			*pQueryCol1Msg;
		wxChoice				*pQueryCol1Choice;
		wxMessage			*pQueryNotMsg;
		wxCheckBox			*pQueryNotCheck;
		wxMessage			*pQueryOperatorMsg;
		wxChoice				*pQueryOperatorChoice;
		wxMessage			*pQueryCol2Msg;
		wxChoice				*pQueryCol2Choice;
		wxMessage			*pQueryValue1Msg;
		wxText				*pQueryValue1Txt;
		wxMessage			*pQueryValue2Msg;
		wxText				*pQueryValue2Txt;
		wxMessage			*pQuerySqlWhereMsg;
		wxMultiText			*pQuerySqlWhereMtxt;
		wxButton				*pQueryAddBtn;
		wxButton				*pQueryAndBtn;
		wxButton				*pQueryOrBtn;
		wxButton				*pQueryLParenBtn;
		wxButton				*pQueryRParenBtn;
		wxButton				*pQueryDoneBtn;
		wxButton				*pQueryClearBtn;
		wxButton				*pQueryCountBtn;
		wxButton				*pQueryHelpBtn;
		wxGroupBox			*pQueryHintGrp;
		wxMessage			*pQueryHintMsg;

		wxText 				*pFocusTxt;

		CqueryDlg(wxWindow *parent, wxDB *pDb, char *tblName[], char *pWhereArg);

		void		OnCommand(wxWindow& win, wxCommandEvent& event);
		bool		OnClose();
		void		OnActivate(bool) {};  // necessary for hot keys

//		bool		SetWidgetPtrs();
		void		AppendToWhere(char *s);
		void		ProcessAddBtn();
		void		ProcessCountBtn();
		bool		ValidateWhereClause();

};  // CqueryDlg
