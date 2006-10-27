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
#include "wx/apptrait.h"
#include "wx/thread.h"
#include "wx/process.h"

#include <sys/wait.h>

// Use polling instead of Mach ports, which doesn't work on Intel
// due to task_for_pid security issues.

// What's a better test for Intel vs PPC?
#ifdef WORDS_BIGENDIAN
#define USE_POLLING 0
#else
#define USE_POLLING 1
#endif

#if USE_POLLING

#if wxUSE_THREADS
class wxProcessTerminationEventHandler: public wxEvtHandler
{
  public:
    wxProcessTerminationEventHandler(wxEndProcessData* data)
    {
        m_data = data;
        Connect(-1, wxEVT_END_PROCESS, wxProcessEventHandler(wxProcessTerminationEventHandler::OnTerminate));
    }

    void OnTerminate(wxProcessEvent& event)
    {
        Disconnect(-1, wxEVT_END_PROCESS, wxProcessEventHandler(wxProcessTerminationEventHandler::OnTerminate));
        wxHandleProcessTermination(m_data);
        delete this;
    }

    wxEndProcessData* m_data;
};

class wxProcessTerminationThread: public wxThread
{
  public:
    wxProcessTerminationThread(wxEndProcessData* data, wxProcessTerminationEventHandler* handler): wxThread(wxTHREAD_DETACHED)
    {
        m_data = data;
        m_handler = handler;
    }

    virtual void* Entry();

    wxProcessTerminationEventHandler* m_handler;
    wxEndProcessData* m_data;
};

// The problem with this is that we may be examining the
// process e.g. in OnIdle at the point this cleans up the process,
// so we need to delay until it's safe.

void* wxProcessTerminationThread::Entry()
{
    while (true)
    {
        usleep(100);
        int status = 0;
        int rc = waitpid(abs(m_data->pid), & status, WNOHANG);
        if (rc != 0)
        {
            if ((rc != -1) && WIFEXITED(status))
                m_data->exitcode = WEXITSTATUS(status);
            else
                m_data->exitcode = -1;

            wxProcessEvent event;
            wxPostEvent(m_handler, event);

            break;
        }
    }

    return NULL;
}

int wxAddProcessCallbackForPid(wxEndProcessData *proc_data, int pid)
{
    if (pid < 1)
        return -1;

    wxProcessTerminationEventHandler* handler = new wxProcessTerminationEventHandler(proc_data);
    wxProcessTerminationThread* thread = new wxProcessTerminationThread(proc_data, handler);

    if (thread->Create() != wxTHREAD_NO_ERROR)
    {
        wxLogDebug(wxT("Could not create termination detection thread."));
        delete thread;
        delete handler;
        return -1;
    }

    thread->Run();

    return 0;
}
#else // !wxUSE_THREADS
int wxAddProcessCallbackForPid(wxEndProcessData*, int)
{
    wxLogDebug(wxT("Could not create termination detection thread."));
    return -1;
}
#endif // wxUSE_THREADS/!wxUSE_THREADS

#else // !USE_POLLING

#include <CoreFoundation/CFMachPort.h>
extern "C" {
#include <mach/mach.h>
}

void wxMAC_MachPortEndProcessDetect(CFMachPortRef port, void *data)
{
    wxEndProcessData *proc_data = (wxEndProcessData*)data;
    wxLogDebug(wxT("Process ended"));
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
    termcb_contextinfo.version = 0;
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

#endif // USE_POLLING/!USE_POLLING

// NOTE: This doesn't really belong here but this was a handy file to
// put it in because it's already compiled for wxCocoa and wxMac GUI lib.
#if wxUSE_GUI

static wxStandardPathsCF gs_stdPaths;
wxStandardPathsBase& wxGUIAppTraits::GetStandardPaths()
{
    return gs_stdPaths;
}

#endif // wxUSE_GUI

