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

#include <memory>

#if wxUSE_DATAVIEWCTRL


#include "wx/app.h"
#include "wx/dataview.h"
#include "wx/uiaction.h"

#ifdef wxHAS_GENERIC_DATAVIEWCTRL
    #include "wx/headerctrl.h"
    #include "wx/textctrl.h"
    #include "wx/weakref.h"

    #ifndef wxHAS_GENERIC_HEADERCTRL
        #include "wx/msw/private.h"
        #include "wx/msw/wrapcctl.h"
    #endif
#endif // wxHAS_GENERIC_DATAVIEWCTRL

#if wxUSE_ACCESSIBILITY
    #include "wx/access.h"
#endif // wxUSE_ACCESSIBILITY

#ifdef __WXGTK__
    #include "waitfor.h"
#endif // __WXGTK__

#include "testableframe.h"
#include "asserthelper.h"

#include <algorithm>
#include <initializer_list>
#include <cstdint>
#include <vector>

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class DataViewCtrlTestCase
{
public:
    explicit DataViewCtrlTestCase(long style);

protected:
    void TestSelectionFor0and1();

    // the dataview control itself
    std::unique_ptr<wxDataViewTreeCtrl> m_dvc;

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

protected:
    // the dataview control itself
    std::unique_ptr<wxDataViewListCtrl> m_dvc;

    // constants
    const wxSize m_size;
    const int m_firstColumnWidth;

    // and the columns
    wxDataViewColumn* m_firstColumn;
    wxDataViewColumn* m_lastColumn;

    wxDECLARE_NO_COPY_CLASS(MultiColumnsDataViewCtrlTestCase);
};

#ifdef wxHAS_GENERIC_DATAVIEWCTRL

class CountedDataViewColumn final : public wxDataViewColumn
{
public:
    CountedDataViewColumn(const wxString& title,
                          unsigned int modelColumn,
                          int* destructionCount)
        : wxDataViewColumn(
              title,
              new wxDataViewTextRenderer(
                  wxDataViewTextRenderer::GetDefaultType(),
                  wxDATAVIEW_CELL_INERT),
              modelColumn,
              80,
              wxALIGN_LEFT,
              wxDATAVIEW_COL_RESIZABLE |
                  wxDATAVIEW_COL_REORDERABLE |
                  wxDATAVIEW_COL_SORTABLE),
          m_destructionCount(destructionCount)
    {
    }

    ~CountedDataViewColumn() override
    {
        ++*m_destructionCount;
    }

private:
    int* const m_destructionCount;
};

wxDataViewColumn* NewTestDataViewColumn(const wxString& title,
                                        unsigned int modelColumn)
{
    return new wxDataViewColumn(
        title,
        new wxDataViewTextRenderer(
            wxDataViewTextRenderer::GetDefaultType(),
            wxDATAVIEW_CELL_INERT),
        modelColumn,
        80,
        wxALIGN_LEFT,
        wxDATAVIEW_COL_RESIZABLE |
            wxDATAVIEW_COL_REORDERABLE |
            wxDATAVIEW_COL_SORTABLE);
}

class ReentrantDataViewRenderer final : public wxDataViewCustomRenderer
{
public:
    enum class Hook
    {
        CreateEditor,
        GetEditorValue
    };

    ReentrantDataViewRenderer(wxDataViewListCtrl* ctrl, Hook hook)
        : wxDataViewCustomRenderer(
              "string", wxDATAVIEW_CELL_EDITABLE, wxALIGN_LEFT),
          m_ctrl(ctrl),
          m_hook(hook)
    {
    }

    void SetNestedTarget(const wxDataViewItem& item,
                         wxDataViewColumn* column)
    {
        m_item = item;
        m_column = column;
    }

    int GetNestedAttempts() const { return m_nestedAttempts; }

    bool Render(wxRect rect, wxDC* dc, int state) override
    {
        RenderText(m_value, 0, rect, dc, state);
        return true;
    }

    wxSize GetSize() const override { return wxSize(80, 20); }

    bool SetValue(const wxVariant& value) override
    {
        m_value = value.GetString();
        return true;
    }

    bool GetValue(wxVariant& value) const override
    {
        value = m_value;
        return true;
    }

    bool HasEditorCtrl() const override { return true; }

    wxWindow* CreateEditorCtrl(wxWindow* parent,
                               wxRect rect,
                               const wxVariant& value) override
    {
        TryNestedEdit(Hook::CreateEditor);
        return new wxTextCtrl(
            parent,
            wxID_ANY,
            value.GetString(),
            rect.GetPosition(),
            rect.GetSize(),
            wxTE_PROCESS_ENTER);
    }

    bool GetValueFromEditorCtrl(wxWindow* ctrl,
                                wxVariant& value) override
    {
        TryNestedEdit(Hook::GetEditorValue);
        wxTextCtrl* const text = wxDynamicCast(ctrl, wxTextCtrl);
        if ( !text )
            return false;
        value = text->GetValue();
        return true;
    }

private:
    void TryNestedEdit(Hook hook)
    {
        if ( m_hook == hook )
        {
            ++m_nestedAttempts;
            m_ctrl->EditItem(m_item, m_column);
        }
    }

    wxDataViewListCtrl* const m_ctrl;
    const Hook m_hook;
    wxDataViewItem m_item;
    wxDataViewColumn* m_column = nullptr;
    int m_nestedAttempts = 0;
    wxString m_value;
};

class TransactionalTreeModel final : public wxDataViewModel
{
public:
    enum class Hook
    {
        None,
        AssociateOnRootChildren,
        AssociateOnParent,
        AssociateOnBatchContainer,
        AssociateOnClearBuild,
        ToggleSecondaryOnNestedCompare,
        NotifyValueChangedOnGetValue,
        DeleteColumnOnGetValue,
        UnsetSortOnParent
    };

    explicit TransactionalTreeModel(bool includeBatchItems = false)
        : m_container{ 3, nullptr, true, {} },
          m_childHigh{ 32, &m_container, false, {} },
          m_childLow{ 31, &m_container, false, {} },
          m_addedChild{ 33, &m_container, false, {} },
          m_two{ 2, nullptr, true, {} },
          m_twoChild{ 21, &m_two, false, {} },
          m_one{ 1, nullptr, false, {} },
          m_includeBatchItems(includeBatchItems)
    {
        m_container.children.push_back(&m_childHigh);
        m_container.children.push_back(&m_childLow);
        m_two.children.push_back(&m_twoChild);
    }

    void SetHook(Hook hook,
                 wxDataViewCtrl* ctrl = nullptr,
                 wxDataViewModel* replacement = nullptr,
                 wxDataViewColumn* secondary = nullptr)
    {
        m_hook = hook;
        m_ctrl = ctrl;
        m_replacement = replacement;
        m_secondary = secondary;
        m_hookFired = false;
    }

    wxDataViewItem Container() { return Item(&m_container); }
    wxDataViewItem ChildHigh() { return Item(&m_childHigh); }
    wxDataViewItem ChildLow() { return Item(&m_childLow); }
    wxDataViewItem Two() { return Item(&m_two); }
    wxDataViewItem One() { return Item(&m_one); }

    int GetHookCount() const { return m_hookCount; }
    int GetCompareCount() const { return m_compareCount; }
    void ClearHasValueColumns() const { m_hasValueColumns.clear(); }
    const std::vector<unsigned int>& GetHasValueColumns() const
    {
        return m_hasValueColumns;
    }

    bool NotifyBatchAdded()
    {
        m_includeBatchItems = true;
        wxDataViewItemArray items;
        items.push_back(Two());
        items.push_back(One());
        return ItemsAdded(wxDataViewItem(), items);
    }

    bool AddHiddenChild()
    {
        m_container.children.insert(
            m_container.children.begin(), &m_addedChild);
        return ItemAdded(Container(), Item(&m_addedChild));
    }

    bool AppendVisibleChild()
    {
        m_container.children.push_back(&m_addedChild);
        return ItemAdded(Container(), Item(&m_addedChild));
    }

    wxDataViewItem AddedChild() { return Item(&m_addedChild); }

    bool DeleteHiddenChild()
    {
        const auto found =
            std::find(m_container.children.begin(),
                      m_container.children.end(),
                      &m_addedChild);
        if ( found == m_container.children.end() )
            return false;

        m_container.children.erase(found);
        return ItemDeleted(Container(), Item(&m_addedChild));
    }

    void GetValue(wxVariant& value,
                  const wxDataViewItem& item,
                  unsigned int col) const override
    {
        Node* const node = FromItem(item);
        if ( m_hook == Hook::NotifyValueChangedOnGetValue &&
                !m_hookFired )
        {
            FireHook();
            const_cast<TransactionalTreeModel*>(this)->
                ValueChanged(item, col);
        }
        else if ( m_hook == Hook::DeleteColumnOnGetValue &&
                    !m_hookFired )
        {
            FireHook();
            m_ctrl->DeleteColumn(m_secondary);
        }

        value = wxString::Format(
            col == 0 ? "item %03d" : "secondary %03d",
            node ? node->value : 0);
    }

    bool SetValue(const wxVariant&,
                  const wxDataViewItem&,
                  unsigned int) override
    {
        return false;
    }

    wxDataViewItem GetParent(const wxDataViewItem& item) const override
    {
        if ( m_hook == Hook::AssociateOnParent && !m_hookFired )
            FireHook();
        else if ( m_hook == Hook::UnsetSortOnParent &&
                    !m_hookFired )
        {
            FireHook();
            m_secondary->UnsetAsSortKey();
        }

        Node* const node = FromItem(item);
        return node && node->parent ? Item(node->parent) : wxDataViewItem();
    }

    bool IsContainer(const wxDataViewItem& item) const override
    {
        Node* const node = FromItem(item);
        if ( m_hook == Hook::AssociateOnBatchContainer &&
                !m_hookFired &&
                node == &m_two )
        {
            FireHook();
        }

        return !item.IsOk() || (node && node->container);
    }

    bool HasValue(const wxDataViewItem&,
                  unsigned int col) const override
    {
        m_hasValueColumns.push_back(col);
        return true;
    }

    unsigned int GetChildren(
        const wxDataViewItem& item,
        wxDataViewItemArray& children) const override
    {
        if ( !item.IsOk() )
        {
            if ( (m_hook == Hook::AssociateOnRootChildren ||
                    m_hook == Hook::AssociateOnClearBuild) &&
                    !m_hookFired )
            {
                FireHook();
            }

            children.push_back(Item(&m_container));
            if ( m_includeBatchItems )
            {
                children.push_back(Item(&m_two));
                children.push_back(Item(&m_one));
            }
            return children.size();
        }

        Node* const node = FromItem(item);
        if ( !node )
            return 0;

        for ( Node* const child : node->children )
            children.push_back(Item(child));
        return children.size();
    }

    int Compare(const wxDataViewItem& lhs,
                const wxDataViewItem& rhs,
                unsigned int,
                bool ascending) const override
    {
        ++m_compareCount;
        Node* const left = FromItem(lhs);
        Node* const right = FromItem(rhs);
        if ( m_hook == Hook::ToggleSecondaryOnNestedCompare &&
                !m_hookFired &&
                left && right &&
                left->parent == &m_container &&
                right->parent == &m_container )
        {
            FireHook();
            m_secondary->SetSortOrder(false);
        }

        const int result =
            (left ? left->value : 0) - (right ? right->value : 0);
        return ascending ? result : -result;
    }

private:
    struct Node
    {
        int value;
        Node* parent;
        bool container;
        wxVector<Node*> children;
    };

    static wxDataViewItem Item(Node* node)
    {
        return wxDataViewItem(node);
    }

    static Node* FromItem(const wxDataViewItem& item)
    {
        return static_cast<Node*>(item.GetID());
    }

    void FireHook() const
    {
        m_hookFired = true;
        ++m_hookCount;
        if ( m_ctrl && m_replacement )
            m_ctrl->AssociateModel(m_replacement);
    }

    mutable Node m_container;
    mutable Node m_childHigh;
    mutable Node m_childLow;
    mutable Node m_addedChild;
    mutable Node m_two;
    mutable Node m_twoChild;
    mutable Node m_one;
    bool m_includeBatchItems;
    mutable Hook m_hook = Hook::None;
    mutable wxDataViewCtrl* m_ctrl = nullptr;
    mutable wxDataViewModel* m_replacement = nullptr;
    mutable wxDataViewColumn* m_secondary = nullptr;
    mutable bool m_hookFired = false;
    mutable int m_hookCount = 0;
    mutable int m_compareCount = 0;
    mutable std::vector<unsigned int> m_hasValueColumns;
};

class CountingFlatDataViewModel final : public wxDataViewModel
{
public:
    explicit CountingFlatDataViewModel(size_t count)
    {
        m_values.reserve(count);
        for ( size_t n = 0; n < count; ++n )
            m_values.push_back(static_cast<int>(count - n));
    }

    int GetCompareCount() const { return m_compareCount; }

    void GetValue(wxVariant& value,
                  const wxDataViewItem& item,
                  unsigned int) const override
    {
        value = static_cast<long>(
            *static_cast<int*>(item.GetID()));
    }

    bool SetValue(const wxVariant&,
                  const wxDataViewItem&,
                  unsigned int) override
    {
        return false;
    }

    wxDataViewItem GetParent(const wxDataViewItem&) const override
    {
        return wxDataViewItem();
    }

    bool IsContainer(const wxDataViewItem& item) const override
    {
        return !item.IsOk();
    }

    unsigned int GetChildren(
        const wxDataViewItem& item,
        wxDataViewItemArray& children) const override
    {
        if ( item.IsOk() )
            return 0;

        for ( const int& value : m_values )
        {
            children.push_back(
                wxDataViewItem(const_cast<int*>(&value)));
        }
        return children.size();
    }

    int Compare(const wxDataViewItem& lhs,
                const wxDataViewItem& rhs,
                unsigned int,
                bool ascending) const override
    {
        ++m_compareCount;
        const int left = *static_cast<int*>(lhs.GetID());
        const int right = *static_cast<int*>(rhs.GetID());
        const int result = left - right;
        return ascending ? result : -result;
    }

private:
    std::vector<int> m_values;
    mutable int m_compareCount = 0;
};

class DestroyOnEditableQueryModel final : public wxDataViewIndexListModel
{
public:
    DestroyOnEditableQueryModel()
        : wxDataViewIndexListModel(1)
    {
    }

    void Arm(wxDataViewCtrl* ctrl)
    {
        m_ctrl = ctrl;
        m_fired = false;
    }

    bool Fired() const { return m_fired; }

    void GetValueByRow(wxVariant& value,
                       unsigned int,
                       unsigned int) const override
    {
        value = "editable";
    }

    bool SetValueByRow(const wxVariant&,
                       unsigned int,
                       unsigned int) override
    {
        return true;
    }

    bool IsEnabledByRow(unsigned int,
                        unsigned int) const override
    {
        if ( m_ctrl && !m_fired )
        {
            m_fired = true;
            wxDataViewCtrl* const ctrl = m_ctrl;
            m_ctrl = nullptr;
            delete ctrl;
        }

        return true;
    }

private:
    mutable wxDataViewCtrl* m_ctrl = nullptr;
    mutable bool m_fired = false;
};

class IconTextFinishModel final : public wxDataViewIndexListModel
{
public:
    explicit IconTextFinishModel(const wxDataViewIconText& value)
        : wxDataViewIndexListModel(1),
          m_value(value)
    {
    }

    void Arm(wxDataViewCtrl* ctrl, wxDataViewColumn* column)
    {
        m_ctrl = ctrl;
        m_column = column;
        m_armedCalls = 0;
    }

    int GetArmedCalls() const { return m_armedCalls; }
    const wxDataViewIconText& GetStoredValue() const { return m_value; }

    void GetValueByRow(wxVariant& value,
                       unsigned int,
                       unsigned int) const override
    {
        if ( m_ctrl )
        {
            ++m_armedCalls;
            wxDataViewCtrl* const ctrl = m_ctrl;
            wxDataViewColumn* const column = m_column;
            m_ctrl = nullptr;
            m_column = nullptr;
            ctrl->DeleteColumn(column);
        }

        value << m_value;
    }

    bool SetValueByRow(const wxVariant& value,
                       unsigned int,
                       unsigned int) override
    {
        wxDataViewIconText updated;
        updated << value;
        m_value = updated;
        return true;
    }

private:
    wxDataViewIconText m_value;
    mutable wxDataViewCtrl* m_ctrl = nullptr;
    mutable wxDataViewColumn* m_column = nullptr;
    mutable int m_armedCalls = 0;
};

class InspectableDataViewCtrl final : public wxDataViewCtrl
{
public:
    InspectableDataViewCtrl(wxWindow* parent, wxWindowID id)
        : wxDataViewCtrl(parent, id)
    {
    }

    using wxDataViewCtrl::GetItemByRow;
    using wxDataViewCtrl::GetRowByItem;
};

void CheckDataViewColumns(
    wxDataViewCtrl* dvc,
    const char* stage,
    std::initializer_list<wxDataViewColumn*> logical,
    std::initializer_list<wxDataViewColumn*> visual)
{
    INFO("Column mutation stage: " << stage);
    REQUIRE( dvc->GetColumnCount() == logical.size() );
    REQUIRE( visual.size() == logical.size() );

    size_t logicalIndex = 0;
    for ( wxDataViewColumn* const column : logical )
    {
        INFO("Logical column index " << logicalIndex);
        CHECK( dvc->GetColumn(logicalIndex) == column );
        ++logicalIndex;
    }

    size_t visualPosition = 0;
    for ( wxDataViewColumn* const column : visual )
    {
        INFO("Visual column position " << visualPosition);
        CHECK( dvc->GetColumnAt(visualPosition) == column );
        CHECK( dvc->GetColumnPosition(column) ==
               static_cast<int>(visualPosition) );
        ++visualPosition;
    }
}

void BeginDataViewHeaderDrag(wxHeaderCtrl* header, int x)
{
    REQUIRE( header );
    header->SetSize(0, 0, 320, 40);

#ifdef wxHAS_GENERIC_HEADERCTRL
    wxMouseEvent down(wxEVT_LEFT_DOWN);
    down.SetId(header->GetId());
    down.SetEventObject(header);
    down.SetPosition(wxPoint(x, 1));
    header->ProcessWindowEvent(down);

    REQUIRE( header->HasCapture() );
#else // native wxMSW header inside the generic data view
    const HWND hwndNative =
        ::FindWindowEx(GetHwndOf(header), nullptr, WC_HEADER, nullptr);
    REQUIRE( hwndNative );

    HDHITTESTINFO hit = {};
    hit.pt.x = x;
    hit.pt.y = 1;
    const int item = static_cast<int>(::SendMessage(
        hwndNative, HDM_HITTEST, 0, reinterpret_cast<LPARAM>(&hit)));
    REQUIRE( item >= 0 );

    // A wxMouseEvent sent to the composite parent doesn't enter the native
    // child's mouse handling. Start the logical gesture at its notification
    // boundary instead: this is not physical input and doesn't acquire native
    // capture. Acceptance and the begin event verify the state needed by the
    // cancellation/lifetime assertions below.
    EventCounter begins(header, wxEVT_HEADER_BEGIN_REORDER);
    NMHEADER notification = {};
    notification.hdr.hwndFrom = hwndNative;
    notification.hdr.idFrom =
        static_cast<UINT_PTR>(::GetWindowLongPtr(hwndNative, GWLP_ID));
    notification.hdr.code = HDN_BEGINDRAG;
    notification.iItem = item;

    const LRESULT result = ::SendMessage(
        ::GetParent(hwndNative), WM_NOTIFY, notification.hdr.idFrom,
        reinterpret_cast<LPARAM>(&notification));
    REQUIRE( result == FALSE );
    REQUIRE( begins.GetCount() == 1 );
#endif // wxHAS_GENERIC_HEADERCTRL
}

#endif // wxHAS_GENERIC_DATAVIEWCTRL

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
        WaitFor("wxDataViewCtrl upder", [this, item, existence]() {
            const bool isItemRectEmpty = m_dvc->GetItemRect(item).IsEmpty();
            switch ( existence )
            {
                case wxITEM_APPEAR:
                    if ( !isItemRectEmpty )
                        return true;
                    break;

                case wxITEM_DISAPPEAR:
                    if ( isItemRectEmpty )
                        return true;
                    break;
            }

            return false;
        });
#else // !__WXGTK__
        wxUnusedVar(item);
        wxUnusedVar(existence);
#endif // __WXGTK__
    }

    // The dataview control.
    std::unique_ptr<wxDataViewCtrl> m_dvc;

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
    m_dvc = make_unique<wxDataViewTreeCtrl>(wxTheApp->GetTopWindow(),
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


MultiColumnsDataViewCtrlTestCase::MultiColumnsDataViewCtrlTestCase()
    : m_size(200, 100), // In DIPs, like the column widths below.
      m_firstColumnWidth(50)
{
    m_dvc = make_unique<wxDataViewListCtrl>(wxTheApp->GetTopWindow(),
                                            wxID_ANY);

    m_firstColumn =
        m_dvc->AppendTextColumn(wxString(), wxDATAVIEW_CELL_INERT, m_firstColumnWidth);
    m_lastColumn =
        m_dvc->AppendTextColumn(wxString(), wxDATAVIEW_CELL_INERT);

    // Set size after columns appending to extend size of the last column.
    m_dvc->SetSize(m_dvc->FromDIP(m_size));
    m_dvc->Layout();
    m_dvc->Refresh();
    m_dvc->Update();
}


DataViewCtrlWithCustomModelTestCase::DataViewCtrlWithCustomModelTestCase()
{
    m_dvc = make_unique<wxDataViewCtrl>(wxTheApp->GetTopWindow(),
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
    CHECK( m_dvc->GetSelections(sel2) == wxSsize(sel) );

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

    // GetItemRect() is only non-empty for items in the viewport, so bring
    // each of the newly shown items into view before checking it.
    m_dvc->EnsureVisible(m_leafHidden);
    UpdateAndWaitForItem(m_leafHidden, wxITEM_APPEAR);
    CHECK( !m_dvc->GetItemRect(m_leafHidden).IsEmpty() );
    m_dvc->EnsureVisible(m_grandchildHidden);
    UpdateAndWaitForItem(m_grandchildHidden, wxITEM_APPEAR);
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
    WaitFor("wxDataViewCtrl layout", [this]() {
        return m_dvc->GetTopItem() != m_root;
    });
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
    WaitFor("wxDataViewCtrl to be realized", [this]() {
        return m_firstColumn->GetWidth() != 0;
    });
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

#ifdef wxHAS_GENERIC_DATAVIEWCTRL

TEST_CASE("wxDVC::GenericColumnOrderMutation",
          "[wxDataViewCtrl][column][generic]")
{
    wxDataViewListCtrl dvc(wxTheApp->GetTopWindow(), wxID_ANY);

    wxDataViewColumn* const colA = NewTestDataViewColumn("A", 0);
    wxDataViewColumn* const colB = NewTestDataViewColumn("B", 1);
    wxDataViewColumn* const colC = NewTestDataViewColumn("C", 2);
    wxDataViewColumn* const colD = NewTestDataViewColumn("D", 3);

    REQUIRE( dvc.AppendColumn(colA) );
    REQUIRE( dvc.AppendColumn(colB) );
    REQUIRE( dvc.AppendColumn(colC) );
    REQUIRE( dvc.AppendColumn(colD) );

    wxHeaderCtrl* const header = dvc.GenericGetHeader();
    REQUIRE( header );

    wxArrayInt order;
    order.push_back(2);
    order.push_back(0);
    order.push_back(3);
    order.push_back(1);
    header->SetColumnsOrder(order);

    CheckDataViewColumns(
        &dvc,
        "initial reorder",
        { colA, colB, colC, colD },
        { colC, colA, colD, colB });

    wxDataViewColumn* const colE = NewTestDataViewColumn("E", 4);
    REQUIRE( dvc.AppendColumn(colE) );
    CheckDataViewColumns(
        &dvc,
        "append",
        { colA, colB, colC, colD, colE },
        { colC, colA, colD, colB, colE });

    wxDataViewColumn* const colP = NewTestDataViewColumn("P", 5);
    REQUIRE( dvc.PrependColumn(colP) );
    CheckDataViewColumns(
        &dvc,
        "prepend",
        { colP, colA, colB, colC, colD, colE },
        { colP, colC, colA, colD, colB, colE });

    wxDataViewColumn* const colX = NewTestDataViewColumn("X", 6);
    REQUIRE( dvc.InsertColumn(2, colX) );
    CheckDataViewColumns(
        &dvc,
        "middle insertion",
        { colP, colA, colX, colB, colC, colD, colE },
        { colP, colC, colX, colA, colD, colB, colE });

    REQUIRE( dvc.DeleteColumn(colA) );
    CheckDataViewColumns(
        &dvc,
        "logical deletion",
        { colP, colX, colB, colC, colD, colE },
        { colP, colC, colX, colD, colB, colE });
}

TEST_CASE("wxDVC::GenericSortingColumnRemapping",
          "[wxDataViewCtrl][column][sort][generic]")
{
    wxDataViewListCtrl dvc(wxTheApp->GetTopWindow(), wxID_ANY);

    wxDataViewColumn* const colA = NewTestDataViewColumn("A", 0);
    wxDataViewColumn* const colB = NewTestDataViewColumn("B", 1);
    wxDataViewColumn* const colC = NewTestDataViewColumn("C", 2);
    wxDataViewColumn* const colD = NewTestDataViewColumn("D", 3);

    REQUIRE( dvc.AppendColumn(colA) );
    REQUIRE( dvc.AppendColumn(colB) );
    REQUIRE( dvc.AppendColumn(colC) );
    REQUIRE( dvc.AppendColumn(colD) );
    REQUIRE( dvc.AllowMultiColumnSort(true) );

    colB->SetSortOrder(true);
    colD->SetSortOrder(false);

    wxVector<wxDataViewColumn*> sorting = dvc.GetSortingColumns();
    REQUIRE( sorting.size() == 2 );
    CHECK( sorting[0] == colB );
    CHECK( sorting[1] == colD );

    wxHeaderCtrl* const header = dvc.GenericGetHeader();
    REQUIRE( header );
    wxArrayInt order;
    order.push_back(2);
    order.push_back(0);
    order.push_back(3);
    order.push_back(1);
    header->SetColumnsOrder(order);

    wxDataViewColumn* const colP = NewTestDataViewColumn("P", 4);
    REQUIRE( dvc.PrependColumn(colP) );
    wxDataViewColumn* const colX = NewTestDataViewColumn("X", 5);
    REQUIRE( dvc.InsertColumn(2, colX) );
    REQUIRE( dvc.DeleteColumn(colA) );

    sorting = dvc.GetSortingColumns();
    REQUIRE( sorting.size() == 2 );
    CHECK( sorting[0] == colB );
    CHECK( sorting[1] == colD );
    CHECK( dvc.GetSortingColumn() == colB );

    REQUIRE( dvc.DeleteColumn(colB) );
    sorting = dvc.GetSortingColumns();
    REQUIRE( sorting.size() == 1 );
    CHECK( sorting[0] == colD );
    CHECK( dvc.GetSortingColumn() == colD );

    REQUIRE( dvc.DeleteColumn(colD) );
    CHECK( dvc.GetSortingColumns().empty() );
    CHECK( dvc.GetSortingColumn() == nullptr );
}

TEST_CASE("wxDVC::GenericSortCancellationIsLifetimeSafe",
          "[wxDataViewCtrl][column][sort][header][lifetime][reentrant][generic]")
{
    SECTION("cancellation destroys the control")
    {
        auto* dvc =
            new wxDataViewListCtrl(wxTheApp->GetTopWindow(), wxID_ANY);
        wxDataViewColumn* const first =
            dvc->AppendTextColumn(
                "first",
                wxDATAVIEW_CELL_INERT,
                80,
                wxALIGN_LEFT,
                wxDATAVIEW_COL_SORTABLE |
                    wxDATAVIEW_COL_REORDERABLE);
        wxDataViewColumn* const target =
            dvc->AppendTextColumn(
                "target",
                wxDATAVIEW_CELL_INERT,
                80,
                wxALIGN_LEFT,
                wxDATAVIEW_COL_SORTABLE |
                    wxDATAVIEW_COL_REORDERABLE);
        REQUIRE( first );
        REQUIRE( target );
        first->SetSortOrder(true);

        wxHeaderCtrl* const header = dvc->GenericGetHeader();
        BeginDataViewHeaderDrag(header, 90);

        int cancellations = 0;
        const wxWeakRef<wxDataViewListCtrl> weakDvc(dvc);
        header->Bind(
            wxEVT_HEADER_DRAGGING_CANCELLED,
            [&](wxHeaderCtrlEvent&)
            {
                ++cancellations;
                // Deleting the header also deletes this bound functor: update
                // its captured outputs before the terminal destruction.
                auto* const dying = dvc;
                dvc = nullptr;
                delete dying;
            });

        // Resetting the old key updates its header item, synchronously
        // cancelling the active drag and deleting both columns.
        target->SetSortOrder(true);

        CHECK( cancellations == 1 );
        CHECK( weakDvc.get() == nullptr );
    }

    SECTION("cancellation removes the requested column")
    {
        wxDataViewListCtrl dvc(wxTheApp->GetTopWindow(), wxID_ANY);
        wxDataViewColumn* const first =
            dvc.AppendTextColumn(
                "first",
                wxDATAVIEW_CELL_INERT,
                80,
                wxALIGN_LEFT,
                wxDATAVIEW_COL_SORTABLE |
                    wxDATAVIEW_COL_REORDERABLE);
        wxDataViewColumn* const target =
            dvc.AppendTextColumn(
                "target",
                wxDATAVIEW_CELL_INERT,
                80,
                wxALIGN_LEFT,
                wxDATAVIEW_COL_SORTABLE |
                    wxDATAVIEW_COL_REORDERABLE);
        REQUIRE( first );
        REQUIRE( target );
        first->SetSortOrder(true);

        wxHeaderCtrl* const header = dvc.GenericGetHeader();
        BeginDataViewHeaderDrag(header, 90);

        int cancellations = 0;
        header->Bind(
            wxEVT_HEADER_DRAGGING_CANCELLED,
            [&](wxHeaderCtrlEvent&)
            {
                ++cancellations;
                CHECK( dvc.DeleteColumn(target) );
            });

        target->SetSortOrder(true);

        CHECK( cancellations == 1 );
        CHECK( dvc.GetColumnCount() == 1 );
        CHECK( dvc.GetColumn(0) == first );
        CHECK( dvc.GetSortingColumns().empty() );
    }
}

TEST_CASE("wxDVC::GenericExpanderColumnDeletion",
          "[wxDataViewCtrl][column][generic]")
{
    wxDataViewListCtrl dvc(wxTheApp->GetTopWindow(), wxID_ANY);

    wxDataViewColumn* const colA = NewTestDataViewColumn("A", 0);
    wxDataViewColumn* const colB = NewTestDataViewColumn("B", 1);
    wxDataViewColumn* const colC = NewTestDataViewColumn("C", 2);

    REQUIRE( dvc.AppendColumn(colA) );
    REQUIRE( dvc.AppendColumn(colB) );
    REQUIRE( dvc.AppendColumn(colC) );

    dvc.SetExpanderColumn(colB);
    CHECK( dvc.GetExpanderColumn() == colB );

    REQUIRE( dvc.DeleteColumn(colA) );
    CHECK( dvc.GetExpanderColumn() == colB );

    REQUIRE( dvc.DeleteColumn(colB) );
    CHECK( dvc.GetExpanderColumn() == nullptr );
}

TEST_CASE("wxDVC::GenericColumnOwnership",
          "[wxDataViewCtrl][column][lifetime][generic]")
{
    int destructionCount = 0;
    auto* const dvc =
        new wxDataViewListCtrl(wxTheApp->GetTopWindow(), wxID_ANY);

    auto* const removed =
        new CountedDataViewColumn("removed", 0, &destructionCount);
    auto* const cleared1 =
        new CountedDataViewColumn("cleared 1", 1, &destructionCount);
    auto* const cleared2 =
        new CountedDataViewColumn("cleared 2", 2, &destructionCount);

    REQUIRE( dvc->AppendColumn(removed) );
    REQUIRE( dvc->AppendColumn(cleared1) );
    REQUIRE( dvc->AppendColumn(cleared2) );

    REQUIRE( dvc->DeleteColumn(removed) );
    const int destructionCountAfterDelete = destructionCount;
    CHECK( destructionCountAfterDelete == 1 );

    REQUIRE( dvc->ClearColumns() );
    CHECK( destructionCount == 3 );

    delete dvc;
    CHECK( destructionCount == 3 );

    // Keep a failing implementation leak-free while preserving the value
    // observed above. A successful implementation has already destroyed it.
    if ( destructionCountAfterDelete == 0 )
        delete removed;
}

TEST_CASE("wxDVC::GenericListStoreColumnSchema",
          "[wxDataViewCtrl][column][liststore][generic]")
{
    wxDataViewListCtrl dvc(wxTheApp->GetTopWindow(), wxID_ANY);

    wxDataViewColumn* const colA = dvc.AppendTextColumn("A");
    wxDataViewColumn* const colB = dvc.AppendTextColumn("B");
    REQUIRE( colA );
    REQUIRE( colB );

    wxVector<wxVariant> values;
    values.push_back(wxVariant("a"));
    values.push_back(wxVariant("b"));
    dvc.AppendItem(values);

    wxHeaderCtrl* const header = dvc.GenericGetHeader();
    REQUIRE( header );
    wxArrayInt reordered;
    reordered.push_back(1);
    reordered.push_back(0);
    header->SetColumnsOrder(reordered);

    wxDataViewColumn* const colX = NewTestDataViewColumn("X", 99);
    REQUIRE( dvc.InsertColumn(1, colX, "string") );
    CHECK( colA->GetModelColumn() == 0 );
    CHECK( colX->GetModelColumn() == 1 );
    CHECK( colB->GetModelColumn() == 2 );

    dvc.SetTextValue("x", 0, 1);
    CHECK( dvc.GetTextValue(0, 0) == "a" );
    CHECK( dvc.GetTextValue(0, 1) == "x" );
    CHECK( dvc.GetTextValue(0, 2) == "b" );

    REQUIRE( dvc.DeleteColumn(colA) );
    CHECK( colX->GetModelColumn() == 0 );
    CHECK( colB->GetModelColumn() == 1 );
    CHECK( dvc.GetTextValue(0, 0) == "x" );
    CHECK( dvc.GetTextValue(0, 1) == "b" );

    REQUIRE( dvc.ClearColumns() );
    REQUIRE( dvc.AppendTextColumn("replacement") );
    dvc.SetTextValue("replacement value", 0, 0);
    CHECK( dvc.GetTextValue(0, 0) == "replacement value" );
}

TEST_CASE("wxDVC::GenericListColumnDeletionSurvivesDestructiveCancellation",
          "[wxDataViewCtrl][column][liststore][header][lifetime][reentrant][generic]")
{
    auto* dvc =
        new wxDataViewListCtrl(wxTheApp->GetTopWindow(), wxID_ANY);
    wxDataViewColumn* const first = dvc->AppendTextColumn(
        "first",
        wxDATAVIEW_CELL_INERT,
        80,
        wxALIGN_LEFT,
        wxDATAVIEW_COL_REORDERABLE);
    wxDataViewColumn* const survivor = dvc->AppendTextColumn(
        "survivor",
        wxDATAVIEW_CELL_INERT,
        80,
        wxALIGN_LEFT,
        wxDATAVIEW_COL_REORDERABLE);
    REQUIRE( first );
    REQUIRE( survivor );
    dvc->AppendItem({ wxVariant("remove"), wxVariant("keep") });

    wxDataViewListStore* const store = dvc->GetStore();
    store->IncRef();
    wxObjectDataPtr<wxDataViewListStore> pinnedStore(store);

    wxHeaderCtrl* const header = dvc->GenericGetHeader();
    BeginDataViewHeaderDrag(header, 90);

    int cancellations = 0;
    const wxWeakRef<wxDataViewListCtrl> weakDvc(dvc);
    header->Bind(
        wxEVT_HEADER_DRAGGING_CANCELLED,
        [&](wxHeaderCtrlEvent&)
        {
            ++cancellations;
            auto* const dying = dvc;
            dvc = nullptr;
            delete dying;
        });

    const bool deleted = dvc->DeleteColumn(first);

    CHECK( deleted );
    CHECK( cancellations == 1 );
    CHECK( weakDvc.get() == nullptr );
    REQUIRE( pinnedStore->m_cols.size() == 1 );
    REQUIRE( pinnedStore->m_data.size() == 1 );
    REQUIRE( pinnedStore->m_data[0]->m_values.size() == 1 );
    CHECK( pinnedStore->m_data[0]->m_values[0].GetString() == "keep" );
}

TEST_CASE("wxDVC::GenericEditingDoneColumnMutation",
          "[wxDataViewCtrl][column][editing][generic]")
{
    wxDataViewListCtrl dvc(wxTheApp->GetTopWindow(), wxID_ANY);

    auto* const colA = new wxDataViewColumn(
        "A",
        new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_EDITABLE),
        0);
    wxDataViewColumn* const colB = dvc.AppendTextColumn("B");
    REQUIRE( dvc.PrependColumn(colA, "string") );
    REQUIRE( colB );

    wxVector<wxVariant> values;
    values.push_back(wxVariant("old A"));
    values.push_back(wxVariant("old B"));
    dvc.AppendItem(values);

    int editingDoneCount = 0;
    wxDataViewColumn* inserted = nullptr;
    dvc.Bind(
        wxEVT_DATAVIEW_ITEM_EDITING_DONE,
        [&](wxDataViewEvent&)
        {
            ++editingDoneCount;
            inserted = NewTestDataViewColumn("inserted", 99);
            CHECK( dvc.InsertColumn(0, inserted, "string") );
            dvc.SetTextValue("new first", 0, 0);
        });

    dvc.EditItem(dvc.RowToItem(0), colA);
    wxTextCtrl* const editor =
        wxDynamicCast(colA->GetRenderer()->GetEditorCtrl(), wxTextCtrl);
    REQUIRE( editor );
    editor->ChangeValue("edited A");

    wxDataViewColumn* const appended =
        NewTestDataViewColumn("appended", 99);
    REQUIRE( dvc.AppendColumn(appended, "string") );

    CHECK( editingDoneCount == 1 );
    REQUIRE( inserted );
    CHECK( dvc.GetColumnCount() == 4 );
    CHECK( dvc.GetColumn(0) == inserted );
    CHECK( dvc.GetColumn(1) == colA );
    CHECK( dvc.GetColumn(2) == colB );
    CHECK( dvc.GetColumn(3) == appended );
    CHECK( inserted->GetModelColumn() == 0 );
    CHECK( colA->GetModelColumn() == 1 );
    CHECK( colB->GetModelColumn() == 2 );
    CHECK( appended->GetModelColumn() == 3 );
    CHECK( dvc.GetTextValue(0, 0) == "new first" );
    CHECK( dvc.GetTextValue(0, 1) == "edited A" );
    CHECK( dvc.GetTextValue(0, 2) == "old B" );
}

TEST_CASE("wxDVC::GenericEditingDoneDeletesRenderer",
          "[wxDataViewCtrl][column][editing][lifetime][generic]")
{
    wxDataViewListCtrl dvc(wxTheApp->GetTopWindow(), wxID_ANY);

    auto* const edited = new wxDataViewColumn(
        "edited",
        new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_EDITABLE),
        0);
    REQUIRE( dvc.AppendColumn(edited, "string") );
    wxDataViewColumn* const survivor = dvc.AppendTextColumn("survivor");
    REQUIRE( survivor );

    wxVector<wxVariant> values;
    values.push_back(wxVariant("remove me"));
    values.push_back(wxVariant("keep me"));
    dvc.AppendItem(values);

    int editingDoneCount = 0;
    dvc.Bind(
        wxEVT_DATAVIEW_ITEM_EDITING_DONE,
        [&](wxDataViewEvent&)
        {
            ++editingDoneCount;
            CHECK( dvc.DeleteColumn(edited) );
        });

    dvc.EditItem(dvc.RowToItem(0), edited);
    wxTextCtrl* const editor =
        wxDynamicCast(edited->GetRenderer()->GetEditorCtrl(), wxTextCtrl);
    REQUIRE( editor );
    editor->ChangeValue("must not leak into the next column");

    wxDataViewColumn* const appended =
        NewTestDataViewColumn("appended", 99);
    REQUIRE( dvc.AppendColumn(appended, "string") );

    CHECK( editingDoneCount == 1 );
    CHECK( dvc.GetColumnCount() == 2 );
    CHECK( dvc.GetColumn(0) == survivor );
    CHECK( dvc.GetColumn(1) == appended );
    CHECK( survivor->GetModelColumn() == 0 );
    CHECK( dvc.GetTextValue(0, 0) == "keep me" );
}

TEST_CASE("wxDVC::GenericModelReplacementFinishesOldEditor",
          "[wxDataViewCtrl][editing][model][generic]")
{
    wxObjectDataPtr<wxDataViewListStore> oldModel(
        new wxDataViewListStore);
    oldModel->AppendColumn("string");
    oldModel->AppendItem({ wxVariant("old model value") });

    wxObjectDataPtr<wxDataViewListStore> newModel(
        new wxDataViewListStore);
    newModel->AppendColumn("string");
    newModel->AppendItem({ wxVariant("new model value") });

    InspectableDataViewCtrl dvc(wxTheApp->GetTopWindow(), wxID_ANY);
    REQUIRE( dvc.AssociateModel(oldModel.get()) );

    auto* const column = new wxDataViewColumn(
        "editable",
        new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_EDITABLE),
        0);
    REQUIRE( dvc.AppendColumn(column) );

    dvc.EditItem(oldModel->GetItem(0), column);
    wxTextCtrl* const editor =
        wxDynamicCast(column->GetRenderer()->GetEditorCtrl(), wxTextCtrl);
    REQUIRE( editor );
    editor->ChangeValue("committed to old model");

    REQUIRE( dvc.AssociateModel(newModel.get()) );

    wxVariant oldValue;
    oldModel->GetValueByRow(oldValue, 0, 0);
    CHECK( oldValue.GetString() == "committed to old model" );

    wxVariant newValue;
    newModel->GetValueByRow(newValue, 0, 0);
    CHECK( newValue.GetString() == "new model value" );
}

TEST_CASE("wxDVC::GenericReentrantRendererEditTransaction",
          "[wxDataViewCtrl][editing][reentrant][renderer][generic]")
{
    ReentrantDataViewRenderer::Hook hook =
        ReentrantDataViewRenderer::Hook::CreateEditor;

    SECTION("CreateEditorCtrl")
    {
        hook = ReentrantDataViewRenderer::Hook::CreateEditor;
    }
    SECTION("GetValueFromEditorCtrl")
    {
        hook = ReentrantDataViewRenderer::Hook::GetEditorValue;
    }

    wxDataViewListCtrl dvc(wxTheApp->GetTopWindow(), wxID_ANY);
    auto* const renderer = new ReentrantDataViewRenderer(&dvc, hook);
    auto* const source = new wxDataViewColumn(
        "source", renderer, 0, 80, wxALIGN_LEFT);
    auto* const target = new wxDataViewColumn(
        "target",
        new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_EDITABLE),
        1,
        80,
        wxALIGN_LEFT);

    REQUIRE( dvc.AppendColumn(source, "string") );
    REQUIRE( dvc.AppendColumn(target, "string") );
    dvc.AppendItem({ wxVariant("source value"), wxVariant("target value") });

    const wxDataViewItem item = dvc.RowToItem(0);
    renderer->SetNestedTarget(item, target);
    dvc.EditItem(item, source);

    wxTextCtrl* const editor =
        wxDynamicCast(renderer->GetEditorCtrl(), wxTextCtrl);
    REQUIRE( editor );
    CHECK( target->GetRenderer()->GetEditorCtrl() == nullptr );

    editor->ChangeValue("committed source");

    // This finishes the active renderer through wxDataViewMainWindow. For the
    // GetValue hook it is also the exact callback boundary under test.
    wxDataViewColumn* const appended =
        dvc.AppendTextColumn("appended", wxDATAVIEW_CELL_INERT);
    REQUIRE( appended );

    CHECK( renderer->GetNestedAttempts() == 1 );
    CHECK( target->GetRenderer()->GetEditorCtrl() == nullptr );
    CHECK( dvc.GetTextValue(0, 0) == "committed source" );
    CHECK( dvc.GetTextValue(0, 1) == "target value" );
    CHECK( dvc.GetColumnCount() == 3 );
}

TEST_CASE("wxDVC::GenericIconTextFinishUsesEditorSnapshot",
          "[wxDataViewCtrl][editing][renderer][lifetime][reentrant][generic]")
{
    const wxDataViewIconText original(
        "original",
        wxBitmapBundle(wxBitmap(2, 2)));
    wxObjectDataPtr<IconTextFinishModel> model(
        new IconTextFinishModel(original));

    InspectableDataViewCtrl dvc(wxTheApp->GetTopWindow(), wxID_ANY);
    auto* const renderer = new wxDataViewIconTextRenderer(
        wxDataViewIconTextRenderer::GetDefaultType(),
        wxDATAVIEW_CELL_EDITABLE);
    auto* const edited =
        new wxDataViewColumn("icon text", renderer, 0);
    REQUIRE( dvc.AppendColumn(edited) );
    REQUIRE( dvc.AssociateModel(model.get()) );

    const wxDataViewItem item = model->GetItem(0);
    dvc.EditItem(item, edited);
    wxTextCtrl* const editor =
        wxDynamicCast(renderer->GetEditorCtrl(), wxTextCtrl);
    REQUIRE( editor );
    editor->ChangeValue("edited");

    // The historical finish path called GetValue() here. This hook would
    // delete the edited column and its editor from inside that callback.
    model->Arm(&dvc, edited);
    wxDataViewColumn* const appended =
        dvc.AppendTextColumn("finish transaction", 0);
    REQUIRE( appended );

    CHECK( model->GetArmedCalls() == 0 );
    CHECK( dvc.GetColumnCount() == 2 );
    CHECK( model->GetStoredValue().GetText() == "edited" );
    CHECK( model->GetStoredValue().GetBitmapBundle().IsSameAs(
        original.GetBitmapBundle()) );
}

TEST_CASE("wxDVC::GenericEditableQueryMayDestroyControl",
          "[wxDataViewCtrl][editing][keyboard][model][lifetime][reentrant][generic]")
{
    wxObjectDataPtr<DestroyOnEditableQueryModel> model(
        new DestroyOnEditableQueryModel);
    auto* dvc =
        new InspectableDataViewCtrl(wxTheApp->GetTopWindow(), wxID_ANY);
    auto* const editable = new wxDataViewColumn(
        "editable",
        new wxDataViewTextRenderer(
            wxDataViewTextRenderer::GetDefaultType(),
            wxDATAVIEW_CELL_EDITABLE),
        0);
    REQUIRE( dvc->AppendColumn(editable) );
    REQUIRE( dvc->AssociateModel(model.get()) );

    const wxDataViewItem item = model->GetItem(0);
    dvc->Select(item);
    dvc->SetCurrentItem(item);

    wxWindow* const mainWindow = dvc->GetMainWindow();
    const wxWeakRef<wxDataViewCtrl> weakDvc(dvc);
    const wxWeakRef<wxWindow> weakMain(mainWindow);
    model->Arm(dvc);

    wxKeyEvent edit(wxEVT_CHAR);
    edit.m_keyCode = WXK_F2;
    edit.SetEventObject(mainWindow);
    edit.SetId(mainWindow->GetId());
    mainWindow->ProcessWindowEvent(edit);

    CHECK( model->Fired() );
    CHECK( weakDvc.get() == nullptr );
    CHECK( weakMain.get() == nullptr );
}

TEST_CASE("wxDVC::GenericRestartedEditorBlocksColumnMutation",
          "[wxDataViewCtrl][column][editing][reentrant][generic]")
{
    wxDataViewListCtrl dvc(wxTheApp->GetTopWindow(), wxID_ANY);

    auto* const colA = new wxDataViewColumn(
        "A",
        new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_EDITABLE),
        0);
    auto* const colB = new wxDataViewColumn(
        "B",
        new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_EDITABLE),
        1);
    REQUIRE( dvc.AppendColumn(colA, "string") );
    REQUIRE( dvc.AppendColumn(colB, "string") );
    dvc.AppendItem({ wxVariant("A"), wxVariant("B") });

    bool restartEditor = true;
    int editingDoneCount = 0;
    dvc.Bind(
        wxEVT_DATAVIEW_ITEM_EDITING_DONE,
        [&](wxDataViewEvent&)
        {
            ++editingDoneCount;
            if ( restartEditor )
            {
                restartEditor = false;
                dvc.EditItem(dvc.RowToItem(0), colB);
            }
        });

    dvc.EditItem(dvc.RowToItem(0), colA);
    REQUIRE( colA->GetRenderer()->GetEditorCtrl() );

    CHECK_FALSE( dvc.DeleteColumn(colA) );
    CHECK( editingDoneCount == 1 );
    CHECK( dvc.GetColumnCount() == 2 );
    CHECK( dvc.GetColumn(0) == colA );
    CHECK( dvc.GetColumn(1) == colB );
    CHECK( colB->GetRenderer()->GetEditorCtrl() );
}

TEST_CASE("wxDVC::GenericRestartedEditorVetoesHeaderResize",
          "[wxDataViewCtrl][column][editing][header][reentrant][generic]")
{
    wxDataViewListCtrl dvc(wxTheApp->GetTopWindow(), wxID_ANY);

    auto* const colA = new wxDataViewColumn(
        "A",
        new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_EDITABLE),
        0,
        80,
        wxALIGN_LEFT,
        wxDATAVIEW_COL_RESIZABLE);
    auto* const colB = new wxDataViewColumn(
        "B",
        new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_EDITABLE),
        1);
    REQUIRE( dvc.AppendColumn(colA, "string") );
    REQUIRE( dvc.AppendColumn(colB, "string") );
    dvc.AppendItem({ wxVariant("A"), wxVariant("B") });

    bool restartEditor = true;
    int editingDoneCount = 0;
    dvc.Bind(
        wxEVT_DATAVIEW_ITEM_EDITING_DONE,
        [&](wxDataViewEvent&)
        {
            ++editingDoneCount;
            if ( restartEditor )
            {
                restartEditor = false;
                dvc.EditItem(dvc.RowToItem(0), colB);
            }
        });

    dvc.EditItem(dvc.RowToItem(0), colA);
    REQUIRE( colA->GetRenderer()->GetEditorCtrl() );

    wxHeaderCtrl* const header = dvc.GenericGetHeader();
    REQUIRE( header );
    const int widthBefore = colA->GetWidth();

    wxHeaderCtrlEvent resizing(wxEVT_HEADER_RESIZING, header->GetId());
    resizing.SetEventObject(header);
    resizing.SetColumn(0);
    resizing.SetWidth(widthBefore + 37);
    header->ProcessWindowEvent(resizing);

    CHECK_FALSE( resizing.IsAllowed() );
    CHECK( editingDoneCount == 1 );
    CHECK( colA->GetWidth() == widthBefore );
    CHECK( colB->GetRenderer()->GetEditorCtrl() );
}

TEST_CASE("wxDVC::GenericReentrantModelAssociationBuild",
          "[wxDataViewCtrl][model][tree][reentrant][generic]")
{
    wxObjectDataPtr<TransactionalTreeModel> modelA(
        new TransactionalTreeModel);
    wxObjectDataPtr<TransactionalTreeModel> modelB(
        new TransactionalTreeModel(true));

    InspectableDataViewCtrl dvc(wxTheApp->GetTopWindow(), wxID_ANY);
    REQUIRE( dvc.AppendColumn(NewTestDataViewColumn("value", 0)) );

    modelA->SetHook(
        TransactionalTreeModel::Hook::AssociateOnRootChildren,
        &dvc,
        modelB.get());
    REQUIRE( dvc.AssociateModel(modelA.get()) );

    CHECK( modelA->GetHookCount() == 1 );
    CHECK( dvc.GetModel() == modelB.get() );
    CHECK( dvc.GetRowByItem(modelB->Container()) == 0 );
    CHECK( dvc.GetRowByItem(modelA->Container()) == wxNOT_FOUND );
}

TEST_CASE("wxDVC::GenericNotifierBatchStopsAfterReassociation",
          "[wxDataViewCtrl][model][notifier][reentrant][generic]")
{
    wxObjectDataPtr<TransactionalTreeModel> modelA(
        new TransactionalTreeModel(true));
    wxObjectDataPtr<TransactionalTreeModel> modelB(
        new TransactionalTreeModel(true));

    InspectableDataViewCtrl dvc(wxTheApp->GetTopWindow(), wxID_ANY);
    REQUIRE( dvc.AppendColumn(NewTestDataViewColumn("value", 0)) );
    REQUIRE( dvc.AssociateModel(modelA.get()) );

    int changedEvents = 0;
    dvc.Bind(
        wxEVT_DATAVIEW_ITEM_VALUE_CHANGED,
        [&](wxDataViewEvent&)
        {
            ++changedEvents;
            if ( changedEvents == 1 )
                CHECK( dvc.AssociateModel(modelB.get()) );
        });

    wxDataViewItemArray changed;
    changed.push_back(modelA->Container());
    changed.push_back(modelA->Two());
    CHECK( modelA->ItemsChanged(changed) );

    CHECK( changedEvents == 1 );
    CHECK( dvc.GetModel() == modelB.get() );
    CHECK( dvc.GetRowByItem(modelB->Container()) == 0 );
}

TEST_CASE("wxDVC::GenericGetParentReassociation",
          "[wxDataViewCtrl][model][tree][reentrant][generic]")
{
    bool useExpand = false;
    SECTION("row lookup")
    {
    }
    SECTION("ExpandAncestors")
    {
        useExpand = true;
    }

    wxObjectDataPtr<TransactionalTreeModel> modelA(
        new TransactionalTreeModel(true));
    wxObjectDataPtr<TransactionalTreeModel> modelB(
        new TransactionalTreeModel(true));

    InspectableDataViewCtrl dvc(wxTheApp->GetTopWindow(), wxID_ANY);
    REQUIRE( dvc.AppendColumn(NewTestDataViewColumn("value", 0)) );
    REQUIRE( dvc.AssociateModel(modelA.get()) );

    modelA->SetHook(
        TransactionalTreeModel::Hook::AssociateOnParent,
        &dvc,
        modelB.get());
    if ( useExpand )
        dvc.Expand(modelA->ChildHigh());
    else
        CHECK( dvc.GetRowByItem(modelA->ChildHigh()) == wxNOT_FOUND );

    CHECK( modelA->GetHookCount() == 1 );
    CHECK( dvc.GetModel() == modelB.get() );
    CHECK( dvc.GetRowByItem(modelB->Container()) == 0 );
}

TEST_CASE("wxDVC::GenericClearedReassociation",
          "[wxDataViewCtrl][model][tree][reentrant][generic]")
{
    wxObjectDataPtr<TransactionalTreeModel> modelA(
        new TransactionalTreeModel(true));
    wxObjectDataPtr<TransactionalTreeModel> modelB(
        new TransactionalTreeModel(true));

    InspectableDataViewCtrl dvc(wxTheApp->GetTopWindow(), wxID_ANY);
    REQUIRE( dvc.AppendColumn(NewTestDataViewColumn("value", 0)) );
    REQUIRE( dvc.AssociateModel(modelA.get()) );

    modelA->SetHook(
        TransactionalTreeModel::Hook::AssociateOnClearBuild,
        &dvc,
        modelB.get());
    CHECK_FALSE( modelA->Cleared() );

    CHECK( modelA->GetHookCount() == 1 );
    CHECK( dvc.GetModel() == modelB.get() );
    CHECK( dvc.GetRowByItem(modelB->Container()) == 0 );
}

TEST_CASE("wxDVC::GenericResortIsAtomicAcrossBranches",
          "[wxDataViewCtrl][model][tree][sort][reentrant][generic]")
{
    wxObjectDataPtr<TransactionalTreeModel> model(
        new TransactionalTreeModel(true));

    InspectableDataViewCtrl dvc(wxTheApp->GetTopWindow(), wxID_ANY);
    wxDataViewColumn* const primary =
        NewTestDataViewColumn("primary", 0);
    wxDataViewColumn* const secondary =
        NewTestDataViewColumn("secondary", 1);
    REQUIRE( dvc.AppendColumn(primary) );
    REQUIRE( dvc.AppendColumn(secondary) );
    REQUIRE( dvc.AssociateModel(model.get()) );

    dvc.Expand(model->Container());
    REQUIRE( dvc.IsExpanded(model->Container()) );
    REQUIRE( dvc.AllowMultiColumnSort(true) );
    primary->SetSortOrder(true);
    secondary->SetSortOrder(true);

    model->SetHook(
        TransactionalTreeModel::Hook::ToggleSecondaryOnNestedCompare,
        &dvc,
        nullptr,
        secondary);
    model->Resort();

    CHECK( model->GetHookCount() == 1 );
    CHECK_FALSE( secondary->IsSortOrderAscending() );

    // The root permutation was planned before the nested comparison aborted.
    // It must not have been published independently of the child branch.
    CHECK( dvc.GetItemByRow(0) == model->Container() );
    CHECK( dvc.GetItemByRow(1) == model->ChildHigh() );
    CHECK( dvc.GetItemByRow(2) == model->ChildLow() );
    CHECK( dvc.GetItemByRow(3) == model->Two() );
    CHECK( dvc.GetItemByRow(4) == model->One() );
}

TEST_CASE("wxDVC::GenericItemAddedTracksSortTransition",
          "[wxDataViewCtrl][model][tree][sort][reentrant][generic]")
{
    wxObjectDataPtr<TransactionalTreeModel> model(
        new TransactionalTreeModel);

    InspectableDataViewCtrl dvc(wxTheApp->GetTopWindow(), wxID_ANY);
    wxDataViewColumn* const column =
        NewTestDataViewColumn("value", 0);
    REQUIRE( dvc.AppendColumn(column) );
    REQUIRE( dvc.AssociateModel(model.get()) );
    dvc.Expand(model->Container());
    REQUIRE( dvc.IsExpanded(model->Container()) );

    column->SetSortOrder(true);
    model->Resort();
    REQUIRE( dvc.GetItemByRow(1) == model->ChildLow() );
    REQUIRE( dvc.GetItemByRow(2) == model->ChildHigh() );

    model->SetHook(
        TransactionalTreeModel::Hook::UnsetSortOnParent,
        &dvc,
        nullptr,
        column);
    REQUIRE( model->AppendVisibleChild() );

    CHECK_FALSE( column->IsSortKey() );
    CHECK( model->GetHookCount() == 1 );
    CHECK( dvc.GetItemByRow(3) == model->AddedChild() );
}

TEST_CASE("wxDVC::GenericSortBuildIsNLogN",
          "[wxDataViewCtrl][model][tree][sort][performance][generic]")
{
    constexpr size_t itemCount = 4096;
    wxObjectDataPtr<CountingFlatDataViewModel> model(
        new CountingFlatDataViewModel(itemCount));

    InspectableDataViewCtrl dvc(wxTheApp->GetTopWindow(), wxID_ANY);
    wxDataViewColumn* const column =
        NewTestDataViewColumn("value", 0);
    REQUIRE( dvc.AppendColumn(column) );
    column->SetSortOrder(true);

    REQUIRE( dvc.AssociateModel(model.get()) );
    CHECK( dvc.GetItemByRow(itemCount - 1).IsOk() );
    CHECK_FALSE( dvc.GetItemByRow(itemCount).IsOk() );
    CHECK( model->GetCompareCount() <
           static_cast<int>(itemCount * 20) );
}

TEST_CASE("wxDVC::GenericAutosizeDoesNotPublishPartialWidth",
          "[wxDataViewCtrl][column][autosize][reentrant][generic]")
{
    wxObjectDataPtr<TransactionalTreeModel> model(
        new TransactionalTreeModel(true));

    InspectableDataViewCtrl dvc(wxTheApp->GetTopWindow(), wxID_ANY);
    wxDataViewColumn* const column =
        NewTestDataViewColumn("wide header", 0);
    column->SetWidth(240);
    REQUIRE( dvc.AppendColumn(column) );
    REQUIRE( dvc.AssociateModel(model.get()) );

    model->SetHook(
        TransactionalTreeModel::Hook::NotifyValueChangedOnGetValue);
    unsigned int measured = 0;
    CHECK_FALSE( dvc.TryGetBestColumnWidth(0, &measured) );
    CHECK( column->GetWidth() == 240 );

    model->SetHook(TransactionalTreeModel::Hook::None);
    CHECK( dvc.TryGetBestColumnWidth(0, &measured) );
    CHECK( measured > 0 );
}

TEST_CASE("wxDVC::GenericRendererSurvivesColumnDeletionCallback",
          "[wxDataViewCtrl][column][renderer][lifetime][reentrant][generic]")
{
    wxObjectDataPtr<TransactionalTreeModel> model(
        new TransactionalTreeModel(true));

    InspectableDataViewCtrl dvc(wxTheApp->GetTopWindow(), wxID_ANY);
    wxDataViewColumn* const column =
        NewTestDataViewColumn("delete during value", 0);
    REQUIRE( dvc.AppendColumn(column) );
    REQUIRE( dvc.AssociateModel(model.get()) );

    model->SetHook(
        TransactionalTreeModel::Hook::DeleteColumnOnGetValue,
        &dvc,
        nullptr,
        column);
    unsigned int measured = 0;
    CHECK_FALSE( dvc.TryGetBestColumnWidth(0, &measured) );
    CHECK( dvc.GetColumnCount() == 0 );
}

TEST_CASE("wxDVC::GenericHiddenItemDoesNotAliasVisibleRow",
          "[wxDataViewCtrl][model][tree][expand][generic]")
{
    wxObjectDataPtr<TransactionalTreeModel> model(
        new TransactionalTreeModel(true));

    wxDataViewCtrl dvc(wxTheApp->GetTopWindow(), wxID_ANY);
    REQUIRE( dvc.AppendColumn(NewTestDataViewColumn("value", 0)) );
    REQUIRE( dvc.AssociateModel(model.get()) );

    dvc.Expand(model->Container());
    dvc.Expand(model->Two());
    REQUIRE( dvc.IsExpanded(model->Container()) );
    REQUIRE( dvc.IsExpanded(model->Two()) );

    dvc.Collapse(model->Container());
    REQUIRE_FALSE( dvc.IsExpanded(model->Container()) );
    REQUIRE( dvc.IsExpanded(model->Two()) );

    CHECK_FALSE( dvc.IsExpanded(model->ChildHigh()) );
    dvc.Collapse(model->ChildHigh());
    CHECK( dvc.IsExpanded(model->Two()) );
}

TEST_CASE("wxDVC::GenericHiddenMutationKeepsVisibleSelection",
          "[wxDataViewCtrl][model][tree][selection][generic]")
{
    wxObjectDataPtr<TransactionalTreeModel> model(
        new TransactionalTreeModel(true));

    wxDataViewCtrl dvc(wxTheApp->GetTopWindow(), wxID_ANY);
    REQUIRE( dvc.AppendColumn(NewTestDataViewColumn("value", 0)) );
    REQUIRE( dvc.AssociateModel(model.get()) );

    dvc.Expand(model->Container());
    REQUIRE( dvc.IsExpanded(model->Container()) );
    dvc.Collapse(model->Container());
    REQUIRE_FALSE( dvc.IsExpanded(model->Container()) );

    dvc.Select(model->Two());
    REQUIRE( dvc.GetSelection() == model->Two() );

    REQUIRE( model->AddHiddenChild() );
    CHECK( dvc.GetSelection() == model->Two() );

    REQUIRE( model->DeleteHiddenChild() );
    CHECK( dvc.GetSelection() == model->Two() );
}

TEST_CASE("wxDVC::GenericListClearUsesPostEditStore",
          "[wxDataViewCtrl][column][editing][model][reentrant][generic]")
{
    wxDataViewListCtrl dvc(wxTheApp->GetTopWindow(), wxID_ANY);
    auto* const editable = new wxDataViewColumn(
        "editable",
        new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_EDITABLE),
        0);
    REQUIRE( dvc.AppendColumn(editable, "string") );
    dvc.AppendItem({ wxVariant("old store") });

    wxDataViewListStore* const oldStore = dvc.GetStore();
    oldStore->IncRef();
    wxObjectDataPtr<wxDataViewListStore> pinnedOldStore(oldStore);

    wxObjectDataPtr<wxDataViewListStore> newStore(
        new wxDataViewListStore);
    newStore->AppendColumn("string");
    newStore->AppendItem({ wxVariant("new store") });

    dvc.Bind(
        wxEVT_DATAVIEW_ITEM_EDITING_DONE,
        [&](wxDataViewEvent&)
        {
            CHECK( dvc.AssociateModel(newStore.get()) );
        });

    dvc.EditItem(dvc.RowToItem(0), editable);
    REQUIRE( editable->GetRenderer()->GetEditorCtrl() );
    REQUIRE( dvc.ClearColumns() );

    CHECK( dvc.GetModel() == newStore.get() );
    CHECK( dvc.GetColumnCount() == 0 );
    CHECK( newStore->m_cols.empty() );
    CHECK( pinnedOldStore->m_cols.size() == 1 );
}

TEST_CASE("wxDVC::GenericColumnsWithoutHeader",
          "[wxDataViewCtrl][column][generic]")
{
    wxDataViewListCtrl dvc(
        wxTheApp->GetTopWindow(),
        wxID_ANY,
        wxDefaultPosition,
        wxDefaultSize,
        wxDV_ROW_LINES | wxDV_NO_HEADER);

    CHECK( dvc.GenericGetHeader() == nullptr );

    wxDataViewColumn* const colA = NewTestDataViewColumn("A", 0);
    wxDataViewColumn* const colB = NewTestDataViewColumn("B", 1);
    wxDataViewColumn* const colC = NewTestDataViewColumn("C", 2);
    wxDataViewColumn* const colP = NewTestDataViewColumn("P", 3);

    REQUIRE( dvc.AppendColumn(colA) );
    REQUIRE( dvc.AppendColumn(colC) );
    REQUIRE( dvc.InsertColumn(1, colB) );
    REQUIRE( dvc.PrependColumn(colP) );

    CheckDataViewColumns(
        &dvc,
        "no-header insertion",
        { colP, colA, colB, colC },
        { colP, colA, colB, colC });

    REQUIRE( dvc.DeleteColumn(colB) );
    CheckDataViewColumns(
        &dvc,
        "no-header deletion",
        { colP, colA, colC },
        { colP, colA, colC });
}

#if wxUSE_ACCESSIBILITY

TEST_CASE("wxDVC::GenericAccessibleReadsAreTransactional",
          "[wxDataViewCtrl][accessibility][model][column][lifetime][reentrant][generic]")
{
    SECTION("column deletion aborts name atomically")
    {
        wxObjectDataPtr<TransactionalTreeModel> model(
            new TransactionalTreeModel(true));
        InspectableDataViewCtrl dvc(
            wxTheApp->GetTopWindow(), wxID_ANY);
        wxDataViewColumn* const column =
            NewTestDataViewColumn("value", 0);
        REQUIRE( dvc.AppendColumn(column) );
        REQUIRE( dvc.AssociateModel(model.get()) );

        wxAccessible* const accessible =
            dvc.GetOrCreateAccessible();
        REQUIRE( accessible );

        model->SetHook(
            TransactionalTreeModel::Hook::DeleteColumnOnGetValue,
            &dvc,
            nullptr,
            column);
        wxString name = "unchanged";
        CHECK( accessible->GetName(1, &name) == wxACC_FAIL );
        CHECK( name == "unchanged" );
        CHECK( dvc.GetColumnCount() == 0 );
    }

    SECTION("description uses model-column identities")
    {
        wxObjectDataPtr<TransactionalTreeModel> model(
            new TransactionalTreeModel(true));
        InspectableDataViewCtrl dvc(
            wxTheApp->GetTopWindow(), wxID_ANY);
        REQUIRE( dvc.AppendColumn(
            NewTestDataViewColumn("model one", 1)) );
        REQUIRE( dvc.AppendColumn(
            NewTestDataViewColumn("model zero", 0)) );
        REQUIRE( dvc.AssociateModel(model.get()) );

        wxAccessible* const accessible =
            dvc.GetOrCreateAccessible();
        REQUIRE( accessible );
        model->ClearHasValueColumns();

        wxString description;
        CHECK( accessible->GetDescription(1, &description) ==
               wxACC_OK );
        const std::vector<unsigned int>& columns =
            model->GetHasValueColumns();
        REQUIRE( columns.size() >= 2 );
        const auto modelOne =
            std::find(columns.begin(), columns.end(), 1u);
        const auto modelZero =
            std::find(columns.begin(), columns.end(), 0u);
        REQUIRE( modelOne != columns.end() );
        REQUIRE( modelZero != columns.end() );
        CHECK( modelOne < modelZero );
    }
}

TEST_CASE("wxDVC::GenericAccessibleFocusAndReverseSelection",
          "[wxDataViewCtrl][accessibility][focus][selection][generic]")
{
    wxDataViewListCtrl dvc(
        wxTheApp->GetTopWindow(),
        wxID_ANY,
        wxDefaultPosition,
        wxSize(300, 160),
        wxDV_MULTIPLE);
    REQUIRE( dvc.AppendTextColumn("value") );
    dvc.AppendItem({ wxVariant("zero") });
    dvc.AppendItem({ wxVariant("one") });
    dvc.AppendItem({ wxVariant("two") });
    dvc.SetCurrentItem(dvc.RowToItem(2));

    wxTextCtrl focusSink(
        wxTheApp->GetTopWindow(), wxID_ANY, "focus sink");
    focusSink.SetFocus();
    wxYield();
    REQUIRE_FALSE( dvc.GetMainWindow()->HasFocus() );

    wxAccessible* const accessible = dvc.GetOrCreateAccessible();
    REQUIRE( accessible );

    int focusedId = 12345;
    wxAccessible* focusedObject =
        reinterpret_cast<wxAccessible*>(static_cast<uintptr_t>(1));
    CHECK( accessible->GetFocus(&focusedId, &focusedObject) ==
           wxACC_OK );
    CHECK( focusedId == 0 );
    CHECK( focusedObject == nullptr );

    long state = 0;
    CHECK( accessible->GetState(3, &state) == wxACC_OK );
    CHECK( (state & wxACC_STATE_SYSTEM_FOCUSED) == 0 );

    const wxAccSelectionFlags addReverse =
        static_cast<wxAccSelectionFlags>(
            wxACC_SEL_EXTENDSELECTION |
            wxACC_SEL_ADDSELECTION);
    CHECK( accessible->Select(1, addReverse) == wxACC_OK );
    wxDataViewItemArray selected;
    CHECK( dvc.GetSelections(selected) == 3 );

    const wxAccSelectionFlags removeReverse =
        static_cast<wxAccSelectionFlags>(
            wxACC_SEL_EXTENDSELECTION |
            wxACC_SEL_REMOVESELECTION);
    CHECK( accessible->Select(1, removeReverse) == wxACC_OK );
    selected.clear();
    CHECK( dvc.GetSelections(selected) == 0 );
}

#endif // wxUSE_ACCESSIBILITY

#endif // wxHAS_GENERIC_DATAVIEWCTRL

#if wxUSE_UIACTIONSIMULATOR

TEST_CASE_METHOD(SingleSelectDataViewCtrlTestCase,
                 "wxDVC::KeyEvents",
                 "[wxDataViewCtrl][event]")
{
    if ( !EnableUITests() )
        return;

    EventCounter keyEvents(m_dvc.get(), wxEVT_KEY_DOWN);

    m_dvc->SetFocus();
    wxYield();

    wxUIActionSimulator sim;
    sim.Char(WXK_DOWN);
    wxYield();

    CHECK( keyEvents.GetCount() == 1 );
}

#endif // wxUSE_UIACTIONSIMULATOR

#endif //wxUSE_DATAVIEWCTRL
