/*
 * Program: wxTest
 * 
 * Author: Robert Roebling
 *
 * Copyright: (C) 1997, GNU (Robert Roebling)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef __GNUG__
#pragma implementation "wxTest.h"
#endif

#include "wxTest.h"

#include "folder.xpm"
#include "list.xpm"

//-----------------------------------------------------------------------------
// main program
//-----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

//-----------------------------------------------------------------------------
// MyDialog
//-----------------------------------------------------------------------------

const  ID_RETURN            = 100;
const  ID_HELLO             = 101;

const  ID_CHECKBOX          = 110;
const  ID_CHECKBOX_CHECK    = 110;
const  ID_CHECKBOX_UNCHECK  = 112;

const  ID_TEXTCTRL          = 115;
const  ID_TEXTCTRL_SET      = 116;
const  ID_TEXTCTRL_DEL      = 117;

const  ID_CHOICE            = 120;
const  ID_CHOICE_SEL_NUM    = 121;
const  ID_CHOICE_SEL_STR    = 122;
const  ID_CHOICE_CLEAR      = 123;
const  ID_CHOICE_APPEND     = 124;

const  ID_LISTBOX           = 130;
const  ID_LISTBOX_SEL_NUM   = 131;
const  ID_LISTBOX_SEL_STR   = 132;
const  ID_LISTBOX_CLEAR     = 133;
const  ID_LISTBOX_APPEND    = 134;

const  ID_RADIOBOX          = 130;
const  ID_RADIOBOX_SEL_NUM  = 131;
const  ID_RADIOBOX_SEL_STR  = 132;

BEGIN_EVENT_TABLE(MyDialog,wxDialog)
  EVT_BUTTON    (ID_RETURN,             MyDialog::OnReturnButton)
  EVT_BUTTON    (ID_HELLO,              MyDialog::OnHelloButton)
  EVT_CHECKBOX  (ID_CHECKBOX,           MyDialog::OnCheckBox)
  EVT_BUTTON    (ID_CHECKBOX_CHECK,     MyDialog::OnCheckBoxButtons)
  EVT_BUTTON    (ID_CHECKBOX_UNCHECK,   MyDialog::OnCheckBoxButtons)
  EVT_TEXT      (ID_TEXTCTRL,           MyDialog::OnTextCtrl)
  EVT_BUTTON    (ID_TEXTCTRL_SET,       MyDialog::OnTextCtrlButtons)
  EVT_BUTTON    (ID_TEXTCTRL_DEL,       MyDialog::OnTextCtrlButtons)
  EVT_CHOICE    (ID_CHOICE,             MyDialog::OnChoice)
  EVT_BUTTON    (ID_CHOICE_SEL_NUM,     MyDialog::OnChoiceButtons)
  EVT_BUTTON    (ID_CHOICE_SEL_STR,     MyDialog::OnChoiceButtons)
  EVT_BUTTON    (ID_CHOICE_CLEAR,       MyDialog::OnChoiceButtons)
  EVT_BUTTON    (ID_CHOICE_APPEND,      MyDialog::OnChoiceButtons)
  EVT_LISTBOX   (ID_LISTBOX,            MyDialog::OnListBox)
  EVT_BUTTON    (ID_LISTBOX_SEL_NUM,    MyDialog::OnListBoxButtons)
  EVT_BUTTON    (ID_LISTBOX_SEL_STR,    MyDialog::OnListBoxButtons)
  EVT_BUTTON    (ID_LISTBOX_CLEAR,      MyDialog::OnListBoxButtons)
  EVT_BUTTON    (ID_LISTBOX_APPEND,     MyDialog::OnListBoxButtons)
  EVT_RADIOBOX  (ID_RADIOBOX,           MyDialog::OnRadioBox)
  EVT_BUTTON    (ID_RADIOBOX_SEL_NUM,   MyDialog::OnRadioBoxButtons)
  EVT_BUTTON    (ID_RADIOBOX_SEL_STR,   MyDialog::OnRadioBoxButtons)
END_EVENT_TABLE()

//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(MyDialog, wxDialog)

MyDialog::MyDialog( wxWindow *parent ) :
  wxDialog( parent, -1, "TestDialog", wxPoint(20,100), wxSize(700,400), wxDIALOG_MODAL )
{
  m_text1 = NULL;
  m_text2 = NULL;

  (void)new wxStaticBox( this, -1, "CheckBox group", wxPoint(20,10), wxSize(140,180) );
  m_checkbox = new wxCheckBox( this, ID_CHECKBOX, "CheckBox", wxPoint(40,35), wxSize(100,30) );
  (void)new wxButton( this, ID_CHECKBOX_CHECK, "Check", wxPoint(40,85), wxSize(100,30) );
  (void)new wxButton( this, ID_CHECKBOX_UNCHECK, "Uncheck", wxPoint(40,135), wxSize(100,30) );
  
  (void)new wxStaticBox( this, -1, "TextCtrl group", wxPoint(20,200), wxSize(140,180) );
  m_textctrl = new wxTextCtrl( this, ID_TEXTCTRL, "TextCtrl", wxPoint(40,35+190), wxSize(100,30) );
  (void)new wxButton( this, ID_TEXTCTRL_SET, "Set 'Hi!'", wxPoint(40,85+190), wxSize(100,30) );
  (void)new wxButton( this, ID_TEXTCTRL_DEL, "Delete", wxPoint(40,135+190), wxSize(100,30) );
  
  wxString choices[4] =
  {
    "This",
    "is",
    "a",
    "wonderfull example."
  };
  
  (void)new wxStaticBox( this, -1, "Choice group", wxPoint(180,10), wxSize(140,330) );
  m_choice = new wxChoice( this, ID_CHOICE, wxPoint(200,35), wxSize(100,30), 4, choices );
  (void)new wxButton( this, ID_CHOICE_SEL_NUM, "Select #2", wxPoint(200,130), wxSize(100,30) );
  (void)new wxButton( this, ID_CHOICE_SEL_STR, "Select 'This'", wxPoint(200,180), wxSize(100,30) );
  (void)new wxButton( this, ID_CHOICE_CLEAR, "Clear", wxPoint(200,230), wxSize(100,30) );
  (void)new wxButton( this, ID_CHOICE_APPEND, "Append 'Hi!'", wxPoint(200,280), wxSize(100,30) );

  (void)new wxStaticBox( this, 100, "ListBox group", wxPoint(340,10), wxSize(140,330) );
  m_listbox = new wxListBox( this, ID_LISTBOX, wxPoint(360,35), wxSize(100,70), 4, choices );
  (void)new wxButton( this, ID_LISTBOX_SEL_NUM, "Select #2", wxPoint(360,130), wxSize(100,30) );
  (void)new wxButton( this, ID_LISTBOX_SEL_STR, "Select 'This'", wxPoint(360,180), wxSize(100,30) );
  (void)new wxButton( this, ID_LISTBOX_CLEAR, "Clear", wxPoint(360,230), wxSize(100,30) );
  (void)new wxButton( this, ID_LISTBOX_APPEND, "Append 'Hi!'", wxPoint(360,280), wxSize(100,30) );
      
  (void)new wxStaticBox( this, -1, "RadioBox group", wxPoint(500,10), wxSize(180,230) );
  m_radiobox = new wxRadioBox( this, ID_RADIOBOX, "Test", wxPoint(520,35), wxSize(-1,-1), 4, choices,
    1, wxRA_VERTICAL );
  
  (void)new wxButton( this, ID_HELLO, "wxScreenDC", wxPoint(540,280), wxSize(120,40) );
  (void)new wxButton( this, ID_RETURN, "Return", wxPoint(540,340), wxSize(120,40) );
  
  m_text1 = new wxStaticText( this, -1, "No event.", wxPoint(170,350), wxSize(300,-1) );
  m_text2 = new wxStaticText( this, -1, "No information.", wxPoint(170,370), wxSize(300,-1) );
  
  InitDialog();
};

void MyDialog::OnTextCtrl( wxCommandEvent &WXUNUSED(event) )
{
};

void MyDialog::OnTextCtrlButtons( wxCommandEvent &event )
{
  switch (event.GetId())
  {
    case ID_TEXTCTRL_SET:
    {
      m_textctrl->SetValue( "Hi!" );
      break;
    };
    case ID_TEXTCTRL_DEL:
    {
      m_textctrl->Delete();
      break;
    };
  };
};

void MyDialog::OnRadioBox( wxCommandEvent &event )
{
  if (!m_text1) return;
  m_text1->SetLabel( "RadioBox Event:");
  wxString tmp = "RadioBox selection string is: ";
  tmp += event.GetString();
  m_text2->SetLabel( tmp );
};

void MyDialog::OnRadioBoxButtons( wxCommandEvent &WXUNUSED(event) )
{
};
    
void MyDialog::OnListBox( wxCommandEvent &event )
{
  if (!m_text1) return;
  m_text1->SetLabel( "ListBox Event:");
  wxString tmp = "ListBox selection string is: ";
  tmp += event.GetString();
  m_text2->SetLabel( tmp );
};

void MyDialog::OnListBoxButtons( wxCommandEvent &event )
{
  switch (event.GetId())
  {
    case ID_LISTBOX_SEL_NUM:
    {
      m_listbox->SetSelection( 2 );
      break;
    };
    case ID_LISTBOX_SEL_STR:
    {
      m_listbox->SetStringSelection( "This" );
      break;
    };
    case ID_LISTBOX_CLEAR:
    {
      m_listbox->Clear();
      break;
    };
    case ID_LISTBOX_APPEND:
    {
      m_listbox->Append( "Hi!" );
      break;
    };
  };
};

void MyDialog::OnCheckBox( wxCommandEvent &event )
{
  if (!m_text1) return;
  m_text1->SetLabel( "CheckBox Event:");
  wxString tmp = "Checkbox is ";
  if (event.Checked()) 
    tmp += "checked.";
  else
    tmp += "unchecked.";
  m_text2->SetLabel( tmp );
};

void MyDialog::OnCheckBoxButtons( wxCommandEvent &event )
{
  switch (event.GetId())
  {
    case ID_CHECKBOX_CHECK:
    {
      m_checkbox->SetValue( TRUE );
      break;
    };
    case ID_CHECKBOX_UNCHECK:
    {
      m_checkbox->SetValue( FALSE );
      break;
    };
  };
};

void MyDialog::OnChoice( wxCommandEvent &event )
{
  if (!m_text1) return;
  m_text1->SetLabel( "Choice Event:");
  wxString tmp = "Choice selection string is: ";
  tmp += event.GetString();
  m_text2->SetLabel( tmp );
};

void MyDialog::OnChoiceButtons( wxCommandEvent &event )
{
  switch (event.GetId())
  {
    case ID_CHOICE_SEL_NUM:
    {
      m_choice->SetSelection( 2 );
      break;
    };
    case ID_CHOICE_SEL_STR:
    {
      m_choice->SetStringSelection( "This" );
      break;
    };
    case ID_CHOICE_CLEAR:
    {
      m_choice->Clear();
      break;
    };
    case ID_CHOICE_APPEND:
    {
      m_choice->Append( "Hi!" );
      break;
    };
  };
};

void MyDialog::OnReturnButton( wxCommandEvent &WXUNUSED(event) )
{
  EndModal( 1 );
};

void MyDialog::OnHelloButton( wxCommandEvent &WXUNUSED(event) )
{
  wxMessageDialog *dialog;
  dialog = new wxMessageDialog( this, "Now, I will paint on Screen.", "wxGTK" );
      dialog->ShowModal();
  delete dialog;
  
  wxScreenDC dc;
  dc.StartDrawingOnTop();
  
  int w = wxSystemSettings::GetSystemMetric( wxSYS_SCREEN_X );
  int h = wxSystemSettings::GetSystemMetric( wxSYS_SCREEN_Y );
  
  dc.SetPen( *wxWHITE_PEN );
  dc.SetBrush( *wxTRANSPARENT_BRUSH );
  for (int i = 0; i < h; i += 3) dc.DrawLine( 0, i, w, i );
  
  dialog = new wxMessageDialog( this, "Now, the stripes will disappear.", "wxGTK" );
  dialog->ShowModal();
  delete dialog;
  
  dc.EndDrawingOnTop();
};

//-----------------------------------------------------------------------------
// MyCanvas
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(MyCanvas, wxScrolledWindow)

BEGIN_EVENT_TABLE(MyCanvas,wxScrolledWindow)
  EVT_BUTTON  (100,  MyDialog::OnReturnButton)
  EVT_PAINT  (MyCanvas::OnPaint)
END_EVENT_TABLE()

MyCanvas::MyCanvas( wxWindow *parent, const wxWindowID id, const wxPoint &pos, const wxSize &size ) 
  : wxScrolledWindow( parent, id, pos, size, wxSUNKEN_BORDER ) 
{
  my_bitmap = new wxBitmap( folder_xpm );
  my_horse = new wxBitmap();
  my_horse->LoadFile( "horse.png", 0 );
  my_backstore = new wxBitmap( 150, 150 );
  my_font = new wxFont( 20, wxROMAN, wxNORMAL, wxNORMAL );
  m_isCreated = FALSE;
  
  SetBackgroundColour( wxColour("Wheat") );
};

MyCanvas::~MyCanvas(void)
{
  delete my_bitmap;
  delete my_backstore;
  delete my_horse;
  delete my_font;
};

void MyCanvas::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
  wxPaintDC dc( this );
  PrepareDC( dc );

  wxMemoryDC memDC;
  memDC.SelectObject( *my_backstore );
  memDC.Clear();
  memDC.SetBrush( *wxBLACK_BRUSH );
  memDC.SetPen( *wxWHITE_PEN );
  memDC.DrawRectangle( 0, 0, 150, 150 );
  memDC.SetTextForeground( *wxWHITE );
  memDC.DrawText( "This is a memory dc.", 10, 10 );
  
  int vx = 0;
  int vy = 0;
  GetVirtualSize( &vx, &vy );
  dc.DrawLine( 5, 5, vx-10, vy-10 );
  dc.DrawLine( 10, 20, 100, 10 );
  dc.DrawLine( 10, 20, 100, 50 );
  dc.DrawLine( 10, 20, 100, 100 );
  
  dc.SetPen( *wxWHITE_PEN );
  dc.DrawLine( 80, 50, 180, 50 );

  dc.SetFont( *my_font );
  
  long x = 0;
  long y = 0;
  dc.GetTextExtent( "Hej, ho, hej, ho.", &x, &y );
  
  dc.SetBrush( *wxTRANSPARENT_BRUSH );
  dc.DrawRectangle( 80, 40, x, y );
  
  dc.SetTextForeground( *wxGREEN );
  dc.DrawText( "Hej, ho, hej, ho.", 80, 40 );
  
  dc.SetTextForeground( *wxBLACK );
  dc.SetFont( *wxNORMAL_FONT );
  dc.DrawText( "Hej, ho, hej, ho. (NormalFont)", 80, 60 );
  dc.SetFont( *wxSMALL_FONT );
  dc.DrawText( "Hej, ho, hej, ho. (SmallFont)", 80, 80 );
  dc.SetFont( *wxITALIC_FONT );
  dc.DrawText( "Hej, ho, hej, ho. (ItalicFont)", 80, 100 );
  
  dc.DrawBitmap( *my_bitmap, 30, 80, TRUE );
  dc.DrawBitmap( *my_horse, 30, 120 );
  
  dc.Blit( 200, 200, 150, 150, &memDC, 0, 0, 0 );
  
  memDC.SelectObject( wxNullBitmap );
  
/*
  dc.SetBrush( *wxBLACK_BRUSH );
  dc.DrawRectangle( 50, 50, 50, 50 );
  dc.SetPen( *wxWHITE_PEN );
  dc.DrawRectangle( 101, 50, 50, 50 );
  dc.DrawRectangle( 50, 101, 50, 50 );
  
  dc.SetBrush( *wxWHITE_BRUSH );
  
  dc.SetPen( *wxWHITE_PEN );
  dc.DrawRectangle( 70, 70, 2, 2 );
  
  dc.SetPen( *wxRED_PEN );
  dc.DrawRectangle( 72, 70, 2, 2 );
  dc.DrawRectangle( 70, 72, 2, 2 );
  
  
  dc.SetPen( *wxRED_PEN );
  dc.DrawRectangle( 82, 80, 2, 2 );
  dc.DrawRectangle( 80, 82, 2, 2 );
  
  dc.SetPen( *wxWHITE_PEN );
  dc.DrawRectangle( 80, 80, 2, 2 );
*/
};

//-----------------------------------------------------------------------------
// MyFrame
//-----------------------------------------------------------------------------

const  ID_OPEN  = 101;
const  ID_SAVE  = 102;
const  ID_MSG   = 103;
const  ID_FONT  = 104;
const  ID_DLG   = 105;
const  ID_QUIT  = 108;
const  ID_ABOUT = 109;

IMPLEMENT_DYNAMIC_CLASS( MyFrame, wxFrame )

BEGIN_EVENT_TABLE(MyFrame,wxFrame)
  EVT_MENU    (ID_OPEN,  MyFrame::OnOpenDialog)
  EVT_MENU    (ID_FONT,  MyFrame::OnFontDialog)
  EVT_MENU    (ID_MSG,   MyFrame::OnMsg)
  EVT_MENU    (ID_DLG,   MyFrame::OnDialog)
  EVT_MENU    (ID_ABOUT, MyFrame::OnAbout)
  EVT_MENU    (ID_QUIT,  MyFrame::OnQuit)
END_EVENT_TABLE()

MyFrame::MyFrame(void) :
  wxFrame( NULL, -1, "Robert's Test application", wxPoint(20,20), wxSize(470,360) )
{
  wxMenu *file_menu = new wxMenu( "Test" );
  file_menu->Append( ID_OPEN, "Open..");
  file_menu->Append( ID_MSG,  "MessageBox..");
  file_menu->Append( ID_FONT, "FontDialog..");
  file_menu->AppendSeparator();
  file_menu->Append( ID_DLG,  "TestDialog..");
  file_menu->AppendSeparator();
  file_menu->Append( ID_ABOUT, "About..");
  file_menu->Append( ID_QUIT, "Exit");
  
  wxMenuBar *menu_bar = new wxMenuBar();
  menu_bar->Append(file_menu, "File");
  menu_bar->Show( TRUE );
  
  SetMenuBar( menu_bar );
  
  CreateStatusBar( 2 );
  
  SetStatusText( "wxGTK v0.12", 0 );
  SetStatusText( "Copyright 1998 Robert Roebling.", 1 );
  
  m_canvas = new MyCanvas( this, -1, wxPoint(2,62), wxSize(300-4,120-4) );
  m_canvas->SetScrollbars( 10, 10, 50, 50 );
  
  m_tb = CreateToolBar();
  m_tb->SetMargins( 2, 2 );
  m_tb->AddTool( 0, wxBitmap( list_xpm ), wxNullBitmap, FALSE, -1, -1, NULL, "This is a button" );
  m_tb->AddTool( 0, wxBitmap( folder_xpm ), wxNullBitmap, TRUE, -1, -1, NULL, "This is a toggle" );
  m_tb->Realize();
  
//  m_timer.Start( 1000, TRUE );
};

void MyFrame::OnDialog( wxCommandEvent &WXUNUSED(event) )
{
  MyDialog *dialog = new MyDialog( this );
  dialog->ShowModal();
  dialog->Close( TRUE );
};

void MyFrame::OnFontDialog( wxCommandEvent &WXUNUSED(event) )
{
  wxFontData data;
  data.SetInitialFont( wxSMALL_FONT );
  data.SetColour( wxRED );
  wxGenericFontDialog dialog( this, &data );
  if (dialog.ShowModal() == wxID_OK)
  {
    wxFontData retData = dialog.GetFontData();
    // do something
  };
};

void MyFrame::OnOpenDialog( wxCommandEvent &WXUNUSED(event) )
{
  wxFileDialog dialog(this, "Testing open file dialog", "", "", "*.txt", 0);
  if (dialog.ShowModal() == wxID_OK)
  {
    wxMessageDialog dialog2(this, dialog.GetPath(), "Selected path");
    dialog2.ShowModal();
  };
};

void MyFrame::OnMsg( wxCommandEvent &WXUNUSED(event) )
{
  wxMessageBox( "There once was a lady from Riga.", "TestBox.", wxYES_NO );
};

void MyFrame::OnQuit( wxCommandEvent &WXUNUSED(event) )
{
  Close( TRUE );
};

void MyFrame::OnAbout( wxCommandEvent &WXUNUSED(event) )
{
  wxDialog dialog( this, -1, "About wxGTK", wxPoint(100,100), wxSize(540,350), wxDIALOG_MODAL );
  
  int w = 0;
  int h = 0;
  dialog.GetSize( &w, &h );

  int x = 30;
  int y = 20;
  int step = 20;

  (void)new wxStaticBox( &dialog, -1, (const char*)NULL, wxPoint(10,10), wxSize(w-20,h-80) );
  
  (void)new wxStaticText( &dialog, -1, "wxGTK v0.12", wxPoint(240,y) );
  y += 2*step-10;
  
  (void)new wxStaticText( &dialog, -1, "Written by Robert Roebling, 1998. More information at:", wxPoint(x,y) );
  y += step;
  (void)new wxStaticText( &dialog, -1, "http://www.freiburg.linux.de/~wxxt", wxPoint(x+50,y) );
  y += 2*step;
  
  (void)new wxStaticText( &dialog, -1, 
    "wxGTK is based on the wxWindows GUI-library written mainly by Julian Smart. See:", wxPoint(x,y) );
  y += step;
  (void)new wxStaticText( &dialog, -1, "http://web.ukonline.co.uk/julian.smart/wxwin", wxPoint(x+50,y) );
  y += 2*step;

  (void)new wxStaticText( &dialog, -1, "wxWindows Copyright: Less restrictive version of LGPL.", wxPoint(x,y) );
  y += 2*step;
  (void)new wxStaticText( &dialog, -1, "For questions concerning wxGTK, you may mail to:", wxPoint(x,y) );
  y += step;
  (void)new wxStaticText( &dialog, -1, "roebling@ruf.uni-freiburg.de", wxPoint(x+50,y) );
  
  (void) new wxButton( &dialog, wxID_OK, "Return", wxPoint(w/2-40,h-50), wxSize(80,30) );
  
  dialog.ShowModal();
};

//-----------------------------------------------------------------------------
// MyApp
//-----------------------------------------------------------------------------

MyApp::MyApp(void) : 
  wxApp( )
{
};

bool MyApp::OnInit(void)
{
  wxFrame *frame = new MyFrame();
  frame->Show( TRUE );
  
  return TRUE;
};





