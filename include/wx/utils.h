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

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma interface "utils.h"
#endif

#include "wx/setup.h"
#include "wx/object.h"
#include "wx/list.h"
#include "wx/filefn.h"

#ifdef __X__
    #include <dirent.h>
    #include <unistd.h>
#endif

#include <stdio.h>

// ----------------------------------------------------------------------------
// Forward declaration
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxProcess;
class WXDLLEXPORT wxFrame;
class WXDLLEXPORT wxWindow;
class WXDLLEXPORT wxWindowList;

// FIXME should use wxStricmp() instead
#if defined(__GNUWIN32__)
    #define stricmp strcasecmp
    #define strnicmp strncasecmp
#endif

// ----------------------------------------------------------------------------
// Macros
// ----------------------------------------------------------------------------

#define wxMax(a,b)            (((a) > (b)) ? (a) : (b))
#define wxMin(a,b)            (((a) < (b)) ? (a) : (b))

// ----------------------------------------------------------------------------
// String functions (deprecated, use wxString)
// ----------------------------------------------------------------------------

// Useful buffer (FIXME VZ: yeah, that is. To be removed!)
WXDLLEXPORT_DATA(extern wxChar*) wxBuffer;

// Make a copy of this string using 'new'
WXDLLEXPORT wxChar* copystring(const wxChar *s);

// Matches string one within string two regardless of case
WXDLLEXPORT bool StringMatch(wxChar *one, wxChar *two, bool subString = TRUE, bool exact = FALSE);

// A shorter way of using strcmp
#define wxStringEq(s1, s2) (s1 && s2 && (wxStrcmp(s1, s2) == 0))

// ----------------------------------------------------------------------------
// Miscellaneous functions
// ----------------------------------------------------------------------------

// Sound the bell
WXDLLEXPORT void wxBell();

// Get OS description as a user-readable string
WXDLLEXPORT wxString wxGetOsDescription();

// Get OS version
WXDLLEXPORT int wxGetOsVersion(int *majorVsn = (int *) NULL,
                               int *minorVsn = (int *) NULL);

// Return a string with the current date/time
WXDLLEXPORT wxString wxNow();

#if wxUSE_GUI
// Don't synthesize KeyUp events holding down a key and producing
// KeyDown events with autorepeat. On by default and always on
// in wxMSW.
WXDLLEXPORT bool wxSetDetectableAutoRepeat( bool flag );

// ----------------------------------------------------------------------------
// Window ID management
// ----------------------------------------------------------------------------

// Generate a unique ID
WXDLLEXPORT long wxNewId();
#if !defined(NewId) && defined(WXWIN_COMPATIBILITY)
    #define NewId wxNewId
#endif

// Ensure subsequent IDs don't clash with this one
WXDLLEXPORT void wxRegisterId(long id);
#if !defined(RegisterId) && defined(WXWIN_COMPATIBILITY)
    #define RegisterId wxRegisterId
#endif

// Return the current ID
WXDLLEXPORT long wxGetCurrentId();

#endif // wxUSE_GUI

// ----------------------------------------------------------------------------
// Various conversions
// ----------------------------------------------------------------------------

WXDLLEXPORT_DATA(extern const wxChar*) wxFloatToStringStr;
WXDLLEXPORT_DATA(extern const wxChar*) wxDoubleToStringStr;

WXDLLEXPORT void StringToFloat(wxChar *s, float *number);
WXDLLEXPORT wxChar* FloatToString(float number, const wxChar *fmt = wxFloatToStringStr);
WXDLLEXPORT void StringToDouble(wxChar *s, double *number);
WXDLLEXPORT wxChar* DoubleToString(double number, const wxChar *fmt = wxDoubleToStringStr);
WXDLLEXPORT void StringToInt(wxChar *s, int *number);
WXDLLEXPORT void StringToLong(wxChar *s, long *number);
WXDLLEXPORT wxChar* IntToString(int number);
WXDLLEXPORT wxChar* LongToString(long number);

// Convert 2-digit hex number to decimal
WXDLLEXPORT int wxHexToDec(const wxString& buf);

// Convert decimal integer to 2-character hex string
WXDLLEXPORT void wxDecToHex(int dec, wxChar *buf);
WXDLLEXPORT wxString wxDecToHex(int dec);

// ----------------------------------------------------------------------------
// Process management
// ----------------------------------------------------------------------------

// Execute another program. Returns 0 if there was an error, a PID otherwise.
WXDLLEXPORT long wxExecute(wxChar **argv, bool sync = FALSE,
                           wxProcess *process = (wxProcess *) NULL);
WXDLLEXPORT long wxExecute(const wxString& command, bool sync = FALSE,
                           wxProcess *process = (wxProcess *) NULL);

// execute the command capturing its output into an array line by line
WXDLLEXPORT long wxExecute(const wxString& command, wxArrayString& output);

enum wxSignal
{
    wxSIGNONE = 0,  // verify if the process exists under Unix
    wxSIGHUP,
    wxSIGINT,
    wxSIGQUIT,
    wxSIGILL,
    wxSIGTRAP,
    wxSIGABRT,
    wxSIGIOT = wxSIGABRT,   // another name
    wxSIGEMT,
    wxSIGFPE,
    wxSIGKILL,
    wxSIGBUS,
    wxSIGSEGV,
    wxSIGSYS,
    wxSIGPIPE,
    wxSIGALRM,
    wxSIGTERM

    // further signals are different in meaning between different Unix systems
};

// the argument is ignored under Windows - the process is always killed
WXDLLEXPORT int wxKill(long pid, wxSignal sig = wxSIGTERM);

// Execute a command in an interactive shell window (always synchronously)
// If no command then just the shell
WXDLLEXPORT bool wxShell(const wxString& command = wxEmptyString);

// As wxShell(), but must give a (non interactive) command and its output will
// be returned in output array
WXDLLEXPORT bool wxShell(const wxString& command, wxArrayString& output);

// Sleep for nSecs seconds
WXDLLEXPORT void wxSleep(int nSecs);

// Sleep for a given amount of milliseconds
WXDLLEXPORT void wxUsleep(unsigned long milliseconds);

// Get free memory in bytes, or -1 if cannot determine amount (e.g. on UNIX)
WXDLLEXPORT long wxGetFreeMemory();

// should wxApp::OnFatalException() be called?
WXDLLEXPORT bool wxHandleFatalExceptions(bool doit = TRUE);

// ----------------------------------------------------------------------------
// Network and username functions.
// ----------------------------------------------------------------------------

// NB: "char *" functions are deprecated, use wxString ones!

// Get eMail address
WXDLLEXPORT bool wxGetEmailAddress(wxChar *buf, int maxSize);
WXDLLEXPORT wxString wxGetEmailAddress();

// Get hostname.
WXDLLEXPORT bool wxGetHostName(wxChar *buf, int maxSize);
WXDLLEXPORT wxString wxGetHostName();

// Get FQDN
WXDLLEXPORT wxString wxGetFullHostName();
WXDLLEXPORT bool wxGetFullHostName(wxChar *buf, int maxSize);

// Get user ID e.g. jacs (this is known as login name under Unix)
WXDLLEXPORT bool wxGetUserId(wxChar *buf, int maxSize);
WXDLLEXPORT wxString wxGetUserId();

// Get user name e.g. Julian Smart
WXDLLEXPORT bool wxGetUserName(wxChar *buf, int maxSize);
WXDLLEXPORT wxString wxGetUserName();

// Get current Home dir and copy to dest (returns pstr->c_str())
WXDLLEXPORT wxString wxGetHomeDir();
WXDLLEXPORT const wxChar* wxGetHomeDir(wxString *pstr);

// Get the user's home dir (caller must copy --- volatile)
// returns NULL is no HOME dir is known
#if defined(__UNIX__) && wxUSE_UNICODE
WXDLLEXPORT const wxMB2WXbuf wxGetUserHome(const wxString& user = wxEmptyString);
#else
WXDLLEXPORT wxChar* wxGetUserHome(const wxString& user = wxEmptyString);
#endif

#if wxUSE_GUI // GUI only things from now on

// ----------------------------------------------------------------------------
// Menu accelerators related things
// ----------------------------------------------------------------------------

WXDLLEXPORT wxChar* wxStripMenuCodes(wxChar *in, wxChar *out = (wxChar *) NULL);
WXDLLEXPORT wxString wxStripMenuCodes(const wxString& str);

#if wxUSE_ACCEL
class WXDLLEXPORT wxAcceleratorEntry;
WXDLLEXPORT wxAcceleratorEntry *wxGetAccelFromString(const wxString& label);
#endif // wxUSE_ACCEL

// ----------------------------------------------------------------------------
// Window search
// ----------------------------------------------------------------------------

// Find the window/widget with the given title or label.
// Pass a parent to begin the search from, or NULL to look through
// all windows.
WXDLLEXPORT wxWindow* wxFindWindowByLabel(const wxString& title, wxWindow *parent = (wxWindow *) NULL);

// Find window by name, and if that fails, by label.
WXDLLEXPORT wxWindow* wxFindWindowByName(const wxString& name, wxWindow *parent = (wxWindow *) NULL);

// Returns menu item id or -1 if none.
WXDLLEXPORT int wxFindMenuItemId(wxFrame *frame, const wxString& menuString, const wxString& itemString);

// ----------------------------------------------------------------------------
// Message/event queue helpers
// ----------------------------------------------------------------------------

// Yield to other apps/messages
WXDLLEXPORT bool wxYield();

// Yield to other apps/messages and disable user input
WXDLLEXPORT bool wxSafeYield(wxWindow *win = NULL);

// Enable or disable input to all top level windows
WXDLLEXPORT void wxEnableTopLevelWindows(bool enable = TRUE);

// Check whether this window wants to process messages, e.g. Stop button
// in long calculations.
WXDLLEXPORT bool wxCheckForInterrupt(wxWindow *wnd);

// Consume all events until no more left
WXDLLEXPORT void wxFlushEvents();

// a class which disables all windows (except, may be, thegiven one) in its
// ctor and enables them back in its dtor
class WXDLLEXPORT wxWindowDisabler
{
public:
    wxWindowDisabler(wxWindow *winToSkip = (wxWindow *)NULL);
    ~wxWindowDisabler();

private:
    wxWindowList *m_winDisabled;

#ifdef __WXMSW__
    wxWindow *m_winTop;
#endif // MSW
};

// ----------------------------------------------------------------------------
// Cursors
// ----------------------------------------------------------------------------

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


// ----------------------------------------------------------------------------
// Reading and writing resources (eg WIN.INI, .Xdefaults)
// ----------------------------------------------------------------------------

#if wxUSE_RESOURCES
WXDLLEXPORT bool wxWriteResource(const wxString& section, const wxString& entry, const wxString& value, const wxString& file = wxEmptyString);
WXDLLEXPORT bool wxWriteResource(const wxString& section, const wxString& entry, float value, const wxString& file = wxEmptyString);
WXDLLEXPORT bool wxWriteResource(const wxString& section, const wxString& entry, long value, const wxString& file = wxEmptyString);
WXDLLEXPORT bool wxWriteResource(const wxString& section, const wxString& entry, int value, const wxString& file = wxEmptyString);

WXDLLEXPORT bool wxGetResource(const wxString& section, const wxString& entry, wxChar **value, const wxString& file = wxEmptyString);
WXDLLEXPORT bool wxGetResource(const wxString& section, const wxString& entry, float *value, const wxString& file = wxEmptyString);
WXDLLEXPORT bool wxGetResource(const wxString& section, const wxString& entry, long *value, const wxString& file = wxEmptyString);
WXDLLEXPORT bool wxGetResource(const wxString& section, const wxString& entry, int *value, const wxString& file = wxEmptyString);
#endif // wxUSE_RESOURCES

void WXDLLEXPORT wxGetMousePosition( int* x, int* y );

// MSW only: get user-defined resource from the .res file.
// Returns NULL or newly-allocated memory, so use delete[] to clean up.
#ifdef __WXMSW__
WXDLLEXPORT extern const wxChar* wxUserResourceStr;
WXDLLEXPORT wxChar* wxLoadUserResource(const wxString& resourceName, const wxString& resourceType = wxUserResourceStr);

// Implemented in utils.cpp: VC++, Win95 only. Sets up a console for standard
// input/output
WXDLLEXPORT void wxRedirectIOToConsole();

#endif // MSW

// ----------------------------------------------------------------------------
// Display and colorss (X only)
// ----------------------------------------------------------------------------

#ifdef __WXGTK__
    void *wxGetDisplay();
#endif

#ifdef __X__
    WXDisplay *wxGetDisplay();
    bool wxSetDisplay(const wxString& display_name);
    wxString wxGetDisplayName();
#endif // X or GTK+

#ifdef __X__

#ifdef __VMS__ // Xlib.h for VMS is not (yet) compatible with C++
               // The resulting warnings are switched off here
#pragma message disable nosimpint
#endif
#include <X11/Xlib.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

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

void wxHSVToXColor(wxHSV *hsv,XColor *xcolor);
void wxXColorToHSV(wxHSV *hsv,XColor *xcolor);
void wxAllocNearestColor(Display *display,Colormap colormap,XColor *xcolor);
void wxAllocColor(Display *display,Colormap colormap,XColor *xcolor);

#endif //__X__

#endif // wxUSE_GUI

// ----------------------------------------------------------------------------
// Error message functions used by wxWindows (deprecated, use wxLog)
// ----------------------------------------------------------------------------

// Format a message on the standard error (UNIX) or the debugging
// stream (Windows)
WXDLLEXPORT void wxDebugMsg(const wxChar *fmt ...);

// Non-fatal error (continues)
WXDLLEXPORT_DATA(extern const wxChar*) wxInternalErrorStr;
WXDLLEXPORT void wxError(const wxString& msg, const wxString& title = wxInternalErrorStr);

// Fatal error (exits)
WXDLLEXPORT_DATA(extern const wxChar*) wxFatalErrorStr;
WXDLLEXPORT void wxFatalError(const wxString& msg, const wxString& title = wxFatalErrorStr);


#endif
    // _WX_UTILSH__
