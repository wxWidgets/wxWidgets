/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/apptraits.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/apptrait.h"
#include "wx/stdpaths.h"
#include "wx/evtloop.h"

#include "wx/qt/timer.h"

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
    return wxPortId();
}

//#############################################################################

wxEventLoopBase *wxConsoleAppTraits::CreateEventLoop()
{
    return new wxEventLoop();
}

//#############################################################################
