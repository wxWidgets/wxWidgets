/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/evtloop_qt.cpp
// Author:      Javier Torres
// Id:          $Id$
// Copyright:   (c) Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/evtloop.h"
#include "wx/app.h"
#include "wx/qt/evtloop_qt.h"

wxQtIdleTimer::wxQtIdleTimer( wxQtEventLoopBase *eventLoop )
{
    m_eventLoop = eventLoop;

    connect( this, SIGNAL( timeout() ), this, SLOT( idle() ) );
    setSingleShot( true );
    start( 0 );
}

bool wxQtIdleTimer::eventFilter( QObject *WXUNUSED( watched ), QEvent *WXUNUSED( event ) )
{
    // Called for each Qt event, start with timeout 0 (run as soon as idle)
    if ( !isActive() )
        start( 0 );

    return false; // Continue handling the event
}

void wxQtIdleTimer::idle()
{
    // Process pending events
    if ( wxTheApp )
        wxTheApp->ProcessPendingEvents();
    
    // Send idle event
    if ( m_eventLoop->ProcessIdle() )
        start( 0 );
}
