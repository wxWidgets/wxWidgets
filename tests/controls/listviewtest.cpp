///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/listviewtest.cpp
// Purpose:     wxListView unit test
// Author:      Steven Lamerton
// Created:     2010-07-10
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_LISTCTRL


#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/listctrl.h"
#include "listbasetest.h"
#include "testableframe.h"

class ListViewTestCase : public ListBaseTestCase, public CppUnit::TestCase
{
public:
    ListViewTestCase() { }

    virtual void setUp() override;
    virtual void tearDown() override;

    virtual wxListCtrl *GetList() const override { return m_list; }

private:
    CPPUNIT_TEST_SUITE( ListViewTestCase );
        wxLIST_BASE_TESTS();
        CPPUNIT_TEST( Selection );
        CPPUNIT_TEST( Focus );
    CPPUNIT_TEST_SUITE_END();

    void Selection();
    void Focus();

    wxListView *m_list;

    wxDECLARE_NO_COPY_CLASS(ListViewTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ListViewTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ListViewTestCase, "ListViewTestCase" );

void ListViewTestCase::setUp()
{
    m_list = new wxListView(wxTheApp->GetTopWindow());
    m_list->SetWindowStyle(wxLC_REPORT);
    m_list->SetSize(400, 200);
}

void ListViewTestCase::tearDown()
{
    DeleteTestWindow(m_list);
    m_list = nullptr;
}

void ListViewTestCase::Selection()
{
    m_list->InsertColumn(0, "Column 0");

    m_list->InsertItem(0, "Item 0");
    m_list->InsertItem(1, "Item 1");
    m_list->InsertItem(2, "Item 2");
    m_list->InsertItem(3, "Item 3");

    m_list->Select(0);
    m_list->Select(2);
    m_list->Select(3);

    CPPUNIT_ASSERT(m_list->IsSelected(0));
    CPPUNIT_ASSERT(!m_list->IsSelected(1));

    long sel = m_list->GetFirstSelected();

    CPPUNIT_ASSERT_EQUAL(0, sel);

    sel = m_list->GetNextSelected(sel);

    CPPUNIT_ASSERT_EQUAL(2, sel);

    sel = m_list->GetNextSelected(sel);

    CPPUNIT_ASSERT_EQUAL(3, sel);

    sel = m_list->GetNextSelected(sel);

    CPPUNIT_ASSERT_EQUAL(-1, sel);

    m_list->Select(0, false);

    CPPUNIT_ASSERT(!m_list->IsSelected(0));
    CPPUNIT_ASSERT_EQUAL(2, m_list->GetFirstSelected());
}

void ListViewTestCase::Focus()
{
    EventCounter focused(m_list, wxEVT_LIST_ITEM_FOCUSED);

    m_list->InsertColumn(0, "Column 0");

    m_list->InsertItem(0, "Item 0");
    m_list->InsertItem(1, "Item 1");
    m_list->InsertItem(2, "Item 2");
    m_list->InsertItem(3, "Item 3");

    CPPUNIT_ASSERT_EQUAL(0, focused.GetCount());
    CPPUNIT_ASSERT_EQUAL(-1, m_list->GetFocusedItem());

    m_list->Focus(0);

    CPPUNIT_ASSERT_EQUAL(1, focused.GetCount());
    CPPUNIT_ASSERT_EQUAL(0, m_list->GetFocusedItem());
}

#endif
