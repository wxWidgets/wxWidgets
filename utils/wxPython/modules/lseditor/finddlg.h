#ifndef __FINDDLG_G__
#define __FINDDLG_G__

#include "wx/dialog.h"
#include "wx/checkbox.h"
#include "wx/combobox.h"

#include "wxstldefs.h"

class wxFindTextDialog : public wxDialog
{
public:
	static wxString mLastExpr;
	static bool     mMatchCase;
	static bool     mMatchWord;
	static StrListT mExprList;  

	wxCheckBox* mpCaseCheck;
	wxCheckBox* mpWordCheck;

public:
	wxFindTextDialog( wxWindow* parent, const string& expr = "" );

	bool     MatchWordOn()  { return mMatchWord; }
	bool     MatchCaseOn() { return mMatchCase; }
	wxString GetExpr()      { return mLastExpr;  }
	void     SetExpr( const wxString& expr );

	wxComboBox* GetCombo();

	virtual bool TransferDataFromWindow();

	void OnKeyHook( wxKeyEvent& event );

	DECLARE_EVENT_TABLE()
};
	
#endif
