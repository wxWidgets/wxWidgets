/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/private/menutest.h
// Purpose:     Internal deterministic probes for WinUI menu projection
// Author:      wxWidgets development team
// Created:     2026-07-23
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PRIVATE_MENUTEST_H_
#define _WX_WINUI_PRIVATE_MENUTEST_H_

#include "wx/defs.h"
#include "wx/string.h"

#if wxUSE_WINUI3 && wxUSE_MENUS

class WXDLLIMPEXP_FWD_CORE wxWindow;
class WXDLLIMPEXP_FWD_CORE wxMenu;

enum class wxWinUIMenuFlowDirection
{
    Unknown = -1,
    LeftToRight = 0,
    RightToLeft = 1
};

struct wxWinUIMenuItemSnapshot
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
    wxWinUIMenuFlowDirection flowDirection =
        wxWinUIMenuFlowDirection::Unknown;
};

// A menu path starts with a wxMenuBar top index, followed by zero or more
// positions of submenu items. These probes execute the same per-menu state
// machine as the sentinel callbacks, without exposing C++/WinRT types.
WXDLLIMPEXP_CORE bool
wxWinUI3MenuBarOpenForTesting(wxWindow *menuBarWindow,
                              const size_t *menuPath,
                              size_t menuPathLength);
WXDLLIMPEXP_CORE bool
wxWinUI3MenuBarCloseForTesting(wxWindow *menuBarWindow,
                               const size_t *menuPath,
                               size_t menuPathLength);
WXDLLIMPEXP_CORE bool
wxWinUI3MenuBarSnapshotForTesting(wxWindow *menuBarWindow,
                                  const size_t *menuPath,
                                  size_t menuPathLength,
                                  size_t itemPosition,
                                  wxWinUIMenuItemSnapshot *snapshot);
WXDLLIMPEXP_CORE bool
wxWinUI3MenuBarInvokeForTesting(wxWindow *menuBarWindow,
                                const size_t *menuPath,
                                size_t menuPathLength,
                                size_t itemPosition);
WXDLLIMPEXP_CORE bool
wxWinUI3MenuBarSetExpandedForTesting(wxWindow *menuBarWindow,
                                     const size_t *menuPath,
                                     size_t menuPathLength,
                                     bool expanded);
WXDLLIMPEXP_CORE bool
wxWinUI3MenuBarGetExpandedForTesting(wxWindow *menuBarWindow,
                                     const size_t *menuPath,
                                     size_t menuPathLength,
                                     bool *expanded);
WXDLLIMPEXP_CORE size_t
wxWinUI3MenuBarProjectionCountForTesting(wxWindow *menuBarWindow);
WXDLLIMPEXP_CORE bool
wxWinUI3MenuBarSentinelContractForTesting(wxWindow *menuBarWindow,
                                          const size_t *menuPath,
                                          size_t menuPathLength);
WXDLLIMPEXP_CORE bool
wxWinUI3MenuBarTopEnabledForTesting(wxWindow *menuBarWindow,
                                    size_t topIndex,
                                    bool *enabled);
WXDLLIMPEXP_CORE bool
wxWinUI3MenuBarTopLabelForTesting(wxWindow *menuBarWindow,
                                  size_t topIndex,
                                  wxString *label,
                                  wxString *accessKey);
WXDLLIMPEXP_CORE int
wxWinUI3MenuBarHeightForTesting(wxWindow *menuBarWindow);
WXDLLIMPEXP_CORE bool
wxWinUI3MenuBarFlowDirectionForTesting(wxWindow *menuBarWindow,
                                       wxWinUIMenuFlowDirection *barDirection,
                                       wxWinUIMenuFlowDirection *firstTopDirection);
WXDLLIMPEXP_CORE bool
wxWinUI3MenuBarRefreshForScaleForTesting(wxWindow *menuBarWindow,
                                         double scale);
// True only after one external projection transaction exhausted its total
// synchronous-plus-deferred retry budget.
WXDLLIMPEXP_CORE bool
wxWinUI3MenuBarBuildQuarantinedForTesting(wxWindow *menuBarWindow);

// Force the next popup dispatcher drain to take the TryEnqueue(false) path.
WXDLLIMPEXP_CORE void wxWinUI3RejectNextPopupDrainForTesting();
WXDLLIMPEXP_CORE void
wxWinUI3FailNextPopupTimersForTesting(bool drain, bool watchdog);
WXDLLIMPEXP_CORE bool
wxWinUI3PopupMenuForTesting(wxWindow *win,
                            wxMenu *menu,
                            int x,
                            int y);

#endif // wxUSE_WINUI3 && wxUSE_MENUS

#endif // _WX_WINUI_PRIVATE_MENUTEST_H_
