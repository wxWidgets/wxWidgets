/////////////////////////////////////////////////////////////////////////////
// Name:        utils.h
// Purpose:     Miscellaneous utilities
// Author:      Julian Smart
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:   	wxWindows license
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
#include "wx/process.h"

#if USE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif

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
wxString WXDLLEXPORT wxNow(void);

// Make a copy of this string using 'new'
char* WXDLLEXPORT copystring(const char *s);

// Generate a unique ID
long WXDLLEXPORT wxNewId(void);
#define NewId wxNewId

// Ensure subsequent IDs don't clash with this one
void WXDLLEXPORT wxRegisterId(long id);
#define RegisterId wxRegisterId

// Return the current ID
long WXDLLEXPORT wxGetCurrentId(void);

// Useful buffer
WXDLLEXPORT_DATA(extern char*) wxBuffer;

WXDLLEXPORT_DATA(extern const char*) wxFloatToStringStr;
WXDLLEXPORT_DATA(extern const char*) wxDoubleToStringStr;

// Various conversions
void WXDLLEXPORT StringToFloat(char *s, float *number);
char* WXDLLEXPORT FloatToString(float number, const char *fmt = wxFloatToStringStr);
void WXDLLEXPORT StringToDouble(char *s, double *number);
char* WXDLLEXPORT DoubleToString(double number, const char *fmt = wxDoubleToStringStr);
void WXDLLEXPORT StringToInt(char *s, int *number);
void WXDLLEXPORT StringToLong(char *s, long *number);
char* WXDLLEXPORT IntToString(int number);
char* WXDLLEXPORT LongToString(long number);

// Matches string one within string two regardless of case
bool WXDLLEXPORT StringMatch(char *one, char *two, bool subString = TRUE, bool exact = FALSE);

// A shorter way of using strcmp
#define wxStringEq(s1, s2) (s1 && s2 && (strcmp(s1, s2) == 0))

// Convert 2-digit hex number to decimal
int WXDLLEXPORT wxHexToDec(char *buf);

// Convert decimal integer to 2-character hex string
void WXDLLEXPORT wxDecToHex(int dec, char *buf);

// Execute another program. Returns 0 if there was an error, a PID otherwise.
long WXDLLEXPORT wxExecute(char **argv, bool Async = FALSE,
                           wxProcess *process = NULL);
long WXDLLEXPORT wxExecute(const wxString& command, bool Async = FALSE,
                           wxProcess *process = NULL);

#define wxSIGTERM 1

int WXDLLEXPORT wxKill(long pid, int sig=wxSIGTERM);

// Execute a command in an interactive shell window
// If no command then just the shell
bool WXDLLEXPORT wxShell(const wxString& command = "");

// Sleep for nSecs seconds under UNIX, do nothing under Windows
void WXDLLEXPORT wxSleep(int nSecs);

// Get free memory in bytes, or -1 if cannot determine amount (e.g. on UNIX)
long WXDLLEXPORT wxGetFreeMemory(void);

// Consume all events until no more left
void WXDLLEXPORT wxFlushEvents(void);

/*
 * Network and username functions.
 *
 */

// Get eMail address
bool WXDLLEXPORT wxGetEmailAddress(char *buf, int maxSize);

// Get hostname.
bool WXDLLEXPORT wxGetHostName(char *buf, int maxSize);

// Get user ID e.g. jacs
bool WXDLLEXPORT wxGetUserId(char *buf, int maxSize);

// Get user name e.g. Julian Smart
bool WXDLLEXPORT wxGetUserName(char *buf, int maxSize);

/*
 * Strip out any menu codes
 */
char* WXDLLEXPORT wxStripMenuCodes(char *in, char *out = NULL);

// Find the window/widget with the given title or label.
// Pass a parent to begin the search from, or NULL to look through
// all windows.
wxWindow* WXDLLEXPORT wxFindWindowByLabel(const wxString& title, wxWindow *parent = NULL);

// Find window by name, and if that fails, by label.
wxWindow* WXDLLEXPORT wxFindWindowByName(const wxString& name, wxWindow *parent = NULL);

// Returns menu item id or -1 if none.
int WXDLLEXPORT wxFindMenuItemId(wxFrame *frame, const wxString& menuString, const wxString& itemString);

// A debugging stream buffer.
// Under Windows, this writes to the Windows debug output.
// Under other platforms, it writes to cerr.

// ALl this horrible gubbins required for Borland, because the calling
// convention needs to be the same as for streambuf.
// Thanks to Gerhard.Vogt@embl-heidelberg.de for this solution.

#if defined(__BORLANDC__) && defined(__BCOPT__) && !defined(_RTL_ALLOW_po) && !defined(__FLAT__)
#pragma option -po-
#endif

// Can't export a class derived from a non-export class
#if !defined(_WINDLL) && !defined(WXUSINGDLL)

#ifdef new
#undef new
#endif

class WXDLLEXPORT wxDebugStreamBuf: public streambuf
{
  public:
    wxDebugStreamBuf(void);
    ~wxDebugStreamBuf(void) {}

    int overflow(int i);
    inline int underflow(void) { return EOF; }
    int sync(void);
};

#if WXDEBUG && USE_GLOBAL_MEMORY_OPERATORS
#define new WXDEBUG_NEW
#endif

#endif

#if defined(__BORLANDC__) && defined(__BCOPT__) && !defined(_RTL_ALLOW_po) && !defined(__FLAT__)
#pragma option -po.
#endif

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
bool WXDLLEXPORT wxYield(void);

// Format a message on the standard error (UNIX) or the debugging
// stream (Windows)
void WXDLLEXPORT wxDebugMsg(const char *fmt ...) ;
 
// Sound the bell
void WXDLLEXPORT wxBell(void) ;
  
// Get OS version
int WXDLLEXPORT wxGetOsVersion(int *majorVsn=NULL,int *minorVsn=NULL) ;

// Set the cursor to the busy cursor for all windows
class WXDLLEXPORT wxCursor;
WXDLLEXPORT_DATA(extern wxCursor*) wxHOURGLASS_CURSOR;
void WXDLLEXPORT wxBeginBusyCursor(wxCursor *cursor = wxHOURGLASS_CURSOR);
 
// Restore cursor to normal
void WXDLLEXPORT wxEndBusyCursor(void);
 
// TRUE if we're between the above two calls
bool WXDLLEXPORT wxIsBusy(void);

/* Error message functions used by wxWindows */

// Non-fatal error (continues)
WXDLLEXPORT_DATA(extern const char*) wxInternalErrorStr;
void WXDLLEXPORT wxError(const wxString& msg, const wxString& title = wxInternalErrorStr);

// Fatal error (exits)
WXDLLEXPORT_DATA(extern const char*) wxFatalErrorStr;
void WXDLLEXPORT wxFatalError(const wxString& msg, const wxString& title = wxFatalErrorStr);

// Reading and writing resources (eg WIN.INI, .Xdefaults)
#if USE_RESOURCES
bool WXDLLEXPORT wxWriteResource(const wxString& section, const wxString& entry, const wxString& value, const wxString& file = "");
bool WXDLLEXPORT wxWriteResource(const wxString& section, const wxString& entry, float value, const wxString& file = "");
bool WXDLLEXPORT wxWriteResource(const wxString& section, const wxString& entry, long value, const wxString& file = "");
bool WXDLLEXPORT wxWriteResource(const wxString& section, const wxString& entry, int value, const wxString& file = "");

bool WXDLLEXPORT wxGetResource(const wxString& section, const wxString& entry, char **value, const wxString& file = "");
bool WXDLLEXPORT wxGetResource(const wxString& section, const wxString& entry, float *value, const wxString& file = "");
bool WXDLLEXPORT wxGetResource(const wxString& section, const wxString& entry, long *value, const wxString& file = "");
bool WXDLLEXPORT wxGetResource(const wxString& section, const wxString& entry, int *value, const wxString& file = "");
#endif // USE_RESOURCES

#ifdef __UNIX__
// 'X' Only, will soon vanish....
// Get current Home dir and copy to dest
char* WXDLLEXPORT wxGetHomeDir( char *dest );
#endif
// Get the user's home dir (caller must copy--- volatile)
// returns NULL is no HOME dir is known
char* WXDLLEXPORT wxGetUserHome(const wxString& user = "");

// Check whether this window wants to process messages, e.g. Stop button
// in long calculations.
bool WXDLLEXPORT wxCheckForInterrupt(wxWindow *wnd);

void WXDLLEXPORT wxGetMousePosition( int* x, int* y );

// MSW only: get user-defined resource from the .res file.
// Returns NULL or newly-allocated memory, so use delete[] to clean up.
#ifdef __WXMSW__
extern const char* WXDLLEXPORT wxUserResourceStr;
char* WXDLLEXPORT wxLoadUserResource(const wxString& resourceName, const wxString& resourceType = wxUserResourceStr);
#endif

// X only
#ifdef __X__
// Get X display: often needed in the wxWindows implementation.
Display *wxGetDisplay(void);
/* Matthew Flatt: Added wxSetDisplay and wxGetDisplayName */
bool wxSetDisplay(const wxString& display_name);
wxString wxGetDisplayName(void);
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
