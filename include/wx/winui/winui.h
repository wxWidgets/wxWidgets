/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/winui.h
// Purpose:     wxWinUI declarations
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_WINUI_H_
#define _WX_WINUI_WINUI_H_

#include "wx/defs.h"

#if wxUSE_WINUI3

WXDLLIMPEXP_CORE bool wxWinUI3Initialize();
WXDLLIMPEXP_CORE void wxWinUI3Uninitialize();
WXDLLIMPEXP_CORE bool wxWinUI3PreTranslateMessage(WXMSG *msg);

#endif // wxUSE_WINUI3

#endif // _WX_WINUI_WINUI_H_
