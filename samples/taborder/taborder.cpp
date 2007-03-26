/////////////////////////////////////////////////////////////////////////////
// Name:        taborder.cpp
// Purpose:     Sample for testing TAB navigation
// Author:      Vadim Zeitlin
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Vadim Zeitlin
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// menu commands and controls ids
enum
{
    // file menu
    TabOrder_Quit = 100,
    TabOrder_About,

    // navigation menu
    TabOrder_TabForward = 200,
    TabOrder_TabBackward,

    TabOrder_Max
};

// status panes: first one is for temporary messages, the second one shows
// current focus
enum
{
    StatusPane_Default,
    StatusPane_Focus,
    StatusPane_Max
};

// ----------------------------------------------------------------------------
// declarations of the classes used in this sample
// ----------------------------------------------------------------------------

// the main application class
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

// and the main sample window
class MyFrame : public wxFrame
{
public:
    MyFrame();

private:
    void OnAbout(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);

    void OnTabForward(wxCommandEvent& event);
    void OnTabBackward(wxCommandEvent& event);

    void OnIdle(wxIdleEvent& event);

    void DoNavigate(long flags)
    {
        wxNavigationKeyEvent event;
        event.SetFlags(flags);
        if ( ProcessEvent(event) )
            wxLogStatus(this, _T("Navigation event processed"));
        else
            wxLogStatus(this, _T("Navigation event ignored"));
    }

    DECLARE_EVENT_TABLE()
};

// and the panel taking up MyFrame client area
class MyPanel : public wxPanel
{
public:
    MyPanel(wxWindow *parent);
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    MyFrame *frame = new MyFrame;
    frame->Show(true);

    return true;
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(TabOrder_Quit,   MyFrame::OnQuit)
    EVT_MENU(TabOrder_About,  MyFrame::OnAbout)

    EVT_MENU(TabOrder_TabForward, MyFrame::OnTabForward)
    EVT_MENU(TabOrder_TabBackward, MyFrame::OnTabBackward)

    EVT_IDLE(MyFrame::OnIdle)
END_EVENT_TABLE()

MyFrame::MyFrame()
       : wxFrame(NULL, wxID_ANY, _T("TabOrder wxWidgets Sample"),
                 wxDefaultPosition, wxSize(700, 450))
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(TabOrder_About, _T("&About...\tF1"));
    menuFile->AppendSeparator();
    menuFile->Append(TabOrder_Quit, _T("E&xit\tAlt-X"), _T("Quit the sample"));

    wxMenu *menuNav = new wxMenu;
    menuNav->Append(TabOrder_TabForward, _T("Tab &forward\tCtrl-F"),
                    _T("Emulate a <Tab> press"));
    menuNav->Append(TabOrder_TabBackward, _T("Tab &backward\tCtrl-B"),
                    _T("Emulate a <Shift-Tab> press"));

    wxMenuBar *mbar = new wxMenuBar;
    mbar->Append(menuFile, _T("&File"));
    mbar->Append(menuNav, _T("&Navigate"));

    SetMenuBar(mbar);

    new MyPanel(this);

    CreateStatusBar(StatusPane_Max);
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("Tab navigation sample\n(c) 2007 Vadim Zeitlin"),
                 _T("About TabOrder wxWidgets Sample"), wxOK, this);
}

void MyFrame::OnTabForward(wxCommandEvent& WXUNUSED(event))
{
    DoNavigate(wxNavigationKeyEvent::IsForward | wxNavigationKeyEvent::FromTab);
}

void MyFrame::OnTabBackward(wxCommandEvent& WXUNUSED(event))
{
    DoNavigate(wxNavigationKeyEvent::IsBackward | wxNavigationKeyEvent::FromTab);
}

void MyFrame::OnIdle( wxIdleEvent& WXUNUSED(event) )
{
    // track the window which has the focus in the status bar
    static wxWindow *s_windowFocus = NULL;
    wxWindow *focus = wxWindow::FindFocus();
    if ( focus != s_windowFocus )
    {
        s_windowFocus = focus;

        wxString msg;
        if ( focus )
        {
            msg.Printf(_T("Focus is at %s"), s_windowFocus->GetName().c_str());
        }
        else
        {
            msg = _T("No focus");
        }

        SetStatusText(msg, StatusPane_Focus);
    }
}

// ----------------------------------------------------------------------------
// MyPanel
// ----------------------------------------------------------------------------

MyPanel::MyPanel(wxWindow *parent)
       : wxPanel(parent, wxID_ANY)
{
    wxSizerFlags flagsBorder = wxSizerFlags().Border();

    wxSizer *sizerH = new wxBoxSizer(wxHORIZONTAL);
    sizerH->Add(new wxButton(this, wxID_ANY, _T("&First")), flagsBorder);
    sizerH->Add(new wxButton(this, wxID_ANY, _T("&Second")), flagsBorder);

    wxSizer *sizerV = new wxBoxSizer(wxVERTICAL);
    sizerV->Add(sizerH, wxSizerFlags(1).Expand());
    sizerV->Add(new wxListBox(this, wxID_ANY), wxSizerFlags(1).Expand());
    SetSizerAndFit(sizerV);
}

