/////////////////////////////////////////////////////////////////////////////
// Name:        utilsexec.cpp
// Purpose:     Execution-related utilities
// Author:      Stefan Csomor
// Modified by: David Elliott
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
//#pragma implementation
#endif

#include "wx/log.h"
#include "wx/utils.h"
#ifdef __DARWIN__
#include "wx/unix/execute.h"
#include <unistd.h>
#include <sys/wait.h>
extern "C" {
#include <mach/mach.h>
}
#include <CoreFoundation/CFMachPort.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __DARWIN__

#include "wx/mac/private.h"
#include "LaunchServices.h"

long wxExecute(const wxString& command, int flags, wxProcess *WXUNUSED(handler))
{
    wxASSERT_MSG( flags == wxEXEC_ASYNC,
        wxT("wxExecute: Only wxEXEC_ASYNC is supported") );

    FSRef fsRef ;
    OSErr err = noErr ;
    err = wxMacPathToFSRef( command , &fsRef ) ;
    if ( noErr == err )
    {
        err = LSOpenFSRef( &fsRef , NULL ) ;
    }

    // 0 means execution failed. Returning non-zero is a PID, but not
    // on Mac where PIDs are 64 bits and won't fit in a long, so we
    // return a dummy value for now.
    return ( err == noErr ) ? -1 : 0;
}

#endif

#ifdef __DARWIN__
void wxMAC_MachPortEndProcessDetect(CFMachPortRef port, void *data)
{
	wxEndProcessData *proc_data = (wxEndProcessData*)data;
	wxLogDebug(wxT("Wow.. this actually worked!"));
	int status = 0;
	int rc = waitpid(abs(proc_data->pid), &status, WNOHANG);
	if(!rc)
	{
		wxLogDebug(wxT("Mach port was invalidated, but process hasn't terminated!"));
		return;
	}
	if((rc != -1) && WIFEXITED(status))
		proc_data->exitcode = WEXITSTATUS(status);
	else
		proc_data->exitcode = -1;
	wxHandleProcessTermination(proc_data);
}

int wxAddProcessCallbackForPid(wxEndProcessData *proc_data, int pid)
{
    if(pid < 1)
        return -1;
    kern_return_t    kernResult;
    mach_port_t    taskOfOurProcess;
    mach_port_t    machPortForProcess;
    taskOfOurProcess = mach_task_self();
    if(taskOfOurProcess == MACH_PORT_NULL)
    {
        wxLogDebug(wxT("No mach_task_self()"));
        return -1;
    }
    wxLogDebug(wxT("pid=%d"),pid);
    kernResult = task_for_pid(taskOfOurProcess,pid, &machPortForProcess);
    if(kernResult != KERN_SUCCESS)
    {
        wxLogDebug(wxT("no task_for_pid()"));
        // try seeing if it is already dead or something
        // FIXME: a better method would be to call the callback function
        // from idle time until the process terminates. Of course, how
        // likely is it that it will take more than 0.1 seconds for the
        // mach terminate event to make its way to the BSD subsystem?
        usleep(100); // sleep for 0.1 seconds
        wxMAC_MachPortEndProcessDetect(NULL, (void*)proc_data);
        return -1;
    }
    CFMachPortContext termcb_contextinfo;
    termcb_contextinfo.version = NULL;
    termcb_contextinfo.info = (void*)proc_data;
    termcb_contextinfo.retain = NULL;
    termcb_contextinfo.release = NULL;
    termcb_contextinfo.copyDescription = NULL;
    CFMachPortRef    CFMachPortForProcess;
    Boolean        ShouldFreePort;
    CFMachPortForProcess = CFMachPortCreateWithPort(NULL, machPortForProcess, NULL, &termcb_contextinfo, &ShouldFreePort);
    if(!CFMachPortForProcess)
    {
        wxLogDebug(wxT("No CFMachPortForProcess"));
        mach_port_deallocate(taskOfOurProcess, machPortForProcess);
        return -1;
    }
    if(ShouldFreePort)
    {
        kernResult = mach_port_deallocate(taskOfOurProcess, machPortForProcess);
        if(kernResult!=KERN_SUCCESS)
        {
            wxLogDebug(wxT("Couldn't deallocate mach port"));
            return -1;
        }
    }
    CFMachPortSetInvalidationCallBack(CFMachPortForProcess, &wxMAC_MachPortEndProcessDetect);
    CFRunLoopSourceRef    runloopsource;
    runloopsource = CFMachPortCreateRunLoopSource(NULL,CFMachPortForProcess, (CFIndex)0);
    if(!runloopsource)
    {
        wxLogDebug(wxT("Couldn't create runloopsource"));
        return -1;
    }
    
    CFRelease(CFMachPortForProcess);

    CFRunLoopAddSource(CFRunLoopGetCurrent(),runloopsource,kCFRunLoopDefaultMode);
    CFRelease(runloopsource);
    wxLogDebug(wxT("Successfully added notification to the runloop"));
    return 0;
}
#endif
