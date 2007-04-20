/////////////////////////////////////////////////////////////////////////////
// Name:        sec/mac/carbon/timer.cpp
// Purpose:     wxTimer implementation
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_TIMER

#ifndef WX_PRECOMP
    #include "wx/dynarray.h"
#endif

#include "wx/mac/private/timer.h"

#ifdef __WXMAC__
    #include "wx/mac/private.h"
#endif

#ifndef __DARWIN__
    #include <Timer.h>
#endif

#define wxMAC_USE_CARBON_TIMER 1

#if wxMAC_USE_CARBON_TIMER

struct MacTimerInfo
{
    wxCarbonTimerImpl* m_timer;
    EventLoopTimerUPP m_proc;
    EventLoopTimerRef   m_timerRef;
};

static pascal void wxProcessTimer( EventLoopTimerRef theTimer, void *data )
{
    if ( data == NULL )
        return;

    wxCarbonTimerImpl* timer = (wxCarbonTimerImpl*)data;

    if ( timer->IsOneShot() )
        timer->Stop();

    timer->Notify();
}

wxCarbonTimerImpl::wxCarbonTimerImpl(wxTimer *timer)
                 : wxTimerImpl(timer)
{
    m_info = new MacTimerInfo();
    m_info->m_timer = this;
    m_info->m_proc = NULL;
    m_info->m_timerRef = kInvalidID;
}

bool wxCarbonTimerImpl::IsRunning() const
{
    return ( m_info->m_timerRef != kInvalidID );
}

wxCarbonTimerImpl::~wxCarbonTimerImpl()
{
    delete m_info;
}

bool wxCarbonTimerImpl::Start( int milliseconds, bool mode )
{
    (void)wxTimerImpl::Start(milliseconds, mode);

    wxCHECK_MSG( m_milli > 0, false, wxT("invalid value for timer timeout") );
    wxCHECK_MSG( m_info->m_timerRef == NULL, false, wxT("attempting to restart a timer") );

    m_info->m_timer = this;
    m_info->m_proc = NewEventLoopTimerUPP( &wxProcessTimer );

    OSStatus err = InstallEventLoopTimer(
        GetMainEventLoop(),
        m_milli*kEventDurationMillisecond,
        IsOneShot() ? 0 : m_milli * kEventDurationMillisecond,
        m_info->m_proc,
        this,
        &m_info->m_timerRef );
    verify_noerr( err );

    return true;
}

void wxCarbonTimerImpl::Stop()
{
    if (m_info->m_timerRef)
        RemoveEventLoopTimer( m_info->m_timerRef );
    if (m_info->m_proc)
        DisposeEventLoopTimerUPP( m_info->m_proc );

    m_info->m_proc = NULL;
    m_info->m_timerRef = kInvalidID;
}

#else // !wxMAC_USE_CARBON_TIMER

typedef struct MacTimerInfo
{
    TMTask m_task;
    wxMacNotifierTableRef m_table;
    wxCarbonTimerImpl* m_timer;
};

static void wxProcessTimer( unsigned long event, void *data );

static pascal void MacTimerProc( TMTask * t )
{
    MacTimerInfo * tm = (MacTimerInfo*) t;
    wxMacAddEvent( tm->m_table, wxProcessTimer, 0, (void*) tm->m_timer, true );
}

// we need this array to track timers that are being deleted within the Notify procedure
// adding the timer before the Notify call and checking after whether it still is in there
// as the destructor would have removed it from the array

wxArrayPtrVoid gTimersInProcess;

static void wxProcessTimer( unsigned long event, void *data )
{
    if ( data == NULL )
        return;

    wxCarbonTimerImpl* timer = (wxCarbonTimerImpl*) data;
    if ( timer->m_oneShot )
        timer->Stop();

    gTimersInProcess.Add( timer );
    timer->Notify();

    int index = gTimersInProcess.Index( timer );
    if ( index != wxNOT_FOUND )
    {
        gTimersInProcess.RemoveAt( index );

        if ( !timer->IsOneShot() && timer->m_info->m_task.tmAddr )
            PrimeTime( (QElemPtr) &timer->m_info->m_task, timer->GetInterval() );
    }
}

void wxCarbonTimerImpl::Init()
{
    m_info = new MacTimerInfo();
    m_info->m_task.tmAddr = NULL;
    m_info->m_task.tmWakeUp = 0;
    m_info->m_task.tmReserved = 0;
    m_info->m_task.qType = 0;
    m_info->m_table = wxMacGetNotifierTable();
    m_info->m_timer = this;
}

bool wxCarbonTimerImpl::IsRunning() const
{
    // as the qType may already indicate it is elapsed, but it
    // was not handled internally yet
    return ( m_info->m_task.tmAddr != NULL );
}

wxCarbonTimerImpl::~wxCarbonTimerImpl()
{
    Stop();
    if (m_info != NULL)
    {
        delete m_info;
        m_info = NULL;
    }

    int index = gTimersInProcess.Index( this );
    if ( index != wxNOT_FOUND )
        gTimersInProcess.RemoveAt( index );
}

bool wxCarbonTimerImpl::Start( int milliseconds, bool mode )
{
    (void)wxTimerBase::Start( milliseconds, mode );

    wxCHECK_MSG( m_milli > 0, false, wxT("invalid value for timer timeout") );
    wxCHECK_MSG( m_info->m_task.tmAddr == NULL, false, wxT("attempting to restart a timer") );

    m_info->m_task.tmAddr = NewTimerUPP( MacTimerProc );
    m_info->m_task.tmWakeUp = 0;
    m_info->m_task.tmReserved = 0;
    m_info->m_task.qType = 0;
    m_info->m_timer = this;
    InsXTime( (QElemPtr) &m_info->m_task );
    PrimeTime( (QElemPtr) &m_info->m_task, m_milli );

    return true;
}

void wxCarbonTimerImpl::Stop()
{
    if ( m_info->m_task.tmAddr )
    {
        RmvTime( (QElemPtr) &m_info->m_task );
        DisposeTimerUPP( m_info->m_task.tmAddr );
        m_info->m_task.tmAddr = NULL;
    }

    wxMacRemoveAllNotifiersForData( wxMacGetNotifierTable(), this );
}

#endif // wxMAC_USE_CARBON_TIMER/!wxMAC_USE_CARBON_TIMER

#endif // wxUSE_TIMER

