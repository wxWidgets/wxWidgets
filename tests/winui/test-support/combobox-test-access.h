/////////////////////////////////////////////////////////////////////////////
// Private, non-installed access to wxComboBox test operations.
// Available only in an explicitly test-enabled WinUI library build.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_COMBOBOX_TEST_ACCESS_H
#define WX_WINUI_COMBOBOX_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
    #error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/combobox.h"
#include <cstdint>
#include <memory>

#if wxUSE_COMBOBOX
class WXDLLIMPEXP_CORE wxWinUIComboBoxTestAccess final
{
public:
    // Implementation-only deterministic seams. They mutate the real WinUI
    // dependency properties/delegates without synthesizing keyboard or mouse
    // input and are intentionally not part of wxComboBoxBase.
    static bool SetPeerText(wxComboBox* control, const wxString& text);
    static bool SetPeerTextViaAutomation(
        wxComboBox* control,
        const wxString& text);
    static bool SelectPeerItem(wxComboBox* control, int selection);
    static bool Retemplate(wxComboBox* control);
    static bool RunTemplateLayoutEdge(
        wxComboBox* control,
        bool forceTransition);
    static bool QueueTemplateLayoutResolution(wxComboBox* control);
    static bool SetEditSelection(
        wxComboBox* control,
        long from,
        long to,
        bool retiredPart = false);
    // Write the current TextBox range without pre-classifying the write as a
    // wx/native-authority boundary. This exercises the production
    // SelectionChanged projection classifier.
    static bool SetRawEditSelection(wxComboBox* control, long from, long to);
    struct WinUIRangeSequenceSnapshot
    {
        bool awaitedExistingProjection = false;
        bool firstSelectAllInjected = false;
        bool firstProjectionObserved = false;
        std::uint64_t firstRangeProjectionMismatches = 0;
        std::uint64_t firstRangeFinalizations = 0;
        std::uint64_t firstPendingRangeRevision = 0;
        long firstPeerSelectionStart = -1;
        long firstPeerSelectionLength = -1;

        bool secondSelectAllInjected = false;
        bool secondProjectionObserved = false;
        std::uint64_t secondRangeProjectionMismatches = 0;
        std::uint64_t secondRangeFinalizations = 0;
        std::uint64_t secondPendingRangeRevision = 0;
        long secondPeerSelectionStart = -1;
        long secondPeerSelectionLength = -1;

        bool independentIntentArmed = false;
        bool independentIntentConsumed = false;
        bool independentObservationArmed = false;
        bool independentRangeInjected = false;
        bool independentRangeObserved = false;
        std::uint64_t finalizedRangeProjectionMismatches = 0;
        std::uint64_t finalizedRangeFinalizations = 0;
        std::uint64_t independentRangeProjectionMismatches = 0;
        std::uint64_t independentRangeFinalizations = 0;
        long independentModelSelectionStart = -1;
        long independentModelSelectionEnd = -1;
        long independentPeerSelectionStart = -1;
        long independentPeerSelectionLength = -1;
        bool independentPendingRange = true;
    };
    // Chain two divergent SelectAll writes and one independent range from the
    // ends of their genuine deferred SelectionChanged callbacks. No callback
    // is invoked synthetically and no dispatcher is pumped by this seam.
    static bool ChainPendingRangeCallbacks(
        wxComboBox* control,
        long independentFrom,
        long independentTo,
        const std::shared_ptr<WinUIRangeSequenceSnapshot>& snapshot);
    struct WinUIRangeDestructionSnapshot
    {
        bool causalHookEntered = false;
        std::uint64_t rangeProjectionMismatches = 0;
        std::uint64_t rangeFinalizations = 0;
        std::uint64_t pendingRangeRevision = 0;
        std::size_t liveCountAfterDestroy =
            static_cast<std::size_t>(-1);
        bool lowDrainObserved = false;
    };
    // Arm a one-shot at the end of the genuine SelectAll projection callback.
    // The hook deletes this control only after the exact range confirmation
    // was queued, and records its causal state before doing so.
    static bool SetRawEditSelectionAndDestroy(
        wxComboBox* control,
        long from,
        long to,
        const std::shared_ptr<WinUIRangeDestructionSnapshot>& snapshot);
    static unsigned GetTemplateState(const wxComboBox* control);
    struct WinUITemplatePeerSnapshot
    {
        unsigned state = 0;
        std::uintptr_t hostContentIdentity = 0;
        std::uintptr_t visualRootIdentity = 0;
        std::uintptr_t visualRootXamlRootIdentity = 0;
        std::uintptr_t visualRootLogicalParentIdentity = 0;
        std::uintptr_t visualRootVisualParentIdentity = 0;
        std::uintptr_t comboIdentity = 0;
        std::uintptr_t templateIdentity = 0;
        std::uintptr_t xamlRootIdentity = 0;
        std::uintptr_t editIdentity = 0;
        std::uint64_t editGeneration = 0;
        unsigned phase = 0;
        int visualRootChildCount = 0;
        int visualChildCount = 0;
        int windowWidth = 0;
        int windowHeight = 0;
        int bridgeWidth = 0;
        int bridgeHeight = 0;
        bool windowVisible = false;
        bool windowShownOnScreen = false;
        bool bridgeVisible = false;
        double actualWidth = 0.0;
        double actualHeight = 0.0;
        double desiredWidth = 0.0;
        double desiredHeight = 0.0;
        long peerSelectionStart = -1;
        long peerSelectionLength = -1;
        bool logicalFocus = false;
        bool nativeFocusInHost = false;
        bool editFocused = false;
    };
    enum WinUITemplatePeerState
    {
        WinUITemplate_HasCombo         = 0x001,
        WinUITemplate_IsEditable       = 0x002,
        WinUITemplate_HasXamlRoot      = 0x004,
        WinUITemplate_HasTemplate      = 0x008,
        WinUITemplate_HasVisualChild   = 0x010,
        WinUITemplate_IsArranged       = 0x020,
        WinUITemplate_HasDesiredSize   = 0x040,
        WinUITemplate_HasEdit          = 0x080,
        WinUITemplate_EditSharesRoot   = 0x100,
        WinUITemplate_HasVisualRoot    = 0x200,
        WinUITemplate_RootHasXamlRoot  = 0x400,
        WinUITemplate_RootOwnsCombo    = 0x800,
        WinUITemplate_Complete         = 0xfff
    };
    enum WinUITemplatePeerPhase
    {
        WinUITemplatePhase_Resolving       = 0x01,
        WinUITemplatePhase_PendingResolve  = 0x02,
        WinUITemplatePhase_Transition      = 0x04,
        WinUITemplatePhase_PeerMutation    = 0x08,
        WinUITemplatePhase_PendingText     = 0x10,
        WinUITemplatePhase_PendingRange    = 0x20,
        WinUITemplatePhase_QueuedResolve   = 0x40
    };
    static bool GetTemplatePeerSnapshot(
        const wxComboBox* control,
        WinUITemplatePeerSnapshot *snapshot);
    enum WinUIPeerActualTheme
    {
        WinUITheme_Unknown = -1,
        WinUITheme_Default,
        WinUITheme_Light,
        WinUITheme_Dark
    };
    static bool GetTextEntryPeerState(
        wxComboBox* control,
        unsigned long *maxLength,
        bool *forceUpper,
        wxPoint *margins,
        std::uintptr_t *editIdentity = nullptr,
        std::uint64_t *editGeneration = nullptr,
        int *editableRootActualTheme = nullptr,
        int *editableRootRequestedTheme = nullptr,
        int *editActualTheme = nullptr,
        bool realize = true);
    static unsigned GetAutoCompleteSuggestionCount(const wxComboBox* control);
    static wxString GetAutoCompleteSuggestion(
        const wxComboBox* control,
        unsigned n);
    static int GetAutoCompleteActiveSuggestion(const wxComboBox* control);
    static bool InvokeAutoCompleteSuggestion(wxComboBox* control, unsigned n);
    static bool InvokeClipboardCommand(wxComboBox* control, wxEventType type);
    // Passive branch telemetry used by the WinUI parity tests. None of these
    // fields resolves, focuses or mutates a peer.
    enum WinUIAutomationTestStage
    {
        WinUIAutomation_Scheduled = 15,
        WinUIAutomation_Succeeded = 24
    };
    struct WinUIDiagnosticSnapshot
    {
        std::uint64_t selectionDispatches = 0;
        std::uint64_t selectionSnapshotDeliveries = 0;
        std::uint64_t peerTextCallbacks = 0;
        std::uint64_t peerTextDeliveries = 0;
        wxString selectionBeforeHandler;
        wxString selectionAfterHandler;
        wxString lastPeerText;
        bool selectionTextAllowed = false;
        bool selectionOwnerSurvived = false;

        std::uint64_t keyPreviewCallbacks = 0;
        std::uint64_t keyPreviewHandled = 0;
        unsigned lastPreviewKey = 0;
        std::uintptr_t keyTargetIdentity = 0;
        std::uintptr_t keySenderIdentity = 0;
        bool lastPreviewHandledOnEntry = false;

        int automationStage = 0;
        long automationHResult = 0;

        std::uint64_t templateTransitions = 0;
        std::uint64_t templateReplayRequests = 0;
        std::uint64_t templateReplayAttempts = 0;
        std::uint64_t templateReplaySuccesses = 0;
        std::uint64_t comboLayoutEdges = 0;
        std::uint64_t comboLayoutResolveRequests = 0;
        std::uint64_t comboLayoutResolveRuns = 0;
        std::uint64_t comboLayoutCoalescedEdges = 0;
        std::uint64_t comboLayoutRealizations = 0;
        std::uint64_t comboLayoutSynchronousRealizations = 0;
        std::uint64_t comboLayoutUnlatchedRealizations = 0;
        std::uint64_t editLayoutEdges = 0;
        std::uint64_t rangeProjectionMismatches = 0;
        std::uint64_t rangeFinalizations = 0;
        bool templateReplayPending = false;
        bool pendingText = false;
        bool pendingRange = false;
        std::uint64_t pendingRangeEpoch = 0;
        std::uint64_t pendingRangeGeneration = 0;
        std::uint64_t pendingRangeRevision = 0;
        long pendingRangeStart = 0;
        long pendingRangeEnd = 0;

        std::uint64_t simpleRealizeAttempts = 0;
        int simpleRequestedWidth = 0;
        int simpleRequestedHeight = 0;
        int simpleSourceWidthPixels = 0;
        int simpleSourceHeightPixels = 0;
        double simpleRasterizationScale = 0.0;
        double simpleComputedWidth = 0.0;
        double simpleComputedHeight = 0.0;
        std::uint64_t simpleLayoutEpoch = 0;
        std::uint64_t simpleLayoutAppliedEpoch = 0;
        double simpleArrangedHeight = 0.0;
        double simpleListHeight = 0.0;
        double simpleViewportHeight = 0.0;
    };
    static bool GetDiagnosticSnapshot(
        const wxComboBox* control,
        WinUIDiagnosticSnapshot *snapshot);
    struct WinUISimplePeerSnapshot
    {
        unsigned state = 0;
        unsigned phase = 0;
        std::uintptr_t editIdentity = 0;
        std::uintptr_t listIdentity = 0;
        std::uintptr_t xamlRootIdentity = 0;
        double rootHeight = 0.0;
        int editRow = -1;
        int listRow = -1;
        bool listRowIsPixel = false;
        double listRowHeight = 0.0;
        double editX = 0.0;
        double editY = 0.0;
        double editWidth = 0.0;
        double editHeight = 0.0;
        double listX = 0.0;
        double listY = 0.0;
        double listWidth = 0.0;
        double listHeight = 0.0;
        double listDesiredHeight = 0.0;
        double listLocalHeight = 0.0;
        double listMinHeight = 0.0;
        double listMaxHeight = 0.0;
        double viewportHeight = 0.0;
        int sourceWidthPixels = 0;
        int sourceHeightPixels = 0;
        double requestedWidth = 0.0;
        double requestedHeight = 0.0;
        std::uint64_t layoutEpoch = 0;
        std::uint64_t layoutAppliedEpoch = 0;
        bool layoutQueued = false;
        bool layoutInProgress = false;
        long peerTextSelectionStart = 0;
        long peerTextSelectionLength = 0;
        double rasterizationScale = 0.0;
        int actualTheme = -1;
        int peerSelection = wxNOT_FOUND;
        bool peerItemsValid = false;
        bool logicalFocus = false;
        bool nativeFocusInHost = false;
        bool editFocused = false;
    };
    enum WinUISimplePeerState
    {
        WinUISimple_HasRoot       = 0x001,
        WinUISimple_HasEdit       = 0x002,
        WinUISimple_HasList       = 0x004,
        WinUISimple_ListVisible   = 0x008,
        WinUISimple_HasTwoRows    = 0x010,
        WinUISimple_ChildOrder    = 0x020,
        WinUISimple_NoComboPeer   = 0x040,
        WinUISimple_ItemsMatch    = 0x080,
        WinUISimple_HostsRoot     = 0x100,
        WinUISimple_ListArranged  = 0x200,
        WinUISimple_RowsAssigned  = 0x400,
        WinUISimple_RectsValid    = 0x800,
        WinUISimple_ListBelowEdit = 0x1000,
        WinUISimple_WidthsAligned = 0x2000,
        WinUISimple_HasViewport   = 0x4000,
        WinUISimple_HasXamlRoot   = 0x8000,
        WinUISimple_ScaleValid    = 0x10000,
        WinUISimple_ThemeObserved = 0x20000,
        WinUISimple_Complete      = 0x3ffff
    };
    static bool GetSimplePeerSnapshot(
        const wxComboBox* control,
        WinUISimplePeerSnapshot *snapshot);
    static unsigned GetSimplePeerState(
        const wxComboBox* control,
        std::uintptr_t *editIdentity = nullptr,
        std::uintptr_t *listIdentity = nullptr);
    static bool NavigateSimpleList(wxComboBox* control, int delta);
    static int GetSimplePageSize(const wxComboBox* control);

    // Final-action hook in the ordinary native range callback.
    static void RunIndependentRangeObservationHook(wxComboBox* control);
};
#endif

#endif
