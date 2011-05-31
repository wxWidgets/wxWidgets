/////////////////////////////////////////////////////////////////////////////
// Name:        collpane.cpp
// Purpose:     wxCollapsiblePane sample
// Author:      Francesco Montorsi
// Modified by:
// Created:     14/10/06
// RCS-ID:      $Id$
// Copyright:   (c) Francesco Montorsi
// Licence:     wxWindows licence
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

#ifndef __WXMSW__
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// ID for the menu commands
enum
{
    PANE_COLLAPSE = 100,
    PANE_EXPAND,
    PANE_SETLABEL,
    PANE_SHOWDLG,
    PANE_ABOUT = wxID_ABOUT,
    PANE_QUIT = wxID_EXIT,

    PANE_BUTTON,
    PANE_TEXTCTRL
};


// ----------------------------------------------------------------------------
// our classes
// ----------------------------------------------------------------------------

class MyApp: public wxApp
{
public:
    MyApp() { }

    virtual bool OnInit();

    wxDECLARE_NO_COPY_CLASS(MyApp);
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

    // UI update handlers
    void OnCollapseUpdateUI(wxUpdateUIEvent& event);
    void OnExpandUpdateUI(wxUpdateUIEvent& event);

private:
    wxCollapsiblePane *m_collPane;
    wxBoxSizer *m_paneSizer;

    DECLARE_EVENT_TABLE()
    wxDECLARE_NO_COPY_CLASS(MyFrame);
};

class MyDialog : public wxDialog
{
public:
    MyDialog(wxFrame *parent);
    void OnToggleStatus(wxCommandEvent& WXUNUSED(ev));
    void OnAlignButton(wxCommandEvent& WXUNUSED(ev));
    void OnPaneChanged(wxCollapsiblePaneEvent& event);

private:
    wxCollapsiblePane *m_collPane;
    wxGridSizer *m_paneSizer;

    DECLARE_EVENT_TABLE()
    wxDECLARE_NO_COPY_CLASS(MyDialog);
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

    EVT_UPDATE_UI(PANE_COLLAPSE, MyFrame::OnCollapseUpdateUI)
    EVT_UPDATE_UI(PANE_EXPAND, MyFrame::OnExpandUpdateUI)
END_EVENT_TABLE()

// My frame constructor
MyFrame::MyFrame()
       : wxFrame(NULL, wxID_ANY, wxT("wxCollapsiblePane sample"),
                 wxDefaultPosition, wxSize(420, 300),
                 wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE)
{
    SetIcon(wxICON(sample));

#if wxUSE_STATUSBAR
    CreateStatusBar(2);
#endif // wxUSE_STATUSBAR

    // Make a menubar
    wxMenu *paneMenu = new wxMenu;
    paneMenu->Append(PANE_COLLAPSE, wxT("Collapse\tCtrl-C"));
    paneMenu->Append(PANE_EXPAND, wxT("Expand\tCtrl-E"));
    paneMenu->AppendSeparator();
    paneMenu->Append(PANE_SETLABEL, wxT("Set label...\tCtrl-L"));
    paneMenu->AppendSeparator();
    paneMenu->Append(PANE_SHOWDLG, wxT("Show dialog...\tCtrl-S"));
    paneMenu->AppendSeparator();
    paneMenu->Append(PANE_QUIT);

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(PANE_ABOUT);

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(paneMenu, wxT("&Pane"));
    menuBar->Append(helpMenu, wxT("&Help"));
    SetMenuBar(menuBar);

    m_collPane = new wxCollapsiblePane(this, -1, wxT("test!"));
    wxWindow *win = m_collPane->GetPane();

    m_paneSizer = new wxBoxSizer( wxVERTICAL );
    m_paneSizer->Add( new wxStaticText(win, -1, wxT("Static text") ), 0, wxALIGN_LEFT );
    m_paneSizer->Add( new wxStaticText(win, -1, wxT("Yet another one!") ), 0, wxALIGN_LEFT );
    m_paneSizer->Add( new wxTextCtrl(win, PANE_TEXTCTRL, wxT("Text control"), wxDefaultPosition, wxSize(80,-1) ), 0, wxALIGN_LEFT );
    m_paneSizer->Add( new wxButton(win, PANE_BUTTON, wxT("Press to align right") ), 0, wxALIGN_LEFT );
    win->SetSizer( m_paneSizer );
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
    wxString text = wxGetTextFromUser
                    (
                        wxT("Enter new label"),
                        wxGetTextFromUserPromptStr,
                        m_collPane->GetLabel()
                    );
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
    info.SetCopyright(wxT("(C) 2006 Francesco Montorsi <frm@users.sourceforge.net>"));

    wxAboutBox(info);
}

void MyFrame::OnCollapseUpdateUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_collPane->IsCollapsed());
}

void MyFrame::OnExpandUpdateUI(wxUpdateUIEvent& event)
{
    event.Enable(m_collPane->IsCollapsed());
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
    EVT_BUTTON(PANE_BUTTON, MyDialog::OnAlignButton)
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
    m_paneSizer = new wxGridSizer(4, 1, 5, 5);

    m_paneSizer->Add( new wxStaticText(win, -1, wxT("Static text") ), 0, wxALIGN_LEFT );
    m_paneSizer->Add( new wxStaticText(win, -1, wxT("Yet another one!") ), 0, wxALIGN_LEFT );
    m_paneSizer->Add( new wxTextCtrl(win, PANE_TEXTCTRL, wxT("Text control"), wxDefaultPosition, wxSize(80,-1) ), 0, wxALIGN_LEFT );
    m_paneSizer->Add( new wxButton(win, PANE_BUTTON, wxT("Press to align right") ), 0, wxALIGN_LEFT );
    win->SetSizer( m_paneSizer );

    win->SetSizer( m_paneSizer );
    m_paneSizer->SetSizeHints(win);

    SetSizer(sz);
    sz->SetSizeHints(this);
}

void MyDialog::OnToggleStatus(wxCommandEvent& WXUNUSED(ev))
{
    m_collPane->Collapse(!m_collPane->IsCollapsed());
}

void MyDialog::OnAlignButton(wxCommandEvent& WXUNUSED(ev))
{
   wxSizerItem *item = m_paneSizer->GetItem( FindWindow(PANE_TEXTCTRL), true );
   item->SetFlag(  wxALIGN_RIGHT );

   Layout();
}

void MyDialog::OnPaneChanged(wxCollapsiblePaneEvent& event)
{
    wxLogMessage(wxT("The pane has just been %s by the user"),
               event.GetCollapsed() ? wxT("collapsed") : wxT("expanded"));
}

