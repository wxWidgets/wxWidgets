/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/utilsexec.cpp
// Purpose:     Execution-related utilities
// Author:      Stefan Csomor
// Modified by: David Elliott
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/utils.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
#endif

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
#define wxEXECUTE_WIN_MESSAGE 10000

#include "wx/mac/private.h"

/*
Below FinderLaunch function comes from:
http://developer.apple.com/technotes/tn/tn1002.html#fndrask
*/
    /* FinderLaunch converts a list of nTargets FSSpec records
    pointed to by the targetList parameter and converts the
    list to an Apple Event.  It then sends that event to the
    Finder.  The array of FSSpec records pointed to by the
    targetList parameter may contain references to files,
    folders, or applications.  The net effect of this command
    is equivalent to the user selecting an icon in one of the
    Finder's windows and then choosing the open command from
    the Finder's file menu. */
static OSErr FinderLaunch(long nTargets, FSSpec *targetList) {
    OSErr err;
    AppleEvent theAEvent, theReply;
    AEAddressDesc fndrAddress;
    AEDescList targetListDesc;
    OSType fndrCreator;
    Boolean wasChanged;
    AliasHandle targetAlias;
    long index;

        /* set up locals  */
    AECreateDesc(typeNull, NULL, 0, &theAEvent);
    AECreateDesc(typeNull, NULL, 0, &fndrAddress);
    AECreateDesc(typeNull, NULL, 0, &theReply);
    AECreateDesc(typeNull, NULL, 0, &targetListDesc);
    targetAlias = NULL;
    fndrCreator = 'MACS';

        /* verify parameters */
    if ((nTargets == 0) || (targetList == NULL)) {
        err = paramErr;
        goto bail;
    }

        /* create an open documents event targeting the
        finder */
    err = AECreateDesc(typeApplSignature, (Ptr) &fndrCreator,
        sizeof(fndrCreator), &fndrAddress);
    if (err != noErr) goto bail;
    err = AECreateAppleEvent(kCoreEventClass, kAEOpenDocuments,
        &fndrAddress, kAutoGenerateReturnID,
        kAnyTransactionID, &theAEvent);
    if (err != noErr) goto bail;

        /* create the list of files to open */
    err = AECreateList(NULL, 0, false, &targetListDesc);
    if (err != noErr) goto bail;
    for ( index=0; index < nTargets; index++) {
        if (targetAlias == NULL)
            err = NewAlias(NULL, (targetList + index),
                  &targetAlias);
        else err = UpdateAlias(NULL, (targetList + index),
                   targetAlias, &wasChanged);
        if (err != noErr) goto bail;
        HLock((Handle) targetAlias);
        err = AEPutPtr(&targetListDesc, (index + 1),
              typeAlias, *targetAlias,
              GetHandleSize((Handle) targetAlias));
        HUnlock((Handle) targetAlias);
        if (err != noErr) goto bail;
    }

        /* add the file list to the Apple Event */
    err = AEPutParamDesc(&theAEvent, keyDirectObject,
          &targetListDesc);
    if (err != noErr) goto bail;

        /* send the event to the Finder */
    err = AESend(&theAEvent, &theReply, kAENoReply,
        kAENormalPriority, kAEDefaultTimeout, NULL, NULL);

        /* clean up and leave */
bail:
    if (targetAlias != NULL) DisposeHandle((Handle) targetAlias);
    AEDisposeDesc(&targetListDesc);
    AEDisposeDesc(&theAEvent);
    AEDisposeDesc(&fndrAddress);
    AEDisposeDesc(&theReply);
    return err;
}

long wxExecute(const wxString& command, int flags, wxProcess *WXUNUSED(handler))
{
    wxASSERT_MSG( flags == wxEXEC_ASYNC,
        wxT("wxExecute: Only wxEXEC_ASYNC is supported") );

    FSSpec fsSpec;
    wxMacFilename2FSSpec(command, &fsSpec);

    // 0 means execution failed. Returning non-zero is a PID, but not
    // on Mac where PIDs are 64 bits and won't fit in a long, so we
    // return a dummy value for now.
    return ( FinderLaunch(1 /*one file*/, &fsSpec) == noErr ) ? -1 : 0;
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
