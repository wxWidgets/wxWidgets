/////////////////////////////////////////////////////////////////////////
// File:        taskbar.h
// Purpose:	    Defines wxTaskBarIcon class for manipulating icons on the
//              task bar. Optional.
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////

#ifndef _WX_TASKBAR_H_
#define _WX_TASKBAR_H_

#ifdef __GNUG__
#pragma interface "taskbar.h"
#endif

#include <wx/list.h>
#include <wx/icon.h>

class wxTaskBarIcon: public wxObject
{
public:
	wxTaskBarIcon();
	virtual ~wxTaskBarIcon();

// Accessors
/* TODO: implementation
    inline WXHWND GetHWND() const { return m_hWnd; }
    inline bool IsOK() const { return (m_hWnd != 0) ; }
    inline bool IsIconInstalled() const { return m_iconAdded; }
*/

// Operations
    bool SetIcon(const wxIcon& icon, const wxString& tooltip = "");
    bool RemoveIcon();

// Overridables
    virtual void OnMouseMove();
    virtual void OnLButtonDown();
    virtual void OnLButtonUp();
    virtual void OnRButtonDown();
    virtual void OnRButtonUp();
    virtual void OnLButtonDClick();
    virtual void OnRButtonDClick();

/* TODO
// Implementation
    static wxTaskBarIcon* FindObjectForHWND(WXHWND hWnd);
    static void AddObject(wxTaskBarIcon* obj);
    static void RemoveObject(wxTaskBarIcon* obj);
    static bool RegisterWindowClass();
    static WXHWND CreateTaskBarWindow();
    long WindowProc( WXHWND hWnd, unsigned int msg, unsigned int wParam, long lParam );
*/

// Data members
protected:
/* TODO: implementation
    WXHWND          m_hWnd;
    static bool     sm_registeredClass;
    static unsigned int sm_taskbarMsg;
    static wxList   sm_taskBarIcons;
*/

    bool            m_iconAdded;
};

#endif
    // _WX_TASKBAR_H_
