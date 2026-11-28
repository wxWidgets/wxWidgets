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

// GDI surfaces which own their background must publish opaque pixels over the
// DWM material. Native XAML slots and deliberately transparent shell surfaces
// must not acquire an opaque paint buffer merely to validate WM_PAINT.
WXDLLIMPEXP_CORE bool
wxWinUIShouldUseOpaquePaintBuffer(const wxWindow *window);

// The other GDI surfaces over the DWM material, those which let it show
// through where they draw nothing: their WM_PAINT goes through a buffer too,
// in which every pixel GDI actually wrote is made opaque. Left as GDI writes
// them, with a zero alpha, such pixels are only added to the material on an
// SDR screen and are not shown at all on an HDR one: text drawn by a generic
// control vanished there.
WXDLLIMPEXP_CORE bool
wxWinUIShouldUsePaintedPixelsBuffer(const wxWindow *window);

// Such a window shows the material instead of a background of its own, so
// text written for that background, black on white typically, can end up
// unreadable. Called before text is drawn at these device coordinates of hdc
// in its current text colour: if hdc is the paint buffer of such a window and
// the text would not stand out from what lies behind it, returns true and
// the colour to draw with instead (same hue, opposite lightness).
WXDLLIMPEXP_CORE bool
wxWinUIAdaptTextColourOverBackdrop(WXHDC hdc, int x, int y,
                                   WXCOLORREF *colour);

// Non-client scrollbar presentation keeps USER32's geometry/range model.
// A non-null region is caller-owned and excludes only bands already presented
// by a ready WinUI peer. Null means use the unchanged native paint path.
WXDLLIMPEXP_CORE bool wxWinUIWindowScrollbarsProjected(WXHWND hwnd, int orient = 0);
// The window is being destroyed.
WXDLLIMPEXP_CORE void wxWinUIWindowScrollbarsForget(wxWindow *window);
WXDLLIMPEXP_CORE void wxWinUIWindowScrollbarModelChanged(
    wxWindow *window, int orient, bool nativePaint);
// Complete a range/page write after USER32 has possibly painted the track
// directly. Does not initialize a host, dispatch events or repaint the client.
WXDLLIMPEXP_CORE void wxWinUIWindowScrollbarModelCommitted(
    wxWindow *window, int orient);
WXDLLIMPEXP_CORE WXHANDLE wxWinUIWindowScrollbarPaintRegion(
    wxWindow *window, WXHANDLE updateRegion);

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
// Around a mouse message sent to a window shown by a XAML control: the
// focus is XAML's to give when such a control is clicked, and wxMSW does not
// give it to the window as it does to one which left the click unprocessed.
WXDLLIMPEXP_CORE void wxWinUIMSWBeginSlotMouseDispatch();
WXDLLIMPEXP_CORE void wxWinUIMSWEndSlotMouseDispatch();
// Counts the WM_CONTEXTMENU messages turned into wxEVT_CONTEXT_MENU, and
// tells whether the last one was processed: the host, which sends the mouse
// buttons to a control, learns from it what USER32 made of a right button.
WXDLLIMPEXP_CORE unsigned long long
wxWinUIMSWGetContextMenuSerial(bool *processed);
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

// True only during the synchronous bridge transfer of a current child focus
// request. Skip default TLW activation restoration at this boundary: the
// requested child has not acquired XAML focus yet, but remains authoritative.
WXDLLIMPEXP_CORE bool
wxWinUITLWHostShouldPreserveFocusOnActivate(wxWindow *window);

// End-of-freeze notification (wxTopLevelWindowMSW::DoThaw): run the single
// catch-up geometry flush the freeze held back.
WXDLLIMPEXP_CORE void wxWinUITLWHostNotifyThaw(wxWindow *window);

// Called for every message this thread retrieves from its queue, by the
// WH_GETMESSAGE hook and by the event-loop fallback. New physical input can
// only arrive through such a retrieval, which lets the pointer router tell
// the synchronous reflection of a message it is sending from a new sample.
WXDLLIMPEXP_CORE void wxWinUINoteMessagePumped();

// Diagnostic: with WX_WINUI_FORCE_SCALE set to a scale ("150" or "1.5"), the
// DPI wx reports for every window and display, which the island is then
// given too; 0 otherwise. Lets a display scale be tried, by hand or by the
// tests, without changing the settings of the monitor.
WXDLLIMPEXP_CORE int wxWinUIGetForcedDPI();

// Paint surfaces (src/winui/paintsurface.cpp): what a window paints goes to
// a bitmap which XAML shows in its place, instead of to its HWND. Setting the
// WX_WINUI_PAINT_SURFACES environment variable to 0 turns them off.
//
// Begin() is called with the rectangle BeginPaint() reported and returns the
// DC to paint on, or null when the window paints its HWND as before;
// BeginDirect() does the same for a wxClientDC, only for a window which
// already has a surface. Both are closed by End(). OwnsDC() tells a DC of a
// surface being painted from one of the window.
class WXDLLIMPEXP_FWD_CORE wxRect;
WXDLLIMPEXP_CORE bool wxWinUIPaintSurfacesEnabled();
WXDLLIMPEXP_CORE WXHDC
wxWinUIPaintSurfaceBegin(wxWindow *window,
                         int left, int top, int right, int bottom);
WXDLLIMPEXP_CORE WXHDC wxWinUIPaintSurfaceBeginDirect(wxWindow *window);
WXDLLIMPEXP_CORE bool wxWinUIPaintSurfaceOwnsDC(wxWindow *window, WXHDC hdc);
WXDLLIMPEXP_CORE void wxWinUIPaintSurfaceEnd(wxWindow *window);
// The window was asked to erase its background: the region its next
// painting covers starts transparent.
WXDLLIMPEXP_CORE void wxWinUIPaintSurfaceNoteErase(wxWindow *window);
WXDLLIMPEXP_CORE void
wxWinUIPaintSurfaceScroll(wxWindow *window,
                          int dx, int dy, const wxRect *rect);
// The window is being destroyed.
WXDLLIMPEXP_CORE void wxWinUIPaintSurfaceForget(wxWindow *window);
// Makes a rectangle of a surface being painted transparent again, in the
// coordinates of its DC; false if hdc is not the DC of such a surface.
WXDLLIMPEXP_CORE bool
wxWinUIPaintSurfaceClearRect(WXHDC hdc, int x, int y, int width, int height);
// A window moved, was resized or was shown: what its parent painted where it
// now is must not show through it.
WXDLLIMPEXP_CORE void wxWinUIPaintSurfaceNoteChildGeometry(wxWindow *child);
// Has XAML draw the border of a window which shows nothing of its HWND, and
// keeps it where the window is. Returns whether it does: the non-client
// band of the HWND is then left unpainted.
WXDLLIMPEXP_CORE bool wxWinUIWindowFrameSync(wxWindow *window);

// Gives a window which keeps no border in Win32 the frame of a control all
// the same: a control whose XAML content fills its window (a tree) counts on
// its client area being its whole window, and still has a frame in WinUI.
WXDLLIMPEXP_CORE void wxWinUIRequestWindowFrame(wxWindow *window);

// Whether the window was given such a frame.
WXDLLIMPEXP_CORE bool wxWinUIWindowHasRequestedFrame(const wxWindow *window);

// The two tones of a dialog of WinUI (ContentDialog): the content area, from
// the top of the client area down to the given height in pixels, has the
// lighter tone of a layer and a line under it; what is below, the row of the
// buttons, keeps the ground of the window. No content tone if the height is
// not positive.
WXDLLIMPEXP_CORE void wxWinUISetDialogContentTone(wxWindow *dialog, int height);

// The same room above and under the standard buttons of a dialog, in all
// dialogs: see dlgcmn.cpp. For a dialog about to be shown.
WXDLLIMPEXP_CORE void wxWinUINormalizeDialogCommands(wxWindow *dialog);

// A dialog of wx shown over the window it belongs to, when the program
// asked for that presentation of its dialogs
// (wxWinUIDialogPresentation::Overlay). It is made of controls of wx, each
// with its window, which a ContentDialog of XAML cannot hold: it stays a
// window of its own, without a title bar or a frame, with the corners and
// the shadow of a ContentDialog, in the middle of its owner and following
// it, its title in large letters above its content, and a veil over the
// owner while that one is disabled. The dialog keeps its events, its focus
// and its modality as they are.
//
// Begin is called before the dialog is shown, End after it is hidden; false
// if the dialog is shown in its own window as usual.
WXDLLIMPEXP_CORE bool wxWinUIBeginDialogCard(wxWindow *dialog);
WXDLLIMPEXP_CORE void wxWinUIEndDialogCard(wxWindow *dialog);
// What a dialog shown that way has around its client area, in pixels: the
// band of its title above it, and at its sides and under it what brings
// what the dialog shows to 24 from the edges, as the content of a
// ContentDialog is, whatever margins the dialog has of its own. All 0 for
// any other window.
struct wxWinUIDialogCardInsets
{
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;
};
WXDLLIMPEXP_CORE wxWinUIDialogCardInsets
wxWinUIGetDialogCardInsets(const wxWindow *dialog);
// Whether a dialog is shown that way over this window, which then keeps
// the look of the active window.
WXDLLIMPEXP_CORE bool wxWinUIDialogCardKeepsOwnerActive(const wxWindow *owner);
// The veil over a top-level window.
WXDLLIMPEXP_CORE void wxWinUISetWindowSmoke(wxWindow *tlw, bool on);

// A window about to be shown is shown with what it holds, not empty first.
// The controls of XAML get their templates and their layout when the island
// first lays itself out, which it does after the window is on screen: a
// dialog showed its frame, then its content a moment later. Begin makes that
// first layout happen now, while the window is hidden, and hides the window
// from the screen (DWM cloak) until the island has drawn it, or a short time
// has gone by; End shows it whatever happened.
WXDLLIMPEXP_CORE void wxWinUIBeginShowWithContent(wxWindow *tlw);
WXDLLIMPEXP_CORE void wxWinUIEndShowWithContent(wxWindow *tlw);

// The pointer was pressed in the island of this top-level window. The
// transient popup of wx which is open (the list of a wxComboCtrl...) is
// dismissed unless the press is in it, as it is in wxMSW by a press on any
// window of wx: the controls of XAML get their pointer from the island, and
// no window of wx sees that press.
WXDLLIMPEXP_CORE void wxWinUIDismissPopupOnOutsidePress(wxWindow *pressedTLW);

// wxNumberEntryDialog is the twin of the wxTextEntryDialog of WinUI: its
// message, and under it a NumberBox with its prompt as header, in the window
// of the dialog or over its parent as the program asked for its dialogs.
// Returns wxID_OK with the number in *value, or wxID_CANCEL.
class WXDLLIMPEXP_FWD_CORE wxDialog;
WXDLLIMPEXP_CORE int wxWinUIShowNumberEntry(wxDialog *dialog,
                                            const wxString& message,
                                            const wxString& prompt,
                                            long *value,
                                            long min, long max);
// The size of the client area of its window, in DIPs.
WXDLLIMPEXP_CORE wxSize wxWinUIGetNumberEntryClientSize(
    const wxWindow *dialog, const wxString& message, const wxString& prompt);
// Whether a window below this top-level one still paints its HWND (an OpenGL
// canvas does): the island then has to keep off its pixels when it erases.
WXDLLIMPEXP_CORE bool wxWinUIPaintSurfaceHasHwndPainters(WXHWND root);
// Start of WM_ERASEBKGND for a window which paints through a surface: the
// region to erase, taken from the DC of the message, becomes transparent
// and the DC returned is the one EVT_ERASE_BACKGROUND draws on, closed by
// End(). Null for a window without a surface.
WXDLLIMPEXP_CORE WXHDC
wxWinUIPaintSurfaceBeginErase(wxWindow *window, WXHDC hdcErase);
// A handler of EVT_ERASE_BACKGROUND drew the background of a window which
// has no surface: gives it one, so that the next erasing is drawn on it.
WXDLLIMPEXP_CORE void wxWinUIPaintSurfaceAdopt(wxWindow *window);

// The caret of a window which paints through a surface is an element of its
// canvas, since its HWND is not what shows. Show() places it, in client
// coordinates, and returns false for a window which paints its HWND, whose
// caret is the one of Win32; Hide() returns whether it had such a caret.
WXDLLIMPEXP_CORE bool
wxWinUICaretShow(wxWindow *window, int x, int y, int width, int height);
WXDLLIMPEXP_CORE bool wxWinUICaretHide(wxWindow *window);
WXDLLIMPEXP_CORE bool wxWinUICaretIsShown(wxWindow *window);

// Diagnostic: with WX_WINUI_FORCE_RTL set to anything but 0, windows which
// were not given a layout direction are laid out right to left.
WXDLLIMPEXP_CORE bool wxWinUIIsRightToLeftForced();

// Run now the wx notifications which XAML events have queued (clicks, item
// activations: see wxWinUIPostNotification()). The event loop does it before
// dispatching its next message; code which raises such an event itself and
// needs its effect at once, a test for instance, calls this.
WXDLLIMPEXP_CORE void wxWinUIFlushNotifications();

// Called for each key press retrieved from the queue: until the first one,
// focus set by the program shows no keyboard focus rectangle.
WXDLLIMPEXP_CORE void wxWinUINoteKeyboardInput();

// End of a top-level WM_SIZE (wxWindowMSW::MSWHandleMessage): the children
// have been laid out, so publish their geometry to the island now instead of
// waiting for the queued flush. During a live resize this keeps the XAML
// content attached to the window frame.
WXDLLIMPEXP_CORE void wxWinUITLWHostFlushAfterResize(wxWindow *tlw);

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
// False means a composite-cohort transaction kept/restored its source graph:
// the caller must roll back its exact native/logical reparent. Ordinary slot
// transfers retain their existing recoverable deferred-retry contract.
WXDLLIMPEXP_CORE bool wxWinUITLWHostNotifyReparent(wxWindow *window,
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
