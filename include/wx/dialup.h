/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dialup.h
// Purpose:     Network related wxWindows classes and functions
// Author:      Vadim Zeitlin
// Modified by:
// Created:     07.07.99
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_NET_H
#define _WX_NET_H

#if wxUSE_DIALUP_MANAGER

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

extern const wxChar *wxEmptyString;

#define WXDIALUP_MANAGER_DEFAULT_BEACONHOST  _T("www.yahoo.com")

// ----------------------------------------------------------------------------
// A class which groups functions dealing with connecting to the network from a
// workstation using dial-up access to the net. There is at most one instance
// of this class in the program accessed via GetDialUpManager().
// ----------------------------------------------------------------------------

/* TODO
 *
 * 1. more configurability for Unix: i.e. how to initiate the connection, how
 *    to check for online status, &c.
 * 2. a function to enumerate all connections (ISPs) and show a dialog in
 *    Dial() allowing to choose between them if no ISP given
 * 3. add an async version of dialing functions which notify the caller about
 *    the progress (or may be even start another thread to monitor it)
 * 4. the static creation/accessor functions are not MT-safe - but is this
 *    really crucial? I think we may suppose they're always called from the
 *    main thread?
 */

class WXDLLEXPORT wxDialUpManager
{
public:
    // this function should create and return the object of the
    // platform-specific class derived from wxDialUpManager. It's implemented
    // in the platform-specific source files.
    static wxDialUpManager *Create();

    // could the dialup manager be initialized correctly? If this function
    // returns FALSE, no other functions will work neither, so it's a good idea
    // to call this function and check its result before calling any other
    // wxDialUpManager methods
    virtual bool IsOk() const = 0;

    // virtual dtor for any base class
    virtual ~wxDialUpManager() { }

    // operations
    // ----------

    // dial the given ISP, use username and password to authentificate
    //
    // if async parameter is FALSE, the function waits until the end of dialing
    // and returns TRUE upon successful completion.
    // if async is TRUE, the function only initiates the connection and returns
    // immediately - the result is reported via events (an event is sent
    // anyhow, but if dialing failed it will be a DISCONNECTED one)
    virtual bool Dial(const wxString& nameOfISP = wxEmptyString,
                      const wxString& username = wxEmptyString,
                      const wxString& password = wxEmptyString,
                      bool async = TRUE) = 0;

    // returns TRUE if (async) dialing is in progress
    virtual bool IsDialing() const = 0;

    // cancel dialing the number initiated with Dial(async = TRUE)
    // NB: this won't result in DISCONNECTED event being sent
    virtual bool CancelDialing() = 0;

    // hang up the currently active dial up connection
    virtual bool HangUp() = 0;

    // online status
    // -------------

    // returns TRUE if the computer is connected to the network: under Windows,
    // this just means that a RAS connection exists, under Unix we check that
    // the "well-known host" (as specified by SetWellKnownHost) is reachable
    virtual bool IsOnline() const = 0;

    // sometimes the built-in logic for determining the online status may fail,
    // so, in general, the user should be allowed to override it. This function
    // allows to forcefully set the online status - whatever our internal
    // algorithm may think about it.
    virtual void SetOnlineStatus(bool isOnline = TRUE) = 0;

    // set misc wxDialUpManager options
    // --------------------------------

    // enable automatical checks for the connection status and sending of
    // wxEVT_DIALUP_CONNECTED/wxEVT_DIALUP_DISCONNECTED events. The interval
    // parameter is only for Unix where we do the check manually: under
    // Windows, the notification about the change of connection status is
    // instantenous.
    //
    // Returns FALSE if couldn't set up automatic check for online status.
    virtual bool EnableAutoCheckOnlineStatus(size_t nSeconds = 60) = 0;

    // disable automatic check for connection status change - notice that the
    // wxEVT_DIALUP_XXX events won't be sent any more neither.
    virtual void DisableAutoCheckOnlineStatus() = 0;

    // additional Unix-only configuration
    // ----------------------------------

    // under Unix, the value of well-known host is used to check whether we're
    // connected to the internet. It's unused under Windows, but this function
    // is always safe to call. The default value is www.yahoo.com.
    virtual void SetWellKnownHost(const wxString& hostname,
                                  int portno = 80) = 0;

    // Sets the commands to start up the network and to hang up again. Used by
    // the Unix implementations only.
    virtual void
    SetConnectCommand(const wxString& commandDial = _T("/usr/bin/pon"),
                      const wxString& commandHangup = _T("/usr/bin/poff")) = 0;
};

// ----------------------------------------------------------------------------
// DIALUP events processing
// ----------------------------------------------------------------------------

// the event class for the dialup events
class WXDLLEXPORT wxDialUpEvent : public wxEvent
{
public:
    wxDialUpEvent(bool isConnected, bool isOwnEvent) : wxEvent(isOwnEvent)
    {
        SetEventType(isConnected ? wxEVT_DIALUP_CONNECTED
                                 : wxEVT_DIALUP_DISCONNECTED);
    }

    // is this a CONNECTED or DISCONNECTED event?
    bool IsConnectedEvent() const
        { return GetEventType() == wxEVT_DIALUP_CONNECTED; }

    // does this event come from wxDialUpManager::Dial() or from some extrenal
    // process (i.e. does it result from our own attempt to establish the
    // connection)?
    bool IsOwnEvent() const { return m_id != 0; }
};

// the type of dialup event handler function
typedef void (wxObject::*wxDialUpEventFunction)(wxDialUpEvent&);

// macros to catch dialup events
#define EVT_DIALUP_CONNECTED(func) { wxEVT_DIALUP_CONNECTED, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxDialUpEventFunction) & func, NULL},
#define EVT_DIALUP_DISCONNECTED(func) { wxEVT_DIALUP_DISCONNECTED, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxDialUpEventFunction) & func, NULL},

#endif // wxUSE_DIALUP_MANAGER

#endif // _WX_NET_H
