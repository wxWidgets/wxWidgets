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

    #ifndef wxHAS_GENERIC_HEADERCTRL
        #include "wx/msw/private.h"
        #include "wx/msw/wrapcctl.h"
        #include "testableframe.h"
    #endif
#endif

#include <memory>

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class TreeListCtrlTestCase
{
public:
    TreeListCtrlTestCase();

protected:
    // Create the control with the given style.
    void Create(long style);

    // Add an item to the tree and increment m_numItems.
    wxTreeListItem AddItem(const char *label,
                           wxTreeListItem parent = wxTreeListItem(),
                           const char *numFiles = "",
                           const char *size = "");


    // Tests:

    // The control itself.
    std::unique_ptr<wxTreeListCtrl> m_treelist;

    // And some of its items.
    wxTreeListItem m_code,
                   m_code_osx,
                   m_code_osx_cocoa;

    // Also the total number of items in it initially
    unsigned m_numItems;

    wxDECLARE_NO_COPY_CLASS(TreeListCtrlTestCase);
};

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
    m_treelist = make_unique<wxTreeListCtrl>(wxTheApp->GetTopWindow(),
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

TreeListCtrlTestCase::TreeListCtrlTestCase()
{
    m_numItems = 0;
    Create(wxTL_MULTIPLE | wxTL_3STATE);
}


// ----------------------------------------------------------------------------
// the tests themselves
// ----------------------------------------------------------------------------

// Test various tree traversal methods.
TEST_CASE_METHOD(TreeListCtrlTestCase, "TreeListCtrl::Traversal", "[treelistctrl]")
{
    // GetParent() tests:
    wxTreeListItem root = m_treelist->GetRootItem();
    CHECK( !m_treelist->GetItemParent(root) );

    CHECK( m_treelist->GetItemParent(m_code) == root );
    CHECK( m_treelist->GetItemParent(m_code_osx) == m_code );

    // GetFirstChild() and GetNextSibling() tests:
    CHECK( m_treelist->GetFirstChild(root) == m_code );
    CHECK( m_treelist->GetNextSibling(
               m_treelist->GetNextSibling(
                   m_treelist->GetFirstChild(m_code))) == m_code_osx );

    // Get{First,Next}Item() test:
    unsigned numItems = 0;
    for ( wxTreeListItem item = m_treelist->GetFirstItem();
          item.IsOk();
          item = m_treelist->GetNextItem(item) )
    {
        numItems++;
    }

    CHECK( numItems == m_numItems );
}

// Test accessing items text.
TEST_CASE_METHOD(TreeListCtrlTestCase, "TreeListCtrl::ItemText", "[treelistctrl]")
{
    CHECK( m_treelist->GetItemText(m_code_osx_cocoa) == "Cocoa" );
    CHECK( m_treelist->GetItemText(m_code_osx_cocoa, 1) == "46" );

    m_treelist->SetItemText(m_code_osx_cocoa, "wxCocoa");
    CHECK( m_treelist->GetItemText(m_code_osx_cocoa) == "wxCocoa" );

    m_treelist->SetItemText(m_code_osx_cocoa, 1, "47");
    CHECK( m_treelist->GetItemText(m_code_osx_cocoa, 1) == "47" );
}

// Test checking and unchecking items.
TEST_CASE_METHOD(TreeListCtrlTestCase, "TreeListCtrl::ItemCheck", "[treelistctrl]")
{
    CHECK( m_treelist->GetCheckedState(m_code) == wxCHK_UNCHECKED );

    m_treelist->CheckItemRecursively(m_code);
    CHECK( m_treelist->GetCheckedState(m_code) == wxCHK_CHECKED );
    CHECK( m_treelist->GetCheckedState(m_code_osx) == wxCHK_CHECKED );
    CHECK( m_treelist->GetCheckedState(m_code_osx_cocoa) == wxCHK_CHECKED );

    m_treelist->UncheckItem(m_code_osx_cocoa);
    CHECK( m_treelist->GetCheckedState(m_code_osx_cocoa) == wxCHK_UNCHECKED );

    m_treelist->UpdateItemParentStateRecursively(m_code_osx_cocoa);
    CHECK( m_treelist->GetCheckedState(m_code_osx) == wxCHK_UNDETERMINED );
    CHECK( m_treelist->GetCheckedState(m_code) == wxCHK_UNDETERMINED );

    m_treelist->CheckItemRecursively(m_code_osx, wxCHK_UNCHECKED);
    m_treelist->UpdateItemParentStateRecursively(m_code_osx_cocoa);
    CHECK( m_treelist->GetCheckedState(m_code_osx) == wxCHK_UNCHECKED );
    CHECK( m_treelist->GetCheckedState(m_code) == wxCHK_UNDETERMINED );
}

TEST_CASE_METHOD(TreeListCtrlTestCase, "TreeListCtrl::ColumnMutation", "[treelistctrl]")
{
#ifdef wxHAS_GENERIC_DATAVIEWCTRL
    wxDataViewCtrl* const view = m_treelist->GetDataView();
    wxHeaderCtrl* const header = view->GenericGetHeader();
    REQUIRE( header );

    wxArrayInt order;
    order.push_back(2);
    order.push_back(0);
    order.push_back(1);
    header->SetColumnsOrder(order);

    // Removing a logical middle column after a visual reorder must preserve
    // both the remaining values and their exact display order.
    REQUIRE( m_treelist->DeleteColumn(1) );
    REQUIRE( (m_treelist->GetColumnCount()) == (2u) );
    REQUIRE( (m_treelist->GetItemText(m_code_osx, 0)) == (wxString("wxOSX")) );
    REQUIRE( (m_treelist->GetItemText(m_code_osx, 1)) == (wxString("2.36 MiB")) );
    REQUIRE( (view->GetColumn(0)->GetModelColumn()) == (0u) );
    REQUIRE( (view->GetColumn(1)->GetModelColumn()) == (1u) );

    const wxArrayInt orderAfterMiddleDelete = header->GetColumnsOrder();
    REQUIRE( (static_cast<int>(orderAfterMiddleDelete.size())) == (2) );
    REQUIRE( (orderAfterMiddleDelete[0]) == (1) );
    REQUIRE( (orderAfterMiddleDelete[1]) == (0) );

    // The old second column is sorted and then promoted to primary. Its
    // renderer and sort state must be promoted with it.
    view->GetColumn(1)->SetSortOrder(false);
    m_treelist->CheckItem(m_code_osx, wxCHK_CHECKED);
    REQUIRE( m_treelist->DeleteColumn(0) );
    REQUIRE( (m_treelist->GetColumnCount()) == (1u) );
    REQUIRE( (m_treelist->GetItemText(m_code_osx, 0)) == (wxString("2.36 MiB")) );
    REQUIRE( (m_treelist->GetCheckedState(m_code_osx)) == (wxCHK_CHECKED) );

    wxDataViewColumn* const primary = view->GetColumn(0);
    REQUIRE( (primary->GetModelColumn()) == (0u) );
    REQUIRE( primary->IsSortKey() );
    REQUIRE( !primary->IsSortOrderAscending() );
    REQUIRE( (view->GetSortingColumn()) == (primary) );
    REQUIRE(
        wxDynamicCast(
            primary->GetRenderer(),
            wxDataViewCheckIconTextRenderer) != nullptr );

    // Appending after all these mutations must keep the promoted value in
    // column zero and create a valid new positional model column.
    REQUIRE( (m_treelist->AppendColumn("Replacement")) == (1) );
    m_treelist->SetItemText(m_code_osx, 1, "replacement value");
    REQUIRE( (m_treelist->GetItemText(m_code_osx, 0)) == (wxString("2.36 MiB")) );
    REQUIRE( (m_treelist->GetItemText(m_code_osx, 1)) == (wxString("replacement value")) );

    REQUIRE( m_treelist->DeleteColumn(1) );
    REQUIRE( m_treelist->DeleteColumn(0) );
    REQUIRE( (m_treelist->GetColumnCount()) == (0u) );
#endif // wxHAS_GENERIC_DATAVIEWCTRL
}

TEST_CASE_METHOD(TreeListCtrlTestCase, "TreeListCtrl::ClearColumnsResetsValues", "[treelistctrl]")
{
    REQUIRE( (m_treelist->GetItemText(m_code_osx, 0)) == (wxString("wxOSX")) );
    REQUIRE( (m_treelist->GetItemText(m_code_osx, 2)) == (wxString("2.36 MiB")) );

    m_treelist->ClearColumns();
    REQUIRE( (m_treelist->GetColumnCount()) == (0u) );

    REQUIRE( (m_treelist->AppendColumn("Fresh")) == (0) );
    REQUIRE( (m_treelist->GetItemText(m_code_osx, 0)) == (wxString()) );
    REQUIRE( (m_treelist->GetItemText(m_code_osx_cocoa, 0)) == (wxString()) );

    m_treelist->SetItemText(m_code_osx, "new primary");
    REQUIRE( (m_treelist->GetItemText(m_code_osx, 0)) == (wxString("new primary")) );
}

TEST_CASE_METHOD(TreeListCtrlTestCase, "TreeListCtrl::DestructiveColumnCancellation", "[treelistctrl]")
{
#ifdef wxHAS_GENERIC_DATAVIEWCTRL
    m_treelist.reset();

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
            REQUIRE( (tree->AppendColumn("first", 80, wxALIGN_LEFT, flags)) == (0) );
            REQUIRE( (tree->AppendColumn("second", 80, wxALIGN_LEFT, flags)) == (1) );
            const wxTreeListItem item =
                tree->AppendItem(tree->GetRootItem(), "row");
            tree->SetItemText(item, 1, "survivor");
            return tree;
        };

    const auto beginDrag =
        [](wxHeaderCtrl* header)
        {
            REQUIRE( header );
            header->SetSize(0, 0, 320, 40);

#ifdef wxHAS_GENERIC_HEADERCTRL
            wxMouseEvent down(wxEVT_LEFT_DOWN);
            down.SetId(header->GetId());
            down.SetEventObject(header);
            down.SetPosition(wxPoint(90, 1));
            header->ProcessWindowEvent(down);
            REQUIRE( header->HasCapture() );
#else // native wxMSW header inside the generic data view
            const HWND hwndNative =
                ::FindWindowEx(GetHwndOf(header), nullptr, WC_HEADER, nullptr);
            REQUIRE( hwndNative );

            HDHITTESTINFO hit = {};
            hit.pt.x = 90;
            hit.pt.y = 1;
            const int item = static_cast<int>(::SendMessage(
                hwndNative, HDM_HITTEST, 0,
                reinterpret_cast<LPARAM>(&hit)));
            REQUIRE( item >= 0 );

            // The composite parent doesn't handle native mouse input. Start
            // the logical gesture with a native notification, not physical
            // input: no native capture is acquired by this test. Verify the
            // begin event and acceptance before testing destructive mutation.
            EventCounter begins(header, wxEVT_HEADER_BEGIN_REORDER);
            NMHEADER notification = {};
            notification.hdr.hwndFrom = hwndNative;
            notification.hdr.idFrom =
                static_cast<UINT_PTR>(
                    ::GetWindowLongPtr(hwndNative, GWLP_ID));
            notification.hdr.code = HDN_BEGINDRAG;
            notification.iItem = item;

            const LRESULT result = ::SendMessage(
                ::GetParent(hwndNative), WM_NOTIFY, notification.hdr.idFrom,
                reinterpret_cast<LPARAM>(&notification));
            REQUIRE( result == FALSE );
            REQUIRE( begins.GetCount() == 1 );
#endif // wxHAS_GENERIC_HEADERCTRL
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
            // The header owns this functor, so all capture access must precede
            // destruction of the composite and its header.
            auto* const dying = wrapper;
            wrapper = nullptr;
            delete dying;
        });

    REQUIRE( wrapper->DeleteColumn(0) );
    REQUIRE( (cancellations) == (1) );
    REQUIRE( !weakWrapper.get() );
    REQUIRE( !weakView.get() );

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
            auto* const dying = view;
            view = nullptr;
            delete dying;
        });

    REQUIRE( wrapper->DeleteColumn(0) );
    REQUIRE( (cancellations) == (1) );
    REQUIRE( weakSurvivingWrapper.get() == wrapper );
    REQUIRE( !weakDestroyedView.get() );

    delete wrapper;
    wrapper = nullptr;
#endif // wxHAS_GENERIC_DATAVIEWCTRL
}

#endif // wxUSE_TREELISTCTRL
