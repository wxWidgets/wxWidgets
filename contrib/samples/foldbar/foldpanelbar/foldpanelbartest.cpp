
/////////////////////////////////////////////////////////////////////////////
// Name:        FoldPanelBarTest.cpp
// Purpose:     FoldPanelBarTest Test application
// Created:     06/18/04
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

enum
{
	ID_COLLAPSEME = 10000,
	ID_EXPANDME
};

#include "wx/foldbar/foldpanelbar.h"
#include "foldtestpanel.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources)
#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
    #include "mondrian.xpm"
#endif

// ----------------------------------------------------------------------------
// MyApp Class
// ----------------------------------------------------------------------------

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

// ----------------------------------------------------------------------------
// MyAppFrame Class
// ----------------------------------------------------------------------------

class MyAppFrame : public wxFrame
{
public:
    MyAppFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
            long style = wxDEFAULT_FRAME_STYLE);

private:
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

	// extra handlers for the bar, to show how it works

	void OnCollapseMe(wxCommandEvent &event);
	void OnExpandMe(wxCommandEvent &event);

private:
	wxMenuBar *CreateMenuBar();
	wxFoldPanelBar *_pnl;

private:
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

enum
{
    // menu items
    FoldPanelBarTest_Quit = 1,
    FoldPanelBarTest_About = wxID_ABOUT
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyAppFrame, wxFrame)
    EVT_MENU(FoldPanelBarTest_Quit,  MyAppFrame::OnQuit)
    EVT_MENU(FoldPanelBarTest_About, MyAppFrame::OnAbout)
	EVT_BUTTON(ID_COLLAPSEME, MyAppFrame::OnCollapseMe)
	EVT_BUTTON(ID_EXPANDME, MyAppFrame::OnExpandMe)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MyApp Implementation
// ----------------------------------------------------------------------------

bool MyApp::OnInit()
{
    MyAppFrame *frame = new MyAppFrame(_T("FoldPanelBarTest wxWindows Test Application"),
                                 wxPoint(50, 50), wxSize(200, 500));

	SetTopWindow(frame);

    frame->Show(TRUE);
    return TRUE;
}

// ----------------------------------------------------------------------------
// MyAppFrame Implementation
// ----------------------------------------------------------------------------

MyAppFrame::MyAppFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
       : wxFrame(NULL, -1, title, pos, size, style)
{
    SetIcon(wxICON(mondrian));

    SetMenuBar(CreateMenuBar());

    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWindows!"));

	_pnl = new wxFoldPanelBar(this, -1, wxDefaultPosition, wxDefaultSize, wxFPB_DEFAULT_STYLE, wxFPB_COLLAPSE_TO_BOTTOM);
		
	wxFoldPanel item = _pnl->AddFoldPanel("Test me", false);
	_pnl->AddFoldPanelWindow(item, new wxButton(item.GetParent(), ID_COLLAPSEME, "Collapse Me"));

	item = _pnl->AddFoldPanel("Test me too!", true);
	_pnl->AddFoldPanelWindow(item, new wxButton(item.GetParent(), ID_EXPANDME, "Expand first one")); 
	_pnl->AddFoldPanelSeperator(item);
	_pnl->AddFoldPanelWindow(item, new FoldTestPanel(item.GetParent(), -1)); 

	_pnl->AddFoldPanelSeperator(item);

	_pnl->AddFoldPanelWindow(item, new wxTextCtrl(item.GetParent(), -1, "Comment"), wxFPB_ALIGN_WIDTH, wxFPB_DEFAULT_YSPACING, 20); 

	item = _pnl->AddFoldPanel("Some opinions ...", false);
	_pnl->AddFoldPanelWindow(item, new wxCheckBox(item.GetParent(), -1, "I like this")); 
	_pnl->AddFoldPanelWindow(item, new wxCheckBox(item.GetParent(), -1, "And also this")); 
	_pnl->AddFoldPanelWindow(item, new wxCheckBox(item.GetParent(), -1, "And gimme this too")); 

	_pnl->AddFoldPanelSeperator(item);
	
	_pnl->AddFoldPanelWindow(item, new wxCheckBox(item.GetParent(), -1, "Check this too if you like")); 
	_pnl->AddFoldPanelWindow(item, new wxCheckBox(item.GetParent(), -1, "What about this")); 


	item = _pnl->AddFoldPanel("Choose one ...", false);
	_pnl->AddFoldPanelWindow(item, new wxStaticText(item.GetParent(), -1, "Enter your comment")); 
	_pnl->AddFoldPanelWindow(item, new wxTextCtrl(item.GetParent(), -1, "Comment"), wxFPB_ALIGN_WIDTH, wxFPB_DEFAULT_YSPACING, 20); 

}

wxMenuBar *MyAppFrame::CreateMenuBar()
{
	wxMenuBar *value = 0;
	
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(FoldPanelBarTest_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(FoldPanelBarTest_About, _T("&About...\tF1"), _T("Show about dialog"));

    value = new wxMenuBar();
    value->Append(menuFile, _T("&File"));
    value->Append(helpMenu, _T("&Help"));
    
    return value;
}	

// event handlers



void MyAppFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyAppFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("This is the About dialog of the FoldPanelBarTest application.\n")
                _T("Welcome to %s"), wxVERSION_STRING);

    wxMessageBox(msg, _T("About FoldPanelBarTest"), wxOK | wxICON_INFORMATION, this);
}

void MyAppFrame::OnCollapseMe(wxCommandEvent &event)
{
	wxFoldPanel item = _pnl->Item(0);
	_pnl->Collapse(item);
}

void MyAppFrame::OnExpandMe(wxCommandEvent &event)
{
	_pnl->Expand(_pnl->Item(0));
	_pnl->Collapse(_pnl->Item(1));
}
