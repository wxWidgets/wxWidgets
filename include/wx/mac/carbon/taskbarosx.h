/////////////////////////////////////////////////////////////////////////
// File:        wx/mac/taskbarosx.h
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
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxTaskBarIcon)
public:
        // type of taskbar item to create (currently only DOCK is implemented)
        enum wxTaskBarIconType
        {
            DOCK
//    ,   CUSTOM_STATUSITEM
//    ,   STATUSITEM 
//    ,   MENUEXTRA 
        ,   DEFAULT_TYPE = DOCK
        };

    wxTaskBarIcon(wxTaskBarIconType iconType = DEFAULT_TYPE);
    virtual ~wxTaskBarIcon();

    bool IsOk() const { return true; }

    bool IsIconInstalled() const;
    bool SetIcon(const wxIcon& icon, const wxString& tooltip = wxEmptyString);
    bool RemoveIcon();
    bool PopupMenu(wxMenu *menu);

protected:
    class wxTaskBarIconImpl* m_impl;    
    friend class wxTaskBarIconImpl;
};
#endif
    // _TASKBAR_H_
