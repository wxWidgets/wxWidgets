///////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/baseunix.cpp
// Purpose:     misc stuff only used in console applications under Unix
// Author:      Vadim Zeitlin
// Modified by:
// Created:     23.06.2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/utils.h"
#endif //WX_PRECOMP

#include "wx/apptrait.h"
#include "wx/unix/execute.h"
#include "wx/evtloop.h"
#include "wx/gsocket.h"

#include "wx/unix/private/timer.h"

// for waitpid()
#include <sys/types.h>
#include <sys/wait.h>

// ============================================================================
// wxConsoleAppTraits implementation
// ============================================================================

int
wxConsoleAppTraits::WaitForChild(wxExecuteData& execData)
{
    int exitcode = 0;
    if ( execData.flags & wxEXEC_SYNC )
    {
        if ( waitpid(execData.pid, &exitcode, 0) == -1 || !WIFEXITED(exitcode) )
        {
            wxLogSysError(_("Waiting for subprocess termination failed"));
        }
    }
    else // asynchronous execution
    {
        wxEndProcessData *endProcData = new wxEndProcessData;
        endProcData->process  = execData.process;
        endProcData->pid      = execData.pid;
        endProcData->tag = wxAddProcessCallback
                           (
                             endProcData,
                             execData.pipeEndProcDetect.Detach(wxPipe::Read)
                           );

        execData.pipeEndProcDetect.Close();
        exitcode = execData.pid;

    }

    return exitcode;
}

#if wxUSE_TIMER

wxTimerImpl *wxConsoleAppTraits::CreateTimerImpl(wxTimer *timer)
{
    // this doesn't work yet as there is no main loop in console applications
    // (but it will be added later)
    return new wxUnixTimerImpl(timer);
}

#endif // wxUSE_TIMER

//  Note: wxConsoleAppTraits::CreateEventLoop() is defined in evtloopunix.cpp!
