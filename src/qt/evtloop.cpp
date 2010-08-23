/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/evtloop.cpp
// Author:      Peter Most, Javier Torres
// Id:          $Id$
// Copyright:   (c) Peter Most, Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/evtloop.h"
#include "wx/qt/evtloop_qt.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QAbstractEventDispatcher>

wxQtEventLoopBase::wxQtEventLoopBase()
{
    // We need a QCoreApplication for event loops, create it here if it doesn't
    // already exist as we can't modify wxConsoleApp
    if ( !QCoreApplication::instance() )
    {
        int n = 0;
        new QCoreApplication( n , NULL );
    }
    
    // Create an idle timer to run each time there are no events (timeout = 0)
    m_qtIdleTimer = new wxQtIdleTimer( this );

    // Pass all events to the idle timer, so it can be restarted each time
    // an event is received
    QCoreApplication::instance()->installEventFilter( m_qtIdleTimer );
}

void wxQtEventLoopBase::Exit(int rc)
{
    QCoreApplication::exit( rc );
}

int wxQtEventLoopBase::Run()
{
    return QCoreApplication::exec();
}

bool wxQtEventLoopBase::Pending() const
{
    return QCoreApplication::hasPendingEvents();
}

bool wxQtEventLoopBase::Dispatch()
{
    QCoreApplication::processEvents();

    return true;
}

int wxQtEventLoopBase::DispatchTimeout(unsigned long timeout)
{
    QCoreApplication::processEvents( QEventLoop::AllEvents, timeout );

    return true;
}

void wxQtEventLoopBase::WakeUp()
{
    QAbstractEventDispatcher::instance()->wakeUp();
}

bool wxQtEventLoopBase::YieldFor(long WXUNUSED( eventsToProcess ))
{
    return Dispatch();
}

#if wxUSE_EVENTLOOP_SOURCE
wxEventLoopSource *wxQtEventLoopBase::AddSourceForFD(int fd, wxEventLoopSourceHandler *handler, int flags)
{
    // --> QSocketNotifier

    return NULL;
}
#endif // wxUSE_EVENTLOOP_SOURCE

//#############################################################################

#if wxUSE_GUI

wxGUIEventLoop::wxGUIEventLoop()
{
}

#else // !wxUSE_GUI

//#############################################################################

#if wxUSE_CONSOLE_EVENTLOOP

wxConsoleEventLoop::wxConsoleEventLoop()
{
}

#endif // wxUSE_CONSOLE_EVENTLOOP

#endif // wxUSE_GUI
