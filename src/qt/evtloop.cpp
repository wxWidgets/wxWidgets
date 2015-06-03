/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/evtloop.cpp
// Author:      Mariano Reingart, Peter Most, Sean D'Epagnier, Javier Torres
// Copyright:   (c) 2010 wxWidgets dev team
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
#include <QtCore/QSocketNotifier>

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
    // already exist as we can't modify wxAppConsole
    if ( !QCoreApplication::instance() )
    {
        new QApplication( wxAppConsole::GetInstance()->argc, wxAppConsole::GetInstance()->argv );
    }
    
    // Create an idle timer to run each time there are no events (timeout = 0)
    m_qtIdleTimer = new wxQtIdleTimer( this );

    // Pass all events to the idle timer, so it can be restarted each time
    // an event is received
    qApp->installEventFilter( m_qtIdleTimer );
}

wxQtEventLoopBase::~wxQtEventLoopBase()
{
    delete m_qtIdleTimer;
}

void wxQtEventLoopBase::ScheduleExit(int rc)
{
    wxCHECK_RET( IsInsideRun(), wxT("can't call ScheduleExit() if not started") );
    m_shouldExit = true;
    QCoreApplication::exit( rc );
}

int wxQtEventLoopBase::DoRun()
{
    int ret;

    // This is placed inside of a loop to take into account nested event loops
    while ( !m_shouldExit )
    {
        // This will print Qt warnins if app already started:
        // "QCoreApplication::exec: The event loop is already running"
        // TODO: check the loopLevel (nested) like in wxGTK
        ret = QCoreApplication::exec();
        // process pending events (if exec was started previously)
        // TODO: use a real new QEventLoop() ?
        QCoreApplication::processEvents();
    }
    OnExit();
    return ret;
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

template <void (wxEventLoopSourceHandler::*function)()>
class wxQtSocketNotifier : public QSocketNotifier
{
public:
    wxQtSocketNotifier(int fd, Type type, wxEventLoopSourceHandler *handler)
        : QSocketNotifier(fd, type),
          m_handler(handler)
    {
        setEnabled(true);

        connect(this, &wxQtSocketNotifier::activated, this, &wxQtSocketNotifier::OnWaiting);
    }

    void OnWaiting()
    {
        (m_handler->*function)();
    }

    wxEventLoopSourceHandler *m_handler;
};

class wxQtEventLoopSource : public wxEventLoopSource
{
public:
    wxQtSocketNotifier<&wxEventLoopSourceHandler::OnReadWaiting> * m_reader;
    wxQtSocketNotifier<&wxEventLoopSourceHandler::OnWriteWaiting> * m_writer;
    wxQtSocketNotifier<&wxEventLoopSourceHandler::OnExceptionWaiting> * m_exception;

    wxQtEventLoopSource(int fd, wxEventLoopSourceHandler *handler, int flags)
        : wxEventLoopSource(handler, fd)
    {
        if ( flags & wxEVENT_SOURCE_INPUT )
            m_reader = new wxQtSocketNotifier<&wxEventLoopSourceHandler::OnReadWaiting>
                (fd, QSocketNotifier::Read, handler);
        else
            m_reader = NULL;

        if ( flags & wxEVENT_SOURCE_OUTPUT )
            m_writer = new wxQtSocketNotifier<&wxEventLoopSourceHandler::OnWriteWaiting>
                (fd, QSocketNotifier::Write, handler);
        else
            m_writer = NULL;

        if ( flags & wxEVENT_SOURCE_EXCEPTION )
            m_exception = new wxQtSocketNotifier<&wxEventLoopSourceHandler::OnExceptionWaiting>
                (fd, QSocketNotifier::Exception, handler);
        else
            m_exception = NULL;
    }

    virtual ~wxQtEventLoopSource()
    {
        // clean up notifiers
        if (m_reader)
            delete m_reader;
        if (m_writer)
            delete m_writer;
        if (m_exception)
            delete m_exception;
    }
};

class wxQtEventLoopSourcesManager : public wxEventLoopSourcesManagerBase
{
public:
    wxEventLoopSource*
    AddSourceForFD(int fd, wxEventLoopSourceHandler* handler, int flags)
    {
        return new wxQtEventLoopSource(fd, handler, flags);
    }
};

wxEventLoopSourcesManagerBase* wxGUIAppTraits::GetEventLoopSourcesManager()
{
    static wxQtEventLoopSourcesManager s_eventLoopSourcesManager;
    return &s_eventLoopSourcesManager;
}

#if !wxUSE_CONSOLE_EVENTLOOP

// Use the GUI event loop sources manager if console support is disabled
// (needed by some common code, will raise an undefinied reference if not done)

wxEventLoopSourcesManagerBase* wxAppTraits::GetEventLoopSourcesManager()
{
    static wxQtEventLoopSourcesManager s_eventLoopSourcesManager;
    return &s_eventLoopSourcesManager;
}

#endif

wxEventLoopSource *wxQtEventLoopBase::AddSourceForFD(int fd, wxEventLoopSourceHandler *handler, int flags)
{
    wxGUIAppTraits *AppTraits = dynamic_cast<wxGUIAppTraits *>(wxApp::GetTraitsIfExists());

    if(AppTraits)
        return AppTraits->GetEventLoopSourcesManager()->AddSourceForFD(fd, handler, flags);

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
