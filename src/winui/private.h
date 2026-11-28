/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/private.h
// Purpose:     private wxWinUI helpers
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PRIVATE_H_
#define _WX_WINUI_PRIVATE_H_

#include "wx/window.h"
#include "wx/weakref.h"

#include "wx/msw/wrapwin.h"

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

#ifdef GetCurrentTime
    #undef GetCurrentTime
#endif

// C++/WinRT's base.h uses std::array in constexpr code which the min/max macros
// from <windows.h> break (they may be pulled in, without NOMINMAX, by wx headers
// included before this one).  Undefine them before including any winrt header.
#ifdef min
    #undef min
#endif
#ifdef max
    #undef max
#endif

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Graphics.h>
#include <winrt/Windows.UI.h>
#include <winrt/Microsoft.UI.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Interop.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Hosting.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.System.h>

// The per-control host: since the one-island-per-TLW re-architecture this is
// a thin PROXY over wxWinUITopLevelHost (wx/winui/private/tlwhost.h) -- the
// control's XAML element lives as a slot in the shared per-top-level island,
// and everything (composition, geometry sync, input routing/synthesis, focus)
// is done by the TLW host.  The historical 7-method contract is preserved so
// the ~30 control implementations stay untouched.
//
// The proxy deliberately caches no pointer to the TLW host or the slot: both
// are looked up on demand through the registry, which keeps it safe against
// every teardown/reparenting order (the TLW host dies with its TLW and takes
// the slots with it).
class wxWinUILoadedState;
// Keep callback-bearing XAML peer graphs alive until either their normal
// dispatcher retirement barrier or the XAML framework shutdown boundary
// consumes the same exact-once ticket. Once dispatcher rundown begins, normal
// completion is frozen: only XamlShutdownCompletedOnThread, while holding its
// DispatcherQueue deferral, may release a retained graph.
using wxWinUIFrameworkRetirementId = std::uint64_t;
wxWinUIFrameworkRetirementId wxWinUIRegisterFrameworkRetirement(
    const winrt::Microsoft::UI::Dispatching::DispatcherQueue& queue,
    std::function<void ()> complete) noexcept;
void wxWinUICompleteFrameworkRetirement(
    wxWinUIFrameworkRetirementId id) noexcept;

// Arm the process-wide UI DispatcherQueue before any control can enter its
// detach path. All four queue phase hooks are required for a publishable
// runtime; the companion XamlShutdownCompletedOnThread hook is armed by the
// bootstrap code.
bool wxWinUIBeginFrameworkRetirementRuntime(
    const winrt::Microsoft::UI::Dispatching::DispatcherQueue& queue) noexcept;

// Some WinUI controls own callback-bearing popup/template graphs outside the
// shared island's normal visual tree. Physical detachment of their slot is
// allowed only after the control-specific gate has proved that the exact
// popup and all focus transitions caused by closing it have retired. The
// completion callback must never capture a wx object: the host installs one
// using only its invalidatable lifetime plus the slot's stable retirement id.
class wxWinUIPhysicalDisconnectGate
{
public:
    virtual ~wxWinUIPhysicalDisconnectGate() = default;

    virtual bool IsSatisfied() const noexcept = 0;
    virtual bool IsDegraded() const noexcept = 0;
    virtual bool IsSealedForHostShutdown() const noexcept = 0;
    virtual winrt::Microsoft::UI::Dispatching::DispatcherQueue
        GetDispatcherQueue() const noexcept = 0;

    // Replaces any previous destination. If the causal boundary was already
    // crossed, the new destination is invoked immediately. This retargeting
    // is what lets a TLW shutdown take ownership after its host lifetime has
    // first been invalidated.
    virtual bool SetCompletion(std::function<void ()> complete) noexcept = 0;

    // Called before TLW logical shutdown crosses its first callback-bearing
    // boundary. It keeps the gate observable even while currently satisfied
    // and makes every later popup publication fail closed/degraded.
    virtual void SealForHostShutdown() noexcept = 0;

    // Last pre-Source.Close observation. XAML Unloaded can clear popup-open
    // properties, so adapters latch any published-but-not-yet-observed debt
    // here while the source still exposes its live control graph.
    virtual void LatchPublishedPopupDebtBeforeSourceClose() noexcept = 0;
    virtual bool CompleteSealedWithoutPopup() noexcept = 0;

    // Called only by the framework-retirement callback while it owns the
    // terminal XAML release boundary. It revokes/forgets every remaining
    // projection callback without calling the normal slot completion.
    virtual void CompleteAtXamlBoundary() noexcept = 0;
};

// Stable per-peer fan-in. Children may be added while the control is live;
// once the host installs a completion destination, no new child is admitted.
// This composes the ComboBox template popup with transient surfaces such as
// its autocomplete MenuFlyout without turning either one's queue turn into a
// proof for the other.
WXDLLIMPEXP_CORE std::shared_ptr<wxWinUIPhysicalDisconnectGate>
wxWinUICreateCompositePhysicalDisconnectGate();
WXDLLIMPEXP_CORE bool wxWinUIAddCompositePhysicalDisconnectGate(
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& composite,
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& child,
    bool reusable = false) noexcept;

// A degraded gate has lost its causal observation (missing exact popup,
// failed hook, rejected queue tail, ...). Refuse every later peer publication
// process-wide so exceptional retained graphs are bounded by the peers which
// were already live at the first fault, rather than by process uptime.
WXDLLIMPEXP_CORE void
wxWinUINotePhysicalDisconnectGateDegraded() noexcept;
WXDLLIMPEXP_CORE bool
wxWinUIIsPhysicalDisconnectPublicationPoisoned() noexcept;

// A direct native composite collection keeps the concrete child type intact.
// Callbacks capture only native/weak state, never raw wx owners. insert/remove
// act on the exact element identity and contains verifies the committed mount.
// The host owns insertion and physical removal, including deferred retirement.
struct wxWinUIDirectCompositeMount
{
    winrt::weak_ref<winrt::Microsoft::UI::Xaml::FrameworkElement> owner;
    std::function<bool(const winrt::Microsoft::UI::Xaml::UIElement&)> insert;
    std::function<bool(const winrt::Microsoft::UI::Xaml::UIElement&)> contains;
    std::function<bool(const winrt::Microsoft::UI::Xaml::UIElement&)> remove;
};

// A template can present its still-owned content outside its visual subtree
// (for example, in its exact Flyout). The adapter witnesses that ownership,
// including current template/part identities; sharing a XamlRoot is not proof.
// Keep these fields immutable after publication. Callbacks must not capture
// raw wx owners, and the adapter supplies any popup retirement gate separately.
struct wxWinUICompositeContentExtension
{
    winrt::weak_ref<winrt::Microsoft::UI::Xaml::FrameworkElement> root;
    std::function<bool()> isCurrent;
};

// Shared production peer host for core controls and optional libraries such as
// wxribbon. This source-private header is not part of the installed API.
class WXDLLIMPEXP_CORE wxWinUIControlHost
{
public:
    wxWinUIControlHost();
    ~wxWinUIControlHost();

    bool Initialize(wxWindow *window);
    void SetPhysicalDisconnectGate(
        const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate);
    void Close(
        const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate = {});

    bool IsOk() const;

    bool SetContent(const winrt::Microsoft::UI::Xaml::UIElement& element);
    bool SetContent(
        const winrt::Microsoft::UI::Xaml::UIElement& visualRoot,
        const winrt::Microsoft::UI::Xaml::UIElement& semanticTarget);
    // Publish into a native composite's content mount, retaining this child's
    // logical slot, state carrier, input identity and accessibility authority.
    bool SetContentInParent(
        const winrt::Microsoft::UI::Xaml::UIElement& visualRoot,
        const winrt::Microsoft::UI::Xaml::UIElement& semanticTarget,
        wxWindow *compositeOwner,
        const winrt::Microsoft::UI::Xaml::Controls::ContentControl& contentMount);
    // The adapter owns this Control's enabled/visibility state. Its identity
    // remains fixed until Close(): replacement is rejected before mutation.
    bool SetContentInParentDirect(
        const winrt::Microsoft::UI::Xaml::Controls::Control& visualRoot,
        const winrt::Microsoft::UI::Xaml::UIElement& semanticTarget,
        wxWindow *compositeOwner,
        const std::shared_ptr<wxWinUIDirectCompositeMount>& mount);
    // Only a published peer can admit one exact extended content subtree.
    // Its identity and the parent's content remain fixed until Close().
    // Initial registration may be dormant until its template is ready: every
    // use still checks isCurrent(). Repeating the same extension revalidates
    // it; refusal changes nothing.
    bool SetCompositeContentExtension(
        const std::shared_ptr<wxWinUICompositeContentExtension>& extension);
    void ClearContent();

    // Apply an element theme to the hosted content, if it is a FrameworkElement.
    void ApplyTheme(winrt::Microsoft::UI::Xaml::ElementTheme theme);

    // Historically: force the per-control island to recompose (islands only
    // committed a frame on a bridge resize).  In the shared tree a plain
    // layout invalidation of the slot container is enough -- and mostly
    // redundant, since XAML invalidates on property changes by itself.
    void ForceRender();

    // No-op kept for source compatibility: the per-island Mica backdrop is
    // gone, the DWM window backdrop shows through the transparent island.
    void ApplyBackdropMaterial();

    HWND GetHostHWND() const;
    HWND GetBridgeHWND() const;
    bool ContainsFocus(HWND hwnd) const;
    bool NavigateFocus(bool forward);

    // Composite controls can nominate the actual editable/focusable template
    // part. The TLW host keeps only a weak reference and validates that it
    // belongs to this slot's current content generation.
    void SetPreferredFocus(
        const winrt::Microsoft::UI::Xaml::UIElement& element);

    // Override the label-derived automation name contributed by the shared
    // slot state adapter. An empty value explicitly returns to the normal
    // wx label/name derivation. The slot remains the sole XAML Name writer.
    void SetAutomationNameOverride(const wxString& name);

    // Composite template parts may not exist until the coalesced slot flush.
    // Use only on an explicit synchronous focus request.
    void SynchronizeForFocus();

    // Limit the slot to an exact local physical-LTR client rectangle (in
    // physical pixels). Empty rectangles intentionally hide the slot; call
    // ClearBridgeClipRect() to return to the full client area. The shared
    // host stores DIPs so the contract survives DPI/TLW migration.
    void SetBridgeClipRect(const wxRect& physicalRect);
    void ClearBridgeClipRect();

    // Compatibility wrapper for the original top-band contract. Pass 0 to
    // fill the whole client area (the default).
    void SetBridgeHeightLimit(int physicalHeight);

    // Reflect a wxWindow::SetCursor() on the slot (per-window cursor, e.g.
    // setting a wait cursor on a single control).  Mapped to ProtectedCursor.
    void ApplyWxCursor(const wxCursor& cursor);

    // The natural size of the hosted XAML content, in physical pixels, or
    // wxDefaultSize if it can't be measured (no content yet).  The element is
    // not size-pinned any more (the slot container is), so this is a plain
    // unconstrained Measure().
    wxSize MeasureContent() const;

    // True once the hosted content has been loaded in a live visual tree, i.e.
    // once its template is applied and MeasureContent() is meaningful.
    bool IsContentLoaded() const { return m_contentLoaded; }

    // Production identity snapshot of the currently committed content.
    winrt::Microsoft::UI::Xaml::UIElement GetContent() const
        { return m_content; }

    // The wx control whose slot this is.
    wxWindow *HostedWindow() const { return m_window; }

private:
    friend class wxWinUIControlHostTestAccess;

    void OnContentLoaded();
    void RevokeLoadedHook();
    bool SetContentImpl(
        const winrt::Microsoft::UI::Xaml::UIElement& visualRoot,
        const winrt::Microsoft::UI::Xaml::UIElement& semanticTarget,
        wxWindow *compositeOwner,
        const winrt::Microsoft::UI::Xaml::Controls::ContentControl& contentMount,
        const std::shared_ptr<wxWinUIDirectCompositeMount>& directMount = {});

    wxWindow *m_window = nullptr;
    winrt::Microsoft::UI::Xaml::UIElement m_content{ nullptr };
    winrt::event_token m_loadedToken{};
    std::shared_ptr<wxWinUILoadedState> m_loadedState;
    std::shared_ptr<wxWinUIPhysicalDisconnectGate>
        m_physicalDisconnectGate;
    bool m_contentLoaded = false;
};

// Current element theme requested by the application (Default == follow system).
winrt::Microsoft::UI::Xaml::ElementTheme wxWinUIGetCurrentElementTheme();
bool wxWinUIIsDarkTheme();

WXDLLIMPEXP_CORE void
wxWinUILogException(const char *what, const winrt::hresult_error& e);
WXDLLIMPEXP_CORE winrt::hstring wxWinUIToHString(const wxString& str);
WXDLLIMPEXP_CORE wxString wxWinUIFromHString(const winrt::hstring& str);

// Convert a wxBitmap to a WinUI WriteableBitmap (premultiplied BGRA).  Returns
// nullptr for an invalid bitmap.
class wxBitmap;
WXDLLIMPEXP_CORE winrt::Microsoft::UI::Xaml::Media::Imaging::WriteableBitmap
wxWinUIWriteableBitmapFromBitmap(const wxBitmap& bitmap);

// Peer state shared by wxChoice and wxComboBox (which reconfigures the same
// WinUI ComboBox to be editable).
class wxChoice;
class wxWinUITextCallbackState;

// Every WinRT delegate keeps this state alive instead of capturing the wx
// control. Close() invalidates the owner before attempting any revocation, so
// a failed revocation or a queued XAML notification can only observe nullptr.
// WinUI callbacks and wx destruction both run on the UI thread; the atomic
// additionally makes the invalidation visible if a projection releases a
// delegate from another thread.
class wxWinUIChoiceCallbackState
{
public:
    explicit wxWinUIChoiceCallbackState(wxChoice *owner)
        : m_owner(owner)
    {
    }

    wxChoice *GetOwner() const
    {
        return m_owner.load(std::memory_order_acquire);
    }

    void Invalidate()
    {
        m_owner.store(nullptr, std::memory_order_release);
    }

    void BeginPeerMutation()
    {
        m_peerMutationDepth.fetch_add(1, std::memory_order_acq_rel);
    }

    void EndPeerMutation()
    {
        const unsigned previous =
            m_peerMutationDepth.fetch_sub(1, std::memory_order_acq_rel);
        wxASSERT_MSG(previous != 0,
                     "unbalanced WinUI peer mutation guard");
    }

    bool IsPeerMutationInProgress() const
    {
        return m_peerMutationDepth.load(std::memory_order_acquire) != 0;
    }

private:
    std::atomic<wxChoice *> m_owner;
    std::atomic<unsigned> m_peerMutationDepth{0};
};

class wxWinUIComboPopupRetirementState;
class wxWinUIFlyoutPopupRetirementState;
class wxWinUICalendarPopupRetirementState;

// A CalendarDatePicker editor owns the exact template AttachedFlyout. Arm
// before publication; native Opening establishes the same popup/focus debt as
// ComboBox, including a close requested before Opened has been delivered.
WXDLLIMPEXP_CORE std::shared_ptr<wxWinUICalendarPopupRetirementState>
wxWinUICreateCalendarPopupRetirementState(
    const winrt::Microsoft::UI::Xaml::Controls::CalendarDatePicker& picker);
WXDLLIMPEXP_CORE std::shared_ptr<wxWinUIPhysicalDisconnectGate>
wxWinUIGetCalendarPopupRetirementGate(
    const std::shared_ptr<wxWinUICalendarPopupRetirementState>& state) noexcept;
WXDLLIMPEXP_CORE void wxWinUIRequestCalendarPopupClose(
    const std::shared_ptr<wxWinUICalendarPopupRetirementState>& state) noexcept;

WXDLLIMPEXP_CORE std::shared_ptr<wxWinUIFlyoutPopupRetirementState>
wxWinUICreateFlyoutPopupRetirementState(
    const winrt::Microsoft::UI::Xaml::Controls::Primitives::FlyoutBase& flyout,
    const winrt::Microsoft::UI::Xaml::XamlRoot& xamlRoot,
    const winrt::Microsoft::UI::Xaml::DependencyObject& exactItem) noexcept;
WXDLLIMPEXP_CORE std::shared_ptr<wxWinUIPhysicalDisconnectGate>
wxWinUIGetFlyoutPopupRetirementGate(
    const std::shared_ptr<wxWinUIFlyoutPopupRetirementState>& state) noexcept;
WXDLLIMPEXP_CORE void wxWinUIResolveFlyoutPopupAfterShow(
    const std::shared_ptr<wxWinUIFlyoutPopupRetirementState>& state) noexcept;
WXDLLIMPEXP_CORE void wxWinUIFailFlyoutPopupShow(
    const std::shared_ptr<wxWinUIFlyoutPopupRetirementState>& state) noexcept;
WXDLLIMPEXP_CORE bool wxWinUISetFlyoutPopupRetirementObserver(
    const std::shared_ptr<wxWinUIFlyoutPopupRetirementState>& state,
    std::function<void ()> observer) noexcept;
WXDLLIMPEXP_CORE std::shared_ptr<wxWinUIComboPopupRetirementState>
wxWinUICreateComboPopupRetirementState(
    const winrt::Microsoft::UI::Xaml::Controls::ComboBox& combo);
WXDLLIMPEXP_CORE std::shared_ptr<wxWinUIPhysicalDisconnectGate>
wxWinUIGetComboPopupRetirementGate(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept;
WXDLLIMPEXP_CORE void wxWinUIRequestComboPopupClose(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept;
WXDLLIMPEXP_CORE void wxWinUIObserveComboPopupClosed(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept;
WXDLLIMPEXP_CORE bool wxWinUIIsComboPopupRetirementSatisfied(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept;
WXDLLIMPEXP_CORE bool wxWinUIIsComboPopupRetirementSealed(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept;
WXDLLIMPEXP_CORE bool wxWinUIArmComboPopupRetirementCompletion(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state,
    std::function<void ()> completion) noexcept;
WXDLLIMPEXP_CORE bool wxWinUIObserveComboPopupOpened(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept;
WXDLLIMPEXP_CORE bool wxWinUIResolveComboPopupAfterOpen(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept;
WXDLLIMPEXP_CORE bool wxWinUIPrepareComboPopupOpen(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept;
WXDLLIMPEXP_CORE void wxWinUICancelPreparedComboPopupOpen(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept;
WXDLLIMPEXP_CORE bool wxWinUIFinishPreparedComboPopupOpen(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept;
WXDLLIMPEXP_CORE void wxWinUIAbortPreparedComboPopupOpen(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept;

// ----------------------------------------------------------------------------
// wxWinUIDialogPresenter: shows a WinUI-drawn dialog, either as a real
// top-level window (the default) or as a ContentDialog overlaying the parent,
// depending on wxWinUIGetDialogPresentation().
//
// Callers describe the dialog abstractly -- a title, a XAML body, and up to
// three buttons -- and get back the id of the button that dismissed it.  This
// keeps every common dialog free of any presentation-specific code.
// ----------------------------------------------------------------------------

class wxDialog;

class wxWinUIDialogPresenter
{
public:
    wxWinUIDialogPresenter();
    ~wxWinUIDialogPresenter();

    wxWinUIDialogPresenter(const wxWinUIDialogPresenter&) = delete;
    wxWinUIDialogPresenter& operator=(const wxWinUIDialogPresenter&) = delete;

    // Prepare the dialog; false if WinUI is unavailable and the caller should
    // fall back to a native dialog.
    bool Create(wxWindow *parent, const wxString& title);

    // Window mode uses this public dialog's HWND instead of creating a shell.
    // The caller keeps its external modal lifetime guard around ShowModal().
    bool CreateForDialog(wxDialog *dialog);

    // Client size in DIPs, including the presenter margins and command row.
    // Common dialogs use this once at Create(), before application resizing.
    static wxSize GetWindowClientSize(const wxSize& contentSize,
                                      size_t buttonCount);

    // The body of the dialog.
    void SetContent(winrt::Microsoft::UI::Xaml::UIElement const& content);

    // Natural size of the body, in DIPs.  Used to size the dialog window; the
    // XAML content can't be measured reliably before it is realised, so the
    // caller has to say how much room it needs.
    void SetContentSize(const wxSize& dipSize) { m_contentSize = dipSize; }

    // Add a button. Window presentation supports the full wx button matrix.
    // Overlay maps up to three buttons to ContentDialog's
    // primary/secondary/close slots and falls back to Window above that
    // framework limit. Exactly one button should be marked as the default.
    void AddButton(int id, const wxString& label, bool isDefault = false);

    // Called when a button is about to dismiss the dialog; returning false
    // keeps the dialog open (used to report a failed validation).
    void SetAcceptHandler(std::function<bool (int)> handler)
        { m_onAccept = std::move(handler); }

    // Disable Escape/title-bar close/ContentDialog external dismissal when
    // the public contract requires an explicit command (e.g. wxYES_NO).
    void SetExternalDismissAllowed(bool allowed)
        { m_canDismissExternally = allowed; }

    // The public wx dialog object can be destroyed by validation/event code
    // while the presenter surface is nested. Tie that source lifetime to the
    // Window/Overlay loop so destruction always cancels and wakes it.
    void SetLifetimeOwner(wxWindow *owner)
    {
        m_lifetimeOwner = owner;
        m_hasLifetimeOwner = owner != nullptr;
    }

    // Show the dialog modally and return the id of the button that dismissed
    // it, or wxID_CANCEL if it was closed some other way.
    int ShowModal();

private:
    struct Button
    {
        int id = wxID_NONE;
        wxString label;
        bool isDefault = false;
    };

    int ShowAsWindow();
    int ShowAsOverlay();
    wxDialog *GetCurrentWindow() const;
    bool LifetimeOwnerIsAlive() const
    {
        if ( !m_hasLifetimeOwner )
            return true;

        wxWindow * const owner = m_lifetimeOwner.get();
        return owner && !owner->IsBeingDeleted();
    }

    wxWeakRef<wxWindow> m_parent;
    wxWeakRef<wxWindow> m_window;
    WXHWND m_windowHwnd = nullptr;
    unsigned long long m_windowGeneration = 0;
    bool m_usesExistingWindow = false;
    wxString m_title;
    winrt::Microsoft::UI::Xaml::UIElement m_content{ nullptr };
    wxSize m_contentSize{ 320, 120 };
    std::vector<Button> m_buttons;
    std::function<bool (int)> m_onAccept;
    bool m_canDismissExternally = true;
    wxWeakRef<wxWindow> m_lifetimeOwner;
    bool m_hasLifetimeOwner = false;
};

// Strip '&' mnemonics ("&&" -> "&"): WinUI labels don't support them.
wxString wxWinUIRemoveMnemonics(const wxString& label);

// Set wrapped, selectable text on a dialog TextBlock.
void wxWinUISetDialogText(winrt::Microsoft::UI::Xaml::Controls::TextBlock const& text,
                          const wxString& value);

// Set (or clear, when tip is empty) a WinUI tooltip on a hosted XAML element.
// Shared by the wxWinUI controls so that wxWindow::SetToolTip() actually shows
// a tooltip over the XAML island instead of being silently dropped.
// Returns true when the state was applied (a null element is a successful
// no-op), false when the XAML setter failed.
bool wxWinUISetToolTip(
    const winrt::Microsoft::UI::Xaml::UIElement& element,
    const wxString& tip,
    wxWindow *owner = nullptr,
    winrt::Windows::Foundation::IInspectable *appliedValue = nullptr,
    // -2 means snapshot the current policy for a newly-managed XAML peer.
    // A wxToolTip-backed slot passes its logical object's creation snapshot.
    int maximumWidth = -2);

// Relinquish a tooltip previously installed by wxWinUISetToolTip(). The
// original LOCAL value is restored only while the exact expected wx-owned
// object (or its globally-suppressed null) is still authoritative. Passing
// DependencyProperty::UnsetValue() clears the local value so Style/resource
// precedence is recovered. A direct XAML replacement wins. The registry entry
// is always released by identity.
bool wxWinUIRestoreToolTip(
    const winrt::Microsoft::UI::Xaml::UIElement& element,
    const winrt::Windows::Foundation::IInspectable& originalLocalValue,
    const winrt::Windows::Foundation::IInspectable& expectedWxValue);

// Retire the helper-registry entry for an element during peer teardown.
// Unlike SetToolTip(""), this never clears a direct application replacement.
bool wxWinUIForgetManagedToolTip(
    const winrt::Microsoft::UI::Xaml::UIElement& element);

bool wxWinUIIsManagedToolTipSuppressed(
    const winrt::Microsoft::UI::Xaml::UIElement& element,
    const winrt::Windows::Foundation::IInspectable& expectedWxValue);
bool wxWinUIIsManagedToolTipCurrent(
    const winrt::Microsoft::UI::Xaml::UIElement& element,
    const winrt::Windows::Foundation::IInspectable& expectedWxValue);

// Find the next keyboard-focusable control after hwndCurrent in the TLW tab
// order, in either direction.  GetNextDlgTabItem() only honours WS_TABSTOP:
// the wx-side veto (AcceptsFocusFromKeyboard(), e.g. after
// DisableFocusFromKeyboard()) lives above it, so the vetoed windows are
// walked past. Foreign HWNDs (including WinAppSDK infrastructure) are skipped;
// native implementation subwindows remain eligible only when their wx control
// explicitly claims them through ContainsHWND(). Bounded by coming back around
// to the FIRST candidate seen; returns null when every candidate refuses the
// keyboard focus. Shared by the Tab pre-translation and the island's
// TakeFocusRequested exit.
HWND wxWinUIFindNextKeyboardFocusable(HWND hwndTLW, HWND hwndCurrent,
                                      bool previous);

// Erase the background of a XAML island window (the bridge covering the whole
// client area of a top-level window, or its input site) with the same surface
// as the wx windows below it: black under a DWM material, so that Windows
// substitutes Mica for it, and the top-level window's own background colour
// when no material is active. Returns false, without painting anything, if
// there is nothing to erase.
bool wxWinUIEraseIslandBackground(HWND island, HDC hdc);

// Diagnostics: how many deep content relayouts have been scheduled.
// One per hosted control load is expected; a stream of them while the
// mouse moves means the window is laying itself out for every sample.
unsigned wxWinUIGetContentRelayoutCount();

// True when this optimisation was taken out by WX_WINUI_DISABLE; see
// the definition for the names it understands.
bool wxWinUIOptimisationDisabled(const char *name);

inline winrt::Microsoft::UI::Xaml::Media::SolidColorBrush
wxWinUIBrush(unsigned char red,
             unsigned char green,
             unsigned char blue,
             unsigned char alpha = 255)
{
    winrt::Windows::UI::Color color{};
    color.A = alpha;
    color.R = red;
    color.G = green;
    color.B = blue;
    return winrt::Microsoft::UI::Xaml::Media::SolidColorBrush(color);
}

// Queue a wx notification raised by a XAML event (a click, the activation
// of an item) instead of sending it from inside the XAML callback. The
// application handler may open a modal dialog or destroy the control, and a
// message loop must not be nested in a XAML delegate nor under the host's own
// pointer routing. The notification runs from the event loop, in the order
// queued and before the next message is dispatched; it must look its control
// up again, as the control may be gone by then.
void wxWinUIPostNotification(std::function<void()> notification);

// Run the queued notifications now (also declared for the tests in
// wx/winui/private/tlwhostmsw.h).
WXDLLIMPEXP_CORE void wxWinUIFlushNotifications();

// The focus state to use when the program, not the user, moves focus to a
// XAML element: it shows no keyboard focus rectangle before the keyboard has
// been used (see hostfocus.cpp).
winrt::Microsoft::UI::Xaml::FocusState wxWinUIProgrammaticFocusState();

// The same for a known target: text entry controls, which show no focus
// rectangle and need the programmatic state to focus their editor, always
// get it.
winrt::Microsoft::UI::Xaml::FocusState wxWinUIProgrammaticFocusState(
    const winrt::Microsoft::UI::Xaml::Controls::Control& target);

#endif // _WX_WINUI_PRIVATE_H_
