/////////////////////////////////////////////////////////////////////////
// File:        wx/msw/taskbar.h
// Purpose:     Defines wxTaskBarIcon class for manipulating icons on the
//              Windows task bar.
// Author:      Julian Smart
// Modified by:
// Created:     24/3/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////

#ifndef _TASKBAR_H_
#define _TASKBAR_H_

#ifdef __GNUG__
#pragma interface "taskbar.h"
#endif

#include "wx/list.h"
#include "wx/icon.h"

class wxTaskBarIcon;

WX_DECLARE_EXPORTED_LIST(wxTaskBarIcon, wxTaskBarIconList);

class WXDLLEXPORT wxTaskBarIcon: public wxTaskBarIconBase
{
    DECLARE_DYNAMIC_CLASS(wxTaskBarIcon)
public:
    wxTaskBarIcon(void);
    virtual ~wxTaskBarIcon(void);

// Accessors
    inline WXHWND GetHWND() const { return m_hWnd; }
    inline bool IsOk() const { return (m_hWnd != 0) ; }
    inline bool IsIconInstalled() const { return m_iconAdded; }

// Operations
    bool SetIcon(const wxIcon& icon, const wxString& tooltip = wxEmptyString);
    bool RemoveIcon(void);
    bool PopupMenu(wxMenu *menu); //, int x, int y);

#if WXWIN_COMPATIBILITY_2_4
    wxDEPRECATED( bool IsOK() const );

// Overridables
    virtual void OnMouseMove(wxEvent&);
    virtual void OnLButtonDown(wxEvent&);
    virtual void OnLButtonUp(wxEvent&);
    virtual void OnRButtonDown(wxEvent&);
    virtual void OnRButtonUp(wxEvent&);
    virtual void OnLButtonDClick(wxEvent&);
    virtual void OnRButtonDClick(wxEvent&);
#endif

// Implementation
    static wxTaskBarIcon* FindObjectForHWND(WXHWND hWnd);
    static void AddObject(wxTaskBarIcon* obj);
    static void RemoveObject(wxTaskBarIcon* obj);
    static bool RegisterWindowClass();
    static WXHWND CreateTaskBarWindow();
    long WindowProc( WXHWND hWnd, unsigned int msg, unsigned int wParam, long lParam );

// Data members
protected:
    WXHWND          m_hWnd;
    bool            m_iconAdded;
    wxIcon          m_icon;
    wxString        m_strTooltip;

    static wxTaskBarIconList sm_taskBarIcons;

#if WXWIN_COMPATIBILITY_2_4
    // non-virtual default event handlers to forward events to the virtuals
    void _OnMouseMove(wxEvent&);
    void _OnLButtonDown(wxEvent&);
    void _OnLButtonUp(wxEvent&);
    void _OnRButtonDown(wxEvent&);
    void _OnRButtonUp(wxEvent&);
    void _OnLButtonDClick(wxEvent&);
    void _OnRButtonDClick(wxEvent&);

    DECLARE_EVENT_TABLE()
#endif
};

#if WXWIN_COMPATIBILITY_2_4    
inline bool wxTaskBarIcon::IsOK() const { return IsOk(); }
#endif

#endif
    // _TASKBAR_H_





