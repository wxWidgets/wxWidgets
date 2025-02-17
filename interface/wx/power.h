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
    Possible power resources that can be locked by wxPowerResourceBlocker.

    @since 3.1.0
 */
enum wxPowerResourceKind
{
    /// Use to prevent automatic display power off.
    wxPOWER_RESOURCE_SCREEN,

    /// Use to prevent automatic system suspend.
    wxPOWER_RESOURCE_SYSTEM
};

/**
    Possible blocking behaviours for power resources.

    @since 3.3.0
 */
enum wxPowerBlockKind
{
    /**
        Prevent the resource from disappearing.

        When combined with ::wxPOWER_RESOURCE_SCREEN, this prevents the screen
        from turning off automatically. When combined with
        ::wxPOWER_RESOURCE_SYSTEM, this prevents the system from suspending.
     */
    wxPOWER_PREVENT,

    /**
        Delay suspend until the application handles the corresponding event.

        This block kind can only be used with ::wxPOWER_RESOURCE_SYSTEM.
     */
    wxPOWER_DELAY
};


/**
    @class wxPowerEvent

    The power events are generated when the system power state changes, e.g. the
    system is suspended, hibernated, plugged into or unplugged from the wall socket
    and so on. wxPowerEvents are emitted by wxWindows.

    Notice that currently these events are generated only under MSW and Linux
    and that under Linux you @e must use wxPowerResource::Acquire() to receive
    them, e.g. typically an application interested in these events should
    initialize wxPowerResourceBlocker for ::wxPOWER_RESOURCE_SYSTEM resource
    using ::wxPOWER_DELAY block kind when initializing either the application
    itself or its main window. E.g. for an application that wants to handle
    suspend to gracefully close the open network connections and then reopen
    them on resume you could do

    @code
    class MyMainFrame : public wxFrame
    {
        ...

    private:
        wxPowerResourceBlocker m_powerDelaySleep;
    };

    MyMainFrame::MyMainFrame()
        : wxFrame(...),
          m_powerDelaySleep(wxPOWER_RESOURCE_SYSTEM,
                            "MyApp needs to close network connections",
                            wxPOWER_DELAY)
    {
        Bind(wxEVT_POWER_SUSPENDED, [](wxPowerEvent&) {
            // Close network connections here.
        });

        Bind(wxEVT_POWER_RESUME, [](wxPowerEvent&) {
            // Restore previously closed network connections here.
        });
    }
    @endcode

    To avoid the need to change the code using this event later when these
    events are implemented on the other platforms please use the test <tt>ifdef
    wxHAS_POWER_EVENTS</tt> instead of directly testing for the platform in
    your code: this symbol will be defined for all platforms supporting the
    power events.

    @beginEventTable{wxPowerEvent}
    @event{EVT_POWER_SUSPENDING(func)}
           @warning This event and the possibility to veto suspend was removed
           from MSW systems starting from Windows Vista. wxPowerResourceBlocker
           can be used to prevent the system from suspending under both XP and
           later systems, use it instead of handling this event.

           System is about to be suspended, this event can be vetoed to prevent
           suspend from taking place.
    @event{EVT_POWER_SUSPENDED(func)}
           System is about to suspend: normally the application should quickly
           (i.e. without user intervention) close all the open files and network
           connections here, possibly remembering them to reopen them later when
           the system is resumed.
    @event{EVT_POWER_SUSPEND_CANCEL(func)}
           @warning This event is currently never generated.

           System suspension was cancelled because some application vetoed it.
    @event{EVT_POWER_RESUME(func)}
           System resumed from suspend: normally the application should restore
           the state in which it had been before the suspension.
    @endEventTable

    @library{wxcore}
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

/**
    Helper functions for acquiring and releasing the given power resource.

    If an application performs a long running task without user interaction it
    is often necessary to prevent the system from automatically suspending or
    powering off the screen and Acquire() method can be used to do this.

    Notice that currently this functionality is only implemented for MSW and
    macOS for ::wxPOWER_RESOURCE_SCREEN and these platforms and Linux for
    ::wxPOWER_RESOURCE_SYSTEM, however the system must be using systemd 183 or
    later for this to work there.

    If possible, use wxPowerResourceBlocker class to ensure that Release() is
    called instead of calling it manually.

    @since 3.1.0
    @library{wxcore}
    @category{misc}

    @see wxPowerResourceBlocker
*/
class wxPowerResource
{
public:
    /**
       Acquire a power resource for the application.

       The default behaviour, chosen by setting @a blockKind to
       ::wxPOWER_PREVENT, is to ensure that the resource of the corresponding
       @a kind remains accessible, i.e. prevent the system from turning off the
       screen or suspending.

       Specifying ::wxPOWER_DELAY for @a blockKind doesn't actually prevent the
       system from suspending but does delay it to allow the application to
       handle the corresponding notifications. Note that under Linux systems
       will *not* receive ::wxEVT_POWER_SUSPENDED without acquiring the system
       resource using ::wxPOWER_DELAY.

       Every call to Acquire @b must be matched by a corresponding call to
       Release() or the system will not suspend until the application ends, use
       wxPowerResourceBlocker to ensure that this happens.

       @param kind Power resource required, either ::wxPOWER_RESOURCE_SCREEN
        or ::wxPOWER_RESOURCE_SYSTEM.
       @param reason Optional reason may be specified which might be used on
           some platforms to inform the user what is preventing power saving.
           It should usually describe the operation requiring the resource and
           specifying it is strongly recommended.
       @param blockKind The default value corresponds to the blocking
           behaviour, the ::wxPOWER_DELAY value can be used to avoid blocking
           the resource but just delay it to allow the application to handle
           the corresponding notifications. This parameter is available in
           wxWidgets 3.3.0 and later.
       @return Returns true if the acquisition was successful.

       @see Release()
    */
    static bool Acquire(wxPowerResourceKind kind,
                        const wxString& reason = wxString(),
                        wxPowerBlockKind blockKind = wxPOWER_PREVENT);

    /**
        Release a previously acquired power resource.

        Release @b must be called for every Acquire() call made to restore
        normal power saving behaviour

        @param kind Power resource to be released.

        @see Acquire()
    */
    static void Release(wxPowerResourceKind kind);
};

/**
    Helper RAII class ensuring that power resources are released.

    A wxPowerResourceBlocker object acquires a power resource in the
    constructor and releases it in the destructor making it impossible to
    forget to release the power resource (which would prevent suspending or
    screen power off until the application ends).

    Example:
    @code
    void MyWindow::DoSomething()
    {
        wxPowerResourceBlocker
            blocker(wxPOWER_RESOURCE_SYSTEM, "Downloading something important");

        if ( !blocker.IsInEffect() )
        {
            // If the resource could not be acquired, tell the user that he has
            // to keep the system alive
            wxLogMessage("Warning: system may suspend while downloading.");
        }

        // Run an important download and the system will not suspend while downloading
        for ( int i = 0; i < download.size(); ++i )
            download.readByte();

        // wxPOWER_RESOURCE_SYSTEM automatically released here.
    }
    @endcode

    @since 3.1.0
    @library{wxcore}
    @category{misc}

    @see wxPowerResource
*/
class wxPowerResourceBlocker
{
public:
    /**
       Acquires the power resource.

       Uses the same parameters as wxPowerResource::Acquire().
    */
    explicit wxPowerResourceBlocker(wxPowerResourceKind kind,
                                    const wxString& reason = wxString(),
                                    wxPowerBlockKind blockKind = wxPOWER_PREVENT);

    /**
        Returns whether the power resource could be acquired.

        This can be used to inform the user that the application will not
        prevent automatic suspending.

        @see wxPowerResource::Acquire()
    */
    bool IsInEffect() const;

    /**
       Releases the power resource.

       @see wxPowerResource::Release()
    */
    ~wxPowerResourceBlocker();
};
