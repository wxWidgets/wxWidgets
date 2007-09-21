///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/wrapcctl.h
// Purpose:     Wrapper for the standard <commctrl.h> header
// Author:      Vadim Zeitlin
// Modified by:
// Created:     03.08.2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_WRAPCCTL_H_
#define _WX_MSW_WRAPCCTL_H_

#include "wx/msw/wrapwin.h"

#include <commctrl.h>

// define things which might be missing from our commctrl.h
#include "wx/msw/missing.h"

// Set Unicode format for a common control
inline void wxSetCCUnicodeFormat(HWND WXUNUSED_IN_WINCE(hwnd))
{
#ifndef __WXWINCE__
    ::SendMessage(hwnd, CCM_SETUNICODEFORMAT, wxUSE_UNICODE, 0);
#else // !__WXWINCE__
    // here it should be already in Unicode anyhow
#endif // __WXWINCE__/!__WXWINCE__
}

#if wxUSE_GUI
// Return the default font for the common controls
//
// this is implemented in msw/settings.cpp
class wxFont;
extern wxFont wxGetCCDefaultFont();
#endif

#endif // _WX_MSW_WRAPCCTL_H_
