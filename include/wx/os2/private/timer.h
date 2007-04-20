/////////////////////////////////////////////////////////////////////////////
// Name:        wx/os2/private/timer.h
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

#include "wx/private/timerimpl.h"

class WXDLLEXPORT wxOS2TimerImpl: public wxTimerImpl
{
friend void wxProcessTimer(wxOS2TimerImpl& timer);

public:
    wxOS2TimerImpl(wxTimer *timer) : wxTimerImpl(timer) { Init(); }
    virtual ~wxOS2TimerImpl();

    virtual void Notify(void);
    virtual bool Start( int  nMilliseconds = -1
                       ,bool bOneShot = FALSE
                      );
    virtual void Stop(void);

    inline virtual bool IsRunning(void) const { return m_ulId != 0L; }
    inline          int GetTimerId(void) const { return m_idTimer; }

protected:
    void Init(void);

    ULONG                            m_ulId;
    HAB                              m_Hab;
};

extern ULONG wxTimerProc( HWND  WXUNUSED(hwnd)
                         ,ULONG
                         ,int   nIdTimer
                         ,ULONG
                        );
#endif
    // _WX_TIMER_H_
