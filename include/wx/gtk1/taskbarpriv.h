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

#ifndef _WX_TASKBARPRIV_H_
#define _WX_TASKBARPRIV_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "taskbarpriv.h"
#endif

#include "wx/toplevel.h"
#include "wx/bitmap.h"

class WXDLLIMPEXP_ADV wxTaskBarIconAreaBase : public wxTopLevelWindow
{
public:
    wxTaskBarIconAreaBase();

    // Returns true if SYSTRAY protocol is supported by the desktop
    bool IsProtocolSupported();
    
    wxEvtHandler *m_invokingWindow;
    
#if wxUSE_MENUS_NATIVE
    virtual bool DoPopupMenu( wxMenu *menu, int x, int y );
#endif // wxUSE_MENUS_NATIVE
};

#endif // _WX_TASKBARPRIV_H_
