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
    #include "wx/listbox.h"
#endif

#include "wx/txtstrm.h"

#include "wx/process.h"

#ifdef __WINDOWS__
    #include "wx/dde.h"
#endif // __WINDOWS__

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

// Define an array of process pointers used by MyFrame
class MyPipedProcess;
WX_DEFINE_ARRAY(MyPipedProcess *, MyProcessesArray);

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);

    void OnClear(wxCommandEvent& event);

    void OnSyncExec(wxCommandEvent& event);
    void OnAsyncExec(wxCommandEvent& event);
    void OnShell(wxCommandEvent& event);
    void OnExecWithRedirect(wxCommandEvent& event);
    void OnDDEExec(wxCommandEvent& event);

    void OnAbout(wxCommandEvent& event);

    // polling output of async processes
    void OnIdle(wxIdleEvent& event);

    // for MyPipedProcess
    void OnProcessTerminated(MyPipedProcess *process);
    wxListBox *GetLogListBox() const { return m_lbox; }

private:
    void ShowOutput(const wxString& cmd,
                    const wxArrayString& output,
                    const wxString& title);

    wxString m_cmdLast;

    wxListBox *m_lbox;

    MyProcessesArray m_running;

    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// This is the handler for process termination events
class MyProcess : public wxProcess
{
public:
    MyProcess(MyFrame *parent, const wxString& cmd)
        : wxProcess(parent), m_cmd(cmd)
    {
        m_parent = parent;
    }

    // instead of overriding this virtual function we might as well process the
    // event from it in the frame class - this might be more convenient in some
    // cases
    virtual void OnTerminate(int pid, int status);

protected:
    MyFrame *m_parent;
    wxString m_cmd;
};

// A specialization of MyProcess for redirecting the output
class MyPipedProcess : public MyProcess
{
public:
    MyPipedProcess(MyFrame *parent, const wxString& cmd)
        : MyProcess(parent, cmd)
        {
            Redirect();
        }

    virtual void OnTerminate(int pid, int status);

    bool HasInput();
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Exec_Quit = 100,
    Exec_ClearLog,
    Exec_SyncExec = 200,
    Exec_AsyncExec,
    Exec_Shell,
    Exec_DDEExec,
    Exec_Redirect,
    Exec_About = 300
};

static const wxChar *DIALOG_TITLE = _T("Exec sample");

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Exec_Quit,  MyFrame::OnQuit)
    EVT_MENU(Exec_ClearLog,  MyFrame::OnClear)

    EVT_MENU(Exec_SyncExec, MyFrame::OnSyncExec)
    EVT_MENU(Exec_AsyncExec, MyFrame::OnAsyncExec)
    EVT_MENU(Exec_Shell, MyFrame::OnShell)
    EVT_MENU(Exec_Redirect, MyFrame::OnExecWithRedirect)
    EVT_MENU(Exec_DDEExec, MyFrame::OnDDEExec)

    EVT_MENU(Exec_About, MyFrame::OnAbout)

    EVT_IDLE(MyFrame::OnIdle)
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

    // create a menu bar
    wxMenu *menuFile = new wxMenu(_T(""), wxMENU_TEAROFF);
    menuFile->Append(Exec_ClearLog, _T("&Clear log\tCtrl-C"),
                     _T("Clear the log window"));
    menuFile->AppendSeparator();
    menuFile->Append(Exec_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    wxMenu *execMenu = new wxMenu;
    execMenu->Append(Exec_SyncExec, _T("Sync &execution...\tCtrl-E"),
                     _T("Launch a program and return when it terminates"));
    execMenu->Append(Exec_AsyncExec, _T("&Async execution...\tCtrl-A"),
                     _T("Launch a program and return immediately"));
    execMenu->Append(Exec_Shell, _T("Execute &shell command...\tCtrl-S"),
                     _T("Launch a shell and execute a command in it"));
    execMenu->Append(Exec_Redirect, _T("Capture command &output...\tCtrl-O"),
                     _T("Launch a program and capture its output"));

#ifdef __WINDOWS__
    execMenu->AppendSeparator();
    execMenu->Append(Exec_DDEExec, _T("Execute command via &DDE...\tCtrl-D"));
#endif

    wxMenu *helpMenu = new wxMenu(_T(""), wxMENU_TEAROFF);
    helpMenu->Append(Exec_About, _T("&About...\tF1"), _T("Show about dialog"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(execMenu, _T("&Exec"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    // create the listbox in which we will show misc messages as they come
    m_lbox = new wxListBox(this, -1);

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar();
    SetStatusText(_T("Welcome to wxWindows exec sample!"));
#endif // wxUSE_STATUSBAR
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnClear(wxCommandEvent& WXUNUSED(event))
{
    m_lbox->Clear();
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("Exec sample\n© 2000 Vadim Zeitlin"),
                 _T("About Exec"), wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnSyncExec(wxCommandEvent& WXUNUSED(event))
{
    wxString cmd = wxGetTextFromUser(_T("Enter the command: "),
                                     DIALOG_TITLE,
                                     m_cmdLast);

    if ( !cmd )
        return;

    wxLogStatus(_T("'%s' is running please wait..."), cmd.c_str());

    int code = wxExecute(cmd, TRUE /* sync */);

    wxLogStatus(_T("Process '%s' terminated with exit code %d."),
                cmd.c_str(), code);
    m_cmdLast = cmd;
}

void MyFrame::OnAsyncExec(wxCommandEvent& WXUNUSED(event))
{
    wxString cmd = wxGetTextFromUser(_T("Enter the command: "),
                                     DIALOG_TITLE,
                                     m_cmdLast);

    if ( !cmd )
        return;

    wxProcess *process = new MyProcess(this, cmd);
    long pid = wxExecute(cmd, FALSE /* async */, process);
    if ( !pid )
    {
        wxLogError(_T("Execution of '%s' failed."), cmd.c_str());

        delete process;
    }
    else
    {
        wxLogStatus(_T("Process %ld (%s) launched."), pid, cmd.c_str());

        m_cmdLast = cmd;
    }
}

void MyFrame::OnShell(wxCommandEvent& WXUNUSED(event))
{
    wxString cmd = wxGetTextFromUser(_T("Enter the command: "),
                                     DIALOG_TITLE,
                                     m_cmdLast);

    if ( !cmd )
        return;

    int code = wxShell(cmd);
    wxLogStatus(_T("Shell command '%s' terminated with exit code %d."),
                cmd.c_str(), code);
    m_cmdLast = cmd;
}

void MyFrame::OnExecWithRedirect(wxCommandEvent& WXUNUSED(event))
{
    wxString cmd = wxGetTextFromUser(_T("Enter the command: "),
                                     DIALOG_TITLE,
                                     m_cmdLast);

    if ( !cmd )
        return;

    bool sync;
    switch ( wxMessageBox(_T("Execute it synchronously?"),
                          _T("Exec question"),
                          wxYES_NO | wxCANCEL | wxICON_QUESTION, this) )
    {
        case wxYES:
            sync = TRUE;
            break;

        case wxNO:
            sync = FALSE;
            break;

        default:
            return;
    }

    if ( sync )
    {
        wxArrayString output, errors;
        int code = wxExecute(cmd, output);
        wxLogStatus(_T("command '%s' terminated with exit code %d."),
                    cmd.c_str(), code);

        if ( code != -1 )
        {
            ShowOutput(cmd, output, _T("Output"));
            ShowOutput(cmd, errors, _T("Errors"));
        }
    }
    else // async exec
    {
        MyPipedProcess *process = new MyPipedProcess(this, cmd);
        if ( !wxExecute(cmd, FALSE /* async */, process) )
        {
            wxLogError(_T("Execution of '%s' failed."), cmd.c_str());

            delete process;
        }
        else
        {
            m_running.Add(process);
        }
    }

    m_cmdLast = cmd;
}

void MyFrame::OnDDEExec(wxCommandEvent& WXUNUSED(event))
{
#ifdef __WINDOWS__
    wxString server = wxGetTextFromUser(_T("Server to connect to:"),
                                        DIALOG_TITLE, _T("IExplore"));
    if ( !server )
        return;

    wxString topic = wxGetTextFromUser(_T("DDE topic:"),
                                       DIALOG_TITLE, _T("WWW_OpenURL"));
    if ( !topic )
        return;

    wxString cmd = wxGetTextFromUser(_T("DDE command:"),
                                     DIALOG_TITLE,
                                     _T("\"file:F:\\wxWindows\\samples\\"
                                        "image\\horse.gif\",,-1,,,,,"));
    if ( !cmd )
        return;

    wxDDEClient client;
    wxConnectionBase *conn = client.MakeConnection("", server, topic);
    if ( !conn )
    {
        wxLogError(_T("Failed to connect to the DDE server '%s'."),
                   server.c_str());
    }
    else
    {
        if ( !conn->Execute(cmd) )
        {
            wxLogError(_T("Failed to execute command '%s' via DDE."),
                       cmd.c_str());
        }
        else
        {
            wxLogStatus(_T("Successfully executed DDE command"));
        }
    }
#endif // __WINDOWS__
}

// input polling
void MyFrame::OnIdle(wxIdleEvent& event)
{
    size_t count = m_running.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        if ( m_running[n]->HasInput() )
        {
            event.RequestMore();
        }
    }
}

void MyFrame::OnProcessTerminated(MyPipedProcess *process)
{
    m_running.Remove(process);
}


void MyFrame::ShowOutput(const wxString& cmd,
                         const wxArrayString& output,
                         const wxString& title)
{
    size_t count = output.GetCount();
    if ( !count )
        return;

    m_lbox->Append(wxString::Format(_T("--- %s of '%s' ---"),
                                    title.c_str(), cmd.c_str()));

    for ( size_t n = 0; n < count; n++ )
    {
        m_lbox->Append(output[n]);
    }

    m_lbox->Append(_T("--- End of output ---"));
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

// ----------------------------------------------------------------------------
// MyPipedProcess
// ----------------------------------------------------------------------------

bool MyPipedProcess::HasInput()
{
    bool hasInput = FALSE;

    wxInputStream& is = *GetInputStream();
    if ( !is.Eof() )
    {
        wxTextInputStream tis(is);

        // this assumes that the output is always line buffered
        wxString msg;
        msg << m_cmd << _T(" (stdout): ") << tis.ReadLine();

        m_parent->GetLogListBox()->Append(msg);

        hasInput = TRUE;
    }

    wxInputStream& es = *GetErrorStream();
    if ( !es.Eof() )
    {
        wxTextInputStream tis(es);

        // this assumes that the output is always line buffered
        wxString msg;
        msg << m_cmd << _T(" (stderr): ") << tis.ReadLine();

        m_parent->GetLogListBox()->Append(msg);

        hasInput = TRUE;
    }

    return hasInput;
}

void MyPipedProcess::OnTerminate(int pid, int status)
{
    // show the rest of the output
    while ( HasInput() )
        ;

    m_parent->OnProcessTerminated(this);

    MyProcess::OnTerminate(pid, status);
}
