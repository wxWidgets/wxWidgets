#ifndef __NEW_TEST_G__
#define __NEW_TEST_G__

#include "objstore.h"
#include "wx/panel.h"

// Define a new application type
class MyApp: public wxApp
{ public:
    bool OnInit(void);
};

class MyFrame: public wxFrame
{ 
public:
	wxObjectStorage mStore;

	wxFrameLayout*  mpLayout;
	wxTextCtrl*     mpClientWnd;
	wxPanel*        mpInternalFrm;

	wxTextCtrl* CreateTextCtrl( const wxString& value );

public:
    MyFrame(wxFrame *frame);
	virtual ~MyFrame();

	bool OnClose(void) { Show(FALSE); return TRUE; }

	void OnLoad( wxCommandEvent& event );
	void OnSave( wxCommandEvent& event );
	void OnExit( wxCommandEvent& event );

	DECLARE_EVENT_TABLE()
};

#define NEW_TEST_SAVE 1101
#define NEW_TEST_LOAD 1102
#define NEW_TEST_EXIT 1103

#endif
