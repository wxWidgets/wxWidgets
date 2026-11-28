/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/private/titlebar.h
// Purpose:     WinAppSDK title-bar policy for an existing wx TLW
// Author:      wxWidgets development team
// Created:     2026-08-02
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PRIVATE_TITLEBAR_H_
#define _WX_WINUI_PRIVATE_TITLEBAR_H_

#include "wx/defs.h"

#if wxUSE_WINUI3

#include "wx/msw/wrapwin.h"

#include <memory>

class WXDLLIMPEXP_FWD_CORE wxWindow;
class wxWinUIHostLifetime;
class wxWinUITopLevelHost;

// V0 deliberately keeps the ordinary Win32 caption as the production mode.
// wx has no public API describing arbitrary title-bar content or non-drag
// regions, so installing Microsoft.UI.Xaml.Controls.TitleBar would either
// invent a second public contract or turn existing client controls into drag
// regions. AppWindowTitleBar is nevertheless acquired from the exact existing
// TLW AppWindow, both to make that ownership explicit and to provide a
// generation-safe implementation seam for a future public contract.
enum class wxWinUITitleBarMode
{
    Unknown,
    System,
    ExtendedAppWindow
};

// Host-owned production lifecycle. These entry points never create another
// HWND, DesktopWindowXamlSource or XAML root.
WXDLLIMPEXP_CORE void wxWinUITitleBarAttachSystemPolicy(
    wxWinUITopLevelHost *host,
    const std::shared_ptr<wxWinUIHostLifetime>& hostLifetime,
    wxWindow *tlw,
    WXHWND hwnd);
WXDLLIMPEXP_CORE void wxWinUITitleBarDetachSystemPolicy(
    wxWinUITopLevelHost *host);

#endif // wxUSE_WINUI3

#endif // _WX_WINUI_PRIVATE_TITLEBAR_H_
