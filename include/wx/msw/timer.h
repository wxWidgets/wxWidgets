/////////////////////////////////////////////////////////////////////////////
// Name:        timer.h
// Purpose:     wxTimer class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TIMER_H_
#define _WX_TIMER_H_

#ifdef __GNUG__
#pragma interface "timer.h"
#endif

#include "wx/object.h"

class WXDLLEXPORT wxTimer : public wxObject
{
public:
    wxTimer();
    ~wxTimer();

    virtual bool Start(int milliseconds = -1,
                       bool one_shot = FALSE); // Start timer
    virtual void Stop();                       // Stop timer

    virtual void Notify() = 0;                 // Override this member

    // Returns the current interval time (0 if stop)
    int Interval() const { return milli; };
    bool OneShot() const { return oneShot; }

public:
    bool oneShot ;
    int  milli ;
    int  lastMilli ;

    long id;

private:
    DECLARE_ABSTRACT_CLASS(wxTimer)
};

// Timer functions (milliseconds)
void WXDLLEXPORT wxStartTimer();
// Gets time since last wxStartTimer or wxGetElapsedTime
long WXDLLEXPORT wxGetElapsedTime(bool resetTimer = TRUE);

// EXPERIMENTAL: comment this out if it doesn't compile.
bool WXDLLEXPORT wxGetLocalTime(long *timeZone, int *dstObserved);

// Get number of seconds since 00:00:00 GMT, Jan 1st 1970.
long WXDLLEXPORT wxGetCurrentTime();

#endif
    // _WX_TIMERH_
