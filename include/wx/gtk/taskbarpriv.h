/////////////////////////////////////////////////////////////////////////
// File:        wx/gtk/taskbarpriv.h
// Purpose:     wxTaskBarIcon (src/unix/taskbarx11.cpp) helper for GTK2
// Author:      Vaclav Slavik
// Modified by:
// Created:     2004/05/29
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik, 2004
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_TASKBARPRIV_H_
#define _WX_GTK_TASKBARPRIV_H_

#include "wx/toplevel.h"

class WXDLLIMPEXP_ADV wxTaskBarIconAreaBase : public wxTopLevelWindow
{
public:
    wxTaskBarIconAreaBase();

    // Returns true if SYSTRAY protocol is supported by the desktop
    static bool IsProtocolSupported();

    wxEvtHandler *m_invokingWindow;

protected:
#if wxUSE_MENUS_NATIVE
    virtual void DoPopupMenuUpdateUI(wxMenu* menu);
#endif // wxUSE_MENUS_NATIVE
};

#endif // _WX_GTK_TASKBARPRIV_H_
