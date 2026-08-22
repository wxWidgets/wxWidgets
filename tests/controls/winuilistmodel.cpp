///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuilistmodel.cpp
// Purpose:     deterministic WinUI ListBox/CheckListBox delta tests
// Author:      wxWidgets development team
// Created:     2026-07-26
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"
#include "waitfor.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_LISTBOX

#include "wx/app.h"
#include "wx/dc.h"
#include "wx/event.h"
#include "wx/frame.h"
#include "wx/listbox.h"
#include "wx/log.h"
#include "wx/sizer.h"

#include <winrt/base.h>

#include <algorithm>
#include <cstdint>
#include <vector>

#if wxUSE_OWNER_DRAWN
    #include "wx/ownerdrw.h"
#endif

#if wxUSE_CHECKLISTBOX
    #include "wx/checklst.h"
#endif

namespace
{

class wxWinUIListCountedClientData final : public wxClientData
{
public:
    explicit wxWinUIListCountedClientData(int *destructionCount)
        : m_destructionCount(destructionCount)
    {
    }

    ~wxWinUIListCountedClientData() override
    {
        ++*m_destructionCount;
    }

private:
    int *m_destructionCount;
};

#if wxUSE_CHECKLISTBOX

class wxWinUITestKeyboardLayout final
{
public:
    explicit wxWinUITestKeyboardLayout(const wchar_t *identifier)
    {
        const int count = ::GetKeyboardLayoutList(0, nullptr);
        std::vector<HKL> layouts(count > 0 ? count : 0);
        if ( count > 0 )
        {
            const int copied = ::GetKeyboardLayoutList(
                count, layouts.data());
            layouts.resize(copied > 0 ? copied : 0);
        }

        m_layout = ::LoadKeyboardLayoutW(identifier, KLF_NOTELLSHELL);
        m_unload = m_layout &&
            std::find(layouts.begin(), layouts.end(), m_layout) ==
                layouts.end();
    }

    ~wxWinUITestKeyboardLayout()
    {
        // KLF_NOTELLSHELL doesn't activate the layout. Still don't unload it
        // if external code made it current while this test was running.
        if ( m_unload && m_layout &&
             ::GetKeyboardLayout(0) != m_layout )
        {
            ::UnloadKeyboardLayout(m_layout);
        }
    }

    explicit operator bool() const { return m_layout != nullptr; }
    std::uintptr_t GetValue() const
    {
        return reinterpret_cast<std::uintptr_t>(m_layout);
    }

private:
    HKL m_layout = nullptr;
    bool m_unload = false;
};

#endif // wxUSE_CHECKLISTBOX

#if wxUSE_OWNER_DRAWN

struct wxWinUIOwnerDrawProbeState
{
    int measureCalls = 0;
    int drawCalls = 0;
    int baseDrawCalls = 0;
    unsigned int lastStatus = 0;
    wxLayoutDirection lastDirection = wxLayout_Default;
    wxFont lastFont;
    HDC activeBaseDrawHDC = nullptr;
    std::uintptr_t baseSelectedFont = 0;
    std::uintptr_t ownerItemFont = 0;
    int baseSelectedFontHeight = 0;
    int ownerItemFontHeight = 0;
    bool baseSelectedOwnerItemFont = false;
    bool refreshReentrantly = false;
    bool reenteredRefresh = false;
    bool failWithHresultOnce = false;
    bool throwOnCreateOnce = false;
    bool destroyOwner = false;
    bool destroyOwnerFromItemDestructor = false;
    wxListBox **ownerToDestroy = nullptr;
    int itemDestructorCalls = 0;
};

class wxWinUIOwnerDrawProbeList;

class wxWinUIOwnerDrawProbeItem final : public wxOwnerDrawn
{
public:
    wxWinUIOwnerDrawProbeItem(wxWinUIOwnerDrawProbeList *owner,
                              wxWinUIOwnerDrawProbeState *state)
        : m_owner(owner),
          m_state(state)
    {
        SetOwnerDrawn();
    }

    ~wxWinUIOwnerDrawProbeItem() override;

    wxString GetName() const override;

    bool OnMeasureItem(size_t *width, size_t *height) override;

    bool OnDrawItem(wxDC& dc,
                    const wxRect& rect,
                    wxODAction,
                    wxODStatus status) override;

private:
    wxWinUIOwnerDrawProbeList *m_owner;
    wxWinUIOwnerDrawProbeState *m_state;
};

class wxWinUIOwnerDrawProbeList final : public wxListBox
{
public:
    explicit wxWinUIOwnerDrawProbeList(
        wxWinUIOwnerDrawProbeState *state)
        : m_state(state)
    {
    }

    wxOwnerDrawn *CreateLboxItem(size_t) override
    {
        if ( m_state->throwOnCreateOnce )
        {
            m_state->throwOnCreateOnce = false;
            throw winrt::hresult_error(E_FAIL);
        }
        return new wxWinUIOwnerDrawProbeItem(this, m_state);
    }

    void DeleteOwnerItemForTesting(unsigned int n)
    {
        DoDeleteOneItem(n);
    }

    void ClearOwnerItemsForTesting()
    {
        DoClear();
    }

private:
    wxWinUIOwnerDrawProbeState *m_state;
};

wxString wxWinUIOwnerDrawProbeItem::GetName() const
{
    if ( m_state->activeBaseDrawHDC )
    {
        const HFONT selectedFont = reinterpret_cast<HFONT>(
            ::GetCurrentObject(m_state->activeBaseDrawHDC, OBJ_FONT));
        const HFONT ownerItemFont = reinterpret_cast<HFONT>(
            GetFont().GetHFONT());
        m_state->baseSelectedFont =
            reinterpret_cast<std::uintptr_t>(selectedFont);
        m_state->ownerItemFont =
            reinterpret_cast<std::uintptr_t>(ownerItemFont);
        m_state->baseSelectedOwnerItemFont =
            selectedFont && selectedFont == ownerItemFont;

        LOGFONTW selectedInfo{};
        LOGFONTW ownerItemInfo{};
        if ( selectedFont &&
             ::GetObjectW(selectedFont, sizeof(selectedInfo),
                          &selectedInfo) == sizeof(selectedInfo) )
        {
            m_state->baseSelectedFontHeight = selectedInfo.lfHeight;
        }
        if ( ownerItemFont &&
             ::GetObjectW(ownerItemFont, sizeof(ownerItemInfo),
                          &ownerItemInfo) == sizeof(ownerItemInfo) )
        {
            m_state->ownerItemFontHeight = ownerItemInfo.lfHeight;
        }
    }

    const int index = m_owner->GetItemIndex(
        const_cast<wxWinUIOwnerDrawProbeItem *>(this));
    return index == wxNOT_FOUND
        ? wxString()
        : m_owner->GetString(static_cast<unsigned int>(index));
}

wxWinUIOwnerDrawProbeItem::~wxWinUIOwnerDrawProbeItem()
{
    ++m_state->itemDestructorCalls;
    if ( m_state->destroyOwnerFromItemDestructor &&
         m_state->ownerToDestroy )
    {
        m_state->destroyOwnerFromItemDestructor = false;
        wxListBox * const doomed = *m_state->ownerToDestroy;
        *m_state->ownerToDestroy = nullptr;
        delete doomed;
    }
}

bool wxWinUIOwnerDrawProbeItem::OnMeasureItem(size_t *width,
                                               size_t *height)
{
    ++m_state->measureCalls;
    if ( width )
        *width = static_cast<size_t>(m_owner->FromDIP(140));
    if ( height )
        *height = static_cast<size_t>(m_owner->FromDIP(24));
    return true;
}

bool wxWinUIOwnerDrawProbeItem::OnDrawItem(
    wxDC& dc,
    const wxRect& rect,
    wxODAction action,
    wxODStatus status)
{
    ++m_state->drawCalls;
    m_state->lastStatus = static_cast<unsigned int>(status);
    m_state->lastDirection = dc.GetLayoutDirection();
    m_state->lastFont = dc.GetFont();

    // Exercise the real wxMSW wxOwnerDrawn implementation. GetName() runs
    // while its SelectInHDC guard is active and records the actual HFONT in
    // the MemoryDC, proving that the retained item's mirrored font -- not
    // merely the custom override's dc.GetFont() -- is selected.
    ++m_state->baseDrawCalls;
    m_state->activeBaseDrawHDC = reinterpret_cast<HDC>(dc.GetHDC());
    const bool baseDrawn = wxOwnerDrawn::OnDrawItem(
        dc, rect, action, status);
    m_state->activeBaseDrawHDC = nullptr;
    if ( !baseDrawn )
        return false;

    const wxColour surface = status & wxOwnerDrawn::wxODSelected
        ? wxColour(192, 64, 32)
        : wxColour(18, 52, 86);
    dc.SetBackground(wxBrush(surface));
    dc.Clear();
    dc.SetTextForeground(
        status & wxOwnerDrawn::wxODSelected
            ? wxColour(255, 255, 255)
            : wxColour(18, 52, 86));
    dc.DrawText(GetName(), rect.GetPosition() + wxPoint(2, 2));

    if ( m_state->failWithHresultOnce )
    {
        m_state->failWithHresultOnce = false;
        throw winrt::hresult_error(E_FAIL);
    }

    if ( m_state->refreshReentrantly &&
         !m_state->reenteredRefresh )
    {
        m_state->reenteredRefresh = true;
        const int index = m_owner->GetItemIndex(this);
        if ( index != wxNOT_FOUND )
            (void)m_owner->RefreshItem(static_cast<size_t>(index));
    }

    if ( m_state->destroyOwner && m_state->ownerToDestroy )
    {
        wxListBox * const doomed = *m_state->ownerToDestroy;
        *m_state->ownerToDestroy = nullptr;
        delete doomed;
    }

    return true;
}

#if wxUSE_CHECKLISTBOX

struct wxWinUICheckDrawProbeState
{
    int drawCalls = 0;
    bool lastChecked = false;
    bool projectBitmap = true;
};

class wxWinUICheckDrawProbeList;

class wxWinUICheckDrawProbeItem final : public wxOwnerDrawn
{
public:
    wxWinUICheckDrawProbeItem(wxWinUICheckDrawProbeList *owner,
                              wxWinUICheckDrawProbeState *state)
        : m_owner(owner),
          m_state(state)
    {
        SetOwnerDrawn();
    }

    wxString GetName() const override;
    bool OnMeasureItem(size_t *width, size_t *height) override;
    bool OnDrawItem(wxDC& dc,
                    const wxRect& rect,
                    wxODAction,
                    wxODStatus) override;

private:
    wxWinUICheckDrawProbeList *m_owner;
    wxWinUICheckDrawProbeState *m_state;
};

class wxWinUICheckDrawProbeList final : public wxCheckListBox
{
public:
    explicit wxWinUICheckDrawProbeList(
        wxWinUICheckDrawProbeState *state)
        : m_state(state)
    {
    }

    wxOwnerDrawn *CreateLboxItem(size_t) override
    {
        return new wxWinUICheckDrawProbeItem(this, m_state);
    }

private:
    wxWinUICheckDrawProbeState *m_state;
};

wxString wxWinUICheckDrawProbeItem::GetName() const
{
    const int index = m_owner->GetItemIndex(
        const_cast<wxWinUICheckDrawProbeItem *>(this));
    return index == wxNOT_FOUND
        ? wxString()
        : m_owner->GetString(static_cast<unsigned int>(index));
}

bool wxWinUICheckDrawProbeItem::OnMeasureItem(size_t *width,
                                               size_t *height)
{
    if ( width )
        *width = static_cast<size_t>(m_owner->FromDIP(140));
    if ( height )
        *height = static_cast<size_t>(m_owner->FromDIP(24));
    return true;
}

bool wxWinUICheckDrawProbeItem::OnDrawItem(
    wxDC& dc,
    const wxRect& rect,
    wxODAction,
    wxODStatus)
{
    ++m_state->drawCalls;
    const int index = m_owner->GetItemIndex(this);
    m_state->lastChecked =
        index != wxNOT_FOUND &&
        m_owner->IsChecked(static_cast<unsigned int>(index));
    dc.SetTextForeground(
        m_state->lastChecked ? wxColour(0, 110, 40)
                             : wxColour(110, 40, 0));
    dc.DrawText(GetName(), rect.GetPosition() + wxPoint(2, 2));
    return m_state->projectBitmap;
}

#endif // wxUSE_CHECKLISTBOX
#endif // wxUSE_OWNER_DRAWN

} // anonymous namespace

TEST_CASE("wxWinUI list controls support default construction",
          "[winui-itemmodel][winui-listmodel][default-create]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxListBox list;
    REQUIRE(list.Create(parent, wxID_ANY));
    CHECK(list.Append("list") == 0);

#if wxUSE_CHECKLISTBOX
    wxCheckListBox checklist;
    REQUIRE(checklist.Create(parent, wxID_ANY));
    CHECK(checklist.Append("check") == 0);
    checklist.Check(0);
    CHECK(checklist.IsChecked(0));
#endif
}

TEST_CASE("wxWinUI ListBox applies stable peer deltas",
          "[winui-itemmodel][winui-listmodel][listbox]")
{
    wxFrame frame(nullptr, wxID_ANY, "list delta owner");
    wxListBox list(&frame, wxID_ANY);

    REQUIRE(list.Append("one") == 0);
    REQUIRE(list.Append("two") == 1);
    REQUIRE(list.Append("three") == 2);

    const std::uint64_t oneId = list.WinUIGetItemIdForTesting(0);
    const std::uint64_t twoId = list.WinUIGetItemIdForTesting(1);
    const std::uint64_t threeId = list.WinUIGetItemIdForTesting(2);
    const std::uintptr_t onePeer =
        list.WinUIGetItemPeerIdentityForTesting(0);
    const std::uintptr_t twoPeer =
        list.WinUIGetItemPeerIdentityForTesting(1);
    const std::uintptr_t threePeer =
        list.WinUIGetItemPeerIdentityForTesting(2);
    REQUIRE(onePeer != 0);
    REQUIRE(twoPeer != 0);
    REQUIRE(threePeer != 0);

    REQUIRE(list.Insert("middle", 1) == 1);
    CHECK(list.WinUIGetItemIdForTesting(0) == oneId);
    CHECK(list.WinUIGetItemIdForTesting(2) == twoId);
    CHECK(list.WinUIGetItemIdForTesting(3) == threeId);
    CHECK(list.WinUIGetItemPeerIdentityForTesting(0) == onePeer);
    CHECK(list.WinUIGetItemPeerIdentityForTesting(2) == twoPeer);
    CHECK(list.WinUIGetItemPeerIdentityForTesting(3) == threePeer);

    list.SetString(2, "two renamed");
    CHECK(list.WinUIGetItemIdForTesting(2) == twoId);
    CHECK(list.WinUIGetItemPeerIdentityForTesting(2) == twoPeer);

    list.Delete(1);
    CHECK(list.WinUIGetItemIdForTesting(0) == oneId);
    CHECK(list.WinUIGetItemIdForTesting(1) == twoId);
    CHECK(list.WinUIGetItemIdForTesting(2) == threeId);
    CHECK(list.WinUIGetItemPeerIdentityForTesting(0) == onePeer);
    CHECK(list.WinUIGetItemPeerIdentityForTesting(1) == twoPeer);
    CHECK(list.WinUIGetItemPeerIdentityForTesting(2) == threePeer);

    REQUIRE(list.WinUIPoisonPeerForTesting());
    CHECK(list.WinUIGetPeerCountForTesting() == list.GetCount() + 1);
    REQUIRE(list.Append("after recovery") == 3);
    CHECK(list.WinUIGetPeerCountForTesting() == list.GetCount());
    CHECK(list.GetString(3) == "after recovery");
}

TEST_CASE("wxWinUI ListBox preserves sorted multiple selection and ownership",
          "[winui-itemmodel][winui-listmodel][listbox]")
{
    wxFrame frame(nullptr, wxID_ANY, "sorted list owner");
    int destroyed = 0;

    {
        wxListBox list(&frame, wxID_ANY, wxDefaultPosition,
                       wxDefaultSize, 0, nullptr,
                       wxLB_SORT | wxLB_MULTIPLE);
        list.Append("aaa");
        list.Append("Aaa");
        list.Append("AAA");
        REQUIRE(list.GetCount() == 3);
        CHECK(list.GetString(0) == "AAA");
        CHECK(list.GetString(1) == "Aaa");
        CHECK(list.GetString(2) == "aaa");

        const std::uint64_t firstSelectedId =
            list.WinUIGetItemIdForTesting(0);
        const std::uint64_t secondSelectedId =
            list.WinUIGetItemIdForTesting(2);
        const std::uintptr_t secondSelectedPeer =
            list.WinUIGetItemPeerIdentityForTesting(2);

        list.SetSelection(0);
        list.SetSelection(2);
        list.SetClientObject(
            2, new wxWinUIListCountedClientData(&destroyed));

        list.SetString(2, "AA");
        REQUIRE(list.GetCount() == 3);
        CHECK(list.WinUIGetItemIdForTesting(0) == secondSelectedId);
        CHECK(list.WinUIGetItemPeerIdentityForTesting(0) ==
              secondSelectedPeer);
        CHECK(list.GetClientObject(0) != nullptr);

        wxArrayInt selections;
        REQUIRE(list.GetSelections(selections) == 2);
        CHECK(list.WinUIGetItemIdForTesting(selections[0]) ==
              secondSelectedId);
        CHECK(list.WinUIGetItemIdForTesting(selections[1]) ==
              firstSelectedId);

        list.SetClientObject(
            0, new wxWinUIListCountedClientData(&destroyed));
        CHECK(destroyed == 1);
        list.Delete(0);
        CHECK(destroyed == 2);

        list.Append("owned at destruction");
        const int owned = list.FindString("owned at destruction", true);
        REQUIRE(owned != wxNOT_FOUND);
        list.SetClientObject(
            owned, new wxWinUIListCountedClientData(&destroyed));
    }

    CHECK(destroyed == 3);
}

TEST_CASE("wxWinUI ListBox peer selection maps by stable ID",
          "[winui-itemmodel][winui-listmodel][listbox][lifetime]")
{
    wxFrame frame(nullptr, wxID_ANY, "list callback owner");
    wxListBox list(&frame, wxID_ANY, wxDefaultPosition,
                   wxDefaultSize, 0, nullptr, wxLB_MULTIPLE);
    list.Append("zero");
    list.Append("one");
    list.Append("two");

    int selectionEvents = 0;
    int lastSelection = wxNOT_FOUND;
    int clientSentinel = 0;
    void *eventClientData = nullptr;
    list.SetClientData(2, &clientSentinel);
    list.Bind(
        wxEVT_LISTBOX,
        [&](wxCommandEvent& event)
        {
            ++selectionEvents;
            lastSelection = event.GetInt();
            eventClientData = event.GetClientData();
        });

    REQUIRE(list.WinUISetPeerSelectionForTesting(2, true));
    CHECK(selectionEvents == 1);
    CHECK(lastSelection == 2);
    CHECK(eventClientData == &clientSentinel);
    CHECK(list.IsSelected(2));

    const std::uint64_t selectedId =
        list.WinUIGetItemIdForTesting(2);
    list.Insert("inserted", 0);
    CHECK(list.WinUIGetItemIdForTesting(3) == selectedId);
    CHECK(list.IsSelected(3));
    CHECK(selectionEvents == 1);

    unsigned doubleClicks = 0;
    int doubleClickItem = wxNOT_FOUND;
    list.Bind(wxEVT_LISTBOX_DCLICK, [&](wxCommandEvent& event)
    {
        ++doubleClicks;
        doubleClickItem = event.GetInt();
    });
    REQUIRE(list.WinUIDoubleTapPeerForTesting(3));
    CHECK(doubleClicks == 1);
    CHECK(doubleClickItem == 3);

    wxListBox *doomed = new wxListBox(&frame, wxID_ANY);
    doomed->Append("delete from event");
    bool destroyedInCallback = false;
    doomed->Bind(
        wxEVT_LISTBOX,
        [&doomed, &destroyedInCallback](wxCommandEvent&)
        {
            destroyedInCallback = true;
            delete doomed;
            doomed = nullptr;
        });

    REQUIRE(doomed->WinUISetPeerSelectionForTesting(0, true));
    CHECK(destroyedInCallback);
    CHECK(doomed == nullptr);
}

TEST_CASE("wxWinUI ListBox hit test uses the visual coordinate mapper",
          "[winui-itemmodel][winui-listmodel][listbox][dpi]")
{
    wxFrame frame(nullptr, wxID_ANY, "list hit-test owner",
                  wxPoint(-30000, -30000), wxSize(360, 260),
                  wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    wxListBox list(&frame, wxID_ANY, wxPoint(10, 10),
                   wxSize(300, 200));
    list.Append("zero");
    list.Append("one");
    list.Append("two");

    const double scale = list.GetDPIScaleFactor();
    const int expectedItemHeightPixels = list.GetCharHeight() + 2;
    double itemHeightDips = 0.0;
    double itemMinHeightDips = -1.0;
    double itemActualHeightDips = 0.0;
    REQUIRE(list.WinUIGetItemPresentationForTesting(
        0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr,
        &itemHeightDips, &itemMinHeightDips, &itemActualHeightDips));
    CHECK(itemHeightDips ==
          Approx(expectedItemHeightPixels / scale).margin(0.01));
    CHECK(itemMinHeightDips == Approx(0.0));

    frame.ShowWithoutActivating();
    REQUIRE(WaitFor("WinUI ListBox first item layout", [&]()
    {
        frame.Update();
        for ( int y = 0; y < list.GetClientSize().y / 2; ++y )
        {
            if ( list.HitTest(wxPoint(list.FromDIP(5), y)) == 0 )
                return true;
        }
        return false;
    }));

    wxRect firstRect;
    wxRect thirdRect;
    REQUIRE(WaitFor("WinUI ListBox compact item metrics", [&]()
    {
        frame.Update();
        return list.GetItemRect(0, firstRect) &&
               list.GetItemRect(2, thirdRect) &&
               list.WinUIGetItemPresentationForTesting(
                   0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                   nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                   nullptr, &itemActualHeightDips) &&
               itemActualHeightDips > 0.0;
    }));
    CAPTURE(scale,
            list.GetDPI().x,
            list.GetDPI().y,
            list.GetCharHeight(),
            expectedItemHeightPixels,
            itemHeightDips,
            itemMinHeightDips,
            itemActualHeightDips,
            firstRect.x,
            firstRect.y,
            firstRect.width,
            firstRect.height,
            thirdRect.x,
            thirdRect.y,
            thirdRect.width,
            thirdRect.height);
    CHECK(firstRect.height == Approx(expectedItemHeightPixels).margin(1));
    CHECK(thirdRect.height == Approx(expectedItemHeightPixels).margin(1));
    CHECK(itemActualHeightDips == Approx(itemHeightDips).margin(0.01));
    CHECK(thirdRect.GetBottom() < 190);
    // This is the same physical wx client point used by the public
    // ListBoxTestCase. ClientPointToElement performs the physical-to-DIP
    // conversion; callers must not pre-convert it with FromDIP().
    CHECK(list.HitTest(wxPoint(290, 190)) == wxNOT_FOUND);
    CHECK(list.HitTest(wxPoint(-1, -1)) == wxNOT_FOUND);

    for ( int n = 3; n < 100; ++n )
        list.Append(wxString::Format("item %02d", n));

    wxListBox::WinUIScrollMetricsForTesting verticalMetrics;
    double realizedLineHeightDips = 0.0;
    int countPerPage = -1;
    REQUIRE(WaitFor("WinUI ListBox authoritative vertical viewport", [&]()
    {
        frame.Update();
        return list.WinUIGetScrollPresentationForTesting(
                   nullptr, nullptr, nullptr, &verticalMetrics) &&
               verticalMetrics.hasAuthoritativeScroll &&
               verticalMetrics.scrollViewportHeight > 0.0 &&
               verticalMetrics.scrollExtentHeight >
                   verticalMetrics.scrollViewportHeight &&
               verticalMetrics.scrollScrollableHeight > 0.0 &&
               list.WinUIGetItemPresentationForTesting(
                   0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                   nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                   nullptr, &realizedLineHeightDips) &&
               realizedLineHeightDips > 0.0 &&
               (countPerPage = list.GetCountPerPage()) > 0;
    }));
    const int expectedCountPerPage = static_cast<int>(
        verticalMetrics.scrollViewportHeight / realizedLineHeightDips);
    CAPTURE(verticalMetrics.scrollExtentHeight,
            verticalMetrics.scrollViewportHeight,
            verticalMetrics.scrollScrollableHeight,
            realizedLineHeightDips,
            countPerPage,
            expectedCountPerPage);
    CHECK(verticalMetrics.scrollScrollableHeight ==
          Approx(verticalMetrics.scrollExtentHeight -
                 verticalMetrics.scrollViewportHeight).margin(0.01));
    CHECK(countPerPage == expectedCountPerPage);

    list.SetFirstItem(20);
    REQUIRE(WaitFor("WinUI ListBox SetFirstItem", [&]()
    {
        return list.GetTopItem() == 20;
    }));
    double virtualizedLineHeightDips = 0.0;
    REQUIRE(list.WinUIGetItemPresentationForTesting(
        20, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        &virtualizedLineHeightDips));
    REQUIRE(virtualizedLineHeightDips > 0.0);
    CHECK(list.GetCountPerPage() == static_cast<int>(
        verticalMetrics.scrollViewportHeight / virtualizedLineHeightDips));
    wxRect offscreenRect;
    CHECK_FALSE(list.GetItemRect(99, offscreenRect));

    list.EnsureVisible(99);
    REQUIRE(WaitFor("WinUI ListBox EnsureVisible", [&]()
    {
        frame.Update();
        wxRect visibleRect;
        return list.GetTopItem() > 20 &&
               list.GetItemRect(99, visibleRect) &&
               visibleRect.height > 0;
    }));
    frame.Hide();

    // LB_GETITEMHEIGHT followed by integer client-height division returns 0
    // on wxMSW when even one fixed row does not fit. Exercise that public
    // edge against the realized inner ScrollViewer viewport, not a synthetic
    // size seam and not the outer shared-host viewport.
    wxFrame tinyFrame(nullptr, wxID_ANY, "short list viewport",
                      wxPoint(-30000, -30000), wxSize(260, 120),
                      wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    wxListBox tinyList(&tinyFrame, wxID_ANY);
    const int tinyLinePixels = tinyList.GetCharHeight() + 2;
    const wxSize tinySize(200, wxMax(1, tinyLinePixels - 1));
    // SetMinSize() is only a constraint; wxSizerItem snapshots GetSize() when
    // the window is added and wxFIXED_MINSIZE then republishes that snapshot.
    // Make the sub-line geometry an actual public wx size writer first, so
    // the sizer snapshots line-height-minus-one physical pixels instead of
    // the ListBox best-size height.
    tinyList.SetSize(tinySize);
    tinyList.SetMinSize(tinySize);
    wxBoxSizer * const tinySizer = new wxBoxSizer(wxVERTICAL);
    tinySizer->Add(&tinyList, 0, wxFIXED_MINSIZE);
    tinyFrame.SetSizer(tinySizer);
    REQUIRE(tinyFrame.Layout());
    REQUIRE(tinyList.GetSize() == tinySize);
    REQUIRE(tinyList.Append("only row") == 0);
    tinyFrame.ShowWithoutActivating();

    wxListBox::WinUIScrollMetricsForTesting tinyMetrics;
    double tinyActualLineHeightDips = 0.0;
    const bool tinyViewportConverged = WaitFor(
        "WinUI ListBox sub-line authoritative viewport", [&]()
    {
        tinyFrame.Update();
        return tinyList.WinUIGetScrollPresentationForTesting(
                   nullptr, nullptr, nullptr, &tinyMetrics) &&
               tinyMetrics.hasAuthoritativeScroll &&
               tinyMetrics.scrollViewportHeight > 0.0 &&
               tinyList.WinUIGetItemPresentationForTesting(
                   0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                   nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                   nullptr, &tinyActualLineHeightDips) &&
               tinyActualLineHeightDips > tinyMetrics.scrollViewportHeight;
    });
    CAPTURE(tinyLinePixels,
            tinySize.y,
            tinyList.GetSize().x,
            tinyList.GetSize().y,
            tinyList.GetClientSize().x,
            tinyList.GetClientSize().y,
            tinyMetrics.hasItemsPanelRoot,
            tinyMetrics.hasAuthoritativeScroll,
            tinyMetrics.scrollExtentHeight,
            tinyMetrics.scrollViewportHeight,
            tinyMetrics.scrollScrollableHeight,
            tinyActualLineHeightDips);
    REQUIRE(tinyViewportConverged);
    CHECK(tinyList.GetCountPerPage() == 0);
    tinyFrame.Hide();
}

TEST_CASE("wxWinUI ListBox projects Windows scrollbar and tab-stop semantics",
          "[winui-itemmodel][winui-listmodel][listbox][scroll]")
{
    wxFrame frame(nullptr, wxID_ANY, "list scroll owner");

    wxListBox needed(&frame, wxID_ANY, wxDefaultPosition,
                      wxSize(220, 120), 0, nullptr,
                      wxLB_HSCROLL | wxLB_NEEDED_SB);
    // A top-level window with exactly one child deliberately expands it to
    // the whole client area. Keep 220x120 as an actual wx layout writer so
    // the requested horizontal extent is wider than the real viewport.
    wxBoxSizer * const neededSizer = new wxBoxSizer(wxVERTICAL);
    neededSizer->Add(&needed, 0, wxFIXED_MINSIZE);
    frame.SetSizer(neededSizer);
    REQUIRE(frame.Layout());
    CHECK(needed.GetSize() == wxSize(220, 120));
    wxFont projectedFont = needed.GetFont();
    projectedFont.SetWeight(wxFONTWEIGHT_BOLD);
    projectedFont.SetFaceName("Arial");
    REQUIRE(needed.SetFont(projectedFont));
    int horizontal = -1;
    int vertical = -1;
    REQUIRE(needed.WinUIGetScrollPresentationForTesting(
        &horizontal, &vertical));
    CHECK(horizontal == 1); // ScrollBarVisibility::Auto
    CHECK(vertical == 1);   // ScrollBarVisibility::Auto

    REQUIRE(needed.Append("first\tsecond\tthird\tfourth") == 0);
    const std::uintptr_t peer =
        needed.WinUIGetItemPeerIdentityForTesting(0);
    REQUIRE(peer != 0);

    const int average = wxMax(1, needed.GetCharWidth());
    const double scale = needed.GetDPIScaleFactor();
    const auto dluToPixels = [average](int dlu)
    {
        const int pixels = ::MulDiv(dlu, average, 4);
        return pixels > 0 ? pixels : 1;
    };
    wxCoord firstRunWidth = 0;
    needed.GetTextExtent("first", &firstRunWidth, nullptr);

    // Before LB_SETTABSTOPS, Windows repeats the system 32-DLU stops.
    wxVector<double> neverSetOffsets;
    REQUIRE(needed.WinUIGetItemPresentationForTesting(
        0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        &neverSetOffsets));
    REQUIRE(neverSetOffsets.size() == 4);
    const int neverSetInterval = dluToPixels(32);
    const int expectedNeverSetSecond =
        ((firstRunWidth / neverSetInterval) + 1) * neverSetInterval;
    CHECK(neverSetOffsets[1] ==
          Approx(expectedNeverSetSecond / scale).margin(0.01));

    wxVector<int> tabStops;
    tabStops.push_back(16);
    tabStops.push_back(48);
    REQUIRE(needed.MSWSetTabStops(tabStops));

    unsigned int tabRuns = 0;
    double tabbedWidth = 0.0;
    wxVector<double> absoluteOffsets;
    unsigned int peerFontWeight = 0;
    double peerFontSize = 0.0;
    wxString peerFontFamily;
    REQUIRE(needed.WinUIGetItemPresentationForTesting(
        0, nullptr, nullptr, &tabRuns, &tabbedWidth, nullptr, nullptr,
        &absoluteOffsets, nullptr, &peerFontWeight, &peerFontSize,
        &peerFontFamily));
    CHECK(tabRuns == 4);
    REQUIRE(absoluteOffsets.size() == 4);
    CHECK(peerFontWeight ==
          static_cast<unsigned int>(needed.GetFont().GetNumericWeight()));
    CHECK(peerFontSize ==
          Approx(needed.GetFont().GetFractionalPointSize() * 96.0 / 72.0)
              .margin(0.01));
    CHECK(peerFontFamily == needed.GetFont().GetFaceName());

    const int defaultInterval = dluToPixels(32);
    const auto nextExplicitOrDefault = [&](int current)
    {
        for ( const int stop : tabStops )
        {
            const int stopPosition = dluToPixels(stop);
            if ( stopPosition > current )
                return stopPosition;
        }
        return ((current / defaultInterval) + 1) * defaultInterval;
    };
    wxCoord secondRunWidth = 0;
    wxCoord thirdRunWidth = 0;
    needed.GetTextExtent("second", &secondRunWidth, nullptr);
    needed.GetTextExtent("third", &thirdRunWidth, nullptr);
    const int expectedAbsoluteSecond =
        nextExplicitOrDefault(firstRunWidth);
    const int expectedAbsoluteThird =
        nextExplicitOrDefault(expectedAbsoluteSecond + secondRunWidth);
    const int expectedAbsoluteFourth =
        nextExplicitOrDefault(expectedAbsoluteThird + thirdRunWidth);
    CHECK(absoluteOffsets[1] ==
          Approx(expectedAbsoluteSecond / scale).margin(0.01));
    CHECK(absoluteOffsets[2] ==
          Approx(expectedAbsoluteThird / scale).margin(0.01));
    CHECK(absoluteOffsets[3] ==
          Approx(expectedAbsoluteFourth / scale).margin(0.01));
    CHECK(tabbedWidth > 0.0);
    CHECK(needed.WinUIGetItemPeerIdentityForTesting(0) == peer);

    wxVector<int> invalidStops;
    invalidStops.push_back(48);
    invalidStops.push_back(16);
    CHECK_FALSE(needed.MSWSetTabStops(invalidStops));
    unsigned int runsAfterInvalid = 0;
    REQUIRE(needed.WinUIGetItemPresentationForTesting(
        0, nullptr, nullptr, &runsAfterInvalid, nullptr, nullptr));
    CHECK(runsAfterInvalid == tabRuns);

    wxVector<int> repeatingStop;
    repeatingStop.push_back(64);
    REQUIRE(needed.MSWSetTabStops(repeatingStop));
    double repeatingWidth = 0.0;
    REQUIRE(needed.WinUIGetItemPresentationForTesting(
        0, nullptr, nullptr, nullptr, &repeatingWidth, nullptr));
    CHECK(repeatingWidth > tabbedWidth);

    const wxVector<int> defaultStops;
    REQUIRE(needed.MSWSetTabStops(defaultStops));
    double defaultWidth = 0.0;
    wxVector<double> explicitDefaultOffsets;
    REQUIRE(needed.WinUIGetItemPresentationForTesting(
        0, nullptr, nullptr, nullptr, &defaultWidth, nullptr, nullptr,
        &explicitDefaultOffsets));
    REQUIRE(explicitDefaultOffsets.size() == 4);
    const int explicitDefaultInterval = dluToPixels(2);
    const int expectedExplicitDefaultSecond =
        ((firstRunWidth / explicitDefaultInterval) + 1) *
            explicitDefaultInterval;
    CHECK(explicitDefaultOffsets[1] ==
          Approx(expectedExplicitDefaultSecond / scale).margin(0.01));
    CHECK(explicitDefaultOffsets[1] !=
          Approx(neverSetOffsets[1]).margin(0.01));
    CHECK(defaultWidth < repeatingWidth);

    needed.SetHorizontalExtent(
        "A deliberately much wider horizontal extent than the item text");
    double enlargedWidth = 0.0;
    REQUIRE(needed.WinUIGetItemPresentationForTesting(
        0, nullptr, nullptr, nullptr, &enlargedWidth, nullptr));
    CHECK(enlargedWidth > defaultWidth);

    // This seam is a getter, not a hidden materialization trigger. Two
    // consecutive observations without an event-loop boundary must preserve
    // every production retry state and the observed tree identically.
    wxListBox::WinUIScrollMetricsForTesting passiveBefore;
    wxListBox::WinUIScrollMetricsForTesting passiveAfter;
    double passiveWidthBefore = -1.0;
    double passiveWidthAfter = -1.0;
    const bool passiveResultBefore =
        needed.WinUIGetScrollPresentationForTesting(
            nullptr, nullptr, &passiveWidthBefore, &passiveBefore);
    const bool passiveResultAfter =
        needed.WinUIGetScrollPresentationForTesting(
            nullptr, nullptr, &passiveWidthAfter, &passiveAfter);
    CHECK(passiveResultAfter == passiveResultBefore);
    CHECK(passiveWidthAfter == passiveWidthBefore);
    CHECK(passiveAfter.listVisualChildCount ==
          passiveBefore.listVisualChildCount);
    CHECK(passiveAfter.hasItemsPanelRoot ==
          passiveBefore.hasItemsPanelRoot);
    CHECK(passiveAfter.hasAuthoritativeScroll ==
          passiveBefore.hasAuthoritativeScroll);
    CHECK(passiveAfter.hasItemsStackPanel ==
          passiveBefore.hasItemsStackPanel);
    CHECK(passiveAfter.hasVirtualizingStackPanel ==
          passiveBefore.hasVirtualizingStackPanel);
    CHECK(passiveAfter.panelScrollOwnerResolvedFromAncestor ==
          passiveBefore.panelScrollOwnerResolvedFromAncestor);
    CHECK(passiveAfter.horizontalPresentationPending ==
          passiveBefore.horizontalPresentationPending);
    CHECK(passiveAfter.horizontalResetPending ==
          passiveBefore.horizontalResetPending);
    CHECK(passiveAfter.materializationRetryQueued ==
          passiveBefore.materializationRetryQueued);
    CHECK(passiveAfter.materializationAttempts ==
          passiveBefore.materializationAttempts);
    CHECK(passiveAfter.materializationRetriesRemaining ==
          passiveBefore.materializationRetriesRemaining);

    frame.ShowWithoutActivating();
    wxListBox::WinUIScrollMetricsForTesting scrollMetrics;
    double scrollableWidth = 0.0;
    const bool hasHorizontalRange =
        WaitFor("WinUI ListBox horizontal scroll range", [&]()
    {
        frame.Update();
        return needed.WinUIGetScrollPresentationForTesting(
                   nullptr, nullptr, &scrollableWidth, &scrollMetrics) &&
               scrollableWidth > 0.0 &&
               scrollMetrics.hasAuthoritativeScroll &&
               !scrollMetrics.horizontalPresentationPending &&
               !scrollMetrics.horizontalResetPending &&
               !scrollMetrics.materializationRetryQueued;
    }, 2000);
    CAPTURE(scrollableWidth,
            scrollMetrics.listVisualChildCount,
            scrollMetrics.hasItemsPanelRoot,
            scrollMetrics.hasAuthoritativeScroll,
            scrollMetrics.horizontalPresentationPending,
            scrollMetrics.horizontalResetPending,
            scrollMetrics.materializationRetryQueued,
            scrollMetrics.materializationAttempts,
            scrollMetrics.materializationRetriesRemaining,
            scrollMetrics.scrollExtentWidth,
            scrollMetrics.scrollViewportWidth,
            scrollMetrics.hasScrollContentPresenter,
            scrollMetrics.contentPresenterCanHorizontallyScroll,
            scrollMetrics.contentPresenterSizesContentToTemplatedParent,
            scrollMetrics.contentPresenterExtentWidth,
            scrollMetrics.contentPresenterViewportWidth,
            scrollMetrics.hasItemsStackPanel,
            scrollMetrics.hasVirtualizingStackPanel,
            scrollMetrics.hasOrientedPanel,
            scrollMetrics.panelCanHorizontallyScroll,
            scrollMetrics.panelScrollOwnerResolvedFromAncestor,
            scrollMetrics.panelScrollOwnerMatches,
            scrollMetrics.contentActualWidth,
            scrollMetrics.contentDesiredWidth,
            scrollMetrics.panelActualWidth,
            scrollMetrics.panelDesiredWidth,
            scrollMetrics.itemActualWidth,
            scrollMetrics.itemDesiredWidth,
            scrollMetrics.itemExplicitWidth);
    REQUIRE(hasHorizontalRange);
    CHECK(needed.GetSize() == wxSize(220, 120));
    CHECK(scrollMetrics.listVisualChildCount > 0);
    CHECK(scrollMetrics.hasItemsPanelRoot);
    CHECK(scrollMetrics.hasAuthoritativeScroll);
    CHECK_FALSE(scrollMetrics.horizontalPresentationPending);
    CHECK_FALSE(scrollMetrics.horizontalResetPending);
    CHECK_FALSE(scrollMetrics.materializationRetryQueued);
    CHECK(scrollMetrics.materializationRetriesRemaining <= 2);
    CHECK(scrollMetrics.hasScrollContentPresenter);
    CHECK(scrollMetrics.contentPresenterCanHorizontallyScroll);
    CHECK_FALSE(
        scrollMetrics.contentPresenterSizesContentToTemplatedParent);
    CHECK(scrollMetrics.hasItemsStackPanel);
    CHECK_FALSE(scrollMetrics.hasVirtualizingStackPanel);
    CHECK_FALSE(scrollMetrics.hasOrientedPanel);
    CHECK_FALSE(scrollMetrics.panelCanHorizontallyScroll);
    CHECK(scrollMetrics.panelScrollOwnerResolvedFromAncestor);
    CHECK(scrollMetrics.panelScrollOwnerMatches);
    CHECK(scrollMetrics.scrollExtentWidth >
          scrollMetrics.scrollViewportWidth);

    const unsigned int stableMaterializationAttempts =
        scrollMetrics.materializationAttempts;
    for ( unsigned int pass = 0; pass < 3; ++pass )
    {
        wxYield();
        frame.Update();
    }
    wxListBox::WinUIScrollMetricsForTesting stableMetrics;
    double stableScrollableWidth = 0.0;
    REQUIRE(needed.WinUIGetScrollPresentationForTesting(
        nullptr, nullptr, &stableScrollableWidth, &stableMetrics));
    CHECK(stableScrollableWidth > 0.0);
    CHECK(stableMetrics.materializationAttempts ==
          stableMaterializationAttempts);
    CHECK_FALSE(stableMetrics.materializationRetryQueued);
    CHECK(stableMetrics.materializationRetriesRemaining <= 2);

    needed.SetHorizontalExtent();
    double recomputedWidth = 0.0;
    REQUIRE(needed.WinUIGetItemPresentationForTesting(
        0, nullptr, nullptr, nullptr, &recomputedWidth, nullptr));
    CHECK(recomputedWidth < enlargedWidth);

    // ItemsPanelRoot survives its last item. Deleting the final item takes the
    // old==new==0 SetHorizontalExtent() path and must still remove the local
    // Width/MinWidth that supplied the former horizontal scroll range.
    needed.Delete(0);
    wxListBox::WinUIScrollMetricsForTesting deleteResetMetrics;
    double deleteResetWidth = -1.0;
    const bool deletedExtentReset =
        WaitFor("WinUI ListBox delete-last extent reset", [&]()
    {
        frame.Update();
        return needed.WinUIGetScrollPresentationForTesting(
                   nullptr, nullptr, &deleteResetWidth,
                   &deleteResetMetrics) &&
               deleteResetWidth <= 0.01 &&
               deleteResetMetrics.hasAuthoritativeScroll &&
               !deleteResetMetrics.horizontalPresentationPending &&
               !deleteResetMetrics.horizontalResetPending &&
               !deleteResetMetrics.materializationRetryQueued;
    }, 2000);
    CAPTURE(deleteResetWidth,
            deleteResetMetrics.listVisualChildCount,
            deleteResetMetrics.hasItemsPanelRoot,
            deleteResetMetrics.hasAuthoritativeScroll,
            deleteResetMetrics.horizontalPresentationPending,
            deleteResetMetrics.horizontalResetPending,
            deleteResetMetrics.materializationRetryQueued,
            deleteResetMetrics.materializationAttempts,
            deleteResetMetrics.materializationRetriesRemaining);
    REQUIRE(deletedExtentReset);
    CHECK(deleteResetMetrics.hasScrollContentPresenter);
    CHECK(deleteResetMetrics.hasItemsPanelRoot);
    CHECK(deleteResetMetrics.hasAuthoritativeScroll);
    CHECK_FALSE(deleteResetMetrics.horizontalPresentationPending);
    CHECK_FALSE(deleteResetMetrics.horizontalResetPending);
    CHECK_FALSE(deleteResetMetrics.materializationRetryQueued);
    CHECK(deleteResetMetrics.materializationRetriesRemaining <= 2);
    CHECK(deleteResetMetrics.hasItemsStackPanel);
    CHECK_FALSE(deleteResetMetrics.hasVirtualizingStackPanel);
    CHECK_FALSE(deleteResetMetrics.hasOrientedPanel);
    CHECK(deleteResetMetrics.panelScrollOwnerResolvedFromAncestor);
    CHECK(deleteResetMetrics.panelScrollOwnerMatches);

    // DoClear() performs the same reset before owner-item destructors can
    // destroy the control. Exercise it independently on the retained panel.
    REQUIRE(needed.Append("replacement") == 0);
    needed.SetHorizontalExtent(
        "Another deliberately much wider horizontal extent for clear");
    REQUIRE(WaitFor("WinUI ListBox clear setup extent", [&]()
    {
        frame.Update();
        double scrollableWidth = 0.0;
        return needed.WinUIGetScrollPresentationForTesting(
                   nullptr, nullptr, &scrollableWidth) &&
               scrollableWidth > 0.0;
    }, 2000));
    needed.Clear();
    wxListBox::WinUIScrollMetricsForTesting clearResetMetrics;
    double clearResetWidth = -1.0;
    const bool clearedExtentReset =
        WaitFor("WinUI ListBox clear extent reset", [&]()
    {
        frame.Update();
        return needed.WinUIGetScrollPresentationForTesting(
                   nullptr, nullptr, &clearResetWidth,
                   &clearResetMetrics) &&
               clearResetWidth <= 0.01 &&
               clearResetMetrics.hasAuthoritativeScroll &&
               !clearResetMetrics.horizontalPresentationPending &&
               !clearResetMetrics.horizontalResetPending &&
               !clearResetMetrics.materializationRetryQueued;
    }, 2000);
    CAPTURE(clearResetWidth,
            clearResetMetrics.listVisualChildCount,
            clearResetMetrics.hasItemsPanelRoot,
            clearResetMetrics.hasAuthoritativeScroll,
            clearResetMetrics.horizontalPresentationPending,
            clearResetMetrics.horizontalResetPending,
            clearResetMetrics.materializationRetryQueued,
            clearResetMetrics.materializationAttempts,
            clearResetMetrics.materializationRetriesRemaining);
    REQUIRE(clearedExtentReset);
    CHECK(clearResetMetrics.hasScrollContentPresenter);
    CHECK(clearResetMetrics.hasItemsPanelRoot);
    CHECK(clearResetMetrics.hasAuthoritativeScroll);
    CHECK_FALSE(clearResetMetrics.horizontalPresentationPending);
    CHECK_FALSE(clearResetMetrics.horizontalResetPending);
    CHECK_FALSE(clearResetMetrics.materializationRetryQueued);
    CHECK(clearResetMetrics.materializationRetriesRemaining <= 2);
    CHECK(clearResetMetrics.hasItemsStackPanel);
    CHECK_FALSE(clearResetMetrics.hasVirtualizingStackPanel);
    CHECK_FALSE(clearResetMetrics.hasOrientedPanel);
    CHECK(clearResetMetrics.panelScrollOwnerResolvedFromAncestor);
    CHECK(clearResetMetrics.panelScrollOwnerMatches);
    frame.Hide();

    wxListBox always(&frame, wxID_ANY, wxDefaultPosition,
                     wxSize(220, 120), 0, nullptr,
                     wxLB_ALWAYS_SB);
    REQUIRE(always.WinUIGetScrollPresentationForTesting(
        &horizontal, &vertical));
    CHECK(horizontal == 0); // ScrollBarVisibility::Disabled
    CHECK(vertical == 3);   // ScrollBarVisibility::Visible

    wxListBox never(&frame, wxID_ANY, wxDefaultPosition,
                    wxSize(220, 120), 0, nullptr,
                    wxLB_NO_SB);
    REQUIRE(never.WinUIGetScrollPresentationForTesting(
        &horizontal, &vertical));
    CHECK(horizontal == 0); // ScrollBarVisibility::Disabled
    CHECK(vertical == 0);   // ScrollBarVisibility::Disabled
}

#if wxUSE_OWNER_DRAWN

TEST_CASE("wxWinUI ListBox owner drawing keeps stable item identity and state",
          "[winui-itemmodel][winui-listmodel][listbox][ownerdraw]")
{
    wxFrame frame(nullptr, wxID_ANY, "owner-draw list owner",
                  wxPoint(-30000, -30000), wxSize(360, 260),
                  wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    wxWinUIOwnerDrawProbeState state;
    wxWinUIOwnerDrawProbeList list(&state);
    REQUIRE(list.Create(&frame, wxID_ANY, wxPoint(10, 10),
                         wxSize(300, 200), 0, nullptr,
                         wxLB_OWNERDRAW | wxLB_SORT));
    // Keep the explicit control size authoritative across frame.Show(). A
    // frame otherwise auto-expands its unique child, which would turn the
    // legitimate wx layout width into the frame's 358-pixel client width and
    // make a 300-DIP DPI oracle internally contradictory.
    wxBoxSizer * const ownerDrawSizer = new wxBoxSizer(wxVERTICAL);
    ownerDrawSizer->Add(&list, 0, wxFIXED_MINSIZE);
    frame.SetSizer(ownerDrawSizer);
    REQUIRE(frame.Layout());
    CHECK(list.GetSize() == wxSize(300, 200));

    REQUIRE(list.Append("beta") == 0);
    REQUIRE(list.Append("alpha") == 0);
    REQUIRE(list.GetCount() == 2);
    wxOwnerDrawn * const alphaItem = list.GetItem(0);
    wxOwnerDrawn * const betaItem = list.GetItem(1);
    REQUIRE(alphaItem);
    REQUIRE(betaItem);
    wxFont ownerFont = list.GetFont();
    ownerFont.SetWeight(wxFONTWEIGHT_BOLD);
    REQUIRE(list.SetFont(ownerFont));
    CHECK(alphaItem->GetFont() == list.GetFont());
    CHECK(betaItem->GetFont() == list.GetFont());
    const std::uint64_t betaId = list.WinUIGetItemIdForTesting(1);
    const std::uintptr_t betaPeer =
        list.WinUIGetItemPeerIdentityForTesting(1);

    betaItem->SetMarginWidth(list.FromDIP(12));
    list.SetString(1, "aardvark");
    CHECK(list.WinUIGetItemIdForTesting(0) == betaId);
    CHECK(list.WinUIGetItemPeerIdentityForTesting(0) == betaPeer);
    CHECK(list.GetItem(0) == betaItem);
    CHECK(list.GetItem(1) == alphaItem);
    CHECK(list.GetItemIndex(betaItem) == 0);
    CHECK(list.GetItemIndex(alphaItem) == 1);

    bool hasBitmap = false;
    wxSize bitmapPixels;
    wxRealPoint bitmapDIPs;
    double ownerContainerHeightDips = 0.0;
    double ownerContainerMinHeightDips = -1.0;
    double ownerContainerActualHeightDips = 0.0;
    REQUIRE(list.WinUIGetItemPresentationForTesting(
        0, &hasBitmap, &bitmapPixels, nullptr, nullptr, nullptr,
        &bitmapDIPs, nullptr, nullptr, nullptr, nullptr, nullptr,
        &ownerContainerHeightDips, &ownerContainerMinHeightDips,
        &ownerContainerActualHeightDips));
    CHECK(hasBitmap);
    CHECK(bitmapPixels.x > 0);
    CHECK(bitmapPixels.y > 0);
    const double bitmapScale = list.GetDPIScaleFactor();
    const int fixedOwnerDrawHeightPixels = list.GetCharHeight() + 2;
    CHECK(bitmapPixels.y == fixedOwnerDrawHeightPixels);
    CHECK(bitmapDIPs.x ==
          Approx(bitmapPixels.x / bitmapScale).margin(0.01));
    CHECK(bitmapDIPs.y ==
          Approx(fixedOwnerDrawHeightPixels / bitmapScale).margin(0.01));
    CHECK(ownerContainerHeightDips == Approx(bitmapDIPs.y).margin(0.01));
    CHECK(ownerContainerMinHeightDips == Approx(0.0));
    CHECK(state.measureCalls > 0);
    CHECK(state.drawCalls > 0);
    CHECK(state.baseDrawCalls > 0);
    CHECK(state.baseSelectedFont != 0);
    CHECK(state.ownerItemFont != 0);
    CHECK(state.baseSelectedOwnerItemFont);
    CHECK(state.ownerItemFont == reinterpret_cast<std::uintptr_t>(
          list.GetFont().GetHFONT()));
    CHECK(state.ownerItemFontHeight != 0);
    CHECK(state.baseSelectedFontHeight == state.ownerItemFontHeight);
    const wxSize drawFontPixels = state.lastFont.GetPixelSize();
    const wxSize ownerFontPixels = list.GetFont().GetPixelSize();
    CAPTURE(bitmapScale,
            list.GetDPI().x,
            list.GetDPI().y,
            state.lastFont.GetFractionalPointSize(),
            list.GetFont().GetFractionalPointSize(),
            drawFontPixels.x,
            drawFontPixels.y,
            ownerFontPixels.x,
            ownerFontPixels.y,
            state.lastFont.GetNativeFontInfoDesc().ToStdString(),
            list.GetFont().GetNativeFontInfoDesc().ToStdString());
    CHECK(state.lastFont == list.GetFont());
    CHECK(state.lastFont.GetWeight() == wxFONTWEIGHT_BOLD);

    frame.Show();
    wxRect ownerDrawRect;
    double xamlRasterizationScale = 0.0;
    REQUIRE(WaitFor("WinUI owner-draw ListBox item layout", [&]()
    {
        frame.Update();
        return list.GetItemRect(0, ownerDrawRect) &&
               ownerDrawRect.width > 0 && ownerDrawRect.height > 0 &&
               list.WinUIGetItemPresentationForTesting(
                   0, &hasBitmap, &bitmapPixels, nullptr, nullptr, nullptr,
                   &bitmapDIPs,
                   nullptr, nullptr, nullptr, nullptr, nullptr,
                   &ownerContainerHeightDips,
                   &ownerContainerMinHeightDips,
                   &ownerContainerActualHeightDips,
                   &xamlRasterizationScale) &&
               hasBitmap && xamlRasterizationScale > 0.0 &&
               ownerContainerActualHeightDips > 0.0;
    }, 2000));
    const double realizedScale = list.GetDPIScaleFactor();
    CAPTURE(bitmapScale,
            realizedScale,
            xamlRasterizationScale,
            bitmapPixels.y,
            bitmapDIPs.y,
            ownerContainerHeightDips,
            ownerContainerMinHeightDips,
            ownerContainerActualHeightDips,
            ownerDrawRect.y,
            ownerDrawRect.height);
    CHECK(realizedScale == Approx(xamlRasterizationScale).margin(0.01));
    CHECK(bitmapPixels.x / bitmapDIPs.x ==
          Approx(xamlRasterizationScale).margin(0.01));
    CHECK(bitmapPixels.y / bitmapDIPs.y ==
          Approx(xamlRasterizationScale).margin(0.01));
    CHECK(bitmapPixels.y == list.GetCharHeight() + 2);
    CHECK(ownerContainerHeightDips ==
          Approx((list.GetCharHeight() + 2) /
                 xamlRasterizationScale).margin(0.01));
    CHECK(ownerContainerActualHeightDips ==
          Approx(ownerContainerHeightDips).margin(0.01));
    CHECK(ownerDrawRect.height == Approx(bitmapPixels.y).margin(1));
    CHECK(betaItem->GetFont() == list.GetFont());
    CHECK(alphaItem->GetFont() == list.GetFont());
    CHECK(state.lastFont == list.GetFont());
    CHECK(state.baseSelectedOwnerItemFont);
    CHECK(state.baseSelectedFontHeight == state.ownerItemFontHeight);
    CHECK(list.RefreshItem(0));

    REQUIRE(list.WinUISetPeerSelectionForTesting(0, true));
    CHECK(state.lastStatus & wxOwnerDrawn::wxODSelected);

    list.Enable(false);
    CHECK(state.lastStatus & wxOwnerDrawn::wxODDisabled);
    list.Enable(true);

    list.SetFocus();
    REQUIRE(WaitFor("WinUI owner-draw ListBox item focus", [&]()
    {
        return list.WinUIFocusPeerItemForTesting(0);
    }, 2000));
    (void)list.RefreshItem(0);
    CHECK(state.lastStatus & wxOwnerDrawn::wxODHasFocus);

    const int beforeRTL = state.drawCalls;
    list.SetLayoutDirection(wxLayout_RightToLeft);
    REQUIRE(WaitFor("WinUI owner-draw ListBox RTL projection", [&]()
    {
        return state.drawCalls > beforeRTL &&
               state.lastDirection == wxLayout_RightToLeft;
    }, 2000));
    bool peerRTL = false;
    REQUIRE(list.WinUIGetPeerLayoutDirectionForTesting(&peerRTL));
    CHECK(peerRTL);

    const double controlWidthDIPs =
        list.WinUIGetControlWidthDIPsForTesting();
    CAPTURE(controlWidthDIPs);
    CHECK(list.GetSize() == wxSize(300, 200));
    CHECK(controlWidthDIPs == Approx(bitmapDIPs.x).margin(0.01));

    const int beforeTheme = state.drawCalls;
    REQUIRE(list.WinUISetPeerThemeForTesting(true));
    int darkTheme = -1;
    std::uint32_t darkForeground = 0;
    std::uint32_t darkPixel = 0;
    REQUIRE(WaitFor("WinUI owner-draw ListBox dark theme state", [&]()
    {
        frame.Update();
        return list.WinUIGetThemePresentationForTesting(
                   0, &darkTheme, &darkForeground, &darkPixel) &&
               darkTheme == 2 &&
               state.drawCalls > beforeTheme;
    }, 2000));
    CHECK((darkForeground >> 24) != 0);
    CHECK(darkPixel == 0xffc04020u);
    CHECK(state.lastStatus & wxOwnerDrawn::wxODSelected);

    const int beforeLightTheme = state.drawCalls;
    REQUIRE(list.WinUISetPeerThemeForTesting(false));
    int lightTheme = -1;
    std::uint32_t lightForeground = 0;
    std::uint32_t lightPixel = 0;
    REQUIRE(WaitFor("WinUI owner-draw ListBox light theme state", [&]()
    {
        frame.Update();
        return list.WinUIGetThemePresentationForTesting(
                   0, &lightTheme, &lightForeground, &lightPixel) &&
               lightTheme == 1 &&
               state.drawCalls > beforeLightTheme;
    }, 2000));
    CHECK((lightForeground >> 24) != 0);
    CHECK(lightPixel == 0xffc04020u);
    CHECK(state.lastStatus & wxOwnerDrawn::wxODSelected);
    CHECK(lightForeground != darkForeground);
    frame.Hide();
}

TEST_CASE("wxWinUI ListBox owner drawing contains reentry and destruction",
          "[winui-itemmodel][winui-listmodel][listbox][ownerdraw][lifetime]")
{
    wxFrame frame(nullptr, wxID_ANY, "owner-draw lifetime owner");

    wxWinUIOwnerDrawProbeState throwState;
    wxWinUIOwnerDrawProbeList throwOnce(&throwState);
    REQUIRE(throwOnce.Create(&frame, wxID_ANY, wxDefaultPosition,
                             wxSize(220, 120), 0, nullptr,
                             wxLB_OWNERDRAW));
    throwState.throwOnCreateOnce = true;
    bool creationExceptionObserved = false;
    try
    {
        (void)throwOnce.Append("throws once");
    }
    catch ( const winrt::hresult_error& )
    {
        creationExceptionObserved = true;
    }
    CHECK(creationExceptionObserved);
    CHECK_FALSE(throwState.throwOnCreateOnce);
    CHECK(throwOnce.GetCount() == 0);
    CHECK(throwOnce.WinUIGetPeerCountForTesting() == 0);
    REQUIRE(throwOnce.Append("recovered") == 0);
    CHECK(throwOnce.GetCount() == 1);
    CHECK(throwOnce.GetItem(0) != nullptr);
    CHECK(throwOnce.WinUIGetPeerCountForTesting() == 1);

    wxWinUIOwnerDrawProbeState destructorState;
    wxWinUIOwnerDrawProbeList *destructorDoomed =
        new wxWinUIOwnerDrawProbeList(&destructorState);
    REQUIRE(destructorDoomed->Create(
        &frame, wxID_ANY, wxDefaultPosition, wxSize(220, 120),
        0, nullptr, wxLB_OWNERDRAW));
    REQUIRE(destructorDoomed->Append("destructor deletes owner") == 0);
    REQUIRE(destructorDoomed->Append("must also be destroyed 1") == 1);
    REQUIRE(destructorDoomed->Append("must also be destroyed 2") == 2);
    wxListBox *destructorOwner = destructorDoomed;
    destructorState.ownerToDestroy = &destructorOwner;
    destructorState.destroyOwnerFromItemDestructor = true;
    destructorDoomed->DeleteOwnerItemForTesting(0);
    CHECK(destructorOwner == nullptr);
    CHECK(destructorState.itemDestructorCalls == 3);

    wxWinUIOwnerDrawProbeState clearDestructorState;
    wxWinUIOwnerDrawProbeList *clearDoomed =
        new wxWinUIOwnerDrawProbeList(&clearDestructorState);
    REQUIRE(clearDoomed->Create(
        &frame, wxID_ANY, wxDefaultPosition, wxSize(220, 120),
        0, nullptr, wxLB_OWNERDRAW));
    REQUIRE(clearDoomed->Append("clear deletes owner") == 0);
    REQUIRE(clearDoomed->Append("clear drains item 1") == 1);
    REQUIRE(clearDoomed->Append("clear drains item 2") == 2);
    wxListBox *clearOwner = clearDoomed;
    clearDestructorState.ownerToDestroy = &clearOwner;
    clearDestructorState.destroyOwnerFromItemDestructor = true;
    clearDoomed->ClearOwnerItemsForTesting();
    CHECK(clearOwner == nullptr);
    CHECK(clearDestructorState.itemDestructorCalls == 3);

    wxWinUIOwnerDrawProbeState reentryState;
    wxWinUIOwnerDrawProbeList reentrant(&reentryState);
    REQUIRE(reentrant.Create(&frame, wxID_ANY, wxDefaultPosition,
                             wxSize(220, 120), 0, nullptr,
                             wxLB_OWNERDRAW));
    REQUIRE(reentrant.Append("reentrant refresh") == 0);
    const std::uint64_t stableId =
        reentrant.WinUIGetItemIdForTesting(0);
    const std::uintptr_t stablePeer =
        reentrant.WinUIGetItemPeerIdentityForTesting(0);
    wxOwnerDrawn * const stableItem = reentrant.GetItem(0);
    const int beforeReentry = reentryState.drawCalls;
    reentryState.refreshReentrantly = true;
    (void)reentrant.RefreshItem(0);
    CHECK(reentryState.reenteredRefresh);
    CHECK(reentryState.drawCalls == beforeReentry + 1);
    CHECK(reentrant.WinUIGetItemIdForTesting(0) == stableId);
    CHECK(reentrant.WinUIGetItemPeerIdentityForTesting(0) == stablePeer);
    CHECK(reentrant.GetItem(0) == stableItem);

    const int beforeFailure = reentryState.drawCalls;
    reentryState.failWithHresultOnce = true;
    {
        wxLogNull noLog;
        (void)reentrant.RefreshItem(0);
    }
    CHECK_FALSE(reentryState.failWithHresultOnce);
    CHECK(reentryState.drawCalls >= beforeFailure + 2);
    CHECK(reentrant.WinUIGetItemIdForTesting(0) == stableId);
    CHECK(reentrant.GetItem(0) == stableItem);
    CHECK(reentrant.WinUIGetPeerCountForTesting() == 1);
    bool recoveredBitmap = false;
    REQUIRE(reentrant.WinUIGetItemPresentationForTesting(
        0, &recoveredBitmap, nullptr, nullptr, nullptr, nullptr));
    CHECK(recoveredBitmap);

    wxWinUIOwnerDrawProbeState destructionState;
    wxWinUIOwnerDrawProbeList *doomed =
        new wxWinUIOwnerDrawProbeList(&destructionState);
    REQUIRE(doomed->Create(&frame, wxID_ANY, wxDefaultPosition,
                           wxSize(220, 120), 0, nullptr,
                           wxLB_OWNERDRAW));
    REQUIRE(doomed->Append("destroy from OnDraw") == 0);
    wxListBox *ownerIdentity = doomed;
    destructionState.ownerToDestroy = &ownerIdentity;
    destructionState.destroyOwner = true;
    wxWinUIOwnerDrawProbeList * const invoking = doomed;
    doomed = nullptr;
    CHECK_FALSE(invoking->RefreshItem(0));
    CHECK(ownerIdentity == nullptr);
}

#endif // wxUSE_OWNER_DRAWN

#if wxUSE_CHECKLISTBOX

#if wxUSE_OWNER_DRAWN

TEST_CASE("wxWinUI CheckListBox redraws owner bitmap before one toggle event",
          "[winui-itemmodel][winui-listmodel][checklistbox][ownerdraw][uia]")
{
    wxFrame frame(nullptr, wxID_ANY, "check overlay owner");
    wxWinUICheckDrawProbeState state;
    wxWinUICheckDrawProbeList list(&state);
    REQUIRE(list.Create(&frame, wxID_ANY, wxDefaultPosition,
                        wxSize(240, 140)));
    REQUIRE(list.Append("checkable owner item") == 0);
    CHECK(list.GetItem(0) != nullptr);

    bool hasBitmap = false;
    bool hasCheckOverlay = false;
    wxSize bitmapPixels;
    REQUIRE(list.WinUIGetItemPresentationForTesting(
        0, &hasBitmap, &bitmapPixels, nullptr, nullptr,
        &hasCheckOverlay));
    CHECK(hasBitmap);
    CHECK(hasCheckOverlay);
    CHECK(bitmapPixels.x > 0);
    CHECK(bitmapPixels.y > 0);

    int events = 0;
    int drawsBeforeToggle = state.drawCalls;
    bool redrawnBeforeEvent = false;
    list.Bind(wxEVT_CHECKLISTBOX, [&](wxCommandEvent&)
    {
        ++events;
        redrawnBeforeEvent =
            state.drawCalls > drawsBeforeToggle &&
            state.lastChecked == list.IsChecked(0);
    });

    frame.ShowWithoutActivating();
    REQUIRE(WaitFor("WinUI checklist peer layout", [&]()
    {
        frame.Update();
        wxRect rect;
        return list.GetItemRect(0, rect);
    }, 2000));

    bool peerFocused = false;
    bool peerPointerTarget = false;
    REQUIRE(list.WinUIActivatePeerCheckForTesting(
        0, &peerFocused, &peerPointerTarget));
    CHECK(peerFocused);
    CHECK(peerPointerTarget);
    CHECK(list.IsChecked(0));
    CHECK(events == 1);
    CHECK(redrawnBeforeEvent);
    bool overlayFocused = false;
    REQUIRE(list.WinUIGetItemPresentationForTesting(
        0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, &overlayFocused));
    CHECK(overlayFocused);

    drawsBeforeToggle = state.drawCalls;
    redrawnBeforeEvent = false;
    REQUIRE(list.WinUITogglePeerViaAutomationForTesting(0));
    CHECK_FALSE(list.IsChecked(0));
    CHECK(events == 2);
    CHECK(redrawnBeforeEvent);

    list.SetSelection(0);
    drawsBeforeToggle = state.drawCalls;
    redrawnBeforeEvent = false;
    bool handled = false;
    REQUIRE(list.WinUIDispatchCheckKeyForTesting(32, &handled));
    CHECK(handled);
    CHECK(list.IsChecked(0));
    CHECK(events == 3);
    CHECK(redrawnBeforeEvent);

    drawsBeforeToggle = state.drawCalls;
    list.Check(0, false);
    CHECK_FALSE(list.IsChecked(0));
    CHECK(state.drawCalls > drawsBeforeToggle);
    CHECK_FALSE(state.lastChecked);
    CHECK(events == 3);
    frame.Hide();
}

TEST_CASE("wxWinUI CheckListBox restores focus after owner projection rebuild",
          "[winui-itemmodel][winui-listmodel][checklistbox][ownerdraw][uia]")
{
    wxFrame frame(nullptr, wxID_ANY, "check overlay rebuild owner",
                  wxDefaultPosition, wxSize(360, 260));
    wxWinUICheckDrawProbeState state;
    state.projectBitmap = false;
    wxWinUICheckDrawProbeList list(&state);
    REQUIRE(list.Create(&frame, wxID_ANY, wxDefaultPosition,
                        wxSize(240, 140)));
    REQUIRE(list.Append("transitioning check owner item") == 0);
    const std::uint64_t stableId = list.WinUIGetItemIdForTesting(0);
    const std::uintptr_t stablePeer =
        list.WinUIGetItemPeerIdentityForTesting(0);
    REQUIRE(stableId != 0);
    REQUIRE(stablePeer != 0);

    frame.ShowWithoutActivating();
    REQUIRE(WaitFor("WinUI direct checklist peer layout", [&]()
    {
        frame.Update();
        wxRect rect;
        return list.GetItemRect(0, rect);
    }, 2000));

    bool peerFocused = false;
    bool peerPointerTarget = false;
    REQUIRE(list.WinUIActivatePeerCheckForTesting(
        0, &peerFocused, &peerPointerTarget));
    REQUIRE(peerFocused);
    REQUIRE(peerPointerTarget);

    // Transition the focused CheckBox from direct ListViewItem content to the
    // owner-draw Grid. Focus can only be restored after the Grid is attached.
    state.projectBitmap = true;
    REQUIRE(list.RefreshItem(0));
    REQUIRE(WaitFor("WinUI checklist rebuilt overlay focus", [&]()
    {
        frame.Update();
        bool hasBitmap = false;
        bool overlayFocused = false;
        return list.WinUIGetItemPresentationForTesting(
                   0, &hasBitmap, nullptr, nullptr, nullptr, nullptr,
                   nullptr, nullptr, &overlayFocused) &&
               hasBitmap && overlayFocused;
    }, 2000));
    CHECK(list.WinUIGetItemIdForTesting(0) == stableId);
    CHECK(list.WinUIGetItemPeerIdentityForTesting(0) == stablePeer);

    // Exercise the inverse transition too. The CheckBox is still focused and
    // must first leave the Grid before it can become direct Content again.
    state.projectBitmap = false;
    REQUIRE(list.RefreshItem(0));
    REQUIRE(WaitFor("WinUI checklist direct overlay focus restored", [&]()
    {
        frame.Update();
        bool hasBitmap = true;
        bool hasCheckOverlay = false;
        bool overlayFocused = false;
        return list.WinUIGetItemPresentationForTesting(
                   0, &hasBitmap, nullptr, nullptr, nullptr,
                   &hasCheckOverlay, nullptr, nullptr, &overlayFocused) &&
               !hasBitmap && hasCheckOverlay && overlayFocused;
    }, 2000));
    CHECK(list.WinUIGetItemIdForTesting(0) == stableId);
    CHECK(list.WinUIGetItemPeerIdentityForTesting(0) == stablePeer);
    frame.Hide();
}

#endif // wxUSE_OWNER_DRAWN

TEST_CASE("wxWinUI CheckListBox preserves check identity and client data",
          "[winui-itemmodel][winui-listmodel][checklistbox]")
{
    wxFrame frame(nullptr, wxID_ANY, "check list delta owner");
    wxCheckListBox list(&frame, wxID_ANY, wxDefaultPosition,
                        wxDefaultSize, 0, nullptr, wxLB_SORT);
    list.Append("aaa");
    list.Append("Aaa");
    list.Append("AAA");
    const std::uintptr_t upperPeer =
        list.WinUIGetItemPeerIdentityForTesting(0);
    const std::uintptr_t titlePeer =
        list.WinUIGetItemPeerIdentityForTesting(1);
    const std::uintptr_t lowerPeer =
        list.WinUIGetItemPeerIdentityForTesting(2);
    REQUIRE(upperPeer != 0);
    REQUIRE(titlePeer != 0);
    REQUIRE(lowerPeer != 0);

    int destroyed = 0;
    int toggleEvents = 0;
    wxClientData *eventClientObject = nullptr;
    wxString eventString;
    list.Bind(
        wxEVT_CHECKLISTBOX,
        [&](wxCommandEvent& event)
        {
            ++toggleEvents;
            eventClientObject = event.GetClientObject();
            eventString = event.GetString();
        });

    list.Check(2);
    CHECK(list.WinUIGetItemPeerIdentityForTesting(0) == upperPeer);
    CHECK(list.WinUIGetItemPeerIdentityForTesting(1) == titlePeer);
    CHECK(list.WinUIGetItemPeerIdentityForTesting(2) == lowerPeer);
    list.SetSelection(2);
    list.SetClientObject(
        2, new wxWinUIListCountedClientData(&destroyed));
    const std::uint64_t checkedId =
        list.WinUIGetItemIdForTesting(2);
    const std::uintptr_t checkedPeer =
        list.WinUIGetItemPeerIdentityForTesting(2);
    CHECK(checkedPeer == lowerPeer);
    CHECK(list.WinUIGetPeerCheckForTesting(2));
    CHECK(toggleEvents == 0);

    list.SetString(2, "AA");
    CHECK(list.WinUIGetItemIdForTesting(0) == checkedId);
    CHECK(list.WinUIGetItemPeerIdentityForTesting(0) == checkedPeer);
    CHECK(list.IsChecked(0));
    CHECK(list.GetSelection() == 0);
    CHECK(list.GetClientObject(0) != nullptr);
    CHECK(list.WinUIGetPeerCheckForTesting(0));
    CHECK(toggleEvents == 0);

    REQUIRE(list.WinUITogglePeerViaAutomationForTesting(0));
    CHECK_FALSE(list.IsChecked(0));
    CHECK(toggleEvents == 1);
    CHECK(eventClientObject == list.GetClientObject(0));
    CHECK(eventString == "AA");

    REQUIRE(list.WinUITogglePeerWithKeyboardForTesting(0));
    CHECK(list.IsChecked(0));
    CHECK(list.WinUIGetPeerCheckForTesting(0));
    CHECK(toggleEvents == 2);

    list.Check(0, false);
    CHECK_FALSE(list.IsChecked(0));
    CHECK(toggleEvents == 2);
    list.Delete(0);
    CHECK(destroyed == 1);
}

TEST_CASE("wxWinUI CheckListBox keyboard matches MSW selected-item semantics",
          "[winui-itemmodel][winui-listmodel][checklistbox][keyboard][lifetime]")
{
    // Decode the same raw virtual key under explicit layouts without changing
    // the active desktop layout or synthesizing physical keyboard input.
    wxWinUITestKeyboardLayout usLayout(L"00000409");
    wxWinUITestKeyboardLayout frLayout(L"0000040C");
    REQUIRE(static_cast<bool>(usLayout));
    REQUIRE(static_cast<bool>(frLayout));

    wxFrame frame(nullptr, wxID_ANY, "check keyboard owner");
    wxCheckListBox list(&frame, wxID_ANY, wxDefaultPosition,
                        wxDefaultSize, 0, nullptr, wxLB_MULTIPLE);
    list.Append("zero");
    list.Append("middle");
    list.Append("two");
    list.SetSelection(0);
    list.SetSelection(2);

    wxVector<int> eventItems;
    wxArrayString eventStrings;
    bool deleteMiddleOnFirstEvent = true;
    list.Bind(wxEVT_CHECKLISTBOX, [&](wxCommandEvent& event)
    {
        eventItems.push_back(event.GetInt());
        eventStrings.push_back(event.GetString());
        if ( deleteMiddleOnFirstEvent )
        {
            deleteMiddleOnFirstEvent = false;
            list.Delete(1);
        }
    });

    bool handled = true;
    REQUIRE(list.WinUIDispatchCheckKeyForTesting(
        'X', &handled, false, usLayout.GetValue()));
    CHECK_FALSE(handled);
    CHECK(eventItems.empty());

    REQUIRE(list.WinUIDispatchCheckKeyForTesting(
        VK_OEM_PLUS, &handled, true, usLayout.GetValue()));
    CHECK(handled);
    REQUIRE(list.GetCount() == 2);
    CHECK(list.GetString(0) == "zero");
    CHECK(list.GetString(1) == "two");
    CHECK(list.IsChecked(0));
    CHECK(list.IsChecked(1));
    REQUIRE(eventItems.size() == 2);
    CHECK(eventItems[0] == 0);
    CHECK(eventItems[1] == 1);
    CHECK(eventStrings[0] == "zero");
    CHECK(eventStrings[1] == "two");

    wxArrayInt selections;
    REQUIRE(list.GetSelections(selections) == 2);
    CHECK(selections[0] == 0);
    CHECK(selections[1] == 1);

    REQUIRE(list.WinUIDispatchCheckKeyForTesting(
        VK_OEM_MINUS, &handled, false, usLayout.GetValue()));
    CHECK(handled);
    CHECK_FALSE(list.IsChecked(0));
    CHECK_FALSE(list.IsChecked(1));

    // Exercise the numpad VirtualKey values through the very same decoder.
    REQUIRE(list.WinUIDispatchCheckKeyForTesting(107, &handled));
    CHECK(handled);
    CHECK(list.IsChecked(0));
    CHECK(list.IsChecked(1));
    REQUIRE(list.WinUIDispatchCheckKeyForTesting(109, &handled));
    CHECK(handled);
    CHECK_FALSE(list.IsChecked(0));
    CHECK_FALSE(list.IsChecked(1));

    // Exercise the raw OEM keys and modifier decoder used by PreviewKeyDown.
    REQUIRE(list.WinUIDispatchCheckKeyForTesting(
        VK_OEM_PLUS, &handled, false, usLayout.GetValue()));
    CHECK_FALSE(handled); // '=' is not the checklist '+' command.
    CHECK_FALSE(list.IsChecked(0));
    CHECK_FALSE(list.IsChecked(1));
    REQUIRE(list.WinUIDispatchCheckKeyForTesting(
        VK_OEM_PLUS, &handled, true, usLayout.GetValue()));
    CHECK(handled);
    CHECK(list.IsChecked(0));
    CHECK(list.IsChecked(1));
    REQUIRE(list.WinUIDispatchCheckKeyForTesting(
        VK_OEM_MINUS, &handled, true, usLayout.GetValue()));
    CHECK_FALSE(handled); // '_' is not the checklist '-' command.
    CHECK(list.IsChecked(0));
    CHECK(list.IsChecked(1));
    REQUIRE(list.WinUIDispatchCheckKeyForTesting(
        VK_OEM_MINUS, &handled, false, usLayout.GetValue()));
    CHECK(handled);
    CHECK_FALSE(list.IsChecked(0));
    CHECK_FALSE(list.IsChecked(1));

    REQUIRE(list.WinUIDispatchCheckKeyForTesting(32, &handled));
    CHECK(handled);
    CHECK(list.IsChecked(0));
    CHECK(list.IsChecked(1));

    // On fr-FR the physical '6' key produces '-' without Shift and '6' with
    // Shift. This is the regression case that hard-coded US OEM keys miss.
    REQUIRE(list.WinUIDispatchCheckKeyForTesting(
        '6', &handled, false, frLayout.GetValue()));
    CHECK(handled);
    CHECK_FALSE(list.IsChecked(0));
    CHECK_FALSE(list.IsChecked(1));
    REQUIRE(list.WinUIDispatchCheckKeyForTesting(
        '6', &handled, true, frLayout.GetValue()));
    CHECK_FALSE(handled);
    CHECK_FALSE(list.IsChecked(0));
    CHECK_FALSE(list.IsChecked(1));
    REQUIRE(list.WinUIDispatchCheckKeyForTesting(
        VK_OEM_MINUS, &handled, false, frLayout.GetValue()));
    CHECK_FALSE(handled);
    REQUIRE(list.WinUIDispatchCheckKeyForTesting(
        VK_OEM_PLUS, &handled, true, frLayout.GetValue()));
    CHECK(handled);
    CHECK(list.IsChecked(0));
    CHECK(list.IsChecked(1));

    REQUIRE(eventItems.size() == 18);
    for ( std::size_t n = 0; n < eventItems.size(); n += 2 )
    {
        CHECK(eventItems[n] == 0);
        CHECK(eventItems[n + 1] == 1);
        CHECK(eventStrings[n] == "zero");
        CHECK(eventStrings[n + 1] == "two");
    }

    list.SetSelection(wxNOT_FOUND);
    const std::size_t eventsBeforeNoSelection = eventItems.size();
    REQUIRE(list.WinUIDispatchCheckKeyForTesting(32, &handled));
    CHECK(handled);
    CHECK(eventItems.size() == eventsBeforeNoSelection);

    wxCheckListBox single(&frame, wxID_ANY);
    single.Append("single zero");
    single.Append("single one");
    single.SetSelection(1);
    int singleEvents = 0;
    int singleEventItem = wxNOT_FOUND;
    single.Bind(wxEVT_CHECKLISTBOX, [&](wxCommandEvent& event)
    {
        ++singleEvents;
        singleEventItem = event.GetInt();
    });
    REQUIRE(single.WinUIDispatchCheckKeyForTesting(
        VK_OEM_PLUS, &handled, true, usLayout.GetValue()));
    CHECK(handled);
    CHECK_FALSE(single.IsChecked(0));
    CHECK(single.IsChecked(1));
    CHECK(singleEvents == 1);
    CHECK(singleEventItem == 1);
}

TEST_CASE("wxWinUI CheckListBox toggle callback is destruction-safe",
          "[winui-itemmodel][winui-listmodel][checklistbox][lifetime]")
{
    wxFrame frame(nullptr, wxID_ANY, "check callback owner");
    wxCheckListBox *list =
        new wxCheckListBox(&frame, wxID_ANY);
    list->Append("delete from event");
    list->SetSelection(0);

    bool destroyedInCallback = false;
    list->Bind(
        wxEVT_CHECKLISTBOX,
        [&list, &destroyedInCallback](wxCommandEvent&)
        {
            destroyedInCallback = true;
            delete list;
            list = nullptr;
        });

    REQUIRE(list->WinUITogglePeerWithKeyboardForTesting(0));
    CHECK(destroyedInCallback);
    CHECK(list == nullptr);
}

#endif // wxUSE_CHECKLISTBOX

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_LISTBOX
