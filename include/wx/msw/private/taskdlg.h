///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/taskdlg.h
// Purpose:     Dark mode support for native TaskDialog.
// Author:      Mohmed Abdel-Fattah (memoarfaa)
// Created:     2026-06-07
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_TASKDLG_H_
#define _WX_MSW_PRIVATE_TASKDLG_H_

#include "wx/msw/wrapcctl.h"

namespace wxMSWDarkMode
{

// ----------------------------------------------------------------------------
// TaskDialog dark mode helpers
// ----------------------------------------------------------------------------
//
// These two functions bracket the lifetime of dark-mode theming for any
// TaskDialog-based dialog (wxMessageDialog, wxRichMessageDialog,
// wxProgressDialog, wxAboutBox).
//
// Typical usage inside a PFTASKDIALOGCALLBACK:
//
//   case TDN_CREATED:
//       wxMSWDarkMode::ApplyToTaskDialog(hwnd, pCfg);
//       break;
//   case TDN_DESTROYED:
//       wxMSWDarkMode::RemoveFromTaskDialog(hwnd);
//       break;
//
// Both functions are no-ops when IsActive() returns false, so no guard is
// needed at the call site.

// Apply dark mode theming to an existing TaskDialog window.
//
// Must be called from TDN_CREATED (not TDN_DIALOG_CONSTRUCTED) so that the
// complete child-window hierarchy exists and UI Automation can walk it.
//
// pCfg may be nullptr; when provided it enables correct icon overdraw on
// Windows 10 where there is no native dark TaskDialog theme.
void AllowForTaskDialog(HWND hwnd, const TASKDIALOGCONFIG* pCfg = nullptr);

// Remove all subclasses and free all GDI/theme resources installed by
// ApplyToTaskDialog().
//
// Must be called from TDN_DESTROYED or WM_DESTROY.
// Safe to call even if ApplyToTaskDialog() was never called for this HWND.
void RemoveFromTaskDialog(HWND hwnd);

} // namespace wxMSWDarkMode

#endif // _WX_MSW_PRIVATE_TASKDLG_H_
