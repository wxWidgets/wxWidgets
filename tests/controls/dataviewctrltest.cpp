///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/dataviewctrltest.cpp
// Purpose:     wxDataViewCtrl unit test
// Author:      Vaclav Slavik
// Created:     2011-08-08
// Copyright:   (c) 2011 Vaclav Slavik <vslavik@gmail.com>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#if wxUSE_DATAVIEWCTRL

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/app.h"
#include "wx/dataview.h"

#include "testableframe.h"
#include "asserthelper.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class DataViewCtrlTestCase
{
public:
    explicit DataViewCtrlTestCase(long style);
    ~DataViewCtrlTestCase();

protected:
    void TestSelectionFor0and1();

    // the dataview control itself
    wxDataViewTreeCtrl *m_dvc;

    // and some of its items
    wxDataViewItem m_root,
                   m_child1,
                   m_child2,
                   m_grandchild;

    wxDECLARE_NO_COPY_CLASS(DataViewCtrlTestCase);
};

class SingleSelectDataViewCtrlTestCase : public DataViewCtrlTestCase
{
public:
    SingleSelectDataViewCtrlTestCase()
        : DataViewCtrlTestCase(wxDV_SINGLE)
    {
    }
};

class MultiSelectDataViewCtrlTestCase : public DataViewCtrlTestCase
{
public:
    MultiSelectDataViewCtrlTestCase()
        : DataViewCtrlTestCase(wxDV_MULTIPLE)
    {
    }
};

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

DataViewCtrlTestCase::DataViewCtrlTestCase(long style)
{
    m_dvc = new wxDataViewTreeCtrl(wxTheApp->GetTopWindow(),
                                   wxID_ANY,
                                   wxDefaultPosition,
                                   wxSize(400, 200),
                                   style);

    m_root = m_dvc->AppendContainer(wxDataViewItem(), "The root");
      m_child1 = m_dvc->AppendContainer(m_root, "child1");
        m_grandchild = m_dvc->AppendItem(m_child1, "grandchild");
      m_child2 = m_dvc->AppendItem(m_root, "child2");

    m_dvc->Layout();
    m_dvc->Expand(m_root);
    m_dvc->Refresh();
    m_dvc->Update();
}

DataViewCtrlTestCase::~DataViewCtrlTestCase()
{
    delete m_dvc;
}

// ----------------------------------------------------------------------------
// the tests themselves
// ----------------------------------------------------------------------------

TEST_CASE_METHOD(MultiSelectDataViewCtrlTestCase,
                 "wxDVC::DeleteSelected",
                 "[wxDataViewCtrl][delete]")
{
    wxDataViewItemArray sel;
    sel.push_back(m_child1);
    sel.push_back(m_grandchild);
    sel.push_back(m_child2);
    m_dvc->SetSelections(sel);

    // delete a selected item
    m_dvc->DeleteItem(m_child1);

    m_dvc->GetSelections(sel);

    // m_child1 and its children should be removed from the selection now
    REQUIRE( sel.size() == 1 );
    CHECK( sel[0] == m_child2 );
}

TEST_CASE_METHOD(MultiSelectDataViewCtrlTestCase,
                 "wxDVC::DeleteNotSelected",
                 "[wxDataViewCtrl][delete]")
{
    // TODO not working on OS X as expected
#ifdef __WXOSX__
    WARN("Disabled under MacOS because this test currently fails");
#else
    wxDataViewItemArray sel;
    sel.push_back(m_child1);
    sel.push_back(m_grandchild);
    m_dvc->SetSelections(sel);

    // delete unselected item
    m_dvc->DeleteItem(m_child2);

    m_dvc->GetSelections(sel);

    // m_child1 and its children should be unaffected
    REQUIRE( sel.size() == 2 );
    CHECK( sel[0] == m_child1 );
    CHECK( sel[1] == m_grandchild );
#endif
}

void DataViewCtrlTestCase::TestSelectionFor0and1()
{
    wxDataViewItemArray selections;

    // Initially there is no selection.
    CHECK( m_dvc->GetSelectedItemsCount() == 0 );
    CHECK( !m_dvc->HasSelection() );
    CHECK( !m_dvc->GetSelection().IsOk() );

    CHECK( !m_dvc->GetSelections(selections) );
    CHECK( selections.empty() );

    // Select one item.
    m_dvc->Select(m_child1);
    CHECK( m_dvc->GetSelectedItemsCount() == 1 );
    CHECK( m_dvc->HasSelection() );
    CHECK( m_dvc->GetSelection().IsOk() );
    REQUIRE( m_dvc->GetSelections(selections) == 1 );
    CHECK( selections[0] == m_child1 );
}

TEST_CASE_METHOD(MultiSelectDataViewCtrlTestCase,
                 "wxDVC::GetSelectionForMulti",
                 "[wxDataViewCtrl][select]")
{
    wxDataViewItemArray selections;

    TestSelectionFor0and1();

    m_dvc->Select(m_child2);

    CHECK( m_dvc->GetSelectedItemsCount() == 2 );
    CHECK( m_dvc->HasSelection() );
    CHECK( !m_dvc->GetSelection().IsOk() );
    REQUIRE( m_dvc->GetSelections(selections) == 2 );
    CHECK( selections[1] == m_child2 );
}

TEST_CASE_METHOD(SingleSelectDataViewCtrlTestCase,
                 "wxDVC::SingleSelection",
                 "[wxDataViewCtrl][selection]")
{
    TestSelectionFor0and1();
}

TEST_CASE_METHOD(SingleSelectDataViewCtrlTestCase,
                 "wxDVC::IsExpanded",
                 "[wxDataViewCtrl][expand]")
{
    CHECK( m_dvc->IsExpanded(m_root) );
    CHECK( !m_dvc->IsExpanded(m_child1) );
    // No idea why, but the native NSOutlineView isItemExpanded: method returns
    // true for this item for some reason.
#ifdef __WXOSX__
    WARN("Disabled under MacOS: IsExpanded() returns true for grand child");
#else
    CHECK( !m_dvc->IsExpanded(m_grandchild) );
#endif
    CHECK( !m_dvc->IsExpanded(m_child2) );
}

TEST_CASE_METHOD(SingleSelectDataViewCtrlTestCase,
                 "wxDVC::GetItemRect",
                 "[wxDataViewCtrl][item]")
{
#ifdef __WXGTK__
    // We need to let the native control have some events to lay itself out.
    wxYield();
#endif // __WXGTK__

    const wxRect rect1 = m_dvc->GetItemRect(m_child1);
    const wxRect rect2 = m_dvc->GetItemRect(m_child2);

    CHECK( rect1 != wxRect() );
    CHECK( rect2 != wxRect() );

    CHECK( rect1.x == rect2.x );
    CHECK( rect1.width == rect2.width );
    CHECK( rect1.height == rect2.height );

    {
        INFO("First child: " << rect1 << ", second one: " << rect2);
        CHECK( rect1.y < rect2.y );
    }

    const wxRect rectNotShown = m_dvc->GetItemRect(m_grandchild);
    CHECK( rectNotShown == wxRect() );

    // Append enough items to make the window scrollable.
    for ( int i = 3; i < 100; ++i )
        m_dvc->AppendItem(m_root, wxString::Format("child%d", i));

    const wxDataViewItem last = m_dvc->AppendItem(m_root, "last");

    // This should scroll the window to bring this item into view.
    m_dvc->EnsureVisible(last);

#ifdef __WXGTK__
    // And again to let it scroll the correct items into view.
    wxYield();
#endif

    // Check that this was indeed the case.
    const wxDataViewItem top = m_dvc->GetTopItem();
    CHECK( top != m_root );

    // Verify that the coordinates are returned in physical coordinates of the
    // window and not the logical coordinates affected by scrolling.
    const wxRect rectScrolled = m_dvc->GetItemRect(top);
    CHECK( rectScrolled.GetBottom() > 0 );
    CHECK( rectScrolled.GetTop() <= m_dvc->GetClientSize().y );

    // Also check that the root item is not currently visible (because it's
    // scrolled off).
    const wxRect rectRoot = m_dvc->GetItemRect(m_root);
    CHECK( rectRoot == wxRect() );
}

#endif //wxUSE_DATAVIEWCTRL
