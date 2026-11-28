/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/keyboard.h
// Purpose:     Intracore WinUI keyboard pipeline contracts (not installed)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#ifndef WX_WINUI_KEYBOARD_H
#define WX_WINUI_KEYBOARD_H

#include "wx/defs.h"

#if wxUSE_WINUI3

class WXDLLIMPEXP_FWD_CORE wxWindow;

// Snapshot the modifiers used to distinguish a real Ctrl+Alt accelerator from
// AltGr (Windows exposes AltGr as Ctrl+right-Alt).
struct wxWinUIKeyboardModifiers
{
    bool shiftDown = false;
    bool controlDown = false;
    bool leftAltDown = false;
    bool rightAltDown = false;
};

enum class wxWinUIKeyboardPipelineResult
{
    NotApplicable,
    NotHandled,
    WxHandled,
    XamlHandled
};

using wxWinUIXamlPreTranslateCallback = bool (*)(WXMSG *, void *);

// Production event-loop/host helpers. These retain their existing contracts.
WXDLLIMPEXP_CORE bool wxWinUI3PreProcessMessage(WXMSG *msg);
WXDLLIMPEXP_CORE bool wxWinUI3AllowMessageProcessing(wxWindow *logicalOwner);
WXDLLIMPEXP_CORE bool wxWinUI3ConsumeBlockedKeyboardMessage(WXMSG *msg);

// Share an explicitly supplied pipeline snapshot with local text filters.
// Shipping has no override state: false leaves each caller's native modifier
// read and its existing Alt/context-bit policy unchanged. This is intracore,
// not an exported application/test entry point.
bool wxWinUIGetKeyboardModifiersOverride(wxWinUIKeyboardModifiers *modifiers);

#endif // wxUSE_WINUI3
#endif // WX_WINUI_KEYBOARD_H
