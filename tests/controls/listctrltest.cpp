///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/listctrltest.cpp
// Purpose:     wxListCtrl unit test
// Author:      Vadim Zeitlin
// Created:     2008-11-26
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
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
#include "testableframe.h"
#include "wx/uiaction.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class ListCtrlTestCase : public CppUnit::TestCase
{
public:
    ListCtrlTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    CPPUNIT_TEST_SUITE( ListCtrlTestCase );
#ifdef wxHAS_LISTCTRL_COLUMN_ORDER
        CPPUNIT_TEST( ColumnsOrder );
#endif // wxHAS_LISTCTRL_COLUMN_ORDER
        CPPUNIT_TEST( ItemRect );
        CPPUNIT_TEST( ChangeMode );
        CPPUNIT_TEST( ItemClick );
        CPPUNIT_TEST( KeyDown );
        CPPUNIT_TEST( DeleteItems );
        CPPUNIT_TEST( ColumnClick );
        CPPUNIT_TEST( InsertItem );
    CPPUNIT_TEST_SUITE_END();

#ifdef wxHAS_LISTCTRL_COLUMN_ORDER
    void ColumnsOrder();
#endif // wxHAS_LISTCTRL_COLUMN_ORDER
    void ItemRect();
    void ChangeMode();
    void ItemClick();
    void KeyDown();
    void DeleteItems();
    void ColumnClick();
    void InsertItem();

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
}

void ListCtrlTestCase::tearDown()
{
    delete m_list;
    m_list = NULL;
}

// ----------------------------------------------------------------------------
// the tests themselves
// ----------------------------------------------------------------------------

#ifdef wxHAS_LISTCTRL_COLUMN_ORDER

void ListCtrlTestCase::ColumnsOrder()
{
    int n;
    wxListItem li;
    li.SetMask(wxLIST_MASK_TEXT);

    // first set up some columns
    static const int NUM_COLS = 3;

    m_list->InsertColumn(0, "Column 0");
    m_list->InsertColumn(1, "Column 1");
    m_list->InsertColumn(2, "Column 2");

    // and a couple of test items too
    m_list->InsertItem(0, "Item 0");
    m_list->SetItem(0, 1, "first in first");

    m_list->InsertItem(1, "Item 1");
    m_list->SetItem(1, 2, "second in second");


    // check that the order is natural in the beginning
    const wxArrayInt orderOrig = m_list->GetColumnsOrder();
    for ( n = 0; n < NUM_COLS; n++ )
        CPPUNIT_ASSERT_EQUAL( n, orderOrig[n] );

    // then rearrange them: using { 2, 0, 1 } order means that column 2 is
    // shown first, then column 0 and finally column 1
    wxArrayInt order(3);
    order[0] = 2;
    order[1] = 0;
    order[2] = 1;
    m_list->SetColumnsOrder(order);

    // check that we get back the same order as we set
    const wxArrayInt orderNew = m_list->GetColumnsOrder();
    for ( n = 0; n < NUM_COLS; n++ )
        CPPUNIT_ASSERT_EQUAL( order[n], orderNew[n] );

    // and the order -> index mappings for individual columns
    for ( n = 0; n < NUM_COLS; n++ )
        CPPUNIT_ASSERT_EQUAL( order[n], m_list->GetColumnIndexFromOrder(n) );

    // and also the reverse mapping
    CPPUNIT_ASSERT_EQUAL( 1, m_list->GetColumnOrder(0) );
    CPPUNIT_ASSERT_EQUAL( 2, m_list->GetColumnOrder(1) );
    CPPUNIT_ASSERT_EQUAL( 0, m_list->GetColumnOrder(2) );


    // finally check that accessors still use indices, not order
    CPPUNIT_ASSERT( m_list->GetColumn(0, li) );
    CPPUNIT_ASSERT_EQUAL( "Column 0", li.GetText() );

    li.SetId(0);
    li.SetColumn(1);
    CPPUNIT_ASSERT( m_list->GetItem(li) );
    CPPUNIT_ASSERT_EQUAL( "first in first", li.GetText() );

    li.SetId(1);
    li.SetColumn(2);
    CPPUNIT_ASSERT( m_list->GetItem(li) );
    CPPUNIT_ASSERT_EQUAL( "second in second", li.GetText() );

    //tidy up when we are finished
    m_list->ClearAll();
}

#endif // wxHAS_LISTCTRL_COLUMN_ORDER

void ListCtrlTestCase::ItemRect()
{
    // set up for the test
    m_list->InsertColumn(0, "Column 0", wxLIST_FORMAT_LEFT, 60);
    m_list->InsertColumn(1, "Column 1", wxLIST_FORMAT_LEFT, 50);
    m_list->InsertColumn(2, "Column 2", wxLIST_FORMAT_LEFT, 40);

    m_list->InsertItem(0, "Item 0");
    m_list->SetItem(0, 1, "first column");
    m_list->SetItem(0, 1, "second column");

    // do test
    wxRect r;
    WX_ASSERT_FAILS_WITH_ASSERT( m_list->GetItemRect(1, r) );
    CPPUNIT_ASSERT( m_list->GetItemRect(0, r) );
    CPPUNIT_ASSERT_EQUAL( 150, r.GetWidth() );

    CPPUNIT_ASSERT( m_list->GetSubItemRect(0, 0, r) );
    CPPUNIT_ASSERT_EQUAL( 60, r.GetWidth() );

    CPPUNIT_ASSERT( m_list->GetSubItemRect(0, 1, r) );
    CPPUNIT_ASSERT_EQUAL( 50, r.GetWidth() );

    CPPUNIT_ASSERT( m_list->GetSubItemRect(0, 2, r) );
    CPPUNIT_ASSERT_EQUAL( 40, r.GetWidth() );

    WX_ASSERT_FAILS_WITH_ASSERT( m_list->GetSubItemRect(0, 3, r) );

    //tidy up when we are finished
    m_list->ClearAll();
}

void ListCtrlTestCase::ChangeMode()
{
    m_list->InsertColumn(0, "Header");
    m_list->InsertItem(0, "First");
    m_list->InsertItem(1, "Second");
    CPPUNIT_ASSERT_EQUAL( 2, m_list->GetItemCount() );

    // check that switching the mode preserves the items
    m_list->SetWindowStyle(wxLC_ICON);
    CPPUNIT_ASSERT_EQUAL( 2, m_list->GetItemCount() );
    CPPUNIT_ASSERT_EQUAL( "First", m_list->GetItemText(0) );

    // and so does switching back
    m_list->SetWindowStyle(wxLC_REPORT);
    CPPUNIT_ASSERT_EQUAL( 2, m_list->GetItemCount() );
    CPPUNIT_ASSERT_EQUAL( "First", m_list->GetItemText(0) );

    //tidy up when we are finished
    m_list->ClearAll();
}

void ListCtrlTestCase::ItemClick()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_list, wxEVT_COMMAND_LIST_ITEM_SELECTED); 
    EventCounter count1(m_list, wxEVT_COMMAND_LIST_ITEM_FOCUSED);
    EventCounter count2(m_list, wxEVT_COMMAND_LIST_ITEM_ACTIVATED);
    EventCounter count3(m_list, wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK);
    EventCounter count4(m_list, wxEVT_COMMAND_LIST_ITEM_MIDDLE_CLICK);

    m_list->InsertColumn(0, "Column 0", wxLIST_FORMAT_LEFT, 60);
    m_list->InsertColumn(1, "Column 1", wxLIST_FORMAT_LEFT, 50);
    m_list->InsertColumn(2, "Column 2", wxLIST_FORMAT_LEFT, 40);

    m_list->InsertItem(0, "Item 0");
    m_list->SetItem(0, 1, "first column");
    m_list->SetItem(0, 2, "second column");

    wxUIActionSimulator sim;

    wxRect pos;
    m_list->GetItemRect(0, pos);

    //We move in slightly so we are not on the edge
    wxPoint point = m_list->ClientToScreen(pos.GetPosition()) + wxPoint(2, 2);

    sim.MouseMove(point);
    sim.MouseClick();
    sim.MouseDblClick();
    sim.MouseClick(wxMOUSE_BTN_RIGHT);
    sim.MouseClick(wxMOUSE_BTN_MIDDLE);
    wxYield();

    //when the first item was selected the focus chages to it, but not
    //on subsequent clicks
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_LIST_ITEM_FOCUSED));
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_LIST_ITEM_SELECTED));
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_LIST_ITEM_ACTIVATED));
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK));
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_LIST_ITEM_MIDDLE_CLICK));

    //tidy up when we are finished
    m_list->ClearAll();
}

void ListCtrlTestCase::KeyDown()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_list, wxEVT_COMMAND_LIST_KEY_DOWN);

    wxUIActionSimulator sim;

    m_list->SetFocus();
    sim.Text("aAbB");
    wxYield();

    CPPUNIT_ASSERT_EQUAL(4, frame->GetEventCount());
}

void ListCtrlTestCase::DeleteItems()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_list, wxEVT_COMMAND_LIST_DELETE_ITEM);
    EventCounter count1(m_list, wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS);


    m_list->InsertColumn(0, "Column 0", wxLIST_FORMAT_LEFT, 60);
    m_list->InsertColumn(1, "Column 1", wxLIST_FORMAT_LEFT, 50);
    m_list->InsertColumn(2, "Column 2", wxLIST_FORMAT_LEFT, 40);

    m_list->InsertItem(0, "Item 0");
    m_list->InsertItem(1, "Item 1");
    m_list->InsertItem(2, "Item 1");

    m_list->DeleteItem(0);
    m_list->DeleteItem(0);
    m_list->DeleteAllItems();

    //tidy up when we are finished, we put this before the asserts as it
    //actually sends a DELETE_ALL_ITEMS event
    m_list->ClearAll();

    CPPUNIT_ASSERT_EQUAL(2, frame->GetEventCount(wxEVT_COMMAND_LIST_DELETE_ITEM));
    CPPUNIT_ASSERT_EQUAL(2, frame->GetEventCount(wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS));
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

void ListCtrlTestCase::InsertItem()
{
   wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_list, wxEVT_COMMAND_LIST_INSERT_ITEM);

    m_list->InsertColumn(0, "Column 0", wxLIST_FORMAT_LEFT, 60);

    wxListItem item;
    item.SetId(0);
    item.SetText("some text");

    m_list->InsertItem(item);
    m_list->InsertItem(1, "more text");

    CPPUNIT_ASSERT_EQUAL(2, frame->GetEventCount(wxEVT_COMMAND_LIST_INSERT_ITEM));

    m_list->ClearAll();
}

