/////////////////////////////////////////////////////////////////////////////
// Name:        utilsexec.mm
// Purpose:     Execution-related utilities for wxCocoa
// Author:      Ryan Norton (carbon darwin version based off of Stefan's code)
// Modified by:
// Created:     2004-10-05
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:   	wxWindows licence
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

long wxExecute(const wxString& command, int flags, wxProcess *WXUNUSED(handler))
{
    wxFAIL_MSG( _T("wxExecute() not yet implemented") );
    return 0;
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

/*
#ifdef __GNUG__
#pragma implementation
#endif

#include "wx/utils.h"

#include "wx/process.h"
#include "wx/stream.h"

#include "wx/cocoa/string.h"

#import <Foundation/Foundation.h>
#import <AppKit/NSWorkspace.h>

//
// RN:  This is a prelimenary implementation - simple
// launching and process redirection works,
// but with the piping tests in the exec sample
// SIGPIPE is triggered...
//

class wxPipeInputStream : public wxInputStream
{
public:
    wxPipeInputStream(NSPipe* thePipe) : 
            m_thePipe(thePipe),
            m_theHandle([m_thePipe fileHandleForReading])
    {
    }

    ~wxPipeInputStream()
    {	
        [m_thePipe release];
    }

protected:
    virtual size_t OnSysRead(void *buffer, size_t size)
    {
        NSData* theData = [m_theHandle readDataOfLength:size];
        memcpy(buffer, [theData bytes], [theData length]);
        return [theData length];
    }
    
    
    NSPipe* 		m_thePipe;
    NSFileHandle*	m_theHandle;
};

class wxPipeOutputStream : public wxOutputStream
{
public:
    wxPipeOutputStream(NSPipe* thePipe) : 
            m_thePipe(thePipe),
            m_theHandle([m_thePipe fileHandleForWriting])
    {
    }

    ~wxPipeOutputStream()
    {	
        [m_thePipe release];
    }

protected:

    virtual size_t OnSysWrite(const void *buffer, size_t bufsize)
    {
        NSData* theData = [NSData dataWithBytesNoCopy:(void*)buffer 
                                  length:bufsize];
        [m_theHandle writeData:theData];
        return bufsize;
    }
        
    NSPipe* 		m_thePipe;
    NSFileHandle*	m_theHandle;
};

@interface wxTaskHandler : NSObject
{
    long m_pid;
    void* m_handle;
}
-(id)init:(void*)handle processIdentifier:(long)pid;
- (void)termHandler:(NSNotification *)aNotification;
@end

@implementation wxTaskHandler : NSObject

-(id)init:(void*)handle processIdentifier:(long)pid 
{
    self = [super init];
    
    m_handle = handle;
    m_pid = pid;

    [[NSNotificationCenter defaultCenter] addObserver:self 
            selector:@selector(termHandler:) 
            name:NSTaskDidTerminateNotification 
            object:nil];
    return self;
}

- (void)termHandler:(NSNotification *)aNotification 
{
    NSTask* theTask = [aNotification object];
    
    if ([theTask processIdentifier] == m_pid)
    {
        ((wxProcess*)m_handle)->OnTerminate([theTask processIdentifier], 
                          [theTask terminationStatus]);
        
        [self release];
    }
}

@end

long wxExecute(const wxString& command, 
                int sync, 
                wxProcess *handle)
{
    NSTask* theTask = [[NSTask alloc] init];
    
    if (handle && handle->IsRedirected())
    {
        NSPipe* theStdinPipe = [[NSPipe alloc] init];
        NSPipe* theStderrPipe = [[NSPipe alloc] init];
        NSPipe* theStdoutPipe = [[NSPipe alloc] init];
    
        [theTask setStandardInput:theStdinPipe];
        [theTask setStandardError:theStderrPipe];
        [theTask setStandardOutput:theStdoutPipe];
        
        handle->SetPipeStreams(new wxPipeInputStream(theStdoutPipe),
                               new wxPipeOutputStream(theStdinPipe),
                               new wxPipeInputStream(theStderrPipe) );
    }
    
    NSArray* theQuoteArguments = 
        [wxNSStringWithWxString(command) componentsSeparatedByString:@"\""];
        
    NSMutableArray* theSeperatedArguments = 
        [NSMutableArray arrayWithCapacity:10];
        
    for (unsigned i = 0; i < [theQuoteArguments count]; ++i)
    {
        [theSeperatedArguments addObjectsFromArray:
            [[theQuoteArguments objectAtIndex:i] componentsSeparatedByString:@" "]
        ];
        
        if(++i < [theQuoteArguments count])
            [theSeperatedArguments addObject:[theQuoteArguments objectAtIndex:i]];
    }
    
    [theTask setLaunchPath:[theSeperatedArguments objectAtIndex:0]];
    [theTask setArguments:theSeperatedArguments];
    [theTask launch];
    
    if(sync & wxEXEC_ASYNC)
    {
        [[wxTaskHandler alloc]init:handle 
                              processIdentifier:[theTask processIdentifier]];
                                
        return 0;
    }
    else
    {
        [theTask waitUntilExit];
        
        return [theTask terminationStatus];
    }                      
}

*/