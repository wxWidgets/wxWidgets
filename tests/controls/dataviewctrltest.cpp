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


#include "wx/app.h"
#include "wx/dataview.h"
#include "wx/uiaction.h"

#ifdef __WXGTK__
    #include "wx/stopwatch.h"
#endif // __WXGTK__

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

class MultiColumnsDataViewCtrlTestCase
{
public:
    MultiColumnsDataViewCtrlTestCase();
    ~MultiColumnsDataViewCtrlTestCase();

protected:
    // the dataview control itself
    wxDataViewListCtrl *m_dvc;

    // constants
    const wxSize m_size;
    const int m_firstColumnWidth;

    // and the columns
    wxDataViewColumn* m_firstColumn;
    wxDataViewColumn* m_lastColumn;

    wxDECLARE_NO_COPY_CLASS(MultiColumnsDataViewCtrlTestCase);
};

class DataViewCtrlTestModel: public wxDataViewModel
{
public:
    // Items of the model.
    //
    // wxTEST_ITEM_NULL
    // |
    // |-- wxTEST_ITEM_ROOT
    //     |
    //     |-- wxTEST_ITEM_CHILD
    //         |
    //         |-- wxTEST_ITEM_GRANDCHILD
    //         |   |
    //         |   |-- wxTEST_ITEM_LEAF
    //         |   |
    //         |   |-- wxTEST_ITEM_LEAF_HIDDEN
    //         |
    //         |-- wxTEST_ITEM_GRANDCHILD_HIDDEN
    //
    enum wxTestItem
    {
        wxTEST_ITEM_NULL,
        wxTEST_ITEM_ROOT,
        wxTEST_ITEM_CHILD,
        wxTEST_ITEM_GRANDCHILD,
        wxTEST_ITEM_LEAF,
        wxTEST_ITEM_LEAF_HIDDEN,
        wxTEST_ITEM_GRANDCHILD_HIDDEN
    };

    DataViewCtrlTestModel()
        : m_root(wxTEST_ITEM_ROOT),
          m_child(wxTEST_ITEM_CHILD),
          m_grandChild(wxTEST_ITEM_GRANDCHILD),
          m_leaf(wxTEST_ITEM_LEAF),
          m_leafHidden(wxTEST_ITEM_LEAF_HIDDEN),
          m_grandchildHidden(wxTEST_ITEM_GRANDCHILD_HIDDEN),
          m_allItemsVisible(false)
    {
    }

    wxDataViewItem GetDataViewItem(wxTestItem item) const
    {
        switch( item )
        {
            case wxTEST_ITEM_NULL:
                return wxDataViewItem();

            case wxTEST_ITEM_ROOT:
                return wxDataViewItem(const_cast<wxTestItem*>(&m_root));

            case wxTEST_ITEM_CHILD:
                return wxDataViewItem(const_cast<wxTestItem*>(&m_child));

            case wxTEST_ITEM_GRANDCHILD:
                return wxDataViewItem(const_cast<wxTestItem*>(&m_grandChild));

            case wxTEST_ITEM_LEAF:
                return wxDataViewItem(const_cast<wxTestItem*>(&m_leaf));

            case wxTEST_ITEM_LEAF_HIDDEN:
                return wxDataViewItem(const_cast<wxTestItem*>(&m_leafHidden));

            case wxTEST_ITEM_GRANDCHILD_HIDDEN:
                return wxDataViewItem(const_cast<wxTestItem*>(&m_grandchildHidden));
        }
        return wxDataViewItem();
    }

    // Overridden wxDataViewModel methods.

    void GetValue(wxVariant &variant, const wxDataViewItem &item,
                  unsigned int WXUNUSED(col)) const override
    {
        switch( GetItemID(item) )
        {
            case wxTEST_ITEM_NULL:
                break;

            case wxTEST_ITEM_ROOT:
                variant = "root";
                break;

            case wxTEST_ITEM_CHILD:
                variant = "child";
                break;

            case wxTEST_ITEM_GRANDCHILD:
                variant = "grand child";
                break;

            case wxTEST_ITEM_LEAF:
                variant = "leaf";
                break;

            case wxTEST_ITEM_LEAF_HIDDEN:
                variant = "initially hidden leaf";
                break;

            case wxTEST_ITEM_GRANDCHILD_HIDDEN:
                variant = "initially hidden";
                break;
        }
    }

    bool SetValue(const wxVariant &WXUNUSED(variant),
                  const wxDataViewItem &WXUNUSED(item),
                  unsigned int WXUNUSED(col)) override
    {
        return false;
    }

    bool HasContainerColumns(const wxDataViewItem &WXUNUSED(item)) const override
    {
        // Always display all the columns, even for the containers.
        return true;
    }

    wxDataViewItem GetParent(const wxDataViewItem &item) const override
    {
        switch( GetItemID(item) )
        {
            case wxTEST_ITEM_NULL:
                FAIL( "The item is the top most container" );
                return wxDataViewItem();

            case wxTEST_ITEM_ROOT:
                return wxDataViewItem();

            case wxTEST_ITEM_CHILD:
                return GetDataViewItem(m_root);

            case wxTEST_ITEM_GRANDCHILD:
            case wxTEST_ITEM_GRANDCHILD_HIDDEN:
                return GetDataViewItem(m_child);

            case wxTEST_ITEM_LEAF:
            case wxTEST_ITEM_LEAF_HIDDEN:
                return GetDataViewItem(m_grandChild);
        }
        return wxDataViewItem();
    }

    bool IsContainer(const wxDataViewItem &item) const override
    {
        switch( GetItemID(item) )
        {
            case wxTEST_ITEM_NULL:
            case wxTEST_ITEM_ROOT:
            case wxTEST_ITEM_CHILD:
            case wxTEST_ITEM_GRANDCHILD:
                return true;

            case wxTEST_ITEM_LEAF:
            case wxTEST_ITEM_LEAF_HIDDEN:
            case wxTEST_ITEM_GRANDCHILD_HIDDEN:
                return false;
        }
        return false;
    }

    unsigned int GetChildren(const wxDataViewItem &item,
                           wxDataViewItemArray &children) const override
    {
        switch( GetItemID(item) )
        {
            case wxTEST_ITEM_NULL:
                children.push_back(GetDataViewItem(m_root));
                return 1;

            case wxTEST_ITEM_ROOT:
                children.push_back(GetDataViewItem(m_child));
                return 1;

            case wxTEST_ITEM_CHILD:
                children.push_back(GetDataViewItem(m_grandChild));

                if ( m_allItemsVisible )
                {
                    children.push_back(GetDataViewItem(m_grandchildHidden));
                    return 2;
                }

                return 1;

            case wxTEST_ITEM_GRANDCHILD:
                children.push_back(GetDataViewItem(m_leaf));

                if ( m_allItemsVisible )
                {
                    children.push_back(GetDataViewItem(m_leafHidden));
                    return 2;
                }

                return 1;

            case wxTEST_ITEM_LEAF:
            case wxTEST_ITEM_LEAF_HIDDEN:
            case wxTEST_ITEM_GRANDCHILD_HIDDEN:
                FAIL( "The item is not a container" );
                return 0;
        }
        return 0;
    }

    enum wxItemsOrder
    {
        wxORDER_LEAF_THEN_GRANCHILD,
        wxORDER_GRANCHILD_THEN_LEAF
    };

    void ShowChildren(wxItemsOrder order)
    {
        m_allItemsVisible = true;
        switch ( order )
        {
            case wxORDER_LEAF_THEN_GRANCHILD:
                ItemAdded(GetDataViewItem(m_grandChild), GetDataViewItem(m_leafHidden));
                ItemAdded(GetDataViewItem(m_child), GetDataViewItem(m_grandchildHidden));
                break;

            case wxORDER_GRANCHILD_THEN_LEAF:
                ItemAdded(GetDataViewItem(m_child), GetDataViewItem(m_grandchildHidden));
                ItemAdded(GetDataViewItem(m_grandChild), GetDataViewItem(m_leafHidden));
                break;
        }
    }

    void HideChildren()
    {
        m_allItemsVisible = false;
        ItemDeleted(GetDataViewItem(m_grandChild), GetDataViewItem(m_leafHidden));
        ItemDeleted(GetDataViewItem(m_child), GetDataViewItem(m_grandchildHidden));
    }

private:
    wxTestItem GetItemID(const wxDataViewItem &dataViewItem) const
    {
        if ( dataViewItem.GetID() == nullptr )
            return wxTEST_ITEM_NULL;
        return *static_cast<wxTestItem*>(dataViewItem.GetID());
    }

    wxTestItem m_root;
    wxTestItem m_child;
    wxTestItem m_grandChild;
    wxTestItem m_leaf;
    wxTestItem m_leafHidden;
    wxTestItem m_grandchildHidden;

    // Whether wxTEST_ITEM_GRANDCHILD_HIDDEN item should be visible or not.
    bool m_allItemsVisible;
};


class DataViewCtrlWithCustomModelTestCase
{
public:
    DataViewCtrlWithCustomModelTestCase();
    ~DataViewCtrlWithCustomModelTestCase();

protected:
    enum wxItemExistence
    {
        wxITEM_APPEAR,
        wxITEM_DISAPPEAR
    };

    void UpdateAndWaitForItem(const wxDataViewItem& item, wxItemExistence existence)
    {
        m_dvc->Refresh();
        m_dvc->Update();

#ifdef __WXGTK__
        // Unfortunately it's not enough to call wxYield() once, so wait up to
        // 0.5 sec.
        wxStopWatch sw;
        while ( true )
        {
            wxYield();

            const bool isItemRectEmpty = m_dvc->GetItemRect(item).IsEmpty();
            switch ( existence )
            {
                case wxITEM_APPEAR:
                    if ( !isItemRectEmpty )
                        return;
                    break;

                case wxITEM_DISAPPEAR:
                    if ( isItemRectEmpty )
                        return;
                    break;
            }

            if ( sw.Time() > 500 )
            {
                WARN("Timed out waiting for wxDataViewCtrl");
                break;
            }
        }
#else // !__WXGTK__
        wxUnusedVar(item);
        wxUnusedVar(existence);
#endif // __WXGTK__
    }

    // The dataview control.
    wxDataViewCtrl *m_dvc;

    // The dataview model.
    DataViewCtrlTestModel *m_model;

    // Its items.
    wxDataViewItem m_root,
                   m_child,
                   m_grandchild,
                   m_leaf,
                   m_leafHidden,
                   m_grandchildHidden;

    wxDECLARE_NO_COPY_CLASS(DataViewCtrlWithCustomModelTestCase);
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

MultiColumnsDataViewCtrlTestCase::MultiColumnsDataViewCtrlTestCase()
    : m_size(200, 100),
      m_firstColumnWidth(50)
{
    m_dvc = new wxDataViewListCtrl(wxTheApp->GetTopWindow(), wxID_ANY);

    m_firstColumn =
        m_dvc->AppendTextColumn(wxString(), wxDATAVIEW_CELL_INERT, m_firstColumnWidth);
    m_lastColumn =
        m_dvc->AppendTextColumn(wxString(), wxDATAVIEW_CELL_INERT);

    // Set size after columns appending to extend size of the last column.
    m_dvc->SetSize(m_size);
    m_dvc->Layout();
    m_dvc->Refresh();
    m_dvc->Update();
}

MultiColumnsDataViewCtrlTestCase::~MultiColumnsDataViewCtrlTestCase()
{
    delete m_dvc;
}

DataViewCtrlWithCustomModelTestCase::DataViewCtrlWithCustomModelTestCase()
{
    m_dvc = new wxDataViewCtrl(wxTheApp->GetTopWindow(),
                               wxID_ANY,
                               wxDefaultPosition,
                               wxSize(400, 200),
                               wxDV_SINGLE);

    m_model = new DataViewCtrlTestModel();
    m_dvc->AssociateModel(m_model);
    m_model->DecRef();

    m_dvc->AppendColumn(
        new wxDataViewColumn(
            "Value",
            new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_INERT),
            0,
            m_dvc->FromDIP(200),
            wxALIGN_LEFT,
            wxDATAVIEW_COL_RESIZABLE));

    m_root = m_model->GetDataViewItem(DataViewCtrlTestModel::wxTEST_ITEM_ROOT);
    m_child = m_model->GetDataViewItem(DataViewCtrlTestModel::wxTEST_ITEM_CHILD);
    m_grandchild =
        m_model->GetDataViewItem(DataViewCtrlTestModel::wxTEST_ITEM_GRANDCHILD);
    m_leaf =
        m_model->GetDataViewItem(DataViewCtrlTestModel::wxTEST_ITEM_LEAF);
    m_leafHidden =
        m_model->GetDataViewItem(DataViewCtrlTestModel::wxTEST_ITEM_LEAF_HIDDEN);
    m_grandchildHidden =
        m_model->GetDataViewItem(DataViewCtrlTestModel::wxTEST_ITEM_GRANDCHILD_HIDDEN);

    m_dvc->Layout();
    m_dvc->Expand(m_root);
    m_dvc->Refresh();
    m_dvc->Update();
}

DataViewCtrlWithCustomModelTestCase::~DataViewCtrlWithCustomModelTestCase()
{
    delete m_dvc;
}

// ----------------------------------------------------------------------------
// the tests themselves
// ----------------------------------------------------------------------------

TEST_CASE_METHOD(MultiSelectDataViewCtrlTestCase,
                 "wxDVC::Selection",
                 "[wxDataViewCtrl][select]")
{
    // Check selection round-trip.
    wxDataViewItemArray sel;
    sel.push_back(m_child1);
    sel.push_back(m_grandchild);
    REQUIRE_NOTHROW( m_dvc->SetSelections(sel) );

    wxDataViewItemArray sel2;
    CHECK( m_dvc->GetSelections(sel2) == static_cast<int>(sel.size()) );

    CHECK( sel2 == sel );

    // Invalid items in GetSelections() input are supposed to be just skipped.
    sel.clear();
    sel.push_back(wxDataViewItem());
    REQUIRE_NOTHROW( m_dvc->SetSelections(sel) );

    CHECK( m_dvc->GetSelections(sel2) == 0 );
    CHECK( sel2.empty() );
}

TEST_CASE_METHOD(MultiSelectDataViewCtrlTestCase,
                 "wxDVC::DeleteSelected",
                 "[wxDataViewCtrl][delete]")
{
#ifdef __WXGTK__
    wxString useASAN;
    if ( wxGetEnv("wxUSE_ASAN", &useASAN) && useASAN == "1" )
    {
        WARN("Skipping test resulting in a memory leak report with wxGTK");
        return;
    }
#endif // __WXGTK__

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

    m_dvc->Collapse(m_root);
    CHECK( !m_dvc->IsExpanded(m_root) );

    m_dvc->ExpandChildren(m_root);
    CHECK( m_dvc->IsExpanded(m_root) );
    CHECK( m_dvc->IsExpanded(m_child1) );

    // Expanding an already expanded node must still expand all its children.
    m_dvc->Collapse(m_child1);
    CHECK( !m_dvc->IsExpanded(m_child1) );
    m_dvc->ExpandChildren(m_root);
    CHECK( m_dvc->IsExpanded(m_child1) );
}

TEST_CASE_METHOD(DataViewCtrlWithCustomModelTestCase,
                 "wxDVC::Expand",
                 "[wxDataViewCtrl][expand]")
{
    CHECK( m_dvc->IsExpanded(m_root) );
    CHECK( !m_dvc->IsExpanded(m_child) );

#ifdef __WXGTK__
    // We need to let the native control have some events to lay itself out.
    wxYield();
#endif // __WXGTK__

    // Unfortunately we can't combine test options with SECTION() so use
    // the additional enum variable.
    enum
    {
        wxOPTIONS_EXPAND_ADD_LEAF_THEN_GRANCHILD,
        wxOPTIONS_DONT_EXPAND_ADD_LEAF_THEN_GRANCHILD,
        wxOPTIONS_EXPAND_ADD_GRANCHILD_THEN_LEAF,
        wxOPTIONS_DONT_EXPAND_ADD_GRANCHILD_THEN_LEAF
    } options wxDUMMY_INITIALIZE(wxOPTIONS_EXPAND_ADD_LEAF_THEN_GRANCHILD);

    SECTION( "Was Expanded, Add The Leaf Then The Grandchild" )
    {
        options = wxOPTIONS_EXPAND_ADD_LEAF_THEN_GRANCHILD;
    }

    SECTION( "Was Not Expanded, Add The Leaf Then The Grandchild" )
    {
        options = wxOPTIONS_DONT_EXPAND_ADD_LEAF_THEN_GRANCHILD;
    }

    SECTION( "Was Expanded, Add The Grandchild Then The Leaf" )
    {
        options = wxOPTIONS_EXPAND_ADD_GRANCHILD_THEN_LEAF;
    }

    SECTION( "Was Not Expanded, Add The Grandchild Then The Leaf" )
    {
        options = wxOPTIONS_DONT_EXPAND_ADD_GRANCHILD_THEN_LEAF;
    }

    switch ( options )
    {
        case wxOPTIONS_EXPAND_ADD_LEAF_THEN_GRANCHILD:
        case wxOPTIONS_EXPAND_ADD_GRANCHILD_THEN_LEAF:
            CHECK( m_dvc->GetItemRect(m_grandchild).IsEmpty() );
            CHECK( m_dvc->GetItemRect(m_leafHidden).IsEmpty() );
            CHECK( m_dvc->GetItemRect(m_grandchildHidden).IsEmpty() );

            m_dvc->Expand(m_child);
            m_dvc->Expand(m_grandchild);
            UpdateAndWaitForItem(m_grandchild, wxITEM_APPEAR);

            CHECK( !m_dvc->GetItemRect(m_grandchild).IsEmpty() );
            CHECK( !m_dvc->GetItemRect(m_leaf).IsEmpty() );
            CHECK( m_dvc->GetItemRect(m_leafHidden).IsEmpty() );
            CHECK( m_dvc->GetItemRect(m_grandchildHidden).IsEmpty() );

            m_dvc->Collapse(m_grandchild);
            m_dvc->Collapse(m_child);
            break;

        case wxOPTIONS_DONT_EXPAND_ADD_LEAF_THEN_GRANCHILD:
        case wxOPTIONS_DONT_EXPAND_ADD_GRANCHILD_THEN_LEAF:
            // Do nothing.
            break;
    }

    // Check wxDataViewModel::ItemAdded().
    switch ( options )
    {
        case wxOPTIONS_EXPAND_ADD_LEAF_THEN_GRANCHILD:
        case wxOPTIONS_DONT_EXPAND_ADD_LEAF_THEN_GRANCHILD:
            m_model->ShowChildren(DataViewCtrlTestModel::wxORDER_LEAF_THEN_GRANCHILD);
            break;

        case wxOPTIONS_EXPAND_ADD_GRANCHILD_THEN_LEAF:
        case wxOPTIONS_DONT_EXPAND_ADD_GRANCHILD_THEN_LEAF:
            m_model->ShowChildren(DataViewCtrlTestModel::wxORDER_GRANCHILD_THEN_LEAF);
            break;
    }

    m_dvc->Expand(m_child);
    m_dvc->Expand(m_grandchild);
    UpdateAndWaitForItem(m_leaf, wxITEM_APPEAR);

    CHECK( m_dvc->IsExpanded(m_child) );
    CHECK( m_dvc->IsExpanded(m_grandchild) );
    CHECK( !m_dvc->GetItemRect(m_grandchild).IsEmpty() );
    CHECK( !m_dvc->GetItemRect(m_leaf).IsEmpty() );
    CHECK( !m_dvc->GetItemRect(m_leafHidden).IsEmpty() );
    CHECK( !m_dvc->GetItemRect(m_grandchildHidden).IsEmpty() );

    m_model->HideChildren();
    UpdateAndWaitForItem(m_leafHidden, wxITEM_DISAPPEAR);

    CHECK( m_dvc->GetItemRect(m_leafHidden).IsEmpty() );
    // Check that the problem with nodes duplication in ItemAdded() fixed.
    CHECK( m_dvc->GetItemRect(m_grandchildHidden).IsEmpty() );
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

    // This forces generic implementation to add m_grandchild to the tree, as
    // it does it only on demand. We want the item to really be there to check
    // that GetItemRect() returns an empty rectangle for collapsed items.
    m_dvc->Expand(m_child1);
    m_dvc->Collapse(m_child1);

    const wxRect rectNotShown = m_dvc->GetItemRect(m_grandchild);
    CHECK( rectNotShown == wxRect() );

    // Append enough items to make the window scrollable.
    for ( int i = 3; i < 100; ++i )
        m_dvc->AppendItem(m_root, wxString::Format("child%d", i));

    const wxDataViewItem last = m_dvc->AppendItem(m_root, "last");

    // This should scroll the window to bring this item into view.
    m_dvc->EnsureVisible(last);

#ifdef __WXGTK__
    // Wait for the list control to be relaid out.
    wxStopWatch sw;
    while ( m_dvc->GetTopItem() == m_root )
    {
        if ( sw.Time() > 500 )
        {
            WARN("Timed out waiting for wxDataViewCtrl layout");
            break;
        }
        wxYield();
    }
#endif // __WXGTK__

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

TEST_CASE_METHOD(SingleSelectDataViewCtrlTestCase,
                 "wxDVC::DeleteAllItems",
                 "[wxDataViewCtrl][delete]")
{
    // The invalid item corresponds to the root of tree store model, so it
    // should have a single item (our m_root) initially.
    CHECK( m_dvc->GetChildCount(wxDataViewItem()) == 1 );

    m_dvc->DeleteAllItems();

    // And none at all after deleting all the items.
    CHECK( m_dvc->GetChildCount(wxDataViewItem()) == 0 );
}

TEST_CASE_METHOD(MultiColumnsDataViewCtrlTestCase,
                 "wxDVC::AppendTextColumn",
                 "[wxDataViewCtrl][column]")
{
#ifdef __WXGTK__
    // Wait for the list control to be realized.
    wxStopWatch sw;
    while ( m_firstColumn->GetWidth() == 0 )
    {
        if ( sw.Time() > 500 )
        {
            WARN("Timed out waiting for wxDataViewListCtrl to be realized");
            break;
        }
        wxYield();
    }
#endif

    // Check the width of the first column.
    CHECK( m_firstColumn->GetWidth() == m_firstColumnWidth );

    // Check that the last column was extended to fit client area.
    const int lastColumnMaxWidth =
        m_dvc->GetClientSize().GetWidth() - m_firstColumnWidth;
    // In GTK and under Mac the width of the last column is less then
    // a remaining client area.
    const int lastColumnMinWidth = lastColumnMaxWidth - 10;
    CHECK( m_lastColumn->GetWidth() <= lastColumnMaxWidth );
    CHECK( m_lastColumn->GetWidth() >= lastColumnMinWidth );
}

#if wxUSE_UIACTIONSIMULATOR

TEST_CASE_METHOD(SingleSelectDataViewCtrlTestCase,
                 "wxDVC::KeyEvents",
                 "[wxDataViewCtrl][event]")
{
    if ( !EnableUITests() )
        return;

    EventCounter keyEvents(m_dvc, wxEVT_KEY_DOWN);

    m_dvc->SetFocus();

    wxUIActionSimulator sim;
    sim.Char(WXK_DOWN);
    wxYield();

    CHECK( keyEvents.GetCount() == 1 );
}

#endif // wxUSE_UIACTIONSIMULATOR

#endif //wxUSE_DATAVIEWCTRL
