/////////////////////////////////////////////////////////////////////////////
// Name:        unix/execute.h
// Purpose:     private details of wxExecute() implementation
// Author:      Vadim Zeitlin
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart, Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIX_EXECUTE_H
#define _WX_UNIX_EXECUTE_H

// if pid > 0, the execution is async and the data is freed in the callback
// executed when the process terminates, if pid < 0, the execution is
// synchronous and the caller (wxExecute) frees the data
struct wxEndProcessData
{
    int pid,                // pid of the process
        tag;                // port dependent value
    wxProcess *process;     // if !NULL: notified on process termination
    int  exitcode;          // the exit code
};

// this function is called when the process terminates from port specific
// callback function and is common to all ports (src/unix/utilsunx.cpp)
extern void wxHandleProcessTermination(wxEndProcessData *proc_data);

// this function is called to associate the port-specific callback with the
// child process. The return valus is port-specific.
extern int wxAddProcessCallback(wxEndProcessData *proc_data, int fd);

#endif // _WX_UNIX_EXECUTE_H
