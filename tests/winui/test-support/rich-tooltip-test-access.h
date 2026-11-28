/////////////////////////////////////////////////////////////////////////////
// Private, non-installed TeachingTip fault/hook/diagnostic adapters.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_RICH_TOOLTIP_TEST_ACCESS_H
#define WX_WINUI_RICH_TOOLTIP_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/winui/private/richtooltip.h"

#if wxUSE_WINUI3 && wxUSE_RICHTOOLTIP

class WXDLLIMPEXP_CORE wxWinUIRichToolTipTestAccess final
{
public:
    enum class Fault
    {
        None,
        Unsupported,
        OperationalAfterAppend,
        StartHResult,
        StartStdException,
        StartUnknownException,
        OpenAfterPeerHResult,
        QueueUnavailable
    };

    enum class HookPoint
    {
        None,
        AfterAppend,
        AfterFlowDirection,
        AfterPeerOpen
    };

    using Hook = void (*)(wxWindow *);

    struct Diagnostics
    {
        unsigned liveStates = 0;
        unsigned liveXamlHandlers = 0;
        unsigned liveTargetBindings = 0;
        unsigned liveVisuals = 0;
        unsigned liveGenericFallbacks = 0;
        unsigned nativeBackendAttempts = 0;
        unsigned genericFallbackStarts = 0;
        unsigned betaGenericFallbacks = 0;
        unsigned unsupportedFallbacks = 0;
        unsigned operationalFailures = 0;
        unsigned cancellations = 0;
        unsigned customizationFallbacks = 0;
        unsigned genericFallbackCancellations = 0;
        unsigned delayedStarts = 0;
        unsigned peerOpens = 0;
        unsigned peerCloseRequests = 0;
        unsigned timeoutClosures = 0;
        unsigned queueShutdownClosures = 0;
        unsigned reparentClosures = 0;
        unsigned cleanupTipRemovalAttempts = 0;
        unsigned cleanupAnchorRemovalAttempts = 0;
        unsigned long long lastPeerOpenSequence = 0;
        unsigned long long lastTimeoutCloseSequence = 0;
        unsigned lastRequestedTimeoutMs = 0;
        unsigned lastRequestedDelayMs = 0;
        bool lastHadTargetRect = false;
        bool lastRightToLeft = false;
        wxRect lastTargetRect;
        wxPoint lastAnchorInRoot;
        wxString lastAutomationName;
        wxString lastAutomationHelpText;
    };

    static void SetFault(Fault fault);
    static void SetNativeBackendEnabled(bool enable);
    static void SetHook(HookPoint point, Hook hook);
    static void ResetDiagnostics();
    static Diagnostics GetDiagnostics();
    static std::size_t TriggerQueueShutdown(wxWindow *ownerOrTLW);
};

#endif // wxUSE_WINUI3 && wxUSE_RICHTOOLTIP

#endif // WX_WINUI_RICH_TOOLTIP_TEST_ACCESS_H
