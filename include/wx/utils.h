/////////////////////////////////////////////////////////////////////////////
// Name:        utils.h
// Purpose:     Miscellaneous utilities
// Author:      Julian Smart
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UTILSH__
#define _WX_UTILSH__

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA) && !defined(__EMX__)
// Some older compilers (such as EMX) cannot handle
// #pragma interface/implementation correctly, iff
// #pragma implementation is used in _two_ translation
// units (as created by e.g. event.cpp compiled for
// libwx_base and event.cpp compiled for libwx_gui_core).
// So we must not use those pragmas for those compilers in
// such files.
    #pragma interface "utils.h"
#endif

#include "wx/object.h"
#include "wx/list.h"
#include "wx/filefn.h"

class WXDLLIMPEXP_BASE wxArrayString;

// need this for wxGetDiskSpace() as we can't, unfortunately, forward declare
// wxLongLong
#include "wx/longlong.h"

#ifdef __X__
    #include <dirent.h>
    #include <unistd.h>
#endif

#include <stdio.h>

// ----------------------------------------------------------------------------
// Forward declaration
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxProcess;
class WXDLLIMPEXP_CORE wxFrame;
class WXDLLIMPEXP_CORE wxWindow;
class WXDLLIMPEXP_CORE wxWindowList;
class WXDLLIMPEXP_CORE wxPoint;

// ----------------------------------------------------------------------------
// Macros
// ----------------------------------------------------------------------------

#define wxMax(a,b)            (((a) > (b)) ? (a) : (b))
#define wxMin(a,b)            (((a) < (b)) ? (a) : (b))

// ----------------------------------------------------------------------------
// String functions (deprecated, use wxString)
// ----------------------------------------------------------------------------

// Make a copy of this string using 'new'
#if WXWIN_COMPATIBILITY_2_4
WXDLLIMPEXP_BASE wxChar* copystring(const wxChar *s);
#endif

// A shorter way of using strcmp
#define wxStringEq(s1, s2) (s1 && s2 && (wxStrcmp(s1, s2) == 0))

// ----------------------------------------------------------------------------
// Miscellaneous functions
// ----------------------------------------------------------------------------

// Sound the bell
WXDLLIMPEXP_BASE void wxBell();

// Get OS description as a user-readable string
WXDLLIMPEXP_BASE wxString wxGetOsDescription();

// Get OS version
WXDLLIMPEXP_BASE int wxGetOsVersion(int *majorVsn = (int *) NULL,
                               int *minorVsn = (int *) NULL);

// Return a string with the current date/time
WXDLLIMPEXP_BASE wxString wxNow();

// Return path where wxWidgets is installed (mostly useful in Unices)
WXDLLIMPEXP_BASE const wxChar *wxGetInstallPrefix();
// Return path to wxWin data (/usr/share/wx/%{version}) (Unices)
WXDLLIMPEXP_BASE wxString wxGetDataDir();


#if wxUSE_GUI

// Get the state of a key (true if pressed, false if not)
// This is generally most useful getting the state of
// the modifier or toggle keys.
WXDLLEXPORT bool wxGetKeyState(wxKeyCode key);


// Don't synthesize KeyUp events holding down a key and producing
// KeyDown events with autorepeat. On by default and always on
// in wxMSW.
WXDLLEXPORT bool wxSetDetectableAutoRepeat( bool flag );

// ----------------------------------------------------------------------------
// Window ID management
// ----------------------------------------------------------------------------

// Generate a unique ID
WXDLLEXPORT long wxNewId();

// Ensure subsequent IDs don't clash with this one
WXDLLEXPORT void wxRegisterId(long id);

// Return the current ID
WXDLLEXPORT long wxGetCurrentId();

#endif // wxUSE_GUI

// ----------------------------------------------------------------------------
// Various conversions
// ----------------------------------------------------------------------------

// these functions are deprecated, use wxString methods instead!
#if WXWIN_COMPATIBILITY_2_4

WXDLLIMPEXP_DATA_BASE(extern const wxChar*) wxFloatToStringStr;
WXDLLIMPEXP_DATA_BASE(extern const wxChar*) wxDoubleToStringStr;

WXDLLIMPEXP_BASE void StringToFloat(const wxChar *s, float *number);
WXDLLIMPEXP_BASE wxChar* FloatToString(float number, const wxChar *fmt = wxFloatToStringStr);
WXDLLIMPEXP_BASE void StringToDouble(const wxChar *s, double *number);
WXDLLIMPEXP_BASE wxChar* DoubleToString(double number, const wxChar *fmt = wxDoubleToStringStr);
WXDLLIMPEXP_BASE void StringToInt(const wxChar *s, int *number);
WXDLLIMPEXP_BASE void StringToLong(const wxChar *s, long *number);
WXDLLIMPEXP_BASE wxChar* IntToString(int number);
WXDLLIMPEXP_BASE wxChar* LongToString(long number);

#endif // WXWIN_COMPATIBILITY_2_4

// Convert 2-digit hex number to decimal
WXDLLIMPEXP_BASE int wxHexToDec(const wxString& buf);

// Convert decimal integer to 2-character hex string
WXDLLIMPEXP_BASE void wxDecToHex(int dec, wxChar *buf);
WXDLLIMPEXP_BASE wxString wxDecToHex(int dec);

// ----------------------------------------------------------------------------
// Process management
// ----------------------------------------------------------------------------

// NB: for backwards compatibility reasons the values of wxEXEC_[A]SYNC *must*
//     be 0 and 1, don't change!

enum
{
    // execute the process asynchronously
    wxEXEC_ASYNC    = 0,

    // execute it synchronously, i.e. wait until it finishes
    wxEXEC_SYNC     = 1,

    // under Windows, don't hide the child even if it's IO is redirected (this
    // is done by default)
    wxEXEC_NOHIDE   = 2,

    // under Unix, if the process is the group leader then passing wxKILL_CHILDREN to wxKill
    // kills all children as well as pid
    wxEXEC_MAKE_GROUP_LEADER = 4
};

// Execute another program.
//
// If flags contain wxEXEC_SYNC, return -1 on failure and the exit code of the
// process if everything was ok. Otherwise (i.e. if wxEXEC_ASYNC), return 0 on
// failure and the PID of the launched process if ok.
WXDLLIMPEXP_BASE long wxExecute(wxChar **argv, int flags = wxEXEC_ASYNC,
                           wxProcess *process = (wxProcess *) NULL);
WXDLLIMPEXP_BASE long wxExecute(const wxString& command, int flags = wxEXEC_ASYNC,
                           wxProcess *process = (wxProcess *) NULL);

// execute the command capturing its output into an array line by line, this is
// always synchronous
WXDLLIMPEXP_BASE long wxExecute(const wxString& command,
                           wxArrayString& output);

// also capture stderr (also synchronous)
WXDLLIMPEXP_BASE long wxExecute(const wxString& command,
                           wxArrayString& output,
                           wxArrayString& error);

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

enum wxKillError
{
    wxKILL_OK,              // no error
    wxKILL_BAD_SIGNAL,      // no such signal
    wxKILL_ACCESS_DENIED,   // permission denied
    wxKILL_NO_PROCESS,      // no such process
    wxKILL_ERROR            // another, unspecified error
};

enum wxKillFlags
{
    wxKILL_NOCHILDREN = 0,  // don't kill children
    wxKILL_CHILDREN = 1     // kill children
};

enum wxShutdownFlags
{
    wxSHUTDOWN_POWEROFF,    // power off the computer
    wxSHUTDOWN_REBOOT       // shutdown and reboot
};

// Shutdown or reboot the PC
WXDLLIMPEXP_BASE bool wxShutdown(wxShutdownFlags wFlags);

// send the given signal to the process (only NONE and KILL are supported under
// Windows, all others mean TERM), return 0 if ok and -1 on error
//
// return detailed error in rc if not NULL
WXDLLIMPEXP_BASE int wxKill(long pid,
                       wxSignal sig = wxSIGTERM,
                       wxKillError *rc = NULL,
                       int flags = wxKILL_NOCHILDREN);

// Execute a command in an interactive shell window (always synchronously)
// If no command then just the shell
WXDLLIMPEXP_BASE bool wxShell(const wxString& command = wxEmptyString);

// As wxShell(), but must give a (non interactive) command and its output will
// be returned in output array
WXDLLIMPEXP_BASE bool wxShell(const wxString& command, wxArrayString& output);

// Sleep for nSecs seconds
WXDLLIMPEXP_BASE void wxSleep(int nSecs);

// Sleep for a given amount of milliseconds
WXDLLIMPEXP_BASE void wxMilliSleep(unsigned long milliseconds);

// Sleep for a given amount of microseconds
WXDLLIMPEXP_BASE void wxMicroSleep(unsigned long microseconds);

// Sleep for a given amount of milliseconds (old, bad name), use wxMilliSleep
wxDEPRECATED( WXDLLIMPEXP_BASE void wxUsleep(unsigned long milliseconds) );

// Get the process id of the current process
WXDLLIMPEXP_BASE unsigned long wxGetProcessId();

// Get free memory in bytes, or -1 if cannot determine amount (e.g. on UNIX)
WXDLLIMPEXP_BASE long wxGetFreeMemory();

// should wxApp::OnFatalException() be called?
WXDLLIMPEXP_BASE bool wxHandleFatalExceptions(bool doit = true);

// ----------------------------------------------------------------------------
// Environment variables
// ----------------------------------------------------------------------------

// returns true if variable exists (value may be NULL if you just want to check
// for this)
WXDLLIMPEXP_BASE bool wxGetEnv(const wxString& var, wxString *value);

// set the env var name to the given value, return true on success
WXDLLIMPEXP_BASE bool wxSetEnv(const wxString& var, const wxChar *value);

// remove the env var from environment
inline bool wxUnsetEnv(const wxString& var) { return wxSetEnv(var, NULL); }

// ----------------------------------------------------------------------------
// Network and username functions.
// ----------------------------------------------------------------------------

// NB: "char *" functions are deprecated, use wxString ones!

// Get eMail address
WXDLLIMPEXP_BASE bool wxGetEmailAddress(wxChar *buf, int maxSize);
WXDLLIMPEXP_BASE wxString wxGetEmailAddress();

// Get hostname.
WXDLLIMPEXP_BASE bool wxGetHostName(wxChar *buf, int maxSize);
WXDLLIMPEXP_BASE wxString wxGetHostName();

// Get FQDN
WXDLLIMPEXP_BASE wxString wxGetFullHostName();
WXDLLIMPEXP_BASE bool wxGetFullHostName(wxChar *buf, int maxSize);

// Get user ID e.g. jacs (this is known as login name under Unix)
WXDLLIMPEXP_BASE bool wxGetUserId(wxChar *buf, int maxSize);
WXDLLIMPEXP_BASE wxString wxGetUserId();

// Get user name e.g. Julian Smart
WXDLLIMPEXP_BASE bool wxGetUserName(wxChar *buf, int maxSize);
WXDLLIMPEXP_BASE wxString wxGetUserName();

// Get current Home dir and copy to dest (returns pstr->c_str())
WXDLLIMPEXP_BASE wxString wxGetHomeDir();
WXDLLIMPEXP_BASE const wxChar* wxGetHomeDir(wxString *pstr);

// Get the user's home dir (caller must copy --- volatile)
// returns NULL is no HOME dir is known
#if defined(__UNIX__) && wxUSE_UNICODE
WXDLLIMPEXP_BASE const wxMB2WXbuf wxGetUserHome(const wxString& user = wxEmptyString);
#else
WXDLLIMPEXP_BASE wxChar* wxGetUserHome(const wxString& user = wxEmptyString);
#endif

// get number of total/free bytes on the disk where path belongs
WXDLLIMPEXP_BASE bool wxGetDiskSpace(const wxString& path,
                                wxLongLong *pTotal = NULL,
                                wxLongLong *pFree = NULL);

#if wxUSE_GUI // GUI only things from now on

// ----------------------------------------------------------------------------
// Menu accelerators related things
// ----------------------------------------------------------------------------

WXDLLEXPORT wxChar* wxStripMenuCodes(const wxChar *in, wxChar *out = (wxChar *) NULL);
WXDLLEXPORT wxString wxStripMenuCodes(const wxString& str);

#if wxUSE_ACCEL
class WXDLLEXPORT wxAcceleratorEntry;
WXDLLEXPORT wxAcceleratorEntry *wxGetAccelFromString(const wxString& label);
#endif // wxUSE_ACCEL

// ----------------------------------------------------------------------------
// Window search
// ----------------------------------------------------------------------------

// Returns menu item id or wxNOT_FOUND if none.
WXDLLEXPORT int wxFindMenuItemId(wxFrame *frame, const wxString& menuString, const wxString& itemString);

// Find the wxWindow at the given point. wxGenericFindWindowAtPoint
// is always present but may be less reliable than a native version.
WXDLLEXPORT wxWindow* wxGenericFindWindowAtPoint(const wxPoint& pt);
WXDLLEXPORT wxWindow* wxFindWindowAtPoint(const wxPoint& pt);

// NB: this function is obsolete, use wxWindow::FindWindowByLabel() instead
//
// Find the window/widget with the given title or label.
// Pass a parent to begin the search from, or NULL to look through
// all windows.
WXDLLEXPORT wxWindow* wxFindWindowByLabel(const wxString& title, wxWindow *parent = (wxWindow *) NULL);

// NB: this function is obsolete, use wxWindow::FindWindowByName() instead
//
// Find window by name, and if that fails, by label.
WXDLLEXPORT wxWindow* wxFindWindowByName(const wxString& name, wxWindow *parent = (wxWindow *) NULL);

// ----------------------------------------------------------------------------
// Message/event queue helpers
// ----------------------------------------------------------------------------

// Yield to other apps/messages and disable user input
WXDLLEXPORT bool wxSafeYield(wxWindow *win = NULL, bool onlyIfNeeded = false);

// Enable or disable input to all top level windows
WXDLLEXPORT void wxEnableTopLevelWindows(bool enable = true);

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

    DECLARE_NO_COPY_CLASS(wxWindowDisabler)
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

// true if we're between the above two calls
WXDLLEXPORT bool wxIsBusy();

// Convenience class so we can just create a wxBusyCursor object on the stack
class WXDLLEXPORT wxBusyCursor
{
public:
    wxBusyCursor(wxCursor* cursor = wxHOURGLASS_CURSOR)
        { wxBeginBusyCursor(cursor); }
    ~wxBusyCursor()
        { wxEndBusyCursor(); }

    // FIXME: These two methods are currently only implemented (and needed?)
    //        in wxGTK.  BusyCursor handling should probably be moved to
    //        common code since the wxGTK and wxMSW implementations are very
    //        similar except for wxMSW using HCURSOR directly instead of
    //        wxCursor..  -- RL.
    static const wxCursor &GetStoredCursor();
    static const wxCursor GetBusyCursor();
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
// #include <X11/Xlib.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

#endif //__X__

#endif // wxUSE_GUI

// ----------------------------------------------------------------------------
// wxYield(): these functions are obsolete, please use wxApp methods instead!
// ----------------------------------------------------------------------------

// Yield to other apps/messages
WXDLLIMPEXP_BASE bool wxYield();

// Like wxYield, but fails silently if the yield is recursive.
WXDLLIMPEXP_BASE bool wxYieldIfNeeded();

// ----------------------------------------------------------------------------
// Error message functions used by wxWidgets (deprecated, use wxLog)
// ----------------------------------------------------------------------------

#if WXWIN_COMPATIBILITY_2_2

// Format a message on the standard error (UNIX) or the debugging
// stream (Windows)
WXDLLIMPEXP_BASE void wxDebugMsg(const wxChar *fmt ...) ATTRIBUTE_PRINTF_1;

// Non-fatal error (continues)
WXDLLIMPEXP_DATA_BASE(extern const wxChar*) wxInternalErrorStr;
WXDLLIMPEXP_BASE void wxError(const wxString& msg, const wxString& title = wxInternalErrorStr);

// Fatal error (exits)
WXDLLIMPEXP_DATA_BASE(extern const wxChar*) wxFatalErrorStr;
WXDLLIMPEXP_BASE void wxFatalError(const wxString& msg, const wxString& title = wxFatalErrorStr);

#endif // WXWIN_COMPATIBILITY_2_2

#endif
    // _WX_UTILSH__
