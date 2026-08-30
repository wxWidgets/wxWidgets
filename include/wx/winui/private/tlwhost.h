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
#include "wx/cursor.h"
#include "wx/weakref.h"
#include "wx/window.h"

#include "wx/msw/wrapwin.h"
#include "wx/winui/private/inputrouter.h"
#include "wx/winui/private/inputstate.h"
#include "wx/winui/private/nativeresize.h"

#include <map>
#include <memory>
#include <array>
#include <cstdint>
#include <deque>
#include <functional>
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

// The MSW-side entry points (focus resolution, thaw/enable notifications)
// live in their own small header so src/msw code can call them without
// pulling the projection in.
#include "wx/winui/private/tlwhostmsw.h"

class wxWinUITopLevelHost;
class wxWinUIVisualCoordinates;
class wxWinUIDropBroker;
class wxWinUIDropBrokerState;
class wxWinUIToolTipPropertyState;
class wxWinUIAutomationNameStylePropertyState;
class wxWinUIContentLoadedState;
class wxWinUIPhysicalDisconnectGate;
struct IRawElementProviderSimple;

struct wxWinUIClipRectDIP
{
    double x = 0.0;
    double y = 0.0;
    double width = 0.0;
    double height = 0.0;

    bool operator==(const wxWinUIClipRectDIP& other) const
    {
        return x == other.x && y == other.y &&
               width == other.width && height == other.height;
    }
};

// Diagnostic result returned by the root-pointer adapter seam.  Tests feed
// an already-normalized sample through the exact production core: this is
// deliberately an observation of the real route, not a second state machine.
enum class wxWinUIRootPointerStatus
{
    Completed,
    PolicyFiltered,
    SlotReleaseConsumed,
    FailedClosed,
    Superseded
};

struct wxWinUIRootPointerOutcome
{
    wxWinUIRootPointerStatus status =
        wxWinUIRootPointerStatus::FailedClosed;
    wxWinUIInputDisposition disposition =
        wxWinUIInputDisposition::Indeterminate;
    wxWinUIInputStormDecision stormDecision =
        wxWinUIInputStormDecision::Deliver;
    unsigned preparedActionCount = 0;
    // The press/release action prepared by the real state machine, when one
    // exists.  This remains diagnostic-only but lets adapter tests retain the
    // exact gesture serial instead of fabricating one.
    wxWinUIInputAction pointerDispatch;
    bool cancelSource = false;
};

// ----------------------------------------------------------------------------
// wxWinUIHostLifetime: invalidatable state captured by callbacks belonging to
// the island itself (root router and TakeFocusRequested).
//
// Keep this separate from wxWinUISlotLifetime: host callbacks don't belong to
// a slot and must not make the slot-lifetime leak counter lie.
// ----------------------------------------------------------------------------

class wxWinUIHostLifetime
{
public:
    explicit wxWinUIHostLifetime(wxWinUITopLevelHost *host)
        : m_host(host)
    {
        ++ms_liveCount;
    }

    ~wxWinUIHostLifetime() { --ms_liveCount; }

    wxWinUITopLevelHost *GetHost() const { return m_host; }
    void Invalidate() { m_host = nullptr; }

    static unsigned GetLiveCount() { return ms_liveCount; }

private:
    wxWinUITopLevelHost *m_host;

    static unsigned ms_liveCount;

    wxDECLARE_NO_COPY_CLASS(wxWinUIHostLifetime);
};

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

    // The visual root is the object physically attached to the carrier and
    // therefore owns layout, clipping, tooltip, Loaded and focus traversal.
    winrt::Microsoft::UI::Xaml::UIElement GetVisualRoot() const
        { return m_content; }

    // UIA properties can belong to a meaningful descendant when the visual
    // root is only a layout wrapper (StaticText Border, vertical Gauge Grid).
    winrt::Microsoft::UI::Xaml::UIElement GetSemanticTarget() const
        { return m_semanticTarget; }

    // Compatibility name used by the existing host/tests: content is the
    // visual root, never an implicit semantic alias.
    winrt::Microsoft::UI::Xaml::UIElement GetContent() const
        { return GetVisualRoot(); }

    // Replace the hosted element (e.g. wxTextCtrl swapping its TextBox for
    // a PasswordBox); passing null just detaches the current content.
    // Mechanically transactional, phase by phase: a failed detach changes
    // nothing; a failed install restores the old content; if even the
    // restore fails, the model is re-synced from what the carrier ACTUALLY
    // holds (never guessed).  Returns false on any failure.
    bool SetContent(const winrt::Microsoft::UI::Xaml::UIElement& element);
    bool SetContent(
        const winrt::Microsoft::UI::Xaml::UIElement& visualRoot,
        const winrt::Microsoft::UI::Xaml::UIElement& semanticTarget);

    // Read the element actually attached to the carrier into `content`.
    // Returns false when the carrier cannot even be READ (as opposed to
    // being readably empty) -- the caller must not treat that as "empty".
    // Also the test introspection: on success the result must always agree
    // with GetContent().
    bool TryGetCarrierContent(
        winrt::Microsoft::UI::Xaml::UIElement& content) const;

    // Last synced geometry, in physical pixels relative to the TLW client
    // area (i.e. the area the island covers).
    const wxRect& GetRectInTLW() const { return m_rectPx; }

    // Deterministic transaction introspection. These values are deliberately
    // read-only: tests use them to prove that a nested pass superseded an
    // outer one and that a bounded observer retry eventually quiesced.
    unsigned long long GetSyncEpochForTest() const { return m_syncEpoch; }
    bool IsContentTransactionInProgressForTest() const
        { return m_contentTransactionInProgress; }
    bool IsToolTipPropertyObserverPendingForTest() const
        { return m_toolTipPropertyObserverPending; }
    unsigned GetToolTipPropertyObserverRetryFailuresForTest() const
        { return m_toolTipPropertyObserverRetryFailures; }
    bool IsContentLoaded() const
        { return m_contentLoaded; }
    bool IsContentLoadedForTest() const
        { return IsContentLoaded(); }
    void SetContentLoadedForTest(bool loaded)
        { m_contentLoaded = loaded; }
    bool IsContentLoadedObserverPendingForTest() const
        { return m_contentLoadedObserverPending; }
    bool HasContentLoadedObserverForTest() const
        { return m_contentLoadedState != nullptr; }
    winrt::Microsoft::UI::Xaml::UIElement
    GetAutomationNameStyleObservedContentForTest() const
        { return m_automationNameStyleObservedContent; }
    unsigned GetContentLoadedObserverRetryFailuresForTest() const
        { return m_contentLoadedObserverRetryFailures; }
    std::weak_ptr<wxWinUISlotLifetime> GetLifetimeForTest() const
        { return m_lifetime; }
    void TestInvokeContentLoadedObserver();
    void SetLastToolTipDPIForTest(int dpi)
        { m_lastToolTipDPI = dpi; }
    std::uintptr_t GetPhysicalDisconnectGateIdentityForTest() const
        { return reinterpret_cast<std::uintptr_t>(m_disconnectGate.get()); }

private:
    friend class wxWinUITopLevelHost;
    friend class wxWinUIVisualCoordinates;

    wxWinUISlot();

    // Keeps the slot allocation alive across synchronous XAML callbacks.
    // Unregister/Shutdown disconnect it immediately but defer delete until
    // the outermost operation returns.
    class OperationGuard
    {
    public:
        explicit OperationGuard(wxWinUISlot *slot);
        ~OperationGuard();

    private:
        wxWinUISlot *m_slot;
        wxDECLARE_NO_COPY_CLASS(OperationGuard);
    };

    void MarkForDeletion() { m_deletePending = true; }
    void EndOperation();

    // Advance the identity represented by the invisible native shell. Besides
    // expiring retained provider objects through the generation check, this
    // explicitly purges UIA's per-HWND provider cache so the same HWND can
    // request a provider for the new XAML content.
    void AdvanceContentGeneration();
    void RetireExposedAccessibilityShellProvider(bool terminal = false);

    // Reset the per-content state caches (used by the SetContent
    // transaction: a fresh element must receive the full state).
    void ResetContentCaches();

    // Restore a tooltip value owned by this slot before its content is
    // detached. This mirrors RestoreAccessibilityView(): host-owned attached
    // values must never leak onto an element subsequently reused by the app.
    bool RestoreToolTip();

    // Restore UIA attached properties temporarily supplied by the common slot
    // adapter. Direct application mutations retain ownership.
    bool RestoreHelpText();
    bool RestoreAutomationName();
    bool ShouldPreserveAutomationNameRelinquishment() const;
    void SeedAutomationNameRelinquishment(const wxString& lastDesiredName);
    void PublishAutomationNameState(
        bool synced,
        bool owned,
        bool appOwned,
        bool relinquished,
        bool suppressedByLabeledBy,
        const wxString& lastDesired,
        const wxString& lastApplied,
        const winrt::Windows::Foundation::IInspectable& originalLocalValue,
        unsigned long long implicitStyleInspectedRevision);

    bool InstallToolTipPropertyObserver();
    void RevokeToolTipPropertyObserver();
    void QueueToolTipPropertyObserverRetry();
    bool InstallAutomationNameStylePropertyObserver();
    void RevokeAutomationNameStylePropertyObserver();
    void QueueAutomationNameStylePropertyObserverRetry();
    bool InstallContentLoadedObserver();
    void RevokeContentLoadedObserver();
    void QueueContentLoadedObserverRetry();
    void HandleContentLoadedObserver(
        const std::shared_ptr<wxWinUIContentLoadedState>& state);

    // Restore an AccessibilityView temporarily overridden while wxAccessible
    // was authoritative. This is part of the content transaction: a detached
    // element must never retain a host-owned Raw value.
    bool RestoreAccessibilityView();

    // Ask the owning host for one coalesced re-sync of this slot.
    void NudgeDirty();

    // Last-resort model repair: mirror m_content from what the carrier
    // actually holds (see SetContent's restore-failure path).
    void SyncContentFromCarrier();

    wxWindow *m_window = nullptr;
    WXHWND m_hwnd = nullptr;

    winrt::Microsoft::UI::Xaml::Controls::Grid m_container{ nullptr };

    // The state carrier between the container and the content: a host-owned
    // ContentControl whose IsEnabled mirrors the wx effective enabled state.
    // Control::IsEnabled is inherited by every descendant control, which is
    // what makes composite contents (a RadioBox grid...) block keyboard and
    // UIA along with the pointer -- and the content's OWN IsEnabled is
    // never touched, so an intrinsically disabled application element stays
    // disabled when the wx window is re-enabled.
    winrt::Microsoft::UI::Xaml::Controls::ContentControl m_carrier{ nullptr };

    winrt::Microsoft::UI::Xaml::UIElement m_content{ nullptr };
    // Null exactly when m_content is null; otherwise either m_content itself
    // or a descendant in its visual subtree. The pair is one transactional
    // content generation.
    winrt::Microsoft::UI::Xaml::UIElement m_semanticTarget{ nullptr };
    winrt::Microsoft::UI::Xaml::UIElement
        m_toolTipObservedContent{ nullptr };
    int64_t m_toolTipPropertyCallbackToken = 0;
    std::shared_ptr<wxWinUIToolTipPropertyState>
        m_toolTipPropertyState;
    // Registration can fail independently after a content transaction has
    // truthfully committed. Keep that degraded state explicit and retry at
    // the next same-content call or common SyncSlot pass.
    bool m_toolTipPropertyObserverPending = false;
    // Automatic retries are deliberately finite. A later explicit dirty
    // notification or same-content SetContent() can always try again, but a
    // permanently failing WinRT registration must not create a CallAfter
    // storm by itself.
    unsigned m_toolTipPropertyObserverRetryFailures = 0;
    winrt::Microsoft::UI::Xaml::FrameworkElement
        m_automationNameStyleObservedContent{ nullptr };
    int64_t m_automationNameStylePropertyCallbackToken = 0;
    int64_t m_automationNamePropertyCallbackToken = 0;
    int64_t m_automationLabeledByPropertyCallbackToken = 0;
    winrt::event_token m_automationNameActualThemeChangedToken{};
    winrt::event_token m_automationNameStyleLayoutUpdatedToken{};
    std::shared_ptr<wxWinUIAutomationNameStylePropertyState>
        m_automationNameStylePropertyState;
    bool m_automationNameStylePropertyObserverPending = false;
    unsigned m_automationNameStylePropertyObserverRetryFailures = 0;
    // StyleProperty, AutomationProperties.Name/LabeledBy and ActualTheme
    // changes are observed synchronously. Arbitrary wxWinUIXamlHost content
    // also snapshots the local Style source on LayoutUpdated: x:Null <->
    // UnsetValue can leave the effective Style null and hence raise no
    // property callback. A pure non-layout transition is caught by the next
    // ordinary host sync; forcing CompositionTarget.Rendering here would keep
    // the UI thread ticking. Native wx peers don't pay for this
    // application-authored-XAML fallback. A monotonic revision, rather than a
    // boolean, lets SyncSlot reject a pass superseded by a second change.
    unsigned long long m_automationNameStyleRevision = 0;
    // Only signals which can really re-resolve an implicit Style (Style,
    // reattach or ActualThemeChanged) advance this second revision.
    // A same-value application-theme assignment deliberately doesn't.
    unsigned long long m_automationNameImplicitStyleRevision = 0;
    unsigned long long m_automationNameImplicitStyleInspectedRevision = 0;

    // Changes whenever the carrier is mutated, even when a failed swap
    // restores the same logical element. SyncSlot() snapshots this value so
    // a synchronous XAML callback cannot replace content under it and then
    // make it commit old-element caches onto the replacement.
    unsigned long long m_contentGeneration = 0;
    // Every SyncSlot() entry advances this epoch. A nested pass therefore
    // makes all cache publications of its outer caller stale even when the
    // content generation itself did not change.
    unsigned long long m_syncEpoch = 0;
    // Covers the policy-only and full SyncSlot tooltip transactions as well
    // as restoration before a content detach. The ToolTipProperty observer
    // must not recursively enter the policy adapter for dependency-property
    // writes made by any of these host transactions. A later application
    // write (notably the asynchronous ToolTip::Closed event) observes zero
    // and can be suppressed immediately even while frozen.
    unsigned m_toolTipPolicySyncDepth = 0;
    // Published before the first carrier mutation and retired only after the
    // carrier/model pair is truthful again. FlushSync() never observes the
    // transitional A-model/B-carrier combination.
    unsigned long long m_contentTransactionEpoch = 0;
    bool m_contentTransactionInProgress = false;
    // A Name write observed while the carrier is between two published
    // content generations is necessarily application-authored: SyncSlot()
    // never writes the semantic target while this flag is set. Preserve this
    // fact across ResetContentCaches() so cross-TLW detach can transfer an
    // otherwise indistinguishable SetName()+ClearValue() decision.
    bool m_contentTransactionAutomationNameRelinquished = false;
    bool m_lastContentTransactionAutomationNameRelinquished = false;
    // Loaded is a property of the shared slot, not of any component wrapper.
    // Track it for arbitrary wxWinUIXamlHost content and future direct slot
    // users too; every callback is bound to the exact lifetime/content epoch.
    winrt::Microsoft::UI::Xaml::FrameworkElement
        m_contentLoadedObservedElement{ nullptr };
    winrt::event_token m_contentLoadedToken{};
    std::shared_ptr<wxWinUIContentLoadedState> m_contentLoadedState;
    bool m_contentLoadedObserverPending = false;
    unsigned m_contentLoadedObserverRetryFailures = 0;
    // A Loaded event for model A can run while SetContent() has already
    // detached A (or attached B) but has not published its outcome. Keep that
    // observation transaction-bound: rollback may apply it to A, commit must
    // discard it instead of blessing B.
    bool m_contentLoadedDuringTransaction = false;
    unsigned long long m_contentLoadedTransactionEpoch = 0;
    unsigned long long m_contentLoadedTransactionGeneration = 0;
    bool m_contentLoaded = false;

    // Preferred focus target of the hosted control, when mere "first
    // focusable element" is wrong (wxRadioBox focuses its SELECTED item).
    // A WEAK XAML reference pushed by the component on every selection
    // change -- never a callback into wx, so nothing here can outlive or
    // dangle into component state.  Survives cross-TLW migrations; cleared
    // with the slot.
    winrt::weak_ref<winrt::Microsoft::UI::Xaml::UIElement> m_preferredFocus;

    wxRect m_rectPx;
    wxRect m_visibleRectPx;
    bool m_visible = false;

    // Set when the carrier became UNREADABLE (TryGetCarrierContent failed):
    // the slot's XAML state can no longer be determined, its lifetime is
    // invalidated (every callback dead) and it must never be reused -- the
    // next RegisterSlot() disposes of it and builds a fresh one.
    bool m_poisoned = false;
    unsigned m_operationDepth = 0;
    bool m_deletePending = false;
    // Removing a routed handler or detaching its source while WinUI is still
    // invoking it can crash inside the focus manager. UnregisterSlot() retires
    // the logical state immediately, but postpones the physical XAML
    // disconnect until the outer slot operation has returned to the
    // dispatcher.
    bool m_disconnectPending = false;
    wxWinUITopLevelHost *m_deferredDisconnectHost = nullptr;
    // Non-null only for a control-specific asynchronous physical boundary.
    // The gate owns no wx pointer; its completion resolves this exact slot
    // through m_disconnectRetirementId and the host's invalidatable lifetime.
    std::shared_ptr<wxWinUIPhysicalDisconnectGate> m_disconnectGate;
    std::uint64_t m_disconnectRetirementId = 0;
    // Exact native-entry transaction associated by GettingFocus. A deferred
    // disconnect may restore its displaced HWND only while the host still
    // names this slot and this epoch; another focus attempt supersedes it.
    unsigned long long m_nativeFocusEntryEpoch = 0;

    // Additional local slot clip in DIPs. It uses physical-LTR coordinates,
    // just like m_rectPx: RTL changes content flow, never the native surface
    // occupied by the slot. A negative width is the compatibility sentinel
    // used by SetSlotClipHeight() for "through the current right edge".
    // m_hasClipRect distinguishes an intentionally empty rectangle from no
    // clip. The revision makes an outer SyncSlot() stale if application code
    // changes the clip from a re-entrant XAML callback.
    wxWinUIClipRectDIP m_clipRectDIP;
    bool m_hasClipRect = false;
    unsigned long long m_clipRevision = 0;

    // Snapshot of the ancestor chain (excluding the TLW) this slot holds a
    // geometry-tracking reference on: a child gets NO event when an ancestor
    // moves, resizes or is shown/hidden, so the host listens on the whole
    // chain (see wxWinUITopLevelHost::AddAncestorRef).
    std::vector<wxWindow *> m_ancestors;

    // State-adapter caches: the last values pushed to the XAML side, so the
    // coalesced flush can re-assert effective enabled/tooltip/UIA name on
    // every run without churning the tree when nothing changed.
    int m_lastEnabled = -1;         // -1 = never synced
    bool m_toolTipSynced = false;
    // True for both an exact wx-owned ToolTip object and an explicit empty
    // wx tooltip masking the authored baseline with null. In the latter case
    // m_lastAppliedToolTip is null and null identity is still significant.
    bool m_toolTipOwned = false;
    bool m_toolTipSuppressed = false;
    bool m_lastToolTipPresent = false;
    wxString m_lastToolTip;
    // SetMaxWidth() is expressed in physical pixels. Cache both the logical
    // value and the DPI used for its XAML-DIP projection so a monitor change
    // re-applies an otherwise unchanged tooltip.
    int m_lastToolTipMaximumWidth = 0;
    int m_lastToolTipDPI = 0;
    // Exact IInspectable installed for the last wx tooltip. Text equality is
    // insufficient: an application may replace it with another ToolTip object
    // carrying the same text, and that replacement must retain ownership.
    winrt::Windows::Foundation::IInspectable m_lastAppliedToolTip{ nullptr };
    // The effective value that predated the first wx tooltip override. It may
    // be a string, a ToolTip object, or null and remains useful for identity
    // tracking while global suppression owns a local null.
    winrt::Windows::Foundation::IInspectable m_originalToolTip{ nullptr };
    // Exact local attached-property baseline. ReadLocalValue() can be
    // UnsetValue even when a Style supplies the effective tooltip; restoring
    // this exact provenance (ClearValue for Unset) is what lets late styles
    // and resources regain dependency-property precedence.
    winrt::Windows::Foundation::IInspectable
        m_originalToolTipLocalValue{ nullptr };

    // UIA HelpText mirrors wx context help, falling back to the tooltip when
    // no context help is supplied. Its attached-property ownership is kept
    // separate from ToolTipService: either property can be authored directly
    // by an application and must be restored exactly when wx relinquishes it.
    bool m_helpTextSynced = false;
    bool m_helpTextOwned = false;
    bool m_lastHelpTextPresent = false;
    wxString m_lastHelpText;
    wxString m_lastAppliedHelpText;
    winrt::Windows::Foundation::IInspectable
        m_originalHelpTextLocalValue{ nullptr };

    // UIA names obey the same explicit ownership rule: an authored XAML name
    // is never overwritten; once wx has supplied a missing name, a direct
    // application mutation relinquishes ownership permanently for this
    // content generation.
    bool m_uiaNameSynced = false;
    bool m_uiaNameOwned = false;
    bool m_uiaNameAppOwned = false;
    // Once an authored or directly application-mutated value is observed,
    // wx must not reclaim the property for this content generation -- even
    // when the application deliberately sets it to the empty string.
    bool m_uiaNameRelinquished = false;
    // LabeledBy is a typed application-authored relationship and takes
    // precedence over wx's scalar fallback Name. Unlike direct Name
    // authorship it is reversible: removing the relationship lets wx restore
    // the fallback if no authored Name exists. This bit distinguishes that
    // temporary suspension from permanent per-generation relinquishment.
    bool m_uiaNameSuppressedByLabeledBy = false;
    // Monotonic publication revision for the complete Name provenance tuple.
    // It is intentionally never reset: dependency-property callbacks can
    // synchronously enter another SyncSlot pass while a physical Set/Clear is
    // in flight, and the nested tuple must invalidate every older writer
    // without an ABA when the content generation later changes.
    unsigned long long m_uiaNameStateRevision = 0;
    wxString m_lastUIAName;
    wxString m_lastAppliedUIAName;
    // Exact local value which existed before wx claimed Name. It is normally
    // UnsetValue, but keeping the IInspectable makes detach/restoration
    // correct even if dependency-property precedence evolves.
    winrt::Windows::Foundation::IInspectable
        m_originalUIANameLocalValue{ nullptr };

    // Some composite controls derive their accessible name from a separate
    // label window, so GetLabel() is deliberately empty. This desired value
    // belongs to the logical slot rather than a content generation and is
    // therefore not cleared by ResetContentCaches().
    bool m_hasAutomationNameOverride = false;
    wxString m_automationNameOverride;
    // Incremented on every explicit set/reset. SyncSlot snapshots it so a
    // reentrant mutation cannot commit a stale name.
    unsigned long long m_automationNameOverrideRevision = 0;

    // Exactly one accessibility tree is authoritative for a slot. With no
    // wxAccessible the XAML peer remains visible and the covered HWND is Raw;
    // with wxAccessible the HWND/MSAA tree wins and the XAML content is
    // temporarily forced Raw. Preserve the exact pre-existing XAML value so
    // authority removal and content replacement can restore application
    // authorship. The generation also retires already-returned shell
    // providers immediately when authority changes.
    bool m_hasWxAccessible = false;
    unsigned long long m_accessibilityAuthorityGeneration = 1;
    bool m_accessibilityViewSynced = false;
    bool m_accessibilityViewOwned = false;
    // Logical slot retirement can precede physical XAML disconnection by a
    // dispatcher turn. Consume the terminal UIA purge once so the later phase
    // cannot act on an HWND value recycled for a different native lifetime.
    bool m_accessibilityShellRetirementTerminal = false;
    // ReadLocalValue() snapshot, including DependencyProperty::UnsetValue().
    // This preserves not only the effective enum but whether the application
    // authored a local attached value at all.
    winrt::Windows::Foundation::IInspectable
        m_originalAccessibilityViewLocalValue{ nullptr };

    bool m_cursorSynced = false;
    unsigned long long m_lastCursorPolicyGeneration = 0;
    // Keep the wx object alive, not just its numeric HCURSOR: custom cursor
    // handles can otherwise be destroyed and later reused while the cache
    // still treats the number as the same cursor.
    wxCursor m_lastCursor;

    // Last verdict of the position-sensitive wxEVT_SET_CURSOR walk. XAML can
    // raise Pressed/Released/Wheel and USER32 capture notifications after the
    // Move which selected it; those terminal/resync paths must re-assert this
    // exact verdict without running application handlers again or falling
    // back to the slot's static cursor. The key is deliberately complete:
    // slot identity is this allocation.  The screen point and leaf client
    // geometry distinguish a stationary pointer after a move/resize, while
    // the ancestor generation distinguishes a same-position reparent whose
    // event-handler chain changed.
    bool m_pointerCursorVerdictValid = false;
    wxPoint m_pointerCursorVerdictPoint;
    wxPoint m_pointerCursorVerdictClientPoint;
    wxSize m_pointerCursorVerdictClientSize;
    unsigned long long m_pointerCursorVerdictPolicyGeneration = 0;
    unsigned long long m_pointerCursorVerdictContentGeneration = 0;
    unsigned long long m_pointerCursorVerdictTopologyGeneration = 0;
    bool m_pointerCursorVerdictHasWxOverride = false;
    winrt::Microsoft::UI::Input::InputCursor
        m_pointerCursorVerdict{ nullptr };
    WXHCURSOR m_pointerCursorVerdictHandle = nullptr;
    wxCursor m_pointerCursorVerdictKeepAlive;
    // Invalidates a routed cursor verdict whenever the leaf geometry or any
    // ancestor topology/geometry participating in the wx event walk changes.
    unsigned long long m_cursorTopologyGeneration = 1;

    // Shared state captured by every XAML callback of this slot; invalidated
    // by DisconnectSlot() before anything is freed.
    std::shared_ptr<wxWinUISlotLifetime> m_lifetime;

    // Everything needed to revoke those callbacks: AddHandler() has no token,
    // removal needs the routed event and the very same boxed delegate.
    std::vector<std::pair<winrt::Microsoft::UI::Xaml::RoutedEvent,
                          winrt::Windows::Foundation::IInspectable>> m_routedHandlers;
    winrt::event_token m_gotFocusToken{};
    winrt::event_token m_lostFocusToken{};

    // The XAML slot produces the same raw press/release stream as the root
    // router. Keep its click/pressed state local to this slot so replacing or
    // destroying another control can never complete this one's gesture.
    wxWinUIInputState m_inputState;
    // A wx input handler can synchronously pump a newer XAML pointer event.
    // Once that happens, the remaining actions of the older transition are
    // stale and must not be delivered after the newer state.
    unsigned long long m_inputTransitionEpoch = 0;

    wxDECLARE_NO_COPY_CLASS(wxWinUISlot);
};

// ----------------------------------------------------------------------------
// wxWinUIVisualCoordinates: the single physical-pixel <-> XAML-DIP mapper.
//
// Public wx APIs keep using physical client/screen pixels. XAML geometry is
// always expressed in DIPs relative to a realized visual. Mixing HWND DPI,
// integer ToDIP() calls and ClientToScreen() is both lossy at fractional
// scales and wrong for mirrored HWNDs. These transactions preserve fractions,
// use the island XamlRoot scale, and publish no result if a synchronous
// Win32/WinRT boundary changed the host, slot, content or HWND identity.
// ----------------------------------------------------------------------------

enum class wxWinUICoordinateResult
{
    Mapped,
    Unavailable,
    Superseded
};

class WXDLLIMPEXP_CORE wxWinUIVisualCoordinates final
{
public:
    using Point = winrt::Windows::Foundation::Point;
    using Rect = winrt::Windows::Foundation::Rect;
    using UIElement = winrt::Microsoft::UI::Xaml::UIElement;

    static wxWinUICoordinateResult ClientPointToRoot(
        wxWindow *anchor,
        const Point& clientPixels,
        Point *rootDips);
    static wxWinUICoordinateResult ScreenPointToRoot(
        wxWindow *anchor,
        const Point& screenPixels,
        Point *rootDips);
    static wxWinUICoordinateResult ClientPointToElement(
        wxWindow *anchor,
        const Point& clientPixels,
        const UIElement& element,
        Point *elementDips);
    static wxWinUICoordinateResult ElementPointToClient(
        wxWindow *anchor,
        const UIElement& element,
        const Point& elementDips,
        Point *clientPixels);
    static wxWinUICoordinateResult ElementBoundsToClient(
        wxWindow *anchor,
        const UIElement& element,
        const Rect& elementBoundsDips,
        Rect *clientBoundsPixels);

    // Projection-free arithmetic seam: CI can exercise 125/150/175 % and
    // mirrored physical spaces without depending on its monitor topology.
    static bool TestRoundTripPhysicalSpaces(
        const Rect& rootScreenPixels,
        const Rect& anchorScreenPixels,
        bool anchorMirrored,
        double scale,
        const Point& clientPixels,
        Point *rootDips,
        Point *roundTrippedClientPixels);

private:
    struct Snapshot;

    static wxWinUITopLevelHost *ResolveHost(wxWindow *anchor);
    static wxWinUICoordinateResult Capture(
        wxWindow *anchor,
        wxWinUITopLevelHost *host,
        wxWinUISlot *slot,
        bool requireSlot,
        Snapshot *snapshot);
    static bool IsCurrent(const Snapshot& snapshot);
    static bool TryGetPhysicalClientRect(
        wxWindow *window,
        winrt::Windows::Foundation::Rect *screenPixels,
        bool *mirrored);
    static Point ClientToScreen(
        const Snapshot& snapshot,
        const Point& clientPixels);
    static Point ScreenToClient(
        const Snapshot& snapshot,
        const Point& screenPixels);
    static Point ScreenToRoot(
        const Snapshot& snapshot,
        const Point& screenPixels);

    static wxWinUICoordinateResult ClientPointToRootOnce(
        wxWindow *anchor,
        const Point& clientPixels,
        Point *rootDips);
    static wxWinUICoordinateResult ScreenPointToRootOnce(
        wxWindow *anchor,
        const Point& screenPixels,
        Point *rootDips);
    static wxWinUICoordinateResult ClientPointToElementOnce(
        wxWindow *anchor,
        const Point& clientPixels,
        const UIElement& element,
        Point *elementDips);
    static wxWinUICoordinateResult ElementPointToClientOnce(
        wxWindow *anchor,
        const UIElement& element,
        const Point& elementDips,
        Point *clientPixels);
    static wxWinUICoordinateResult ElementBoundsToClientOnce(
        wxWindow *anchor,
        const UIElement& element,
        const Rect& elementBoundsDips,
        Rect *clientBoundsPixels);

    wxDECLARE_NO_COPY_CLASS(wxWinUIVisualCoordinates);
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
//    lazily by the framework) are subclassed for focus, cancellation and
//    lifetime bookkeeping, but deliberately do NOT return HTTRANSPARENT.
//    The transparent root is hit-testable everywhere and is the single input
//    receiver: its island-first router forwards real native messages to the
//    wx window below whenever no XAML peer owns the point. WM_NCHITTEST
//    pass-through, WS_EX_TRANSPARENT and
//    ContentIsland::IsHitTestVisibleWhenTransparent(false) proved
//    non-deterministic or discarded the pointer instead of forwarding it;
//  - the root Canvas carries NO SystemBackdrop, so the live GDI of the wx
//    children shows through wherever XAML draws nothing.
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

    // Find the host that actually owns this window's slot. This deliberately
    // does not infer ownership from the current wx parent: a failed
    // cross-TLW migration can temporarily leave the truthful slot in the old
    // host.
    static wxWinUITopLevelHost *FindSlotOwner(const wxWindow *window);

    // Resolve the current TLW host and reconcile an existing slot into it.
    // Returns null instead of allowing a second slot when migration fails.
    static wxWinUITopLevelHost *ReconcileSlotOwner(wxWindow *window,
                                                    bool createIfNeeded);

    // Is this HWND the island bridge of one of the live hosts?  (Used by the
    // focus plumbing: ::GetFocus() on a bridge must resolve to the focused
    // slot's wx window, not to the TLW.)
    static wxWinUITopLevelHost *FindForBridge(HWND hwnd);

    // Focus resolution for wxWindow::FindFocus(): if the native focus HWND
    // belongs to one of the live islands, return the wx window owning the
    // focused slot (or the TLW when a free element has it); null otherwise.
    static wxWindow *ResolveFocusHwnd(WXHWND hwnd);
    static wxWindow *ResolveFocusCounterpartHwnd(WXHWND hwnd);

    // Suppress the native KILL_FOCUS generated only by the synchronous
    // shell-HWND -> island transfer performed after wxWindow::SetFocus()
    // already delivered the logical focus event for a slotted control.
    static bool ShouldSuppressNativeKillFocus(wxWindow *window,
                                              WXHWND destination);

    // The wx window whose slot currently holds the XAML focus, if any.
    wxWindow *GetFocusOwner() const { return m_focusOwner; }

    // ----- slots -----

    // Register (or re-register) the XAML element of a wx child window.
    // Returns null on failure.  The slot is owned by the host.
    wxWinUISlot *RegisterSlot(wxWindow *window,
                              const winrt::Microsoft::UI::Xaml::UIElement& element);
    wxWinUISlot *RegisterSlot(
        wxWindow *window,
        const winrt::Microsoft::UI::Xaml::UIElement& visualRoot,
        const winrt::Microsoft::UI::Xaml::UIElement& semanticTarget,
        const std::shared_ptr<wxWinUIPhysicalDisconnectGate>&
            physicalDisconnectGate = {});

    void UnregisterSlot(
        wxWindow *window,
        const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate = {});
    void SetSlotPhysicalDisconnectGate(
        wxWindow *window,
        const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate);

    wxWinUISlot *FindSlot(const wxWindow *window) const;

    // Snapshot and validate the native HWND shell represented by a slot.
    // The returned lifetime is intentionally weak outside the call: UIA
    // providers must not keep a removed slot alive.
    bool TryGetAccessibilityShellState(
        const wxWindow *window,
        std::shared_ptr<wxWinUISlotLifetime> *lifetime,
        WXHWND *hwnd,
        unsigned long long *hwndGeneration,
        unsigned long long *contentGeneration,
        bool *hasWxAccessible,
        unsigned long long *authorityGeneration) const;

    // Called by wxWindowBase::SetAccessible()/GetOrCreateAccessible(). It is
    // deliberately a no-op when no slot exists: changing accessibility must
    // never manufacture an island host. Existing slots are synchronized
    // immediately so WM_GETOBJECT cannot expose both trees in the interval
    // before the next coalesced geometry flush.
    void AccessibilityAuthorityChanged(wxWindow *window,
                                       bool hasWxAccessible);

    // Reconcile every slotted window in this reparented subtree immediately.
    // This is called by the MSW Reparent() shim: relying only on the next
    // coalesced flush loses the slot if the old TLW is destroyed first.
    void HandleReparent(wxWindow *subtreeRoot);
    static bool CanReparentSubtreeNow(wxWindow *subtreeRoot);
    static void HandleReparentAcrossHosts(wxWindow *subtreeRoot,
                                          wxWindow *oldTLW);
    // Preserve the one logical XAML focus owner before USER32 reparents its
    // invisible shell. SetParent() and lazy creation of the destination
    // island are synchronous focus/activation boundaries, so waiting until
    // HandleReparentAcrossHosts() is too late. The returned opaque token is
    // owned by the caller and must always be passed to
    // CancelPreparedFocusReparent(); that call is a no-op after the normal
    // migration path has consumed the preparation.
    static unsigned long long PrepareFocusReparent(
        wxWindow *subtreeRoot,
        wxWindow *oldTLW,
        wxWindow *newTLW);
    static void SetPreparedFocusReparentNativeBoundary(
        unsigned long long token,
        bool active);
    static void CancelPreparedFocusReparent(unsigned long long token);

    // Clip the slot to an exact local physical-LTR rectangle in DIPs. Empty
    // rectangles are meaningful and hide the slot; use ClearSlotClip() to
    // remove the limit. The rectangle follows the slot across DPI/TLW moves.
    void SetSlotClipRect(wxWindow *window, const wxRect& rectDIP);
    void SetSlotClipRect(wxWindow *window,
                         double xDIP,
                         double yDIP,
                         double widthDIP,
                         double heightDIP);
    void ClearSlotClip(wxWindow *window);

    // Compatibility wrapper: clip to the top portion, through the current
    // right edge (DIPs; <= 0 removes the limit).
    void SetSlotClipHeight(wxWindow *window, double heightDIP);

    // Shared arithmetic used by wxWinUIControlHost and SyncSlot. Keeping the
    // fractional DIP rectangle until the final physical-pixel rounding makes
    // 125/150/175% clips exactly reversible and independently testable.
    static wxWinUIClipRectDIP PhysicalClipRectToDIP(
        const wxRect& rectPx,
        double scale);
    static wxRect ClipRectDIPToPhysical(
        const wxWinUIClipRectDIP& rectDIP,
        double scale,
        const wxSize& slotSizePx);

    // Give the keyboard focus to a slot: native focus to the bridge, XAML
    // focus to the slot's element.
    void FocusSlot(wxWindow *window);

    // Projection-free tail of wxWindowMSW::HandleSetFocus(). This is not a wx
    // event handler: application consumption of wxEVT_SET_FOCUS must not
    // prevent the already-delivered shell focus from reaching its XAML peer.
    void HandleNativeShellFocus(wxWindow *window, wxWindow *previous);
    bool ConsumeNativeFocusRollback(wxWindow *window);
    bool ConsumeMigrationShellFocus(wxWindow *window);
    static bool NoteMigrationShellFocusDeparture(
        wxWindow *window,
        WXHWND destination);

    // Complete a still-current logical focus request when the component's
    // XAML content reaches Loaded. This avoids timing loops while preserving
    // the exact target/content/intent identities across initial realization.
    void NotifySlotContentLoaded(wxWindow *window);
    void NotifySlotContentLoaded(
        wxWindow *window,
        const winrt::Microsoft::UI::Xaml::UIElement& expectedContent,
        unsigned long long expectedGeneration = 0);

    // Register the slot's preferred focus target (weakly referenced, see
    // wxWinUISlot::m_preferredFocus).  A null element clears it.
    void SetSlotPreferredFocus(
        wxWindow *window,
        const winrt::Microsoft::UI::Xaml::UIElement& target);

    // Set/reset the desired automation-name override independently of the
    // current content. An empty name resets to normal label derivation.
    void SetSlotAutomationNameOverride(wxWindow *window,
                                       const wxString& name);

    // ----- geometry sync -----

    // Schedule a coalesced re-sync of one slot / of everything.
    void MarkDirty(wxWindow *window);
    void MarkAllDirty();

    // Run the pending sync immediately.
    void FlushSync();

    // Projection-free MSW notifications enter here after a native geometry,
    // scrollbar or sibling-order mutation. This never creates a host; the
    // caller has already resolved an existing one.
    void NotifyNativeLayoutMutation(bool zOrderMayHaveChanged,
                                    wxWindow *mutated = nullptr);
    // Marks every slot living under this window, and returns false when
    // the window is not an ancestor (nor the owner) of any slot, so the
    // caller can fall back to a full pass.
    bool MarkSlotsUnderAncestor(wxWindow *ancestor);
    // Lets the window under the pointer paint what the routed move
    // invalidated; see the implementation.
    void LetPointerTargetPaint(WXHWND target);

    // End-of-freeze notification (via wxWinUITLWHostNotifyThaw): run the
    // single catch-up flush the freeze held back; no-op unless one is owed.
    void NotifyThaw();

    // ----- hit test -----

    // Does this screen point hit interactive XAML content (or any open
    // popup, which captures everything)?
    bool HitTestContent(const POINT& ptScreen);

    // Resolve the public wxFindWindowAtPoint() contract through the shared
    // island. Z-ordered slotted XAML wins over the covered native tree; free
    // XAML (menus/dialog layers) at or above the top slot resolves
    // conservatively to the TLW; where the root is transparent, the deepest
    // native wx child below the bridge is returned, including disabled
    // windows.
    wxWindow *ResolveWindowAtScreenPoint(const wxPoint& ptScreen);

    // ----- tree access -----

    winrt::Microsoft::UI::Xaml::Controls::Canvas Root() const { return m_root; }
    winrt::Microsoft::UI::Xaml::XamlRoot GetXamlRoot() const;
    HWND GetBridgeHwnd() const { return m_bridge; }
    wxWindow *GetTLW() const { return m_tlw; }
    bool IsShuttingDown() const { return m_shuttingDown; }

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    bool OwnsOleDropRegistration() const;
    wxWinUIDropBroker *GetDropBrokerForTest() const
        { return m_dropBroker.get(); }
#endif
#if wxUSE_DRAG_AND_DROP
    void UpdateBridgeFileAcceptance();
    static void UpdateFileAcceptanceForAll();
#endif

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

    // Conservatively invalidate every live host. This is used for inherited
    // enabled state: the window that changed need not itself own
    // a slot, and a failed cross-TLW migration can truthfully leave a
    // descendant's slot in the old host until the next retry.
    static void MarkAllHostsDirty();

    // Apply global tooltip policy immediately to every live host. A frozen
    // host receives only this policy transaction synchronously; its geometry
    // and structural catch-up remain owed until Thaw().
    static void SynchronizeAllHosts();

    // Invalidate both the per-slot cursor caches and the island-level cursor
    // override after global/busy policy changes, then restore the cursor for
    // the currently authoritative pointer surface.
    static void NotifyCursorPolicyChanged(bool emitSetCursorEvents = true);

    // MSW capture plumbing calls this before explicit SetCapture/
    // ReleaseCapture and on WM_CAPTURECHANGED. Public only because the
    // projection-free tlwhostmsw bridge must reach the host implementation.
    void NotifyNativeCaptureMutation();

    // A client-area XAML grip is an explicit resize affordance, not a native
    // hit-test result. Its weak peer validator must still identify the same
    // live grip when the private native wake is processed.
    bool RequestNativeResize(
        wxWindow *source,
        const winrt::Microsoft::UI::Xaml::UIElement& grip,
        int hitTest,
        const wxWinUIPointerSample& sample,
        std::function<bool ()> isCurrent);
    static void ScheduleNativeResizeWakesAfterGlobalOperation();

    // ----- lifetime introspection (used by the lifecycle unit tests) -----

    // Number of live hosts / of registered slots across every host.
    static unsigned GetLiveHostCount();
    static unsigned GetLiveSlotCount();
    static unsigned GetPendingPhysicalDisconnectGateCountForTest();
    static unsigned GetPendingTLWTerminalRetirementCountForTest();
    void ShutdownForTest() { Shutdown(); }
    std::weak_ptr<wxWinUIHostLifetime> GetLifetimeForTest() const
        { return m_hostLifetime; }

    // Re-arm physical host deletion for every logically shut down host once
    // the process-wide WinUI callback transaction has fully unwound.
    static void SchedulePendingDeletesAfterGlobalOperation();

    // Number of live per-slot callback states (wxWinUISlotLifetime): once
    // every hosted window of a TLW has been destroyed and the pending
    // dispatch has drained, this must come back down to what it was.
    static unsigned GetLiveSlotLifetimeCount()
        { return wxWinUISlotLifetime::GetLiveCount(); }

    // Number of active FrameworkElement.StyleProperty observers used to
    // detect an AutomationProperties.Name supplied by a late Style. A strong
    // application reference to detached XAML content must not keep one alive.
    static unsigned GetLiveAutomationNameStyleObserverCountForTest();
    // Same proof for the shared per-slot FrameworkElement.Loaded observer:
    // retaining application content after host shutdown must not retain a
    // callback state capable of reaching wx.
    static unsigned GetLiveContentLoadedObserverCountForTest();

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
    static unsigned GetFlushCallbackAttemptCount();
    unsigned GetOwnFlushRunCountForTest() const
        { return m_flushRunsForTest; }

    // Number of sibling groups for which the real HWND order asks a native
    // surface to overlap above XAML while the one shared bridge necessarily
    // renders all XAML above every native child. This is a diagnostic limit,
    // not an attempt to manufacture another island.
    static unsigned GetImpossibleZOrderCount();
    static void ResetImpossibleZOrderCountForTest();

    // Host-owned callback state and subscriptions are tracked separately
    // from the per-slot ones.
    static unsigned GetLiveHostLifetimeCount()
        { return wxWinUIHostLifetime::GetLiveCount(); }
    static unsigned GetRootHandlerAddCount();
    static unsigned GetRootHandlerRevokeCount();
    static unsigned GetLiveSubclassContextCount();

    // Deterministic seams for host construction and the provisional first
    // OLE acquisition. The hook is invoked while an OperationGuard is held,
    // so a test may destroy the TLW or mutate presentation without allowing
    // the host allocation to disappear from underneath the transaction.
    enum class InitializationHookPointForTest
    {
        AfterProvisionalPublication,
        AfterSourceInitialization,
        AfterRootRouter,
        AfterTitleBarPolicy,
        AfterTLWBindings,
        AfterDropBrokerReadyBeforeCommit,
        BeforeExceptionLog
    };
    using InitializationHookForTest =
        void (*)(wxWindow *, InitializationHookPointForTest);
    enum
    {
        TestInitializationFault_None = 0,
        TestInitializationFault_AfterTLWBindings = 1
    };
    static void TestSetInitializationHook(
        InitializationHookForTest callback);
    static void TestFailInitialization(unsigned mask);

    // Fault injection for the registration-rollback tests: the Nth handler
    // subscription performed by the next BindSlotEvents() throws.  0 (the
    // default) disables the injection, and it disarms itself after firing,
    // so a forgotten reset cannot poison later registrations.
    static void TestFailHandlerAdd(unsigned nth);

    // Fault injection for the content-swap transaction tests: a mask of
    // TestContentFault_* bits, each consumed by its phase when it fires.
    enum
    {
        TestContentFault_Detach = 1,
        TestContentFault_Install = 2,
        TestContentFault_Restore = 4,
        TestContentFault_Read = 8,  // TryGetCarrierContent() read failure
        TestContentFault_ToolTipObserver = 16,
        TestContentFault_AutomationNameTransferRead = 32,
        TestContentFault_AutomationNameRestoreRead = 64
    };
    static void TestFailContentSwap(unsigned mask);
    // Fail the next N tooltip-property observer registrations. Unlike the
    // phase bit above this is a countdown, allowing deterministic coverage
    // of multiple deferred retries before a later success.
    static void TestFailToolTipObserverAdds(unsigned count);
    static void TestFailContentLoadedObserverAdds(unsigned count);
    static void TestFailInputPointerSourceSet(unsigned nth);

    // One-shot seam at the actual synchronous reentrancy boundary of slot
    // publication. XAML Loaded is intentionally not used by the lifecycle
    // tests: a newly attached carrier can remain Collapsed until the next
    // coalesced layout flush, making Loaded asynchronous and non-portable.
    static void TestOnNextSlotAttached(
        std::function<void (wxWindow *)> callback);
    static unsigned GetPendingFocusMigrationCountForTest();
    // One-shot seam after a slot has atomically published its geometry
    // cache. It exercises a structural mutation raised reentrantly by a
    // setter in an earlier slot without relying on asynchronous XAML layout.
    static void TestOnNextSlotSynced(
        std::function<void (wxWindow *)> callback);
    // One-shot seam after the requested element has been written to the
    // carrier but before SetContent() publishes it as the slot model. It
    // models a synchronous Loaded/property callback entering FlushSync().
    static void TestOnNextContentCarrierSet(
        std::function<void (wxWindow *)> callback);
    // One-shot seam immediately after SetContent() publishes its transaction
    // flag and before the old element is detached. It lets tests drive the
    // real generation-bound Loaded callback and then choose commit/rollback
    // without depending on asynchronous XAML layout timing.
    static void TestOnNextContentTransactionStarted(
        std::function<void (wxWindow *)> callback);
    // One-shot seam immediately after Canvas::SetZIndex(), i.e. the
    // application-code boundary whose nested flush/destruction invariants
    // the z-order lifecycle tests exercise.
    static void TestOnNextZOrderSet(std::function<void ()> callback);
    // One-shot seam immediately after ResolveWindowAtScreenPoint() reads a
    // snapshotted slot's IsHitTestVisible value. It lets tests destroy that
    // exact slot at the first callback boundary and prove no raw m_slots
    // iterator or wxWinUISlot pointer survives the getter.
    static void TestOnNextWindowAtPointSlotStateRead(
        std::function<void (wxWindow *)> callback);
    static bool TestPointerContactActiveSnapshot(
        wxWinUIInputDevice device,
        int virtualKey,
        unsigned pointerType,
        unsigned pointerFlags,
        unsigned penFlags);
    wxWinUIRootPointerOutcome TestRouteRootPointerSample(
        const wxWinUIPointerSample& sample,
        bool sourceAlreadyHandled = false);
    bool TestRouteRootPointerBoundary(
        bool entered,
        const wxWinUIPointerSample& sample);
    void TestHandleRootPointerInterrupted(
        bool captureLost,
        const wxWinUIPointerSample& sample)
        { HandleRootPointerInterrupted(captureLost, sample); }

    bool TestRouteSlotPointer(
        wxWindow *window,
        const wxWinUIPointerSample& sample,
        wxWinUIInputSurface physicalSurface =
            wxWinUIInputSurface::Native);
    bool TestMapRootPointerPointForSlot(
        wxWindow *window,
        double xDIP,
        double yDIP,
        wxPoint *screen,
        bool *inside) const;
    bool TestRouteOwnedSlotRelease(
        const wxWinUIPointerSample& sample)
        { return DispatchSlotOwnedRelease(sample); }
    bool TestRememberModalAwaitingRelease(
        const wxWinUIInputAction& action)
        { return RememberModalAwaitingRelease(action); }
    wxWinUIInputAction TestTakeModalAwaitingRelease(
        wxWinUIInputDevice device,
        std::uint32_t pointerId,
        wxWinUIInputButton button)
        { return TakeModalAwaitingRelease(device, pointerId, button); }
    std::size_t GetModalAwaitingReleaseCountForTest() const;
    void TestCancelSlotPointerOwners(
        const wxWinUIPointerSample& sample,
        bool allPointers)
        { CancelSlotPointerOwners(sample, allPointers); }
    std::size_t GetSlotPointerOwnerCountForTest() const;
    std::uint32_t GetInputSiteGenerationForTest() const
        { return m_inputSiteGeneration; }
    bool TestCanPostIslandCancelMode(
        std::uint32_t expectedGeneration) const
        { return CanPostIslandCancelMode(expectedGeneration); }
    void TestPostIslandCancelMode(std::uint32_t expectedGeneration)
        { PostIslandCancelMode(expectedGeneration); }
    bool HasDeferredIslandCancelForTest() const
        { return m_islandCancelDeferred; }
    unsigned GetPostedIslandCancelCountForTest() const
        { return m_postedIslandCancelMessages; }
    void TestRetireIslandCancelTarget();

    // Test seam: drive the island's take-focus exit path directly (the
    // real trigger -- Tab off the island's last element -- needs a live
    // keyboard).
    void TestTakeFocusRequested(bool previous)
        { OnTakeFocusRequested(previous); }

    // Cumulative count of per-slot cursor applications: proves the
    // wxWindow::SetCursor() plumbing reaches the slot seam.
    static unsigned GetSlotCursorSetCount();
    winrt::Microsoft::UI::Input::InputCursor
    GetSlotCursorForTest(const wxWindow *window) const;
    winrt::Microsoft::UI::Input::InputCursor
    GetIslandCursorForTest() const;
    WXHCURSOR GetIslandCursorHandleForTest() const
        { return m_lastIslandPointerHandle; }
    bool IsInputPointerSourceAuthoritativeForTest() const
        { return m_inputPointerSourceAuthoritative; }
    bool TestActivateSlotCursor(wxWindow *window,
                                bool emitSetCursorEvent = false);
    bool TestApplySlotPointerCursor(
        wxWindow *window,
        const wxPoint& screenPoint,
        wxWinUIInputKind kind);
    bool TestSimulateXamlCursorSelection(
        const winrt::Microsoft::UI::Input::InputCursor& cursor);
    bool TestRefreshPointerCursorAt(const wxPoint& screenPoint)
    {
        m_lastPointerScreen.x = screenPoint.x;
        m_lastPointerScreen.y = screenPoint.y;
        return RefreshPointerCursorAtLastPoint();
    }

    // Live Loaded-hook count across the control hosts (defined in
    // ctrlhost.cpp): a failed content swap must leave the old hook in
    // place and drop only the new one.
    static unsigned GetLiveLoadedHookCount();

private:
    friend class wxWinUISlot;
    friend WXDLLIMPEXP_CORE bool wxWinUIGetNativeResizeSnapshotForTesting(
        wxWindow *window, wxWinUINativeResizeSnapshot *snapshot);
    friend class wxWinUIVisualCoordinates;
    friend class wxWinUIDropBroker;
    friend class wxWinUIDropBrokerState;
    friend WXDLLIMPEXP_CORE void *
    wxWinUITLWHostBeginWindowEventDispatch(const wxWindowBase *window);
    friend WXDLLIMPEXP_CORE void
    wxWinUITLWHostEndWindowEventDispatch(void *cookie);
    friend WXDLLIMPEXP_CORE bool
    wxWinUITLWHostDeferTopLevelDestroy(wxWindow *window);
    friend WXDLLIMPEXP_CORE bool
    wxWinUITLWHostDeferModalDialogDestroy(wxWindow *window);
    friend WXDLLIMPEXP_CORE wxWinUIDestroyDeferralResult
    wxWinUITLWHostDeferPopupDestroy(
        wxWindow *window,
        wxWinUIPopupDestroySemantics semantics);

    explicit wxWinUITopLevelHost(wxWindow *tlw);
    virtual ~wxWinUITopLevelHost();

    // Prevent a nested wxYield() from running the deferred host deletion
    // while one of its public transactions is still on the stack.
    class OperationGuard
    {
    public:
        explicit OperationGuard(wxWinUITopLevelHost *host);
        ~OperationGuard();

    private:
        wxWinUITopLevelHost *m_host;
        wxDECLARE_NO_COPY_CLASS(OperationGuard);
    };

    void EndOperation();
    void RequestDeferredDelete();
    void ScheduleDeferredDelete();

    bool CaptureInitializationIdentity();
    bool HasInitializationContext() const;
    bool IsReadyForLookup() const;
    void InvokeInitializationHookForTest(
        InitializationHookPointForTest point);
    void MaybeThrowInitializationFault(unsigned fault);
    bool Initialize();
    bool FinishInitializationAfterDropBroker();
    void ResumeDeferredInitialization(std::uint64_t pendingId);
#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    bool IsDropBrokerPresentationReady() const;
    bool IsDropBrokerPresentationStable(
        unsigned long long expectedGeneration) const;
    void RestartDropBrokerAfterPresentationInvalidation();
    void TryInitializeDropBrokerAfterPresentation();
#endif
    void Shutdown();

    // Subclass procedure shared by the bridge and its inner input window.
    static LRESULT CALLBACK BridgeSubclassProc(HWND hwnd, UINT msg,
                                               WPARAM wParam, LPARAM lParam,
                                               UINT_PTR subclassId,
                                               DWORD_PTR refData);

    static LRESULT CALLBACK NativeResizeSubclassProc(
        HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
        UINT_PTR subclassId, DWORD_PTR refData);
    struct NativeResizeRequest;
    bool ValidateNativeResize(
        const std::shared_ptr<NativeResizeRequest>& request);
    bool PostNativeResizeWake();
    void DispatchNativeResize(std::uint64_t ticket);
    void FinishNativeResize(
        std::shared_ptr<NativeResizeRequest> request,
        bool cancelled);
    void CancelNativeResize(const wxWinUIPointerSample& sample);

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

#if wxUSE_DRAG_AND_DROP
    enum class DropSurfaceResolution
    {
        Native,
        Slot,
        Blocking,
        Unstable
    };
    DropSurfaceResolution ResolveDropSurface(const POINT& screen,
                                             wxWindow **seed);
    bool RouteDropFiles(WXWPARAM drop);
#endif

    void ScheduleFlush();
    void ScheduleDeferredSlotDisconnects();
    bool CompleteDeferredSlotDisconnect(wxWinUISlot& slot);
    void CompleteDeferredSlotDisconnect(std::uint64_t retirementId) noexcept;
    enum class DeferredTransferDisconnectResult
    {
        NotFound,
        StillActive,
        Completed
    };
    DeferredTransferDisconnectResult CompleteDeferredTransferDisconnect(
        wxWindow *window,
        WXHWND hwnd,
        const winrt::Microsoft::UI::Xaml::UIElement& content);
    void FlushDeferredSlotDisconnects();
    void InvalidateStructure();
    void SyncSlot(wxWinUISlot& slot);
    void SynchronizeSlotToolTipPolicy(wxWinUISlot& slot);
    void SyncSlotAccessibilityAuthority(wxWinUISlot& slot,
                                        bool hasWxAccessible);
    bool RecomputeZOrder(unsigned long long expectedGeneration);
    bool MigrateSlotToCurrentTLW(wxWindow *window);
    wxWinUISlot *RegisterSlotInternal(
        wxWindow *window,
        const winrt::Microsoft::UI::Xaml::UIElement& element,
        const winrt::Microsoft::UI::Xaml::UIElement& semanticTarget,
        wxWinUITopLevelHost *transferFrom,
        bool accessibilityFactoryAlreadyResolved,
        bool preserveAutomationNameRelinquishment,
        const wxString& lastDesiredAutomationName,
        const std::shared_ptr<wxWinUIPhysicalDisconnectGate>&
            physicalDisconnectGate);

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
    bool BindRootRouter();
    void OnRootPointer(UINT message,
                       const winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args);
    wxWinUIRootPointerOutcome RouteRootPointerSampleAtEpoch(
        const wxWinUIPointerSample& sample,
        bool sourceAlreadyHandled,
        unsigned long long eventEpoch);
    enum class XamlHitResolution
    {
        Miss,
        Hit,
        Unavailable
    };
    XamlHitResolution PointOverXamlContent(
        const wxPoint& ptClientPx,
        const winrt::Windows::Foundation::Point *exactRootDips = nullptr);

    // Mirror the native target's cursor onto the island root while hovering
    // non-XAML areas (the island otherwise paints its own arrow everywhere);
    // reset to the XAML default when back over real content.
    bool MirrorNativeCursor(const wxWinUINativeTarget& target,
                            LRESULT hitTest,
                            unsigned long long expectedEpoch);
    bool ApplySlotPointerCursor(
        const std::shared_ptr<wxWinUISlotLifetime>& state,
        bool emitSetCursorEvent = false,
        unsigned long long expectedEpoch = 0,
        bool reuseCurrentVerdict = false);
    bool ApplyGenericPointerCursor();
    void ApplyXamlPointerCursor(
        const winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args,
        bool emitSetCursorEvent,
        unsigned long long expectedEpoch);
    void ObserveXamlPointerCursor();
    bool RelinquishIslandPointerCursor();
    bool SetIslandPointerCursor(
        const winrt::Microsoft::UI::Input::InputCursor& cursor,
        WXHCURSOR sourceHandle = nullptr);
    bool ResolveEffectivePointerCursor(
        wxWindow *window,
        winrt::Microsoft::UI::Input::InputCursor *cursor,
        wxCursor *keepAlive = nullptr,
        WXHCURSOR *sourceHandle = nullptr,
        bool *hasWxOverride = nullptr) const;
    void RefreshActivePointerCursor(bool emitSetCursorEvent = true);
    bool RefreshPointerCursorAtLastPoint(bool emitSetCursorEvent = true);
    void ResetRootCursor();
    void CloseNativeHover();
    bool AbortUndeliveredNativeHover(
        const wxWinUIInputAction& nextAction);
    void MarkNativeHoverDispatchStarted(
        const wxWinUIInputAction& action);
    bool BuildRootPointerSample(
        const winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args,
        wxWinUIInputKind kind,
        wxWinUIPointerSample *sample) const;
    bool ClassifyRootPointer(
        const wxPoint& pointClientPx,
        const POINT& pointScreen,
        const winrt::Windows::Foundation::Point *exactRootDips,
        bool sourceAlreadyHandled,
        wxWinUINativeHit *nativeHit,
        wxWinUIRouteObservation *observation,
        bool *hasNativeHit);
    void OnRootPointerBoundary(
        bool entered,
        const winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args);
    bool RouteRootPointerBoundaryAtEpoch(
        bool entered,
        const wxWinUIPointerSample& sample,
        unsigned long long eventEpoch);
    void OnRootPointerInterrupted(
        bool captureLost,
        const winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args);
    void OnRootNativeInterrupted(bool captureLost);
    void HandleRootPointerInterrupted(
        bool captureLost,
        const wxWinUIPointerSample& sample);
    bool SyncNativeCapture(const wxWinUIPointerSample& sample,
                           bool *hasNativeCapture = nullptr,
                           bool *executedActions = nullptr,
                           bool *interruptedPress = nullptr,
                           bool *superseded = nullptr);
    bool ResolveInputTarget(const wxWinUIInputTargetKey& key,
                            wxWinUINativeTarget *target) const;
    bool ExecuteInputTransition(
        const wxWinUIInputTransition& transition,
        const wxWinUINativeHit *currentHit = nullptr,
        bool *superseded = nullptr);
    bool ExecuteInputAction(const wxWinUIInputAction& action,
                            const wxWinUINativeHit *currentHit,
                            unsigned long long expectedEpoch);
    void ExecuteEmergencyInputCleanup(
        const wxWinUIInputAction& action);
    bool QueueModalNativeDispatch(const wxWinUINativeHit& hit,
                                  const wxWinUIInputAction& action,
                                  UINT message,
                                  int virtualKey,
                                  WORD xButton,
                                  wxWinUIInputDevice device,
                                  std::uint32_t pointerId);
    void CancelPendingNativeDispatch(
        const wxWinUIPointerSample& sample);
    bool HasPendingNativeDispatch(
        const wxWinUIPointerSample& sample) const;
    void DispatchPendingNativeInput();
    bool RememberModalAwaitingRelease(
        const wxWinUIInputAction& action);
    wxWinUIInputAction TakeModalAwaitingRelease(
        wxWinUIInputDevice device,
        std::uint32_t pointerId,
        wxWinUIInputButton button);
    void ForgetModalAwaitingRelease(std::uint64_t gestureSerial);
    void PruneModalAwaitingReleases();

    // Force the island's input site to abandon any in-flight pointer
    // interaction.  Called after a press is forwarded to a native target: the
    // physical release then goes to that target (or into its modal loop), the
    // island never sees its own PointerReleased, and its input state machine
    // would otherwise stay stuck "pressed" and swallow all further input
    // (the rapid near-border resize freeze; WM_CANCELMODE was the only cure).
    void CancelIslandPointerState(bool postCancelMode = true);
    void PostIslandCancelMode();
    void PostIslandCancelMode(std::uint32_t expectedGeneration);
    bool CanPostIslandCancelMode(
        std::uint32_t expectedGeneration) const;
    void AcknowledgeIslandCancelMessage(
        std::uint32_t generation);
    void RetireIslandCancelTarget(HWND retiredTarget);
    void RetryDeferredIslandCancelMode();
    void AdvanceInputSiteGeneration();
    void OnSlotPointer(
        const std::shared_ptr<wxWinUISlotLifetime>& state,
        const winrt::Microsoft::UI::Xaml::Controls::Grid& container,
        const winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args,
        wxWinUIInputKind kind);
    bool MapRootPointerPointForSlot(
        const wxWinUISlot& slot,
        double xDIP,
        double yDIP,
        POINT *screen,
        bool *inside) const;
    bool SendSlotMouseEvent(
        const std::shared_ptr<wxWinUISlotLifetime>& state,
        const wxWinUIInputAction& action);
    void ExecuteSlotInputCleanup(
        const std::shared_ptr<wxWinUISlotLifetime>& state,
        const wxWinUIInputAction& action);
    bool RouteSlotPointerSample(
        const std::shared_ptr<wxWinUISlotLifetime>& state,
        const wxWinUIPointerSample& sample,
        wxWinUIInputSurface physicalSurface =
            wxWinUIInputSurface::Native);
    void UpdateSlotPointerOwner(
        const std::shared_ptr<wxWinUISlotLifetime>& state,
        const wxWinUIInputAction& action);
    void RememberSlotRelease(const wxWinUIInputAction& action);
    bool ConsumeSlotRelease(const wxWinUIPointerSample& sample);
    bool HasSlotPointerOwner(const wxWinUIPointerSample& sample);
    void ForgetSlotPointerOwners(
        const std::shared_ptr<wxWinUISlotLifetime>& state);
    bool DispatchSlotOwnedRelease(const wxWinUIPointerSample& sample);
    void CancelSlotPointerOwners(const wxWinUIPointerSample& sample,
                                 bool allPointers);

    // wx event plumbing
    void OnSlotWindowGeometry(wxEvent& event);   // move/size/show
    void OnSlotWindowDestroy(wxWindowDestroyEvent& event);
    void OnTLWSize(wxSizeEvent& event);
    void OnTLWDpiChanged(wxDPIChangedEvent& event);
    void OnTLWVisibility(wxEvent& event);
    void OnTLWDestroy(wxWindowDestroyEvent& event);

    // The island is releasing the focus (Tab pressed on its first/last
    // element): move it to the neighbouring wx tab stop.
    void OnTakeFocusRequested(bool previous);

    enum class FocusOrigin
    {
        LogicalRequest,
        WxSetAlreadyDelivered,
        // A focused logical wx control kept its identity while its XAML slot
        // moved to another TLW. Acquiring the destination peer must not emit
        // a second KILL/SET pair for that same control.
        MigrationContinuation
    };

    struct FocusRequest
    {
        unsigned long long id = 0;
        // A nested/native focus redirection supersedes every request from an
        // older logical intent, even while its stack frame is still alive.
        // All request lookups must match this generation before using the
        // previous-window hint or the WxSetAlreadyDelivered flag.
        unsigned long long intentGeneration = 0;
        std::shared_ptr<wxWinUISlotLifetime> target;
        wxWeakRef<wxWindow> previous;
        WXHWND nativeAuthority = nullptr;
        unsigned long long nativeAuthorityGeneration = 0;
        FocusOrigin origin = FocusOrigin::LogicalRequest;
    };

    struct FocusMigration
    {
        std::shared_ptr<wxWinUIHostLifetime> sourceHost;
        std::shared_ptr<wxWinUIHostLifetime> destinationHost;
        std::shared_ptr<wxWinUISlotLifetime> sourceSlot;
        std::shared_ptr<wxWinUISlotLifetime> destinationSlot;
        wxWeakRef<wxWindow> window;
        // Non-owning comparison key only. Never dereference it: unlike the
        // weak ref it remains available to clear a stale logical owner after
        // synchronous wxWindow destruction.
        wxWindow *windowIdentity = nullptr;
        wxWeakRef<wxWindow> previous;
        WXHWND hwnd = nullptr;
        unsigned long long hwndGeneration = 0;
        // Exact USER32 authority carried by the transport. This starts at
        // the parked shell but can become the destination TLW/island when
        // SetParent synchronously activates it. Unlike hwndGeneration above,
        // this uses the generic native-HWND generation domain.
        WXHWND transportNativeAuthority = nullptr;
        unsigned long long transportNativeAuthorityGeneration = 0;
        wxWeakRef<wxWindow> preparedDestinationTLW;
        WXHWND preparedDestinationTLWHwnd = nullptr;
        unsigned long long preparedDestinationTLWHwndGeneration = 0;
        wxWeakRef<wxWindow> preparedSourceTLW;
        WXHWND preparedSourceTLWHwnd = nullptr;
        unsigned long long preparedSourceTLWHwndGeneration = 0;
        unsigned long long sourceContentGeneration = 0;
        unsigned long long destinationContentGeneration = 0;
        unsigned long long sourceFocusIntentGeneration = 0;
        bool logicalFocusWasActive = false;
        bool deferredFocusWasPending = false;
        FocusOrigin deferredOrigin = FocusOrigin::LogicalRequest;
        WXHWND deferredNativeAuthority = nullptr;
        unsigned long long deferredNativeAuthorityGeneration = 0;
        // Parking the active XAML focus on the control's invisible shell is
        // the only native edge in the transfer. HandleSetFocus() consumes
        // this exact marker so the implementation move cannot publish a
        // duplicate wx SET_FOCUS.
        bool shellFocusParking = false;
        bool shellFocusConsumed = false;
        bool shellLogicalLossDelivered = false;
        bool sourceLostFocusObserved = false;
        // Non-zero only between the projection-free pre-SetParent hook and
        // MigrateSlotToCurrentTLW(). It makes cancellation generation-exact:
        // a stale outer Reparent() can never roll back a nested A->B->C
        // winner or a later transaction reusing the same wxWindow address.
        unsigned long long preparedReparentId = 0;
        bool preparedNativeReparentBoundaryActive = false;
        bool committedSuccessfully = false;
        bool committed = false;
    };

    void FocusSlotImpl(wxWindow *window,
                       FocusOrigin origin,
                       wxWindow *previous,
                       unsigned long long intentGeneration = 0,
                       bool allowDeferredRetry = true);
    void ClearDeferredFocusRequest(
        unsigned long long expectedGeneration = 0);
    bool CommitFocusMigration(
        const std::shared_ptr<FocusMigration>& migration,
        const std::shared_ptr<wxWinUISlotLifetime>& destination);
    void AbortFocusMigration(
        const std::shared_ptr<FocusMigration>& migration,
        bool dispatchLogicalLoss,
        wxWindow *replacement = nullptr);
    static bool HasFocusMigrationNativeAuthority(
        const std::shared_ptr<FocusMigration>& migration,
        wxWinUITopLevelHost *destination = nullptr);
    bool IsSuppressedMigrationLoss(
        const std::shared_ptr<wxWinUISlotLifetime>& source);
    static std::map<wxWindow *, std::shared_ptr<FocusMigration>>
        ms_focusMigrations;
    void RetryDeferredFocusAfterFlush();
    FocusRequest *FindFocusRequest(wxWindow *target);
    wxWindow *FindFocusedSlot() const;
    bool HasNativeFocusAuthority() const;
    void InvalidateFocusIntent(unsigned long long expectedGeneration = 0);
    void TransitionFocus(wxWindow *newOwner,
                         wxWindow *previousHint,
                         bool wxSetAlreadyDelivered,
                         wxWindow *lossDestination = nullptr);
    void OnXamlSlotGotFocus(
        const std::shared_ptr<wxWinUISlotLifetime>& state,
        const winrt::Microsoft::UI::Xaml::Controls::Grid& container);
    void OnXamlSlotLostFocus(
        const std::shared_ptr<wxWinUISlotLifetime>& state,
        const winrt::Microsoft::UI::Xaml::Controls::Grid& container);

    void BindSlotWindow(wxWinUISlot& slot);
    void UnbindSlotWindow(wxWinUISlot& slot);
    void BindSlotAncestors(wxWinUISlot& slot);
    void UnbindSlotAncestors(wxWinUISlot& slot);

    // Reference-counted geometry tracking on the ancestors of the slotted
    // windows: any ancestor move/size/show marks the whole host dirty.
    void AddAncestorRef(wxWindow *ancestor);
    void ReleaseAncestorRef(wxWindow *ancestor);
    void OnAncestorGeometry(wxEvent& event);
    void OnAncestorDestroy(wxWindowDestroyEvent& event);

    // Before initialization commits, m_tlw is an identity/registry key only:
    // every dereference is resolved afresh through m_tlwLifetime and checked
    // against both the wx/HWND and native HWND generations captured before
    // provisional publication. Established host operations keep their
    // existing OperationGuard/lifetime discipline after that commit.
    wxWindow *m_tlw = nullptr;
    wxWeakRef<wxWindow> m_tlwLifetime;
    WXHWND m_tlwHwnd = nullptr;
    unsigned long long m_tlwHwndGeneration = 0;
    unsigned long long m_tlwNativeHwndGeneration = 0;
    bool m_initializing = true;
    bool m_initializationComplete = false;
    bool m_initializationDeferred = false;
    bool m_deferredResumeInProgress = false;
    bool m_tlwSizeBound = false;
    bool m_tlwDpiBound = false;
    bool m_tlwShowBound = false;
    bool m_tlwIconizeBound = false;
    bool m_tlwDestroyBound = false;
    bool m_tlwMoveStartLogBound = false;
    bool m_tlwMoveEndLogBound = false;
    bool m_tlwActivateLogBound = false;

    winrt::Microsoft::UI::Xaml::Hosting::DesktopWindowXamlSource m_source{ nullptr };
    winrt::Microsoft::UI::Xaml::Controls::Canvas m_root{ nullptr };
    winrt::Microsoft::UI::Input::InputPointerSource
        m_inputPointerSource{ nullptr };
    winrt::Microsoft::UI::Input::InputCursor
        m_lastIslandPointerCursor{ nullptr };
    WXHCURSOR m_lastIslandPointerHandle = nullptr;
    bool m_islandPointerCursorApplied = false;
    bool m_inputPointerSourceAuthoritative = false;
    bool m_rootPointerCursorAuthoritative = false;
    winrt::event_token m_takeFocusToken{};
    HWND m_bridge = nullptr;
    HWND m_inner = nullptr;

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    std::unique_ptr<wxWinUIDropBroker> m_dropBroker;
    // RegisterDragDrop() is intentionally deferred while a newly-created TLW
    // is hidden. DesktopWindowXamlSource publishes its native bridge routing
    // during first presentation; acquiring only after the visible geometry
    // flush prevents that publication from superseding our OLE pair.
    bool m_dropBrokerAwaitingPresentation = false;
    bool m_dropBrokerAcquireInProgress = false;
#endif
#if wxUSE_DRAG_AND_DROP
    bool m_bridgeAcceptsFiles = false;
#endif

    // Heap contexts passed to SetWindowSubclass(). They hold only a weak
    // host lifetime, never this. If RemoveWindowSubclass() fails, ownership
    // stays with the HWND and WM_NCDESTROY deletes the inactive context.
    void *m_bridgeSubclassContext = nullptr;
    void *m_innerSubclassContext = nullptr;
    HWND m_innerSubclassRetryWindow = nullptr;
    unsigned m_innerSubclassRetryCount = 0;
    bool m_innerSubclassFailureLogged = false;
    bool m_bridgePinFailureLogged = false;

    std::map<wxWindow *, wxWinUISlot *> m_slots;
    std::vector<wxWinUISlot *> m_deferredDisconnectSlots;
    bool m_deferredDisconnectScheduled = false;

    // Ancestors currently listened on, with their reference count.
    std::map<wxWindow *, int> m_ancestorRefs;

    std::set<wxWindow *> m_dirty;
    bool m_allDirty = false;
    // Structural/overlap invalidation is an epoch, not a bool: a nested
    // FlushSync() may publish a newer generation while an outer SetZIndex()
    // callback is still on the stack. Only a complete pass for the exact
    // snapshotted generation advances m_structureAppliedGeneration.
    unsigned long long m_structureGeneration = 1;
    // The structure generation the pointer sample being routed resolved
    // its native hit at. Equal generations mean no native geometry
    // mutation was reported since, which lets the pre-delivery proof
    // re-check the one target instead of walking the tree again.
    unsigned long long m_hitLayoutGeneration = 0;
    // The last native hit resolved for a pointer sample, kept so that the
    // next sample can reuse it when nothing moved in between: a pointer
    // reports hundreds of times per second and almost always stays over
    // the window it was already over.
    wxWinUINativeHit m_lastResolvedHit;
    unsigned long long m_lastResolvedLayoutGeneration = 0;

    // A synthetic WM_MOUSEMOVE is sent, not posted, so it bypasses the queue
    // arbitration that normally lets WM_PAINT through once input stops. A
    // window that invalidates itself on every movement would then never be
    // painted while the pointer keeps moving. This is the last moment a
    // forced update ran, used to bound how often one is performed.
    unsigned long m_lastPointerPaintTick = 0;

    unsigned long long m_structureAppliedGeneration = 0;
    bool m_flushScheduled = false;
    // Per-host companion to the process-wide diagnostic counter above.
    // Tests which toggle a global policy must distinguish this frozen host
    // from unrelated live hosts legitimately flushed by the same operation.
    unsigned m_flushRunsForTest = 0;
    // A flush ran while the TLW was frozen: one catch-up flush is owed at
    // thaw time (see wxWinUITLWHostNotifyThaw), instead of rescheduling in
    // a loop for the whole freeze.
    bool m_frozenDirty = false;
    bool m_shuttingDown = false;
    bool m_shutdownFinalizing = false;
    bool m_shutdownComplete = false;
    // Stable ownership transferred out of m_slots by the logical shutdown
    // phase. Physical handler revocation/source close can then wait until no
    // routed callback is on the host stack.
    std::vector<wxWinUISlot *> m_shutdownSlots;
    // Unsatisfied gates can still belong to live slots when the TLW begins
    // logical shutdown (DestroyChildren() is later). Snapshot them before
    // the slot map and source/root ownership are retired, then merge them
    // with already-deferred slots into the TLW's one terminal retirement.
    // These aliases were never counted by
    // gs_pendingPhysicalDisconnectGates and must not affect that counter.
    std::vector<std::shared_ptr<wxWinUIPhysicalDisconnectGate>>
        m_shutdownPhysicalDisconnectGates;
    unsigned m_operationDepth = 0;
    bool m_deleteRequested = false;
    bool m_deleteScheduled = false;

    // Captured by the root-router handlers instead of a raw this, so a
    // dispatch racing the teardown finds an invalidated state, not a dying
    // host.  Invalidated first thing in Shutdown().
    std::shared_ptr<wxWinUIHostLifetime> m_hostLifetime;

    // The root-router registrations, revoked in Shutdown() before Close():
    // AddHandler() has no token, removal needs the routed event and the
    // very same boxed delegate -- and revoking them releases their captured
    // m_hostLifetime copies synchronously instead of waiting for the dead
    // XAML tree's deferred finalization.
    std::vector<std::pair<winrt::Microsoft::UI::Xaml::RoutedEvent,
                          winrt::Windows::Foundation::IInspectable>> m_rootHandlers;

    // Focus arbiter: Win32 focus is the authority, XAML follows.
    wxWindow *m_focusOwner = nullptr;
    // Shared by the source and destination only while a logically focused
    // control's slot crosses TLWs. It suppresses the detach LostFocus and is
    // committed solely by real focus in the destination XamlRoot.
    std::shared_ptr<FocusMigration> m_focusMigration;
    std::vector<FocusRequest> m_focusRequests;
    // A single logical intent can straddle the target's XAML load/layout pass
    // (notably initial focus in a newly shown frame-class dialog). Keep the
    // identity, never the wxWindow itself. It receives at most one retry at
    // the end of the coalesced host flush and, if that happened before
    // Loaded, one final retry from the existing control-host Loaded hook.
    // A newer intent invalidates all of this state atomically.
    FocusRequest m_deferredFocusRequest;
    unsigned long long m_deferredFocusContentGeneration = 0;
    unsigned long long m_deferredFocusFlushRetryGeneration = 0;
    bool m_deferredFocusRetryInProgress = false;
    unsigned long long m_nextFocusRequestId = 0;
    unsigned long long m_focusIntentGeneration = 0;
    // Exact slot identity only across the synchronous ::SetFocus(m_bridge).
    // It supplies truthful counterparts to native KILL_FOCUS and suppresses
    // the target shell's implementation-only loss without retaining wxWindow.
    std::weak_ptr<wxWinUISlotLifetime> m_nativeFocusTransferTarget;
    // One-shot identity for the opposite direction. The focused bridge/inner
    // HWND receives WM_KILLFOCUS before XAML LostFocus clears m_focusOwner;
    // latch its exact logical source and native destination so the incoming
    // HWND's wxEVT_SET_FOCUS still names the slot it really left.
    std::weak_ptr<wxWinUISlotLifetime> m_nativeFocusDepartureSource;
    WXHWND m_nativeFocusDepartureDestination = nullptr;
    unsigned long long m_nativeFocusDepartureDestinationGeneration = 0;
    // Native authority displaced when focus enters the bridge without a
    // host FocusRequest (e.g. application-authored XAML calling Focus()).
    // If that routed source unregisters before GotFocus can publish an owner,
    // the deferred disconnect restores this exact generation instead of
    // leaving an ownerless InputSite focused.
    WXHWND m_nativeFocusEntryAuthority = nullptr;
    unsigned long long m_nativeFocusEntryAuthorityGeneration = 0;
    std::weak_ptr<wxWinUISlotLifetime> m_nativeFocusEntryTarget;
    unsigned long long m_nativeFocusEntryEpoch = 0;
    unsigned long long m_nextNativeFocusEntryEpoch = 0;
    // A failed host focus attempt may restore the same slotted shell which
    // started it. Its nested WM_SETFOCUS is an implementation rollback, not
    // a new logical request: consume it exactly once by identity+epoch.
    std::weak_ptr<wxWinUISlotLifetime> m_nativeFocusRollbackTarget;
    WXHWND m_nativeFocusRollbackShell = nullptr;
    unsigned long long m_nativeFocusRollbackShellGeneration = 0;
    unsigned long long m_nativeFocusRollbackEpoch = 0;
    unsigned long long m_nextNativeFocusRollbackEpoch = 0;

    // Last native window the root router forwarded mouse input to, for
    // Native hover bookkeeping uses a generation-safe identity: an HWND can
    // be destroyed and reused between any two re-entrant callbacks.
    wxWinUINativeTarget m_lastNativeTarget;
    wxWinUINativeArea m_lastNativeArea = wxWinUINativeArea::Client;
    // Exact HoverEnter committed by the semantic state but not yet exposed to
    // a native wndproc. A failed final refresh can roll it back silently;
    // once SendMessage begins it is cleared before application re-entrance.
    wxWinUIInputAction m_undeliveredNativeHoverEnter;
    bool m_hasUndeliveredNativeHoverEnter = false;

    // Pure, hostless-tested ordering policy for hover/click/capture/pointer
    // parity and the proof-armed storm guard. The adapter below owns all HWND
    // validation and executes only the semantic actions it returns.
    wxWinUIInputState m_inputState;
    // Incremented for every transition execution. If a callback re-enters
    // the router, the outer executor abandons its now-stale remaining
    // actions instead of cancelling or dispatching a newly-created gesture.
    unsigned long long m_inputTransitionEpoch = 0;

    // Scrollbar cutouts of the current bridge region, to skip SetWindowRgn
    // when nothing changed.
    std::vector<RECT> m_regionCuts;
    bool m_regionValid = false;
    bool m_regionRTL = false;
    bool m_regionFailureLogged = false;

    // Cursor mirroring state: last (target, hit-test) pair and whether a
    // native cursor currently overrides the root's.
    wxWinUINativeTarget m_cursorTarget;
    LRESULT m_cursorHit = 0;
    bool m_cursorMirrored = false;
    wxPoint m_nativeCursorVerdictPoint;
    wxPoint m_nativeCursorVerdictClientPoint;
    wxSize m_nativeCursorVerdictClientSize;
    std::vector<WXHWND> m_nativeCursorVerdictChain;
    enum class ActiveCursorSurface
    {
        GenericXaml,
        Slot,
        Native
    };
    ActiveCursorSurface m_activeCursorSurface =
        ActiveCursorSurface::GenericXaml;
    std::weak_ptr<wxWinUISlotLifetime> m_activeCursorSlot;
    unsigned long long m_activeCursorPolicyGeneration = 0;
    bool m_refreshingPointerCursor = false;
    bool m_pointerCursorRefreshPending = false;
    bool m_pointerCursorRefreshNeedsSetCursorEvent = false;
    unsigned m_cursorResolutionDepth = 0;
    bool m_cursorPolicyReplayPending = false;
    // Advances whenever this host commits an island/root/native cursor
    // selection. It lets an outer routed callback preserve a newer nested
    // topology winner even when neither pointer epoch nor global policy
    // changed.
    unsigned long long m_cursorSelectionGeneration = 0;

    struct PendingNativeDispatch
    {
        wxWinUINativeHit hit;
        wxWinUIInputAction action;
        UINT message = 0;
        int virtualKey = 0;
        WORD xButton = 0;
        wxWinUIInputDevice device = wxWinUIInputDevice::Mouse;
        std::uint32_t pointerId = 0;
        unsigned long long sequence = 0;
        unsigned long long cancellationGeneration = 0;
        std::uint32_t inputSiteGeneration = 0;
        bool pointerInsideRoot = true;
    };
    std::deque<PendingNativeDispatch> m_pendingNativeInput;
    // Shares the host's capture/InputSite cancellation authority, but keeps
    // the grip's typed peer validation separate from native geometric hits.
    std::shared_ptr<NativeResizeRequest> m_nativeResize;
    wxWinUINativeResizeSnapshot m_nativeResizeSnapshot;
    bool m_nativeResizeWakePosted = false;
    PendingNativeDispatch m_inFlightNativeInput;
    unsigned long long m_nextNativeInputSequence = 0;
    unsigned long long m_nativeInputCancellationGeneration = 0;
    bool m_nativeInputDispatchScheduled = false;
    bool m_nativeInputDispatchInFlight = false;
    bool m_nativeInputDispatchCommitted = false;
    bool m_cancelingIslandPointerState = false;
    std::uint32_t m_inputSiteGeneration = 0;
    std::uint32_t m_deferredIslandCancelGeneration = 0;
    bool m_islandCancelDeferred = false;
    std::uint32_t m_postedIslandCancelGeneration = 0;
    unsigned m_postedIslandCancelMessages = 0;
    unsigned long long m_nativeCaptureMutationGeneration = 0;
    // Exact committed modal DOWNs whose system loops returned before their
    // physical contacts ended. Mouse buttons are independent and inputstate
    // supports five concurrent presses, so this must never be a single
    // overwritable slot. Primary touch/pen add at most one identity each in
    // the V0 policy; the same conservative bound as slot owners is ample.
    static constexpr std::size_t ModalAwaitingReleaseCapacity = 15;
    std::array<wxWinUIInputAction,
               ModalAwaitingReleaseCapacity> m_modalAwaitingReleases;

    struct SlotPointerOwner
    {
        std::weak_ptr<wxWinUISlotLifetime> state;
        wxWinUIInputDevice device = wxWinUIInputDevice::Mouse;
        std::uint32_t pointerId = 0;
        wxWinUIInputButton button = wxWinUIInputButton::None;
        std::uint64_t gestureSerial = 0;
        std::uint64_t downTimestamp = 0;

        bool IsActive() const { return gestureSerial != 0; }
        void Clear() { *this = SlotPointerOwner(); }
    };
    // Mouse exposes five independent buttons; primary touch and pen can add
    // their own identities. This fixed table covers every V0 combination
    // without allocating in the pointer hot path.
    std::array<SlotPointerOwner, 15> m_slotPointerOwners;

    struct SlotReleaseReceipt
    {
        wxWinUIInputDevice device = wxWinUIInputDevice::Mouse;
        std::uint32_t pointerId = 0;
        wxWinUIInputButton button = wxWinUIInputButton::None;
        std::uint64_t timestamp = 0;
        std::uint64_t gestureSerial = 0;

        bool IsActive() const { return gestureSerial != 0; }
        void Clear() { *this = SlotReleaseReceipt(); }
    };
    // A slot release bubbles to the root after the slot handler. Remember
    // exactly that physical sample so a reentrant DOWN from the wx UP handler
    // cannot be mistaken for the owner of the old bubbling release.
    std::array<SlotReleaseReceipt, 15> m_slotReleaseReceipts;

    // Screen position of the last pointer event the island delivered; feeds
    // the spurious-WM_SETCURSOR filter in BridgeSubclassProc (see there).
    POINT m_lastPointerScreen = { -100000, -100000 };

    wxDECLARE_NO_COPY_CLASS(wxWinUITopLevelHost);
};

// Build the exact Raw-only shell provider used by WM_GETOBJECT without
// passing it through USER32's LRESULT marshalling. This implementation-only
// seam lets lifecycle tests retain a real provider across slot generations.
// The returned COM reference is owned by the caller.
WXDLLIMPEXP_CORE HRESULT
wxWinUITestCreateInvisibleShellProvider(
    wxWindow *window,
    IRawElementProviderSimple **provider);

// Cumulative count of exact shell-provider retirements performed through the
// real WM_DESTROY/DoDetachHWND path.
WXDLLIMPEXP_CORE unsigned
wxWinUITestGetAccessibilityShellProviderRetireCount();

// Number of live Raw-only invisible shell provider objects. Unlike the
// cumulative retirement count, this catches a retained UIA fragment after its
// slot/owner has gone away. Providers can be released by UIA from a COM
// callback thread, so the implementation uses an atomic counter.
WXDLLIMPEXP_CORE unsigned
wxWinUITestGetLiveInvisibleShellProviderCount();

// ----------------------------------------------------------------------------
// wxWinUIControlHostProbe: test-only handle onto a real wxWinUIControlHost.
//
// wxWinUIControlHost is an internal (non-exported) class and, since the
// per-TLW rearchitecture, nothing in the library drives its ClearContent()
// path.  This exported probe lets the lifecycle unit tests exercise that
// path directly: create a host on a window, set/clear its content and close
// it, observing the results through the shared host's introspection
// (FindSlot / GetContent / GetLiveLoadedHookCount).
// ----------------------------------------------------------------------------

class wxWinUIControlHost;

class WXDLLIMPEXP_CORE wxWinUIControlHostProbe
{
public:
    // Build an initialized proxy without registering a slot yet. This is the
    // deterministic entry point for the first-registration reentrancy tests.
    static wxWinUIControlHostProbe *CreateEmpty(wxWindow *window);

    // Build a control host on `window` and set an initial Button content.
    // Returns null on failure.  The probe owns the control host.
    static wxWinUIControlHostProbe *Create(wxWindow *window);
    ~wxWinUIControlHostProbe();

    // wxWinUIControlHost::SetContent() with a fresh Button (a new element).
    // The label makes nested generations distinguishable in the real slot.
    bool SetContent(const wxString& label = "probe");

    // Snapshot of the proxy's own committed content model, independent of the
    // shared slot. Tests compare both identities after nested publication.
    winrt::Microsoft::UI::Xaml::UIElement GetContentForTesting() const;

    // Delete the owned wxWinUIControlHost while leaving this wrapper alive.
    // Used only to exercise destruction at the synchronous attach boundary.
    void DestroyControlHostForTesting();

    // wxWinUIControlHost::ClearContent().
    void ClearContent();

    // Drive the exact physical-pixel clip seam through the real control host.
    // Used by migration tests whose detach callback must exercise the same
    // FindSlotOwner()/DIP conversion path as wxNotebook.
    void SetBridgeClipRect(const wxRect& physicalRect);

    // wxWinUIControlHost::Close().
    void Close();

private:
    wxWinUIControlHostProbe() = default;

    wxWinUIControlHost *m_host = nullptr;

    wxDECLARE_NO_COPY_CLASS(wxWinUIControlHostProbe);
};

#if wxUSE_TOOLTIPS
// Internal deterministic cleanup seam; not part of the wx public API.
WXDLLIMPEXP_CORE unsigned wxWinUIGetManagedToolTipCountForTesting();

enum wxWinUIToolTipFaultForTesting
{
    wxWinUIToolTipFault_None = 0,
    wxWinUIToolTipFault_SetBeforeCommit = 1,
    wxWinUIToolTipFault_SetAfterCommit = 2,
    wxWinUIToolTipFault_Policy = 4
};

// Faults are consumed independently for each selected seam. UINT_MAX means a
// permanent fault, used to prove that deferred policy replay is bounded.
WXDLLIMPEXP_CORE void
wxWinUISetToolTipFaultForTesting(unsigned faults, unsigned count);
WXDLLIMPEXP_CORE void wxWinUIResetToolTipPolicyDiagnosticsForTesting();
WXDLLIMPEXP_CORE unsigned
wxWinUIGetToolTipPolicyFaultAttemptCountForTesting();
WXDLLIMPEXP_CORE unsigned
wxWinUIGetToolTipPolicyReplayScheduleCountForTesting();
WXDLLIMPEXP_CORE unsigned
wxWinUIGetToolTipPolicyReplayExecutionCountForTesting();
WXDLLIMPEXP_CORE unsigned
wxWinUIGetToolTipPolicyQuarantinedEntryCountForTesting();
WXDLLIMPEXP_CORE bool
wxWinUIIsToolTipPolicyReplayScheduledForTesting();
WXDLLIMPEXP_CORE bool wxWinUIIsToolTipPolicyShutdownForTesting();

// Deterministic physical-retirement seam. It exercises host ownership,
// migration, shutdown and counter contracts without using sleeps or treating
// an arbitrary dispatcher turn as proof of popup causality.
WXDLLIMPEXP_CORE std::shared_ptr<wxWinUIPhysicalDisconnectGate>
wxWinUICreatePhysicalDisconnectGateForTesting(
    const winrt::Microsoft::UI::Dispatching::DispatcherQueue& queue,
    bool satisfied = true,
    bool degraded = false);
WXDLLIMPEXP_CORE bool wxWinUISetPhysicalDisconnectGateStateForTesting(
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate,
    bool satisfied,
    bool degraded = false);
WXDLLIMPEXP_CORE bool wxWinUIIsPhysicalDisconnectGateSatisfiedForTesting(
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate);
WXDLLIMPEXP_CORE bool wxWinUIIsPhysicalDisconnectGateDegradedForTesting(
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate);
WXDLLIMPEXP_CORE bool
wxWinUIOpenPhysicalDisconnectGateAtSealedCompletionForTesting(
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate);
WXDLLIMPEXP_CORE bool
wxWinUIOpenPhysicalDisconnectGateBeforeSourceCloseForTesting(
    const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate);
WXDLLIMPEXP_CORE bool
wxWinUIIsPhysicalDisconnectPublicationPoisoned() noexcept;
WXDLLIMPEXP_CORE void
wxWinUIResetPhysicalDisconnectPublicationPoisonForTesting() noexcept;
WXDLLIMPEXP_CORE void
wxWinUIPoisonPhysicalDisconnectPublicationForTesting() noexcept;
#endif

#endif // wxUSE_WINUI3

#endif // _WX_WINUI_PRIVATE_TLWHOST_H_
