/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/private/richtooltip.h
// Purpose:     WinUI rich-tooltip presentation contract
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

// Synchronous reparent notification from the shared TLW host. The
// implementation keeps only weak state/window references and closes sessions
// before their old XAML root can be migrated or destroyed.
WXDLLIMPEXP_CORE void
wxWinUINotifyTeachingTipReparent(wxWindow *subtreeRoot);

#endif // wxUSE_WINUI3 && wxUSE_RICHTOOLTIP

#endif // _WX_WINUI_PRIVATE_RICHTOOLTIP_H_
