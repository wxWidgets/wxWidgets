/////////////////////////////////////////////////////////////////////////////
// Name:        power.h
// Purpose:     documentation for wxPowerEvent class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxPowerEvent
    @wxheader{power.h}

    The power events are generated when the system power state changes, e.g. the
    system is suspended, hibernated, plugged into or unplugged from the wall socket
    and so on.

    Notice that currently only suspend and resume events are generated and only
    under MS Windows platform. To avoid the need to change the code using this
    event later when these events are implemented on the other platforms please use
    the test @c ifdef wxHAS_POWER_EVENTS instead of directly testing for
    the platform in your code: this symbol will be defined for all platforms
    supporting the power events.

    @library{wxbase}
    @category{FIXME}

    @seealso
    wxGetPowerType, wxGetBatteryState
*/
class wxPowerEvent : public wxEvent
{
public:
    /**
        Call this to prevent suspend from taking place in
        @c wxEVT_POWER_SUSPENDING handler (it is ignored for all the others).
    */
    void Veto();
};
