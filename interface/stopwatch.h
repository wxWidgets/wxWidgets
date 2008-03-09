/////////////////////////////////////////////////////////////////////////////
// Name:        stopwatch.h
// Purpose:     documentation for wxStopWatch class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxStopWatch
    @wxheader{stopwatch.h}

    The wxStopWatch class allow you to measure time intervals. For example, you may
    use it to measure the time elapsed by some function:

    @code
    wxStopWatch sw;
        CallLongRunningFunction();
        wxLogMessage("The long running function took %ldms to execute",
                     sw.Time());
        sw.Pause();
        ... stopwatch is stopped now ...
        sw.Resume();
        CallLongRunningFunction();
        wxLogMessage("And calling it twice took $ldms in all", sw.Time());
    @endcode

    @library{wxbase}
    @category{misc}

    @seealso
    wxTimer
*/
class wxStopWatch
{
public:
    /**
        Constructor. This starts the stop watch.
    */
    wxStopWatch();

    /**
        Pauses the stop watch. Call Resume() to resume
        time measuring again.
        If this method is called several times, @c Resume() must be called the same
        number of times to really resume the stop watch. You may, however, call
        Start() to resume it unconditionally.
    */
    void Pause();

    /**
        Resumes the stop watch which had been paused with
        Pause().
    */
    void Resume();

    /**
        (Re)starts the stop watch with a given initial value.
    */
    void Start(long milliseconds = 0);

    /**
        Returns the time in milliseconds since the start (or restart) or the last call
        of
        Pause().
    */
    long Time() const;
};


// ============================================================================
// Global functions/macros
// ============================================================================

/**
    Returns the number of seconds since local time 00:00:00 Jan 1st 1970.

    @see wxDateTime::Now
*/
long wxGetLocalTime();

/**
    Returns the number of seconds since GMT 00:00:00 Jan 1st 1970.

    @see wxDateTime::Now
*/
long wxGetUTCTime();

/**
    Returns the number of milliseconds since local time 00:00:00 Jan 1st 1970.

    @see wxDateTime::Now, wxLongLong
*/
wxLongLong wxGetLocalTimeMillis();

