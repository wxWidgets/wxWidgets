/////////////////////////////////////////////////////////////////////////////
// Name:        timer.cpp
// Purpose:     wxTimer implementation
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "timer.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_TIMER

#ifndef WX_PRECOMP
    #include "wx/hashmap.h"
    #include "wx/setup.h"
    #include "wx/window.h"
    #include "wx/list.h"
    #include "wx/event.h"
    #include "wx/app.h"
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#include "wx/timer.h"

#include "wx/msw/private.h"

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

WX_DECLARE_HASH_MAP( long,
                     wxTimer*,
                     wxIntegerHash,
                     wxIntegerEqual,
                     wxTimerMap );

wxTimerMap wxTimerList;
UINT WINAPI _EXPORT wxTimerProc(HWND hwnd, WORD, int idTimer, DWORD);

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#ifdef __WIN32__
    #define _EXPORT
#else
    #define _EXPORT _export
#endif

// should probably be in wx/msw/private.h
#ifdef __WXMICROWIN__
    #define MakeProcInstance(proc, hinst) proc
#endif

IMPLEMENT_ABSTRACT_CLASS(wxTimer, wxObject)

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
    long id = m_id;

    wxTimer::Stop();

    wxTimerList.erase(id);
}

bool wxTimer::Start(int milliseconds, bool oneShot)
{
    (void)wxTimerBase::Start(milliseconds, oneShot);

    wxCHECK_MSG( m_milli > 0, false, wxT("invalid value for timer timeour") );

    TIMERPROC wxTimerProcInst = (TIMERPROC)
        MakeProcInstance((FARPROC)wxTimerProc, wxGetInstance());

    m_id = ::SetTimer(NULL, (UINT)(m_id ? m_id : 1),
                      (UINT)m_milli, wxTimerProcInst);

    if ( m_id > 0 )
    {
        wxTimerList[m_id] = this;

        return true;
    }
    else
    {
        wxLogSysError(_("Couldn't create a timer"));

        return false;
    }
}

void wxTimer::Stop()
{
    if ( m_id )
    {
        ::KillTimer(NULL, (UINT)m_id);

        wxTimerList.erase(m_id);
    }

    m_id = 0;
}

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

void wxProcessTimer(wxTimer& timer)
{
    // Avoid to process spurious timer events
    if ( timer.m_id == 0)
        return;

    if ( timer.IsOneShot() )
        timer.Stop();

    timer.Notify();
}

UINT WINAPI _EXPORT wxTimerProc(HWND WXUNUSED(hwnd), WORD, int idTimer, DWORD)
{
    
    wxTimerMap::iterator node = wxTimerList.find((long)idTimer);

    wxCHECK_MSG( node != wxTimerList.end(), 0,
                 wxT("bogus timer id in wxTimerProc") );

    wxProcessTimer(*(node->second));

    return 0;
}

#endif // wxUSE_TIMER
