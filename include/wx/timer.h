/////////////////////////////////////////////////////////////////////////////
// Name:        wx/timer.h
// Purpose:     wxTimer, wxStopWatch and global time-related functions
// Author:      Julian Smart (wxTimer), Sylvain Bougnoux (wxStopWatch)
// Modified by: Vadim Zeitlin (wxTimerBase)
//              Guillermo Rodriguez (global clean up)
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TIMER_H_BASE_
#define _WX_TIMER_H_BASE_

#ifdef __GNUG__
    #pragma interface "timerbase.h"
#endif

#include "wx/setup.h"
#include "wx/object.h"
#include "wx/longlong.h"
#include "wx/event.h"

#if wxUSE_GUI

// ----------------------------------------------------------------------------
// wxTimer
// ----------------------------------------------------------------------------

// the interface of wxTimer class
class WXDLLEXPORT wxTimerBase : public wxObject
{
public:
    // ctors and initializers
    // ----------------------

    // default: if you don't call SetOwner(), your only chance to get timer
    // notifications is to override Notify() in the derived class
    wxTimerBase() { Init(); SetOwner(NULL); }

    // ctor which allows to avoid having to override Notify() in the derived
    // class: the owner will get timer notifications which can be handled with
    // EVT_TIMER
    wxTimerBase(wxEvtHandler *owner, int id = -1)
        { Init(); SetOwner(owner, id); }

    // same as ctor above
    void SetOwner(wxEvtHandler *owner, int id = -1)
        { m_owner = owner; m_idTimer = id; }

    // working with the timer
    // ----------------------

    // start the timer: if milliseconds == -1, use the same value as for the
    // last Start()
    virtual bool Start(int milliseconds = -1, bool oneShot = FALSE)
    {
        if ( milliseconds != -1 )
        {
            m_milli = milliseconds;
        }

        m_oneShot = oneShot;

        return TRUE;
    }


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
#elif defined(__WXQT__)
    #include "wx/qt/timer.h"
#elif defined(__WXMAC__)
    #include "wx/mac/timer.h"
#elif defined(__WXPM__)
    #include "wx/os2/timer.h"
#elif defined(__WXSTUBS__)
    #include "wx/stubs/timer.h"
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

private:
    int m_interval;

    DECLARE_DYNAMIC_CLASS(wxTimerEvent)
};

typedef void (wxEvtHandler::*wxTimerEventFunction)(wxTimerEvent&);

#define EVT_TIMER(id, func) { wxEVT_TIMER, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTimerEventFunction) & func, NULL},

#endif // wxUSE_GUI

// ----------------------------------------------------------------------------
// wxStopWatch: measure time intervals with up to 1ms resolution
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxStopWatch
{
public:
    // ctor starts the stop watch
    wxStopWatch()        { Start(); }
    void Start(long t = 0);
    void Pause()  { m_pause = GetElapsedTime(); }
    void Resume() { Start(m_pause); }

    // get elapsed time since the last Start() or Pause() in milliseconds
    long Time() const;

protected:
    // returns the elapsed time since t0
    long GetElapsedTime() const;

private:
    wxLongLong m_t0;      // the time of the last Start()
    long m_pause;         // the time of the last Pause() or 0
};


// Starts a global timer
// -- DEPRECATED: use wxStopWatch instead
void WXDLLEXPORT wxStartTimer();

// Gets elapsed milliseconds since last wxStartTimer or wxGetElapsedTime
// -- DEPRECATED: use wxStopWatch instead
long WXDLLEXPORT wxGetElapsedTime(bool resetTimer = TRUE);

// ----------------------------------------------------------------------------
// global time functions
// ----------------------------------------------------------------------------

// Get number of seconds since local time 00:00:00 Jan 1st 1970.
extern long WXDLLEXPORT wxGetLocalTime();

// Get number of seconds since GMT 00:00:00, Jan 1st 1970.
extern long WXDLLEXPORT wxGetUTCTime();

// Get number of milliseconds since local time 00:00:00 Jan 1st 1970
extern wxLongLong WXDLLEXPORT wxGetLocalTimeMillis();

#define wxGetCurrentTime() wxGetLocalTime()

#endif
    // _WX_TIMER_H_BASE_
