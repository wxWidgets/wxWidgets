/////////////////////////////////////////////////////////////////////////////
// Name:        layout.cpp
// Purpose:     Layout sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

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

#include "wx/sizer.h"
#include "wx/statline.h"
#include "wx/notebook.h"

#include "layout.h"

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

MyApp::MyApp()
{
}

bool MyApp::OnInit()
{
  // Create the main frame window
  MyFrame *frame = new MyFrame;

  frame->Show(TRUE);

  return TRUE;
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(LAYOUT_ABOUT, MyFrame::OnAbout)
  EVT_MENU(LAYOUT_QUIT, MyFrame::OnQuit)

  EVT_MENU(LAYOUT_TEST_CONSTRAINTS, MyFrame::TestConstraints)
  EVT_MENU(LAYOUT_TEST_SIZER, MyFrame::TestFlexSizers)
  EVT_MENU(LAYOUT_TEST_NB_SIZER, MyFrame::TestNotebookSizers)
END_EVENT_TABLE()

// Define my frame constructor
MyFrame::MyFrame()
       : wxFrame(NULL, -1, _T("wxWindows Layout Demo"),
                 wxDefaultPosition, wxDefaultSize,
                 wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE)
{
  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(LAYOUT_TEST_CONSTRAINTS, _T("Test &constraints"));
  file_menu->Append(LAYOUT_TEST_SIZER, _T("Test wx&FlexSizer"));
  file_menu->Append(LAYOUT_TEST_NB_SIZER, _T("&Test notebook sizers"));

  file_menu->AppendSeparator();
  file_menu->Append(LAYOUT_QUIT, _T("E&xit"), _T("Quit program"));

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(LAYOUT_ABOUT, _T("&About"), _T("About layout demo"));

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, _T("&File"));
  menu_bar->Append(help_menu, _T("&Help"));

  // Associate the menu bar with the frame
  SetMenuBar(menu_bar);

  CreateStatusBar(2);
  SetStatusText(_T("wxWindows layout demo"));


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

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event) )
{
    Close(TRUE);
}

void MyFrame::TestConstraints(wxCommandEvent& WXUNUSED(event) )
{
    MyConstraintsFrame *
        newFrame = new MyConstraintsFrame(_T("Constraints Test Frame"), 100, 100);
    newFrame->Show(TRUE);
}

void MyFrame::TestFlexSizers(wxCommandEvent& WXUNUSED(event) )
{
    MyFlexSizerFrame *newFrame = new MyFlexSizerFrame(_T("Flex Sizer Test Frame"), 50, 50);
    newFrame->Show(TRUE);
}

void MyFrame::TestNotebookSizers(wxCommandEvent& WXUNUSED(event) )
{
    MySizerDialog dialog( this, _T("Notebook Sizer Test Dialog") );

    dialog.ShowModal();
}


void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    (void)wxMessageBox(_T("wxWindows GUI library layout demo\n"),
            _T("About Layout Demo"), wxOK|wxICON_INFORMATION);
}

// ----------------------------------------------------------------------------
// MyConstraintsFrame
// ----------------------------------------------------------------------------

MyConstraintsFrame::MyConstraintsFrame(const wxChar *title, int x, int y)
                  : wxFrame(NULL, -1, title, wxPoint(x, y) )
{
  // Make a panel
  wxPanel *panel = new wxPanel(this);

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

  wxTextCtrl *mtext = new wxTextCtrl(panel, -1,
                                     _T("This frame is laid out using\n"
                                        "constraints, but the preferred\n"
                                        "layout mechanism now are sizers."),
                                     wxDefaultPosition,
                                     wxDefaultSize,
                                     wxTE_MULTILINE);

  wxLayoutConstraints *b3 = new wxLayoutConstraints;
  b3->top.Below         (btn1, 5);
  b3->left.RightOf      (list, 5);
  b3->right.SameAs      (panel, wxRight, 5);
  b3->bottom.SameAs     (panel, wxBottom, 5);
  mtext->SetConstraints(b3);

  wxTextCtrl *canvas = new wxTextCtrl(this, -1, _T("yet another window"));

  // Make a text window
  wxTextCtrl *text_window = new wxTextCtrl(this, -1, _T(""),
                                           wxDefaultPosition,
                                           wxDefaultSize,
                                           wxTE_MULTILINE);

  // Set constraints for panel subwindow
  wxLayoutConstraints *c1 = new wxLayoutConstraints;

  c1->left.SameAs       (this, wxLeft);
  c1->top.SameAs        (this, wxTop);
  c1->right.PercentOf   (this, wxWidth, 50);
  c1->height.PercentOf  (this, wxHeight, 50);

  panel->SetConstraints(c1);

  // Set constraints for canvas subwindow
  wxLayoutConstraints *c2 = new wxLayoutConstraints;

  c2->left.SameAs       (panel, wxRight);
  c2->top.SameAs        (this, wxTop);
  c2->right.SameAs      (this, wxRight);
  c2->height.PercentOf  (this, wxHeight, 50);

  canvas->SetConstraints(c2);

  // Set constraints for text subwindow
  wxLayoutConstraints *c3 = new wxLayoutConstraints;
  c3->left.SameAs       (this, wxLeft);
  c3->top.Below         (panel);
  c3->right.SameAs      (this, wxRight);
  c3->bottom.SameAs     (this, wxBottom);

  text_window->SetConstraints(c3);

  SetAutoLayout(TRUE);
}

// ----------------------------------------------------------------------------
//  MyFlexSizerFrame
// ----------------------------------------------------------------------------

void MyFlexSizerFrame::InitFlexSizer(wxFlexGridSizer *sizer)
{
    for ( int i = 0; i < 3; i++ )
    {
        for ( int j = 0; j < 3; j++ )
        {
            sizer->Add(new wxStaticText
                           (
                            this,
                            -1,
                            wxString::Format(_T("(%d, %d)"), i + 1, j + 1),
                            wxDefaultPosition,
                            wxDefaultSize,
                            wxALIGN_CENTER
                           ),
                       0, wxEXPAND | wxALIGN_CENTER_VERTICAL | wxALL, 3);
        }
    }
}

MyFlexSizerFrame::MyFlexSizerFrame(const wxChar *title, int x, int y )
            : wxFrame(NULL, -1, title, wxPoint(x, y) )
{
    wxFlexGridSizer *sizerFlex;

    // consttuct the first column
    wxSizer *sizerCol1 = new wxBoxSizer(wxVERTICAL);
    sizerCol1->Add(new wxStaticText(this, -1, _T("Ungrowable:")), 0, wxCENTER | wxTOP, 20);
    sizerFlex = new wxFlexGridSizer(3, 3);
    InitFlexSizer(sizerFlex);
    sizerCol1->Add(sizerFlex, 1, wxALL | wxEXPAND, 10);

    sizerCol1->Add(new wxStaticText(this, -1, _T("Growable middle column:")), 0, wxCENTER | wxTOP, 20);
    sizerFlex = new wxFlexGridSizer(3, 3);
    InitFlexSizer(sizerFlex);
    sizerFlex->AddGrowableCol(1);
    sizerCol1->Add(sizerFlex, 1, wxALL | wxEXPAND, 10);

    sizerCol1->Add(new wxStaticText(this, -1, _T("Growable middle row:")), 0, wxCENTER | wxTOP, 20);
    sizerFlex = new wxFlexGridSizer(3, 3);
    InitFlexSizer(sizerFlex);
    sizerFlex->AddGrowableRow(1);
    sizerCol1->Add(sizerFlex, 1, wxALL | wxEXPAND, 10);

    sizerCol1->Add(new wxStaticText(this, -1, _T("All growable columns:")), 0, wxCENTER | wxTOP, 20);
    sizerFlex = new wxFlexGridSizer(3, 3);
    InitFlexSizer(sizerFlex);
    sizerFlex->AddGrowableCol(0, 1);
    sizerFlex->AddGrowableCol(1, 2);
    sizerFlex->AddGrowableCol(2, 3);
    sizerCol1->Add(sizerFlex, 1, wxALL | wxEXPAND, 10);

    // the second one
    wxSizer *sizerCol2 = new wxBoxSizer(wxVERTICAL);
    sizerCol2->Add(new wxStaticText(this, -1, _T("Growable middle row and column:")), 0, wxCENTER | wxTOP, 20);
    sizerFlex = new wxFlexGridSizer(3, 3);
    InitFlexSizer(sizerFlex);
    sizerFlex->AddGrowableCol(1);
    sizerFlex->AddGrowableRow(1);
    sizerCol2->Add(sizerFlex, 1, wxALL | wxEXPAND, 10);

    sizerCol2->Add(new wxStaticText(this, -1, _T("Same with horz flex direction")), 0, wxCENTER | wxTOP, 20);
    sizerFlex = new wxFlexGridSizer(3, 3);
    InitFlexSizer(sizerFlex);
    sizerFlex->AddGrowableCol(1);
    sizerFlex->AddGrowableRow(1);
    sizerFlex->SetFlexibleDirection(wxHORIZONTAL);
    sizerCol2->Add(sizerFlex, 1, wxALL | wxEXPAND, 10);

    sizerCol2->Add(new wxStaticText(this, -1, _T("Same with grow mode == \"none\"")), 0, wxCENTER | wxTOP, 20);
    sizerFlex = new wxFlexGridSizer(3, 3);
    InitFlexSizer(sizerFlex);
    sizerFlex->AddGrowableCol(1);
    sizerFlex->AddGrowableRow(1);
    sizerFlex->SetFlexibleDirection(wxHORIZONTAL);
    sizerFlex->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_NONE);
    sizerCol2->Add(sizerFlex, 1, wxALL | wxEXPAND, 10);

    sizerCol2->Add(new wxStaticText(this, -1, _T("Same with grow mode == \"all\"")), 0, wxCENTER | wxTOP, 20);
    sizerFlex = new wxFlexGridSizer(3, 3);
    InitFlexSizer(sizerFlex);
    sizerFlex->AddGrowableCol(1);
    sizerFlex->AddGrowableRow(1);
    sizerFlex->SetFlexibleDirection(wxHORIZONTAL);
    sizerFlex->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_ALL);
    sizerCol2->Add(sizerFlex, 1, wxALL | wxEXPAND, 10);

    // add both columns to grid sizer
    wxGridSizer *sizerTop = new wxGridSizer(2, 0, 20);
    sizerTop->Add(sizerCol1, 1, wxEXPAND);
    sizerTop->Add(sizerCol2, 1, wxEXPAND);

    SetSizer(sizerTop);
    sizerTop->SetSizeHints(this);
}

// ----------------------------------------------------------------------------
// MySizerDialog
// ----------------------------------------------------------------------------

MySizerDialog::MySizerDialog(wxWindow *parent, const wxChar *title)
             : wxDialog(parent, -1, wxString(title))
{
    // Begin with first hierarchy: a notebook at the top and
    // and OK button at the bottom.

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    wxNotebook *notebook = new wxNotebook( this, -1 );
    wxNotebookSizer *nbs = new wxNotebookSizer( notebook );
    topsizer->Add( nbs, 1, wxGROW );

    wxButton *button = new wxButton( this, wxID_OK, _T("OK") );
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
    SetSizer( topsizer );
    topsizer->SetSizeHints( this );
}

