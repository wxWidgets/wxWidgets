/////////////////////////////////////////////////////////////////////////////
// Name:        timer.h
// Purpose:     wxTimer class
// Author:      David Webster
// Modified by:
// Created:     10/17/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TIMER_H_
#define _WX_TIMER_H_

#include "wx/object.h"

class WXDLLEXPORT wxTimer: public wxObject
{
friend void wxProcessTimer(wxTimer& timer);

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

protected:
    bool oneShot ;
    int  milli ;
    int  lastMilli ;

    long id;

private:
    DECLARE_ABSTRACT_CLASS(wxTimer)
};

#endif
    // _WX_TIMER_H_
