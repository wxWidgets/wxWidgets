/////////////////////////////////////////////////////////////////////////////
// Name:        log.cpp
// Purpose:     Assorted wxLogXXX functions, and wxLog (sink for logs)
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
  #pragma implementation "log.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

// wxWindows
#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/intl.h"
    #include "wx/app.h"

    #if wxUSE_GUI
        #include "wx/window.h"
        #ifdef __WXMSW__
            #include "wx/msw/private.h"
        #endif
        #include "wx/msgdlg.h"
    #endif
#endif //WX_PRECOMP

#include  "wx/file.h"
#include  "wx/textfile.h"
#include  "wx/utils.h"
#include  "wx/wxchar.h"
#include  "wx/log.h"
#include  "wx/thread.h"

// other standard headers
#include  <errno.h>
#include  <stdlib.h>
#include  <time.h>

#ifdef  __WXMSW__
  #include  "wx/msw/private.h"      // includes windows.h for OutputDebugString
#else   //Unix
  #include  <signal.h>
#endif  //Win/Unix

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
static wxChar s_szBuf[LOG_BUFFER_SIZE];

#if wxUSE_THREADS

// the critical section protecting the static buffer
static wxCriticalSection gs_csLogBuf;

#endif // wxUSE_THREADS

// ----------------------------------------------------------------------------
// implementation of Log functions
//
// NB: unfortunately we need all these distinct functions, we can't make them
//     macros and not all compilers inline vararg functions.
// ----------------------------------------------------------------------------

// generic log function
void wxLogGeneric(wxLogLevel level, const wxChar *szFormat, ...)
{
  if ( wxLog::GetActiveTarget() != NULL ) {
    wxCRIT_SECT_LOCKER(locker, gs_csLogBuf);

    va_list argptr;
    va_start(argptr, szFormat);
    wxVsnprintf(s_szBuf, WXSIZEOF(s_szBuf), szFormat, argptr);
    va_end(argptr);

    wxLog::OnLog(level, s_szBuf, time(NULL));
  }
}

#define IMPLEMENT_LOG_FUNCTION(level)                             \
  void wxLog##level(const wxChar *szFormat, ...)                  \
  {                                                               \
    if ( wxLog::GetActiveTarget() != NULL ) {                     \
      wxCRIT_SECT_LOCKER(locker, gs_csLogBuf);                    \
                                                                  \
      va_list argptr;                                             \
      va_start(argptr, szFormat);                                 \
      wxVsnprintf(s_szBuf, WXSIZEOF(s_szBuf), szFormat, argptr);  \
      va_end(argptr);                                             \
                                                                  \
      wxLog::OnLog(wxLOG_##level, s_szBuf, time(NULL));           \
    }                                                             \
  }

IMPLEMENT_LOG_FUNCTION(FatalError)
IMPLEMENT_LOG_FUNCTION(Error)
IMPLEMENT_LOG_FUNCTION(Warning)
IMPLEMENT_LOG_FUNCTION(Message)
IMPLEMENT_LOG_FUNCTION(Info)
IMPLEMENT_LOG_FUNCTION(Status)

// same as info, but only if 'verbose' mode is on
void wxLogVerbose(const wxChar *szFormat, ...)
{
  wxLog *pLog = wxLog::GetActiveTarget();
  if ( pLog != NULL && pLog->GetVerbose() ) {
    wxCRIT_SECT_LOCKER(locker, gs_csLogBuf);

    va_list argptr;
    va_start(argptr, szFormat);
    wxVsnprintf(s_szBuf, WXSIZEOF(s_szBuf), szFormat, argptr);
    va_end(argptr);

    wxLog::OnLog(wxLOG_Info, s_szBuf, time(NULL));
  }
}

// debug functions
#ifdef __WXDEBUG__
#define IMPLEMENT_LOG_DEBUG_FUNCTION(level)                       \
  void wxLog##level(const wxChar *szFormat, ...)                  \
  {                                                               \
    if ( wxLog::GetActiveTarget() != NULL ) {                     \
      wxCRIT_SECT_LOCKER(locker, gs_csLogBuf);                    \
                                                                  \
      va_list argptr;                                             \
      va_start(argptr, szFormat);                                 \
      wxVsnprintf(s_szBuf, WXSIZEOF(s_szBuf), szFormat, argptr);  \
      va_end(argptr);                                             \
                                                                  \
      wxLog::OnLog(wxLOG_##level, s_szBuf, time(NULL));           \
    }                                                             \
  }

  void wxLogTrace(const wxChar *mask, const wxChar *szFormat, ...)
  {
    wxLog *pLog = wxLog::GetActiveTarget();

    if ( pLog != NULL && wxLog::IsAllowedTraceMask(mask) ) {
      wxCRIT_SECT_LOCKER(locker, gs_csLogBuf);

      wxChar *p = s_szBuf;
      size_t len = WXSIZEOF(s_szBuf);
      strncpy(s_szBuf, _T("("), len);
      len -= 2; // strlen("(")
      p += 2;
      strncat(p, mask, len);
      size_t lenMask = wxStrlen(mask);
      len -= lenMask;
      p += lenMask;

      strncat(p, _T("): "), len);
      len -= 3;
      p += 3;

      va_list argptr;
      va_start(argptr, szFormat);
      wxVsnprintf(p, len, szFormat, argptr);
      va_end(argptr);

      wxLog::OnLog(wxLOG_Trace, s_szBuf, time(NULL));
    }
  }

  void wxLogTrace(wxTraceMask mask, const wxChar *szFormat, ...)
  {
    wxLog *pLog = wxLog::GetActiveTarget();

    // we check that all of mask bits are set in the current mask, so
    // that wxLogTrace(wxTraceRefCount | wxTraceOle) will only do something
    // if both bits are set.
    if ( pLog != NULL && ((pLog->GetTraceMask() & mask) == mask) ) {
      wxCRIT_SECT_LOCKER(locker, gs_csLogBuf);

      va_list argptr;
      va_start(argptr, szFormat);
      wxVsnprintf(s_szBuf, WXSIZEOF(s_szBuf), szFormat, argptr);
      va_end(argptr);

      wxLog::OnLog(wxLOG_Trace, s_szBuf, time(NULL));
    }
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
    wxStrncat(s_szBuf, szErrMsg, WXSIZEOF(s_szBuf) - wxStrlen(s_szBuf));

    wxLog::OnLog(wxLOG_Error, s_szBuf, time(NULL));
}

void WXDLLEXPORT wxLogSysError(const wxChar *szFormat, ...)
{
    wxCRIT_SECT_LOCKER(locker, gs_csLogBuf);

    va_list argptr;
    va_start(argptr, szFormat);
    wxVsnprintf(s_szBuf, WXSIZEOF(s_szBuf), szFormat, argptr);
    va_end(argptr);

    wxLogSysErrorHelper(wxSysErrorCode());
}

void WXDLLEXPORT wxLogSysError(long lErrCode, const wxChar *szFormat, ...)
{
    wxCRIT_SECT_LOCKER(locker, gs_csLogBuf);

    va_list argptr;
    va_start(argptr, szFormat);
    wxVsnprintf(s_szBuf, WXSIZEOF(s_szBuf), szFormat, argptr);
    va_end(argptr);

    wxLogSysErrorHelper(lErrCode);
}

// ----------------------------------------------------------------------------
// wxLog class implementation
// ----------------------------------------------------------------------------

wxLog::wxLog()
{
    m_bHasMessages = FALSE;

    // enable verbose messages by default in the debug builds
#ifdef __WXDEBUG__
    m_bVerbose = TRUE;
#else // release
    m_bVerbose = FALSE;
#endif // debug/release
}

wxLog *wxLog::GetActiveTarget()
{
    if ( ms_bAutoCreate && ms_pLogger == NULL ) {
        // prevent infinite recursion if someone calls wxLogXXX() from
        // wxApp::CreateLogTarget()
        static bool s_bInGetActiveTarget = FALSE;
        if ( !s_bInGetActiveTarget ) {
            s_bInGetActiveTarget = TRUE;

            // ask the application to create a log target for us
            if ( wxTheApp != NULL )
                ms_pLogger = wxTheApp->CreateLogTarget();
            else
                ms_pLogger = new wxLogStderr;

            s_bInGetActiveTarget = FALSE;

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

void wxLog::RemoveTraceMask(const wxString& str)
{
    int index = ms_aTraceMasks.Index(str);
    if ( index != wxNOT_FOUND )
        ms_aTraceMasks.Remove((size_t)index);
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
            abort();
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
                                                    : wxT("Debug: "),
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
    // do nothing
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

    // under Windows, programs usually don't have stderr at all, so show the
    // messages also under debugger - unless it's a console program
#if defined(__WXMSW__) && wxUSE_GUI
    OutputDebugString(str + wxT("\r\n"));
#endif // MSW
#if defined(__WXMAC__) && wxUSE_GUI
    debugstr(str + wxT("\r\n"));
#endif // MSW
}

// ----------------------------------------------------------------------------
// wxLogStream implementation
// ----------------------------------------------------------------------------

#if wxUSE_STD_IOSTREAM
wxLogStream::wxLogStream(ostream *ostr)
{
    if ( ostr == NULL )
        m_ostr = &cerr;
    else
        m_ostr = ostr;
}

void wxLogStream::DoLogString(const wxChar *szString, time_t WXUNUSED(t))
{
    (*m_ostr) << wxConvertWX2MB(szString) << endl;
}
#endif // wxUSE_STD_IOSTREAM

// ============================================================================
// Global functions/variables
// ============================================================================

// ----------------------------------------------------------------------------
// static variables
// ----------------------------------------------------------------------------

wxLog          *wxLog::ms_pLogger      = (wxLog *)NULL;
bool            wxLog::ms_doLog        = TRUE;
bool            wxLog::ms_bAutoCreate  = TRUE;

size_t          wxLog::ms_suspendCount = 0;

#if wxUSE_GUI
    const wxChar *wxLog::ms_timestamp  = wxT("%X");  // time only, no date
#else
    const wxChar *wxLog::ms_timestamp  = NULL;       // save space
#endif

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
#ifdef  __WXMSW__
#ifdef  __WIN32__
    return ::GetLastError();
#else   //WIN16
    // TODO what to do on Windows 3.1?
    return 0;
#endif  //WIN16/32
#else   //Unix
    return errno;
#endif  //Win/Unix
}

// get error message from system
const wxChar *wxSysErrorMsg(unsigned long nErrCode)
{
    if ( nErrCode == 0 )
        nErrCode = wxSysErrorCode();

#ifdef  __WXMSW__
#ifdef  __WIN32__
    static wxChar s_szBuf[LOG_BUFFER_SIZE / 2];

    // get error message from system
    LPVOID lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, nErrCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf,
            0, NULL);

    // copy it to our buffer and free memory
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

    return s_szBuf;
#else   //Win16
    // TODO
    return NULL;
#endif // Win16/32
#else   // Unix
#if wxUSE_UNICODE
    static wxChar s_szBuf[LOG_BUFFER_SIZE / 2];
    wxConvCurrent->MB2WC(s_szBuf, strerror(nErrCode), WXSIZEOF(s_szBuf) -1);
    return s_szBuf;
#else
    return strerror((int)nErrCode);
#endif
#endif  // Win/Unix
}

// ----------------------------------------------------------------------------
// debug helper
// ----------------------------------------------------------------------------

#ifdef  __WXDEBUG__

// break into the debugger
void Trap()
{
#ifdef __WXMSW__
    DebugBreak();
#elif defined(__WXMAC__)
#if __powerc
    Debugger();
#else
    SysBreak();
#endif
#elif defined(__UNIX__)
    raise(SIGTRAP);
#else
    // TODO
#endif // Win/Unix
}

// this function is called when an assert fails
void wxOnAssert(const wxChar *szFile, int nLine, const wxChar *szMsg)
{
    // this variable can be set to true to suppress "assert failure" messages
    static bool s_bNoAsserts = FALSE;
    static bool s_bInAssert = FALSE;        // FIXME MT-unsafe

    if ( s_bInAssert ) {
        // He-e-e-e-elp!! we're trapped in endless loop
        Trap();

        s_bInAssert = FALSE;

        return;
    }

    s_bInAssert = TRUE;

    wxChar szBuf[LOG_BUFFER_SIZE];

    // make life easier for people using VC++ IDE: clicking on the message
    // will take us immediately to the place of the failed assert
    wxSnprintf(szBuf, WXSIZEOF(szBuf),
#ifdef __VISUALC__
               wxT("%s(%d): assert failed"),
#else  // !VC++
    // make the error message more clear for all the others
               wxT("Assert failed in file %s at line %d"),
#endif // VC/!VC
               szFile, nLine);

    if ( szMsg != NULL ) {
        wxStrcat(szBuf, wxT(": "));
        wxStrcat(szBuf, szMsg);
    }
    else {
        wxStrcat(szBuf, wxT("."));
    }

    if ( !s_bNoAsserts ) {
        // send it to the normal log destination
        wxLogDebug(szBuf);

#if wxUSE_GUI || defined(__WXMSW__)
        // this message is intentionally not translated - it is for
        // developpers only
        wxStrcat(szBuf, wxT("\nDo you want to stop the program?"
                            "\nYou can also choose [Cancel] to suppress "
                            "further warnings."));

#if wxUSE_GUI
        switch ( wxMessageBox(szBuf, "Debug",
                              wxYES_NO | wxCANCEL | wxICON_STOP ) ) {
            case wxYES:
                Trap();
                break;

            case wxCANCEL:
                s_bNoAsserts = TRUE;
                break;

            //case wxNO: nothing to do
        }
#else // !GUI, but MSW
        switch ( ::MessageBox(NULL, szBuf, "Debug",
                              MB_YESNOCANCEL | MB_ICONSTOP ) ) {
            case IDYES:
                Trap();
                break;

            case IDCANCEL:
                s_bNoAsserts = TRUE;
                break;

            //case IDNO: nothing to do
        }
#endif // GUI or MSW

#else // !GUI
        Trap();
#endif // GUI/!GUI
    }

    s_bInAssert = FALSE;
}

#endif  //WXDEBUG

