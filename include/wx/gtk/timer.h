/////////////////////////////////////////////////////////////////////////////
// Name:        timer.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKTIMERH__
#define __GTKTIMERH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"

//-----------------------------------------------------------------------------
// derived classes
//-----------------------------------------------------------------------------

class wxTimer;

//-----------------------------------------------------------------------------
// global functions
//-----------------------------------------------------------------------------

/* Timer functions (milliseconds) */
void wxStartTimer();

/* Gets time since last wxStartTimer or wxGetElapsedTime */
long wxGetElapsedTime(bool resetTimer = TRUE);

/* EXPERIMENTAL: comment this out if it doesn't compile. */
bool wxGetLocalTime(long *timeZone, int *dstObserved);

/* Get number of seconds since 00:00:00 GMT, Jan 1st 1970. */
long wxGetCurrentTime();

//-----------------------------------------------------------------------------
// wxTimer
//-----------------------------------------------------------------------------

class wxTimer: public wxObject
{
public:
    wxTimer();
    ~wxTimer();

    int Interval() const { return m_time; }

    bool OneShot() const { return m_oneShot; }

    virtual bool Start( int millisecs = -1, bool oneShot = FALSE );
    virtual void Stop();

    virtual void Notify() = 0;  // override this

protected:
    int  m_tag;
    int  m_time;
    bool m_oneShot;

private:
    DECLARE_ABSTRACT_CLASS(wxTimer)
};

#endif // __GTKTIMERH__
