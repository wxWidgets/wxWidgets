/////////////////////////////////////////////////////////////////////////////
// Name:        minimal.cpp
// Purpose:     Popup wxWidgets sample
// Author:      Robert Roebling
// Modified by:
// Created:     2005-02-04
// RCS-ID:      $Id$
// Copyright:   (c) 2005 Robert Roebling
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

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/popupwin.h"
#include "wx/spinctrl.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources and even
// though we could still include the XPM here it would be unused)
#if !defined(__WXMSW__) && !defined(__WXPM__)
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    Minimal_Quit = wxID_EXIT,
    Minimal_About = wxID_ABOUT,
    Minimal_TestDialog,
    Minimal_StartSimplePopup,
    Minimal_StartScrolledPopup,
    Minimal_LogWindow,
    Minimal_PopupButton,
    Minimal_PopupSpinctrl
};

//----------------------------------------------------------------------------
// SimpleTransientPopup
//----------------------------------------------------------------------------
class SimpleTransientPopup: public wxPopupTransientWindow
{
public:
    SimpleTransientPopup( wxWindow *parent );
    virtual ~SimpleTransientPopup();

    // wxPopupTransientWindow virtual methods are all overridden to log them
    virtual void Popup(wxWindow *focus = NULL);
    virtual void OnDismiss();
    virtual bool ProcessLeftDown(wxMouseEvent& event);
    virtual bool Show( bool show = true );

    wxScrolledWindow* GetChild() { return m_panel; }

private:
    wxScrolledWindow *m_panel;
    wxButton *m_button;
    wxSpinCtrl *m_spinCtrl;

private:
    void OnMouse( wxMouseEvent &event );
    void OnSize( wxSizeEvent &event );
    void OnSetFocus( wxFocusEvent &event );
    void OnKillFocus( wxFocusEvent &event );
    void OnButton( wxCommandEvent& event );
    void OnSpinCtrl( wxSpinEvent& event );

private:
    DECLARE_CLASS(SimpleTransientPopup)
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// SimpleTransientPopup
//----------------------------------------------------------------------------
IMPLEMENT_CLASS(SimpleTransientPopup,wxPopupTransientWindow)

BEGIN_EVENT_TABLE(SimpleTransientPopup,wxPopupTransientWindow)
    EVT_MOUSE_EVENTS( SimpleTransientPopup::OnMouse )
    EVT_SIZE( SimpleTransientPopup::OnSize )
    EVT_SET_FOCUS( SimpleTransientPopup::OnSetFocus )
    EVT_KILL_FOCUS( SimpleTransientPopup::OnKillFocus )
    EVT_BUTTON( Minimal_PopupButton, SimpleTransientPopup::OnButton )
    EVT_SPINCTRL( Minimal_PopupSpinctrl, SimpleTransientPopup::OnSpinCtrl )
END_EVENT_TABLE()

SimpleTransientPopup::SimpleTransientPopup( wxWindow *parent ) :
    wxPopupTransientWindow( parent )
{
    m_panel = new wxScrolledWindow( this, wxID_ANY );
    m_panel->SetBackgroundColour( *wxLIGHT_GREY );
    wxStaticText *text = new wxStaticText( m_panel, wxID_ANY,
                          wxT("wx.PopupTransientWindow is a\n")
                          wxT("wx.PopupWindow which disappears\n")
                          wxT("automatically when the user\n")
                          wxT("clicks the mouse outside it or if it\n")
                          wxT("(or its first child) loses focus in \n")
                          wxT("any other way."), wxPoint( 10,10) );
    wxSize size = text->GetBestSize();

    m_button = new wxButton(m_panel, Minimal_PopupButton, wxT("Press Me"), wxPoint(0, size.y + 10));
    size.y = m_button->GetRect().GetBottom();
    m_spinCtrl = new wxSpinCtrl(m_panel, Minimal_PopupSpinctrl, wxT("Hello"), wxPoint(0, size.y + 5));
    size.y = m_spinCtrl->GetRect().GetBottom();
    
    m_panel->SetSize( size.x+20, size.y+20 );
    SetClientSize( size.x+20, size.y+20 );
}

SimpleTransientPopup::~SimpleTransientPopup()
{
}

void SimpleTransientPopup::Popup(wxWindow *focus)
{
    wxLogMessage( wxT("0x%x SimpleTransientPopup::Popup"), long(this) );
    wxPopupTransientWindow::Popup(focus);
}

void SimpleTransientPopup::OnDismiss()
{
    wxLogMessage( wxT("0x%x SimpleTransientPopup::OnDismiss"), long(this) );
    wxPopupTransientWindow::OnDismiss();
}

bool SimpleTransientPopup::ProcessLeftDown(wxMouseEvent& event)
{
    wxLogMessage( wxT("0x%x SimpleTransientPopup::ProcessLeftDown pos(%d, %d)"), long(this), event.GetX(), event.GetY());
    return wxPopupTransientWindow::ProcessLeftDown(event);
}
bool SimpleTransientPopup::Show( bool show )
{
    wxLogMessage( wxT("0x%x SimpleTransientPopup::Show %d"), long(this), int(show));
    return wxPopupTransientWindow::Show(show);
}

void SimpleTransientPopup::OnSize(wxSizeEvent &event)
{
    wxLogMessage( wxT("0x%x SimpleTransientPopup::OnSize"), long(this) );
    event.Skip();
}

void SimpleTransientPopup::OnSetFocus(wxFocusEvent &event)
{
    wxLogMessage( wxT("0x%x SimpleTransientPopup::OnSetFocus"), long(this) );
    event.Skip();
}

void SimpleTransientPopup::OnKillFocus(wxFocusEvent &event)
{
    wxLogMessage( wxT("0x%x SimpleTransientPopup::OnKillFocus"), long(this) );
    event.Skip();
}

void SimpleTransientPopup::OnMouse(wxMouseEvent &event)
{
    wxLogMessage( wxT("0x%x SimpleTransientPopup::OnMouse pos(%d, %d)"), long(this), event.GetX(), event.GetY());
    event.Skip();
}

void SimpleTransientPopup::OnButton(wxCommandEvent& event)
{
    wxLogMessage( wxT("0x%x SimpleTransientPopup::OnButton ID %d"), long(this), event.GetId());

    wxButton *button = wxDynamicCast(event.GetEventObject(), wxButton);
    if (button->GetLabel() == wxT("Press Me"))
        button->SetLabel(wxT("Pressed"));
    else
        button->SetLabel(wxT("Press Me"));

    event.Skip();
}

void SimpleTransientPopup::OnSpinCtrl(wxSpinEvent& event)
{
    wxLogMessage( wxT("0x%x SimpleTransientPopup::OnSpinCtrl ID %d Value %d"), long(this), event.GetId(), event.GetInt());
    event.Skip();
}

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class MyDialog : public wxDialog
{
public:
    MyDialog(const wxString& title);

    void OnStartSimplePopup(wxCommandEvent& event);
    void OnStartScrolledPopup(wxCommandEvent& event);

private:
    DECLARE_EVENT_TABLE()
};

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title);
    virtual ~MyFrame();

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnTestDialog(wxCommandEvent& event);
    void OnStartSimplePopup(wxCommandEvent& event);
    void OnStartScrolledPopup(wxCommandEvent& event);

private:
    wxTextCtrl *m_logWin;
    wxLog *m_logOld;
    DECLARE_EVENT_TABLE()
};

class MyApp : public wxApp
{
public:
    virtual bool OnInit();

    MyFrame *m_frame;
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------


IMPLEMENT_APP(MyApp)

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // create the main application window
    m_frame = new MyFrame(_T("Popup wxWidgets App"));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    m_frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
    EVT_MENU(Minimal_TestDialog, MyFrame::OnTestDialog)
    EVT_BUTTON(Minimal_StartSimplePopup, MyFrame::OnStartSimplePopup)
    EVT_BUTTON(Minimal_StartScrolledPopup, MyFrame::OnStartScrolledPopup)
END_EVENT_TABLE()

MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    wxMenu *menuFile = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, _T("&About...\tF1"), _T("Show about dialog"));

    menuFile->Append(Minimal_TestDialog, _T("&Test dialog\tAlt-T"), _T("Test dialog"));
    menuFile->Append(Minimal_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

    wxButton *button1 = new wxButton( this, Minimal_StartSimplePopup, wxT("Show simple popup"), wxPoint(20,20) );
    wxButton *button2 = new wxButton( this, Minimal_StartScrolledPopup, wxT("Show scrolled popup"), wxPoint(20,70) );

    m_logWin = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                             wxDefaultSize, wxTE_MULTILINE );
    m_logWin->SetEditable(false);
    wxLogTextCtrl* logger = new wxLogTextCtrl( m_logWin );
    m_logOld = logger->SetActiveTarget( logger );
    logger->SetTimestamp( NULL );

    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    topSizer->Add( button1, 0 );
    topSizer->Add( button2, 0 );
    topSizer->Add( m_logWin, 1, wxEXPAND );

    SetAutoLayout( true );
    SetSizer( topSizer );

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWidgets!"));
#endif // wxUSE_STATUSBAR
}

MyFrame::~MyFrame()
{
    delete wxLog::SetActiveTarget(m_logOld);
}


// event handlers

void MyFrame::OnStartSimplePopup(wxCommandEvent& event)
{
    wxLogMessage( wxT("================================================") );
    SimpleTransientPopup* popup = new SimpleTransientPopup( this );
    wxWindow *btn = (wxWindow*) event.GetEventObject();
    wxPoint pos = btn->ClientToScreen( wxPoint(0,0) );
    wxSize sz = btn->GetSize();
    popup->Position( pos, sz );
    wxLogMessage( wxT("0x%x Simple Popup Shown pos(%d, %d) size(%d, %d)"), long(popup), pos.x, pos.y, sz.x, sz.y );
    popup->Popup();
}

void MyFrame::OnStartScrolledPopup(wxCommandEvent& event)
{
    wxLogMessage( wxT("================================================") );
    SimpleTransientPopup* popup = new SimpleTransientPopup( this );
    popup->GetChild()->SetScrollbars(1, 1, 1000, 1000);
    wxWindow *btn = (wxWindow*) event.GetEventObject();
    wxPoint pos = btn->ClientToScreen( wxPoint(0,0) );
    wxSize sz = btn->GetSize();
    popup->Position( pos, sz );
    wxLogMessage( wxT("0x%x Scrolled Popup Shown pos(%d, %d) size(%d, %d)"), long(popup), pos.x, pos.y, sz.x, sz.y );
    popup->Popup();
}

void MyFrame::OnTestDialog(wxCommandEvent& WXUNUSED(event))
{
    MyDialog dialog( wxT("Test") );
    dialog.ShowModal();
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("This is the About dialog of the popup sample.\n")
                _T("Welcome to %s"), wxVERSION_STRING);

    wxMessageBox(msg, _T("About Popup"), wxOK | wxICON_INFORMATION, this);
}

// ----------------------------------------------------------------------------
// test dialog
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyDialog, wxDialog)
    EVT_BUTTON(Minimal_StartSimplePopup, MyDialog::OnStartSimplePopup)
    EVT_BUTTON(Minimal_StartScrolledPopup, MyDialog::OnStartScrolledPopup)
END_EVENT_TABLE()

MyDialog::MyDialog(const wxString& title)
       : wxDialog(NULL, wxID_ANY, title, wxPoint(50,50), wxSize(400,300))
{

    new wxButton( this, Minimal_StartSimplePopup, wxT("Show simple popup"), wxPoint(20,20) );
    new wxButton( this, Minimal_StartScrolledPopup, wxT("Show scrolled popup"), wxPoint(20,60) );

    new wxButton( this, wxID_OK, wxT("OK"), wxPoint(20,200) );
}

void MyDialog::OnStartSimplePopup(wxCommandEvent& event)
{
    wxLogMessage( wxT("================================================") );
    SimpleTransientPopup* popup = new SimpleTransientPopup( this );
    wxWindow *btn = (wxWindow*) event.GetEventObject();
    wxPoint pos = btn->ClientToScreen( wxPoint(0,0) );
    wxSize sz = btn->GetSize();
    popup->Position( pos, sz );
    wxLogMessage( wxT("0x%x Dialog Simple Popup Shown pos(%d, %d) size(%d, %d)"), long(popup), pos.x, pos.y, sz.x, sz.y );
    popup->Popup();
}

void MyDialog::OnStartScrolledPopup(wxCommandEvent& event)
{
    wxLogMessage( wxT("================================================") );
    SimpleTransientPopup* popup = new SimpleTransientPopup( this );
    popup->GetChild()->SetScrollbars(1, 1, 1000, 1000);
    wxWindow *btn = (wxWindow*) event.GetEventObject();
    wxPoint pos = btn->ClientToScreen( wxPoint(0,0) );
    wxSize sz = btn->GetSize();
    popup->Position( pos, sz );
    wxLogMessage( wxT("0x%x Dialog Scrolled Popup Shown pos(%d, %d) size(%d, %d)"), long(popup), pos.x, pos.y, sz.x, sz.y );
    popup->Popup();
}
