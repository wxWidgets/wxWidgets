/////////////////////////////////////////////////////////////////////////////
// Name:        msw/utilsexec.cpp
// Purpose:     Various utilities
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/intl.h"
#endif

#include "wx/log.h"

#ifdef __WIN32__
    #include "wx/stream.h"
    #include "wx/process.h"
#endif

#include "wx/msw/private.h"

#include <ctype.h>

#if !defined(__GNUWIN32__) && !defined(__WXWINE__) && !defined(__SALFORDC__)
    #include <direct.h>
#ifndef __MWERKS__
    #include <dos.h>
#endif
#endif

#if defined(__GNUWIN32__) && !defined(__TWIN32__)
    #include <sys/unistd.h>
    #include <sys/stat.h>
#endif

#if defined(__WIN32__) && !defined(__WXWINE__)
#include <io.h>

#ifndef __GNUWIN32__
#include <shellapi.h>
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef __WATCOMC__
    #if !(defined(_MSC_VER) && (_MSC_VER > 800))
        #include <errno.h>
    #endif
#endif
#include <stdarg.h>

#if wxUSE_IPC
    #include "wx/dde.h"         // for WX_DDE hack in wxExecute
#endif // wxUSE_IPC

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// this message is sent when the process we're waiting for terminates
#define wxWM_PROC_TERMINATED (WM_USER + 10000)

// ----------------------------------------------------------------------------
// this module globals
// ----------------------------------------------------------------------------

// we need to create a hidden window to receive the process termination
// notifications and for this we need a (Win) class name for it which we will
// register the first time it's needed
static const wxChar *gs_classForHiddenWindow = NULL;

// ----------------------------------------------------------------------------
// private types
// ----------------------------------------------------------------------------

// structure describing the process we're being waiting for
struct wxExecuteData
{
public:
    ~wxExecuteData()
    {
#ifndef __WIN16__
        if ( !::CloseHandle(hProcess) )
        {
            wxLogLastError(wxT("CloseHandle(hProcess)"));
        }
#endif
    }

    HWND       hWnd;          // window to send wxWM_PROC_TERMINATED to
    HANDLE     hProcess;      // handle of the process
    DWORD      dwProcessId;   // pid of the process
    wxProcess *handler;
    DWORD      dwExitCode;    // the exit code of the process
    bool       state;         // set to FALSE when the process finishes
};

#if defined(__WIN32__) && wxUSE_STREAMS

// ----------------------------------------------------------------------------
// wxPipeStreams
// ----------------------------------------------------------------------------

class wxPipeInputStream: public wxInputStream
{
public:
    wxPipeInputStream(HANDLE hInput);
    ~wxPipeInputStream();

    virtual bool Eof() const;

protected:
    size_t OnSysRead(void *buffer, size_t len);

protected:
    HANDLE m_hInput;
};

class wxPipeOutputStream: public wxOutputStream
{
public:
    wxPipeOutputStream(HANDLE hOutput);
    ~wxPipeOutputStream();

protected:
    size_t OnSysWrite(const void *buffer, size_t len);

protected:
    HANDLE m_hOutput;
};

// ==================
// wxPipeInputStream
// ==================

wxPipeInputStream::wxPipeInputStream(HANDLE hInput)
{
    m_hInput = hInput;
}

wxPipeInputStream::~wxPipeInputStream()
{
    ::CloseHandle(m_hInput);
}

bool wxPipeInputStream::Eof() const
{
    DWORD nAvailable;

    // function name is misleading, it works with anon pipes as well
    DWORD rc = ::PeekNamedPipe
                    (
                      m_hInput,     // handle
                      NULL, 0,      // ptr to buffer and its size
                      NULL,         // [out] bytes read
                      &nAvailable,  // [out] bytes available
                      NULL          // [out] bytes left
                    );

    if ( !rc )
    {
        if ( ::GetLastError() != ERROR_BROKEN_PIPE )
        {
            // unexpected error
            wxLogLastError(_T("PeekNamedPipe"));
        }

        // don't try to continue reading from a pipe if an error occured or if
        // it had been closed
        return TRUE;
    }
    else
    {
        return nAvailable == 0;
    }
}

size_t wxPipeInputStream::OnSysRead(void *buffer, size_t len)
{
    // reading from a pipe may block if there is no more data, always check for
    // EOF first
    m_lasterror = wxSTREAM_NOERROR;
    if ( Eof() )
        return 0;

    DWORD bytesRead;
    if ( !::ReadFile(m_hInput, buffer, len, &bytesRead, NULL) )
    {
        if ( ::GetLastError() == ERROR_BROKEN_PIPE )
            m_lasterror = wxSTREAM_EOF;
        else
            m_lasterror = wxSTREAM_READ_ERROR;
    }

    return bytesRead;
}

// ==================
// wxPipeOutputStream
// ==================

wxPipeOutputStream::wxPipeOutputStream(HANDLE hOutput)
{
    m_hOutput = hOutput;
}

wxPipeOutputStream::~wxPipeOutputStream()
{
    ::CloseHandle(m_hOutput);
}

size_t wxPipeOutputStream::OnSysWrite(const void *buffer, size_t len)
{
    DWORD bytesRead;

    m_lasterror = wxSTREAM_NOERROR;
    if ( !::WriteFile(m_hOutput, buffer, len, &bytesRead, NULL) )
    {
        if ( ::GetLastError() == ERROR_BROKEN_PIPE )
            m_lasterror = wxSTREAM_EOF;
        else
            m_lasterror = wxSTREAM_READ_ERROR;
    }

    return bytesRead;
}

#endif // __WIN32__

// ============================================================================
// implementation
// ============================================================================

#ifdef __WIN32__

static DWORD wxExecuteThread(wxExecuteData *data)
{
    WaitForSingleObject(data->hProcess, INFINITE);

    // get the exit code
    if ( !GetExitCodeProcess(data->hProcess, &data->dwExitCode) )
    {
        wxLogLastError(wxT("GetExitCodeProcess"));
    }

    wxASSERT_MSG( data->dwExitCode != STILL_ACTIVE,
                  wxT("process should have terminated") );

    // send a message indicating process termination to the window
    SendMessage(data->hWnd, wxWM_PROC_TERMINATED, 0, (LPARAM)data);

    return 0;
}

// window procedure of a hidden window which is created just to receive
// the notification message when a process exits
LRESULT APIENTRY _EXPORT wxExecuteWindowCbk(HWND hWnd, UINT message,
                                            WPARAM wParam, LPARAM lParam)
{
    if ( message == wxWM_PROC_TERMINATED )
    {
        DestroyWindow(hWnd);    // we don't need it any more

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

        return 0;
    }
    else
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}
#endif // Win32

long wxExecute(const wxString& cmd, bool sync, wxProcess *handler)
{
    wxCHECK_MSG( !!cmd, 0, wxT("empty command in wxExecute") );

    wxString command;

#if wxUSE_IPC
    // DDE hack: this is really not pretty, but we need to allow this for
    // transparent handling of DDE servers in wxMimeTypesManager. Usually it
    // returns the command which should be run to view/open/... a file of the
    // given type. Sometimes, however, this command just launches the server
    // and an additional DDE request must be made to really open the file. To
    // keep all this well hidden from the application, we allow a special form
    // of command: WX_DDE#<command>#DDE_SERVER#DDE_TOPIC#DDE_COMMAND in which
    // case we execute just <command> and process the rest below
    wxString ddeServer, ddeTopic, ddeCommand;
    static const size_t lenDdePrefix = 7;   // strlen("WX_DDE:")
    if ( cmd.Left(lenDdePrefix) == _T("WX_DDE#") )
    {
        const wxChar *p = cmd.c_str() + 7;
        while ( *p && *p != _T('#') )
        {
            command += *p++;
        }

        if ( *p )
        {
            // skip '#'
            p++;
        }
        else
        {
            wxFAIL_MSG(_T("invalid WX_DDE command in wxExecute"));
        }

        while ( *p && *p != _T('#') )
        {
            ddeServer += *p++;
        }

        if ( *p )
        {
            // skip '#'
            p++;
        }
        else
        {
            wxFAIL_MSG(_T("invalid WX_DDE command in wxExecute"));
        }

        while ( *p && *p != _T('#') )
        {
            ddeTopic += *p++;
        }

        if ( *p )
        {
            // skip '#'
            p++;
        }
        else
        {
            wxFAIL_MSG(_T("invalid WX_DDE command in wxExecute"));
        }

        while ( *p )
        {
            ddeCommand += *p++;
        }

        // maybe we don't have to launch the DDE server at all - if it is
        // already running, for example
        wxDDEClient client;
        wxLogNull nolog;
        wxConnectionBase *conn = client.MakeConnection(_T(""),
                                                       ddeServer,
                                                       ddeTopic);
        if ( conn )
        {
            // FIXME we don't check the return code as for some strange reason
            //       it will sometimes be FALSE - it is probably a bug in our
            //       DDE code but I don't see anything wrong there
            (void)conn->Execute(ddeCommand);

            // ok, the command executed - return value indicating success,
            // making it up for async case as we really don't have any way to
            // get the real PID of the DDE server here
            return sync ? 0 : -1;
        }
        //else: couldn't establish DDE conversation, now try launching the app
        //      and sending the DDE request again
    }
    else
#endif // wxUSE_IPC
    {
        // no DDE
        command = cmd;
    }

#if defined(__WIN32__) && !defined(__TWIN32__)

    // the IO redirection is only supported with wxUSE_STREAMS
    BOOL redirect = FALSE;
#if wxUSE_STREAMS
    // the first elements are reading ends, the second are the writing ones
    HANDLE hpipeStdin[2],
           hpipeStdinWrite = INVALID_HANDLE_VALUE,
           hpipeStdout[2],
           hpipeStderr[2];

    // open the pipes to which child process IO will be redirected if needed
    if ( handler && handler->IsRedirected() )
    {
        // default secutiry attributes
        SECURITY_ATTRIBUTES security;

        security.nLength              = sizeof(security);
        security.lpSecurityDescriptor = NULL;
        security.bInheritHandle       = TRUE;

        // create stdin pipe
        if ( !::CreatePipe(&hpipeStdin[0], &hpipeStdin[1], &security, 0) )
        {
            wxLogSysError(_("Can't create the inter-process read pipe"));

            // indicate failure in both cases
            return sync ? -1 : 0;
        }

        // and a stdout one
        if ( !::CreatePipe(&hpipeStdout[0], &hpipeStdout[1], &security, 0) )
        {
            ::CloseHandle(hpipeStdin[0]);
            ::CloseHandle(hpipeStdin[1]);

            wxLogSysError(_("Can't create the inter-process write pipe"));

            return sync ? -1 : 0;
        }

        (void)::CreatePipe(&hpipeStderr[0], &hpipeStderr[1], &security, 0);

        redirect = TRUE;
    }
#endif // wxUSE_STREAMS

    // create the process
    STARTUPINFO si;
    wxZeroMemory(si);
    si.cb = sizeof(si);

#if wxUSE_STREAMS
    if ( redirect )
    {
        // when the std IO is redirected, we don't show the (console) process
        // window
        si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;

        si.hStdInput = hpipeStdin[0];
        si.hStdOutput = hpipeStdout[1];
        si.hStdError = hpipeStderr[1];

        si.wShowWindow = SW_HIDE;

        // we must duplicate the handle to the write side of stdin pipe to make
        // it non inheritable: indeed, we must close hpipeStdin[1] before
        // launching the child process as otherwise this handle will be
        // inherited by the child which will never close it and so the pipe
        // will never be closed and the child will be left stuck in ReadFile()
        if ( !::DuplicateHandle
                (
                    GetCurrentProcess(),
                    hpipeStdin[1],
                    GetCurrentProcess(),
                    &hpipeStdinWrite,
                    0,                      // desired access: unused here
                    FALSE,                  // not inherited
                    DUPLICATE_SAME_ACCESS   // same access as for src handle
                ) )
        {
            wxLogLastError(_T("DuplicateHandle"));
        }

        ::CloseHandle(hpipeStdin[1]);
    }
#endif // wxUSE_STREAMS

    PROCESS_INFORMATION pi;
    DWORD dwFlags = CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED;

    bool ok = ::CreateProcess
                (
                 NULL,              // application name (use only cmd line)
                 (wxChar *)
                 command.c_str(),   // full command line
                 NULL,              // security attributes: defaults for both
                 NULL,              //   the process and its main thread
                 redirect,          // inherit handles if we use pipes
                 dwFlags,           // process creation flags
                 NULL,              // environment (use the same)
                 NULL,              // current directory (use the same)
                 &si,               // startup info (unused here)
                 &pi                // process info
                ) != 0;

#if wxUSE_STREAMS
    // we can close the pipe ends used by child anyhow
    if ( redirect )
    {
        ::CloseHandle(hpipeStdin[0]);
        ::CloseHandle(hpipeStdout[1]);
        ::CloseHandle(hpipeStderr[1]);
    }
#endif // wxUSE_STREAMS

    if ( !ok )
    {
#if wxUSE_STREAMS
        // close the other handles too
        if ( redirect )
        {
            ::CloseHandle(hpipeStdout[0]);
            ::CloseHandle(hpipeStderr[0]);
        }
#endif // wxUSE_STREAMS

        wxLogSysError(_("Execution of command '%s' failed"), command.c_str());

        return sync ? -1 : 0;
    }

#if wxUSE_STREAMS
    if ( redirect )
    {
        // We can now initialize the wxStreams
        wxInputStream *inStream = new wxPipeInputStream(hpipeStdout[0]),
                      *errStream = new wxPipeInputStream(hpipeStderr[0]);
        wxOutputStream *outStream = new wxPipeOutputStream(hpipeStdinWrite);

        handler->SetPipeStreams(inStream, outStream, errStream);
    }
#endif // wxUSE_STREAMS

    // register the class for the hidden window used for the notifications
    if ( !gs_classForHiddenWindow )
    {
        gs_classForHiddenWindow = _T("wxHiddenWindow");

        WNDCLASS wndclass;
        wxZeroMemory(wndclass);
        wndclass.lpfnWndProc   = (WNDPROC)wxExecuteWindowCbk;
        wndclass.hInstance     = wxGetInstance();
        wndclass.lpszClassName = gs_classForHiddenWindow;

        if ( !::RegisterClass(&wndclass) )
        {
            wxLogLastError(wxT("RegisterClass(hidden window)"));
        }
    }

    // create a hidden window to receive notification about process
    // termination
    HWND hwnd = ::CreateWindow(gs_classForHiddenWindow, NULL,
                               WS_OVERLAPPEDWINDOW,
                               0, 0, 0, 0, NULL,
                               (HMENU)NULL, wxGetInstance(), 0);
    wxASSERT_MSG( hwnd, wxT("can't create a hidden window for wxExecute") );

    // Alloc data
    wxExecuteData *data = new wxExecuteData;
    data->hProcess    = pi.hProcess;
    data->dwProcessId = pi.dwProcessId;
    data->hWnd        = hwnd;
    data->state       = sync;
    if ( sync )
    {
        // handler may be !NULL for capturing program output, but we don't use
        // it wxExecuteData struct in this case
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

    // resume process we created now - whether the thread creation succeeded or
    // not
    if ( ::ResumeThread(pi.hThread) == (DWORD)-1 )
    {
        // ignore it - what can we do?
        wxLogLastError(wxT("ResumeThread in wxExecute"));
    }

    // close unneeded handle
    if ( !::CloseHandle(pi.hThread) )
        wxLogLastError(wxT("CloseHandle(hThread)"));

    if ( !hThread )
    {
        wxLogLastError(wxT("CreateThread in wxExecute"));

        DestroyWindow(hwnd);
        delete data;

        // the process still started up successfully...
        return pi.dwProcessId;
    }

    ::CloseHandle(hThread);

#if wxUSE_IPC
    // second part of DDE hack: now establish the DDE conversation with the
    // just launched process
    if ( !!ddeServer )
    {
        wxDDEClient client;
        wxConnectionBase *conn;

        {
            // try doing it the first time without error messages
            wxLogNull nolog;

            conn = client.MakeConnection(_T(""), ddeServer, ddeTopic);
        }

        if ( !conn )
        {
            // give the app some time to initialize itself: in fact, a common
            // reason for failure is that we tried to open DDE conversation too
            // soon (before the app had time to setup its DDE server), so wait
            // a bit and try again
            ::Sleep(2000);

            wxConnectionBase *conn = client.MakeConnection(_T(""),
                                                           ddeServer,
                                                           ddeTopic);
            if ( !conn )
            {
                wxLogError(_("Couldn't launch DDE server '%s'."), command.c_str());
            }
        }

        if ( conn )
        {
            // FIXME just as above we don't check Execute() return code
            wxLogNull nolog;
            (void)conn->Execute(ddeCommand);
        }
    }
#endif // wxUSE_IPC

    if ( !sync )
    {
        // clean up will be done when the process terminates

        // return the pid
        return pi.dwProcessId;
    }

    // waiting until command executed (disable everything while doing it)
#if wxUSE_GUI
    {
        wxBusyCursor bc;

        wxWindowDisabler wd;
#endif // wxUSE_GUI

    while ( data->state )
        wxYield();

#if wxUSE_GUI
    }
#endif // wxUSE_GUI

    DWORD dwExitCode = data->dwExitCode;
    delete data;

    // return the exit code
    return dwExitCode;
#else // Win16
    long instanceID = WinExec((LPCSTR) WXSTRINGCAST command, SW_SHOW);
    if (instanceID < 32) return(0);

    if (sync) {
        int running;
        do {
            wxYield();
            running = GetModuleUsage((HINSTANCE)instanceID);
        } while (running);
    }

    return(instanceID);
#endif // Win16/32
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

#if wxUSE_GUI

// ----------------------------------------------------------------------------
// Metafile helpers
// ----------------------------------------------------------------------------

extern void PixelToHIMETRIC(LONG *x, LONG *y)
{
    ScreenHDC hdcRef;

    int iWidthMM = GetDeviceCaps(hdcRef, HORZSIZE),
        iHeightMM = GetDeviceCaps(hdcRef, VERTSIZE),
        iWidthPels = GetDeviceCaps(hdcRef, HORZRES),
        iHeightPels = GetDeviceCaps(hdcRef, VERTRES);

    *x *= (iWidthMM * 100);
    *x /= iWidthPels;
    *y *= (iHeightMM * 100);
    *y /= iHeightPels;
}

extern void HIMETRICToPixel(LONG *x, LONG *y)
{
    ScreenHDC hdcRef;

    int iWidthMM = GetDeviceCaps(hdcRef, HORZSIZE),
        iHeightMM = GetDeviceCaps(hdcRef, VERTSIZE),
        iWidthPels = GetDeviceCaps(hdcRef, HORZRES),
        iHeightPels = GetDeviceCaps(hdcRef, VERTRES);

    *x *= iWidthPels;
    *x /= (iWidthMM * 100);
    *y *= iHeightPels;
    *y /= (iHeightMM * 100);
}

#endif // wxUSE_GUI
