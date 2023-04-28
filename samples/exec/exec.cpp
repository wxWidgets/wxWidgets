/////////////////////////////////////////////////////////////////////////////
// Name:        exec.cpp
// Purpose:     exec sample demonstrates wxExecute and related functions
// Author:      Vadim Zeitlin
// Modified by:
// Created:     15.01.00
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

#include "wx/filename.h"
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

#include <memory>
#include <vector>

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

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
    virtual bool OnInit() override;
};

// Define an array of process pointers used by MyFrame
class MyPipedProcess;
using MyPipedProcessesArray = std::vector<MyPipedProcess*>;

class MyProcess;
using MyProcessesArray = std::vector<MyProcess*>;

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
    void OnAsyncExec(wxCommandEvent& event);
    void OnShell(wxCommandEvent& event);
    void OnExecWithRedirect(wxCommandEvent& event);
    void OnExecWithPipe(wxCommandEvent& event);

    void OnPOpen(wxCommandEvent& event);

    void OnFileExec(wxCommandEvent& event);
    void OnFileLaunch(wxCommandEvent& event);
    void OnOpenURL(wxCommandEvent& event);
    void OnShowCommandForExt(wxCommandEvent& event);

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

    int GetExecFlags() const;

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
    wxDECLARE_EVENT_TABLE();
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
        s += '\n';
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

    wxDECLARE_EVENT_TABLE();
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
    virtual void OnTerminate(int pid, int status) override;

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

    virtual void OnTerminate(int pid, int status) override;

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

    virtual bool HasInput() override;

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
    Exec_Kill = 100,
    Exec_ClearLog,
    Exec_BeginBusyCursor,
    Exec_EndBusyCursor,
    Exec_SyncExec = 200,
    Exec_AsyncExec,
    Exec_Shell,
    Exec_POpen,
    Exec_OpenFile,
    Exec_ShowCommandForExt,
    Exec_LaunchFile,
    Exec_OpenURL,
    Exec_DDEExec,
    Exec_DDERequest,
    Exec_Redirect,
    Exec_Pipe,
    Exec_Flags_HideConsole,
    Exec_Flags_ShowConsole,
    Exec_Flags_NoEvents,
    Exec_About = wxID_ABOUT,
    Exec_Quit = wxID_EXIT,

    // control ids
    Exec_Btn_Send = 1000,
    Exec_Btn_SendFile,
    Exec_Btn_Get,
    Exec_Btn_Close
};

static wxString GetDialogTitle()
{
    return "Exec sample";
}

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Exec_Quit,  MyFrame::OnQuit)
    EVT_MENU(Exec_Kill,  MyFrame::OnKill)
    EVT_MENU(Exec_ClearLog,  MyFrame::OnClear)
    EVT_MENU(Exec_BeginBusyCursor,  MyFrame::OnBeginBusyCursor)
    EVT_MENU(Exec_EndBusyCursor,  MyFrame::OnEndBusyCursor)

    EVT_MENU(Exec_SyncExec, MyFrame::OnSyncExec)
    EVT_MENU(Exec_AsyncExec, MyFrame::OnAsyncExec)
    EVT_MENU(Exec_Shell, MyFrame::OnShell)
    EVT_MENU(Exec_Redirect, MyFrame::OnExecWithRedirect)
    EVT_MENU(Exec_Pipe, MyFrame::OnExecWithPipe)

    EVT_MENU(Exec_POpen, MyFrame::OnPOpen)

    EVT_MENU(Exec_OpenFile, MyFrame::OnFileExec)
    EVT_MENU(Exec_ShowCommandForExt, MyFrame::OnShowCommandForExt)
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
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(MyPipeFrame, wxFrame)
    EVT_BUTTON(Exec_Btn_Send, MyPipeFrame::OnBtnSend)
    EVT_BUTTON(Exec_Btn_SendFile, MyPipeFrame::OnBtnSendFile)
    EVT_BUTTON(Exec_Btn_Get, MyPipeFrame::OnBtnGet)
    EVT_BUTTON(Exec_Btn_Close, MyPipeFrame::OnBtnClose)

    EVT_TEXT_ENTER(wxID_ANY, MyPipeFrame::OnTextEnter)

    EVT_CLOSE(MyPipeFrame::OnClose)

    EVT_END_PROCESS(wxID_ANY, MyPipeFrame::OnProcessTerm)
wxEND_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
wxIMPLEMENT_APP(MyApp);

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
    MyFrame *frame = new MyFrame("Exec wxWidgets sample",
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
       : wxFrame(nullptr, wxID_ANY, title, pos, size),
         m_timerIdleWakeUp(this, Exec_TimerIdle),
         m_timerBg(this, Exec_TimerBg)
{
    SetIcon(wxICON(sample));

    m_pidLast = 0;

#ifdef __WXMAC__
    // we need this in order to allow the about menu relocation, since ABOUT is
    // not the default id of the about menu
    wxApp::s_macAboutMenuItemId = Exec_About;
#endif

    // create a menu bar
    wxMenu *menuFile = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
    menuFile->Append(Exec_Kill, "&Kill process...\tCtrl-K",
                     "Kill a process by PID");
    menuFile->AppendSeparator();
    menuFile->Append(Exec_OpenFile, "Open &file...\tCtrl-F",
                     "Launch the command to open this kind of files");
    menuFile->Append(Exec_ShowCommandForExt,
                     "Show association for extension...\tShift-Ctrl-A",
                     "Show the command associated with the given extension");
    menuFile->Append(Exec_LaunchFile, "La&unch file...\tShift-Ctrl-F",
                     "Launch the default application associated with the file");
    menuFile->Append(Exec_OpenURL, "Open &URL...\tCtrl-U",
                     "Launch the default browser with the given URL");
    menuFile->AppendSeparator();
    menuFile->Append(Exec_BeginBusyCursor, "Show &busy cursor\tCtrl-C");
    menuFile->Append(Exec_EndBusyCursor, "Show &normal cursor\tShift-Ctrl-C");
    menuFile->AppendSeparator();
    menuFile->Append(Exec_ClearLog, "&Clear log\tCtrl-L",
                     "Clear the log window");
    menuFile->AppendSeparator();
    menuFile->Append(Exec_Quit, "E&xit\tAlt-X", "Quit this program");

    wxMenu *flagsMenu = new wxMenu;
    flagsMenu->AppendCheckItem(Exec_Flags_HideConsole, "Always &hide console");
    flagsMenu->AppendCheckItem(Exec_Flags_ShowConsole, "Always &show console");
    flagsMenu->AppendCheckItem(Exec_Flags_NoEvents, "Disable &events",
                               "This flag is valid for sync execution only");

    wxMenu *execMenu = new wxMenu;
    execMenu->AppendSubMenu(flagsMenu, "Execution flags");
    execMenu->AppendSeparator();
    execMenu->Append(Exec_SyncExec, "Sync &execution...\tCtrl-E",
                     "Launch a program and return when it terminates");
    execMenu->Append(Exec_AsyncExec, "&Async execution...\tCtrl-A",
                     "Launch a program and return immediately");
    execMenu->Append(Exec_Shell, "Execute &shell command...\tCtrl-S",
                     "Launch a shell and execute a command in it");
    execMenu->AppendSeparator();
    execMenu->Append(Exec_Redirect, "Capture command &output...\tCtrl-O",
                     "Launch a program and capture its output");
    execMenu->Append(Exec_Pipe, "&Pipe through command...",
                     "Pipe a string through a filter");
    execMenu->Append(Exec_POpen, "&Open a pipe to a command...\tCtrl-P",
                     "Open a pipe to and from another program");

#ifdef __WINDOWS__
    execMenu->AppendSeparator();
    execMenu->Append(Exec_DDEExec, "Execute command via &DDE...\tCtrl-D");
    execMenu->Append(Exec_DDERequest, "Send DDE &request...\tCtrl-R");
#endif

    wxMenu *helpMenu = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
    helpMenu->Append(Exec_About, "&About\tF1", "Show about dialog");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, "&File");
    menuBar->Append(execMenu, "&Exec");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    // create the listbox in which we will show misc messages as they come
    m_lbox = new wxListBox(this, wxID_ANY);
    wxFont font(wxFontInfo(12).Family(wxFONTFAMILY_TELETYPE));
    if ( font.IsOk() )
        m_lbox->SetFont(font);

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText("Welcome to wxWidgets exec sample!");
#endif // wxUSE_STATUSBAR

    m_timerBg.Start(1000);
}

MyFrame::~MyFrame()
{
    // any processes left until now must be deleted manually: normally this is
    // done when the associated process terminates but it must be still running
    // if this didn't happen until now
    for ( auto process : m_allAsync )
    {
        delete process;
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
    wxMessageBox("Exec wxWidgets Sample\n(c) 2000-2002 Vadim Zeitlin",
                 "About Exec", wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnKill(wxCommandEvent& WXUNUSED(event))
{
    long pid = wxGetNumberFromUser("Please specify the process to kill",
                                   "Enter PID:",
                                   "Exec question",
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
        "Just test (SIGNONE)",
        "Hangup (SIGHUP)",
        "Interrupt (SIGINT)",
        "Quit (SIGQUIT)",
        "Illegal instruction (SIGILL)",
        "Trap (SIGTRAP)",
        "Abort (SIGABRT)",
        "Emulated trap (SIGEMT)",
        "FP exception (SIGFPE)",
        "Kill (SIGKILL)",
        "Bus (SIGBUS)",
        "Segment violation (SIGSEGV)",
        "System (SIGSYS)",
        "Broken pipe (SIGPIPE)",
        "Alarm (SIGALRM)",
        "Terminate (SIGTERM)",
    };

    static int s_sigLast = wxSIGNONE;
    int sig = wxGetSingleChoiceIndex("How to kill the process?",
                                     "Exec question",
                                     WXSIZEOF(signalNames), signalNames,
                                     s_sigLast,
                                     this);
    switch ( sig )
    {
        default:
            wxFAIL_MSG( "unexpected return value" );
            wxFALLTHROUGH;

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
            wxLogStatus("Process %ld is running.", pid);
        }
        else
        {
            wxLogStatus("No process with pid = %ld.", pid);
        }
    }
    else // not SIGNONE
    {
        wxKillError rc = wxProcess::Kill(pid, (wxSignal)sig);
        if ( rc == wxKILL_OK )
        {
            wxLogStatus("Process %ld killed with signal %d.", pid, sig);
        }
        else
        {
            const wxString errorText[] = { "", // no error
                                     "signal not supported",
                                     "permission denied",
                                     "no such process",
                                     "unspecified error" };

            wxLogStatus("Failed to kill process %ld with signal %d: %s",
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

    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(ExecQueryDialog, wxDialog)
    EVT_UPDATE_UI(TEXT_CWD, ExecQueryDialog::OnUpdateWorkingDirectoryUI)
    EVT_UPDATE_UI(TEXT_ENVIRONMENT, ExecQueryDialog::OnUpdateEnvironmentUI)
wxEND_EVENT_TABLE()

ExecQueryDialog::ExecQueryDialog(const wxString& cmd)
    : wxDialog(nullptr, wxID_ANY, GetDialogTitle(),
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

int MyFrame::GetExecFlags() const
{
    wxMenuBar* const mbar = GetMenuBar();

    int flags = 0;

    if ( mbar->IsChecked(Exec_Flags_HideConsole) )
        flags |= wxEXEC_HIDE_CONSOLE;
    if ( mbar->IsChecked(Exec_Flags_ShowConsole) )
        flags |= wxEXEC_SHOW_CONSOLE;
    if ( mbar->IsChecked(Exec_Flags_NoEvents) )
        flags |= wxEXEC_NOEVENTS;

    return flags;
}

void MyFrame::DoAsyncExec(const wxString& cmd)
{
    MyProcess * const process = new MyProcess(this, cmd);
    m_pidLast = wxExecute(cmd, wxEXEC_ASYNC | GetExecFlags(), process);
    if ( !m_pidLast )
    {
        wxLogError("Execution of '%s' failed.", cmd);

        delete process;
    }
    else
    {
        wxLogStatus("Process %ld (%s) launched.", m_pidLast, cmd);

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

    wxLogStatus( "'%s' is running please wait...", cmd );

    int code = wxExecute(cmd, wxEXEC_SYNC | GetExecFlags(), nullptr, &env);

    wxLogStatus("Process '%s' terminated with exit code %d.",
        cmd, code);

    m_cmdLast = cmd;
}

void MyFrame::OnAsyncExec(wxCommandEvent& WXUNUSED(event))
{
    wxString cmd = wxGetTextFromUser("Enter the command: ",
                                     GetDialogTitle(),
                                     m_cmdLast);

    if ( !cmd )
        return;

    DoAsyncExec(cmd);
}

void MyFrame::OnShell(wxCommandEvent& WXUNUSED(event))
{
    wxString cmd = wxGetTextFromUser("Enter the command: ",
                                     GetDialogTitle(),
                                     m_cmdLast);

    if ( !cmd )
        return;

    int code = wxShell(cmd);
    wxLogStatus("Shell command '%s' terminated with exit code %d.",
                cmd, code);
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

    wxString cmd = wxGetTextFromUser("Enter the command: ",
                                     GetDialogTitle(),
                                     m_cmdLast);

    if ( !cmd )
        return;

    bool sync;
    switch ( wxMessageBox("Execute it synchronously?",
                          "Exec question",
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

        ShowOutput(cmd, output, "Output");
        ShowOutput(cmd, errors, "Errors");
    }
    else // async exec
    {
        MyPipedProcess *process = new MyPipedProcess(this, cmd);
        if ( !wxExecute(cmd, wxEXEC_ASYNC, process) )
        {
            wxLogError("Execution of '%s' failed.", cmd);

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
        m_cmdLast = "tr [a-z] [A-Z]";

    wxString cmd = wxGetTextFromUser("Enter the command: ",
                                     GetDialogTitle(),
                                     m_cmdLast);

    if ( !cmd )
        return;

    wxString input = wxGetTextFromUser("Enter the string to send to it: ",
                                       GetDialogTitle());
    if ( !input )
        return;

    // always execute the filter asynchronously
    MyPipedProcess2 *process = new MyPipedProcess2(this, cmd, input);
    long pid = wxExecute(cmd, wxEXEC_ASYNC, process);
    if ( pid )
    {
        wxLogStatus("Process %ld (%s) launched.", pid, cmd);

        AddPipedProcess(process);
    }
    else
    {
        wxLogError("Execution of '%s' failed.", cmd);

        delete process;
    }

    m_cmdLast = cmd;
}

void MyFrame::OnPOpen(wxCommandEvent& WXUNUSED(event))
{
    wxString cmd = wxGetTextFromUser("Enter the command to launch: ",
                                     GetDialogTitle(),
                                     m_cmdLast);
    if ( cmd.empty() )
        return;

    wxProcess *process = wxProcess::Open(cmd);
    if ( !process )
    {
        wxLogError("Failed to launch the command.");
        return;
    }

    wxLogVerbose("PID of the new process: %ld", process->GetPid());

    wxOutputStream *out = process->GetOutputStream();
    if ( !out )
    {
        wxLogError("Failed to connect to child stdin");
        return;
    }

    wxInputStream *in = process->GetInputStream();
    if ( !in )
    {
        wxLogError("Failed to connect to child stdout");
        return;
    }

    new MyPipeFrame(this, cmd, process);
}

static wxString gs_lastFile;

static bool AskUserForFileName()
{
    wxString filename;

#if wxUSE_FILEDLG
    filename = wxLoadFileSelector("any", wxEmptyString, gs_lastFile);
#else // !wxUSE_FILEDLG
    filename = wxGetTextFromUser("Enter the file name", "exec sample",
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

    wxString ext = wxFileName(gs_lastFile).GetExt();
    wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension(ext);
    if ( !ft )
    {
        wxLogError("Impossible to determine the file type for extension '%s'",
                   ext);
        return;
    }

    wxString cmd;
    bool ok = false;
    const wxFileType::MessageParameters params(gs_lastFile);
#ifdef __WXMSW__
    // try editor, for instance Notepad if extension is .xml
    cmd = ft->GetExpandedCommand("edit", params);
    ok = !cmd.empty();
#endif
    if (!ok) // else try viewer
        ok = ft->GetOpenCommand(&cmd, params);
    delete ft;
    if ( !ok )
    {
        wxLogError("Impossible to find out how to open files of extension '%s'",
                   ext);
        return;
    }

    DoAsyncExec(cmd);
}

void MyFrame::OnShowCommandForExt(wxCommandEvent& WXUNUSED(event))
{
    static wxString s_ext;

    wxString ext = wxGetTextFromUser
                   (
                    "Enter the extension without leading dot",
                    "Exec sample",
                    s_ext,
                    this
                   );
    if ( ext.empty() )
        return;

    s_ext = ext;

    std::unique_ptr<wxFileType>
        ft(wxTheMimeTypesManager->GetFileTypeFromExtension(ext));
    if ( !ft )
    {
        wxLogError("Information for extension \"%s\" not found", ext);
        return;
    }

    const wxString cmd = ft->GetOpenCommand("file." + ext);
    if ( cmd.empty() )
    {
        wxLogWarning("Open command for extension \"%s\" not defined.", ext);
        return;
    }

    wxLogMessage("Open command for files of extension \"%s\" is\n%s",
                 ext, cmd);
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
    static wxString s_url("http://www.wxwidgets.org/");

    wxString filename = wxGetTextFromUser
                        (
                            "Enter the URL",
                            "exec sample",
                            s_url,
                            this
                        );

    if ( filename.empty() )
        return;

    s_url = filename;

    if ( !wxLaunchDefaultBrowser(s_url) )
    {
        wxLogError("Failed to open URL \"%s\"", s_url);
    }
}

// ----------------------------------------------------------------------------
// DDE stuff
// ----------------------------------------------------------------------------

#ifdef __WINDOWS__

bool MyFrame::GetDDEServer()
{
    wxString server = wxGetTextFromUser("Server to connect to:",
                                        GetDialogTitle(), m_server);
    if ( !server )
        return false;

    m_server = server;

    wxString topic = wxGetTextFromUser("DDE topic:", GetDialogTitle(), m_topic);
    if ( !topic )
        return false;

    m_topic = topic;

    wxString cmd = wxGetTextFromUser("DDE command:", GetDialogTitle(), m_cmdDde);
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
        wxLogError("Failed to connect to the DDE server '%s'.",
                   m_server);
    }
    else
    {
        if ( !conn->Execute(m_cmdDde) )
        {
            wxLogError("Failed to execute command '%s' via DDE.",
                       m_cmdDde);
        }
        else
        {
            wxLogStatus("Successfully executed DDE command");
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
        wxLogError("Failed to connect to the DDE server '%s'.",
                   m_server);
    }
    else
    {
        if ( !conn->Request(m_cmdDde) )
        {
            wxLogError("Failed to  send request '%s' via DDE.",
                       m_cmdDde);
        }
        else
        {
            wxLogStatus("Successfully sent DDE request.");
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
    for ( auto process : m_running )
    {
        if ( process->HasInput() )
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
    m_allAsync.erase(std::find(m_allAsync.begin(), m_allAsync.end(), process));

    delete process;
}

void MyFrame::AddPipedProcess(MyPipedProcess *process)
{
    if ( m_running.empty() )
    {
        // we want to start getting the timer events to ensure that a
        // steady stream of idle events comes in -- otherwise we
        // wouldn't be able to poll the child process input
        m_timerIdleWakeUp.Start(100);
    }
    //else: the timer is already running

    m_running.push_back(process);
    m_allAsync.push_back(process);
}

void MyFrame::RemovePipedProcess(MyPipedProcess *process)
{
    m_running.erase(std::find(m_running.begin(), m_running.end(), process));

    if ( m_running.empty() )
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

    m_lbox->Append(wxString::Format("--- %s of '%s' ---",
                                    title, cmd));

    for ( size_t n = 0; n < count; n++ )
    {
        m_lbox->Append(output[n]);
    }

    m_lbox->Append(wxString::Format("--- End of %s ---",
                                    title.Lower()));
}

// ----------------------------------------------------------------------------
// MyProcess
// ----------------------------------------------------------------------------

void MyProcess::OnTerminate(int pid, int status)
{
    wxLogStatus(m_parent, "Process %u ('%s') terminated with exit code %d.",
                pid, m_cmd, status);

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
        msg << m_cmd << " (stdout): " << tis.ReadLine();

        m_parent->GetLogListBox()->Append(msg);

        hasInput = true;
    }

    if ( IsErrorAvailable() )
    {
        wxTextInputStream tis(*GetErrorStream());

        // this assumes that the output is always line buffered
        wxString msg;
        msg << m_cmd << " (stderr): " << tis.ReadLine();

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
             // in a real program we'd check that the streams are non-null here
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
        Add(new wxButton(panel, Exec_Btn_Send, "&Send"), 0, wxALL, 5);
    sizerBtns->
        Add(new wxButton(panel, Exec_Btn_SendFile, "&File..."), 0, wxALL, 5);
    sizerBtns->
        Add(new wxButton(panel, Exec_Btn_Get, "&Get"), 0, wxALL, 5);
    sizerBtns->
        Add(new wxButton(panel, Exec_Btn_Close, "&Close"), 0, wxALL, 5);

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
    wxFileDialog filedlg(this, "Select file to send");
    if ( filedlg.ShowModal() != wxID_OK )
        return;

    wxFFile file(filedlg.GetFilename(), "r");
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
        char buffer[4096];
        buffer[in.Read(buffer, WXSIZEOF(buffer) - 1).LastRead()] = '\0';

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
        m_process = nullptr;
        process->SetNextHandler(nullptr);

        process->CloseOutput();
    }

    event.Skip();
}

void MyPipeFrame::OnProcessTerm(wxProcessEvent& WXUNUSED(event))
{
    DoGet();

    wxDELETE(m_process);

    wxLogWarning("The other process has terminated, closing");

    DisableInput();
    DisableOutput();
}
