///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuitransientwindows.cpp
// Purpose:     WinUI popup and mini-frame transient-window contract tests
// Author:      wxWidgets development team
// Created:     2026-08-02
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

#include "wx/app.h"
#include "wx/button.h"
#include "wx/frame.h"
#include "wx/minifram.h"
#include "wx/popupwin.h"
#include "wx/toplevel.h"
#include "wx/utils.h"
#include "wx/weakref.h"
#include "wx/window.h"

#include "wx/msw/wrapwin.h"
#include "wx/winui/private/dialogsession.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"
#include "wx/winui/private/transient.h"

#include <memory>
#include <new>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace
{

void DrainTransientWindows(unsigned rounds = 12)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        wxYield();
        wxMilliSleep(3);
    }
}

template <typename Predicate>
bool DrainTransientWindowsUntil(Predicate predicate,
                                unsigned rounds = 120)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        if ( predicate() )
            return true;
        DrainTransientWindows(1);
    }

    return predicate();
}

void ShowOffscreenWithoutActivation(wxTopLevelWindow *window, int ordinal)
{
    REQUIRE(window != nullptr);
    const HWND hwnd = static_cast<HWND>(window->GetHWND());
    REQUIRE(hwnd != nullptr);

    ::SetWindowLongPtr(
        hwnd,
        GWL_EXSTYLE,
        ::GetWindowLongPtr(hwnd, GWL_EXSTYLE) |
            WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW);
    window->Move(wxPoint(-30000 + ordinal * 320, -30000));
    window->ShowWithoutActivating();
}

bool SameLiveRegistry(const wxWinUITransientRegistrySnapshot& lhs,
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

struct HostPhysicalSnapshot
{
    unsigned hosts = wxWinUITopLevelHost::GetLiveHostCount();
    unsigned hostLifetimes =
        wxWinUITopLevelHost::GetLiveHostLifetimeCount();
    unsigned slots = wxWinUITopLevelHost::GetLiveSlotCount();
    unsigned slotLifetimes =
        wxWinUITopLevelHost::GetLiveSlotLifetimeCount();
    unsigned slotHandlerAdds =
        wxWinUITopLevelHost::GetSlotHandlerAddCount();
    unsigned slotHandlerRevokes =
        wxWinUITopLevelHost::GetSlotHandlerRevokeCount();
    unsigned rootHandlerAdds =
        wxWinUITopLevelHost::GetRootHandlerAddCount();
    unsigned rootHandlerRevokes =
        wxWinUITopLevelHost::GetRootHandlerRevokeCount();
};

template <typename T>
struct TrackedLifetime
{
    const char *label = nullptr;
    const void *identity = nullptr;
    std::weak_ptr<T> lifetime;
};

struct TrackedOwnerIdentity
{
    const char *label = nullptr;
    wxWinUITransientOwnerIdentityForTesting identity;
};

template <typename T>
void TrackLifetime(std::vector<TrackedLifetime<T>>& tracked,
                   const char *label,
                   const std::weak_ptr<T>& weakLifetime)
{
    const std::shared_ptr<T> lifetime = weakLifetime.lock();
    REQUIRE(lifetime != nullptr);
    tracked.push_back({ label, lifetime.get(), weakLifetime });
}

void TrackOwnerIdentity(std::vector<TrackedOwnerIdentity>& tracked,
                        const char *label,
                        std::uintptr_t ownerKey)
{
    const wxWinUITransientOwnerIdentityForTesting identity =
        wxWinUIGetTransientOwnerIdentityForTesting(ownerKey);
    REQUIRE(identity);
    tracked.push_back({ label, identity });
}

template <typename T>
bool AllTrackedLifetimesExpired(
    const std::vector<TrackedLifetime<T>>& tracked)
{
    for ( const TrackedLifetime<T>& item : tracked )
    {
        if ( !item.lifetime.expired() )
            return false;
    }

    return true;
}

bool AllTrackedOwnersRetired(
    const std::vector<TrackedOwnerIdentity>& tracked)
{
    for ( const TrackedOwnerIdentity& item : tracked )
    {
        if ( wxWinUIHasTransientOwnerIdentityForTesting(item.identity) )
            return false;
    }

    return true;
}

template <typename T>
std::string DescribeTrackedLifetimes(
    const std::vector<TrackedLifetime<T>>& tracked)
{
    std::ostringstream description;
    for ( const TrackedLifetime<T>& item : tracked )
    {
        description << (item.label ? item.label : "unnamed") << '@'
                    << item.identity << " expired="
                    << item.lifetime.expired() << "; ";
    }
    return description.str();
}

std::string DescribeTrackedOwners(
    const std::vector<TrackedOwnerIdentity>& tracked)
{
    std::ostringstream description;
    for ( const TrackedOwnerIdentity& item : tracked )
    {
        description << (item.label ? item.label : "unnamed") << '@'
                    << reinterpret_cast<const void *>(item.identity.ownerKey)
                    << " epoch=" << item.identity.epoch << " present="
                    << wxWinUIHasTransientOwnerIdentityForTesting(
                           item.identity)
                    << "; ";
    }
    return description.str();
}

std::string DescribePhysicalSnapshot(const HostPhysicalSnapshot& snapshot)
{
    std::ostringstream description;
    description << "hosts=" << snapshot.hosts
                << " host-lifetimes=" << snapshot.hostLifetimes
                << " slots=" << snapshot.slots
                << " slot-lifetimes=" << snapshot.slotLifetimes
                << " slot-add/revoke=" << snapshot.slotHandlerAdds << '/'
                << snapshot.slotHandlerRevokes
                << " root-add/revoke=" << snapshot.rootHandlerAdds << '/'
                << snapshot.rootHandlerRevokes;
    return description.str();
}

std::string DescribeRegistrySnapshot(
    const wxWinUITransientRegistrySnapshot& snapshot)
{
    std::ostringstream description;
    description << "owners=" << snapshot.ownerCount
                << " transients=" << snapshot.transientCount
                << " callbacks=" << snapshot.cancelCallbackCount
                << " modal=" << snapshot.modalCount
                << " popup=" << snapshot.popupCount
                << " teaching=" << snapshot.teachingTipCount
                << " pending-retire=" << snapshot.pendingOwnerRetireCount
                << " active-retire=" << snapshot.activeOwnerRetirementCount
                << " retire-enqueue/callback="
                << snapshot.ownerRetireEnqueueCount << '/'
                << snapshot.ownerRetireCallbackCount;
    return description.str();
}

bool SameLiveHostObjects(const HostPhysicalSnapshot& before)
{
    return wxWinUITopLevelHost::GetLiveHostCount() == before.hosts &&
           wxWinUITopLevelHost::GetLiveHostLifetimeCount() ==
               before.hostLifetimes &&
           wxWinUITopLevelHost::GetLiveSlotCount() == before.slots &&
           wxWinUITopLevelHost::GetLiveSlotLifetimeCount() ==
               before.slotLifetimes;
}

void CheckBalancedHandlers(const HostPhysicalSnapshot& before)
{
    CHECK(wxWinUITopLevelHost::GetSlotHandlerAddCount() -
              before.slotHandlerAdds ==
          wxWinUITopLevelHost::GetSlotHandlerRevokeCount() -
              before.slotHandlerRevokes);
    CHECK(wxWinUITopLevelHost::GetRootHandlerAddCount() -
              before.rootHandlerAdds ==
          wxWinUITopLevelHost::GetRootHandlerRevokeCount() -
              before.rootHandlerRevokes);
}

void DestroyAndDrain(wxWindow *window)
{
    if ( !window )
        return;

    const wxWeakRef<wxWindow> weak(window);
    if ( !window->IsBeingDeleted() )
        (void)window->Destroy();
    REQUIRE(DrainTransientWindowsUntil([weak]() { return !weak; }));
}

#if wxUSE_POPUPWIN

class TransientPopupProbe : public wxPopupTransientWindow
{
public:
    using wxPopupTransientWindowBase::DismissAndNotify;

    explicit TransientPopupProbe(wxWindow *parent,
                                 int style = wxPU_CONTAINS_CONTROLS)
        : wxPopupTransientWindow(parent, style)
    {
    }

    bool CanDismiss() override
    {
        ++canDismissCalls;
        if ( reopenDuringCanDismiss )
        {
            reopenDuringCanDismiss = false;
            Hide();
            Popup();
        }
        return allowOutsideDismiss;
    }

    void Dismiss() override
    {
        wxPopupTransientWindow::Dismiss();
        if ( destroyDuringDismiss )
        {
            destroyDuringDismiss = false;
            Destroy();
        }
    }

    bool Show(bool show = true) override
    {
        const bool changed = wxPopupTransientWindow::Show(show);
        wxWindow * const nestedFocus = nestedPopupFocus.get();
        if ( show && nestedFocus )
        {
            nestedPopupFocus = nullptr;
            Popup(nestedFocus);
        }
        return changed;
    }

    bool allowOutsideDismiss = true;
    bool destroyOnDismiss = false;
    bool deleteOnDismiss = false;
    bool reopenOnDismiss = false;
    bool rawReshowOnDismiss = false;
    bool reopenDuringCanDismiss = false;
    bool destroyDuringDismiss = false;
    wxWeakRef<wxWindow> nestedPopupFocus;
    unsigned canDismissCalls = 0;
    unsigned dismissNotifications = 0;
    std::shared_ptr<unsigned> sharedDismissNotifications;

protected:
    void OnDismiss() override
    {
        ++dismissNotifications;
        if ( sharedDismissNotifications )
            ++*sharedDismissNotifications;
        if ( rawReshowOnDismiss )
        {
            const HWND hwnd = static_cast<HWND>(GetHWND());
            if ( hwnd )
                (void)::ShowWindow(hwnd, SW_SHOWNA);
        }
        if ( reopenOnDismiss )
            Popup();
        if ( destroyOnDismiss )
            Destroy();
        else if ( deleteOnDismiss )
            delete this;
    }
};

class HostilePlainPopupProbe final : public wxPopupWindow
{
public:
    explicit HostilePlainPopupProbe(wxWindow *parent)
        : wxPopupWindow(parent, wxPU_CONTAINS_CONTROLS)
    {
    }

    bool Show(bool show = true) override
    {
        if ( !show && refuseVirtualHide )
        {
            ++refusedHideCalls;
            return false;
        }
        const bool changed = wxPopupWindow::Show(show);
        if ( !show && rawReshowAfterBaseHide )
        {
            const HWND hwnd = static_cast<HWND>(GetHWND());
            if ( hwnd )
                (void)::ShowWindow(hwnd, SW_SHOWNA);
        }
        return changed;
    }

    bool refuseVirtualHide = false;
    bool rawReshowAfterBaseHide = false;
    unsigned refusedHideCalls = 0;
};

// Regression tripwires for the exported ABI/API contract restored in this
// phase: neither transient class gains instance state, and the historical
// protected helper remains void.
static_assert(sizeof(wxPopupTransientWindowBase) == sizeof(wxPopupWindow));
static_assert(sizeof(wxPopupTransientWindow) ==
              sizeof(wxPopupTransientWindowBase));
static_assert(std::is_same<
                  decltype(std::declval<TransientPopupProbe&>()
                               .DismissAndNotify()),
                  void>::value,
              "DismissAndNotify() must retain its historical void API");

wxWindow *gs_popupPublishReparentTarget = nullptr;
bool gs_popupPublishNestedAttempted = false;
bool gs_popupPublishNestedSucceeded = false;
bool gs_popupPublishDestroyAttempted = false;
wxWindow *gs_popupLogicalReparentTarget = nullptr;
bool gs_popupCreateDestroyAttempted = false;
unsigned gs_popupOwnerDeactivateTails = 0;
wxWindow *gs_popupCancelOwnerDuringPublish = nullptr;
std::unique_ptr<wxWinUITransientOwnerRetirement>
    gs_popupPublishOwnerRetirement;
bool gs_popupPublishOwnerCancelled = false;

void ReparentPopupDuringOwnerPublication(wxPopupWindow *popup)
{
    gs_popupPublishNestedAttempted = true;
    wxWindow * const target = gs_popupPublishReparentTarget;
    wxWinUIResetPopupOwnerPublishHookForTesting();
    gs_popupPublishNestedSucceeded = target && popup->Reparent(target);
}

void DestroyPopupDuringOwnerPublication(wxPopupWindow *popup)
{
    gs_popupPublishDestroyAttempted = true;
    wxWinUIResetPopupOwnerPublishHookForTesting();
    (void)popup->Destroy();
}

void CancelDestinationDuringPopupOwnerPublication(
    wxPopupWindow *WXUNUSED(popup))
{
    wxWindow * const owner = gs_popupCancelOwnerDuringPublish;
    gs_popupPublishOwnerCancelled = true;
    wxWinUIResetPopupOwnerPublishHookForTesting();
    if ( !owner )
        return;

    gs_popupPublishOwnerRetirement =
        std::make_unique<wxWinUITransientOwnerRetirement>(
            wxWinUIBeginTransientOwnerRetirement(owner));
    wxWinUICancelTransientSessions(owner);
}

void ReparentPopupLogicallyDuringOwnerPublication(wxPopupWindow *popup)
{
    wxWindow * const target = gs_popupLogicalReparentTarget;
    wxWinUIResetPopupOwnerPublishHookForTesting();
    if ( target )
        (void)popup->wxWindowBase::Reparent(target);
}

void DestroyOwnerlessPopupDuringCreate(wxPopupWindow *popup)
{
    gs_popupCreateDestroyAttempted = true;
    wxWinUIResetPopupCreateHookForTesting();
    (void)popup->Destroy();
}

void CountPopupOwnerDeactivateTail(wxWindow *WXUNUSED(owner))
{
    ++gs_popupOwnerDeactivateTails;
}

struct PopupCaptureReleaseScenario
{
    wxWeakRef<wxWindow> first;
    wxWeakRef<wxWindow> second;
    bool persistent = false;
    bool reopenPopup = false;
    bool destroyPopup = false;
    bool dispatchCaptureLostEvent = false;
    bool nativeRecapture = false;
    bool invoked = false;
    unsigned invocations = 0;
};

PopupCaptureReleaseScenario *gs_popupCaptureReleaseScenario = nullptr;

struct PersistentPopupNativeReshowState
{
    bool enabled = true;
    bool inReshow = false;
    unsigned reshowCount = 0;
};

LRESULT CALLBACK PersistentPopupNativeReshowSubclass(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam,
    UINT_PTR subclassId,
    DWORD_PTR referenceData)
{
    PersistentPopupNativeReshowState * const state =
        reinterpret_cast<PersistentPopupNativeReshowState *>(referenceData);
    if ( message == WM_NCDESTROY )
    {
        if ( state )
            state->enabled = false;
        (void)::RemoveWindowSubclass(
            hwnd, PersistentPopupNativeReshowSubclass, subclassId);
        return ::DefSubclassProc(hwnd, message, wParam, lParam);
    }

    const LRESULT result =
        ::DefSubclassProc(hwnd, message, wParam, lParam);
    if ( message == WM_WINDOWPOSCHANGED && state && state->enabled &&
            !state->inReshow && ::IsWindow(hwnd) &&
            !::IsWindowVisible(hwnd) )
    {
        state->inReshow = true;
        ++state->reshowCount;
        (void)::ShowWindow(hwnd, SW_SHOWNA);
        state->inReshow = false;
    }

    return result;
}

void RecapturePopupChildAfterRelease(wxPopupTransientWindow *popup,
                                     unsigned int pass)
{
    PopupCaptureReleaseScenario * const scenario =
        gs_popupCaptureReleaseScenario;
    if ( !scenario || !popup || !popup->IsShown() )
        return;

    scenario->invoked = true;
    ++scenario->invocations;
    if ( scenario->destroyPopup )
    {
        (void)popup->Destroy();
        return;
    }
    if ( scenario->reopenPopup )
    {
        (void)popup->Hide();
        popup->Popup();
        return;
    }

    wxWindow * const target =
        (pass % 2 == 0 ? scenario->second : scenario->first).get();
    if ( scenario->nativeRecapture && target )
    {
        // Deliberately bypass wxWindowBase::CaptureMouse(): production peers
        // and application code can call USER32 directly, and terminal
        // retirement must remain bounded even in that hostile case.
        (void)::SetCapture(static_cast<HWND>(target->GetHWND()));
        return;
    }
    if ( scenario->dispatchCaptureLostEvent && target )
    {
        wxMouseCaptureLostEvent event(target->GetId());
        event.SetEventObject(target);
        (void)target->GetEventHandler()->ProcessEvent(event);
        return;
    }
    if ( target && !target->IsBeingDeleted() &&
            popup->IsDescendant(target) && !target->HasCapture() )
    {
        target->CaptureMouse();
    }

    if ( !scenario->persistent )
        gs_popupCaptureReleaseScenario = nullptr;
}

class PopupCaptureReleaseHookScope final
{
public:
    explicit PopupCaptureReleaseHookScope(
        PopupCaptureReleaseScenario& scenario)
    {
        gs_popupCaptureReleaseScenario = &scenario;
        wxWinUISetPopupCaptureReleaseHookForTesting(
            &RecapturePopupChildAfterRelease);
    }

    ~PopupCaptureReleaseHookScope()
    {
        Reset();
    }

    void Reset()
    {
        gs_popupCaptureReleaseScenario = nullptr;
        wxWinUIResetPopupCaptureReleaseHookForTesting();
    }
};

#endif // wxUSE_POPUPWIN

bool gs_nonOwnedDestroyAttempted = false;
wxWindow *gs_nonOwnedOwnerDestroyTarget = nullptr;
bool gs_nonOwnedOwnerDestroyAttempted = false;
wxWindow *gs_nonOwnedLayoutReparentTarget = nullptr;
bool gs_nonOwnedLayoutReparentAttempted = false;

void DestroyNonOwnedDuringOwnerPublication(wxWindow *window)
{
    gs_nonOwnedDestroyAttempted = true;
    wxWinUIResetNonOwnedOwnerPublishHookForTesting();
    (void)window->Destroy();
}

void DestroyProspectiveOwnerDuringNonOwnedPublication(
    wxWindow *WXUNUSED(window))
{
    wxWindow * const owner = gs_nonOwnedOwnerDestroyTarget;
    gs_nonOwnedOwnerDestroyAttempted = true;
    wxWinUIResetNonOwnedOwnerPublishHookForTesting();
    if ( owner )
        (void)owner->Destroy();
}

void ReparentNonOwnedDuringLayoutRefresh(wxWindow *window)
{
    wxWindow * const target = gs_nonOwnedLayoutReparentTarget;
    gs_nonOwnedLayoutReparentAttempted = true;
    wxWinUIResetNonOwnedLayoutRefreshHookForTesting();
    if ( window && target )
        (void)window->wxWindowBase::Reparent(target);
}

} // anonymous namespace

#if wxUSE_POPUPWIN

TEST_CASE("WinUITransientWindows::PopupHostShowHideAndReparent",
          "[winui-beta-transients][popup][HostLifecycle]")
{
    const wxWinUITransientRegistrySnapshot registryBefore =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    const HostPhysicalSnapshot physicalBefore;
    std::vector<TrackedLifetime<wxWinUIHostLifetime>> ownedHostLifetimes;
    std::vector<TrackedLifetime<wxWinUISlotLifetime>> ownedSlotLifetimes;
    std::vector<TrackedOwnerIdentity> ownedRegistryOwners;

    wxFrame * const ownerA =
        new wxFrame(nullptr, wxID_ANY, "popup-owner-a",
                    wxPoint(-30000, -30000), wxSize(260, 160));
    wxFrame * const ownerB =
        new wxFrame(nullptr, wxID_ANY, "popup-owner-b",
                    wxPoint(-29600, -30000), wxSize(260, 160));
    ShowOffscreenWithoutActivation(ownerA, 0);
    ShowOffscreenWithoutActivation(ownerB, 1);

    wxPopupWindow * const popup = new wxPopupWindow;
    REQUIRE(popup->Create(ownerA, wxPU_CONTAINS_CONTROLS));
    popup->SetSize(wxRect(-29200, -30000, 220, 120));
    const HWND popupHwnd = static_cast<HWND>(popup->GetHWND());
    REQUIRE(popupHwnd != nullptr);
    CHECK((::GetWindowLongPtr(popupHwnd, GWL_STYLE) & WS_POPUP) != 0);
    CHECK((::GetWindowLongPtr(popupHwnd, GWL_STYLE) & WS_CHILD) == 0);
    CHECK(::GetWindow(popupHwnd, GW_OWNER) ==
          static_cast<HWND>(ownerA->GetHWND()));

    // A plain wxWindow remains a native/GDI child while the wxButton obtains
    // a slot in the ordinary shared per-TLW WinUI host. There is no popup-
    // specific island or presenter.
    wxWindow * const gdi =
        new wxWindow(popup, wxID_ANY, wxPoint(5, 5), wxSize(60, 25));
    wxButton * const xaml =
        new wxButton(popup, wxID_ANY, "xaml", wxPoint(70, 5),
                     wxSize(80, 30));
    wxWinUITopLevelHost * const popupHost =
        wxWinUITopLevelHost::FindForTLW(popup);
    REQUIRE(popupHost != nullptr);
    TrackLifetime(ownedHostLifetimes, "popup-host",
                  popupHost->GetLifetimeForTest());
    REQUIRE(popupHost->FindSlot(xaml) != nullptr);
    TrackLifetime(ownedSlotLifetimes, "popup-slot-initial",
                  popupHost->FindSlot(xaml)->GetLifetimeForTest());
    CHECK(wxWinUITopLevelHost::FindSlotOwner(xaml) == popupHost);
    CHECK(wxWinUITopLevelHost::FindSlotOwner(gdi) == nullptr);

    REQUIRE(popup->Show());
    CHECK(popup->IsShown());
    CHECK(wxWinUITransientCountForTesting(
              ownerA, wxWinUITransientKind::Popup) == 1);
    REQUIRE(popup->Hide());
    CHECK_FALSE(popup->IsShown());
    CHECK(wxWinUITransientCountForTesting(ownerA) == 0);

    // Reparenting the popup itself migrates its native owner and central
    // transient generation, while its children remain on the popup's own
    // ordinary TLW host.
    REQUIRE(popup->Show());
    REQUIRE(popup->Reparent(ownerB));
    CHECK(popup->IsShown());
    CHECK(popup->MSWGetOwner() == ownerB);
    CHECK(::GetWindow(popupHwnd, GW_OWNER) ==
          static_cast<HWND>(ownerB->GetHWND()));
    CHECK(wxWinUITransientCountForTesting(ownerA) == 0);
    CHECK(wxWinUITransientCountForTesting(
              ownerB, wxWinUITransientKind::Popup) == 1);
    CHECK(wxWinUITopLevelHost::FindSlotOwner(xaml) == popupHost);
    REQUIRE(popup->Hide());

    // Prove that WS_POPUP owner migration uses the checked non-owned-window
    // transaction, including rollback when GWLP_HWNDPARENT publication fails.
    wxWinUIMSWFailNextOwnerWriteForTest();
    CHECK_FALSE(popup->Reparent(ownerA));
    CHECK(popup->GetParent() == ownerB);
    CHECK(::GetWindow(popupHwnd, GW_OWNER) ==
          static_cast<HWND>(ownerB->GetHWND()));
    REQUIRE(popup->Reparent(ownerA));
    CHECK(popup->MSWGetOwner() == ownerA);
    CHECK(::GetWindow(popupHwnd, GW_OWNER) ==
          static_cast<HWND>(ownerA->GetHWND()));

    // Child migration reuses the destination TLW host and comes back to the
    // exact popup host; it never creates a popup-specific slot manager.
    REQUIRE(xaml->Reparent(ownerB));
    wxWinUITopLevelHost * const ownerBHost =
        wxWinUITopLevelHost::FindSlotOwner(xaml);
    REQUIRE(ownerBHost != nullptr);
    TrackLifetime(ownedHostLifetimes, "owner-b-host",
                  ownerBHost->GetLifetimeForTest());
    REQUIRE(ownerBHost->FindSlot(xaml) != nullptr);
    TrackLifetime(ownedSlotLifetimes, "owner-b-slot",
                  ownerBHost->FindSlot(xaml)->GetLifetimeForTest());
    CHECK(ownerBHost == wxWinUITopLevelHost::FindForTLW(ownerB));
    CHECK(ownerBHost != popupHost);
    REQUIRE(gdi->Reparent(ownerB));
    CHECK(wxWinUITopLevelHost::FindSlotOwner(gdi) == nullptr);

    REQUIRE(xaml->Reparent(popup));
    REQUIRE(gdi->Reparent(popup));
    CHECK(wxWinUITopLevelHost::FindSlotOwner(xaml) == popupHost);
    REQUIRE(popupHost->FindSlot(xaml) != nullptr);
    TrackLifetime(ownedSlotLifetimes, "popup-slot-restored",
                  popupHost->FindSlot(xaml)->GetLifetimeForTest());
    CHECK(wxWinUITopLevelHost::FindSlotOwner(gdi) == nullptr);

    const wxWeakRef<wxWindow> popupWeak(popup);
    const std::uintptr_t popupKey = reinterpret_cast<std::uintptr_t>(popup);
    REQUIRE(popup->Show());
    REQUIRE(popup->Destroy());
    TrackOwnerIdentity(ownedRegistryOwners, "popup-owner", popupKey);
    REQUIRE(DrainTransientWindowsUntil([popupWeak]() { return !popupWeak; }));
    CHECK(wxWinUITransientCountForTesting(ownerA) == 0);

    const wxWeakRef<wxWindow> ownerAWeak(ownerA);
    const wxWeakRef<wxWindow> ownerBWeak(ownerB);
    const std::uintptr_t ownerAKey = reinterpret_cast<std::uintptr_t>(ownerA);
    const std::uintptr_t ownerBKey = reinterpret_cast<std::uintptr_t>(ownerB);
    ownerA->Destroy();
    TrackOwnerIdentity(ownedRegistryOwners, "owner-a", ownerAKey);
    ownerB->Destroy();
    TrackOwnerIdentity(ownedRegistryOwners, "owner-b", ownerBKey);
    const bool restored = DrainTransientWindowsUntil([&]()
    {
        return !popupWeak && !ownerAWeak && !ownerBWeak &&
               AllTrackedLifetimesExpired(ownedHostLifetimes) &&
               AllTrackedLifetimesExpired(ownedSlotLifetimes) &&
               AllTrackedOwnersRetired(ownedRegistryOwners);
    });
    const HostPhysicalSnapshot physicalAfter;
    const wxWinUITransientRegistrySnapshot registryAfter =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    INFO("physical before: " << DescribePhysicalSnapshot(physicalBefore));
    INFO("physical after: " << DescribePhysicalSnapshot(physicalAfter));
    INFO("registry before: " << DescribeRegistrySnapshot(registryBefore));
    INFO("registry after: " << DescribeRegistrySnapshot(registryAfter));
    INFO("owned hosts: " <<
         DescribeTrackedLifetimes(ownedHostLifetimes));
    INFO("owned slots: " <<
         DescribeTrackedLifetimes(ownedSlotLifetimes));
    INFO("owned registry owners: " <<
         DescribeTrackedOwners(ownedRegistryOwners));
    INFO("owner weak refs expired: A=" << !ownerAWeak
         << " B=" << !ownerBWeak << " popup=" << !popupWeak);
    REQUIRE(restored);
    CheckBalancedHandlers(physicalBefore);
}

TEST_CASE("WinUITransientWindows::OwnerlessPopup",
          "[winui-beta-transients][popup][ownerless]")
{
    const wxWinUITransientRegistrySnapshot registryBefore =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    const HostPhysicalSnapshot physicalBefore;

    wxPopupWindow * const popup = new wxPopupWindow;
    REQUIRE(popup->Create(nullptr, wxPU_CONTAINS_CONTROLS));
    popup->SetSize(wxRect(-30000, -30000, 180, 90));
    const HWND popupHwnd = static_cast<HWND>(popup->GetHWND());
    REQUIRE(popupHwnd != nullptr);
    CHECK((::GetWindowLongPtr(popupHwnd, GWL_STYLE) & WS_POPUP) != 0);
    CHECK((::GetWindowLongPtr(popupHwnd, GWL_STYLE) & WS_CHILD) == 0);
    CHECK(::GetWindow(popupHwnd, GW_OWNER) == nullptr);
    wxButton * const child = new wxButton(popup, wxID_ANY, "ownerless");
    REQUIRE(wxWinUITopLevelHost::FindSlotOwner(child) != nullptr);
    CHECK(popup->MSWGetOwner() == nullptr);

    REQUIRE(popup->Show());
    const wxWinUITransientRegistrySnapshot registryAfter =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    INFO("registry before/after: owners " << registryBefore.ownerCount << "/"
         << registryAfter.ownerCount << ", transients "
         << registryBefore.transientCount << "/"
         << registryAfter.transientCount << ", callbacks "
         << registryBefore.cancelCallbackCount << "/"
         << registryAfter.cancelCallbackCount << ", modal "
         << registryBefore.modalCount << "/" << registryAfter.modalCount
         << ", popup " << registryBefore.popupCount << "/"
         << registryAfter.popupCount << ", teaching "
         << registryBefore.teachingTipCount << "/"
         << registryAfter.teachingTipCount << ", pending-retire "
         << registryBefore.pendingOwnerRetireCount << "/"
         << registryAfter.pendingOwnerRetireCount << ", active-retire "
         << registryBefore.activeOwnerRetirementCount << "/"
         << registryAfter.activeOwnerRetirementCount);
    CHECK(SameLiveRegistry(registryAfter, registryBefore));
    REQUIRE(popup->Hide());

    // The WinUI port deliberately supports the plan's ownerless case as a
    // caller-owned extension: it has no native owner and cannot inherit an
    // arbitrary application's TLW lifetime. Explicit Destroy() is therefore
    // its shutdown contract.
    DestroyAndDrain(popup);
    REQUIRE(DrainTransientWindowsUntil([&]()
    {
        // Owner retirement is posted from the popup's wxEVT_DESTROY handler.
        // Losing the weak popup and its host can therefore precede the
        // manager callback by one event-loop turn: keep pumping until both
        // independent lifetime domains have converged.
        return SameLiveHostObjects(physicalBefore) &&
               SameLiveRegistry(
                   wxWinUIGetTransientRegistrySnapshotForTesting(),
                   registryBefore);
    }));
    CheckBalancedHandlers(physicalBefore);
}

TEST_CASE("WinUITransientWindows::OwnerlessCreateDestructionIsTerminal",
          "[winui-beta-transients][popup][ownerless][reentrancy]")
{
    wxPopupWindow * const popup = new wxPopupWindow;
    const wxWeakRef<wxWindow> popupWeak(popup);
    gs_popupCreateDestroyAttempted = false;
    wxWinUISetPopupCreateHookForTesting(
        &DestroyOwnerlessPopupDuringCreate);

    CHECK_FALSE(popup->Create(nullptr, wxPU_CONTAINS_CONTROLS));
    wxWinUIResetPopupCreateHookForTesting();
    CHECK(gs_popupCreateDestroyAttempted);
    REQUIRE(DrainTransientWindowsUntil([popupWeak]() { return !popupWeak; }));
}

TEST_CASE("WinUITransientWindows::PopupLatestInvocationAndFocusGeneration",
          "[winui-beta-transients][popup][focus][reentrancy]")
{
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "focus-owner",
                    wxPoint(-30000, -30000), wxSize(300, 160));
    ShowOffscreenWithoutActivation(owner, 0);
    wxButton * const ownerFocusA =
        new wxButton(owner, wxID_ANY, "owner-a",
                     wxPoint(5, 5), wxSize(90, 30));
    wxButton * const ownerFocusB =
        new wxButton(owner, wxID_ANY, "owner-b",
                     wxPoint(105, 5), wxSize(90, 30));
    ownerFocusA->SetFocus();
    REQUIRE(wxWindow::FindFocus() == ownerFocusA);

    TransientPopupProbe * const popup = new TransientPopupProbe(owner);
    popup->SetSize(wxRect(-29600, -30000, 220, 100));
    wxButton * const popupFocusA =
        new wxButton(popup, wxID_ANY, "popup-a",
                     wxPoint(5, 5), wxSize(90, 30));
    wxButton * const popupFocusB =
        new wxButton(popup, wxID_ANY, "popup-b",
                     wxPoint(105, 5), wxSize(90, 30));

    // The nested Popup(B) completes while the outer Popup(A) Show() frame is
    // still active. Only B's operation token may publish a focus tail.
    popup->nestedPopupFocus = popupFocusB;
    popup->Popup(popupFocusA);
    REQUIRE(popup->IsShown());
    CHECK(wxWindow::FindFocus() == popupFocusB);
    popup->Dismiss();
    CHECK(wxWindow::FindFocus() == ownerFocusA);

    // Dismiss() on an already hidden popup is a strict no-op and must not
    // restore ownerFocusA from the preceding visibility generation.
    ownerFocusB->SetFocus();
    REQUIRE(wxWindow::FindFocus() == ownerFocusB);
    popup->Dismiss();
    CHECK(wxWindow::FindFocus() == ownerFocusB);

    // A direct Show(), rather than Popup(), owns a fresh focus snapshot too.
    REQUIRE(popup->Show());
    popupFocusA->SetFocus();
    REQUIRE(wxWindow::FindFocus() == popupFocusA);
    popup->Dismiss();
    CHECK(wxWindow::FindFocus() == ownerFocusB);

    DestroyAndDrain(popup);
    owner->Destroy();
    DrainTransientWindows();
}

TEST_CASE("WinUITransientWindows::OnDismissReopenInvalidatesNativeTail",
          "[winui-beta-transients][popup][outside-click][reentrancy]")
{
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "reopen-owner",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(owner, 0);
    TransientPopupProbe * const popup = new TransientPopupProbe(owner);
    popup->reopenOnDismiss = true;
    popup->Popup();
    REQUIRE(::GetActiveWindow() !=
            static_cast<HWND>(owner->GetHWND()));

    gs_popupOwnerDeactivateTails = 0;
    wxWinUISetPopupOwnerDeactivateHookForTesting(
        &CountPopupOwnerDeactivateTail);
    wxWinUITestPopupOutsideClick(popup);
    wxWinUIResetPopupOwnerDeactivateHookForTesting();

    CHECK(popup->IsShown());
    CHECK(popup->dismissNotifications == 1);
    CHECK(gs_popupOwnerDeactivateTails == 0);

    popup->reopenOnDismiss = false;
    popup->Dismiss();
    DestroyAndDrain(popup);
    owner->Destroy();
    DrainTransientWindows();
}

TEST_CASE("WinUITransientWindows::LogicalParentTLWMigration",
          "[winui-beta-transients][popup][reparent][registry]")
{
    const wxWinUITransientRegistrySnapshot registryBefore =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    wxFrame * const ownerA =
        new wxFrame(nullptr, wxID_ANY, "logical-owner-a",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    wxFrame * const ownerB =
        new wxFrame(nullptr, wxID_ANY, "logical-owner-b",
                    wxPoint(-29600, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(ownerA, 0);
    ShowOffscreenWithoutActivation(ownerB, 1);
    wxWindow * const anchor = new wxWindow(ownerA, wxID_ANY);
    wxPopupWindow * const popup =
        new wxPopupWindow(anchor, wxPU_CONTAINS_CONTROLS);
    new wxButton(popup, wxID_ANY, "slot");

    REQUIRE(popup->Show());
    CHECK(wxWinUITransientCountForTesting(
              ownerA, wxWinUITransientKind::Popup) == 1);
    REQUIRE(anchor->Reparent(ownerB));

    // A repeated Show() is the idempotent synchronization point for an
    // already-visible popup whose intermediate logical parent moved TLWs.
    CHECK_FALSE(popup->Show());
    CHECK(popup->MSWGetOwner() == ownerB);
    CHECK(::GetWindow(static_cast<HWND>(popup->GetHWND()), GW_OWNER) ==
          static_cast<HWND>(ownerB->GetHWND()));
    CHECK(wxWinUITransientCountForTesting(ownerA) == 0);
    CHECK(wxWinUITransientCountForTesting(
              ownerB, wxWinUITransientKind::Popup) == 1);

    DestroyAndDrain(popup);
    ownerA->Destroy();
    ownerB->Destroy();
    REQUIRE(DrainTransientWindowsUntil([&]()
    {
        return SameLiveRegistry(
            wxWinUIGetTransientRegistrySnapshotForTesting(),
            registryBefore);
    }));
}

TEST_CASE("WinUITransientWindows::StaleLogicalOwnerCancelMigratesImmediately",
          "[winui-beta-transients][popup][reparent][registry][reentrancy]")
{
    const wxWinUITransientRegistrySnapshot registryBefore =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    wxFrame * const ownerA =
        new wxFrame(nullptr, wxID_ANY, "stale-logical-a",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    wxFrame * const ownerB =
        new wxFrame(nullptr, wxID_ANY, "stale-logical-b",
                    wxPoint(-29600, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(ownerA, 0);
    ShowOffscreenWithoutActivation(ownerB, 1);
    wxWindow * const anchor = new wxWindow(ownerA, wxID_ANY);
    wxPopupWindow * const popup =
        new wxPopupWindow(anchor, wxPU_CONTAINS_CONTROLS);
    REQUIRE(popup->Show());
    REQUIRE(anchor->Reparent(ownerB));

    // The old owner withdraws its manager generation before invoking the
    // callback. The callback must discover B from the exact live topology and
    // register there without requiring a repeated user Show() call.
    wxWinUICancelTransientSessions(ownerA);
    CHECK(popup->IsShown());
    CHECK(popup->MSWGetOwner() == ownerB);
    CHECK(::GetWindow(static_cast<HWND>(popup->GetHWND()), GW_OWNER) ==
          static_cast<HWND>(ownerB->GetHWND()));
    CHECK(wxWinUITransientCountForTesting(ownerA) == 0);
    CHECK(wxWinUITransientCountForTesting(
              ownerB, wxWinUITransientKind::Popup) == 1);

    DestroyAndDrain(popup);
    ownerA->Destroy();
    ownerB->Destroy();
    REQUIRE(DrainTransientWindowsUntil([&]()
    {
        return SameLiveRegistry(
            wxWinUIGetTransientRegistrySnapshotForTesting(),
            registryBefore);
    }));
}

TEST_CASE("WinUITransientWindows::PopupOwnerRecoveryVerifiesNativeWrite",
          "[winui-beta-transients][popup][owner][failure][reentrancy]")
{
    wxFrame * const ownerA =
        new wxFrame(nullptr, wxID_ANY, "recover-owner-a",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    wxFrame * const ownerB =
        new wxFrame(nullptr, wxID_ANY, "recover-owner-b",
                    wxPoint(-29600, -30000), wxSize(220, 120));
    wxFrame * const ownerC =
        new wxFrame(nullptr, wxID_ANY, "recover-owner-c",
                    wxPoint(-29200, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(ownerA, 0);
    ShowOffscreenWithoutActivation(ownerB, 1);
    ShowOffscreenWithoutActivation(ownerC, 2);

    wxPopupWindow * const popup =
        new wxPopupWindow(ownerA, wxPU_CONTAINS_CONTROLS);
    REQUIRE(popup->Show());
    REQUIRE(popup->wxWindowBase::Reparent(ownerB));

    gs_popupLogicalReparentTarget = ownerC;
    wxWinUISetPopupOwnerPublishHookForTesting(
        &ReparentPopupLogicallyDuringOwnerPublication);
    wxWinUIFailNextPopupOwnerRecoveryForTesting();
    CHECK_FALSE(popup->Show());
    wxWinUIResetPopupOwnerPublishHookForTesting();
    gs_popupLogicalReparentTarget = nullptr;

    CHECK_FALSE(popup->IsShown());
    CHECK(popup->GetParent() == ownerC);
    CHECK(popup->MSWGetOwner() == ownerC);
    CHECK(::GetWindow(static_cast<HWND>(popup->GetHWND()), GW_OWNER) ==
          static_cast<HWND>(ownerC->GetHWND()));
    CHECK(wxWinUITransientCountForTesting(ownerA) == 0);
    CHECK(wxWinUITransientCountForTesting(ownerB) == 0);
    CHECK(wxWinUITransientCountForTesting(ownerC) == 0);

    REQUIRE(popup->Show());
    CHECK(wxWinUITransientCountForTesting(
              ownerC, wxWinUITransientKind::Popup) == 1);
    DestroyAndDrain(popup);
    ownerA->Destroy();
    ownerB->Destroy();
    ownerC->Destroy();
    DrainTransientWindows();
}

TEST_CASE("WinUITransientWindows::NestedOwnerPublicationWins",
          "[winui-beta-transients][popup][owner][reentrancy]")
{
    const wxWinUITransientRegistrySnapshot registryBefore =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    wxFrame * const ownerA =
        new wxFrame(nullptr, wxID_ANY, "publish-owner-a",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    wxFrame * const ownerB =
        new wxFrame(nullptr, wxID_ANY, "publish-owner-b",
                    wxPoint(-29600, -30000), wxSize(220, 120));
    wxFrame * const ownerC =
        new wxFrame(nullptr, wxID_ANY, "publish-owner-c",
                    wxPoint(-29200, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(ownerA, 0);
    ShowOffscreenWithoutActivation(ownerB, 1);
    ShowOffscreenWithoutActivation(ownerC, 2);

    wxWindow * const anchor = new wxWindow(ownerA, wxID_ANY);
    wxPopupWindow * const popup =
        new wxPopupWindow(anchor, wxPU_CONTAINS_CONTROLS);
    REQUIRE(popup->Show());
    REQUIRE(anchor->Reparent(ownerB));

    // The outer visible-Show synchronizes A -> B. At its synchronous native
    // owner write, a nested transaction migrates the popup itself to C. The
    // outer B registration and native write must be discarded without hiding
    // or unregistering the newer C generation.
    gs_popupPublishReparentTarget = ownerC;
    gs_popupPublishNestedAttempted = false;
    gs_popupPublishNestedSucceeded = false;
    wxWinUISetPopupOwnerPublishHookForTesting(
        &ReparentPopupDuringOwnerPublication);
    CHECK_FALSE(popup->Show());
    wxWinUIResetPopupOwnerPublishHookForTesting();
    gs_popupPublishReparentTarget = nullptr;

    CHECK(gs_popupPublishNestedAttempted);
    CHECK(gs_popupPublishNestedSucceeded);
    CHECK(popup->IsShown());
    CHECK(popup->GetParent() == ownerC);
    CHECK(popup->MSWGetOwner() == ownerC);
    CHECK(::GetWindow(static_cast<HWND>(popup->GetHWND()), GW_OWNER) ==
          static_cast<HWND>(ownerC->GetHWND()));
    CHECK(wxWinUITransientCountForTesting(ownerA) == 0);
    CHECK(wxWinUITransientCountForTesting(ownerB) == 0);
    CHECK(wxWinUITransientCountForTesting(
              ownerC, wxWinUITransientKind::Popup) == 1);

    DestroyAndDrain(popup);
    ownerA->Destroy();
    ownerB->Destroy();
    ownerC->Destroy();
    REQUIRE(DrainTransientWindowsUntil([&]()
    {
        return SameLiveRegistry(
            wxWinUIGetTransientRegistrySnapshotForTesting(),
            registryBefore);
    }));
}

TEST_CASE("WinUITransientWindows::OwnerPublicationDestroyIsTerminal",
          "[winui-beta-transients][popup][owner][destroy][reentrancy]")
{
    const wxWinUITransientRegistrySnapshot registryBefore =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    wxFrame * const ownerA =
        new wxFrame(nullptr, wxID_ANY, "publish-destroy-a",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    wxFrame * const ownerB =
        new wxFrame(nullptr, wxID_ANY, "publish-destroy-b",
                    wxPoint(-29600, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(ownerA, 0);
    ShowOffscreenWithoutActivation(ownerB, 1);

    wxWindow * const anchor = new wxWindow(ownerA, wxID_ANY);
    wxPopupWindow * const popup =
        new wxPopupWindow(anchor, wxPU_CONTAINS_CONTROLS);
    REQUIRE(popup->Show());
    REQUIRE(anchor->Reparent(ownerB));

    const wxWeakRef<wxWindow> popupWeak(popup);
    gs_popupPublishDestroyAttempted = false;
    wxWinUISetPopupOwnerPublishHookForTesting(
        &DestroyPopupDuringOwnerPublication);
    CHECK_FALSE(popup->Show());
    wxWinUIResetPopupOwnerPublishHookForTesting();

    CHECK(gs_popupPublishDestroyAttempted);
    REQUIRE(DrainTransientWindowsUntil([popupWeak]() { return !popupWeak; }));
    CHECK(wxWinUITransientCountForTesting(ownerA) == 0);
    CHECK(wxWinUITransientCountForTesting(ownerB) == 0);

    ownerA->Destroy();
    ownerB->Destroy();
    REQUIRE(DrainTransientWindowsUntil([&]()
    {
        return SameLiveRegistry(
            wxWinUIGetTransientRegistrySnapshotForTesting(),
            registryBefore);
    }));
}

TEST_CASE("WinUITransientWindows::StaleOwnerCancelCannotKillMigration",
          "[winui-beta-transients][popup][owner][registry][reentrancy]")
{
    const wxWinUITransientRegistrySnapshot registryBefore =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    wxFrame * const ownerA =
        new wxFrame(nullptr, wxID_ANY, "cancel-migrate-a",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    wxFrame * const ownerB =
        new wxFrame(nullptr, wxID_ANY, "cancel-migrate-b",
                    wxPoint(-29600, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(ownerA, 0);
    ShowOffscreenWithoutActivation(ownerB, 1);

    TransientPopupProbe * const popup = new TransientPopupProbe(ownerA);
    const wxWeakRef<wxWindow> popupWeak(popup);
    bool migrationAttempted = false;
    bool migrationSucceeded = false;
    auto migrateFirst = wxWinUIRegisterTransient(
        ownerA, wxWinUITransientKind::Popup,
        [popupWeak, ownerB, &migrationAttempted, &migrationSucceeded]()
        {
            migrationAttempted = true;
            wxPopupWindow * const live =
                wxDynamicCast(popupWeak.get(), wxPopupWindow);
            migrationSucceeded = live && live->Reparent(ownerB);
        });
    REQUIRE(migrateFirst);
    popup->Popup();
    REQUIRE(popup->IsShown());
    CHECK(wxWinUITransientCountForTesting(
              ownerA, wxWinUITransientKind::Popup) == 2);

    wxWinUICancelTransientSessions(ownerA);
    CHECK(migrationAttempted);
    CHECK(migrationSucceeded);
    CHECK(popup->IsShown());
    CHECK(popup->GetParent() == ownerB);
    CHECK(popup->dismissNotifications == 0);
    CHECK(wxWinUITransientCountForTesting(ownerA) == 0);
    CHECK(wxWinUITransientCountForTesting(
              ownerB, wxWinUITransientKind::Popup) == 1);

    migrateFirst.Reset();
    DestroyAndDrain(popup);
    ownerA->Destroy();
    ownerB->Destroy();
    REQUIRE(DrainTransientWindowsUntil([&]()
    {
        return SameLiveRegistry(
            wxWinUIGetTransientRegistrySnapshotForTesting(),
            registryBefore);
    }));
}

TEST_CASE("WinUITransientWindows::NewRegistrationCancelledDuringPublication",
          "[winui-beta-transients][popup][owner][registry][reentrancy]")
{
    const wxWinUITransientRegistrySnapshot registryBefore =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    wxFrame * const ownerA =
        new wxFrame(nullptr, wxID_ANY, "publish-cancel-new-a",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    wxFrame * const ownerB =
        new wxFrame(nullptr, wxID_ANY, "publish-cancel-new-b",
                    wxPoint(-29600, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(ownerA, 0);
    ShowOffscreenWithoutActivation(ownerB, 1);

    wxPopupWindow * const popup =
        new wxPopupWindow(ownerA, wxPU_CONTAINS_CONTROLS);
    const HWND popupHwnd = static_cast<HWND>(popup->GetHWND());
    REQUIRE(popupHwnd != nullptr);
    REQUIRE(popup->wxWindowBase::Reparent(ownerB));

    gs_popupCancelOwnerDuringPublish = ownerB;
    gs_popupPublishOwnerCancelled = false;
    wxWinUISetPopupOwnerPublishHookForTesting(
        &CancelDestinationDuringPopupOwnerPublication);
    CHECK_FALSE(popup->Show());
    wxWinUIResetPopupOwnerPublishHookForTesting();
    gs_popupCancelOwnerDuringPublish = nullptr;

    CHECK(gs_popupPublishOwnerCancelled);
    REQUIRE(gs_popupPublishOwnerRetirement != nullptr);
    CHECK(static_cast<bool>(*gs_popupPublishOwnerRetirement));
    CHECK_FALSE(popup->IsShown());
    CHECK_FALSE(::IsWindowVisible(popupHwnd));
    CHECK(popup->MSWGetOwner() == ownerB);
    CHECK(::GetWindow(popupHwnd, GW_OWNER) ==
          static_cast<HWND>(ownerB->GetHWND()));
    CHECK(wxWinUITransientCountForTesting(ownerA) == 0);
    CHECK(wxWinUITransientCountForTesting(ownerB) == 0);

    // Releasing the destination retirement makes the still-hidden object
    // reusable; no cancelled local registration was committed to its sidecar.
    gs_popupPublishOwnerRetirement.reset();
    REQUIRE(popup->Show());
    CHECK(wxWinUITransientCountForTesting(
              ownerB, wxWinUITransientKind::Popup) == 1);

    DestroyAndDrain(popup);
    ownerA->Destroy();
    ownerB->Destroy();
    REQUIRE(DrainTransientWindowsUntil([&]()
    {
        return SameLiveRegistry(
            wxWinUIGetTransientRegistrySnapshotForTesting(), registryBefore);
    }));
}

TEST_CASE("WinUITransientWindows::MigrationCancelledDuringPublication",
          "[winui-beta-transients][popup][owner][registry][reentrancy]")
{
    const wxWinUITransientRegistrySnapshot registryBefore =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    wxFrame * const ownerA =
        new wxFrame(nullptr, wxID_ANY, "publish-cancel-migrate-a",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    wxFrame * const ownerB =
        new wxFrame(nullptr, wxID_ANY, "publish-cancel-migrate-b",
                    wxPoint(-29600, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(ownerA, 0);
    ShowOffscreenWithoutActivation(ownerB, 1);

    wxPopupWindow * const popup =
        new wxPopupWindow(ownerA, wxPU_CONTAINS_CONTROLS);
    const HWND popupHwnd = static_cast<HWND>(popup->GetHWND());
    REQUIRE(popupHwnd != nullptr);
    REQUIRE(popup->Show());
    REQUIRE(popup->wxWindowBase::Reparent(ownerB));

    gs_popupCancelOwnerDuringPublish = ownerB;
    gs_popupPublishOwnerCancelled = false;
    wxWinUISetPopupOwnerPublishHookForTesting(
        &CancelDestinationDuringPopupOwnerPublication);
    CHECK_FALSE(popup->Show());
    wxWinUIResetPopupOwnerPublishHookForTesting();
    gs_popupCancelOwnerDuringPublish = nullptr;

    CHECK(gs_popupPublishOwnerCancelled);
    REQUIRE(gs_popupPublishOwnerRetirement != nullptr);
    CHECK(static_cast<bool>(*gs_popupPublishOwnerRetirement));
    CHECK_FALSE(popup->IsShown());
    CHECK_FALSE(::IsWindowVisible(popupHwnd));
    CHECK(popup->MSWGetOwner() == ownerB);
    CHECK(::GetWindow(popupHwnd, GW_OWNER) ==
          static_cast<HWND>(ownerB->GetHWND()));
    CHECK(wxWinUITransientCountForTesting(ownerA) == 0);
    CHECK(wxWinUITransientCountForTesting(ownerB) == 0);

    gs_popupPublishOwnerRetirement.reset();
    REQUIRE(popup->Show());
    CHECK(popup->MSWGetOwner() == ownerB);
    CHECK(::GetWindow(popupHwnd, GW_OWNER) ==
          static_cast<HWND>(ownerB->GetHWND()));
    CHECK(wxWinUITransientCountForTesting(
              ownerB, wxWinUITransientKind::Popup) == 1);

    DestroyAndDrain(popup);
    ownerA->Destroy();
    ownerB->Destroy();
    REQUIRE(DrainTransientWindowsUntil([&]()
    {
        return SameLiveRegistry(
            wxWinUIGetTransientRegistrySnapshotForTesting(), registryBefore);
    }));
}

TEST_CASE("WinUITransientWindows::OutsideDismissCaptureAndReentrancy",
          "[winui-beta-transients][popup][outside-click][capture]")
{
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "transient-owner",
                    wxPoint(-30000, -30000), wxSize(240, 140));
    ShowOffscreenWithoutActivation(owner, 0);
    wxButton * const ownerFocus =
        new wxButton(owner, wxID_ANY, "owner-focus", wxPoint(5, 5),
                     wxSize(100, 30));
    ownerFocus->SetFocus();
    REQUIRE(wxWindow::FindFocus() == ownerFocus);

    TransientPopupProbe * const popup = new TransientPopupProbe(owner);
    popup->SetSize(wxRect(-29600, -30000, 190, 100));
    wxButton * const focus =
        new wxButton(popup, wxID_ANY, "focus", wxPoint(10, 10),
                     wxSize(90, 30));
    wxWindow * const recapture =
        new wxWindow(popup, wxID_ANY, wxPoint(110, 10), wxSize(40, 30));
    PopupCaptureReleaseScenario captureScenario;
    captureScenario.first = focus;
    captureScenario.second = recapture;
    PopupCaptureReleaseHookScope captureHook(captureScenario);

    popup->Popup(focus);
    REQUIRE(popup->IsShown());
    CHECK(wxWinUITransientCountForTesting(
              owner, wxWinUITransientKind::Popup) == 1);
    focus->CaptureMouse();
    REQUIRE(focus->HasCapture());

    popup->allowOutsideDismiss = false;
    wxWinUITestPopupOutsideClick(popup);
    CHECK(popup->IsShown());
    CHECK(focus->HasCapture());
    CHECK(popup->dismissNotifications == 0);

    popup->allowOutsideDismiss = true;
    popup->reopenDuringCanDismiss = true;
    wxWinUITestPopupOutsideClick(popup);
    CHECK(popup->IsShown());
    CHECK(popup->dismissNotifications == 0);
    CHECK(wxWinUITransientCountForTesting(
              owner, wxWinUITransientKind::Popup) == 1);

    wxWinUITestPopupOutsideClick(popup);
    CHECK_FALSE(popup->IsShown());
    CHECK_FALSE(focus->HasCapture());
    CHECK_FALSE(recapture->HasCapture());
    CHECK(captureScenario.invoked);
    CHECK(wxWindow::FindFocus() == ownerFocus);
    CHECK(popup->dismissNotifications == 1);
    CHECK(wxWinUITransientCountForTesting(owner) == 0);

    // A capture callback which starts a newer visibility generation wins over
    // the outer dismissal. The same visible popup is not hidden afterwards.
    captureScenario.invoked = false;
    captureScenario.reopenPopup = true;
    gs_popupCaptureReleaseScenario = &captureScenario;
    popup->Popup(focus);
    REQUIRE(popup->IsShown());
    focus->CaptureMouse();
    wxWinUITestPopupOutsideClick(popup);
    CHECK(popup->IsShown());
    CHECK(captureScenario.invoked);
    CHECK(popup->dismissNotifications == 1);

    // An explicit Dismiss() is intentionally not an outside-dismiss
    // notification.
    captureScenario.reopenPopup = false;
    captureHook.Reset();
    popup->Dismiss();
    CHECK_FALSE(popup->IsShown());
    CHECK(popup->dismissNotifications == 1);

    // A queued deactivation belongs to the exact visibility generation that
    // observed it; hiding and reopening before CallAfter drains invalidates it.
    popup->Popup(focus);
    WXLRESULT ignored = 0;
    (void)popup->MSWHandleMessage(
        &ignored, WM_ACTIVATE, MAKEWPARAM(WA_INACTIVE, FALSE), 0);
    REQUIRE(popup->Hide());
    popup->Popup(focus);
    DrainTransientWindows();
    CHECK(popup->IsShown());
    CHECK(popup->dismissNotifications == 1);
    popup->Dismiss();

    // Exercise the production WM_ACTIVATE path without SendInput.
    popup->Popup(focus);
    (void)popup->MSWHandleMessage(
        &ignored, WM_ACTIVATE, MAKEWPARAM(WA_INACTIVE, TRUE), 0);
    REQUIRE(DrainTransientWindowsUntil(
        [popup]() { return !popup->IsShown(); }));
    CHECK(popup->dismissNotifications == 2);
    CHECK(wxWinUITransientCountForTesting(owner) == 0);

    // OnDismiss() may reclaim the wrapper synchronously. The outside-click
    // tail must not read it or send owner state through a stale pointer.
    const auto finalNotifications = std::make_shared<unsigned>(0);
    popup->sharedDismissNotifications = finalNotifications;
    popup->deleteOnDismiss = true;
    popup->Popup(focus);
    const wxWeakRef<wxWindow> popupWeak(popup);
    wxWinUITestPopupOutsideClick(popup);
    REQUIRE(DrainTransientWindowsUntil([popupWeak]() { return !popupWeak; }));
    CHECK(*finalNotifications == 1);
    CHECK(wxWinUITransientCountForTesting(owner) == 0);

    owner->Destroy();
    DrainTransientWindows();
}

TEST_CASE("WinUITransientWindows::DestroyScheduledDismissSkipsNotification",
          "[winui-beta-transients][popup][destroy][reentrancy]")
{
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "dismiss-destroy-owner",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(owner, 0);

    TransientPopupProbe * const popup = new TransientPopupProbe(owner);
    const auto notifications = std::make_shared<unsigned>(0);
    popup->sharedDismissNotifications = notifications;
    popup->destroyDuringDismiss = true;
    popup->Popup();
    REQUIRE(popup->IsShown());

    const wxWeakRef<wxWindow> popupWeak(popup);
    wxWinUITestPopupOutsideClick(popup);

    // Dismiss() hid and scheduled destruction synchronously. The lifetime
    // guard must not enter OnDismiss() on this unavailable generation.
    CHECK(*notifications == 0);
    REQUIRE(DrainTransientWindowsUntil([popupWeak]() { return !popupWeak; }));
    CHECK(*notifications == 0);

    owner->Destroy();
    DrainTransientWindows();
}

TEST_CASE("WinUITransientWindows::CaptureHookDestroyInvalidatesDismiss",
          "[winui-beta-transients][popup][capture][destroy][reentrancy]")
{
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "capture-destroy-owner",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(owner, 0);

    TransientPopupProbe * const popup = new TransientPopupProbe(owner);
    wxWindow * const capture =
        new wxWindow(popup, wxID_ANY, wxPoint(5, 5), wxSize(40, 30));
    const auto notifications = std::make_shared<unsigned>(0);
    popup->sharedDismissNotifications = notifications;

    PopupCaptureReleaseScenario captureScenario;
    captureScenario.destroyPopup = true;
    PopupCaptureReleaseHookScope captureHook(captureScenario);
    popup->Popup();
    capture->CaptureMouse();
    REQUIRE(capture->HasCapture());

    const wxWeakRef<wxWindow> popupWeak(popup);
    wxWinUITestPopupOutsideClick(popup);
    CHECK(captureScenario.invoked);
    CHECK(*notifications == 0);
    REQUIRE(DrainTransientWindowsUntil([popupWeak]() { return !popupWeak; }));
    CHECK(*notifications == 0);
    CHECK(wxWinUITransientCountForTesting(owner) == 0);

    owner->Destroy();
    DrainTransientWindows();
}

TEST_CASE("WinUITransientWindows::ParentPendingDestroyOwnsChildTeardown",
          "[winui-beta-transients][popup][owner][destroy]")
{
    const wxWinUITransientRegistrySnapshot registryBefore =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "pending-parent-owner",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(owner, 0);
    TransientPopupProbe * const popup = new TransientPopupProbe(owner);
    new wxButton(popup, wxID_ANY, "child");
    popup->Popup();

    const wxWeakRef<wxWindow> ownerWeak(owner);
    const wxWeakRef<wxWindow> popupWeak(popup);
    REQUIRE(owner->Destroy());
    if ( wxWindow * const livePopup = popupWeak.get() )
    {
        // The owner is scheduled but deliberately still alive. The popup must
        // choose synchronous child teardown instead of entering a dangling
        // wxPendingDelete/private-queue state.
        REQUIRE(livePopup->Destroy());
    }

    REQUIRE(DrainTransientWindowsUntil([&]()
    {
        return !ownerWeak && !popupWeak &&
               SameLiveRegistry(
                   wxWinUIGetTransientRegistrySnapshotForTesting(),
                   registryBefore);
    }));
}

TEST_CASE("WinUITransientWindows::NestedAndOwnerTeardown",
          "[winui-beta-transients][popup][nested][owner]")
{
    const wxWinUITransientRegistrySnapshot registryBefore =
        wxWinUIGetTransientRegistrySnapshotForTesting();

    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "nested-popup-owner",
                    wxPoint(-30000, -30000), wxSize(240, 140));
    ShowOffscreenWithoutActivation(owner, 0);
    TransientPopupProbe * const outer = new TransientPopupProbe(owner);
    outer->SetSize(wxRect(-29600, -30000, 190, 100));
    new wxButton(outer, wxID_ANY, "outer");
    TransientPopupProbe * const inner = new TransientPopupProbe(outer);
    inner->SetSize(wxRect(-29300, -30000, 170, 90));
    new wxButton(inner, wxID_ANY, "inner");

    outer->Popup();
    inner->Popup();
    REQUIRE(outer->IsShown());
    REQUIRE(inner->IsShown());
    CHECK(wxWinUITransientCountForTesting(
              owner, wxWinUITransientKind::Popup) == 1);
    CHECK(wxWinUITransientCountForTesting(
              outer, wxWinUITransientKind::Popup) == 1);

    // Hiding the outer generation cancels the nested one first. The owner's
    // retirement lease remains published across OnDismiss(), so an arbitrary
    // callback cannot resurrect a newer child generation.
    inner->reopenOnDismiss = true;
    REQUIRE(outer->Hide());
    CHECK_FALSE(outer->IsShown());
    CHECK_FALSE(inner->IsShown());
    CHECK(inner->dismissNotifications == 1);
    CHECK_FALSE(inner->IsShown());
    CHECK(wxWinUITransientCountForTesting(owner) == 0);
    CHECK(wxWinUITransientCountForTesting(outer) == 0);

    outer->Popup();
    inner->Popup();
    const wxWeakRef<wxWindow> ownerWeak(owner);
    const wxWeakRef<wxWindow> outerWeak(outer);
    const wxWeakRef<wxWindow> innerWeak(inner);
    owner->Destroy();

    REQUIRE(DrainTransientWindowsUntil([&]()
    {
        return !ownerWeak &&
               SameLiveRegistry(
                   wxWinUIGetTransientRegistrySnapshotForTesting(),
                   registryBefore);
    }));

    // Native owner destruction is allowed to destroy owned popups outright;
    // if wx keeps either wrapper alive, the manager must at least have made
    // it non-visible and ownerless for the remainder of its safe teardown.
    if ( wxWindow * const liveInner = innerWeak.get() )
    {
        CHECK_FALSE(liveInner->IsShown());
        DestroyAndDrain(liveInner);
    }
    if ( wxWindow * const liveOuter = outerWeak.get() )
    {
        CHECK_FALSE(liveOuter->IsShown());
        DestroyAndDrain(liveOuter);
    }
}

TEST_CASE("WinUITransientWindows::NestedDesktopChildOutsideRouting",
          "[winui-beta-transients][popup][desktop-child][nested]")
{
    const wxWinUITransientRegistrySnapshot registryBefore =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "desktop-nested-owner",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(owner, 0);

    TransientPopupProbe * const outer =
        new TransientPopupProbe(owner, wxBORDER_NONE);
    TransientPopupProbe * const inner =
        new TransientPopupProbe(outer, wxBORDER_NONE);
    outer->SetSize(wxRect(-29600, -30000, 170, 90));
    inner->SetSize(wxRect(-29300, -30000, 150, 80));
    outer->Popup();
    inner->Popup();
    REQUIRE(outer->IsShown());
    REQUIRE(inner->IsShown());

    // Closing the most recent no-focus popup restores the outer popup as the
    // global MSW outside-click target instead of clearing the singleton.
    inner->MSWDismissUnfocusedPopup();
    CHECK_FALSE(inner->IsShown());
    CHECK(outer->IsShown());
    CHECK(inner->dismissNotifications == 1);
    outer->MSWDismissUnfocusedPopup();
    CHECK_FALSE(outer->IsShown());
    CHECK(outer->dismissNotifications == 1);

    DestroyAndDrain(inner);
    DestroyAndDrain(outer);
    owner->Destroy();
    REQUIRE(DrainTransientWindowsUntil([&]()
    {
        return SameLiveRegistry(
            wxWinUIGetTransientRegistrySnapshotForTesting(),
            registryBefore);
    }));
}

TEST_CASE("WinUITransientWindows::PersistentRecaptureIsNotFalseDismissal",
          "[winui-beta-transients][popup][capture][reentrancy]")
{
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "recapture-owner",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(owner, 0);
    TransientPopupProbe * const popup = new TransientPopupProbe(owner);
    wxWindow * const captureA =
        new wxWindow(popup, wxID_ANY, wxPoint(5, 5), wxSize(30, 30));
    wxWindow * const captureB =
        new wxWindow(popup, wxID_ANY, wxPoint(45, 5), wxSize(30, 30));
    PopupCaptureReleaseScenario captureScenario;
    captureScenario.first = captureA;
    captureScenario.second = captureB;
    captureScenario.persistent = true;
    PopupCaptureReleaseHookScope captureHook(captureScenario);

    popup->Popup();
    captureA->CaptureMouse();
    wxWinUITestPopupOutsideClick(popup);
    CHECK(popup->IsShown());
    CHECK(popup->dismissNotifications == 0);
    CHECK(wxWinUITransientCountForTesting(
              owner, wxWinUITransientKind::Popup) == 1);

    CHECK(captureScenario.invoked);
    captureHook.Reset();
    if ( wxWindow * const capture = wxWindow::GetCapture() )
        capture->ReleaseMouse();
    wxWinUITestPopupOutsideClick(popup);
    CHECK_FALSE(popup->IsShown());
    CHECK(popup->dismissNotifications == 1);

    DestroyAndDrain(popup);
    owner->Destroy();
    DrainTransientWindows();
}

TEST_CASE("WinUITransientWindows::OwnerCancelBlocksCaptureLostRecapture",
          "[winui-beta-transients][popup][capture][owner][reentrancy]")
{
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "terminal-capture-owner",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(owner, 0);
    TransientPopupProbe * const popup = new TransientPopupProbe(owner);
    wxWindow * const captureA =
        new wxWindow(popup, wxID_ANY, wxPoint(5, 5), wxSize(30, 30));
    wxWindow * const captureB =
        new wxWindow(popup, wxID_ANY, wxPoint(45, 5), wxSize(30, 30));
    unsigned captureLostCalls = 0;
    const auto recaptureFromEvent =
        [&captureLostCalls](wxMouseCaptureLostEvent& event)
        {
            ++captureLostCalls;
            wxWindow * const target =
                wxDynamicCast(event.GetEventObject(), wxWindow);
            if ( target && !target->HasCapture() )
                target->CaptureMouse();
        };
    captureA->Bind(wxEVT_MOUSE_CAPTURE_LOST, recaptureFromEvent);
    captureB->Bind(wxEVT_MOUSE_CAPTURE_LOST, recaptureFromEvent);

    PopupCaptureReleaseScenario captureScenario;
    captureScenario.first = captureA;
    captureScenario.second = captureB;
    captureScenario.persistent = true;
    captureScenario.dispatchCaptureLostEvent = true;
    PopupCaptureReleaseHookScope captureHook(captureScenario);

    popup->Popup();
    captureA->CaptureMouse();
    REQUIRE(captureA->HasCapture());
    wxWinUICancelTransientSessions(owner);

    CHECK(captureScenario.invoked);
    CHECK(captureLostCalls >= 1);
    CHECK_FALSE(popup->IsShown());
    CHECK_FALSE(::IsWindowVisible(
        static_cast<HWND>(popup->GetHWND())));
    CHECK(wxWindow::GetCapture() == nullptr);
    CHECK(popup->dismissNotifications == 1);
    CHECK(wxWinUITransientCountForTesting(owner) == 0);

    captureHook.Reset();
    DestroyAndDrain(popup);
    owner->Destroy();
    DrainTransientWindows();
}

TEST_CASE("WinUITransientWindows::NativeRecaptureIsBoundedAndTerminal",
          "[winui-beta-transients][popup][capture][owner][failure]")
{
    const wxWinUITransientRegistrySnapshot registryBefore =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "native-recapture-owner",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(owner, 0);
    TransientPopupProbe * const popup = new TransientPopupProbe(owner);
    wxWindow * const captureA =
        new wxWindow(popup, wxID_ANY, wxPoint(5, 5), wxSize(30, 30));
    wxWindow * const captureB =
        new wxWindow(popup, wxID_ANY, wxPoint(45, 5), wxSize(30, 30));
    const wxWeakRef<wxWindow> popupWeak(popup);
    const HWND popupHwnd = static_cast<HWND>(popup->GetHWND());
    const auto dismissNotifications = std::make_shared<unsigned>(0);
    popup->sharedDismissNotifications = dismissNotifications;

    PopupCaptureReleaseScenario captureScenario;
    captureScenario.first = captureA;
    captureScenario.second = captureB;
    captureScenario.persistent = true;
    captureScenario.nativeRecapture = true;
    PopupCaptureReleaseHookScope captureHook(captureScenario);

    popup->Popup();
    captureA->CaptureMouse();
    REQUIRE(captureA->HasCapture());
    wxWinUICancelTransientSessions(owner);

    // Dismiss() has a 16-pass cooperative bound and the authoritative owner
    // cancellation adds exactly 32 terminal passes before hiding/destroying.
    CHECK(captureScenario.invocations == 48);
    CHECK(*dismissNotifications == 1);
    CHECK(wxWinUITransientCountForTesting(owner) == 0);
    CHECK((!::IsWindow(popupHwnd) || !::IsWindowVisible(popupHwnd)));

    captureHook.Reset();
    if ( wxWindow * const live = popupWeak.get() )
    {
        if ( !wxWinUITLWHostIsDestroyScheduled(live) )
            (void)live->Destroy();
    }
    REQUIRE(DrainTransientWindowsUntil([popupWeak]() { return !popupWeak; }));
    CHECK(wxWindow::GetCapture() == nullptr);
    owner->Destroy();
    REQUIRE(DrainTransientWindowsUntil([&]()
    {
        return SameLiveRegistry(
            wxWinUIGetTransientRegistrySnapshotForTesting(),
            registryBefore);
    }));
}

TEST_CASE("WinUITransientWindows::PopupBridgeNativeCaptureIsReleased",
          "[winui-beta-transients][popup][capture][owner][HostLifecycle]")
{
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "bridge-capture-owner",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(owner, 0);
    TransientPopupProbe * const popup = new TransientPopupProbe(owner);
    new wxButton(popup, wxID_ANY, "xaml-slot",
                 wxPoint(5, 5), wxSize(90, 30));
    popup->Popup();
    REQUIRE(popup->IsShown());

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindForTLW(popup);
    REQUIRE(host != nullptr);
    const HWND bridge = host->GetBridgeHwnd();
    REQUIRE(bridge != nullptr);
    REQUIRE(::IsChild(static_cast<HWND>(popup->GetHWND()), bridge));

    (void)::SetCapture(bridge);
    REQUIRE(::GetCapture() == bridge);
    CHECK(wxWindow::GetCapture() == nullptr);
    wxWinUICancelTransientSessions(owner);

    CHECK_FALSE(popup->IsShown());
    CHECK_FALSE(::IsWindowVisible(
        static_cast<HWND>(popup->GetHWND())));
    CHECK(::GetCapture() != bridge);
    CHECK(wxWindow::GetCapture() == nullptr);
    CHECK(wxWinUITransientCountForTesting(owner) == 0);

    DestroyAndDrain(popup);
    owner->Destroy();
    DrainTransientWindows();
}

TEST_CASE("WinUITransientWindows::ForeignNativeCaptureIsNotStolen",
          "[winui-beta-transients][popup][capture][owner]")
{
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "foreign-capture-owner",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    wxFrame * const foreign =
        new wxFrame(nullptr, wxID_ANY, "foreign-capture-authority",
                    wxPoint(-29600, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(owner, 0);
    ShowOffscreenWithoutActivation(foreign, 1);
    TransientPopupProbe * const popup = new TransientPopupProbe(owner);
    popup->Popup();
    REQUIRE(popup->IsShown());

    const HWND foreignHwnd = static_cast<HWND>(foreign->GetHWND());
    REQUIRE(foreignHwnd != nullptr);
    (void)::SetCapture(foreignHwnd);
    REQUIRE(::GetCapture() == foreignHwnd);
    CHECK(wxWindow::GetCapture() == foreign);

    wxWinUICancelTransientSessions(owner);
    CHECK_FALSE(popup->IsShown());
    CHECK_FALSE(::IsWindowVisible(
        static_cast<HWND>(popup->GetHWND())));
    CHECK(::GetCapture() == foreignHwnd);
    CHECK(wxWinUITransientCountForTesting(owner) == 0);

    if ( ::GetCapture() == foreignHwnd )
        (void)::ReleaseCapture();
    DestroyAndDrain(popup);
    owner->Destroy();
    foreign->Destroy();
    DrainTransientWindows();
}

TEST_CASE("WinUITransientWindows::RawWxHwndCaptureUsesNativeRelease",
          "[winui-beta-transients][popup][capture][owner][failure]")
{
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "raw-wx-capture-owner",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(owner, 0);
    TransientPopupProbe * const popup = new TransientPopupProbe(owner);
    wxWindow * const child =
        new wxWindow(popup, wxID_ANY, wxPoint(5, 5), wxSize(40, 30));
    popup->Popup();
    REQUIRE(popup->IsShown());

    const HWND childHwnd = static_cast<HWND>(child->GetHWND());
    REQUIRE(childHwnd != nullptr);
    (void)::SetCapture(childHwnd);
    REQUIRE(::GetCapture() == childHwnd);
    // GetCapture() maps the raw HWND back to wxWindow even though no
    // CaptureMouse() call pushed it on wxMouseCapture::stack. Production must
    // not call ReleaseMouse() and assert/pop an empty stack in this case.
    CHECK(wxWindow::GetCapture() == child);

    wxWinUICancelTransientSessions(owner);
    CHECK_FALSE(popup->IsShown());
    CHECK(::GetCapture() != childHwnd);
    CHECK(wxWinUITransientCountForTesting(owner) == 0);

    DestroyAndDrain(popup);
    owner->Destroy();
    DrainTransientWindows();
}

TEST_CASE("WinUITransientWindows::PlainPopupHostileHideIsTerminal",
          "[winui-beta-transients][popup][owner][reentrancy]")
{
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "hostile-hide-owner",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(owner, 0);
    HostilePlainPopupProbe * const popup =
        new HostilePlainPopupProbe(owner);
    popup->SetSize(wxRect(-29600, -30000, 160, 80));
    REQUIRE(popup->Show());
    popup->refuseVirtualHide = true;

    wxWinUICancelTransientSessions(owner);
    CHECK(popup->refusedHideCalls == 1);
    CHECK_FALSE(popup->IsShown());
    CHECK_FALSE(::IsWindowVisible(
        static_cast<HWND>(popup->GetHWND())));
    CHECK(wxWinUITransientCountForTesting(owner) == 0);

    // Terminal applies to the cancelled generation, not the reusable object.
    REQUIRE(popup->Show());
    popup->refuseVirtualHide = false;
    DestroyAndDrain(popup);
    owner->Destroy();
    DrainTransientWindows();
}

TEST_CASE("WinUITransientWindows::PlainPopupRawReshowIsForcedHidden",
          "[winui-beta-transients][popup][owner][reentrancy][native]")
{
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "raw-reshow-plain-owner",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(owner, 0);
    HostilePlainPopupProbe * const popup =
        new HostilePlainPopupProbe(owner);
    popup->SetSize(wxRect(-29600, -30000, 160, 80));
    const HWND popupHwnd = static_cast<HWND>(popup->GetHWND());
    REQUIRE(popupHwnd != nullptr);
    REQUIRE(popup->Show());
    popup->rawReshowAfterBaseHide = true;

    wxWinUICancelTransientSessions(owner);
    CHECK_FALSE(popup->IsShown());
    CHECK_FALSE(::IsWindowVisible(popupHwnd));
    CHECK(wxWinUITransientCountForTesting(owner) == 0);

    popup->rawReshowAfterBaseHide = false;
    REQUIRE(popup->Show());
    DestroyAndDrain(popup);
    owner->Destroy();
    DrainTransientWindows();
}

TEST_CASE("WinUITransientWindows::OnDismissRawReshowIsForcedHidden",
          "[winui-beta-transients][popup][owner][reentrancy][native]")
{
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "raw-reshow-dismiss-owner",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(owner, 0);
    TransientPopupProbe * const popup = new TransientPopupProbe(owner);
    const HWND popupHwnd = static_cast<HWND>(popup->GetHWND());
    REQUIRE(popupHwnd != nullptr);
    popup->rawReshowOnDismiss = true;
    popup->Popup();
    REQUIRE(popup->IsShown());

    wxWinUICancelTransientSessions(owner);
    CHECK(popup->dismissNotifications == 1);
    CHECK_FALSE(popup->IsShown());
    CHECK_FALSE(::IsWindowVisible(popupHwnd));
    CHECK(wxWinUITransientCountForTesting(owner) == 0);

    popup->rawReshowOnDismiss = false;
    REQUIRE(popup->Show());
    DestroyAndDrain(popup);
    owner->Destroy();
    DrainTransientWindows();
}

TEST_CASE("WinUITransientWindows::PersistentNativeReshowForcesDestroy",
          "[winui-beta-transients][popup][owner][reentrancy][native][failure]")
{
    const wxWinUITransientRegistrySnapshot registryBefore =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "persistent-reshow-owner",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(owner, 0);
    wxPopupWindow * const popup =
        new wxPopupWindow(owner, wxPU_CONTAINS_CONTROLS);
    const wxWeakRef<wxWindow> popupWeak(popup);
    const HWND popupHwnd = static_cast<HWND>(popup->GetHWND());
    REQUIRE(popupHwnd != nullptr);
    const unsigned long long popupGeneration =
        wxWinUIMSWGetNativeHwndGeneration(
            reinterpret_cast<WXHWND>(popupHwnd));
    REQUIRE(popupGeneration != 0);
    REQUIRE(popup->Show());

    PersistentPopupNativeReshowState reshowState;
    const UINT_PTR subclassId =
        reinterpret_cast<UINT_PTR>(&reshowState);
    REQUIRE(::SetWindowSubclass(
                popupHwnd, PersistentPopupNativeReshowSubclass,
                subclassId, reinterpret_cast<DWORD_PTR>(&reshowState)) !=
            FALSE);

    wxWinUICancelTransientSessions(owner);
    CHECK(reshowState.reshowCount >= 2);
    CHECK(wxWinUITransientCountForTesting(owner) == 0);

    // A subclass which defeats every SW_HIDE makes native visibility an
    // unsatisfied terminal postcondition. The manager must therefore destroy
    // this exact popup generation, not return a visible registry-less shell.
    const bool retired = DrainTransientWindowsUntil(
        [popupWeak]() { return !popupWeak; });
    CHECK(retired);
    const bool nativeRetired =
        !::IsWindow(popupHwnd) ||
        wxWinUIMSWGetNativeHwndGeneration(
            reinterpret_cast<WXHWND>(popupHwnd)) != popupGeneration;
    CHECK(nativeRetired);

    // Keep the stack-backed subclass context valid even if either independent
    // terminal postcondition fails: detach it from the exact native generation
    // before this scope returns, then finish the ordinary wx destruction path.
    if ( !nativeRetired )
    {
        reshowState.enabled = false;
        (void)::RemoveWindowSubclass(
            popupHwnd, PersistentPopupNativeReshowSubclass, subclassId);
    }
    if ( !retired )
    {
        if ( wxWindow * const live = popupWeak.get() )
        {
            if ( !wxWinUITLWHostIsDestroyScheduled(live) )
                (void)live->Destroy();
        }
        (void)DrainTransientWindowsUntil([popupWeak]() { return !popupWeak; });
    }

    owner->Destroy();
    REQUIRE(DrainTransientWindowsUntil([&]()
    {
        return SameLiveRegistry(
            wxWinUIGetTransientRegistrySnapshotForTesting(), registryBefore);
    }));
}

TEST_CASE("WinUITransientWindows::PopupAllocationFailureRollsBack",
          "[winui-beta-transients][popup][allocation][failure][registry]")
{
    const wxWinUITransientRegistrySnapshot registryBefore =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "allocation-owner",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(owner, 0);
    wxPopupWindow * const popup =
        new wxPopupWindow(owner, wxPU_CONTAINS_CONTROLS);
    const wxWeakRef<wxWindow> popupWeak(popup);
    const HWND popupHwnd = static_cast<HWND>(popup->GetHWND());
    REQUIRE(popupHwnd != nullptr);

    // The historical fail-next seam now starts at the cancel-token allocation.
    // No manager or sidecar state exists yet and the hidden object is reusable.
    wxWinUIFailNextPopupAllocationForTesting();
    REQUIRE_THROWS_AS(popup->Show(), std::bad_alloc);
    CHECK_FALSE(popup->IsShown());
    CHECK_FALSE(::IsWindowVisible(popupHwnd));
    CHECK(wxWinUITransientCountForTesting(owner) == 0);
    REQUIRE(popup->Show());
    CHECK(wxWinUITransientCountForTesting(
              owner, wxWinUITransientKind::Popup) == 1);

    // Ending-session allocation failure must roll back endingSession and the
    // temporary owner retirement, retaining the visible manager generation.
    wxWinUIFailNextPopupAllocationForTesting();
    REQUIRE_THROWS_AS(popup->Hide(), std::bad_alloc);
    CHECK(popup->IsShown());
    CHECK(::IsWindowVisible(popupHwnd));
    CHECK(wxWinUITransientCountForTesting(
              owner, wxWinUITransientKind::Popup) == 1);

    // Destroy preparation has the same strong rollback contract: no terminal
    // flags or retirement lease survive a failed ownership allocation.
    wxWinUIFailNextPopupAllocationForTesting();
    REQUIRE_THROWS_AS(popup->Destroy(), std::bad_alloc);
    CHECK(popupWeak.get() == popup);
    CHECK_FALSE(wxWinUITLWHostIsDestroyScheduled(popup));
    CHECK(popup->IsShown());
    CHECK(wxWinUITransientCountForTesting(
              owner, wxWinUITransientKind::Popup) == 1);

    REQUIRE(popup->Destroy());
    REQUIRE(DrainTransientWindowsUntil([popupWeak]() { return !popupWeak; }));
    owner->Destroy();
    REQUIRE(DrainTransientWindowsUntil([&]()
    {
        return SameLiveRegistry(
            wxWinUIGetTransientRegistrySnapshotForTesting(), registryBefore);
    }));
}

TEST_CASE("WinUITransientWindows::VisibleMigrationAllocationMatrix",
          "[winui-beta-transients][popup][allocation][failure][registry][reparent]")
{
    struct AllocationCase
    {
        wxWinUITransientAllocationSite site;
        const char *name;
    };
    const AllocationCase cases[] = {
        { wxWinUITransientAllocationSite::PopupCancelToken,
          "cancel token" },
        { wxWinUITransientAllocationSite::RegistrationImpl,
          "registration impl" },
        { wxWinUITransientAllocationSite::RegistrationOwnerEntry,
          "owner map entry" },
        { wxWinUITransientAllocationSite::RegistrationObserver,
          "owner observer" },
        { wxWinUITransientAllocationSite::RegistrationEntry,
          "transient vector entry" },
        { wxWinUITransientAllocationSite::PopupRegistrationHolder,
          "popup registration holder" }
    };

    const wxWinUITransientRegistrySnapshot registryBefore =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    wxFrame * const ownerA =
        new wxFrame(nullptr, wxID_ANY, "allocation-migration-a",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    wxFrame * const ownerB =
        new wxFrame(nullptr, wxID_ANY, "allocation-migration-b",
                    wxPoint(-29600, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(ownerA, 0);
    ShowOffscreenWithoutActivation(ownerB, 1);

    for ( const AllocationCase& allocationCase : cases )
    {
        INFO("allocation site: " << allocationCase.name);
        wxPopupWindow * const popup =
            new wxPopupWindow(ownerA, wxPU_CONTAINS_CONTROLS);
        const HWND popupHwnd = static_cast<HWND>(popup->GetHWND());
        REQUIRE(popupHwnd != nullptr);
        REQUIRE(popup->Show());
        REQUIRE(popup->wxWindowBase::Reparent(ownerB));

        wxWinUIFailTransientAllocationAtForTesting(allocationCase.site);
        bool sawBadAlloc = false;
        try
        {
            (void)popup->Show();
        }
        catch ( const std::bad_alloc& )
        {
            sawBadAlloc = true;
        }
        catch ( ... )
        {
        }
        wxWinUIResetTransientAllocationFailureForTesting();

        CHECK(sawBadAlloc);
        CHECK(popup->GetParent() == ownerB);
        CHECK_FALSE(popup->IsShown());
        CHECK_FALSE(::IsWindowVisible(popupHwnd));
        CHECK(wxWinUITransientCountForTesting(ownerA) == 0);
        CHECK(wxWinUITransientCountForTesting(ownerB) == 0);

        // The failed A -> B transaction retires A instead of leaving a logical
        // B popup backed by A's manager token. A clean retry must register B.
        REQUIRE(popup->Show());
        CHECK(popup->MSWGetOwner() == ownerB);
        CHECK(::GetWindow(popupHwnd, GW_OWNER) ==
              static_cast<HWND>(ownerB->GetHWND()));
        CHECK(wxWinUITransientCountForTesting(
                  ownerB, wxWinUITransientKind::Popup) == 1);
        DestroyAndDrain(popup);
        CHECK(wxWinUITransientCountForTesting(ownerA) == 0);
        CHECK(wxWinUITransientCountForTesting(ownerB) == 0);
    }

    ownerA->Destroy();
    ownerB->Destroy();
    REQUIRE(DrainTransientWindowsUntil([&]()
    {
        return SameLiveRegistry(
            wxWinUIGetTransientRegistrySnapshotForTesting(), registryBefore);
    }));
}

TEST_CASE("WinUITransientWindows::OwnerRetirementRejectsEveryKind",
          "[winui-beta-transients][popup][registry][reentrancy]")
{
    const wxWinUITransientRegistrySnapshot registryBefore =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "retirement-owner",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(owner, 0);

    bool popupReplacement = false;
    bool tipReplacement = false;
    bool modalReplacement = false;
    auto popup = wxWinUIRegisterTransient(
        owner, wxWinUITransientKind::Popup,
        [&]()
        {
            popupReplacement = static_cast<bool>(wxWinUIRegisterTransient(
                owner, wxWinUITransientKind::Popup, []() {}));
        });
    auto tip = wxWinUIRegisterTransient(
        owner, wxWinUITransientKind::TeachingTip,
        [&]()
        {
            tipReplacement = static_cast<bool>(wxWinUIRegisterTransient(
                owner, wxWinUITransientKind::TeachingTip, []() {}));
        });
    auto modal = wxWinUIRegisterTransient(
        owner, wxWinUITransientKind::ModalDialog,
        [&]()
        {
            modalReplacement = static_cast<bool>(wxWinUIRegisterTransient(
                owner, wxWinUITransientKind::ModalDialog, []() {}));
        });
    REQUIRE(popup);
    REQUIRE(tip);
    REQUIRE(modal);

    auto retirement = wxWinUIBeginTransientOwnerRetirement(owner);
    REQUIRE(retirement);
    CHECK(wxWinUIGetTransientRegistrySnapshotForTesting()
              .activeOwnerRetirementCount ==
          registryBefore.activeOwnerRetirementCount + 1);
    wxWinUICancelTransientSessions(owner);
    CHECK_FALSE(popupReplacement);
    CHECK_FALSE(tipReplacement);
    CHECK_FALSE(modalReplacement);
    CHECK(wxWinUITransientCountForTesting(owner) == 0);

    retirement.Reset();
    popup = wxWinUIRegisterTransient(
        owner, wxWinUITransientKind::Popup, []() {});
    tip = wxWinUIRegisterTransient(
        owner, wxWinUITransientKind::TeachingTip, []() {});
    modal = wxWinUIRegisterTransient(
        owner, wxWinUITransientKind::ModalDialog, []() {});
    REQUIRE(popup);
    REQUIRE(tip);
    REQUIRE(modal);
    popup.Reset();
    tip.Reset();
    modal.Reset();

    owner->Destroy();
    REQUIRE(DrainTransientWindowsUntil([&]()
    {
        return SameLiveRegistry(
            wxWinUIGetTransientRegistrySnapshotForTesting(),
            registryBefore);
    }));
}

TEST_CASE("WinUITransientWindows::DefaultDesktopChildPopup",
          "[winui-beta-transients][popup][desktop-child]")
{
    const wxWinUITransientRegistrySnapshot registryBefore =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    wxFrame * const ownerA =
        new wxFrame(nullptr, wxID_ANY, "default-popup-a",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    wxFrame * const ownerB =
        new wxFrame(nullptr, wxID_ANY, "default-popup-b",
                    wxPoint(-29600, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(ownerA, 0);
    ShowOffscreenWithoutActivation(ownerB, 1);
    wxButton * const ownerFocus =
        new wxButton(ownerA, wxID_ANY, "desktop-owner-focus",
                     wxPoint(5, 5), wxSize(120, 30));
    ownerFocus->SetFocus();
    REQUIRE(wxWindow::FindFocus() == ownerFocus);

    TransientPopupProbe * const popup =
        new TransientPopupProbe(ownerA, wxBORDER_NONE);
    popup->SetSize(wxRect(-29200, -30000, 160, 80));
    const HWND hwnd = static_cast<HWND>(popup->GetHWND());
    REQUIRE(hwnd != nullptr);
    ::SetWindowLongPtr(
        hwnd, GWL_EXSTYLE,
        ::GetWindowLongPtr(hwnd, GWL_EXSTYLE) | WS_EX_NOACTIVATE);
    CHECK((::GetWindowLongPtr(hwnd, GWL_STYLE) & WS_CHILD) != 0);
    CHECK(::GetParent(hwnd) == ::GetDesktopWindow());

    popup->Popup();
    REQUIRE(popup->IsShown());
    CHECK_FALSE(popup->Show());
    CHECK(wxWinUITransientCountForTesting(
              ownerA, wxWinUITransientKind::Popup) == 1);
    REQUIRE(popup->Reparent(ownerB));
    CHECK(popup->IsShown());
    CHECK(popup->GetParent() == ownerB);
    CHECK(::GetParent(hwnd) == ::GetDesktopWindow());
    CHECK(wxWinUITransientCountForTesting(ownerA) == 0);
    CHECK(wxWinUITransientCountForTesting(
              ownerB, wxWinUITransientKind::Popup) == 1);

    popup->MSWDismissUnfocusedPopup();
    CHECK_FALSE(popup->IsShown());
    CHECK(popup->dismissNotifications == 1);
    CHECK(wxWindow::FindFocus() == ownerFocus);

    DestroyAndDrain(popup);
    ownerA->Destroy();
    ownerB->Destroy();
    REQUIRE(DrainTransientWindowsUntil([&]()
    {
        return SameLiveRegistry(
            wxWinUIGetTransientRegistrySnapshotForTesting(),
            registryBefore);
    }));
}

TEST_CASE("WinUITransientWindows::OwnerDestroyDismissesExactlyOnce",
          "[winui-beta-transients][popup][owner][capture][reentrancy]")
{
    const wxWinUITransientRegistrySnapshot registryBefore =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "destroy-owner",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(owner, 0);
    TransientPopupProbe * const popup = new TransientPopupProbe(owner);
    wxWindow * const captureA =
        new wxWindow(popup, wxID_ANY, wxPoint(5, 5), wxSize(40, 30));
    wxWindow * const captureB =
        new wxWindow(popup, wxID_ANY, wxPoint(50, 5), wxSize(40, 30));
    PopupCaptureReleaseScenario captureScenario;
    captureScenario.first = captureA;
    captureScenario.second = captureB;
    captureScenario.persistent = true;
    PopupCaptureReleaseHookScope captureHook(captureScenario);
    const auto notifications = std::make_shared<unsigned>(0);
    popup->sharedDismissNotifications = notifications;
    popup->destroyOnDismiss = true;
    popup->Popup();
    captureA->CaptureMouse();
    REQUIRE(captureA->HasCapture());

    const wxWeakRef<wxWindow> ownerWeak(owner);
    const wxWeakRef<wxWindow> popupWeak(popup);
    owner->Destroy();
    REQUIRE(DrainTransientWindowsUntil([&]()
    {
        return !ownerWeak && !popupWeak &&
               SameLiveRegistry(
                   wxWinUIGetTransientRegistrySnapshotForTesting(),
                   registryBefore);
    }));
    CHECK(*notifications == 1);
    CHECK(wxWindow::GetCapture() == nullptr);
}

TEST_CASE("WinUITransientWindows::PopupHundredCycles",
          "[winui-beta-transients][popup][stress]")
{
    const HostPhysicalSnapshot physicalBefore;
    const wxWinUITransientRegistrySnapshot registryBefore =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "popup-cycle-owner",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(owner, 0);
    const HostPhysicalSnapshot withOwner;

    for ( unsigned cycle = 0; cycle < 100; ++cycle )
    {
        INFO("popup cycle " << cycle);
        wxPopupWindow * const popup =
            new wxPopupWindow(owner, wxPU_CONTAINS_CONTROLS);
        new wxButton(popup, wxID_ANY, "slot");
        REQUIRE(popup->Show());
        REQUIRE(popup->Hide());
        const wxWeakRef<wxWindow> popupWeak(popup);
        REQUIRE(popup->Destroy());
        REQUIRE(DrainTransientWindowsUntil([&]()
        {
            return !popupWeak && SameLiveHostObjects(withOwner) &&
                   wxWinUITransientCountForTesting(owner) == 0;
        }));
    }

    owner->Destroy();
    REQUIRE(DrainTransientWindowsUntil([&]()
    {
        return SameLiveHostObjects(physicalBefore) &&
               SameLiveRegistry(
                   wxWinUIGetTransientRegistrySnapshotForTesting(),
                   registryBefore);
    }));
    CheckBalancedHandlers(physicalBefore);
}

#endif // wxUSE_POPUPWIN

#if wxUSE_MINIFRAME

TEST_CASE("WinUITransientWindows::MiniFrameLifecycleAndHost",
          "[winui-beta-transients][miniframe][HostLifecycle]")
{
    const HostPhysicalSnapshot physicalBefore;
    wxFrame * const ownerA =
        new wxFrame(nullptr, wxID_ANY, "mini-owner-a",
                    wxPoint(-30000, -30000), wxSize(260, 160));
    wxFrame * const ownerB =
        new wxFrame(nullptr, wxID_ANY, "mini-owner-b",
                    wxPoint(-29600, -30000), wxSize(260, 160));
    ShowOffscreenWithoutActivation(ownerA, 0);
    ShowOffscreenWithoutActivation(ownerB, 1);

    wxMiniFrame * const mini = new wxMiniFrame;
    REQUIRE(mini->Create(ownerA, wxID_ANY, "mini",
                         wxPoint(-29200, -30000), wxSize(220, 120)));
    CHECK(mini->GetParent() == ownerA);
    CHECK(mini->HasFlag(wxFRAME_TOOL_WINDOW));
    CHECK(mini->HasFlag(wxFRAME_FLOAT_ON_PARENT));
    CHECK(::GetWindow(static_cast<HWND>(mini->GetHWND()), GW_OWNER) ==
          static_cast<HWND>(ownerA->GetHWND()));

    wxWinUIMSWFailNextOwnerWriteForTest();
    CHECK_FALSE(mini->Reparent(ownerB));
    CHECK(mini->GetParent() == ownerA);
    CHECK(::GetWindow(static_cast<HWND>(mini->GetHWND()), GW_OWNER) ==
          static_cast<HWND>(ownerA->GetHWND()));

    wxWindow * const gdi =
        new wxWindow(mini, wxID_ANY, wxPoint(5, 5), wxSize(60, 25));
    wxButton * const xaml =
        new wxButton(mini, wxID_ANY, "slot", wxPoint(70, 5),
                     wxSize(80, 30));
    wxWinUITopLevelHost * const miniHost =
        wxWinUITopLevelHost::FindForTLW(mini);
    REQUIRE(miniHost != nullptr);
    CHECK(wxWinUITopLevelHost::FindSlotOwner(xaml) == miniHost);
    CHECK(wxWinUITopLevelHost::FindSlotOwner(gdi) == nullptr);

    mini->ShowWithoutActivating();
    CHECK(mini->IsShown());

    bool vetoClose = true;
    unsigned closeEvents = 0;
    mini->Bind(
        wxEVT_CLOSE_WINDOW,
        [&](wxCloseEvent& event)
        {
            ++closeEvents;
            if ( vetoClose )
                event.Veto();
            else
                event.Skip();
        });
    mini->Close();
    CHECK(mini->IsShown());
    CHECK(closeEvents == 1);

    REQUIRE(mini->Reparent(ownerB));
    CHECK(mini->GetParent() == ownerB);
    CHECK(::GetWindow(static_cast<HWND>(mini->GetHWND()), GW_OWNER) ==
          static_cast<HWND>(ownerB->GetHWND()));

    REQUIRE(xaml->Reparent(ownerB));
    wxWinUITopLevelHost * const ownerBHost =
        wxWinUITopLevelHost::FindSlotOwner(xaml);
    REQUIRE(ownerBHost != nullptr);
    CHECK(ownerBHost == wxWinUITopLevelHost::FindForTLW(ownerB));
    CHECK(ownerBHost != miniHost);
    REQUIRE(gdi->Reparent(ownerB));
    REQUIRE(xaml->Reparent(mini));
    REQUIRE(gdi->Reparent(mini));
    CHECK(wxWinUITopLevelHost::FindSlotOwner(xaml) == miniHost);

    // USER32 owns the minimize/restore propagation for floating owned TLWs;
    // no WinUI-specific non-client or host path is involved.
    ownerB->Iconize(true);
    DrainTransientWindows();
    CHECK(ownerB->IsIconized());
    CHECK_FALSE(::IsWindowVisible(static_cast<HWND>(mini->GetHWND())));
    ownerB->Iconize(false);
    DrainTransientWindows();
    CHECK_FALSE(ownerB->IsIconized());
    CHECK(::IsWindowVisible(static_cast<HWND>(mini->GetHWND())) != 0);

    const wxWeakRef<wxWindow> miniWeak(mini);
    vetoClose = false;
    mini->Close();
    REQUIRE(DrainTransientWindowsUntil([miniWeak]() { return !miniWeak; }));
    CHECK(closeEvents == 2);

    ownerA->Destroy();
    ownerB->Destroy();
    REQUIRE(DrainTransientWindowsUntil([&]()
    {
        return SameLiveHostObjects(physicalBefore);
    }));
    CheckBalancedHandlers(physicalBefore);
}

TEST_CASE("WinUITransientWindows::MiniFrameDestroyDuringOwnerPublication",
          "[winui-beta-transients][miniframe][reparent][reentrancy]")
{
    const HostPhysicalSnapshot physicalBefore;
    wxFrame * const ownerA =
        new wxFrame(nullptr, wxID_ANY, "mini-destroy-a",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    wxFrame * const ownerB =
        new wxFrame(nullptr, wxID_ANY, "mini-destroy-b",
                    wxPoint(-29600, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(ownerA, 0);
    ShowOffscreenWithoutActivation(ownerB, 1);
    wxMiniFrame * const mini =
        new wxMiniFrame(ownerA, wxID_ANY, "mini-destroy",
                        wxPoint(-29200, -30000), wxSize(180, 90));
    new wxButton(mini, wxID_ANY, "slot");
    mini->ShowWithoutActivating();

    const wxWeakRef<wxWindow> miniWeak(mini);
    gs_nonOwnedDestroyAttempted = false;
    wxWinUISetNonOwnedOwnerPublishHookForTesting(
        &DestroyNonOwnedDuringOwnerPublication);
    CHECK_FALSE(mini->Reparent(ownerB));
    wxWinUIResetNonOwnedOwnerPublishHookForTesting();
    CHECK(gs_nonOwnedDestroyAttempted);
    REQUIRE(DrainTransientWindowsUntil([miniWeak]() { return !miniWeak; }));

    ownerA->Destroy();
    ownerB->Destroy();
    REQUIRE(DrainTransientWindowsUntil([&]()
    {
        return SameLiveHostObjects(physicalBefore);
    }));
    CheckBalancedHandlers(physicalBefore);
}

TEST_CASE("WinUITransientWindows::MiniFrameOwnerDestroyedDuringPublication",
          "[winui-beta-transients][miniframe][reparent][owner][reentrancy]")
{
    const HostPhysicalSnapshot physicalBefore;
    wxFrame * const ownerA =
        new wxFrame(nullptr, wxID_ANY, "mini-owner-destroy-a",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    wxFrame * const ownerB =
        new wxFrame(nullptr, wxID_ANY, "mini-owner-destroy-b",
                    wxPoint(-29600, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(ownerA, 0);
    ShowOffscreenWithoutActivation(ownerB, 1);
    wxMiniFrame * const mini =
        new wxMiniFrame(ownerA, wxID_ANY, "mini-owner-destroy",
                        wxPoint(-29200, -30000), wxSize(180, 90));
    new wxButton(mini, wxID_ANY, "slot");
    mini->ShowWithoutActivating();

    const wxWeakRef<wxWindow> miniWeak(mini);
    const wxWeakRef<wxWindow> ownerBWeak(ownerB);
    gs_nonOwnedOwnerDestroyTarget = ownerB;
    gs_nonOwnedOwnerDestroyAttempted = false;
    wxWinUISetNonOwnedOwnerPublishHookForTesting(
        &DestroyProspectiveOwnerDuringNonOwnedPublication);
    CHECK_FALSE(mini->Reparent(ownerB));
    wxWinUIResetNonOwnedOwnerPublishHookForTesting();
    gs_nonOwnedOwnerDestroyTarget = nullptr;

    CHECK(gs_nonOwnedOwnerDestroyAttempted);
    CHECK((ownerBWeak.get() == nullptr ||
           wxWinUITLWHostIsDestroyScheduled(ownerBWeak.get())));
    if ( wxWindow * const liveMini = miniWeak.get() )
    {
        if ( !wxWinUITLWHostIsDestroyScheduled(liveMini) )
            (void)liveMini->Destroy();
    }
    if ( wxWindow * const liveOwnerB = ownerBWeak.get() )
    {
        if ( !wxWinUITLWHostIsDestroyScheduled(liveOwnerB) )
            (void)liveOwnerB->Destroy();
    }

    ownerA->Destroy();
    REQUIRE(DrainTransientWindowsUntil([&]()
    {
        return !miniWeak && !ownerBWeak &&
               SameLiveHostObjects(physicalBefore);
    }));
    CheckBalancedHandlers(physicalBefore);
}

TEST_CASE("WinUITransientWindows::MiniFrameLayoutMutationConverges",
          "[winui-beta-transients][miniframe][reparent][rtl][reentrancy]")
{
    const HostPhysicalSnapshot physicalBefore;
    wxFrame * const ownerA =
        new wxFrame(nullptr, wxID_ANY, "mini-layout-a",
                    wxPoint(-30000, -30000), wxSize(220, 120));
    wxFrame * const ownerB =
        new wxFrame(nullptr, wxID_ANY, "mini-layout-b",
                    wxPoint(-29600, -30000), wxSize(220, 120));
    wxFrame * const ownerC =
        new wxFrame(nullptr, wxID_ANY, "mini-layout-c",
                    wxPoint(-29200, -30000), wxSize(220, 120));
    ShowOffscreenWithoutActivation(ownerA, 0);
    ShowOffscreenWithoutActivation(ownerB, 1);
    ShowOffscreenWithoutActivation(ownerC, 2);
    wxMiniFrame * const mini =
        new wxMiniFrame(ownerA, wxID_ANY, "mini-layout",
                        wxPoint(-28800, -30000), wxSize(180, 90));
    new wxButton(mini, wxID_ANY, "slot");
    mini->ShowWithoutActivating();

    gs_nonOwnedLayoutReparentTarget = ownerC;
    gs_nonOwnedLayoutReparentAttempted = false;
    wxWinUISetNonOwnedLayoutRefreshHookForTesting(
        &ReparentNonOwnedDuringLayoutRefresh);
    const bool reparented = mini->Reparent(ownerB);
    wxWinUIResetNonOwnedLayoutRefreshHookForTesting();
    gs_nonOwnedLayoutReparentTarget = nullptr;

    REQUIRE(reparented);
    CHECK(gs_nonOwnedLayoutReparentAttempted);
    CHECK(mini->GetParent() == ownerC);
    CHECK(::GetWindow(static_cast<HWND>(mini->GetHWND()), GW_OWNER) ==
          static_cast<HWND>(ownerC->GetHWND()));

    DestroyAndDrain(mini);
    ownerA->Destroy();
    ownerB->Destroy();
    ownerC->Destroy();
    REQUIRE(DrainTransientWindowsUntil([&]()
    {
        return SameLiveHostObjects(physicalBefore);
    }));
    CheckBalancedHandlers(physicalBefore);
}

TEST_CASE("WinUITransientWindows::MiniFrameOwnerAndHundredCycles",
          "[winui-beta-transients][miniframe][stress]")
{
    const HostPhysicalSnapshot physicalBefore;
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "mini-cycle-owner",
                    wxPoint(-30000, -30000), wxSize(240, 140));
    ShowOffscreenWithoutActivation(owner, 0);
    const HostPhysicalSnapshot withOwner;

    for ( unsigned cycle = 0; cycle < 100; ++cycle )
    {
        INFO("mini-frame cycle " << cycle);
        wxMiniFrame * const mini =
            new wxMiniFrame(owner, wxID_ANY, "mini-cycle",
                            wxPoint(-29600, -30000), wxSize(180, 90));
        wxButton * const child = new wxButton(mini, wxID_ANY, "slot");
        REQUIRE(wxWinUITopLevelHost::FindSlotOwner(child) != nullptr);
        mini->ShowWithoutActivating();
        CHECK(mini->IsShown());
        REQUIRE(mini->Hide());
        CHECK_FALSE(mini->IsShown());

        const wxWeakRef<wxWindow> miniWeak(mini);
        REQUIRE(mini->Destroy());
        REQUIRE(DrainTransientWindowsUntil([&]()
        {
            return !miniWeak &&
                   SameLiveHostObjects(withOwner);
        }));
    }

    // The floating frame is an owned TLW. Destroying the owner must retire its
    // HWND, host and child slots even if it is currently shown.
    wxMiniFrame * const owned =
        new wxMiniFrame(owner, wxID_ANY, "owned-mini",
                        wxPoint(-29600, -30000), wxSize(180, 90));
    new wxButton(owned, wxID_ANY, "owned-slot");
    owned->ShowWithoutActivating();
    const wxWeakRef<wxWindow> ownerWeak(owner);
    const wxWeakRef<wxWindow> ownedWeak(owned);
    owner->Destroy();
    REQUIRE(DrainTransientWindowsUntil([&]()
    {
        return !ownerWeak && !ownedWeak &&
               SameLiveHostObjects(physicalBefore);
    }));
    CheckBalancedHandlers(physicalBefore);
}

#endif // wxUSE_MINIFRAME

#endif // __WXWINUI__ && wxUSE_WINUI3
