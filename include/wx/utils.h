/////////////////////////////////////////////////////////////////////////////
// Name:        utils.h
// Purpose:     Miscellaneous utilities
// Author:      Julian Smart
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UTILSH__
#define _WX_UTILSH__

#ifdef __GNUG__
#pragma interface "utils.h"
#endif

#include "wx/setup.h"
#include "wx/object.h"
#include "wx/list.h"
#include "wx/window.h"
#include "wx/filefn.h"

#include "wx/ioswrap.h"

class WXDLLEXPORT wxProcess;

#ifdef __X__
#ifndef __VMS__
/*steve: these two are not known under VMS */
#include <dirent.h>
#include <unistd.h>
#endif
#endif

#include <stdio.h>

#ifdef __GNUWIN32__
#define stricmp strcasecmp
#define strnicmp strncasecmp
#endif

// Forward declaration
class WXDLLEXPORT wxFrame;

// Stupid ASCII macros
#define   wxToUpper(C)      (((C) >= 'a' && (C) <= 'z')? (C) - 'a' + 'A': (C))
#define   wxToLower(C)      (((C) >= 'A' && (C) <= 'Z')? (C) - 'A' + 'a': (C))

// Return a string with the current date/time
WXDLLEXPORT wxString wxNow();

// Make a copy of this string using 'new'
WXDLLEXPORT char* copystring(const char *s);

// Generate a unique ID
WXDLLEXPORT long wxNewId();
#define NewId wxNewId

// Ensure subsequent IDs don't clash with this one
WXDLLEXPORT void wxRegisterId(long id);
#define RegisterId wxRegisterId

// Return the current ID
WXDLLEXPORT long wxGetCurrentId();

// Useful buffer
WXDLLEXPORT_DATA(extern char*) wxBuffer;

WXDLLEXPORT_DATA(extern const char*) wxFloatToStringStr;
WXDLLEXPORT_DATA(extern const char*) wxDoubleToStringStr;

// Various conversions
WXDLLEXPORT void StringToFloat(char *s, float *number);
WXDLLEXPORT char* FloatToString(float number, const char *fmt = wxFloatToStringStr);
WXDLLEXPORT void StringToDouble(char *s, double *number);
WXDLLEXPORT char* DoubleToString(double number, const char *fmt = wxDoubleToStringStr);
WXDLLEXPORT void StringToInt(char *s, int *number);
WXDLLEXPORT void StringToLong(char *s, long *number);
WXDLLEXPORT char* IntToString(int number);
WXDLLEXPORT char* LongToString(long number);

// Matches string one within string two regardless of case
WXDLLEXPORT bool StringMatch(char *one, char *two, bool subString = TRUE, bool exact = FALSE);

// A shorter way of using strcmp
#define wxStringEq(s1, s2) (s1 && s2 && (strcmp(s1, s2) == 0))

// Convert 2-digit hex number to decimal
WXDLLEXPORT int wxHexToDec(const wxString& buf);

// Convert decimal integer to 2-character hex string
WXDLLEXPORT void wxDecToHex(int dec, char *buf);
WXDLLEXPORT wxString wxDecToHex(int dec);

// Execute another program. Returns 0 if there was an error, a PID otherwise.
WXDLLEXPORT long wxExecute(char **argv, bool sync = FALSE,
                           wxProcess *process = (wxProcess *) NULL);
WXDLLEXPORT long wxExecute(const wxString& command, bool sync = FALSE,
                           wxProcess *process = (wxProcess *) NULL);

#define wxSIGTERM 1

WXDLLEXPORT int wxKill(long pid, int sig=wxSIGTERM);

// Execute a command in an interactive shell window
// If no command then just the shell
WXDLLEXPORT bool wxShell(const wxString& command = wxEmptyString);

// Sleep for nSecs seconds under UNIX, do nothing under Windows
WXDLLEXPORT void wxSleep(int nSecs);

// Sleep for a given amount of milliseconds
WXDLLEXPORT void wxUsleep(unsigned long milliseconds);

// Get free memory in bytes, or -1 if cannot determine amount (e.g. on UNIX)
WXDLLEXPORT long wxGetFreeMemory();

// Consume all events until no more left
WXDLLEXPORT void wxFlushEvents();

/*
 * Network and username functions.
 *
 */

// Get eMail address
WXDLLEXPORT bool wxGetEmailAddress(char *buf, int maxSize);

// Get hostname.
WXDLLEXPORT bool wxGetHostName(char *buf, int maxSize);
WXDLLEXPORT bool wxGetHostName(wxString& buf);

// Get user ID e.g. jacs
WXDLLEXPORT bool wxGetUserId(char *buf, int maxSize);
WXDLLEXPORT bool wxGetUserId(wxString& buf);

// Get user name e.g. Julian Smart
WXDLLEXPORT bool wxGetUserName(char *buf, int maxSize);
WXDLLEXPORT bool wxGetUserName(wxString& buf);

/*
 * Strip out any menu codes
 */
WXDLLEXPORT char* wxStripMenuCodes(char *in, char *out = (char *) NULL);
WXDLLEXPORT wxString wxStripMenuCodes(const wxString& str);

// Find the window/widget with the given title or label.
// Pass a parent to begin the search from, or NULL to look through
// all windows.
WXDLLEXPORT wxWindow* wxFindWindowByLabel(const wxString& title, wxWindow *parent = (wxWindow *) NULL);

// Find window by name, and if that fails, by label.
WXDLLEXPORT wxWindow* wxFindWindowByName(const wxString& name, wxWindow *parent = (wxWindow *) NULL);

// Returns menu item id or -1 if none.
WXDLLEXPORT int wxFindMenuItemId(wxFrame *frame, const wxString& menuString, const wxString& itemString);

/*
#if (!defined(__MINMAX_DEFINED) && !defined(max))
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#define __MINMAX_DEFINED 1
#endif
*/

#define wxMax(a,b)            (((a) > (b)) ? (a) : (b))
#define wxMin(a,b)            (((a) < (b)) ? (a) : (b))

// Yield to other apps/messages
WXDLLEXPORT bool wxYield();

// Yield to other apps/messages and disable user input
WXDLLEXPORT bool wxSafeYield(wxWindow *win = NULL);

// Format a message on the standard error (UNIX) or the debugging
// stream (Windows)
WXDLLEXPORT void wxDebugMsg(const char *fmt ...) ;
 
// Sound the bell
WXDLLEXPORT void wxBell(void) ;
  
// Get OS version
WXDLLEXPORT int wxGetOsVersion(int *majorVsn= (int *) NULL,int *minorVsn= (int *) NULL) ;

// Set the cursor to the busy cursor for all windows
class WXDLLEXPORT wxCursor;
WXDLLEXPORT_DATA(extern wxCursor*) wxHOURGLASS_CURSOR;
WXDLLEXPORT void wxBeginBusyCursor(wxCursor *cursor = wxHOURGLASS_CURSOR);

// Restore cursor to normal
WXDLLEXPORT void wxEndBusyCursor();
 
// TRUE if we're between the above two calls
WXDLLEXPORT bool wxIsBusy();

// Convenience class so we can just create a wxBusyCursor object on the stack
class WXDLLEXPORT wxBusyCursor
{
public:
    wxBusyCursor(wxCursor* cursor = wxHOURGLASS_CURSOR)
        { wxBeginBusyCursor(cursor); }
   ~wxBusyCursor()
        { wxEndBusyCursor(); }
};

// Error message functions used by wxWindows

// Non-fatal error (continues)
WXDLLEXPORT_DATA(extern const char*) wxInternalErrorStr;
WXDLLEXPORT void wxError(const wxString& msg, const wxString& title = wxInternalErrorStr);

// Fatal error (exits)
WXDLLEXPORT_DATA(extern const char*) wxFatalErrorStr;
WXDLLEXPORT void wxFatalError(const wxString& msg, const wxString& title = wxFatalErrorStr);

// Reading and writing resources (eg WIN.INI, .Xdefaults)
#if wxUSE_RESOURCES
WXDLLEXPORT bool wxWriteResource(const wxString& section, const wxString& entry, const wxString& value, const wxString& file = wxEmptyString);
WXDLLEXPORT bool wxWriteResource(const wxString& section, const wxString& entry, float value, const wxString& file = wxEmptyString);
WXDLLEXPORT bool wxWriteResource(const wxString& section, const wxString& entry, long value, const wxString& file = wxEmptyString);
WXDLLEXPORT bool wxWriteResource(const wxString& section, const wxString& entry, int value, const wxString& file = wxEmptyString);

WXDLLEXPORT bool wxGetResource(const wxString& section, const wxString& entry, char **value, const wxString& file = wxEmptyString);
WXDLLEXPORT bool wxGetResource(const wxString& section, const wxString& entry, float *value, const wxString& file = wxEmptyString);
WXDLLEXPORT bool wxGetResource(const wxString& section, const wxString& entry, long *value, const wxString& file = wxEmptyString);
WXDLLEXPORT bool wxGetResource(const wxString& section, const wxString& entry, int *value, const wxString& file = wxEmptyString);
#endif // wxUSE_RESOURCES

// Get current Home dir and copy to dest (returns pstr->c_str())
WXDLLEXPORT const char* wxGetHomeDir(wxString *pstr);

// Get the user's home dir (caller must copy--- volatile)
// returns NULL is no HOME dir is known
WXDLLEXPORT char* wxGetUserHome(const wxString& user = wxEmptyString);

// Check whether this window wants to process messages, e.g. Stop button
// in long calculations.
WXDLLEXPORT bool wxCheckForInterrupt(wxWindow *wnd);

void WXDLLEXPORT wxGetMousePosition( int* x, int* y );

// MSW only: get user-defined resource from the .res file.
// Returns NULL or newly-allocated memory, so use delete[] to clean up.
#ifdef __WXMSW__
WXDLLEXPORT extern const char* wxUserResourceStr;
WXDLLEXPORT char* wxLoadUserResource(const wxString& resourceName, const wxString& resourceType = wxUserResourceStr);

// Implemented in utils.cpp: VC++, Win95 only. Sets up a console for standard
// input/output
WXDLLEXPORT void wxRedirectIOToConsole();

#endif

// X only
#ifdef __X__
WXDisplay *wxGetDisplay();
bool wxSetDisplay(const wxString& display_name);
wxString wxGetDisplayName();
#endif

#ifdef __X__

#include <X11/Xlib.h>

#define wxMAX_RGB           0xff
#define wxMAX_SV            1000
#define wxSIGN(x)           ((x < 0) ? -x : x)
#define wxH_WEIGHT          4
#define wxS_WEIGHT          1
#define wxV_WEIGHT          2

typedef struct wx_hsv {
                        int h,s,v;
                      } wxHSV;
 
#define wxMax3(x,y,z) ((x > y) ? ((x > z) ? x : z) : ((y > z) ? y : z))
#define wxMin3(x,y,z) ((x < y) ? ((x < z) ? x : z) : ((y < z) ? y : z))

#define wxMax2(x,y)   ((x > y) ? x : y)
#define wxMin2(x,y)   ((x < y) ? x : y)

void wxHSVToXColor(wxHSV *hsv,XColor *xcolor);
void wxXColorToHSV(wxHSV *hsv,XColor *xcolor);
void wxAllocNearestColor(Display *display,Colormap colormap,XColor *xcolor);
void wxAllocColor(Display *display,Colormap colormap,XColor *xcolor);

#endif //__X__

#endif
    // _WX_UTILSH__
