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

#endif // wxUSE_LISTCTRL
