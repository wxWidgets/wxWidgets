/////////////////////////////////////////////////////////////////////////////
// Name:        timer.h
// Purpose:     wxTimer class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TIMER_H_
#define _WX_TIMER_H_

#ifdef __GNUG__
    #pragma interface "timer.h"
#endif

class WXDLLEXPORT wxTimer : public wxTimerBase
{
friend void wxProcessTimer(wxTimer& timer);

public:
    wxTimer();
    ~wxTimer();

    virtual bool Start(int milliseconds = -1, bool oneShot = FALSE);
    virtual void Stop();

    virtual bool IsRunning() const { return m_id != 0; }

protected:
    long m_id;

private:
    DECLARE_ABSTRACT_CLASS(wxTimer)
};

#endif
    // _WX_TIMERH_
