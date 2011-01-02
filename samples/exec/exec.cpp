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
    #include "wx/checkbox.h"
    #include "wx/stattext.h"
    #include "wx/textctrl.h"
    #include "wx/listbox.h"

    #include "wx/sizer.h"
#endif

#include "wx/txtstrm.h"
#include "wx/numdlg.h"
#include "wx/textdlg.h"
#include "wx/ffile.h"
#include "wx/stopwatch.h"

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
WX_DEFINE_ARRAY_PTR(MyPipedProcess *, MyPipedProcessesArray);

class MyProcess;
WX_DEFINE_ARRAY_PTR(MyProcess *, MyProcessesArray);

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor and dtor
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    virtual ~MyFrame();

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);

    void OnKill(wxCommandEvent& event);

    void OnClear(wxCommandEvent& event);

    void OnBeginBusyCursor(wxCommandEvent& event);
    void OnEndBusyCursor(wxCommandEvent& event);

    void OnSyncExec(wxCommandEvent& event);
    void OnSyncNoEventsExec(wxCommandEvent& event);
    void OnAsyncExec(wxCommandEvent& event);
    void OnShell(wxCommandEvent& event);
    void OnExecWithRedirect(wxCommandEvent& event);
    void OnExecWithPipe(wxCommandEvent& event);

    void OnPOpen(wxCommandEvent& event);

    void OnFileExec(wxCommandEvent& event);
    void OnFileLaunch(wxCommandEvent& event);
    void OnOpenURL(wxCommandEvent& event);

    void OnAbout(wxCommandEvent& event);

    // polling output of async processes
    void OnIdleTimer(wxTimerEvent& event);
    void OnIdle(wxIdleEvent& event);

    // for MyPipedProcess
    void OnProcessTerminated(MyPipedProcess *process);
    wxListBox *GetLogListBox() const { return m_lbox; }

    // for MyProcess
    void OnAsyncTermination(MyProcess *process);

    // timer updating a counter in the background
    void OnBgTimer(wxTimerEvent& event);

private:
    void ShowOutput(const wxString& cmd,
                    const wxArrayString& output,
                    const wxString& title);

    void DoAsyncExec(const wxString& cmd);

    void AddAsyncProcess(MyProcess *process) { m_allAsync.push_back(process); }

    void AddPipedProcess(MyPipedProcess *process);
    void RemovePipedProcess(MyPipedProcess *process);


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

    // array of running processes with redirected IO
    MyPipedProcessesArray m_running;

    // array of all asynchrously running processes
    MyProcessesArray m_allAsync;

    // the idle event wake up timer
    wxTimer m_timerIdleWakeUp;

    // a background timer allowing to easily check visually whether the
    // messages are processed or not
    wxTimer m_timerBg;

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
        s += wxT('\n');
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
    // timer ids
    Exec_TimerIdle = 10,
    Exec_TimerBg,

    // menu items
    Exec_Quit = 100,
    Exec_Kill,
    Exec_ClearLog,
    Exec_BeginBusyCursor,
    Exec_EndBusyCursor,
    Exec_SyncExec = 200,
    Exec_SyncNoEventsExec,
    Exec_AsyncExec,
    Exec_Shell,
    Exec_POpen,
    Exec_OpenFile,
    Exec_LaunchFile,
    Exec_OpenURL,
    Exec_DDEExec,
    Exec_DDERequest,
    Exec_Redirect,
    Exec_Pipe,
    Exec_About = wxID_ABOUT,

    // control ids
    Exec_Btn_Send = 1000,
    Exec_Btn_SendFile,
    Exec_Btn_Get,
    Exec_Btn_Close
};

static const wxChar *DIALOG_TITLE = wxT("Exec sample");

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
    EVT_MENU(Exec_BeginBusyCursor,  MyFrame::OnBeginBusyCursor)
    EVT_MENU(Exec_EndBusyCursor,  MyFrame::OnEndBusyCursor)

    EVT_MENU(Exec_SyncExec, MyFrame::OnSyncExec)
    EVT_MENU(Exec_SyncNoEventsExec, MyFrame::OnSyncNoEventsExec)
    EVT_MENU(Exec_AsyncExec, MyFrame::OnAsyncExec)
    EVT_MENU(Exec_Shell, MyFrame::OnShell)
    EVT_MENU(Exec_Redirect, MyFrame::OnExecWithRedirect)
    EVT_MENU(Exec_Pipe, MyFrame::OnExecWithPipe)

    EVT_MENU(Exec_POpen, MyFrame::OnPOpen)

    EVT_MENU(Exec_OpenFile, MyFrame::OnFileExec)
    EVT_MENU(Exec_LaunchFile, MyFrame::OnFileLaunch)
    EVT_MENU(Exec_OpenURL, MyFrame::OnOpenURL)

#ifdef __WINDOWS__
    EVT_MENU(Exec_DDEExec, MyFrame::OnDDEExec)
    EVT_MENU(Exec_DDERequest, MyFrame::OnDDERequest)
#endif // __WINDOWS__

    EVT_MENU(Exec_About, MyFrame::OnAbout)

    EVT_IDLE(MyFrame::OnIdle)

    EVT_TIMER(Exec_TimerIdle, MyFrame::OnIdleTimer)
    EVT_TIMER(Exec_TimerBg, MyFrame::OnBgTimer)
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
    if ( !wxApp::OnInit() )
        return false;

    // Create the main application window
    MyFrame *frame = new MyFrame(wxT("Exec wxWidgets sample"),
                                 wxDefaultPosition, wxSize(500, 140));

    // Show it
    frame->Show(true);

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
         m_timerIdleWakeUp(this, Exec_TimerIdle),
         m_timerBg(this, Exec_TimerBg)
{
    m_pidLast = 0;

#ifdef __WXMAC__
    // we need this in order to allow the about menu relocation, since ABOUT is
    // not the default id of the about menu
    wxApp::s_macAboutMenuItemId = Exec_About;
#endif

    // create a menu bar
    wxMenu *menuFile = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
    menuFile->Append(Exec_Kill, wxT("&Kill process...\tCtrl-K"),
                     wxT("Kill a process by PID"));
    menuFile->AppendSeparator();
    menuFile->Append(Exec_ClearLog, wxT("&Clear log\tCtrl-L"),
                     wxT("Clear the log window"));
    menuFile->AppendSeparator();
    menuFile->Append(Exec_BeginBusyCursor, wxT("Show &busy cursor\tCtrl-C"));
    menuFile->Append(Exec_EndBusyCursor, wxT("Show &normal cursor\tShift-Ctrl-C"));
    menuFile->AppendSeparator();
    menuFile->Append(Exec_Quit, wxT("E&xit\tAlt-X"), wxT("Quit this program"));

    wxMenu *execMenu = new wxMenu;
    execMenu->Append(Exec_SyncExec, wxT("Sync &execution...\tCtrl-E"),
                     wxT("Launch a program and return when it terminates"));
    execMenu->Append(Exec_SyncNoEventsExec, wxT("Sync execution and &block...\tCtrl-B"),
                     wxT("Launch a program and block until it terminates"));
    execMenu->Append(Exec_AsyncExec, wxT("&Async execution...\tCtrl-A"),
                     wxT("Launch a program and return immediately"));
    execMenu->Append(Exec_Shell, wxT("Execute &shell command...\tCtrl-S"),
                     wxT("Launch a shell and execute a command in it"));
    execMenu->AppendSeparator();
    execMenu->Append(Exec_Redirect, wxT("Capture command &output...\tCtrl-O"),
                     wxT("Launch a program and capture its output"));
    execMenu->Append(Exec_Pipe, wxT("&Pipe through command..."),
                     wxT("Pipe a string through a filter"));
    execMenu->Append(Exec_POpen, wxT("&Open a pipe to a command...\tCtrl-P"),
                     wxT("Open a pipe to and from another program"));

    execMenu->AppendSeparator();
    execMenu->Append(Exec_OpenFile, wxT("Open &file...\tCtrl-F"),
                     wxT("Launch the command to open this kind of files"));
    execMenu->Append(Exec_LaunchFile, wxT("La&unch file...\tShift-Ctrl-F"),
                     wxT("Launch the default application associated with the file"));
    execMenu->Append(Exec_OpenURL, wxT("Open &URL...\tCtrl-U"),
                     wxT("Launch the default browser with the given URL"));
#ifdef __WINDOWS__
    execMenu->AppendSeparator();
    execMenu->Append(Exec_DDEExec, wxT("Execute command via &DDE...\tCtrl-D"));
    execMenu->Append(Exec_DDERequest, wxT("Send DDE &request...\tCtrl-R"));
#endif

    wxMenu *helpMenu = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
    helpMenu->Append(Exec_About, wxT("&About...\tF1"), wxT("Show about dialog"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, wxT("&File"));
    menuBar->Append(execMenu, wxT("&Exec"));
    menuBar->Append(helpMenu, wxT("&Help"));

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
    CreateStatusBar(2);
    SetStatusText(wxT("Welcome to wxWidgets exec sample!"));
#endif // wxUSE_STATUSBAR

    m_timerBg.Start(1000);
}

MyFrame::~MyFrame()
{
    // any processes left until now must be deleted manually: normally this is
    // done when the associated process terminates but it must be still running
    // if this didn't happen until now
    for ( size_t n = 0; n < m_allAsync.size(); n++ )
    {
        delete m_allAsync[n];
    }
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

void MyFrame::OnBeginBusyCursor(wxCommandEvent& WXUNUSED(event))
{
    wxBeginBusyCursor();
}

void MyFrame::OnEndBusyCursor(wxCommandEvent& WXUNUSED(event))
{
    wxEndBusyCursor();
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxT("Exec wxWidgets Sample\n(c) 2000-2002 Vadim Zeitlin"),
                 wxT("About Exec"), wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnKill(wxCommandEvent& WXUNUSED(event))
{
    long pid = wxGetNumberFromUser(wxT("Please specify the process to kill"),
                                   wxT("Enter PID:"),
                                   wxT("Exec question"),
                                   m_pidLast,
                                   // we need the full unsigned int range
                                   -INT_MAX, INT_MAX,
                                   this);
    if ( pid == -1 )
    {
        // cancelled
        return;
    }

    m_pidLast = pid;

    static const wxString signalNames[] =
    {
        wxT("Just test (SIGNONE)"),
        wxT("Hangup (SIGHUP)"),
        wxT("Interrupt (SIGINT)"),
        wxT("Quit (SIGQUIT)"),
        wxT("Illegal instruction (SIGILL)"),
        wxT("Trap (SIGTRAP)"),
        wxT("Abort (SIGABRT)"),
        wxT("Emulated trap (SIGEMT)"),
        wxT("FP exception (SIGFPE)"),
        wxT("Kill (SIGKILL)"),
        wxT("Bus (SIGBUS)"),
        wxT("Segment violation (SIGSEGV)"),
        wxT("System (SIGSYS)"),
        wxT("Broken pipe (SIGPIPE)"),
        wxT("Alarm (SIGALRM)"),
        wxT("Terminate (SIGTERM)"),
    };

    static int s_sigLast = wxSIGNONE;
    int sig = wxGetSingleChoiceIndex(wxT("How to kill the process?"),
                                     wxT("Exec question"),
                                     WXSIZEOF(signalNames), signalNames,
                                     s_sigLast,
                                     this);
    switch ( sig )
    {
        default:
            wxFAIL_MSG( wxT("unexpected return value") );
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

    s_sigLast = sig;

    if ( sig == wxSIGNONE )
    {
        // This simply calls Kill(wxSIGNONE) but using it is more convenient.
        if ( wxProcess::Exists(pid) )
        {
            wxLogStatus(wxT("Process %ld is running."), pid);
        }
        else
        {
            wxLogStatus(wxT("No process with pid = %ld."), pid);
        }
    }
    else // not SIGNONE
    {
        wxKillError rc = wxProcess::Kill(pid, (wxSignal)sig);
        if ( rc == wxKILL_OK )
        {
            wxLogStatus(wxT("Process %ld killed with signal %d."), pid, sig);
        }
        else
        {
            static const wxChar *errorText[] =
            {
                wxT(""), // no error
                wxT("signal not supported"),
                wxT("permission denied"),
                wxT("no such process"),
                wxT("unspecified error"),
            };

            wxLogStatus(wxT("Failed to kill process %ld with signal %d: %s"),
                        pid, sig, errorText[rc]);
        }
    }
}

// ----------------------------------------------------------------------------
// execution options dialog
// ----------------------------------------------------------------------------

enum ExecQueryDialogID
{
    TEXT_EXECUTABLE,
    TEXT_CWD,
    TEXT_ENVIRONMENT
};

class ExecQueryDialog : public wxDialog
{
public:
    ExecQueryDialog(const wxString& cmd);

    wxString GetExecutable() const
    {
        return m_executable->GetValue();
    }

    wxString GetWorkDir() const
    {
        return m_useCWD->GetValue() ? m_cwdtext->GetValue() : wxString();
    }

    void GetEnvironment(wxEnvVariableHashMap& env);

private:
    void OnUpdateWorkingDirectoryUI(wxUpdateUIEvent& event)
    {
        event.Enable(m_useCWD->GetValue());
    }

    void OnUpdateEnvironmentUI(wxUpdateUIEvent& event)
    {
        event.Enable(m_useEnv->GetValue());
    }

    wxTextCtrl* m_executable;
    wxTextCtrl* m_cwdtext;
    wxTextCtrl* m_envtext;
    wxCheckBox* m_useCWD;
    wxCheckBox* m_useEnv;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(ExecQueryDialog, wxDialog)
    EVT_UPDATE_UI(TEXT_CWD, ExecQueryDialog::OnUpdateWorkingDirectoryUI)
    EVT_UPDATE_UI(TEXT_ENVIRONMENT, ExecQueryDialog::OnUpdateEnvironmentUI)
END_EVENT_TABLE()

ExecQueryDialog::ExecQueryDialog(const wxString& cmd)
    : wxDialog(NULL, wxID_ANY, DIALOG_TITLE,
               wxDefaultPosition, wxDefaultSize,
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    wxSizer* globalSizer = new wxBoxSizer(wxVERTICAL);

    m_executable = new wxTextCtrl(this, TEXT_EXECUTABLE, wxString());
    m_cwdtext = new wxTextCtrl(this, TEXT_CWD, wxString());
    m_envtext = new wxTextCtrl(this, TEXT_ENVIRONMENT, wxString(),
                               wxDefaultPosition, wxSize(300, 200),
                               wxTE_MULTILINE|wxHSCROLL);

    const wxSizerFlags flagsExpand = wxSizerFlags().Expand().Border();
    globalSizer->Add(new wxStaticText(this, wxID_ANY, "Enter the command: "),
                     flagsExpand);
    globalSizer->Add(m_executable, flagsExpand);

    m_useCWD = new wxCheckBox(this, wxID_ANY, "Working directory: ");
    globalSizer->Add(m_useCWD, flagsExpand);
    globalSizer->Add(m_cwdtext, flagsExpand);

    m_useEnv = new wxCheckBox(this, wxID_ANY, "Environment: ");
    globalSizer->Add(m_useEnv, flagsExpand);
    globalSizer->Add(m_envtext, wxSizerFlags(flagsExpand).Proportion(1));

    globalSizer->Add(CreateStdDialogButtonSizer(wxOK|wxCANCEL), flagsExpand);
    SetSizerAndFit(globalSizer);


    m_executable->SetValue(cmd);
    m_cwdtext->SetValue(wxGetCwd());
    wxEnvVariableHashMap env;
    if ( wxGetEnvMap(&env) )
    {
        for ( wxEnvVariableHashMap::iterator it = env.begin();
              it != env.end();
              ++it )
        {
            m_envtext->AppendText(it->first + '=' + it->second + '\n');
        }
    }
    m_useCWD->SetValue(false);
    m_useEnv->SetValue(false);
}

void ExecQueryDialog::GetEnvironment(wxEnvVariableHashMap& env)
{
    env.clear();
    if ( m_useEnv->GetValue() )
    {
        wxString name,
                 value;

        const int nb = m_envtext->GetNumberOfLines();
        for ( int l = 0; l < nb; l++ )
        {
            const wxString line = m_envtext->GetLineText(l).Trim();

            if ( !line.empty() )
            {
                name = line.BeforeFirst('=', &value);
                if ( name.empty() )
                {
                    wxLogWarning("Skipping invalid environment line \"%s\".", line);
                    continue;
                }

                env[name] = value;
            }
        }
    }
}

static bool QueryExec(wxString& cmd, wxExecuteEnv& env)
{
    ExecQueryDialog dialog(cmd);

    if ( dialog.ShowModal() != wxID_OK )
        return false;

    cmd = dialog.GetExecutable();
    env.cwd = dialog.GetWorkDir();
    dialog.GetEnvironment(env.env);

    return true;
}

// ----------------------------------------------------------------------------
// event handlers: exec menu
// ----------------------------------------------------------------------------

void MyFrame::DoAsyncExec(const wxString& cmd)
{
    MyProcess * const process = new MyProcess(this, cmd);
    m_pidLast = wxExecute(cmd, wxEXEC_ASYNC, process);
    if ( !m_pidLast )
    {
        wxLogError(wxT("Execution of '%s' failed."), cmd.c_str());

        delete process;
    }
    else
    {
        wxLogStatus(wxT("Process %ld (%s) launched."), m_pidLast, cmd.c_str());

        m_cmdLast = cmd;

        // the parent frame keeps track of all async processes as it needs to
        // free them if we exit before the child process terminates
        AddAsyncProcess(process);
    }
}

void MyFrame::OnSyncExec(wxCommandEvent& WXUNUSED(event))
{
    wxString cmd;
    wxExecuteEnv env;
    if ( !QueryExec(cmd, env) )
        return;

    wxLogStatus( wxT("'%s' is running please wait..."), cmd.c_str() );

    int code = wxExecute(cmd, wxEXEC_SYNC, NULL, &env);

    wxLogStatus(wxT("Process '%s' terminated with exit code %d."),
        cmd.c_str(), code);

    m_cmdLast = cmd;
}

void MyFrame::OnSyncNoEventsExec(wxCommandEvent& WXUNUSED(event))
{
    wxString cmd = wxGetTextFromUser(wxT("Enter the command: "),
                                     DIALOG_TITLE,
                                     m_cmdLast);

    if ( !cmd )
        return;

    wxLogStatus( wxT("'%s' is running please wait..."), cmd.c_str() );

    int code = wxExecute(cmd, wxEXEC_BLOCK);

    wxLogStatus(wxT("Process '%s' terminated with exit code %d."),
        cmd.c_str(), code);

    m_cmdLast = cmd;
}

void MyFrame::OnAsyncExec(wxCommandEvent& WXUNUSED(event))
{
    wxString cmd = wxGetTextFromUser(wxT("Enter the command: "),
                                     DIALOG_TITLE,
                                     m_cmdLast);

    if ( !cmd )
        return;

    DoAsyncExec(cmd);
}

void MyFrame::OnShell(wxCommandEvent& WXUNUSED(event))
{
    wxString cmd = wxGetTextFromUser(wxT("Enter the command: "),
                                     DIALOG_TITLE,
                                     m_cmdLast);

    if ( !cmd )
        return;

    int code = wxShell(cmd);
    wxLogStatus(wxT("Shell command '%s' terminated with exit code %d."),
                cmd.c_str(), code);
    m_cmdLast = cmd;
}

void MyFrame::OnExecWithRedirect(wxCommandEvent& WXUNUSED(event))
{
    if ( !m_cmdLast )
    {
#ifdef __WXMSW__
        m_cmdLast = "type Makefile.in";
#else
        m_cmdLast = "cat -n Makefile";
#endif
    }

    wxString cmd = wxGetTextFromUser(wxT("Enter the command: "),
                                     DIALOG_TITLE,
                                     m_cmdLast);

    if ( !cmd )
        return;

    bool sync;
    switch ( wxMessageBox(wxT("Execute it synchronously?"),
                          wxT("Exec question"),
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
        wxLogStatus("\"%s\" is running please wait...", cmd);

        wxStopWatch sw;

        wxArrayString output, errors;
        int code = wxExecute(cmd, output, errors);

        wxLogStatus("Command \"%s\" terminated after %ldms; exit code %d.",
                    cmd, sw.Time(), code);

        if ( code != -1 )
        {
            ShowOutput(cmd, output, wxT("Output"));
            ShowOutput(cmd, errors, wxT("Errors"));
        }
    }
    else // async exec
    {
        MyPipedProcess *process = new MyPipedProcess(this, cmd);
        if ( !wxExecute(cmd, wxEXEC_ASYNC, process) )
        {
            wxLogError(wxT("Execution of '%s' failed."), cmd.c_str());

            delete process;
        }
        else
        {
            AddPipedProcess(process);
        }
    }

    m_cmdLast = cmd;
}

void MyFrame::OnExecWithPipe(wxCommandEvent& WXUNUSED(event))
{
    if ( !m_cmdLast )
        m_cmdLast = wxT("tr [a-z] [A-Z]");

    wxString cmd = wxGetTextFromUser(wxT("Enter the command: "),
                                     DIALOG_TITLE,
                                     m_cmdLast);

    if ( !cmd )
        return;

    wxString input = wxGetTextFromUser(wxT("Enter the string to send to it: "),
                                       DIALOG_TITLE);
    if ( !input )
        return;

    // always execute the filter asynchronously
    MyPipedProcess2 *process = new MyPipedProcess2(this, cmd, input);
    long pid = wxExecute(cmd, wxEXEC_ASYNC, process);
    if ( pid )
    {
        wxLogStatus(wxT("Process %ld (%s) launched."), pid, cmd.c_str());

        AddPipedProcess(process);
    }
    else
    {
        wxLogError(wxT("Execution of '%s' failed."), cmd.c_str());

        delete process;
    }

    m_cmdLast = cmd;
}

void MyFrame::OnPOpen(wxCommandEvent& WXUNUSED(event))
{
    wxString cmd = wxGetTextFromUser(wxT("Enter the command to launch: "),
                                     DIALOG_TITLE,
                                     m_cmdLast);
    if ( cmd.empty() )
        return;

    wxProcess *process = wxProcess::Open(cmd);
    if ( !process )
    {
        wxLogError(wxT("Failed to launch the command."));
        return;
    }

    wxLogVerbose(wxT("PID of the new process: %ld"), process->GetPid());

    wxOutputStream *out = process->GetOutputStream();
    if ( !out )
    {
        wxLogError(wxT("Failed to connect to child stdin"));
        return;
    }

    wxInputStream *in = process->GetInputStream();
    if ( !in )
    {
        wxLogError(wxT("Failed to connect to child stdout"));
        return;
    }

    new MyPipeFrame(this, cmd, process);
}

static wxString gs_lastFile;

static bool AskUserForFileName()
{
    wxString filename;

#if wxUSE_FILEDLG
    filename = wxLoadFileSelector(wxT("any"), wxEmptyString, gs_lastFile);
#else // !wxUSE_FILEDLG
    filename = wxGetTextFromUser(wxT("Enter the file name"), wxT("exec sample"),
                                 gs_lastFile);
#endif // wxUSE_FILEDLG/!wxUSE_FILEDLG

    if ( filename.empty() )
        return false;

    gs_lastFile = filename;

    return true;
}

void MyFrame::OnFileExec(wxCommandEvent& WXUNUSED(event))
{
    if ( !AskUserForFileName() )
        return;

    wxString ext = gs_lastFile.AfterLast(wxT('.'));
    wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension(ext);
    if ( !ft )
    {
        wxLogError(wxT("Impossible to determine the file type for extension '%s'"),
                   ext.c_str());
        return;
    }

    wxString cmd;
    bool ok = ft->GetOpenCommand(&cmd,
                                 wxFileType::MessageParameters(gs_lastFile));
    delete ft;
    if ( !ok )
    {
        wxLogError(wxT("Impossible to find out how to open files of extension '%s'"),
                   ext.c_str());
        return;
    }

    DoAsyncExec(cmd);
}

void MyFrame::OnFileLaunch(wxCommandEvent& WXUNUSED(event))
{
    if ( !AskUserForFileName() )
        return;

    if ( !wxLaunchDefaultApplication(gs_lastFile) )
    {
        wxLogError("Opening \"%s\" in default application failed.", gs_lastFile);
    }
}

void MyFrame::OnOpenURL(wxCommandEvent& WXUNUSED(event))
{
    static wxString s_url(wxT("http://www.wxwidgets.org/"));

    wxString filename = wxGetTextFromUser
                        (
                            wxT("Enter the URL"),
                            wxT("exec sample"),
                            s_url,
                            this
                        );

    if ( filename.empty() )
        return;

    s_url = filename;

    if ( !wxLaunchDefaultBrowser(s_url) )
    {
        wxLogError(wxT("Failed to open URL \"%s\""), s_url.c_str());
    }
}

// ----------------------------------------------------------------------------
// DDE stuff
// ----------------------------------------------------------------------------

#ifdef __WINDOWS__

bool MyFrame::GetDDEServer()
{
    wxString server = wxGetTextFromUser(wxT("Server to connect to:"),
                                        DIALOG_TITLE, m_server);
    if ( !server )
        return false;

    m_server = server;

    wxString topic = wxGetTextFromUser(wxT("DDE topic:"), DIALOG_TITLE, m_topic);
    if ( !topic )
        return false;

    m_topic = topic;

    wxString cmd = wxGetTextFromUser(wxT("DDE command:"), DIALOG_TITLE, m_cmdDde);
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
        wxLogError(wxT("Failed to connect to the DDE server '%s'."),
                   m_server.c_str());
    }
    else
    {
        if ( !conn->Execute(m_cmdDde) )
        {
            wxLogError(wxT("Failed to execute command '%s' via DDE."),
                       m_cmdDde.c_str());
        }
        else
        {
            wxLogStatus(wxT("Successfully executed DDE command"));
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
        wxLogError(wxT("Failed to connect to the DDE server '%s'."),
                   m_server.c_str());
    }
    else
    {
        if ( !conn->Request(m_cmdDde) )
        {
            wxLogError(wxT("Failed to  send request '%s' via DDE."),
                       m_cmdDde.c_str());
        }
        else
        {
            wxLogStatus(wxT("Successfully sent DDE request."));
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

void MyFrame::OnIdleTimer(wxTimerEvent& WXUNUSED(event))
{
    wxWakeUpIdle();
}

void MyFrame::OnBgTimer(wxTimerEvent& WXUNUSED(event))
{
    static unsigned long s_ticks = 0;
    SetStatusText(wxString::Format("%lu ticks", s_ticks++), 1);
}

void MyFrame::OnProcessTerminated(MyPipedProcess *process)
{
    RemovePipedProcess(process);
}

void MyFrame::OnAsyncTermination(MyProcess *process)
{
    m_allAsync.Remove(process);

    delete process;
}

void MyFrame::AddPipedProcess(MyPipedProcess *process)
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
    m_allAsync.Add(process);
}

void MyFrame::RemovePipedProcess(MyPipedProcess *process)
{
    m_running.Remove(process);

    if ( m_running.IsEmpty() )
    {
        // we don't need to get idle events all the time any more
        m_timerIdleWakeUp.Stop();
    }
}

void MyFrame::ShowOutput(const wxString& cmd,
                         const wxArrayString& output,
                         const wxString& title)
{
    size_t count = output.GetCount();
    if ( !count )
        return;

    m_lbox->Append(wxString::Format(wxT("--- %s of '%s' ---"),
                                    title.c_str(), cmd.c_str()));

    for ( size_t n = 0; n < count; n++ )
    {
        m_lbox->Append(output[n]);
    }

    m_lbox->Append(wxString::Format(wxT("--- End of %s ---"),
                                    title.Lower().c_str()));
}

// ----------------------------------------------------------------------------
// MyProcess
// ----------------------------------------------------------------------------

void MyProcess::OnTerminate(int pid, int status)
{
    wxLogStatus(m_parent, wxT("Process %u ('%s') terminated with exit code %d."),
                pid, m_cmd.c_str(), status);

    m_parent->OnAsyncTermination(this);
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
        msg << m_cmd << wxT(" (stdout): ") << tis.ReadLine();

        m_parent->GetLogListBox()->Append(msg);

        hasInput = true;
    }

    if ( IsErrorAvailable() )
    {
        wxTextInputStream tis(*GetErrorStream());

        // this assumes that the output is always line buffered
        wxString msg;
        msg << m_cmd << wxT(" (stderr): ") << tis.ReadLine();

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
        Add(new wxButton(panel, Exec_Btn_Send, wxT("&Send")), 0, wxALL, 5);
    sizerBtns->
        Add(new wxButton(panel, Exec_Btn_SendFile, wxT("&File...")), 0, wxALL, 5);
    sizerBtns->
        Add(new wxButton(panel, Exec_Btn_Get, wxT("&Get")), 0, wxALL, 5);
    sizerBtns->
        Add(new wxButton(panel, Exec_Btn_Close, wxT("&Close")), 0, wxALL, 5);

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
    wxFileDialog filedlg(this, wxT("Select file to send"));
    if ( filedlg.ShowModal() != wxID_OK )
        return;

    wxFFile file(filedlg.GetFilename(), wxT("r"));
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
        buffer[in.Read(buffer, WXSIZEOF(buffer) - 1).LastRead()] = wxT('\0');

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

    wxDELETE(m_process);

    wxLogWarning(wxT("The other process has terminated, closing"));

    DisableInput();
    DisableOutput();
}
