/////////////////////////////////////////////////////////////////////////
// File:        taskbar.h
// Purpose:	    Defines wxTaskBarIcon class for manipulating icons on the
//              Windows task bar.
// Author:      Julian Smart
// Modified by:
// Created:     24/3/98
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////

#ifndef _TASKBAR_H_
#define _TASKBAR_H_

#ifdef __GNUG__
#pragma interface "taskbar.h"
#endif

#include <wx/list.h>
#include <wx/icon.h>

class wxTaskBarIcon: public wxObject
{
public:
	wxTaskBarIcon(void);
	virtual ~wxTaskBarIcon(void);

// Accessors
    inline WXHWND GetHWND() const { return m_hWnd; }
    inline bool IsOK() const { return (m_hWnd != 0) ; }
    inline bool IsIconInstalled() const { return m_iconAdded; }

// Operations
    bool SetIcon(const wxIcon& icon, const wxString& tooltip = "");
    bool RemoveIcon(void);

// Overridables
    virtual void OnMouseMove(void);
    virtual void OnLButtonDown(void);
    virtual void OnLButtonUp(void);
    virtual void OnRButtonDown(void);
    virtual void OnRButtonUp(void);
    virtual void OnLButtonDClick(void);
    virtual void OnRButtonDClick(void);

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
    static wxList   sm_taskBarIcons;
    static bool     sm_registeredClass;
    static unsigned int sm_taskbarMsg;
};

#endif
    // _TASKBAR_H_

