/////////////////////////////////////////////////////////////////////////////
// Name:        _power.i
// Purpose:     SWIG interface for wx poser events and functions
//
// Author:      Robin Dunn
//
// Created:     28-May-2006
// RCS-ID:      $Id$
// Copyright:   (c) 2006 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module

//---------------------------------------------------------------------------
%newgroup

%{
#include <wx/power.h>
%}



%{
#ifndef wxHAS_POWER_EVENTS
// Dummy class and other definitions for platforms that don't have them

class wxPowerEvent : public wxEvent
{
public:
    wxPowerEvent(wxEventType evtType) : wxEvent(wxID_NONE, evtType) {}
    void Veto() {}
    bool IsVetoed() const { return false; }

    virtual wxEvent *Clone() const { return new wxPowerEvent(*this); }
};

enum {
    wxEVT_POWER_SUSPENDING,
    wxEVT_POWER_SUSPENDED,
    wxEVT_POWER_SUSPEND_CANCEL,
    wxEVT_POWER_RESUME,
};

wxPowerType wxGetPowerType()       { return wxPOWER_UNKNOWN; }
wxBatteryState wxGetBatteryState() { return wxBATTERY_UNKNOWN_STATE; }

#endif
%}



enum wxPowerType
{
    wxPOWER_SOCKET,
    wxPOWER_BATTERY,
    wxPOWER_UNKNOWN
};

enum wxBatteryState
{
    wxBATTERY_NORMAL_STATE,    // system is fully usable
    wxBATTERY_LOW_STATE,       // start to worry
    wxBATTERY_CRITICAL_STATE,  // save quickly
    wxBATTERY_SHUTDOWN_STATE,  // too late
    wxBATTERY_UNKNOWN_STATE
};


DocStr(wxPowerEvent,
"wx.PowerEvent is generated when the system online status changes.
Currently this is only implemented for Windows.",
"");
class wxPowerEvent : public wxEvent
{
public:
    wxPowerEvent(wxEventType evtType);

    // Veto the operation (only makes sense with EVT_POWER_SUSPENDING)
    void Veto();
    bool IsVetoed() const;
};


%constant wxEventType wxEVT_POWER_SUSPENDING;
%constant wxEventType wxEVT_POWER_SUSPENDED;
%constant wxEventType wxEVT_POWER_SUSPEND_CANCEL;
%constant wxEventType wxEVT_POWER_RESUME;

%pythoncode {
EVT_POWER_SUSPENDING       = wx.PyEventBinder( wxEVT_POWER_SUSPENDING , 1 )
EVT_POWER_SUSPENDED        = wx.PyEventBinder( wxEVT_POWER_SUSPENDED , 1 )
EVT_POWER_SUSPEND_CANCEL   = wx.PyEventBinder( wxEVT_POWER_SUSPEND_CANCEL , 1 )
EVT_POWER_RESUME           = wx.PyEventBinder( wxEVT_POWER_RESUME , 1 )
}


DocDeclStr(
    wxPowerType , wxGetPowerType(),
    "return the current system power state: online or offline", "");


DocDeclStr(
    wxBatteryState , wxGetBatteryState(),
    "return approximate battery state", "");


//---------------------------------------------------------------------------
