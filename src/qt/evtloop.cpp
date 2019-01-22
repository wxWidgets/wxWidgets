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
#include <QtCore/QTimer>
#include <QtCore/QEventLoop>

#include <QtWidgets/QApplication>

class wxQtIdleTimer : public QTimer
{

public:
    wxQtIdleTimer( wxQtEventLoopBase *eventLoop );
    virtual bool eventFilter( QObject * watched, QEvent * event  );

private:
    void idle();

private:
    wxQtEventLoopBase *m_eventLoop;
};

wxQtIdleTimer::wxQtIdleTimer( wxQtEventLoopBase *eventLoop )
{
    m_eventLoop = eventLoop;

    connect( this, &QTimer::timeout, this, &wxQtIdleTimer::idle );
    setSingleShot( true );
}

bool wxQtIdleTimer::eventFilter( QObject *WXUNUSED( watched ), QEvent *WXUNUSED( event ) )
{
    // Called for each Qt event, start with timeout 0 (run as soon as idle)
    if ( !isActive() )
        m_eventLoop->ScheduleIdleCheck();

    return false; // Continue handling the event
}

void wxQtIdleTimer::idle()
{
    // Process pending events
    if ( wxTheApp )
        wxTheApp->ProcessPendingEvents();

    // Send idle event
    if ( m_eventLoop->ProcessIdle() )
        m_eventLoop->ScheduleIdleCheck();
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


    m_qtEventLoop = new QEventLoop;
}

wxQtEventLoopBase::~wxQtEventLoopBase()
{
    qApp->removeEventFilter(m_qtIdleTimer);
    delete m_qtEventLoop;
    delete m_qtIdleTimer;
}

void wxQtEventLoopBase::ScheduleExit(int rc)
{
    wxCHECK_RET( IsInsideRun(), wxT("can't call ScheduleExit() if not started") );
    m_shouldExit = true;
    m_qtEventLoop->exit(rc);
}

int wxQtEventLoopBase::DoRun()
{
    const int ret = m_qtEventLoop->exec();
    OnExit();
    return ret;
}

bool wxQtEventLoopBase::Pending() const
{
    QAbstractEventDispatcher *instance = QAbstractEventDispatcher::instance();
    return instance->hasPendingEvents();
}

bool wxQtEventLoopBase::Dispatch()
{
    m_qtEventLoop->processEvents();
    return true;
}

int wxQtEventLoopBase::DispatchTimeout(unsigned long timeout)
{
    m_qtEventLoop->processEvents(QEventLoop::AllEvents, timeout);
    return true;
}

void wxQtEventLoopBase::WakeUp()
{
    QAbstractEventDispatcher *instance = QAbstractEventDispatcher::instance();
    if ( instance )
        instance->wakeUp();
}

void wxQtEventLoopBase::DoYieldFor(long eventsToProcess)
{
    while (wxTheApp && wxTheApp->Pending())
        // TODO: implement event filtering using the eventsToProcess mask
        wxTheApp->Dispatch();

    wxEventLoopBase::DoYieldFor(eventsToProcess);
}

void wxQtEventLoopBase::ScheduleIdleCheck()
{
    if ( IsInsideRun() )
        m_qtIdleTimer->start(0);
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

    if ( AppTraits )
        return AppTraits->GetEventLoopSourcesManager()->AddSourceForFD(fd, handler, flags);

    return NULL;
}

#endif // wxUSE_EVENTLOOP_SOURCE

//#############################################################################

#if wxUSE_GUI

wxGUIEventLoop::wxGUIEventLoop()
{
}

#endif // wxUSE_GUI
