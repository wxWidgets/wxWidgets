/////////////////////////////////////////////////////////////////////////////
// Private, non-installed adapters for existing host cursor policy and state.
/////////////////////////////////////////////////////////////////////////////
#ifndef WX_WINUI_HOST_CURSOR_TEST_ACCESS_H
#define WX_WINUI_HOST_CURSOR_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/winui/private/tlwhost.h"

#if wxUSE_WINUI3

class WXDLLIMPEXP_CORE wxWinUIHostCursorTestAccess final
{
public:
    // Reads and actions use the existing host/slot state and native policy.
    // The XAML-selection action writes the actual Cursor property without
    // manufacturing ownership, then uses the production observer to reconcile.
    static unsigned GetSlotCursorSetCount();
    static winrt::Microsoft::UI::Input::InputCursor
    GetSlotCursor(
        const wxWinUITopLevelHost& host,
        const wxWindow *window);
    static winrt::Microsoft::UI::Input::InputCursor
    GetIslandCursor(
        const wxWinUITopLevelHost& host);
    static bool
    ActivateSlotCursor(
        wxWinUITopLevelHost& host,
        wxWindow *window,
        bool emitSetCursorEvent = false);
    static bool
    ApplySlotPointerCursor(
        wxWinUITopLevelHost& host,
        wxWindow *window,
        const wxPoint& screenPoint,
        wxWinUIInputKind kind);
    static bool
    SimulateXamlCursorSelection(
        wxWinUITopLevelHost& host,
        const winrt::Microsoft::UI::Input::InputCursor& cursor);
    static WXHCURSOR
    GetIslandCursorHandle(
        const wxWinUITopLevelHost& host);
    static bool
    IsInputPointerSourceAuthoritative(
        const wxWinUITopLevelHost& host);
    static bool
    RefreshPointerCursorAt(
        wxWinUITopLevelHost& host,
        const wxPoint& screenPoint);
};

#endif // wxUSE_WINUI3

#endif // WX_WINUI_HOST_CURSOR_TEST_ACCESS_H
