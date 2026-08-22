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

// Boundaries which can synchronously re-enter wx code through WinRT, logging
// or a test callback. The callback is implementation-only and is always
// invoked at most once at a time: a nested title-bar operation doesn't call
// it recursively.
enum class wxWinUITitleBarHookPointForTesting
{
    AfterExtensionWrite,
    AfterDisableWrite,
    AfterResetToDefault,
    AfterDragRectanglesWrite,
    BeforeExceptionLog,
    AfterExceptionLog
};

using wxWinUITitleBarHookForTesting = void (*)(
    wxWindow *tlw,
    wxWinUITitleBarHookPointForTesting point);

enum wxWinUITitleBarFaultForTesting : unsigned
{
    wxWINUI_TITLEBAR_FAULT_NONE = 0,
    wxWINUI_TITLEBAR_FAULT_DISABLE_WRITE = 1u << 0,
    wxWINUI_TITLEBAR_FAULT_RESET_TO_DEFAULT = 1u << 1,
    wxWINUI_TITLEBAR_FAULT_EXTENSION_WRITE_AFTER = 1u << 2,
    wxWINUI_TITLEBAR_FAULT_DRAG_WRITE_AFTER = 1u << 3,
    wxWINUI_TITLEBAR_FAULT_READ_BEFORE_MUTATION = 1u << 4,
    wxWINUI_TITLEBAR_FAULT_READ_AFTER_MUTATION = 1u << 5
};

struct wxWinUITitleBarSnapshot
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

// Host-owned production lifecycle. These entry points never create another
// HWND, DesktopWindowXamlSource or XAML root.
WXDLLIMPEXP_CORE void wxWinUITitleBarAttachSystemPolicy(
    wxWinUITopLevelHost *host,
    const std::shared_ptr<wxWinUIHostLifetime>& hostLifetime,
    wxWindow *tlw,
    WXHWND hwnd);
WXDLLIMPEXP_CORE void wxWinUITitleBarDetachSystemPolicy(
    wxWinUITopLevelHost *host);

// Implementation-only deterministic seams. SetExtendedForTesting() is not a
// wx API: it only exercises AppWindowTitleBar's enable/disable/fallback
// transaction on otherwise empty test frames. Production never calls it.
WXDLLIMPEXP_CORE bool wxWinUITitleBarSetExtendedForTesting(
    wxWindow *tlw,
    bool extended);
WXDLLIMPEXP_CORE bool wxWinUITitleBarSnapshotForTesting(
    wxWindow *tlw,
    wxWinUITitleBarSnapshot *snapshot);
WXDLLIMPEXP_CORE void wxWinUITitleBarFailNextExtensionForTesting();
WXDLLIMPEXP_CORE void wxWinUITitleBarSetHookForTesting(
    wxWinUITitleBarHookForTesting hook);
WXDLLIMPEXP_CORE void wxWinUITitleBarSetFaultMaskForTesting(unsigned mask);
WXDLLIMPEXP_CORE bool
wxWinUITitleBarInvalidateNativeIdentityForTesting(wxWindow *tlw);
WXDLLIMPEXP_CORE bool
wxWinUITitleBarInvalidateAppWindowIdentityForTesting(wxWindow *tlw);
WXDLLIMPEXP_CORE unsigned wxWinUITitleBarGetLivePolicyCountForTesting();
WXDLLIMPEXP_CORE unsigned
wxWinUITitleBarGetTeardownResetCountForTesting();

#endif // wxUSE_WINUI3

#endif // _WX_WINUI_PRIVATE_TITLEBAR_H_
