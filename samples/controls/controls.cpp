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

#ifdef __WXGTK__
#include "mondrian.xpm"
#endif

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
    void OnChoice( wxCommandEvent &event );
    void OnChoiceButtons( wxCommandEvent &event );
    void OnCombo( wxCommandEvent &event );
    void OnComboButtons( wxCommandEvent &event );
    void OnRadio( wxCommandEvent &event );
    void OnRadioButtons( wxCommandEvent &event );
    
    wxListBox   *m_listbox;
    wxChoice    *m_choice;
    wxComboBox  *m_combo;
    wxRadioBox  *m_radio;
    
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
#ifdef __WXMSW__
  frame->SetIcon(wxIcon("mondrian"));
#else
  frame->SetIcon(wxIcon( mondrian_xpm ));
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

const  ID_CHOICE            = 120;
const  ID_CHOICE_SEL_NUM    = 121;
const  ID_CHOICE_SEL_STR    = 122;
const  ID_CHOICE_CLEAR      = 123;
const  ID_CHOICE_APPEND     = 124;

const  ID_COMBO             = 140;
const  ID_COMBO_SEL_NUM     = 141;
const  ID_COMBO_SEL_STR     = 142;
const  ID_COMBO_CLEAR       = 143;
const  ID_COMBO_APPEND      = 144;

const  ID_TEXT              = 150;

const  ID_RADIOBOX          = 160;
const  ID_RADIOBOX_SEL_NUM  = 161;
const  ID_RADIOBOX_SEL_STR  = 162;

BEGIN_EVENT_TABLE(MyPanel, wxPanel)
  EVT_SIZE      (                       MyPanel::OnSize)
  EVT_LISTBOX   (ID_LISTBOX,            MyPanel::OnListBox)
  EVT_BUTTON    (ID_LISTBOX_SEL_NUM,    MyPanel::OnListBoxButtons)
  EVT_BUTTON    (ID_LISTBOX_SEL_STR,    MyPanel::OnListBoxButtons)
  EVT_BUTTON    (ID_LISTBOX_CLEAR,      MyPanel::OnListBoxButtons)
  EVT_BUTTON    (ID_LISTBOX_APPEND,     MyPanel::OnListBoxButtons)
  EVT_CHOICE    (ID_CHOICE,             MyPanel::OnChoice)
  EVT_BUTTON    (ID_CHOICE_SEL_NUM,     MyPanel::OnChoiceButtons)
  EVT_BUTTON    (ID_CHOICE_SEL_STR,     MyPanel::OnChoiceButtons)
  EVT_BUTTON    (ID_CHOICE_CLEAR,       MyPanel::OnChoiceButtons)
  EVT_BUTTON    (ID_CHOICE_APPEND,      MyPanel::OnChoiceButtons)
  EVT_CHOICE    (ID_COMBO,              MyPanel::OnCombo)
  EVT_BUTTON    (ID_COMBO_SEL_NUM,      MyPanel::OnComboButtons)
  EVT_BUTTON    (ID_COMBO_SEL_STR,      MyPanel::OnComboButtons)
  EVT_BUTTON    (ID_COMBO_CLEAR,        MyPanel::OnComboButtons)
  EVT_BUTTON    (ID_COMBO_APPEND,       MyPanel::OnComboButtons)
  EVT_RADIOBOX  (ID_RADIOBOX,           MyPanel::OnRadio)
  EVT_BUTTON    (ID_RADIOBOX_SEL_NUM,   MyPanel::OnRadioButtons)
  EVT_BUTTON    (ID_RADIOBOX_SEL_STR,   MyPanel::OnRadioButtons)
END_EVENT_TABLE()

MyPanel::MyPanel( wxFrame *frame, int x, int y, int w, int h ) :
  wxPanel( frame, -1, wxPoint(x, y), wxSize(w, h) )
{
  m_text = new wxTextCtrl( this, -1, "This is the log window.\n", wxPoint(0,50), wxSize(100,50), wxTE_MULTILINE );
  
  m_notebook = new wxNotebook( this, ID_NOTEBOOK, wxPoint(0,0), wxSize(200,150) );
  
  wxString choices[] =
  {
    "This",
    "is a",
    "wonderful",
    "example.",
  };
  
  wxPanel *panel = new wxPanel(m_notebook);
  m_listbox = new wxListBox( panel, ID_LISTBOX, wxPoint(10,10), wxSize(120,70), 4, choices );
  (void)new wxButton( panel, ID_LISTBOX_SEL_NUM, "Select #2", wxPoint(180,30), wxSize(100,30) );
  (void)new wxButton( panel, ID_LISTBOX_SEL_STR, "Select 'This'", wxPoint(300,30), wxSize(100,30) );
  (void)new wxButton( panel, ID_LISTBOX_CLEAR, "Clear", wxPoint(180,80), wxSize(100,30) );
  (void)new wxButton( panel, ID_LISTBOX_APPEND, "Append 'Hi!'", wxPoint(300,80), wxSize(100,30) );
  m_notebook->AddPage(panel, "wxList");
  
  panel = new wxPanel(m_notebook);
  m_choice = new wxChoice( panel, ID_CHOICE, wxPoint(10,10), wxSize(120,-1), 4, choices );
  (void)new wxButton( panel, ID_CHOICE_SEL_NUM, "Select #2", wxPoint(180,30), wxSize(100,30) );
  (void)new wxButton( panel, ID_CHOICE_SEL_STR, "Select 'This'", wxPoint(300,30), wxSize(100,30) );
  (void)new wxButton( panel, ID_CHOICE_CLEAR, "Clear", wxPoint(180,80), wxSize(100,30) );
  (void)new wxButton( panel, ID_CHOICE_APPEND, "Append 'Hi!'", wxPoint(300,80), wxSize(100,30) );
  m_notebook->AddPage(panel, "wxChoice");
  
  panel = new wxPanel(m_notebook);
  m_combo = new wxComboBox( panel, ID_COMBO, "This", wxPoint(10,10), wxSize(170,-1), 4, choices );
  (void)new wxButton( panel, ID_COMBO_SEL_NUM, "Select #2", wxPoint(180,30), wxSize(100,30) );
  (void)new wxButton( panel, ID_COMBO_SEL_STR, "Select 'This'", wxPoint(300,30), wxSize(100,30) );
  (void)new wxButton( panel, ID_COMBO_CLEAR, "Clear", wxPoint(180,80), wxSize(100,30) );
  (void)new wxButton( panel, ID_COMBO_APPEND, "Append 'Hi!'", wxPoint(300,80), wxSize(100,30) );
  m_notebook->AddPage(panel, "wxComboBox");
  
  wxTextCtrl *text = new wxTextCtrl( m_notebook, ID_TEXT, "Write text here.", wxPoint(10,10), wxSize(120,100), wxTE_MULTILINE );
  m_notebook->AddPage( text, "wxTextCtrl" );
  
  panel = new wxPanel(m_notebook);
  m_radio = new wxRadioBox( panel, ID_RADIOBOX, "This", wxPoint(10,10), wxSize(-1,-1), 4, choices );
  (void)new wxButton( panel, ID_RADIOBOX_SEL_NUM, "Select #2", wxPoint(200,30), wxSize(100,30) );
  (void)new wxButton( panel, ID_RADIOBOX_SEL_STR, "Select 'This'", wxPoint(200,80), wxSize(100,30) );
  m_notebook->AddPage(panel, "wxRadioBox");
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
  m_text->WriteText( "ListBox selection string is: " );
  m_text->WriteText( event.GetString() );
  m_text->WriteText( "\n" );
}

void MyPanel::OnListBoxButtons( wxCommandEvent &event )
{
  switch (event.GetId())
  {
    case ID_LISTBOX_SEL_NUM:
    {
      m_listbox->SetSelection( 2 );
      break;
    }
    case ID_LISTBOX_SEL_STR:
    {
      m_listbox->SetStringSelection( "This" );
      break;
    }
    case ID_LISTBOX_CLEAR:
    {
      m_listbox->Clear();
      break;
    }
    case ID_LISTBOX_APPEND:
    {
      m_listbox->Append( "Hi!" );
      break;
    }
  }
}

void MyPanel::OnChoice( wxCommandEvent &event )
{
  m_text->WriteText( "Choice selection string is: " );
  m_text->WriteText( event.GetString() );
  m_text->WriteText( "\n" );
}

void MyPanel::OnChoiceButtons( wxCommandEvent &event )
{
  switch (event.GetId())
  {
    case ID_CHOICE_SEL_NUM:
    {
      m_choice->SetSelection( 2 );
      break;
    }
    case ID_CHOICE_SEL_STR:
    {
      m_choice->SetStringSelection( "This" );
      break;
    }
    case ID_CHOICE_CLEAR:
    {
      m_choice->Clear();
      break;
    }
    case ID_CHOICE_APPEND:
    {
      m_choice->Append( "Hi!" );
      break;
    }
  }
}

void MyPanel::OnCombo( wxCommandEvent &event )
{
  m_text->WriteText( "ComboBox selection string is: " );
  m_text->WriteText( event.GetString() );
  m_text->WriteText( "\n" );
}

void MyPanel::OnComboButtons( wxCommandEvent &event )
{
  switch (event.GetId())
  {
    case ID_COMBO_SEL_NUM:
    {
      m_combo->SetSelection( 2 );
      break;
    }
    case ID_COMBO_SEL_STR:
    {
      m_combo->SetStringSelection( "This" );
      break;
    }
    case ID_COMBO_CLEAR:
    {
      m_combo->Clear();
      break;
    }
    case ID_COMBO_APPEND:
    {
      m_combo->Append( "Hi!" );
      break;
    }
  }
}

void MyPanel::OnRadio( wxCommandEvent &event )
{
  m_text->WriteText( "RadioBox selection string is: " );
  m_text->WriteText( event.GetString() );
  m_text->WriteText( "\n" );
}

void MyPanel::OnRadioButtons( wxCommandEvent &event )
{
  switch (event.GetId())
  {
    case ID_RADIOBOX_SEL_NUM:
    {
      m_radio->SetSelection( 2 );
      break;
    }
    case ID_RADIOBOX_SEL_STR:
    {
      m_radio->SetStringSelection( "This" );
      break;
    }
  }
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


