/////////////////////////////////////////////////////////////////////////////
// Private, non-installed wxSplashScreen lifetime test access.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_SPLASH_TEST_ACCESS_H
#define WX_WINUI_SPLASH_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/defs.h"

#include <cstddef>

#if wxUSE_WINUI3 && wxUSE_SPLASH

class wxSplashScreen;

class WXDLLIMPEXP_CORE wxWinUISplashTestAccess final
{
public:
    enum class HookPoint
    {
        AfterContent,
        AfterTimerStart,
        AfterFilterRegistration,
        AfterShow,
        AfterUpdate
    };

    using Hook = void (*)(wxSplashScreen *, HookPoint);

    static void SetHook(Hook hook);
    static void ResetHook();
    static void FailNextRuntimeAllocation();
    static std::size_t GetRuntimeStateCount();
};

#endif // wxUSE_WINUI3 && wxUSE_SPLASH

#endif // WX_WINUI_SPLASH_TEST_ACCESS_H
