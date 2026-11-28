/////////////////////////////////////////////////////////////////////////////
// Private, non-installed access to wxNotebook test operations.
// Available only in an explicitly test-enabled WinUI library build.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_NOTEBOOK_TEST_ACCESS_H
#define WX_WINUI_NOTEBOOK_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/notebook.h"

#if wxUSE_NOTEBOOK

class WXDLLIMPEXP_CORE wxWinUINotebookTestAccess final
{
public:
    enum class PeerMutation
    {
        InsertPage,
        RemovePage,
        ClearPages
    };

    enum class ProjectionPoint
    {
        LabelText,
        AutomationName,
        IconSource,
        TabPadding,
        TabSize,
        TabGeometryMeasure
    };

    enum class PeerRetirementQueueFault
    {
        None,
        RejectFirstTurn,
        RejectSecondTurn
    };

    enum class FrameworkRetirementHookFault : unsigned
    {
        None = 0,
        RejectFrameworkHook = 1,
        RejectShutdownHook = 2,
        RejectBothHooks = 3
    };

    struct FrameworkRetirementSnapshot
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

    struct ExtendedLabelMetricSnapshot
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

    using ProjectionHook =
        void (*)(wxNotebook *, void *);
    using FrameworkRetirementPhaseHook = void (*)(void *);

    static void FailNextPeerMutation(wxNotebook& control, PeerMutation mutation);
    static size_t GetPeerPageCount(const wxNotebook& control);
    static wxString GetPeerPageText(const wxNotebook& control, size_t page);
    static wxString GetPeerAutomationName(const wxNotebook& control, size_t page);
    static bool QueueSelectionCallback(wxNotebook& control, size_t page);
    static void ClosePeer(wxNotebook& control);
    static size_t GetLiveCallbackStateCount();
    static size_t GetPendingPeerRetirementCount();
    static size_t GetFrameworkRetirementCount();
    static void SetPeerRetirementQueueFault(PeerRetirementQueueFault fault);
    static PeerRetirementQueueFault GetPeerRetirementQueueFault();
    static void SetFrameworkRetirementHookFault(FrameworkRetirementHookFault fault);
    static FrameworkRetirementHookFault GetFrameworkRetirementHookFault();
    static FrameworkRetirementSnapshot GetFrameworkRetirementSnapshot();
    static std::uint64_t RegisterUnboundFrameworkRetirement(
        FrameworkRetirementPhaseHook hook,
        void *context);
    static void CompleteFrameworkRetirement(std::uint64_t id);
    static void SimulateShutdownStarting(
        FrameworkRetirementPhaseHook hook = nullptr,
        void *context = nullptr);
    static void SimulateFrameworkShutdownStarting(
        FrameworkRetirementPhaseHook hook = nullptr,
        void *context = nullptr);
    static void SimulateFrameworkShutdownCompleted(
        FrameworkRetirementPhaseHook hook = nullptr,
        void *context = nullptr);
    static void SimulateShutdownCompleted(
        FrameworkRetirementPhaseHook hook = nullptr,
        void *context = nullptr);
    static void SimulateXamlShutdownCompleted(
        FrameworkRetirementPhaseHook hook = nullptr,
        void *context = nullptr);
    static void ResetFrameworkRetirementRuntime();
    static wxRect GetTabIconRect(const wxNotebook& control, size_t page);
    static wxRect GetTabLabelRect(const wxNotebook& control, size_t page);
    static wxRect GetActualTabRect(const wxNotebook& control, size_t page);
    static wxRect GetActualTabIconRect(const wxNotebook& control, size_t page);
    static wxRect GetActualTabLabelRect(const wxNotebook& control, size_t page);
    static bool HasPendingExtendedLabelMetrics(const wxNotebook& control);
    static std::uint64_t GetExtendedLayoutContinuationCount(const wxNotebook& control);
    static ExtendedLabelMetricSnapshot GetExtendedLabelMetricSnapshot(
        const wxNotebook& control,
        size_t page);
    static wxSize GetPeerTabPadding(const wxNotebook& control, size_t page);
    static wxSize GetPeerTabSize(const wxNotebook& control, size_t page);
    static wxSize GetPeerIconPixelSize(const wxNotebook& control, size_t page);
    static wxSize GetPeerIconDIPSize(const wxNotebook& control, size_t page);
    static std::uint64_t GetPeerIconGeneration(const wxNotebook& control, size_t page);
    static bool IsPeerRTL(const wxNotebook& control);
    static bool IsUsingExtendedSurface(const wxNotebook& control);
    static bool IsPeerTabStop(const wxNotebook& control, size_t page);
    static bool InvokeTabOverflow(wxNotebook& control, bool forward);
    static bool RefreshForScale(wxNotebook& control, double scale);
    static void SetNextProjectionHook(
        wxNotebook& control,
        ProjectionPoint point,
        ProjectionHook hook,
        void *context);
};

#endif // wxUSE_NOTEBOOK

#endif // WX_WINUI_NOTEBOOK_TEST_ACCESS_H
