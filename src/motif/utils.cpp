/////////////////////////////////////////////////////////////////////////////
// Name:        utils.cpp
// Purpose:     Various utilities
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// Note: this is done in utilscmn.cpp now.
// #pragma implementation
// #pragma implementation "utils.h"
#endif

#include "wx/setup.h"
#include "wx/utils.h"
#include "wx/app.h"

#include <ctype.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <Xm/Xm.h>

// Get full hostname (eg. DoDo.BSn-Germany.crg.de)
bool wxGetHostName(char *buf, int maxSize)
{
    // TODO
    return FALSE;
}

// Get user ID e.g. jacs
bool wxGetUserId(char *buf, int maxSize)
{
    // TODO
    return FALSE;
}

// Get user name e.g. Julian Smart
bool wxGetUserName(char *buf, int maxSize)
{
    // TODO
    return FALSE;
}

int wxKill(long pid, int sig)
{
    // TODO
    return 0;
}

//
// Execute a program in an Interactive Shell
//
bool wxShell(const wxString& command)
{
    // TODO
    return FALSE;
}

// Get free memory in bytes, or -1 if cannot determine amount (e.g. on UNIX)
long wxGetFreeMemory()
{
    // TODO
    return 0;
}

void wxSleep(int nSecs)
{
    // TODO
}

// Consume all events until no more left
void wxFlushEvents()
{
}

// Output a debug message, in a system dependent fashion.
void wxDebugMsg(const char *fmt ...)
{
  va_list ap;
  static char buffer[512];

  if (!wxTheApp->GetWantDebugOutput())
    return ;

  va_start(ap, fmt);

  // wvsprintf(buffer,fmt,ap) ;
  // TODO: output buffer

  va_end(ap);
}

// Non-fatal error: pop up message box and (possibly) continue
void wxError(const wxString& msg, const wxString& title)
{
    // TODO
    wxExit();
}

// Fatal error: pop up message box and abort
void wxFatalError(const wxString& msg, const wxString& title)
{
    // TODO
}

// Emit a beeeeeep
void wxBell()
{
    // TODO
}

int wxGetOsVersion(int *majorVsn, int *minorVsn)
{
    // TODO
    return 0;
}

// Reading and writing resources (eg WIN.INI, .Xdefaults)
#if USE_RESOURCES
bool wxWriteResource(const wxString& section, const wxString& entry, const wxString& value, const wxString& file)
{
    // TODO
    return FALSE;
}

bool wxWriteResource(const wxString& section, const wxString& entry, float value, const wxString& file)
{
  char buf[50];
  sprintf(buf, "%.4f", value);
  return wxWriteResource(section, entry, buf, file);
}

bool wxWriteResource(const wxString& section, const wxString& entry, long value, const wxString& file)
{
  char buf[50];
  sprintf(buf, "%ld", value);
  return wxWriteResource(section, entry, buf, file);
}

bool wxWriteResource(const wxString& section, const wxString& entry, int value, const wxString& file)
{
  char buf[50];
  sprintf(buf, "%d", value);
  return wxWriteResource(section, entry, buf, file);
}

bool wxGetResource(const wxString& section, const wxString& entry, char **value, const wxString& file)
{
    // TODO
    return FALSE;
}

bool wxGetResource(const wxString& section, const wxString& entry, float *value, const wxString& file)
{
  char *s = NULL;
  bool succ = wxGetResource(section, entry, (char **)&s, file);
  if (succ)
  {
    *value = (float)strtod(s, NULL);
    delete[] s;
    return TRUE;
  }
  else return FALSE;
}

bool wxGetResource(const wxString& section, const wxString& entry, long *value, const wxString& file)
{
  char *s = NULL;
  bool succ = wxGetResource(section, entry, (char **)&s, file);
  if (succ)
  {
    *value = strtol(s, NULL, 10);
    delete[] s;
    return TRUE;
  }
  else return FALSE;
}

bool wxGetResource(const wxString& section, const wxString& entry, int *value, const wxString& file)
{
  char *s = NULL;
  bool succ = wxGetResource(section, entry, (char **)&s, file);
  if (succ)
  {
    *value = (int)strtol(s, NULL, 10);
    delete[] s; 
    return TRUE;
  }
  else return FALSE;
}
#endif // USE_RESOURCES

static int wxBusyCursorCount = 0;

// Set the cursor to the busy cursor for all windows
void wxBeginBusyCursor(wxCursor *cursor)
{
  wxBusyCursorCount ++;
  if (wxBusyCursorCount == 1)
  {
        // TODO
  }
  else
  {
        // TODO
  }
}

// Restore cursor to normal
void wxEndBusyCursor()
{
  if (wxBusyCursorCount == 0)
    return;
    
  wxBusyCursorCount --;
  if (wxBusyCursorCount == 0)
  {
    // TODO
  }
}

// TRUE if we're between the above two calls
bool wxIsBusy()
{
  return (wxBusyCursorCount > 0);
}    

char *wxGetUserHome (const wxString& user)
{
    // TODO
    return NULL;
}

// Check whether this window wants to process messages, e.g. Stop button
// in long calculations.
bool wxCheckForInterrupt(wxWindow *wnd)
{
    // TODO
    return FALSE;
}

void wxGetMousePosition( int* x, int* y )
{
    // TODO
};

// Return TRUE if we have a colour display
bool wxColourDisplay()
{
    // TODO
    return TRUE;
}

// Returns depth of screen
int wxDisplayDepth()
{
    // TODO
    return 0;
}

// Get size of display
void wxDisplaySize(int *width, int *height)
{
    // TODO
}

/* Configurable display in Motif */
static WXDisplay *gs_currentDisplay = NULL;
static wxString gs_displayName;

WXDisplay *wxGetDisplay()
{
  if (gs_currentDisplay)
    return gs_currentDisplay;

  return XtDisplay ((Widget) wxTheApp->GetTopLevelWidget());
}

bool wxSetDisplay(const wxString& display_name)
{
  gs_displayName = display_name;
  
  if (display_name.IsNull() || display_name.IsEmpty())
  {
      gs_currentDisplay = NULL;
      return TRUE;
  }
  else
  {
    Cardinal argc = 0;

    Display *display = XtOpenDisplay((XtAppContext) wxTheApp->GetAppContext(),
				     (const char*) display_name,
				     (const char*) wxTheApp->GetAppName(),
				     (const char*) wxTheApp->GetClassName(),
				     NULL,
# if XtSpecificationRelease < 5
				     0, &argc, NULL);
# else
				     0, (int *)&argc, NULL);
# endif

    if (display)
    {
      gs_currentDisplay = (WXDisplay*) display;
      return TRUE;
    } else
      return FALSE;
  }
  return FALSE;
}

wxString wxGetDisplayName()
{
  return gs_displayName;
}
