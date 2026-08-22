/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/private/richtooltip.h
// Purpose:     Testable WinUI rich-tooltip presentation contract
// Author:      wxWidgets development team
// Created:     2026-07-24
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PRIVATE_RICHTOOLTIP_H_
#define _WX_WINUI_PRIVATE_RICHTOOLTIP_H_

#include "wx/defs.h"
#include "wx/richtooltip.h"

#include <cstddef>

#if wxUSE_WINUI3 && wxUSE_RICHTOOLTIP

class wxWindow;

// Projection-free counterpart of TeachingTipPlacementMode. Keeping the
// public wxTipKind -> WinUI mapping here makes it possible to test every
// orientation without creating a XAML island.
enum class wxWinUITeachingTipPlacement
{
    Auto,
    Top,
    Bottom,
    TopRight,
    TopLeft,
    BottomRight,
    BottomLeft
};

struct wxWinUITeachingTipContract
{
    wxWinUITeachingTipPlacement placement =
        wxWinUITeachingTipPlacement::Auto;
    bool showTail = true;
};

inline wxWinUITeachingTipContract
wxWinUIResolveTeachingTipContract(wxTipKind kind)
{
    // wxTipKind names the side of the balloon carrying the tail, whereas
    // TeachingTipPlacementMode names the side of the target occupied by the
    // balloon. The vertical (and for corner tails, horizontal) direction is
    // therefore intentionally inverted.
    switch ( kind )
    {
        case wxTipKind_None:
            return { wxWinUITeachingTipPlacement::Auto, false };

        case wxTipKind_TopLeft:
            return { wxWinUITeachingTipPlacement::BottomRight, true };

        case wxTipKind_Top:
            return { wxWinUITeachingTipPlacement::Bottom, true };

        case wxTipKind_TopRight:
            return { wxWinUITeachingTipPlacement::BottomLeft, true };

        case wxTipKind_BottomLeft:
            return { wxWinUITeachingTipPlacement::TopRight, true };

        case wxTipKind_Bottom:
            return { wxWinUITeachingTipPlacement::Top, true };

        case wxTipKind_BottomRight:
            return { wxWinUITeachingTipPlacement::TopLeft, true };

        case wxTipKind_Auto:
            return { wxWinUITeachingTipPlacement::Auto, true };
    }

    return { wxWinUITeachingTipPlacement::Auto, true };
}

// Presentation failures must not all be treated alike. In particular, an
// operational failure after creating or attaching a TeachingTip must never
// silently switch to a second, generic implementation: this could leave two
// independently-owned transient visuals behind. Only a genuinely unavailable
// TeachingTip API permits that fallback.
enum class wxWinUITeachingTipShowResult
{
    Shown,
    Unsupported,
    Cancelled,
    OperationalFailure
};

inline bool
wxWinUITeachingTipAllowsGenericFallback(wxWinUITeachingTipShowResult result)
{
    return result == wxWinUITeachingTipShowResult::Unsupported;
}

// One-shot deterministic seams for lifetime and failure-path tests. They are
// private port infrastructure, not part of the wxRichToolTip API.
enum class wxWinUITeachingTipFaultForTesting
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

enum class wxWinUITeachingTipHookPointForTesting
{
    None,
    AfterAppend,
    AfterFlowDirection,
    AfterPeerOpen
};

using wxWinUITeachingTipHookForTesting = void (*)(wxWindow *);

struct wxWinUITeachingTipDiagnosticsForTesting
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
    // Monotonic observation order for asynchronous tests. Unlike wall-clock
    // sampling, these remain deterministic if one wxYield() drains both the
    // delayed-open and timeout ticks.
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

WXDLLIMPEXP_CORE void wxWinUISetTeachingTipFaultForTesting(
    wxWinUITeachingTipFaultForTesting fault);
// Exported only because shared-library tests cannot reach internal symbols.
// This private port test seam is not exposed by wxRichToolTip, has no
// environment-variable route, defaults off and is reset by the fixture.
// Production presentation therefore remains fail-closed on the bounded
// generic backend unless test code deliberately includes this private header.
WXDLLIMPEXP_CORE void
wxWinUISetNativeTeachingTipBackendForTesting(bool enable);
WXDLLIMPEXP_CORE void wxWinUISetTeachingTipHookForTesting(
    wxWinUITeachingTipHookPointForTesting point,
    wxWinUITeachingTipHookForTesting hook);
WXDLLIMPEXP_CORE void wxWinUIResetTeachingTipDiagnosticsForTesting();
WXDLLIMPEXP_CORE wxWinUITeachingTipDiagnosticsForTesting
wxWinUIGetTeachingTipDiagnosticsForTesting();
WXDLLIMPEXP_CORE std::size_t
wxWinUITriggerTeachingTipQueueShutdownForTesting(wxWindow *ownerOrTLW);

// Synchronous reparent notification from the shared TLW host. The
// implementation keeps only weak state/window references and closes sessions
// before their old XAML root can be migrated or destroyed.
WXDLLIMPEXP_CORE void
wxWinUINotifyTeachingTipReparent(wxWindow *subtreeRoot);

#endif // wxUSE_WINUI3 && wxUSE_RICHTOOLTIP

#endif // _WX_WINUI_PRIVATE_RICHTOOLTIP_H_
