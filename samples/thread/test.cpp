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
    void OnStopThread(wxCommandEvent& event);
    void OnPauseThread(wxCommandEvent& event);
    void OnResumeThread(wxCommandEvent& event);

    void OnIdle(wxIdleEvent &event);
    bool OnClose() { return TRUE; }

    // called by dying thread
    void OnThreadExit(wxThread *thread);

public:
    wxArrayThread m_threads;

private:
    // crit section protects access to the array below
    wxCriticalSection m_critsect;

    // the array of threads which finished (either because they did their work
    // or because they were explicitly stopped)
    wxArrayInt m_aToDelete;

    // just some place to put our messages in
    wxTextCtrl *m_txtctrl;

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
    msg << wxTime().FormatTime() << ": " << text;

    // before doing any GUI calls we must ensure that this thread is the only
    // one doing it!
    wxMutexGuiLocker guiLocker;

    m_frame->WriteText(msg);
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
    TEST_ABOUT         = 102,
    TEST_CLEAR         = 103,
    TEST_START_THREAD  = 203,
    TEST_STOP_THREAD   = 204,
    TEST_PAUSE_THREAD  = 205,
    TEST_RESUME_THREAD = 206
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(TEST_QUIT, MyFrame::OnQuit)
    EVT_MENU(TEST_ABOUT, MyFrame::OnAbout)
    EVT_MENU(TEST_CLEAR, MyFrame::OnClear)
    EVT_MENU(TEST_START_THREAD, MyFrame::OnStartThread)
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
    CreateStatusBar();

    m_txtctrl = new wxTextCtrl(this, -1, "", wxPoint(0, 0), wxSize(0, 0),
                               wxTE_MULTILINE | wxTE_READONLY);

}

void MyFrame::OnStartThread(wxCommandEvent& WXUNUSED(event) )
{
    MyThread *thread = new MyThread(this);

    if ( thread->Create() != wxTHREAD_NO_ERROR )
    {
        wxLogError("Can't create thread!");
    }

    wxCriticalSectionLocker enter(m_critsect);
    m_threads.Add(thread);

    if ( thread->Run() != wxTHREAD_NO_ERROR )
    {
        wxLogError("Can't start thread!");
    }
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
        m_threads.Last()->Delete();
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
        wxLogError("No thread to resume!");
    else
        m_threads[n]->Resume();
}

void MyFrame::OnPauseThread(wxCommandEvent& WXUNUSED(event) )
{
    wxCriticalSectionLocker enter(m_critsect);

    // pause last running thread
    int n = m_threads.Count() - 1;
    while ( n >= 0 && !m_threads[n]->IsRunning() )
        n--;

    if ( n < 0 )
        wxLogError("No thread to pause!");
    else
        m_threads[n]->Pause();
}

// set the frame title indicating the current number of threads
void MyFrame::OnIdle(wxIdleEvent &event)
{
    // first remove from the array all the threads which died since last call
    {
        wxCriticalSectionLocker enter(m_critsect);

        size_t nCount = m_aToDelete.Count();
        for ( size_t n = 0; n < nCount; n++ )
        {
            // index should be shifted by n because we've already deleted 
            // n-1 elements from the array
            m_threads.Remove((size_t)m_aToDelete[n] - n);
        }

        m_aToDelete.Empty();
    }

    size_t nRunning = 0,
           nCount = m_threads.Count();
    for ( size_t n = 0; n < nCount; n++ )
    {
        if ( m_threads[n]->IsRunning() )
            nRunning++;
    }

    wxLogStatus(this, "%u threads total, %u running.", nCount, nRunning);
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
    int index = m_threads.Index(thread);
    wxCHECK_RET( index != -1, "unknown thread being deleted??" );

    wxCriticalSectionLocker enter(m_critsect);

    m_aToDelete.Add(index);
}
