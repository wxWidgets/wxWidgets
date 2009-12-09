/////////////////////////////////////////////////////////////////////////////
// Name:        src/symbian/timer.cpp
// Purpose:     wxTimer implementation
// Author:      Andrei Matuk
// RCS-ID:      $Id$
// Copyright:   (c) 2009 Andrei Matuk <Veon.UA@gmail.com>
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
//    #include "wx/window.h"
    #include "wx/list.h"
    #include "wx/event.h"
    #include "wx/app.h"
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#include "wx/apptrait.h"
#include "wx/symbian/timer.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

//IMPLEMENT_ABSTRACT_CLASS(wxTimer, wxEvtHandler)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxTimer class
// ----------------------------------------------------------------------------

void wxSymbianTimerImpl::Init()
{
    CTimer::ConstructL();
    CActiveScheduler::Add(this);
}

wxSymbianTimerImpl::~wxSymbianTimerImpl()
{
    Stop();
}

bool wxSymbianTimerImpl::Start(int milliseconds, bool oneShot)
{
    // notice that this will stop an already running timer
    wxTimerImpl::Start(milliseconds, oneShot);

    //wxCHECK_MSG( m_milli > 0, false, wxT("invalid value for timer timeout") );
    CTimer::After(milliseconds * 1000);
    return true;
}

void wxSymbianTimerImpl::Stop()
{
    CTimer::Cancel();
}

void wxSymbianTimerImpl::RunL()
{
    // Called by OS when timer expires
    if (CTimer::iStatus == KErrNone)
    {
        Notify();

        if ( !IsOneShot() )
            Start(m_milli, false);
    }
}

// ============================================================================
// global functions
// ============================================================================
/*
wxUsecClock_t wxGetLocalTimeUsec()
{
#ifdef HAVE_GETTIMEOFDAY
    struct timeval tv;
    struct timezone tz;
    if ( wxGetTimeOfDay(&tv, &tz) != -1 )
    {
        wxUsecClock_t val = 1000000L; // usec/sec
        val *= tv.tv_sec;
        return val + tv.tv_usec;
    }
#endif // HAVE_GETTIMEOFDAY

    return wxGetLocalTimeMillis() * 1000L;
}
*/
wxTimerImpl *wxConsoleAppTraits::CreateTimerImpl(wxTimer *timer)
{
    return new wxSymbianTimerImpl(timer);
}

#endif // wxUSE_TIMER

