/////////////////////////////////////////////////////////////////////////////
// Name:        wx/timer.h
// Purpose:     wxTimer, wxStopWatch and global time-related functions
// Author:      Julian Smart (wxTimer), Sylvain Bougnoux (wxStopWatch)
// Modified by: Vadim Zeitlin (wxTimerBase)
//              Guillermo Rodriguez (global clean up)
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TIMER_H_BASE_
#define _WX_TIMER_H_BASE_

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "timerbase.h"
#endif

#include "wx/setup.h"
#include "wx/object.h"
#include "wx/longlong.h"
#include "wx/event.h"

#if wxUSE_GUI && wxUSE_TIMER

// ----------------------------------------------------------------------------
// wxTimer
// ----------------------------------------------------------------------------

// more readable flags for Start():

// generate notifications periodically until the timer is stopped (default)
#define wxTIMER_CONTINUOUS FALSE

// only send the notification once and then stop the timer
#define wxTIMER_ONE_SHOT TRUE

// the interface of wxTimer class
class WXDLLEXPORT wxTimerBase : public wxEvtHandler
{
public:
    // ctors and initializers
    // ----------------------

    // default: if you don't call SetOwner(), your only chance to get timer
    // notifications is to override Notify() in the derived class
    wxTimerBase() { Init(); SetOwner(this); }

    // ctor which allows to avoid having to override Notify() in the derived
    // class: the owner will get timer notifications which can be handled with
    // EVT_TIMER
    wxTimerBase(wxEvtHandler *owner, int id = -1)
        { Init(); SetOwner(owner, id); }

    // same as ctor above
    void SetOwner(wxEvtHandler *owner, int id = -1)
        { m_owner = owner; m_idTimer = id; }

    virtual ~wxTimerBase();

    // working with the timer
    // ----------------------

    // start the timer: if milliseconds == -1, use the same value as for the
    // last Start()
    //
    // it is now valid to call Start() multiple times: this just restarts the
    // timer if it is already running
    virtual bool Start(int milliseconds = -1, bool oneShot = FALSE);

    // stop the timer
    virtual void Stop() = 0;

    // override this in your wxTimer-derived class if you want to process timer
    // messages in it, use non default ctor or SetOwner() otherwise
    virtual void Notify();

    // getting info
    // ------------

    // return TRUE if the timer is running
    virtual bool IsRunning() const = 0;

    // get the (last) timer interval in the milliseconds
    int GetInterval() const { return m_milli; }

    // return TRUE if the timer is one shot
    bool IsOneShot() const { return m_oneShot; }

#if WXWIN_COMPATIBILITY_2
    // deprecated functions
    int Interval() const { return GetInterval(); };
    bool OneShot() const { return IsOneShot(); }
#endif // WXWIN_COMPATIBILITY_2

protected:
    // common part of all ctors
    void Init() { m_oneShot = FALSE; m_milli = 0; }

    wxEvtHandler *m_owner;
    int     m_idTimer;

    int     m_milli;        // the timer interval
    bool    m_oneShot;      // TRUE if one shot

    DECLARE_NO_COPY_CLASS(wxTimerBase)
};

// ----------------------------------------------------------------------------
// wxTimer itself
// ----------------------------------------------------------------------------

#if defined(__WXMSW__)
    #include "wx/msw/timer.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/timer.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/timer.h"
#elif defined(__WXX11__) || defined(__WXMGL__) || defined(__WXCOCOA__)
    #include "wx/generic/timer.h"
#elif defined(__WXMAC__)
    #include "wx/mac/timer.h"
#elif defined(__WXPM__)
    #include "wx/os2/timer.h"
#endif

// ----------------------------------------------------------------------------
// wxTimerRunner: starts the timer in its ctor, stops in the dtor
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxTimerRunner
{
public:
    wxTimerRunner(wxTimer& timer) : m_timer(timer) { }
    wxTimerRunner(wxTimer& timer, int milli, bool oneShot = FALSE)
        : m_timer(timer)
    {
        m_timer.Start(milli, oneShot);
    }

    void Start(int milli, bool oneShot = FALSE)
    {
        m_timer.Start(milli, oneShot);
    }

    ~wxTimerRunner()
    {
        if ( m_timer.IsRunning() )
        {
            m_timer.Stop();
        }
    }

private:
    wxTimer& m_timer;
};

// ----------------------------------------------------------------------------
// wxTimerEvent
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxTimerEvent : public wxEvent
{
public:
    wxTimerEvent(int id = 0, int interval = 0) : wxEvent(id)
    {
        m_eventType = wxEVT_TIMER;

        m_interval = interval;
    }

    // accessors
    int GetInterval() const { return m_interval; }

    // implement the base class pure virtual
    virtual wxEvent *Clone() const { return new wxTimerEvent(*this); }

private:
    int m_interval;

    DECLARE_DYNAMIC_CLASS(wxTimerEvent)
};

typedef void (wxEvtHandler::*wxTimerEventFunction)(wxTimerEvent&);

#define EVT_TIMER(id, func) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_TIMER, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTimerEventFunction) & func, NULL),

#endif // wxUSE_GUI && wxUSE_TIMER

// ----------------------------------------------------------------------------
// wxStopWatch: measure time intervals with up to 1ms resolution
// ----------------------------------------------------------------------------

#if wxUSE_STOPWATCH

class WXDLLEXPORT wxStopWatch
{
public:
    // ctor starts the stop watch
    wxStopWatch() { m_pauseCount = 0; Start(); }

    // start the stop watch at the moment t0
    void Start(long t0 = 0);

    // pause the stop watch
    void Pause()
    {
        if ( !m_pauseCount++ )
            m_pause = GetElapsedTime();
    }

    // resume it
    void Resume()
    {
        wxASSERT_MSG( m_pauseCount > 0,
                      _T("Resuming stop watch which is not paused") );

        if ( !--m_pauseCount )
            Start(m_pause);
    }

    // get elapsed time since the last Start() in milliseconds
    long Time() const;

protected:
    // returns the elapsed time since t0
    long GetElapsedTime() const;

private:
    // the time of the last Start()
    wxLongLong m_t0;

    // the time of the last Pause() (only valid if m_pauseCount > 0)
    long m_pause;

    // if > 0, the stop watch is paused, otherwise it is running
    int m_pauseCount;
};

#endif // wxUSE_STOPWATCH

#if wxUSE_LONGLONG

// Starts a global timer
// -- DEPRECATED: use wxStopWatch instead
void WXDLLEXPORT wxStartTimer();

// Gets elapsed milliseconds since last wxStartTimer or wxGetElapsedTime
// -- DEPRECATED: use wxStopWatch instead
long WXDLLEXPORT wxGetElapsedTime(bool resetTimer = TRUE);

#endif // wxUSE_LONGLONG

// ----------------------------------------------------------------------------
// global time functions
// ----------------------------------------------------------------------------

// Get number of seconds since local time 00:00:00 Jan 1st 1970.
extern long WXDLLEXPORT wxGetLocalTime();

// Get number of seconds since GMT 00:00:00, Jan 1st 1970.
extern long WXDLLEXPORT wxGetUTCTime();

#if wxUSE_LONGLONG
// Get number of milliseconds since local time 00:00:00 Jan 1st 1970
extern wxLongLong WXDLLEXPORT wxGetLocalTimeMillis();
#endif // wxUSE_LONGLONG

#define wxGetCurrentTime() wxGetLocalTime()

#endif
    // _WX_TIMER_H_BASE_
