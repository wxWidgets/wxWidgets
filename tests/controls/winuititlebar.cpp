///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuititlebar.cpp
// Purpose:     WinAppSDK title-bar policy and full-screen MenuBar contracts
// Author:      wxWidgets development team
// Created:     2026-08-02
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/utils.h"
#endif

#if wxUSE_MENUS
    #include "wx/menu.h"
#endif

#include "wx/msw/wrapwin.h"
#include "wx/weakref.h"
#include "wx/winui/private/titlebar.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"

#include <memory>

namespace
{

void DrainTitleBarEvents(unsigned rounds = 8)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        wxYield();
        wxMilliSleep(2);
    }
}

template <typename Predicate>
bool DrainTitleBarEventsUntil(Predicate predicate, unsigned rounds = 200)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        if ( predicate() )
            return true;
        DrainTitleBarEvents(1);
    }

    return predicate();
}

wxFrame *CreateTitleBarFrame(const wxString& title, int ordinal = 0)
{
    wxFrame * const frame = new wxFrame(
        wxTheApp->GetTopWindow(), wxID_ANY, title,
        wxPoint(-30000 + ordinal * 360, -30000), wxSize(320, 200),
        wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);

    const HWND hwnd = reinterpret_cast<HWND>(frame->GetHWND());
    REQUIRE(hwnd != nullptr);
    ::SetWindowLongPtr(
        hwnd, GWL_EXSTYLE,
        ::GetWindowLongPtr(hwnd, GWL_EXSTYLE) |
            WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW);
    frame->ShowWithoutActivating();
    REQUIRE(wxWinUITopLevelHost::ForWindow(frame, true) != nullptr);
    DrainTitleBarEvents();
    return frame;
}

wxFrame *ResolveLiveTitleBarFrame(const wxWeakRef<wxWindow>& lifetime)
{
    wxWindow * const window = lifetime.get();
    if ( !window || window->IsBeingDeleted() ||
         wxWinUITLWHostIsDestroyScheduled(window) )
    {
        return nullptr;
    }

    return static_cast<wxFrame *>(window);
}

void DestroyTitleBarFrame(wxFrame *frame)
{
    if ( !frame )
        return;

    const wxWeakRef<wxWindow> lifetime(frame);
    if ( frame->IsFullScreen() )
        frame->ShowFullScreen(false);
    frame->Destroy();
    DrainTitleBarEventsUntil([lifetime]() { return !lifetime; });
}

struct TitleBarFrameGuard
{
    explicit TitleBarFrameGuard(wxFrame *value)
        : lifetime(value)
    {
    }
    ~TitleBarFrameGuard()
    {
        if ( wxWindow * const live = lifetime.get() )
            DestroyTitleBarFrame(static_cast<wxFrame *>(live));
    }

    wxWeakRef<wxWindow> lifetime;
};

enum class ReentrantTitleBarAction
{
    None,
    Detach,
    Destroy
};

wxWinUITitleBarHookPointForTesting gs_reentrantTitleBarPoint =
    wxWinUITitleBarHookPointForTesting::AfterExtensionWrite;
ReentrantTitleBarAction gs_reentrantTitleBarAction =
    ReentrantTitleBarAction::None;
bool gs_reentrantTitleBarHookCalled = false;

void ReenterTitleBarTransaction(
    wxWindow *window,
    wxWinUITitleBarHookPointForTesting point)
{
    if ( point != gs_reentrantTitleBarPoint ||
         gs_reentrantTitleBarAction == ReentrantTitleBarAction::None )
    {
        return;
    }

    gs_reentrantTitleBarHookCalled = true;
    wxWinUITitleBarSetHookForTesting(nullptr);
    if ( gs_reentrantTitleBarAction == ReentrantTitleBarAction::Detach )
    {
        wxWinUITopLevelHost * const host =
            wxWinUITopLevelHost::ForWindow(window, false);
        if ( host )
            wxWinUITitleBarDetachSystemPolicy(host);
    }
    else
    {
        window->Destroy();
    }
}

class TitleBarTestSeamGuard
{
public:
    TitleBarTestSeamGuard(
        wxWinUITitleBarHookPointForTesting point,
        ReentrantTitleBarAction action)
    {
        gs_reentrantTitleBarPoint = point;
        gs_reentrantTitleBarAction = action;
        gs_reentrantTitleBarHookCalled = false;
        wxWinUITitleBarSetFaultMaskForTesting(
            wxWINUI_TITLEBAR_FAULT_NONE);
        wxWinUITitleBarSetHookForTesting(&ReenterTitleBarTransaction);
    }

    ~TitleBarTestSeamGuard()
    {
        wxWinUITitleBarSetHookForTesting(nullptr);
        wxWinUITitleBarSetFaultMaskForTesting(
            wxWINUI_TITLEBAR_FAULT_NONE);
        gs_reentrantTitleBarAction = ReentrantTitleBarAction::None;
    }
};

class TitleBarFaultGuard
{
public:
    ~TitleBarFaultGuard()
    {
        wxWinUITitleBarSetFaultMaskForTesting(
            wxWINUI_TITLEBAR_FAULT_NONE);
    }
};

using HostInitializationPoint =
    wxWinUITopLevelHost::InitializationHookPointForTest;

HostInitializationPoint gs_hostInitializationDestroyPoint =
    HostInitializationPoint::AfterTLWBindings;
bool gs_hostInitializationHookCalled = false;
bool gs_hostInitializationPolicyObserved = false;
unsigned gs_hostInitializationPoliciesBefore = 0;

void DestroyAndPumpDuringHostInitialization(
    wxWindow *window,
    HostInitializationPoint point)
{
    if ( point != gs_hostInitializationDestroyPoint )
        return;

    gs_hostInitializationHookCalled = true;
    gs_hostInitializationPolicyObserved =
        wxWinUITitleBarGetLivePolicyCountForTesting() >
            gs_hostInitializationPoliciesBefore;
    wxWinUITopLevelHost::TestSetInitializationHook(nullptr);
    window->Destroy();
    DrainTitleBarEvents(3);
}

class HostInitializationSeamGuard
{
public:
    HostInitializationSeamGuard(
        HostInitializationPoint destroyPoint,
        unsigned faultMask =
            wxWinUITopLevelHost::TestInitializationFault_None)
    {
        gs_hostInitializationDestroyPoint = destroyPoint;
        gs_hostInitializationHookCalled = false;
        gs_hostInitializationPolicyObserved = false;
        gs_hostInitializationPoliciesBefore =
            wxWinUITitleBarGetLivePolicyCountForTesting();
        wxWinUITopLevelHost::TestFailInitialization(faultMask);
        wxWinUITopLevelHost::TestSetInitializationHook(
            &DestroyAndPumpDuringHostInitialization);
    }

    ~HostInitializationSeamGuard()
    {
        wxWinUITopLevelHost::TestSetInitializationHook(nullptr);
        wxWinUITopLevelHost::TestFailInitialization(
            wxWinUITopLevelHost::TestInitializationFault_None);
    }
};

struct HostInitializationCounters
{
    unsigned hosts = 0;
    unsigned hostLifetimes = 0;
    unsigned subclassContexts = 0;
    unsigned titleBarPolicies = 0;
    unsigned rootHandlerAdds = 0;
    unsigned rootHandlerRevokes = 0;
};

HostInitializationCounters CaptureHostInitializationCounters()
{
    HostInitializationCounters counters;
    counters.hosts = wxWinUITopLevelHost::GetLiveHostCount();
    counters.hostLifetimes =
        wxWinUITopLevelHost::GetLiveHostLifetimeCount();
    counters.subclassContexts =
        wxWinUITopLevelHost::GetLiveSubclassContextCount();
    counters.titleBarPolicies =
        wxWinUITitleBarGetLivePolicyCountForTesting();
    counters.rootHandlerAdds =
        wxWinUITopLevelHost::GetRootHandlerAddCount();
    counters.rootHandlerRevokes =
        wxWinUITopLevelHost::GetRootHandlerRevokeCount();
    return counters;
}

bool HostInitializationStateRetired(
    const HostInitializationCounters& before)
{
    return wxWinUITopLevelHost::GetLiveHostCount() == before.hosts &&
           wxWinUITopLevelHost::GetLiveHostLifetimeCount() ==
               before.hostLifetimes &&
           wxWinUITopLevelHost::GetLiveSubclassContextCount() ==
               before.subclassContexts &&
           wxWinUITitleBarGetLivePolicyCountForTesting() ==
               before.titleBarPolicies;
}

wxFrame *CreateHostInitializationFrame(const wxString& title)
{
    wxFrame * const frame = new wxFrame(
        wxTheApp->GetTopWindow(), wxID_ANY, title,
        wxPoint(-30000, -30000), wxSize(320, 200),
        wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    const HWND hwnd = reinterpret_cast<HWND>(frame->GetHWND());
    REQUIRE(hwnd != nullptr);
    ::SetWindowLongPtr(
        hwnd, GWL_EXSTYLE,
        ::GetWindowLongPtr(hwnd, GWL_EXSTYLE) |
            WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW);
    frame->ShowWithoutActivating();
    REQUIRE(wxWinUITopLevelHost::FindForTLW(frame) == nullptr);
    return frame;
}

#if wxUSE_MENUS
wxMenuBar *CreateTitleBarMenuBar(const wxString& label)
{
    wxMenuBar * const menuBar = new wxMenuBar;
    menuBar->Append(new wxMenu, label);
    return menuBar;
}

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
#endif

} // anonymous namespace

TEST_CASE("WinUITitleBar::SystemDefaultAndPrivateAppWindowSeam",
          "[winui-beta-aux][winui-beta-titlebar][titlebar]")
{
    DrainTitleBarEvents();
    const unsigned policiesBefore =
        wxWinUITitleBarGetLivePolicyCountForTesting();
    wxFrame * const frame = CreateTitleBarFrame("titlebar system mode");
    TitleBarFrameGuard cleanup(frame);

    wxWinUITitleBarSnapshot initial;
    REQUIRE(wxWinUITitleBarSnapshotForTesting(frame, &initial));
    CHECK(initial.attached);
    CHECK(initial.hwndIdentityCurrent);
    CHECK(initial.hwnd == frame->GetHWND());
    CHECK(initial.hwndGeneration != 0);
    CHECK(initial.currentHwndGeneration == initial.hwndGeneration);
    CHECK(initial.nativeHwndGeneration != 0);
    CHECK(initial.currentNativeHwndGeneration ==
          initial.nativeHwndGeneration);
    CHECK(initial.mode != wxWinUITitleBarMode::ExtendedAppWindow);
    if ( initial.mode == wxWinUITitleBarMode::Unknown )
        CHECK_FALSE(initial.systemFallback);
    REQUIRE(initial.firstPartyXamlTitleBarScanSucceeded);
    CHECK_FALSE(initial.firstPartyXamlTitleBarInstalled);
    CHECK(initial.firstPartyXamlTitleBarCount == 0);
    CHECK(wxWinUITitleBarGetLivePolicyCountForTesting() ==
          policiesBefore + 1);

    const HWND hwnd = reinterpret_cast<HWND>(frame->GetHWND());
    const LONG_PTR styleBefore = ::GetWindowLongPtr(hwnd, GWL_STYLE);
    const LONG_PTR exStyleBefore = ::GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    REQUIRE(::GetSystemMenu(hwnd, FALSE) != nullptr);

    if ( initial.mode == wxWinUITitleBarMode::System &&
         initial.appWindowAvailable && initial.customizationSupported )
    {
        REQUIRE(initial.appWindowId != 0);
        CHECK(initial.appWindowIdentityCurrent);
        CHECK(initial.currentAppWindowId == initial.appWindowId);
        REQUIRE(wxWinUITitleBarSetExtendedForTesting(frame, true));

        wxWinUITitleBarSnapshot extended;
        REQUIRE(wxWinUITitleBarSnapshotForTesting(frame, &extended));
        CHECK(extended.mode == wxWinUITitleBarMode::ExtendedAppWindow);
        CHECK(extended.dragRectangleWriteCommitted);
        CHECK(extended.dragRectangleWidth > 0);
        CHECK(extended.dragRectangleHeight > 0);
        CHECK(extended.hwnd == initial.hwnd);
        CHECK(extended.hwndGeneration == initial.hwndGeneration);
        REQUIRE(extended.firstPartyXamlTitleBarScanSucceeded);
        CHECK_FALSE(extended.firstPartyXamlTitleBarInstalled);
        CHECK(::GetWindowLongPtr(hwnd, GWL_STYLE) == styleBefore);
        CHECK(::GetWindowLongPtr(hwnd, GWL_EXSTYLE) == exStyleBefore);
        CHECK(::GetSystemMenu(hwnd, FALSE) != nullptr);

        REQUIRE(wxWinUITitleBarSetExtendedForTesting(frame, false));
        wxWinUITitleBarSnapshot disabled;
        REQUIRE(wxWinUITitleBarSnapshotForTesting(frame, &disabled));
        CHECK(disabled.mode == wxWinUITitleBarMode::System);
        CHECK_FALSE(disabled.systemFallback);
        CHECK_FALSE(disabled.dragRectangleWriteCommitted);

        // One-shot failure occurs after the real extension write and must
        // roll the AppWindowTitleBar back to system mode, not merely flip a
        // test flag. A subsequent request proves that the controller re-arms.
        wxWinUITitleBarFailNextExtensionForTesting();
        CHECK_FALSE(wxWinUITitleBarSetExtendedForTesting(frame, true));
        wxWinUITitleBarSnapshot fallback;
        REQUIRE(wxWinUITitleBarSnapshotForTesting(frame, &fallback));
        CHECK(fallback.mode == wxWinUITitleBarMode::System);
        CHECK(fallback.systemFallback);

        REQUIRE(wxWinUITitleBarSetExtendedForTesting(frame, true));
        REQUIRE(wxWinUITitleBarSetExtendedForTesting(frame, false));
    }
    else
    {
        // Unsupported OS/runtime is an explicit system fallback. A failed
        // AppWindow observation is instead Unknown and never masquerades as
        // a successful fallback.
        CHECK_FALSE(wxWinUITitleBarSetExtendedForTesting(frame, true));
        wxWinUITitleBarSnapshot fallback;
        REQUIRE(wxWinUITitleBarSnapshotForTesting(frame, &fallback));
        if ( fallback.mode == wxWinUITitleBarMode::System )
            CHECK(fallback.systemFallback);
        else
        {
            CHECK(fallback.mode == wxWinUITitleBarMode::Unknown);
            CHECK_FALSE(fallback.systemFallback);
        }
    }

    if ( initial.appWindowAvailable && initial.appWindowIdentityCurrent )
    {
        REQUIRE(wxWinUITitleBarInvalidateAppWindowIdentityForTesting(frame));
        wxWinUITitleBarSnapshot staleAppWindow;
        REQUIRE(wxWinUITitleBarSnapshotForTesting(frame, &staleAppWindow));
        CHECK_FALSE(staleAppWindow.appWindowIdentityCurrent);
        CHECK(staleAppWindow.currentAppWindowId == 0);
        CHECK_FALSE(wxWinUITitleBarSetExtendedForTesting(frame, true));
    }

    // HWND values can be recycled independently of the wxWindow/HWND
    // association generation. Make only the native identity stale and prove
    // the controller refuses every subsequent AppWindow/title-bar mutation.
    REQUIRE(wxWinUITitleBarInvalidateNativeIdentityForTesting(frame));
    wxWinUITitleBarSnapshot staleNative;
    REQUIRE(wxWinUITitleBarSnapshotForTesting(frame, &staleNative));
    CHECK_FALSE(staleNative.hwndIdentityCurrent);
    CHECK(staleNative.hwndGeneration ==
          staleNative.currentHwndGeneration);
    CHECK(staleNative.nativeHwndGeneration !=
          staleNative.currentNativeHwndGeneration);
    CHECK_FALSE(wxWinUITitleBarSetExtendedForTesting(frame, true));
}

TEST_CASE("WinUITitleBar::TwoTopLevelsKeepDistinctAppWindowIdentity",
          "[winui-beta-aux][winui-beta-titlebar][titlebar]")
{
    wxFrame * const first = CreateTitleBarFrame("titlebar first", 0);
    TitleBarFrameGuard cleanupFirst(first);
    wxFrame * const second = CreateTitleBarFrame("titlebar second", 1);
    TitleBarFrameGuard cleanupSecond(second);

    wxWinUITitleBarSnapshot firstSnapshot;
    wxWinUITitleBarSnapshot secondSnapshot;
    REQUIRE(wxWinUITitleBarSnapshotForTesting(first, &firstSnapshot));
    REQUIRE(wxWinUITitleBarSnapshotForTesting(second, &secondSnapshot));
    CHECK(firstSnapshot.hwnd != secondSnapshot.hwnd);
    CHECK(firstSnapshot.hwndGeneration != 0);
    CHECK(secondSnapshot.hwndGeneration != 0);
    CHECK(firstSnapshot.nativeHwndGeneration != 0);
    CHECK(secondSnapshot.nativeHwndGeneration != 0);
    CHECK(firstSnapshot.currentHwndGeneration ==
          firstSnapshot.hwndGeneration);
    CHECK(secondSnapshot.currentHwndGeneration ==
          secondSnapshot.hwndGeneration);
    CHECK(firstSnapshot.currentNativeHwndGeneration ==
          firstSnapshot.nativeHwndGeneration);
    CHECK(secondSnapshot.currentNativeHwndGeneration ==
          secondSnapshot.nativeHwndGeneration);
    if ( firstSnapshot.appWindowAvailable &&
         secondSnapshot.appWindowAvailable )
    {
        CHECK(firstSnapshot.appWindowIdentityCurrent);
        CHECK(secondSnapshot.appWindowIdentityCurrent);
        CHECK(firstSnapshot.currentAppWindowId ==
              firstSnapshot.appWindowId);
        CHECK(secondSnapshot.currentAppWindowId ==
              secondSnapshot.appWindowId);
        CHECK(firstSnapshot.appWindowId != 0);
        CHECK(secondSnapshot.appWindowId != 0);
        CHECK(firstSnapshot.appWindowId != secondSnapshot.appWindowId);
    }

    if ( firstSnapshot.mode == wxWinUITitleBarMode::System &&
         firstSnapshot.customizationSupported )
    {
        REQUIRE(wxWinUITitleBarSetExtendedForTesting(first, true));
        wxWinUITitleBarSnapshot firstExtended;
        wxWinUITitleBarSnapshot secondStillSystem;
        REQUIRE(wxWinUITitleBarSnapshotForTesting(first, &firstExtended));
        REQUIRE(wxWinUITitleBarSnapshotForTesting(
            second, &secondStillSystem));
        CHECK(firstExtended.mode ==
              wxWinUITitleBarMode::ExtendedAppWindow);
        CHECK(secondStillSystem.mode == wxWinUITitleBarMode::System);
        REQUIRE(wxWinUITitleBarSetExtendedForTesting(first, false));
    }
}

TEST_CASE("WinUITitleBar::UnknownFailsClosedUntilExactRecovery",
          "[winui-beta-aux][winui-beta-titlebar][titlebar][transaction]")
{
    wxFrame * const frame = CreateTitleBarFrame("titlebar unknown");
    TitleBarFrameGuard cleanup(frame);
    TitleBarFaultGuard resetFaults;

    wxWinUITitleBarSnapshot initial;
    REQUIRE(wxWinUITitleBarSnapshotForTesting(frame, &initial));
    if ( initial.mode != wxWinUITitleBarMode::System ||
         !initial.customizationSupported )
    {
        INFO("The runtime has no customizable AppWindowTitleBar");
        SUCCEED();
        return;
    }

    wxWinUITitleBarSetFaultMaskForTesting(
        wxWINUI_TITLEBAR_FAULT_RESET_TO_DEFAULT |
        wxWINUI_TITLEBAR_FAULT_READ_AFTER_MUTATION);
    CHECK_FALSE(wxWinUITitleBarSetExtendedForTesting(frame, false));

    wxWinUITitleBarSnapshot unknown;
    REQUIRE(wxWinUITitleBarSnapshotForTesting(frame, &unknown));
    CHECK(unknown.mode == wxWinUITitleBarMode::Unknown);
    CHECK_FALSE(unknown.systemFallback);
    CHECK_FALSE(wxWinUITitleBarSetExtendedForTesting(frame, true));

    wxWinUITitleBarSetFaultMaskForTesting(
        wxWINUI_TITLEBAR_FAULT_NONE);
    REQUIRE(wxWinUITitleBarSetExtendedForTesting(frame, false));
    wxWinUITitleBarSnapshot recovered;
    REQUIRE(wxWinUITitleBarSnapshotForTesting(frame, &recovered));
    CHECK(recovered.mode == wxWinUITitleBarMode::System);
    CHECK_FALSE(recovered.systemFallback);
}

TEST_CASE("WinUITitleBar::DetachDuringSetterExtractsBeforeReset",
          "[winui-beta-aux][winui-beta-titlebar][titlebar][transaction]")
{
    DrainTitleBarEvents();
    const unsigned policiesBefore =
        wxWinUITitleBarGetLivePolicyCountForTesting();
    wxFrame * const frame = CreateTitleBarFrame("titlebar detach setter");
    TitleBarFrameGuard cleanup(frame);

    wxWinUITitleBarSnapshot initial;
    REQUIRE(wxWinUITitleBarSnapshotForTesting(frame, &initial));
    if ( initial.mode != wxWinUITitleBarMode::System ||
         !initial.customizationSupported )
    {
        INFO("The runtime has no customizable AppWindowTitleBar");
        SUCCEED();
        return;
    }

    {
        TitleBarTestSeamGuard seam(
            wxWinUITitleBarHookPointForTesting::AfterExtensionWrite,
            ReentrantTitleBarAction::Detach);
        CHECK_FALSE(wxWinUITitleBarSetExtendedForTesting(frame, true));
        CHECK(gs_reentrantTitleBarHookCalled);
    }

    wxWinUITitleBarSnapshot detached;
    CHECK_FALSE(wxWinUITitleBarSnapshotForTesting(frame, &detached));
    CHECK(wxWinUITitleBarGetLivePolicyCountForTesting() == policiesBefore);
}

TEST_CASE("WinUITitleBar::SingleResetBoundaryCannotRepublishAfterDestroy",
           "[winui-beta-aux][winui-beta-titlebar][titlebar][transaction]")
{
    DrainTitleBarEvents();
    const unsigned policiesBefore =
        wxWinUITitleBarGetLivePolicyCountForTesting();
    const unsigned hostsBefore = wxWinUITopLevelHost::GetLiveHostCount();
    wxFrame * const frame = CreateTitleBarFrame("titlebar destroy reset");
    TitleBarFrameGuard cleanup(frame);

    wxWinUITitleBarSnapshot initial;
    REQUIRE(wxWinUITitleBarSnapshotForTesting(frame, &initial));
    if ( initial.mode != wxWinUITitleBarMode::System ||
         !initial.customizationSupported )
    {
        INFO("The runtime has no customizable AppWindowTitleBar");
        SUCCEED();
        return;
    }
    const wxWeakRef<wxWindow> lifetime(frame);
    REQUIRE(wxWinUITitleBarSetExtendedForTesting(frame, true));

    // ResetToDefault() is the only supported reset mutation. Poison the old
    // explicit-disable boundary with a destructive callback: a successful
    // reset with the callback untouched discriminates the single-call path
    // from the former false-then-ResetToDefault sequence.
    {
        TitleBarTestSeamGuard seam(
            wxWinUITitleBarHookPointForTesting::AfterDisableWrite,
            ReentrantTitleBarAction::Destroy);
        REQUIRE(wxWinUITitleBarSetExtendedForTesting(frame, false));
        CHECK_FALSE(gs_reentrantTitleBarHookCalled);
    }
    REQUIRE(lifetime);
    wxWinUITitleBarSnapshot reset;
    REQUIRE(wxWinUITitleBarSnapshotForTesting(frame, &reset));
    REQUIRE(reset.mode == wxWinUITitleBarMode::System);

    // Destruction immediately after the one real WinRT mutation must still
    // invalidate publication before its readback can publish System again.
    REQUIRE(wxWinUITitleBarSetExtendedForTesting(frame, true));
    {
        TitleBarTestSeamGuard seam(
            wxWinUITitleBarHookPointForTesting::AfterResetToDefault,
            ReentrantTitleBarAction::Destroy);
        CHECK_FALSE(wxWinUITitleBarSetExtendedForTesting(frame, false));
        CHECK(gs_reentrantTitleBarHookCalled);
    }

    // Attached queries and setters must lose authority as soon as Destroy()
    // is scheduled, before the pending-delete pump drains the actual TLW.
    wxWinUITitleBarSnapshot scheduled;
    CHECK_FALSE(wxWinUITitleBarSnapshotForTesting(frame, &scheduled));
    CHECK_FALSE(wxWinUITitleBarSetExtendedForTesting(frame, true));

    REQUIRE(DrainTitleBarEventsUntil(
        [lifetime, policiesBefore, hostsBefore]()
        {
            return !lifetime &&
                   wxWinUITitleBarGetLivePolicyCountForTesting() ==
                       policiesBefore &&
                   wxWinUITopLevelHost::GetLiveHostCount() == hostsBefore;
        }));
}

TEST_CASE("WinUITitleBar::DestroyFromExceptionLogCannotRepublish",
          "[winui-beta-aux][winui-beta-titlebar][titlebar][transaction]")
{
    DrainTitleBarEvents();
    const unsigned policiesBefore =
        wxWinUITitleBarGetLivePolicyCountForTesting();
    const unsigned hostsBefore = wxWinUITopLevelHost::GetLiveHostCount();
    wxFrame * const frame = CreateTitleBarFrame("titlebar destroy log");
    TitleBarFrameGuard cleanup(frame);

    wxWinUITitleBarSnapshot initial;
    REQUIRE(wxWinUITitleBarSnapshotForTesting(frame, &initial));
    if ( initial.mode != wxWinUITitleBarMode::System ||
         !initial.customizationSupported )
    {
        INFO("The runtime has no customizable AppWindowTitleBar");
        SUCCEED();
        return;
    }

    const wxWeakRef<wxWindow> lifetime(frame);
    {
        TitleBarTestSeamGuard seam(
            wxWinUITitleBarHookPointForTesting::BeforeExceptionLog,
            ReentrantTitleBarAction::Destroy);
        wxWinUITitleBarSetFaultMaskForTesting(
            wxWINUI_TITLEBAR_FAULT_EXTENSION_WRITE_AFTER);
        CHECK_FALSE(wxWinUITitleBarSetExtendedForTesting(frame, true));
        CHECK(gs_reentrantTitleBarHookCalled);
    }

    wxWinUITitleBarSnapshot scheduled;
    CHECK_FALSE(wxWinUITitleBarSnapshotForTesting(frame, &scheduled));
    CHECK_FALSE(wxWinUITitleBarSetExtendedForTesting(frame, false));

    REQUIRE(DrainTitleBarEventsUntil(
        [lifetime, policiesBefore, hostsBefore]()
        {
            return !lifetime &&
                   wxWinUITitleBarGetLivePolicyCountForTesting() ==
                       policiesBefore &&
                   wxWinUITopLevelHost::GetLiveHostCount() == hostsBefore;
        }));
}

TEST_CASE("WinUITitleBar::DestroyAndYieldDuringHostInitializationRollsBack",
          "[winui-beta-aux][winui-beta-titlebar][titlebar][transaction]"
          "[host-initialization]")
{
    DrainTitleBarEvents();
    const HostInitializationCounters before =
        CaptureHostInitializationCounters();
    wxFrame * const frame =
        CreateHostInitializationFrame("host init destroy boundary");
    TitleBarFrameGuard cleanup(frame);
    const wxWeakRef<wxWindow> lifetime(frame);

    {
        HostInitializationSeamGuard seam(
            HostInitializationPoint::AfterTLWBindings);
        CHECK(wxWinUITopLevelHost::ForWindow(frame, true) == nullptr);
        CHECK(gs_hostInitializationHookCalled);
        CHECK(gs_hostInitializationPolicyObserved);
    }

    // The provisional registry entry is withdrawn before ForWindow() returns,
    // whether the nested yield completed the TLW delete or left it scheduled.
    if ( wxWindow * const live = lifetime.get() )
        CHECK(wxWinUITopLevelHost::FindForTLW(live) == nullptr);
    CHECK(wxWinUITopLevelHost::GetLiveHostCount() == before.hosts);
    REQUIRE(DrainTitleBarEventsUntil(
        [lifetime, before]()
        {
            return !lifetime && HostInitializationStateRetired(before);
        }));

    const unsigned adds =
        wxWinUITopLevelHost::GetRootHandlerAddCount() -
        before.rootHandlerAdds;
    const unsigned revokes =
        wxWinUITopLevelHost::GetRootHandlerRevokeCount() -
        before.rootHandlerRevokes;
    CHECK(adds != 0);
    CHECK(adds == revokes);
}

TEST_CASE("WinUITitleBar::DestroyAndYieldFromHostInitLogRollsBack",
          "[winui-beta-aux][winui-beta-titlebar][titlebar][transaction]"
          "[host-initialization]")
{
    DrainTitleBarEvents();
    const HostInitializationCounters before =
        CaptureHostInitializationCounters();
    wxFrame * const frame =
        CreateHostInitializationFrame("host init log destroy boundary");
    TitleBarFrameGuard cleanup(frame);
    const wxWeakRef<wxWindow> lifetime(frame);

    {
        HostInitializationSeamGuard seam(
            HostInitializationPoint::BeforeExceptionLog,
            wxWinUITopLevelHost::TestInitializationFault_AfterTLWBindings);
        CHECK(wxWinUITopLevelHost::ForWindow(frame, true) == nullptr);
        CHECK(gs_hostInitializationHookCalled);
        CHECK(gs_hostInitializationPolicyObserved);
    }

    if ( wxWindow * const live = lifetime.get() )
        CHECK(wxWinUITopLevelHost::FindForTLW(live) == nullptr);
    CHECK(wxWinUITopLevelHost::GetLiveHostCount() == before.hosts);
    REQUIRE(DrainTitleBarEventsUntil(
        [lifetime, before]()
        {
            return !lifetime && HostInitializationStateRetired(before);
        }));

    const unsigned adds =
        wxWinUITopLevelHost::GetRootHandlerAddCount() -
        before.rootHandlerAdds;
    const unsigned revokes =
        wxWinUITopLevelHost::GetRootHandlerRevokeCount() -
        before.rootHandlerRevokes;
    CHECK(adds != 0);
    CHECK(adds == revokes);
}

#if wxUSE_MENUS

TEST_CASE("WinUITitleBar::FullScreenNoMenuBarHidesXamlMenuBar",
          "[winui-beta-aux][winui-beta-titlebar][titlebar][menu]")
{
    wxFrame * const frame = CreateTitleBarFrame("fullscreen menubar");
    TitleBarFrameGuard cleanup(frame);
    wxMenuBar * const menuBar = new wxMenuBar;
    menuBar->Append(new wxMenu, "&File");
    frame->SetMenuBar(menuBar);
    DrainTitleBarEvents();

    wxWindow * const menuBarWindow = FindOnlyFrameChild(frame);
    REQUIRE(menuBarWindow != nullptr);
    REQUIRE(menuBarWindow->IsShown());
    REQUIRE(frame->GetClientAreaOrigin().y > 0);

    REQUIRE(frame->ShowFullScreen(true, wxFULLSCREEN_NOMENUBAR));
    DrainTitleBarEvents();
    CHECK_FALSE(menuBarWindow->IsShown());
    CHECK(frame->GetClientAreaOrigin().y == 0);

    REQUIRE(frame->ShowFullScreen(false));
    DrainTitleBarEvents();
    CHECK(menuBarWindow->IsShown());
    CHECK(frame->GetClientAreaOrigin().y > 0);
}

TEST_CASE("WinUITitleBar::MenuInstalledDuringFullScreenStaysHidden",
          "[winui-beta-aux][winui-beta-titlebar][titlebar][menu]")
{
    wxFrame * const frame = CreateTitleBarFrame("late fullscreen menubar");
    TitleBarFrameGuard cleanup(frame);
    REQUIRE(frame->ShowFullScreen(true, wxFULLSCREEN_NOMENUBAR));

    wxMenuBar * const menuBar = new wxMenuBar;
    menuBar->Append(new wxMenu, "&File");
    frame->SetMenuBar(menuBar);
    DrainTitleBarEvents();

    wxWindow * const menuBarWindow = FindOnlyFrameChild(frame);
    REQUIRE(menuBarWindow != nullptr);
    CHECK_FALSE(menuBarWindow->IsShown());
    CHECK(frame->GetClientAreaOrigin().y == 0);

    REQUIRE(frame->ShowFullScreen(false));
    DrainTitleBarEvents();
    CHECK(menuBarWindow->IsShown());
    CHECK(frame->GetClientAreaOrigin().y > 0);
}

TEST_CASE("WinUITitleBar::InitiallyHiddenMenuIsNotRestored",
          "[winui-beta-aux][winui-beta-titlebar][titlebar][menu]")
{
    wxFrame * const frame = CreateTitleBarFrame(
        "initially hidden fullscreen menubar");
    TitleBarFrameGuard cleanup(frame);
    frame->SetMenuBar(CreateTitleBarMenuBar("&Hidden"));
    DrainTitleBarEvents();

    wxWindow * const menuBarWindow = FindOnlyFrameChild(frame);
    REQUIRE(menuBarWindow != nullptr);
    REQUIRE(menuBarWindow->Show(false));
    REQUIRE_FALSE(menuBarWindow->IsShown());

    REQUIRE(frame->ShowFullScreen(true, wxFULLSCREEN_NOMENUBAR));
    REQUIRE(frame->ShowFullScreen(false));
    DrainTitleBarEvents();
    CHECK_FALSE(menuBarWindow->IsShown());
}

TEST_CASE("WinUITitleBar::FullScreenConvergesReentrantMenuReplacements",
          "[winui-beta-aux][winui-beta-titlebar][titlebar][menu]"
          "[transaction]")
{
    wxFrame * const frame = CreateTitleBarFrame(
        "reentrant fullscreen menubar");
    TitleBarFrameGuard cleanup(frame);
    frame->SetMenuBar(CreateTitleBarMenuBar("&First"));
    DrainTitleBarEvents();

    wxWindow * const firstWindow = FindOnlyFrameChild(frame);
    REQUIRE(firstWindow != nullptr);
    const wxWeakRef<wxWindow> frameLifetime(frame);
    const auto replacedOnEntry = std::make_shared<bool>(false);
    firstWindow->Bind(
        wxEVT_SHOW,
        [frameLifetime, replacedOnEntry](wxShowEvent& event)
        {
            if ( !event.IsShown() && !*replacedOnEntry )
            {
                *replacedOnEntry = true;
                if ( wxFrame * const liveFrame =
                         ResolveLiveTitleBarFrame(frameLifetime) )
                {
                    liveFrame->SetMenuBar(
                        CreateTitleBarMenuBar("&Entry winner"));
                }
            }
            event.Skip();
        });

    REQUIRE(frame->ShowFullScreen(true, wxFULLSCREEN_NOMENUBAR));
    REQUIRE(DrainTitleBarEventsUntil(
        [frameLifetime, replacedOnEntry]()
        {
            wxFrame * const liveFrame =
                ResolveLiveTitleBarFrame(frameLifetime);
            wxWindow * const current = liveFrame
                ? FindOnlyFrameChild(liveFrame)
                : nullptr;
            return *replacedOnEntry && current && !current->IsShown();
        }));

    wxWindow * const exitCandidate = FindOnlyFrameChild(frame);
    REQUIRE(exitCandidate != nullptr);
    const wxWeakRef<wxWindow> exitCandidateLifetime(exitCandidate);
    const auto replacedOnExit = std::make_shared<bool>(false);
    exitCandidate->Bind(
        wxEVT_SHOW,
        [frameLifetime, replacedOnExit](wxShowEvent& event)
        {
            if ( event.IsShown() && !*replacedOnExit )
            {
                *replacedOnExit = true;
                if ( wxFrame * const liveFrame =
                         ResolveLiveTitleBarFrame(frameLifetime) )
                {
                    liveFrame->SetMenuBar(
                        CreateTitleBarMenuBar("&Exit winner"));
                }
            }
            event.Skip();
        });

    REQUIRE(frame->ShowFullScreen(false));
    REQUIRE(DrainTitleBarEventsUntil(
        [frameLifetime, replacedOnExit]()
        {
            wxFrame * const liveFrame =
                ResolveLiveTitleBarFrame(frameLifetime);
            wxWindow * const current = liveFrame
                ? FindOnlyFrameChild(liveFrame)
                : nullptr;
            return *replacedOnExit && current && current->IsShown();
        }));
    wxWindow * const finalWindow = FindOnlyFrameChild(frame);
    REQUIRE(finalWindow != nullptr);
    CHECK((!exitCandidateLifetime ||
           exitCandidateLifetime.get() != finalWindow));
    CHECK(frame->GetClientAreaOrigin().y > 0);
}

TEST_CASE("WinUITitleBar::FullScreenConvergesReentrantMenuRemoval",
          "[winui-beta-aux][winui-beta-titlebar][titlebar][menu]"
          "[transaction]")
{
    wxFrame * const frame = CreateTitleBarFrame(
        "reentrant fullscreen menu removal");
    TitleBarFrameGuard cleanup(frame);
    frame->SetMenuBar(CreateTitleBarMenuBar("&Remove on entry"));
    DrainTitleBarEvents();

    wxWindow * const initialWindow = FindOnlyFrameChild(frame);
    REQUIRE(initialWindow != nullptr);
    const wxWeakRef<wxWindow> frameLifetime(frame);
    const auto removedOnEntry = std::make_shared<bool>(false);
    initialWindow->Bind(
        wxEVT_SHOW,
        [frameLifetime, removedOnEntry](wxShowEvent& event)
        {
            if ( !event.IsShown() && !*removedOnEntry )
            {
                *removedOnEntry = true;
                if ( wxFrame * const liveFrame =
                         ResolveLiveTitleBarFrame(frameLifetime) )
                {
                    liveFrame->SetMenuBar(nullptr);
                }
            }
            event.Skip();
        });

    REQUIRE(frame->ShowFullScreen(true, wxFULLSCREEN_NOMENUBAR));
    REQUIRE(DrainTitleBarEventsUntil(
        [frameLifetime, removedOnEntry]()
        {
            wxFrame * const liveFrame =
                ResolveLiveTitleBarFrame(frameLifetime);
            return *removedOnEntry && liveFrame &&
                   !FindOnlyFrameChild(liveFrame);
        }));

    frame->SetMenuBar(CreateTitleBarMenuBar("&Remove on exit"));
    DrainTitleBarEvents();
    wxWindow * const lateWindow = FindOnlyFrameChild(frame);
    REQUIRE(lateWindow != nullptr);
    REQUIRE_FALSE(lateWindow->IsShown());
    const auto removedOnExit = std::make_shared<bool>(false);
    lateWindow->Bind(
        wxEVT_SHOW,
        [frameLifetime, removedOnExit](wxShowEvent& event)
        {
            if ( event.IsShown() && !*removedOnExit )
            {
                *removedOnExit = true;
                if ( wxFrame * const liveFrame =
                         ResolveLiveTitleBarFrame(frameLifetime) )
                {
                    liveFrame->SetMenuBar(nullptr);
                }
            }
            event.Skip();
        });

    REQUIRE(frame->ShowFullScreen(false));
    REQUIRE(DrainTitleBarEventsUntil(
        [frameLifetime, removedOnExit]()
        {
            wxFrame * const liveFrame =
                ResolveLiveTitleBarFrame(frameLifetime);
            return *removedOnExit && liveFrame &&
                   !FindOnlyFrameChild(liveFrame);
        }));
    CHECK(frame->GetClientAreaOrigin().y == 0);
}

TEST_CASE("WinUITitleBar::DestroyDuringFullScreenMenuHideRetiresState",
          "[winui-beta-aux][winui-beta-titlebar][titlebar][menu]"
          "[transaction][lifetime]")
{
    DrainTitleBarEvents();
    const unsigned policiesBefore =
        wxWinUITitleBarGetLivePolicyCountForTesting();
    const unsigned hostsBefore = wxWinUITopLevelHost::GetLiveHostCount();
    wxFrame * const frame = CreateTitleBarFrame(
        "destroy during fullscreen menu hide");
    TitleBarFrameGuard cleanup(frame);
    frame->SetMenuBar(CreateTitleBarMenuBar("&Destroy"));
    DrainTitleBarEvents();

    wxWindow * const menuBarWindow = FindOnlyFrameChild(frame);
    REQUIRE(menuBarWindow != nullptr);
    const wxWeakRef<wxWindow> frameLifetime(frame);
    const auto destroyRequested = std::make_shared<bool>(false);
    menuBarWindow->Bind(
        wxEVT_SHOW,
        [frameLifetime, destroyRequested](wxShowEvent& event)
        {
            if ( !event.IsShown() && !*destroyRequested )
            {
                *destroyRequested = true;
                if ( wxFrame * const liveFrame =
                         ResolveLiveTitleBarFrame(frameLifetime) )
                {
                    liveFrame->Destroy();
                }
            }
            event.Skip();
        });

    const wxWeakRef<wxWindow> lifetime(frame);
    CHECK_FALSE(frame->ShowFullScreen(true, wxFULLSCREEN_NOMENUBAR));
    CHECK(*destroyRequested);
    REQUIRE(DrainTitleBarEventsUntil(
        [lifetime, policiesBefore, hostsBefore]()
        {
            return !lifetime &&
                   wxWinUITitleBarGetLivePolicyCountForTesting() ==
                       policiesBefore &&
                   wxWinUITopLevelHost::GetLiveHostCount() == hostsBefore;
        }));
}

#endif // wxUSE_MENUS

TEST_CASE("WinUITitleBar::HundredHostCyclesRetireExactPolicies",
          "[winui-beta-aux][winui-beta-titlebar][titlebar][lifetime]")
{
    DrainTitleBarEvents();
    const unsigned policiesBefore =
        wxWinUITitleBarGetLivePolicyCountForTesting();
    const unsigned hostsBefore = wxWinUITopLevelHost::GetLiveHostCount();

    for ( int cycle = 0; cycle < 100; ++cycle )
    {
        wxFrame * const frame = CreateTitleBarFrame(
            wxString::Format("titlebar lifecycle %d", cycle));
        wxWinUITitleBarSnapshot snapshot;
        REQUIRE(wxWinUITitleBarSnapshotForTesting(frame, &snapshot));
        REQUIRE(snapshot.hwndIdentityCurrent);
        REQUIRE(snapshot.hwndGeneration != 0);
        if ( cycle % 25 == 0 &&
             snapshot.mode == wxWinUITitleBarMode::System &&
             snapshot.customizationSupported )
        {
            // Teardown itself must relinquish an active test extension while
            // the exact HWND/native/AppWindow identity is still current.
            REQUIRE(wxWinUITitleBarSetExtendedForTesting(frame, true));
        }
        const bool destroyExtended =
            cycle % 25 == 0 &&
            snapshot.mode == wxWinUITitleBarMode::System &&
            snapshot.customizationSupported;
        const unsigned teardownResetsBefore =
            wxWinUITitleBarGetTeardownResetCountForTesting();

        const wxWeakRef<wxWindow> lifetime(frame);
        frame->Destroy();
        REQUIRE(DrainTitleBarEventsUntil(
            [lifetime, policiesBefore, hostsBefore]()
            {
                return !lifetime &&
                       wxWinUITitleBarGetLivePolicyCountForTesting() ==
                           policiesBefore &&
                       wxWinUITopLevelHost::GetLiveHostCount() == hostsBefore;
            }));
        if ( destroyExtended )
        {
            CHECK(wxWinUITitleBarGetTeardownResetCountForTesting() ==
                  teardownResetsBefore + 1);
        }
    }

    CHECK(wxWinUITitleBarGetLivePolicyCountForTesting() == policiesBefore);
    CHECK(wxWinUITopLevelHost::GetLiveHostCount() == hostsBefore);
}

#endif // __WXWINUI__ && wxUSE_WINUI3
