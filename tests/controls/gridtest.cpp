///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/listbox.cpp
// Purpose:     wxListBox unit test
// Author:      Steven Lamerton
// Created:     2010-06-25
// RCS-ID:      $Id$
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
        CPPUNIT_TEST( CellEdit );
        CPPUNIT_TEST( CellClick );
    CPPUNIT_TEST_SUITE_END();

    void CellEdit();
    void CellClick();

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
    m_grid->CreateGrid(10, 2);
    m_grid->SetSize(400, 200);
    m_grid->Refresh();
    m_grid->Update();
}

void GridTestCase::tearDown()
{
    wxDELETE(m_grid);
}

void GridTestCase::CellEdit()
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

void GridTestCase::CellClick()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    frame->CountWindowEvents(m_grid, wxEVT_GRID_CELL_LEFT_CLICK);
    frame->CountWindowEvents(m_grid, wxEVT_GRID_CELL_LEFT_DCLICK);
    frame->CountWindowEvents(m_grid, wxEVT_GRID_CELL_RIGHT_CLICK);
    frame->CountWindowEvents(m_grid, wxEVT_GRID_CELL_RIGHT_DCLICK);


    wxUIActionSimulator sim;

    wxRect rect = m_grid->CellToRect(0, 0);
    wxPoint point = m_grid->CalcScrolledPosition(rect.GetPosition());
    point = frame->ClientToScreen(point + wxPoint(m_grid->GetRowLabelSize(),
                                                  m_grid->GetColLabelSize())
                                        + wxPoint(2, 2));

    sim.MouseMove(point);
    sim.MouseClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_GRID_CELL_LEFT_CLICK));

    sim.MouseDblClick();
    wxYield();

    //A double click event sends a single click event first
    //test to ensure this still happens in the future
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_GRID_CELL_LEFT_CLICK));
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_GRID_CELL_LEFT_DCLICK));

    sim.MouseClick(wxMOUSE_BTN_RIGHT);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_GRID_CELL_RIGHT_CLICK));

    sim.MouseDblClick(wxMOUSE_BTN_RIGHT);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_GRID_CELL_RIGHT_CLICK));
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_GRID_CELL_RIGHT_DCLICK)); 
}
