/////////////////////////////////////////////////////////////////////////////
// Name:        collpane.cpp
// Purpose:     wxCollapsiblePane sample
// Author:      Francesco Montorsi
// Modified by:
// Created:     14/10/06
// RCS-ID:      $Id$
// Copyright:   (c) Francesco Montorsi
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/log.h"

    #include "wx/app.h"
    #include "wx/frame.h"

    #include "wx/scrolwin.h"
    #include "wx/menu.h"

    #include "wx/textdlg.h"       // for wxGetTextFromUser
#endif

#include "wx/collpane.h"
#include "wx/sizer.h"
#include "wx/stattext.h"
#include "wx/clrpicker.h"
#include "wx/filepicker.h"
#include "wx/fontpicker.h"
#include "wx/aboutdlg.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// ID for the menu commands
enum
{
    PANE_COLLAPSE,
    PANE_EXPAND,
    PANE_SETLABEL,
    PANE_SHOWDLG,
    PANE_ABOUT = wxID_ABOUT,
    PANE_QUIT = wxID_EXIT
};


// ----------------------------------------------------------------------------
// our classes
// ----------------------------------------------------------------------------

class MyApp: public wxApp
{
public:
    MyApp() { }

    virtual bool OnInit();

    DECLARE_NO_COPY_CLASS(MyApp)
};

class MyFrame: public wxFrame
{
public:
    MyFrame();
    virtual ~MyFrame();

    // Menu commands
    void OnCollapse(wxCommandEvent& event);
    void OnExpand(wxCommandEvent& event);
    void OnSetLabel(wxCommandEvent& event);
    void OnShowDialog(wxCommandEvent& event);
    void Quit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    // Menu command update functions
    void UpdateUI(wxUpdateUIEvent& event);

private:
    wxCollapsiblePane *m_collPane;

    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(MyFrame)
};

class MyDialog : public wxDialog
{
public:
    MyDialog(wxFrame *parent);
    void OnToggleStatus(wxCommandEvent& WXUNUSED(ev));
    void OnPaneChanged(wxCollapsiblePaneEvent& event);

private:
    wxCollapsiblePane *m_collPane;

    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(MyDialog)
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
    // create and show the main frame
    MyFrame* frame = new MyFrame;

    frame->Show(true);

    return true;
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(PANE_COLLAPSE, MyFrame::OnCollapse)
    EVT_MENU(PANE_EXPAND, MyFrame::OnExpand)
    EVT_MENU(PANE_SETLABEL, MyFrame::OnSetLabel)
    EVT_MENU(PANE_SHOWDLG, MyFrame::OnShowDialog)
    EVT_MENU(PANE_ABOUT, MyFrame::OnAbout)
    EVT_MENU(PANE_QUIT, MyFrame::Quit)

    EVT_UPDATE_UI(wxID_ANY, MyFrame::UpdateUI)
END_EVENT_TABLE()

// My frame constructor
MyFrame::MyFrame()
       : wxFrame(NULL, wxID_ANY, _T("wxCollapsiblePane sample"),
                 wxDefaultPosition, wxSize(420, 300),
                 wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE)
{
#if wxUSE_STATUSBAR
    CreateStatusBar(2);
#endif // wxUSE_STATUSBAR

    // Make a menubar
    wxMenu *paneMenu = new wxMenu;
    paneMenu->Append(PANE_COLLAPSE, _T("Collapse\tCtrl-C"));
    paneMenu->Append(PANE_EXPAND, _T("Expand\tCtrl-E"));
    paneMenu->AppendSeparator();
    paneMenu->Append(PANE_SETLABEL, _T("Set label...\tCtrl-S"));
    paneMenu->AppendSeparator();
    paneMenu->Append(PANE_SHOWDLG, _T("Show dialog...\tCtrl-S"));
    paneMenu->AppendSeparator();
    paneMenu->Append(PANE_QUIT);

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(PANE_ABOUT);

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(paneMenu, _T("&Pane"));
    menuBar->Append(helpMenu, _T("&Help"));
    SetMenuBar(menuBar);

    m_collPane = new wxCollapsiblePane(this, -1, wxT("test!"));
    wxWindow *win = m_collPane->GetPane();

    new wxStaticText(win, -1, wxT("Static control with absolute coords"), wxPoint(10,2));
    new wxStaticText(win, -1, wxT("Yet another one!"), wxPoint(30, 30));
    new wxTextCtrl(win, -1, wxT("You can place anything you like inside a wxCollapsiblePane"),
                   wxPoint(5, 60), wxSize(300, -1));
}

MyFrame::~MyFrame()
{
}

// menu command handlers

void MyFrame::Quit(wxCommandEvent& WXUNUSED(event) )
{
    Close(true);
}

void MyFrame::OnCollapse(wxCommandEvent& WXUNUSED(event) )
{
    m_collPane->Collapse();
}

void MyFrame::OnExpand(wxCommandEvent& WXUNUSED(event) )
{
    m_collPane->Expand();
}

void MyFrame::OnSetLabel(wxCommandEvent& WXUNUSED(event) )
{
    wxString text = wxGetTextFromUser(wxT("Input the new label"));
    m_collPane->SetLabel(text);
}

void MyFrame::OnShowDialog(wxCommandEvent& WXUNUSED(event) )
{
    MyDialog dlg(this);
    dlg.ShowModal();
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    wxAboutDialogInfo info;
    info.SetName(_("wxCollapsiblePane sample"));
    info.SetDescription(_("This sample program demonstrates usage of wxCollapsiblePane"));
    info.SetCopyright(_T("(C) 2006 Francesco Montorsi <frm@users.sourceforge.net>"));

    wxAboutBox(info);
}

void MyFrame::UpdateUI(wxUpdateUIEvent& event)
{
    GetMenuBar()->Enable(PANE_COLLAPSE, !m_collPane->IsCollapsed());
    GetMenuBar()->Enable(PANE_EXPAND, m_collPane->IsCollapsed());
}


// ----------------------------------------------------------------------------
// MyDialog
// ----------------------------------------------------------------------------

enum
{
    PANEDLG_TOGGLESTATUS_BTN = wxID_HIGHEST
};

BEGIN_EVENT_TABLE(MyDialog, wxDialog)
    EVT_BUTTON(PANEDLG_TOGGLESTATUS_BTN, MyDialog::OnToggleStatus)
    EVT_COLLAPSIBLEPANE_CHANGED(wxID_ANY, MyDialog::OnPaneChanged)
END_EVENT_TABLE()

MyDialog::MyDialog(wxFrame *parent)
                : wxDialog(parent, wxID_ANY, wxT("Test dialog"),
                            wxDefaultPosition, wxDefaultSize,
                            wxRESIZE_BORDER|wxDEFAULT_DIALOG_STYLE )
{
    wxSizer *sz = new wxBoxSizer(wxVERTICAL);
    sz->Add(new wxStaticText(this, -1,
        wxT("This dialog allows you to test the wxCollapsiblePane control")),
        0, wxALL, 5);
    sz->Add(new wxButton(this, PANEDLG_TOGGLESTATUS_BTN, wxT("Change status")),
        1, wxGROW|wxALL, 5);

    m_collPane = new wxCollapsiblePane(this, -1, wxT("Click here for a surprise"));
    sz->Add(m_collPane, 0, wxGROW|wxALL, 5);
    sz->Add(new wxTextCtrl(this, -1, wxT("just a test")), 0, wxGROW|wxALL, 5);
    sz->AddSpacer(10);
    sz->Add(new wxButton(this, wxID_OK), 0, wxALIGN_RIGHT|wxALL, 5);

    // now add test controls in the collapsible pane
    wxWindow *win = m_collPane->GetPane();
    wxSizer *paneSz = new wxGridSizer(2, 2, 5, 5);
    paneSz->Add(new wxColourPickerCtrl(win, -1), 1, wxGROW|wxALL, 2);
    paneSz->Add(new wxFontPickerCtrl(win, -1), 1, wxGROW|wxALL, 2);
    paneSz->Add(new wxFilePickerCtrl(win, -1), 1, wxALL|wxALIGN_CENTER, 2);
    paneSz->Add(new wxDirPickerCtrl(win, -1), 1, wxALL|wxALIGN_CENTER, 2);
    win->SetSizer(paneSz);
    paneSz->SetSizeHints(win);

    SetSizer(sz);
    sz->SetSizeHints(this);
}

void MyDialog::OnToggleStatus(wxCommandEvent& WXUNUSED(ev))
{
    m_collPane->Collapse(!m_collPane->IsCollapsed());
}

void MyDialog::OnPaneChanged(wxCollapsiblePaneEvent &event)
{
    wxLogDebug(wxT("The pane has just been %s by the user"),
               event.GetCollapsed() ? wxT("collapsed") : wxT("expanded"));
}

