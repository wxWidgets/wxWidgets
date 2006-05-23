/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/timer.cpp
// Purpose:     wxTimer implementation
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/timer.h"

#ifndef WX_PRECOMP
    #include "wx/dynarray.h"
#endif

IMPLEMENT_ABSTRACT_CLASS(wxTimer, wxEvtHandler)

#ifdef __WXMAC__
#include "wx/mac/private.h"
#endif
#ifndef __DARWIN__
#include <Timer.h>
#endif

typedef struct MacTimerInfo
{
    TMTask m_task;
    wxMacNotifierTableRef m_table ;
    wxTimer* m_timer ;
} ;

static void wxProcessTimer( unsigned long event , void *data ) ;

static pascal void MacTimerProc( TMTask * t )
{
    MacTimerInfo * tm = (MacTimerInfo*)  t ;
    wxMacAddEvent( tm->m_table , wxProcessTimer, 0 , (void*) tm->m_timer , TRUE ) ;
}

// we need this array to track timers that are being deleted within the Notify procedure
// adding the timer before the Notify call and checking after whether it still is in there
// as the destructor would have removed it from the array

wxArrayPtrVoid gTimersInProcess ;

static void wxProcessTimer( unsigned long event , void *data )
{
    if ( !data )
        return ;

    wxTimer* timer = (wxTimer*) data ;

    if ( timer->IsOneShot() )
        timer->Stop() ;

    gTimersInProcess.Add( timer ) ;

    timer->Notify();

    int index = gTimersInProcess.Index( timer ) ;

    if ( index != wxNOT_FOUND )
    {
        gTimersInProcess.RemoveAt( index ) ;

        if ( !timer->IsOneShot() && timer->m_info->m_task.tmAddr )
        {
            PrimeTime( (QElemPtr)  &timer->m_info->m_task , timer->GetInterval() ) ;
        }
    }
}

void wxTimer::Init()
{
    m_info = new MacTimerInfo() ;
    m_info->m_task.tmAddr = NULL ;
    m_info->m_task.tmWakeUp = 0 ;
    m_info->m_task.tmReserved = 0 ;
    m_info->m_task.qType = 0 ;
    m_info->m_table = wxMacGetNotifierTable() ;
    m_info->m_timer = this ;
}

bool wxTimer::IsRunning() const
{
    // as the qType may already indicate it is elapsed, but it
    // was not handled internally yet
    return ( m_info->m_task.tmAddr != NULL ) ;
}

wxTimer::~wxTimer()
{
    Stop();
    if (m_info != NULL) {
        delete m_info ;
        m_info = NULL ;
    }
    int index = gTimersInProcess.Index( this ) ;
    if ( index != wxNOT_FOUND )
        gTimersInProcess.RemoveAt( index ) ;
}

bool wxTimer::Start(int milliseconds,bool mode)
{
    (void)wxTimerBase::Start(milliseconds, mode);

    wxCHECK_MSG( m_milli > 0, false, wxT("invalid value for timer timeout") );
    wxCHECK_MSG( m_info->m_task.tmAddr == NULL , false, wxT("attempting to restart a timer") );

#if defined(UNIVERSAL_INTERFACES_VERSION) && (UNIVERSAL_INTERFACES_VERSION >= 0x0340)
    m_info->m_task.tmAddr = NewTimerUPP( MacTimerProc ) ;
#else
    m_info->m_task.tmAddr = NewTimerProc( MacTimerProc ) ;
#endif
    m_info->m_task.tmWakeUp = 0 ;
    m_info->m_task.tmReserved = 0 ;
    m_info->m_task.qType = 0 ;
    m_info->m_timer = this ;
    InsXTime((QElemPtr) &m_info->m_task ) ;
    PrimeTime( (QElemPtr) &m_info->m_task , m_milli ) ;
    return true;
}

void wxTimer::Stop()
{
    if ( m_info->m_task.tmAddr )
    {
        RmvTime(  (QElemPtr) &m_info->m_task ) ;
        DisposeTimerUPP(m_info->m_task.tmAddr) ;
        m_info->m_task.tmAddr = NULL ;
    }
    wxMacRemoveAllNotifiersForData( wxMacGetNotifierTable() , this ) ;
}
