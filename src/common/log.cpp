/////////////////////////////////////////////////////////////////////////////
// Name:        log.cpp
// Purpose:     Assorted wxLogXXX functions, and wxLog (sink for logs)
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "log.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_LOG

// wxWidgets
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/arrstr.h"
    #include "wx/intl.h"
    #include "wx/string.h"
#endif //WX_PRECOMP

#include "wx/apptrait.h"
#include "wx/file.h"
#include "wx/log.h"
#include "wx/msgout.h"
#include "wx/textfile.h"
#include "wx/thread.h"
#include "wx/utils.h"
#include "wx/wxchar.h"

// other standard headers
#ifndef __WXWINCE__
#include <errno.h>
#endif

#include <stdlib.h>

#ifndef __WXWINCE__
#include <time.h>
#else
#include "wx/msw/wince/time.h"
#endif

#if defined(__WINDOWS__)
    #include "wx/msw/private.h" // includes windows.h
#endif

// ----------------------------------------------------------------------------
// non member functions
// ----------------------------------------------------------------------------

// define this to enable wrapping of log messages
//#define LOG_PRETTY_WRAP

#ifdef  LOG_PRETTY_WRAP
  static void wxLogWrap(FILE *f, const char *pszPrefix, const char *psz);
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// log functions can't allocate memory (LogError("out of memory...") should
// work!), so we use a static buffer for all log messages
#define LOG_BUFFER_SIZE   (4096)

// static buffer for error messages
static wxChar   s_szBufStatic[LOG_BUFFER_SIZE];

static wxChar  *s_szBuf     = s_szBufStatic;
static size_t   s_szBufSize = WXSIZEOF( s_szBufStatic );

#if wxUSE_THREADS

// the critical section protecting the static buffer
static wxCriticalSection gs_csLogBuf;

#endif // wxUSE_THREADS

// return true if we have a non NULL non disabled log target
static inline bool IsLoggingEnabled()
{
    return wxLog::IsEnabled() && (wxLog::GetActiveTarget() != NULL);
}

// ----------------------------------------------------------------------------
// implementation of Log functions
//
// NB: unfortunately we need all these distinct functions, we can't make them
//     macros and not all compilers inline vararg functions.
// ----------------------------------------------------------------------------

// wrapper for wxVsnprintf(s_szBuf) which always NULL-terminates it
static inline void PrintfInLogBug(const wxChar *szFormat, va_list argptr)
{
    if ( wxVsnprintf(s_szBuf, s_szBufSize, szFormat, argptr) < 0 )
    {
        // must NUL-terminate it manually
        s_szBuf[s_szBufSize - 1] = _T('\0');
    }
    //else: NUL-terminated by vsnprintf()
}

// generic log function
void wxVLogGeneric(wxLogLevel level, const wxChar *szFormat, va_list argptr)
{
    if ( IsLoggingEnabled() ) {
        wxCRIT_SECT_LOCKER(locker, gs_csLogBuf);

        PrintfInLogBug(szFormat, argptr);

        wxLog::OnLog(level, s_szBuf, time(NULL));
    }
}

void wxLogGeneric(wxLogLevel level, const wxChar *szFormat, ...)
{
    va_list argptr;
    va_start(argptr, szFormat);
    wxVLogGeneric(level, szFormat, argptr);
    va_end(argptr);
}

#define IMPLEMENT_LOG_FUNCTION(level)                               \
  void wxVLog##level(const wxChar *szFormat, va_list argptr)        \
  {                                                                 \
    if ( IsLoggingEnabled() ) {                                     \
      wxCRIT_SECT_LOCKER(locker, gs_csLogBuf);                      \
                                                                    \
      PrintfInLogBug(szFormat, argptr);                             \
                                                                    \
      wxLog::OnLog(wxLOG_##level, s_szBuf, time(NULL));             \
    }                                                               \
  }                                                                 \
                                                                    \
  void wxLog##level(const wxChar *szFormat, ...)                    \
  {                                                                 \
    va_list argptr;                                                 \
    va_start(argptr, szFormat);                                     \
    wxVLog##level(szFormat, argptr);                                \
    va_end(argptr);                                                 \
  }

IMPLEMENT_LOG_FUNCTION(Error)
IMPLEMENT_LOG_FUNCTION(Warning)
IMPLEMENT_LOG_FUNCTION(Message)
IMPLEMENT_LOG_FUNCTION(Info)
IMPLEMENT_LOG_FUNCTION(Status)

void wxSafeShowMessage(const wxString& title, const wxString& text)
{
#ifdef __WINDOWS__
    ::MessageBox(NULL, text, title, MB_OK | MB_ICONSTOP);
#else
    wxFprintf(stderr, _T("%s: %s\n"), title.c_str(), text.c_str());
#endif
}

// fatal errors can't be suppressed nor handled by the custom log target and
// always terminate the program
void wxVLogFatalError(const wxChar *szFormat, va_list argptr)
{
    wxVsnprintf(s_szBuf, s_szBufSize, szFormat, argptr);

    wxSafeShowMessage(_T("Fatal Error"), s_szBuf);

#ifdef __WXWINCE__
    ExitThread(3);
#else
    abort();
#endif
}

void wxLogFatalError(const wxChar *szFormat, ...)
{
    va_list argptr;
    va_start(argptr, szFormat);
    wxVLogFatalError(szFormat, argptr);

    // some compilers warn about unreachable code and it shouldn't matter
    // for the others anyhow...
    //va_end(argptr);
}

// same as info, but only if 'verbose' mode is on
void wxVLogVerbose(const wxChar *szFormat, va_list argptr)
{
    if ( IsLoggingEnabled() ) {
        wxLog *pLog = wxLog::GetActiveTarget();
        if ( pLog != NULL && pLog->GetVerbose() ) {
            wxCRIT_SECT_LOCKER(locker, gs_csLogBuf);

            wxVsnprintf(s_szBuf, s_szBufSize, szFormat, argptr);

            wxLog::OnLog(wxLOG_Info, s_szBuf, time(NULL));
        }
    }
}

void wxLogVerbose(const wxChar *szFormat, ...)
{
    va_list argptr;
    va_start(argptr, szFormat);
    wxVLogVerbose(szFormat, argptr);
    va_end(argptr);
}

// debug functions
#ifdef __WXDEBUG__
#define IMPLEMENT_LOG_DEBUG_FUNCTION(level)                         \
  void wxVLog##level(const wxChar *szFormat, va_list argptr)        \
  {                                                                 \
    if ( IsLoggingEnabled() ) {                                     \
      wxCRIT_SECT_LOCKER(locker, gs_csLogBuf);                      \
                                                                    \
      wxVsnprintf(s_szBuf, s_szBufSize, szFormat, argptr);    \
                                                                    \
      wxLog::OnLog(wxLOG_##level, s_szBuf, time(NULL));             \
    }                                                               \
  }                                                                 \
  void wxLog##level(const wxChar *szFormat, ...)                    \
  {                                                                 \
    va_list argptr;                                                 \
    va_start(argptr, szFormat);                                     \
    wxVLog##level(szFormat, argptr);                                \
    va_end(argptr);                                                 \
  }

  void wxVLogTrace(const wxChar *mask, const wxChar *szFormat, va_list argptr)
  {
    if ( IsLoggingEnabled() && wxLog::IsAllowedTraceMask(mask) ) {
      wxCRIT_SECT_LOCKER(locker, gs_csLogBuf);

      wxChar *p = s_szBuf;
      size_t len = s_szBufSize;
      wxStrncpy(s_szBuf, _T("("), len);
      len -= 1; // strlen("(")
      p += 1;
      wxStrncat(p, mask, len);
      size_t lenMask = wxStrlen(mask);
      len -= lenMask;
      p += lenMask;

      wxStrncat(p, _T(") "), len);
      len -= 2;
      p += 2;

      wxVsnprintf(p, len, szFormat, argptr);

      wxLog::OnLog(wxLOG_Trace, s_szBuf, time(NULL));
    }
  }

  void wxLogTrace(const wxChar *mask, const wxChar *szFormat, ...)
  {
    va_list argptr;
    va_start(argptr, szFormat);
    wxVLogTrace(mask, szFormat, argptr);
    va_end(argptr);
  }

  void wxVLogTrace(wxTraceMask mask, const wxChar *szFormat, va_list argptr)
  {
    // we check that all of mask bits are set in the current mask, so
    // that wxLogTrace(wxTraceRefCount | wxTraceOle) will only do something
    // if both bits are set.
    if ( IsLoggingEnabled() && ((wxLog::GetTraceMask() & mask) == mask) ) {
      wxCRIT_SECT_LOCKER(locker, gs_csLogBuf);

      wxVsnprintf(s_szBuf, s_szBufSize, szFormat, argptr);

      wxLog::OnLog(wxLOG_Trace, s_szBuf, time(NULL));
    }
  }

  void wxLogTrace(wxTraceMask mask, const wxChar *szFormat, ...)
  {
    va_list argptr;
    va_start(argptr, szFormat);
    wxVLogTrace(mask, szFormat, argptr);
    va_end(argptr);
  }

#else // release
  #define IMPLEMENT_LOG_DEBUG_FUNCTION(level)
#endif

IMPLEMENT_LOG_DEBUG_FUNCTION(Debug)
IMPLEMENT_LOG_DEBUG_FUNCTION(Trace)

// wxLogSysError: one uses the last error code, for other  you must give it
// explicitly

// common part of both wxLogSysError
void wxLogSysErrorHelper(long lErrCode)
{
    wxChar szErrMsg[LOG_BUFFER_SIZE / 2];
    wxSnprintf(szErrMsg, WXSIZEOF(szErrMsg),
               _(" (error %ld: %s)"), lErrCode, wxSysErrorMsg(lErrCode));
    wxStrncat(s_szBuf, szErrMsg, s_szBufSize - wxStrlen(s_szBuf));

    wxLog::OnLog(wxLOG_Error, s_szBuf, time(NULL));
}

void WXDLLEXPORT wxVLogSysError(const wxChar *szFormat, va_list argptr)
{
    if ( IsLoggingEnabled() ) {
        wxCRIT_SECT_LOCKER(locker, gs_csLogBuf);

        wxVsnprintf(s_szBuf, s_szBufSize, szFormat, argptr);

        wxLogSysErrorHelper(wxSysErrorCode());
    }
}

void WXDLLEXPORT wxLogSysError(const wxChar *szFormat, ...)
{
    va_list argptr;
    va_start(argptr, szFormat);
    wxVLogSysError(szFormat, argptr);
    va_end(argptr);
}

void WXDLLEXPORT wxVLogSysError(long lErrCode, const wxChar *szFormat, va_list argptr)
{
    if ( IsLoggingEnabled() ) {
        wxCRIT_SECT_LOCKER(locker, gs_csLogBuf);

        wxVsnprintf(s_szBuf, s_szBufSize, szFormat, argptr);

        wxLogSysErrorHelper(lErrCode);
    }
}

void WXDLLEXPORT wxLogSysError(long lErrCode, const wxChar *szFormat, ...)
{
    va_list argptr;
    va_start(argptr, szFormat);
    wxVLogSysError(lErrCode, szFormat, argptr);
    va_end(argptr);
}

// ----------------------------------------------------------------------------
// wxLog class implementation
// ----------------------------------------------------------------------------

wxChar *wxLog::SetLogBuffer( wxChar *buf, size_t size)
{
    wxChar *oldbuf = s_szBuf;

    if( buf == 0 )
    {
        s_szBuf = s_szBufStatic;
        s_szBufSize = WXSIZEOF( s_szBufStatic );
    }
    else
    {
        s_szBuf = buf;
        s_szBufSize = size;
    }

    return (oldbuf == s_szBufStatic ) ? 0 : oldbuf;
}

wxLog *wxLog::GetActiveTarget()
{
    if ( ms_bAutoCreate && ms_pLogger == NULL ) {
        // prevent infinite recursion if someone calls wxLogXXX() from
        // wxApp::CreateLogTarget()
        static bool s_bInGetActiveTarget = false;
        if ( !s_bInGetActiveTarget ) {
            s_bInGetActiveTarget = true;

            // ask the application to create a log target for us
            if ( wxTheApp != NULL )
                ms_pLogger = wxTheApp->GetTraits()->CreateLogTarget();
            else
                ms_pLogger = new wxLogStderr;

            s_bInGetActiveTarget = false;

            // do nothing if it fails - what can we do?
        }
    }

    return ms_pLogger;
}

wxLog *wxLog::SetActiveTarget(wxLog *pLogger)
{
    if ( ms_pLogger != NULL ) {
        // flush the old messages before changing because otherwise they might
        // get lost later if this target is not restored
        ms_pLogger->Flush();
    }

    wxLog *pOldLogger = ms_pLogger;
    ms_pLogger = pLogger;

    return pOldLogger;
}

void wxLog::DontCreateOnDemand()
{
    ms_bAutoCreate = false;

    // this is usually called at the end of the program and we assume that it
    // is *always* called at the end - so we free memory here to avoid false
    // memory leak reports from wxWin  memory tracking code
    ClearTraceMasks();
}

void wxLog::RemoveTraceMask(const wxString& str)
{
    int index = ms_aTraceMasks.Index(str);
    if ( index != wxNOT_FOUND )
        ms_aTraceMasks.RemoveAt((size_t)index);
}

void wxLog::ClearTraceMasks()
{
    ms_aTraceMasks.Clear();
}

void wxLog::TimeStamp(wxString *str)
{
    if ( ms_timestamp )
    {
        wxChar buf[256];
        time_t timeNow;
        (void)time(&timeNow);
        wxStrftime(buf, WXSIZEOF(buf), ms_timestamp, localtime(&timeNow));

        str->Empty();
        *str << buf << wxT(": ");
    }
}

void wxLog::DoLog(wxLogLevel level, const wxChar *szString, time_t t)
{
    switch ( level ) {
        case wxLOG_FatalError:
            DoLogString(wxString(_("Fatal error: ")) + szString, t);
            DoLogString(_("Program aborted."), t);
            Flush();
#ifdef __WXWINCE__
            ExitThread(3);
#else
            abort();
#endif
            break;

        case wxLOG_Error:
            DoLogString(wxString(_("Error: ")) + szString, t);
            break;

        case wxLOG_Warning:
            DoLogString(wxString(_("Warning: ")) + szString, t);
            break;

        case wxLOG_Info:
            if ( GetVerbose() )
        case wxLOG_Message:
        case wxLOG_Status:
        default:    // log unknown log levels too
                DoLogString(szString, t);
            break;

        case wxLOG_Trace:
        case wxLOG_Debug:
#ifdef __WXDEBUG__
            {
                wxString msg = level == wxLOG_Trace ? wxT("Trace: ")
                                                    : wxT("Debug: ");
                msg << szString;
                DoLogString(msg, t);
            }
#endif // Debug
            break;
    }
}

void wxLog::DoLogString(const wxChar *WXUNUSED(szString), time_t WXUNUSED(t))
{
    wxFAIL_MSG(wxT("DoLogString must be overriden if it's called."));
}

void wxLog::Flush()
{
    // nothing to do here
}

/*static*/ bool wxLog::IsAllowedTraceMask(const wxChar *mask)
{
    for ( wxArrayString::iterator it = ms_aTraceMasks.begin(),
                                  en = ms_aTraceMasks.end();
         it != en; ++it )
        if ( *it == mask)
            return true;
    return false;
}

// ----------------------------------------------------------------------------
// wxLogStderr class implementation
// ----------------------------------------------------------------------------

wxLogStderr::wxLogStderr(FILE *fp)
{
    if ( fp == NULL )
        m_fp = stderr;
    else
        m_fp = fp;
}

void wxLogStderr::DoLogString(const wxChar *szString, time_t WXUNUSED(t))
{
    wxString str;
    TimeStamp(&str);
    str << szString;

    fputs(str.mb_str(), m_fp);
    fputc(_T('\n'), m_fp);
    fflush(m_fp);

    // under GUI systems such as Windows or Mac, programs usually don't have
    // stderr at all, so show the messages also somewhere else, typically in
    // the debugger window so that they go at least somewhere instead of being
    // simply lost
    if ( m_fp == stderr )
    {
        wxAppTraits *traits = wxTheApp ? wxTheApp->GetTraits() : NULL;
        if ( traits && !traits->HasStderr() )
        {
            wxMessageOutputDebug dbgout;
            dbgout.Printf(_T("%s\n"), str.c_str());
        }
    }
}

// ----------------------------------------------------------------------------
// wxLogStream implementation
// ----------------------------------------------------------------------------

#if wxUSE_STD_IOSTREAM
#include "wx/ioswrap.h"
wxLogStream::wxLogStream(wxSTD ostream *ostr)
{
    if ( ostr == NULL )
        m_ostr = &wxSTD cerr;
    else
        m_ostr = ostr;
}

void wxLogStream::DoLogString(const wxChar *szString, time_t WXUNUSED(t))
{
    wxString str;
    TimeStamp(&str);
    (*m_ostr) << str << wxConvertWX2MB(szString) << wxSTD endl;
}
#endif // wxUSE_STD_IOSTREAM

// ----------------------------------------------------------------------------
// wxLogChain
// ----------------------------------------------------------------------------

wxLogChain::wxLogChain(wxLog *logger)
{
    m_bPassMessages = true;

    m_logNew = logger;
    m_logOld = wxLog::SetActiveTarget(this);
}

wxLogChain::~wxLogChain()
{
    delete m_logOld;

    if ( m_logNew != this )
        delete m_logNew;
}

void wxLogChain::SetLog(wxLog *logger)
{
    if ( m_logNew != this )
        delete m_logNew;

    m_logNew = logger;
}

void wxLogChain::Flush()
{
    if ( m_logOld )
        m_logOld->Flush();

    // be careful to avoid infinite recursion
    if ( m_logNew && m_logNew != this )
        m_logNew->Flush();
}

void wxLogChain::DoLog(wxLogLevel level, const wxChar *szString, time_t t)
{
    // let the previous logger show it
    if ( m_logOld && IsPassingMessages() )
    {
        // bogus cast just to access protected DoLog
        ((wxLogChain *)m_logOld)->DoLog(level, szString, t);
    }

    if ( m_logNew && m_logNew != this )
    {
        // as above...
        ((wxLogChain *)m_logNew)->DoLog(level, szString, t);
    }
}

// ----------------------------------------------------------------------------
// wxLogPassThrough
// ----------------------------------------------------------------------------

#ifdef __VISUALC__
    // "'this' : used in base member initializer list" - so what?
    #pragma warning(disable:4355)
#endif // VC++

wxLogPassThrough::wxLogPassThrough()
                : wxLogChain(this)
{
}

#ifdef __VISUALC__
    #pragma warning(default:4355)
#endif // VC++

// ============================================================================
// Global functions/variables
// ============================================================================

// ----------------------------------------------------------------------------
// static variables
// ----------------------------------------------------------------------------

wxLog          *wxLog::ms_pLogger      = (wxLog *)NULL;
bool            wxLog::ms_doLog        = true;
bool            wxLog::ms_bAutoCreate  = true;
bool            wxLog::ms_bVerbose     = false;

wxLogLevel      wxLog::ms_logLevel     = wxLOG_Max;  // log everything by default

size_t          wxLog::ms_suspendCount = 0;

const wxChar   *wxLog::ms_timestamp    = wxT("%X");  // time only, no date

wxTraceMask     wxLog::ms_ulTraceMask  = (wxTraceMask)0;
wxArrayString   wxLog::ms_aTraceMasks;

// ----------------------------------------------------------------------------
// stdout error logging helper
// ----------------------------------------------------------------------------

// helper function: wraps the message and justifies it under given position
// (looks more pretty on the terminal). Also adds newline at the end.
//
// TODO this is now disabled until I find a portable way of determining the
//      terminal window size (ok, I found it but does anybody really cares?)
#ifdef LOG_PRETTY_WRAP
static void wxLogWrap(FILE *f, const char *pszPrefix, const char *psz)
{
    size_t nMax = 80; // FIXME
    size_t nStart = strlen(pszPrefix);
    fputs(pszPrefix, f);

    size_t n;
    while ( *psz != '\0' ) {
        for ( n = nStart; (n < nMax) && (*psz != '\0'); n++ )
            putc(*psz++, f);

        // wrapped?
        if ( *psz != '\0' ) {
            /*putc('\n', f);*/
            for ( n = 0; n < nStart; n++ )
                putc(' ', f);

            // as we wrapped, squeeze all white space
            while ( isspace(*psz) )
                psz++;
        }
    }

    putc('\n', f);
}
#endif  //LOG_PRETTY_WRAP

// ----------------------------------------------------------------------------
// error code/error message retrieval functions
// ----------------------------------------------------------------------------

// get error code from syste
unsigned long wxSysErrorCode()
{
#if defined(__WXMSW__) && !defined(__WXMICROWIN__)
    return ::GetLastError();
#else   //Unix
    return errno;
#endif  //Win/Unix
}

// get error message from system
const wxChar *wxSysErrorMsg(unsigned long nErrCode)
{
    if ( nErrCode == 0 )
        nErrCode = wxSysErrorCode();

#if defined(__WXMSW__) && !defined(__WXMICROWIN__)
    static wxChar s_szBuf[LOG_BUFFER_SIZE / 2];

    // get error message from system
    LPVOID lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, nErrCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf,
            0, NULL);

    // copy it to our buffer and free memory
    // Crashes on SmartPhone
#if !defined(__SMARTPHONE__) /* of WinCE */
     if( lpMsgBuf != 0 ) {
        wxStrncpy(s_szBuf, (const wxChar *)lpMsgBuf, WXSIZEOF(s_szBuf) - 1);
        s_szBuf[WXSIZEOF(s_szBuf) - 1] = wxT('\0');

        LocalFree(lpMsgBuf);

        // returned string is capitalized and ended with '\r\n' - bad
        s_szBuf[0] = (wxChar)wxTolower(s_szBuf[0]);
        size_t len = wxStrlen(s_szBuf);
        if ( len > 0 ) {
            // truncate string
            if ( s_szBuf[len - 2] == wxT('\r') )
                s_szBuf[len - 2] = wxT('\0');
        }
    }
    else
#endif
    {
        s_szBuf[0] = wxT('\0');
    }

    return s_szBuf;
#else   // Unix-WXMICROWIN
#if wxUSE_UNICODE
    static wxChar s_szBuf[LOG_BUFFER_SIZE / 2];
    wxConvCurrent->MB2WC(s_szBuf, strerror(nErrCode), WXSIZEOF(s_szBuf) -1);
    return s_szBuf;
#else
    return strerror((int)nErrCode);
#endif
#endif  // Win/Unix-WXMICROWIN
}

#endif // wxUSE_LOG

