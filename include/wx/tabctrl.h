/////////////////////////////////////////////////////////////////////////////
// Name:        wx/tabctrl.h
// Purpose:     wxTabCtrl base header
// Author:      Julian Smart
// Modified by:
// Created:
// Copyright:   (c) Julian Smart
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TABCTRL_H_BASE_
#define _WX_TABCTRL_H_BASE_

#include "wx/defs.h"

#if wxUSE_TAB_DIALOG

extern WXDLLIMPEXP_CORE const wxEventType wxEVT_COMMAND_TAB_SEL_CHANGED;
extern WXDLLIMPEXP_CORE const wxEventType wxEVT_COMMAND_TAB_SEL_CHANGING;

#if defined(__WXMSW__)
    #include "wx/msw/tabctrl.h"
#elif defined(__WXMAC__)
    #include "wx/osx/tabctrl.h"
#elif defined(__WXPM__)
    #include "wx/os2/tabctrl.h"
#endif

#endif // wxUSE_TAB_DIALOG

#endif // _WX_TABCTRL_H_BASE_
