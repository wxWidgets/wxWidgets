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
class wxComboBox;
class wxTextCompleter;

// A self-UI Automation client must run away from the XAML UI thread. Keep
// only lock-free completion telemetry in the control implementation: the MTA
// worker owns no wx/WinRT object and may outlive a control teardown safely.
struct wxWinUIComboAutomationTestState;

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
std::shared_ptr<wxWinUIPhysicalDisconnectGate>
wxWinUICreateCompositePhysicalDisconnectGate();
bool wxWinUIAddCompositePhysicalDisconnectGate(
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
WXDLLIMPEXP_CORE void
wxWinUIResetPhysicalDisconnectPublicationPoisonForTesting() noexcept;

class wxWinUIControlHost
{
public:
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

    // Implementation-only snapshot used by wxWinUIControlHostProbe to prove
    // that the proxy model and the shared slot still describe the same
    // content after a synchronous nested SetContent().
    winrt::Microsoft::UI::Xaml::UIElement GetContentForTesting() const
        { return m_content; }

    // The wx control whose slot this is.
    wxWindow *HostedWindow() const { return m_window; }

    // Implementation-only seam used to exercise callers which must survive
    // synchronous application re-entry from the Loaded path of SetContent().
    // The hook is one-shot and is invoked as the final operation of
    // OnContentLoaded().
    void SetNextContentLoadedHookForTesting(std::function<void()> hook)
    {
        m_nextContentLoadedHookForTesting = std::move(hook);
    }
    // Deterministically crosses the pending Loaded boundary for Create()
    // lifetime tests. OnContentLoaded() may destroy this host, so the
    // implementation performs no access after dispatching it.
    void DispatchPendingContentLoadedHookForTesting();

private:
    void OnContentLoaded();
    void RevokeLoadedHook();

    wxWindow *m_window = nullptr;
    winrt::Microsoft::UI::Xaml::UIElement m_content{ nullptr };
    winrt::event_token m_loadedToken{};
    std::shared_ptr<wxWinUILoadedState> m_loadedState;
    std::function<void()> m_nextContentLoadedHookForTesting;
    std::shared_ptr<wxWinUIPhysicalDisconnectGate>
        m_physicalDisconnectGate;
    bool m_contentLoaded = false;
};

// Current element theme requested by the application (Default == follow system).
winrt::Microsoft::UI::Xaml::ElementTheme wxWinUIGetCurrentElementTheme();
bool wxWinUIIsDarkTheme();

void wxWinUILogException(const char *what, const winrt::hresult_error& e);
winrt::hstring wxWinUIToHString(const wxString& str);
wxString wxWinUIFromHString(const winrt::hstring& str);

// Convert a wxBitmap to a WinUI WriteableBitmap (premultiplied BGRA).  Returns
// nullptr for an invalid bitmap.
class wxBitmap;
winrt::Microsoft::UI::Xaml::Media::Imaging::WriteableBitmap
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

std::shared_ptr<wxWinUIFlyoutPopupRetirementState>
wxWinUICreateFlyoutPopupRetirementState(
    const winrt::Microsoft::UI::Xaml::Controls::MenuFlyout& flyout,
    const winrt::Microsoft::UI::Xaml::XamlRoot& xamlRoot,
    const winrt::Microsoft::UI::Xaml::DependencyObject& exactItem) noexcept;
std::shared_ptr<wxWinUIPhysicalDisconnectGate>
wxWinUIGetFlyoutPopupRetirementGate(
    const std::shared_ptr<wxWinUIFlyoutPopupRetirementState>& state) noexcept;
void wxWinUIResolveFlyoutPopupAfterShow(
    const std::shared_ptr<wxWinUIFlyoutPopupRetirementState>& state) noexcept;
void wxWinUIFailFlyoutPopupShow(
    const std::shared_ptr<wxWinUIFlyoutPopupRetirementState>& state) noexcept;
bool wxWinUISetFlyoutPopupRetirementObserver(
    const std::shared_ptr<wxWinUIFlyoutPopupRetirementState>& state,
    std::function<void ()> observer) noexcept;
bool wxWinUIIsComboPopupRetirementSatisfied(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept;
bool wxWinUIIsComboPopupRetirementSealed(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept;
bool wxWinUIArmComboPopupRetirementCompletion(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state,
    std::function<void ()> completion) noexcept;
bool wxWinUIObserveComboPopupOpened(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept;
bool wxWinUIResolveComboPopupAfterOpen(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept;
bool wxWinUIPrepareComboPopupOpen(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept;
void wxWinUICancelPreparedComboPopupOpen(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept;
bool wxWinUIFinishPreparedComboPopupOpen(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept;
void wxWinUIAbortPreparedComboPopupOpen(
    const std::shared_ptr<wxWinUIComboPopupRetirementState>& state) noexcept;

class wxWinUIChoiceImpl
{
public:
    ~wxWinUIChoiceImpl();

    // Revoke every callback which can reach wxChoice/wxComboBox before
    // detaching the peer from the shared host. This is deliberately
    // idempotent: wxChoice calls it while its item/text state is still alive,
    // and the impl destructor calls it again as a final safety net.
    void Close();

    wxWinUIControlHost host;
    std::shared_ptr<wxWinUIChoiceCallbackState> callbackState;
    winrt::Microsoft::UI::Xaml::Controls::ComboBox comboBox{ nullptr };
    // Editable ComboBox uses a stable layout root as the slot visual while
    // comboBox remains the semantic/focus peer. This keeps the host's common
    // style observers off the x:Load-backed templated control itself.
    winrt::Microsoft::UI::Xaml::Controls::Grid editableRoot{ nullptr };
    winrt::event_token selectionChangedToken{};
    winrt::event_token choiceKeyDownToken{};
    winrt::event_token choiceDropDownOpenedToken{};
    winrt::event_token choiceDropDownClosedToken{};
    // Created before the ComboBox event delegates are published, but dormant
    // (and free of global FocusManager hooks) until the first real open. Its
    // callbacks never retain or dereference the wxChoice owner.
    std::shared_ptr<wxWinUIComboPopupRetirementState>
        popupRetirementState;
    std::shared_ptr<wxWinUIPhysicalDisconnectGate>
        physicalDisconnectGate;
    std::vector<std::uint64_t> peerItemIds;
    bool peerItemsValid = true;
    std::uint64_t pendingItemId = 0;
    bool hasPendingSelection = false;
    bool choiceDropDownOpen = false;
    bool popupReopenPending = false;
    std::uint64_t popupReopenGeneration = 0;
    unsigned popupReopenSchedulesForTesting = 0;
    unsigned popupReopenRunsForTesting = 0;

    // wxCB_SIMPLE has no WinUI ComboBox equivalent. Its production peer is a
    // real two-row surface: an editable TextBox followed by a ListBox which
    // remains in the visual tree at all times. Choice item operations route
    // to simpleListBox while it is present; comboBox remains the peer for all
    // other styles.
    winrt::Microsoft::UI::Xaml::Controls::Grid simpleRoot{ nullptr };
    winrt::Microsoft::UI::Xaml::Controls::ListBox
        simpleListBox{ nullptr };
    winrt::event_token simpleSelectionChangedToken{};
    long long simpleTextChangedCallbackToken = 0;
    winrt::event_token simpleKeyDownToken{};
    // SIMPLE is initially constructed while its shared-host slot is still
    // collapsed.  Its first useful editor extent therefore arrives on a
    // later LayoutUpdated edge.  Coalesce that exact edge onto the island
    // dispatcher and serialize it with synchronous DoSetSize() realizations.
    bool simpleLayoutQueued = false;
    bool simpleLayoutInProgress = false;
    bool simpleLayoutReplayPending = false;
    std::uint64_t simpleLayoutRevision = 0;
    int simpleLayoutQueuedWidthPixels = 0;
    int simpleLayoutQueuedHeightPixels = 0;
    double simpleLayoutQueuedScale = 0.0;
    double simpleLayoutQueuedEditHeight = 0.0;
    std::uint64_t simpleLayoutEpoch = 0;
    std::uint64_t simpleLayoutAppliedEpoch = 0;

    // wxComboBox only: the inner TextBox of an editable ComboBox (resolved
    // transactionally from the current control-template generation), and its
    // hooks. The outer Choice callback state remains responsible for item
    // selection; textCallbackState owns every derived-class delegate.
    std::shared_ptr<wxWinUITextCallbackState> textCallbackState;
    winrt::Microsoft::UI::Xaml::Controls::TextBox editBox{ nullptr };
    winrt::Microsoft::UI::Xaml::Controls::TextBox
        retiredEditBoxForTesting{ nullptr };
    winrt::event_token textSubmittedToken{};
    long long textChangedCallbackToken = 0;
    long long editTextChangedCallbackToken = 0;
    winrt::event_token editSelectionChangedToken{};
    winrt::event_token editLayoutUpdatedToken{};
    winrt::event_token editKeyDownToken{};
    winrt::event_token editOuterKeyDownToken{};
    winrt::event_token editPointerPressedToken{};
    winrt::event_token editCopyingToken{};
    winrt::event_token editCuttingToken{};
    winrt::event_token editPasteToken{};
    winrt::event_token comboLoadedToken{};
    winrt::event_token comboLayoutUpdatedToken{};
    winrt::Microsoft::UI::Xaml::FrameworkElement
        comboThemeBoundaryRoot{ nullptr };
    winrt::event_token comboThemeBoundaryChangedToken{};
    long long comboTemplateChangedCallbackToken = 0;
    winrt::event_token dropDownOpenedToken{};
    winrt::event_token dropDownClosedToken{};
    std::shared_ptr<wxTextCompleter> autoCompleter;
    winrt::Microsoft::UI::Xaml::Controls::MenuFlyout
        autoCompleteFlyout{ nullptr };
    std::shared_ptr<wxWinUIFlyoutPopupRetirementState>
        autoCompleteRetirementState;
    wxString pendingTextValue;
    // Scope a programmatic Text writer to both the exact template editor and
    // the stable selected item it accompanied. XAML may deliver an older
    // TextProperty notification after a re-entrant selection/item mutation;
    // that stale notification must not consume the newer writer's marker.
    std::uint64_t pendingTextEditGeneration = 0;
    std::uint64_t pendingTextItemId = 0;

    // A template/theme replacement can project a final SelectAll after the
    // synchronous SelectedIndex -> Text -> Select replay. A revision is
    // created only by that exact divergent SelectionChanged edge. Its queued
    // passive confirmation may retire only the same revision.
    long pendingTextSelectionStart = 0;
    long pendingTextSelectionEnd = 0;
    std::uint64_t pendingTextSelectionGeneration = 0;
    std::uint64_t pendingTextSelectionEpoch = 0;
    std::uint64_t pendingTextSelectionRevision = 0;
    std::uintptr_t pendingTextSelectionEditIdentity = 0;
    bool hasPendingTextSelection = false;
    // Test-only one-shot consumed as the final action of the genuine
    // SelectionChanged projection branch, after its confirmation was queued.
    // It is moved out before invocation because it may delete the owner.
    std::function<void(wxComboBox *)>
        pendingRangeProjectionHookForTesting;
    // Test-only intent consumed only by the exact current range confirmation,
    // after it has retired the template ticket on the editor dispatcher.
    std::function<void(wxComboBox *)>
        pendingRangeFinalizationHookForTesting;
    // Test-only one-shot consumed as the final action of the genuine ordinary
    // SelectionChanged handler, after it imported the live peer range.
    std::function<void(wxComboBox *)>
        independentRangeObservationHookForTesting;
    std::uint64_t editGeneration = 0;
    std::uint64_t dropDownGeneration = 0;
    std::uint64_t autoCompleteGeneration = 0;
    int autoCompleteSelection = -1;
    bool autoCompleteRetirementPending = false;
    bool autoCompleteReplayPending = false;
    bool hasPendingTextValue = false;
    bool clipboardOperationInProgress = false;
    bool comboPartResolving = false;
    bool comboPartPending = false;
    // LayoutUpdated is an observation edge only. If the template part is
    // missing or changed, one exact dispatcher ticket performs the mutating
    // realization after the callback has returned. Keep the queued latch set
    // for the complete continuation, including any nested UpdateLayout().
    bool comboLayoutResolveQueued = false;
    bool comboLayoutResolveRunning = false;
    bool comboLayoutForcedRetryAttempted = false;
    unsigned comboLayoutCallbackDepth = 0;
    std::uint64_t comboLayoutResolveRevision = 0;
    std::uint64_t comboLayoutForcedRetryGeneration = 0;
    bool comboTemplateReplayPending = false;
    bool comboTemplateTransition = false;
    bool dropDownOpen = false;

    // Passive Gate diagnostics. These are deliberately plain values so the
    // observation seam never has to cross a XAML boundary to report them.
    std::uint64_t diagnosticSelectionDispatches = 0;
    std::uint64_t diagnosticSelectionSnapshotDeliveries = 0;
    std::uint64_t diagnosticPeerTextCallbacks = 0;
    std::uint64_t diagnosticPeerTextDeliveries = 0;
    wxString diagnosticSelectionBeforeHandler;
    wxString diagnosticSelectionAfterHandler;
    wxString diagnosticLastPeerText;
    bool diagnosticSelectionTextAllowed = false;
    bool diagnosticSelectionOwnerSurvived = false;
    std::uint64_t diagnosticKeyPreviewCallbacks = 0;
    std::uint64_t diagnosticKeyPreviewHandled = 0;
    unsigned diagnosticLastPreviewKey = 0;
    std::uintptr_t diagnosticKeyTargetIdentity = 0;
    std::uintptr_t diagnosticKeySenderIdentity = 0;
    bool diagnosticLastPreviewHandledOnEntry = false;
    int diagnosticAutomationStage = 0;
    std::shared_ptr<wxWinUIComboAutomationTestState>
        diagnosticAutomationOperation;
    std::uint64_t diagnosticTemplateTransitions = 0;
    std::uint64_t diagnosticTemplateReplayRequests = 0;
    std::uint64_t diagnosticTemplateReplayAttempts = 0;
    std::uint64_t diagnosticTemplateReplaySuccesses = 0;
    std::uint64_t diagnosticComboLayoutEdges = 0;
    std::uint64_t diagnosticComboLayoutResolveRequests = 0;
    std::uint64_t diagnosticComboLayoutResolveRuns = 0;
    std::uint64_t diagnosticComboLayoutCoalescedEdges = 0;
    std::uint64_t diagnosticComboLayoutRealizations = 0;
    std::uint64_t diagnosticComboLayoutSynchronousRealizations = 0;
    std::uint64_t diagnosticComboLayoutUnlatchedRealizations = 0;
    std::uint64_t diagnosticEditLayoutEdges = 0;
    std::uint64_t diagnosticRangeProjectionMismatches = 0;
    std::uint64_t diagnosticRangeFinalizations = 0;
    std::uint64_t diagnosticSimpleRealizeAttempts = 0;
    int diagnosticSimpleRequestedWidth = 0;
    int diagnosticSimpleRequestedHeight = 0;
    int diagnosticSimpleSourceWidthPixels = 0;
    int diagnosticSimpleSourceHeightPixels = 0;
    double diagnosticSimpleRasterizationScale = 0.0;
    double diagnosticSimpleComputedWidth = 0.0;
    double diagnosticSimpleComputedHeight = 0.0;
    double diagnosticSimpleArrangedHeight = 0.0;
    double diagnosticSimpleListHeight = 0.0;
    double diagnosticSimpleViewportHeight = 0.0;
};

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

#endif // _WX_WINUI_PRIVATE_H_
