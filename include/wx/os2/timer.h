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

class WXDLLEXPORT wxTimer: public wxTimerBase
{
friend void wxProcessTimer(wxTimer& timer);

public:
    wxTimer();
    ~wxTimer();

    virtual bool Start( int  nMilliseconds = -1
                       ,bool bOneShot = FALSE
                      );
    virtual void Stop(void);

    inline virtual bool IsRunning(void) const { return m_ulId != 0L; }

protected:
    ULONG                            m_ulId;
    HAB                              m_Hab;

private:
    DECLARE_ABSTRACT_CLASS(wxTimer)
};

#endif
    // _WX_TIMER_H_
