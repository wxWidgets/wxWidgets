/////////////////////////////////////////////////////////////////////////////
// Name:        utilsexec.cpp
// Purpose:     Various utilities
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "utils.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/setup.h"
#include "wx/utils.h"
#include "wx/app.h"
#endif

#include "wx/msw/private.h"
#include <windows.h>

#include <ctype.h>

#ifndef __GNUWIN32__
#include <direct.h>
#include <dos.h>
#endif

#ifdef __GNUWIN32__
#include <sys/unistd.h>
#include <sys/stat.h>
#ifndef __MINGW32__
#include <std.h>
#endif

#endif

#ifdef __WIN32__
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

#define wxEXECUTE_WIN_MESSAGE 10000

struct wxExecuteData {
  HWND window;
  HINSTANCE process;
  wxProcess *handler;
  char state;
};

static DWORD wxExecuteThread(wxExecuteData *data)
{
  WaitForSingleObject(data->process, INFINITE);

  // Send an implicit message to the window
  SendMessage(data->window, wxEXECUTE_WIN_MESSAGE, 0, (LPARAM)data);

  return 0;
}

LRESULT APIENTRY _EXPORT wxExecuteWindowCbk(HWND hWnd, UINT message,
                                            WPARAM wParam, LPARAM lParam)
{
  wxExecuteData *data = (wxExecuteData *)lParam;

  if (message == wxEXECUTE_WIN_MESSAGE) {
    DestroyWindow(hWnd);
    if (data->handler)
      data->handler->OnTerminate((int)data->process);

    if (data->state)
      data->state = 0;
    else
      delete data;
  }
  return 0;
}

extern char wxPanelClassName[];

long wxExecute(const wxString& command, bool sync, wxProcess *handler)
{
  if (command == "")
    return 0;

#ifdef __WIN32__
  char * cl;
  char * argp;
  int clen;
  HINSTANCE result;
  DWORD dresult;
  HWND window;
  wxExecuteData *data;
  DWORD tid;

  // copy the command line
  clen = command.Length();
  if (!clen) return -1;
  cl = (char *) calloc( 1, 256);
  if (!cl) return -1;
  strcpy( cl, WXSTRINGCAST command);

  // isolate command and arguments
  argp = strchr( cl, ' ');
  if (argp)
    *argp++ = '\0';

#ifdef __GNUWIN32__
  result = ShellExecute((HWND) (wxTheApp->GetTopWindow() ? (HWND) wxTheApp->GetT
opWindow()->GetHWND() : NULL),
     (const wchar_t) "open", (const wchar_t) cl, (const wchar_t) arg
p,i
     (const wchar_t) NULL, SW_SHOWNORMAL);
#else
  result = ShellExecute( (HWND) (wxTheApp->GetTopWindow() ? wxTheApp->GetTopWindow()->GetHWND() : NULL),
     "open", cl, argp, NULL, SW_SHOWNORMAL);
#endif

  if (((long)result) <= 32) {
   free(cl);
   return 0;
  }

  // Alloc data
  data = new wxExecuteData;

  // Create window
  window = CreateWindow(wxPanelClassName, NULL, 0, 0, 0, 0, 0, NULL,
                        (HMENU) NULL, wxGetInstance(), 0);

  FARPROC ExecuteWindowInstance = MakeProcInstance((FARPROC) wxExecuteWindowCbk,
                                                   wxGetInstance());

  SetWindowLong(window, GWL_WNDPROC, (LONG) ExecuteWindowInstance);
  SetWindowLong(window, GWL_USERDATA, (LONG) data);

  data->process = result;
  data->window  = window;
  data->state   = sync;
  data->handler = (sync) ? NULL : handler;

  dresult = (DWORD)CreateThread(NULL, 0,
                                (LPTHREAD_START_ROUTINE)wxExecuteThread,
                                (void *)data, 0, &tid);
  if (dresult == 0) {
    wxDebugMsg("wxExecute PANIC: I can't create the waiting thread !"); 
    DestroyWindow(window);
    return (long)result;
  }

  if (!sync)
  {
    free(cl);
    return (long)result;
  }

  // waiting until command executed
  while (data->state)
    wxYield();

  free(cl);
  return 0;
#else
  long instanceID = WinExec((LPCSTR) WXSTRINGCAST command, SW_SHOW);
  if (instanceID < 32) return(0);

  if (sync) {
    int running;
    do {
      wxYield();
      running = GetModuleUsage((HANDLE)instanceID);
    } while (running);
  }
  return(instanceID);
#endif
}
