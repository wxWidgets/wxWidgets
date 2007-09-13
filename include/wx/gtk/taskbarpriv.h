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

#include <gtk/gtkversion.h>
#if GTK_CHECK_VERSION(2, 1, 0)

class WXDLLIMPEXP_ADV wxTaskBarIconAreaBase : public wxTopLevelWindow
{
public:
    wxTaskBarIconAreaBase();

    // Returns true if SYSTRAY protocol is supported by the desktop
    bool IsProtocolSupported();

    virtual bool IsDecorCacheable() const;

    wxEvtHandler *m_invokingWindow;

protected:
#if wxUSE_MENUS_NATIVE
    virtual bool DoPopupMenu( wxMenu *menu, int x, int y );
#endif // wxUSE_MENUS_NATIVE
};

#endif // GTK_CHECK_VERSION(2, 1, 0)
#endif // _WX_GTK_TASKBARPRIV_H_
