///////////////////////////////////////////////////////////////////////////////
// Name:        tests/menu/winuimenubar.cpp
// Purpose:     Deterministic tests for WinUI menu projection and routing
// Author:      wxWidgets development team
// Created:     2026-07-23
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_MENUS

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/menu.h"
    #include "wx/utils.h"
#endif // WX_PRECOMP

#include "wx/bitmap.h"
#include "wx/bmpbndl.h"
#include "wx/stopwatch.h"
#include "wx/weakref.h"
#include "wx/winui/private/menutest.h"
#include "wx/winui/private/tlwhost.h"

#include <winrt/base.h>

#include <functional>
#include <vector>

namespace
{

void DrainMenuEvents(int rounds = 8)
{
    for ( int i = 0; i < rounds; ++i )
    {
        wxYield();
        wxMilliSleep(2);
    }
}

template <typename Predicate>
bool WaitForMenuCondition(Predicate predicate, long timeoutMs = 1500)
{
    wxStopWatch stopwatch;
    do
    {
        if ( predicate() )
            return true;

        wxYield();
        wxMilliSleep(2);
    }
    while ( stopwatch.Time() < timeoutMs );

    return predicate();
}

class ContentCarrierHookReset final
{
public:
    ~ContentCarrierHookReset() { Clear(); }

    void Clear()
    {
        if ( !m_active )
            return;

        wxWinUITopLevelHost::TestOnNextContentCarrierSet({});
        m_active = false;
    }

private:
    bool m_active = true;
};

struct MenuHostLifetimeSnapshot
{
    MenuHostLifetimeSnapshot()
        : hosts(wxWinUITopLevelHost::GetLiveHostCount()),
          slots(wxWinUITopLevelHost::GetLiveSlotCount()),
          slotStates(wxWinUITopLevelHost::GetLiveSlotLifetimeCount()),
          hostStates(wxWinUITopLevelHost::GetLiveHostLifetimeCount()),
          subclasses(wxWinUITopLevelHost::GetLiveSubclassContextCount()),
          loadedHooks(wxWinUITopLevelHost::GetLiveLoadedHookCount()),
          slotAdds(wxWinUITopLevelHost::GetSlotHandlerAddCount()),
          slotRevokes(wxWinUITopLevelHost::GetSlotHandlerRevokeCount()),
          rootAdds(wxWinUITopLevelHost::GetRootHandlerAddCount()),
          rootRevokes(wxWinUITopLevelHost::GetRootHandlerRevokeCount())
    {
    }

    void CheckRestored() const
    {
        CHECK(wxWinUITopLevelHost::GetLiveHostCount() == hosts);
        CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == slots);
        CHECK(wxWinUITopLevelHost::GetLiveSlotLifetimeCount() == slotStates);
        CHECK(wxWinUITopLevelHost::GetLiveHostLifetimeCount() == hostStates);
        CHECK(wxWinUITopLevelHost::GetLiveSubclassContextCount() ==
              subclasses);
        CHECK(wxWinUITopLevelHost::GetLiveLoadedHookCount() == loadedHooks);
        CHECK(wxWinUITopLevelHost::GetSlotHandlerAddCount() - slotAdds ==
              wxWinUITopLevelHost::GetSlotHandlerRevokeCount() -
                  slotRevokes);
        CHECK(wxWinUITopLevelHost::GetRootHandlerAddCount() - rootAdds ==
              wxWinUITopLevelHost::GetRootHandlerRevokeCount() -
                  rootRevokes);
    }

    unsigned hosts;
    unsigned slots;
    unsigned slotStates;
    unsigned hostStates;
    unsigned subclasses;
    unsigned loadedHooks;
    unsigned slotAdds;
    unsigned slotRevokes;
    unsigned rootAdds;
    unsigned rootRevokes;
};

wxWindow *FindOnlyFrameChild(wxFrame *frame)
{
    wxWindow *result = nullptr;
    for ( wxWindow * const child : frame->GetChildren() )
    {
        if ( child->GetHWND() )
        {
            if ( result )
                return nullptr;
            result = child;
        }
    }
    return result;
}

struct MenuBarFixture
{
    MenuBarFixture()
    {
        frame = new wxFrame(
            wxTheApp->GetTopWindow(),
            wxID_ANY,
            "WinUI menu projection test",
            wxPoint(-30000, -30000),
            wxSize(320, 200),
            wxDEFAULT_FRAME_STYLE |
                wxFRAME_NO_TASKBAR |
                wxFRAME_TOOL_WINDOW);
        menuBar = new wxMenuBar;
    }

    void Attach()
    {
        frame->SetMenuBar(menuBar);
        // The real XamlRoot and popup presenters must be realized for these
        // lifecycle tests, but the gate must not activate or surface a taskbar
        // window while the machine is in use.
        frame->ShowWithoutActivating();
        DrainMenuEvents();
        menuBarWindow = FindOnlyFrameChild(frame);
        REQUIRE(menuBarWindow != nullptr);
        REQUIRE(wxWinUI3MenuBarHeightForTesting(menuBarWindow) > 0);
    }

    void Destroy()
    {
        if ( frame )
        {
            frame->Destroy();
            frame = nullptr;
            menuBar = nullptr;
            menuBarWindow = nullptr;
            DrainMenuEvents();
        }
    }

    ~MenuBarFixture() { Destroy(); }

    wxFrame *frame = nullptr;
    wxMenuBar *menuBar = nullptr;
    wxWindow *menuBarWindow = nullptr;
};

class PopupOpenExceptionFrame final : public wxFrame
{
public:
    PopupOpenExceptionFrame()
        : wxFrame(wxTheApp->GetTopWindow(), wxID_ANY,
                  "WinUI popup OPEN exception test")
    {
    }

    bool DoSendMenuOpenCloseEvent(wxEventType eventType,
                                  wxMenu *menu) override
    {
        const bool handled =
            wxFrame::DoSendMenuOpenCloseEvent(eventType, menu);
        if ( eventType == wxEVT_MENU_OPEN )
        {
            ++opens;
            throw winrt::hresult_error(E_FAIL);
        }

        if ( eventType == wxEVT_MENU_CLOSE )
            ++closes;
        return handled;
    }

    unsigned opens = 0;
    unsigned closes = 0;
};

} // anonymous namespace

TEST_CASE("WinUIMenu::ReentrantRebuildHasPersistentBudgetAndRearms",
          "[WinUIMenu][menu][reentrancy][lifetime]")
{
    MenuBarFixture fixture;
    wxMenu * const top = new wxMenu;
    top->Append(wxID_HIGHEST + 509, "Command");
    fixture.menuBar->Append(top, "&Initial");
    fixture.Attach();

    unsigned callbacks = 0;
    std::function<void (wxWindow *)> storm;
    storm =
        [&](wxWindow *window)
        {
            if ( window != fixture.menuBarWindow )
                return;

            ++callbacks;
            fixture.menuBar->SetMenuLabel(
                0, wxString::Format("&Storm %u", callbacks));
            // Deliberately never converge. The production transaction, not
            // this adversarial hook, must stop the event-loop feedback.
            wxWinUITopLevelHost::TestOnNextContentCarrierSet(storm);
        };

    ContentCarrierHookReset resetHook;
    wxWinUITopLevelHost::TestOnNextContentCarrierSet(storm);
    fixture.menuBar->SetMenuLabel(0, "&Outer");

    // One public mutation consumes one bounded synchronous wave first.
    CHECK(callbacks == 8);
    REQUIRE(WaitForMenuCondition(
        [&]()
        {
            return wxWinUI3MenuBarBuildQuarantinedForTesting(
                fixture.menuBarWindow);
        }));

    // The second wave exhausts the persistent transaction budget. No third
    // CallAfter is posted, even though the hook has rearmed itself forever.
    CHECK(callbacks == 16);
    const unsigned callbacksAtQuarantine = callbacks;
    DrainMenuEvents(16);
    CHECK(callbacks == callbacksAtQuarantine);
    CHECK(wxWinUI3MenuBarBuildQuarantinedForTesting(
        fixture.menuBarWindow));

    // Never let the global one-shot seam retain this stack frame. A later,
    // genuinely external model mutation starts a fresh epoch/budget and
    // converges normally.
    resetHook.Clear();
    fixture.menuBar->SetMenuLabel(0, "&Stable");
    CHECK_FALSE(wxWinUI3MenuBarBuildQuarantinedForTesting(
        fixture.menuBarWindow));

    wxString label;
    wxString accessKey;
    REQUIRE(wxWinUI3MenuBarTopLabelForTesting(
        fixture.menuBarWindow, 0, &label, &accessKey));
    CHECK(label == "Stable");
    CHECK(accessKey.CmpNoCase("S") == 0);
}

TEST_CASE("WinUIMenu::StaleBuildContinuationCannotOverwriteNewEpoch",
          "[WinUIMenu][menu][reentrancy][lifetime]")
{
    MenuBarFixture fixture;
    wxMenu * const top = new wxMenu;
    top->Append(wxID_HIGHEST + 510, "Command");
    fixture.menuBar->Append(top, "&Initial");
    fixture.Attach();

    unsigned callbacks = 0;
    std::function<void (wxWindow *)> storm;
    storm =
        [&](wxWindow *window)
        {
            if ( window != fixture.menuBarWindow )
                return;

            ++callbacks;
            fixture.menuBar->SetMenuLabel(
                0, wxString::Format("&Old %u", callbacks));
            wxWinUITopLevelHost::TestOnNextContentCarrierSet(storm);
        };

    ContentCarrierHookReset resetStorm;
    wxWinUITopLevelHost::TestOnNextContentCarrierSet(storm);
    fixture.menuBar->SetMenuLabel(0, "&Outer");
    REQUIRE(callbacks == 8);

    // Invalidate the queued continuation with a new external transaction.
    resetStorm.Clear();
    fixture.menuBar->SetMenuLabel(0, "&Winning");

    unsigned staleWrites = 0;
    ContentCarrierHookReset resetProbe;
    wxWinUITopLevelHost::TestOnNextContentCarrierSet(
        [&](wxWindow *window)
        {
            if ( window == fixture.menuBarWindow )
                ++staleWrites;
        });
    DrainMenuEvents(16);
    resetProbe.Clear();
    CHECK(staleWrites == 0);

    wxString label;
    wxString accessKey;
    REQUIRE(wxWinUI3MenuBarTopLabelForTesting(
        fixture.menuBarWindow, 0, &label, &accessKey));
    CHECK(label == "Winning");
    CHECK(accessKey.CmpNoCase("W") == 0);
}

TEST_CASE("WinUIMenu::DestroyChildDuringCreateIsTransactional",
          "[WinUIMenu][menu][reentrancy][lifetime]")
{
    const MenuHostLifetimeSnapshot lifetime;
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "WinUI MenuBar child-create destroy");
    wxMenuBar * const bar = new wxMenuBar;
    bar->Append(new wxMenu, "&File");

    bool hookRan = false;
    ContentCarrierHookReset resetHook;
    wxWinUITopLevelHost::TestOnNextContentCarrierSet(
        [&](wxWindow *window)
        {
            if ( wxGetTopLevelParent(window) == frame )
            {
                hookRan = true;
                window->Destroy();
            }
        });

    // The non-TLW child is deleted synchronously from SetContent(). The
    // attach wrapper must not call Destroy() again on its stale raw pointer.
    frame->SetMenuBar(bar);
    resetHook.Clear();
    CHECK(hookRan);
    CHECK(FindOnlyFrameChild(frame) == nullptr);

    frame->Destroy();
    DrainMenuEvents(16);
    lifetime.CheckRestored();
}

TEST_CASE("WinUIMenu::DestroyFrameDuringCreateIsTransactional",
          "[WinUIMenu][menu][reentrancy][lifetime]")
{
    const MenuHostLifetimeSnapshot lifetime;
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "WinUI MenuBar frame-create destroy");
    const wxWeakRef<wxWindow> frameAlive(frame);
    wxMenuBar * const bar = new wxMenuBar;
    bar->Append(new wxMenu, "&File");

    bool hookRan = false;
    ContentCarrierHookReset resetHook;
    wxWinUITopLevelHost::TestOnNextContentCarrierSet(
        [&](wxWindow *window)
        {
            if ( wxGetTopLevelParent(window) == frame )
            {
                hookRan = true;
                frame->Destroy();
            }
        });

    frame->SetMenuBar(bar);
    resetHook.Clear();
    CHECK(hookRan);
    const bool destroyed =
        WaitForMenuCondition([&]() { return !frameAlive; });
    CHECK(destroyed);
    if ( wxWindow * const survivor = frameAlive.get() )
        survivor->Destroy();
    DrainMenuEvents(16);
    lifetime.CheckRestored();
}

TEST_CASE("WinUIMenu::OpenBeforeProjectionAndExactClose",
          "[WinUIMenu][menu]")
{
    constexpr int IdDynamic = wxID_HIGHEST + 511;
    constexpr int IdCheck = wxID_HIGHEST + 512;
    constexpr int IdSub = wxID_HIGHEST + 513;

    MenuBarFixture fixture;
    wxMenu * const top = new wxMenu;
    wxMenuItem * const dynamic =
        top->Append(IdDynamic, "&Before\tCtrl+B");
    wxMenuItem * const check =
        top->AppendCheckItem(IdCheck, "&Check");
    wxMenu * const sub = new wxMenu;
    sub->Append(IdSub, "S&ub item");
    top->AppendSubMenu(sub, "&Submenu");
    wxMenu * const empty = new wxMenu;
    fixture.menuBar->Append(top, "&File");
    fixture.menuBar->Append(empty, "&Empty");

    std::vector<wxMenu *> opened;
    std::vector<wxMenu *> closed;
    bool mutateOnce = true;

    fixture.frame->Bind(
        wxEVT_MENU_OPEN,
        [&](wxMenuEvent& event)
        {
            opened.push_back(event.GetMenu());
            if ( event.GetMenu() == top && mutateOnce )
            {
                mutateOnce = false;
                dynamic->SetItemLabel("&After\tCtrl+T");
                dynamic->Enable(false);
                dynamic->SetBitmap(
                    wxBitmapBundle::FromBitmap(wxBitmap(8, 8)));
                check->Check(true);
            }
            event.Skip();
        });
    fixture.frame->Bind(
        wxEVT_MENU_CLOSE,
        [&](wxMenuEvent& event)
        {
            closed.push_back(event.GetMenu());
            event.Skip();
        });

    fixture.Attach();

    const size_t topPath[] = { 0 };
    REQUIRE(wxWinUI3MenuBarOpenForTesting(
        fixture.menuBarWindow, topPath, WXSIZEOF(topPath)));
    REQUIRE(opened.size() == 1);
    CHECK(opened.back() == top);

    wxWinUIMenuItemSnapshot dynamicPeer;
    REQUIRE(wxWinUI3MenuBarSnapshotForTesting(
        fixture.menuBarWindow,
        topPath,
        WXSIZEOF(topPath),
        0,
        &dynamicPeer));
    CHECK(dynamicPeer.label == "After");
    CHECK(dynamicPeer.accessKey == "A");
    CHECK(dynamicPeer.accelerator == "Ctrl+T");
    CHECK(!dynamicPeer.enabled);
    CHECK(dynamicPeer.hasIcon);

    wxWinUIMenuItemSnapshot checkPeer;
    REQUIRE(wxWinUI3MenuBarSnapshotForTesting(
        fixture.menuBarWindow,
        topPath,
        WXSIZEOF(topPath),
        1,
        &checkPeer));
    CHECK(checkPeer.checked);

    REQUIRE(wxWinUI3MenuBarCloseForTesting(
        fixture.menuBarWindow, topPath, WXSIZEOF(topPath)));
    REQUIRE(closed.size() == 1);
    CHECK(closed.back() == top);
    DrainMenuEvents();

    // A menu with no public items still has the private sentinel, hence an
    // exact balanced OPEN/CLOSE pair.
    const size_t emptyPath[] = { 1 };
    REQUIRE(wxWinUI3MenuBarOpenForTesting(
        fixture.menuBarWindow, emptyPath, WXSIZEOF(emptyPath)));
    REQUIRE(opened.size() == 2);
    CHECK(opened.back() == empty);
    REQUIRE(wxWinUI3MenuBarCloseForTesting(
        fixture.menuBarWindow, emptyPath, WXSIZEOF(emptyPath)));
    REQUIRE(closed.size() == 2);
    CHECK(closed.back() == empty);
    DrainMenuEvents();

    // Opening a submenu opens its ancestors first and closing is balanced in
    // reverse order, with each exact wxMenu pointer preserved.
    const size_t subPath[] = { 0, 2 };
    REQUIRE(wxWinUI3MenuBarOpenForTesting(
        fixture.menuBarWindow, subPath, WXSIZEOF(subPath)));
    REQUIRE(opened.size() == 4);
    CHECK(opened[2] == top);
    CHECK(opened[3] == sub);

    wxWinUIMenuItemSnapshot subPeer;
    REQUIRE(wxWinUI3MenuBarSnapshotForTesting(
        fixture.menuBarWindow,
        subPath,
        WXSIZEOF(subPath),
        0,
        &subPeer));
    CHECK(subPeer.label == "Sub item");
    CHECK(subPeer.accessKey == "u");

    REQUIRE(wxWinUI3MenuBarCloseForTesting(
        fixture.menuBarWindow, subPath, WXSIZEOF(subPath)));
    REQUIRE(closed.size() == 4);
    CHECK(closed[2] == sub);
    CHECK(closed[3] == top);
}

TEST_CASE("WinUIMenu::CanonicalCommandRouting",
          "[WinUIMenu][menu]")
{
    constexpr int IdDuplicate = wxID_HIGHEST + 521;
    constexpr int IdCheck = wxID_HIGHEST + 522;
    constexpr int IdRadio1 = wxID_HIGHEST + 523;
    constexpr int IdRadio2 = wxID_HIGHEST + 524;

    MenuBarFixture fixture;
    wxMenu * const top = new wxMenu;
    wxMenu * const branchA = new wxMenu;
    wxMenu * const branchB = new wxMenu;
    branchA->Append(IdDuplicate, "A command");
    branchA->AppendCheckItem(IdCheck, "A check");
    branchA->AppendRadioItem(IdRadio1, "Radio one");
    branchA->AppendRadioItem(IdRadio2, "Radio two");
    branchA->Check(IdRadio1, true);
    branchB->Append(IdDuplicate, "B command");
    top->AppendSubMenu(branchA, "Branch &A");
    top->AppendSubMenu(branchB, "Branch &B");
    fixture.menuBar->Append(top, "&Commands");
    fixture.Attach();

    std::vector<wxString> route;
    int commandInt = -99;
    wxObject *source = nullptr;

    branchA->Bind(
        wxEVT_MENU,
        [&](wxCommandEvent& event)
        {
            route.push_back("branchA");
            commandInt = event.GetInt();
            source = event.GetEventObject();
            event.Skip();
        });
    top->Bind(
        wxEVT_MENU,
        [&](wxCommandEvent& event)
        {
            route.push_back("top");
            event.Skip();
        });
    fixture.menuBar->Bind(
        wxEVT_MENU,
        [&](wxCommandEvent& event)
        {
            route.push_back("bar");
            event.Skip();
        });
    fixture.frame->Bind(
        wxEVT_MENU,
        [&](wxCommandEvent&)
        {
            route.push_back("frame");
        });

    const size_t branchAPath[] = { 0, 0 };
    REQUIRE(wxWinUI3MenuBarOpenForTesting(
        fixture.menuBarWindow,
        branchAPath,
        WXSIZEOF(branchAPath)));
    REQUIRE(wxWinUI3MenuBarInvokeForTesting(
        fixture.menuBarWindow,
        branchAPath,
        WXSIZEOF(branchAPath),
        0));
    REQUIRE(wxWinUI3MenuBarCloseForTesting(
        fixture.menuBarWindow,
        branchAPath,
        WXSIZEOF(branchAPath)));
    DrainMenuEvents();

    REQUIRE(route.size() == 4);
    CHECK(route[0] == "branchA");
    CHECK(route[1] == "top");
    CHECK(route[2] == "bar");
    CHECK(route[3] == "frame");
    CHECK(source == branchA);
    CHECK(commandInt == -1);

    route.clear();
    commandInt = -99;
    REQUIRE(wxWinUI3MenuBarOpenForTesting(
        fixture.menuBarWindow,
        branchAPath,
        WXSIZEOF(branchAPath)));
    REQUIRE(wxWinUI3MenuBarInvokeForTesting(
        fixture.menuBarWindow,
        branchAPath,
        WXSIZEOF(branchAPath),
        1));
    REQUIRE(wxWinUI3MenuBarCloseForTesting(
        fixture.menuBarWindow,
        branchAPath,
        WXSIZEOF(branchAPath)));
    DrainMenuEvents();
    CHECK(branchA->IsChecked(IdCheck));
    CHECK(commandInt == 1);

    // Selecting an already selected radio is a no-op in the canonical MSW
    // command implementation.
    route.clear();
    REQUIRE(wxWinUI3MenuBarOpenForTesting(
        fixture.menuBarWindow,
        branchAPath,
        WXSIZEOF(branchAPath)));

    wxWinUIMenuItemSnapshot selectedRadio;
    REQUIRE(wxWinUI3MenuBarSnapshotForTesting(
        fixture.menuBarWindow,
        branchAPath,
        WXSIZEOF(branchAPath),
        2,
        &selectedRadio));
    CHECK(selectedRadio.radio);
    CHECK(selectedRadio.checked);

    wxWinUIMenuItemSnapshot unselectedRadio;
    REQUIRE(wxWinUI3MenuBarSnapshotForTesting(
        fixture.menuBarWindow,
        branchAPath,
        WXSIZEOF(branchAPath),
        3,
        &unselectedRadio));
    CHECK(unselectedRadio.radio);
    CHECK(!unselectedRadio.checked);

    REQUIRE(wxWinUI3MenuBarInvokeForTesting(
        fixture.menuBarWindow,
        branchAPath,
        WXSIZEOF(branchAPath),
        2));
    REQUIRE(wxWinUI3MenuBarCloseForTesting(
        fixture.menuBarWindow,
        branchAPath,
        WXSIZEOF(branchAPath)));
    DrainMenuEvents();
    CHECK(route.empty());
}

TEST_CASE("WinUIMenu::DisabledSubmenuAndMetrics",
          "[WinUIMenu][menu]")
{
    MenuBarFixture fixture;
    wxMenu * const top = new wxMenu;
    wxMenu * const sub = new wxMenu;
    sub->Append(wxID_HIGHEST + 531, "Child");
    wxMenuItem * const submenu = top->AppendSubMenu(sub, "&Disabled");
    submenu->Enable(false);
    fixture.menuBar->Append(top, "&Top");
    fixture.Attach();

    bool topEnabled = false;
    REQUIRE(wxWinUI3MenuBarTopEnabledForTesting(
        fixture.menuBarWindow, 0, &topEnabled));
    CHECK(topEnabled);

    fixture.menuBar->EnableTop(0, false);
    DrainMenuEvents();
    REQUIRE(wxWinUI3MenuBarTopEnabledForTesting(
        fixture.menuBarWindow, 0, &topEnabled));
    CHECK(!topEnabled);

    fixture.menuBar->EnableTop(0, true);
    DrainMenuEvents();
    REQUIRE(wxWinUI3MenuBarTopEnabledForTesting(
        fixture.menuBarWindow, 0, &topEnabled));
    CHECK(topEnabled);

    const size_t topPath[] = { 0 };
    REQUIRE(wxWinUI3MenuBarOpenForTesting(
        fixture.menuBarWindow, topPath, WXSIZEOF(topPath)));

    wxWinUIMenuItemSnapshot snapshot;
    REQUIRE(wxWinUI3MenuBarSnapshotForTesting(
        fixture.menuBarWindow,
        topPath,
        WXSIZEOF(topPath),
        0,
        &snapshot));
    CHECK(snapshot.submenu);
    CHECK(!snapshot.enabled);
    CHECK(snapshot.accessKey == "D");
    CHECK(wxWinUI3MenuBarHeightForTesting(fixture.menuBarWindow) ==
          fixture.frame->FromDIP(40));

    REQUIRE(wxWinUI3MenuBarCloseForTesting(
        fixture.menuBarWindow, topPath, WXSIZEOF(topPath)));
}

TEST_CASE("WinUIMenu::DPIBundleAndHotFlowDirectionProjection",
          "[WinUIMenu][menu][dpi][rtl][winui-009]")
{
    wxBitmap one;
    wxBitmap two;
    REQUIRE(one.CreateWithDIPSize(wxSize(16, 16), 1.0, 32));
    REQUIRE(two.CreateWithDIPSize(wxSize(16, 16), 2.0, 32));
    const wxBitmapBundle bundle =
        wxBitmapBundle::FromBitmaps(one, two);

    MenuBarFixture fixture;
    fixture.frame->SetLayoutDirection(wxLayout_LeftToRight);
    wxMenu * const top = new wxMenu;
    wxMenuItem * const command =
        top->Append(wxID_HIGHEST + 536, "&Command");
    command->SetBitmap(bundle);
    wxMenu * const sub = new wxMenu;
    wxMenuItem * const child =
        sub->Append(wxID_HIGHEST + 537, "&Child");
    child->SetBitmap(bundle);
    top->AppendSubMenu(sub, "&Submenu");
    fixture.menuBar->Append(top, "&Top");
    fixture.Attach();

    const size_t topPath[] = { 0 };
    const size_t subPath[] = { 0, 1 };
    REQUIRE(wxWinUI3MenuBarRefreshForScaleForTesting(
        fixture.menuBarWindow, 1.0));
    REQUIRE(wxWinUI3MenuBarOpenForTesting(
        fixture.menuBarWindow, subPath, WXSIZEOF(subPath)));

    const auto snapshot =
        [&](const size_t *path, size_t pathLength,
            size_t itemPosition)
        {
            wxWinUIMenuItemSnapshot result;
            REQUIRE(wxWinUI3MenuBarSnapshotForTesting(
                fixture.menuBarWindow,
                path, pathLength, itemPosition, &result));
            return result;
        };

    wxWinUIMenuFlowDirection barFlow =
        wxWinUIMenuFlowDirection::Unknown;
    wxWinUIMenuFlowDirection topFlow =
        wxWinUIMenuFlowDirection::Unknown;
    REQUIRE(wxWinUI3MenuBarFlowDirectionForTesting(
        fixture.menuBarWindow, &barFlow, &topFlow));
    CHECK(barFlow == wxWinUIMenuFlowDirection::LeftToRight);
    CHECK(topFlow == wxWinUIMenuFlowDirection::LeftToRight);

    wxWinUIMenuItemSnapshot commandPeer =
        snapshot(topPath, WXSIZEOF(topPath), 0);
    wxWinUIMenuItemSnapshot childPeer =
        snapshot(subPath, WXSIZEOF(subPath), 0);
    CHECK(commandPeer.iconPixelWidth == 16);
    CHECK(commandPeer.iconPixelHeight == 16);
    CHECK(childPeer.iconPixelWidth == 16);
    CHECK(childPeer.iconPixelHeight == 16);
    CHECK(commandPeer.flowDirection ==
          wxWinUIMenuFlowDirection::LeftToRight);
    CHECK(childPeer.flowDirection ==
          wxWinUIMenuFlowDirection::LeftToRight);

    // Keep both presenters logically open: the hot direction transaction
    // must update the bar, its top items, and the already-detached nested
    // flyout peers without rebuilding their wxMenu projections.
    fixture.frame->SetLayoutDirection(wxLayout_RightToLeft);
    REQUIRE(WaitForMenuCondition(
        [&]()
        {
            if ( !wxWinUI3MenuBarFlowDirectionForTesting(
                     fixture.menuBarWindow, &barFlow, &topFlow) )
            {
                return false;
            }
            commandPeer =
                snapshot(topPath, WXSIZEOF(topPath), 0);
            childPeer =
                snapshot(subPath, WXSIZEOF(subPath), 0);
            return barFlow ==
                       wxWinUIMenuFlowDirection::RightToLeft &&
                   topFlow ==
                       wxWinUIMenuFlowDirection::RightToLeft &&
                   commandPeer.flowDirection ==
                       wxWinUIMenuFlowDirection::RightToLeft &&
                   childPeer.flowDirection ==
                       wxWinUIMenuFlowDirection::RightToLeft;
        }));

    fixture.frame->SetLayoutDirection(wxLayout_LeftToRight);
    REQUIRE(WaitForMenuCondition(
        [&]()
        {
            if ( !wxWinUI3MenuBarFlowDirectionForTesting(
                     fixture.menuBarWindow, &barFlow, &topFlow) )
            {
                return false;
            }
            commandPeer =
                snapshot(topPath, WXSIZEOF(topPath), 0);
            childPeer =
                snapshot(subPath, WXSIZEOF(subPath), 0);
            return barFlow ==
                       wxWinUIMenuFlowDirection::LeftToRight &&
                   topFlow ==
                       wxWinUIMenuFlowDirection::LeftToRight &&
                   commandPeer.flowDirection ==
                       wxWinUIMenuFlowDirection::LeftToRight &&
                   childPeer.flowDirection ==
                       wxWinUIMenuFlowDirection::LeftToRight;
        }));

    REQUIRE(wxWinUI3MenuBarCloseForTesting(
        fixture.menuBarWindow, subPath, WXSIZEOF(subPath)));
    DrainMenuEvents();

    REQUIRE(wxWinUI3MenuBarRefreshForScaleForTesting(
        fixture.menuBarWindow, 2.0));
    REQUIRE(wxWinUI3MenuBarOpenForTesting(
        fixture.menuBarWindow, subPath, WXSIZEOF(subPath)));
    commandPeer = snapshot(topPath, WXSIZEOF(topPath), 0);
    childPeer = snapshot(subPath, WXSIZEOF(subPath), 0);
    CHECK(commandPeer.iconPixelWidth == 32);
    CHECK(commandPeer.iconPixelHeight == 32);
    CHECK(childPeer.iconPixelWidth == 32);
    CHECK(childPeer.iconPixelHeight == 32);
    REQUIRE(wxWinUI3MenuBarCloseForTesting(
        fixture.menuBarWindow, subPath, WXSIZEOF(subPath)));
    DrainMenuEvents();

    REQUIRE(wxWinUI3MenuBarRefreshForScaleForTesting(
        fixture.menuBarWindow, 1.0));
    REQUIRE(wxWinUI3MenuBarOpenForTesting(
        fixture.menuBarWindow, topPath, WXSIZEOF(topPath)));
    commandPeer = snapshot(topPath, WXSIZEOF(topPath), 0);
    CHECK(commandPeer.iconPixelWidth == 16);
    CHECK(commandPeer.iconPixelHeight == 16);
    REQUIRE(wxWinUI3MenuBarCloseForTesting(
        fixture.menuBarWindow, topPath, WXSIZEOF(topPath)));
}

TEST_CASE("WinUIMenu::PhysicalFlyoutLifecycleAndBoundedProjection",
          "[WinUIMenu][menu]")
{
    MenuBarFixture fixture;
    wxMenu * const top = new wxMenu;
    wxMenu * const sub = new wxMenu;
    sub->Append(wxID_HIGHEST + 541, "Child");
    top->AppendSubMenu(sub, "&Submenu");
    fixture.menuBar->Append(top, "&Physical");

    std::vector<wxMenu *> opened;
    std::vector<wxMenu *> closed;
    fixture.frame->Bind(
        wxEVT_MENU_OPEN,
        [&](wxMenuEvent& event)
        {
            opened.push_back(event.GetMenu());
            event.Skip();
        });
    fixture.frame->Bind(
        wxEVT_MENU_CLOSE,
        [&](wxMenuEvent& event)
        {
            closed.push_back(event.GetMenu());
            event.Skip();
        });
    fixture.Attach();

    const size_t topPath[] = { 0 };
    const size_t subPath[] = { 0, 0 };
    REQUIRE(wxWinUI3MenuBarSentinelContractForTesting(
        fixture.menuBarWindow, topPath, WXSIZEOF(topPath)));
    bool expanded = true;
    REQUIRE(wxWinUI3MenuBarGetExpandedForTesting(
        fixture.menuBarWindow, topPath, WXSIZEOF(topPath), &expanded));
    CHECK_FALSE(expanded);
    size_t stableProjectionCount = 0;

    for ( int iteration = 0; iteration < 3; ++iteration )
    {
        REQUIRE(wxWinUI3MenuBarSetExpandedForTesting(
            fixture.menuBarWindow, topPath, WXSIZEOF(topPath), true));
        REQUIRE(WaitForMenuCondition(
            [&]()
            {
                return opened.size() >=
                       static_cast<size_t>(iteration * 2 + 1);
            }));
        REQUIRE(opened.size() == static_cast<size_t>(iteration * 2 + 1));
        CHECK(opened.back() == top);
        REQUIRE(wxWinUI3MenuBarGetExpandedForTesting(
            fixture.menuBarWindow,
            topPath,
            WXSIZEOF(topPath),
            &expanded));
        CHECK(expanded);
        REQUIRE(wxWinUI3MenuBarSentinelContractForTesting(
            fixture.menuBarWindow, subPath, WXSIZEOF(subPath)));

        REQUIRE(wxWinUI3MenuBarSetExpandedForTesting(
            fixture.menuBarWindow, subPath, WXSIZEOF(subPath), true));
        REQUIRE(WaitForMenuCondition(
            [&]()
            {
                return opened.size() >=
                       static_cast<size_t>(iteration * 2 + 2);
            }));
        REQUIRE(opened.size() == static_cast<size_t>(iteration * 2 + 2));
        CHECK(opened.back() == sub);
        REQUIRE(wxWinUI3MenuBarGetExpandedForTesting(
            fixture.menuBarWindow,
            topPath,
            WXSIZEOF(topPath),
            &expanded));
        CHECK(expanded);
        REQUIRE(wxWinUI3MenuBarGetExpandedForTesting(
            fixture.menuBarWindow,
            subPath,
            WXSIZEOF(subPath),
            &expanded));
        CHECK(expanded);
        CHECK(closed.size() == static_cast<size_t>(iteration * 2));

        const size_t projectionCount =
            wxWinUI3MenuBarProjectionCountForTesting(
                fixture.menuBarWindow);
        if ( iteration == 0 )
            stableProjectionCount = projectionCount;
        CHECK(projectionCount == stableProjectionCount);

        REQUIRE(wxWinUI3MenuBarSetExpandedForTesting(
            fixture.menuBarWindow, subPath, WXSIZEOF(subPath), false));
        REQUIRE(WaitForMenuCondition(
            [&]()
            {
                return closed.size() >=
                       static_cast<size_t>(iteration * 2 + 1);
            }));
        REQUIRE(closed.size() == static_cast<size_t>(iteration * 2 + 1));
        CHECK(closed.back() == sub);
        REQUIRE(wxWinUI3MenuBarGetExpandedForTesting(
            fixture.menuBarWindow,
            subPath,
            WXSIZEOF(subPath),
            &expanded));
        CHECK_FALSE(expanded);
        REQUIRE(wxWinUI3MenuBarGetExpandedForTesting(
            fixture.menuBarWindow,
            topPath,
            WXSIZEOF(topPath),
            &expanded));
        CHECK(expanded);

        REQUIRE(wxWinUI3MenuBarSetExpandedForTesting(
            fixture.menuBarWindow, topPath, WXSIZEOF(topPath), false));
        REQUIRE(WaitForMenuCondition(
            [&]()
            {
                return closed.size() >=
                       static_cast<size_t>(iteration * 2 + 2);
            }));
        REQUIRE(closed.size() == static_cast<size_t>(iteration * 2 + 2));
        CHECK(closed.back() == top);
        REQUIRE(wxWinUI3MenuBarGetExpandedForTesting(
            fixture.menuBarWindow,
            topPath,
            WXSIZEOF(topPath),
            &expanded));
        CHECK_FALSE(expanded);
    }
}

TEST_CASE("WinUIMenu::OpenCanRemoveAndDeleteCurrentMenu",
          "[WinUIMenu][menu][reentrancy][lifetime]")
{
    MenuBarFixture fixture;
    wxMenu * const top = new wxMenu;
    wxMenu * const child = new wxMenu;
    child->Append(wxID_HIGHEST + 547, "Child command");
    top->AppendSubMenu(child, "&Child");
    fixture.menuBar->Append(top, "&First");
    fixture.Attach();

    bool removedFromOpen = false;
    wxMenu *removedMenu = nullptr;
    fixture.frame->Bind(
        wxEVT_MENU_OPEN,
        [&](wxMenuEvent& event)
        {
            if ( event.GetMenu() == top && !removedFromOpen )
            {
                removedFromOpen = true;
                removedMenu = fixture.menuBar->Remove(0);
            }
            event.Skip();
        });

    // The first OPEN detaches the current menu and invalidates the projected
    // path. The seam must stop before looking up its child; deleting the
    // detached menu immediately afterwards must then be harmless.
    const size_t childPath[] = { 0, 0 };
    CHECK_FALSE(wxWinUI3MenuBarOpenForTesting(
        fixture.menuBarWindow, childPath, WXSIZEOF(childPath)));
    CHECK(removedFromOpen);
    CHECK(removedMenu == top);
    delete removedMenu;
}

TEST_CASE("WinUIMenu::OpenCanReplaceAndDeleteMenuBar",
          "[WinUIMenu][menu][reentrancy][lifetime]")
{
    MenuBarFixture fixture;
    wxMenu * const top = new wxMenu;
    wxMenu * const child = new wxMenu;
    child->Append(wxID_HIGHEST + 548, "Child command");
    top->AppendSubMenu(child, "&Child");
    fixture.menuBar->Append(top, "&First");
    fixture.Attach();

    wxMenuBar *firstBar = fixture.menuBar;
    wxMenuBar * const winningBar = new wxMenuBar;
    winningBar->Append(new wxMenu, "&Winning");
    const wxWeakRef<wxWindow> oldWindow(fixture.menuBarWindow);

    bool replacedFromOpen = false;
    fixture.frame->Bind(
        wxEVT_MENU_OPEN,
        [&](wxMenuEvent& event)
        {
            if ( event.GetMenu() == top && !replacedFromOpen )
            {
                replacedFromOpen = true;
                fixture.frame->SetMenuBar(winningBar);
            }
            event.Skip();
        });

    const size_t childPath[] = { 0, 0 };
    CHECK_FALSE(wxWinUI3MenuBarOpenForTesting(
        fixture.menuBarWindow, childPath, WXSIZEOF(childPath)));
    delete firstBar;
    firstBar = nullptr;
    DrainMenuEvents();
    CHECK(replacedFromOpen);
    CHECK(firstBar == nullptr);
    CHECK(!oldWindow);
    CHECK(fixture.frame->GetMenuBar() == winningBar);

    fixture.menuBar = winningBar;
    fixture.menuBarWindow = FindOnlyFrameChild(fixture.frame);
    REQUIRE(fixture.menuBarWindow != nullptr);
}

TEST_CASE("WinUIMenu::OpenCanDestroyFrame",
          "[WinUIMenu][menu][reentrancy][lifetime]")
{
    const MenuHostLifetimeSnapshot lifetime;
    MenuBarFixture fixture;
    wxMenu * const top = new wxMenu;
    wxMenu * const child = new wxMenu;
    child->Append(wxID_HIGHEST + 549, "Child command");
    top->AppendSubMenu(child, "&Child");
    fixture.menuBar->Append(top, "&First");
    fixture.Attach();

    const wxWeakRef<wxWindow> frameAlive(fixture.frame);
    const wxWeakRef<wxWindow> childAlive(fixture.menuBarWindow);
    bool destroyedFromOpen = false;
    fixture.frame->Bind(
        wxEVT_MENU_OPEN,
        [&](wxMenuEvent& event)
        {
            if ( event.GetMenu() == top && !destroyedFromOpen )
            {
                destroyedFromOpen = true;
                fixture.frame->Destroy();
            }
            event.Skip();
        });

    const size_t childPath[] = { 0, 0 };
    CHECK_FALSE(wxWinUI3MenuBarOpenForTesting(
        fixture.menuBarWindow, childPath, WXSIZEOF(childPath)));
    CHECK(destroyedFromOpen);

    // The TLW deletion is asynchronous. Disarm the fixture's raw ownership
    // view before draining the destroy queue.
    fixture.frame = nullptr;
    fixture.menuBar = nullptr;
    fixture.menuBarWindow = nullptr;
    const bool destroyed = WaitForMenuCondition(
        [&]() { return !frameAlive && !childAlive; });
    CHECK(destroyed);
    if ( wxWindow * const survivor = frameAlive.get() )
        survivor->Destroy();
    DrainMenuEvents(16);
    lifetime.CheckRestored();
}

TEST_CASE("WinUIMenu::ReentrantReplacementFromClose",
          "[WinUIMenu][menu]")
{
    MenuBarFixture fixture;
    wxMenu * const firstMenu = new wxMenu;
    firstMenu->Append(wxID_HIGHEST + 551, "First");
    fixture.menuBar->Append(firstMenu, "&First");
    fixture.Attach();

    wxMenuBar * const firstBar = fixture.menuBar;
    wxMenuBar * const supersededBar = new wxMenuBar;
    supersededBar->Append(new wxMenu, "&Superseded");
    wxMenuBar * const winningBar = new wxMenuBar;
    winningBar->Append(new wxMenu, "&Winning");
    const wxWeakRef<wxWindow> oldWindow(fixture.menuBarWindow);

    bool replacedFromClose = false;
    fixture.frame->Bind(
        wxEVT_MENU_CLOSE,
        [&](wxMenuEvent& event)
        {
            if ( event.GetMenu() == firstMenu && !replacedFromClose )
            {
                replacedFromClose = true;
                fixture.frame->SetMenuBar(winningBar);
            }
            event.Skip();
        });

    const size_t firstPath[] = { 0 };
    REQUIRE(wxWinUI3MenuBarOpenForTesting(
        fixture.menuBarWindow, firstPath, WXSIZEOF(firstPath)));

    // The outer replacement retires the open bar and emits CLOSE. Its handler
    // installs winningBar; the outer superseded generation must not overwrite
    // that inner result when Deactivate() returns.
    fixture.frame->SetMenuBar(supersededBar);
    DrainMenuEvents();
    CHECK(replacedFromClose);
    CHECK(fixture.frame->GetMenuBar() == winningBar);
    CHECK(!oldWindow);

    fixture.menuBar = winningBar;
    fixture.menuBarWindow = FindOnlyFrameChild(fixture.frame);
    REQUIRE(fixture.menuBarWindow != nullptr);
    CHECK(wxWinUI3MenuBarHeightForTesting(fixture.menuBarWindow) > 0);

    delete firstBar;
    delete supersededBar;
}

TEST_CASE("WinUIMenu::DescendantCloseCanReplaceAndDeleteMenuBar",
          "[WinUIMenu][menu]")
{
    MenuBarFixture fixture;
    wxMenu * const top = new wxMenu;
    wxMenu * const child = new wxMenu;
    child->Append(wxID_HIGHEST + 556, "Child command");
    top->AppendSubMenu(child, "&Child");
    fixture.menuBar->Append(top, "&First");
    fixture.Attach();

    wxMenuBar *firstBar = fixture.menuBar;
    wxMenuBar * const winningBar = new wxMenuBar;
    winningBar->Append(new wxMenu, "&Winning");
    const wxWeakRef<wxWindow> oldWindow(fixture.menuBarWindow);

    bool replacedFromChildClose = false;
    fixture.frame->Bind(
        wxEVT_MENU_CLOSE,
        [&](wxMenuEvent& event)
        {
            if ( event.GetMenu() == child && !replacedFromChildClose )
            {
                replacedFromChildClose = true;
                fixture.frame->SetMenuBar(winningBar);
                delete firstBar;
                firstBar = nullptr;
            }
            event.Skip();
        });

    const size_t childPath[] = { 0, 0 };
    const size_t topPath[] = { 0 };
    REQUIRE(wxWinUI3MenuBarOpenForTesting(
        fixture.menuBarWindow, childPath, WXSIZEOF(childPath)));

    // Closing a parent is a real cascade: its descendant CLOSE is emitted
    // first. Deleting the old wxMenuBar from that descendant callback must
    // stop the parent transition and all work on the retired XAML generation.
    REQUIRE(wxWinUI3MenuBarCloseForTesting(
        fixture.menuBarWindow, topPath, WXSIZEOF(topPath)));
    CHECK_FALSE(wxWinUI3MenuBarOpenForTesting(
        fixture.menuBarWindow, topPath, WXSIZEOF(topPath)));

    DrainMenuEvents();
    CHECK(replacedFromChildClose);
    CHECK(firstBar == nullptr);
    CHECK(fixture.frame->GetMenuBar() == winningBar);
    CHECK(!oldWindow);

    fixture.menuBar = winningBar;
    fixture.menuBarWindow = FindOnlyFrameChild(fixture.frame);
    REQUIRE(fixture.menuBarWindow != nullptr);
    CHECK(wxWinUI3MenuBarHeightForTesting(fixture.menuBarWindow) > 0);
}

TEST_CASE("WinUIMenu::PopupTimerFailureIsBounded",
          "[WinUIMenu][menu]")
{
    wxFrame * const frame =
        new wxFrame(wxTheApp->GetTopWindow(), wxID_ANY,
                    "WinUI bounded popup test");
    frame->Show();
    DrainMenuEvents();

    wxMenu popup;
    popup.Append(wxID_HIGHEST + 561, "Command");
    unsigned opens = 0;
    unsigned closes = 0;
    frame->Bind(
        wxEVT_MENU_OPEN,
        [&](wxMenuEvent& event)
        {
            ++opens;
            event.Skip();
        });
    frame->Bind(
        wxEVT_MENU_CLOSE,
        [&](wxMenuEvent& event)
        {
            ++closes;
            event.Skip();
        });

    wxWinUI3FailNextPopupTimersForTesting(true, true);
    wxStopWatch elapsed;
    CHECK(wxWinUI3PopupMenuForTesting(frame, &popup, 4, 4));
    CHECK(elapsed.Time() < 2000);
    CHECK(opens == 1);
    CHECK(closes == 1);
    wxWinUI3FailNextPopupTimersForTesting(false, false);

    opens = 0;
    closes = 0;
    wxWinUI3FailNextPopupTimersForTesting(false, true);
    wxWinUI3RejectNextPopupDrainForTesting();
    wxStopWatch rejectedDrainElapsed;
    CHECK(wxWinUI3PopupMenuForTesting(frame, &popup, 4, 4));
    CHECK(rejectedDrainElapsed.Time() < 2000);
    CHECK(opens == 1);
    CHECK(closes == 1);
    wxWinUI3FailNextPopupTimersForTesting(false, false);

    frame->Destroy();
    DrainMenuEvents();
}

TEST_CASE("WinUIMenu::PopupFrameDestroyedFromOpenIsBounded",
          "[WinUIMenu][menu]")
{
    wxFrame *frame =
        new wxFrame(wxTheApp->GetTopWindow(), wxID_ANY,
                    "WinUI popup destroy test");
    frame->Show();
    DrainMenuEvents();

    wxMenu popup;
    popup.Append(wxID_HIGHEST + 564, "Command");
    unsigned opens = 0;
    unsigned closes = 0;
    const wxWeakRef<wxWindow> frameAlive(frame);
    frame->Bind(
        wxEVT_MENU_OPEN,
        [&](wxMenuEvent& event)
        {
            ++opens;
            frame->Destroy();
            event.Skip();
        });
    frame->Bind(
        wxEVT_MENU_CLOSE,
        [&](wxMenuEvent& event)
        {
            ++closes;
            event.Skip();
        });

    wxStopWatch elapsed;
    CHECK(wxWinUI3PopupMenuForTesting(frame, &popup, 4, 4));
    CHECK(elapsed.Time() < 2000);
    CHECK(opens == 1);
    CHECK(closes == 1);

    frame = nullptr;
    DrainMenuEvents();
    CHECK(!frameAlive);
}

TEST_CASE("WinUIMenu::PopupOpenExceptionDoesNotFallBack",
          "[WinUIMenu][menu]")
{
    PopupOpenExceptionFrame * const frame =
        new PopupOpenExceptionFrame;
    frame->Show();
    DrainMenuEvents();

    wxMenu popup;
    popup.Append(wxID_HIGHEST + 566, "Command");

    // Call the WinUI implementation directly: returning true is the contract
    // that prevents wxWindow::PopupMenu() from starting a second native
    // OPEN/CLOSE session after the first one has already begun.
    wxStopWatch elapsed;
    CHECK(wxWinUI3PopupMenuForTesting(frame, &popup, 4, 4));
    CHECK(elapsed.Time() < 2000);
    CHECK(frame->opens == 1);
    CHECK(frame->closes == 1);

    frame->Destroy();
    DrainMenuEvents();
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_MENUS
