/////////////////////////////////////////////////////////////////////////////
// Name:        foldpanelbartest.cpp
// Purpose:
// Author:      Jorgen Bodde
// Modified by: ABX - 19/12/2004 : possibility of horizontal orientation
//                               : wxWidgets coding standards
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

#include "wx/foldbar/foldpanelbar.h"
#include "foldtestpanel.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources)
#include "sample.xpm"

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
    MyAppFrame(const wxString& title,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxDEFAULT_FRAME_STYLE);

private:
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnOrientation(wxCommandEvent& event);

    // extra handlers for the bar, to show how it works

    void OnCollapseMe(wxCommandEvent &event);
    void OnExpandMe(wxCommandEvent &event);

private:
    wxMenuBar *CreateMenuBar();
    void CreateFoldBar(bool vertical = true);
    wxFoldPanelBar *m_pnl;

private:
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

enum
{
    // menu items
    FoldPanelBarTest_Quit = wxID_EXIT,
    FoldPanelBarTest_About = wxID_ABOUT,
    ID_COLLAPSEME = wxID_HIGHEST,
    ID_EXPANDME,
    FoldPanelBarTest_Horizontal,
    FoldPanelBarTest_Vertical
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyAppFrame, wxFrame)
    EVT_MENU(FoldPanelBarTest_Quit,       MyAppFrame::OnQuit)
    EVT_MENU(FoldPanelBarTest_About,      MyAppFrame::OnAbout)
    EVT_MENU(FoldPanelBarTest_Horizontal, MyAppFrame::OnOrientation)
    EVT_MENU(FoldPanelBarTest_Vertical,   MyAppFrame::OnOrientation)
    EVT_BUTTON(ID_COLLAPSEME,             MyAppFrame::OnCollapseMe)
    EVT_BUTTON(ID_EXPANDME,               MyAppFrame::OnExpandMe)
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
    MyAppFrame *frame = new MyAppFrame(_T("FoldPanelBarTest wxWidgets Test Application"));

    SetTopWindow(frame);

    frame->Show(true);
    return true;
}

// ----------------------------------------------------------------------------
// MyAppFrame Implementation
// ----------------------------------------------------------------------------

MyAppFrame::MyAppFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
       : wxFrame(NULL, wxID_ANY, title, pos, size, style), m_pnl(NULL)
{
    SetIcon(wxIcon(sample_xpm));

    SetMenuBar(CreateMenuBar());

    CreateFoldBar();

    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWidgets!"));
}

wxMenuBar *MyAppFrame::CreateMenuBar()
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(FoldPanelBarTest_Horizontal, _T("&Horizontal\tAlt-H"));
    menuFile->Append(FoldPanelBarTest_Vertical, _T("&Vertical\tAlt-V"));
    menuFile->AppendSeparator();
    menuFile->Append(FoldPanelBarTest_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(FoldPanelBarTest_About, _T("&About...\tF1"), _T("Show about dialog"));

    wxMenuBar *value = new wxMenuBar();
    value->Append(menuFile, _T("&File"));
    value->Append(helpMenu, _T("&Help"));

    return value;
}

void MyAppFrame::CreateFoldBar(bool vertical)
{
    if (vertical)
        SetSize(200,500);
    else
        SetSize(900,200);

    wxFoldPanelBar *bar = new wxFoldPanelBar(this,
                                             wxID_ANY,
                                             wxDefaultPosition,
                                             wxDefaultSize,
                                             wxFPB_DEFAULT_STYLE | ( vertical ? wxFPB_VERTICAL : wxFPB_HORIZONTAL ) ,
                                             wxFPB_COLLAPSE_TO_BOTTOM);

    wxFoldPanel item = bar->AddFoldPanel(_T("Test me"), false);
    bar->AddFoldPanelWindow(item, new wxButton(item.GetParent(), ID_COLLAPSEME, _T("Collapse Me")));

    item = bar->AddFoldPanel(_T("Test me too!"), true);
    bar->AddFoldPanelWindow(item, new wxButton(item.GetParent(), ID_EXPANDME, _T("Expand first one")));
    bar->AddFoldPanelSeperator(item);
    bar->AddFoldPanelWindow(item, new FoldTestPanel(item.GetParent(), wxID_ANY));

    bar->AddFoldPanelSeperator(item);

    bar->AddFoldPanelWindow(item, new wxTextCtrl(item.GetParent(), wxID_ANY, _T("Comment")), wxFPB_ALIGN_WIDTH, wxFPB_DEFAULT_SPACING, 20);

    item = bar->AddFoldPanel(_T("Some opinions ..."), false);
    bar->AddFoldPanelWindow(item, new wxCheckBox(item.GetParent(), wxID_ANY, _T("I like this")));
    if( vertical)
    {
        // do not add this for horizontal for better presentation
        bar->AddFoldPanelWindow(item, new wxCheckBox(item.GetParent(), wxID_ANY, _T("And also this")));
        bar->AddFoldPanelWindow(item, new wxCheckBox(item.GetParent(), wxID_ANY, _T("And gimme this too")));
    }

    bar->AddFoldPanelSeperator(item);

    bar->AddFoldPanelWindow(item, new wxCheckBox(item.GetParent(), wxID_ANY, _T("Check this too if you like")));
    if( vertical)
    {
        // do not add this for horizontal for better presentation
        bar->AddFoldPanelWindow(item, new wxCheckBox(item.GetParent(), wxID_ANY, _T("What about this")));
    }

    item = bar->AddFoldPanel(_T("Choose one ..."), false);
    bar->AddFoldPanelWindow(item, new wxStaticText(item.GetParent(), wxID_ANY, _T("Enter your comment")));
    bar->AddFoldPanelWindow(item, new wxTextCtrl(item.GetParent(), wxID_ANY, _T("Comment")), wxFPB_ALIGN_WIDTH, wxFPB_DEFAULT_SPACING, 20);

    if (m_pnl) m_pnl->Destroy();

    m_pnl = bar;

    wxSize size = GetClientSize();
    m_pnl->SetSize( 0, 0, size.GetWidth(), size.GetHeight() );
}

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

void MyAppFrame::OnOrientation(wxCommandEvent& event)
{
    CreateFoldBar(event.GetId()==FoldPanelBarTest_Vertical);
}

void MyAppFrame::OnCollapseMe(wxCommandEvent &WXUNUSED(event))
{
    wxFoldPanel item = m_pnl->Item(0);
    m_pnl->Collapse(item);
}

void MyAppFrame::OnExpandMe(wxCommandEvent &WXUNUSED(event))
{
    m_pnl->Expand(m_pnl->Item(0));
    m_pnl->Collapse(m_pnl->Item(1));
}
