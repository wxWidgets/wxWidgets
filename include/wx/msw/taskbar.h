/////////////////////////////////////////////////////////////////////////
// File:        wx/msw/taskbar.h
// Purpose:     Defines wxTaskBarIcon class for manipulating icons on the
//              Windows task bar.
// Author:      Julian Smart
// Modified by: Vaclav Slavik
// Created:     24/3/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////

#ifndef _WX_TASKBAR_H_
#define _WX_TASKBAR_H_

#include "wx/icon.h"

// private helper class:
class WXDLLIMPEXP_FWD_ADV wxTaskBarIconWindow;

class WXDLLIMPEXP_ADV wxTaskBarIcon : public wxTaskBarIconBase
{
public:
    wxTaskBarIcon();
    virtual ~wxTaskBarIcon();

    // Accessors
    bool IsOk() const { return true; }
    bool IsIconInstalled() const { return m_iconAdded; }

    // Operations
    bool SetIcon(const wxIcon& icon, const wxString& tooltip = wxEmptyString);
    bool RemoveIcon(void);
    bool PopupMenu(wxMenu *menu);

protected:
    friend class wxTaskBarIconWindow;

    long WindowProc(unsigned int msg, unsigned int wParam, long lParam);
    void RegisterWindowMessages();


    wxTaskBarIconWindow *m_win;
    bool                 m_iconAdded;
    wxIcon               m_icon;
    wxString             m_strTooltip;

    DECLARE_DYNAMIC_CLASS_NO_COPY(wxTaskBarIcon)
};

#endif // _WX_TASKBAR_H_
