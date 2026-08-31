///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuidialogsession.cpp
// Purpose:     Deterministic tests for WinUI modal presenter session state
// Author:      wxWidgets development team
// Created:     2026-07-24
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

#include "wx/app.h"
#include "wx/button.h"
#include "wx/dialog.h"
#include "wx/frame.h"
#include "wx/panel.h"
#include "wx/scopeguard.h"
#include "wx/toplevel.h"
#include "wx/utils.h"
#include "wx/weakref.h"
#include "wx/msw/wrapwin.h"
#include "wx/winui/private/dialogsession.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"
#include "wx/winui/private/transient.h"

#include <UIAutomation.h>

#include <cstdint>
#include <initializer_list>

namespace
{

void DrainTransientHarnessDispatch(unsigned rounds = 20)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        wxYield();
        wxMilliSleep(5);
    }
}

template <typename Predicate>
bool DrainTransientHarnessUntil(Predicate predicate,
                                unsigned rounds = 100)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        if ( predicate() )
            return true;
        DrainTransientHarnessDispatch(1);
    }

    return predicate();
}

void ShowTransientHarnessWindow(wxTopLevelWindow *window, int ordinal)
{
    REQUIRE(window != nullptr);
    const HWND hwnd = static_cast<HWND>(window->GetHWND());
    REQUIRE(hwnd != nullptr);

    // The harness needs realized TLWs/XAML roots but no desktop activation.
    // The full gate is also launched through the isolated-desktop runner.
    ::SetWindowLongPtr(
        hwnd,
        GWL_EXSTYLE,
        ::GetWindowLongPtr(hwnd, GWL_EXSTYLE) |
            WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW);
    window->Move(wxPoint(-32000 + ordinal * 360, -32000));
    window->ShowWithoutActivating();
}

std::size_t CountLiveHarnessHwnds(
    std::initializer_list<HWND> hwnds)
{
    std::size_t count = 0;
    for ( const HWND hwnd : hwnds )
    {
        if ( hwnd && ::IsWindow(hwnd) )
            ++count;
    }
    return count;
}

bool RegistryLifetimeEquals(
    const wxWinUITransientRegistrySnapshot& lhs,
    const wxWinUITransientRegistrySnapshot& rhs)
{
    // The two deferred-retirement counters are monotonic diagnostics and are
    // intentionally excluded. Every live ownership/callback count must match.
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

} // anonymous namespace

TEST_CASE("WinUIDialogSession::WindowKeyboardContract",
          "[winui-dialog-session]")
{
    CHECK(wxWinUIResolveDialogWindowKey(
              wxWinUIDialogWindowKey::Enter, false, wxID_OK) ==
          wxID_OK);
    CHECK(wxWinUIResolveDialogWindowKey(
              wxWinUIDialogWindowKey::Enter, true, wxID_OK) ==
          wxID_NONE);
    CHECK(wxWinUIResolveDialogWindowKey(
              wxWinUIDialogWindowKey::Escape, false, wxID_YES) ==
          wxID_CANCEL);
    CHECK(wxWinUIResolveDialogWindowKey(
              wxWinUIDialogWindowKey::Other, false, wxID_OK) ==
          wxID_NONE);

    CHECK(wxWinUIExtractDialogAccessKey("&Open") == "O");
    CHECK(wxWinUIExtractDialogAccessKey("Save && E&xit") == "x");
    CHECK(wxWinUIExtractDialogAccessKey("No mnemonic").empty());
    CHECK(wxWinUIExtractDialogAccessKey("Trailing &").empty());
}

TEST_CASE("WinUIDialogSession::OneModalPerOwner",
          "[winui-dialog-session]")
{
    wxWinUITransientModalGate gate;

    const auto first = gate.TryAcquire(0x111);
    REQUIRE(first);
    CHECK(gate.IsCurrent(first));
    CHECK(gate.GetActiveCount() == 1);

    // Reentrant presentation on the same TLW is rejected, while another TLW
    // remains independent.
    CHECK_FALSE(gate.TryAcquire(0x111));
    const auto other = gate.TryAcquire(0x222);
    REQUIRE(other);
    CHECK(gate.GetActiveCount() == 2);

    CHECK(gate.Release(first));
    CHECK_FALSE(gate.IsCurrent(first));
    CHECK(gate.IsCurrent(other));
    CHECK(gate.Release(other));
    CHECK(gate.GetActiveCount() == 0);
}

TEST_CASE("WinUIDialogSession::GenerationRejectsStaleCleanup",
          "[winui-dialog-session]")
{
    wxWinUITransientModalGate gate;

    const auto oldLease = gate.TryAcquire(0x123);
    REQUIRE(oldLease);
    CHECK(gate.Cancel(oldLease.ownerKey));

    const auto newLease = gate.TryAcquire(0x123);
    REQUIRE(newLease);
    CHECK(newLease.generation != oldLease.generation);

    // A delayed cleanup from the first dialog must not release the newer one.
    CHECK_FALSE(gate.Release(oldLease));
    CHECK(gate.IsCurrent(newLease));
    CHECK(gate.Release(newLease));
}

TEST_CASE("WinUIDialogSession::CentralShutdownIsIdempotent",
          "[winui-dialog-session]")
{
    wxWinUITransientModalGate gate;
    const auto first = gate.TryAcquire(0x111);
    const auto second = gate.TryAcquire(0x222);
    REQUIRE(first);
    REQUIRE(second);

    gate.CancelAll();
    CHECK(gate.GetActiveCount() == 0);
    CHECK_FALSE(gate.Release(first));
    CHECK_FALSE(gate.Release(second));
    CHECK_FALSE(gate.Cancel(0x111));
    CHECK_FALSE(gate.TryAcquire(0));
}

TEST_CASE("WinUIDialogSession::ContentDialogResultMatrix",
          "[winui-dialog-session]")
{
    const int ids[] = { wxID_YES, wxID_NO, wxID_HELP };
    wxWinUIDialogResultState result;

    CHECK(result.Resolve(wxWinUIDialogCompletion::Primary, ids, 3) ==
          wxID_YES);
    CHECK(result.Resolve(wxWinUIDialogCompletion::Secondary, ids, 3) ==
          wxID_NO);

    // None without an accepted close-button click is Escape or an external
    // dismissal, never an implicit activation of the third button.
    CHECK(result.Resolve(wxWinUIDialogCompletion::None, ids, 3) ==
          wxID_CANCEL);

    result.AcceptButton(2);
    CHECK(result.Resolve(wxWinUIDialogCompletion::None, ids, 3) ==
          wxID_HELP);

    result.Reset();
    CHECK(result.Resolve(wxWinUIDialogCompletion::None, ids, 3) ==
          wxID_CANCEL);

    // Malformed/short button vectors fail closed.
    CHECK(result.Resolve(wxWinUIDialogCompletion::Primary, nullptr, 0) ==
          wxID_CANCEL);
    CHECK(result.Resolve(wxWinUIDialogCompletion::Secondary, ids, 1) ==
          wxID_CANCEL);
    result.AcceptButton(2);
    CHECK(result.Resolve(wxWinUIDialogCompletion::None, ids, 2) ==
          wxID_CANCEL);
}

TEST_CASE("WinUIDialogSession::VetoedCloseIsNotAResult",
          "[winui-dialog-session]")
{
    const int ids[] = { wxID_OK, wxID_APPLY, wxID_CLOSE };
    wxWinUIDialogResultState result;

    // The production close handler records index 2 only after validation.
    // A veto therefore leaves the result untouched.
    const bool accepted = false;
    if ( accepted )
        result.AcceptButton(2);

    CHECK(result.Resolve(wxWinUIDialogCompletion::None, ids, 3) ==
          wxID_CANCEL);
}

TEST_CASE("WinUIDialogSession::ExternalDismissPolicy",
          "[winui-dialog-session]")
{
    CHECK(wxWinUIShouldCancelExternalDialogDismiss(false, false));
    CHECK_FALSE(wxWinUIShouldCancelExternalDialogDismiss(false, true));
    CHECK_FALSE(wxWinUIShouldCancelExternalDialogDismiss(true, false));
}

TEST_CASE("WinUIDialogSession::LoopRequiresBothSafetyGates",
          "[winui-dialog-session]")
{
    SECTION("rejected dispatcher")
    {
        wxWinUIDialogLoopState loop;
        CHECK_FALSE(loop.Prepare(false, true));
        CHECK(loop.IsAborted());
        CHECK_FALSE(loop.Start());
    }

    SECTION("missing shutdown hook")
    {
        wxWinUIDialogLoopState loop;
        CHECK_FALSE(loop.Prepare(true, false));
        CHECK(loop.IsAborted());
        CHECK_FALSE(loop.Start());
    }

    SECTION("fully armed")
    {
        wxWinUIDialogLoopState loop;
        REQUIRE(loop.Prepare(true, true));
        REQUIRE(loop.Start());
        CHECK(loop.IsRunning());
        loop.Complete();
        CHECK(loop.IsCompleted());
        CHECK_FALSE(loop.IsRunning());
        CHECK_FALSE(loop.Start());
    }

    SECTION("abort and completion are mutually exclusive")
    {
        wxWinUIDialogLoopState aborted;
        REQUIRE(aborted.Prepare(true, true));
        REQUIRE(aborted.Start());
        REQUIRE(aborted.Abort());
        CHECK_FALSE(aborted.Complete());
        CHECK(aborted.IsAborted());
        CHECK_FALSE(aborted.IsCompleted());

        wxWinUIDialogLoopState completed;
        REQUIRE(completed.Prepare(true, true));
        REQUIRE(completed.Start());
        REQUIRE(completed.Complete());
        CHECK_FALSE(completed.Abort());
        CHECK(completed.IsCompleted());
        CHECK_FALSE(completed.IsAborted());
    }
}

TEST_CASE("WinUIDialogSession::ShutdownCannotRearmLoop",
          "[winui-dialog-session]")
{
    wxWinUIDialogLoopState loop;
    REQUIRE(loop.Prepare(true, true));
    REQUIRE(loop.Start());

    loop.Abort();
    CHECK(loop.IsAborted());
    CHECK_FALSE(loop.IsRunning());
    CHECK_FALSE(loop.Start());

    loop.Invalidate();
    CHECK_FALSE(loop.IsActive());
    CHECK_FALSE(loop.Prepare(true, true));
}

TEST_CASE("WinUIDialogSession::RuntimeRegistry",
          "[winui-dialog-session]")
{
    wxFrame * const firstOwner =
        new wxFrame(nullptr, wxID_ANY, "transient-owner-1");
    wxFrame * const secondOwner =
        new wxFrame(nullptr, wxID_ANY, "transient-owner-2");

    int firstCancelled = 0;
    int tipCancelled = 0;
    int otherCancelled = 0;
    auto first = wxWinUIRegisterTransient(
        firstOwner,
        wxWinUITransientKind::ModalDialog,
        [&]() { ++firstCancelled; });
    REQUIRE(first);

    // Exactly one modal per TLW, while a non-modal transient and a modal on
    // another TLW remain independent.
    CHECK_FALSE(wxWinUIRegisterTransient(
        firstOwner, wxWinUITransientKind::ModalDialog, []() {}));
    auto tip = wxWinUIRegisterTransient(
        firstOwner,
        wxWinUITransientKind::TeachingTip,
        [&]() { ++tipCancelled; });
    auto other = wxWinUIRegisterTransient(
        secondOwner,
        wxWinUITransientKind::ModalDialog,
        [&]() { ++otherCancelled; });
    REQUIRE(tip);
    REQUIRE(other);
    CHECK(wxWinUITransientCountForTesting(firstOwner) == 2);
    CHECK(wxWinUITransientCountForTesting(
              firstOwner, wxWinUITransientKind::ModalDialog) == 1);
    CHECK(wxWinUITransientCountForTesting(
              firstOwner, wxWinUITransientKind::TeachingTip) == 1);
    CHECK(wxWinUITransientCountForTesting(secondOwner) == 1);

    wxWinUICancelTransientSessions(firstOwner);
    CHECK(firstCancelled == 1);
    CHECK(tipCancelled == 1);
    CHECK(otherCancelled == 0);
    CHECK_FALSE(first);
    CHECK_FALSE(tip);
    CHECK(other);
    CHECK(wxWinUITransientCountForTesting(firstOwner) == 0);
    CHECK(wxWinUITransientCountForTesting(secondOwner) == 1);

    // Stale registration cleanup is harmless after central cancellation.
    first.Reset();
    tip.Reset();
    CHECK(other);
    other.Reset();
    CHECK(wxWinUITransientCountForTesting(secondOwner) == 0);

    firstOwner->Destroy();
    secondOwner->Destroy();
}

TEST_CASE("WinUIDialogSession::RuntimeCancelAllowsReentrantReplacement",
          "[winui-dialog-session]")
{
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "transient-reentrant");

    wxWinUITransientRegistration replacement;
    auto original = wxWinUIRegisterTransient(
        owner,
        wxWinUITransientKind::ModalDialog,
        [&]()
        {
            replacement = wxWinUIRegisterTransient(
                owner,
                wxWinUITransientKind::ModalDialog,
                []() {});
        });
    REQUIRE(original);
    CHECK(wxWinUITransientCountForTesting(owner) == 1);

    wxWinUICancelTransientSessions(owner);
    CHECK_FALSE(original);
    CHECK(replacement);
    CHECK(wxWinUITransientCountForTesting(owner) == 1);

    replacement.Reset();
    CHECK(wxWinUITransientCountForTesting(owner) == 0);
    owner->Destroy();
}

TEST_CASE("WinUIDialogSession::RuntimeRegistrationMoveAndReset",
          "[winui-dialog-session]")
{
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "transient-move");

    int cancelled = 0;
    auto registration = wxWinUIRegisterTransient(
        owner,
        wxWinUITransientKind::Popup,
        [&]() { ++cancelled; });
    REQUIRE(registration);
    const std::uint64_t generation = registration.GetGeneration();

    wxWinUITransientRegistration moved(std::move(registration));
    CHECK_FALSE(registration);
    REQUIRE(moved);
    CHECK(moved.GetGeneration() == generation);
    CHECK(wxWinUITransientCountForTesting(owner) == 1);

    wxWinUITransientRegistration assigned;
    assigned = std::move(moved);
    CHECK_FALSE(moved);
    REQUIRE(assigned);
    CHECK(assigned.GetGeneration() == generation);

    assigned.Reset();
    CHECK_FALSE(assigned);
    CHECK(cancelled == 0);
    CHECK(wxWinUITransientCountForTesting(owner) == 0);

    // Explicit unregister is not cancellation.
    wxWinUICancelTransientSessions(owner);
    CHECK(cancelled == 0);
    owner->Destroy();
}

TEST_CASE("WinUIDialogSession::RuntimeOwnerDestroyCancelsCentrally",
          "[winui-dialog-session]")
{
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "transient-owner-destroy");

    int cancelled = 0;
    wxWinUITransientRegistration replacement;
    auto registration = wxWinUIRegisterTransient(
        owner,
        wxWinUITransientKind::TeachingTip,
        [&]()
        {
            ++cancelled;
            // Reentrant central cancellation must not erase the manager's
            // OwnerObserver while its wxEVT_DESTROY method is on the stack.
            wxWinUICancelTransientSessions(owner);
            replacement = wxWinUIRegisterTransient(
                owner,
                wxWinUITransientKind::TeachingTip,
                []() {});
        });
    REQUIRE(registration);

    wxPanel * const child =
        new wxPanel(owner, wxID_ANY);
    wxWindowDestroyEvent childDestroying(child);
    owner->ProcessWindowEvent(childDestroying);
    CHECK(cancelled == 0);
    CHECK(registration);
    CHECK(wxWinUITransientCountForTesting(owner) == 1);

    // Drive the real wxEVT_DESTROY hook deterministically.  The manager marks
    // the TLW as shutting down before invoking cancellation callbacks, so
    // they cannot resurrect a transient on a dying owner.
    wxWindowDestroyEvent destroying(owner);
    owner->ProcessWindowEvent(destroying);
    CHECK(cancelled == 1);
    CHECK_FALSE(registration);
    CHECK_FALSE(replacement);
    CHECK(wxWinUITransientCountForTesting(owner) == 0);

    registration.Reset();
    owner->Destroy();
}

TEST_CASE("WinUIDialogSession::LogicalOwnerDestroyGatesFirstRegistration",
          "[winui-dialog-session][winui-beta-dialogs]"
          "[winui-beta-transients][lifetime]")
{
    // Earlier modal stress cases can leave the observer-retirement CallAfter
    // queued until the next dispatch turn. Drain those completed owner epochs
    // before taking an address-sensitive baseline: a newly allocated frame is
    // otherwise allowed to reuse one of their numeric addresses, correctly
    // replacing (rather than incrementing) the stale registry entry.
    REQUIRE(DrainTransientHarnessUntil(
        []()
        {
            return wxWinUIGetTransientRegistrySnapshotForTesting()
                       .pendingOwnerRetireCount == 0;
        },
        // A preceding 100-cycle dialog stress can enqueue one retirement per
        // owner.  Keep this gate bounded, but don't make its success depend on
        // all CallAfter callbacks being serviced within the default 500 ms on
        // a busy machine.
        500));

    const wxWinUITransientRegistrySnapshot registryBefore =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "transient-logical-destroy");
    const wxWeakRef<wxWindow> weakOwner(owner);

    // Exercise the logical boundary directly, before pending-delete or the
    // shared TLW host can make IsBeingDeleted()/destroy-scheduled observable.
    // Even an owner with no previous registry entry must acquire a tombstone.
    REQUIRE_FALSE(wxWinUITLWHostIsDestroyScheduled(owner));
    wxWinUINotifyTransientOwnerDestroyScheduled(owner);
    REQUIRE_FALSE(wxWinUITLWHostIsDestroyScheduled(owner));

    const wxWinUITransientRegistrySnapshot afterNotify =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    CHECK(afterNotify.ownerCount == registryBefore.ownerCount + 1);
    CHECK(afterNotify.pendingOwnerRetireCount ==
          registryBefore.pendingOwnerRetireCount + 1);
    CHECK_FALSE(wxWinUIRegisterTransient(
        owner, wxWinUITransientKind::ModalDialog, []() {}));
    CHECK_FALSE(wxWinUIRegisterTransient(
        owner, wxWinUITransientKind::Popup, []() {}));
    CHECK_FALSE(wxWinUIRegisterTransient(
        owner, wxWinUITransientKind::TeachingTip, []() {}));

    // Repeated logical notifications keep the same owner epoch and observer.
    wxWinUINotifyTransientOwnerDestroyScheduled(owner);
    const wxWinUITransientRegistrySnapshot afterSecondNotify =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    CHECK(afterSecondNotify.ownerCount == afterNotify.ownerCount);
    CHECK(afterSecondNotify.pendingOwnerRetireCount ==
          afterNotify.pendingOwnerRetireCount);

    owner->Destroy();
    REQUIRE(DrainTransientHarnessUntil(
        [weakOwner, &registryBefore]()
        {
            return !weakOwner && RegistryLifetimeEquals(
                wxWinUIGetTransientRegistrySnapshotForTesting(),
                registryBefore);
        }));
}

TEST_CASE("WinUIDialogSession::ExtendedTransientLifecycleHarness",
          "[winui-dialog-session][winui-beta-dialogs]"
          "[winui-beta-transients][lifetime][stress]")
{
    DrainTransientHarnessDispatch();

    const wxWinUITransientRegistrySnapshot registryBaseline =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    const std::size_t detachedOwnerBaseline =
        wxWinUI3GetOwnerSnapshotForTesting(nullptr).transactionCount;
    const unsigned hostBaseline =
        wxWinUITopLevelHost::GetLiveHostCount();
    const unsigned slotBaseline =
        wxWinUITopLevelHost::GetLiveSlotCount();
    const unsigned slotLifetimeBaseline =
        wxWinUITopLevelHost::GetLiveSlotLifetimeCount();
    const unsigned automationObserverBaseline =
        wxWinUITopLevelHost::
            GetLiveAutomationNameStyleObserverCountForTest();
    const unsigned loadedObserverBaseline =
        wxWinUITopLevelHost::GetLiveContentLoadedObserverCountForTest();
    const unsigned shellProviderBaseline =
        wxWinUITestGetLiveInvisibleShellProviderCount();
    const unsigned handlerAddsBaseline =
        wxWinUITopLevelHost::GetSlotHandlerAddCount();
    const unsigned handlerRevokesBaseline =
        wxWinUITopLevelHost::GetSlotHandlerRevokeCount();

    wxFrame * const first =
        new wxFrame(nullptr, wxID_ANY, "transient harness first",
                    wxPoint(-32000, -32000), wxSize(300, 180));
    wxFrame * const second =
        new wxFrame(nullptr, wxID_ANY, "transient harness second",
                    wxPoint(-31640, -32000), wxSize(300, 180));
    wxButton * const firstSlot =
        new wxButton(first, wxID_ANY, "first slot");
    wxButton * const secondSlot =
        new wxButton(second, wxID_ANY, "second slot");

    wxWeakRef<wxWindow> weakFirst(first);
    wxWeakRef<wxWindow> weakSecond(second);
    wxWeakRef<wxWindow> weakModeless;
    const auto cleanup = wxMakeGuard(
        [&]()
        {
            if ( wxWindow * const window = weakModeless.get() )
                window->Destroy();
            if ( wxWindow * const window = weakFirst.get() )
                window->Destroy();
            if ( wxWindow * const window = weakSecond.get() )
                window->Destroy();
            DrainTransientHarnessDispatch();
        });

    ShowTransientHarnessWindow(first, 0);
    ShowTransientHarnessWindow(second, 1);
    REQUIRE(DrainTransientHarnessUntil(
        [&]()
        {
            return wxWinUITopLevelHost::FindSlotOwner(firstSlot) &&
                   wxWinUITopLevelHost::FindSlotOwner(secondSlot);
        }));

    // Realize the logical owner before creating the modeless dialog. This is
    // the production ordering for an active parent and makes its native owner
    // contract observable instead of relying on wx's hidden-owner fallback.
    wxDialog * const modeless =
        new wxDialog(first, wxID_ANY, "transient harness modeless",
                     wxPoint(-31280, -32000), wxSize(300, 180));
    weakModeless = wxWeakRef<wxWindow>(modeless);
    wxButton * const modelessSlot =
        new wxButton(modeless, wxID_ANY, "modeless slot");
    ShowTransientHarnessWindow(modeless, 2);
    REQUIRE(DrainTransientHarnessUntil(
        [&]()
        {
            return wxWinUITopLevelHost::FindSlotOwner(firstSlot) &&
                   wxWinUITopLevelHost::FindSlotOwner(secondSlot) &&
                   wxWinUITopLevelHost::FindSlotOwner(modelessSlot);
        }));

    const HWND firstHwnd = static_cast<HWND>(first->GetHWND());
    const HWND secondHwnd = static_cast<HWND>(second->GetHWND());
    const HWND modelessHwnd = static_cast<HWND>(modeless->GetHWND());
    const HWND firstSlotHwnd = static_cast<HWND>(firstSlot->GetHWND());
    const HWND secondSlotHwnd = static_cast<HWND>(secondSlot->GetHWND());
    const HWND modelessSlotHwnd =
        static_cast<HWND>(modelessSlot->GetHWND());
    REQUIRE(CountLiveHarnessHwnds(
                { firstHwnd, secondHwnd, modelessHwnd,
                  firstSlotHwnd, secondSlotHwnd, modelessSlotHwnd }) == 6);
    CHECK(modeless->IsShown());
    CHECK(modeless->GetParent() == first);
    CHECK(wxWinUITopLevelHost::GetLiveHostCount() >= hostBaseline + 3);
    CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == slotBaseline + 3);

    // Retain one real Raw-only shell provider across owner destruction. Its
    // state must become unavailable immediately and the COM fragment itself
    // must disappear exactly when the final reference is released.
    IRawElementProviderSimple *shellProvider = nullptr;
    REQUIRE(wxWinUITestCreateInvisibleShellProvider(
                modelessSlot, &shellProvider) == S_OK);
    REQUIRE(shellProvider != nullptr);
    auto releaseShellProvider = wxMakeGuard(
        [&]()
        {
            if ( shellProvider )
            {
                shellProvider->Release();
                shellProvider = nullptr;
            }
        });
    CHECK(wxWinUITestGetLiveInvisibleShellProviderCount() ==
          shellProviderBaseline + 1);
    VARIANT shellControlState;
    ::VariantInit(&shellControlState);
    REQUIRE(shellProvider->GetPropertyValue(
                UIA_IsControlElementPropertyId,
                &shellControlState) == S_OK);
    CHECK(shellControlState.vt == VT_BOOL);
    CHECK(shellControlState.boolVal == VARIANT_FALSE);
    ::VariantClear(&shellControlState);

    // One hundred complete register/cancel/reset cycles exercise both TLWs,
    // concurrent popup/TeachingTip ownership, modal exclusion and monotonic
    // generations without realizing or clicking a physical popup.
    std::uint64_t lastModalGeneration = 0;
    std::uint64_t lastNonModalGeneration = 0;
    unsigned cancelledCallbacks = 0;
    for ( unsigned cycle = 0; cycle < 100; ++cycle )
    {
        auto firstModal = wxWinUIRegisterTransient(
            first,
            wxWinUITransientKind::ModalDialog,
            [&]() { ++cancelledCallbacks; });
        auto popup = wxWinUIRegisterTransient(
            first,
            wxWinUITransientKind::Popup,
            [&]() { ++cancelledCallbacks; });
        auto tip = wxWinUIRegisterTransient(
            modeless,
            wxWinUITransientKind::TeachingTip,
            [&]() { ++cancelledCallbacks; });
        int secondModalCancelled = 0;
        auto secondModal = wxWinUIRegisterTransient(
            second,
            wxWinUITransientKind::ModalDialog,
            [&]() { ++secondModalCancelled; });
        REQUIRE(firstModal);
        REQUIRE(popup);
        REQUIRE(tip);
        REQUIRE(secondModal);

        CHECK(firstModal.GetGeneration() > lastModalGeneration);
        CHECK(secondModal.GetGeneration() >
              firstModal.GetGeneration());
        lastModalGeneration = secondModal.GetGeneration();
        CHECK(popup.GetGeneration() > lastNonModalGeneration);
        CHECK(tip.GetGeneration() > popup.GetGeneration());
        lastNonModalGeneration = tip.GetGeneration();
        CHECK_FALSE(wxWinUIRegisterTransient(
            first, wxWinUITransientKind::ModalDialog, []() {}));

        const wxWinUITransientRegistrySnapshot active =
            wxWinUIGetTransientRegistrySnapshotForTesting();
        CHECK(active.ownerCount == registryBaseline.ownerCount + 3);
        CHECK(active.transientCount ==
              registryBaseline.transientCount + 4);
        CHECK(active.cancelCallbackCount ==
              registryBaseline.cancelCallbackCount + 4);
        CHECK(active.modalCount == registryBaseline.modalCount + 2);
        CHECK(active.popupCount == registryBaseline.popupCount + 1);
        CHECK(active.teachingTipCount ==
              registryBaseline.teachingTipCount + 1);

        wxWinUICancelTransientSessions(first);
        wxWinUICancelTransientSessions(modeless);
        CHECK(cancelledCallbacks == (cycle + 1) * 3);
        CHECK_FALSE(firstModal);
        CHECK_FALSE(popup);
        CHECK_FALSE(tip);

        // Explicit Reset is normal completion, not cancellation.
        secondModal.Reset();
        CHECK(secondModalCancelled == 0);
        CHECK(RegistryLifetimeEquals(
            wxWinUIGetTransientRegistrySnapshotForTesting(),
            registryBaseline));
    }

    // Exercise the real native owner transaction. A synthetic WM_SIZE is the
    // production minimize boundary but cannot activate or move the desktop.
    const LONG_PTR modelessOwner =
        ::GetWindowLongPtr(modelessHwnd, GWLP_HWNDPARENT);
    const HWND modelessOwnerHwnd =
        reinterpret_cast<HWND>(modelessOwner);
    REQUIRE(modelessOwnerHwnd != nullptr);
    REQUIRE(::IsWindow(modelessOwnerHwnd));
    REQUIRE(modelessOwnerHwnd == firstHwnd);
    REQUIRE(wxWinUITransientOwnerActivation(modelessHwnd, true));
    CHECK(wxWinUI3GetOwnerSnapshotForTesting(modelessHwnd).
              transactionCount == detachedOwnerBaseline + 1);
    CHECK(::GetWindowLongPtr(modelessHwnd, GWLP_HWNDPARENT) == 0);

    ::SendMessage(modelessOwnerHwnd, WM_SIZE, SIZE_MINIMIZED, 0);
    CHECK(wxWinUI3GetOwnerSnapshotForTesting(modelessHwnd).
              transactionCount == detachedOwnerBaseline);
    CHECK(::GetWindowLongPtr(modelessHwnd, GWLP_HWNDPARENT) ==
          modelessOwner);
    ::SendMessage(modelessOwnerHwnd, WM_SIZE, SIZE_RESTORED, 0);

    // A cross-TLW reparent must migrate the one existing slot, not leak or
    // duplicate it. The old TLW is destroyed later while this child survives.
    wxWinUITopLevelHost * const secondHost =
        wxWinUITopLevelHost::FindSlotOwner(secondSlot);
    REQUIRE(secondHost != nullptr);
    REQUIRE(firstSlot->Reparent(second));
    REQUIRE(DrainTransientHarnessUntil(
        [&]()
        {
            return wxWinUITopLevelHost::FindSlotOwner(firstSlot) ==
                   secondHost;
        }));
    CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == slotBaseline + 3);

    // Model a nested presentation from the real cancellation callback: the
    // first TLW releases its modal lease before arbitrary user code opens the
    // second TLW's dialog and destroys the first owner.
    unsigned outerModalCancels = 0;
    unsigned nestedModalCancels = 0;
    wxWinUITransientRegistration nestedModal;
    auto outerModal = wxWinUIRegisterTransient(
        first,
        wxWinUITransientKind::ModalDialog,
        [&]()
        {
            ++outerModalCancels;
            nestedModal = wxWinUIRegisterTransient(
                second,
                wxWinUITransientKind::ModalDialog,
                [&]() { ++nestedModalCancels; });
            if ( wxWindow * const owner = weakFirst.get() )
                owner->Destroy();
        });
    unsigned popupCancels = 0;
    unsigned tipCancels = 0;
    auto popup = wxWinUIRegisterTransient(
        modeless,
        wxWinUITransientKind::Popup,
        [&]() { ++popupCancels; });
    auto tip = wxWinUIRegisterTransient(
        modeless,
        wxWinUITransientKind::TeachingTip,
        [&]() { ++tipCancels; });
    REQUIRE(outerModal);
    REQUIRE(popup);
    REQUIRE(tip);

    // Leave both an owner detach and a generation-bound open continuation in
    // flight. Destruction must invalidate the registration before the queued
    // callback runs; the callback itself still executes exactly once.
    REQUIRE(wxWinUITransientOwnerActivation(modelessHwnd, true));
    CHECK(wxWinUI3GetOwnerSnapshotForTesting(modelessHwnd).
              transactionCount == detachedOwnerBaseline + 1);
    const std::uint64_t queuedPopupGeneration = popup.GetGeneration();
    wxWeakRef<wxWindow> queuedOwner(modeless);
    unsigned openEnqueueCount = 0;
    unsigned openCallbackCount = 0;
    unsigned lateOpenCallbackCount = 0;
    ++openEnqueueCount;
    wxTheApp->CallAfter(
        [&]()
        {
            ++openCallbackCount;
            wxWindow * const owner = queuedOwner.get();
            if ( owner &&
                    !wxWinUITLWHostIsDestroyScheduled(owner) &&
                    popup &&
                    popup.GetGeneration() == queuedPopupGeneration )
            {
                ++lateOpenCallbackCount;
            }
        });

    const wxWinUITransientRegistrySnapshot beforeOwnerDestroy =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    wxWinUICancelTransientSessions(first);
    CHECK(outerModalCancels == 1);
    CHECK_FALSE(outerModal);
    REQUIRE(nestedModal);

    // wxTopLevelWindow::Destroy() is deferred. The first owner was destroyed
    // reentrantly by the modal cancellation above and the modeless owner is
    // requested here, so both logical tombstones must retire exactly once.
    // Request the latter before yielding so the queued continuation can only
    // observe dead state.
    if ( wxWindow * const window = weakModeless.get() )
        window->Destroy();

    const bool ownerDestroyDrained = DrainTransientHarnessUntil(
        [&]()
        {
            const wxWinUITransientRegistrySnapshot registry =
                wxWinUIGetTransientRegistrySnapshotForTesting();
            return openCallbackCount == 1 &&
                   weakFirst.get() == nullptr &&
                   weakModeless.get() == nullptr &&
                   registry.ownerRetireCallbackCount ==
                       beforeOwnerDestroy.ownerRetireCallbackCount + 2 &&
                   registry.pendingOwnerRetireCount ==
                       registryBaseline.pendingOwnerRetireCount;
        });
    if ( !ownerDestroyDrained )
    {
        const wxWinUITransientRegistrySnapshot registry =
            wxWinUIGetTransientRegistrySnapshotForTesting();
        INFO("open callbacks=" << openCallbackCount
             << ", first alive=" << (weakFirst.get() != nullptr)
             << ", modeless alive=" << (weakModeless.get() != nullptr)
             << ", retire callbacks=" << registry.ownerRetireCallbackCount
             << " expected="
             << beforeOwnerDestroy.ownerRetireCallbackCount + 2
             << ", pending retire=" << registry.pendingOwnerRetireCount
             << " baseline=" << registryBaseline.pendingOwnerRetireCount
             << ", owners=" << registry.ownerCount
             << ", transients=" << registry.transientCount);
    }
    REQUIRE(ownerDestroyDrained);
    CHECK(openEnqueueCount == 1);
    CHECK(openCallbackCount == 1);
    CHECK(lateOpenCallbackCount == 0);
    CHECK(popupCancels == 1);
    CHECK(tipCancels == 1);
    CHECK_FALSE(popup);
    CHECK_FALSE(tip);
    CHECK(nestedModal);
    CHECK(wxWinUI3GetOwnerSnapshotForTesting(modelessHwnd).
              transactionCount == detachedOwnerBaseline);

    const wxWinUITransientRegistrySnapshot afterOwnerDestroy =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    CHECK(afterOwnerDestroy.ownerRetireEnqueueCount ==
          beforeOwnerDestroy.ownerRetireEnqueueCount + 2);
    CHECK(afterOwnerDestroy.ownerRetireCallbackCount ==
          beforeOwnerDestroy.ownerRetireCallbackCount + 2);
    CHECK(afterOwnerDestroy.pendingOwnerRetireCount ==
          registryBaseline.pendingOwnerRetireCount);

    ::VariantInit(&shellControlState);
    CHECK(shellProvider->GetPropertyValue(
              UIA_IsControlElementPropertyId,
              &shellControlState) == UIA_E_ELEMENTNOTAVAILABLE);
    CHECK(shellControlState.vt == VT_EMPTY);
    ::VariantClear(&shellControlState);
    shellProvider->Release();
    shellProvider = nullptr;
    CHECK(wxWinUITestGetLiveInvisibleShellProviderCount() ==
          shellProviderBaseline);

    wxWinUICancelTransientSessions(second);
    CHECK(nestedModalCancels == 1);
    CHECK_FALSE(nestedModal);
    nestedModal.Reset();

    if ( wxWindow * const window = weakSecond.get() )
        window->Destroy();
    REQUIRE(DrainTransientHarnessUntil(
        [&]()
        {
            return weakSecond.get() == nullptr &&
                   CountLiveHarnessHwnds(
                       { firstHwnd, secondHwnd, modelessHwnd,
                         firstSlotHwnd, secondSlotHwnd,
                         modelessSlotHwnd }) == 0 &&
                   wxWinUITopLevelHost::GetLiveHostCount() ==
                       hostBaseline &&
                   wxWinUITopLevelHost::GetLiveSlotCount() ==
                       slotBaseline &&
                   wxWinUITopLevelHost::GetLiveSlotLifetimeCount() ==
                       slotLifetimeBaseline &&
                   RegistryLifetimeEquals(
                       wxWinUIGetTransientRegistrySnapshotForTesting(),
                       registryBaseline);
        }));

    const wxWinUITransientRegistrySnapshot finalRegistry =
        wxWinUIGetTransientRegistrySnapshotForTesting();
    CAPTURE(finalRegistry.ownerCount, registryBaseline.ownerCount,
            finalRegistry.transientCount, registryBaseline.transientCount,
            finalRegistry.cancelCallbackCount,
            registryBaseline.cancelCallbackCount,
            finalRegistry.modalCount, registryBaseline.modalCount,
            finalRegistry.popupCount, registryBaseline.popupCount,
            finalRegistry.teachingTipCount,
            registryBaseline.teachingTipCount,
            finalRegistry.pendingOwnerRetireCount,
            registryBaseline.pendingOwnerRetireCount,
            finalRegistry.activeOwnerRetirementCount,
            registryBaseline.activeOwnerRetirementCount);
    CHECK(RegistryLifetimeEquals(finalRegistry, registryBaseline));
    CHECK(wxWinUI3GetOwnerSnapshotForTesting(nullptr).transactionCount ==
          detachedOwnerBaseline);
    CHECK(wxWinUITopLevelHost::
              GetLiveAutomationNameStyleObserverCountForTest() ==
          automationObserverBaseline);
    CHECK(wxWinUITopLevelHost::
              GetLiveContentLoadedObserverCountForTest() ==
          loadedObserverBaseline);
    CHECK(wxWinUITopLevelHost::GetSlotHandlerAddCount() -
              handlerAddsBaseline ==
          wxWinUITopLevelHost::GetSlotHandlerRevokeCount() -
              handlerRevokesBaseline);
    CHECK(wxWinUITestGetLiveInvisibleShellProviderCount() ==
          shellProviderBaseline);
}

#endif // __WXWINUI__ && wxUSE_WINUI3
