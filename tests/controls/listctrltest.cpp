///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/listctrltest.cpp
// Purpose:     wxListCtrl unit test
// Author:      Vadim Zeitlin
// Created:     2008-11-26
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
//              (c) 2010 Steven Lamerton
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

#include "wx/listctrl.h"
#include "listbasetest.h"
#include "testableframe.h"
#include "wx/uiaction.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class ListCtrlTestCase : public ListBaseTestCase, public CppUnit::TestCase
{
public:
    ListCtrlTestCase() { }

    virtual void setUp();
    virtual void tearDown();

    virtual wxListCtrl *GetList() const { return m_list; }

private:
    CPPUNIT_TEST_SUITE( ListCtrlTestCase );
        wxLIST_BASE_TESTS();
        CPPUNIT_TEST( ColumnClick );
        CPPUNIT_TEST( ColumnDrag );
    CPPUNIT_TEST_SUITE_END();

    //Column events are only supported in wxListCtrl currently so we test them
    //here rather than in ListBaseTest
    void ColumnClick();
    void ColumnDrag();

    wxListCtrl *m_list;

    DECLARE_NO_COPY_CLASS(ListCtrlTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ListCtrlTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ListCtrlTestCase, "ListCtrlTestCase" );

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

void ListCtrlTestCase::setUp()
{
    m_list = new wxListCtrl(wxTheApp->GetTopWindow());
    m_list->SetWindowStyle(wxLC_REPORT);
    m_list->SetSize(400, 200);
}

void ListCtrlTestCase::tearDown()
{
    delete m_list;
    m_list = NULL;
}

void ListCtrlTestCase::ColumnDrag()
{
   wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_list, wxEVT_COMMAND_LIST_COL_BEGIN_DRAG);
    EventCounter count1(m_list, wxEVT_COMMAND_LIST_COL_DRAGGING);
    EventCounter count2(m_list, wxEVT_COMMAND_LIST_COL_END_DRAG);

    m_list->InsertColumn(0, "Column 0");
    m_list->InsertColumn(1, "Column 1");
    m_list->InsertColumn(2, "Column 2");
    m_list->Update();
    m_list->SetFocus();

    wxUIActionSimulator sim;

    wxPoint pt = m_list->ClientToScreen(wxPoint(m_list->GetColumnWidth(0), 5));

    sim.MouseDragDrop(pt.x, pt.y, pt.x + 50, pt.y);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_LIST_COL_BEGIN_DRAG));
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_LIST_COL_DRAGGING));
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_LIST_COL_END_DRAG));

    m_list->ClearAll();
}

void ListCtrlTestCase::ColumnClick()
{
   wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_list, wxEVT_COMMAND_LIST_COL_CLICK);
    EventCounter count1(m_list, wxEVT_COMMAND_LIST_COL_RIGHT_CLICK);


    m_list->InsertColumn(0, "Column 0", wxLIST_FORMAT_LEFT, 60);

    wxUIActionSimulator sim;

    sim.MouseMove(m_list->ClientToScreen(wxPoint(4, 4)));
    sim.MouseClick();
    sim.MouseClick(wxMOUSE_BTN_RIGHT);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_LIST_COL_CLICK));
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_LIST_COL_RIGHT_CLICK));

    m_list->ClearAll();
}
