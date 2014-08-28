///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/scrtimer.h
// Purpose:     wxScrollTimer: small helper class for wxScrollArrow/Thumb
// Author:      Vadim Zeitlin
// Modified by:
// Created:     18.02.01
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_SCRTIMER_H
#define _WX_UNIV_SCRTIMER_H

// NB: this class is implemented in scrolbar.cpp

#include "wx/defs.h"

#if wxUSE_TIMER

#include "wx/timer.h"

// ----------------------------------------------------------------------------
// wxScrollTimer: the timer used when the arrow or scrollbar shaft is kept
// pressed
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxScrollTimer : public wxTimer
{
public:
    // default ctor
    wxScrollTimer();

    // start generating the events
    void StartAutoScroll();

    // the base class method
    virtual void Notify() wxOVERRIDE;

protected:
    // to implement in derived classes: perform the scroll action and return
    // true to continue scrolling or false to stop
    virtual bool DoNotify() = 0;

    // should we skip the next timer event?
    bool m_skipNext;
};

#endif // wxUSE_TIMER

#endif // _WX_UNIV_SCRTIMER_H
