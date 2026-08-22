/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/private/tlwhostmsw.h
// Purpose:     MSW-side entry points of the shared per-TLW island host
// Author:      wxWidgets development team
// Created:     2026-07-22
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PRIVATE_TLWHOSTMSW_H_
#define _WX_WINUI_PRIVATE_TLWHOSTMSW_H_

#include "wx/defs.h"
#include "wx/gdicmn.h"

#if wxUSE_WINUI3

class WXDLLIMPEXP_FWD_CORE wxWindow;
class WXDLLIMPEXP_FWD_CORE wxWindowBase;

// The few functions src/msw code calls into the shared island host
// (src/winui/tlwhost.cpp), declared here so those translation units don't
// have to pull the C++/WinRT projection in.

// Keep physical XAML teardown outside every wx event dispatched for a window
// owned by a live island. Every WinUI control callback routes application
// events through the non-virtual wxWindowBase::HandleWindowEvent() boundary;
// an event handler is allowed to destroy its TLW and run a nested wxYield(),
// but that nested loop must not revoke the delegate or close its
// DesktopWindowXamlSource while the original XAML callback is still on the
// stack.
//
// The cookie is an opaque existing-host reference, never a newly created host.
// Calls are nestable and confined to the GUI thread.
WXDLLIMPEXP_CORE void *
wxWinUITLWHostBeginWindowEventDispatch(const wxWindowBase *window);
WXDLLIMPEXP_CORE void
wxWinUITLWHostEndWindowEventDispatch(void *cookie);

class wxWinUITLWHostWindowEventGuard final
{
public:
    explicit wxWinUITLWHostWindowEventGuard(const wxWindowBase *window)
        : m_cookie(wxWinUITLWHostBeginWindowEventDispatch(window))
    {
    }

    ~wxWinUITLWHostWindowEventGuard()
    {
        if ( m_cookie )
            wxWinUITLWHostEndWindowEventDispatch(m_cookie);
    }

private:
    void *m_cookie;
    wxDECLARE_NO_COPY_CLASS(wxWinUITLWHostWindowEventGuard);
};

// Delay wxTopLevelWindowMSW::Destroy() while any XAML/host/slot callback is
// active. Processing wxPendingDelete from a nested wxYield() would otherwise
// destroy an island HWND under WinUI's focus/input stack, including after a
// cross-TLW reparent. The GUI-thread transaction queues weak TLW identities
// and completes the already-requested destruction on a later wx turn, without
// invoking a public Destroy()/Show() override twice. Direct `delete` of a live
// TLW remains outside the wx destruction contract; application code must use
// Destroy().
WXDLLIMPEXP_CORE bool
wxWinUITLWHostDeferTopLevelDestroy(wxWindow *window);

// wxDialog sends wxEVT_DESTROY before wxTopLevelWindowMSW::Destroy() for a
// regular modal dialog. Intercept that special preamble while a WinUI callback
// is active, so the host remains authoritative until the callback unwinds.
WXDLLIMPEXP_CORE bool
wxWinUITLWHostDeferModalDialogDestroy(wxWindow *window);

// Return true when the exact window is already being destroyed, has entered
// wxPendingDelete, or is waiting in the private WinUI destroy queue. Weak
// references and live HWNDs alone are insufficient during a retained XAML
// callback because all three states deliberately keep the C++ object alive.
WXDLLIMPEXP_CORE bool
wxWinUITLWHostIsDestroyScheduled(const wxWindow *window);

// Return true only when this exact window identity is waiting in the private
// WinUI deferred-destroy queue. Unlike wxWinUITLWHostIsDestroyScheduled(),
// this deliberately doesn't inspect IsBeingDeleted() or wxPendingDelete and
// therefore never inherits an ancestor's destruction state.
WXDLLIMPEXP_CORE bool
wxWinUITLWHostHasDeferredDestroyExact(const wxWindow *window);

// Popup windows are top-level wx windows but are not wxTopLevelWindowMSW
// instances. Keep their two existing destruction contracts distinct while a
// WinUI callback is active: ordinary popups destroy immediately, transient
// popups enter wxPendingDelete. The tri-state preserves the transient
// "Destroy() twice is an error" contract while the first request is waiting
// in the private WinUI queue rather than wxPendingDelete.
enum class wxWinUIDestroyDeferralResult
{
    NotDeferred,
    Deferred,
    AlreadyDeferred
};

enum class wxWinUIPopupDestroySemantics
{
    Immediate,
    PendingDelete
};

WXDLLIMPEXP_CORE wxWinUIDestroyDeferralResult
wxWinUITLWHostDeferPopupDestroy(
    wxWindow *window,
    wxWinUIPopupDestroySemantics semantics);

// Replay a popup request through a qualified, non-virtual MSW completion
// path. This must never use wxWinUIMSWCompleteDeferredTopLevelDestroy(),
// whose target is specifically wxTopLevelWindowMSW.
WXDLLIMPEXP_CORE bool
wxWinUIMSWCompleteDeferredPopupDestroy(
    wxWindow *window,
    wxWinUIPopupDestroySemantics semantics);

// Finish the common MSW delayed-destruction mechanics without re-entering a
// virtual Destroy() override. `hideBeforePending` is false only when an
// existing wxPendingDelete entry was temporarily shielded for a WinUI
// transaction. `destroyEventAlreadySent` is true only for the modal-dialog
// preamble above.
WXDLLIMPEXP_CORE bool
wxWinUIMSWCompleteDeferredTopLevelDestroy(
    wxWindow *window,
    bool hideBeforePending,
    bool destroyEventAlreadySent);

// Return the generation of this exact wxWindow/HWND association, or zero if
// either side no longer names the same native window. A handle recycled after
// destroy always receives a fresh generation, even if USER32 reuses the same
// numeric HWND for the same wx object.
WXDLLIMPEXP_CORE unsigned long long
wxWinUIMSWGetHwndGeneration(wxWindow *window, WXHWND hwnd);

// Deterministic seams for the cross-TLW DPI transaction tests. Production
// uses the HWND DPI and real SetParent(); tests supply per-TLW values without
// moving windows across the user's physical monitors and can inject one
// native-parent failure to verify the logical rollback.
using wxWinUIReparentDPIQueryForTest = wxSize (*)(wxWindow *window);
WXDLLIMPEXP_CORE void
wxWinUIMSWSetReparentDPIQueryForTest(
    wxWinUIReparentDPIQueryForTest query);
WXDLLIMPEXP_CORE void
wxWinUIMSWFailNextSetParentForTest();
using wxWinUIAfterSetParentForTest = void (*)(wxWindow *window);
WXDLLIMPEXP_CORE void
wxWinUIMSWSetAfterSetParentForTest(
    wxWinUIAfterSetParentForTest callback);

// One-shot seam after SubclassWin() has propagated WS_EX_CONTROLPARENT to
// the native parent chain. It proves that the adopted-window transaction
// revalidates the exact logical/native lifetime before wxEVT_CREATE.
using wxWinUIAfterEnsureControlParentStyleForTest =
    void (*)(wxWindow *window);
WXDLLIMPEXP_CORE void
wxWinUIMSWSetAfterEnsureControlParentStyleForTest(
    wxWinUIAfterEnsureControlParentStyleForTest callback);

// One-shot seam after a native HWND has been removed from the wx map and the
// window object, but before raw-HWND UIA cleanup. Tests may destroy the window
// here to prove that detach finishes exclusively from local snapshots.
using wxWinUIAfterMSWHandleDepublishedForTest =
    void (*)(wxWindow *window);
WXDLLIMPEXP_CORE void
wxWinUIMSWSetAfterHandleDepublishedForTest(
    wxWinUIAfterMSWHandleDepublishedForTest callback);

// One-shot seam immediately after the native layout-direction style write
// and before the revision is committed. It lets tests perform a nested
// explicit request or destroy/reparent the exact window at the same
// last-writer boundary as WM_STYLECHANGING/WM_STYLECHANGED reentrance.
using wxWinUIAfterLayoutDirectionNativeWriteForTest =
    void (*)(wxWindow *window);
WXDLLIMPEXP_CORE void
wxWinUIMSWSetAfterLayoutDirectionNativeWriteForTest(
    wxWinUIAfterLayoutDirectionNativeWriteForTest callback);

// Force the next non-owned HWND owner publication to fail before touching
// USER32, proving that wx logical parentage is reconciled back to the native
// owner instead of remaining split after a failed Reparent().
WXDLLIMPEXP_CORE void wxWinUIMSWFailNextOwnerWriteForTest();

// Return a generation tied to this exact native HWND lifetime, including for
// implementation HWNDs that have no direct wxWindow association. The value is
// stored as a window property, so a USER32 handle recycled after WM_NCDESTROY
// receives a different generation even when its numeric value and class are
// identical.
WXDLLIMPEXP_CORE unsigned long long
wxWinUIMSWGetNativeHwndGeneration(WXHWND hwnd);

// Retire the raw-only UIA provider identity exposed for an invisible shell
// before its HWND association is detached or recycled. Idempotent: it calls
// UiaReturnRawElementProvider(..., nullptr) only for a shell which actually
// exposed our provider during this exact native lifetime.
WXDLLIMPEXP_CORE void
wxWinUITLWHostRetireAccessibilityShellProvider(WXHWND hwnd);

// Bracket a synthetic client-mouse dispatch performed by the island router.
// wxMSW still generates its logical enter/move events, but must not ask
// USER32 to track leave on the native HWND hidden below the island: USER32
// would immediately see the bridge as the real hit and create ENTER/EXIT
// storms. Calls are nestable and confined to the GUI thread.
WXDLLIMPEXP_CORE void wxWinUIMSWBeginSyntheticMouseDispatch();
WXDLLIMPEXP_CORE void wxWinUIMSWEndSyntheticMouseDispatch();

// Report an explicit wx capture mutation before calling SetCapture() or
// ReleaseCapture(), and whenever MSW observes WM_CAPTURECHANGED. This lets a
// callback-bearing router cleanup distinguish the capture it intended to
// retire from one deliberately established by a nested wx handler.
WXDLLIMPEXP_CORE void
wxWinUITLWHostNotifyCaptureMutation(wxWindow *window);

// Resolve a native focus HWND sitting on an island bridge to the wx window
// owning the focused slot; null when the HWND belongs to no live island.
WXDLLIMPEXP_CORE wxWindow *wxWinUITLWHostResolveFocus(WXHWND hwnd);
WXDLLIMPEXP_CORE wxWindow *
wxWinUITLWHostResolveFocusCounterpart(WXHWND hwnd);

// Resolve wxFindWindowAtPoint() when USER32 reports an island implementation
// HWND. Returns null when the HWND belongs to no live island; otherwise the
// logical XAML slot, native wx child below the transparent root, or TLW
// fail-closed result is returned.
WXDLLIMPEXP_CORE wxWindow *
wxWinUITLWHostResolveWindowAtPoint(WXHWND hwnd,
                                   const wxPoint& screenPoint);

// Route a public wxWindow::SetFocus() directly to a live XAML slot before the
// hidden shell HWND can emit CHILD_FOCUS/SET_FOCUS out of logical order.
// Returns true when the host consumed the request and the native shell path
// must not run. An unrealized slot normally returns false so USER32 publishes
// the synchronous shell focus transition; if that exact shell already owns
// focus, the host refreshes the pending XAML content generation itself and
// returns true because USER32 cannot emit another WM_SETFOCUS edge.
WXDLLIMPEXP_CORE bool wxWinUITLWHostSetFocus(wxWindow *window);

// Consume the nested WM_SETFOCUS produced only when a failed island focus
// attempt restores the same slotted shell. Ordinary shell focus always
// publishes its normal wx CHILD/SET pair before the post-event XAML handoff.
WXDLLIMPEXP_CORE bool
wxWinUITLWHostConsumeNativeFocusRollback(
    wxWindow *window,
    WXHWND shell,
    unsigned long long shellGeneration);

// Consume the implementation-only WM_SETFOCUS used to park a logically
// focused slotted control on its shell while its XAML carrier crosses TLWs.
// The migration ticket is generation-bound; ordinary shell focus keeps the
// normal wx CHILD_FOCUS/SET_FOCUS contract.
WXDLLIMPEXP_CORE bool
wxWinUITLWHostConsumeMigrationShellFocus(
    wxWindow *window,
    WXHWND shell,
    unsigned long long shellGeneration);

// Record a real departure from a migration-parking shell before the ordinary
// wxWindowMSW KILL_FOCUS event is dispatched. This lets the ticket retire the
// old logical owner without publishing a second loss later.
WXDLLIMPEXP_CORE bool
wxWinUITLWHostNoteMigrationShellFocusDeparture(
    wxWindow *window,
    WXHWND destination);

// Complete a native shell HWND's WM_SETFOCUS after all application wx focus
// handlers have run. The HWND generation makes this safe if a handler destroys
// and recreates native state; the implementation only hands off to XAML while
// the exact shell still owns native focus.
WXDLLIMPEXP_CORE void
wxWinUITLWHostAfterNativeSetFocus(wxWindow *window,
                                  WXHWND shell,
                                  unsigned long long shellGeneration,
                                  wxWindow *previous);

// True only while a slotted window's native shell is synchronously handing
// focus to its island after its logical wx SET_FOCUS was already delivered.
WXDLLIMPEXP_CORE bool
wxWinUITLWHostShouldSuppressNativeKillFocus(wxWindow *window,
                                            WXHWND destination);

// End-of-freeze notification (wxTopLevelWindowMSW::DoThaw): run the single
// catch-up geometry flush the freeze held back.
WXDLLIMPEXP_CORE void wxWinUITLWHostNotifyThaw(wxWindow *window);

// WM_ENABLE notification (wxWindowMSW::MSWWindowProc): the effective
// enabled state of the hosted descendants changed, re-sync the slots.
WXDLLIMPEXP_CORE void wxWinUITLWHostNotifyEnable(wxWindow *window);

// Native layout mutations which have no reliable wx event counterpart.
// WM_WINDOWPOSCHANGED covers sibling Raise()/Lower() as well as direct
// SetWindowPos() calls; SetScrollbar() changes the bridge cut-outs. The
// implementation only looks up existing hosts and must never create one.
WXDLLIMPEXP_CORE void
wxWinUITLWHostNotifyNativeLayout(wxWindow *window,
                                 bool zOrderMayHaveChanged);

// Slot-state mutation with no geometry event of its own (tooltip set,
// WM_SETTEXT feeding the UIA name): re-sync that one window's slot.
WXDLLIMPEXP_CORE void wxWinUITLWHostNotifySlotState(wxWindow *window);

#if wxUSE_TOOLTIPS
// Mirror the process-wide wxToolTip policy into every XAML island. WinUI has
// no ToolTipService-wide enable switch, so the implementation temporarily
// detaches managed values and restores the exact objects when re-enabled.
// Returns false when a nested policy change superseded this transaction.
WXDLLIMPEXP_CORE bool wxWinUISetToolTipsEnabled(bool enabled);
WXDLLIMPEXP_CORE bool wxWinUIAreToolTipsEnabled();
WXDLLIMPEXP_CORE void wxWinUISetToolTipMaxWidth(int width);
WXDLLIMPEXP_CORE int wxWinUIGetToolTipMaxWidth();

// Bracket the lifetime of the process-wide XAML tooltip policy. Shutdown
// invalidates any queued replay and releases all registry-held XAML objects;
// it must run before WindowsXamlManager::Close().
WXDLLIMPEXP_CORE void wxWinUIInitializeToolTipPolicy();
WXDLLIMPEXP_CORE void wxWinUIShutdownToolTipPolicy();
#endif

// wxWindowBase accessibility mutation. This only updates an already existing
// slot; it must never create a top-level host as a side effect.
WXDLLIMPEXP_CORE void
wxWinUITLWHostNotifyAccessibilityAuthority(wxWindow *window,
                                           bool hasWxAccessible);

// True only for the invisible native HWND shell of an authoritative live
// XAML slot. Top-level windows, island HWNDs and native non-slotted controls
// are deliberately excluded.
WXDLLIMPEXP_CORE bool
wxWinUITLWHostIsInvisibleAccessibilityShell(wxWindow *window);

// Handle OBJID_CLIENT/UiaRootObjectId for such a shell. A caller-provided
// wxAccessible remains authoritative and is never suppressed.
WXDLLIMPEXP_CORE bool
wxWinUITLWHostHandleShellGetObject(wxWindow *window,
                                   WXWPARAM wParam,
                                   WXLPARAM lParam,
                                   bool hasWxAccessible,
                                   WXLRESULT *result);

// A successful native reparent must migrate the slots synchronously.  The old
// TLW can be destroyed before the next CallAfter-based geometry flush.
WXDLLIMPEXP_CORE void wxWinUITLWHostNotifyReparent(wxWindow *window,
                                                   wxWindow *oldTLW);

// A cross-TLW move cannot detach a carrier while one of its exact popup
// gates still waits for Closed/LostFocus/the final dispatcher tail.
WXDLLIMPEXP_CORE bool
wxWinUITLWHostCanReparentSubtreeNow(wxWindow *window);

// Bracket USER32 SetParent() with an exact focus-migration preparation.
// Preparation parks an active XAML focus owner on its invisible shell before
// native activation can publish a logical loss. The cancellation call is
// mandatory on every exit and is idempotent after successful host migration.
WXDLLIMPEXP_CORE unsigned long long
wxWinUITLWHostPrepareFocusReparent(wxWindow *window,
                                   wxWindow *oldTLW,
                                   wxWindow *newTLW);
WXDLLIMPEXP_CORE void
wxWinUITLWHostSetPreparedFocusReparentNativeBoundary(
    unsigned long long token,
    bool active);
WXDLLIMPEXP_CORE void
wxWinUITLWHostCancelPreparedFocusReparent(unsigned long long token);

// Transactionally detach an active WinUI transient from its owner to avoid
// the DWM/XAML interactive-resize penalty, and restore it when inactive.
// Returns true only when the native owner link really changed. Destruction
// consumes any detached transaction without restoring a dying window.
WXDLLIMPEXP_CORE bool
wxWinUITransientOwnerActivation(WXHWND hwnd, bool active);
WXDLLIMPEXP_CORE void
wxWinUITransientOwnerRestoreDependents(WXHWND owner, bool primeNow);
WXDLLIMPEXP_CORE void wxWinUITransientWindowDestroyed(WXHWND hwnd);

// Forget a completed/in-flight backdrop prime after DWM rebuilt the frame.
// An exact in-flight one-pixel shrink is restored before the epoch is retired.
WXDLLIMPEXP_CORE void wxWinUIInvalidateBackdropPrime(WXHWND hwnd);
WXDLLIMPEXP_CORE void wxWinUIBackdropWindowDestroyed(WXHWND hwnd);

#if wxUSE_DRAG_AND_DROP && wxUSE_OLE
// Acquire one owner token for the exact native TLW generation covered by a
// broker acquisition or retained registration. Coverage is owner-counted: a
// failed/retrying broker may release only its own token and can never erase a
// tombstone retained by an older broker whose RevokeDragDrop() failed.
// Returning zero means that the coverage record could not be published.
using wxWinUIOleDropRegistrationCoverageToken = unsigned long long;
WXDLLIMPEXP_CORE wxWinUIOleDropRegistrationCoverageToken
wxWinUITLWHostAcquireOleDropRegistrationCoverage(
    WXHWND hwnd, unsigned long long generation) noexcept;

// Release exactly the token returned by the acquire call above. A token is
// deliberately never released while its broker still owns the native TLW
// registration, including after logical host removal.
WXDLLIMPEXP_CORE void
wxWinUITLWHostReleaseOleDropRegistrationCoverage(
    WXHWND hwnd,
    unsigned long long generation,
    wxWinUIOleDropRegistrationCoverageToken token) noexcept;

// True when the current top-level host owns the one physical OLE
// registration covering this logical window, or when an exact generation
// tombstone proves that a terminal broker registration is still retained.
// SetDropTarget() must then keep only its logical binding and must not register
// the covered shell HWND.
WXDLLIMPEXP_CORE bool
wxWinUITLWHostOwnsOleDropRegistration(wxWindow *window);

// Reconcile shell-vs-broker registration after a subtree crosses TLWs.
WXDLLIMPEXP_CORE void
wxWinUITLWHostNotifyOleDropTopology(wxWindow *subtreeRoot);
#endif

#if wxUSE_DRAG_AND_DROP
// Recompute whether the island bridge needs WS_EX_ACCEPTFILES after the
// acceptance bit of a logical descendant changes.
WXDLLIMPEXP_CORE void
wxWinUITLWHostNotifyDragAcceptFiles(wxWindow *window);
#endif

#endif // wxUSE_WINUI3

#endif // _WX_WINUI_PRIVATE_TLWHOSTMSW_H_
