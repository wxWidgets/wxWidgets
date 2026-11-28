///////////////////////////////////////////////////////////////////////////////
// Name:        tests/winui/test-support/toolbar-test-access.h
// Purpose:     what the tests may see and do in the WinUI tool bar
// Author:      wxWidgets development team
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_TOOLBAR_TEST_ACCESS_H
#define WX_WINUI_TOOLBAR_TEST_ACCESS_H

#include "wx/toolbar.h"

#if wxUSE_TOOLBAR
class WXDLLIMPEXP_CORE wxWinUIToolBarTestAccess final
{
public:
    // The XAML element which stands for a tool, as it is now.
    struct PeerSnapshot
    {
        bool enabled = false;
        bool toggled = false;
        bool overflowed = false;
        bool hasIcon = false;
        bool usesDisabledBitmap = false;
        bool showsText = false;
        bool control = false;
        bool dropdown = false;
        double iconOpacity = 1.0;
        wxRect bounds;
        wxString label;
        wxString toolTip;
    };

    // Does what a screen reader does to the button of the tool, or to the
    // button of its menu: false if there is none or if it is disabled.
    static bool InvokeTool(wxToolBar& toolbar, int toolid,
                           bool dropdownPart = false);

    static bool GetToolPeerState(
        const wxToolBar& toolbar, int toolid, PeerSnapshot *snapshot);

    // The tools which have something in the bar, separators and spaces
    // included, and those which are in the menu of what does not fit.
    static size_t GetPeerToolCount(const wxToolBar& toolbar);
    static size_t GetOverflowedToolCount(const wxToolBar& toolbar);

    // The tool bars whose XAML side is alive.
    static size_t GetLiveCallbackStateCount();
};
#endif // wxUSE_TOOLBAR

#endif // WX_WINUI_TOOLBAR_TEST_ACCESS_H
