///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuiauxwindows.cpp
// Purpose:     Portable and WinUI splash/tip auxiliary-window contract tests
// Author:      wxWidgets development team
// Created:     2026-08-02
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_TIPWINDOW

#include "wx/app.h"
#include "wx/frame.h"
#include "wx/tipwin.h"
#include "wx/utils.h"
#include "wx/weakref.h"

#include <memory>
#include <new>

namespace
{

class BacklinkReplacingTipWindow : public wxTipWindow
{
public:
    explicit BacklinkReplacingTipWindow(
        bool throwAfterReplacement,
        wxTipWindow** observableReplacementStorage = nullptr)
        : m_throwAfterReplacement(throwAfterReplacement),
          m_observableReplacementStorage(observableReplacementStorage)
    {
    }

    void Popup(wxWindow* focus = nullptr) override
    {
        wxUnusedVar(focus);

        // By default this storage intentionally expires before Create()
        // resumes. One section supplies persistent canary storage to make the
        // same required detach observable without memory instrumentation. The
        // rollback may assign m_windowPtr itself, but must never compare or
        // write through a replacement address afterwards.
        wxTipWindow* temporaryStorage = this;
        SetTipWindowPtr(m_observableReplacementStorage
                            ? m_observableReplacementStorage
                            : &temporaryStorage);

        if ( m_throwAfterReplacement )
#if wxUSE_EXCEPTIONS
            throw std::bad_alloc();
#else
            wxFAIL_MSG("throwing tip rollback seam requires exceptions");
#endif
    }

private:
    const bool m_throwAfterReplacement;
    wxTipWindow** const m_observableReplacementStorage;
};

template <typename Predicate>
bool DrainPortableAuxiliaryWindowsUntil(Predicate predicate,
                                        unsigned rounds = 160)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        if ( predicate() )
            return true;

        wxYield();
        wxMilliSleep(3);
    }

    return predicate();
}

} // anonymous namespace

TEST_CASE("AuxiliaryWindows::TipBacklinkRollback",
          "[aux-windows][tipwin]")
{
    wxFrame* const owner = new wxFrame(
        wxTheApp->GetTopWindow(), wxID_ANY, "portable tip rollback",
        wxPoint(-30000, -30000), wxSize(240, 140));
    const wxWeakRef<wxWindow> weakOwner(owner);
    owner->ShowWithoutActivating();

    SECTION("ordinary_failure")
    {
        std::unique_ptr<BacklinkReplacingTipWindow> tip(
            new BacklinkReplacingTipWindow(false));
        wxTipWindow* const rawTip = tip.get();
        wxTipWindow* historicalStorage = rawTip;

        CHECK_FALSE(tip->Create(
            owner, "temporary backlink failure", 120,
            &historicalStorage));
        CHECK(historicalStorage == rawTip);

        // Under memory instrumentation this reset is the decisive assertion:
        // ~wxTipWindow() must not write through the expired stack storage
        // installed by Popup().
        tip.reset();
    }

#if wxUSE_EXCEPTIONS
    SECTION("exception_unwind")
    {
        std::unique_ptr<BacklinkReplacingTipWindow> tip(
            new BacklinkReplacingTipWindow(true));
        wxTipWindow* const rawTip = tip.get();
        wxTipWindow* historicalStorage = rawTip;

        CHECK_THROWS_AS(
            tip->Create(owner, "temporary backlink throw", 120,
                        &historicalStorage),
            std::bad_alloc);
        CHECK(historicalStorage == rawTip);

        // The replacement storage has already left scope while unwinding the
        // overridden Popup(). Destruction must remain independent of it.
        tip.reset();
    }
#endif // wxUSE_EXCEPTIONS

    SECTION("replacement_storage_is_detached")
    {
        std::unique_ptr<wxTipWindow> sentinel(new wxTipWindow);
        wxTipWindow* replacementStorage = nullptr;
        std::unique_ptr<BacklinkReplacingTipWindow> tip(
            new BacklinkReplacingTipWindow(false, &replacementStorage));
        wxTipWindow* const rawTip = tip.get();
        replacementStorage = rawTip;
        wxTipWindow* historicalStorage = rawTip;

        CHECK_FALSE(tip->Create(
            owner, "observable replacement storage", 120,
            &historicalStorage));
        CHECK(historicalStorage == rawTip);

        replacementStorage = sentinel.get();
        tip.reset();
        CHECK(replacementStorage == sentinel.get());
    }

#if wxUSE_EXCEPTIONS
    SECTION("replacement_storage_is_detached_during_unwind")
    {
        std::unique_ptr<wxTipWindow> sentinel(new wxTipWindow);
        wxTipWindow* replacementStorage = nullptr;
        std::unique_ptr<BacklinkReplacingTipWindow> tip(
            new BacklinkReplacingTipWindow(true, &replacementStorage));
        wxTipWindow* const rawTip = tip.get();
        replacementStorage = rawTip;
        wxTipWindow* historicalStorage = rawTip;

        CHECK_THROWS_AS(
            tip->Create(owner, "observable replacement throw", 120,
                        &historicalStorage),
            std::bad_alloc);
        CHECK(historicalStorage == rawTip);

        replacementStorage = sentinel.get();
        tip.reset();
        CHECK(replacementStorage == sentinel.get());
    }
#endif // wxUSE_EXCEPTIONS

    owner->Destroy();
    REQUIRE(DrainPortableAuxiliaryWindowsUntil(
        [weakOwner]() { return !weakOwner; }));
}

TEST_CASE("AuxiliaryWindows::TransientDestroyIsDeferred",
          "[aux-windows][popupwin]")
{
    wxFrame* const owner = new wxFrame(
        wxTheApp->GetTopWindow(), wxID_ANY, "portable popup lifetime",
        wxPoint(-30000, -30000), wxSize(240, 140));
    const wxWeakRef<wxWindow> weakOwner(owner);
    owner->ShowWithoutActivating();

    wxPopupTransientWindow* const popup =
        new wxPopupTransientWindow(owner);
    const wxWeakRef<wxWindow> weakPopup(popup);
    bool destroyAccepted = false;
    const int eventId = wxID_HIGHEST + 730;
    popup->Bind(
        wxEVT_MENU,
        [popup, &destroyAccepted](wxCommandEvent&)
        {
            destroyAccepted = popup->Destroy();
        },
        eventId);

    wxCommandEvent event(wxEVT_MENU, eventId);
    event.SetEventObject(popup);
    REQUIRE(popup->ProcessWindowEvent(event));
    REQUIRE(destroyAccepted);

    // wxPopupTransientWindowBase promises delayed destruction specifically so
    // an ordinary handler can return without deleting its own event target.
    CHECK(weakPopup.get() == popup);
    REQUIRE(DrainPortableAuxiliaryWindowsUntil(
        [weakPopup]() { return !weakPopup; }));

    owner->Destroy();
    REQUIRE(DrainPortableAuxiliaryWindowsUntil(
        [weakOwner]() { return !weakOwner; }));
}

#endif // wxUSE_TIPWINDOW

#if defined(__WXWINUI__) && wxUSE_WINUI3 && \
        (wxUSE_SPLASH || wxUSE_TIPWINDOW)

#include "wx/app.h"
#include "wx/frame.h"
#include "wx/timer.h"
#include "wx/utils.h"
#include "wx/weakref.h"

#include "wx/msw/wrapwin.h"
#include <commctrl.h>
#if wxUSE_SPLASH
    #include "wx/splash.h"
#endif
#if wxUSE_TIPWINDOW
    #include "wx/tipwin.h"
#endif

#include "wx/winui/private/auxwindows.h"
#include "wx/winui/private/dialogsession.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"
#include "wx/winui/private/transient.h"
#include "wx/scopeguard.h"

#include <memory>
#include <new>

namespace
{

void DrainAuxiliaryWindows(unsigned rounds = 12)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        wxYield();
        wxMilliSleep(3);
    }
}

template <typename Predicate>
bool DrainAuxiliaryWindowsUntil(Predicate predicate,
                                 unsigned rounds = 160)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        if ( predicate() )
            return true;
        DrainAuxiliaryWindows(1);
    }

    return predicate();
}

template <typename Predicate>
bool DrainAuxiliaryWindowsWithoutSleepingUntil(Predicate predicate,
                                                unsigned rounds = 32)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        if ( predicate() )
            return true;

        // These proofs exercise CallAfter/pending-delete transactions only:
        // no timer or wall-clock edge is involved or permitted.
        wxTheApp->ProcessPendingEvents();
        wxTheApp->ProcessIdle();
    }

    return predicate();
}

unsigned PendingDeleteOccurrences(const wxObject* const object)
{
    unsigned count = 0;
    for ( wxList::compatibility_iterator node = wxPendingDelete.GetFirst();
          node;
          node = node->GetNext() )
    {
        if ( node->GetData() == object )
            ++count;
    }
    return count;
}

wxFrame* CreateAuxiliaryOwner(const wxString& title, int ordinal)
{
    wxFrame* const owner = new wxFrame(
        wxTheApp->GetTopWindow(), wxID_ANY, title,
        wxPoint(-30000 + ordinal * 300, -30000), wxSize(240, 140));
    owner->ShowWithoutActivating();
    return owner;
}

bool SameTransientRegistry(const wxWinUITransientRegistrySnapshot& lhs,
                           const wxWinUITransientRegistrySnapshot& rhs)
{
    return lhs.ownerCount == rhs.ownerCount &&
           lhs.transientCount == rhs.transientCount &&
           lhs.cancelCallbackCount == rhs.cancelCallbackCount &&
           lhs.modalCount == rhs.modalCount &&
           lhs.popupCount == rhs.popupCount &&
           lhs.teachingTipCount == rhs.teachingTipCount &&
           lhs.pendingOwnerRetireCount == rhs.pendingOwnerRetireCount &&
           lhs.activeOwnerRetirementCount ==
               rhs.activeOwnerRetirementCount;
}

struct AuxiliaryHostSnapshot
{
    unsigned hosts = wxWinUITopLevelHost::GetLiveHostCount();
    unsigned hostLifetimes =
        wxWinUITopLevelHost::GetLiveHostLifetimeCount();
    unsigned slots = wxWinUITopLevelHost::GetLiveSlotCount();
    unsigned slotLifetimes =
        wxWinUITopLevelHost::GetLiveSlotLifetimeCount();
};

bool SameAuxiliaryHostSnapshot(const AuxiliaryHostSnapshot& before)
{
    return wxWinUITopLevelHost::GetLiveHostCount() == before.hosts &&
           wxWinUITopLevelHost::GetLiveHostLifetimeCount() ==
               before.hostLifetimes &&
           wxWinUITopLevelHost::GetLiveSlotCount() == before.slots &&
           wxWinUITopLevelHost::GetLiveSlotLifetimeCount() ==
               before.slotLifetimes;
}

#if wxUSE_SPLASH

wxWeakRef<wxWindow> gs_scheduledSplashLifetime;
wxWeakRef<wxWindow> gs_splashReparentDestination;
wxWeakRef<wxWindow> gs_reparentedSplashLifetime;
bool gs_splashReparented = false;

#if wxUSE_EXCEPTIONS
void ThrowAfterSplashFilter(
    wxSplashScreen*,
    const wxWinUISplashHookPointForTesting point)
{
    if ( point ==
            wxWinUISplashHookPointForTesting::AfterFilterRegistration )
        throw std::bad_alloc();
}
#endif // wxUSE_EXCEPTIONS

void DestroySplashAfterUpdate(
    wxSplashScreen* const splash,
    const wxWinUISplashHookPointForTesting point)
{
    if ( point != wxWinUISplashHookPointForTesting::AfterUpdate )
        return;

    gs_scheduledSplashLifetime = splash;
    splash->Destroy();
}

void DestroySplashAfterFilter(
    wxSplashScreen* const splash,
    const wxWinUISplashHookPointForTesting point)
{
    if ( point !=
            wxWinUISplashHookPointForTesting::AfterFilterRegistration )
    {
        return;
    }

    gs_scheduledSplashLifetime = splash;
    splash->Destroy();
}

void ReparentSplashAfterShow(
    wxSplashScreen* const splash,
    const wxWinUISplashHookPointForTesting point)
{
    if ( point != wxWinUISplashHookPointForTesting::AfterShow )
        return;

    wxWindow* const destination = gs_splashReparentDestination.get();
    if ( !destination )
        return;

    gs_reparentedSplashLifetime = splash;
    gs_splashReparented = splash->Reparent(destination);
}

#endif // wxUSE_SPLASH

#if wxUSE_TIPWINDOW
wxWeakRef<wxWindow> gs_scheduledTipLifetime;
wxWeakRef<wxWindow> gs_hostileTipCaptureView;
wxWeakRef<wxWindow> gs_reentrantCloseTipLifetime;
wxWeakRef<wxWindow> gs_tipViewReparentDestination;
wxWeakRef<wxWindow> gs_reparentedTipView;
wxWinUITipHookPointForTesting gs_tipViewReparentPoint =
    wxWinUITipHookPointForTesting::AfterViewAdjust;
unsigned gs_hostileTipCapturePasses = 0;
unsigned gs_reentrantCloseAttempts = 0;
bool gs_tipViewReparented = false;

void ScheduleCreatingTipForDestruction(wxTipWindow* tip)
{
    gs_scheduledTipLifetime = tip;
    tip->Destroy();
}

#if wxUSE_EXCEPTIONS
void ThrowDuringTipCreate(wxTipWindow*)
{
    throw std::bad_alloc();
}

void DeleteCreatingTipAndThrow(wxTipWindow* tip)
{
    delete tip;
    throw std::bad_alloc();
}

void ScheduleCreatingTipForDestructionAndThrow(wxTipWindow* tip)
{
    gs_scheduledTipLifetime = tip;
    tip->Destroy();
    throw std::bad_alloc();
}
#endif // wxUSE_EXCEPTIONS

void DetachTipBacklinkAfterPublication(
    wxTipWindow* const tip,
    const wxWinUITipHookPointForTesting point)
{
    if ( point ==
            wxWinUITipHookPointForTesting::AfterPointerPublication )
    {
        tip->SetTipWindowPtr(nullptr);
    }
}

#if wxUSE_EXCEPTIONS
void DetachTipBacklinkAndThrowAfterPublication(
    wxTipWindow* const tip,
    const wxWinUITipHookPointForTesting point)
{
    if ( point !=
            wxWinUITipHookPointForTesting::AfterPointerPublication )
    {
        return;
    }

    tip->SetTipWindowPtr(nullptr);
    throw std::bad_alloc();
}
#endif // wxUSE_EXCEPTIONS

void ReplaceTipBacklinkWithExpiredStorageAfterPublication(
    wxTipWindow* const tip,
    const wxWinUITipHookPointForTesting point)
{
    if ( point !=
            wxWinUITipHookPointForTesting::AfterPointerPublication )
    {
        return;
    }

    // Intentionally non-null and intentionally dead as soon as this callback
    // returns. The production rollback is allowed to assign m_windowPtr only.
    wxTipWindow* temporaryStorage = tip;
    tip->SetTipWindowPtr(&temporaryStorage);
}

#if wxUSE_EXCEPTIONS
void ReplaceTipBacklinkWithExpiredStorageAndThrowAfterPublication(
    wxTipWindow* const tip,
    const wxWinUITipHookPointForTesting point)
{
    if ( point !=
            wxWinUITipHookPointForTesting::AfterPointerPublication )
    {
        return;
    }

    wxTipWindow* temporaryStorage = tip;
    tip->SetTipWindowPtr(&temporaryStorage);
    throw std::bad_alloc();
}
#endif // wxUSE_EXCEPTIONS

bool DispatchTipCloseFromOrdinaryEvent(wxTipWindow* const tip)
{
    const int eventId = wxID_HIGHEST + 731;
    tip->Bind(
        wxEVT_MENU,
        [tip](wxCommandEvent&)
        {
            tip->Close();
        },
        eventId);

    wxCommandEvent event(wxEVT_MENU, eventId);
    event.SetEventObject(tip);
    return tip->ProcessWindowEvent(event);
}

struct PersistentTipNativeReshowState
{
    bool enabled = true;
    bool inReshow = false;
    unsigned reshowCount = 0;
};

LRESULT CALLBACK PersistentTipNativeReshowSubclass(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam,
    UINT_PTR subclassId,
    DWORD_PTR referenceData)
{
    PersistentTipNativeReshowState* const state =
        reinterpret_cast<PersistentTipNativeReshowState*>(referenceData);
    if ( message == WM_NCDESTROY )
    {
        if ( state )
            state->enabled = false;
        (void)::RemoveWindowSubclass(
            hwnd, PersistentTipNativeReshowSubclass, subclassId);
        return ::DefSubclassProc(hwnd, message, wParam, lParam);
    }

    const LRESULT result =
        ::DefSubclassProc(hwnd, message, wParam, lParam);
    if ( message == WM_WINDOWPOSCHANGED && state && state->enabled &&
         !state->inReshow && ::IsWindow(hwnd) && !::IsWindowVisible(hwnd) )
    {
        state->inReshow = true;
        ++state->reshowCount;
        (void)::ShowWindow(hwnd, SW_SHOWNA);
        state->inReshow = false;
    }

    return result;
}

bool InstallPersistentTipNativeReshow(
    HWND hwnd,
    PersistentTipNativeReshowState* state)
{
    return hwnd && state &&
           ::SetWindowSubclass(
               hwnd,
               PersistentTipNativeReshowSubclass,
               reinterpret_cast<UINT_PTR>(state),
               reinterpret_cast<DWORD_PTR>(state));
}

void ReparentTipViewAtBoundary(
    wxTipWindow* const tip,
    const wxWinUITipHookPointForTesting point)
{
    if ( point != gs_tipViewReparentPoint )
        return;

    wxWindow* const destination = gs_tipViewReparentDestination.get();
    wxWindowList::compatibility_iterator childNode =
        tip->GetChildren().GetFirst();
    wxWindow* const view = childNode ? childNode->GetData() : nullptr;
    if ( !destination || !view )
        return;

    gs_reparentedTipView = view;
    gs_tipViewReparented = view->Reparent(destination);
}

void RecaptureTipViewNatively(
    wxPopupTransientWindow* const popup,
    unsigned int)
{
    ++gs_hostileTipCapturePasses;
    wxWindow* const view = gs_hostileTipCaptureView.get();
    if ( !view || !popup->IsDescendant(view) )
        return;

    const HWND viewHwnd = static_cast<HWND>(view->GetHWND());
    if ( viewHwnd && ::IsWindow(viewHwnd) )
        (void)::SetCapture(viewHwnd);
}

void ReenterSameTipCloseDuringCaptureRelease(
    wxPopupTransientWindow* const popup,
    const unsigned int pass)
{
    wxWindow* const expected = gs_reentrantCloseTipLifetime.get();
    if ( pass != 0 || expected != popup )
        return;

    ++gs_reentrantCloseAttempts;
    static_cast<wxTipWindow*>(popup)->Close();
}

// The public transient base is directly subclassable and intentionally has no
// wx class-info distinct from wxPopupWindow. Terminal fallback must therefore
// preserve its delayed-destruction contract without relying on a cast to the
// concrete MSW wxPopupTransientWindow class.
class DirectTransientTerminalPopup final
    : public wxPopupTransientWindowBase
{
public:
    DirectTransientTerminalPopup(wxWindow* const parent,
                                 const std::shared_ptr<unsigned>& destroyCalls)
        : m_destroyCalls(destroyCalls),
          m_created(Create(parent, wxPU_CONTAINS_CONTROLS))
    {
    }

    bool IsCreatedForTest() const { return m_created; }

    void Popup(wxWindow* focus = nullptr) override
    {
        (void)Show();
        if ( focus && IsDescendant(focus) )
            focus->SetFocus();
    }

    void Dismiss() override
    {
        (void)Hide();
    }

    bool Destroy() override
    {
        ++*m_destroyCalls;
        return wxPopupTransientWindowBase::Destroy();
    }

private:
    const std::shared_ptr<unsigned> m_destroyCalls;
    const bool m_created;
};
#endif

} // anonymous namespace

#if wxUSE_SPLASH

TEST_CASE("WinUIAuxiliaryWindows::SplashLifecycle",
          "[winui-beta-aux][winui-beta-aux-windows][splash]")
{
    const AuxiliaryHostSnapshot hostsBefore;
    const std::size_t runtimeStatesBefore =
        wxWinUIGetSplashRuntimeStateCountForTesting();
    wxFrame* const owner = CreateAuxiliaryOwner("splash owner", 0);
    const wxWeakRef<wxWindow> weakOwner(owner);
    wxBitmap bitmap(80, 40, 32);
    REQUIRE(bitmap.IsOk());

    SECTION("input_filter_and_timer_close")
    {
        wxSplashScreen* const inputSplash = new wxSplashScreen(
            bitmap, wxSPLASH_NO_CENTRE | wxSPLASH_NO_TIMEOUT, 0,
            owner, wxID_ANY, wxPoint(-30000, -30000));
        const wxWeakRef<wxWindow> weakInputSplash(inputSplash);
        REQUIRE(inputSplash->IsShown());
        REQUIRE(inputSplash->GetSplashWindow());
        CHECK(inputSplash->GetClientSize() == bitmap.GetLogicalSize());

        // Dispatch through an unrelated handler so this proves that the
        // splash registered itself in the global filter chain.
        wxEvtHandler sink;
        wxKeyEvent input(wxEVT_KEY_DOWN);
        sink.ProcessEvent(input);
        REQUIRE(DrainAuxiliaryWindowsUntil(
            [weakInputSplash]() { return !weakInputSplash; }));

        wxSplashScreen* const timerSplash = new wxSplashScreen(
            bitmap, wxSPLASH_NO_CENTRE | wxSPLASH_TIMEOUT, 15,
            owner, wxID_ANY, wxPoint(-30000, -30000));
        const wxWeakRef<wxWindow> weakTimerSplash(timerSplash);
        REQUIRE(timerSplash->IsShown());
        CHECK(timerSplash->GetTimeout() == 15);

        // Let the owned one-shot timer deliver normally; directly calling
        // OnNotify() would not prove that Start() and owner teardown work.
        REQUIRE(DrainAuxiliaryWindowsUntil(
            [weakTimerSplash]() { return !weakTimerSplash; }));
    }

    SECTION("ownerless_scaled_bitmap_and_bounded_timeout")
    {
        wxBitmap scaledBitmap(160, 80, 32);
        REQUIRE(scaledBitmap.IsOk());
        scaledBitmap.SetScaleFactor(2);

        wxSplashScreen* const splash = new wxSplashScreen(
            scaledBitmap, wxSPLASH_NO_CENTRE | wxSPLASH_TIMEOUT, 0,
            nullptr, wxID_ANY, wxPoint(-30000, -30000));
        const wxWeakRef<wxWindow> weakSplash(splash);
        REQUIRE(splash->IsShown());
        CHECK(splash->GetTimeout() == 0);
        CHECK(splash->GetClientSize() == scaledBitmap.GetLogicalSize());

        wxCloseEvent close(wxEVT_CLOSE_WINDOW, splash->GetId());
        splash->OnCloseWindow(close);
        REQUIRE(DrainAuxiliaryWindowsUntil(
            [weakSplash]() { return !weakSplash; }));
    }

    SECTION("retained_owner_shutdown")
    {
        wxSplashScreen* const splash = new wxSplashScreen(
            bitmap, wxSPLASH_NO_CENTRE | wxSPLASH_NO_TIMEOUT, 0,
            owner, wxID_ANY, wxPoint(-30000, -30000));
        const wxWeakRef<wxWindow> weakSplash(splash);
        REQUIRE(splash->IsShown());

        std::unique_ptr<wxWinUITLWHostWindowEventGuard> retainedOwner(
            new wxWinUITLWHostWindowEventGuard(owner));
        owner->Destroy();
        REQUIRE(wxWinUITLWHostIsDestroyScheduled(owner));
        retainedOwner.reset();

        REQUIRE(DrainAuxiliaryWindowsUntil(
            [weakOwner, weakSplash]()
            {
                return !weakOwner && !weakSplash;
            }));
    }

    SECTION("one_hundred_cycles")
    {
        for ( int i = 0; i < 100; ++i )
        {
            wxSplashScreen* const splash = new wxSplashScreen(
                bitmap, wxSPLASH_NO_CENTRE | wxSPLASH_NO_TIMEOUT, 0,
                owner, wxID_ANY, wxPoint(-30000, -30000));
            const wxWeakRef<wxWindow> weakSplash(splash);
            REQUIRE(splash->IsShown());
            wxCloseEvent close(wxEVT_CLOSE_WINDOW, splash->GetId());
            splash->OnCloseWindow(close);
            INFO("splash cycle: " << i);
            REQUIRE(DrainAuxiliaryWindowsUntil(
                [weakSplash]() { return !weakSplash; }));
        }
    }

    SECTION("construction_rollback_and_update_revalidation")
    {
#if wxUSE_EXCEPTIONS
        wxWinUISetSplashHookForTesting(&ThrowAfterSplashFilter);
        wxScopeGuard resetHook = wxMakeGuard(
            []() { wxWinUIResetSplashHookForTesting(); });
        wxUnusedVar(resetHook);

        CHECK_THROWS_AS(
            (void)new wxSplashScreen(
                bitmap, wxSPLASH_NO_CENTRE | wxSPLASH_NO_TIMEOUT, 0,
                owner, wxID_ANY, wxPoint(-30000, -30000)),
            std::bad_alloc);
        wxWinUIResetSplashHookForTesting();
        CHECK(wxWinUIGetSplashRuntimeStateCountForTesting() ==
              runtimeStatesBefore);

        // Processing an unrelated event after constructor unwinding also
        // proves that no intrusive filter node still points at the dead
        // partially constructed object.
        wxEvtHandler sink;
        wxKeyEvent input(wxEVT_KEY_DOWN);
        sink.ProcessEvent(input);
#endif // wxUSE_EXCEPTIONS

        gs_scheduledSplashLifetime.Release();
        wxWinUISetSplashHookForTesting(&DestroySplashAfterFilter);
        (void)new wxSplashScreen(
            bitmap, wxSPLASH_NO_CENTRE | wxSPLASH_NO_TIMEOUT, 0,
            owner, wxID_ANY, wxPoint(-30000, -30000));
        wxWinUIResetSplashHookForTesting();
        REQUIRE(gs_scheduledSplashLifetime);
        REQUIRE(DrainAuxiliaryWindowsUntil(
            []() { return !gs_scheduledSplashLifetime; }));
        CHECK(wxWinUIGetSplashRuntimeStateCountForTesting() ==
              runtimeStatesBefore);

#if wxUSE_EXCEPTIONS
        wxWinUIFailNextSplashRuntimeAllocationForTesting();
        CHECK_THROWS_AS(
            (void)new wxSplashScreen(
                bitmap, wxSPLASH_NO_CENTRE | wxSPLASH_NO_TIMEOUT, 0,
                owner, wxID_ANY, wxPoint(-30000, -30000)),
            std::bad_alloc);
        CHECK(wxWinUIGetSplashRuntimeStateCountForTesting() ==
              runtimeStatesBefore);
#endif // wxUSE_EXCEPTIONS

        gs_scheduledSplashLifetime.Release();
        wxWinUISetSplashHookForTesting(&DestroySplashAfterUpdate);
        (void)new wxSplashScreen(
            bitmap, wxSPLASH_NO_CENTRE | wxSPLASH_NO_TIMEOUT, 0,
            owner, wxID_ANY, wxPoint(-30000, -30000));
        wxWinUIResetSplashHookForTesting();
        REQUIRE(gs_scheduledSplashLifetime);
        REQUIRE(DrainAuxiliaryWindowsUntil(
            []() { return !gs_scheduledSplashLifetime; }));
        CHECK(wxWinUIGetSplashRuntimeStateCountForTesting() ==
              runtimeStatesBefore);

        wxFrame* const reparentDestination =
            CreateAuxiliaryOwner("splash reparent destination", 3);
        const wxWeakRef<wxWindow> weakReparentDestination(
            reparentDestination);
        gs_splashReparentDestination = reparentDestination;
        gs_reparentedSplashLifetime.Release();
        gs_splashReparented = false;
        wxWinUISetSplashHookForTesting(&ReparentSplashAfterShow);

        wxSplashScreen* const reparentedSplash = new wxSplashScreen(
            bitmap, wxSPLASH_NO_CENTRE | wxSPLASH_NO_TIMEOUT, 0,
            nullptr, wxID_ANY, wxPoint(-30000, -30000));
        wxWinUIResetSplashHookForTesting();
        gs_splashReparentDestination.Release();

        REQUIRE(gs_splashReparented);
        REQUIRE(gs_reparentedSplashLifetime.get() == reparentedSplash);
        CHECK(reparentedSplash->GetParent() == reparentDestination);
        CHECK_FALSE(reparentedSplash->IsShown());
        CHECK(reparentedSplash->GetSplashWindow() == nullptr);
        CHECK(wxWinUIGetSplashRuntimeStateCountForTesting() ==
              runtimeStatesBefore);

        // The original owner is deliberately null: this catches an owner check
        // which only validates a non-null weak parent. This test runs after the
        // WinUI test application initialized XAML. It proves hostile
        // reparenting at a real callback boundary, but does not claim to cover
        // pre-initialization or post-teardown XAML access; those states require
        // an isolated process-level probe.
        reparentedSplash->Destroy();
        reparentDestination->Destroy();
        REQUIRE(DrainAuxiliaryWindowsUntil(
            [weakReparentDestination]()
            {
                return !gs_reparentedSplashLifetime &&
                       !weakReparentDestination;
            }));
    }

    if ( weakOwner )
        owner->Destroy();
    REQUIRE(DrainAuxiliaryWindowsUntil(
        [weakOwner, &hostsBefore]()
        {
            return !weakOwner && SameAuxiliaryHostSnapshot(hostsBefore);
        }));
    CHECK(wxWinUIGetSplashRuntimeStateCountForTesting() ==
          runtimeStatesBefore);
}

#endif // wxUSE_SPLASH

#if wxUSE_TIPWINDOW

TEST_CASE("WinUIAuxiliaryWindows::PopupTerminalFallbackContracts",
          "[winui-beta-aux][winui-beta-aux-windows][popupwin]"
          "[pending-delete]")
{
    wxFrame* const owner =
        CreateAuxiliaryOwner("direct transient terminal owner", 4);
    const wxWeakRef<wxWindow> weakOwner(owner);
    SECTION("direct_transient_outside_operation_enters_pending_delete")
    {
        const auto virtualDestroyCalls = std::make_shared<unsigned>(0);
        DirectTransientTerminalPopup* const popup =
            new DirectTransientTerminalPopup(owner, virtualDestroyCalls);
        REQUIRE(popup->IsCreatedForTest());
        popup->Popup();
        REQUIRE(popup->IsShown());
        REQUIRE(wxWinUITransientCountForTesting(
                    owner, wxWinUITransientKind::Popup) == 1);

        const wxWeakRef<wxWindow> weakPopup(popup);
        wxWinUIFailNextPopupTerminalNativeIdentityForTesting();
        REQUIRE(wxWinUIRetirePopupTerminal(popup));

        // This call is made directly by the test, outside any window event or
        // WinUI operation guard. Membership proves it selected the ordinary
        // delayed popup contract rather than the private host queue.
        CHECK(*virtualDestroyCalls == 0);
        CHECK(weakPopup.get() == popup);
        CHECK(wxPendingDelete.Member(popup));
        CHECK(PendingDeleteOccurrences(popup) == 1);
        CHECK(wxWinUITLWHostIsDestroyScheduled(popup));

        REQUIRE(DrainAuxiliaryWindowsWithoutSleepingUntil(
            [weakPopup]() { return !weakPopup; }));
        CHECK(*virtualDestroyCalls == 0);
        CHECK(wxWinUITransientCountForTesting(owner) == 0);
    }

    SECTION("plain_popup_is_delayed_once")
    {
        wxPopupWindow* const popup =
            new wxPopupWindow(owner, wxPU_CONTAINS_CONTROLS);
        if ( !popup->IsShown() )
            REQUIRE(popup->Show());
        REQUIRE(popup->IsShown());
        const wxWeakRef<wxWindow> weakPopup(popup);

        wxWinUIFailNextPopupTerminalNativeIdentityForTesting();
        REQUIRE(wxWinUIRetirePopupTerminal(popup));
        REQUIRE(weakPopup.get() == popup);
        CHECK(wxPendingDelete.Member(popup));
        CHECK(PendingDeleteOccurrences(popup) == 1);

        // The first pending-delete entry is authoritative. A second public
        // terminal request is rejected and cannot append the same object.
        CHECK_FALSE(wxWinUIRetirePopupTerminal(popup));
        CHECK(PendingDeleteOccurrences(popup) == 1);

        REQUIRE(DrainAuxiliaryWindowsWithoutSleepingUntil(
            [weakPopup]() { return !weakPopup; }));
        CHECK(wxWinUITransientCountForTesting(owner) == 0);
    }

    SECTION("destroy_scheduled_parent_replays_child_immediately")
    {
        const auto virtualDestroyCalls = std::make_shared<unsigned>(0);
        DirectTransientTerminalPopup* const popup =
            new DirectTransientTerminalPopup(owner, virtualDestroyCalls);
        REQUIRE(popup->IsCreatedForTest());
        popup->Popup();
        REQUIRE(popup->IsShown());
        const wxWeakRef<wxWindow> weakPopup(popup);

        REQUIRE(owner->Destroy());
        REQUIRE(wxWinUITLWHostIsDestroyScheduled(owner));
        REQUIRE(weakPopup.get() == popup);

        wxWinUIFailNextPopupTerminalNativeIdentityForTesting();
        REQUIRE(wxWinUIRetirePopupTerminal(popup));
        CHECK_FALSE(weakPopup);
        CHECK(*virtualDestroyCalls == 0);

        // A parent already scheduled for deletion makes delaying its child
        // unsafe. Terminal fallback must immediately use the qualified base
        // completion, without re-entering the application's Destroy() override.
        REQUIRE(DrainAuxiliaryWindowsWithoutSleepingUntil(
            [weakOwner]() { return !weakOwner; }));
        CHECK(*virtualDestroyCalls == 0);
    }

    if ( weakOwner )
        owner->Destroy();
    REQUIRE(DrainAuxiliaryWindowsWithoutSleepingUntil(
        [weakOwner]() { return !weakOwner; }));
}

TEST_CASE("WinUIAuxiliaryWindows::TipWindowLifecycle",
          "[winui-beta-aux][winui-beta-aux-windows][tipwin]")
{
    const AuxiliaryHostSnapshot hostsBefore;
    const std::size_t runtimeStatesBefore =
        wxWinUIGetTipRuntimeStateCountForTesting();
    const wxWinUITransientRegistrySnapshot registryBefore =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    wxFrame* const owner = CreateAuxiliaryOwner("tip owner", 1);
    const wxWeakRef<wxWindow> weakOwner(owner);

    SECTION("long_text_bounding_rect_and_outside_dismiss")
    {
        wxRect bounds(-32000, -32000, 10, 10);
        wxTipWindow::Ref tip = wxTipWindow::New(
            owner,
            "A deliberately long WinUI tip whose lines must wrap without "
            "creating another XAML island or retaining its owner.",
            120, &bounds);
        REQUIRE(tip);
        wxTipWindow* rawTip = tip.operator->();
        const wxWeakRef<wxWindow> weakTip(rawTip);
        REQUIRE(rawTip->IsShown());
        CHECK(rawTip->GetClientSize().x > 0);
        CHECK(rawTip->GetClientSize().y > 0);
        CHECK(wxWinUITransientCountForTesting(
                  owner, wxWinUITransientKind::Popup) == 1);

        wxWindowList::compatibility_iterator childNode =
            rawTip->GetChildren().GetFirst();
        REQUIRE(childNode);
        wxWindow* const view = childNode->GetData();
        REQUIRE(view);
        wxMouseEvent motion(wxEVT_MOTION);
        motion.SetId(view->GetId());
        motion.SetEventObject(view);
        motion.SetPosition(wxPoint(0, 0));
        view->ProcessWindowEvent(motion);
        CHECK_FALSE(tip);
        REQUIRE(DrainAuxiliaryWindowsUntil(
            [weakTip]() { return !weakTip; }));
        CHECK(wxWinUITransientCountForTesting(owner) == 0);

        tip = wxTipWindow::New(owner, "outside-click seam", 120);
        REQUIRE(tip);
        rawTip = tip.operator->();
        const wxWeakRef<wxWindow> weakOutsideTip(rawTip);

        // This is the real popup outside-click seam: it executes CanDismiss,
        // Dismiss and wxTipWindow::OnDismiss without desktop input injection.
        wxWinUITestPopupOutsideClick(rawTip);
        CHECK_FALSE(tip);
        REQUIRE(DrainAuxiliaryWindowsUntil(
            [weakOutsideTip]() { return !weakOutsideTip; }));
        CHECK(wxWinUITransientCountForTesting(owner) == 0);
    }

    SECTION("same_tip_close_reentrancy_is_bounded")
    {
        wxTipWindow::Ref tip =
            wxTipWindow::New(owner, "reentrant close", 120);
        REQUIRE(tip);
        wxTipWindow* const rawTip = tip.operator->();
        const wxWeakRef<wxWindow> weakTip(rawTip);
        wxWindowList::compatibility_iterator childNode =
            rawTip->GetChildren().GetFirst();
        REQUIRE(childNode);
        wxWindow* const view = childNode->GetData();
        REQUIRE(view);

        view->CaptureMouse();
        REQUIRE(wxWindow::GetCapture() == view);
        gs_reentrantCloseTipLifetime = rawTip;
        gs_reentrantCloseAttempts = 0;
        wxWinUISetPopupCaptureReleaseHookForTesting(
            &ReenterSameTipCloseDuringCaptureRelease);
        wxScopeGuard resetHook = wxMakeGuard(
            []()
            {
                wxWinUIResetPopupCaptureReleaseHookForTesting();
                gs_reentrantCloseTipLifetime.Release();
            });
        wxUnusedVar(resetHook);

        rawTip->Close();
        wxWinUIResetPopupCaptureReleaseHookForTesting();
        gs_reentrantCloseTipLifetime.Release();

        // The existing capture-release seam called Close() on the exact same
        // tip while its outer Close() transaction was active. The inner call
        // must be a no-op; only the outer call clears Ref and schedules one
        // delayed destruction.
        CHECK(gs_reentrantCloseAttempts == 1);
        CHECK_FALSE(tip);
        CHECK(wxWindow::GetCapture() == nullptr);
        CHECK(weakTip.get() == rawTip);
        CHECK(wxPendingDelete.Member(rawTip));
        CHECK(PendingDeleteOccurrences(rawTip) == 1);

        REQUIRE(DrainAuxiliaryWindowsWithoutSleepingUntil(
            [weakTip]() { return !weakTip; }));
        CHECK(wxWinUITransientCountForTesting(owner) == 0);
    }

    SECTION("capture_release_and_destructive_create")
    {
        wxTipWindow::Ref tip = wxTipWindow::New(owner, "captured tip", 120);
        REQUIRE(tip);
        wxTipWindow* const rawTip = tip.operator->();
        const wxWeakRef<wxWindow> weakTip(rawTip);
        wxWindowList::compatibility_iterator childNode =
            rawTip->GetChildren().GetFirst();
        REQUIRE(childNode);
        wxWindow* const view = childNode->GetData();
        REQUIRE(view);

        view->CaptureMouse();
        REQUIRE(wxWindow::GetCapture() == view);
        rawTip->Close();
        CHECK_FALSE(tip);
        CHECK(wxWindow::GetCapture() == nullptr);
        REQUIRE(DrainAuxiliaryWindowsUntil(
            [weakTip]() { return !weakTip; }));

        tip = wxTipWindow::New(owner, "hostile native recapture", 120);
        REQUIRE(tip);
        wxTipWindow* const hostileTip = tip.operator->();
        const wxWeakRef<wxWindow> weakHostileTip(hostileTip);
        childNode = hostileTip->GetChildren().GetFirst();
        REQUIRE(childNode);
        wxWindow* const hostileView = childNode->GetData();
        REQUIRE(hostileView);
        hostileView->CaptureMouse();
        REQUIRE(wxWindow::GetCapture() == hostileView);

        gs_hostileTipCaptureView = hostileView;
        gs_hostileTipCapturePasses = 0;
        wxWinUISetPopupCaptureReleaseHookForTesting(
            &RecaptureTipViewNatively);
        wxScopeGuard resetCaptureHook = wxMakeGuard(
            []()
            {
                wxWinUIResetPopupCaptureReleaseHookForTesting();
                gs_hostileTipCaptureView.Release();
            });
        wxUnusedVar(resetCaptureHook);

        // Enter through an ordinary application event, with no synthetic host
        // lifetime guard. Terminal retirement of a transient must select the
        // delayed wxPopupTransientWindowBase::Destroy() contract itself.
        REQUIRE(DispatchTipCloseFromOrdinaryEvent(hostileTip));
        CHECK(weakHostileTip.get() == hostileTip);
        CHECK(gs_hostileTipCapturePasses > 16);
        CHECK(gs_hostileTipCapturePasses <= 64);
        CHECK(wxWindow::GetCapture() == nullptr);
        REQUIRE(tip);
        CHECK(wxWinUITLWHostIsDestroyScheduled(hostileTip));

        wxWinUIResetPopupCaptureReleaseHookForTesting();
        gs_hostileTipCaptureView.Release();
        REQUIRE(DrainAuxiliaryWindowsUntil(
            [weakHostileTip]() { return !weakHostileTip; }));
        CHECK_FALSE(tip);
        CHECK(wxWinUITransientCountForTesting(owner) == 0);

        wxWinUISetTipCreateHookForTesting(
            [](wxTipWindow* creatingTip)
            {
                // Synchronous deletion is the harshest legal callback outcome:
                // New() must not let its provisional unique_ptr delete twice.
                delete creatingTip;
            });
        wxScopeGuard resetHook = wxMakeGuard(
            []() { wxWinUIResetTipCreateHookForTesting(); });
        wxUnusedVar(resetHook);

        tip = wxTipWindow::New(owner, "destroy during create", 120);
        CHECK_FALSE(tip);
        CHECK(wxWinUITransientCountForTesting(owner) == 0);

        wxWinUIResetTipCreateHookForTesting();
        gs_scheduledTipLifetime.Release();
        wxWinUISetTipCreateHookForTesting(
            &ScheduleCreatingTipForDestruction);
        tip = wxTipWindow::New(owner, "schedule destroy during create", 120);
        CHECK_FALSE(tip);
        REQUIRE(DrainAuxiliaryWindowsUntil(
            []() { return !gs_scheduledTipLifetime; }));
        CHECK(wxWinUITransientCountForTesting(owner) == 0);

#if wxUSE_EXCEPTIONS
        wxWinUISetTipCreateHookForTesting(&ThrowDuringTipCreate);
        CHECK_THROWS_AS(
            (void)wxTipWindow::New(owner, "throw during create", 120),
            std::bad_alloc);
        wxWinUIResetTipCreateHookForTesting();
        CHECK(wxWinUITransientCountForTesting(owner) == 0);
        CHECK(wxWinUIGetTipRuntimeStateCountForTesting() ==
              runtimeStatesBefore);

        wxWinUISetTipCreateHookForTesting(&DeleteCreatingTipAndThrow);
        CHECK_THROWS_AS(
            (void)wxTipWindow::New(owner, "delete and throw", 120),
            std::bad_alloc);
        wxWinUIResetTipCreateHookForTesting();
        CHECK(wxWinUITransientCountForTesting(owner) == 0);
        CHECK(wxWinUIGetTipRuntimeStateCountForTesting() ==
              runtimeStatesBefore);

        gs_scheduledTipLifetime.Release();
        wxWinUISetTipCreateHookForTesting(
            &ScheduleCreatingTipForDestructionAndThrow);
        CHECK_THROWS_AS(
            (void)wxTipWindow::New(owner, "destroy and throw", 120),
            std::bad_alloc);
        wxWinUIResetTipCreateHookForTesting();
        REQUIRE(DrainAuxiliaryWindowsUntil(
            []() { return !gs_scheduledTipLifetime; }));
        CHECK(wxWinUITransientCountForTesting(owner) == 0);
        CHECK(wxWinUIGetTipRuntimeStateCountForTesting() ==
              runtimeStatesBefore);
#endif // wxUSE_EXCEPTIONS
    }

    SECTION("pointer_publication_is_a_bidirectional_commit")
    {
        wxWinUISetTipHookForTesting(&DetachTipBacklinkAfterPublication);
        wxScopeGuard resetHook = wxMakeGuard(
            []() { wxWinUIResetTipHookForTesting(); });
        wxUnusedVar(resetHook);

        wxTipWindow::Ref tip =
            wxTipWindow::New(owner, "detached publication", 120);
        CHECK_FALSE(tip);
        wxWinUIResetTipHookForTesting();
        CHECK(wxWinUITransientCountForTesting(owner) == 0);
        CHECK(wxWinUIGetTipRuntimeStateCountForTesting() ==
              runtimeStatesBefore);

#if wxUSE_EXCEPTIONS
        wxWinUISetTipHookForTesting(
            &DetachTipBacklinkAndThrowAfterPublication);
        CHECK_THROWS_AS(
            (void)wxTipWindow::New(owner, "throw after publication", 120),
            std::bad_alloc);
        wxWinUIResetTipHookForTesting();
        CHECK(wxWinUITransientCountForTesting(owner) == 0);
        CHECK(wxWinUIGetTipRuntimeStateCountForTesting() ==
              runtimeStatesBefore);
#endif // wxUSE_EXCEPTIONS

        wxWinUISetTipHookForTesting(
            &ReplaceTipBacklinkWithExpiredStorageAfterPublication);
        tip = wxTipWindow::New(
            owner, "expired replacement publication", 120);
        CHECK_FALSE(tip);
        wxWinUIResetTipHookForTesting();
        CHECK(wxWinUITransientCountForTesting(owner) == 0);
        CHECK(wxWinUIGetTipRuntimeStateCountForTesting() ==
              runtimeStatesBefore);

#if wxUSE_EXCEPTIONS
        wxWinUISetTipHookForTesting(
            &ReplaceTipBacklinkWithExpiredStorageAndThrowAfterPublication);
        CHECK_THROWS_AS(
            (void)wxTipWindow::New(
                owner, "expired replacement throw", 120),
            std::bad_alloc);
        wxWinUIResetTipHookForTesting();
        CHECK(wxWinUITransientCountForTesting(owner) == 0);
        CHECK(wxWinUIGetTipRuntimeStateCountForTesting() ==
              runtimeStatesBefore);
#endif // wxUSE_EXCEPTIONS

        // The historical two-step convention owns arbitrary caller storage:
        // Create() must neither read its old value nor overwrite it. A non-this
        // sentinel makes the latter observable; leaving it uninitialized is
        // also valid because only New() marks storage as provisionally owned.
        std::unique_ptr<wxTipWindow> historical(new wxTipWindow);
        std::unique_ptr<wxTipWindow> sentinel(new wxTipWindow);
        wxTipWindow* historicalExternal = sentinel.get();
        REQUIRE(historical->Create(
            owner, "historical pointer convention", 120,
            &historicalExternal));
        CHECK(historicalExternal == sentinel.get());
        historicalExternal = historical.release();
        const wxWeakRef<wxWindow> weakHistorical(historicalExternal);
        historicalExternal->Close();
        CHECK(historicalExternal == nullptr);
        REQUIRE(DrainAuxiliaryWindowsUntil(
            [weakHistorical]() { return !weakHistorical; }));
        sentinel.reset();
        CHECK(wxWinUITransientCountForTesting(owner) == 0);
        CHECK(wxWinUIGetTipRuntimeStateCountForTesting() ==
              runtimeStatesBefore);
    }

    SECTION("view_parent_is_revalidated_after_callback_boundaries")
    {
        const auto exerciseReparentBoundary =
            [&](const wxWinUITipHookPointForTesting point,
                const char* const label)
            {
                INFO("tip view boundary: " << label);
                gs_tipViewReparentPoint = point;
                gs_tipViewReparentDestination = owner;
                gs_reparentedTipView.Release();
                gs_tipViewReparented = false;
                wxWinUISetTipHookForTesting(&ReparentTipViewAtBoundary);

                wxTipWindow::Ref tip =
                    wxTipWindow::New(owner, "reparented view", 120);
                wxWinUIResetTipHookForTesting();
                CHECK_FALSE(tip);
                REQUIRE(gs_tipViewReparented);
                wxWindow* const foreignView =
                    gs_reparentedTipView.get();
                REQUIRE(foreignView);
                CHECK(foreignView->GetParent() == owner);

                if ( tip )
                    tip->Close();
                foreignView->Destroy();
                gs_tipViewReparentDestination.Release();
                REQUIRE(DrainAuxiliaryWindowsUntil(
                    []() { return !gs_reparentedTipView; }));
                CHECK(wxWinUITransientCountForTesting(owner) == 0);
                CHECK(wxWinUIGetTipRuntimeStateCountForTesting() ==
                      runtimeStatesBefore);
            };

        exerciseReparentBoundary(
            wxWinUITipHookPointForTesting::AfterViewAdjust,
            "after Adjust");
        exerciseReparentBoundary(
            wxWinUITipHookPointForTesting::AfterPopup,
            "after Popup");
    }

    SECTION("allocation_failure_and_native_reshow_are_terminal")
    {
#if wxUSE_EXCEPTIONS
        wxWinUIFailNextTipRuntimeAllocationForTesting();
        CHECK_THROWS_AS(
            (void)wxTipWindow::New(owner, "runtime allocation", 120),
            std::bad_alloc);
        CHECK(wxWinUIGetTipRuntimeStateCountForTesting() ==
              runtimeStatesBefore);
        CHECK(wxWinUITransientCountForTesting(owner) == 0);
#endif // wxUSE_EXCEPTIONS

        wxTipWindow::Ref tip =
            wxTipWindow::New(owner, "persistent native reshow", 120);
        REQUIRE(tip);
        wxTipWindow* rawTip = tip.operator->();
        const wxWeakRef<wxWindow> weakReshowTip(rawTip);
        const HWND hwnd = static_cast<HWND>(rawTip->GetHWND());
        REQUIRE(hwnd != nullptr);
        PersistentTipNativeReshowState persistentReshow;
        REQUIRE(InstallPersistentTipNativeReshow(hwnd, &persistentReshow));
        wxScopeGuard removePersistentReshowSubclass = wxMakeGuard(
            [hwnd, &persistentReshow]()
            {
                persistentReshow.enabled = false;
                if ( ::IsWindow(hwnd) )
                {
                    (void)::RemoveWindowSubclass(
                        hwnd,
                        PersistentTipNativeReshowSubclass,
                        reinterpret_cast<UINT_PTR>(&persistentReshow));
                }
            });
        wxUnusedVar(removePersistentReshowSubclass);

        // No retained operation guard: an ordinary event must return while the
        // transient is still alive and pending-delete owns final destruction.
        REQUIRE(DispatchTipCloseFromOrdinaryEvent(rawTip));
        CHECK(weakReshowTip.get() == rawTip);
        REQUIRE(tip);
        CHECK(persistentReshow.reshowCount != 0);
        CHECK(wxWinUITLWHostIsDestroyScheduled(rawTip));

        REQUIRE(DrainAuxiliaryWindowsUntil(
            [weakReshowTip]() { return !weakReshowTip; }));
        CHECK_FALSE(tip);
        CHECK(wxWinUITransientCountForTesting(owner) == 0);

        tip = wxTipWindow::New(owner, "missing native generation", 120);
        REQUIRE(tip);
        rawTip = tip.operator->();
        const wxWeakRef<wxWindow> weakMissingNativeTip(rawTip);
        const HWND missingNativeHwnd =
            static_cast<HWND>(rawTip->GetHWND());
        REQUIRE(missingNativeHwnd != nullptr);
        PersistentTipNativeReshowState missingNativeReshow;
        REQUIRE(InstallPersistentTipNativeReshow(
            missingNativeHwnd, &missingNativeReshow));
        wxScopeGuard removeMissingNativeReshowSubclass = wxMakeGuard(
            [missingNativeHwnd, &missingNativeReshow]()
            {
                missingNativeReshow.enabled = false;
                if ( ::IsWindow(missingNativeHwnd) )
                {
                    (void)::RemoveWindowSubclass(
                        missingNativeHwnd,
                        PersistentTipNativeReshowSubclass,
                        reinterpret_cast<UINT_PTR>(&missingNativeReshow));
                }
            });
        wxUnusedVar(removeMissingNativeReshowSubclass);

        wxWinUIFailNextPopupTerminalNativeIdentityForTesting();
        REQUIRE(DispatchTipCloseFromOrdinaryEvent(rawTip));
        CHECK(weakMissingNativeTip.get() == rawTip);
        REQUIRE(tip);
        CHECK(missingNativeReshow.reshowCount != 0);
        CHECK(wxWinUITLWHostIsDestroyScheduled(rawTip));

        REQUIRE(DrainAuxiliaryWindowsUntil(
            [weakMissingNativeTip]() { return !weakMissingNativeTip; }));
        CHECK_FALSE(tip);
        CHECK(wxWinUITransientCountForTesting(owner) == 0);
    }

    SECTION("teaching_tip_concurrency_and_retained_owner")
    {
        unsigned teachingTipDismissals = 0;
        wxWinUITransientRegistration teachingTip = wxWinUIRegisterTransient(
            owner, wxWinUITransientKind::TeachingTip,
            [&teachingTipDismissals]() { ++teachingTipDismissals; });
        REQUIRE(teachingTip);

        wxTipWindow::Ref tip = wxTipWindow::New(owner, "concurrent tip", 100);
        REQUIRE(tip);
        const wxWeakRef<wxWindow> weakTip(tip.operator->());
        CHECK(wxWinUITransientCountForTesting(owner) == 2);

        std::unique_ptr<wxWinUITLWHostWindowEventGuard> retainedOwner(
            new wxWinUITLWHostWindowEventGuard(owner));
        owner->Destroy();
        REQUIRE(wxWinUITLWHostIsDestroyScheduled(owner));
        CHECK_FALSE(tip);
        CHECK(teachingTipDismissals == 1);
        teachingTip = wxWinUITransientRegistration();
        retainedOwner.reset();

        REQUIRE(DrainAuxiliaryWindowsUntil(
            [weakOwner, weakTip]()
            {
                return !weakOwner && !weakTip;
            }));
    }

    SECTION("one_hundred_cycles")
    {
        for ( int i = 0; i < 100; ++i )
        {
            wxTipWindow::Ref tip = wxTipWindow::New(
                owner, wxString::Format("tip cycle %d", i), 100);
            REQUIRE(tip);
            const wxWeakRef<wxWindow> weakTip(tip.operator->());
            tip->Close();
            CHECK_FALSE(tip);
            INFO("tip cycle: " << i);
            REQUIRE(DrainAuxiliaryWindowsUntil(
                [weakTip]() { return !weakTip; }));
            REQUIRE(wxWinUITransientCountForTesting(owner) == 0);
        }
    }

    if ( weakOwner )
        owner->Destroy();
    REQUIRE(DrainAuxiliaryWindowsWithoutSleepingUntil(
        [weakOwner]() { return !weakOwner; }));
    const bool auxiliaryStateDrained =
        DrainAuxiliaryWindowsWithoutSleepingUntil(
        [&registryBefore, &hostsBefore]()
        {
            return SameTransientRegistry(
                wxWinUIGetTransientRegistrySnapshotForTesting(),
                registryBefore) &&
                   SameAuxiliaryHostSnapshot(hostsBefore);
        });
    if ( !auxiliaryStateDrained )
    {
        const wxWinUITransientRegistrySnapshot registry =
            wxWinUIGetTransientRegistrySnapshotForTesting();
        INFO("registry owners=" << registry.ownerCount
             << " baseline=" << registryBefore.ownerCount
             << ", transients=" << registry.transientCount
             << " baseline=" << registryBefore.transientCount
             << ", pending retire=" << registry.pendingOwnerRetireCount
             << " baseline=" << registryBefore.pendingOwnerRetireCount
             << ", hosts=" << wxWinUITopLevelHost::GetLiveHostCount()
             << " baseline=" << hostsBefore.hosts
             << ", host lifetimes="
             << wxWinUITopLevelHost::GetLiveHostLifetimeCount()
             << " baseline=" << hostsBefore.hostLifetimes
             << ", slots=" << wxWinUITopLevelHost::GetLiveSlotCount()
             << " baseline=" << hostsBefore.slots
             << ", slot lifetimes="
             << wxWinUITopLevelHost::GetLiveSlotLifetimeCount()
             << " baseline=" << hostsBefore.slotLifetimes);
    }
    REQUIRE(auxiliaryStateDrained);
    CHECK(wxWinUIGetTipRuntimeStateCountForTesting() ==
          runtimeStatesBefore);
}

#endif // wxUSE_TIPWINDOW

#endif // __WXWINUI__ && wxUSE_WINUI3 && (wxUSE_SPLASH || wxUSE_TIPWINDOW)
