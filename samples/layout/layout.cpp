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
#include "wx/gbsizer.h"
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
  EVT_MENU(LAYOUT_TEST_GB_SIZER, MyFrame::TestGridBagSizer)    
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
  file_menu->Append(LAYOUT_TEST_GB_SIZER, _T("Test &gridbag sizer"));

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

void MyFrame::TestGridBagSizer(wxCommandEvent& WXUNUSED(event) )
{
    MyGridBagSizerFrame *newFrame = new
        MyGridBagSizerFrame(_T("wxGridBagSizer Test Frame"), 50, 50);
    newFrame->Show(TRUE);
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
      _T("This frame is laid out using\nconstraints, but the preferred\nlayout mechanism now are sizers."),
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

// ----------------------------------------------------------------------------
// MyGridBagSizerFrame
// ----------------------------------------------------------------------------

// some simple macros to help make the sample code below more clear
#define TEXTCTRL(text)   new wxTextCtrl(p, -1, _T(text))
#define MLTEXTCTRL(text) new wxTextCtrl(p, -1, _T(text), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE)
#define POS(r, c)        wxGBPosition(r,c)
#define SPAN(r, c)       wxGBSpan(r,c)

wxChar* gbsDescription =_T("\
The wxGridBagSizer is similar to the wxFlexGridSizer except the items are explicitly positioned\n\
in a virtual cell of the layout grid, and column or row spanning is allowed.  For example, this\n\
static text is positioned at (0,0) and it spans 7 columns.");


// Some IDs
enum {
    GBS_HIDE_BTN = 1212,
    GBS_SHOW_BTN,
    GBS_MOVE_BTN1,
    GBS_MOVE_BTN2,
    
    GBS_MAX,
};


BEGIN_EVENT_TABLE(MyGridBagSizerFrame, wxFrame)
    EVT_BUTTON( GBS_HIDE_BTN,  MyGridBagSizerFrame::OnHideBtn)
    EVT_BUTTON( GBS_SHOW_BTN,  MyGridBagSizerFrame::OnShowBtn)
    EVT_BUTTON( GBS_MOVE_BTN1, MyGridBagSizerFrame::OnMoveBtn)
    EVT_BUTTON( GBS_MOVE_BTN2, MyGridBagSizerFrame::OnMoveBtn)
END_EVENT_TABLE()


MyGridBagSizerFrame::MyGridBagSizerFrame(const wxChar *title, int x, int y )
    : wxFrame( NULL, -1, title, wxPoint(x, y) )
{
    wxPanel* p = new wxPanel(this, -1);
    m_panel = p;
    m_gbs = new wxGridBagSizer();

    
    m_gbs->Add( new wxStaticText(p, -1, gbsDescription),
                POS(0,0), SPAN(1, 7),
                wxALIGN_CENTER | wxALL, 5);
    
    m_gbs->Add( TEXTCTRL("pos(1,0)"),   POS(1,0) );
    m_gbs->Add( TEXTCTRL("pos(1,1)"),   POS(1,1) );
    m_gbs->Add( TEXTCTRL("pos(2,0)"),   POS(2,0) );
    m_gbs->Add( TEXTCTRL("pos(2,1)"),   POS(2,1) );
    m_gbs->Add( MLTEXTCTRL("pos(3,2), span(1,2)\nthis row and col are growable"),
              POS(3,2), SPAN(1,2), wxEXPAND );
    m_gbs->Add( MLTEXTCTRL("pos(4,3)\nspan(3,1)"),
              POS(4,3), SPAN(3,1), wxEXPAND );
    m_gbs->Add( TEXTCTRL("pos(5,4)"),   POS(5,4), wxDefaultSpan, wxEXPAND );
    m_gbs->Add( TEXTCTRL("pos(6,5)"),   POS(6,5), wxDefaultSpan, wxEXPAND );
    m_gbs->Add( TEXTCTRL("pos(7,6)"),   POS(7,6) );
    
    //m_gbs->Add( TEXTCTRL("bad position"), POS(4,3) );  // Test for assert
    //m_gbs->Add( TEXTCTRL("bad position"), POS(5,3) );  // Test for assert


    m_moveBtn1 = new wxButton(p, GBS_MOVE_BTN1, _T("Move this to (3,6)"));
    m_moveBtn2 = new wxButton(p, GBS_MOVE_BTN2, _T("Move this to (3,6)"));
    m_gbs->Add( m_moveBtn1, POS(10,2) );
    m_gbs->Add( m_moveBtn2, POS(10,3) );
    
    m_hideBtn = new wxButton(p, GBS_HIDE_BTN, _T("Hide this item -->"));
    m_gbs->Add(m_hideBtn, POS(12, 3));

    m_hideTxt = new wxTextCtrl(p, -1, _T("pos(12,4), size(150, -1)"),
                                wxDefaultPosition, wxSize(150,-1));
    m_gbs->Add( m_hideTxt, POS(12,4) );
    
    m_showBtn = new wxButton(p, GBS_SHOW_BTN, _T("<-- Show it again"));
    m_gbs->Add(m_showBtn, POS(12, 5));
    m_showBtn->Disable();

    m_gbs->Add(10,10, POS(14,0));
    
    m_gbs->AddGrowableRow(3);
    m_gbs->AddGrowableCol(2);
    
    p->SetSizerAndFit(m_gbs);
    SetClientSize(p->GetSize());
}

    
void MyGridBagSizerFrame::OnHideBtn(wxCommandEvent&)
{
    m_gbs->Hide(m_hideTxt);
    m_hideBtn->Disable();
    m_showBtn->Enable();
    m_gbs->Layout();
}

void MyGridBagSizerFrame::OnShowBtn(wxCommandEvent&)
{
    m_gbs->Show(m_hideTxt);
    m_hideBtn->Enable();
    m_showBtn->Disable();
    m_gbs->Layout();
}


void MyGridBagSizerFrame::OnMoveBtn(wxCommandEvent& event)
{
    wxButton* btn = (wxButton*)event.GetEventObject();
    wxGBPosition curPos = m_gbs->GetItemPosition(btn);

    // if it's already at the "other" spot then move it back
    if (curPos == wxGBPosition(3,6))
    {
        m_gbs->SetItemPosition(btn, m_lastPos);
        btn->SetLabel(_T("Move this to (3,6)"));
    }
    else 
    {
        if ( m_gbs->CheckForIntersection(wxGBPosition(3,6), wxGBSpan(1,1)) )
            wxMessageBox(
_T("wxGridBagSizer will not allow items to be in the same cell as\n\
another item, so this operation will fail.  You will also get an assert\n\
when compiled in debug mode."), _T("Warning"), wxOK | wxICON_INFORMATION);
        
        if ( m_gbs->SetItemPosition(btn, wxGBPosition(3,6)) )
        {
            m_lastPos = curPos;
            btn->SetLabel(_T("Move it back"));
        }
    }
    m_gbs->Layout();
}
