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
// private functions
// ----------------------------------------------------------------------------

static void DoCrash()
{
    char *p = 0;
    strcpy(p, "Let's crash");
}

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

    // 2nd-level exception handling: we get all the exceptions occurring in any
    // event handler here
    virtual bool OnExceptionInMainLoop();

    // 3rd, and final, level exception handling: whenever an unhandled
    // exception is caught, this function is called
    virtual void OnUnhandledException();

    // and now for something different: this function is called in case of a
    // crash (e.g. dereferencing null pointer, division by 0, ...)
    virtual void OnFatalException();

#ifdef __WXDEBUG__
    // in debug mode, you can override this function to do something different
    // (e.g. log the assert to file) whenever an assertion fails
    virtual void OnAssert(const wxChar *file,
                          int line,
                          const wxChar *cond,
                          const wxChar *msg);
#endif // __WXDEBUG__
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame();

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnDialog(wxCommandEvent& event);

    void OnThrowInt(wxCommandEvent& event);
    void OnThrowString(wxCommandEvent& event);
    void OnThrowObject(wxCommandEvent& event);
    void OnThrowUnhandled(wxCommandEvent& event);

    void OnCrash(wxCommandEvent& event);
#if wxUSE_ON_FATAL_EXCEPTION
    void OnHandleCrash(wxCommandEvent& event);
#endif

    // 1st-level exception handling: we overload ProcessEvent() to be able to
    // catch exceptions which occur in MyFrame methods here
    virtual bool ProcessEvent(wxEvent& event);

#ifdef __WXDEBUG__
    // show how an assert failure message box looks like
    void OnShowAssert(wxCommandEvent& event);
#endif // __WXDEBUG__

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

// Another exception class which just has to be different from anything else
class UnhandledException
{
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // control ids and menu items
    Except_ThrowInt = wxID_HIGHEST,
    Except_ThrowString,
    Except_ThrowObject,
    Except_ThrowUnhandled,
    Except_Crash,
#if wxUSE_ON_FATAL_EXCEPTION
    Except_HandleCrash,
#endif // wxUSE_ON_FATAL_EXCEPTION
#ifdef __WXDEBUG__
    Except_ShowAssert,
#endif // __WXDEBUG__
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
    EVT_MENU(Except_ThrowInt, MyFrame::OnThrowInt)
    EVT_MENU(Except_ThrowString, MyFrame::OnThrowString)
    EVT_MENU(Except_ThrowObject, MyFrame::OnThrowObject)
    EVT_MENU(Except_ThrowUnhandled, MyFrame::OnThrowUnhandled)
    EVT_MENU(Except_Crash, MyFrame::OnCrash)
#if wxUSE_ON_FATAL_EXCEPTION
    EVT_MENU(Except_HandleCrash, MyFrame::OnHandleCrash)
#endif // wxUSE_ON_FATAL_EXCEPTION
#ifdef __WXDEBUG__
    EVT_MENU(Except_ShowAssert, MyFrame::OnShowAssert)
#endif // __WXDEBUG__
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
    if ( !wxApp::OnInit() )
        return false;

    // create the main application window
    MyFrame *frame = new MyFrame();

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

bool MyApp::OnExceptionInMainLoop()
{
    try
    {
        throw;
    }
    catch ( int i )
    {
        wxLogWarning(_T("Caught an int %d in MyApp."), i);
    }
    catch ( MyException& e )
    {
        wxLogWarning(_T("Caught MyException(%s) in MyApp."), e.what());
    }
    catch ( ... )
    {
        throw;
    }

    return true;
}

void MyApp::OnUnhandledException()
{
    // this shows how we may let some exception propagate uncaught
    try
    {
        throw;
    }
    catch ( UnhandledException& )
    {
        throw;
    }
    catch ( ... )
    {
        wxMessageBox(_T("Unhandled exception caught, program will terminate."),
                     _T("wxExcept Sample"), wxOK | wxICON_ERROR);
    }
}

void MyApp::OnFatalException()
{
    wxMessageBox(_T("Program has crashed and will terminate."),
                 _T("wxExcept Sample"), wxOK | wxICON_ERROR);
}

#ifdef __WXDEBUG__

void MyApp::OnAssert(const wxChar *file,
                     int line,
                     const wxChar *cond,
                     const wxChar *msg)
{
    // we don't have anything special to do here
    wxApp::OnAssert(file, line, cond, msg);
}

#endif // __WXDEBUG__

// ============================================================================
// MyFrame implementation
// ============================================================================

// frame constructor
MyFrame::MyFrame()
       : wxFrame(NULL, wxID_ANY, _T("Except wxWidgets App"),
                 wxPoint(50, 50), wxSize(450, 340))
{
    // set the frame icon
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(Except_Dialog, _T("Show &dialog\tCtrl-D"));
    menuFile->AppendSeparator();
    menuFile->Append(Except_ThrowInt, _T("Throw an &int\tCtrl-I"));
    menuFile->Append(Except_ThrowString, _T("Throw a &string\tCtrl-S"));
    menuFile->Append(Except_ThrowObject, _T("Throw an &object\tCtrl-O"));
    menuFile->Append(Except_ThrowUnhandled,
                        _T("Throw &unhandled exception\tCtrl-U"));
    menuFile->Append(Except_Crash, _T("&Crash\tCtrl-C"));
    menuFile->AppendSeparator();
#if wxUSE_ON_FATAL_EXCEPTION
    menuFile->AppendCheckItem(Except_HandleCrash, _T("&Handle crashes\tCtrl-H"));
    menuFile->AppendSeparator();
#endif // wxUSE_ON_FATAL_EXCEPTION
#ifdef __WXDEBUG__
    menuFile->Append(Except_ShowAssert, _T("Provoke &assert failure\tCtrl-A"));
    menuFile->AppendSeparator();
#endif // __WXDEBUG__
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
        wxLogWarning(_T("An exception in MyDialog"));

        Destroy();
        throw;
    }
}

void MyFrame::OnThrowInt(wxCommandEvent& WXUNUSED(event))
{
    throw -17;
}

void MyFrame::OnThrowString(wxCommandEvent& WXUNUSED(event))
{
    throw _T("string thrown from MyFrame");
}

void MyFrame::OnThrowObject(wxCommandEvent& WXUNUSED(event))
{
    throw MyException(_T("Exception thrown from MyFrame"));
}

void MyFrame::OnThrowUnhandled(wxCommandEvent& WXUNUSED(event))
{
    throw UnhandledException();
}

void MyFrame::OnCrash(wxCommandEvent& WXUNUSED(event))
{
    DoCrash();
}

#if wxUSE_ON_FATAL_EXCEPTION

void MyFrame::OnHandleCrash(wxCommandEvent& event)
{
    wxHandleFatalExceptions(event.IsChecked());
}

#endif // wxUSE_ON_FATAL_EXCEPTION

#ifdef __WXDEBUG__

void MyFrame::OnShowAssert(wxCommandEvent& WXUNUSED(event))
{
    // provoke an assert from wxArrayString
    wxArrayString arr;
    arr[0];
}

#endif // __WXDEBUG__

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
    throw MyException(_T("Exception thrown from MyDialog"));
}

void MyDialog::OnCrash(wxCommandEvent& WXUNUSED(event))
{
    DoCrash();
}

