/////////////////////////////////////////////////////////////////////////////
// Name:        exec.cpp
// Purpose:     exec sample demonstrates wxExecute and related functions
// Author:      Vadim Zeitlin
// Modified by:
// Created:     15.01.00
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "exec.cpp"
    #pragma interface "exec.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/utils.h"
    #include "wx/menu.h"
    #include "wx/msgdlg.h"
    #include "wx/textdlg.h"
#endif

#include "wx/process.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);

    void OnSyncExec(wxCommandEvent& event);
    void OnAsyncExec(wxCommandEvent& event);
    void OnShell(wxCommandEvent& event);

    void OnAbout(wxCommandEvent& event);

private:
    wxString m_cmdLast;

    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// This is the handler for process termination events
class MyProcess : public wxProcess
{
public:
    MyProcess(wxFrame *parent, const wxString& cmd)
        : wxProcess(parent), m_cmd(cmd)
    {
        m_parent = parent;
    }

    // instead of overriding this virtual function we might as well process the
    // event from it in the frame class - this might be more convenient in some
    // cases
    virtual void OnTerminate(int pid, int status);

private:
    wxFrame *m_parent;
    wxString m_cmd;
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Exec_Quit = 100,
    Exec_SyncExec = 200,
    Exec_AsyncExec,
    Exec_Shell,
    Exec_About = 300
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Exec_Quit,  MyFrame::OnQuit)

    EVT_MENU(Exec_SyncExec, MyFrame::OnSyncExec)
    EVT_MENU(Exec_AsyncExec, MyFrame::OnAsyncExec)
    EVT_MENU(Exec_Shell, MyFrame::OnShell)

    EVT_MENU(Exec_About, MyFrame::OnAbout)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // Create the main application window
    MyFrame *frame = new MyFrame(_T("Exec wxWindows sample"),
                                 wxDefaultPosition, wxSize(500, 140));

    // Show it and tell the application that it's our main window
    frame->Show(TRUE);
    SetTopWindow(frame);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
    return TRUE;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
#ifdef __WXMAC__
    // we need this in order to allow the about menu relocation, since ABOUT is
    // not the default id of the about menu
    wxApp::s_macAboutMenuItemId = Exec_About;
#endif

    // set the frame icon
    SetIcon(wxICON(mondrian));

    // create a menu bar
    wxMenu *menuFile = new wxMenu(_T(""), wxMENU_TEAROFF);
    menuFile->Append(Exec_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    wxMenu *execMenu = new wxMenu;
    execMenu->Append(Exec_SyncExec, _T("Sync &execution...\tCtrl-E"),
                     _T("Launch a program and return when it terminates"));
    execMenu->Append(Exec_AsyncExec, _T("&Async execution...\tCtrl-A"),
                     _T("Launch a program and return immediately"));
    execMenu->Append(Exec_Shell, _T("Execute &shell command...\tCtrl-S"),
                     _T("Launch a shell and execute a command in it"));

    wxMenu *helpMenu = new wxMenu(_T(""), wxMENU_TEAROFF);
    helpMenu->Append(Exec_About, _T("&About...\tF1"), _T("Show about dialog"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(execMenu, _T("&Exec"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWindows!"));
#endif // wxUSE_STATUSBAR
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("Exec sample\n© 2000 Vadim Zeitlin"),
                 _T("About Exec"), wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnSyncExec(wxCommandEvent& WXUNUSED(event))
{
    wxString cmd = wxGetTextFromUser(_T("Enter the command: "),
                                     _T("Exec sample"),
                                     m_cmdLast);

    if ( !cmd )
        return;

    int code = wxExecute(cmd, TRUE /* sync */);
    wxLogStatus(_T("Process '%s' terminated with exit code %d."),
                cmd.c_str(), code);
    m_cmdLast = cmd;
}

void MyFrame::OnAsyncExec(wxCommandEvent& WXUNUSED(event))
{
    wxString cmd = wxGetTextFromUser(_T("Enter the command: "),
                                     _T("Exec sample"),
                                     m_cmdLast);

    if ( !cmd )
        return;

    wxProcess *process = new MyProcess(this, cmd);
    if ( !wxExecute(cmd, FALSE /* async */, process) )
    {
        wxLogError(_T("Execution of '%s' failed."), cmd.c_str());

        delete process;
    }
    else
    {
        m_cmdLast = cmd;
    }
}

void MyFrame::OnShell(wxCommandEvent& WXUNUSED(event))
{
    wxString cmd = wxGetTextFromUser(_T("Enter the command: "),
                                     _T("Exec sample"),
                                     m_cmdLast);

    if ( !cmd )
        return;

    int code = wxShell(cmd);
    wxLogStatus(_T("Shell command '%s' terminated with exit code %d."),
                cmd.c_str(), code);
    m_cmdLast = cmd;
}

// ----------------------------------------------------------------------------
// MyProcess
// ----------------------------------------------------------------------------

void MyProcess::OnTerminate(int pid, int status)
{
    wxLogStatus(m_parent, _T("Process %u ('%s') terminated with exit code %d."),
                pid, m_cmd.c_str(), status);

    // we're not needed any more
    delete this;
}
