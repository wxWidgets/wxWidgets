/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/evtloop.cpp
// Author:      Peter Most, Javier Torres
// Copyright:   (c) Peter Most, Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/app.h"
#include "wx/apptrait.h"
#include "wx/evtloop.h"
#include "wx/private/eventloopsourcesmanager.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QAbstractEventDispatcher>

wxQtIdleTimer::wxQtIdleTimer( wxQtEventLoopBase *eventLoop )
{
    m_eventLoop = eventLoop;

    connect( this, &QTimer::timeout, this, &wxQtIdleTimer::idle );
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

void wxQtEventLoopBase::ScheduleExit(int rc)
{
    wxCHECK_RET( IsInsideRun(), wxT("can't call ScheduleExit() if not started") );
    
    QCoreApplication::exit( rc );
}

int wxQtEventLoopBase::DoRun()
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

void wxQtEventLoopBase::DoYieldFor(long eventsToProcess)
{
    while (wxTheApp && wxTheApp->Pending())
        // TODO: implement event filtering using the eventsToProcess mask
        wxTheApp->Dispatch();

    wxEventLoopBase::DoYieldFor(eventsToProcess);
}

#if wxUSE_EVENTLOOP_SOURCE
wxEventLoopSource *wxQtEventLoopBase::AddSourceForFD(int fd, wxEventLoopSourceHandler *handler, int flags)
{
    // --> QSocketNotifier

    return NULL;
}

class wxQtEventLoopSourcesManager : public wxEventLoopSourcesManagerBase
{
public:
    wxEventLoopSource *
    AddSourceForFD(int WXUNUSED(fd),
                   wxEventLoopSourceHandler* WXUNUSED(handler),
                   int WXUNUSED(flags))
    {
        wxFAIL_MSG("Monitoring FDs in the main loop is not implemented in wxQT");

        return NULL;
    }
};

wxEventLoopSourcesManagerBase* wxGUIAppTraits::GetEventLoopSourcesManager()
{
    static wxQtEventLoopSourcesManager s_eventLoopSourcesManager;

    return &s_eventLoopSourcesManager;
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
