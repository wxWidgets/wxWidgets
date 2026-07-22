/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/private/tlwhost.h
// Purpose:     one shared XAML island per top-level window
// Author:      wxWidgets development team
// Created:     2026-07-20
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PRIVATE_TLWHOST_H_
#define _WX_WINUI_PRIVATE_TLWHOST_H_

#include "wx/defs.h"

#if wxUSE_WINUI3

#include "wx/event.h"
#include "wx/gdicmn.h"
#include "wx/window.h"

#include "wx/msw/wrapwin.h"

#include <map>
#include <memory>
#include <set>
#include <vector>

// C++/WinRT's base.h uses std::array in constexpr code which the min/max
// macros from <windows.h> break; GetCurrentTime clashes with a WinRT method.
#ifdef GetCurrentTime
    #undef GetCurrentTime
#endif
#ifdef min
    #undef min
#endif
#ifdef max
    #undef max
#endif

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Microsoft.UI.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Hosting.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>

// Focus resolution entry point for src/msw/window.cpp (DoFindFocus).
WXDLLIMPEXP_CORE wxWindow *wxWinUITLWHostResolveFocus(WXHWND hwnd);

class wxWinUITopLevelHost;

// ----------------------------------------------------------------------------
// wxWinUISlotLifetime: the only bridge from a XAML callback back to wx.
//
// Every handler the host attaches for a slot captures a shared_ptr to this
// object instead of raw wxWindow/host pointers.  Disconnecting the slot
// invalidates it, so a callback that still fires afterwards (late dispatch,
// element kept alive by the framework) finds a null window/host and bails
// out instead of touching freed memory -- and a recycled wxWindow address
// can never be mistaken for the old window.  The live-instance counter lets
// the lifecycle tests prove that no callback state outlives its slot.
// ----------------------------------------------------------------------------

class wxWinUISlotLifetime
{
public:
    wxWinUISlotLifetime(wxWindow *window, wxWinUITopLevelHost *host)
        : m_window(window), m_host(host)
    {
        ++ms_liveCount;
    }

    ~wxWinUISlotLifetime() { --ms_liveCount; }

    bool IsValid() const { return m_window != nullptr; }
    wxWindow *GetWindow() const { return m_window; }
    wxWinUITopLevelHost *GetHost() const { return m_host; }

    void Invalidate()
    {
        m_window = nullptr;
        m_host = nullptr;
    }

    static unsigned GetLiveCount() { return ms_liveCount; }

private:
    wxWindow *m_window;
    wxWinUITopLevelHost *m_host;

    static unsigned ms_liveCount;

    wxDECLARE_NO_COPY_CLASS(wxWinUISlotLifetime);
};

// ----------------------------------------------------------------------------
// wxWinUISlot: one hosted control's entry in the shared per-TLW XAML tree.
//
// The slot owns a Grid container placed on the host's root Canvas at the
// control's TLW-client position (in DIPs), with pinned Width/Height, a Clip
// mirroring the wx ancestor clipping and a ZIndex mirroring the wx paint
// order.  The container has a single child: the control's XAML element.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxWinUISlot
{
public:
    wxWindow *GetWindow() const { return m_window; }

    winrt::Microsoft::UI::Xaml::Controls::Grid GetContainer() const
        { return m_container; }

    winrt::Microsoft::UI::Xaml::UIElement GetContent() const
        { return m_content; }

    // Replace the hosted element (e.g. wxTextCtrl swapping its TextBox for a
    // PasswordBox).  Passing null just detaches the current content.
    void SetContent(const winrt::Microsoft::UI::Xaml::UIElement& element);

    // Last synced geometry, in physical pixels relative to the TLW client
    // area (i.e. the area the island covers).
    const wxRect& GetRectInTLW() const { return m_rectPx; }

private:
    friend class wxWinUITopLevelHost;

    wxWinUISlot() = default;

    wxWindow *m_window = nullptr;
    WXHWND m_hwnd = nullptr;

    winrt::Microsoft::UI::Xaml::Controls::Grid m_container{ nullptr };
    winrt::Microsoft::UI::Xaml::UIElement m_content{ nullptr };

    wxRect m_rectPx;
    bool m_visible = false;

    // Additional clip: only the top portion of the slot (in physical px) is
    // rendered/hit-testable.  -1 == no limit.  Used by wxNotebook to keep the
    // TabView band off its page windows.
    int m_clipHeightPx = -1;

    // Snapshot of the ancestor chain (excluding the TLW) this slot holds a
    // geometry-tracking reference on: a child gets NO event when an ancestor
    // moves, resizes or is shown/hidden, so the host listens on the whole
    // chain (see wxWinUITopLevelHost::AddAncestorRef).
    std::vector<wxWindow *> m_ancestors;

    // Shared state captured by every XAML callback of this slot; invalidated
    // by DisconnectSlot() before anything is freed.
    std::shared_ptr<wxWinUISlotLifetime> m_lifetime;

    // Everything needed to revoke those callbacks: AddHandler() has no token,
    // removal needs the routed event and the very same boxed delegate.
    std::vector<std::pair<winrt::Microsoft::UI::Xaml::RoutedEvent,
                          winrt::Windows::Foundation::IInspectable>> m_routedHandlers;
    winrt::event_token m_gotFocusToken{};
    winrt::event_token m_lostFocusToken{};

    wxDECLARE_NO_COPY_CLASS(wxWinUISlot);
};

// ----------------------------------------------------------------------------
// wxWinUITopLevelHost: the single XAML island covering a top-level window.
//
// Validated topology (see samples/winuispike):
//  - the bridge HWND covers the whole TLW client area and is kept at the TOP
//    of the child z-order: an island's composition band renders relative to
//    the siblings' GDI according to the HWND z-order, so top is the only
//    position where XAML content is visible at all;
//  - the bridge and its inner InputSiteWindow (same UI thread; created
//    lazily by the framework) are subclassed and answer WM_NCHITTEST with
//    HTTRANSPARENT wherever there is no XAML content, which is what routes
//    native input to the wx windows below.  WS_EX_TRANSPARENT and
//    ContentIsland::IsHitTestVisibleWhenTransparent(false) do NOT work: they
//    make the pointer pipeline discard the input instead of forwarding it;
//  - the root Canvas is transparent and carries NO SystemBackdrop, so the
//    live GDI of the wx children shows through wherever XAML draws nothing.
//
// Threading invariants: everything runs on the single wx/XAML UI thread;
// geometry sync is coalesced through CallAfter, never done from inside a
// XAML callback; the island content is populated only once XamlRoot() is
// valid; the source is closed without clearing its Content first.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxWinUITopLevelHost : public wxEvtHandler
{
public:
    // Find the host of the top-level parent of the given window, creating it
    // on first use if requested.  Returns null if the window has no TLW
    // parent or if the island cannot be created (WinAppSDK missing...).
    static wxWinUITopLevelHost *ForWindow(wxWindow *window, bool createIfNeeded);

    // Find an existing host for this top-level window, if any.
    static wxWinUITopLevelHost *FindForTLW(const wxWindow *tlw);

    // Is this HWND the island bridge of one of the live hosts?  (Used by the
    // focus plumbing: ::GetFocus() on a bridge must resolve to the focused
    // slot's wx window, not to the TLW.)
    static wxWinUITopLevelHost *FindForBridge(HWND hwnd);

    // Focus resolution for wxWindow::FindFocus(): if the native focus HWND
    // belongs to one of the live islands, return the wx window owning the
    // focused slot (or the TLW when a free element has it); null otherwise.
    static wxWindow *ResolveFocusHwnd(WXHWND hwnd);

    // The wx window whose slot currently holds the XAML focus, if any.
    wxWindow *GetFocusOwner() const { return m_focusOwner; }

    // ----- slots -----

    // Register (or re-register) the XAML element of a wx child window.
    // Returns null on failure.  The slot is owned by the host.
    wxWinUISlot *RegisterSlot(wxWindow *window,
                              const winrt::Microsoft::UI::Xaml::UIElement& element);

    void UnregisterSlot(wxWindow *window);

    wxWinUISlot *FindSlot(const wxWindow *window) const;

    // Clip the slot to its top portion (physical px; <= 0 removes the limit).
    void SetSlotClipHeight(wxWindow *window, int physicalHeight);

    // Reflect a per-window wxCursor on the slot (null == default arrow).
    void SetSlotCursor(wxWindow *window,
                       const winrt::Microsoft::UI::Input::InputCursor& cursor);

    // Give the keyboard focus to a slot: native focus to the bridge, XAML
    // focus to the slot's element.
    void FocusSlot(wxWindow *window);

    // ----- geometry sync -----

    // Schedule a coalesced re-sync of one slot / of everything.
    void MarkDirty(wxWindow *window);
    void MarkAllDirty();

    // Run the pending sync immediately.
    void FlushSync();

    // ----- hit test -----

    // Does this screen point hit interactive XAML content (or any open
    // popup, which captures everything)?
    bool HitTestContent(const POINT& ptScreen);

    // ----- tree access -----

    winrt::Microsoft::UI::Xaml::Controls::Canvas Root() const { return m_root; }
    winrt::Microsoft::UI::Xaml::XamlRoot GetXamlRoot() const;
    HWND GetBridgeHwnd() const { return m_bridge; }
    wxWindow *GetTLW() const { return m_tlw; }

    // Map a screen point to the root canvas coordinate space (TLW-client
    // DIPs): the anchor space every transient surface (flyout, teaching tip)
    // shares.
    wxPoint ScreenToRootDIP(const wxPoint& screenPt) const;

    // Physical screen rect of the TLW client area, normalized left<right
    // even for mirrored (RTL) windows.  All island<->screen conversions go
    // through this rect: ScreenToClient/ClientToScreen flip the X axis on
    // WS_EX_LAYOUTRTL windows while island coordinates stay physically LTR.
    RECT GetClientScreenRect() const;

    // Effective DIP scale of the island.
    double GetScale() const;

    // Apply a XAML theme to the whole tree.
    void ApplyTheme(winrt::Microsoft::UI::Xaml::ElementTheme theme);

    // Apply a XAML theme to every live host.
    static void ApplyThemeToAll(winrt::Microsoft::UI::Xaml::ElementTheme theme);

    // ----- lifetime introspection (used by the lifecycle unit tests) -----

    // Number of live hosts / of registered slots across every host.
    static unsigned GetLiveHostCount();
    static unsigned GetLiveSlotCount();

    // Number of live per-slot callback states (wxWinUISlotLifetime): once
    // every hosted window of a TLW has been destroyed and the pending
    // dispatch has drained, this must come back down to what it was.
    static unsigned GetLiveSlotLifetimeCount()
        { return wxWinUISlotLifetime::GetLiveCount(); }

    // Cumulative balance of the per-slot XAML handler registrations
    // (AddHandler + GotFocus/LostFocus): every add must eventually be
    // matched by a revocation, on every removal path -- unregister,
    // registration rollback and host shutdown alike.
    static unsigned GetSlotHandlerAddCount();
    static unsigned GetSlotHandlerRevokeCount();

    // Cumulative count of scheduled (CallAfter posted) and actually executed
    // coalesced flushes: under Freeze() the schedule count must stay flat
    // instead of growing per MarkDirty(), and thawing runs one catch-up.
    static unsigned GetFlushScheduleCount();
    static unsigned GetFlushRunCount();

private:
    explicit wxWinUITopLevelHost(wxWindow *tlw);
    virtual ~wxWinUITopLevelHost();

    bool Initialize();
    void Shutdown();

    // Subclass procedure shared by the bridge and its inner input window.
    static LRESULT CALLBACK BridgeSubclassProc(HWND hwnd, UINT msg,
                                               WPARAM wParam, LPARAM lParam,
                                               UINT_PTR subclassId,
                                               DWORD_PTR refData);

    // The inner InputSiteWindow only exists once the island content has
    // realized; subclass it as soon as it shows up (idempotent).
    void EnsureInnerSubclass();

    // Keep the bridge above the wx children (new children are created above
    // it); called from the coalesced flush.
    void EnsureBridgeOnTop();

    // Cut the native scrollbar bands of the wx children OUT of the bridge
    // window region: there the island simply doesn't exist, so the real
    // input goes straight to the native scrollbar and its tracking loops
    // (thumb drag, arrow auto-repeat) work natively.
    void RebuildBridgeRegion();

    void ScheduleFlush();
    void SyncSlot(wxWinUISlot& slot);
    void RecomputeZOrder();

    // Input synthesis: XAML pointer/focus events on a slot container are
    // re-sent as the wx events the application expects on the slot's window.
    void BindSlotEvents(wxWinUISlot& slot);

    // The single disconnect transaction, shared by UnregisterSlot() and the
    // failure paths of RegisterSlot(): invalidate the callback state, revoke
    // every handler, detach the content, then drop the container from the
    // root.  After it returns no callback can reach wx through this slot.
    void DisconnectSlot(wxWinUISlot& slot);

    // First half of DisconnectSlot(), also run for every slot before the
    // island source is closed at shutdown: invalidate the shared callback
    // state and revoke every handler, leaving the visual tree untouched
    // (Close() needs the tree still attached).
    void RevokeSlotCallbacks(wxWinUISlot& slot);

    // Deterministic native routing: the island receives ALL the input (its
    // root is hit-testable everywhere) and forwards real WM_* messages --
    // client and non-client alike, so native scrollbars work -- to the
    // deepest wx window under the point whenever no XAML content is hit.
    // Native mouse capture then takes over for drags (scrollbar thumb...).
    void BindRootRouter();
    void OnRootPointer(UINT message,
                       const winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args);
    bool PointOverXamlContent(const wxPoint& ptClientPx);
    HWND FindNativeTargetAt(const POINT& ptScreen) const;

    // Mirror the native target's cursor onto the island root while hovering
    // non-XAML areas (the island otherwise paints its own arrow everywhere);
    // reset to the XAML default when back over real content.
    void MirrorNativeCursor(HWND target, LRESULT hitTest);
    void ResetRootCursor();

    // Force the island's input site to abandon any in-flight pointer
    // interaction.  Called after a press is forwarded to a native target: the
    // physical release then goes to that target (or into its modal loop), the
    // island never sees its own PointerReleased, and its input state machine
    // would otherwise stay stuck "pressed" and swallow all further input
    // (the rapid near-border resize freeze; WM_CANCELMODE was the only cure).
    void CancelIslandPointerState();
    void SendSlotMouseEvent(wxWindow *window,
                            wxEventType type,
                            const winrt::Microsoft::UI::Xaml::Controls::Grid& container,
                            const winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args);

    // wx event plumbing
    void OnSlotWindowGeometry(wxEvent& event);   // move/size/show
    void OnSlotWindowDestroy(wxWindowDestroyEvent& event);
    void OnSlotWindowFocus(wxFocusEvent& event);
    void OnTLWSize(wxSizeEvent& event);
    void OnTLWDpiChanged(wxDPIChangedEvent& event);
    void OnTLWDestroy(wxWindowDestroyEvent& event);

    // The island is releasing the focus (Tab pressed on its first/last
    // element): move it to the neighbouring wx tab stop.
    void OnTakeFocusRequested(bool previous);

    void BindSlotWindow(wxWinUISlot& slot);
    void UnbindSlotWindow(wxWinUISlot& slot);

    // Reference-counted geometry tracking on the ancestors of the slotted
    // windows: any ancestor move/size/show marks the whole host dirty.
    void AddAncestorRef(wxWindow *ancestor);
    void ReleaseAncestorRef(wxWindow *ancestor);
    void OnAncestorGeometry(wxEvent& event);
    void OnAncestorDestroy(wxWindowDestroyEvent& event);

    wxWindow *m_tlw = nullptr;

    winrt::Microsoft::UI::Xaml::Hosting::DesktopWindowXamlSource m_source{ nullptr };
    winrt::Microsoft::UI::Xaml::Controls::Canvas m_root{ nullptr };
    winrt::event_token m_takeFocusToken{};
    HWND m_bridge = nullptr;
    HWND m_inner = nullptr;

    std::map<wxWindow *, wxWinUISlot *> m_slots;

    // Ancestors currently listened on, with their reference count.
    std::map<wxWindow *, int> m_ancestorRefs;

    std::set<wxWindow *> m_dirty;
    bool m_allDirty = false;
    bool m_structureDirty = false;
    bool m_flushScheduled = false;
    // A flush ran while the TLW was frozen: one catch-up flush is owed at
    // thaw time (see wxWinUITLWHostNotifyThaw), instead of rescheduling in
    // a loop for the whole freeze.
    bool m_frozenDirty = false;
    bool m_shuttingDown = false;

    friend void wxWinUITLWHostNotifyThaw(wxWindow *window);

    // Focus arbiter: Win32 focus is the authority, XAML follows.
    wxWindow *m_focusOwner = nullptr;
    bool m_forwardingFocus = false;

    // Last native window the root router forwarded mouse input to, for
    // WM_MOUSELEAVE bookkeeping.
    HWND m_lastNativeTarget = nullptr;

    // Scrollbar cutouts of the current bridge region, to skip SetWindowRgn
    // when nothing changed.
    std::vector<RECT> m_regionCuts;

    // Cursor mirroring state: last (target, hit-test) pair and whether a
    // native cursor currently overrides the root's.
    HWND m_cursorTarget = nullptr;
    LRESULT m_cursorHit = 0;
    bool m_cursorMirrored = false;

    // Screen position of the last pointer event the island delivered; feeds
    // the spurious-WM_SETCURSOR filter in BridgeSubclassProc (see there).
    POINT m_lastPointerScreen = { -100000, -100000 };

    // Phantom-replay breaker state (see OnRootPointer): consecutive island
    // moves at the exact same position -- a real mouse never produces those,
    // the frozen input site replays one by the tens of thousands per second.
    wxPoint m_lastMovePx = wxPoint(-100000, -100000);
    unsigned m_samePosMoves = 0;

    wxDECLARE_NO_COPY_CLASS(wxWinUITopLevelHost);
};

#endif // wxUSE_WINUI3

#endif // _WX_WINUI_PRIVATE_TLWHOST_H_
