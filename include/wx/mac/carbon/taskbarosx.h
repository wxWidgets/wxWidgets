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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "taskbarosx.h"
#endif

class WXDLLEXPORT wxIcon;
class WXDLLEXPORT wxMenu;

class WXDLLEXPORT wxTaskBarIcon : public wxTaskBarIconBase
{
public:
        //type of taskbar item to create 
    //TODO:  currently only DOCK is implemented
        enum wxTaskBarIconType
        {
                DOCK,
                STATUSITEM,
                MENUEXTRA
        };
        
    wxTaskBarIcon(const wxTaskBarIconType& nType = DOCK);
    virtual ~wxTaskBarIcon();

    inline bool IsOk() const { return true; }
    inline bool IsIconInstalled() const { return m_iconAdded; }
    
    //TODO: not tested extensively
    bool SetIcon(const wxIcon& icon, const wxString& tooltip = wxEmptyString);
    bool RemoveIcon();
    //TODO: end not tested extensively
    
    //pops up the menu
    bool PopupMenu(wxMenu *menu);

    //internal functions - don't call       
    wxMenu* GetCurrentMenu();
    wxMenu* DoCreatePopupMenu();

protected:
    wxTaskBarIconType m_nType;
    void* m_pEventHandlerRef;
    wxMenu* m_pMenu;
    WXHMENU m_theLastMenu;
    bool m_iconAdded;
            
    DECLARE_DYNAMIC_CLASS(wxTaskBarIcon)
};
#endif
    // _TASKBAR_H_
