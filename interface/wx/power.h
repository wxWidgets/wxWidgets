/////////////////////////////////////////////////////////////////////////////
// Name:        power.h
// Purpose:     interface of wxPowerEvent
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

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


/**
    @class wxPowerEvent

    The power events are generated when the system power state changes, e.g. the
    system is suspended, hibernated, plugged into or unplugged from the wall socket
    and so on. wxPowerEvents are emitted by wxWindows.

    Notice that currently only suspend and resume events are generated and only
    under MS Windows platform. To avoid the need to change the code using this
    event later when these events are implemented on the other platforms please
    use the test <tt>ifdef wxHAS_POWER_EVENTS</tt> instead of directly testing for
    the platform in your code: this symbol will be defined for all platforms
    supporting the power events.

    @beginEventTable{wxPowerEvent}
    @event{EVT_POWER_SUSPENDING(func)}
           System is about to be suspended, this event can be vetoed to prevent
           suspend from taking place.
    @event{EVT_POWER_SUSPENDED(func)}
           System is about to suspend: normally the application should quickly
           (i.e. without user intervention) close all the open files and network
           connections here, possibly remembering them to reopen them later when
           the system is resumed.
    @event{EVT_POWER_SUSPEND_CANCEL(func)}
           System suspension was cancelled because some application vetoed it.
    @event{EVT_POWER_RESUME(func)}
           System resumed from suspend: normally the application should restore
           the state in which it had been before the suspension.
    @endEventTable

    @library{wxbase}
    @category{events}

    @see ::wxGetPowerType(), ::wxGetBatteryState()
*/
class wxPowerEvent : public wxEvent
{
public:
    wxPowerEvent();
    wxPowerEvent(wxEventType evtType);
    
    /**
        Call this to prevent suspend from taking place in @c wxEVT_POWER_SUSPENDING
        handler (it is ignored for all the others).
    */
    void Veto();

    /**
       Returns whether Veto has been called.
    */
    bool IsVetoed() const;
};

wxEventType wxEVT_POWER_SUSPENDING;
wxEventType wxEVT_POWER_SUSPENDED;
wxEventType wxEVT_POWER_SUSPEND_CANCEL;
wxEventType wxEVT_POWER_RESUME;


