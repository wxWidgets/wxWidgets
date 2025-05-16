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

class wxQtIdleTimer : public QTimer, public wxRefCounter
{

public:
    wxQtIdleTimer();
    ~wxQtIdleTimer();
    virtual bool eventFilter( QObject * watched, QEvent * event  ) override;

private:
    void idle();
    void ScheduleIdleCheck();
};

wxQtIdleTimer::wxQtIdleTimer()
{
    // We need a QCoreApplication for event loops, create it here if it doesn't
    // already exist as we can't modify wxAppConsole
    if ( !QCoreApplication::instance() )
    {
        new QApplication(wxAppConsole::GetInstance()->argc, wxAppConsole::GetInstance()->argv);
    }


    // Pass all events to the idle timer, so it can be restarted each time
    // an event is received
    qApp->installEventFilter(this);

    connect(this, &QTimer::timeout, this, &wxQtIdleTimer::idle);
    setSingleShot(true);
}


wxQtIdleTimer::~wxQtIdleTimer()
{
    qApp->removeEventFilter(this);
}

bool wxQtIdleTimer::eventFilter( QObject *WXUNUSED( watched ), QEvent *WXUNUSED( event ) )
{
    // Called for each Qt event, start with timeout 0 (run as soon as idle)
    if ( !isActive() )
       ScheduleIdleCheck();

    return false; // Continue handling the event
}

void wxQtIdleTimer::idle()
{
    // Process pending events
    if ( wxTheApp )
        wxTheApp->ProcessPendingEvents();

    // Send idle event
    if (wxTheApp->ProcessIdle())
        ScheduleIdleCheck();
}


namespace
{
    wxObjectDataPtr<wxQtIdleTimer> gs_idleTimer;
}

void wxQtIdleTimer::ScheduleIdleCheck()
{
    wxQtEventLoopBase *eventLoop = static_cast<wxQtEventLoopBase*>(wxEventLoop::GetActive());
    if ( eventLoop )
        eventLoop->ScheduleIdleCheck();
}

wxQtEventLoopBase::wxQtEventLoopBase()
{
    // Create an idle timer to run each time there are no events (timeout = 0)
    if ( !gs_idleTimer )
        gs_idleTimer.reset(new wxQtIdleTimer());

    m_qtIdleTimer = gs_idleTimer;
    m_qtEventLoop = new QEventLoop;
}

wxQtEventLoopBase::~wxQtEventLoopBase()
{
    //Clear the shared timer if this is the only external reference to it
    if ( gs_idleTimer->GetRefCount() <= 2 )
        gs_idleTimer.reset(nullptr);

    delete m_qtEventLoop;
}

void wxQtEventLoopBase::DoStop(int rc)
{
    m_qtEventLoop->exit(rc);
}

int wxQtEventLoopBase::DoRun()
{
    return m_qtEventLoop->exec();
}

bool wxQtEventLoopBase::Pending() const
{
    // Note that we are not using any of the QAbstractEventDispatcher::hasPendingEvents()
    // or QCoreApplication::hasPendingEvents() functions here to check for pending events,
    // as the functions were deprecated in Qt5.3 and removed entirely in Qt6 due to their
    // unreliable way to check for pending events according to the Qt developers.
    //
    // So, in the absence of a replacement for these functions, this function is useless
    // under wxQt as it just returns false.

    return false;
}

bool wxQtEventLoopBase::Dispatch()
{
    if ( m_qtEventLoop->processEvents(QEventLoop::WaitForMoreEvents) )
    {
        return !m_qtEventLoop->isRunning();
    }

    return true;
}

int wxQtEventLoopBase::DispatchTimeout(unsigned long timeout)
{
    m_qtEventLoop->processEvents(QEventLoop::AllEvents, timeout);
    return true;
}

bool wxQtEventLoopBase::QtDispatch() const
{
    return m_qtEventLoop->processEvents();
}

void wxQtEventLoopBase::WakeUp()
{
    QAbstractEventDispatcher *instance = QAbstractEventDispatcher::instance();
    if ( instance )
        instance->wakeUp();
}

void wxQtEventLoopBase::DoYieldFor(long eventsToProcess)
{

    QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents;

    if ( !(eventsToProcess & wxEVT_CATEGORY_USER_INPUT) )
        flags |= QEventLoop::ExcludeUserInputEvents;

    if ( !(eventsToProcess & wxEVT_CATEGORY_SOCKET) )
        flags |= QEventLoop::ExcludeSocketNotifiers;

    m_qtEventLoop->processEvents(flags);

    wxEventLoopBase::DoYieldFor(eventsToProcess);
}

void wxQtEventLoopBase::ScheduleIdleCheck()
{
    if ( IsInsideRun() && !m_shouldExit )
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
            m_reader = nullptr;

        if ( flags & wxEVENT_SOURCE_OUTPUT )
            m_writer = new wxQtSocketNotifier<&wxEventLoopSourceHandler::OnWriteWaiting>
                (fd, QSocketNotifier::Write, handler);
        else
            m_writer = nullptr;

        if ( flags & wxEVENT_SOURCE_EXCEPTION )
            m_exception = new wxQtSocketNotifier<&wxEventLoopSourceHandler::OnExceptionWaiting>
                (fd, QSocketNotifier::Exception, handler);
        else
            m_exception = nullptr;
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
    AddSourceForFD(int fd, wxEventLoopSourceHandler* handler, int flags) override
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

#endif // wxUSE_EVENTLOOP_SOURCE

//#############################################################################

#if wxUSE_GUI

wxGUIEventLoop::wxGUIEventLoop()
{
}

#endif // wxUSE_GUI
