/////////////////////////////////////////////////////////////////////////////
// Private, non-installed access to StatusBar test operations.
// Available only in an explicitly test-enabled WinUI library build.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_STATUSBAR_TEST_ACCESS_H
#define WX_WINUI_STATUSBAR_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
    #error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/statusbr.h"

struct wxWinUIAppearanceSnapshot;

#if wxUSE_STATUSBAR
enum class wxWinUIStatusBarReentryPointForTesting
{
    RebuildLoaded,
    TextValue,
    AppearanceRootFont,
    DPIBorderX,
    MinHeightBeforeResize
};

struct wxWinUIStatusBarSizeGripSnapshot
{
    double widthDips = 0.0;
    double heightDips = 0.0;
    int horizontalAlignment = 0;
    int cursorShape = 0;
    int nativeHitTest = 0;
    wxString automationName;
    wxString localizedControlType;
    bool overlaysFields = false;
    bool reservesFieldSpace = false;
    double fieldReservationDips = 0.0;
};

class WXDLLIMPEXP_CORE wxWinUIStatusBarTestAccess final
{
public:
    using ReentryHook = void (*)(wxStatusBar*, void*);

    static bool HasSizeGrip(const wxStatusBar& bar);
    static bool GetSizeGripState(const wxStatusBar& bar,
                                 wxWinUIStatusBarSizeGripSnapshot* snapshot);
    // Invoke the production grip action, without replacing native dispatch.
    static bool InvokeSizeGrip(wxStatusBar& bar,
                               bool isMouse,
                               bool isPrimary,
                               bool isLeftButtonPressed,
                               const wxPoint& screenPoint);
    static void SetTopLevelMaximized(wxStatusBar& bar, bool maximized);
    static bool GetFieldState(const wxStatusBar& bar,
                             int field,
                             wxString* renderedText,
                             int* textTrimming,
                             int* fieldStyle,
                             bool* hasToolTip,
                             double* columnValue,
                             int* columnUnitType,
                             wxString* automationName,
                             int* borderElementCount = nullptr,
                             int* fieldGridFlowDirection = nullptr,
                             int* textFlowDirection = nullptr);
    static bool GetAppearance(const wxStatusBar& bar,
                              wxWinUIAppearanceSnapshot* snapshot);
    static bool UsesThemeBorders(const wxStatusBar& bar);
    static void SetNextReentryHook(wxStatusBar& bar,
                                  wxWinUIStatusBarReentryPointForTesting point,
                                  ReentryHook hook, void* context);
    static unsigned long long GetModelRevision(const wxStatusBar& bar);
    static bool HasDeferredRebuild(const wxStatusBar& bar);
    static bool IsRebuildQuarantined(const wxStatusBar& bar);
    // Bypass the outer wxEvtHandler dispatcher: synchronous deletion from
    // this lifetime seam must test the implementation, not event-table walks.
    static void DeliverDPIChanged(wxStatusBar& bar, wxDPIChangedEvent& event);
};
#endif // wxUSE_STATUSBAR

#endif // WX_WINUI_STATUSBAR_TEST_ACCESS_H
