/////////////////////////////////////////////////////////////////////////////
// Name:        wx/timer.h
// Purpose:     wxTimer class and global time-related functions
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TIMER_H_BASE_
#define _WX_TIMER_H_BASE_

#ifdef __GNUG__
    #pragma interface "timerbase.h"
#endif

#include "wx/object.h"

// ----------------------------------------------------------------------------
// wxTimer
// ----------------------------------------------------------------------------

// the interface of wxTimer class
class WXDLLEXPORT wxTimerBase : public wxObject
{
public:
    wxTimerBase() { m_oneShot = FALSE; m_milli = 0; }

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

    // override this in your wxTimer-derived class
    virtual void Notify() = 0;

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
    int     m_milli;        // the timer interval
    bool    m_oneShot;      // TRUE if one shot
};

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
// wxStopWatch
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxStopWatch
{
public: 
    // ctor starts the stop watch
    wxStopWatch() { Start(); }

    void Start(long t = 0); // (re)start it t milliseconds ago
    inline void Pause();
    void Resume() { Start(m_pause); }

    // get the elapsed time since the last Start() or Pause() in milliseconds
    long Time() const;

protected:
    // returns the elapsed time since t0
    inline long GetElapsedTime() const;
    
private:
    long m_t0;          // the time of the last Start()
    long m_pause;       // the time of the last Pause() or 0
};

// the old name
#ifdef WXWIN_COMPATIBILITY_2
    typedef wxStopWatch wxChrono;
#endif // WXWIN_COMPATIBILITY_2

// ----------------------------------------------------------------------------
// global time functions
// ----------------------------------------------------------------------------

// Timer functions (milliseconds) -- use wxStopWatch instead
void WXDLLEXPORT wxStartTimer();

// Gets time since last wxStartTimer or wxGetElapsedTime -- use wxStopWatch
// instead
long WXDLLEXPORT wxGetElapsedTime(bool resetTimer = TRUE);

// Get the local time
bool WXDLLEXPORT wxGetLocalTime(long *timeZone, int *dstObserved);

// Get number of seconds since 00:00:00 GMT, Jan 1st 1970.
long WXDLLEXPORT wxGetCurrentTime();

// Get number of milliseconds since 00:00:00 GMT, Jan 1st 1970.
long WXDLLEXPORT wxGetCurrentMTime();

// ----------------------------------------------------------------------------
// inline functions
// ----------------------------------------------------------------------------

inline long wxStopWatch::GetElapsedTime() const
{
    return wxGetCurrentMTime() - m_t0;
}

inline void wxStopWatch::Pause()
{
    m_pause = GetElapsedTime();
}

#endif
    // _WX_TIMER_H_BASE_
