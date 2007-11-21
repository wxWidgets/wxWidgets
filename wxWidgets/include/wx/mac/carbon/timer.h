/////////////////////////////////////////////////////////////////////////////
// Name:        timer.h
// Purpose:     wxTimer class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TIMER_H_
#define _WX_TIMER_H_

#include "wx/object.h"
#include "wx/mac/macnotfy.h"

class wxTimer ;

struct MacTimerInfo ;

class WXDLLEXPORT wxTimer: public wxTimerBase
{
public:
    wxTimer() { Init(); }
    wxTimer(wxEvtHandler *owner, int id = -1) : wxTimerBase(owner, id) { Init(); }
    virtual ~wxTimer();

    virtual bool Start(int milliseconds = -1,
                       bool one_shot = FALSE); // Start timer
    virtual void Stop();                       // Stop timer

    virtual bool IsRunning() const ;

    MacTimerInfo* m_info;
protected :
    void Init();
private:

    DECLARE_ABSTRACT_CLASS(wxTimer)
};

#endif
    // _WX_TIMER_H_
