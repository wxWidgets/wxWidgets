/////////////////////////////////////////////////////////////////////////
// File:        taskbarcmn.cpp
// Purpose:     Common parts of wxTaskBarIcon class
// Author:      Julian Smart
// Modified by:
// Created:     04/04/2003
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, 2003
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "taskbar.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/defs.h"

#ifdef wxHAS_TASK_BAR_ICON

// DLL options compatibility check:
#include "wx/app.h"
WX_CHECK_BUILD_OPTIONS("wxAdvanced")


#include "wx/menu.h"
#include "wx/taskbar.h"

DEFINE_EVENT_TYPE( wxEVT_TASKBAR_MOVE )
DEFINE_EVENT_TYPE( wxEVT_TASKBAR_LEFT_DOWN )
DEFINE_EVENT_TYPE( wxEVT_TASKBAR_LEFT_UP )
DEFINE_EVENT_TYPE( wxEVT_TASKBAR_RIGHT_DOWN )
DEFINE_EVENT_TYPE( wxEVT_TASKBAR_RIGHT_UP )
DEFINE_EVENT_TYPE( wxEVT_TASKBAR_LEFT_DCLICK )
DEFINE_EVENT_TYPE( wxEVT_TASKBAR_RIGHT_DCLICK )


BEGIN_EVENT_TABLE(wxTaskBarIconBase, wxEvtHandler)
    EVT_TASKBAR_RIGHT_DOWN(wxTaskBarIconBase::OnRightButtonDown)
END_EVENT_TABLE()

void wxTaskBarIconBase::OnRightButtonDown(wxTaskBarIconEvent& WXUNUSED(event))
{
    wxMenu *menu = CreatePopupMenu();
    if (menu)
    {
        PopupMenu(menu);
        delete menu;
    }
}

#endif // defined(wxHAS_TASK_BAR_ICON)
