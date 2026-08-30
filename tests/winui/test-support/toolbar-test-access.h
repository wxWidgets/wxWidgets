/////////////////////////////////////////////////////////////////////////////
// Private, non-installed access to wxToolBar test operations.
// Available only in an explicitly test-enabled WinUI library build.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_TOOLBAR_TEST_ACCESS_H
#define WX_WINUI_TOOLBAR_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/toolbar.h"

#if wxUSE_TOOLBAR
class WXDLLIMPEXP_CORE wxWinUIToolBarTestAccess final
{
public:
    struct PeerSnapshot
    {
        bool enabled = false;
        bool toggled = false;
        bool overflowed = false;
        bool overflowEligible = false;
        bool hasIcon = false;
        bool usesDisabledBitmap = false;
        bool showsText = false;
        bool horizontalText = false;
        bool stretchable = false;
        bool control = false;
        wxSize selectedPixelSize;
        wxRect bounds;
        wxString toolTip;
        wxString peerToolTip;
        wxString helpText;
        wxString automationName;
        wxString automationLocalizedControlType;
        unsigned long long peerIdentity = 0;
        int automationControlType = 0;
        bool radioAutomationRole = false;
        bool supportsInvokePattern = false;
        bool supportsTogglePattern = false;
        bool supportsSelectionItemPattern = false;
    };

    using CallbackHook =
        void (*)(wxToolBar *toolbar, void *context);

    static bool InvokeTool(wxToolBar& toolbar, int toolid, bool dropdownPart = false);
    static bool QueueToolClick(wxToolBar& toolbar, int toolid);
    static bool IsRootLoaded(const wxToolBar& toolbar);
    static bool IsOverflowChevronReady(const wxToolBar& toolbar);
    static bool RequestOpenOverflow(wxToolBar& toolbar);
    static bool IsOverflowToolLoaded(
        const wxToolBar& toolbar, int toolid, bool dropdownPart = false);
    static bool InvokeOverflowTool(
        wxToolBar& toolbar, int toolid, bool dropdownPart = false);
    static bool HoverTool(wxToolBar& toolbar, int toolid, bool entered);
    static bool RightClickTool(wxToolBar& toolbar, int toolid);
    static bool GetToolPeerState(
        const wxToolBar& toolbar, int toolid, PeerSnapshot *snapshot);
    static size_t GetPeerToolCount(const wxToolBar& toolbar);
    static size_t GetOverflowedToolCount(const wxToolBar& toolbar);
    static bool ApplyOverflowExtent(wxToolBar& toolbar, wxCoord extent);
    static bool IsOverflowChevronVisible(const wxToolBar& toolbar);
    static wxRect GetOverflowChevronBounds(const wxToolBar& toolbar);
    static wxString GetOverflowChevronName(const wxToolBar& toolbar);
    static bool RefreshForScale(wxToolBar& toolbar, double scale);
    static void SetNextRebuildLoadedHook(
        wxToolBar& toolbar, CallbackHook hook, void *context);
    static void SetNextShortHelpSetterHook(
        wxToolBar& toolbar, CallbackHook hook, void *context);
    static void SetNextEnableSetterHook(
        wxToolBar& toolbar, CallbackHook hook, void *context);
    static void SetNextOverflowMutationHook(
        wxToolBar& toolbar, CallbackHook hook, void *context);
    static void FailNextOverflowMutation(wxToolBar& toolbar, unsigned boundary);
    static void FailNextShortHelpSetters(
        wxToolBar& toolbar, int toolid, unsigned count);
    static void FailNextRebuild(wxToolBar& toolbar);
    static void ClosePeer(wxToolBar& toolbar);
    static size_t GetLiveCallbackStateCount();
};
#endif // wxUSE_TOOLBAR

#endif // WX_WINUI_TOOLBAR_TEST_ACCESS_H
