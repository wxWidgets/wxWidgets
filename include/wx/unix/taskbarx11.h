/////////////////////////////////////////////////////////////////////////
// File:        wx/unix/taskbarx11.h
// Purpose:     Defines wxTaskBarIcon class for most common X11 desktops
// Author:      Vaclav Slavik
// Modified by:
// Created:     04/04/2003
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik, 2003
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////

#ifndef _TASKBAR_H_
#define _TASKBAR_H_

#ifdef __GNUG__
#pragma interface "taskbarx11.h"
#endif

class WXDLLEXPORT wxIcon;
class WXDLLEXPORT wxFrame;
class WXDLLEXPORT wxWindow;

class WXDLLEXPORT wxTaskBarIcon: public wxTaskBarIconBase
{
public:
    wxTaskBarIcon();
    virtual ~wxTaskBarIcon();

    // Accessors:
    bool IsOk() const;
    bool IsIconInstalled() const;

    // Operations:
    bool SetIcon(const wxIcon& icon, const wxString& tooltip = wxEmptyString);
    bool RemoveIcon();
    bool PopupMenu(wxMenu *menu);

protected:
    wxFrame *m_iconWnd;
    wxWindow *m_iconArea;

    DECLARE_DYNAMIC_CLASS(wxTaskBarIcon)
};

#endif
    // _TASKBAR_H_
