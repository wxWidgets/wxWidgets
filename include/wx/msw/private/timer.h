/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/timer.h
// Purpose:     wxTimer class
// Author:      Julian Smart
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_TIMER_H_
#define _WX_MSW_PRIVATE_TIMER_H_

#if wxUSE_TIMER

#include "wx/private/timer.h"
#include "wx/msw/wrapwin.h"     // for WPARAM

class WXDLLIMPEXP_BASE wxMSWTimerImpl : public wxTimerImpl
{
public:
    wxMSWTimerImpl(wxTimer *timer) : wxTimerImpl(timer) { m_id = 0; }

    virtual bool Start(int milliseconds = -1, bool oneShot = false) override;
    virtual void Stop() override;

    virtual bool IsRunning() const override { return m_id != 0; }

#if defined(__WXWINUI__) && wxUSE_WINUI3
    // Implementation-only oracle used by the WinUI hybrid-control lifetime
    // tests. wxTimer::GetId() is the wx event id and is deliberately distinct
    // from the USER32 timer identity stored in WM_TIMER::wParam.
    WPARAM GetNativeIdForTesting() const { return m_id; }
#endif

protected:
    // this must be 64 bit under Win64 as WPARAM (storing timer ids) is 64 bit
    // there and so the ids may possibly not fit in 32 bits
    WPARAM m_id;
};

#endif // wxUSE_TIMER

#endif // _WX_TIMERH_
