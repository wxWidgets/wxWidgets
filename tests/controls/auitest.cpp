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

#if wxUSE_AUI


#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/panel.h"

#include "wx/aui/auibook.h"

#include "asserthelper.h"

// ----------------------------------------------------------------------------
// test fixtures
// ----------------------------------------------------------------------------

class AuiNotebookTestCase
{
public:
    AuiNotebookTestCase()
        : nb(new wxAuiNotebook(wxTheApp->GetTopWindow()))
    {
    }

    ~AuiNotebookTestCase()
    {
        delete nb;
    }

protected:
    wxAuiNotebook* const nb;
};

// ----------------------------------------------------------------------------
// the tests themselves
// ----------------------------------------------------------------------------

TEST_CASE_METHOD(AuiNotebookTestCase, "wxAuiNotebook::DoGetBestSize", "[aui]")
{
    wxPanel *p = new wxPanel(nb);
    p->SetMinSize(wxSize(100, 100));
    REQUIRE( nb->AddPage(p, "Center Pane") );

    const int tabHeight = nb->GetTabCtrlHeight();

    SECTION( "Single pane with multiple tabs" )
    {
        p = new wxPanel(nb);
        p->SetMinSize(wxSize(300, 100));
        nb->AddPage(p, "Center Tab 2");

        p = new wxPanel(nb);
        p->SetMinSize(wxSize(100, 200));
        nb->AddPage(p, "Center Tab 3");

        CHECK( nb->GetBestSize() == wxSize(300, 200 + tabHeight) );
    }

    SECTION( "Horizontal split" )
    {
        p = new wxPanel(nb);
        p->SetMinSize(wxSize(25, 0));
        nb->AddPage(p, "Left Pane");
        nb->Split(nb->GetPageCount()-1, wxLEFT);

        CHECK( nb->GetBestSize() == wxSize(125, 100 + tabHeight) );

        p = new wxPanel(nb);
        p->SetMinSize(wxSize(50, 0));
        nb->AddPage(p, "Right Pane 1");
        nb->Split(nb->GetPageCount()-1, wxRIGHT);

        CHECK( nb->GetBestSize() == wxSize(175, 100 + tabHeight) );

        p = new wxPanel(nb);
        p->SetMinSize(wxSize(100, 0));
        nb->AddPage(p, "Right Pane 2");
        nb->Split(nb->GetPageCount()-1, wxRIGHT);

        CHECK( nb->GetBestSize() == wxSize(275, 100 + tabHeight) );
    }

    SECTION( "Vertical split" )
    {
        p = new wxPanel(nb);
        p->SetMinSize(wxSize(0, 100));
        nb->AddPage(p, "Top Pane 1");
        nb->Split(nb->GetPageCount()-1, wxTOP);

        p = new wxPanel(nb);
        p->SetMinSize(wxSize(0, 50));
        nb->AddPage(p, "Top Pane 2");
        nb->Split(nb->GetPageCount()-1, wxTOP);

        CHECK( nb->GetBestSize() == wxSize(100, 250 + 3*tabHeight) );

        p = new wxPanel(nb);
        p->SetMinSize(wxSize(0, 25));
        nb->AddPage(p, "Bottom Pane");
        nb->Split(nb->GetPageCount()-1, wxBOTTOM);

        CHECK( nb->GetBestSize() == wxSize(100, 275 + 4*tabHeight) );
    }

    SECTION( "Surrounding panes" )
    {
        p = new wxPanel(nb);
        p->SetMinSize(wxSize(50, 25));
        nb->AddPage(p, "Bottom Pane");
        nb->Split(nb->GetPageCount()-1, wxBOTTOM);

        p = new wxPanel(nb);
        p->SetMinSize(wxSize(50, 120));
        nb->AddPage(p, "Right Pane");
        nb->Split(nb->GetPageCount()-1, wxRIGHT);

        p = new wxPanel(nb);
        p->SetMinSize(wxSize(225, 50));
        nb->AddPage(p, "Top Pane");
        nb->Split(nb->GetPageCount()-1, wxTOP);

        p = new wxPanel(nb);
        p->SetMinSize(wxSize(25, 105));
        nb->AddPage(p, "Left Pane");
        nb->Split(nb->GetPageCount()-1, wxLEFT);

        CHECK( nb->GetBestSize() == wxSize(250, 175 + 3*tabHeight) );
    }
}

TEST_CASE_METHOD(AuiNotebookTestCase, "wxAuiNotebook::RTTI", "[aui][rtti]")
{
    wxBookCtrlBase* const book = nb;
    CHECK( wxDynamicCast(book, wxAuiNotebook) == nb );

    CHECK( wxDynamicCast(nb, wxBookCtrlBase) == book );
}

TEST_CASE_METHOD(AuiNotebookTestCase, "wxAuiNotebook::FindPage", "[aui]")
{
    wxPanel *p1 = new wxPanel(nb);
    wxPanel *p2 = new wxPanel(nb);
    wxPanel *p3 = new wxPanel(nb);
    REQUIRE( nb->AddPage(p1, "Page 1") );
    REQUIRE( nb->AddPage(p2, "Page 2") );

    CHECK( nb->FindPage(nullptr) == wxNOT_FOUND );
    CHECK( nb->FindPage(p1) == 0 );
    CHECK( nb->FindPage(p2) == 1 );
    CHECK( nb->FindPage(p3) == wxNOT_FOUND );
}

#endif
