///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/listctrltest.cpp
// Purpose:     wxListCtrl unit test
// Author:      Vadim Zeitlin
// Created:     2008-11-26
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
//              (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#if wxUSE_LISTCTRL


#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/listctrl.h"
#include "wx/artprov.h"
#include "wx/imaglist.h"
#include "wx/weakref.h"
#include "listbasetest.h"
#include "testableframe.h"
#include "wx/uiaction.h"
#if wxUSE_TEXTCTRL
#include "wx/textctrl.h"
#endif
#include <algorithm>
#include <functional>
#include <memory>
#include <utility>

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class ListCtrlTestCase : public ListBaseTestCase
{
public:
    ListCtrlTestCase();
    virtual ~ListCtrlTestCase() override;

    virtual wxListCtrl *GetList() const override { return m_list; }

protected:
    wxListCtrl *m_list;

    wxDECLARE_NO_COPY_CLASS(ListCtrlTestCase);
};

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

ListCtrlTestCase::ListCtrlTestCase()
{
    m_list = new wxListCtrl(wxTheApp->GetTopWindow());
    m_list->SetWindowStyle(wxLC_REPORT | wxLC_EDIT_LABELS);
    m_list->SetSize(400, 200);

    wxTheApp->GetTopWindow()->Raise();
}

ListCtrlTestCase::~ListCtrlTestCase()
{
    DeleteTestWindow(m_list);
}

wxLIST_BASE_TESTS(ListCtrl, "[listctrl]")

#ifdef __WXWINUI__

namespace
{

int wxCALLBACK CountListComparisons(wxIntPtr item1,
                                    wxIntPtr item2,
                                    wxIntPtr sortData)
{
    int* const count = reinterpret_cast<int*>(sortData);
    ++*count;
    return item1 < item2 ? -1 : item1 > item2 ? 1 : 0;
}

enum class ReentrantSortAction
{
    Destroy,
    DeleteItem
};

struct ReentrantSortState
{
    wxListCtrl* list;
    ReentrantSortAction action;
    int calls = 0;
    bool nestedResult = false;
};

int wxCALLBACK ReentrantListCompare(wxIntPtr item1,
                                    wxIntPtr item2,
                                    wxIntPtr sortData)
{
    ReentrantSortState* const state =
        reinterpret_cast<ReentrantSortState*>(sortData);
    if ( state->calls++ == 0 )
    {
        if ( state->action == ReentrantSortAction::Destroy )
        {
            delete state->list;
        }
        else
        {
            state->nestedResult = state->list->DeleteItem(0);
        }
    }

    return item1 < item2 ? -1 : item1 > item2 ? 1 : 0;
}

void PopulateReentrantList(wxListCtrl* const list)
{
    list->InsertColumn(0, "Column 0");
    for ( long i = 0; i < 3; ++i )
    {
        list->InsertItem(i, wxString::Format("Item %ld", i));
        list->SetItemData(i, static_cast<wxUIntPtr>(i));
    }
}

} // anonymous namespace

TEST_CASE_METHOD(ListCtrlTestCase,
                 "ListCtrl::SetItemDestruction",
                 "[listctrl][winui-listctrl-lifetime]")
{
    PopulateReentrantList(m_list);

    wxListCtrl* const list = m_list;
    const wxWeakRef<wxWindow> lifetime(list);
    m_list = nullptr;

    list->Bind(
        wxEVT_LIST_ITEM_SELECTED,
        [list](wxListEvent&)
        {
            delete list;
        });

    wxListItem item;
    item.SetId(1);
    item.SetMask(wxLIST_MASK_STATE);
    item.SetState(wxLIST_STATE_SELECTED);

    const bool result = list->SetItem(item);
    if ( lifetime )
        delete list;

    CHECK_FALSE(result);
    CHECK_FALSE(lifetime);
}

TEST_CASE_METHOD(ListCtrlTestCase,
                 "ListCtrl::SortItemsDestruction",
                 "[listctrl][winui-listctrl-lifetime]")
{
    PopulateReentrantList(m_list);
    REQUIRE(m_list->SetItemState(
        0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED));
    REQUIRE(m_list->SetItemState(
        1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED));

    wxListCtrl* const list = m_list;
    const wxWeakRef<wxWindow> lifetime(list);
    m_list = nullptr;
    int comparisons = 0;

    list->Bind(
        wxEVT_LIST_ITEM_DESELECTED,
        [list](wxListEvent&)
        {
            delete list;
        });

    const bool result = list->SortItems(
        CountListComparisons, reinterpret_cast<wxIntPtr>(&comparisons));
    if ( lifetime )
        delete list;

    CHECK_FALSE(result);
    CHECK_FALSE(lifetime);
    CHECK(comparisons == 0);
}

TEST_CASE_METHOD(ListCtrlTestCase,
                 "ListCtrl::SortItemsTopologyReentry",
                 "[listctrl][winui-listctrl-lifetime]")
{
    PopulateReentrantList(m_list);
    REQUIRE(m_list->SetItemState(
        0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED));
    REQUIRE(m_list->SetItemState(
        1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED));

    bool deleted = false;
    m_list->Bind(
        wxEVT_LIST_ITEM_DESELECTED,
        [this, &deleted](wxListEvent& event)
        {
            event.Skip();
            if ( !deleted )
            {
                deleted = true;
                CHECK(m_list->DeleteItem(0));
            }
        });

    int comparisons = 0;
    const bool result = m_list->SortItems(
        CountListComparisons, reinterpret_cast<wxIntPtr>(&comparisons));

    CHECK(deleted);
    CHECK_FALSE(result);
    CHECK(comparisons == 0);
    CHECK(m_list->GetItemCount() == 2);
}

TEST_CASE_METHOD(ListCtrlTestCase,
                 "ListCtrl::SortItemsResetCurrentDestruction",
                 "[listctrl][winui-listctrl-lifetime]")
{
    PopulateReentrantList(m_list);
    REQUIRE(m_list->SetItemState(
        1, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED));

    wxListCtrl* const list = m_list;
    const wxWeakRef<wxWindow> lifetime(list);
    m_list = nullptr;
    int comparisons = 0;

    list->Bind(
        wxEVT_LIST_ITEM_FOCUSED,
        [list](wxListEvent&)
        {
            delete list;
        });

    const bool result = list->SortItems(
        CountListComparisons, reinterpret_cast<wxIntPtr>(&comparisons));
    if ( lifetime )
        delete list;

    CHECK_FALSE(result);
    CHECK_FALSE(lifetime);
    CHECK(comparisons == 0);
}

TEST_CASE_METHOD(ListCtrlTestCase,
                 "ListCtrl::SortComparatorDestruction",
                 "[listctrl][winui-listctrl-lifetime]")
{
    PopulateReentrantList(m_list);

    wxListCtrl* const list = m_list;
    const wxWeakRef<wxWindow> lifetime(list);
    m_list = nullptr;
    ReentrantSortState state{
        list, ReentrantSortAction::Destroy
    };

    const bool result = list->SortItems(
        ReentrantListCompare, reinterpret_cast<wxIntPtr>(&state));
    if ( lifetime )
        delete list;

    CHECK_FALSE(result);
    CHECK_FALSE(lifetime);
    CHECK(state.calls == 1);
}

TEST_CASE_METHOD(ListCtrlTestCase,
                 "ListCtrl::SortComparatorTopologyReentry",
                 "[listctrl][winui-listctrl-lifetime]")
{
    PopulateReentrantList(m_list);
    ReentrantSortState state{
        m_list, ReentrantSortAction::DeleteItem
    };

    const bool result = m_list->SortItems(
        ReentrantListCompare, reinterpret_cast<wxIntPtr>(&state));

    CHECK_FALSE(result);
    CHECK(state.calls == 1);
    CHECK(state.nestedResult);
    CHECK(m_list->GetItemCount() == 2);
}

TEST_CASE_METHOD(ListCtrlTestCase,
                 "ListCtrl::SortPermutationSuccess",
                 "[listctrl][winui-listctrl-lifetime]")
{
    m_list->InsertColumn(0, "Column 0");

    static const wxUIntPtr keys[] = {2, 1, 2, 0, 1};
    static const wxColour colours[] =
    {
        wxColour(10, 20, 30),
        wxColour(20, 30, 40),
        wxColour(30, 40, 50),
        wxColour(40, 50, 60),
        wxColour(50, 60, 70)
    };
    for ( long i = 0; i < 5; ++i )
    {
        m_list->InsertItem(i, wxString::Format("unique-%ld", i));
        m_list->SetItemData(i, keys[i]);
        m_list->SetItemTextColour(i, colours[i]);
    }

    int comparisons = 0;
    REQUIRE(m_list->SortItems(
        CountListComparisons, reinterpret_cast<wxIntPtr>(&comparisons)));

    static const long expectedOriginalIndices[] = {3, 1, 4, 0, 2};
    REQUIRE(comparisons > 0);
    for ( long i = 0; i < 5; ++i )
    {
        const long original = expectedOriginalIndices[i];
        CHECK(m_list->GetItemText(i) ==
              wxString::Format("unique-%ld", original));
        CHECK(m_list->GetItemData(i) == keys[original]);
        CHECK(m_list->GetItemTextColour(i) == colours[original]);
    }
}

#endif // __WXWINUI__

// Note that wxLIST_BASE_TESTS() already defines "ListCtrl::EditLabel" test.
TEST_CASE_METHOD(ListCtrlTestCase, "ListCtrl::CallEditLabel", "[listctrl]")
{
    EventCounter editItem(m_list, wxEVT_LIST_BEGIN_LABEL_EDIT);
    EventCounter endEditItem(m_list, wxEVT_LIST_END_LABEL_EDIT);

    m_list->InsertColumn(0, "Column 0");
    m_list->InsertItem(0, "foo");
    m_list->EditLabel(0);

    m_list->EndEditLabel(true);

    CHECK(editItem.GetCount() == 1);
    CHECK(endEditItem.GetCount() == 1);
}

TEST_CASE_METHOD(ListCtrlTestCase, "ListCtrl::SubitemRect", "[listctrl]")
{
    wxBitmap bmp = wxArtProvider::GetBitmap(wxART_ERROR);

    wxImageList* const iml = new wxImageList(bmp.GetWidth(), bmp.GetHeight());
    iml->Add(bmp);
    m_list->AssignImageList(iml, wxIMAGE_LIST_SMALL);

    m_list->InsertColumn(0, "Column 0");
    m_list->InsertColumn(1, "Column 1");
    m_list->InsertColumn(2, "Column 2");
    for ( int i = 0; i < 3; i++ )
    {
        long index = m_list->InsertItem(i, wxString::Format("This is item %d", i), 0);
        m_list->SetItem(index, 1, wxString::Format("Column 1 item %d", i));
        m_list->SetItem(index, 2, wxString::Format("Column 2 item %d", i));
    }

    wxRect rectLabel, rectIcon, rectItem;

    // First check a subitem with an icon: it should have a valid icon
    // rectangle and the label rectangle should be adjacent to it.
    m_list->GetSubItemRect(1, 0, rectItem, wxLIST_RECT_BOUNDS);
    m_list->GetSubItemRect(1, 0, rectIcon, wxLIST_RECT_ICON);
    m_list->GetSubItemRect(1, 0, rectLabel, wxLIST_RECT_LABEL);

    CHECK(!rectIcon.IsEmpty());
    // Note that we can't use "==" here, in the native MSW version there is a
    // gap between the item rectangle and the icon one.
    CHECK(rectIcon.GetLeft() >= rectItem.GetLeft());
    CHECK(rectLabel.GetLeft() == rectIcon.GetRight() + 1);
    CHECK(rectLabel.GetRight() == rectItem.GetRight());

    // For a subitem without an icon, label rectangle is the same one as the
    // entire item one and the icon rectangle should be empty.
    m_list->GetSubItemRect(1, 1, rectItem, wxLIST_RECT_BOUNDS);
    m_list->GetSubItemRect(1, 1, rectIcon, wxLIST_RECT_ICON);
    m_list->GetSubItemRect(1, 1, rectLabel, wxLIST_RECT_LABEL);

    CHECK(rectIcon.IsEmpty());
    // Here we can't check for exact equality either as there can be a margin.
    CHECK(rectLabel.GetLeft() >= rectItem.GetLeft());
    CHECK(rectLabel.GetRight() == rectItem.GetRight());
}

TEST_CASE_METHOD(ListCtrlTestCase, "ListCtrl::ColumnCount", "[listctrl]")
{
    CHECK(m_list->GetColumnCount() == 0);
    m_list->InsertColumn(0, "Column 0");
    m_list->InsertColumn(1, "Column 1");
    CHECK(m_list->GetColumnCount() == 2);

    // Recreate the control in other modes to check the count there as well.
    delete m_list;
    m_list = new wxListCtrl(wxTheApp->GetTopWindow(), wxID_ANY,
                            wxDefaultPosition, wxDefaultSize,
                            wxLC_LIST);
    CHECK(m_list->GetColumnCount() == 1);

    delete m_list;
    m_list = new wxListCtrl(wxTheApp->GetTopWindow(), wxID_ANY,
                            wxDefaultPosition, wxDefaultSize,
                            wxLC_ICON);
    CHECK(m_list->GetColumnCount() == 0);

    delete m_list;
    m_list = new wxListCtrl(wxTheApp->GetTopWindow(), wxID_ANY,
                            wxDefaultPosition, wxDefaultSize,
                            wxLC_SMALL_ICON);
    CHECK(m_list->GetColumnCount() == 0);
}

#if wxUSE_UIACTIONSIMULATOR

TEST_CASE_METHOD(ListCtrlTestCase, "ListCtrl::ColumnDrag", "[listctrl]")
{
    if ( !EnableUITests() )
        return;

    EventCounter begindrag(m_list, wxEVT_LIST_COL_BEGIN_DRAG);
    EventCounter dragging(m_list, wxEVT_LIST_COL_DRAGGING);
    EventCounter enddrag(m_list, wxEVT_LIST_COL_END_DRAG);

    m_list->InsertColumn(0, "Column 0");
    m_list->InsertColumn(1, "Column 1");
    m_list->InsertColumn(2, "Column 2");
    m_list->Update();
    m_list->SetFocus();

    wxUIActionSimulator sim;

    wxPoint pt = m_list->ClientToScreen(wxPoint(m_list->GetColumnWidth(0), 5));

    sim.MouseMove(pt);
    wxYield();

    sim.MouseDown();
    wxYield();

    sim.MouseMove(pt.x + 50, pt.y);
    wxYield();

    sim.MouseUp();
    wxYield();

    CHECK( begindrag.GetCount() == 1 );
    CHECK( dragging.GetCount() > 0 );
    CHECK( enddrag.GetCount() == 1 );

    m_list->ClearAll();
}

TEST_CASE_METHOD(ListCtrlTestCase, "ListCtrl::ColumnClick", "[listctrl]")
{
    if ( !EnableUITests() )
        return;

    EventCounter colclick(m_list, wxEVT_LIST_COL_CLICK);
    EventCounter colrclick(m_list, wxEVT_LIST_COL_RIGHT_CLICK);


    m_list->InsertColumn(0, "Column 0", wxLIST_FORMAT_LEFT, 60);

    wxUIActionSimulator sim;

    sim.MouseMove(m_list->ClientToScreen(wxPoint(4, 4)));
    wxYield();

    sim.MouseClick();
    sim.MouseClick(wxMOUSE_BTN_RIGHT);
    wxYield();

    CHECK( colclick.GetCount() == 1 );
    CHECK( colrclick.GetCount() == 1 );

    m_list->ClearAll();
}
#endif // wxUSE_UIACTIONSIMULATOR

#ifdef _WX_GENERIC_LISTCTRL_H_
namespace
{
class GenericOrderList final : public wxGenericListCtrl
{
public:
    explicit GenericOrderList(bool virtualList)
        : wxGenericListCtrl(wxTheApp->GetTopWindow(), wxID_ANY,
                            wxDefaultPosition, wxSize(600, 240),
                            wxLC_REPORT | (virtualList ? wxLC_VIRTUAL : 0))
    {
    }

    static wxString Value(long row, long column)
    {
        return wxString::Format("row %ld column %ld", row, column);
    }

protected:
    wxString OnGetItemText(long row, long column) const override
    {
        return Value(row, column);
    }
};
}

TEST_CASE("GenericListCtrl::ColumnOrderPreservesModelAndHitGeometry",
          "[listctrl][generic-listctrl][column-order]")
{
    const bool virtualList = GENERATE(false, true);
    CAPTURE(virtualList);
    // Exercise the generic model selected by wx/listctrl.h. Native wxMSW and
    // wxQt don't build this class; their public API is tested above.
    auto list = std::make_unique<GenericOrderList>(virtualList);
    const int widths[] = {130, 150, 170};
    for ( int column = 0; column < 3; ++column )
        REQUIRE(list->InsertColumn(column, wxString::Format("Column %d", column),
                                   wxLIST_FORMAT_LEFT, widths[column]) == column);
    if ( virtualList )
    {
        list->SetItemCount(20);
    }
    else
    {
        for ( long row = 0; row < 20; ++row )
        {
            REQUIRE(list->InsertItem(row, GenericOrderList::Value(row, 0)) == row);
            for ( int column = 1; column < 3; ++column )
                REQUIRE(list->SetItem(row, column, GenericOrderList::Value(row, column)));
        }
    }

    int permutation[] = {0, 1, 2};
    do
    {
        wxArrayInt order;
        for ( const int column : permutation ) order.Add(column);
        REQUIRE(list->SetColumnsOrder(order));
        REQUIRE(list->GetColumnsOrder() == order);
        list->Update();
        int previousRight = -1;
        for ( int position = 0; position < 3; ++position )
        {
            const int column = permutation[position];
            CAPTURE(position, column);
            CHECK(list->GetColumnOrder(column) == position);
            CHECK(list->GetColumnIndexFromOrder(position) == column);
            CHECK(list->GetItemText(0, column) == GenericOrderList::Value(0, column));
            CHECK(list->GetColumnWidth(column) == widths[column]);
            wxRect bounds;
            REQUIRE(list->GetSubItemRect(0, column, bounds));
            CHECK(bounds.x > previousRight);
            previousRight = bounds.GetRight();
            int flags = 0;
            long hitColumn = -1;
            const wxPoint point(bounds.x + bounds.width / 2,
                                bounds.y + bounds.height / 2);
            CHECK(list->HitTest(point, flags, &hitColumn) == 0);
            CHECK(hitColumn == column);
        }
    }
    while ( std::next_permutation(std::begin(permutation), std::end(permutation)) );
}
#if wxUSE_TEXTCTRL
namespace
{
class GenericLabelList final : public wxGenericListCtrl
{
public:
    explicit GenericLabelList(bool virtualList = false)
        : wxGenericListCtrl(wxTheApp->GetTopWindow(), wxID_ANY,
                            wxDefaultPosition, wxSize(400, 200),
                            wxLC_REPORT | wxLC_EDIT_LABELS |
                            (virtualList ? wxLC_VIRTUAL : 0))
    {
        InsertColumn(0, "Label", wxLIST_FORMAT_LEFT, 240);
        if ( virtualList ) SetItemCount(2);
        else
        {
            InsertItem(0, "Original");
            InsertItem(1, "Survivor");
        }
    }

    int getter = 0;
    mutable std::function<void()> onRead;

private:
    void Read(int which) const
    {
        if ( getter == which && onRead )
        {
            // Copy the callback out before it can destroy this object.
            const auto callback = std::move(onRead);
            callback();
        }
    }
    wxString OnGetItemText(long, long) const override
    { Read(0); return "Virtual label"; }
    int OnGetItemColumnImage(long, long) const override
    { Read(1); return -1; }
    wxItemAttr* OnGetItemAttr(long) const override
    { Read(2); return nullptr; }
};
}

TEST_CASE("GenericListCtrl::EndEditLabelReportsEditorNotVetoAndAllowsAppend",
          "[listctrl][generic-listctrl][label-edit-contract]")
{
    const bool veto = GENERATE(false, true);
    auto list = std::make_unique<GenericLabelList>();
    unsigned begins = 0, ends = 0;
    list->Bind(wxEVT_LIST_BEGIN_LABEL_EDIT, [&](wxListEvent& event)
    {
        ++begins;
        CHECK(event.GetText() == "Original");
        CHECK(list->GetEditControl());
    });
    list->Bind(wxEVT_LIST_END_LABEL_EDIT, [&](wxListEvent& event)
    {
        ++ends;
        CHECK_FALSE(event.IsEditCancelled());
        CHECK(event.GetIndex() == 0);
        CHECK(event.GetText() == "Candidate");
        CHECK(list->GetItemText(0) == "Original");
        // EditableListBox appends its next empty row here. A new later row
        // changes topology without invalidating the edited item's identity.
        REQUIRE(list->InsertItem(2, "Appended during END") == 2);
        if ( veto ) event.Veto();
    });
    auto* const editor = list->EditLabel(0);
    REQUIRE(editor);
    editor->ChangeValue("Candidate");
    // Public EndEditLabel returns whether an editor existed, including when
    // END vetoed. The private UIA adapter separately observes acceptance.
    CHECK(list->EndEditLabel(false));
    CHECK_FALSE(list->EndEditLabel(false));
    CHECK_FALSE(list->GetEditControl());
    CHECK(begins == 1u);
    CHECK(ends == 1u);
    CHECK(list->GetItemText(0) == (veto ? "Original" : "Candidate"));
    CHECK(list->GetItemText(1) == "Survivor");
    CHECK(list->GetItemText(2) == "Appended during END");
}

TEST_CASE("GenericListCtrl::LabelEditRejectsRetiredItemDuringEvents",
          "[listctrl][generic-listctrl][label-edit-contract]")
{
    const bool atBegin = GENERATE(false, true);
    const bool destroy = GENERATE(false, true);
    auto list = std::make_unique<GenericLabelList>();
    unsigned callbacks = 0;
    list->Bind(atBegin ? wxEVT_LIST_BEGIN_LABEL_EDIT : wxEVT_LIST_END_LABEL_EDIT,
        [&](wxListEvent&)
        {
            ++callbacks;
            if ( destroy ) list.reset();
            else REQUIRE(list->DeleteItem(0));
            // Consume this event: no dispatch continuation on a deleted handler.
        });
    auto* const editor = list->EditLabel(0);
    if ( atBegin ) CHECK_FALSE(editor);
    else
    {
        REQUIRE(editor);
        editor->ChangeValue("Must not overwrite the successor");
        CHECK(list->EndEditLabel(false));
    }
    CHECK(callbacks == 1u);
    if ( !destroy )
    {
        CHECK_FALSE(list->GetEditControl());
        CHECK(list->GetItemCount() == 1);
        CHECK(list->GetItemText(0) == "Survivor");
    }
}

TEST_CASE("GenericListCtrl::LabelEditRevalidatesEachVirtualGetter",
          "[listctrl][generic-listctrl][label-edit-contract]")
{
    const int getter = GENERATE(0, 1, 2);
    const bool destroy = GENERATE(false, true);
    auto list = std::make_unique<GenericLabelList>(true);
    list->getter = getter;
    unsigned callbacks = 0, begins = 0;
    list->Bind(wxEVT_LIST_BEGIN_LABEL_EDIT, [&](wxListEvent&) { ++begins; });
    list->onRead = [&]
    {
        ++callbacks;
        if ( destroy ) list.reset();
        else list->SetItemCount(0);
    };
    CHECK_FALSE(list->EditLabel(0));
    CHECK(callbacks == 1u);
    CHECK(begins == 0u);
    if ( !destroy ) CHECK_FALSE(list->GetEditControl());
}
#endif // wxUSE_TEXTCTRL
#endif // generic list implementation

#endif // wxUSE_LISTCTRL
