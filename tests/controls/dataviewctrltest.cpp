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

    m_dvc->SetSize(400, 200);
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
    CHECK( !m_dvc->IsExpanded(m_grandchild) );
    CHECK( !m_dvc->IsExpanded(m_child2) );
}

#endif //wxUSE_DATAVIEWCTRL
