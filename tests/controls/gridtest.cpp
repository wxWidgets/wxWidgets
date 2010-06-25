///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/listbox.cpp
// Purpose:     wxListBox unit test
// Author:      Steven Lamerton
// Created:     2010-06-25
// RCS-ID:      $Id: $
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/grid.h"
#include "testableframe.h"
#include "wx/uiaction.h"

class GridTestCase : public CppUnit::TestCase
{
public:
    GridTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    CPPUNIT_TEST_SUITE( GridTestCase );
        CPPUNIT_TEST( ItemEdit );
    CPPUNIT_TEST_SUITE_END();

    void ItemEdit();

    wxGrid *m_grid;

    DECLARE_NO_COPY_CLASS(GridTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( GridTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( GridTestCase, "GridTestCase" );

void GridTestCase::setUp()
{
    m_grid = new wxGrid(wxTheApp->GetTopWindow(), wxID_ANY);
    m_grid->CreateGrid(10, 10);
    m_grid->SetSize(200, 200);
    m_grid->Refresh();
    m_grid->Update();
}

void GridTestCase::tearDown()
{
    wxDELETE(m_grid);
}

void GridTestCase::ItemEdit()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    frame->CountWindowEvents(m_grid, wxEVT_GRID_CELL_CHANGING);
    frame->CountWindowEvents(m_grid, wxEVT_GRID_CELL_CHANGED);

    wxUIActionSimulator sim;

    m_grid->SetFocus();
    m_grid->SetGridCursor(1, 1);
    m_grid->ShowCellEditControl();

    sim.Text("abab");
    sim.Char(WXK_RETURN);

    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_GRID_CELL_CHANGING));
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_GRID_CELL_CHANGED));
}
