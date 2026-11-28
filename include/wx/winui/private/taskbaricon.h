///////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/private/taskbaricon.h
// Purpose:     Private wxTaskBarIcon WinUI applied-state contract
// Author:      wxWidgets development team
// Created:     2026-08-03
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PRIVATE_TASKBARICON_H_
#define _WX_WINUI_PRIVATE_TASKBARICON_H_

#include "wx/defs.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_TASKBARICON

enum class wxWinUITaskBarIconAppliedState
{
    Absent,
    Present,
    Unknown
};

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_TASKBARICON

#endif // _WX_WINUI_PRIVATE_TASKBARICON_H_
