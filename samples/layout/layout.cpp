/////////////////////////////////////////////////////////////////////////////
// Name:        layout.cpp
// Purpose:     Layout sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
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
#include "wx/notebook.h"

#include "layout.h"

// Declare two frames
MyFrame   *frame = (MyFrame *) NULL;
wxMenuBar *menu_bar = (wxMenuBar *) NULL;

IMPLEMENT_APP(MyApp)

MyApp::MyApp()
{
}

bool MyApp::OnInit()
{
  // Create the main frame window
  frame = new MyFrame(NULL, _T("wxWindows Layout Demo"), -1, -1, 400, 300);

  frame->SetAutoLayout(TRUE);

  // Give it a status line
  frame->CreateStatusBar(2);

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(LAYOUT_TEST_SIZER, _T("&Test sizers"),      _T("Test sizer"));
  file_menu->Append(LAYOUT_TEST_NB, _T("&Test notebook sizers"),      _T("Test notebook sizer"));

  file_menu->AppendSeparator();
  file_menu->Append(LAYOUT_QUIT, _T("E&xit"),                _T("Quit program"));

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(LAYOUT_ABOUT, _T("&About"),              _T("About layout demo"));

  menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, _T("&File"));
  menu_bar->Append(help_menu, _T("&Help"));

  // Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  // Make a panel
  wxPanel *panel = new wxPanel(frame);

  // Create some panel items
  wxButton *btn1 = new wxButton(panel, -1, _T("A button (1)")) ;

  wxLayoutConstraints *b1 = new wxLayoutConstraints;
  b1->centreX.SameAs    (panel, wxCentreX);
  b1->top.SameAs        (panel, wxTop, 5);
  b1->width.PercentOf   (panel, wxWidth, 80);
  b1->height.AsIs       ();
  btn1->SetConstraints(b1);

  wxListBox *list = new wxListBox(panel, -1,
                                  wxPoint(-1, -1), wxSize(200, 100));
  list->Append(_T("Apple"));
  list->Append(_T("Pear"));
  list->Append(_T("Orange"));
  list->Append(_T("Banana"));
  list->Append(_T("Fruit"));

  wxLayoutConstraints *b2 = new wxLayoutConstraints;
  b2->top.Below         (btn1, 5);
  b2->left.SameAs       (panel, wxLeft, 5);
  b2->width.PercentOf   (panel, wxWidth, 40);
  b2->bottom.SameAs     (panel, wxBottom, 5);
  list->SetConstraints(b2);

  wxTextCtrl *mtext = new wxTextCtrl(panel, -1, _T("Some text"));

  wxLayoutConstraints *b3 = new wxLayoutConstraints;
  b3->top.Below         (btn1, 5);
  b3->left.RightOf      (list, 5);
  b3->right.SameAs      (panel, wxRight, 5);
  b3->bottom.SameAs     (panel, wxBottom, 5);
  mtext->SetConstraints(b3);

  MyWindow *canvas = new MyWindow(frame, 0, 0, 400, 400, wxRETAINED);

  // Make a text window
  MyTextWindow *text_window = new MyTextWindow(frame, 0, 250, 400, 150);

  // Set constraints for panel subwindow
  wxLayoutConstraints *c1 = new wxLayoutConstraints;

  c1->left.SameAs       (frame, wxLeft);
  c1->top.SameAs        (frame, wxTop);
  c1->right.PercentOf   (frame, wxWidth, 50);
  c1->height.PercentOf  (frame, wxHeight, 50);

  panel->SetConstraints(c1);

  // Set constraints for canvas subwindow
  wxLayoutConstraints *c2 = new wxLayoutConstraints;

  c2->left.SameAs       (panel, wxRight);
  c2->top.SameAs        (frame, wxTop);
  c2->right.SameAs      (frame, wxRight);
  c2->height.PercentOf  (frame, wxHeight, 50);

  canvas->SetConstraints(c2);

  // Set constraints for text subwindow
  wxLayoutConstraints *c3 = new wxLayoutConstraints;
  c3->left.SameAs       (frame, wxLeft);
  c3->top.Below         (panel);
  c3->right.SameAs      (frame, wxRight);
  c3->bottom.SameAs     (frame, wxBottom);

  text_window->SetConstraints(c3);

  frame->Show(TRUE);

  frame->SetStatusText(_T("wxWindows layout demo"));

  SetTopWindow(frame);

  return TRUE;
}

//-----------------------------------------------------------------
//  MyFrame
//-----------------------------------------------------------------

// Define my frame constructor
MyFrame::MyFrame(wxFrame *frame, const wxChar *title, int x, int y, int w, int h)
       : wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h))
{
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(LAYOUT_QUIT, MyFrame::OnQuit)
  EVT_MENU(LAYOUT_TEST_SIZER, MyFrame::TestSizers)
  EVT_MENU(LAYOUT_TEST_NB, MyFrame::TestNotebookSizers)
  EVT_MENU(LAYOUT_ABOUT, MyFrame::About)
END_EVENT_TABLE()

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event) )
{
    Close(TRUE);
}

void MyFrame::TestSizers(wxCommandEvent& WXUNUSED(event) )
{
    MySizerFrame *newFrame = new MySizerFrame(NULL, _T("Sizer Test Frame"), 50, 50);
    newFrame->Show(TRUE);
}

void MyFrame::TestNotebookSizers(wxCommandEvent& WXUNUSED(event) )
{
    wxDialog dialog( this, -1, wxString(_T("Notebook Sizer Test Dialog"))  );

    // Begin with first hierarchy: a notebook at the top and
    // and OK button at the bottom.

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    wxNotebook *notebook = new wxNotebook( &dialog, -1 );
    wxNotebookSizer *nbs = new wxNotebookSizer( notebook );
    topsizer->Add( nbs, 1, wxGROW );

    wxButton *button = new wxButton( &dialog, wxID_OK, _T("OK") );
    topsizer->Add( button, 0, wxALIGN_RIGHT | wxALL, 10 );

    // First page: one big text ctrl
    wxTextCtrl *multi = new wxTextCtrl( notebook, -1, _T("TextCtrl."), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
    notebook->AddPage( multi, _T("Page One") );

    // Second page: a text ctrl and a button
    wxPanel *panel = new wxPanel( notebook, -1 );
    notebook->AddPage( panel, _T("Page Two") );

    wxSizer *panelsizer = new wxBoxSizer( wxVERTICAL );

    wxTextCtrl *text = new wxTextCtrl( panel, -1, _T("TextLine 1."), wxDefaultPosition, wxSize(250,-1) );
    panelsizer->Add( text, 0, wxGROW|wxALL, 30 );
    text = new wxTextCtrl( panel, -1, _T("TextLine 2."), wxDefaultPosition, wxSize(250,-1) );
    panelsizer->Add( text, 0, wxGROW|wxALL, 30 );
    wxButton *button2 = new wxButton( panel, -1, _T("Hallo") );
    panelsizer->Add( button2, 0, wxALIGN_RIGHT | wxLEFT|wxRIGHT|wxBOTTOM, 30 );

    panel->SetAutoLayout( TRUE );
    panel->SetSizer( panelsizer );

    // Tell dialog to use sizer

    dialog.SetAutoLayout( TRUE );
    dialog.SetSizer( topsizer );
    topsizer->Fit( &dialog );
    topsizer->SetSizeHints( &dialog );

    dialog.ShowModal();
}


void MyFrame::About(wxCommandEvent& WXUNUSED(event) )
{
    (void)wxMessageBox(_T("wxWindows GUI library layout demo\n"),
            _T("About Layout Demo"), wxOK|wxCENTRE);
}

//-----------------------------------------------------------------
//  MyWindow
//-----------------------------------------------------------------

BEGIN_EVENT_TABLE(MyWindow, wxWindow)
    EVT_PAINT(MyWindow::OnPaint)
END_EVENT_TABLE()

// Define a constructor for my canvas
MyWindow::MyWindow(wxFrame *frame, int x, int y, int w, int h, long style)
        : wxWindow(frame, -1, wxPoint(x, y), wxSize(w, h), style)
{
}

MyWindow::~MyWindow()
{
}

// Define the repainting behaviour
void MyWindow::OnPaint(wxPaintEvent& WXUNUSED(event) )
{
  wxPaintDC dc(this);
  dc.SetPen(* wxGREEN_PEN);
  dc.DrawLine(0, 0, 200, 200);
  dc.DrawLine(200, 0, 0, 200);

  dc.SetBrush(* wxCYAN_BRUSH);
  dc.SetPen(* wxRED_PEN);

  dc.DrawRectangle(100, 100, 100, 50);
  dc.DrawRoundedRectangle(150, 150, 100, 50, 20);

  dc.DrawEllipse(250, 250, 100, 50);
#if wxUSE_SPLINES
  dc.DrawSpline(50, 200, 50, 100, 200, 10);
#endif // wxUSE_SPLINES
  dc.DrawLine(50, 230, 200, 230);

  dc.SetPen(* wxBLACK_PEN);
  dc.DrawArc(50, 300, 100, 250, 100, 300 );
}

//-----------------------------------------------------------------
//  MySizerFrame
//-----------------------------------------------------------------

MySizerFrame::MySizerFrame(wxFrame *frame, wxChar *title, int x, int y )
            : wxFrame(frame, -1, title, wxPoint(x, y) )
{
  // we want to get a dialog that is stretchable because it
  // has a text ctrl in the middle. at the bottom, we have
  // two buttons which.

  wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

  // 1) top: create wxStaticText with minimum size equal to its default size
  topsizer->Add(
    new wxStaticText( this, -1, _T("An explanation (wxALIGN_RIGHT).") ),
    0,                         // make vertically unstretchable
    wxALIGN_RIGHT |            // right align text
    wxTOP | wxLEFT | wxRIGHT,  // make border all around except wxBOTTOM
    5 );                      // set border width to 5

  // 2) top: create wxTextCtrl with minimum size (100x60)
  topsizer->Add(
    new wxTextCtrl( this, -1, _T("My text (wxEXPAND)."), wxDefaultPosition, wxSize(100,60), wxTE_MULTILINE),
    1,            // make vertically stretchable
    wxEXPAND |    // make horizontally stretchable
    wxALL,        //   and make border all around
    5 );         // set border width to 5

  // 2.5) Gratuitous test of wxStaticBoxSizers
  wxBoxSizer *statsizer = new wxStaticBoxSizer(
    new wxStaticBox(this, -1, _T("A wxStaticBoxSizer")),
    wxVERTICAL );
  statsizer->Add(
    new wxStaticText(this, -1, _T("And some TEXT inside it")),
    0,
    wxCENTER |
    wxALL,
    30);
  topsizer->Add(statsizer, 1, wxEXPAND | wxALL, 10);

    // 2.7) And a test of wxGridSizer
    wxGridSizer *gridsizer = new wxGridSizer(2, 5, 5);
    gridsizer->Add(new wxStaticText(this, -1, _T("Label")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    gridsizer->Add(new wxTextCtrl(this, -1, _T("Grid sizer demo")), 1,
                   wxGROW | wxALIGN_CENTER_VERTICAL);
    gridsizer->Add(new wxStaticText(this, -1, _T("Another label")), 0,
                   wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    gridsizer->Add(new wxTextCtrl(this, -1, _T("More text")), 1,
                   wxGROW | wxALIGN_CENTER_VERTICAL);
    gridsizer->Add(new wxStaticText(this, -1, _T("Final label")), 0,
                   wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    gridsizer->Add(new wxTextCtrl(this, -1, _T("And yet more text")), 1,
                   wxGROW | wxALIGN_CENTER_VERTICAL);
    topsizer->Add(gridsizer, 1, wxGROW | wxALL, 10);


  // 3) middle: create wxStaticLine with minimum size (3x3)
  topsizer->Add(
     new wxStaticLine( this, -1, wxDefaultPosition, wxSize(3,3), wxHORIZONTAL),
     0,           // make vertically unstretchable
     wxEXPAND |   // make horizontally stretchable
     wxALL,       //   and make border all around
     5 );         // set border width to 5


  // 4) bottom: create two centred wxButtons
  wxBoxSizer *button_box = new wxBoxSizer( wxHORIZONTAL );
  button_box->Add(
     new wxButton( this, -1, _T("Two buttons in a box") ),
     0,           // make horizontally unstretchable
     wxALL,       // make border all around
     7 );         // set border width to 7
  button_box->Add(
     new wxButton( this, -1, _T("(wxCENTER)") ),
     0,           // make horizontally unstretchable
     wxALL,       // make border all around
     7 );         // set border width to 7

  topsizer->Add(
     button_box,
     0,          // make vertically unstretchable
     wxCENTER ); // no border and centre horizontally

  // don't allow frame to get smaller than what the sizers tell it and also set
  // the initial size as calculated by the sizers
  topsizer->SetSizeHints( this );

  SetSizer( topsizer );
}



