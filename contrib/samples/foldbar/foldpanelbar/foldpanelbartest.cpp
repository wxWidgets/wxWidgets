/////////////////////////////////////////////////////////////////////////////
// Name:        foldpanelbartest.cpp
// Purpose:
// Author:      Jorgen Bodde
// Modified by:
// Created:     18/06/2004
// RCS-ID:      $Id$
// Copyright:   (c) Jorgen Bodde
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
// event tables and other macros for wxWidgets
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
    MyAppFrame *frame = new MyAppFrame(_T("FoldPanelBarTest wxWidgets Test Application"),
                                 wxPoint(50, 50), wxSize(200, 500));

    SetTopWindow(frame);

    frame->Show(true);
    return true;
}

// ----------------------------------------------------------------------------
// MyAppFrame Implementation
// ----------------------------------------------------------------------------

MyAppFrame::MyAppFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
       : wxFrame(NULL, wxID_ANY, title, pos, size, style)
{
    SetIcon(wxICON(mondrian));

    SetMenuBar(CreateMenuBar());

    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWidgets!"));

    _pnl = new wxFoldPanelBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFPB_DEFAULT_STYLE, wxFPB_COLLAPSE_TO_BOTTOM);

    wxFoldPanel item = _pnl->AddFoldPanel(_T("Test me"), false);
    _pnl->AddFoldPanelWindow(item, new wxButton(item.GetParent(), ID_COLLAPSEME, _T("Collapse Me")));

    item = _pnl->AddFoldPanel(_T("Test me too!"), true);
    _pnl->AddFoldPanelWindow(item, new wxButton(item.GetParent(), ID_EXPANDME, _T("Expand first one")));
    _pnl->AddFoldPanelSeperator(item);
    _pnl->AddFoldPanelWindow(item, new FoldTestPanel(item.GetParent(), wxID_ANY));

    _pnl->AddFoldPanelSeperator(item);

    _pnl->AddFoldPanelWindow(item, new wxTextCtrl(item.GetParent(), wxID_ANY, _T("Comment")), wxFPB_ALIGN_WIDTH, wxFPB_DEFAULT_YSPACING, 20);

    item = _pnl->AddFoldPanel(_T("Some opinions ..."), false);
    _pnl->AddFoldPanelWindow(item, new wxCheckBox(item.GetParent(), wxID_ANY, _T("I like this")));
    _pnl->AddFoldPanelWindow(item, new wxCheckBox(item.GetParent(), wxID_ANY, _T("And also this")));
    _pnl->AddFoldPanelWindow(item, new wxCheckBox(item.GetParent(), wxID_ANY, _T("And gimme this too")));

    _pnl->AddFoldPanelSeperator(item);

    _pnl->AddFoldPanelWindow(item, new wxCheckBox(item.GetParent(), wxID_ANY, _T("Check this too if you like")));
    _pnl->AddFoldPanelWindow(item, new wxCheckBox(item.GetParent(), wxID_ANY, _T("What about this")));


    item = _pnl->AddFoldPanel(_T("Choose one ..."), false);
    _pnl->AddFoldPanelWindow(item, new wxStaticText(item.GetParent(), wxID_ANY, _T("Enter your comment")));
    _pnl->AddFoldPanelWindow(item, new wxTextCtrl(item.GetParent(), wxID_ANY, _T("Comment")), wxFPB_ALIGN_WIDTH, wxFPB_DEFAULT_YSPACING, 20);

}

wxMenuBar *MyAppFrame::CreateMenuBar()
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(FoldPanelBarTest_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(FoldPanelBarTest_About, _T("&About...\tF1"), _T("Show about dialog"));

    wxMenuBar *value = new wxMenuBar();
    value->Append(menuFile, _T("&File"));
    value->Append(helpMenu, _T("&Help"));

    return value;
}

// event handlers



void MyAppFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyAppFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("This is the About dialog of the FoldPanelBarTest application.\n")
                _T("Welcome to %s"), wxVERSION_STRING);

    wxMessageBox(msg, _T("About FoldPanelBarTest"), wxOK | wxICON_INFORMATION, this);
}

void MyAppFrame::OnCollapseMe(wxCommandEvent &WXUNUSED(event))
{
    wxFoldPanel item = _pnl->Item(0);
    _pnl->Collapse(item);
}

void MyAppFrame::OnExpandMe(wxCommandEvent &WXUNUSED(event))
{
    _pnl->Expand(_pnl->Item(0));
    _pnl->Collapse(_pnl->Item(1));
}
