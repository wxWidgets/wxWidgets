/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/corefoundation/utilsexec_cf.cpp
// Purpose:     Execution-related utilities for Darwin
// Author:      David Elliott
// Modified by: Stefan Csomor (added necessary wxT for unicode builds)
// Created:     2004-11-04
// RCS-ID:      $Id$
// Copyright:   (c) David Elliott
// Licence:     wxWindows licence
// Notes:       This code comes from src/mac/carbon/utilsexc.cpp,1.11
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/utils.h"
#endif //ndef WX_PRECOMP
#include "wx/unix/execute.h"

#include <CoreFoundation/CFMachPort.h>
#include <sys/wait.h>
extern "C" {
#include <mach/mach.h>
}

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

