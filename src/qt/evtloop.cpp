/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/evtloop.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/evtloop.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QAbstractEventDispatcher>

#if wxUSE_BASE

wxQtEventLoopBase::wxQtEventLoopBase()
{
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

#endif // wxUSE_BASE

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
