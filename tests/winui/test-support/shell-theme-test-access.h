/////////////////////////////////////////////////////////////////////////////
// Private, non-installed shell-theme policy and native-boundary adapters.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_SHELL_THEME_TEST_ACCESS_H
#define WX_WINUI_SHELL_THEME_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/winui/private/shelltheme.h"

#if wxUSE_WINUI3

class WXDLLIMPEXP_CORE wxWinUIShellThemeTestAccess final
{
public:
    struct NativeOps
    {
        void *context = nullptr;

        bool (*setImmersiveDarkMode)(void *, WXHWND, bool) = nullptr;
        bool (*setSystemBackdrop)(void *, WXHWND, bool) = nullptr;
        bool (*extendFrameIntoClientArea)(void *, WXHWND, bool) = nullptr;
        bool (*setBackdropMarker)(void *, WXHWND, bool) = nullptr;
        bool (*setControlTheme)(void *, WXHWND, bool, bool) = nullptr;
    };

    static wxWinUIShellThemePolicy ResolvePolicy(
        wxWinUIAppTheme appTheme,
        bool systemDark,
        bool highContrast);
    static void SetNativeOps(const NativeOps& operations);
    static void ResetNativeOps();
};

#endif // wxUSE_WINUI3

#endif // WX_WINUI_SHELL_THEME_TEST_ACCESS_H
