/////////////////////////////////////////////////////////////////////////
// File:        taskbar.h
// Purpose:	    Defines wxTaskBarIcon class for manipulating icons on the
//              task bar. Optional.
// Author:      David Webster
// Modified by:
// Created:     10/17/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////

#ifndef _WX_TASKBAR_H_
#define _WX_TASKBAR_H_

#include <wx/event.h>
#include <wx/list.h>
#include <wx/icon.h>

class WXDLLEXPORT wxTaskBarIcon: public wxEvtHandler {
    DECLARE_DYNAMIC_CLASS(wxTaskBarIcon)
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
    bool PopupMenu(wxMenu *menu); //, int x, int y);

// Overridables
    virtual void OnMouseMove(wxEvent&);
    virtual void OnLButtonDown(wxEvent&);
    virtual void OnLButtonUp(wxEvent&);
    virtual void OnRButtonDown(wxEvent&);
    virtual void OnRButtonUp(wxEvent&);
    virtual void OnLButtonDClick(wxEvent&);
    virtual void OnRButtonDClick(wxEvent&);

// Implementation
    static wxTaskBarIcon* FindObjectForHWND(WXHWND hWnd);
    static void AddObject(wxTaskBarIcon* obj);
    static void RemoveObject(wxTaskBarIcon* obj);
    static bool RegisterWindowClass();
    static WXHWND CreateTaskBarWindow();
    MRESULT WindowProc( WXHWND hWnd, UINT msg, MPARAM wParam, MPARAM lParam );

// Data members
protected:
    WXHWND          m_hWnd;
    bool            m_iconAdded;
    static wxList   sm_taskBarIcons;
    static bool     sm_registeredClass;
    static unsigned int sm_taskbarMsg;

    // non-virtual default event handlers to forward events to the virtuals
    void _OnMouseMove(wxEvent&);
    void _OnLButtonDown(wxEvent&);
    void _OnLButtonUp(wxEvent&);
    void _OnRButtonDown(wxEvent&);
    void _OnRButtonUp(wxEvent&);
    void _OnLButtonDClick(wxEvent&);
    void _OnRButtonDClick(wxEvent&);

    DECLARE_EVENT_TABLE()
};

const wxEventType wxEVT_TASKBAR_MOVE =                  wxEVT_FIRST + 1550;
const wxEventType wxEVT_TASKBAR_LEFT_DOWN =             wxEVT_FIRST + 1551;
const wxEventType wxEVT_TASKBAR_LEFT_UP =               wxEVT_FIRST + 1552;
const wxEventType wxEVT_TASKBAR_RIGHT_DOWN =            wxEVT_FIRST + 1553;
const wxEventType wxEVT_TASKBAR_RIGHT_UP =              wxEVT_FIRST + 1554;
const wxEventType wxEVT_TASKBAR_LEFT_DCLICK =           wxEVT_FIRST + 1555;
const wxEventType wxEVT_TASKBAR_RIGHT_DCLICK =          wxEVT_FIRST + 1556;

#define EVT_TASKBAR_MOVE(fn)         wxEventTableEntry(wxEVT_TASKBAR_MOVE, -1, -1, (wxObjectEventFunction) (wxEventFunction) &fn, NULL),
#define EVT_TASKBAR_LEFT_DOWN(fn)    wxEventTableEntry(wxEVT_TASKBAR_LEFT_DOWN, -1, -1, (wxObjectEventFunction) (wxEventFunction) &fn, NULL),
#define EVT_TASKBAR_LEFT_UP(fn)      wxEventTableEntry(wxEVT_TASKBAR_LEFT_UP, -1, -1, (wxObjectEventFunction) (wxEventFunction) &fn, NULL),
#define EVT_TASKBAR_RIGHT_DOWN(fn)   wxEventTableEntry(wxEVT_TASKBAR_RIGHT_DOWN, -1, -1, (wxObjectEventFunction) (wxEventFunction) &fn, NULL),
#define EVT_TASKBAR_RIGHT_UP(fn)     wxEventTableEntry(wxEVT_TASKBAR_RIGHT_UP, -1, -1, (wxObjectEventFunction) (wxEventFunction) &fn, NULL),
#define EVT_TASKBAR_LEFT_DCLICK(fn)  wxEventTableEntry(wxEVT_TASKBAR_LEFT_DCLICK, -1, -1, (wxObjectEventFunction) (wxEventFunction) &fn, NULL),
#define EVT_TASKBAR_RIGHT_DCLICK(fn) wxEventTableEntry(wxEVT_TASKBAR_RIGHT_DCLICK, -1, -1, (wxObjectEventFunction) (wxEventFunction) &fn, NULL),


#endif
    // _TASKBAR_H_

