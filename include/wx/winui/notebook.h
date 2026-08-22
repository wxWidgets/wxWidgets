/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/notebook.h
// Purpose:     wxWinUI wxNotebook declaration (WinUI TabView)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_NOTEBOOK_H_
#define _WX_WINUI_NOTEBOOK_H_

#include "wx/arrstr.h"

#include <cstdint>
#include <memory>

class wxWinUINotebookImpl;
class wxWinUINotebookCallbackState;
class wxDPIChangedEvent;

class WXDLLIMPEXP_CORE wxNotebook : public wxNotebookBase
{
public:
    enum class WinUIPeerMutationForTesting
    {
        InsertPage,
        RemovePage,
        ClearPages
    };

    enum class WinUIProjectionPointForTesting
    {
        LabelText,
        AutomationName,
        IconSource,
        TabPadding,
        TabSize,
        TabGeometryMeasure
    };

    enum class WinUIPeerRetirementQueueFaultForTesting
    {
        None,
        RejectFirstTurn,
        RejectSecondTurn
    };

    enum class WinUIFrameworkRetirementHookFaultForTesting : unsigned
    {
        None = 0,
        RejectFrameworkHook = 1,
        RejectShutdownHook = 2,
        RejectBothHooks = 3
    };

    struct WinUIFrameworkRetirementSnapshotForTesting
    {
        size_t entries = 0;
        size_t unboundEntries = 0;
        size_t queueStates = 0;
        size_t shutdownStartingHooks = 0;
        size_t frameworkStartingHooks = 0;
        size_t frameworkHooks = 0;
        size_t shutdownHooks = 0;
        size_t activeStates = 0;
        size_t shutdownStartingStates = 0;
        size_t frameworkStartingStates = 0;
        size_t xamlCompletedStates = 0;
        size_t frameworkDoneStates = 0;
        size_t shutdownDoneStates = 0;
        bool xamlShutdownHookInstalled = false;
        bool rundown = false;
        bool xamlTerminal = false;
        bool xamlCompletionActive = false;
        bool runtimeTerminal = false;
        bool phaseOrderValid = true;
    };

    struct WinUIExtendedLabelMetricSnapshotForTesting
    {
        wxSize naturalSize;
        wxRect publishedRect;
        wxSize actualSize;
        wxRect liveActualRect;
        std::uintptr_t labelIdentity = 0;
        std::uint64_t probeEpoch = 0;
        std::uint64_t layoutEpoch = 0;
        std::uint64_t surfaceGeneration = 0;
        std::uint64_t modelRevision = 0;
        std::uint64_t layoutRevision = 0;
        std::uint64_t styleRevision = 0;
        std::uint64_t publishedModelRevision = 0;
        std::uint64_t publishedLayoutRevision = 0;
        std::uint64_t continuationCount = 0;
        bool pending = true;
    };

    using WinUIProjectionHookForTesting =
        void (*)(wxNotebook *, void *);
    using WinUIFrameworkRetirementPhaseHookForTesting = void (*)(void *);

    wxNotebook();
    wxNotebook(wxWindow *parent,
               wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxASCII_STR(wxNotebookNameStr));
    ~wxNotebook() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxASCII_STR(wxNotebookNameStr));

    // wxBookCtrlBase overrides
    int SetSelection(size_t nPage) override;
    int ChangeSelection(size_t nPage) override;

    bool SetPageText(size_t nPage, const wxString& strText) override;
    wxString GetPageText(size_t nPage) const override;

    int GetPageImage(size_t nPage) const override;
    bool SetPageImage(size_t nPage, int nImage) override;

    bool InsertPage(size_t nPage,
                    wxNotebookPage *pPage,
                    const wxString& strText,
                    bool bSelect = false,
                    int imageId = NO_IMAGE) override;

    bool DeleteAllPages() override;

    // wxNotebookBase overrides
    int GetRowCount() const override;
    void SetPadding(const wxSize& padding) override;
    void SetTabSize(const wxSize& sz) override;
    wxSize CalcSizeFromPage(const wxSize& sizePage) const override;
    wxRect GetTabRect(size_t page) const override;
    int HitTest(const wxPoint& pt, long *flags = nullptr) const override;

    // wxWindow override: notebook alignment and presentation styles are
    // live on MSW and must remain live when backed by the shared WinUI tree.
    void SetWindowStyleFlag(long style) override;

    // Implementation-only deterministic seams. They exercise the real
    // lifetime and transaction boundaries without synthesizing input.
    void WinUIFailNextPeerMutationForTesting(
        WinUIPeerMutationForTesting mutation);
    size_t WinUIGetPeerPageCountForTesting() const;
    wxString WinUIGetPeerPageTextForTesting(size_t page) const;
    wxString WinUIGetPeerAutomationNameForTesting(size_t page) const;
    bool WinUIQueueSelectionCallbackForTesting(size_t page);
    void WinUIClosePeerForTesting();
    static size_t WinUIGetLiveCallbackStateCountForTesting();
    static size_t WinUIGetPendingPeerRetirementCountForTesting();
    static size_t WinUIGetFrameworkRetirementCountForTesting();
    static void WinUISetPeerRetirementQueueFaultForTesting(
        WinUIPeerRetirementQueueFaultForTesting fault);
    static WinUIPeerRetirementQueueFaultForTesting
        WinUIGetPeerRetirementQueueFaultForTesting();
    static void WinUISetFrameworkRetirementHookFaultForTesting(
        WinUIFrameworkRetirementHookFaultForTesting fault);
    static WinUIFrameworkRetirementHookFaultForTesting
        WinUIGetFrameworkRetirementHookFaultForTesting();
    static WinUIFrameworkRetirementSnapshotForTesting
        WinUIGetFrameworkRetirementSnapshotForTesting();
    static std::uint64_t WinUIRegisterUnboundFrameworkRetirementForTesting(
        WinUIFrameworkRetirementPhaseHookForTesting hook,
        void *context);
    static void WinUICompleteFrameworkRetirementForTesting(
        std::uint64_t id);
    static void WinUISimulateShutdownStartingForTesting(
        WinUIFrameworkRetirementPhaseHookForTesting hook = nullptr,
        void *context = nullptr);
    static void WinUISimulateFrameworkShutdownStartingForTesting(
        WinUIFrameworkRetirementPhaseHookForTesting hook = nullptr,
        void *context = nullptr);
    static void WinUISimulateFrameworkShutdownCompletedForTesting(
        WinUIFrameworkRetirementPhaseHookForTesting hook = nullptr,
        void *context = nullptr);
    static void WinUISimulateShutdownCompletedForTesting(
        WinUIFrameworkRetirementPhaseHookForTesting hook = nullptr,
        void *context = nullptr);
    static void WinUISimulateXamlShutdownCompletedForTesting(
        WinUIFrameworkRetirementPhaseHookForTesting hook = nullptr,
        void *context = nullptr);
    static void WinUIResetFrameworkRetirementRuntimeForTesting();
    wxRect WinUIGetTabIconRectForTesting(size_t page) const;
    wxRect WinUIGetTabLabelRectForTesting(size_t page) const;
    wxRect WinUIGetActualTabRectForTesting(size_t page) const;
    wxRect WinUIGetActualTabIconRectForTesting(size_t page) const;
    wxRect WinUIGetActualTabLabelRectForTesting(size_t page) const;
    bool WinUIHasPendingExtendedLabelMetricsForTesting() const;
    std::uint64_t WinUIGetExtendedLayoutContinuationCountForTesting() const;
    WinUIExtendedLabelMetricSnapshotForTesting
        WinUIGetExtendedLabelMetricSnapshotForTesting(size_t page) const;
    wxSize WinUIGetPeerTabPaddingForTesting(size_t page) const;
    wxSize WinUIGetPeerTabSizeForTesting(size_t page) const;
    wxSize WinUIGetPeerIconPixelSizeForTesting(size_t page) const;
    wxSize WinUIGetPeerIconDIPSizeForTesting(size_t page) const;
    std::uint64_t WinUIGetPeerIconGenerationForTesting(size_t page) const;
    bool WinUIIsPeerRTLForTesting() const;
    bool WinUIIsUsingExtendedSurfaceForTesting() const;
    bool WinUIIsPeerTabStopForTesting(size_t page) const;
    bool WinUIInvokeTabOverflowForTesting(bool forward);
    bool WinUIRefreshForScaleForTesting(double scale);
    void WinUISetNextProjectionHookForTesting(
        WinUIProjectionPointForTesting point,
        WinUIProjectionHookForTesting hook,
        void *context);

protected:
    bool MSWOnEffectiveLayoutDirectionChanged() override;
    void Init();

    wxWindow *DoRemovePage(size_t nPage) override;

    void DoSize() override;
    wxRect GetPageRect() const override;

    void UpdateSelectedPage(size_t newsel) override;
    wxBookCtrlEvent* CreatePageChangingEvent() const override;
    void MakeChangedEvent(wxBookCtrlEvent& event) override;
    void OnImagesChanged() override;

    // Capture the tab-strip height from the WinUI TabView layout (once it is
    // known) so the page area can be derived deterministically from the client
    // size; called when the TabView completes a layout pass.
    void UpdateTabStripHeightFromLayout();

    // Notify of a user-driven tab change coming from the WinUI TabView.
    void OnTabViewSelectionChanged(int sel);

    // Snapshot focus ownership before wxBookCtrlBase hides the old page.
    int DoSetSelectionPreservingFocus(size_t nPage, int flags);
    bool EnsureSelectionVisible(size_t selection);

    std::unique_ptr<wxWinUINotebookImpl> m_winui;
    wxArrayString m_pageTexts;
    wxArrayInt m_pageImages;

    // Height (in DIPs) of the WinUI TabView tab strip, measured from its layout.
    // -1 until the first layout pass provides a usable value.
    int m_tabStripHeightDIP = -1;

private:
    bool UsesExtendedPeerSurface() const;
    bool EnsureExtendedPeerSurface();
    bool InstallPeerSurfaceForCurrentStyle();
    bool LayoutExtendedPeerSurface();
    void QueueExtendedPeerLayoutContinuation();
    wxRect GetExtendedPageRect() const;
    void UpdateExtendedPeerSelection(size_t selection);

    // Project the complete logical header model as one last-writer-wins
    // transaction. XAML dependency-property setters can synchronously run
    // LayoutUpdated/application code, so every pass is revision-checked and
    // stale passes are retried instead of publishing their tail.
    bool SyncPeerProjection();
    bool ProjectPeerModelPass(
        std::uint64_t revision,
        const std::shared_ptr<wxWinUINotebookCallbackState>& state,
        std::uint64_t callbackGeneration,
        wxWinUINotebookImpl *impl);
    bool IsPeerProjectionCurrent(
        std::uint64_t revision,
        const std::shared_ptr<wxWinUINotebookCallbackState>& state,
        std::uint64_t callbackGeneration,
        wxWinUINotebookImpl *impl) const;
    void InvokeProjectionHookForTesting(
        WinUIProjectionPointForTesting point);
    wxNotebook *RealizeTabGeometryForQuery() const;
    wxRect GetCachedTabRect(size_t page,
                            int selection,
                            wxLayoutDirection direction,
                            double scale) const;
    wxRect GetCachedTabPartRect(size_t page,
                                int part,
                                int selection,
                                wxLayoutDirection direction,
                                double scale) const;
    wxRect GetRealizedTabElementRect(size_t page,
                                     int part,
                                     bool allowCachedFallback) const;
    void OnDPIChanged(wxDPIChangedEvent& event);

    // These values use wx client pixels, as documented by wxNotebook. They
    // are converted exactly once to XAML DIPs whenever they are applied.
    wxSize m_tabPadding;
    wxSize m_tabSize;
    bool m_hasTabPadding = false;
    bool m_hasTabSize = false;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxNotebook);
};

#endif // _WX_WINUI_NOTEBOOK_H_
