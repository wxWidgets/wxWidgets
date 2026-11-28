/////////////////////////////////////////////////////////////////////////////
// Non-installed deterministic boundaries for real popup lifetime transactions.
/////////////////////////////////////////////////////////////////////////////
#ifndef WX_WINUI_POPUP_TEST_ACCESS_H
#define WX_WINUI_POPUP_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/defs.h"

#if wxUSE_WINUI3 && wxUSE_POPUPWIN
class WXDLLIMPEXP_FWD_CORE wxWindow;
class WXDLLIMPEXP_FWD_CORE wxPopupWindow;
class WXDLLIMPEXP_FWD_CORE wxPopupTransientWindow;

class WXDLLIMPEXP_CORE wxWinUIPopupTestAccess final
{
public:
    using OwnerPublishHook = void (*)(wxPopupWindow *popup);
    using CreateHook = void (*)(wxPopupWindow *popup);
    using OwnerDeactivateHook = void (*)(wxWindow *owner);
    using CaptureReleaseHook =
        void (*)(wxPopupTransientWindow *popup, unsigned int pass);

    // Existing exact native publication / recovery boundaries, unchanged.
    static void SetOwnerPublishHook(OwnerPublishHook hook);
    static void ResetOwnerPublishHook();
    static void FailNextOwnerRecovery();
    // Uses the existing central allocation fault backend in tests only.
    static void FailNextAllocation();
    // Ownerless HWND creation, native deactivation tail, expected release.
    static void SetCreateHook(CreateHook hook);
    static void ResetCreateHook();
    static void SetOwnerDeactivateHook(OwnerDeactivateHook hook);
    static void ResetOwnerDeactivateHook();
    static void SetCaptureReleaseHook(CaptureReleaseHook hook);
    static void ResetCaptureReleaseHook();
    // Force the established missing-identity terminal fallback without HWND
    // mutation; this does not replace real retirement or deferred destruction.
    static void FailNextTerminalNativeIdentity();
    // Direct deterministic entry into the existing dismissal implementation;
    // not a synthesized native click and not physical-input qualification.
    static void DismissFromOutside(wxPopupTransientWindow *popup);
};
#endif // wxUSE_WINUI3 && wxUSE_POPUPWIN

#endif // WX_WINUI_POPUP_TEST_ACCESS_H
