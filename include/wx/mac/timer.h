/////////////////////////////////////////////////////////////////////////////
// Name:        timer.h
// Purpose:     wxTimer class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TIMER_H_
#define _WX_TIMER_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "timer.h"
#endif

#include "wx/object.h"
#include "wx/mac/macnotfy.h"

class wxTimer ;

struct MacTimerInfo ;

class WXDLLEXPORT wxTimer: public wxTimerBase
{
public:
	wxTimer() { Init(); }
	wxTimer(wxEvtHandler *owner, int id = -1) : wxTimerBase(owner, id) { Init(); }
    ~wxTimer();

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
