/////////////////////////////////////////////////////////////////////////////
// Name:        layout.cpp
// Purpose:     Layout sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_CONSTRAINTS
#error You must set wxUSE_CONSTRAINTS to 1 in setup.h!
#endif

#include <ctype.h>
#include "wx/sizer.h"
#include "wx/statline.h"

#include "layout.h"

// Declare two frames
MyFrame   *frame = (MyFrame *) NULL;
wxMenuBar *menu_bar = (wxMenuBar *) NULL;

IMPLEMENT_APP(MyApp)

MyApp::MyApp()
{
}

bool MyApp::OnInit(void)
{
  // Create the main frame window
  frame = new MyFrame((MyFrame *) NULL, (char *) "wxWindows Layout Demo", 0, 0, 550, 500);

  frame->SetAutoLayout(TRUE);

  // Give it a status line
  frame->CreateStatusBar(2);

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(LAYOUT_LOAD_FILE, "&Load file",      "Load a text file");
  file_menu->Append(LAYOUT_TEST_NEW, "&Test new sizers",      "Test new sizer code");

  file_menu->AppendSeparator();
  file_menu->Append(LAYOUT_QUIT, "E&xit",                "Quit program");

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(LAYOUT_ABOUT, "&About",              "About layout demo");

  menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");
  menu_bar->Append(help_menu, "&Help");

  // Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  // Make a panel
  frame->panel = new wxPanel(frame, 0, 0, 1000, 500, wxTAB_TRAVERSAL);
  frame->panel->SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
//  frame->panel->SetAutoLayout(TRUE);

  // Create some panel items
  wxButton *btn1 = new wxButton(frame->panel, -1, "A button (1)") ;

  wxLayoutConstraints *b1 = new wxLayoutConstraints;
  b1->centreX.SameAs    (frame->panel, wxCentreX);
  b1->top.SameAs        (frame->panel, wxTop, 5);
  b1->width.PercentOf   (frame->panel, wxWidth, 80);
  b1->height.PercentOf  (frame->panel, wxHeight, 10);
  btn1->SetConstraints(b1);

  wxListBox *list = new wxListBox(frame->panel, -1,
                                  wxPoint(-1, -1), wxSize(200, 100));
  list->Append("Apple");
  list->Append("Pear");
  list->Append("Orange");
  list->Append("Banana");
  list->Append("Fruit");

  wxLayoutConstraints *b2 = new wxLayoutConstraints;
  b2->top.Below         (btn1, 5);
  b2->left.SameAs       (frame->panel, wxLeft, 5);
  b2->width.PercentOf   (frame->panel, wxWidth, 40);
  b2->bottom.SameAs     (frame->panel, wxBottom, 5);
  list->SetConstraints(b2);

  wxTextCtrl *mtext = new wxTextCtrl(frame->panel, -1, "Some text",
                        wxPoint(-1, -1), wxSize(150, 100));

  wxLayoutConstraints *b3 = new wxLayoutConstraints;
  b3->top.Below         (btn1, 5);
  b3->left.RightOf      (list, 5);
  b3->right.SameAs      (frame->panel, wxRight, 5);
  b3->bottom.SameAs     (frame->panel, wxBottom, 5);
  mtext->SetConstraints(b3);

  frame->canvas = new MyWindow(frame, 0, 0, 400, 400, wxRETAINED);

  // Give it scrollbars: the virtual canvas is 20 * 50 = 1000 pixels in each direction
//  canvas->SetScrollbars(20, 20, 50, 50, 4, 4);

  // Make a text window
  frame->text_window = new MyTextWindow(frame, 0, 250, 400, 250);

  // Set constraints for panel subwindow
  wxLayoutConstraints *c1 = new wxLayoutConstraints;

  c1->left.SameAs       (frame, wxLeft);
  c1->top.SameAs        (frame, wxTop);
  c1->right.PercentOf   (frame, wxWidth, 50);
  c1->height.PercentOf  (frame, wxHeight, 50);

  frame->panel->SetConstraints(c1);

  // Set constraints for canvas subwindow
  wxLayoutConstraints *c2 = new wxLayoutConstraints;

  c2->left.SameAs       (frame->panel, wxRight);
  c2->top.SameAs        (frame, wxTop);
  c2->right.SameAs      (frame, wxRight);
  c2->height.PercentOf  (frame, wxHeight, 50);

  frame->canvas->SetConstraints(c2);

  // Set constraints for text subwindow
  wxLayoutConstraints *c3 = new wxLayoutConstraints;
  c3->left.SameAs       (frame, wxLeft);
  c3->top.Below         (frame->panel);
  c3->right.SameAs      (frame, wxRight);
  c3->bottom.SameAs     (frame, wxBottom);

  frame->text_window->SetConstraints(c3);

  frame->Show(TRUE);

  frame->SetStatusText("wxWindows layout demo");

  SetTopWindow(frame);
  return TRUE;
}

//-----------------------------------------------------------------
//  MyFrame
//-----------------------------------------------------------------

// Define my frame constructor
MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h))
{
  panel = (wxPanel *) NULL;
  text_window = (MyTextWindow *) NULL;
  canvas = (MyWindow *) NULL;
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(LAYOUT_LOAD_FILE, MyFrame::LoadFile)
  EVT_MENU(LAYOUT_QUIT, MyFrame::Quit)
  EVT_MENU(LAYOUT_TEST_NEW, MyFrame::TestNewSizers)
  EVT_MENU(LAYOUT_ABOUT, MyFrame::About)
  EVT_SIZE(MyFrame::OnSize)
END_EVENT_TABLE()

void MyFrame::LoadFile(wxCommandEvent& WXUNUSED(event) )
{
      wxString s = wxFileSelector( _T("Load text file"), (const wxChar *) NULL, 
                                   (const wxChar *) NULL, (const wxChar *) NULL, _T("*.txt") );
      if (s != "")
      {
#ifdef __WXMSW__
        frame->text_window->LoadFile(s);
#endif
      }
}

void MyFrame::Quit(wxCommandEvent& WXUNUSED(event) )
{
      this->Close(TRUE);
}

void MyFrame::TestNewSizers(wxCommandEvent& WXUNUSED(event) )
{
  NewSizerFrame *newFrame = new NewSizerFrame((MyFrame *) NULL, "Sizer Test Frame", 50, 50 );
  newFrame->Show(TRUE);
}

void MyFrame::About(wxCommandEvent& WXUNUSED(event) )
{
      (void)wxMessageBox("wxWindows GUI library layout demo\n",
            "About Layout Demo", wxOK|wxCENTRE);
}

// Size the subwindows when the frame is resized
void MyFrame::OnSize(wxSizeEvent& WXUNUSED(event) )
{
    Layout();
}

void MyFrame::Draw(wxDC& dc, bool WXUNUSED(draw_bitmaps) )
{
  dc.SetPen(* wxGREEN_PEN);
  dc.DrawLine(0, 0, 200, 200);
  dc.DrawLine(200, 0, 0, 200);

  dc.SetBrush(* wxCYAN_BRUSH);
  dc.SetPen(* wxRED_PEN);

  dc.DrawRectangle(100, 100, 100, 50);
  dc.DrawRoundedRectangle(150, 150, 100, 50, 20);

  dc.DrawEllipse(250, 250, 100, 50);
  dc.DrawSpline(50, 200, 50, 100, 200, 10);
  dc.DrawLine(50, 230, 200, 230);

  dc.SetPen(* wxBLACK_PEN);
  dc.DrawArc(50, 300, 100, 250, 100, 300 );
}

//-----------------------------------------------------------------
//  MyWindow
//-----------------------------------------------------------------

BEGIN_EVENT_TABLE(MyWindow, wxWindow)
    EVT_PAINT(MyWindow::OnPaint)
END_EVENT_TABLE()

// Define a constructor for my canvas
MyWindow::MyWindow(wxFrame *frame, int x, int y, int w, int h, long style):
 wxWindow(frame, -1, wxPoint(x, y), wxSize(w, h), style)
{
}

MyWindow::~MyWindow(void)
{
}

// Define the repainting behaviour
void MyWindow::OnPaint(wxPaintEvent& WXUNUSED(event) )
{
  wxPaintDC dc(this);
  frame->Draw(dc,TRUE);
}

//-----------------------------------------------------------------
//  NewSizerFrame
//-----------------------------------------------------------------

NewSizerFrame::NewSizerFrame(wxFrame *frame, char *title, int x, int y ):
  wxFrame(frame, -1, title, wxPoint(x, y) )
{
  // we want to get a dialog that is stretchable because it
  // has a text ctrl in the middle. at the bottom, we have
  // two buttons which.

  wxBox *topsizer = new wxBox( wxVERTICAL );
  
  // 1) top: create wxStaticText with minimum size equal to its default size
  topsizer->Add( 
    new wxStaticText( this, -1, "An explanation (wxALIGN_RIGHT)." ),
    0,                         // make vertically unstretchable
    wxALIGN_RIGHT |            // right align text
    wxTOP | wxLEFT | wxRIGHT,  // make border all around except wxBOTTOM
    5 );                      // set border width to 5

  // 2) top: create wxTextCtrl with minimum size (100x60)
  topsizer->Add( 
    new wxTextCtrl( this, -1, "My text (wxEXPAND).", wxDefaultPosition, wxSize(100,60), wxTE_MULTILINE),
    1,            // make vertically stretchable
    wxEXPAND |    // make horizontally stretchable
    wxALL,        //   and make border all around
    5 );         // set border width to 5


  // 3) middle: create wxStaticLine with minimum size (3x3)
  topsizer->Add( 
     new wxStaticLine( this, -1, wxDefaultPosition, wxSize(3,3), wxHORIZONTAL), 
     0,           // make vertically unstretchable
     wxEXPAND |   // make horizontally stretchable
     wxALL,       //   and make border all around
     5 );         // set border width to 5
     

  // 4) bottom: create two centred wxButtons  
  wxBox *button_box = new wxBox( wxHORIZONTAL );
  button_box->Add(
     new wxButton( this, -1, "Two buttons in a box" ), 
     0,           // make horizontally unstretchable
     wxALL,       // make border all around
     7 );         // set border width to 7
  button_box->Add(
     new wxButton( this, -1, "(wxCENTER)" ), 
     0,           // make horizontally unstretchable
     wxALL,       // make border all around
     7 );         // set border width to 7
  
  topsizer->Add( 
     button_box,
     0,          // make vertically unstretchable
     wxCENTER ); // no border and centre horizontally

  
  // set frame to minimum size
  topsizer->Fit( this );  
  
  // don't allow frame to get smaller than what the sizers tell ye
  topsizer->SetSizeHints( this );  
  
  SetSizer( topsizer );
  
  SetAutoLayout( TRUE );
}



