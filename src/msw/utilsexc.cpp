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

class wxPipeInputStream: public wxInputStream {
public:
    wxPipeInputStream(HANDLE hInput);
    ~wxPipeInputStream();

protected:
    size_t OnSysRead(void *buffer, size_t len);

protected:
    HANDLE m_hInput;
};

class wxPipeOutputStream: public wxOutputStream {
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

size_t wxPipeInputStream::OnSysRead(void *buffer, size_t len)
{
    DWORD bytesRead;

    m_lasterror = wxSTREAM_NOERROR;
    if (! ::ReadFile(m_hInput, buffer, len, &bytesRead, NULL) ) {
        if (GetLastError() == ERROR_BROKEN_PIPE)
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
    if (! ::WriteFile(m_hOutput, buffer, len, &bytesRead, NULL) ) {
        if (GetLastError() == ERROR_BROKEN_PIPE)
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
    // of command: WX_DDE:<command>:DDE_SERVER:DDE_TOPIC:DDE_COMMAND in which
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
    }
    else
#endif // wxUSE_IPC
    {
        // no DDE
        command = cmd;
    }

#if defined(__WIN32__) && !defined(__TWIN32__)
    // the old code is disabled because we really need a process handle
    // if we want to execute it asynchronously or even just get its
    // return code and for this we must use CreateProcess() and not
    // ShellExecute()
#if 0
    // isolate command and arguments
    wxString commandName;
    bool insideQuotes = FALSE;
    const char *pc;
    for ( pc = command.c_str(); *pc != '\0'; pc++ )
    {
        switch ( *pc )
        {
            case ' ':
            case '\t':
                if ( !insideQuotes )
                    break;
                // fall through

            case '"':
                insideQuotes = !insideQuotes;
                // fall through

            default:
                commandName += *pc;
                continue;   // skip the next break
        }

        // only reached for space not inside quotes
        break;
    }
    wxString commandArgs = pc;

    wxWindow *winTop = wxTheApp->GetTopWindow();
    HWND hwndTop = (HWND)(winTop ? winTop->GetHWND() : 0);

    HANDLE result;
#ifdef __GNUWIN32__
    result = ShellExecute(hwndTop,
                          (const wchar_t)"open",
                          (const wchar_t)commandName,
                          (const wchar_t)commandArgs,
                          (const wchar_t)NULL,
                          SW_SHOWNORMAL);
#else // !GNUWIN32
    result = ShellExecute(hwndTop, "open", commandName,
                          commandArgs, NULL, SW_SHOWNORMAL);
#endif // GNUWIN32

    if ( ((long)result) <= 32 )
        wxLogSysError(_("Can't execute command '%s'"), command.c_str());

    return result;
#else // 1

    HANDLE hpipeRead[2];
    HANDLE hpipeWrite[2];
    HANDLE hStdIn = INVALID_HANDLE_VALUE;
    HANDLE hStdOut = INVALID_HANDLE_VALUE;

    // we need to inherit handles in the child process if we want to redirect
    // its IO
    BOOL inheritHandles = FALSE;

    // open the pipes to which child process IO will be redirected if needed
    if ( handler && handler->IsRedirected() )
    {
        SECURITY_ATTRIBUTES security;

        security.nLength              = sizeof(security);
        security.lpSecurityDescriptor = NULL;
        security.bInheritHandle       = TRUE;

        if ( !::CreatePipe(&hpipeRead[0], &hpipeRead[1], &security, 0) )
        {
            wxLogSysError(_("Can't create the inter-process read pipe"));

            return 0;
        }

        if ( !::CreatePipe(&hpipeWrite[0], &hpipeWrite[1], &security, 0) )
        {
            ::CloseHandle(hpipeRead[0]);
            ::CloseHandle(hpipeRead[1]);

            wxLogSysError(_("Can't create the inter-process write pipe"));

            return 0;
        }

        // We need to save the old stdio handles to restore them after the call
        // to CreateProcess
        hStdIn  = ::GetStdHandle(STD_INPUT_HANDLE);
        hStdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);

        if ( !::SetStdHandle(STD_INPUT_HANDLE, hpipeRead[0]) ||
             !::SetStdHandle(STD_OUTPUT_HANDLE, hpipeWrite[1]) )
        {
            wxLogDebug(_T("Failed to change stdin/out handles"));
        }

        inheritHandles = TRUE;
    }

    // create the process
    STARTUPINFO si;
    wxZeroMemory(si);

    si.cb = sizeof(si);

    PROCESS_INFORMATION pi;

    if ( ::CreateProcess(
                         NULL,       // application name (use only cmd line)
                         (wxChar *)command.c_str(),  // full command line
                         NULL,       // security attributes: defaults for both
                         NULL,       //   the process and its main thread
                         inheritHandles,      // inherit handles if we need pipes
                         CREATE_DEFAULT_ERROR_MODE |
                         CREATE_SUSPENDED,           // flags
                         NULL,       // environment (use the same)
                         NULL,       // current directory (use the same)
                         &si,        // startup info (unused here)
                         &pi         // process info
                        ) == 0 )
    {
        if ( inheritHandles )
        {
            ::CloseHandle(hpipeWrite[0]);
            ::CloseHandle(hpipeWrite[1]);
            ::CloseHandle(hpipeRead[0]);
            ::CloseHandle(hpipeRead[1]);
        }

        wxLogSysError(_("Execution of command '%s' failed"), command.c_str());

        return 0;
    }

    // Restore the old stdio handles
    if ( inheritHandles )
    {
        if ( !::SetStdHandle(STD_INPUT_HANDLE, hStdIn) ||
             !::SetStdHandle(STD_OUTPUT_HANDLE, hStdOut) )
        {
            wxLogDebug(_T("Failed to restore old stdin/out handles"));
        }

        // they're still opened in child process
        ::CloseHandle(hpipeWrite[1]);
        ::CloseHandle(hpipeRead[0]);

#if wxUSE_STREAMS
        // We can now initialize the wxStreams
        wxInputStream *inStream = new wxPipeInputStream(hpipeWrite[0]);
        wxOutputStream *outStream = new wxPipeOutputStream(hpipeRead[1]);

        handler->SetPipeStreams(inStream, outStream);
#endif
    }

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

#if wxUSE_IPC
    // second part of DDE hack: now establish the DDE conversation with the
    // just launched process
    if ( !!ddeServer )
    {
        wxDDEClient client;
        wxConnectionBase *conn = client.MakeConnection(_T(""),
                                                       ddeServer,
                                                       ddeTopic);
        if ( !conn || !conn->Execute(ddeCommand) )
        {
            wxLogError(_("Couldn't launch DDE server '%s'."), command.c_str());
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
#endif // 0/1
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
