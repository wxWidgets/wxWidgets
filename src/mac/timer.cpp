/////////////////////////////////////////////////////////////////////////////
// Name:        timer.cpp
// Purpose:     wxTimer implementation
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "timer.h"
#endif

#include "wx/timer.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_ABSTRACT_CLASS(wxTimer, wxObject)
#endif

static void wxProcessTimer( unsigned long event , void *data ) ;

static pascal void MacTimerProc( TMTask * t )
{
	MacTimerInfo * tm = (MacTimerInfo*)  t ;
	wxMacAddEvent( tm->m_table , wxProcessTimer, 0 , (void*) tm->m_timer , TRUE ) ;
}

static void wxProcessTimer( unsigned long event , void *data )
{
	if ( !data )
		return ;
		
	wxTimer* timer = (wxTimer*) data ;
	if ( timer->IsOneShot() )
		timer->Stop() ;
		
    timer->Notify();

    if ( timer->m_info.m_task.tmAddr && !timer->IsOneShot() )
    {
	    PrimeTime( (QElemPtr)  &timer->m_info.m_task , timer->GetInterval() ) ;
    }
}

void wxTimer::Init()
{
	m_info.m_task.tmAddr = NULL ;
	m_info.m_task.tmWakeUp = 0 ;
	m_info.m_task.tmReserved = 0 ;
	m_info.m_task.qType = 0 ;
	m_info.m_table = wxMacGetNotifierTable() ;
	m_info.m_timer = this ;
}

bool wxTimer::IsRunning() const 
{
	return ( m_info.m_task.qType & kTMTaskActive ) ;
}

wxTimer::~wxTimer()
{
    Stop();
}

bool wxTimer::Start(int milliseconds,bool mode)
{
    (void)wxTimerBase::Start(milliseconds, mode);

    wxCHECK_MSG( m_milli > 0, FALSE, wxT("invalid value for timer timeour") );
    wxCHECK_MSG( m_info.m_task.tmAddr == NULL , FALSE, wxT("attempting to restart a timer") );

    m_milli = milliseconds;
#ifdef __UNIX__
    m_info.m_task.tmAddr = NewTimerUPP( MacTimerProc ) ;
#else
    m_info.m_task.tmAddr = NewTimerProc( MacTimerProc ) ;
#endif
    m_info.m_task.tmWakeUp = 0 ;
    m_info.m_task.tmReserved = 0 ;
    InsXTime((QElemPtr) &m_info.m_task ) ;
    PrimeTime( (QElemPtr) &m_info.m_task , m_milli ) ;
    return FALSE;
}

void wxTimer::Stop()
{
    m_milli = 0 ;
    if ( m_info.m_task.tmAddr )
    {
    	RmvTime(  (QElemPtr) &m_info.m_task ) ;
    	DisposeTimerUPP(m_info.m_task.tmAddr) ;
    	m_info.m_task.tmAddr = NULL ;
    }
    wxMacRemoveAllNotifiersForData( wxMacGetNotifierTable() , this ) ;
}



