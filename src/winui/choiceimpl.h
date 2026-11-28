/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/choiceimpl.h
// Purpose:     private wxChoice/wxComboBox implementation state
// Author:      wxWidgets development team
// Created:     2026-09-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_CHOICEIMPL_H_
#define _WX_WINUI_CHOICEIMPL_H_

#include "private.h"

class wxComboBox;
class wxTextCompleter;

#ifdef WXWINUI_TEST_SUPPORT
struct wxWinUIComboAutomationTestState;
#endif

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
#ifdef WXWINUI_TEST_SUPPORT
    unsigned popupReopenSchedulesForTesting = 0;
    unsigned popupReopenRunsForTesting = 0;
#endif

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
    // collapsed. Its first useful editor extent therefore arrives on a later
    // LayoutUpdated edge. Coalesce that exact edge onto the island dispatcher
    // and serialize it with synchronous DoSetSize() realizations.
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
#ifdef WXWINUI_TEST_SUPPORT
    winrt::Microsoft::UI::Xaml::Controls::TextBox
        retiredEditBoxForTesting{ nullptr };
#endif
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
#ifdef WXWINUI_TEST_SUPPORT
    // One-shot consumed as the final action of the genuine SelectionChanged
    // projection branch, after its confirmation was queued. It is moved out
    // before invocation because it may delete the owner.
    std::function<void(wxComboBox *)> pendingRangeProjectionHookForTesting;
    // Intent consumed only by the exact current range confirmation, after it
    // has retired the template ticket on the editor dispatcher.
    std::function<void(wxComboBox *)> pendingRangeFinalizationHookForTesting;
    // One-shot consumed as the final action of the genuine ordinary
    // SelectionChanged handler, after it imported the live peer range.
    std::function<void(wxComboBox *)>
        independentRangeObservationHookForTesting;
#endif
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

#ifdef WXWINUI_TEST_SUPPORT
    // Passive diagnostics stay in the owning implementation so observation
    // never has to cross a XAML boundary.
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
#endif
};

#endif // _WX_WINUI_CHOICEIMPL_H_
