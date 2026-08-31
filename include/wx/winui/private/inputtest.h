/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/private/inputtest.h
// Purpose:     Internal WinUI keyboard pipeline and deterministic test seams
// Author:      wxWidgets development team
// Created:     2026-07-23
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PRIVATE_INPUTTEST_H_
#define _WX_WINUI_PRIVATE_INPUTTEST_H_

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

using wxWinUIXamlPreTranslateForTest = bool (*)(WXMSG *, void *);

// Run the exact semantic keyboard stages used in production, but with the
// logical island owner, modifier snapshot and final XAML stage supplied by the
// caller. This keeps layout-dependent AltGr/dead-key tests deterministic.
WXDLLIMPEXP_CORE wxWinUIKeyboardPipelineResult
wxWinUI3RunKeyboardPipelineForTesting(
    WXMSG *msg,
    wxWindow *logicalOwner,
    const wxWinUIKeyboardModifiers& modifiers,
    wxWinUIXamlPreTranslateForTest xamlPreTranslate,
    void *xamlContext);

// Complete pre-processing used by the event-loop fallback when the
// WH_GETMESSAGE hook is unavailable. It preserves the hook's
// navigation/accelerator-before-XAML ordering and feeds
// ContentPreTranslateMessage exactly once.
WXDLLIMPEXP_CORE bool wxWinUI3PreProcessMessage(WXMSG *msg);
WXDLLIMPEXP_CORE bool
wxWinUI3AllowMessageProcessing(wxWindow *logicalOwner);

// Consume the deferred wxEVT_CHAR_HOOK veto used while an IME is active.
// This must run before either wx accelerators or XAML see WM_KEYDOWN or
// WM_SYSKEYDOWN.
WXDLLIMPEXP_CORE bool
wxWinUI3ConsumeBlockedKeyboardMessage(WXMSG *msg);

// Test-only controls for exercising the real hook and fallback paths without
// relying on the host keyboard layout or on SetWindowsHookEx() failing.
WXDLLIMPEXP_CORE bool
wxWinUI3ForceEventLoopFallbackForTesting(bool forceFallback);
WXDLLIMPEXP_CORE void
wxWinUI3SetKeyboardModifiersForTesting(
    const wxWinUIKeyboardModifiers *modifiers);
WXDLLIMPEXP_CORE bool
wxWinUI3GetKeyboardModifiersOverrideForTesting(
    wxWinUIKeyboardModifiers *modifiers);
WXDLLIMPEXP_CORE void
wxWinUI3SetBlockedKeyboardKeyForTesting(WXWPARAM key);

// Exercise the production HWND resolver and WH_GETMESSAGE removal semantics
// while keeping the final XAML call injectable and countable.
WXDLLIMPEXP_CORE wxWinUIKeyboardPipelineResult
wxWinUI3ProcessKeyboardMessageForTesting(
    WXMSG *msg,
    const wxWinUIKeyboardModifiers& modifiers,
    wxWinUIXamlPreTranslateForTest xamlPreTranslate,
    void *xamlContext);
WXDLLIMPEXP_CORE bool
wxWinUI3ProcessGetMessageHookForTesting(
    int code,
    WXWPARAM removalMode,
    WXMSG *msg,
    const wxWinUIKeyboardModifiers& modifiers,
    wxWinUIXamlPreTranslateForTest xamlPreTranslate,
    void *xamlContext);
WXDLLIMPEXP_CORE bool
wxWinUI3ProcessEventLoopFallbackForTesting(
    WXMSG *msg,
    const wxWinUIKeyboardModifiers& modifiers,
    wxWinUIXamlPreTranslateForTest xamlPreTranslate,
    void *xamlContext);

#endif // wxUSE_WINUI3

#endif // _WX_WINUI_PRIVATE_INPUTTEST_H_
