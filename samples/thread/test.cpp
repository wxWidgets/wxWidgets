/////////////////////////////////////////////////////////////////////////////
// Name:        test.cpp
// Purpose:     wxWindows thread sample
// Author:      Julian Smart(minimal)/Guilhem Lavaux(thread test)
// Modified by:
// Created:     06/16/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Markus Holzem, Guilhem Lavaux
// Licence:   	wxWindows license
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

#include "wx/thread.h"
#include "wx/dynarray.h"

// Define a new application type
class MyApp: public wxApp
{
 public:
    bool OnInit(void);
};

WX_DEFINE_ARRAY(wxThread *,wxArrayThread);

// Define a new frame type
class MyFrame: public wxFrame
{
 public:
    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
    
 public:
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnStartThread(wxCommandEvent& event);
    void OnStopThread(wxCommandEvent& event);
    void OnPauseThread(wxCommandEvent& event);
    bool OnClose(void) { return TRUE; }

 public:
   wxArrayThread m_threads;
   wxTextCtrl *m_txtctrl;

   DECLARE_EVENT_TABLE()
};

class MyThread: public wxThread
{
 public:
    MyThread(MyFrame *frame);

    void *Entry();
 public:
    MyFrame *m_frame;
};

MyThread::MyThread(MyFrame *frame)
  : wxThread()
{
  m_frame = frame;
}

void *MyThread::Entry()
{
  wxString text;

  text.Printf("Thread 0x%x: Hello !\n", GetID());
  DeferDestroy(TRUE);

  while (1) {
    TestDestroy();
    m_frame->m_txtctrl->WriteText(text);
    wxSleep(1);
  }
  
  return NULL;
}

// ID for the menu commands
#define TEST_QUIT 	  1
#define TEST_TEXT 	  101
#define TEST_ABOUT 	  102
#define TEST_START_THREAD 103
#define TEST_STOP_THREAD  104
#define TEST_PAUSE_THREAD 105

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_MENU(TEST_QUIT, MyFrame::OnQuit)
	EVT_MENU(TEST_ABOUT, MyFrame::OnAbout)
	EVT_MENU(TEST_START_THREAD, MyFrame::OnStartThread)
	EVT_MENU(TEST_STOP_THREAD, MyFrame::OnStopThread)
	EVT_MENU(TEST_PAUSE_THREAD, MyFrame::OnPauseThread)
END_EVENT_TABLE()

// Create a new application object
IMPLEMENT_APP	(MyApp)

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit(void)
{
  // Create the main frame window
  MyFrame *frame = new MyFrame(NULL, "Minimal wxWindows App", 50, 50, 450, 340);

  // Give it an icon
#ifdef __WXMSW__
  frame->SetIcon(wxIcon("AIAI"));
#endif
#ifdef __X__
  frame->SetIcon(wxIcon("aiai.xbm"));
#endif

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(TEST_ABOUT, "&About");
  file_menu->Append(TEST_QUIT, "E&xit");
  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, "&File");

  wxMenu *thread_menu = new wxMenu;
  thread_menu->Append(TEST_START_THREAD, "Start a new thread");
  thread_menu->Append(TEST_STOP_THREAD, "Stop a running thread");
  thread_menu->Append(TEST_PAUSE_THREAD, "Pause a running thread");
  menu_bar->Append(thread_menu, "Thread");
  frame->SetMenuBar(menu_bar);

  // Make a panel with a message
  wxPanel *panel = new wxPanel(frame, -1, wxPoint(0, 0), wxSize(400, 200), wxTAB_TRAVERSAL);

  frame->m_txtctrl = new wxTextCtrl(panel, -1, "", wxPoint(10, 10), wxSize(390, 190),
                           wxTE_MULTILINE);

  // Show the frame
  frame->Show(TRUE);
  
  SetTopWindow(frame);

  return TRUE;
}

// My frame constructor
MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h))
{}

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

  delete (m_threads[no_thrd]);
  m_threads.Remove(no_thrd);
}

void MyFrame::OnPauseThread(wxCommandEvent& WXUNUSED(event) )
{}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event) )
{
  uint i;
  for (i=0;i<m_threads.Count();i++)
    delete (m_threads[i]);
  Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
  wxMessageDialog dialog(this, "wxThread sample (based on minimal)\nJulian Smart and Guilhem Lavaux",
  	"About wxThread sample", wxYES_NO|wxCANCEL);

  dialog.ShowModal();
}


