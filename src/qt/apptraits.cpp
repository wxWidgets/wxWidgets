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

wxAppTraits::wxAppTraits()
{
}

#if wxUSE_CONFIG
wxConfigBase *wxAppTraits::CreateConfig()
{
    return NULL;
}
#endif // wxUSE_CONFIG

#if wxUSE_LOG
wxLog *wxAppTraits::CreateLogTarget()
{
    return NULL;
}
#endif // wxUSE_LOG

wxMessageOutput *wxAppTraits::CreateMessageOutput()
{
    return NULL;
}

#if wxUSE_FONTMAP
wxFontMapper *wxAppTraits::CreateFontMapper()
{
    return NULL;
}
#endif // wxUSE_FONTMAP

wxRendererNative *wxAppTraits::CreateRenderer()
{
    return NULL;
}

wxStandardPaths& wxAppTraits::GetStandardPaths()
{
    static wxStandardPaths s_standardPaths;

    return s_standardPaths;
}

#if wxUSE_INTL
void wxAppTraits::SetLocale()
{
}
#endif // wxUSE_INTL

bool wxAppTraits::ShowAssertDialog(const wxString& msg)
{
    return false;
}

bool wxAppTraits::HasStderr()
{
    return false;
}

wxSocketManager *wxAppTraits::GetSocketManager()
{
    return wxAppTraitsBase::GetSocketManager();
}


wxEventLoopBase *wxAppTraits::CreateEventLoop()
{
    return new wxEventLoop();
}

#if wxUSE_TIMER
wxTimerImpl *wxAppTraits::CreateTimerImpl(wxTimer *timer)
{
    return new wxQtTimerImpl( timer );
}
#endif

// #if wxUSE_THREADS
void wxAppTraits::MutexGuiEnter()
{
}

void wxAppTraits::MutexGuiLeave()
{
}

// #endif

wxPortId wxAppTraits::GetToolkitVersion(int *majVer, int *minVer) const
{
    return wxPortId();
}


bool wxAppTraits::IsUsingUniversalWidgets() const
{
    return false;
}

wxString wxAppTraits::GetDesktopEnvironment() const
{
    return wxString( "QT" );
}

#if wxUSE_STACKWALKER
wxString wxAppTraits::GetAssertStackTrace()
{
    return wxString();
}

#endif

//#############################################################################

wxConsoleAppTraits::wxConsoleAppTraits()
{
}

wxConsoleAppTraits::~wxConsoleAppTraits()
{
}

wxEventLoopBase *wxConsoleAppTraits::CreateEventLoop()
{
    return new wxEventLoop();
}

//#############################################################################


