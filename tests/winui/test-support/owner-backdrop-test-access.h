/////////////////////////////////////////////////////////////////////////////
// Private, non-installed Owner/Backdrop transaction test adapters.
/////////////////////////////////////////////////////////////////////////////
#ifndef WX_WINUI_OWNER_BACKDROP_TEST_ACCESS_H
#define WX_WINUI_OWNER_BACKDROP_TEST_ACCESS_H
#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif
#include "../../../src/winui/transientops.h"
#include <cstddef>
#if wxUSE_WINUI3
struct wxWinUITransientSnapshot
{
    std::size_t transactionCount = 0;
    bool inFlight = false;
    bool restorePending = false;
    unsigned long long generation = 0;
    unsigned long long epoch = 0;
};

// These functions are implementation-only test seams. The caller must drain
// callbacks before replacing/resetting the table; stale callbacks are still
// generation/epoch checked and become harmless no-ops.
class WXDLLIMPEXP_CORE wxWinUIOwnerBackdropTestAccess final
{
public:
    static void SetOwnerNativeOps(const wxWinUIOwnerNativeOps& operations);
    static void ResetOwnerNativeOps();
    static wxWinUITransientSnapshot GetOwnerSnapshot(WXHWND hwnd);
    static void SetBackdropNativeOps(const wxWinUIBackdropNativeOps& operations);
    static void ResetBackdropNativeOps();
    static wxWinUITransientSnapshot GetBackdropSnapshot(WXHWND hwnd);
};
#endif // wxUSE_WINUI3
#endif // WX_WINUI_OWNER_BACKDROP_TEST_ACCESS_H
