/////////////////////////////////////////////////////////////////////////////
// Private, non-installed input-router observations.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_INPUT_ROUTER_TEST_ACCESS_H
#define WX_WINUI_INPUT_ROUTER_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/defs.h"

#if wxUSE_WINUI3

class WXDLLIMPEXP_CORE wxWinUIInputRouterTestAccess final
{
public:
    struct Counters
    {
        unsigned long long resolveAttempts = 0;
        unsigned long long resolveRestarts = 0;
        unsigned long long resolveUnstable = 0;
        unsigned long long transparentCandidates = 0;
    };

    static Counters GetCounters();
    static void ResetCounters();
};

#endif // wxUSE_WINUI3

#endif // WX_WINUI_INPUT_ROUTER_TEST_ACCESS_H
