/////////////////////////////////////////////////////////////////////////////
// Non-installed deterministic adapters for the real WinUI keyboard pipeline.
/////////////////////////////////////////////////////////////////////////////
#ifndef WX_WINUI_KEYBOARD_TEST_ACCESS_H
#define WX_WINUI_KEYBOARD_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "../../../src/winui/keyboard.h"

#if wxUSE_WINUI3

class WXDLLIMPEXP_CORE wxWinUIKeyboardTestAccess final
{
public:
    static wxWinUIKeyboardPipelineResult RunPipeline(
        WXMSG *msg, wxWindow *logicalOwner,
        const wxWinUIKeyboardModifiers& modifiers,
        wxWinUIXamlPreTranslateCallback xamlPreTranslate, void *xamlContext);
    static wxWinUIKeyboardPipelineResult ProcessKeyboardMessage(
        WXMSG *msg, const wxWinUIKeyboardModifiers& modifiers,
        wxWinUIXamlPreTranslateCallback xamlPreTranslate, void *xamlContext);
    static bool ProcessGetMessageHook(
        int code, WXWPARAM removalMode, WXMSG *msg,
        const wxWinUIKeyboardModifiers& modifiers,
        wxWinUIXamlPreTranslateCallback xamlPreTranslate, void *xamlContext);
    static bool ProcessEventLoopFallback(
        WXMSG *msg, const wxWinUIKeyboardModifiers& modifiers,
        wxWinUIXamlPreTranslateCallback xamlPreTranslate, void *xamlContext);
    static bool ForceEventLoopFallback(bool forceFallback);
    static void SetKeyboardModifiers(const wxWinUIKeyboardModifiers *modifiers);
    static bool GetKeyboardModifiersOverride(wxWinUIKeyboardModifiers *modifiers);
    static void SetBlockedKeyboardKey(WXWPARAM key);
};

#endif // wxUSE_WINUI3
#endif // WX_WINUI_KEYBOARD_TEST_ACCESS_H
