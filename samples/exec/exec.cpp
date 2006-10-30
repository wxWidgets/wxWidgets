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

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"
    #include "wx/frame.h"
    #include "wx/panel.h"

    #include "wx/timer.h"

    #include "wx/utils.h"
    #include "wx/menu.h"

    #include "wx/msgdlg.h"
    #include "wx/textdlg.h"
    #include "wx/filedlg.h"
    #include "wx/choicdlg.h"

    #include "wx/button.h"
    #include "wx/textctrl.h"
    #include "wx/listbox.h"

    #include "wx/sizer.h"
#endif

#include "wx/txtstrm.h"
#include "wx/numdlg.h"
#include "wx/ffile.h"

#include "wx/process.h"

#include "wx/mimetype.h"

#ifdef __WINDOWS__
    #include "wx/dde.h"
#endif // __WINDOWS__

// ----------------------------------------------------------------------------
// the usual application and main frame classes
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
WX_DEFINE_ARRAY_PTR(MyPipedProcess *, MyProcessesArray);

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);

    void OnKill(wxCommandEvent& event);

    void OnClear(wxCommandEvent& event);

    void OnSyncExec(wxCommandEvent& event);
    void OnAsyncExec(wxCommandEvent& event);
    void OnShell(wxCommandEvent& event);
    void OnExecWithRedirect(wxCommandEvent& event);
    void OnExecWithPipe(wxCommandEvent& event);

    void OnPOpen(wxCommandEvent& event);

    void OnFileExec(wxCommandEvent& event);

    void OnAbout(wxCommandEvent& event);

    // polling output of async processes
    void OnTimer(wxTimerEvent& event);
    void OnIdle(wxIdleEvent& event);

    // for MyPipedProcess
    void OnProcessTerminated(MyPipedProcess *process);
    wxListBox *GetLogListBox() const { return m_lbox; }

private:
    void ShowOutput(const wxString& cmd,
                    const wxArrayString& output,
                    const wxString& title);

    void DoAsyncExec(const wxString& cmd);

    void AddAsyncProcess(MyPipedProcess *process)
    {
        if ( m_running.IsEmpty() )
        {
            // we want to start getting the timer events to ensure that a
            // steady stream of idle events comes in -- otherwise we
            // wouldn't be able to poll the child process input
            m_timerIdleWakeUp.Start(100);
        }
        //else: the timer is already running

        m_running.Add(process);
    }

    void RemoveAsyncProcess(MyPipedProcess *process)
    {
        m_running.Remove(process);

        if ( m_running.IsEmpty() )
        {
            // we don't need to get idle events all the time any more
            m_timerIdleWakeUp.Stop();
        }
    }

    // the PID of the last process we launched asynchronously
    long m_pidLast;

    // last command we executed
    wxString m_cmdLast;

#ifdef __WINDOWS__
    void OnDDEExec(wxCommandEvent& event);
    void OnDDERequest(wxCommandEvent& event);

    bool GetDDEServer();

    // last params of a DDE transaction
    wxString m_server,
             m_topic,
             m_cmdDde;
#endif // __WINDOWS__

    wxListBox *m_lbox;

    MyProcessesArray m_running;

    // the idle event wake up timer
    wxTimer m_timerIdleWakeUp;

    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// MyPipeFrame: allows the user to communicate with the child process
// ----------------------------------------------------------------------------

class MyPipeFrame : public wxFrame
{
public:
    MyPipeFrame(wxFrame *parent,
                const wxString& cmd,
                wxProcess *process);

protected:
    void OnTextEnter(wxCommandEvent& WXUNUSED(event)) { DoSend(); }
    void OnBtnSend(wxCommandEvent& WXUNUSED(event)) { DoSend(); }
    void OnBtnSendFile(wxCommandEvent& WXUNUSED(event));
    void OnBtnGet(wxCommandEvent& WXUNUSED(event)) { DoGet(); }
    void OnBtnClose(wxCommandEvent& WXUNUSED(event)) { DoClose(); }

    void OnClose(wxCloseEvent& event);

    void OnProcessTerm(wxProcessEvent& event);

    void DoSend()
    {
        wxString s(m_textOut->GetValue());
        s += _T('\n');
        m_out.Write(s.c_str(), s.length());
        m_textOut->Clear();

        DoGet();
    }

    void DoGet();
    void DoClose();

private:
    void DoGetFromStream(wxTextCtrl *text, wxInputStream& in);
    void DisableInput();
    void DisableOutput();


    wxProcess *m_process;

    wxOutputStream &m_out;
    wxInputStream &m_in,
                  &m_err;

    wxTextCtrl *m_textOut,
               *m_textIn,
               *m_textErr;

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// wxProcess-derived classes
// ----------------------------------------------------------------------------

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

    virtual bool HasInput();
};

// A version of MyPipedProcess which also sends input to the stdin of the
// child process
class MyPipedProcess2 : public MyPipedProcess
{
public:
    MyPipedProcess2(MyFrame *parent, const wxString& cmd, const wxString& input)
        : MyPipedProcess(parent, cmd), m_input(input)
        {
        }

    virtual bool HasInput();

private:
    wxString m_input;
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Exec_Quit = 100,
    Exec_Kill,
    Exec_ClearLog,
    Exec_SyncExec = 200,
    Exec_AsyncExec,
    Exec_Shell,
    Exec_POpen,
    Exec_OpenFile,
    Exec_DDEExec,
    Exec_DDERequest,
    Exec_Redirect,
    Exec_Pipe,
    Exec_About = 300,

    // control ids
    Exec_Btn_Send = 1000,
    Exec_Btn_SendFile,
    Exec_Btn_Get,
    Exec_Btn_Close
};

static const wxChar *DIALOG_TITLE = _T("Exec sample");

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Exec_Quit,  MyFrame::OnQuit)
    EVT_MENU(Exec_Kill,  MyFrame::OnKill)
    EVT_MENU(Exec_ClearLog,  MyFrame::OnClear)

    EVT_MENU(Exec_SyncExec, MyFrame::OnSyncExec)
    EVT_MENU(Exec_AsyncExec, MyFrame::OnAsyncExec)
    EVT_MENU(Exec_Shell, MyFrame::OnShell)
    EVT_MENU(Exec_Redirect, MyFrame::OnExecWithRedirect)
    EVT_MENU(Exec_Pipe, MyFrame::OnExecWithPipe)

    EVT_MENU(Exec_POpen, MyFrame::OnPOpen)

    EVT_MENU(Exec_OpenFile, MyFrame::OnFileExec)

#ifdef __WINDOWS__
    EVT_MENU(Exec_DDEExec, MyFrame::OnDDEExec)
    EVT_MENU(Exec_DDERequest, MyFrame::OnDDERequest)
#endif // __WINDOWS__

    EVT_MENU(Exec_About, MyFrame::OnAbout)

    EVT_IDLE(MyFrame::OnIdle)

    EVT_TIMER(wxID_ANY, MyFrame::OnTimer)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyPipeFrame, wxFrame)
    EVT_BUTTON(Exec_Btn_Send, MyPipeFrame::OnBtnSend)
    EVT_BUTTON(Exec_Btn_SendFile, MyPipeFrame::OnBtnSendFile)
    EVT_BUTTON(Exec_Btn_Get, MyPipeFrame::OnBtnGet)
    EVT_BUTTON(Exec_Btn_Close, MyPipeFrame::OnBtnClose)

    EVT_TEXT_ENTER(wxID_ANY, MyPipeFrame::OnTextEnter)

    EVT_CLOSE(MyPipeFrame::OnClose)

    EVT_END_PROCESS(wxID_ANY, MyPipeFrame::OnProcessTerm)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
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
    MyFrame *frame = new MyFrame(_T("Exec wxWidgets sample"),
                                 wxDefaultPosition, wxSize(500, 140));

    // Show it and tell the application that it's our main window
    frame->Show(true);
    SetTopWindow(frame);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

#ifdef __VISUALC__
#pragma warning(disable: 4355) // this used in base member initializer list
#endif

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, wxID_ANY, title, pos, size),
         m_timerIdleWakeUp(this)
{
    m_pidLast = 0;

#ifdef __WXMAC__
    // we need this in order to allow the about menu relocation, since ABOUT is
    // not the default id of the about menu
    wxApp::s_macAboutMenuItemId = Exec_About;
#endif

    // create a menu bar
    wxMenu *menuFile = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
    menuFile->Append(Exec_Kill, _T("&Kill process...\tCtrl-K"),
                     _T("Kill a process by PID"));
    menuFile->AppendSeparator();
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
    execMenu->AppendSeparator();
    execMenu->Append(Exec_Redirect, _T("Capture command &output...\tCtrl-O"),
                     _T("Launch a program and capture its output"));
    execMenu->Append(Exec_Pipe, _T("&Pipe through command..."),
                     _T("Pipe a string through a filter"));
    execMenu->Append(Exec_POpen, _T("&Open a pipe to a command...\tCtrl-P"),
                     _T("Open a pipe to and from another program"));

    execMenu->AppendSeparator();
    execMenu->Append(Exec_OpenFile, _T("Open &file...\tCtrl-F"),
                     _T("Launch the command to open this kind of files"));
#ifdef __WINDOWS__
    execMenu->AppendSeparator();
    execMenu->Append(Exec_DDEExec, _T("Execute command via &DDE...\tCtrl-D"));
    execMenu->Append(Exec_DDERequest, _T("Send DDE &request...\tCtrl-R"));
#endif

    wxMenu *helpMenu = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
    helpMenu->Append(Exec_About, _T("&About...\tF1"), _T("Show about dialog"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(execMenu, _T("&Exec"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    // create the listbox in which we will show misc messages as they come
    m_lbox = new wxListBox(this, wxID_ANY);
    wxFont font(12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL,
                wxFONTWEIGHT_NORMAL);
    if ( font.Ok() )
        m_lbox->SetFont(font);

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar();
    SetStatusText(_T("Welcome to wxWidgets exec sample!"));
#endif // wxUSE_STATUSBAR
}

// ----------------------------------------------------------------------------
// event handlers: file and help menu
// ----------------------------------------------------------------------------

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnClear(wxCommandEvent& WXUNUSED(event))
{
    m_lbox->Clear();
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("Exec wxWidgets Sample\n(c) 2000-2002 Vadim Zeitlin"),
                 _T("About Exec"), wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnKill(wxCommandEvent& WXUNUSED(event))
{
    long pid = wxGetNumberFromUser(_T("Please specify the process to kill"),
                                   _T("Enter PID:"),
                                   _T("Exec question"),
                                   m_pidLast,
                                   // we need the full unsigned int range
                                   -INT_MAX, INT_MAX,
                                   this);
    if ( pid == -1 )
    {
        // cancelled
        return;
    }

    static const wxString signalNames[] =
    {
        _T("Just test (SIGNONE)"),
        _T("Hangup (SIGHUP)"),
        _T("Interrupt (SIGINT)"),
        _T("Quit (SIGQUIT)"),
        _T("Illegal instruction (SIGILL)"),
        _T("Trap (SIGTRAP)"),
        _T("Abort (SIGABRT)"),
        _T("Emulated trap (SIGEMT)"),
        _T("FP exception (SIGFPE)"),
        _T("Kill (SIGKILL)"),
        _T("Bus (SIGBUS)"),
        _T("Segment violation (SIGSEGV)"),
        _T("System (SIGSYS)"),
        _T("Broken pipe (SIGPIPE)"),
        _T("Alarm (SIGALRM)"),
        _T("Terminate (SIGTERM)"),
    };

    int sig = wxGetSingleChoiceIndex(_T("How to kill the process?"),
                                     _T("Exec question"),
                                     WXSIZEOF(signalNames), signalNames,
                                     this);
    switch ( sig )
    {
        default:
            wxFAIL_MSG( _T("unexpected return value") );
            // fall through

        case -1:
            // cancelled
            return;

        case wxSIGNONE:
        case wxSIGHUP:
        case wxSIGINT:
        case wxSIGQUIT:
        case wxSIGILL:
        case wxSIGTRAP:
        case wxSIGABRT:
        case wxSIGEMT:
        case wxSIGFPE:
        case wxSIGKILL:
        case wxSIGBUS:
        case wxSIGSEGV:
        case wxSIGSYS:
        case wxSIGPIPE:
        case wxSIGALRM:
        case wxSIGTERM:
            break;
    }

    if ( sig == 0 )
    {
        if ( wxProcess::Exists(pid) )
            wxLogStatus(_T("Process %ld is running."), pid);
        else
            wxLogStatus(_T("No process with pid = %ld."), pid);
    }
    else // not SIGNONE
    {
        wxKillError rc = wxProcess::Kill(pid, (wxSignal)sig);
        if ( rc == wxKILL_OK )
        {
            wxLogStatus(_T("Process %ld killed with signal %d."), pid, sig);
        }
        else
        {
            static const wxChar *errorText[] =
            {
                _T(""), // no error
                _T("signal not supported"),
                _T("permission denied"),
                _T("no such process"),
                _T("unspecified error"),
            };

            wxLogStatus(_T("Failed to kill process %ld with signal %d: %s"),
                        pid, sig, errorText[rc]);
        }
    }
}

// ----------------------------------------------------------------------------
// event handlers: exec menu
// ----------------------------------------------------------------------------

void MyFrame::DoAsyncExec(const wxString& cmd)
{
    wxProcess *process = new MyProcess(this, cmd);
    m_pidLast = wxExecute(cmd, wxEXEC_ASYNC, process);
    if ( !m_pidLast )
    {
        wxLogError( _T("Execution of '%s' failed."), cmd.c_str() );

        delete process;
    }
    else
    {
        wxLogStatus( _T("Process %ld (%s) launched."),
            m_pidLast, cmd.c_str() );

        m_cmdLast = cmd;
    }
}

void MyFrame::OnSyncExec(wxCommandEvent& WXUNUSED(event))
{
    wxString cmd = wxGetTextFromUser(_T("Enter the command: "),
                                     DIALOG_TITLE,
                                     m_cmdLast);

    if ( !cmd )
        return;

    wxLogStatus( _T("'%s' is running please wait..."), cmd.c_str() );

    int code = wxExecute(cmd, wxEXEC_SYNC);

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

    DoAsyncExec(cmd);
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
            sync = true;
            break;

        case wxNO:
            sync = false;
            break;

        default:
            return;
    }

    if ( sync )
    {
        wxArrayString output, errors;
        int code = wxExecute(cmd, output, errors);
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
        if ( !wxExecute(cmd, wxEXEC_ASYNC, process) )
        {
            wxLogError(_T("Execution of '%s' failed."), cmd.c_str());

            delete process;
        }
        else
        {
            AddAsyncProcess(process);
        }
    }

    m_cmdLast = cmd;
}

void MyFrame::OnExecWithPipe(wxCommandEvent& WXUNUSED(event))
{
    if ( !m_cmdLast )
        m_cmdLast = _T("tr [a-z] [A-Z]");

    wxString cmd = wxGetTextFromUser(_T("Enter the command: "),
                                     DIALOG_TITLE,
                                     m_cmdLast);

    if ( !cmd )
        return;

    wxString input = wxGetTextFromUser(_T("Enter the string to send to it: "),
                                       DIALOG_TITLE);
    if ( !input )
        return;

    // always execute the filter asynchronously
    MyPipedProcess2 *process = new MyPipedProcess2(this, cmd, input);
    long pid = wxExecute(cmd, wxEXEC_ASYNC, process);
    if ( pid )
    {
        wxLogStatus( _T("Process %ld (%s) launched."), pid, cmd.c_str() );

        AddAsyncProcess(process);
    }
    else
    {
        wxLogError(_T("Execution of '%s' failed."), cmd.c_str());

        delete process;
    }

    m_cmdLast = cmd;
}

void MyFrame::OnPOpen(wxCommandEvent& WXUNUSED(event))
{
    wxString cmd = wxGetTextFromUser(_T("Enter the command to launch: "),
                                     DIALOG_TITLE,
                                     m_cmdLast);
    if ( cmd.empty() )
        return;

    wxProcess *process = wxProcess::Open(cmd);
    if ( !process )
    {
        wxLogError(_T("Failed to launch the command."));
        return;
    }

    wxLogVerbose(_T("PID of the new process: %ld"), process->GetPid());

    wxOutputStream *out = process->GetOutputStream();
    if ( !out )
    {
        wxLogError(_T("Failed to connect to child stdin"));
        return;
    }

    wxInputStream *in = process->GetInputStream();
    if ( !in )
    {
        wxLogError(_T("Failed to connect to child stdout"));
        return;
    }

    new MyPipeFrame(this, cmd, process);
}

void MyFrame::OnFileExec(wxCommandEvent& WXUNUSED(event))
{
    static wxString s_filename;

    wxString filename;

#if wxUSE_FILEDLG
    filename = wxLoadFileSelector(wxEmptyString, wxEmptyString, s_filename);
#endif // wxUSE_FILEDLG

    if ( filename.empty() )
        return;

    s_filename = filename;

    wxString ext = filename.AfterFirst(_T('.'));
    wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension(ext);
    if ( !ft )
    {
        wxLogError(_T("Impossible to determine the file type for extension '%s'"),
                   ext.c_str());
        return;
    }

    wxString cmd;
    bool ok = ft->GetOpenCommand(&cmd,
                                 wxFileType::MessageParameters(filename));
    delete ft;
    if ( !ok )
    {
        wxLogError(_T("Impossible to find out how to open files of extension '%s'"),
                   ext.c_str());
        return;
    }

    DoAsyncExec(cmd);
}

// ----------------------------------------------------------------------------
// DDE stuff
// ----------------------------------------------------------------------------

#ifdef __WINDOWS__

bool MyFrame::GetDDEServer()
{
    wxString server = wxGetTextFromUser(_T("Server to connect to:"),
                                        DIALOG_TITLE, m_server);
    if ( !server )
        return false;

    m_server = server;

    wxString topic = wxGetTextFromUser(_T("DDE topic:"), DIALOG_TITLE, m_topic);
    if ( !topic )
        return false;

    m_topic = topic;

    wxString cmd = wxGetTextFromUser(_T("DDE command:"), DIALOG_TITLE, m_cmdDde);
    if ( !cmd )
        return false;

    m_cmdDde = cmd;

    return true;
}

void MyFrame::OnDDEExec(wxCommandEvent& WXUNUSED(event))
{
    if ( !GetDDEServer() )
        return;

    wxDDEClient client;
    wxConnectionBase *conn = client.MakeConnection(wxEmptyString, m_server, m_topic);
    if ( !conn )
    {
        wxLogError(_T("Failed to connect to the DDE server '%s'."),
                   m_server.c_str());
    }
    else
    {
        if ( !conn->Execute(m_cmdDde) )
        {
            wxLogError(_T("Failed to execute command '%s' via DDE."),
                       m_cmdDde.c_str());
        }
        else
        {
            wxLogStatus(_T("Successfully executed DDE command"));
        }
    }
}

void MyFrame::OnDDERequest(wxCommandEvent& WXUNUSED(event))
{
    if ( !GetDDEServer() )
        return;

    wxDDEClient client;
    wxConnectionBase *conn = client.MakeConnection(wxEmptyString, m_server, m_topic);
    if ( !conn )
    {
        wxLogError(_T("Failed to connect to the DDE server '%s'."),
                   m_server.c_str());
    }
    else
    {
        if ( !conn->Request(m_cmdDde) )
        {
            wxLogError(_T("Failed to  send request '%s' via DDE."),
                       m_cmdDde.c_str());
        }
        else
        {
            wxLogStatus(_T("Successfully sent DDE request."));
        }
    }
}

#endif // __WINDOWS__

// ----------------------------------------------------------------------------
// various helpers
// ----------------------------------------------------------------------------

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

void MyFrame::OnTimer(wxTimerEvent& WXUNUSED(event))
{
    wxWakeUpIdle();
}

void MyFrame::OnProcessTerminated(MyPipedProcess *process)
{
    RemoveAsyncProcess(process);
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

    m_lbox->Append(wxString::Format(_T("--- End of %s ---"),
                                    title.Lower().c_str()));
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
    bool hasInput = false;

    if ( IsInputAvailable() )
    {
        wxTextInputStream tis(*GetInputStream());

        // this assumes that the output is always line buffered
        wxString msg;
        msg << m_cmd << _T(" (stdout): ") << tis.ReadLine();

        m_parent->GetLogListBox()->Append(msg);

        hasInput = true;
    }

    if ( IsErrorAvailable() )
    {
        wxTextInputStream tis(*GetErrorStream());

        // this assumes that the output is always line buffered
        wxString msg;
        msg << m_cmd << _T(" (stderr): ") << tis.ReadLine();

        m_parent->GetLogListBox()->Append(msg);

        hasInput = true;
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

// ----------------------------------------------------------------------------
// MyPipedProcess2
// ----------------------------------------------------------------------------

bool MyPipedProcess2::HasInput()
{
    if ( !m_input.empty() )
    {
        wxTextOutputStream os(*GetOutputStream());
        os.WriteString(m_input);

        CloseOutput();
        m_input.clear();

        // call us once again - may be we'll have output
        return true;
    }

    return MyPipedProcess::HasInput();
}

// ============================================================================
// MyPipeFrame implementation
// ============================================================================

MyPipeFrame::MyPipeFrame(wxFrame *parent,
                         const wxString& cmd,
                         wxProcess *process)
           : wxFrame(parent, wxID_ANY, cmd),
             m_process(process),
             // in a real program we'd check that the streams are !NULL here
             m_out(*process->GetOutputStream()),
             m_in(*process->GetInputStream()),
             m_err(*process->GetErrorStream())
{
    m_process->SetNextHandler(this);

    wxPanel *panel = new wxPanel(this, wxID_ANY);

    m_textOut = new wxTextCtrl(panel, wxID_ANY, wxEmptyString,
                              wxDefaultPosition, wxDefaultSize,
                              wxTE_PROCESS_ENTER);
    m_textIn = new wxTextCtrl(panel, wxID_ANY, wxEmptyString,
                               wxDefaultPosition, wxDefaultSize,
                               wxTE_MULTILINE | wxTE_RICH);
    m_textIn->SetEditable(false);
    m_textErr = new wxTextCtrl(panel, wxID_ANY, wxEmptyString,
                               wxDefaultPosition, wxDefaultSize,
                               wxTE_MULTILINE | wxTE_RICH);
    m_textErr->SetEditable(false);

    wxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
    sizerTop->Add(m_textOut, 0, wxGROW | wxALL, 5);

    wxSizer *sizerBtns = new wxBoxSizer(wxHORIZONTAL);
    sizerBtns->
        Add(new wxButton(panel, Exec_Btn_Send, _T("&Send")), 0, wxALL, 5);
    sizerBtns->
        Add(new wxButton(panel, Exec_Btn_SendFile, _T("&File...")), 0, wxALL, 5);
    sizerBtns->
        Add(new wxButton(panel, Exec_Btn_Get, _T("&Get")), 0, wxALL, 5);
    sizerBtns->
        Add(new wxButton(panel, Exec_Btn_Close, _T("&Close")), 0, wxALL, 5);

    sizerTop->Add(sizerBtns, 0, wxCENTRE | wxALL, 5);
    sizerTop->Add(m_textIn, 1, wxGROW | wxALL, 5);
    sizerTop->Add(m_textErr, 1, wxGROW | wxALL, 5);

    panel->SetSizer(sizerTop);
    sizerTop->Fit(this);

    Show();
}

void MyPipeFrame::OnBtnSendFile(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_FILEDLG
    wxFileDialog filedlg(this, _T("Select file to send"));
    if ( filedlg.ShowModal() != wxID_OK )
        return;

    wxFFile file(filedlg.GetFilename(), _T("r"));
    wxString data;
    if ( !file.IsOpened() || !file.ReadAll(&data) )
        return;

    // can't write the entire string at once, this risk overflowing the pipe
    // and we would dead lock
    size_t len = data.length();
    const wxChar *pc = data.c_str();
    while ( len )
    {
        const size_t CHUNK_SIZE = 4096;
        m_out.Write(pc, len > CHUNK_SIZE ? CHUNK_SIZE : len);

        // note that not all data could have been written as we don't block on
        // the write end of the pipe
        const size_t lenChunk = m_out.LastWrite();

        pc += lenChunk;
        len -= lenChunk;

        DoGet();
    }
#endif // wxUSE_FILEDLG
}

void MyPipeFrame::DoGet()
{
    // we don't have any way to be notified when any input appears on the
    // stream so we have to poll it :-(
    DoGetFromStream(m_textIn, m_in);
    DoGetFromStream(m_textErr, m_err);
}

void MyPipeFrame::DoGetFromStream(wxTextCtrl *text, wxInputStream& in)
{
    while ( in.CanRead() )
    {
        wxChar buffer[4096];
        buffer[in.Read(buffer, WXSIZEOF(buffer) - 1).LastRead()] = _T('\0');

        text->AppendText(buffer);
    }
}

void MyPipeFrame::DoClose()
{
    m_process->CloseOutput();

    DisableInput();
}

void MyPipeFrame::DisableInput()
{
    m_textOut->SetEditable(false);
    FindWindow(Exec_Btn_Send)->Disable();
    FindWindow(Exec_Btn_SendFile)->Disable();
    FindWindow(Exec_Btn_Close)->Disable();
}

void MyPipeFrame::DisableOutput()
{
    FindWindow(Exec_Btn_Get)->Disable();
}

void MyPipeFrame::OnClose(wxCloseEvent& event)
{
    if ( m_process )
    {
        // we're not interested in getting the process termination notification
        // if we are closing it ourselves
        wxProcess *process = m_process;
        m_process = NULL;
        process->SetNextHandler(NULL);

        process->CloseOutput();
    }

    event.Skip();
}

void MyPipeFrame::OnProcessTerm(wxProcessEvent& WXUNUSED(event))
{
    DoGet();

    delete m_process;
    m_process = NULL;

    wxLogWarning(_T("The other process has terminated, closing"));

    DisableInput();
    DisableOutput();
}
