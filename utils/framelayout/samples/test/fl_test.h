#ifndef __NEW_TEST_G__
#define __NEW_TEST_G__

#include "wx/panel.h"

// Define a new application type
class MyApp: public wxApp
{ public:
    bool OnInit(void);
};

class MyFrame: public wxFrame
{ 
public:

	wxFrameLayout*  mpLayout;
	wxTextCtrl*     mpClientWnd;
	wxPanel*        mpInternalFrm;

	wxTextCtrl* CreateTextCtrl( const wxString& value );

public:
    MyFrame(wxFrame *frame);
	virtual ~MyFrame();

	bool OnClose(void) { Show(FALSE); return TRUE; }

	void OnExit( wxCommandEvent& event );

	DECLARE_EVENT_TABLE()
};

#define NEW_TEST_EXIT 1101

#endif
