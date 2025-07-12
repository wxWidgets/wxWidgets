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

class ListViewTestCase : public ListBaseTestCase
{
public:
    ListViewTestCase();
    virtual ~ListViewTestCase() override;

    virtual wxListCtrl *GetList() const override { return m_list; }

protected:
    wxListView *m_list;

    wxDECLARE_NO_COPY_CLASS(ListViewTestCase);
};

ListViewTestCase::ListViewTestCase()
{
    m_list = new wxListView(wxTheApp->GetTopWindow());
    m_list->SetWindowStyle(wxLC_REPORT);
    m_list->SetSize(400, 200);
}

ListViewTestCase::~ListViewTestCase()
{
    DeleteTestWindow(m_list);
}

wxLIST_BASE_TESTS(ListView, "[listctrl][listview]")

TEST_CASE_METHOD(ListViewTestCase, "ListView::Selection", "[listctrl][listview]")
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

TEST_CASE_METHOD(ListViewTestCase, "ListView::Focus", "[listctrl][listview]")
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
