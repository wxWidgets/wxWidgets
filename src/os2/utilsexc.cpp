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

#ifndef NO_ERROR
#  define NO_ERROR  0
#endif

// structure describing the process we're being waiting for
struct wxExecuteData
{
public:
    ~wxExecuteData()
    {
        DosExit(EXIT_PROCESS, 0);
    }

    HWND                            hWnd;          // window to send wxWM_PROC_TERMINATED to [not used]
    RESULTCODES                     vResultCodes;
    wxProcess*                      pHandler;
    ULONG                           ulExitCode;    // the exit code of the process
    bool                            bState;        // set to FALSE when the process finishes
};

static ULONG wxExecuteThread(
  wxExecuteData*                    pData
)
{
    ULONG                           ulRc;
    PID                             vPidChild;

     ulRc = ::DosWaitChild( DCWA_PROCESSTREE
                          ,DCWW_WAIT
                          ,&pData->vResultCodes
                          ,&vPidChild
                          ,pData->vResultCodes.codeTerminate // process PID to look at
                         );
    if (ulRc != NO_ERROR)
    {
        wxLogLastError("DosWaitChild");
    }
    delete pData;


//    ::WinSendMsg(pData->hWnd, (ULONG)wxWM_PROC_TERMINATED, 0, (MPARAM)pData);
    return 0;
}

// Unlike windows where everything needs a window, console apps in OS/2
// need no windows so this is not ever used
MRESULT APIENTRY wxExecuteWindowCbk(
  HWND                              hWnd
, ULONG                             ulMessage
, MPARAM                            wParam
, MPARAM                            lParam
)
{
    if (ulMessage == wxWM_PROC_TERMINATED)
    {
        wxExecuteData*              pData = (wxExecuteData *)lParam;

        if (pData->pHandler)
        {
            pData->pHandler->OnTerminate( (int)pData->vResultCodes.codeTerminate
                                         ,(int)pData->vResultCodes.codeResult
                                        );
        }

        if (pData->bState)
        {
            // we're executing synchronously, tell the waiting thread
            // that the process finished
            pData->bState = 0;
        }
        else
        {
            // asynchronous execution - we should do the clean up
            delete pData;
        }
        ::WinDestroyWindow(hWnd);    // we don't need it any more
    }
    return 0;
}

extern wxChar wxPanelClassName[];

long wxExecute(
  const wxString&                   rCommand
, bool                              bSync
, wxProcess*                        pHandler
)
{
    wxCHECK_MSG(!!rCommand, 0, wxT("empty command in wxExecute"));

    // create the process
    UCHAR                           vLoadError[CCHMAXPATH] = {0};
    RESULTCODES                     vResultCodes = {0};
    ULONG                           ulExecFlag;
    PSZ                             zArgs = NULL;
    PSZ                             zEnvs = NULL;
    ULONG                           ulWindowId;
    APIRET                          rc;
    PFNWP                           pOldProc;
    TID                             vTID;

    if (bSync)
        ulExecFlag = EXEC_SYNC;
    else
        ulExecFlag = EXEC_ASYNCRESULT;

    if (::DosExecPgm( (PCHAR)vLoadError
                     ,sizeof(vLoadError)
                     ,ulExecFlag
                     ,zArgs
                     ,zEnvs
                     ,&vResultCodes
                     ,rCommand
                    ))
    {
        wxLogSysError(_("Execution of command '%s' failed"), rCommand.c_str());
        return 0;
    }

    // PM does not need non visible object windows to run console child processes
/*
    HWND                            hwnd = ::WinCreateWindow( HWND_DESKTOP
                                                             ,wxPanelClassName
                                                             ,NULL
                                                             ,0
                                                             ,0
                                                             ,0
                                                             ,0
                                                             ,0
                                                             ,NULLHANDLE
                                                             ,NULLHANDLE
                                                             ,ulWindowId
                                                             ,NULL
                                                             ,NULL
                                                            );
    wxASSERT_MSG( hwnd, wxT("can't create a hidden window for wxExecute") );
    pOldProc = ::WinSubclassWindow(hwnd, (PFNWP)&wxExecuteWindowCbk);

*/
    // Alloc data
    wxExecuteData*                  pData = new wxExecuteData;

    pData->vResultCodes = vResultCodes;
    pData->hWnd         = NULLHANDLE;
    pData->bState       = bSync;
    if (bSync)
    {
        wxASSERT_MSG(!pHandler, wxT("wxProcess param ignored for sync execution"));
        pData->pHandler = NULL;
    }
    else
    {
        // may be NULL or not
        pData->pHandler = pHandler;
    }

    rc = ::DosCreateThread( &vTID
                           ,(PFNTHREAD)&wxExecuteThread
                           ,(ULONG)pData
                           ,CREATE_READY|STACK_SPARSE
                           ,8192
                          );
    if (rc != NO_ERROR)
    {
        wxLogLastError("CreateThread in wxExecute");

//      ::WinDestroyWindow(hwnd);
        delete pData;

        // the process still started up successfully...
        return vResultCodes.codeTerminate;
    }
    if (!bSync)
    {
        // clean up will be done when the process terminates

        // return the pid
        return vResultCodes.codeTerminate;
    }
    ::DosWaitThread(&vTID, DCWW_WAIT);

    ULONG ulExitCode = pData->vResultCodes.codeResult;
    delete pData;

    // return the exit code
    return (long)ulExitCode;
}

long wxExecute(
  char**                            ppArgv
, bool                              bSync
, wxProcess*                        pHandler
)
{
    wxString                        sCommand;

    while (*ppArgv != NULL)
    {
        sCommand << *ppArgv++ << ' ';
    }
    sCommand.RemoveLast();
    return wxExecute( sCommand
                     ,bSync
                     ,pHandler
                    );
}

bool wxGetFullHostName(
  wxChar*                           zBuf
, int                               nMaxSize
)
{
#if wxUSE_NET_API
    char                            zServer[256];
    char                            zComputer[256];
    unsigned short                  nLevel = 0;
    unsigned char*                  zBuffer;
    unsigned short                  nBuffer;
    unsigned short*                 pnTotalAvail;

    NetBios32GetInfo( (const unsigned char*)zServer
                     ,(const unsigned char*)zComputer
                     ,nLevel
                     ,zBuffer
                     ,nBuffer
                     ,pnTotalAvail
                    );
    strncpy(zBuf, zComputer, nMaxSize);
    zBuf[nMaxSize] = _T('\0');
#else
    strcpy(zBuf, "noname");
#endif
    return *zBuf ? TRUE : FALSE;
    return TRUE;
}

