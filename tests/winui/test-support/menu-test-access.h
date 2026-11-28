/////////////////////////////////////////////////////////////////////////////
// Private, non-installed access to deterministic WinUI menu probes.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_MENU_TEST_ACCESS_H
#define WX_WINUI_MENU_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
    #error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/defs.h"
#include "wx/string.h"

#if wxUSE_WINUI3 && wxUSE_MENUS

class WXDLLIMPEXP_FWD_CORE wxWindow;
class WXDLLIMPEXP_FWD_CORE wxMenu;

class WXDLLIMPEXP_CORE wxWinUIMenuTestAccess final
{
public:
    enum class FlowDirection
    {
        Unknown = -1,
        LeftToRight = 0,
        RightToLeft = 1
    };

    struct ItemSnapshot
    {
        wxString label;
        wxString accessKey;
        wxString accelerator;
        bool enabled = false;
        bool checked = false;
        bool radio = false;
        bool submenu = false;
        bool hasIcon = false;
        int iconPixelWidth = 0;
        int iconPixelHeight = 0;
        FlowDirection flowDirection = FlowDirection::Unknown;
    };

    static bool Open(wxWindow *menuBarWindow,
                     const size_t *menuPath,
                     size_t menuPathLength);
    static bool Close(wxWindow *menuBarWindow,
                      const size_t *menuPath,
                      size_t menuPathLength);
    static bool GetSnapshot(wxWindow *menuBarWindow,
                            const size_t *menuPath,
                            size_t menuPathLength,
                            size_t itemPosition,
                            ItemSnapshot *snapshot);
    static bool Invoke(wxWindow *menuBarWindow,
                       const size_t *menuPath,
                       size_t menuPathLength,
                       size_t itemPosition);
    static bool SetExpanded(wxWindow *menuBarWindow,
                            const size_t *menuPath,
                            size_t menuPathLength,
                            bool expanded);
    static bool GetExpanded(wxWindow *menuBarWindow,
                            const size_t *menuPath,
                            size_t menuPathLength,
                            bool *expanded);
    static size_t GetProjectionCount(wxWindow *menuBarWindow);
    static bool HasSentinelContract(wxWindow *menuBarWindow,
                                    const size_t *menuPath,
                                    size_t menuPathLength);
    static bool GetTopEnabled(wxWindow *menuBarWindow,
                              size_t topIndex,
                              bool *enabled);
    static bool GetTopLabel(wxWindow *menuBarWindow,
                            size_t topIndex,
                            wxString *label,
                            wxString *accessKey);
    static int GetBarHeight(wxWindow *menuBarWindow);
    static bool GetFlowDirection(wxWindow *menuBarWindow,
                                 FlowDirection *barDirection,
                                 FlowDirection *firstTopDirection);
    static bool RefreshForScale(wxWindow *menuBarWindow, double scale);
    static bool IsBuildQuarantined(wxWindow *menuBarWindow);

    static void RejectNextPopupDrain();
    static void FailNextPopupTimers(bool drain, bool watchdog);
    static bool PopupMenu(wxWindow *win,
                          wxMenu *menu,
                          int x,
                          int y);
};

#endif // wxUSE_WINUI3 && wxUSE_MENUS

#endif // WX_WINUI_MENU_TEST_ACCESS_H
