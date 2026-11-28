/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/private/transient.h
// Purpose:     Test seam for WinUI transient-owner and backdrop transactions
// Author:      wxWidgets development team
// Created:     2026-07-24
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PRIVATE_TRANSIENT_H_
#define _WX_WINUI_PRIVATE_TRANSIENT_H_

#include "wx/defs.h"

#if wxUSE_WINUI3

class wxWindow;
#if wxUSE_POPUPWIN
class wxPopupWindow;
class wxPopupTransientWindow;

// Private cross-translation-unit hooks. Popup lifetime/session state lives in
// a cpp sidecar so the public exported classes keep their historical layout
// and DismissAndNotify() signature.
WXDLLIMPEXP_CORE bool
wxWinUIPopupPrepareForDestroy(wxPopupWindow *popup);

// Converge a popup to a terminal logical/native state using the same bounded
// capture-release transaction as owner cancellation. The function may destroy
// (or schedule destruction of) the popup when callbacks keep capture or native
// visibility alive. When no exact HWND identity exists, ownership is still
// transferred to the type-independent delayed popup-destroy pipeline. Using
// one delayed fallback is required for applications deriving directly from
// wxPopupTransientWindowBase, which has no distinct wx class-info. A false
// result therefore means that an already-active terminal transaction remains
// authoritative, or that neither retirement nor destroy scheduling committed.
WXDLLIMPEXP_CORE bool
wxWinUIRetirePopupTerminal(wxPopupWindow *popup);

// Return true while the central transient manager is terminally retiring the
// popup containing this window. wxWindowBase::CaptureMouse() consults this
// before mutating its capture stack so a hostile capture-lost callback cannot
// resurrect capture after the manager has already withdrawn its registration.
WXDLLIMPEXP_CORE bool
wxWinUIPopupBlocksCapture(wxWindow *window);

// Release the current capture through the wx stack when it owns the capture,
// or directly through USER32 for a native SetCapture() which bypassed that
// stack. The return value is a verified postcondition, not the API result.
WXDLLIMPEXP_CORE bool
wxWinUIPopupReleaseCapture(wxWindow *window);


#endif


#endif // wxUSE_WINUI3

#endif // _WX_WINUI_PRIVATE_TRANSIENT_H_
