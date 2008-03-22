/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/corefoundation/utilsexec_cf.cpp
// Purpose:     Execution-related utilities for Darwin
// Author:      David Elliott, Ryan Norton (wxMacExecute)
// Modified by: Stefan Csomor (added necessary wxT for unicode builds)
// Created:     2004-11-04
// RCS-ID:      $Id$
// Copyright:   (c) David Elliott, Ryan Norton
// Licence:     wxWindows licence
// Notes:       This code comes from src/mac/carbon/utilsexc.cpp,1.11
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/utils.h"
#endif //ndef WX_PRECOMP
#include "wx/unix/execute.h"
#include "wx/stdpaths.h"
#include "wx/app.h"
#include "wx/apptrait.h"
#include "wx/thread.h"
#include "wx/process.h"

#include <sys/wait.h>

#include <CoreFoundation/CFSocket.h>

/*!
    Called due to source signal detected by the CFRunLoop.
    This is nearly identical to the wxGTK equivalent.
 */
extern "C" void WXCF_EndProcessDetector(CFSocketRef s,
                                        CFSocketCallBackType WXUNUSED(callbackType),
                                        CFDataRef WXUNUSED(address),
                                        void const *WXUNUSED(data),
                                        void *info)
{
    wxEndProcessData * const proc_data = static_cast<wxEndProcessData*>(info);

/// This code could reasonably be shared between wxMac/wxCocoa and wxGTK ///
    // PID is always positive on UNIX but wx uses the sign bit as a flag.
    int pid = (proc_data->pid > 0) ? proc_data->pid : -proc_data->pid;
    int status = 0;
    int rc = waitpid(pid, &status, WNOHANG);
    if(rc == 0)
    {
        // Keep waiting in case we got a spurious notification
        // NOTE: In my limited testing, this doesn't happen.
        return;
    }

    if(rc == -1)
    {   // Error.. really shouldn't happen but try to gracefully handle it
        wxLogLastError(_T("waitpid"));
        proc_data->exitcode = -1;
    }
    else
    {   // Process ended for some reason
        wxASSERT_MSG(rc == pid, _T("unexpected waitpid() return value"));

        if(WIFEXITED(status))
            proc_data->exitcode = WEXITSTATUS(status);
        else if(WIFSIGNALED(status))
            // wxGTK doesn't do this but why not?
            proc_data->exitcode = -WTERMSIG(status);
        else
        {   // Should NEVER happen according to waitpid docs
            wxLogError(wxT("waitpid indicates process exited but not due to exiting or signalling"));
            proc_data->exitcode = -1;
        }
    }
/// The above code could reasonably be shared between wxMac/wxCocoa and wxGTK ///

    /*
        Either waitpid failed or the process ended successfully.  Either way,
        we're done.  It's not if waitpid is going to magically succeed when
        we get fired again.  CFSocketInvalidate closes the fd for us and also
        invalidates the run loop source for us which should cause it to
        release the CFSocket (thus causing it to be deallocated) and remove
        itself from the runloop which should release it and cause it to also
        be deallocated.  Of course, it's possible the RunLoop hangs onto
        one or both of them by retaining/releasing them within its stack
        frame.  However, that shouldn't be depended on.  Assume that s is
        deallocated due to the following call.
     */
    CFSocketInvalidate(s);

    // Now tell wx that the process has ended.
    wxHandleProcessTermination(proc_data);
}

/*!
    Implements the GUI-specific AddProcessCallback() for both wxMac and
    wxCocoa using the CFSocket/CFRunLoop API which is available to both.
    Takes advantage of the fact that sockets on UNIX are just regular
    file descriptors and thus even a non-socket file descriptor can
    apparently be used with CFSocket so long as you only tell CFSocket
    to do things with it that would be valid for a non-socket fd.
 */
int wxGUIAppTraits::AddProcessCallback(wxEndProcessData *proc_data, int fd)
{
    static int s_last_tag = 0;
    CFSocketContext context =
    {   0
    ,   static_cast<void*>(proc_data)
    ,   NULL
    ,   NULL
    ,   NULL
    };
    CFSocketRef cfSocket = CFSocketCreateWithNative(kCFAllocatorDefault,fd,kCFSocketReadCallBack,&WXCF_EndProcessDetector,&context);
    if(cfSocket == NULL)
    {
        wxLogError(wxT("Failed to create socket for end process detection"));
        return 0;
    }
    CFRunLoopSourceRef runLoopSource = CFSocketCreateRunLoopSource(kCFAllocatorDefault, cfSocket, /*highest priority:*/0);
    if(runLoopSource == NULL)
    {
        wxLogError(wxT("Failed to create CFRunLoopSource from CFSocket for end process detection"));
        // closes the fd.. we can't really stop it, nor do we necessarily want to.
        CFSocketInvalidate(cfSocket);
        CFRelease(cfSocket);
        return 0;
    }
    // Now that the run loop source has the socket retained and we no longer
    // need to refer to it within this method, we can release it.
    CFRelease(cfSocket);

    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
    // Now that the run loop has the source retained we can release it.
    CFRelease(runLoopSource);

    /*
        Feed wx some bullshit.. we don't use it since CFSocket helpfully passes
        itself into our callback and that's enough to be able to
        CFSocketInvalidate it which is all we need to do to get everything we
        just created to be deallocated.
     */
    return ++s_last_tag;
}

/////////////////////////////////////////////////////////////////////////////

// NOTE: This doesn't really belong here but this was a handy file to
// put it in because it's already compiled for wxCocoa and wxMac GUI lib.
#if wxUSE_STDPATHS
static wxStandardPathsCF gs_stdPaths;
wxStandardPathsBase& wxGUIAppTraits::GetStandardPaths()
{
    return gs_stdPaths;
}
#endif

