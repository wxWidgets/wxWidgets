/////////////////////////////////////////////////////////////////////////////
// Name:        _taskbar.i
// Purpose:     SWIG interface defs for wxTaskBarIcon
//
// Author:      Robin Dunn
//
// Created:     2-June-1998
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

%{
%}

//---------------------------------------------------------------------------
%newgroup;


%{
#ifdef __WXMAC__
// implement dummy classes and such for wxMac

class wxTaskBarIcon : public wxEvtHandler
{
public:
    wxTaskBarIcon()  { PyErr_SetNone(PyExc_NotImplementedError); }
};
 

class wxTaskBarIconEvent : public wxEvent
{
public:
    wxTaskBarIconEvent(wxEventType, wxTaskBarIcon *)
        { PyErr_SetNone(PyExc_NotImplementedError); }
};

enum {
    wxEVT_TASKBAR_MOVE = 0,
    wxEVT_TASKBAR_LEFT_DOWN = 0,
    wxEVT_TASKBAR_LEFT_UP = 0,
    wxEVT_TASKBAR_RIGHT_DOWN = 0,
    wxEVT_TASKBAR_RIGHT_UP = 0,
    wxEVT_TASKBAR_LEFT_DCLICK = 0,
    wxEVT_TASKBAR_RIGHT_DCLICK = 0,
};
#endif
%}




class wxTaskBarIcon : public wxEvtHandler
{
public:
    wxTaskBarIcon();
    ~wxTaskBarIcon();
   

#ifndef __WXMAC__
    bool IsOk() const;
    bool IsIconInstalled() const;

    bool SetIcon(const wxIcon& icon, const wxString& tooltip = wxPyEmptyString);
    bool RemoveIcon(void);
    bool PopupMenu(wxMenu *menu);
#endif
};



class wxTaskBarIconEvent : public wxEvent
{
public:
    wxTaskBarIconEvent(wxEventType evtType, wxTaskBarIcon *tbIcon);
};


enum {
    wxEVT_TASKBAR_MOVE,
    wxEVT_TASKBAR_LEFT_DOWN,
    wxEVT_TASKBAR_LEFT_UP,
    wxEVT_TASKBAR_RIGHT_DOWN,
    wxEVT_TASKBAR_RIGHT_UP,
    wxEVT_TASKBAR_LEFT_DCLICK,
    wxEVT_TASKBAR_RIGHT_DCLICK,
};

%pythoncode {
EVT_TASKBAR_MOVE = wx.PyEventBinder (         wxEVT_TASKBAR_MOVE )
EVT_TASKBAR_LEFT_DOWN = wx.PyEventBinder (    wxEVT_TASKBAR_LEFT_DOWN )
EVT_TASKBAR_LEFT_UP = wx.PyEventBinder (      wxEVT_TASKBAR_LEFT_UP )
EVT_TASKBAR_RIGHT_DOWN = wx.PyEventBinder (   wxEVT_TASKBAR_RIGHT_DOWN )
EVT_TASKBAR_RIGHT_UP = wx.PyEventBinder (     wxEVT_TASKBAR_RIGHT_UP )
EVT_TASKBAR_LEFT_DCLICK = wx.PyEventBinder (  wxEVT_TASKBAR_LEFT_DCLICK )
EVT_TASKBAR_RIGHT_DCLICK = wx.PyEventBinder ( wxEVT_TASKBAR_RIGHT_DCLICK )
}   

//---------------------------------------------------------------------------
