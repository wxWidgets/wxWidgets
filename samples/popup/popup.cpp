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

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources and even
// though we could still include the XPM here it would be unused)
#if !defined(__WXMSW__) && !defined(__WXPM__)
    #include "../sample.xpm"
#endif

class SimpleTransientPopup: public wxPopupTransientWindow
{
public:
    SimpleTransientPopup( wxWindow *parent );
    virtual ~SimpleTransientPopup();
    
    void OnDismiss();
    
private:
    wxPanel *m_panel;
    
private:
    void OnMouse( wxMouseEvent &event );
    void OnSize( wxSizeEvent &event );
    void OnSetFocus( wxFocusEvent &event );
    void OnKillFocus( wxFocusEvent &event );

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
END_EVENT_TABLE()

SimpleTransientPopup::SimpleTransientPopup( wxWindow *parent ) :
    wxPopupTransientWindow( parent )
{
    m_panel = new wxPanel( this, -1 );
    m_panel->SetBackgroundColour( *wxLIGHT_GREY );
    wxStaticText *text = new wxStaticText( m_panel, -1, 
                          wxT("wx.PopupTransientWindow is a\n")
                          wxT("wx.PopupWindow which disappears\n")
                          wxT("automatically when the user\n")
                          wxT("clicks the mouse outside it or if it\n")
                          wxT("(or its first child) loses focus in \n")
                          wxT("any other way."), wxPoint( 10,10) );
    wxSize size = text->GetBestSize();
    m_panel->SetSize( size.x+20, size.y+20 );
    SetClientSize( size.x+20, size.y+20 );
}

SimpleTransientPopup::~SimpleTransientPopup()
{
}

void SimpleTransientPopup::OnDismiss()
{
}

void SimpleTransientPopup::OnSize(wxSizeEvent &event)
{
    event.Skip();
}

void SimpleTransientPopup::OnSetFocus(wxFocusEvent &event)
{
    event.Skip();
}

void SimpleTransientPopup::OnKillFocus(wxFocusEvent &event)
{
    event.Skip();
}

void SimpleTransientPopup::OnMouse(wxMouseEvent &event)
{
    event.Skip();
}

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

class MyDialog : public wxDialog
{
public:
    MyDialog(const wxString& title);

    void OnStartSimplePopup(wxCommandEvent& event);
    void OnStartComplexPopup(wxCommandEvent& event);

private:
    DECLARE_EVENT_TABLE()
};

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title);

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnTestDialog(wxCommandEvent& event);
    void OnStartSimplePopup(wxCommandEvent& event);
    void OnStartComplexPopup(wxCommandEvent& event);

private:
    DECLARE_EVENT_TABLE()
};

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
    Minimal_StartComplexPopup,
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------


IMPLEMENT_APP(MyApp)

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // create the main application window
    MyFrame *frame = new MyFrame(_T("Popup wxWidgets App"));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

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
    EVT_BUTTON(Minimal_StartComplexPopup, MyFrame::OnStartComplexPopup)
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

    new wxButton( this, Minimal_StartSimplePopup, wxT("Show simple popup"), wxPoint(20,20) );
    new wxButton( this, Minimal_StartComplexPopup, wxT("Show complex popup"), wxPoint(20,120) );


#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWidgets!"));
#endif // wxUSE_STATUSBAR
}

// event handlers

void MyFrame::OnStartSimplePopup(wxCommandEvent& event)
{
    SimpleTransientPopup* popup = new SimpleTransientPopup( this );
    wxWindow *btn = (wxWindow*) event.GetEventObject();
    wxPoint pos = btn->ClientToScreen( wxPoint(0,0) );
    wxSize sz = btn->GetSize();
    popup->Position( pos, sz );
    popup->Popup();
}

void MyFrame::OnStartComplexPopup(wxCommandEvent& WXUNUSED(event))
{
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
    EVT_BUTTON(Minimal_StartComplexPopup, MyDialog::OnStartComplexPopup)
END_EVENT_TABLE()

MyDialog::MyDialog(const wxString& title)
       : wxDialog(NULL, wxID_ANY, title, wxPoint(50,50), wxSize(400,300))
{

    new wxButton( this, Minimal_StartSimplePopup, wxT("Show simple popup"), wxPoint(20,20) );
    new wxButton( this, Minimal_StartComplexPopup, wxT("Show complex popup"), wxPoint(20,80) );

    new wxButton( this, wxID_OK, wxT("OK"), wxPoint(20,200) );
}

void MyDialog::OnStartSimplePopup(wxCommandEvent& event)
{
    SimpleTransientPopup* popup = new SimpleTransientPopup( this );
    wxWindow *btn = (wxWindow*) event.GetEventObject();
    wxPoint pos = btn->ClientToScreen( wxPoint(0,0) );
    wxSize sz = btn->GetSize();
    popup->Position( pos, sz );
    popup->Popup();
}

void MyDialog::OnStartComplexPopup(wxCommandEvent& WXUNUSED(event))
{
}

