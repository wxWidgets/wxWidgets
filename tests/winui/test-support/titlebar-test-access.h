/////////////////////////////////////////////////////////////////////////////
// Private, non-installed TitleBar fault and native observation access.
/////////////////////////////////////////////////////////////////////////////
#ifndef WX_WINUI_TITLEBAR_TEST_ACCESS_H
#define WX_WINUI_TITLEBAR_TEST_ACCESS_H
#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/winui/private/titlebar.h"

#if wxUSE_WINUI3
class WXDLLIMPEXP_CORE wxWinUITitleBarTestAccess final
{
public:
    // Exactly the existing non-recursive synchronous callback boundaries.
    enum class HookPoint
    {
        AfterExtensionWrite,
        AfterDisableWrite,
        AfterResetToDefault,
        AfterDragRectanglesWrite,
        BeforeExceptionLog,
        AfterExceptionLog
    };
    using Hook = void (*)(wxWindow *tlw, HookPoint point);

    enum Fault : unsigned
    {
        Fault_None = 0,
        Fault_DisableWrite = 1u << 0,
        Fault_ResetToDefault = 1u << 1,
        Fault_ExtensionWriteAfter = 1u << 2,
        Fault_DragWriteAfter = 1u << 3,
        Fault_ReadBeforeMutation = 1u << 4,
        Fault_ReadAfterMutation = 1u << 5
    };

    // This is a native/XAML observation, NOT a passive cached snapshot.
    // Identity reads and the generation-checked visual-tree scan are retained.
    struct Snapshot
    {
        bool attached = false;
        bool hwndIdentityCurrent = false;
        bool appWindowAvailable = false;
        bool appWindowIdentityCurrent = false;
        bool customizationSupported = false;
        bool systemFallback = false;
        bool firstPartyXamlTitleBarScanSucceeded = false;
        bool firstPartyXamlTitleBarInstalled = false;
        bool dragRectangleWriteCommitted = false;
        wxWinUITitleBarMode mode = wxWinUITitleBarMode::Unknown;
        WXHWND hwnd = nullptr;
        unsigned long long hwndGeneration = 0;
        unsigned long long currentHwndGeneration = 0;
        unsigned long long nativeHwndGeneration = 0;
        unsigned long long currentNativeHwndGeneration = 0;
        unsigned long long appWindowId = 0;
        unsigned long long currentAppWindowId = 0;
        unsigned firstPartyXamlTitleBarCount = 0;
        int dragRectangleX = 0;
        int dragRectangleY = 0;
        int dragRectangleWidth = 0;
        int dragRectangleHeight = 0;
    };

    // Exercises real AppWindow enable/disable/fallback on owned test frames.
    // It is not a public wx title-bar customization API.
    static bool SetExtended(wxWindow *tlw, bool extended);
    static bool GetSnapshot(wxWindow *tlw, Snapshot *snapshot);
    static void FailNextExtension();
    static void SetHook(Hook hook);
    static void SetFaultMask(unsigned mask);
    static bool InvalidateNativeIdentity(wxWindow *tlw);
    static bool InvalidateAppWindowIdentity(wxWindow *tlw);
    static unsigned GetLivePolicyCount();
    static unsigned GetTeardownResetCount();
};
#endif // wxUSE_WINUI3
#endif // WX_WINUI_TITLEBAR_TEST_ACCESS_H
