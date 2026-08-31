/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/private/transient.h
// Purpose:     Test seam for WinUI transient-owner and backdrop transactions
// Author:      wxWidgets development team
// Created:     2026-07-24
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PRIVATE_TRANSIENT_H_
#define _WX_WINUI_PRIVATE_TRANSIENT_H_

#include "wx/defs.h"

#if wxUSE_WINUI3

#include <cstddef>

class wxTipWindow;
class wxWindow;
#if wxUSE_POPUPWIN
class wxPopupWindow;
class wxPopupTransientWindow;

// Private cross-translation-unit hooks. Popup lifetime/session state lives in
// a cpp sidecar so the public exported classes keep their historical layout
// and DismissAndNotify() signature.
WXDLLIMPEXP_CORE bool
wxWinUIPopupPrepareForDestroy(wxPopupWindow *popup);

// Converge a popup to a terminal logical/native state using the same bounded
// capture-release transaction as owner cancellation. The function may destroy
// (or schedule destruction of) the popup when callbacks keep capture or native
// visibility alive. When no exact HWND identity exists, ownership is still
// transferred to the type-independent delayed popup-destroy pipeline. Using
// one delayed fallback is required for applications deriving directly from
// wxPopupTransientWindowBase, which has no distinct wx class-info. A false
// result therefore means that an already-active terminal transaction remains
// authoritative, or that neither retirement nor destroy scheduling committed.
WXDLLIMPEXP_CORE bool
wxWinUIRetirePopupTerminal(wxPopupWindow *popup);
WXDLLIMPEXP_CORE void
wxWinUITestPopupOutsideClick(wxPopupTransientWindow *popup);

// Return true while the central transient manager is terminally retiring the
// popup containing this window. wxWindowBase::CaptureMouse() consults this
// before mutating its capture stack so a hostile capture-lost callback cannot
// resurrect capture after the manager has already withdrawn its registration.
WXDLLIMPEXP_CORE bool
wxWinUIPopupBlocksCapture(wxWindow *window);

// Release the current capture through the wx stack when it owns the capture,
// or directly through USER32 for a native SetCapture() which bypassed that
// stack. The return value is a verified postcondition, not the API result.
WXDLLIMPEXP_CORE bool
wxWinUIPopupReleaseCapture(wxWindow *window);

// Callback boundary immediately after an ownerless popup's HWND has been
// created, but before inherited attributes are published. This is the only
// ownerless-Create-only seam and remains null in production.
using wxWinUIPopupCreateHookForTesting =
    void (*)(wxPopupWindow *popup);
WXDLLIMPEXP_CORE void wxWinUISetPopupCreateHookForTesting(
    wxWinUIPopupCreateHookForTesting hook);
WXDLLIMPEXP_CORE void wxWinUIResetPopupCreateHookForTesting();

// Synchronous callback seam at the USER32 native-owner publication boundary.
// Production leaves it null; tests use it to prove nested owner migration and
// destruction cannot commit an obsolete transient generation.
using wxWinUIPopupOwnerPublishHookForTesting =
    void (*)(wxPopupWindow *popup);
WXDLLIMPEXP_CORE void wxWinUISetPopupOwnerPublishHookForTesting(
    wxWinUIPopupOwnerPublishHookForTesting hook);
WXDLLIMPEXP_CORE void wxWinUIResetPopupOwnerPublishHookForTesting();

// Fail exactly the next recovery write after a popup owner publication became
// stale. This proves that the recovery loop verifies native postconditions
// instead of assuming that a rollback SetWindowLongPtr() succeeded.
WXDLLIMPEXP_CORE void
wxWinUIFailNextPopupOwnerRecoveryForTesting();

// Fail the next instrumented popup/registry ownership allocation before it can
// be committed. This exercises the production rollback guards without
// replacing the allocator or adding state to the public classes.
WXDLLIMPEXP_CORE void
wxWinUIFailNextPopupAllocationForTesting();

// Observe the final owner-deactivation tail. Tests use this to prove that an
// OnDismiss() callback which reopens/reparents a popup invalidates the old
// native continuation.
using wxWinUIPopupOwnerDeactivateHookForTesting =
    void (*)(wxWindow *owner);
WXDLLIMPEXP_CORE void wxWinUISetPopupOwnerDeactivateHookForTesting(
    wxWinUIPopupOwnerDeactivateHookForTesting hook);
WXDLLIMPEXP_CORE void wxWinUIResetPopupOwnerDeactivateHookForTesting();

// Synchronous seam immediately after an expected wx capture release during
// popup dismissal. It is null in production; tests use it to deterministically
// model a capture-lost callback transferring capture to another descendant and
// to prove that the convergence loop is both effective and bounded.
using wxWinUIPopupCaptureReleaseHookForTesting =
    void (*)(wxPopupTransientWindow *popup, unsigned int pass);
WXDLLIMPEXP_CORE void wxWinUISetPopupCaptureReleaseHookForTesting(
    wxWinUIPopupCaptureReleaseHookForTesting hook);
WXDLLIMPEXP_CORE void wxWinUIResetPopupCaptureReleaseHookForTesting();

// Make the next public terminal-retirement request observe a missing native
// generation without mutating USER32 state. This proves that the no-HWND
// fallback still transfers ownership to the deferred destroy pipeline.
WXDLLIMPEXP_CORE void
wxWinUIFailNextPopupTerminalNativeIdentityForTesting();
#endif

// Synchronous seam after a checked non-owned-window owner publication. It is
// shared by Dialog/MiniFrame/Popup reparenting and allows tests to destroy or
// reparent the object at the exact USER32 boundary.
using wxWinUINonOwnedOwnerPublishHookForTesting =
    void (*)(wxWindow *window);
WXDLLIMPEXP_CORE void wxWinUISetNonOwnedOwnerPublishHookForTesting(
    wxWinUINonOwnedOwnerPublishHookForTesting hook);
WXDLLIMPEXP_CORE void wxWinUIResetNonOwnedOwnerPublishHookForTesting();

// Synchronous seam immediately after inherited RTL projection. Production
// leaves it null; tests use it to mutate/destroy the prospective owner at the
// last callback-bearing boundary of wxNonOwnedWindow::Reparent().
using wxWinUINonOwnedLayoutRefreshHookForTesting =
    void (*)(wxWindow *window);
WXDLLIMPEXP_CORE void wxWinUISetNonOwnedLayoutRefreshHookForTesting(
    wxWinUINonOwnedLayoutRefreshHookForTesting hook);
WXDLLIMPEXP_CORE void wxWinUIResetNonOwnedLayoutRefreshHookForTesting();

// Deliberately independent of RECT so the state-machine tests don't need to
// include the complete Windows headers.
struct wxWinUITransientRect
{
    long left = 0;
    long top = 0;
    long right = 0;
    long bottom = 0;

    bool operator==(const wxWinUITransientRect& other) const
    {
        return left == other.left && top == other.top &&
               right == other.right && bottom == other.bottom;
    }

    bool operator!=(const wxWinUITransientRect& other) const
    {
        return !(*this == other);
    }
};

// Every native boundary used by the two state machines is injectable. The
// production table verifies USER32 calls and their postconditions; tests use
// a deterministic fake to exercise failures, HWND reuse and reentrancy.
struct WXDLLIMPEXP_CORE wxWinUIOwnerNativeOps
{
    void *context = nullptr;

    unsigned long long (*getGeneration)(void *, WXHWND) = nullptr;
    bool (*isWindow)(void *, WXHWND) = nullptr;

    bool (*getOwner)(void *, WXHWND, WXHWND *) = nullptr;
    bool (*setOwner)(void *, WXHWND, WXHWND, WXHWND *) = nullptr;
    bool (*setOwnerMarker)(void *, WXHWND, WXHWND) = nullptr;
    bool (*getOwnerMarker)(void *, WXHWND, WXHWND *) = nullptr;
    bool (*clearOwnerMarker)(void *, WXHWND) = nullptr;
};

struct WXDLLIMPEXP_CORE wxWinUIBackdropNativeOps
{
    void *context = nullptr;

    unsigned long long (*getGeneration)(void *, WXHWND) = nullptr;
    bool (*isWindow)(void *, WXHWND) = nullptr;
    bool (*getRect)(void *, WXHWND, wxWinUITransientRect *) = nullptr;
    bool (*setSize)(void *, WXHWND, int, int) = nullptr;
    bool (*setPrimeMarker)(void *, WXHWND) = nullptr;
    bool (*hasPrimeMarker)(void *, WXHWND) = nullptr;
    bool (*clearPrimeMarker)(void *, WXHWND) = nullptr;

    // Returns false without taking ownership of callbackData. On success it
    // owns the data, invokes callback at most once, and always invokes destroy
    // exactly once even when application shutdown discards the queued work.
    bool (*post)(void *,
                 void (*callback)(void *),
                 void (*destroy)(void *),
                 void *callbackData) = nullptr;
};

struct WXDLLIMPEXP_CORE wxWinUITransientSnapshot
{
    std::size_t transactionCount = 0;
    bool inFlight = false;
    bool restorePending = false;
    unsigned long long generation = 0;
    unsigned long long epoch = 0;
};

// These functions are implementation-only test seams. The caller must drain
// callbacks before replacing/resetting the table; stale callbacks are still
// generation/epoch checked and become harmless no-ops.
WXDLLIMPEXP_CORE void
wxWinUI3SetOwnerNativeOpsForTesting(
    const wxWinUIOwnerNativeOps& operations);
WXDLLIMPEXP_CORE void wxWinUI3ResetOwnerNativeOpsForTesting();
WXDLLIMPEXP_CORE wxWinUITransientSnapshot
wxWinUI3GetOwnerSnapshotForTesting(WXHWND hwnd);

WXDLLIMPEXP_CORE void
wxWinUI3SetBackdropNativeOpsForTesting(
    const wxWinUIBackdropNativeOps& operations);
WXDLLIMPEXP_CORE void wxWinUI3ResetBackdropNativeOpsForTesting();
WXDLLIMPEXP_CORE wxWinUITransientSnapshot
wxWinUI3GetBackdropSnapshotForTesting(WXHWND hwnd);

#if wxUSE_TIPWINDOW

// Test-only callback boundary used to prove that wxTipWindow::New() doesn't
// retain or delete a window which destroyed (or destroy-scheduled) itself
// while Create() was publishing its native popup shell.
using wxWinUITipCreateHookForTesting = void (*)(wxTipWindow *tip);

WXDLLIMPEXP_CORE void wxWinUISetTipCreateHookForTesting(
    wxWinUITipCreateHookForTesting hook);
WXDLLIMPEXP_CORE void wxWinUIResetTipCreateHookForTesting();

#endif // wxUSE_TIPWINDOW

#endif // wxUSE_WINUI3

#endif // _WX_WINUI_PRIVATE_TRANSIENT_H_
