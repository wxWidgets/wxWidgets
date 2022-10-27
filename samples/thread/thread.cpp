/////////////////////////////////////////////////////////////////////////////
// Name:        thread.cpp
// Purpose:     wxWidgets thread sample
// Author:      Guilhem Lavaux, Vadim Zeitlin
// Modified by:
// Created:     06/16/98
// Copyright:   (c) 1998-2009 wxWidgets team
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


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if !wxUSE_THREADS
    #error "This sample requires thread support!"
#endif // wxUSE_THREADS

#include "wx/thread.h"
#include "wx/dynarray.h"
#include "wx/numdlg.h"
#include "wx/progdlg.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

#include "../sample.xpm"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// define this to use wxExecute in the exec tests, otherwise just use system
#define USE_EXECUTE

#ifdef USE_EXECUTE
    #define EXEC(cmd) wxExecute((cmd), wxEXEC_SYNC)
#else
    #define EXEC(cmd) system(cmd)
#endif

class MyThread;
WX_DEFINE_ARRAY_PTR(wxThread *, wxArrayThread);

// ----------------------------------------------------------------------------
// the application object
// ----------------------------------------------------------------------------

class MyApp : public wxApp
{
public:
    MyApp();
    virtual ~MyApp(){}

    virtual bool OnInit() override;

    // critical section protects access to all of the fields below
    wxCriticalSection m_critsect;

    // all the threads currently alive - as soon as the thread terminates, it's
    // removed from the array
    wxArrayThread m_threads;

    // semaphore used to wait for the threads to exit, see MyFrame::OnQuit()
    wxSemaphore m_semAllDone;

    // indicates that we're shutting down and all threads should exit
    bool m_shuttingDown;
};

// ----------------------------------------------------------------------------
// the main application frame
// ----------------------------------------------------------------------------

class MyFrame : public wxFrame,
                private wxLog
{
public:
    // ctor
    MyFrame(const wxString& title);
    virtual ~MyFrame();

    // accessors for MyWorkerThread (called in its context!)
    bool Cancelled();

protected:
    virtual void DoLogRecord(wxLogLevel level,
                             const wxString& msg,
                             const wxLogRecordInfo& info) override;

private:
    // event handlers
    // --------------

    void OnQuit(wxCommandEvent& event);
    void OnClear(wxCommandEvent& event);

    void OnStartThread(wxCommandEvent& event);
    void OnStartThreads(wxCommandEvent& event);
    void OnStopThread(wxCommandEvent& event);
    void OnPauseThread(wxCommandEvent& event);
    void OnResumeThread(wxCommandEvent& event);

    void OnStartWorker(wxCommandEvent& event);
    void OnExecMain(wxCommandEvent& event);
    void OnStartGUIThread(wxCommandEvent& event);

    void OnShowCPUs(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnIdle(wxIdleEvent &event);
    void OnWorkerEvent(wxThreadEvent& event);
    void OnUpdateWorker(wxUpdateUIEvent& event);


    // logging helper
    void DoLogLine(wxTextCtrl *text,
                   const wxString& timestr,
                   const wxString& threadstr,
                   const wxString& msg);


    // thread helper functions
    // -----------------------

    // helper function - creates a new thread (but doesn't run it)
    MyThread *CreateThread();

    // update display in our status bar: called during idle handling
    void UpdateThreadStatus();


    // internal variables
    // ------------------

    // just some place to put our messages in
    wxTextCtrl *m_txtctrl;

    // old log target, we replace it with one using m_txtctrl during this
    // frame life time
    wxLog *m_oldLogger;

    // the array of pending messages to be displayed and the critical section
    // protecting it
    wxArrayString m_messages;
    wxCriticalSection m_csMessages;

    // remember the number of running threads and total number of threads
    size_t m_nRunning,
           m_nCount;

    // the progress dialog which we show while worker thread is running
    wxProgressDialog *m_dlgProgress;

    // was the worker thread cancelled by user?
    bool m_cancelled;
    wxCriticalSection m_csCancelled;        // protects m_cancelled

    wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// ID for the menu commands
enum
{
    THREAD_QUIT  = wxID_EXIT,
    THREAD_ABOUT = wxID_ABOUT,
    THREAD_TEXT          = 101,
    THREAD_CLEAR,
    THREAD_START_THREAD  = 201,
    THREAD_START_THREADS,
    THREAD_STOP_THREAD,
    THREAD_PAUSE_THREAD,
    THREAD_RESUME_THREAD,

    THREAD_START_WORKER,
    THREAD_EXEC_MAIN,
    THREAD_START_GUI_THREAD,

    THREAD_SHOWCPUS,

    WORKER_EVENT = wxID_HIGHEST,   // this one gets sent from MyWorkerThread
    GUITHREAD_EVENT                  // this one gets sent from MyGUIThread
};

// ----------------------------------------------------------------------------
// a simple thread
// ----------------------------------------------------------------------------

class MyThread : public wxThread
{
public:
    MyThread();
    virtual ~MyThread();

    // thread execution starts here
    virtual void *Entry() override;

public:
    unsigned m_count;
};

// ----------------------------------------------------------------------------
// a worker thread
// ----------------------------------------------------------------------------

class MyWorkerThread : public wxThread
{
public:
    MyWorkerThread(MyFrame *frame);

    // thread execution starts here
    virtual void *Entry() override;

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit() override;

public:
    MyFrame *m_frame;
    unsigned m_count;
};

// ----------------------------------------------------------------------------
// a thread which executes GUI calls using wxMutexGuiEnter/Leave
// ----------------------------------------------------------------------------

#define GUITHREAD_BMP_SIZE          300
#define GUITHREAD_NUM_UPDATES       50
class MyImageDialog;

class MyGUIThread : public wxThread
{
public:
    MyGUIThread(MyImageDialog *dlg) : wxThread(wxTHREAD_JOINABLE)
    {
        m_dlg = dlg;
    }

    virtual ExitCode Entry() override;

private:
    MyImageDialog *m_dlg;
};

// ----------------------------------------------------------------------------
// a helper dialog used by MyFrame::OnStartGUIThread
// ----------------------------------------------------------------------------

class MyImageDialog: public wxDialog
{
public:
    // ctor
    MyImageDialog(wxFrame *frame);
    ~MyImageDialog();

    // stuff used by MyGUIThread:
    wxBitmap m_bmp;    // the bitmap drawn by MyGUIThread
    wxCriticalSection m_csBmp;        // protects m_bmp

private:
    void OnGUIThreadEvent(wxThreadEvent& event);
    void OnPaint(wxPaintEvent&);

    MyGUIThread m_thread;
    int m_nCurrentProgress;

    wxDECLARE_EVENT_TABLE();
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// Create a new application object
wxIMPLEMENT_APP(MyApp);

MyApp::MyApp()
{
    m_shuttingDown = false;
}

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // uncomment this to get some debugging messages from the trace code
    // on the console (or just set WXTRACE env variable to include "thread")
    wxLog::AddTraceMask("thread");

    // Create the main frame window
    new MyFrame("wxWidgets threads sample");

    return true;
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(THREAD_QUIT, MyFrame::OnQuit)
    EVT_MENU(THREAD_CLEAR, MyFrame::OnClear)
    EVT_MENU(THREAD_START_THREAD, MyFrame::OnStartThread)
    EVT_MENU(THREAD_START_THREADS, MyFrame::OnStartThreads)
    EVT_MENU(THREAD_STOP_THREAD, MyFrame::OnStopThread)
    EVT_MENU(THREAD_PAUSE_THREAD, MyFrame::OnPauseThread)
    EVT_MENU(THREAD_RESUME_THREAD, MyFrame::OnResumeThread)

    EVT_MENU(THREAD_START_WORKER, MyFrame::OnStartWorker)
    EVT_MENU(THREAD_EXEC_MAIN, MyFrame::OnExecMain)
    EVT_MENU(THREAD_START_GUI_THREAD, MyFrame::OnStartGUIThread)

    EVT_MENU(THREAD_SHOWCPUS, MyFrame::OnShowCPUs)
    EVT_MENU(THREAD_ABOUT, MyFrame::OnAbout)

    EVT_UPDATE_UI(THREAD_START_WORKER, MyFrame::OnUpdateWorker)
    EVT_THREAD(WORKER_EVENT, MyFrame::OnWorkerEvent)
    EVT_IDLE(MyFrame::OnIdle)
wxEND_EVENT_TABLE()

// My frame constructor
MyFrame::MyFrame(const wxString& title)
       : wxFrame(nullptr, wxID_ANY, title)
{
    m_oldLogger = wxLog::GetActiveTarget();

    SetIcon(wxICON(sample));

    // Make a menubar
    wxMenuBar *menuBar = new wxMenuBar;

    wxMenu *menuFile = new wxMenu;
    menuFile->Append(THREAD_CLEAR, "&Clear log\tCtrl-L");
    menuFile->AppendSeparator();
    menuFile->Append(THREAD_QUIT, "E&xit\tAlt-X");
    menuBar->Append(menuFile, "&File");

    wxMenu *menuThread = new wxMenu;
    menuThread->Append(THREAD_START_THREAD, "&Start a new thread\tCtrl-N");
    menuThread->Append(THREAD_START_THREADS, "Start &many threads at once");
    menuThread->Append(THREAD_STOP_THREAD, "S&top the last spawned thread\tCtrl-S");
    menuThread->AppendSeparator();
    menuThread->Append(THREAD_PAUSE_THREAD, "&Pause the last spawned running thread\tCtrl-P");
    menuThread->Append(THREAD_RESUME_THREAD, "&Resume the first suspended thread\tCtrl-R");
    menuThread->AppendSeparator();
    menuThread->Append(THREAD_START_WORKER, "Start a &worker thread\tCtrl-W");
    menuThread->Append(THREAD_EXEC_MAIN, "&Launch a program from main thread\tF5");
    menuThread->Append(THREAD_START_GUI_THREAD, "Launch a &GUI thread\tF6");
    menuBar->Append(menuThread, "&Thread");

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(THREAD_SHOWCPUS, "&Show CPU count");
    menuHelp->AppendSeparator();
    menuHelp->Append(THREAD_ABOUT, "&About");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

    m_nRunning = m_nCount = 0;

    m_dlgProgress = nullptr;

#if wxUSE_STATUSBAR
    CreateStatusBar(2);
#endif // wxUSE_STATUSBAR

    // create the logging text control and a header showing the meaning of the
    // different columns
    wxTextCtrl *header = new wxTextCtrl(this, wxID_ANY, "",
                                        wxDefaultPosition, wxDefaultSize,
                                        wxTE_READONLY);
    DoLogLine(header, "  Time", " Thread", "Message");
    m_txtctrl = new wxTextCtrl(this, wxID_ANY, "",
                               wxDefaultPosition, wxDefaultSize,
                               wxTE_MULTILINE | wxTE_READONLY);
    wxLog::SetActiveTarget(this);

    // use fixed width font to align output in nice columns
    wxFont font(wxFontInfo().Family(wxFONTFAMILY_TELETYPE));
    header->SetFont(font);
    m_txtctrl->SetFont(font);

    m_txtctrl->SetFocus();

    // layout and show the frame
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(header, wxSizerFlags().Expand());
    sizer->Add(m_txtctrl, wxSizerFlags(1).Expand());
    SetSizer(sizer);

    SetSize(600, 350);
    Show();
}

MyFrame::~MyFrame()
{
    wxLog::SetActiveTarget(m_oldLogger);

    // NB: although the OS will terminate all the threads anyhow when the main
    //     one exits, it's good practice to do it ourselves -- even if it's not
    //     completely trivial in this example

    // tell all the threads to terminate: note that they can't terminate while
    // we're deleting them because they will block in their OnExit() -- this is
    // important as otherwise we might access invalid array elements

    {
        wxCriticalSectionLocker locker(wxGetApp().m_critsect);

        // check if we have any threads running first
        const wxArrayThread& threads = wxGetApp().m_threads;
        size_t count = threads.GetCount();

        if ( !count )
            return;

        // set the flag indicating that all threads should exit
        wxGetApp().m_shuttingDown = true;
    }

    // now wait for them to really terminate
    wxGetApp().m_semAllDone.Wait();
}

void
MyFrame::DoLogLine(wxTextCtrl *text,
                   const wxString& timestr,
                   const wxString& threadstr,
                   const wxString& msg)
{
    text->AppendText(wxString::Format("%9s %10s %s", timestr, threadstr, msg));
}

void
MyFrame::DoLogRecord(wxLogLevel level,
                     const wxString& msg,
                     const wxLogRecordInfo& info)
{
    // let the default GUI logger treat warnings and errors as they should be
    // more noticeable than just another line in the log window and also trace
    // messages as there may be too many of them
    if ( level <= wxLOG_Warning || level == wxLOG_Trace )
    {
        m_oldLogger->LogRecord(level, msg, info);
        return;
    }

    DoLogLine
    (
        m_txtctrl,
        wxDateTime(wxLongLong(info.timestampMS)).FormatISOTime(),
        info.threadId == wxThread::GetMainId()
            ? wxString("main")
            : wxString::Format("%lx", info.threadId),
        msg + "\n"
    );
}

MyThread *MyFrame::CreateThread()
{
    MyThread *thread = new MyThread;

    if ( thread->Create() != wxTHREAD_NO_ERROR )
    {
        wxLogError("Can't create thread!");
    }

    wxCriticalSectionLocker enter(wxGetApp().m_critsect);
    wxGetApp().m_threads.Add(thread);

    return thread;
}

void MyFrame::UpdateThreadStatus()
{
    wxCriticalSectionLocker enter(wxGetApp().m_critsect);

    // update the counts of running/total threads
    size_t nRunning = 0,
           nCount = wxGetApp().m_threads.Count();
    for ( size_t n = 0; n < nCount; n++ )
    {
        if ( wxGetApp().m_threads[n]->IsRunning() )
            nRunning++;
    }

    if ( nCount != m_nCount || nRunning != m_nRunning )
    {
        m_nRunning = nRunning;
        m_nCount = nCount;

        wxLogStatus(this, "%u threads total, %u running.", unsigned(nCount), unsigned(nRunning));
    }
    //else: avoid flicker - don't print anything
}

bool MyFrame::Cancelled()
{
    wxCriticalSectionLocker lock(m_csCancelled);

    return m_cancelled;
}

// ----------------------------------------------------------------------------
// MyFrame - event handlers
// ----------------------------------------------------------------------------

void MyFrame::OnStartThreads(wxCommandEvent& WXUNUSED(event) )
{
    static long s_num;

    s_num = wxGetNumberFromUser("How many threads to start: ", "",
                                "wxThread sample", s_num, 1, 10000, this);
    if ( s_num == -1 )
    {
        s_num = 10;

        return;
    }

    unsigned count = unsigned(s_num), n;

    wxArrayThread threads;

    // first create them all...
    for ( n = 0; n < count; n++ )
    {
        wxThread *thr = CreateThread();

        // we want to show the effect of SetPriority(): the first thread will
        // have the lowest priority, the second - the highest, all the rest
        // the normal one
        if ( n == 0 )
            thr->SetPriority(wxPRIORITY_MIN);
        else if ( n == 1 )
            thr->SetPriority(wxPRIORITY_MAX);
        else
            thr->SetPriority(wxPRIORITY_DEFAULT);

        threads.Add(thr);
    }

#if wxUSE_STATUSBAR
    wxString msg;
    msg.Printf("%d new threads created.", count);
    SetStatusText(msg, 1);
#endif // wxUSE_STATUSBAR

    // ...and then start them
    for ( n = 0; n < count; n++ )
    {
        threads[n]->Run();
    }
}

void MyFrame::OnStartThread(wxCommandEvent& WXUNUSED(event) )
{
    MyThread *thread = CreateThread();

    if ( thread->Run() != wxTHREAD_NO_ERROR )
    {
        wxLogError("Can't start thread!");
    }

#if wxUSE_STATUSBAR
    SetStatusText("New thread started.", 1);
#endif // wxUSE_STATUSBAR
}

void MyFrame::OnStopThread(wxCommandEvent& WXUNUSED(event) )
{
    wxThread* toDelete = nullptr;
    {
    wxCriticalSectionLocker enter(wxGetApp().m_critsect);

    // stop the last thread
    if ( wxGetApp().m_threads.IsEmpty() )
    {
        wxLogError("No thread to stop!");
    }
    else
    {
        toDelete = wxGetApp().m_threads.Last();
    }
    }

    if ( toDelete )
    {
        // This can still crash if the thread gets to delete itself
        // in the mean time.
        toDelete->Delete();

#if wxUSE_STATUSBAR
        SetStatusText("Last thread stopped.", 1);
#endif // wxUSE_STATUSBAR
    }
}

void MyFrame::OnResumeThread(wxCommandEvent& WXUNUSED(event) )
{
    wxCriticalSectionLocker enter(wxGetApp().m_critsect);

    // resume first suspended thread
    size_t n = 0, count = wxGetApp().m_threads.Count();
    while ( n < count && !wxGetApp().m_threads[n]->IsPaused() )
        n++;

    if ( n == count )
    {
        wxLogError("No thread to resume!");
    }
    else
    {
        wxGetApp().m_threads[n]->Resume();

#if wxUSE_STATUSBAR
        SetStatusText("Thread resumed.", 1);
#endif // wxUSE_STATUSBAR
    }
}

void MyFrame::OnPauseThread(wxCommandEvent& WXUNUSED(event) )
{
    wxCriticalSectionLocker enter(wxGetApp().m_critsect);

    // pause last running thread
    int n = wxGetApp().m_threads.Count() - 1;
    while ( n >= 0 && !wxGetApp().m_threads[n]->IsRunning() )
        n--;

    if ( n < 0 )
    {
        wxLogError("No thread to pause!");
    }
    else
    {
        wxGetApp().m_threads[n]->Pause();

#if wxUSE_STATUSBAR
        SetStatusText("Thread paused.", 1);
#endif // wxUSE_STATUSBAR
    }
}

void MyFrame::OnIdle(wxIdleEvent& event)
{
    UpdateThreadStatus();

    event.Skip();
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event) )
{
    Close(true);
}

void MyFrame::OnExecMain(wxCommandEvent& WXUNUSED(event))
{
    wxString cmd = wxGetTextFromUser("Please enter the command to execute",
                                     "Enter command",
#ifdef __WXMSW__
                                     "notepad",
#else
                                     "/bin/echo \"Message from another process\"",
#endif
                                     this);
    if (cmd.IsEmpty())
        return;     // user clicked cancel

    wxLogMessage("The exit code from the main program is %ld",
                 EXEC(cmd));
}

void MyFrame::OnShowCPUs(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;

    int nCPUs = wxThread::GetCPUCount();
    switch ( nCPUs )
    {
        case -1:
            msg = "Unknown number of CPUs";
            break;

        case 0:
            msg = "WARNING: you're running without any CPUs!";
            break;

        case 1:
            msg = "This system only has one CPU.";
            break;

        default:
            msg.Printf("This system has %d CPUs", nCPUs);
    }

    wxLogMessage(msg);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    wxMessageDialog dialog(this,
                           "wxWidgets multithreaded application sample\n"
                           "(c) 1998 Julian Smart, Guilhem Lavaux\n"
                           "(c) 2000 Robert Roebling\n"
                           "(c) 1999,2009 Vadim Zeitlin",
                           "About wxThread sample",
                           wxOK | wxICON_INFORMATION);

    dialog.ShowModal();
}

void MyFrame::OnClear(wxCommandEvent& WXUNUSED(event))
{
    m_txtctrl->Clear();
}

void MyFrame::OnUpdateWorker(wxUpdateUIEvent& event)
{
    event.Enable( m_dlgProgress == nullptr );
}

void MyFrame::OnStartWorker(wxCommandEvent& WXUNUSED(event))
{
    MyWorkerThread *thread = new MyWorkerThread(this);

    if ( thread->Create() != wxTHREAD_NO_ERROR )
    {
        wxLogError("Can't create thread!");
        return;
    }

    m_dlgProgress = new wxProgressDialog
                        (
                         "Progress dialog",
                         "Wait until the thread terminates or press [Cancel]",
                         100,
                         this,
                         wxPD_CAN_ABORT |
                         wxPD_APP_MODAL |
                         wxPD_ELAPSED_TIME |
                         wxPD_ESTIMATED_TIME |
                         wxPD_REMAINING_TIME
                        );

    // thread is not running yet, no need for crit sect
    m_cancelled = false;

    thread->Run();
}

void MyFrame::OnWorkerEvent(wxThreadEvent& event)
{
    int n = event.GetInt();
    if ( n == -1 )
    {
        m_dlgProgress->Destroy();
        m_dlgProgress = nullptr;

        // the dialog is aborted because the event came from another thread, so
        // we may need to wake up the main event loop for the dialog to be
        // really closed
        wxWakeUpIdle();
    }
    else
    {
        if ( !m_dlgProgress->Update(n) )
        {
            wxCriticalSectionLocker lock(m_csCancelled);

            m_cancelled = true;
        }
    }
}

void MyFrame::OnStartGUIThread(wxCommandEvent& WXUNUSED(event))
{
    // we use this to check that disabling logging only affects the main thread
    // but the messages from the worker thread will still be logged
    wxLogNull noLog;
    wxLogMessage("You shouldn't see this message because of wxLogNull");

    MyImageDialog dlg(this);

    dlg.ShowModal();
}


// ----------------------------------------------------------------------------
// MyImageDialog
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyImageDialog, wxDialog)
    EVT_THREAD(GUITHREAD_EVENT, MyImageDialog::OnGUIThreadEvent)
    EVT_PAINT(MyImageDialog::OnPaint)
wxEND_EVENT_TABLE()

MyImageDialog::MyImageDialog(wxFrame *parent)
    : wxDialog(parent, wxID_ANY, "Image created by a secondary thread",
               wxDefaultPosition, wxSize(GUITHREAD_BMP_SIZE,GUITHREAD_BMP_SIZE)*1.5, wxDEFAULT_DIALOG_STYLE),
      m_thread(this)
{
    m_nCurrentProgress = 0;

    CentreOnScreen();

    // NOTE: no need to lock m_csBmp until the thread isn't started:

    // create the bitmap
    if (!m_bmp.Create(GUITHREAD_BMP_SIZE,GUITHREAD_BMP_SIZE) || !m_bmp.IsOk())
    {
        wxLogError("Couldn't create the bitmap!");
        return;
    }

    // clean it
    wxMemoryDC dc(m_bmp);
    dc.SetBackground(*wxBLACK_BRUSH);
    dc.Clear();

    // draw the bitmap from a secondary thread
    if ( m_thread.Create() != wxTHREAD_NO_ERROR ||
         m_thread.Run() != wxTHREAD_NO_ERROR )
    {
        wxLogError("Can't create/run thread!");
        return;
    }
}

MyImageDialog::~MyImageDialog()
{
    // in case our thread is still running and for some reason we are destroyed,
    // do wait for the thread to complete as it assumes that its MyImageDialog
    // pointer is always valid
    m_thread.Delete();
}

void MyImageDialog::OnGUIThreadEvent(wxThreadEvent& event)
{
    m_nCurrentProgress = int(((float)event.GetInt()*100)/GUITHREAD_NUM_UPDATES);

    Refresh();
}

void MyImageDialog::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
    wxPaintDC dc(this);

    const wxSize& sz = dc.GetSize();

    {
        // paint the bitmap
        wxCriticalSectionLocker locker(m_csBmp);
        dc.DrawBitmap(m_bmp, (sz.GetWidth()-GUITHREAD_BMP_SIZE)/2,
                             (sz.GetHeight()-GUITHREAD_BMP_SIZE)/2);
    }

    // paint a sort of progress bar with a 10px border:
    dc.SetBrush(*wxRED_BRUSH);
    dc.DrawRectangle(10,10, m_nCurrentProgress*(sz.GetWidth()-20)/100,30);
    dc.SetTextForeground(*wxBLUE);
    dc.DrawText(wxString::Format("%d%%", m_nCurrentProgress),
                (sz.GetWidth()-dc.GetCharWidth()*2)/2,
                25-dc.GetCharHeight()/2);
}

// ----------------------------------------------------------------------------
// MyThread
// ----------------------------------------------------------------------------

MyThread::MyThread()
        : wxThread()
{
    m_count = 0;
}

MyThread::~MyThread()
{
    wxCriticalSectionLocker locker(wxGetApp().m_critsect);

    wxArrayThread& threads = wxGetApp().m_threads;
    threads.Remove(this);

    if ( threads.IsEmpty() )
    {
        // signal the main thread that there are no more threads left if it is
        // waiting for us
        if ( wxGetApp().m_shuttingDown )
        {
            wxGetApp().m_shuttingDown = false;

            wxGetApp().m_semAllDone.Post();
        }
    }
}

wxThread::ExitCode MyThread::Entry()
{
    // setting thread name helps with debugging, as the debugger
    // may be able to show thread names along with the list of threads.
    SetName("My Thread");

    wxLogMessage("Thread started (priority = %u).", GetPriority());

    for ( m_count = 0; m_count < 10; m_count++ )
    {
        // check if the application is shutting down: in this case all threads
        // should stop a.s.a.p.
        {
            wxCriticalSectionLocker locker(wxGetApp().m_critsect);
            if ( wxGetApp().m_shuttingDown )
                return nullptr;
        }

        // check if just this thread was asked to exit
        if ( TestDestroy() )
            break;

        wxLogMessage("Thread progress: %u", m_count);

        // wxSleep() can't be called from non-GUI thread!
        wxThread::Sleep(1000);
    }

    wxLogMessage("Thread finished.");

    return nullptr;
}


// ----------------------------------------------------------------------------
// MyWorkerThread
// ----------------------------------------------------------------------------

// define this symbol to 1 to test if the YieldFor() call in the wxProgressDialog::Update
// function provokes a race condition in which the second wxThreadEvent posted by
// MyWorkerThread::Entry is processed by the YieldFor() call of wxProgressDialog::Update
// and results in the destruction of the progress dialog itself, resulting in a crash later.
#define TEST_YIELD_RACE_CONDITION       0

MyWorkerThread::MyWorkerThread(MyFrame *frame)
        : wxThread()
{
    m_frame = frame;
    m_count = 0;
}

void MyWorkerThread::OnExit()
{
}

wxThread::ExitCode MyWorkerThread::Entry()
{
    // setting thread name helps with debugging, as the debugger
    // may be able to show thread names along with the list of threads.
    SetName("Worker Thread");

#if TEST_YIELD_RACE_CONDITION
    if ( TestDestroy() )
        return nullptr;

    wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT );

    event.SetInt( 50 );
    wxQueueEvent( m_frame, event.Clone() );

    event.SetInt(-1);
    wxQueueEvent( m_frame, event.Clone() );
#else
    for ( m_count = 0; !m_frame->Cancelled() && (m_count < 100); m_count++ )
    {
        // check if we were asked to exit
        if ( TestDestroy() )
            break;

        // create any type of command event here
        wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT );
        event.SetInt( m_count );

        // send in a thread-safe way
        wxQueueEvent( m_frame, event.Clone() );

        wxMilliSleep(200);
    }

    wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT );
    event.SetInt(-1); // that's all
    wxQueueEvent( m_frame, event.Clone() );
#endif

    return nullptr;
}


// ----------------------------------------------------------------------------
// MyGUIThread
// ----------------------------------------------------------------------------

wxThread::ExitCode MyGUIThread::Entry()
{
    // uncomment this to check that disabling logging here does disable it for
    // this thread -- but not the main one if you also comment out wxLogNull
    // line in MyFrame::OnStartGUIThread()
    //wxLogNull noLog;

    // this goes to the main window
    wxLogMessage("GUI thread starting");

    // use a thread-specific log target for this thread to show that its
    // messages don't appear in the main window while it runs
    wxLogBuffer logBuf;
    wxLog::SetThreadActiveTarget(&logBuf);

    for (int i=0; i<GUITHREAD_NUM_UPDATES && !TestDestroy(); i++)
    {
        // inform the GUI toolkit that we're going to use GUI functions
        // from a secondary thread:
        wxMutexGuiEnter();

        {
            wxCriticalSectionLocker lock(m_dlg->m_csBmp);

            // draw some more stuff on the bitmap
            wxMemoryDC dc(m_dlg->m_bmp);
            dc.SetBrush((i%2)==0 ? *wxBLUE_BRUSH : *wxGREEN_BRUSH);
            dc.DrawRectangle(rand()%GUITHREAD_BMP_SIZE, rand()%GUITHREAD_BMP_SIZE, 30, 30);

            // simulate long drawing time:
            wxMilliSleep(200);
        }

        // if we don't release the GUI mutex the MyImageDialog won't be able to refresh
        wxMutexGuiLeave();

        // notify the dialog that another piece of our masterpiece is complete:
        wxThreadEvent event( wxEVT_THREAD, GUITHREAD_EVENT );
        event.SetInt(i+1);
        wxQueueEvent( m_dlg, event.Clone() );

        if ( !((i + 1) % 10) )
        {
            // this message will go to the buffer
            wxLogMessage("Step #%d.", i + 1);
        }

        // give the main thread the time to refresh before we lock the GUI mutex again
        // FIXME: find a better way to do this!
        wxMilliSleep(100);
    }

    // now remove the thread-specific thread target
    wxLog::SetThreadActiveTarget(nullptr);

    // so that this goes to the main window again
    wxLogMessage("GUI thread finished.");

    return (ExitCode)0;
}
