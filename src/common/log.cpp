/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/log.cpp
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

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_LOG

// wxWidgets
#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/app.h"
    #include "wx/arrstr.h"
    #include "wx/intl.h"
    #include "wx/string.h"
    #include "wx/utils.h"
#endif //WX_PRECOMP

#include "wx/apptrait.h"
#include "wx/datetime.h"
#include "wx/file.h"
#include "wx/msgout.h"
#include "wx/textfile.h"
#include "wx/thread.h"
#include "wx/crt.h"

// other standard headers
#ifndef __WXWINCE__
#include <errno.h>
#endif

#include <stdlib.h>

#ifndef __WXPALMOS5__
#ifndef __WXWINCE__
#include <time.h>
#else
#include "wx/msw/wince/time.h"
#endif
#endif /* ! __WXPALMOS5__ */

#if defined(__WINDOWS__)
    #include "wx/msw/private.h" // includes windows.h
#endif

#if wxUSE_THREADS

// define static functions providing access to the critical sections we use
// instead of just using static critical section variables as log functions may
// be used during static initialization and while this is certainly not
// advisable it's still better to not crash (as we'd do if we used a yet
// uninitialized critical section) if it happens

static inline wxCriticalSection& GetTraceMaskCS()
{
    static wxCriticalSection s_csTrace;

    return s_csTrace;
}

static inline wxCriticalSection& GetPreviousLogCS()
{
    static wxCriticalSection s_csPrev;

    return s_csPrev;
}

#endif // wxUSE_THREADS

// ----------------------------------------------------------------------------
// non member functions
// ----------------------------------------------------------------------------

// define this to enable wrapping of log messages
//#define LOG_PRETTY_WRAP

#ifdef  LOG_PRETTY_WRAP
  static void wxLogWrap(FILE *f, const char *pszPrefix, const char *psz);
#endif

// ----------------------------------------------------------------------------
// module globals
// ----------------------------------------------------------------------------

namespace
{

// this struct is used to store information about the previous log message used
// by OnLog() to (optionally) avoid logging multiple copies of the same message
struct PreviousLogInfo
{
    PreviousLogInfo()
    {
        numRepeated = 0;
    }


    // previous message itself
    wxString msg;

    // its level
    wxLogLevel level;

    // other information about it
    wxLogRecordInfo info;

    // the number of times it was already repeated
    unsigned numRepeated;
};

PreviousLogInfo gs_prevLog;

} // anonymous namespace

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// implementation of Log functions
//
// NB: unfortunately we need all these distinct functions, we can't make them
//     macros and not all compilers inline vararg functions.
// ----------------------------------------------------------------------------

// generic log function
void wxVLogGeneric(wxLogLevel level, const wxString& format, va_list argptr)
{
    if ( wxLog::IsEnabled() )
    {
        wxLog::OnLog(level, wxString::FormatV(format, argptr));
    }
}

#if !wxUSE_UTF8_LOCALE_ONLY
void wxDoLogGenericWchar(wxLogLevel level, const wxChar *format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    wxVLogGeneric(level, format, argptr);
    va_end(argptr);
}
#endif // wxUSE_UTF8_LOCALE_ONLY

#if wxUSE_UNICODE_UTF8
void wxDoLogGenericUtf8(wxLogLevel level, const char *format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    wxVLogGeneric(level, format, argptr);
    va_end(argptr);
}
#endif // wxUSE_UNICODE_UTF8

#if !wxUSE_UTF8_LOCALE_ONLY
    #define IMPLEMENT_LOG_FUNCTION_WCHAR(level)                         \
      void wxDoLog##level##Wchar(const wxChar *format, ...)             \
      {                                                                 \
        va_list argptr;                                                 \
        va_start(argptr, format);                                       \
        wxVLog##level(format, argptr);                                  \
        va_end(argptr);                                                 \
      }
#else
    #define IMPLEMENT_LOG_FUNCTION_WCHAR(level)
#endif

#if wxUSE_UNICODE_UTF8
    #define IMPLEMENT_LOG_FUNCTION_UTF8(level)                          \
      void wxDoLog##level##Utf8(const char *format, ...)                \
      {                                                                 \
        va_list argptr;                                                 \
        va_start(argptr, format);                                       \
        wxVLog##level(format, argptr);                                  \
        va_end(argptr);                                                 \
      }
#else
    #define IMPLEMENT_LOG_FUNCTION_UTF8(level)
#endif

#define IMPLEMENT_LOG_FUNCTION(level)                               \
  void wxVLog##level(const wxString& format, va_list argptr)        \
  {                                                                 \
    if ( wxLog::IsEnabled() )                                       \
      wxLog::OnLog(wxLOG_##level, wxString::FormatV(format, argptr));  \
  }                                                                 \
  IMPLEMENT_LOG_FUNCTION_WCHAR(level)                               \
  IMPLEMENT_LOG_FUNCTION_UTF8(level)

IMPLEMENT_LOG_FUNCTION(Error)
IMPLEMENT_LOG_FUNCTION(Warning)
IMPLEMENT_LOG_FUNCTION(Message)
IMPLEMENT_LOG_FUNCTION(Info)
IMPLEMENT_LOG_FUNCTION(Status)

void wxSafeShowMessage(const wxString& title, const wxString& text)
{
#ifdef __WINDOWS__
    ::MessageBox(NULL, text.t_str(), title.t_str(), MB_OK | MB_ICONSTOP);
#else
    wxFprintf(stderr, wxS("%s: %s\n"), title.c_str(), text.c_str());
    fflush(stderr);
#endif
}

// fatal errors can't be suppressed nor handled by the custom log target and
// always terminate the program
void wxVLogFatalError(const wxString& format, va_list argptr)
{
    wxSafeShowMessage(wxS("Fatal Error"), wxString::FormatV(format, argptr));

#ifdef __WXWINCE__
    ExitThread(3);
#else
    abort();
#endif
}

#if !wxUSE_UTF8_LOCALE_ONLY
void wxDoLogFatalErrorWchar(const wxChar *format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    wxVLogFatalError(format, argptr);

    // some compilers warn about unreachable code and it shouldn't matter
    // for the others anyhow...
    //va_end(argptr);
}
#endif // wxUSE_UTF8_LOCALE_ONLY

#if wxUSE_UNICODE_UTF8
void wxDoLogFatalErrorUtf8(const char *format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    wxVLogFatalError(format, argptr);

    // some compilers warn about unreachable code and it shouldn't matter
    // for the others anyhow...
    //va_end(argptr);
}
#endif // wxUSE_UNICODE_UTF8

// same as info, but only if 'verbose' mode is on
void wxVLogVerbose(const wxString& format, va_list argptr)
{
    if ( wxLog::IsEnabled() ) {
        if ( wxLog::GetActiveTarget() != NULL && wxLog::GetVerbose() )
            wxLog::OnLog(wxLOG_Info, wxString::FormatV(format, argptr));
    }
}

#if !wxUSE_UTF8_LOCALE_ONLY
void wxDoLogVerboseWchar(const wxChar *format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    wxVLogVerbose(format, argptr);
    va_end(argptr);
}
#endif // !wxUSE_UTF8_LOCALE_ONLY

#if wxUSE_UNICODE_UTF8
void wxDoLogVerboseUtf8(const char *format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    wxVLogVerbose(format, argptr);
    va_end(argptr);
}
#endif // wxUSE_UNICODE_UTF8

// ----------------------------------------------------------------------------
// debug and trace functions
// ----------------------------------------------------------------------------

#if wxUSE_LOG_DEBUG
    void wxVLogDebug(const wxString& format, va_list argptr)
    {
        if ( wxLog::IsEnabled() )
        {
            wxLog::OnLog(wxLOG_Debug, wxString::FormatV(format, argptr));
        }
    }

#if !wxUSE_UTF8_LOCALE_ONLY
    void wxDoLogDebugWchar(const wxChar *format, ...)
    {
        va_list argptr;
        va_start(argptr, format);
        wxVLogDebug(format, argptr);
        va_end(argptr);
    }
#endif // !wxUSE_UTF8_LOCALE_ONLY

#if wxUSE_UNICODE_UTF8
    void wxDoLogDebugUtf8(const char *format, ...)
    {
        va_list argptr;
        va_start(argptr, format);
        wxVLogDebug(format, argptr);
        va_end(argptr);
    }
#endif // wxUSE_UNICODE_UTF8
#endif // wxUSE_LOG_DEBUG

#if wxUSE_LOG_TRACE
  void wxVLogTrace(const wxString& mask, const wxString& format, va_list argptr)
  {
    if ( wxLog::IsEnabled() && wxLog::IsAllowedTraceMask(mask) ) {
      wxString msg;
      msg << wxS("(") << mask << wxS(") ") << wxString::FormatV(format, argptr);

      wxLog::OnLog(wxLOG_Trace, msg);
    }
  }

#if !wxUSE_UTF8_LOCALE_ONLY
  void wxDoLogTraceWchar(const wxString& mask, const wxChar *format, ...)
  {
    va_list argptr;
    va_start(argptr, format);
    wxVLogTrace(mask, format, argptr);
    va_end(argptr);
  }
#endif // !wxUSE_UTF8_LOCALE_ONLY

#if wxUSE_UNICODE_UTF8
  void wxDoLogTraceUtf8(const wxString& mask, const char *format, ...)
  {
    va_list argptr;
    va_start(argptr, format);
    wxVLogTrace(mask, format, argptr);
    va_end(argptr);
  }
#endif // wxUSE_UNICODE_UTF8

// deprecated (but not declared as such because we don't want to complicate
// DECLARE_LOG_FUNCTION macros even more) overloads for wxTraceMask
#if WXWIN_COMPATIBILITY_2_8
  void wxVLogTrace(wxTraceMask mask, const wxString& format, va_list argptr)
  {
    // we check that all of mask bits are set in the current mask, so
    // that wxLogTrace(wxTraceRefCount | wxTraceOle) will only do something
    // if both bits are set.
    if ( wxLog::IsEnabled() && ((wxLog::GetTraceMask() & mask) == mask) ) {
      wxLog::OnLog(wxLOG_Trace, wxString::FormatV(format, argptr));
    }
  }

#if !wxUSE_UTF8_LOCALE_ONLY
  void wxDoLogTraceWchar(wxTraceMask mask, const wxChar *format, ...)
  {
    va_list argptr;
    va_start(argptr, format);
    wxVLogTrace(mask, format, argptr);
    va_end(argptr);
  }
#endif // !wxUSE_UTF8_LOCALE_ONLY

#if wxUSE_UNICODE_UTF8
  void wxDoLogTraceUtf8(wxTraceMask mask, const char *format, ...)
  {
    va_list argptr;
    va_start(argptr, format);
    wxVLogTrace(mask, format, argptr);
    va_end(argptr);
  }
#endif // wxUSE_UNICODE_UTF8

#endif // WXWIN_COMPATIBILITY_2_8

#ifdef __WATCOMC__
#if WXWIN_COMPATIBILITY_2_8
  // workaround for http://bugzilla.openwatcom.org/show_bug.cgi?id=351
  void wxDoLogTraceWchar(int mask, const wxChar *format, ...)
  {
    va_list argptr;
    va_start(argptr, format);
    wxVLogTrace(mask, format, argptr);
    va_end(argptr);
  }
#endif // WXWIN_COMPATIBILITY_2_8

  void wxDoLogTraceWchar(const char *mask, const wxChar *format, ...)
  {
    va_list argptr;
    va_start(argptr, format);
    wxVLogTrace(mask, format, argptr);
    va_end(argptr);
  }

  void wxDoLogTraceWchar(const wchar_t *mask, const wxChar *format, ...)
  {
    va_list argptr;
    va_start(argptr, format);
    wxVLogTrace(mask, format, argptr);
    va_end(argptr);
  }

#if WXWIN_COMPATIBILITY_2_8
  void wxVLogTrace(int mask, const wxString& format, va_list argptr)
    { wxVLogTrace((wxTraceMask)mask, format, argptr); }
#endif // WXWIN_COMPATIBILITY_2_8
  void wxVLogTrace(const char *mask, const wxString& format, va_list argptr)
    { wxVLogTrace(wxString(mask), format, argptr); }
  void wxVLogTrace(const wchar_t *mask, const wxString& format, va_list argptr)
    { wxVLogTrace(wxString(mask), format, argptr); }
#endif // __WATCOMC__
#endif // wxUSE_LOG_TRACE


// wxLogSysError: one uses the last error code, for other  you must give it
// explicitly

// return the system error message description
static inline wxString wxLogSysErrorHelper(long err)
{
    return wxString::Format(_(" (error %ld: %s)"), err, wxSysErrorMsg(err));
}

void WXDLLIMPEXP_BASE wxVLogSysError(const wxString& format, va_list argptr)
{
    wxVLogSysError(wxSysErrorCode(), format, argptr);
}

#if !wxUSE_UTF8_LOCALE_ONLY
void WXDLLIMPEXP_BASE wxDoLogSysErrorWchar(const wxChar *format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    wxVLogSysError(format, argptr);
    va_end(argptr);
}
#endif // !wxUSE_UTF8_LOCALE_ONLY

#if wxUSE_UNICODE_UTF8
void WXDLLIMPEXP_BASE wxDoLogSysErrorUtf8(const char *format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    wxVLogSysError(format, argptr);
    va_end(argptr);
}
#endif // wxUSE_UNICODE_UTF8

void WXDLLIMPEXP_BASE wxVLogSysError(long err, const wxString& format, va_list argptr)
{
    if ( wxLog::IsEnabled() )
    {
        wxLog::OnLog(wxLOG_Error,
                     wxString::FormatV(format, argptr) + wxLogSysErrorHelper(err));
    }
}

#if !wxUSE_UTF8_LOCALE_ONLY
void WXDLLIMPEXP_BASE wxDoLogSysErrorWchar(long lErrCode, const wxChar *format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    wxVLogSysError(lErrCode, format, argptr);
    va_end(argptr);
}
#endif // !wxUSE_UTF8_LOCALE_ONLY

#if wxUSE_UNICODE_UTF8
void WXDLLIMPEXP_BASE wxDoLogSysErrorUtf8(long lErrCode, const char *format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    wxVLogSysError(lErrCode, format, argptr);
    va_end(argptr);
}
#endif // wxUSE_UNICODE_UTF8

#ifdef __WATCOMC__
// workaround for http://bugzilla.openwatcom.org/show_bug.cgi?id=351
void WXDLLIMPEXP_BASE wxDoLogSysErrorWchar(unsigned long lErrCode, const wxChar *format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    wxVLogSysError(lErrCode, format, argptr);
    va_end(argptr);
}

void WXDLLIMPEXP_BASE wxVLogSysError(unsigned long err, const wxString& format, va_list argptr)
    { wxVLogSysError((long)err, format, argptr); }
#endif // __WATCOMC__

// ----------------------------------------------------------------------------
// wxLog class implementation
// ----------------------------------------------------------------------------

unsigned wxLog::LogLastRepeatIfNeeded()
{
    wxCRIT_SECT_LOCKER(lock, GetPreviousLogCS());

    return LogLastRepeatIfNeededUnlocked();
}

unsigned wxLog::LogLastRepeatIfNeededUnlocked()
{
    const unsigned count = gs_prevLog.numRepeated;

    if ( gs_prevLog.numRepeated )
    {
        wxString msg;
#if wxUSE_INTL
        msg.Printf(wxPLURAL("The previous message repeated once.",
                            "The previous message repeated %lu times.",
                            gs_prevLog.numRepeated),
                   gs_prevLog.numRepeated);
#else
        msg.Printf(wxS("The previous message was repeated %lu times."),
                   gs_prevLog.numRepeated);
#endif
        gs_prevLog.numRepeated = 0;
        gs_prevLog.msg.clear();
        DoLogRecord(gs_prevLog.level, msg, gs_prevLog.info);
    }

    return count;
}

wxLog::~wxLog()
{
    // Flush() must be called before destroying the object as otherwise some
    // messages could be lost
    if ( gs_prevLog.numRepeated )
    {
        wxMessageOutputDebug().Printf
        (
            wxS("Last repeated message (\"%s\", %lu times) wasn't output"),
            gs_prevLog.msg,
            gs_prevLog.numRepeated
        );
    }
}

// ----------------------------------------------------------------------------
// wxLog logging functions
// ----------------------------------------------------------------------------

/* static */
void
wxLog::OnLog(wxLogLevel level, const wxString& msg, time_t t)
{
    wxLogRecordInfo info;
    info.timestamp = t;
#if wxUSE_THREADS
    info.threadId = wxThread::GetCurrentId();
#endif // wxUSE_THREADS

    OnLog(level, msg, info);
}

/* static */
void
wxLog::OnLog(wxLogLevel level,
             const wxString& msg,
             const wxLogRecordInfo& info)
{
    if ( IsEnabled() && ms_logLevel >= level )
    {
        wxLog *pLogger = GetActiveTarget();
        if ( pLogger )
        {
            if ( GetRepetitionCounting() )
            {
                wxCRIT_SECT_LOCKER(lock, GetPreviousLogCS());

                if ( msg == gs_prevLog.msg )
                {
                    gs_prevLog.numRepeated++;

                    // nothing else to do, in particular, don't log the
                    // repeated message
                    return;
                }

                pLogger->LogLastRepeatIfNeededUnlocked();

                // reset repetition counter for a new message
                gs_prevLog.msg = msg;
                gs_prevLog.level = level;
                gs_prevLog.info = info;
            }

            pLogger->DoLogRecord(level, msg, info);
        }
    }
}

void wxLog::DoLogRecord(wxLogLevel level,
                             const wxString& msg,
                             const wxLogRecordInfo& info)
{
#if WXWIN_COMPATIBILITY_2_8
    // call the old DoLog() to ensure that existing custom log classes still
    // work
    //
    // as the user code could have defined it as either taking "const char *"
    // (in ANSI build) or "const wxChar *" (in ANSI/Unicode), we have no choice
    // but to call both of them
    DoLog(level, (const char*)msg.mb_str(), info.timestamp);
    DoLog(level, (const wchar_t*)msg.wc_str(), info.timestamp);
#endif // WXWIN_COMPATIBILITY_2_8


    // TODO: it would be better to extract message formatting in a separate
    //       wxLogFormatter class but for now we hard code formatting here

    wxString prefix;

    // don't time stamp debug messages under MSW as debug viewers usually
    // already have an option to do it
#ifdef __WXMSW__
    if ( level != wxLOG_Debug && level != wxLOG_Trace )
#endif // __WXMSW__
        TimeStamp(&prefix);

    // TODO: use the other wxLogRecordInfo fields

    switch ( level )
    {
        case wxLOG_Error:
            prefix += _("Error: ");
            break;

        case wxLOG_Warning:
            prefix += _("Warning: ");
            break;

        // don't prepend "debug/trace" prefix under MSW as it goes to the debug
        // window anyhow and so can't be confused with something else
#ifndef __WXMSW__
        case wxLOG_Debug:
            // this prefix (as well as the one below) is intentionally not
            // translated as nobody translates debug messages anyhow
            prefix += "Debug: ";
            break;

        case wxLOG_Trace:
            prefix += "Trace: ";
            break;
#endif // !__WXMSW__
    }

    DoLogTextAtLevel(level, prefix + msg);
}

void wxLog::DoLogTextAtLevel(wxLogLevel level, const wxString& msg)
{
    // we know about debug messages (because using wxMessageOutputDebug is the
    // right thing to do in 99% of all cases and also for compatibility) but
    // anything else needs to be handled in the derived class
    if ( level == wxLOG_Debug || level == wxLOG_Trace )
    {
        wxMessageOutputDebug().Output(msg + wxS('\n'));
    }
    else
    {
        DoLogText(msg);
    }
}

void wxLog::DoLogText(const wxString& WXUNUSED(msg))
{
    // in 2.8-compatible build the derived class might override DoLog() or
    // DoLogString() instead so we can't have this assert there
#if !WXWIN_COMPATIBILITY_2_8
    wxFAIL_MSG( "must be overridden if it is called" );
#endif // WXWIN_COMPATIBILITY_2_8
}

#if WXWIN_COMPATIBILITY_2_8

void wxLog::DoLog(wxLogLevel WXUNUSED(level), const char *szString, time_t t)
{
    DoLogString(szString, t);
}

void wxLog::DoLog(wxLogLevel WXUNUSED(level), const wchar_t *wzString, time_t t)
{
    DoLogString(wzString, t);
}

#endif // WXWIN_COMPATIBILITY_2_8

// ----------------------------------------------------------------------------
// wxLog active target management
// ----------------------------------------------------------------------------

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

void wxLog::DoCreateOnDemand()
{
    ms_bAutoCreate = true;
}

void wxLog::AddTraceMask(const wxString& str)
{
    wxCRIT_SECT_LOCKER(lock, GetTraceMaskCS());

    ms_aTraceMasks.push_back(str);
}

void wxLog::RemoveTraceMask(const wxString& str)
{
    wxCRIT_SECT_LOCKER(lock, GetTraceMaskCS());

    int index = ms_aTraceMasks.Index(str);
    if ( index != wxNOT_FOUND )
        ms_aTraceMasks.RemoveAt((size_t)index);
}

void wxLog::ClearTraceMasks()
{
    wxCRIT_SECT_LOCKER(lock, GetTraceMaskCS());

    ms_aTraceMasks.Clear();
}

void wxLog::TimeStamp(wxString *str)
{
#if wxUSE_DATETIME
    if ( !ms_timestamp.empty() )
    {
        wxChar buf[256];
        time_t timeNow;
        (void)time(&timeNow);

        struct tm tm;
        wxStrftime(buf, WXSIZEOF(buf),
                    ms_timestamp, wxLocaltime_r(&timeNow, &tm));

        str->Empty();
        *str << buf << wxS(": ");
    }
#endif // wxUSE_DATETIME
}

void wxLog::Flush()
{
    LogLastRepeatIfNeeded();
}

/*static*/ bool wxLog::IsAllowedTraceMask(const wxString& mask)
{
    wxCRIT_SECT_LOCKER(lock, GetTraceMaskCS());

    for ( wxArrayString::iterator it = ms_aTraceMasks.begin(),
                                  en = ms_aTraceMasks.end();
         it != en; ++it )
    {
        if ( *it == mask)
            return true;
    }

    return false;
}

// ----------------------------------------------------------------------------
// wxLogBuffer implementation
// ----------------------------------------------------------------------------

void wxLogBuffer::Flush()
{
    if ( !m_str.empty() )
    {
        wxMessageOutputBest out;
        out.Printf(wxS("%s"), m_str.c_str());
        m_str.clear();
    }
}

void wxLogBuffer::DoLogTextAtLevel(wxLogLevel level, const wxString& msg)
{
    // don't put debug messages in the buffer, we don't want to show
    // them to the user in a msg box, log them immediately
    switch ( level )
    {
        case wxLOG_Debug:
        case wxLOG_Trace:
            wxLog::DoLogTextAtLevel(level, msg);
            break;

        default:
            m_str << msg << wxS("\n");
    }
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

void wxLogStderr::DoLogText(const wxString& msg)
{
    wxFputs(msg + '\n', m_fp);
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
            wxMessageOutputDebug().Output(msg + wxS('\n'));
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

void wxLogStream::DoLogText(const wxString& msg)
{
    (*m_ostr) << msg << wxSTD endl;
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

void wxLogChain::DoLogRecord(wxLogLevel level,
                             const wxString& msg,
                             const wxLogRecordInfo& info)
{
    // let the previous logger show it
    if ( m_logOld && IsPassingMessages() )
        m_logOld->LogRecord(level, msg, info);

    // and also send it to the new one
    if ( m_logNew && m_logNew != this )
        m_logNew->LogRecord(level, msg, info);
}

#ifdef __VISUALC__
    // "'this' : used in base member initializer list" - so what?
    #pragma warning(disable:4355)
#endif // VC++

// ----------------------------------------------------------------------------
// wxLogInterposer
// ----------------------------------------------------------------------------

wxLogInterposer::wxLogInterposer()
                : wxLogChain(this)
{
}

// ----------------------------------------------------------------------------
// wxLogInterposerTemp
// ----------------------------------------------------------------------------

wxLogInterposerTemp::wxLogInterposerTemp()
                : wxLogChain(this)
{
    DetachOldLog();
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

bool            wxLog::ms_bRepetCounting = false;

wxLog          *wxLog::ms_pLogger      = NULL;
bool            wxLog::ms_doLog        = true;
bool            wxLog::ms_bAutoCreate  = true;
bool            wxLog::ms_bVerbose     = false;

wxLogLevel      wxLog::ms_logLevel     = wxLOG_Max;  // log everything by default

size_t          wxLog::ms_suspendCount = 0;

wxString        wxLog::ms_timestamp(wxS("%X"));  // time only, no date

#if WXWIN_COMPATIBILITY_2_8
wxTraceMask     wxLog::ms_ulTraceMask  = (wxTraceMask)0;
#endif // wxDEBUG_LEVEL

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
    static wxChar s_szBuf[1024];

    // get error message from system
    LPVOID lpMsgBuf;
    if ( ::FormatMessage
         (
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            nErrCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf,
            0,
            NULL
         ) == 0 )
    {
        // if this happens, something is seriously wrong, so don't use _() here
        // for safety
        wxSprintf(s_szBuf, wxS("unknown error %lx"), nErrCode);
        return s_szBuf;
    }


    // copy it to our buffer and free memory
    // Crashes on SmartPhone (FIXME)
#if !defined(__SMARTPHONE__) /* of WinCE */
    if( lpMsgBuf != 0 )
    {
        wxStrlcpy(s_szBuf, (const wxChar *)lpMsgBuf, WXSIZEOF(s_szBuf));

        LocalFree(lpMsgBuf);

        // returned string is capitalized and ended with '\r\n' - bad
        s_szBuf[0] = (wxChar)wxTolower(s_szBuf[0]);
        size_t len = wxStrlen(s_szBuf);
        if ( len > 0 ) {
            // truncate string
            if ( s_szBuf[len - 2] == wxS('\r') )
                s_szBuf[len - 2] = wxS('\0');
        }
    }
    else
#endif // !__SMARTPHONE__
    {
        s_szBuf[0] = wxS('\0');
    }

    return s_szBuf;
#else // !__WXMSW__
    #if wxUSE_UNICODE
        static wchar_t s_wzBuf[1024];
        wxConvCurrent->MB2WC(s_wzBuf, strerror((int)nErrCode),
                             WXSIZEOF(s_wzBuf) - 1);
        return s_wzBuf;
    #else
        return strerror((int)nErrCode);
    #endif
#endif  // __WXMSW__/!__WXMSW__
}

#endif // wxUSE_LOG
