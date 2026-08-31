/////////////////////////////////////////////////////////////////////////////
// Private, non-installed adapters for existing wxTipWindow callback boundaries.
/////////////////////////////////////////////////////////////////////////////
#ifndef WX_WINUI_TIP_WINDOW_TEST_ACCESS_H
#define WX_WINUI_TIP_WINDOW_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/defs.h"
#include <cstddef>

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_TIPWINDOW

class WXDLLIMPEXP_FWD_CORE wxTipWindow;

class WXDLLIMPEXP_CORE wxWinUITipWindowTestAccess final
{
public:
    enum class HookPoint
    {
        AfterPointerPublication,
        AfterViewAdjust,
        AfterPopup
    };

    using CreateHook = void (*)(wxTipWindow *tip);
    using Hook = void (*)(wxTipWindow *, HookPoint);

    // These existing hooks remain installed until reset/replaced. The create
    // hook runs after native popup creation and before external publication;
    // all production weak-identity/backlink/view checks remain after callbacks.
    static void SetCreateHook(CreateHook hook);
    static void ResetCreateHook();
    static void SetHook(Hook hook);
    static void ResetHook();

    // Fail before runtime sidecar insertion (wxUSE_EXCEPTIONS tests only).
    // Close still uses its allocation-free intrusive TLS record.
    static void FailNextRuntimeAllocation();
    static std::size_t GetRuntimeStateCount();
};

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_TIPWINDOW

#endif // WX_WINUI_TIP_WINDOW_TEST_ACCESS_H
