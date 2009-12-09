/////////////////////////////////////////////////////////////////////////////
// Name:        wx/symbian/timer.h
// Purpose:     wxTimer class
// Author:      Andrei Matuk
// RCS-ID:      $Id$
// Copyright:   (c) 2009 Andrei Matuk <Veon.UA@gmail.com>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TIMER_H_
#define _WX_TIMER_H_

#if wxUSE_TIMER
#include "e32std.h"
#include "e32base.h"
#include "wx/private/timer.h"

class WXDLLEXPORT wxSymbianTimerImpl : public wxTimerImpl, public CTimer
{
public:
    wxSymbianTimerImpl(wxTimer *timer) : wxTimerImpl(timer), CTimer(EPriorityStandard)  { Init(); }
    virtual ~wxSymbianTimerImpl();

    virtual bool Start(int milliseconds = -1, bool oneShot = FALSE);
    virtual void Stop();

    virtual bool IsRunning() const { return CActive::IsActive(); }

protected:
    void Init();
    void RunL();        // Callback from Symbian when event signalled
};

#endif //  wxUSE_TIMER
#endif // _WX_TIMERH_
