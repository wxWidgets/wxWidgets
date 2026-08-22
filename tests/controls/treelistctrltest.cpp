///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/treelistctrltest.cpp
// Purpose:     wxTreeListCtrl unit test.
// Author:      Vadim Zeitlin
// Created:     2011-08-27
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#if wxUSE_TREELISTCTRL


#include "wx/treelist.h"

#include "wx/app.h"
#include "wx/dataview.h"
#ifdef wxHAS_GENERIC_DATAVIEWCTRL
    #include "wx/headerctrl.h"
    #include "wx/weakref.h"
#endif

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class TreeListCtrlTestCase : public CppUnit::TestCase
{
public:
    TreeListCtrlTestCase() { }

    virtual void setUp() override;
    virtual void tearDown() override;

private:
    CPPUNIT_TEST_SUITE( TreeListCtrlTestCase );
        CPPUNIT_TEST( Traversal );
        CPPUNIT_TEST( ItemText );
        CPPUNIT_TEST( ItemCheck );
        CPPUNIT_TEST( ColumnMutation );
        CPPUNIT_TEST( ClearColumnsResetsValues );
        CPPUNIT_TEST( DestructiveColumnCancellation );
    CPPUNIT_TEST_SUITE_END();

    // Create the control with the given style.
    void Create(long style);

    // Add an item to the tree and increment m_numItems.
    wxTreeListItem AddItem(const char *label,
                           wxTreeListItem parent = wxTreeListItem(),
                           const char *numFiles = "",
                           const char *size = "");


    // Tests:
    void Traversal();
    void ItemText();
    void ItemCheck();
    void ColumnMutation();
    void ClearColumnsResetsValues();
    void DestructiveColumnCancellation();


    // The control itself.
    wxTreeListCtrl *m_treelist;

    // And some of its items.
    wxTreeListItem m_code,
                   m_code_osx,
                   m_code_osx_cocoa;

    // Also the total number of items in it initially
    unsigned m_numItems;

    wxDECLARE_NO_COPY_CLASS(TreeListCtrlTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( TreeListCtrlTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( TreeListCtrlTestCase, "TreeListCtrlTestCase" );

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

wxTreeListItem
TreeListCtrlTestCase::AddItem(const char *label,
                              wxTreeListItem parent,
                              const char *numFiles,
                              const char *size)
{
    if ( !parent.IsOk() )
        parent = m_treelist->GetRootItem();

    wxTreeListItem item = m_treelist->AppendItem(parent, label);
    m_treelist->SetItemText(item, 1, numFiles);
    m_treelist->SetItemText(item, 2, size);

    m_numItems++;

    return item;
}

void TreeListCtrlTestCase::Create(long style)
{
    m_treelist = new wxTreeListCtrl(wxTheApp->GetTopWindow(),
                                    wxID_ANY,
                                    wxDefaultPosition,
                                    wxSize(400, 200),
                                    style);

    m_treelist->AppendColumn("Component");
    m_treelist->AppendColumn("# Files");
    m_treelist->AppendColumn("Size");

    // Fill the control with the same data as used in the treelist sample:
    m_code = AddItem("Code");
        AddItem("wxMSW", m_code, "313", "3.94 MiB");
        AddItem("wxGTK", m_code, "180", "1.66 MiB");

        m_code_osx = AddItem("wxOSX", m_code, "265", "2.36 MiB");
            AddItem("Core", m_code_osx, "31", "347 KiB");
            AddItem("Carbon", m_code_osx, "91", "1.34 MiB");
            m_code_osx_cocoa = AddItem("Cocoa", m_code_osx, "46", "512 KiB");

    wxTreeListItem Documentation = AddItem("Documentation");
        AddItem("HTML", Documentation, "many");
        AddItem("CHM", Documentation, "1");

    wxTreeListItem Samples = AddItem("Samples");
        AddItem("minimal", Samples, "1", "7 KiB");
        AddItem("widgets", Samples, "28", "419 KiB");

    m_treelist->Refresh();
    m_treelist->Update();
}

void TreeListCtrlTestCase::setUp()
{
    m_numItems = 0;
    Create(wxTL_MULTIPLE | wxTL_3STATE);
}

void TreeListCtrlTestCase::tearDown()
{
    delete m_treelist;
    m_treelist = nullptr;
}

// ----------------------------------------------------------------------------
// the tests themselves
// ----------------------------------------------------------------------------

// Test various tree traversal methods.
void TreeListCtrlTestCase::Traversal()
{
    // GetParent() tests:
    wxTreeListItem root = m_treelist->GetRootItem();
    CPPUNIT_ASSERT( !m_treelist->GetItemParent(root) );

    CPPUNIT_ASSERT_EQUAL( root, m_treelist->GetItemParent(m_code) );
    CPPUNIT_ASSERT_EQUAL( m_code, m_treelist->GetItemParent(m_code_osx) );


    // GetFirstChild() and GetNextSibling() tests:
    CPPUNIT_ASSERT_EQUAL( m_code, m_treelist->GetFirstChild(root) );
    CPPUNIT_ASSERT_EQUAL
    (
        m_code_osx,
        m_treelist->GetNextSibling
        (
            m_treelist->GetNextSibling
            (
                m_treelist->GetFirstChild(m_code)
            )
        )
    );

    // Get{First,Next}Item() test:
    unsigned numItems = 0;
    for ( wxTreeListItem item = m_treelist->GetFirstItem();
          item.IsOk();
          item = m_treelist->GetNextItem(item) )
    {
        numItems++;
    }

    CPPUNIT_ASSERT_EQUAL( m_numItems, numItems );
}

// Test accessing items text.
void TreeListCtrlTestCase::ItemText()
{
    CPPUNIT_ASSERT_EQUAL( "Cocoa", m_treelist->GetItemText(m_code_osx_cocoa) );
    CPPUNIT_ASSERT_EQUAL( "46", m_treelist->GetItemText(m_code_osx_cocoa, 1) );

    m_treelist->SetItemText(m_code_osx_cocoa, "wxCocoa");
    CPPUNIT_ASSERT_EQUAL( "wxCocoa", m_treelist->GetItemText(m_code_osx_cocoa) );

    m_treelist->SetItemText(m_code_osx_cocoa, 1, "47");
    CPPUNIT_ASSERT_EQUAL( "47", m_treelist->GetItemText(m_code_osx_cocoa, 1) );
}

// Test checking and unchecking items.
void TreeListCtrlTestCase::ItemCheck()
{
    CPPUNIT_ASSERT_EQUAL( wxCHK_UNCHECKED,
                          m_treelist->GetCheckedState(m_code) );

    m_treelist->CheckItemRecursively(m_code);
    CPPUNIT_ASSERT_EQUAL( wxCHK_CHECKED,
                          m_treelist->GetCheckedState(m_code) );
    CPPUNIT_ASSERT_EQUAL( wxCHK_CHECKED,
                          m_treelist->GetCheckedState(m_code_osx) );
    CPPUNIT_ASSERT_EQUAL( wxCHK_CHECKED,
                          m_treelist->GetCheckedState(m_code_osx_cocoa) );

    m_treelist->UncheckItem(m_code_osx_cocoa);
    CPPUNIT_ASSERT_EQUAL( wxCHK_UNCHECKED,
                          m_treelist->GetCheckedState(m_code_osx_cocoa) );

    m_treelist->UpdateItemParentStateRecursively(m_code_osx_cocoa);
    CPPUNIT_ASSERT_EQUAL( wxCHK_UNDETERMINED,
                          m_treelist->GetCheckedState(m_code_osx) );
    CPPUNIT_ASSERT_EQUAL( wxCHK_UNDETERMINED,
                          m_treelist->GetCheckedState(m_code) );

    m_treelist->CheckItemRecursively(m_code_osx, wxCHK_UNCHECKED);
    m_treelist->UpdateItemParentStateRecursively(m_code_osx_cocoa);
    CPPUNIT_ASSERT_EQUAL( wxCHK_UNCHECKED,
                          m_treelist->GetCheckedState(m_code_osx) );
    CPPUNIT_ASSERT_EQUAL( wxCHK_UNDETERMINED,
                          m_treelist->GetCheckedState(m_code) );
}

void TreeListCtrlTestCase::ColumnMutation()
{
#ifdef wxHAS_GENERIC_DATAVIEWCTRL
    wxDataViewCtrl* const view = m_treelist->GetDataView();
    wxHeaderCtrl* const header = view->GenericGetHeader();
    CPPUNIT_ASSERT( header );

    wxArrayInt order;
    order.push_back(2);
    order.push_back(0);
    order.push_back(1);
    header->SetColumnsOrder(order);

    // Removing a logical middle column after a visual reorder must preserve
    // both the remaining values and their exact display order.
    CPPUNIT_ASSERT( m_treelist->DeleteColumn(1) );
    CPPUNIT_ASSERT_EQUAL( 2u, m_treelist->GetColumnCount() );
    CPPUNIT_ASSERT_EQUAL( wxString("wxOSX"),
                          m_treelist->GetItemText(m_code_osx, 0) );
    CPPUNIT_ASSERT_EQUAL( wxString("2.36 MiB"),
                          m_treelist->GetItemText(m_code_osx, 1) );
    CPPUNIT_ASSERT_EQUAL( 0u, view->GetColumn(0)->GetModelColumn() );
    CPPUNIT_ASSERT_EQUAL( 1u, view->GetColumn(1)->GetModelColumn() );

    const wxArrayInt orderAfterMiddleDelete = header->GetColumnsOrder();
    CPPUNIT_ASSERT_EQUAL( 2, static_cast<int>(orderAfterMiddleDelete.size()) );
    CPPUNIT_ASSERT_EQUAL( 1, orderAfterMiddleDelete[0] );
    CPPUNIT_ASSERT_EQUAL( 0, orderAfterMiddleDelete[1] );

    // The old second column is sorted and then promoted to primary. Its
    // renderer and sort state must be promoted with it.
    view->GetColumn(1)->SetSortOrder(false);
    m_treelist->CheckItem(m_code_osx, wxCHK_CHECKED);
    CPPUNIT_ASSERT( m_treelist->DeleteColumn(0) );
    CPPUNIT_ASSERT_EQUAL( 1u, m_treelist->GetColumnCount() );
    CPPUNIT_ASSERT_EQUAL( wxString("2.36 MiB"),
                          m_treelist->GetItemText(m_code_osx, 0) );
    CPPUNIT_ASSERT_EQUAL( wxCHK_CHECKED,
                          m_treelist->GetCheckedState(m_code_osx) );

    wxDataViewColumn* const primary = view->GetColumn(0);
    CPPUNIT_ASSERT_EQUAL( 0u, primary->GetModelColumn() );
    CPPUNIT_ASSERT( primary->IsSortKey() );
    CPPUNIT_ASSERT( !primary->IsSortOrderAscending() );
    CPPUNIT_ASSERT_EQUAL( primary, view->GetSortingColumn() );
    CPPUNIT_ASSERT(
        wxDynamicCast(
            primary->GetRenderer(),
            wxDataViewCheckIconTextRenderer) != nullptr );

    // Appending after all these mutations must keep the promoted value in
    // column zero and create a valid new positional model column.
    CPPUNIT_ASSERT_EQUAL(
        1,
        m_treelist->AppendColumn("Replacement") );
    m_treelist->SetItemText(m_code_osx, 1, "replacement value");
    CPPUNIT_ASSERT_EQUAL( wxString("2.36 MiB"),
                          m_treelist->GetItemText(m_code_osx, 0) );
    CPPUNIT_ASSERT_EQUAL( wxString("replacement value"),
                          m_treelist->GetItemText(m_code_osx, 1) );

    CPPUNIT_ASSERT( m_treelist->DeleteColumn(1) );
    CPPUNIT_ASSERT( m_treelist->DeleteColumn(0) );
    CPPUNIT_ASSERT_EQUAL( 0u, m_treelist->GetColumnCount() );
#endif // wxHAS_GENERIC_DATAVIEWCTRL
}

void TreeListCtrlTestCase::ClearColumnsResetsValues()
{
    CPPUNIT_ASSERT_EQUAL(
        wxString("wxOSX"),
        m_treelist->GetItemText(m_code_osx, 0) );
    CPPUNIT_ASSERT_EQUAL(
        wxString("2.36 MiB"),
        m_treelist->GetItemText(m_code_osx, 2) );

    m_treelist->ClearColumns();
    CPPUNIT_ASSERT_EQUAL( 0u, m_treelist->GetColumnCount() );

    CPPUNIT_ASSERT_EQUAL( 0, m_treelist->AppendColumn("Fresh") );
    CPPUNIT_ASSERT_EQUAL( wxString(),
                          m_treelist->GetItemText(m_code_osx, 0) );
    CPPUNIT_ASSERT_EQUAL( wxString(),
                          m_treelist->GetItemText(m_code_osx_cocoa, 0) );

    m_treelist->SetItemText(m_code_osx, "new primary");
    CPPUNIT_ASSERT_EQUAL( wxString("new primary"),
                          m_treelist->GetItemText(m_code_osx, 0) );
}

void TreeListCtrlTestCase::DestructiveColumnCancellation()
{
#ifdef wxHAS_GENERIC_DATAVIEWCTRL
    delete m_treelist;
    m_treelist = nullptr;

    const auto createTreeList =
        []()
        {
            auto* const tree = new wxTreeListCtrl(
                wxTheApp->GetTopWindow(),
                wxID_ANY,
                wxDefaultPosition,
                wxSize(320, 160),
                wxTL_MULTIPLE);
            const int flags = wxCOL_RESIZABLE | wxCOL_REORDERABLE;
            CPPUNIT_ASSERT_EQUAL(
                0, tree->AppendColumn("first", 80, wxALIGN_LEFT, flags));
            CPPUNIT_ASSERT_EQUAL(
                1, tree->AppendColumn("second", 80, wxALIGN_LEFT, flags));
            const wxTreeListItem item =
                tree->AppendItem(tree->GetRootItem(), "row");
            tree->SetItemText(item, 1, "survivor");
            return tree;
        };

    const auto beginDrag =
        [](wxHeaderCtrl* header)
        {
            CPPUNIT_ASSERT( header );
            header->SetSize(0, 0, 320, 40);

            wxMouseEvent down(wxEVT_LEFT_DOWN);
            down.SetId(header->GetId());
            down.SetEventObject(header);
            down.SetPosition(wxPoint(90, 1));
            header->ProcessWindowEvent(down);
            CPPUNIT_ASSERT( header->HasCapture() );
        };

    // Destroying the composite consumes the internal columns. The already
    // reduced pinned tree model must not be restored from stale node offsets.
    wxTreeListCtrl* wrapper = createTreeList();
    wxDataViewCtrl* view = wrapper->GetDataView();
    wxHeaderCtrl* header = view->GenericGetHeader();
    beginDrag(header);

    int cancellations = 0;
    const wxWeakRef<wxTreeListCtrl> weakWrapper(wrapper);
    const wxWeakRef<wxDataViewCtrl> weakView(view);
    header->Bind(
        wxEVT_HEADER_DRAGGING_CANCELLED,
        [&](wxHeaderCtrlEvent&)
        {
            ++cancellations;
            delete wrapper;
            wrapper = nullptr;
        });

    CPPUNIT_ASSERT( wrapper->DeleteColumn(0) );
    CPPUNIT_ASSERT_EQUAL(1, cancellations);
    CPPUNIT_ASSERT( !weakWrapper.get() );
    CPPUNIT_ASSERT( !weakView.get() );

    // GetDataView() is public: its lifetime must be checked independently
    // from the wrapper before any rollback or column remapping.
    wrapper = createTreeList();
    view = wrapper->GetDataView();
    header = view->GenericGetHeader();
    beginDrag(header);

    cancellations = 0;
    const wxWeakRef<wxTreeListCtrl> weakSurvivingWrapper(wrapper);
    const wxWeakRef<wxDataViewCtrl> weakDestroyedView(view);
    header->Bind(
        wxEVT_HEADER_DRAGGING_CANCELLED,
        [&](wxHeaderCtrlEvent&)
        {
            ++cancellations;
            delete view;
            view = nullptr;
        });

    CPPUNIT_ASSERT( wrapper->DeleteColumn(0) );
    CPPUNIT_ASSERT_EQUAL(1, cancellations);
    CPPUNIT_ASSERT( weakSurvivingWrapper.get() == wrapper );
    CPPUNIT_ASSERT( !weakDestroyedView.get() );

    delete wrapper;
    wrapper = nullptr;
#endif // wxHAS_GENERIC_DATAVIEWCTRL
}

#endif // wxUSE_TREELISTCTRL
