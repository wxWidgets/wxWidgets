/////////////////////////////////////////////////////////////////////////////
// Name:        utilsexec.cpp
// Purpose:     Various utilities
// Author:      David Webster
// Modified by:
// Created:     10/17/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/setup.h"
#include "wx/utils.h"
#include "wx/app.h"
#include "wx/intl.h"
#endif

#include "wx/log.h"

#include "wx/process.h"

#include "wx/os2/private.h"

#define INCL_DOS
#include <os2.h>

#include <ctype.h>
#include <direct.h>

#include <sys/stat.h>
#include <io.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

// this message is sent when the process we're waiting for terminates
#define wxWM_PROC_TERMINATED (WM_USER + 10000)

// structure describing the process we're being waiting for
struct wxExecuteData
{
public:
    ~wxExecuteData()
    {
// TODO:
/*
        if ( !::CloseHandle(hProcess) )
        {
            wxLogLastError("CloseHandle(hProcess)");
        }
*/
    }

    HWND       hWnd;          // window to send wxWM_PROC_TERMINATED to
    HANDLE     hProcess;      // handle of the process
    DWORD      dwProcessId;   // pid of the process
    wxProcess *handler;
    DWORD      dwExitCode;    // the exit code of the process
    bool       state;         // set to FALSE when the process finishes
};


static DWORD wxExecuteThread(wxExecuteData *data)
{
// TODO:
/*
    WaitForSingleObject(data->hProcess, INFINITE);

    // get the exit code
    if ( !GetExitCodeProcess(data->hProcess, &data->dwExitCode) )
    {
        wxLogLastError("GetExitCodeProcess");
    }

    wxASSERT_MSG( data->dwExitCode != STILL_ACTIVE,
                  wxT("process should have terminated") );

    // send a message indicating process termination to the window
    SendMessage(data->hWnd, wxWM_PROC_TERMINATED, 0, (LPARAM)data);
*/
    return 0;
}

// window procedure of a hidden window which is created just to receive
// the notification message when a process exits
MRESULT APIENTRY wxExecuteWindowCbk(HWND hWnd, UINT message,
                                    MPARAM wParam, MPARAM lParam)
{
    if ( message == wxWM_PROC_TERMINATED )
    {
//        DestroyWindow(hWnd);    // we don't need it any more

        wxExecuteData *data = (wxExecuteData *)lParam;
        if ( data->handler )
        {
            data->handler->OnTerminate((int)data->dwProcessId,
                                       (int)data->dwExitCode);
        }

        if ( data->state )
        {
            // we're executing synchronously, tell the waiting thread
            // that the process finished
            data->state = 0;
        }
        else
        {
            // asynchronous execution - we should do the clean up
            delete data;
        }
    }

    return 0;
}

extern wxChar wxPanelClassName[];

long wxExecute(const wxString& command, bool sync, wxProcess *handler)
{
    wxCHECK_MSG( !!command, 0, wxT("empty command in wxExecute") );

    // the old code is disabled because we really need a process handle
    // if we want to execute it asynchronously or even just get its
    // return code and for this we must use CreateProcess() and not
    // ShellExecute()

    // create the process
// TODO:
/*
    STARTUPINFO si;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);

    PROCESS_INFORMATION pi;

    if ( ::CreateProcess(
                         NULL,       // application name (use only cmd line)
                         (wxChar *)command.c_str(),  // full command line
                         NULL,       // security attributes: defaults for both
                         NULL,       //   the process and its main thread
                         FALSE,      // don't inherit handles
                         CREATE_DEFAULT_ERROR_MODE,  // flags
                         NULL,       // environment (use the same)
                         NULL,       // current directory (use the same)
                         &si,        // startup info (unused here)
                         &pi         // process info
                        ) == 0 )
    {
        wxLogSysError(_("Execution of command '%s' failed"), command.c_str());

        return 0;
    }

    // close unneeded handle
    if ( !::CloseHandle(pi.hThread) )
        wxLogLastError("CloseHandle(hThread)");

    // create a hidden window to receive notification about process
    // termination
    HWND hwnd = ::CreateWindow(wxPanelClassName, NULL, 0, 0, 0, 0, 0, NULL,
                               (HMENU)NULL, wxGetInstance(), 0);
    wxASSERT_MSG( hwnd, wxT("can't create a hidden window for wxExecute") );

    FARPROC ExecuteWindowInstance = MakeProcInstance((FARPROC)wxExecuteWindowCbk,
                                                     wxGetInstance());

    ::SetWindowLong(hwnd, GWL_WNDPROC, (LONG) ExecuteWindowInstance);

    // Alloc data
    wxExecuteData *data = new wxExecuteData;
    data->hProcess    = pi.hProcess;
    data->dwProcessId = pi.dwProcessId;
    data->hWnd        = hwnd;
    data->state       = sync;
    if ( sync )
    {
        wxASSERT_MSG( !handler, wxT("wxProcess param ignored for sync execution") );

        data->handler = NULL;
    }
    else
    {
        // may be NULL or not
        data->handler = handler;
    }

    DWORD tid;
    HANDLE hThread = ::CreateThread(NULL,
                                    0,
                                    (LPTHREAD_START_ROUTINE)wxExecuteThread,
                                    (void *)data,
                                    0,
                                    &tid);

    if ( !hThread )
    {
        wxLogLastError("CreateThread in wxExecute");

        DestroyWindow(hwnd);
        delete data;

        // the process still started up successfully...
        return pi.dwProcessId;
    }

    if ( !sync )
    {
        // clean up will be done when the process terminates

        // return the pid
        return pi.dwProcessId;
    }

    // waiting until command executed
    while ( data->state )
        wxYield();

    DWORD dwExitCode = data->dwExitCode;
    delete data;

    // return the exit code
    return dwExitCode;
*/
    return 0;
}

long wxExecute(char **argv, bool sync, wxProcess *handler)
{
    wxString command;

    while ( *argv != NULL )
    {
        command << *argv++ << ' ';
    }

    command.RemoveLast();

    return wxExecute(command, sync, handler);
}

bool wxGetFullHostName(wxChar *buf, int maxSize)
{
    DWORD nSize = maxSize ;
// TODO:
/*
    if ( !::GetComputerName(buf, &nSize) )
    {
        wxLogLastError("GetComputerName");

        return FALSE;
    }
*/
    return TRUE;
}

