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
  file_menu->Append(LAYOUT_TEST, "&Test sizers",      "Test sizer code");

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
  frame->panel->SetBackgroundColour(wxColour(192, 192, 192));
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
  EVT_MENU(LAYOUT_TEST, MyFrame::TestSizers)
  EVT_MENU(LAYOUT_ABOUT, MyFrame::About)
  EVT_SIZE(MyFrame::OnSize)
END_EVENT_TABLE()

void MyFrame::LoadFile(wxCommandEvent& WXUNUSED(event) )
{
      wxString s = wxFileSelector("Load text file", (const char *) NULL, (const char *) NULL, (const char *) NULL, "*.txt");
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

void MyFrame::TestSizers(wxCommandEvent& WXUNUSED(event) )
{
  SizerFrame *newFrame = new SizerFrame((MyFrame *) NULL, (char *) "Sizer Test Frame", 50, 50, 500, 500);
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


SizerFrame::SizerFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h))
{
  panel = new wxPanel(this, -1, wxPoint(0, 0), wxSize(-1, -1), wxTAB_TRAVERSAL);
  panel->SetBackgroundColour(wxColour(192, 192, 192));

  // A sizer to fit the whole panel, plus two sizers, one
  // above the other. A button is centred on the lower
  // sizer; a rowcol containing 3 buttons is centred on the upper
  // sizer.
  wxSizer *expandSizer = new wxSizer(panel, wxSizerExpand);
  expandSizer->SetName("expandSizer");

  wxLayoutConstraints *c;

  /////// TOP OF PANEL
  ///////
  wxSizer *topSizer = new wxSizer(expandSizer);
  topSizer->SetName("topSizer");

  // Specify constraints for the top sizer
  c = new wxLayoutConstraints;
  c->left.SameAs       (expandSizer, wxLeft);
  c->top.SameAs        (expandSizer, wxTop);
  c->right.SameAs      (expandSizer, wxRight);
  c->height.PercentOf  (expandSizer, wxHeight, 50);

  topSizer->SetConstraints(c);

 /*
  * Add a row-col sizer and some buttons
  */

  // Default is layout by rows, 20 columns per row, shrink to fit.
  wxRowColSizer *rowCol = new wxRowColSizer(topSizer);
  rowCol->SetName("rowCol");
  
  wxButton *button = new wxButton(panel, -1, "Button 1");
  rowCol->AddSizerChild(button);

  button = new wxButton(panel, -1, "Button 2");
  rowCol->AddSizerChild(button);

  button = new wxButton(panel, -1, "Button 3");
  rowCol->AddSizerChild(button);

  // Centre the rowcol in the middle of the upper sizer
  c = new wxLayoutConstraints;
  c->centreX.SameAs    (topSizer, wxCentreX);
  c->centreY.SameAs    (topSizer, wxCentreY);
  c->width.AsIs();
  c->height.AsIs();
  rowCol->SetConstraints(c);

  /////// BOTTOM OF PANEL
  ///////
  wxSizer *bottomSizer = new wxSizer(expandSizer);

  // Specify constraints for the bottom sizer
  c = new wxLayoutConstraints;
  c->left.SameAs       (expandSizer, wxLeft);
  c->top.PercentOf     (expandSizer, wxHeight, 50);
  c->right.SameAs      (expandSizer, wxRight);
  c->height.PercentOf  (expandSizer, wxHeight, 50);

  bottomSizer->SetConstraints(c);

  wxButton *button2 = new wxButton(panel, -1, "Test button");

  // The button should be a child of the bottom sizer
  bottomSizer->AddSizerChild(button2);

  // Centre the button on the sizer
  c = new wxLayoutConstraints;
  c->centreX.SameAs    (bottomSizer, wxCentreX);
  c->centreY.SameAs    (bottomSizer, wxCentreY);
  c->width.PercentOf   (bottomSizer, wxWidth, 20);
  c->height.PercentOf  (bottomSizer, wxHeight, 20);
  button2->SetConstraints(c);
}

BEGIN_EVENT_TABLE(SizerFrame, wxFrame)
  EVT_SIZE(SizerFrame::OnSize)
END_EVENT_TABLE()


// Size the subwindows when the frame is resized
void SizerFrame::OnSize(wxSizeEvent& event)
{
  wxFrame::OnSize(event);
  panel->Layout();
}

