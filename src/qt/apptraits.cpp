/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/apptraits.cpp
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/apptrait.h"
#include "wx/stdpaths.h"
#include "wx/evtloop.h"

#include "wx/qt/timer.h"

#include <QtGlobal>

wxEventLoopBase *wxGUIAppTraits::CreateEventLoop()
{
    return new wxEventLoop();
}

#if wxUSE_TIMER
wxTimerImpl *wxGUIAppTraits::CreateTimerImpl(wxTimer *timer)
{
    return new wxQtTimerImpl( timer );
}
#endif

// #if wxUSE_THREADS
// void wxGUIAppTraits::MutexGuiEnter()
// {
// }
// 
// void wxGUIAppTraits::MutexGuiLeave()
// {
// }

// #endif

wxPortId wxGUIAppTraits::GetToolkitVersion(int *majVer, int *minVer) const
{
    if ( majVer )
        *majVer = QT_VERSION >> 16;
    if ( minVer )
        *minVer = (QT_VERSION >> 8) & 0xFF;

    return wxPORT_QT;
}

//#############################################################################

#if wxUSE_CONSOLE_EVENTLOOP

wxEventLoopBase *wxConsoleAppTraits::CreateEventLoop()
{
    return new wxEventLoop();
}

#endif

//#############################################################################
