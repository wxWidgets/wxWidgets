/////////////////////////////////////////////////////////////////////////////
// Name:        msw/timer.cpp
// Purpose:     wxTimer implementation
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "timer.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_TIMER

#ifndef WX_PRECOMP
    #include "wx/window.h"
    #include "wx/list.h"
    #include "wx/event.h"
    #include "wx/app.h"
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#include "wx/hashmap.h"

#include "wx/timer.h"

#include "wx/msw/private.h"

// from utils.cpp
extern "C" WXDLLIMPEXP_BASE HWND
wxCreateHiddenWindow(LPCTSTR *pclassname, LPCTSTR classname, WNDPROC wndproc);

// ----------------------------------------------------------------------------
// private globals
// ----------------------------------------------------------------------------

// define a hash containing all the timers: it is indexed by timer id and
// contains the corresponding timer
WX_DECLARE_HASH_MAP(unsigned long, wxTimer *, wxIntegerHash, wxIntegerEqual,
                    wxTimerMap);

static wxTimerMap g_timerMap;

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

void WINAPI wxTimerProc(HWND hwnd, WORD, int idTimer, DWORD);

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxTimer, wxEvtHandler)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxTimer class
// ----------------------------------------------------------------------------

void wxTimer::Init()
{
    m_id = 0;
}

wxTimer::~wxTimer()
{
    wxTimer::Stop();
}

bool wxTimer::Start(int milliseconds, bool oneShot)
{
    (void)wxTimerBase::Start(milliseconds, oneShot);

    wxCHECK_MSG( m_milli > 0, false, wxT("invalid value for timer timeour") );

    m_id = ::SetTimer
             (
                NULL,                       // don't use window
                1,                          // id ignored with NULL hwnd anyhow
                (UINT)m_milli,              // delay
                (TIMERPROC)wxTimerProc      // timer proc to call
             );

    if ( !m_id )
    {
        wxLogSysError(_("Couldn't create a timer"));

        return false;
    }

    // check that SetTimer() didn't reuse an existing id: according to the MSDN
    // this can happen and this would be catastrophic to us as we rely on ids
    // uniquely identifying the timers because we use them as keys in the hash
    if ( g_timerMap.find(m_id) != g_timerMap.end() )
    {
        wxLogError(_("Timer creation failed."));

        ::KillTimer(NULL, m_id);
        m_id = 0;

        return false;
    }

    g_timerMap[m_id] = this;

    return true;
}

void wxTimer::Stop()
{
    if ( m_id )
    {
        ::KillTimer(NULL, m_id);

        g_timerMap.erase(m_id);
    }

    m_id = 0;
}

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

void wxProcessTimer(wxTimer& timer)
{
    wxASSERT_MSG( timer.m_id != 0, _T("bogus timer id") );

    if ( timer.IsOneShot() )
        timer.Stop();

    timer.Notify();
}

void WINAPI wxTimerProc(HWND WXUNUSED(hwnd), WORD, int idTimer, DWORD)
{
    wxTimerMap::iterator node = g_timerMap.find(idTimer);

    wxCHECK_RET( node != g_timerMap.end(), wxT("bogus timer id in wxTimerProc") );

    wxProcessTimer(*(node->second));
}

#endif // wxUSE_TIMER

