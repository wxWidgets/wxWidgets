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
class MyApp: public wxApp
{
public:
    bool OnInit(void);
};

WX_DEFINE_ARRAY(wxThread *, wxArrayThread);

// Define a new frame type
class MyFrame: public wxFrame
{
public:
    // ctor
    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);

    // operations
    void WriteText(const wxString& text) { m_txtctrl->WriteText(text); }

    // callbacks
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnStartThread(wxCommandEvent& event);
    void OnStopThread(wxCommandEvent& event);
    void OnPauseThread(wxCommandEvent& event);
    void OnResumeThread(wxCommandEvent& event);

    void OnSize(wxSizeEvent &event);
    void OnIdle(wxIdleEvent &event);
    bool OnClose() { return TRUE; }

public:
    wxArrayThread m_threads;

private:
    wxTextCtrl *m_txtctrl;

    DECLARE_EVENT_TABLE()
};

class MyThread: public wxThread
{
public:
    MyThread(MyFrame *frame);

    // thread execution starts here
    virtual void *Entry();

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
    wxMutexGuiEnter();
    m_frame->WriteText(msg);
    wxMutexGuiLeave();
}

void *MyThread::Entry()
{
    wxString text;

    DeferDestroy(TRUE);

    text.Printf("Thread 0x%x started.\n", GetID());
    WriteText(text);

    for ( m_count = 0; m_count < 20; m_count++ )
    {
        // check if we were asked to exit
        if ( TestDestroy() )
            break;

        text.Printf("[%u] Thread 0x%x here.\n", m_count, GetID());
        WriteText(text);

        wxSleep(1);
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
    TEST_START_THREAD  = 103,
    TEST_STOP_THREAD   = 104,
    TEST_PAUSE_THREAD  = 105,
    TEST_RESUME_THREAD = 106
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(TEST_QUIT, MyFrame::OnQuit)
    EVT_MENU(TEST_ABOUT, MyFrame::OnAbout)
    EVT_MENU(TEST_START_THREAD, MyFrame::OnStartThread)
    EVT_MENU(TEST_STOP_THREAD, MyFrame::OnStopThread)
    EVT_MENU(TEST_PAUSE_THREAD, MyFrame::OnPauseThread)
    EVT_MENU(TEST_RESUME_THREAD, MyFrame::OnResumeThread)

    EVT_SIZE(MyFrame::OnSize)

    EVT_IDLE(MyFrame::OnIdle)
END_EVENT_TABLE()

// Create a new application object
IMPLEMENT_APP  (MyApp)

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit()
{
    // Create the main frame window
    MyFrame *frame = new MyFrame((wxFrame *)NULL, "", 50, 50, 450, 340);

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(TEST_ABOUT, "&About");
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
MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h)
       : wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h))
{
    wxPanel *panel = new wxPanel(this, -1, wxPoint(0, 0), wxSize(400, 200),
                                 wxTAB_TRAVERSAL);

    m_txtctrl = new wxTextCtrl(panel, -1, "", wxPoint(10,30), wxSize(390, 190),
                               wxTE_MULTILINE);

    (void)new wxStaticText(panel, -1, "Log window", wxPoint(10,10));
}

void MyFrame::OnStartThread(wxCommandEvent& WXUNUSED(event) )
{
    MyThread *thread = new MyThread(this);

    thread->Create();

    m_threads.Add(thread);
}

void MyFrame::OnStopThread(wxCommandEvent& WXUNUSED(event) )
{
    int no_thrd = m_threads.Count()-1;

    if (no_thrd < 0)
        return;

    delete m_threads[no_thrd];
    m_threads.Remove(no_thrd);
}

void MyFrame::OnResumeThread(wxCommandEvent& WXUNUSED(event) )
{
    // resume first suspended thread
    size_t n = 0;
    while ( n < m_threads.Count() && m_threads[n]->IsPaused() )
        n--;

    if ( n < 0 )
        wxLogError("No thread to pause!");
    else
        m_threads[n]->Resume();
}

void MyFrame::OnPauseThread(wxCommandEvent& WXUNUSED(event) )
{
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
    size_t nRunning = 0,
           nCount = m_threads.Count();
    for ( size_t n = 0; n < nCount; n++ )
    {
        if ( m_threads[n]->IsRunning() )
            nRunning++;
    }

    wxString title;
    title.Printf("wxWindows thread sample (%u threads, %u running).",
                 nCount, nRunning);
    SetTitle(title);
}

void MyFrame::OnSize(wxSizeEvent& event)
{
    wxFrame::OnSize(event);

    wxSize size( GetClientSize() );

    m_txtctrl->SetSize( 10, 30, size.x-20, size.y-40 );
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event) )
{
    for ( size_t i = 0; i < m_threads.Count(); i++ )
        delete m_threads[i];

    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    wxMessageDialog dialog(this, "wxThread sample (based on minimal)\n"
                                 "Julian Smart and Guilhem Lavaux",
                           "About wxThread sample",
                           wxOK | wxICON_INFORMATION);

    dialog.ShowModal();
}


