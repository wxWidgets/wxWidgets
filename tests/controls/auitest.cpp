///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/auitest.cpp
// Purpose:     wxAui control tests
// Author:      Sebastian Walderich
// Created:     2018-12-19
// Copyright:   (c) 2018 Sebastian Walderich
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/aui/auibook.h"
#include "wx/panel.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

TEST_CASE( "wxAuiNotebook::DoGetBestSize", "[aui]" ) {
  wxWindow *frame = wxTheApp->GetTopWindow();
  REQUIRE( frame );
  wxAuiNotebook *nb = new wxAuiNotebook(frame);
  REQUIRE ( nb );
  wxPanel *p = new wxPanel(frame);
  REQUIRE ( p );
  p->SetMinSize(wxSize(100, 100));
  REQUIRE( nb->AddPage(p, "Center Pane") );
  int tabHeight = nb->GetTabCtrlHeight();
  wxSize auiSize;

  SECTION( "Single pane with multiple tabs" ) {
    p = new wxPanel(frame);
    p->SetMinSize(wxSize(300, 100));
    nb->AddPage(p, "Center Tab 2");

    p = new wxPanel(frame);
    p->SetMinSize(wxSize(100, 200));
    nb->AddPage(p, "Center Tab 3");

    auiSize = nb->GetBestSize();
    CHECK( auiSize.x == 300 );
    CHECK( auiSize.y == 200 + tabHeight );
  }

  SECTION( "Horizontal split" ) {
    p = new wxPanel(frame);
    p->SetMinSize(wxSize(25, 0));
    nb->AddPage(p, "Left Pane");
    nb->Split(nb->GetPageCount()-1, wxLEFT);

    auiSize = nb->GetBestSize();
    CHECK( auiSize.x == 125 );
    CHECK( auiSize.y == 100 + tabHeight );

    p = new wxPanel(frame);
    p->SetMinSize(wxSize(50, 0));
    nb->AddPage(p, "Right Pane 1");
    nb->Split(nb->GetPageCount()-1, wxRIGHT);

    auiSize = nb->GetBestSize();
    CHECK( auiSize.x == 175 );
    CHECK( auiSize.y == 100 + tabHeight );

    p = new wxPanel(frame);
    p->SetMinSize(wxSize(100, 0));
    nb->AddPage(p, "Right Pane 2");
    nb->Split(nb->GetPageCount()-1, wxRIGHT);

    auiSize = nb->GetBestSize();
    CHECK( auiSize.x == 275 );
    CHECK( auiSize.y == 100 + tabHeight );
  }

  SECTION( "Vertical split" ) {
    p = new wxPanel(frame);
    p->SetMinSize(wxSize(0, 100));
    nb->AddPage(p, "Top Pane 1");
    nb->Split(nb->GetPageCount()-1, wxTOP);

    p = new wxPanel(frame);
    p->SetMinSize(wxSize(0, 50));
    nb->AddPage(p, "Top Pane 2");
    nb->Split(nb->GetPageCount()-1, wxTOP);

    auiSize = nb->GetBestSize();
    CHECK( auiSize.x == 100 );
    CHECK( auiSize.y == 250 + 3*tabHeight );

    p = new wxPanel(frame);
    p->SetMinSize(wxSize(0, 25));
    nb->AddPage(p, "Bottom Pane");
    nb->Split(nb->GetPageCount()-1, wxBOTTOM);

    auiSize = nb->GetBestSize();
    CHECK( auiSize.x == 100 );
    CHECK( auiSize.y == 275 + 4*tabHeight );
  }

  SECTION( "Surrounding panes" ) {
    p = new wxPanel(frame);
    p->SetMinSize(wxSize(50, 25));
    nb->AddPage(p, "Bottom Pane");
    nb->Split(nb->GetPageCount()-1, wxBOTTOM);

    p = new wxPanel(frame);
    p->SetMinSize(wxSize(50, 120));
    nb->AddPage(p, "Right Pane");
    nb->Split(nb->GetPageCount()-1, wxRIGHT);

    p = new wxPanel(frame);
    p->SetMinSize(wxSize(225, 50));
    nb->AddPage(p, "Top Pane");
    nb->Split(nb->GetPageCount()-1, wxTOP);

    p = new wxPanel(frame);
    p->SetMinSize(wxSize(25, 105));
    nb->AddPage(p, "Left Pane");
    nb->Split(nb->GetPageCount()-1, wxLEFT);

    auiSize = nb->GetBestSize();
    CHECK( auiSize.x == 250 );
    CHECK( auiSize.y == 175 + 3*tabHeight );
  }

  wxDELETE(nb);
}
