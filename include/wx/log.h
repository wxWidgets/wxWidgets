/////////////////////////////////////////////////////////////////////////////
// Name:        log.h
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

#ifdef    __GNUG__
#pragma interface "log.h"
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
  wxLOG_Info,       // informational message (a.k.a. 'Verbose')
  wxLOG_Status,     // informational: might go to the status line of GUI app
  wxLOG_Debug,      // never shown to the user, disabled in release mode
  wxLOG_Trace,      // trace messages are also only enabled in debug mode
  wxLOG_Progress,   // used for progress indicator (not yet)
  wxLOG_User = 100  // user defined levels start here
};

// meaning of different bits of the trace mask (which allows selectively
// enable/disable some trace messages)
#define wxTraceMemAlloc 0x0001  // trace memory allocation (new/delete)
#define wxTraceMessages 0x0002  // trace window messages/X callbacks
#define wxTraceResAlloc 0x0004  // trace GDI resource allocation
#define wxTraceRefCount 0x0008  // trace various ref counting operations

#ifdef  __WXMSW__
  #define wxTraceOleCalls 0x0100  // OLE interface calls
#endif

typedef unsigned long wxTraceMask;
typedef unsigned long wxLogLevel;

// ----------------------------------------------------------------------------
// forward declarations
// ----------------------------------------------------------------------------
class wxTextCtrl;
class wxLogFrame;
class wxFrame;
class ostream;

// ----------------------------------------------------------------------------
// derive from this class to redirect (or suppress, or ...) log messages
// normally, only a single instance of this class exists but it's not enforced
// ----------------------------------------------------------------------------
class WXDLLEXPORT wxLog
{
public:
  // ctor
  wxLog();

  // sink function
  static void OnLog(wxLogLevel level, const char *szString)
    { if ( ms_pLogger != 0 ) ms_pLogger->DoLog(level, szString); }

  // message buffering
    // flush shows all messages if they're not logged immediately
    // (FILE and iostream logs don't need it, but wxGuiLog does to avoid
    //  showing 17 modal dialogs one after another)
  virtual void Flush();
    // call to Flush() may be optimized: call it only if this function
    // returns true (although Flush() also returns immediately if there
    // is no messages, this functions is more efficient because inline)
  bool HasPendingMessages() const { return m_bHasMessages; }

  // only one sink is active at each moment
    // get current log target, will call wxApp::CreateLogTarget() to create one
    // if none exists
  static wxLog *GetActiveTarget();
    // change log target, pLogger = NULL disables logging. if bNoFlashOld is true,
    // the old log target isn't flashed which might lead to loss of messages!
    // returns the previous log target
  static wxLog *SetActiveTarget(wxLog *pLogger, bool bNoFlashOld = FALSE);

  // functions controlling the default wxLog behaviour
    // verbose mode is activated by standard command-line '-verbose' option
  void SetVerbose(bool bVerbose = TRUE) { m_bVerbose = bVerbose; }
    // sets the format for timestamp prepended by wxLog::DoLog(): it's
    // passed to strftime() function, see it's documentation for details.
    // no time stamp at all if szTF is NULL or empty
    // NB: the string is not copied, so it's lifetime must be long enough!
  void SetTimeStampFormat(const char *szTF) { m_szTimeFormat = szTF; }
    // trace mask (see wxTraceXXX constants for details)
  static void SetTraceMask(wxTraceMask ulMask) { ms_ulTraceMask = ulMask; }
    // should GetActiveTarget() try to create a new log object if the current
    // is NULL?
  static void DontCreateOnDemand() { ms_bAutoCreate = FALSE; }

  // accessors
    // gets the verbose status
  bool GetVerbose() const { return m_bVerbose; }
    // get current time format
  const char *GetTimeStampFormat() const { return m_szTimeFormat; }
    // get trace mask
  static wxTraceMask GetTraceMask() { return ms_ulTraceMask; }

  // make dtor virtual for all derived classes
  virtual ~wxLog() { }

protected:
  bool m_bHasMessages;

  bool          m_bVerbose;     // FALSE => ignore LogInfo messages
  const char   *m_szTimeFormat; // format for strftime()

  // the logging functions that can be overriden
    // default DoLog() prepends the time stamp and a prefix corresponding
    // to the message to szString and then passes it to DoLogString()
  virtual void DoLog(wxLogLevel level, const char *szString);
    // default DoLogString does nothing but is not pure virtual because if
    // you override DoLog() you might not need it at all
  virtual void DoLogString(const char *szString);

  // helpers
    // put the time stamp in the current format into the string
  wxString TimeStamp() const;

private:
  // static variables
  // ----------------
  static wxLog      *ms_pLogger;      // currently active log sink
  static bool        ms_bAutoCreate;  // automatically create new log targets?
  static wxTraceMask ms_ulTraceMask;  // controls wxLogTrace behaviour
};

// ----------------------------------------------------------------------------
// "trivial" derivations of wxLog
// ----------------------------------------------------------------------------

// log everything to a "FILE *", stderr by default
class WXDLLEXPORT wxLogStderr : public wxLog
{
public:
  // redirect log output to a FILE
  wxLogStderr(FILE *fp = (FILE *) NULL);

private:
  // implement sink function
  virtual void DoLogString(const char *szString);

  FILE *m_fp;
};

// log everything to an "ostream", cerr by default
class ostream;
class WXDLLEXPORT wxLogStream : public wxLog
{
public:
  // redirect log output to an ostream
  wxLogStream(ostream *ostr = (ostream *) NULL);

protected:
  // implement sink function
  virtual void DoLogString(const char *szString);

  // @@ using ptr here to avoid including <iostream.h> from this file
  ostream *m_ostr;
};

// log everything to a text window (GUI only of course)
class WXDLLEXPORT wxLogTextCtrl : public wxLogStream
{
public:
  // we just create an ostream from wxTextCtrl and use it in base class
  wxLogTextCtrl(wxTextCtrl *pTextCtrl);
 ~wxLogTextCtrl();
};

// ----------------------------------------------------------------------------
// GUI log target, the default one for wxWindows programs
// ----------------------------------------------------------------------------
class WXDLLEXPORT wxLogGui : public wxLog
{
public:
  // ctor
  wxLogGui();

  // show all messages that were logged since the last Flush()
  virtual void Flush();

protected:
  virtual void DoLog(wxLogLevel level, const char *szString);

  wxArrayString m_aMessages;
  bool          m_bErrors;
};

// ----------------------------------------------------------------------------
// (background) log window: this class forwards all log messages to the log
// target which was active when it was instantiated, but also collects them
// to the log window. This window has it's own menu which allows the user to
// close it, clear the log contents or save it to the file.
// ----------------------------------------------------------------------------
class WXDLLEXPORT wxLogWindow : public wxLog
{
public:
  wxLogWindow(wxFrame *pParent,         // the parent frame (can be NULL)
              const char *szTitle,      // the title of the frame
              bool bShow = TRUE,        // show window immediately?
              bool bPassToOld = TRUE);  // pass log messages to the old target?
  ~wxLogWindow();

  // window operations
    // show/hide the log window
  void Show(bool bShow = TRUE);
    // retrieve the pointer to the frame
  wxFrame *GetFrame() const;

  // accessors
    // the previous log target (may be NULL)
  wxLog *GetOldLog() const { return m_pOldLog; }
    // are we passing the messages to the previous log target?
  bool IsPassingMessages() const { return m_bPassMessages; }

  // we can pass the messages to the previous log target (we're in this mode by
  // default: we collect all messages in the window, but also let the default
  // processing take place)
  void PassMessages(bool bDoPass) { m_bPassMessages = bDoPass; }

  // base class virtuals
    // we don't need it ourselves, but we pass it to the previous logger
  virtual void Flush();

  // overridables
    // called immediately after the log frame creation allowing for
    // any extra initializations
  virtual void OnFrameCreate(wxFrame *frame);
    // called right before the log frame is going to be deleted
  virtual void OnFrameDelete(wxFrame *frame);

protected:
  virtual void DoLog(wxLogLevel level, const char *szString);
  virtual void DoLogString(const char *szString);

private:
  bool        m_bPassMessages;  // pass messages to m_pOldLog?
  wxLog      *m_pOldLog;        // previous log target
  wxLogFrame *m_pLogFrame;      // the log frame
};

// ----------------------------------------------------------------------------
// /dev/null log target: suppress logging until this object goes out of scope
// ----------------------------------------------------------------------------

// example of usage:
/*
void Foo() {
  wxFile file;

  // wxFile.Open() normally complains if file can't be opened, we don't want it
  wxLogNull logNo;
  if ( !file.Open("bar") )
    ... process error ourselves ...

  // ~wxLogNull called, old log sink restored
}
*/
class WXDLLEXPORT wxLogNull
{
public:
  // ctor saves old log target, dtor restores it
  wxLogNull() { m_pPrevLogger = wxLog::SetActiveTarget((wxLog *)NULL, TRUE); }
 ~wxLogNull() { (void)wxLog::SetActiveTarget(m_pPrevLogger);  }

private:
  wxLog *m_pPrevLogger; // old log target
};

// ============================================================================
// global functions
// ============================================================================

// ----------------------------------------------------------------------------
// Log functions should be used by application instead of stdio, iostream &c
// for log messages for easy redirection
// ----------------------------------------------------------------------------

// define wxLog<level>
// -------------------

#define DECLARE_LOG_FUNCTION(level)                                 \
        extern void WXDLLEXPORT wxLog##level(const char *szFormat, ...)
#define DECLARE_LOG_FUNCTION2(level, arg1)                          \
        extern void WXDLLEXPORT wxLog##level(arg1, const char *szFormat, ...)

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
#ifdef  __WXDEBUG__
  DECLARE_LOG_FUNCTION(Debug);

  // first king of LogTrace is uncoditional: it doesn't check the level,
  // while the second one does nothing if all of level bits are not set
  // in wxLog::GetActive()->GetTraceMask().
  DECLARE_LOG_FUNCTION(Trace);
  DECLARE_LOG_FUNCTION2(Trace, wxTraceMask mask);
#else   //!debug
  // these functions do nothing
  inline void wxLogDebug(const char *, ...) { }
  inline void wxLogTrace(const char *, ...) { }
  inline void wxLogTrace(wxTraceMask, const char *, ...) { }
#endif


// are we in 'verbose' mode?
// (note that it's often handy to change this var manually from the
//  debugger, thus enabling/disabling verbose reporting for some
//  parts of the program only)
WXDLLEXPORT_DATA(extern bool) g_bVerbose;

// fwd decl to avoid including iostream.h here
class ostream;

// ----------------------------------------------------------------------------
// get error code/error message from system in a portable way
// ----------------------------------------------------------------------------

// return the last system error code
unsigned long WXDLLEXPORT wxSysErrorCode();
// return the error message for given (or last if 0) error code
const char* WXDLLEXPORT wxSysErrorMsg(unsigned long nErrCode = 0);

// ----------------------------------------------------------------------------
// debug only logging functions: use them with API name and error code
// ----------------------------------------------------------------------------

#ifdef  __WXDEBUG__
  #define wxLogApiError(api, rc)                                              \
                    wxLogDebug("At %s(%d) '%s' failed with error %lx (%s).",  \
                               __FILE__, __LINE__, api,                       \
                               rc, wxSysErrorMsg(rc))
  #define wxLogLastError(api) wxLogApiError(api, wxSysErrorCode())
#else   //!debug
  inline void wxLogApiError(const char *, long) { }
  inline void wxLogLastError(const char *) { }
#endif  //debug/!debug

#endif  // _WX_LOG_H_
