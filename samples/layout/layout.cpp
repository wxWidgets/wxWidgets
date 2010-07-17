/////////////////////////////////////////////////////////////////////////////
// Name:        layout.cpp
// Purpose:     Layout sample
// Author:      Julian Smart
// Modified by: Robin Dunn, Vadim Zeitlin
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
//                  2005 Vadim Zeitlin
// Licence:     wxWindows licence
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

#include "wx/sizer.h"
#include "wx/gbsizer.h"
#include "wx/statline.h"
#include "wx/notebook.h"
#include "wx/spinctrl.h"
#include "wx/wrapsizer.h"
#include "wx/generic/stattextg.h"

#include "layout.h"

#ifndef __WXMSW__
    #include "../sample.xpm"
#endif


// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
  if ( !wxApp::OnInit() )
      return false;

  // Create the main frame window
  MyFrame *frame = new MyFrame;

  frame->Show(true);

  return true;
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(LAYOUT_ABOUT, MyFrame::OnAbout)
  EVT_MENU(LAYOUT_QUIT, MyFrame::OnQuit)

  EVT_MENU(LAYOUT_TEST_PROPORTIONS, MyFrame::TestProportions)
  EVT_MENU(LAYOUT_TEST_SIZER, MyFrame::TestFlexSizers)
  EVT_MENU(LAYOUT_TEST_NB_SIZER, MyFrame::TestNotebookSizers)
  EVT_MENU(LAYOUT_TEST_GB_SIZER, MyFrame::TestGridBagSizer)
  EVT_MENU(LAYOUT_TEST_SET_MINIMAL, MyFrame::TestSetMinimal)
  EVT_MENU(LAYOUT_TEST_NESTED, MyFrame::TestNested)
  EVT_MENU(LAYOUT_TEST_WRAP, MyFrame::TestWrap)
END_EVENT_TABLE()

// Define my frame constructor
MyFrame::MyFrame()
       : wxFrame(NULL, wxID_ANY, wxT("wxWidgets Layout Demo"))
{
    SetIcon(wxICON(sample));

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(LAYOUT_TEST_PROPORTIONS, wxT("&Proportions demo...\tF1"));
    file_menu->Append(LAYOUT_TEST_SIZER, wxT("Test wx&FlexSizer...\tF2"));
    file_menu->Append(LAYOUT_TEST_NB_SIZER, wxT("Test &notebook sizers...\tF3"));
    file_menu->Append(LAYOUT_TEST_GB_SIZER, wxT("Test &gridbag sizer...\tF4"));
    file_menu->Append(LAYOUT_TEST_SET_MINIMAL, wxT("Test Set&ItemMinSize...\tF5"));
    file_menu->Append(LAYOUT_TEST_NESTED, wxT("Test nested sizer in a wxPanel...\tF6"));
    file_menu->Append(LAYOUT_TEST_WRAP, wxT("Test wrap sizers...\tF7"));

    file_menu->AppendSeparator();
    file_menu->Append(LAYOUT_QUIT, wxT("E&xit"), wxT("Quit program"));

    wxMenu *help_menu = new wxMenu;
    help_menu->Append(LAYOUT_ABOUT, wxT("&About"), wxT("About layout demo..."));

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, wxT("&File"));
    menu_bar->Append(help_menu, wxT("&Help"));

    // Associate the menu bar with the frame
    SetMenuBar(menu_bar);

#if wxUSE_STATUSBAR
    CreateStatusBar(2);
    SetStatusText(wxT("wxWidgets layout demo"));
#endif // wxUSE_STATUSBAR

    wxPanel* p = new wxPanel(this, wxID_ANY);

    // we want to get a dialog that is stretchable because it
    // has a text ctrl in the middle. at the bottom, we have
    // two buttons which.

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    // 1) top: create wxStaticText with minimum size equal to its default size
    topsizer->Add(
        new wxStaticText( p, wxID_ANY, wxT("An explanation (wxALIGN_RIGHT).") ),
        wxSizerFlags().Align(wxALIGN_RIGHT).Border(wxALL & ~wxBOTTOM, 5));
    topsizer->Add(
        new wxStaticText( p, wxID_ANY, wxT("An explanation (wxALIGN_LEFT).") ),
        wxSizerFlags().Align(wxALIGN_LEFT).Border(wxALL & ~wxBOTTOM, 5));
    topsizer->Add(
        new wxStaticText( p, wxID_ANY, wxT("An explanation (wxALIGN_CENTRE_HORIZONTAL).") ),
        wxSizerFlags().Align(wxALIGN_CENTRE_HORIZONTAL).Border(wxALL & ~wxBOTTOM, 5));

    // 2) top: create wxTextCtrl with minimum size (100x60)
    topsizer->Add(
        new wxTextCtrl( p, wxID_ANY, wxT("My text (wxEXPAND)."), wxDefaultPosition, wxSize(100,60), wxTE_MULTILINE),
        wxSizerFlags(1).Expand().Border(wxALL, 5));

    // 2.5) Gratuitous test of wxStaticBoxSizers
    wxBoxSizer *statsizer = new wxStaticBoxSizer(
        new wxStaticBox(p, wxID_ANY, wxT("A wxStaticBoxSizer")), wxVERTICAL );
    statsizer->Add(
        new wxStaticText(p, wxID_ANY, wxT("And some TEXT inside it")),
        wxSizerFlags().Border(wxALL, 30));
    topsizer->Add(
        statsizer,
        wxSizerFlags(1).Expand().Border(wxALL, 10));

    // 2.7) And a test of wxGridSizer
    wxGridSizer *gridsizer = new wxGridSizer(2, 5, 5);
    gridsizer->Add(new wxStaticText(p, wxID_ANY, wxT("Label")),
                wxSizerFlags().Align(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL));
    gridsizer->Add(new wxTextCtrl(p, wxID_ANY, wxT("Grid sizer demo")),
                wxSizerFlags(1).Align(wxGROW | wxALIGN_CENTER_VERTICAL));
    gridsizer->Add(new wxStaticText(p, wxID_ANY, wxT("Another label")),
                wxSizerFlags().Align(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL));
    gridsizer->Add(new wxTextCtrl(p, wxID_ANY, wxT("More text")),
                wxSizerFlags(1).Align(wxGROW | wxALIGN_CENTER_VERTICAL));
    gridsizer->Add(new wxStaticText(p, wxID_ANY, wxT("Final label")),
                wxSizerFlags().Align(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL));
    gridsizer->Add(new wxTextCtrl(p, wxID_ANY, wxT("And yet more text")),
                wxSizerFlags().Align(wxGROW | wxALIGN_CENTER_VERTICAL));
    topsizer->Add(
        gridsizer,
        wxSizerFlags().Proportion(1).Expand().Border(wxALL, 10));


#if wxUSE_STATLINE
    // 3) middle: create wxStaticLine with minimum size (3x3)
    topsizer->Add(
        new wxStaticLine( p, wxID_ANY, wxDefaultPosition, wxSize(3,3), wxHORIZONTAL),
        wxSizerFlags().Expand());
#endif // wxUSE_STATLINE


    // 4) bottom: create two centred wxButtons
    wxBoxSizer *button_box = new wxBoxSizer( wxHORIZONTAL );
    button_box->Add(
        new wxButton( p, wxID_ANY, wxT("Two buttons in a box") ),
        wxSizerFlags().Border(wxALL, 7));
    button_box->Add(
        new wxButton( p, wxID_ANY, wxT("(wxCENTER)") ),
        wxSizerFlags().Border(wxALL, 7));

    topsizer->Add(button_box, wxSizerFlags().Center());

    p->SetSizer( topsizer );

    // don't allow frame to get smaller than what the sizers tell it and also set
    // the initial size as calculated by the sizers
    topsizer->SetSizeHints( this );
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::TestProportions(wxCommandEvent& WXUNUSED(event))
{
    (new MyProportionsFrame(this))->Show();
}

void MyFrame::TestFlexSizers(wxCommandEvent& WXUNUSED(event) )
{
    MyFlexSizerFrame *newFrame = new MyFlexSizerFrame(wxT("Flex Sizer Test Frame"), 50, 50);
    newFrame->Show(true);
}

void MyFrame::TestNotebookSizers(wxCommandEvent& WXUNUSED(event) )
{
    MySizerDialog dialog( this, wxT("Notebook Sizer Test Dialog") );

    dialog.ShowModal();
}

void MyFrame::TestSetMinimal(wxCommandEvent& WXUNUSED(event) )
{
    MySimpleSizerFrame *newFrame = new MySimpleSizerFrame(wxT("Simple Sizer Test Frame"), 50, 50);
    newFrame->Show(true);
}

void MyFrame::TestNested(wxCommandEvent& WXUNUSED(event) )
{
    MyNestedSizerFrame *newFrame = new MyNestedSizerFrame(wxT("Nested Sizer Test Frame"), 50, 50);
    newFrame->Show(true);
}

void MyFrame::TestWrap(wxCommandEvent& WXUNUSED(event) )
{
    MyWrapSizerFrame *newFrame = new MyWrapSizerFrame(wxT("Wrap Sizer Test Frame"), 50, 50);
    newFrame->Show(true);
}


void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    (void)wxMessageBox(wxT("wxWidgets GUI library layout demo\n"),
            wxT("About Layout Demo"), wxOK|wxICON_INFORMATION);
}

void MyFrame::TestGridBagSizer(wxCommandEvent& WXUNUSED(event) )
{
    MyGridBagSizerFrame *newFrame = new
        MyGridBagSizerFrame(wxT("wxGridBagSizer Test Frame"), 50, 50);
    newFrame->Show(true);
}

// ----------------------------------------------------------------------------
// MyProportionsFrame
// ----------------------------------------------------------------------------

MyProportionsFrame::MyProportionsFrame(wxFrame *parent)
                  : wxFrame(parent, wxID_ANY, wxT("Box Sizer Proportions Demo"))
{
    size_t n;

    // create the controls
    wxPanel *panel = new wxPanel(this, wxID_ANY);
    for ( n = 0; n < WXSIZEOF(m_spins); n++ )
    {
        m_spins[n] = new wxSpinCtrl(panel);
        m_spins[n]->SetValue(n);
    }

    // lay them out
    m_sizer = new wxStaticBoxSizer(wxHORIZONTAL, panel,
                wxT("Try changing elements proportions and resizing the window"));
    for ( n = 0; n < WXSIZEOF(m_spins); n++ )
        m_sizer->Add(m_spins[n], wxSizerFlags().Border());

    // put everything together
    panel->SetSizer(m_sizer);
    wxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
    sizerTop->Add(panel, wxSizerFlags(1).Expand().Border());
    UpdateProportions();
    SetSizerAndFit(sizerTop);

    // and connect the events
    Connect(wxEVT_COMMAND_TEXT_UPDATED,
                wxCommandEventHandler(MyProportionsFrame::OnProportionUpdated));
    Connect(wxEVT_COMMAND_SPINCTRL_UPDATED,
            wxSpinEventHandler(MyProportionsFrame::OnProportionChanged));
}

void MyProportionsFrame::UpdateProportions()
{
    for ( size_t n = 0; n < WXSIZEOF(m_spins); n++ )
    {
        m_sizer->GetItem(n)->SetProportion(m_spins[n]->GetValue());
    }

    m_sizer->Layout();
}

void MyProportionsFrame::OnProportionUpdated(wxCommandEvent& WXUNUSED(event))
{
    UpdateProportions();
}

void MyProportionsFrame::OnProportionChanged(wxSpinEvent& WXUNUSED(event))
{
    UpdateProportions();
}

// ----------------------------------------------------------------------------
//  MyFlexSizerFrame
// ----------------------------------------------------------------------------

void MyFlexSizerFrame::InitFlexSizer(wxFlexGridSizer *sizer, wxWindow* parent)
{
    for ( int i = 0; i < 3; i++ )
    {
        for ( int j = 0; j < 3; j++ )
        {
            wxWindow * const cell = new wxGenericStaticText
                                        (
                                            parent,
                                            wxID_ANY,
                                            wxString::Format("(%d, %d)",
                                                             i + 1, j + 1)
                                        );
            if ( (i + j) % 2 )
                cell->SetBackgroundColour( *wxLIGHT_GREY );
            sizer->Add(cell, 0, wxEXPAND | wxALIGN_CENTER_VERTICAL | wxALL, 3);
        }
    }
}

MyFlexSizerFrame::MyFlexSizerFrame(const wxString &title, int x, int y )
            : wxFrame(NULL, wxID_ANY, title, wxPoint(x, y) )
{
    wxFlexGridSizer *sizerFlex;
    wxPanel* p = new wxPanel(this, wxID_ANY);

    // consttuct the first column
    wxSizer *sizerCol1 = new wxBoxSizer(wxVERTICAL);
    sizerCol1->Add(new wxStaticText(p, wxID_ANY, wxT("Ungrowable:")), 0, wxCENTER | wxTOP, 20);
    sizerFlex = new wxFlexGridSizer(3, 3, wxSize(5, 5));
    InitFlexSizer(sizerFlex, p);
    sizerCol1->Add(sizerFlex, 1, wxALL | wxEXPAND, 10);

    sizerCol1->Add(new wxStaticText(p, wxID_ANY, wxT("Growable middle column:")), 0, wxCENTER | wxTOP, 20);
    sizerFlex = new wxFlexGridSizer(3, 3, wxSize(5, 5));
    InitFlexSizer(sizerFlex, p);
    sizerFlex->AddGrowableCol(1);
    sizerCol1->Add(sizerFlex, 1, wxALL | wxEXPAND, 10);

    sizerCol1->Add(new wxStaticText(p, wxID_ANY, wxT("Growable middle row:")), 0, wxCENTER | wxTOP, 20);
    sizerFlex = new wxFlexGridSizer(3, 3, wxSize(5, 5));
    InitFlexSizer(sizerFlex, p);
    sizerFlex->AddGrowableRow(1);
    sizerCol1->Add(sizerFlex, 1, wxALL | wxEXPAND, 10);

    sizerCol1->Add(new wxStaticText(p, wxID_ANY, wxT("All growable columns:")), 0, wxCENTER | wxTOP, 20);
    sizerFlex = new wxFlexGridSizer(3, 3, wxSize(5, 5));
    InitFlexSizer(sizerFlex, p);
    sizerFlex->AddGrowableCol(0, 1);
    sizerFlex->AddGrowableCol(1, 2);
    sizerFlex->AddGrowableCol(2, 3);
    sizerCol1->Add(sizerFlex, 1, wxALL | wxEXPAND, 10);

    // the second one
    wxSizer *sizerCol2 = new wxBoxSizer(wxVERTICAL);
    sizerCol2->Add(new wxStaticText(p, wxID_ANY, wxT("Growable middle row and column:")), 0, wxCENTER | wxTOP, 20);
    sizerFlex = new wxFlexGridSizer(3, 3, wxSize(5, 5));
    InitFlexSizer(sizerFlex, p);
    sizerFlex->AddGrowableCol(1);
    sizerFlex->AddGrowableRow(1);
    sizerCol2->Add(sizerFlex, 1, wxALL | wxEXPAND, 10);

    sizerCol2->Add(new wxStaticText(p, wxID_ANY, wxT("Same with horz flex direction")), 0, wxCENTER | wxTOP, 20);
    sizerFlex = new wxFlexGridSizer(3, 3, wxSize(5, 5));
    InitFlexSizer(sizerFlex, p);
    sizerFlex->AddGrowableCol(1);
    sizerFlex->AddGrowableRow(1);
    sizerFlex->SetFlexibleDirection(wxHORIZONTAL);
    sizerCol2->Add(sizerFlex, 1, wxALL | wxEXPAND, 10);

    sizerCol2->Add(new wxStaticText(p, wxID_ANY, wxT("Same with grow mode == \"none\"")), 0, wxCENTER | wxTOP, 20);
    sizerFlex = new wxFlexGridSizer(3, 3, wxSize(5, 5));
    InitFlexSizer(sizerFlex, p);
    sizerFlex->AddGrowableCol(1);
    sizerFlex->AddGrowableRow(1);
    sizerFlex->SetFlexibleDirection(wxHORIZONTAL);
    sizerFlex->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_NONE);
    sizerCol2->Add(sizerFlex, 1, wxALL | wxEXPAND, 10);

    sizerCol2->Add(new wxStaticText(p, wxID_ANY, wxT("Same with grow mode == \"all\"")), 0, wxCENTER | wxTOP, 20);
    sizerFlex = new wxFlexGridSizer(3, 3, wxSize(5, 5));
    InitFlexSizer(sizerFlex, p);
    sizerFlex->AddGrowableCol(1);
    sizerFlex->AddGrowableRow(1);
    sizerFlex->SetFlexibleDirection(wxHORIZONTAL);
    sizerFlex->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_ALL);
    sizerCol2->Add(sizerFlex, 1, wxALL | wxEXPAND, 10);

    // add both columns to grid sizer
    wxGridSizer *sizerTop = new wxGridSizer(2, 0, 20);
    sizerTop->Add(sizerCol1, 1, wxEXPAND);
    sizerTop->Add(sizerCol2, 1, wxEXPAND);

    p->SetSizer(sizerTop);
    sizerTop->SetSizeHints(this);
}

// ----------------------------------------------------------------------------
// MySizerDialog
// ----------------------------------------------------------------------------

MySizerDialog::MySizerDialog(wxWindow *parent, const wxString &title)
             : wxDialog(parent, wxID_ANY, wxString(title))
{
    // Begin with first hierarchy: a notebook at the top and
    // and OK button at the bottom.

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    wxNotebook *notebook = new wxNotebook( this, wxID_ANY );
    topsizer->Add( notebook, 1, wxGROW );

    wxButton *button = new wxButton( this, wxID_OK, wxT("OK") );
    topsizer->Add( button, 0, wxALIGN_RIGHT | wxALL, 10 );

    // First page: one big text ctrl
    wxTextCtrl *multi = new wxTextCtrl( notebook, wxID_ANY, wxT("TextCtrl."), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
    notebook->AddPage( multi, wxT("Page One") );

    // Second page: a text ctrl and a button
    wxPanel *panel = new wxPanel( notebook, wxID_ANY );
    notebook->AddPage( panel, wxT("Page Two") );

    wxSizer *panelsizer = new wxBoxSizer( wxVERTICAL );

    wxTextCtrl *text = new wxTextCtrl( panel, wxID_ANY, wxT("TextLine 1."), wxDefaultPosition, wxSize(250,wxDefaultCoord) );
    panelsizer->Add( text, 0, wxGROW|wxALL, 30 );
    text = new wxTextCtrl( panel, wxID_ANY, wxT("TextLine 2."), wxDefaultPosition, wxSize(250,wxDefaultCoord) );
    panelsizer->Add( text, 0, wxGROW|wxALL, 30 );
    wxButton *button2 = new wxButton( panel, wxID_ANY, wxT("Hallo") );
    panelsizer->Add( button2, 0, wxALIGN_RIGHT | wxLEFT|wxRIGHT|wxBOTTOM, 30 );

    panel->SetSizer( panelsizer );

    // Tell dialog to use sizer
    SetSizerAndFit( topsizer );
}

// ----------------------------------------------------------------------------
// MyGridBagSizerFrame
// ----------------------------------------------------------------------------

// some simple macros to help make the sample code below more clear
#define TEXTCTRL(text)   new wxTextCtrl(p, wxID_ANY, wxT(text))
#define MLTEXTCTRL(text) new wxTextCtrl(p, wxID_ANY, wxT(text), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE)
#define POS(r, c)        wxGBPosition(r,c)
#define SPAN(r, c)       wxGBSpan(r,c)

const wxChar gbsDescription[] =wxT("\
The wxGridBagSizer is similar to the wxFlexGridSizer except the items are explicitly positioned\n\
in a virtual cell of the layout grid, and column or row spanning is allowed.  For example, this\n\
static text is positioned at (0,0) and it spans 7 columns.");


// Some IDs
enum {
    GBS_HIDE_BTN = 1212,
    GBS_SHOW_BTN,
    GBS_MOVE_BTN1,
    GBS_MOVE_BTN2,

    GBS_MAX
};


BEGIN_EVENT_TABLE(MyGridBagSizerFrame, wxFrame)
    EVT_BUTTON( GBS_HIDE_BTN,  MyGridBagSizerFrame::OnHideBtn)
    EVT_BUTTON( GBS_SHOW_BTN,  MyGridBagSizerFrame::OnShowBtn)
    EVT_BUTTON( GBS_MOVE_BTN1, MyGridBagSizerFrame::OnMoveBtn)
    EVT_BUTTON( GBS_MOVE_BTN2, MyGridBagSizerFrame::OnMoveBtn)
END_EVENT_TABLE()


MyGridBagSizerFrame::MyGridBagSizerFrame(const wxString &title, int x, int y )
    : wxFrame( NULL, wxID_ANY, title, wxPoint(x, y) )
{
    wxPanel* p = new wxPanel(this, wxID_ANY);
    m_panel = p;
    m_gbs = new wxGridBagSizer();


    m_gbs->Add( new wxStaticText(p, wxID_ANY, gbsDescription),
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


    m_moveBtn1 = new wxButton(p, GBS_MOVE_BTN1, wxT("Move this to (3,6)"));
    m_moveBtn2 = new wxButton(p, GBS_MOVE_BTN2, wxT("Move this to (3,6)"));
    m_gbs->Add( m_moveBtn1, POS(10,2) );
    m_gbs->Add( m_moveBtn2, POS(10,3) );

    m_hideBtn = new wxButton(p, GBS_HIDE_BTN, wxT("Hide this item -->"));
    m_gbs->Add(m_hideBtn, POS(12, 3));

    m_hideTxt = new wxTextCtrl(p, wxID_ANY, wxT("pos(12,4), size(150, wxDefaultCoord)"),
                                wxDefaultPosition, wxSize(150,wxDefaultCoord));
    m_gbs->Add( m_hideTxt, POS(12,4) );

    m_showBtn = new wxButton(p, GBS_SHOW_BTN, wxT("<-- Show it again"));
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
        btn->SetLabel(wxT("Move this to (3,6)"));
    }
    else
    {
        if ( m_gbs->CheckForIntersection(wxGBPosition(3,6), wxGBSpan(1,1)) )
            wxMessageBox(
wxT("wxGridBagSizer will not allow items to be in the same cell as\n\
another item, so this operation will fail.  You will also get an assert\n\
when compiled in debug mode."), wxT("Warning"), wxOK | wxICON_INFORMATION);

        if ( m_gbs->SetItemPosition(btn, wxGBPosition(3,6)) )
        {
            m_lastPos = curPos;
            btn->SetLabel(wxT("Move it back"));
        }
    }
    m_gbs->Layout();
}

// ----------------------------------------------------------------------------
// MySimpleSizerFrame
// ----------------------------------------------------------------------------

// Some IDs
enum {
    ID_SET_SMALL = 1300,
    ID_SET_BIG
};

BEGIN_EVENT_TABLE(MySimpleSizerFrame, wxFrame)
    EVT_MENU( ID_SET_SMALL, MySimpleSizerFrame::OnSetSmallSize)
    EVT_MENU( ID_SET_BIG, MySimpleSizerFrame::OnSetBigSize)
END_EVENT_TABLE()

MySimpleSizerFrame::MySimpleSizerFrame(const wxString &title, int x, int y )
    : wxFrame( NULL, wxID_ANY, title, wxPoint(x, y) )
{
    wxMenu *menu = new wxMenu;

    menu->Append(ID_SET_SMALL, wxT("Make text control small\tF4"));
    menu->Append(ID_SET_BIG, wxT("Make text control big\tF5"));

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(menu, wxT("&File"));

    SetMenuBar( menu_bar );

    wxBoxSizer *main_sizer = new wxBoxSizer( wxHORIZONTAL );

    m_target = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80, wxDefaultCoord ) );
    main_sizer->Add( m_target, 1, wxALL, 5 );

    main_sizer->Add( new wxStaticText( this, wxID_ANY, wxT("Set alternating sizes using F4 and F5") ), 0, wxALL, 5 );

    SetSizer( main_sizer);

    Layout();
    GetSizer()->Fit( this );
}

void MySimpleSizerFrame::OnSetSmallSize( wxCommandEvent& WXUNUSED(event))
{
    GetSizer()->SetItemMinSize( m_target, 40, -1 );
    Layout();
    GetSizer()->Fit( this );
}

void MySimpleSizerFrame::OnSetBigSize( wxCommandEvent& WXUNUSED(event))
{
    GetSizer()->SetItemMinSize( m_target, 140, -1 );
    Layout();
    GetSizer()->Fit( this );
}


// ----------------------------------------------------------------------------
// MyNestedSizerFrame
// ----------------------------------------------------------------------------


MyNestedSizerFrame::MyNestedSizerFrame(const wxString &title, int x, int y )
    : wxFrame( NULL, wxID_ANY, title, wxPoint(x, y) )
{
    wxMenu *menu = new wxMenu;

    menu->Append(wxID_ABOUT, wxT("Do nothing"));

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(menu, wxT("&File"));

    SetMenuBar( menu_bar );

    wxBoxSizer *main_sizer = new wxBoxSizer( wxVERTICAL );

    main_sizer->Add( new wxStaticText( this, -1, wxT("Hello outside") ), 0, wxALIGN_CENTER );
    main_sizer->Add( new wxStaticText( this, -1, wxT("Hello outside") ), 0, wxALIGN_CENTER );
    main_sizer->Add( new wxStaticText( this, -1, wxT("Hello outside") ), 0, wxALIGN_CENTER );
    main_sizer->Add( new wxStaticText( this, -1, wxT("Hello outside") ), 0, wxALIGN_CENTER );

    wxPanel *panel = new wxPanel( this, -1, wxDefaultPosition, wxDefaultSize,
                                  wxTAB_TRAVERSAL | wxSUNKEN_BORDER );
    main_sizer->Add( panel, 0, wxALIGN_CENTER );
    wxBoxSizer *panel_sizer = new wxBoxSizer( wxVERTICAL );
    panel->SetSizer( panel_sizer );
    panel_sizer->Add( new wxStaticText( panel, -1, wxT("Hello inside") ) );
    panel_sizer->Add( new wxStaticText( panel, -1, wxT("Hello inside") ) );
    panel_sizer->Add( new wxStaticText( panel, -1, wxT("Hello inside") ) );

    main_sizer->Add( new wxStaticText( this, -1, wxT("Hello outside") ), 0, wxALIGN_CENTER );

    m_target = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80, wxDefaultCoord ) );
    main_sizer->Add( m_target, 1, wxALL|wxGROW, 5 );

    SetSizerAndFit( main_sizer);
}


// ----------------------------------------------------------------------------
// MyWrapSizerFrame
// ----------------------------------------------------------------------------


MyWrapSizerFrame::MyWrapSizerFrame(const wxString &title, int x, int y )
    : wxFrame( NULL, wxID_ANY, title, wxPoint(x, y), wxSize(200,-1) )
{
    wxMenu *menu = new wxMenu;

    menu->Append(wxID_ABOUT, "Do nothing");

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(menu, "&File");

    SetMenuBar( menu_bar );

    wxBoxSizer *root = new wxBoxSizer( wxVERTICAL );

#if 0
    wxSizer *row = new wxWrapSizer;
    int i;
    for (i = 0; i < 4; i++)
       row->Add( new wxButton( this, -1, "Hello" ), 0, wxALL, 10 );
    root->Add( row, 0, wxGROW );

    row = new wxWrapSizer;
    for (i = 0; i < 4; i++)
       row->Add( new wxButton( this, -1, "Hello" ) );
    root->Add( row, 0, wxGROW );

#else
    // A number of checkboxes inside a wrap sizer
    wxSizer *ps_mid = new wxStaticBoxSizer( wxVERTICAL, this, "Wrapping check-boxes" );
    wxSizer *ps_mid_wrap = new wxWrapSizer(wxHORIZONTAL);
    ps_mid->Add( ps_mid_wrap, 100, wxEXPAND );
    for( int ix=0; ix<6; ix++ )
            ps_mid_wrap->Add( new wxCheckBox(this,wxID_ANY,wxString::Format("Option %d",ix+1)), 0, wxALIGN_CENTRE|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    root->Add( ps_mid, 0, wxEXPAND | wxALL, 5 );

    // A shaped item inside a box sizer
    wxSizer *ps_bottom = new wxStaticBoxSizer( wxVERTICAL, this, "With wxSHAPED item" );
    wxSizer *ps_bottom_box = new wxBoxSizer(wxHORIZONTAL);
    ps_bottom->Add( ps_bottom_box, 100, wxEXPAND );
    ps_bottom_box->Add( new wxListBox(this,wxID_ANY,wxPoint(0,0),wxSize(70,70)), 0, wxEXPAND|wxSHAPED );
    ps_bottom_box->Add( 10,10 );
    ps_bottom_box->Add( new wxCheckBox(this,wxID_ANY,"A much longer option..."), 100, 0, 5 );

    root->Add( ps_bottom, 1, wxEXPAND | wxALL, 5 );
#endif

    // Set sizer for window
    SetSizerAndFit( root );
}

