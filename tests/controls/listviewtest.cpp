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

    CHECK(m_list->IsSelected(0));
    CHECK(!m_list->IsSelected(1));

    long sel = m_list->GetFirstSelected();

    CHECK( sel == 0 );

    sel = m_list->GetNextSelected(sel);

    CHECK( sel == 2 );

    sel = m_list->GetNextSelected(sel);

    CHECK( sel == 3 );

    sel = m_list->GetNextSelected(sel);

    CHECK( sel == -1 );

    m_list->Select(0, false);

    CHECK(!m_list->IsSelected(0));
    CHECK( m_list->GetFirstSelected() == 2 );
}

TEST_CASE_METHOD(ListViewTestCase, "ListView::Focus", "[listctrl][listview]")
{
    EventCounter focused(m_list, wxEVT_LIST_ITEM_FOCUSED);

    m_list->InsertColumn(0, "Column 0");

    m_list->InsertItem(0, "Item 0");
    m_list->InsertItem(1, "Item 1");
    m_list->InsertItem(2, "Item 2");
    m_list->InsertItem(3, "Item 3");

    CHECK( focused.GetCount() == 0 );
    CHECK( m_list->GetFocusedItem() == -1 );

    m_list->Focus(0);

    CHECK( focused.GetCount() == 1 );
    CHECK( m_list->GetFocusedItem() == 0 );
}

TEST_CASE_METHOD(ListViewTestCase, "ListView::AppendColumn", "[listctrl][listview]")
{
    m_list->AppendColumn("Column 0");
    m_list->AppendColumn("Column 1");

    m_list->InsertItem(0, "First item");
    m_list->SetItem(0, 1, "First subitem");

    // Appending a column shouldn't change the existing items.
    m_list->AppendColumn("Column 2");

    CHECK( m_list->GetItemText(0) == "First item" );
    CHECK( m_list->GetItemText(0, 1) == "First subitem" );
}

#endif
