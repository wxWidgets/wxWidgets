/////////////////////////////////////////////////////////////////////////////
// Name:        wx/log.h
// Purpose:     Assorted wxLogXXX functions, and wxLog (sink for logs)
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef   _WX_LOG_H_
#define   _WX_LOG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "log.h"
#endif

#include "wx/setup.h"
#include "wx/string.h"

// ----------------------------------------------------------------------------
// forward declarations
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxTextCtrl;
class WXDLLEXPORT wxLogFrame;
class WXDLLEXPORT wxFrame;

// ----------------------------------------------------------------------------
// types
// ----------------------------------------------------------------------------

typedef unsigned long wxTraceMask;
typedef unsigned long wxLogLevel;

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if wxUSE_LOG

#include <time.h>   // for time_t

#include "wx/dynarray.h"

#ifndef wxUSE_LOG_DEBUG
#  ifdef __WXDEBUG__
#    define wxUSE_LOG_DEBUG 1
#  else // !__WXDEBUG__
#    define wxUSE_LOG_DEBUG 0
#  endif
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// different standard log levels (you may also define your own)
enum
{
    wxLOG_FatalError, // program can't continue, abort immediately
    wxLOG_Error,      // a serious error, user must be informed about it
    wxLOG_Warning,    // user is normally informed about it but may be ignored
    wxLOG_Message,    // normal message (i.e. normal output of a non GUI app)
    wxLOG_Status,     // informational: might go to the status line of GUI app
    wxLOG_Info,       // informational message (a.k.a. 'Verbose')
    wxLOG_Debug,      // never shown to the user, disabled in release mode
    wxLOG_Trace,      // trace messages are also only enabled in debug mode
    wxLOG_Progress,   // used for progress indicator (not yet)
    wxLOG_User = 100, // user defined levels start here
    wxLOG_Max = 10000
};

// symbolic trace masks - wxLogTrace("foo", "some trace message...") will be
// discarded unless the string "foo" has been added to the list of allowed
// ones with AddTraceMask()

#define wxTRACE_MemAlloc wxT("memalloc") // trace memory allocation (new/delete)
#define wxTRACE_Messages wxT("messages") // trace window messages/X callbacks
#define wxTRACE_ResAlloc wxT("resalloc") // trace GDI resource allocation
#define wxTRACE_RefCount wxT("refcount") // trace various ref counting operations

#ifdef  __WXMSW__
    #define wxTRACE_OleCalls wxT("ole")  // OLE interface calls
#endif

// the trace masks have been superceded by symbolic trace constants, they're
// for compatibility only andwill be removed soon - do NOT use them

// meaning of different bits of the trace mask (which allows selectively
// enable/disable some trace messages)
#define wxTraceMemAlloc 0x0001  // trace memory allocation (new/delete)
#define wxTraceMessages 0x0002  // trace window messages/X callbacks
#define wxTraceResAlloc 0x0004  // trace GDI resource allocation
#define wxTraceRefCount 0x0008  // trace various ref counting operations

#ifdef  __WXMSW__
    #define wxTraceOleCalls 0x0100  // OLE interface calls
#endif

#include "wx/ioswrap.h"

// ----------------------------------------------------------------------------
// derive from this class to redirect (or suppress, or ...) log messages
// normally, only a single instance of this class exists but it's not enforced
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxLog
{
public:
    // ctor
    wxLog();

    // Internal buffer.
        // Allow replacement of the fixed size static buffer with
        // a user allocated one.  Pass in NULL to restore the
        // built in static buffer.
    static wxChar *SetLogBuffer( wxChar *buf, size_t size = 0 );

    // these functions allow to completely disable all log messages
        // is logging disabled now?
    static bool IsEnabled() { return ms_doLog; }
        // change the flag state, return the previous one
    static bool EnableLogging(bool doIt = TRUE)
        { bool doLogOld = ms_doLog; ms_doLog = doIt; return doLogOld; }

    // static sink function - see DoLog() for function to overload in the
    // derived classes
    static void OnLog(wxLogLevel level, const wxChar *szString, time_t t)
    {
        if ( IsEnabled() && ms_logLevel >= level ) {
            wxLog *pLogger = GetActiveTarget();
            if ( pLogger )
                pLogger->DoLog(level, szString, t);
        }
    }

    // message buffering
        // flush shows all messages if they're not logged immediately (FILE
        // and iostream logs don't need it, but wxGuiLog does to avoid showing
        // 17 modal dialogs one after another)
    virtual void Flush();
        // call to Flush() may be optimized: call it only if this function
        // returns true (although Flush() also returns immediately if there is
        // no messages, this functions is more efficient because inline)
    bool HasPendingMessages() const { return m_bHasMessages; }

    // only one sink is active at each moment
        // flush the active target if any
    static void FlushActive()
    {
        if ( !ms_suspendCount )
        {
            wxLog *log = GetActiveTarget();
            if ( log && log->HasPendingMessages() )
                log->Flush();
        }
    }
        // get current log target, will call wxApp::CreateLogTarget() to
        // create one if none exists
    static wxLog *GetActiveTarget();
        // change log target, pLogger may be NULL
    static wxLog *SetActiveTarget(wxLog *pLogger);

        // suspend the message flushing of the main target until the next call
        // to Resume() - this is mainly for internal use (to prevent wxYield()
        // from flashing the messages)
    static void Suspend() { ms_suspendCount++; }
        // must be called for each Suspend()!
    static void Resume() { ms_suspendCount--; }

    // functions controlling the default wxLog behaviour
        // verbose mode is activated by standard command-line '-verbose'
        // option
    static void SetVerbose(bool bVerbose = TRUE) { ms_bVerbose = bVerbose; }

        // Set log level.  Log messages with level > logLevel will not be logged.
    static void SetLogLevel(wxLogLevel logLevel) { ms_logLevel = logLevel; }

        // should GetActiveTarget() try to create a new log object if the
        // current is NULL?
    static void DontCreateOnDemand();

        // trace mask (see wxTraceXXX constants for details)
    static void SetTraceMask(wxTraceMask ulMask) { ms_ulTraceMask = ulMask; }
        // add string trace mask
    static void AddTraceMask(const wxString& str) { ms_aTraceMasks.Add(str); }
        // add string trace mask
    static void RemoveTraceMask(const wxString& str);
        // remove all string trace masks
    static void ClearTraceMasks();
    	// get string trace masks
    static const wxArrayString &GetTraceMasks() { return ms_aTraceMasks; }

        // sets the timestamp string: this is used as strftime() format string
        // for the log targets which add time stamps to the messages - set it
        // to NULL to disable time stamping completely.
    static void SetTimestamp(const wxChar *ts) { ms_timestamp = ts; }


    // accessors
        // gets the verbose status
    static bool GetVerbose() { return ms_bVerbose; }
        // get trace mask
    static wxTraceMask GetTraceMask() { return ms_ulTraceMask; }
        // is this trace mask in the list?
    static bool IsAllowedTraceMask(const wxChar *mask)
        { return ms_aTraceMasks.Index(mask) != wxNOT_FOUND; }
        // return the current loglevel limit
    static wxLogLevel GetLogLevel() { return ms_logLevel; }

        // get the current timestamp format string (may be NULL)
    static const wxChar *GetTimestamp() { return ms_timestamp; }


    // helpers
        // put the time stamp into the string if ms_timestamp != NULL (don't
        // change it otherwise)
    static void TimeStamp(wxString *str);

    // make dtor virtual for all derived classes
    virtual ~wxLog() { }

protected:
    bool m_bHasMessages; // any messages in the queue?

    // the logging functions that can be overriden
        // default DoLog() prepends the time stamp and a prefix corresponding
        // to the message to szString and then passes it to DoLogString()
    virtual void DoLog(wxLogLevel level, const wxChar *szString, time_t t);
        // default DoLogString does nothing but is not pure virtual because if
        // you override DoLog() you might not need it at all
    virtual void DoLogString(const wxChar *szString, time_t t);

private:
    // static variables
    // ----------------

    static wxLog      *ms_pLogger;      // currently active log sink
    static bool        ms_doLog;        // FALSE => all logging disabled
    static bool        ms_bAutoCreate;  // create new log targets on demand?
    static bool        ms_bVerbose;     // FALSE => ignore LogInfo messages

    static wxLogLevel  ms_logLevel;     // limit logging to levels <= ms_logLevel

    static size_t      ms_suspendCount; // if positive, logs are not flushed

    // format string for strftime(), if NULL, time stamping log messages is
    // disabled
    static const wxChar *ms_timestamp;

    static wxTraceMask ms_ulTraceMask;   // controls wxLogTrace behaviour
    static wxArrayString ms_aTraceMasks; // more powerful filter for wxLogTrace
};

// ----------------------------------------------------------------------------
// "trivial" derivations of wxLog
// ----------------------------------------------------------------------------

// log everything to a "FILE *", stderr by default
class WXDLLEXPORT wxLogStderr : public wxLog
{
    DECLARE_NO_COPY_CLASS(wxLogStderr)

public:
    // redirect log output to a FILE
    wxLogStderr(FILE *fp = (FILE *) NULL);

protected:
    // implement sink function
    virtual void DoLogString(const wxChar *szString, time_t t);

    FILE *m_fp;
};

#if wxUSE_STD_IOSTREAM

// log everything to an "ostream", cerr by default
class WXDLLEXPORT wxLogStream : public wxLog
{
public:
    // redirect log output to an ostream
    wxLogStream(wxSTD ostream *ostr = (wxSTD ostream *) NULL);

protected:
    // implement sink function
    virtual void DoLogString(const wxChar *szString, time_t t);

    // using ptr here to avoid including <iostream.h> from this file
    wxSTD ostream *m_ostr;
};

#endif // wxUSE_STD_IOSTREAM

// ----------------------------------------------------------------------------
// /dev/null log target: suppress logging until this object goes out of scope
// ----------------------------------------------------------------------------

// example of usage:
/*
    void Foo()
    {
        wxFile file;

        // wxFile.Open() normally complains if file can't be opened, we don't
        // want it
        wxLogNull logNo;

        if ( !file.Open("bar") )
            ... process error ourselves ...

        // ~wxLogNull called, old log sink restored
    }
 */
class WXDLLEXPORT wxLogNull
{
public:
    wxLogNull() : m_flagOld(wxLog::EnableLogging(FALSE)) { }
    ~wxLogNull() { (void)wxLog::EnableLogging(m_flagOld); }

private:
    bool m_flagOld; // the previous value of the wxLog::ms_doLog
};

// ----------------------------------------------------------------------------
// chaining log target: installs itself as a log target and passes all
// messages to the real log target given to it in the ctor but also forwards
// them to the previously active one
//
// note that you don't have to call SetActiveTarget() with this class, it
// does it itself in its ctor
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxLogChain : public wxLog
{
public:
    wxLogChain(wxLog *logger);
    virtual ~wxLogChain();

    // change the new log target
    void SetLog(wxLog *logger);

    // this can be used to temporarily disable (and then reenable) passing
    // messages to the old logger (by default we do pass them)
    void PassMessages(bool bDoPass) { m_bPassMessages = bDoPass; }

    // are we passing the messages to the previous log target?
    bool IsPassingMessages() const { return m_bPassMessages; }

    // return the previous log target (may be NULL)
    wxLog *GetOldLog() const { return m_logOld; }

    // override base class version to flush the old logger as well
    virtual void Flush();

protected:
    // pass the chain to the old logger if needed
    virtual void DoLog(wxLogLevel level, const wxChar *szString, time_t t);

private:
    // the current log target
    wxLog *m_logNew;

    // the previous log target
    wxLog *m_logOld;

    // do we pass the messages to the old logger?
    bool m_bPassMessages;
};

// a chain log target which uses itself as the new logger
class WXDLLEXPORT wxLogPassThrough : public wxLogChain
{
public:
    wxLogPassThrough();
};

// ----------------------------------------------------------------------------
// the following log targets are only compiled in if the we're compiling the
// GUI part (andnot just the base one) of the library, they're implemented in
// src/generic/logg.cpp *and not src/common/log.cpp unlike all the rest)
// ----------------------------------------------------------------------------

#if wxUSE_GUI

#if wxUSE_TEXTCTRL

// log everything to a text window (GUI only of course)
class WXDLLEXPORT wxLogTextCtrl : public wxLog
{
public:
    wxLogTextCtrl(wxTextCtrl *pTextCtrl);

private:
    // implement sink function
    virtual void DoLogString(const wxChar *szString, time_t t);

    // the control we use
    wxTextCtrl *m_pTextCtrl;
};

#endif // wxUSE_TEXTCTRL

// ----------------------------------------------------------------------------
// GUI log target, the default one for wxWindows programs
// ----------------------------------------------------------------------------

#if wxUSE_LOGGUI

class WXDLLEXPORT wxLogGui : public wxLog
{
public:
    // ctor
    wxLogGui();

    // show all messages that were logged since the last Flush()
    virtual void Flush();

protected:
    virtual void DoLog(wxLogLevel level, const wxChar *szString, time_t t);

    // empty everything
    void Clear();

    wxArrayString m_aMessages;      // the log message texts
    wxArrayInt    m_aSeverity;      // one of wxLOG_XXX values
    wxArrayLong   m_aTimes;         // the time of each message
    bool          m_bErrors,        // do we have any errors?
                  m_bWarnings;      // any warnings?
};

#endif // wxUSE_LOGGUI

// ----------------------------------------------------------------------------
// (background) log window: this class forwards all log messages to the log
// target which was active when it was instantiated, but also collects them
// to the log window. This window has it's own menu which allows the user to
// close it, clear the log contents or save it to the file.
// ----------------------------------------------------------------------------

#if wxUSE_LOGWINDOW

class WXDLLEXPORT wxLogWindow : public wxLogPassThrough
{
public:
    wxLogWindow(wxFrame *pParent,         // the parent frame (can be NULL)
                const wxChar *szTitle,    // the title of the frame
                bool bShow = TRUE,        // show window immediately?
                bool bPassToOld = TRUE);  // pass messages to the old target?

    ~wxLogWindow();

    // window operations
        // show/hide the log window
    void Show(bool bShow = TRUE);
        // retrieve the pointer to the frame
    wxFrame *GetFrame() const;

    // overridables
        // called immediately after the log frame creation allowing for
        // any extra initializations
    virtual void OnFrameCreate(wxFrame *frame);
        // called if the user closes the window interactively, will not be
        // called if it is destroyed for another reason (such as when program
        // exits) - return TRUE from here to allow the frame to close, FALSE
        // to prevent this from happening
    virtual bool OnFrameClose(wxFrame *frame);
        // called right before the log frame is going to be deleted: will
        // always be called unlike OnFrameClose()
    virtual void OnFrameDelete(wxFrame *frame);

protected:
    virtual void DoLog(wxLogLevel level, const wxChar *szString, time_t t);
    virtual void DoLogString(const wxChar *szString, time_t t);

private:
    wxLogFrame *m_pLogFrame;      // the log frame
};

#endif // wxUSE_LOGWINDOW

#endif // wxUSE_GUI

// ============================================================================
// global functions
// ============================================================================

// ----------------------------------------------------------------------------
// Log functions should be used by application instead of stdio, iostream &c
// for log messages for easy redirection
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// get error code/error message from system in a portable way
// ----------------------------------------------------------------------------

// return the last system error code
WXDLLEXPORT unsigned long wxSysErrorCode();

// return the error message for given (or last if 0) error code
WXDLLEXPORT const wxChar* wxSysErrorMsg(unsigned long nErrCode = 0);

// ----------------------------------------------------------------------------
// define wxLog<level>
// ----------------------------------------------------------------------------

#define DECLARE_LOG_FUNCTION(level)                                 \
extern void WXDLLEXPORT wxVLog##level(const wxChar *szFormat,       \
                                      va_list argptr);              \
extern void WXDLLEXPORT wxLog##level(const wxChar *szFormat,        \
                                     ...) ATTRIBUTE_PRINTF_1
#define DECLARE_LOG_FUNCTION2(level, arg1)                          \
extern void WXDLLEXPORT wxVLog##level(arg1, const wxChar *szFormat, \
                                      va_list argptr);              \
extern void WXDLLEXPORT wxLog##level(arg1, const wxChar *szFormat,  \
                                     ...) ATTRIBUTE_PRINTF_2

#else // !wxUSE_LOG

// log functions do nothing at all
#define DECLARE_LOG_FUNCTION(level)                                 \
inline void WXDLLEXPORT wxVLog##level(const wxChar *szFormat,       \
                                     va_list argptr) {}             \
inline void WXDLLEXPORT wxLog##level(const wxChar *szFormat, ...) {}
#define DECLARE_LOG_FUNCTION2(level, arg1)                          \
inline void WXDLLEXPORT wxVLog##level(arg1, const wxChar *szFormat, \
                                     va_list argptr) {}             \
inline void WXDLLEXPORT wxLog##level(arg1, const wxChar *szFormat, ...) {}

#endif // wxUSE_LOG/!wxUSE_LOG

// a generic function for all levels (level is passes as parameter)
DECLARE_LOG_FUNCTION2(Generic, wxLogLevel level);

// one function per each level
DECLARE_LOG_FUNCTION(FatalError);
DECLARE_LOG_FUNCTION(Error);
DECLARE_LOG_FUNCTION(Warning);
DECLARE_LOG_FUNCTION(Message);
DECLARE_LOG_FUNCTION(Info);
DECLARE_LOG_FUNCTION(Verbose);

// this function sends the log message to the status line of the top level
// application frame, if any
DECLARE_LOG_FUNCTION(Status);

// this one is the same as previous except that it allows to explicitly
// specify the frame to which the output should go
DECLARE_LOG_FUNCTION2(Status, wxFrame *pFrame);

// additional one: as wxLogError, but also logs last system call error code
// and the corresponding error message if available
DECLARE_LOG_FUNCTION(SysError);

// and another one which also takes the error code (for those broken APIs
// that don't set the errno (like registry APIs in Win32))
DECLARE_LOG_FUNCTION2(SysError, long lErrCode);

// debug functions do nothing in release mode
#if wxUSE_LOG_DEBUG
    DECLARE_LOG_FUNCTION(Debug);

    // first kind of LogTrace is unconditional: it doesn't check the level,
    DECLARE_LOG_FUNCTION(Trace);

    // this second version will only log the message if the mask had been
    // added to the list of masks with AddTraceMask()
    DECLARE_LOG_FUNCTION2(Trace, const wxChar *mask);

    // the last one does nothing if all of level bits are not set
    // in wxLog::GetActive()->GetTraceMask() - it's deprecated in favour of
    // string identifiers
    DECLARE_LOG_FUNCTION2(Trace, wxTraceMask mask);
#else   //!debug
    // these functions do nothing in release builds
    inline void wxVLogDebug(const wxChar *, va_list) { }
    inline void wxLogDebug(const wxChar *, ...) { }
    inline void wxVLogTrace(const wxChar *, va_list) { }
    inline void wxLogTrace(const wxChar *, ...) { }
    inline void wxVLogTrace(wxTraceMask, const wxChar *, va_list) { }
    inline void wxLogTrace(wxTraceMask, const wxChar *, ...) { }
    inline void wxVLogTrace(const wxChar *, const wxChar *, va_list) { }
    inline void wxLogTrace(const wxChar *, const wxChar *, ...) { }
#endif // debug/!debug

// wxLogFatalError helper: show the (fatal) error to the user in a safe way,
// i.e. without using wxMessageBox() for example because it could crash
void WXDLLEXPORT wxSafeShowMessage(const wxString& title, const wxString& text);

// ----------------------------------------------------------------------------
// debug only logging functions: use them with API name and error code
// ----------------------------------------------------------------------------

#ifdef __WXDEBUG__
    // make life easier for people using VC++ IDE: clicking on the message
    // will take us immediately to the place of the failed API
#ifdef __VISUALC__
    #define wxLogApiError(api, rc)                                            \
        wxLogDebug(wxT("%s(%d): '%s' failed with error 0x%08lx (%s)."),       \
                   __TFILE__, __LINE__, api,                                  \
                   (long)rc, wxSysErrorMsg(rc))
#else // !VC++
    #define wxLogApiError(api, rc)                                            \
        wxLogDebug(wxT("In file %s at line %d: '%s' failed with "             \
                       "error 0x%08lx (%s)."),                                \
                   __TFILE__, __LINE__, api,                                  \
                   (long)rc, wxSysErrorMsg(rc))
#endif // VC++/!VC++

    #define wxLogLastError(api) wxLogApiError(api, wxSysErrorCode())

#else   //!debug
    inline void wxLogApiError(const wxChar *, long) { }
    inline void wxLogLastError(const wxChar *) { }
#endif  //debug/!debug

#endif  // _WX_LOG_H_

// vi:sts=4:sw=4:et
