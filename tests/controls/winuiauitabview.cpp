/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuiauitabview.cpp
// Purpose:     Native AUI TabView headers and authoritative notebook commands
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_AUI

#include "wx/app.h"
#include "wx/aui/auibook.h"
#include "wx/evtloop.h"
#include "wx/frame.h"
#include "wx/panel.h"
#include "wx/sizer.h"
#include "wx/scopeguard.h"
#include "wx/stopwatch.h"
#include "wx/weakref.h"
#include "wx/winui/private/tlwhost.h"
#ifdef WXAUI_NOTEBOOK_TEST_SUPPORT
#include "../winui/test-support/aui-notebook-test-access.h"
#endif
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <memory>

namespace
{
namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace A = X::Automation;

template<typename Predicate>
bool WaitAui(Predicate predicate)
{
    auto* const loop = wxEventLoopBase::GetActive();
    if ( !loop || !wxTheApp ) return false;
    wxStopWatch elapsed;
    while ( elapsed.Time() < 1500 )
    {
        if ( predicate() ) return true;
        wxTheApp->ProcessPendingEvents();
        if ( wxEventLoopBase::GetActive() != loop ) return false;
        if ( predicate() ) return true;
        if ( loop->DispatchTimeout(5) == 0 ) return false;
    }
    return predicate();
}

wxPoint AuiPosition()
{
    wchar_t desktop[256]{};
    DWORD needed = 0;
    if ( ::GetUserObjectInformationW(::GetThreadDesktop(::GetCurrentThreadId()),
            UOI_NAME, desktop, sizeof(desktop), &needed) &&
         wxString(desktop).StartsWith("wxWinUITest-") ) return wxPoint(80, 80);
    return wxPoint(-30000, -30000);
}

template<typename T>
T FindAuiPart(const X::DependencyObject& root, const wchar_t* name)
{
    if ( !root ) return nullptr;
    if ( const auto part = root.try_as<T>(); part && part.Name() == name ) return part;
    const int count = X::Media::VisualTreeHelper::GetChildrenCount(root);
    for ( int i = 0; i < count; ++i )
        if ( auto found = FindAuiPart<T>(
                X::Media::VisualTreeHelper::GetChild(root, i), name) ) return found;
    return nullptr;
}

C::TabView AuiView(wxAuiTabCtrl* tabs)
{
    auto* const host = tabs ? wxWinUITopLevelHost::FindSlotOwner(tabs) : nullptr;
    auto* const slot = host ? host->FindSlot(tabs) : nullptr;
    return slot ? slot->GetContent().try_as<C::TabView>() : nullptr;
}

C::TabViewItem AuiItem(const C::TabView& view, const wxString& caption)
{
    if ( view )
        for ( const auto& item : view.TabItems() )
            if ( const auto tab = item.try_as<C::TabViewItem>(); tab &&
                 wxString(A::AutomationProperties::GetName(tab).c_str()) == caption ) return tab;
    return nullptr;
}

A::Provider::ISelectionItemProvider AuiSelection(const C::TabViewItem& item)
{
    const auto peer = A::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(item);
    return peer ? peer.GetPattern(A::Peers::PatternInterface::SelectionItem)
                     .try_as<A::Provider::ISelectionItemProvider>() : nullptr;
}

A::Provider::IInvokeProvider AuiInvoke(const C::Button& button)
{
    const auto peer = A::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(button);
    return peer ? peer.GetPattern(A::Peers::PatternInterface::Invoke)
                     .try_as<A::Provider::IInvokeProvider>() : nullptr;
}

struct AuiFixture
{
    AuiFixture(bool rtl = false, bool bottom = false)
    {
        frame = std::make_unique<wxFrame>(nullptr, wxID_ANY,
            "Native AUI tabs", AuiPosition(), wxSize(700, 420));
        frame->SetLayoutDirection(rtl ? wxLayout_RightToLeft : wxLayout_LeftToRight);
        const long style = (bottom ? wxAUI_NB_BOTTOM : wxAUI_NB_TOP) |
            wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_SPLIT | wxAUI_NB_CLOSE_ON_ALL_TABS;
        book = new wxAuiNotebook(frame.get(), wxID_ANY, wxDefaultPosition,
                                 wxDefaultSize, style);
        auto* const sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(book, 1, wxEXPAND);
        frame->SetSizer(sizer);
        first = new wxPanel(book);
        second = new wxPanel(book);
        third = new wxPanel(book);
        REQUIRE(book->AddPage(first, "First", true));
        REQUIRE(book->AddPage(second, "Second"));
        REQUIRE(book->AddPage(third, "Third"));
        frame->ShowWithoutActivating();
        REQUIRE(WaitAui([&]
        {
            int index = -1;
            if ( !book->FindTab(first, &tabs, &index) ) return false;
            view = AuiView(tabs);
            const auto tab = AuiItem(view, "Third");
            return view && view.IsLoaded() && view.TabItems().Size() == 3 &&
                   tab && tab.IsLoaded() && tab.ActualHeight() >= 32;
        }));
    }

    std::unique_ptr<wxFrame> frame;
    wxAuiNotebook* book = nullptr;
    wxAuiTabCtrl* tabs = nullptr;
    wxWindow* first = nullptr;
    wxWindow* second = nullptr;
    wxWindow* third = nullptr;
    C::TabView view{nullptr};
};
} // namespace

TEST_CASE("WinUIAui::NativeSelectionVetoGeometryAndRetainedHeaders",
          "[winui-aui-native]")
{
    const bool rtl = GENERATE(false, true);
    const bool bottom = GENERATE(false, true);
    AuiFixture fixture(rtl, bottom);
    const auto first = AuiItem(fixture.view, "First");
    const auto second = AuiItem(fixture.view, "Second");
    REQUIRE(first);
    REQUIRE(second);
    const auto provider = AuiSelection(second);
    REQUIRE(provider);
    int changing = 0, changed = 0;
    bool veto = true;
    fixture.book->Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGING, [&](wxAuiNotebookEvent& event)
    {
        CHECK(event.GetEventObject() == fixture.book);
        CHECK(event.GetSelection() == 1);
        ++changing;
        if ( veto ) event.Veto();
    });
    fixture.book->Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGED, [&](wxAuiNotebookEvent&)
    {
        ++changed;
    });
    provider.Select();
    REQUIRE(WaitAui([&] { return changing == 1 && fixture.view.SelectedItem() == first; }));
    CHECK(changed == 0);
    CHECK(fixture.book->GetSelection() == 0);
    CHECK(fixture.first->IsShown());
    CHECK_FALSE(fixture.second->IsShown());
    veto = false;
    provider.Select();
    REQUIRE(WaitAui([&] { return fixture.book->GetSelection() == 1 && changed == 1; }));
    CHECK(changing == 2);
    CHECK(fixture.view.SelectedItem() == second);
    CHECK(fixture.second->IsShown());
    CHECK_FALSE(fixture.first->IsShown());

    fixture.book->ChangeSelection(0);
    REQUIRE(WaitAui([&] { return fixture.view.SelectedItem() == first; }));
    CHECK(changing == 2);
    CHECK(changed == 1);
    REQUIRE(fixture.book->SetPageText(1, "Renamed second"));
    REQUIRE(WaitAui([&] { return AuiItem(fixture.view, "Renamed second") == second; }));

    // Public AUI hit testing uses the exact native item rectangle (including
    // RTL), not a separately painted GDI tab measurement.
    REQUIRE(WaitAui([&]
    {
        const auto& info = fixture.tabs->GetPages()[1];
        return !info.rect.IsEmpty();
    }));
    const auto& rect = fixture.tabs->GetPages()[1].rect;
    const auto hit = fixture.tabs->TabHitTest(rect.GetPosition() + rect.GetSize() / 2);
    CHECK(hit.window == fixture.second);
    CHECK(hit.pos == 1);
    CHECK(AuiItem(fixture.view, "First") == first);
}

TEST_CASE("WinUIAui::NativeCloseVetoAndRetiredProvider",
          "[winui-aui-native]")
{
    AuiFixture fixture;
    const auto second = AuiItem(fixture.view, "Second");
    const auto retainedSelection = AuiSelection(second);
    REQUIRE(retainedSelection);
    C::Button close{nullptr};
    REQUIRE(WaitAui([&]
    {
        close = FindAuiPart<C::Button>(second, L"CloseButton");
        return close && close.IsLoaded() && close.ActualWidth() > 0;
    }));
    const auto invoke = AuiInvoke(close);
    REQUIRE(invoke);
    bool veto = true;
    int closing = 0, closed = 0;
    fixture.book->Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSE, [&](wxAuiNotebookEvent& event)
    {
        CHECK(event.GetSelection() == 1);
        ++closing;
        if ( veto ) event.Veto();
    });
    fixture.book->Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSED, [&](wxAuiNotebookEvent&)
    {
        ++closed;
    });
    invoke.Invoke();
    CHECK(closing == 1);
    CHECK(closed == 0);
    CHECK(fixture.book->GetPageCount() == 3);
    CHECK(AuiItem(fixture.view, "Second") == second);
    veto = false;
    invoke.Invoke();
    REQUIRE(WaitAui([&]
    {
        return fixture.book->GetPageCount() == 2 &&
               fixture.view.TabItems().Size() == 2 && !second.IsLoaded();
    }));
    CHECK(closing == 2);
    CHECK(closed == 1);
    const int selected = fixture.book->GetSelection();
    try { retainedSelection.Select(); } catch ( const winrt::hresult_error& ) {}
    CHECK(fixture.book->GetSelection() == selected);
    CHECK(fixture.book->GetPageCount() == 2);
}

TEST_CASE("WinUIAui::MakeTabVisibleUsesNativeScrollViewport",
          "[winui-aui-native]")
{
    const bool rtl = GENERATE(false, true);
    AuiFixture fixture(rtl);
    for ( int i = 0; i < 24; ++i )
        REQUIRE(fixture.book->AddPage(new wxPanel(fixture.book),
            wxString::Format("Long overflow page number %02d", i)));
    fixture.book->SetSize(0, 0, 330, 300);
    fixture.book->Layout();
    C::ScrollViewer scroll{nullptr};
    REQUIRE(WaitAui([&]
    {
        scroll = FindAuiPart<C::ScrollViewer>(fixture.view, L"ScrollViewer");
        return fixture.view.TabItems().Size() == fixture.tabs->GetPageCount() &&
               scroll && scroll.IsLoaded() && scroll.ScrollableWidth() > 0;
    }));
    const int last = int(fixture.tabs->GetPageCount()) - 1;
    const auto lastPeer = fixture.view.TabItems().GetAt(last).as<C::TabViewItem>();
    fixture.tabs->MakeTabVisible(last, fixture.tabs);
    const bool shown = WaitAui([&]
    {
        return scroll.HorizontalOffset() > 0 && lastPeer.IsLoaded() &&
               fixture.tabs->GetTabOffset() > 0;
    });
    INFO("offset=" << scroll.HorizontalOffset() << " range=" << scroll.ScrollableWidth()
         << " viewport=" << scroll.ViewportWidth() << " tabOffset=" << fixture.tabs->GetTabOffset()
         << " lastLoaded=" << lastPeer.IsLoaded() << " lastWidth=" << lastPeer.ActualWidth());
    INFO("sameView=" << (AuiView(fixture.tabs) == fixture.view)
         << " samePeer=" << (fixture.view.TabItems().GetAt(last) == lastPeer)
         << " oldScrollLoaded=" << scroll.IsLoaded());
    REQUIRE(shown);
    wxStopWatch settled;
    REQUIRE(WaitAui([&] { return settled.Time() >= 150; }));
    wxInfoDC dc(fixture.tabs);
    CHECK(fixture.tabs->IsTabVisible(last,
        int(fixture.tabs->GetTabOffset()), &dc, fixture.tabs));
    CHECK(fixture.view.TabItems().GetAt(last) == lastPeer);
    // Showing a tab is not a selection command.
    CHECK(fixture.book->GetSelection() == 0);
    // A completed reveal must not keep pulling back an explicit native scroll.
    // The TabView may still bring its selected first tab into view, a few
    // pixels from the start: 4 were seen on the CI machine, against the
    // thousands of the revealed tab.
    const double revealedOffset = scroll.HorizontalOffset();
    const auto atStart = [&] { return scroll.HorizontalOffset() <= 8; };
    REQUIRE(scroll.ChangeView(0.0, nullptr, nullptr, true));
    const bool backToStart = WaitAui(atStart);
    INFO("revealed at " << revealedOffset << ", after the explicit scroll at "
         << scroll.HorizontalOffset());
    REQUIRE(backToStart);
    settled.Start();
    REQUIRE(WaitAui([&] { return settled.Time() >= 100; }));
    CHECK(atStart());

    // A later request supersedes pending realization of the previous target.
    fixture.tabs->MakeTabVisible(last, fixture.tabs);
    fixture.tabs->MakeTabVisible(0, fixture.tabs);
    settled.Start();
    REQUIRE(WaitAui([&] { return settled.Time() >= 150; }));
    CHECK(fixture.tabs->IsTabVisible(0,
        int(fixture.tabs->GetTabOffset()), &dc, fixture.tabs));
    CHECK(fixture.book->GetSelection() == 0);
}

TEST_CASE("WinUIAui::PendingRevealYieldsToReentrantLayoutAndRetirement",
          "[winui-aui-native][lifetime]")
{
    const bool retire = GENERATE(false, true);
    AuiFixture fixture;
    for ( int i = 0; i < 18; ++i )
        REQUIRE(fixture.book->AddPage(new wxPanel(fixture.book),
            wxString::Format("Overflow pending page %02d", i)));
    fixture.book->SetSize(0, 0, 330, 300);
    REQUIRE(WaitAui([&]
    {
        return fixture.view.TabItems().Size() == fixture.tabs->GetPageCount();
    }));
    const int last = int(fixture.tabs->GetPageCount()) - 1;
    bool armed = true;
    int callbacks = 0;
    const auto token = fixture.view.LayoutUpdated([&](const auto&, const auto&)
    {
        if ( !armed ) return;
        armed = false;
        ++callbacks;
        if ( retire ) fixture.book->DeletePage(last);
        fixture.tabs->MakeTabVisible(0, fixture.tabs);
    });
    const auto revoke = wxMakeGuard([&] { fixture.view.LayoutUpdated(token); });
    fixture.view.InvalidateMeasure();
    fixture.tabs->MakeTabVisible(last, fixture.tabs);
    REQUIRE(WaitAui([&] { return callbacks == 1; }));
    wxStopWatch settled;
    REQUIRE(WaitAui([&] { return settled.Time() >= 150; }));
    wxInfoDC dc(fixture.tabs);
    CHECK(fixture.tabs->IsTabVisible(0,
        int(fixture.tabs->GetTabOffset()), &dc, fixture.tabs));
    CHECK(fixture.book->GetSelection() == 0);
    CHECK(fixture.book->GetPageCount() == size_t(retire ? last : last + 1));
}

TEST_CASE("WinUIAui::NativeCallbacksMayDestroyNotebook",
          "[winui-aui-native][lifetime]")
{
    const bool close = GENERATE(false, true);
    AuiFixture fixture;
    wxWeakRef<wxAuiNotebook> weak(fixture.book);
    const auto second = AuiItem(fixture.view, "Second");
    const auto selection = AuiSelection(second);
    REQUIRE(selection);
    int callbacks = 0;
    fixture.book->Bind(close ? wxEVT_AUINOTEBOOK_PAGE_CLOSE : wxEVT_AUINOTEBOOK_PAGE_CHANGING,
        [&](wxAuiNotebookEvent&)
        {
            ++callbacks;
            delete fixture.book;
            fixture.book = nullptr;
        });
    if ( close )
    {
        const auto button = FindAuiPart<C::Button>(second, L"CloseButton");
        REQUIRE(button);
        const auto invoke = AuiInvoke(button);
        REQUIRE(invoke);
        invoke.Invoke();
    }
    else selection.Select();
    CHECK_FALSE(weak);
    REQUIRE(WaitAui([&] { return !second.IsLoaded(); }));
    CHECK(callbacks == 1);
    try { selection.Select(); } catch ( const winrt::hresult_error& ) {}
    CHECK(callbacks == 1);
}

TEST_CASE("WinUIAui::PaintNeverPublishesReentrantNativeHeaders",
          "[winui-aui-native][lifetime]")
{
    AuiFixture fixture;
    const auto first = AuiItem(fixture.view, "First");
    REQUIRE(first);
    wxWeakRef<wxAuiNotebook> weak(fixture.book);
    bool sendingPaint = false;
    int changes = 0;
    const auto property = C::TabViewItem::HeaderProperty();
    const auto token = first.RegisterPropertyChangedCallback(property,
        [&](const auto&, const auto&)
        {
            ++changes;
            CHECK_FALSE(sendingPaint);
            // The explicit pre-paint Update phase is allowed to run callbacks
            // which destroy the notebook. The WM_PAINT phase must never do so.
            if ( !sendingPaint )
            {
                delete fixture.book;
                fixture.book = nullptr;
            }
        });
    const auto remove = wxMakeGuard([&]
    {
        first.UnregisterPropertyChangedCallback(property, token);
    });

    // Exercise the internal tab-container mutation + Refresh path directly,
    // without the public SetPageText() helper's immediate Update call. The
    // actual XAML property observer, not a fabricated wx event, is the oracle.
    fixture.tabs->GetPage(0).caption = "Pending native caption";
    fixture.tabs->Refresh();
    const HWND hwnd = static_cast<HWND>(fixture.tabs->GetHWND());
    REQUIRE(::InvalidateRect(hwnd, nullptr, FALSE) != FALSE);
    sendingPaint = true;
    ::SendMessage(hwnd, WM_PAINT, 0, 0);
    sendingPaint = false;
    CHECK(changes == 0);
    CHECK(weak);
    CHECK_FALSE(::GetUpdateRect(hwnd, nullptr, FALSE));

    fixture.tabs->Update();
    CHECK(changes == 1);
    CHECK_FALSE(weak);
    REQUIRE(WaitAui([&] { return !first.IsLoaded(); }));
}

#ifdef WXAUI_NOTEBOOK_TEST_SUPPORT
TEST_CASE("WinUIAui::ExistingPaneTransactionsRetainNativeOrderAndOwnership",
          "[winui-aui-native]")
{
    AuiFixture fixture;
    const auto first = AuiItem(fixture.view, "First");
    const auto second = AuiItem(fixture.view, "Second");
    int done = 0;
    fixture.book->Bind(wxEVT_AUINOTEBOOK_DRAG_DONE, [&](wxAuiNotebookEvent&) { ++done; });
    // Same-pane reordering occurs during DragMotion, not EndDrag. The private
    // seam supplies the target's real native rectangle instead of moving the
    // physical mouse; all hit testing and AUI callbacks still run normally.
    int motion = 0;
    fixture.book->Bind(wxEVT_AUINOTEBOOK_DRAG_MOTION,
        [&](wxAuiNotebookEvent&) { ++motion; });
    wxAuiNotebookTestAccess::DragMotion(fixture.book, fixture.tabs, 1, fixture.tabs, 0);
    REQUIRE(WaitAui([&]
    {
        return fixture.tabs->GetWindowFromIdx(0) == fixture.second &&
               fixture.view.TabItems().GetAt(0) == second;
    }));
    CHECK(fixture.view.TabItems().GetAt(1) == first);
    CHECK(motion == 1);
    wxAuiNotebookTestAccess::EndDrag(fixture.book, fixture.tabs, 0, fixture.tabs, 0);
    CHECK(done == 0); // No cross-pane transaction was needed on release.
    CHECK(fixture.book->GetPage(0) == fixture.first); // canonical insertion order

    fixture.book->Split(2, wxRIGHT);
    wxAuiTabCtrl* other = nullptr;
    int index = -1;
    REQUIRE(fixture.book->FindTab(fixture.third, &other, &index));
    REQUIRE(other != fixture.tabs);
    C::TabView otherView{nullptr};
    REQUIRE(WaitAui([&]
    {
        otherView = AuiView(other);
        const auto item = AuiItem(otherView, "Third");
        return item && item.IsLoaded() && fixture.view.TabItems().Size() == 2;
    }));
    CHECK(fixture.third->GetParent() == fixture.book);
    CHECK(wxWinUITopLevelHost::FindSlotOwner(other) ==
          wxWinUITopLevelHost::FindSlotOwner(fixture.tabs));
    wxAuiNotebookTestAccess::EndDrag(fixture.book, fixture.tabs, 0, other, 0);
    REQUIRE(WaitAui([&]
    {
        return other->GetIdxFromWindow(fixture.second) != wxNOT_FOUND &&
               fixture.tabs->GetIdxFromWindow(fixture.second) == wxNOT_FOUND &&
               otherView.TabItems().Size() == 2 && fixture.view.TabItems().Size() == 1;
    }));
    CHECK(fixture.second->GetParent() == fixture.book);
    CHECK(fixture.book->GetPageCount() == 3);
    CHECK(done == 1);
}
#endif

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_AUI
