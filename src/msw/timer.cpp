/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/timer.cpp
// Purpose:     wxTimer implementation
// Author:      Julian Smart
// Modified by: Vadim Zeitlin (use hash map instead of list, global rewrite)
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_TIMER

#include "wx/msw/private/timer.h"

#ifndef WX_PRECOMP
    #include "wx/list.h"
    #include "wx/event.h"
    #include "wx/app.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/hashmap.h"
#endif

#include "wx/msw/private.h"

// ----------------------------------------------------------------------------
// private globals
// ----------------------------------------------------------------------------

// define a hash containing all the timers: it is indexed by timer id and
// contains the corresponding timer
WX_DECLARE_HASH_MAP(unsigned long, wxMSWTimerImpl *, wxIntegerHash, wxIntegerEqual,
                    wxTimerMap);

// instead of using a global here, wrap it in a static function as otherwise it
// could have been used before being initialized if a timer object were created
// globally
static wxTimerMap& TimerMap()
{
    static wxTimerMap s_timerMap;

    return s_timerMap;
}

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// timer callback used for all timers
void WINAPI wxTimerProc(HWND hwnd, UINT msg, UINT_PTR idTimer, DWORD dwTime);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxMSWTimerImpl class
// ----------------------------------------------------------------------------

bool wxMSWTimerImpl::Start(int milliseconds, bool oneShot)
{
    if ( !wxTimerImpl::Start(milliseconds, oneShot) )
        return false;

    m_id = ::SetTimer
             (
                NULL,                       // don't use window
                1,                          // id ignored with NULL hwnd anyhow
                (UINT)m_milli,              // delay
                wxTimerProc                 // timer proc to call
             );

    if ( !m_id )
    {
        wxLogSysError(_("Couldn't create a timer"));

        return false;
    }

    // check that SetTimer() didn't reuse an existing id: according to the MSDN
    // this can happen and this would be catastrophic to us as we rely on ids
    // uniquely identifying the timers because we use them as keys in the hash
    if ( TimerMap().find(m_id) != TimerMap().end() )
    {
        wxLogError(_("Timer creation failed."));

        ::KillTimer(NULL, m_id);
        m_id = 0;

        return false;
    }

    TimerMap()[m_id] = this;

    return true;
}

void wxMSWTimerImpl::Stop()
{
    wxASSERT_MSG( m_id, _T("should be running") );

    ::KillTimer(NULL, m_id);

    TimerMap().erase(m_id);

    m_id = 0;
}

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

void wxProcessTimer(wxMSWTimerImpl& timer)
{
    wxASSERT_MSG( timer.IsRunning(), _T("bogus timer id") );

    if ( timer.IsOneShot() )
        timer.Stop();

    timer.Notify();
}

void WINAPI
wxTimerProc(HWND WXUNUSED(hwnd),
            UINT WXUNUSED(msg),
            UINT_PTR idTimer,
            DWORD WXUNUSED(dwTime))
{
    wxTimerMap::iterator node = TimerMap().find((unsigned long)idTimer);

    wxCHECK_RET( node != TimerMap().end(), wxT("bogus timer id in wxTimerProc") );

    wxProcessTimer(*(node->second));
}

#endif // wxUSE_TIMER
