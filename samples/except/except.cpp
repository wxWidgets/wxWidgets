/////////////////////////////////////////////////////////////////////////////
// Name:        samples/except/except.cpp
// Purpose:     shows how C++ exceptions can be used in wxWidgets
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2003-09-17
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

    #include "wx/thread.h"
#endif

#include "wx/uiaction.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows it is in resources)
#ifndef wxHAS_IMAGES_IN_RESOURCES
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
    MyApp()
    {
        m_numStoredExceptions = 0;
    }

    // override base class virtuals
    // ----------------------------

    // program startup
    virtual bool OnInit() wxOVERRIDE;

    // 2nd-level exception handling: we get all the exceptions occurring in any
    // event handler here
    virtual bool OnExceptionInMainLoop() wxOVERRIDE;

    // 2nd-level exception handling helpers: if we can't deal with the
    // exception immediately, we may also store it and rethrow it later, when
    // we're back from events processing loop.
    //
    // Notice that overriding these methods is not necessary when using C++11
    // as they have a perfectly serviceable implementation inside the library
    // itself in this case.
    virtual bool StoreCurrentException() wxOVERRIDE;
    virtual void RethrowStoredException() wxOVERRIDE;

    // 3rd, and final, level exception handling: whenever an unhandled
    // exception is caught, this function is called
    virtual void OnUnhandledException() wxOVERRIDE;

    // and now for something different: this function is called in case of a
    // crash (e.g. dereferencing null pointer, division by 0, ...)
    virtual void OnFatalException() wxOVERRIDE;

    // you can override this function to do something different (e.g. log the
    // assert to file) whenever an assertion fails
    virtual void OnAssertFailure(const wxChar *file,
                                 int line,
                                 const wxChar *func,
                                 const wxChar *cond,
                                 const wxChar *msg) wxOVERRIDE;

private:
    // This stores the number of times StoreCurrentException() was called,
    // typically at most 1.
    int m_numStoredExceptions;
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
    void OnThrowFromYield(wxCommandEvent& event);

    void OnCrash(wxCommandEvent& event);
    void OnTrap(wxCommandEvent& event);
#if wxUSE_ON_FATAL_EXCEPTION
    void OnHandleCrash(wxCommandEvent& event);
#endif

protected:

    // 1st-level exception handling: we overload ProcessEvent() to be able to
    // catch exceptions which occur in MyFrame methods here
    virtual bool ProcessEvent(wxEvent& event) wxOVERRIDE;

    // provoke assert in main or worker thread
    //
    // this is used to show how an assert failure message box looks like
    void OnShowAssert(wxCommandEvent& event);
#if wxUSE_THREADS
    void OnShowAssertInThread(wxCommandEvent& event);
#endif // wxUSE_THREADS

private:
    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
};

// A simple dialog which has only some buttons to throw exceptions
class MyDialog : public wxDialog
{
public:
    MyDialog(wxFrame *parent);

    // event handlers
    void OnThrowInt(wxCommandEvent& event);
    void OnThrowObject(wxCommandEvent& event);
    void OnThrowUnhandled(wxCommandEvent& event);
    void OnCrash(wxCommandEvent& event);

private:
    wxDECLARE_EVENT_TABLE();
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
//
// It is not handled by OnExceptionInMainLoop() but is still handled by
// explicit try/catch blocks so it's not quite completely unhandled, actually.
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
    Except_ThrowFromYield,
    Except_Crash,
    Except_Trap,
#if wxUSE_ON_FATAL_EXCEPTION
    Except_HandleCrash,
#endif // wxUSE_ON_FATAL_EXCEPTION
    Except_ShowAssert,
#if wxUSE_THREADS
    Except_ShowAssertInThread,
#endif // wxUSE_THREADS
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
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Except_Quit,  MyFrame::OnQuit)
    EVT_MENU(Except_About, MyFrame::OnAbout)
    EVT_MENU(Except_Dialog, MyFrame::OnDialog)
    EVT_MENU(Except_ThrowInt, MyFrame::OnThrowInt)
    EVT_MENU(Except_ThrowString, MyFrame::OnThrowString)
    EVT_MENU(Except_ThrowObject, MyFrame::OnThrowObject)
    EVT_MENU(Except_ThrowUnhandled, MyFrame::OnThrowUnhandled)
    EVT_MENU(Except_ThrowFromYield, MyFrame::OnThrowFromYield)
    EVT_MENU(Except_Crash, MyFrame::OnCrash)
    EVT_MENU(Except_Trap, MyFrame::OnTrap)
#if wxUSE_ON_FATAL_EXCEPTION
    EVT_MENU(Except_HandleCrash, MyFrame::OnHandleCrash)
#endif // wxUSE_ON_FATAL_EXCEPTION
    EVT_MENU(Except_ShowAssert, MyFrame::OnShowAssert)
#if wxUSE_THREADS
    EVT_MENU(Except_ShowAssertInThread, MyFrame::OnShowAssertInThread)
#endif // wxUSE_THREADS
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(MyDialog, wxDialog)
    EVT_BUTTON(Except_ThrowInt, MyDialog::OnThrowInt)
    EVT_BUTTON(Except_ThrowObject, MyDialog::OnThrowObject)
    EVT_BUTTON(Except_ThrowUnhandled, MyDialog::OnThrowUnhandled)
    EVT_BUTTON(Except_Crash, MyDialog::OnCrash)
wxEND_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
wxIMPLEMENT_APP(MyApp);

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
        wxLogWarning("Caught an int %d in MyApp.", i);
    }
    catch ( MyException& e )
    {
        wxLogWarning("Caught MyException(%s) in MyApp.", e.what());
    }
    catch ( ... )
    {
        throw;
    }

    return true;
}

bool MyApp::StoreCurrentException()
{
    try
    {
        throw;
    }
    catch ( UnhandledException& )
    {
        if ( m_numStoredExceptions )
        {
            wxLogWarning("Unexpectedly many exceptions to store.");
        }

        m_numStoredExceptions++;

        return true;
    }
    catch ( ... )
    {
        // Don't know how to store other exceptions.
    }

    return false;
}

void MyApp::RethrowStoredException()
{
    if ( m_numStoredExceptions )
    {
        m_numStoredExceptions = 0;

        throw UnhandledException();
    }
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
        wxMessageBox("Unhandled exception caught, program will terminate.",
                     "wxExcept Sample", wxOK | wxICON_ERROR);
    }
}

void MyApp::OnFatalException()
{
    wxMessageBox("Program has crashed and will terminate.",
                 "wxExcept Sample", wxOK | wxICON_ERROR);
}

void MyApp::OnAssertFailure(const wxChar *file,
                            int line,
                            const wxChar *func,
                            const wxChar *cond,
                            const wxChar *msg)
{
    // take care to not show the message box from a worker thread, this doesn't
    // work as it doesn't have any event loop
    if ( !wxIsMainThread() ||
            wxMessageBox
            (
                wxString::Format("An assert failed in %s().", func) +
                "\n"
                "Do you want to call the default assert handler?",
                "wxExcept Sample",
                wxYES_NO | wxICON_QUESTION
            ) == wxYES )
    {
        wxApp::OnAssertFailure(file, line, func, cond, msg);
    }
}

// ============================================================================
// MyFrame implementation
// ============================================================================

// frame constructor
MyFrame::MyFrame()
       : wxFrame(NULL, wxID_ANY, "Except wxWidgets App",
                 wxPoint(50, 50), wxSize(450, 340))
{
    // set the frame icon
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(Except_Dialog, "Show &dialog\tCtrl-D");
    menuFile->AppendSeparator();
    menuFile->Append(Except_ThrowInt, "Throw an &int\tCtrl-I");
    menuFile->Append(Except_ThrowString, "Throw a &string\tCtrl-S");
    menuFile->Append(Except_ThrowObject, "Throw an &object\tCtrl-O");
    menuFile->Append(Except_ThrowUnhandled,
                        "Throw &unhandled exception\tCtrl-U");
    menuFile->Append(Except_ThrowFromYield,
                        "Throw from wx&Yield()\tCtrl-Y");
    menuFile->Append(Except_Crash, "&Crash\tCtrl-C");
    menuFile->Append(Except_Trap, "&Trap\tCtrl-T",
                     "Break into the debugger (if one is running)");
    menuFile->AppendSeparator();
#if wxUSE_ON_FATAL_EXCEPTION
    menuFile->AppendCheckItem(Except_HandleCrash, "&Handle crashes\tCtrl-H");
    menuFile->AppendSeparator();
#endif // wxUSE_ON_FATAL_EXCEPTION
    menuFile->Append(Except_ShowAssert, "Provoke &assert failure\tCtrl-A");
#if wxUSE_THREADS
    menuFile->Append(Except_ShowAssertInThread,
                     "Assert failure in &thread\tShift-Ctrl-A");
#endif // wxUSE_THREADS
    menuFile->AppendSeparator();
    menuFile->Append(Except_Quit, "E&xit\tCtrl-Q", "Quit this program");

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Except_About, "&About\tF1", "Show about dialog");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, "&File");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText("Welcome to wxWidgets!");
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
        wxLogMessage("Caught a string \"%s\" in MyFrame", msg);

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
    catch ( UnhandledException& )
    {
        wxLogMessage("Caught unhandled exception inside the dialog.");
    }
    catch ( ... )
    {
        wxLogWarning("An exception in MyDialog");

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
    throw "string thrown from MyFrame";
}

void MyFrame::OnThrowObject(wxCommandEvent& WXUNUSED(event))
{
    throw MyException("Exception thrown from MyFrame");
}

void MyFrame::OnThrowUnhandled(wxCommandEvent& WXUNUSED(event))
{
    throw UnhandledException();
}

void MyFrame::OnThrowFromYield(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_UIACTIONSIMULATOR
    // Simulate selecting the "Throw unhandled" menu item, its handler will be
    // executed from inside wxYield() and as the exception is not handled by
    // our OnExceptionInMainLoop(), will call StoreCurrentException() and, when
    // wxYield() regains control, RethrowStoredException().
    //
    // Notice that if we didn't override these methods we wouldn't be able to
    // catch this exception here!
    try
    {
        wxUIActionSimulator sim;
        sim.Char('U', wxMOD_CONTROL);
        wxYield();
    }
    catch ( UnhandledException& )
    {
        wxLogMessage("Caught unhandled exception inside wxYield().");
    }
#else // !wxUSE_UIACTIONSIMULATOR
    wxLogError("Can't trigger an exception inside wxYield() "
               "without wxUIActionSimulator, please rebuild "
               "with wxUSE_UIACTIONSIMULATOR=1.");
#endif // wxUSE_UIACTIONSIMULATOR/!wxUSE_UIACTIONSIMULATOR
}

void MyFrame::OnCrash(wxCommandEvent& WXUNUSED(event))
{
    DoCrash();
}

void MyFrame::OnTrap(wxCommandEvent& WXUNUSED(event))
{
    wxTrap();
}

#if wxUSE_ON_FATAL_EXCEPTION

void MyFrame::OnHandleCrash(wxCommandEvent& event)
{
    wxHandleFatalExceptions(event.IsChecked());
}

#endif // wxUSE_ON_FATAL_EXCEPTION

void MyFrame::OnShowAssert(wxCommandEvent& WXUNUSED(event))
{
    // provoke an assert from wxArrayString
    wxArrayString arr;
    arr[0];
}

#if wxUSE_THREADS

void MyFrame::OnShowAssertInThread(wxCommandEvent& WXUNUSED(event))
{
    class AssertThread : public wxThread
    {
    public:
        AssertThread()
            : wxThread(wxTHREAD_JOINABLE)
        {
        }

    protected:
        virtual void *Entry() wxOVERRIDE
        {
            wxFAIL_MSG("Test assert in another thread.");

            return 0;
        }
    };

    AssertThread thread;
    thread.Create();
    thread.Run();
    thread.Wait();
}

#endif // wxUSE_THREADS

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( "This is the About dialog of the except sample.\n"
                "Welcome to %s", wxVERSION_STRING);

    wxMessageBox(msg, "About Except", wxOK | wxICON_INFORMATION, this);
}

// ============================================================================
// MyDialog implementation
// ============================================================================

MyDialog::MyDialog(wxFrame *parent)
        : wxDialog(parent, wxID_ANY, wxString("Throw exception dialog"))
{
    wxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

    sizerTop->Add(new wxButton(this, Except_ThrowInt, "Throw &int"),
                  0, wxEXPAND | wxALL, 5);
    sizerTop->Add(new wxButton(this, Except_ThrowObject, "Throw &object"),
                  0, wxEXPAND | wxALL, 5);
    sizerTop->Add(new wxButton(this, Except_ThrowUnhandled, "Throw &unhandled"),
                  0, wxEXPAND | wxALL, 5);
    sizerTop->Add(new wxButton(this, Except_Crash, "&Crash"),
                  0, wxEXPAND | wxALL, 5);
    sizerTop->Add(new wxButton(this, wxID_CANCEL, "&Cancel"),
                  0, wxEXPAND | wxALL, 5);

    SetSizerAndFit(sizerTop);
}

void MyDialog::OnThrowInt(wxCommandEvent& WXUNUSED(event))
{
    throw 17;
}

void MyDialog::OnThrowObject(wxCommandEvent& WXUNUSED(event))
{
    throw MyException("Exception thrown from MyDialog");
}

void MyDialog::OnThrowUnhandled(wxCommandEvent& WXUNUSED(event))
{
    throw UnhandledException();
}

void MyDialog::OnCrash(wxCommandEvent& WXUNUSED(event))
{
    DoCrash();
}

