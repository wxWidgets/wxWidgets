/////////////////////////////////////////////////////////////////////////////
// Name:        minimal.cpp
// Purpose:     Controls wxWindows sample
// Author:      Robert Roebling
// Modified by:
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "minimal.cpp"
#pragma interface "minimal.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/notebook.h"

//----------------------------------------------------------------------
// class definitions
//----------------------------------------------------------------------

class MyApp: public wxApp
{ 
  public:
    bool OnInit(void);
};

class MyPanel: public wxPanel
{
  public:
 
    MyPanel(wxFrame *frame, int x, int y, int w, int h);
    
    void OnSize( wxSizeEvent& event );
    void OnListBox( wxCommandEvent &event );
    void OnListBoxButtons( wxCommandEvent &event );
    
    wxListBox   *m_listbox;
    
    wxTextCtrl  *m_text;
    wxNotebook  *m_notebook;    
  
  DECLARE_EVENT_TABLE() 
};

class MyFrame: public wxFrame
{
  public:
  
    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
    
  public:
  
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    bool OnClose(void) { return TRUE; }
    
  DECLARE_EVENT_TABLE()    
};

//----------------------------------------------------------------------
// main()
//----------------------------------------------------------------------

IMPLEMENT_APP	(MyApp)

//----------------------------------------------------------------------
// MyApp
//----------------------------------------------------------------------

const   MINIMAL_QUIT   = 100;
const   MINIMAL_TEXT   = 101;
const   MINIMAL_ABOUT  = 102;

bool MyApp::OnInit(void)
{
  // Create the main frame window
  MyFrame *frame = new MyFrame(NULL, "Controls wxWindows App", 50, 50, 500, 420 );
  
  // Give it an icon
#ifdef __WINDOWS__
  frame->SetIcon(wxIcon("mondrian"));
#endif
#ifdef __X__
  frame->SetIcon(wxIcon("aiai.xbm"));
#endif

  wxMenu *file_menu = new wxMenu;

  file_menu->Append(MINIMAL_ABOUT, "&About");
  file_menu->Append(MINIMAL_QUIT, "E&xit");
  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, "&File");
  frame->SetMenuBar(menu_bar);

  frame->Show(TRUE);
  
  SetTopWindow(frame);

  return TRUE;
}

//----------------------------------------------------------------------
// MyPanel
//----------------------------------------------------------------------

const  ID_NOTEBOOK          = 1000;

const  ID_LISTBOX           = 130;
const  ID_LISTBOX_SEL_NUM   = 131;
const  ID_LISTBOX_SEL_STR   = 132;
const  ID_LISTBOX_CLEAR     = 133;
const  ID_LISTBOX_APPEND    = 134;

BEGIN_EVENT_TABLE(MyPanel, wxPanel)
  EVT_SIZE      (                       MyPanel::OnSize)
  EVT_LISTBOX   (ID_LISTBOX,            MyPanel::OnListBox)
  EVT_BUTTON    (ID_LISTBOX_SEL_NUM,    MyPanel::OnListBoxButtons)
  EVT_BUTTON    (ID_LISTBOX_SEL_STR,    MyPanel::OnListBoxButtons)
  EVT_BUTTON    (ID_LISTBOX_CLEAR,      MyPanel::OnListBoxButtons)
  EVT_BUTTON    (ID_LISTBOX_APPEND,     MyPanel::OnListBoxButtons)
END_EVENT_TABLE()

MyPanel::MyPanel( wxFrame *frame, int x, int y, int w, int h ) :
  wxPanel( frame, -1, wxPoint(x, y), wxSize(w, h) )
{
  m_text = new wxTextCtrl( this, -1, "This is the log window.\n", wxPoint(0,50), wxSize(100,50), wxTE_MULTILINE );
  
  m_notebook = new wxNotebook( this, ID_NOTEBOOK, wxPoint(0,0), wxSize(200,150) );
  
  wxString choices[4] =
  {
    "This",
    "is",
    "a",
    "wonderfull example."
  };
  
  wxPanel *panel = m_notebook->CreatePage( 0, "wxList" );
  
  m_listbox = new wxListBox( panel, ID_LISTBOX, wxPoint(10,10), wxSize(120,70), 4, choices );
  (void)new wxButton( panel, ID_LISTBOX_SEL_NUM, "Select #2", wxPoint(140,30), wxSize(100,30) );
  (void)new wxButton( panel, ID_LISTBOX_SEL_STR, "Select 'This'", wxPoint(260,30), wxSize(100,30) );
  (void)new wxButton( panel, ID_LISTBOX_CLEAR, "Clear", wxPoint(140,80), wxSize(100,30) );
  (void)new wxButton( panel, ID_LISTBOX_APPEND, "Append 'Hi!'", wxPoint(260,80), wxSize(100,30) );
  
  m_notebook->CreatePage( 1, "wxChoice" );
}

void MyPanel::OnSize( wxSizeEvent& WXUNUSED(event) )
{
  int x = 0;
  int y = 0;
  GetClientSize( &x, &y );
  
  if (m_notebook) m_notebook->SetSize( 2, 2, x-4, y/2-4 );
  if (m_text) m_text->SetSize( 2, y/2+2, x-4, y/2-4 );
}

void MyPanel::OnListBox( wxCommandEvent &event )
{
  m_text->WriteText( "ListBox Event:\n");
  m_text->WriteText( "ListBox selection string is: " );
  m_text->WriteText( event.GetString() );
  m_text->WriteText( "\n" );
}

void MyPanel::OnListBoxButtons( wxCommandEvent &WXUNUSED(event) )
{
}

//----------------------------------------------------------------------
// MyFrame
//----------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(MINIMAL_QUIT,   MyFrame::OnQuit)
    EVT_MENU(MINIMAL_ABOUT,  MyFrame::OnAbout)
END_EVENT_TABLE()

MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h))
{
  (void*) new MyPanel( this, 10, 10, 300, 100 );
}

void MyFrame::OnQuit (wxCommandEvent& WXUNUSED(event) )
{
  Close(TRUE);
}

void MyFrame::OnAbout( wxCommandEvent& WXUNUSED(event) )
{
  wxMessageDialog dialog(this, "This is a control sample", "About Controls", wxOK );
  dialog.ShowModal();
}


