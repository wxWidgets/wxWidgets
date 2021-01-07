/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/timer.cpp
// Author:      Javier Torres
// Copyright:   (c) Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_TIMER

#include "wx/qt/private/timer.h"
#include <QtCore/QTimerEvent>

wxQtTimerImpl::wxQtTimerImpl( wxTimer* timer )
    : wxTimerImpl( timer )
{
    m_timerId = -1;
}

bool wxQtTimerImpl::Start( int millisecs, bool oneShot )
{
    if ( !wxTimerImpl::Start( millisecs, oneShot ) )
        return false;

    if ( m_timerId >= 0 )
        return false;

    m_timerId = startTimer( GetInterval() );

    return m_timerId >= 0;
}

void wxQtTimerImpl::Stop()
{
    killTimer( m_timerId );
    m_timerId = -1;
}

bool wxQtTimerImpl::IsRunning() const
{
    return m_timerId >= 0;
}

void wxQtTimerImpl::timerEvent( QTimerEvent * event )
{
    if ( event->timerId() == m_timerId )
    {
        if ( IsOneShot() )
        {
            Stop();
        }
        Notify();
    }
}

#endif // wxUSE_TIMER
