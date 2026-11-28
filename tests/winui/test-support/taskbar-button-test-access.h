///////////////////////////////////////////////////////////////////////////////
// Private, non-installed MSW/WinUI taskbar-button test adapter.
///////////////////////////////////////////////////////////////////////////////

#ifndef WX_MSW_TASKBAR_BUTTON_TEST_ACCESS_H
#define WX_MSW_TASKBAR_BUTTON_TEST_ACCESS_H

#ifndef WXMSW_TASKBARBUTTON_TEST_SUPPORT
#error Enable the private MSW taskbar-button test support to use this header
#endif

#include "wx/msw/private/taskbarbutton.h"

#if defined(__WXMSW__) && wxUSE_TASKBARBUTTON

class WXDLLIMPEXP_CORE wxMSWTaskBarButtonTestAccess final
{
public:
    using NativeFactory =
        std::shared_ptr<wxMSWTaskBarButtonNativeBackend> (*)();

    struct Snapshot
    {
        bool available { false };
        bool retired { false };
        WXHWND hwnd { nullptr };
        unsigned long long hwndGeneration { 0 };
        unsigned long long shellEpoch { 0 };

        int desiredProgressRange { 0 };
        int desiredProgressValue { 0 };
        bool desiredProgressValueKnown { false };
        wxTaskBarButtonState desiredProgressState {
            wxTASKBAR_BUTTON_NO_PROGRESS
        };

        bool progressValueAppliedKnown { false };
        int appliedProgressRange { 0 };
        int appliedProgressValue { 0 };
        bool progressStateAppliedKnown { false };
        wxTaskBarButtonState appliedProgressState {
            wxTASKBAR_BUTTON_NO_PROGRESS
        };

        bool thumbButtonsAppliedKnown { false };
        bool thumbButtonsInitialized { false };
        std::size_t desiredThumbButtonCount { 0 };
        std::size_t appliedThumbButtonCount { 0 };
    };

    // GUI-thread-only replacement seam. Existing taskbar-button objects keep
    // the backend they already own.
    static void SetNativeFactory(NativeFactory factory);
    static void ResetNativeFactory();
    static bool GetSnapshot(wxTaskBarButton* button, Snapshot* snapshot);
};

#endif // __WXMSW__ && wxUSE_TASKBARBUTTON
#endif // WX_MSW_TASKBAR_BUTTON_TEST_ACCESS_H
