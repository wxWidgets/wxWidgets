/////////////////////////////////////////////////////////////////////////
// File:        wx/mac/classic/taskbarosx.h
// Purpose:     Defines wxTaskBarIcon class for OSX
// Author:      Ryan Norton
// Modified by:
// Created:     04/04/2003
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton, 2003
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////

#ifndef _TASKBAR_H_
#define _TASKBAR_H_

class WXDLLEXPORT wxIcon;
class WXDLLEXPORT wxMenu;

class WXDLLEXPORT wxTaskBarIcon : public wxTaskBarIconBase
{
public:
    //type of taskbar item to create (currently only DOCK is implemented)
    enum wxTaskBarIconType
    {
        DOCK,
        STATUSITEM,
        MENUEXTRA
    };

    wxTaskBarIcon(const wxTaskBarIconType& nType = DOCK);
    virtual ~wxTaskBarIcon();

    // Operations:
    bool SetIcon(const wxIcon& icon, const wxString& tooltip = wxEmptyString);
    bool RemoveIcon();
    bool PopupMenu(wxMenu *menu);

protected:
    wxTaskBarIconType m_nType;

    DECLARE_DYNAMIC_CLASS(wxTaskBarIcon)
};

#endif
    // _TASKBAR_H_
