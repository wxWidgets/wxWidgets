/////////////////////////////////////////////////////////////////////////////
// Name:        test.cpp
// Purpose:     wxWindows thread sample
// Author:      Julian Smart(minimal)/Guilhem Lavaux(thread test)
// Modified by:
// Created:     06/16/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Markus Holzem, Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/*
    TODO:

    1. show how SetPriority() works.
    2. use worker threads to update progress controls instead of writing
       messages - it will be more visual
 */

#ifdef __GNUG__
    #pragma implementation "test.cpp"
    #pragma interface "test.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if !wxUSE_THREADS
    #error "This sample requires thread support!"
#endif // wxUSE_THREADS

#include "wx/thread.h"
#include "wx/dynarray.h"
#include "wx/time.h"

// Define a new application type
class MyApp : public wxApp
{
public:
    bool OnInit();
};

class MyThread;
WX_DEFINE_ARRAY(wxThread *, wxArrayThread);

// Define a new frame type
class MyFrame: public wxFrame
{
public:
    // ctor
    MyFrame(wxFrame *frame, const wxString& title, int x, int y, int w, int h);

    // operations
    void WriteText(const wxString& text) { m_txtctrl->WriteText(text); }

    // callbacks
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnClear(wxCommandEvent& event);

    void OnStartThread(wxCommandEvent& event);
    void OnStartThreads(wxCommandEvent& event);
    void OnStopThread(wxCommandEvent& event);
    void OnPauseThread(wxCommandEvent& event);
    void OnResumeThread(wxCommandEvent& event);

    void OnIdle(wxIdleEvent &event);

    // called by dying thread _in_that_thread_context_
    void OnThreadExit(wxThread *thread);

private:
    // helper function - creates a new thread (but doesn't run it)
    MyThread *CreateThread();
    
    // crit section protects access to all of the arrays below
    wxCriticalSection m_critsect;

    // all the threads currently alive - as soon as the thread terminates, it's
    // removed from the array
    wxArrayThread m_threads;

    // just some place to put our messages in
    wxTextCtrl *m_txtctrl;

    // remember the number of running threads and total number of threads
    size_t m_nRunning, m_nCount;

    DECLARE_EVENT_TABLE()
};

class MyThread : public wxThread
{
public:
    MyThread(MyFrame *frame);

    // thread execution starts here
    virtual void *Entry();

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit();

    // write something to the text control
    void WriteText(const wxString& text);

public:
    size_t   m_count;
    MyFrame *m_frame;
};

MyThread::MyThread(MyFrame *frame)
        : wxThread()
{
    m_count = 0;
    m_frame = frame;
}

void MyThread::WriteText(const wxString& text)
{
    wxString msg;

    // before doing any GUI calls we must ensure that this thread is the only
    // one doing it!
    
    wxMutexGuiEnter();
    
    msg << wxTime().FormatTime() << ": " << text;

    m_frame->WriteText(msg);

    wxMutexGuiLeave();
}

void MyThread::OnExit()
{
    m_frame->OnThreadExit(this);
}

void *MyThread::Entry()
{
    wxString text;

    text.Printf("Thread 0x%x started.\n", GetID());
    WriteText(text);

    for ( m_count = 0; m_count < 10; m_count++ )
    {
        // check if we were asked to exit
        if ( TestDestroy() )
            break;

        text.Printf("[%u] Thread 0x%x here.\n", m_count, GetID());
        WriteText(text);

        // wxSleep() can't be called from non-GUI thread!
        wxThread::Sleep(1000);
    }

    text.Printf("Thread 0x%x finished.\n", GetID());
    WriteText(text);

    return NULL;
}

// ID for the menu commands
enum
{
    TEST_QUIT          = 1,
    TEST_TEXT          = 101,
    TEST_ABOUT,
    TEST_CLEAR,
    TEST_START_THREAD  = 201,
    TEST_START_THREADS,
    TEST_STOP_THREAD,
    TEST_PAUSE_THREAD,
    TEST_RESUME_THREAD
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(TEST_QUIT, MyFrame::OnQuit)
    EVT_MENU(TEST_ABOUT, MyFrame::OnAbout)
    EVT_MENU(TEST_CLEAR, MyFrame::OnClear)
    EVT_MENU(TEST_START_THREAD, MyFrame::OnStartThread)
    EVT_MENU(TEST_START_THREADS, MyFrame::OnStartThreads)
    EVT_MENU(TEST_STOP_THREAD, MyFrame::OnStopThread)
    EVT_MENU(TEST_PAUSE_THREAD, MyFrame::OnPauseThread)
    EVT_MENU(TEST_RESUME_THREAD, MyFrame::OnResumeThread)

    EVT_IDLE(MyFrame::OnIdle)
END_EVENT_TABLE()

// Create a new application object
IMPLEMENT_APP  (MyApp)

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit()
{
    // Create the main frame window
    MyFrame *frame = new MyFrame((wxFrame *)NULL, "wxWindows threads sample",
                                 50, 50, 450, 340);

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(TEST_CLEAR, "&Clear log");
    file_menu->AppendSeparator();
    file_menu->Append(TEST_ABOUT, "&About");
    file_menu->AppendSeparator();
    file_menu->Append(TEST_QUIT, "E&xit");
    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, "&File");

    wxMenu *thread_menu = new wxMenu;
    thread_menu->Append(TEST_START_THREAD, "&Start a new thread");
    thread_menu->Append(TEST_START_THREADS, "Start &many threads at once");
    thread_menu->Append(TEST_STOP_THREAD, "S&top a running thread");
    thread_menu->AppendSeparator();
    thread_menu->Append(TEST_PAUSE_THREAD, "&Pause a running thread");
    thread_menu->Append(TEST_RESUME_THREAD, "&Resume suspended thread");
    menu_bar->Append(thread_menu, "&Thread");
    frame->SetMenuBar(menu_bar);

    // Show the frame
    frame->Show(TRUE);

    SetTopWindow(frame);

    return TRUE;
}

// My frame constructor
MyFrame::MyFrame(wxFrame *frame, const wxString& title,
                 int x, int y, int w, int h)
       : wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h))
{
    m_nRunning = m_nCount = 0;

    CreateStatusBar(2);

    m_txtctrl = new wxTextCtrl(this, -1, "", wxPoint(0, 0), wxSize(0, 0),
                               wxTE_MULTILINE | wxTE_READONLY);

}

MyThread *MyFrame::CreateThread()
{
    MyThread *thread = new MyThread(this);

    if ( thread->Create() != wxTHREAD_NO_ERROR )
    {
        wxLogError("Can't create thread!");
    }

    wxCriticalSectionLocker enter(m_critsect);
    m_threads.Add(thread);

    return thread;
}

void MyFrame::OnStartThreads(wxCommandEvent& WXUNUSED(event) )
{
    static wxString s_str;
    s_str = wxGetTextFromUser("How many threads to start: ",
                              "wxThread sample",
                              s_str, this);
    if ( s_str.IsEmpty() )
        return;

    size_t count, n;
    sscanf(s_str, "%u", &count);
    if ( count == 0 )
        return;

    wxArrayThread threads;

    // first create them all...
    for ( n = 0; n < count; n++ )
    {
        threads.Add(CreateThread());
    }

    wxString msg;
    msg.Printf("%d new threads created.", count);
    SetStatusText(msg, 1);

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

    SetStatusText("New thread started.", 1);
}

void MyFrame::OnStopThread(wxCommandEvent& WXUNUSED(event) )
{
    // stop the last thread
    if ( m_threads.IsEmpty() )
    {
        wxLogError("No thread to stop!");
    }
    else
    {
        m_critsect.Enter();

        wxThread *thread = m_threads.Last();

        // it's important to leave critical section before calling Delete()
        // because delete will (implicitly) call OnThreadExit() which also tries
        // to enter the same crit section - would dead lock.
        m_critsect.Leave();

        thread->Delete();

        SetStatusText("Thread stopped.", 1);
    }
}

void MyFrame::OnResumeThread(wxCommandEvent& WXUNUSED(event) )
{
    wxCriticalSectionLocker enter(m_critsect);

    // resume first suspended thread
    size_t n = 0, count = m_threads.Count();
    while ( n < count && !m_threads[n]->IsPaused() )
        n++;

    if ( n == count )
    {
        wxLogError("No thread to resume!");
    }
    else
    {
        m_threads[n]->Resume();

        SetStatusText("Thread resumed.", 1);
    }
}

void MyFrame::OnPauseThread(wxCommandEvent& WXUNUSED(event) )
{
    wxCriticalSectionLocker enter(m_critsect);

    // pause last running thread
    int n = m_threads.Count() - 1;
    while ( n >= 0 && !m_threads[n]->IsRunning() )
        n--;

    if ( n < 0 )
    {
        wxLogError("No thread to pause!");
    }
    else
    {
        m_threads[n]->Pause();
    
        SetStatusText("Thread paused.", 1);
    }
}

// set the frame title indicating the current number of threads
void MyFrame::OnIdle(wxIdleEvent &event)
{
    // update the counts of running/total threads
    size_t nRunning = 0,
           nCount = m_threads.Count();
    for ( size_t n = 0; n < nCount; n++ )
    {
        if ( m_threads[n]->IsRunning() )
            nRunning++;
    }

    if ( nCount != m_nCount || nRunning != m_nRunning )
    {
        m_nRunning = nRunning;
        m_nCount = nCount;

        wxLogStatus(this, "%u threads total, %u running.", nCount, nRunning);
    }
    //else: avoid flicker - don't print anything
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event) )
{
    size_t count = m_threads.Count();
    for ( size_t i = 0; i < count; i++ )
    {
        m_threads[i]->Delete();
    }

    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    wxMessageDialog dialog(this, "wxWindows multithreaded application sample\n"
                                 "(c) 1998 Julian Smart, Guilhem Lavaux\n"
                                 "(c) 1999 Vadim Zeitlin",
                           "About wxThread sample",
                           wxOK | wxICON_INFORMATION);

    dialog.ShowModal();
}

void MyFrame::OnClear(wxCommandEvent& WXUNUSED(event))
{
    m_txtctrl->Clear();
}

void MyFrame::OnThreadExit(wxThread *thread)
{
    wxCriticalSectionLocker enter(m_critsect);

    m_threads.Remove(thread);
}
