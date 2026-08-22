/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/combobox.h
// Purpose:     wxWinUI wxComboBox declaration
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_COMBOBOX_H_
#define _WX_WINUI_COMBOBOX_H_

#include "wx/choice.h"
#include "wx/textentry.h"

#include <memory>

class WXDLLIMPEXP_CORE wxComboBox : public wxChoice,
                                    public wxTextEntry
{
public:
    wxComboBox();
    wxComboBox(wxWindow *parent,
               wxWindowID id,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int n = 0,
               const wxString choices[] = nullptr,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxASCII_STR(wxComboBoxNameStr));
    wxComboBox(wxWindow *parent,
               wxWindowID id,
               const wxString& value,
               const wxPoint& pos,
               const wxSize& size,
               const wxArrayString& choices,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxASCII_STR(wxComboBoxNameStr));
    ~wxComboBox() override;

    void SetFocus() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0,
                const wxString choices[] = nullptr,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxComboBoxNameStr));
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxComboBoxNameStr));

    bool IsListEmpty() const { return wxItemContainer::IsEmpty(); }
    bool IsTextEmpty() const { return wxTextEntry::IsEmpty(); }

    void Clear() override;
    wxString GetValue() const override;
    void SetValue(const wxString& value) override;
    void SetString(unsigned int n, const wxString& value) override;
    wxString GetStringSelection() const override
        { return wxChoice::GetStringSelection(); }
    void Popup();
    void Dismiss();
    void SetSelection(int n) override;
    void SetSelection(long from, long to) override;
    int GetSelection() const override { return wxChoice::GetSelection(); }
    void GetSelection(long *from, long *to) const override;

    bool IsEditable() const override;
    void SetEditable(bool editable) override;
    void SetMaxLength(unsigned long len) override;
    void ForceUpper() override;
    bool SetHint(const wxString& hint) override;
    wxString GetHint() const override;
    void WriteText(const wxString& text) override;
    void Remove(long from, long to) override;
    void Copy() override;
    void Cut() override;
    void Paste() override;
    void Undo() override;
    void Redo() override;
    bool CanUndo() const override;
    bool CanRedo() const override;
    void SetInsertionPoint(long pos) override;
    long GetInsertionPoint() const override;
    long GetLastPosition() const override;

    void Command(wxCommandEvent& event) override;

    const wxTextEntry* WXGetTextEntry() const override
        { return m_snapshotTextDispatchDepth ? nullptr : this; }

    // Implementation-only deterministic seams. They mutate the real WinUI
    // dependency properties/delegates without synthesizing keyboard or mouse
    // input and are intentionally not part of wxComboBoxBase.
    bool WinUISetPeerTextForTesting(const wxString& text);
    bool WinUISetPeerTextViaAutomationForTesting(const wxString& text);
    bool WinUISelectPeerItemForTesting(int selection);
    bool WinUIRetemplateForTesting();
    bool WinUIRunTemplateLayoutEdgeForTesting(bool forceTransition);
    bool WinUIQueueTemplateLayoutResolutionForTesting();
    bool WinUISetEditSelectionForTesting(long from,
                                         long to,
                                         bool retiredPart = false);
    // Write the current TextBox range without pre-classifying the write as a
    // wx/native-authority boundary. This exercises the production
    // SelectionChanged projection classifier.
    bool WinUISetRawEditSelectionForTesting(long from, long to);
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
    bool WinUIChainPendingRangeCallbacksForTesting(
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
    bool WinUISetRawEditSelectionAndDestroyForTesting(
        long from,
        long to,
        const std::shared_ptr<WinUIRangeDestructionSnapshot>& snapshot);
    unsigned WinUIGetTemplateStateForTesting() const;
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
    bool WinUIGetTemplatePeerSnapshotForTesting(
        WinUITemplatePeerSnapshot *snapshot) const;
    enum WinUIPeerActualTheme
    {
        WinUITheme_Unknown = -1,
        WinUITheme_Default,
        WinUITheme_Light,
        WinUITheme_Dark
    };
    bool WinUIGetTextEntryPeerStateForTesting(
        unsigned long *maxLength,
        bool *forceUpper,
        wxPoint *margins,
        std::uintptr_t *editIdentity = nullptr,
        std::uint64_t *editGeneration = nullptr,
        int *editableRootActualTheme = nullptr,
        int *editableRootRequestedTheme = nullptr,
        int *editActualTheme = nullptr,
        bool realize = true);
    unsigned WinUIGetAutoCompleteSuggestionCountForTesting() const;
    wxString WinUIGetAutoCompleteSuggestionForTesting(unsigned n) const;
    int WinUIGetAutoCompleteActiveSuggestionForTesting() const;
    bool WinUIInvokeAutoCompleteSuggestionForTesting(unsigned n);
    bool WinUIInvokeClipboardCommandForTesting(wxEventType type);
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
    bool WinUIGetDiagnosticSnapshotForTesting(
        WinUIDiagnosticSnapshot *snapshot) const;
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
    bool WinUIGetSimplePeerSnapshotForTesting(
        WinUISimplePeerSnapshot *snapshot) const;
    unsigned WinUIGetSimplePeerStateForTesting(
        std::uintptr_t *editIdentity = nullptr,
        std::uintptr_t *listIdentity = nullptr) const;
    bool WinUINavigateSimpleListForTesting(int delta);
    int WinUIGetSimplePageSizeForTesting() const;

protected:
    wxSize DoGetBestSize() const override;
    void DoSetSize(int x, int y, int width, int height,
                   int sizeFlags = wxSIZE_AUTO) override;
    bool WinUIWantsEditablePeerDuringCreate() const override
        { return m_editable; }
    wxString DoGetValue() const override;
    wxWindow *GetEditableWindow() override { return this; }
    void DoSetValue(const wxString& value, int flags = 0) override;
    void EnableTextChangedEvents(bool enable) override;
    void SendSelectionEvent() override;
    bool MSWShouldPreProcessMessage(WXMSG* msg) override;
    bool DoSetMargins(const wxPoint& margins) override;
    wxPoint DoGetMargins() const override;
    bool DoAutoCompleteStrings(const wxArrayString& choices) override;
    bool DoAutoCompleteFileNames(int flags) override;
    bool DoAutoCompleteCustom(wxTextCompleter *completer) override;

private:
    void CloseTextPeer();
    void ArmPendingTextValue(const wxString& value,
                             std::uint64_t selectedItemId);
    void ClearPendingTextValue();
    void ReconcilePendingTextValueAtPeerEdge();
    void ClearPendingTextSelection();
    bool CreateSimplePeer();
    void QueueSimplePeerLayoutAtPeerEdge();
    void SendTextEvent(int item = wxNOT_FOUND);
    void SendTextEvent(int item, const wxString& value);
    void SendMaxLengthEvent();
    bool SendClipboardEvent(wxEventType type);
    void ClampTextState();
    void ApplyTextToPeer();
    bool ApplyTextSelectionToPeer();
    void OnPeerTextChanged();
    bool SuppressPendingTextSelectionChange();
    bool HandlePendingTextSelectionChange();
    void ReplayPendingTextSelection();
    void RunIndependentRangeObservationHookForTesting();
    void OnPeerDropDownChanged(bool open);
    bool ReadTextSelectionFromPeer();
    bool EnsureThemeTransitionBoundary();
    bool RetireEditPartForTemplateTransition();
    bool ReplayTemplateStateToEditPart();
    void OnPeerTemplateTransition(bool forceEditableReload);
    void OnPeerLayoutUpdated(bool forceTransitionForTesting = false);
    void QueueEditPartResolutionAtLayoutEdge(
        bool forceTransitionForTesting = false);
    // Template application/layout can invoke arbitrary application code.
    // Return false if that code destroyed this control.
    bool ResolveEditPart(bool updateLayout = true);
    void ResolveEditPartOnce(bool updateLayout);
    bool RealizeSimplePeerLayout();
    bool AttachEditPeerHandlers();
    bool NavigateSimpleList(int delta);
    int GetSimpleListPageSize() const;
    bool DismissAutoComplete();
    void RefreshAutoComplete();
    void NavigateAutoComplete(int delta);
    void AcceptAutoCompleteSelection();
    void AcceptAutoCompleteSuggestion(const wxString& suggestion,
                                      std::uint64_t generation);
    WXHWND GetEditHWND() const override { return GetHWND(); }

    wxString m_value;
    long m_insertionPoint = 0;
    long m_selectionStart = 0;
    long m_selectionEnd = 0;
    bool m_editable = true;
    bool m_allowTextEvents = true;
    // wxCommandEvent::GetString() normally re-reads wxEVT_TEXT payloads from
    // WXGetTextEntry(). Snapshot notifications deliberately describe the
    // accepted selection from before arbitrary COMBOBOX-handler re-entry.
    unsigned m_snapshotTextDispatchDepth = 0;
    unsigned long m_maxLength = 0;
    wxCoord m_horizontalMargin = -1;
    bool m_forceUpper = false;
    wxString m_hint;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxComboBox);
};

#endif // _WX_WINUI_COMBOBOX_H_
