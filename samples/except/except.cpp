/////////////////////////////////////////////////////////////////////////////
// Name:        samples/except/except.cpp
// Purpose:     shows how C++ exceptions can be used in wxWidgets
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2003-09-17
// RCS-ID:      $Id$
// Copyright:   (c) 2003-2005 Vadim Zeitlin
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

#if !wxUSE_EXCEPTIONS
    #error "This sample only works with wxUSE_EXCEPTIONS == 1"
#endif // !wxUSE_EXCEPTIONS

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/log.h"

    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/dialog.h"
    #include "wx/menu.h"

    #include "wx/button.h"
    #include "wx/sizer.h"

    #include "wx/utils.h"
    #include "wx/msgdlg.h"
    #include "wx/icon.h"
#endif

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources)
#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // program startup
    virtual bool OnInit();

    // 2nd-level exception handling: we get all the exceptions occuring in any
    // event handler here
    virtual void HandleEvent(wxEvtHandler *handler,
                             wxEventFunction func,
                             wxEvent& event) const;

    // 3rd, and final, level exception handling: whenever an unhandled
    // exception is caught, this function is called
    virtual void OnUnhandledException();
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
            long style = wxDEFAULT_FRAME_STYLE);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnDialog(wxCommandEvent& event);
    void OnThrowString(wxCommandEvent& event);

    // 1st-level exception handling: we overload ProcessEvent() to be able to
    // catch exceptions which occur in MyFrame methods here
    virtual bool ProcessEvent(wxEvent& event);

private:
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

// A simple dialog which has only some buttons to throw exceptions
class MyDialog : public wxDialog
{
public:
    MyDialog(wxFrame *parent);

    // event handlers
    void OnThrowInt(wxCommandEvent& event);
    void OnThrowObject(wxCommandEvent& event);
    void OnCrash(wxCommandEvent& event);

private:
    DECLARE_EVENT_TABLE()
};

// A trivial exception class
class MyException
{
public:
    MyException(const wxString& msg) : m_msg(msg) { }

    const wxChar *what() const { return m_msg.c_str(); }

private:
    wxString m_msg;
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // control ids
    Except_ThrowInt = 100,
    Except_ThrowObject,
    Except_Crash,

    // menu items
    Except_ThrowString = 200,
    Except_Dialog,

    Except_Quit = wxID_EXIT,
    Except_About = wxID_ABOUT
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Except_Quit,  MyFrame::OnQuit)
    EVT_MENU(Except_About, MyFrame::OnAbout)
    EVT_MENU(Except_Dialog, MyFrame::OnDialog)
    EVT_MENU(Except_ThrowString, MyFrame::OnThrowString)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyDialog, wxDialog)
    EVT_BUTTON(Except_ThrowInt, MyDialog::OnThrowInt)
    EVT_BUTTON(Except_ThrowObject, MyDialog::OnThrowObject)
    EVT_BUTTON(Except_Crash, MyDialog::OnCrash)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// MyApp implementation
// ============================================================================

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // create the main application window
    MyFrame *frame = new MyFrame(_T("Except wxWidgets App"),
                                 wxPoint(50, 50), wxSize(450, 340));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

void
MyApp::HandleEvent(wxEvtHandler *handler,
                   wxEventFunction func,
                   wxEvent& event) const
{
    try
    {
        wxApp::HandleEvent(handler, func, event);
    }
    catch ( int i )
    {
        wxLogError(_T("Caught an int %d in MyApp."), i);
    }
}

void MyApp::OnUnhandledException()
{
    wxMessageBox(_T("Unhandled exception caught, program will terminate."),
                 _T("wxExcept Sample"), wxOK | wxICON_ERROR);
}

// ============================================================================
// MyFrame implementation
// ============================================================================

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
       : wxFrame(NULL, wxID_ANY, title, pos, size, style)
{
    // set the frame icon
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(Except_Dialog, _T("Show &dialog\tCtrl-D"));
    menuFile->Append(Except_ThrowString, _T("Throw a &string\tCtrl-S"));
    menuFile->AppendSeparator();
    menuFile->Append(Except_Quit, _T("E&xit\tCtrl-Q"), _T("Quit this program"));

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Except_About, _T("&About...\tF1"), _T("Show about dialog"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR && !defined(__WXWINCE__)
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWidgets!"));
#endif // wxUSE_STATUSBAR
}

bool MyFrame::ProcessEvent(wxEvent& event)
{
    try
    {
        return wxFrame::ProcessEvent(event);
    }
    catch ( const wxChar *msg )
    {
        wxLogMessage(_T("Caught a string \"%s\" in MyFrame"), msg);

        return true;
    }
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnDialog(wxCommandEvent& WXUNUSED(event))
{
    try
    {
        MyDialog dlg(this);

        dlg.ShowModal();
    }
    catch ( ... )
    {
        Destroy();
        throw;
    }
}

void MyFrame::OnThrowString(wxCommandEvent& WXUNUSED(event))
{
    throw _T("some string");
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("This is the About dialog of the except sample.\n")
                _T("Welcome to %s"), wxVERSION_STRING);

    wxMessageBox(msg, _T("About Except"), wxOK | wxICON_INFORMATION, this);
}

// ============================================================================
// MyDialog implementation
// ============================================================================

MyDialog::MyDialog(wxFrame *parent)
        : wxDialog(parent, wxID_ANY, wxString(_T("Throw exception dialog")))
{
    wxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

    sizerTop->Add(new wxButton(this, Except_ThrowInt, _T("Throw &int")),
                  0, wxCENTRE | wxALL, 5);
    sizerTop->Add(new wxButton(this, Except_ThrowObject, _T("Throw &object")),
                  0, wxCENTRE | wxALL, 5);
    sizerTop->Add(new wxButton(this, Except_Crash, _T("&Crash")),
                  0, wxCENTRE | wxALL, 5);
    sizerTop->Add(new wxButton(this, wxID_CANCEL, _T("&Cancel")),
                  0, wxCENTRE | wxALL, 5);

    SetSizer(sizerTop);
    sizerTop->Fit(this);
}

void MyDialog::OnThrowInt(wxCommandEvent& WXUNUSED(event))
{
    throw 17;
}

void MyDialog::OnThrowObject(wxCommandEvent& WXUNUSED(event))
{
    throw MyException(_T("Exception thrown from the dialog"));
}

void MyDialog::OnCrash(wxCommandEvent& WXUNUSED(event))
{
    char *p = 0;
    strcpy(p, "Let's crash");
}

