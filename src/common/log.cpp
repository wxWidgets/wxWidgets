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
  #include  <wx/string.h>
  #include  <wx/app.h>
  #include  <wx/generic/msgdlgg.h>
#endif

#include  <wx/intl.h>
#include  <wx/log.h>

// other standard headers
#include  <errno.h>
#include  <stdlib.h>
#include  <time.h>

// C++ headers
#include  <iostream.h>

// _WINDOWS_ is defined when windows.h is included,
// __WINDOWS__ is defined for MS Windows compilation
#if       defined(__WINDOWS__) && !defined(_WINDOWS_)
#include  <windows.h>
#endif  //windows.h

// ----------------------------------------------------------------------------
// non member functions
// ----------------------------------------------------------------------------

// define this to enable wrapping of log messages
//#define LOG_PRETTY_WRAP

#ifdef LOG_PRETTY_WRAP
  static void wxLogWrap(FILE *f, const char *pszPrefix, const char *psz);
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// implementation of Log functions
//
// NB: unfortunately we need all these distinct functions, we can't make them
//     macros and not all compilers inline vararg functions.
// ----------------------------------------------------------------------------

// log functions can't allocate memory (LogError("out of memory...") should
// work!), so we use a static buffer for all log messages
#define LOG_BUFFER_SIZE   (4096)

// static buffer for error messages (@@@ MT-unsafe)
static char s_szBuf[LOG_BUFFER_SIZE];

// generic log function
void wxLogGeneric(wxLog::Level level, wxTString strFormat, ...)
{
  if ( wxLog::GetActiveTarget() != NULL ) {               
    va_list argptr;                                             
    va_start(argptr, strFormat);                                
    vsprintf(s_szBuf, strFormat, argptr);                       
    va_end(argptr);                                             
                                                                
    wxLog::OnLog(level, s_szBuf);                         
  }                                                             
}

#define IMPLEMENT_LOG_FUNCTION(level)                             \
  void wxLog##level(wxTString strFormat, ...)                     \
  {                                                               \
    if ( wxLog::GetActiveTarget() != NULL ) {                     \
      va_list argptr;                                             \
      va_start(argptr, strFormat);                                \
      vsprintf(s_szBuf, strFormat, argptr);                       \
      va_end(argptr);                                             \
                                                                  \
      wxLog::OnLog(wxLog::level, s_szBuf);                        \
    }                                                             \
  }

IMPLEMENT_LOG_FUNCTION(FatalError)
IMPLEMENT_LOG_FUNCTION(Error)
IMPLEMENT_LOG_FUNCTION(Warning)
IMPLEMENT_LOG_FUNCTION(Message)
IMPLEMENT_LOG_FUNCTION(Info)
IMPLEMENT_LOG_FUNCTION(Status)

// debug functions don't use wxTString
#undef  IMPLEMENT_LOG_FUNCTION
#define IMPLEMENT_LOG_FUNCTION(level)                             \
  void wxLog##level(const char *szFormat, ...)                    \
  {                                                               \
    if ( wxLog::GetActiveTarget() != NULL ) {                     \
      va_list argptr;                                             \
      va_start(argptr, szFormat);                                 \
      vsprintf(s_szBuf, szFormat, argptr);                        \
      va_end(argptr);                                             \
                                                                  \
      wxLog::OnLog(wxLog::level, s_szBuf);                        \
    }                                                             \
  }

IMPLEMENT_LOG_FUNCTION(Debug)
IMPLEMENT_LOG_FUNCTION(Trace)

void wxLogVerbose(wxTString strFormat, ...)
{                                            
  if ( wxLog::GetVerbose() && wxLog::GetActiveTarget() != NULL ) {
    va_list argptr;                              
    va_start(argptr, strFormat);                 
    vsprintf(s_szBuf, strFormat, argptr);        
    va_end(argptr);                              
                                                 
    wxLog::OnLog(wxLog::Info, s_szBuf);
  }                                              
}
  
void wxLogSysError(wxTString str, ...)
{
  if ( wxLog::GetActiveTarget() != NULL ) {
    va_list argptr;
    va_start(argptr, str);
    vsprintf(s_szBuf, str, argptr);
    va_end(argptr);

    char szErrMsg[LOG_BUFFER_SIZE / 2];
    sprintf(szErrMsg, _(" (error %ld: %s)"), wxSysErrorCode(), wxSysErrorMsg());
    strncat(s_szBuf, szErrMsg, WXSIZEOF(s_szBuf) - strlen(s_szBuf));

    wxLog::OnLog(wxLog::Error, s_szBuf);
  }
}

void WXDLLEXPORT wxLogSysError(long lErrCode, wxTString strFormat, ...)
{
  if ( wxLog::GetActiveTarget() != NULL ) {
    va_list argptr;
    va_start(argptr, strFormat);
    vsprintf(s_szBuf, strFormat, argptr);
    va_end(argptr);

    char szErrMsg[LOG_BUFFER_SIZE / 2];
    sprintf(szErrMsg, _(" (error %ld: %s)"), lErrCode, wxSysErrorMsg(lErrCode));
    strncat(s_szBuf, szErrMsg, WXSIZEOF(s_szBuf) - strlen(s_szBuf));

    wxLog::OnLog(wxLog::Error, s_szBuf);
  }
}

// ----------------------------------------------------------------------------
// wxLog class implementation
// ----------------------------------------------------------------------------

wxLog::wxLog()
{
  m_bHasMessages = FALSE;
}

wxLog *wxLog::GetActiveTarget() 
{ 
  if ( !ms_bInitialized ) {
    // prevent infinite recursion if someone calls wxLogXXX() from wxApp
    ms_bInitialized = TRUE;

    // ask the application to create a log target for us if it exists
    if ( wxTheApp != NULL )
      ms_pLogger = wxTheApp->CreateLogTarget();
    else
      ms_pLogger = new wxLogStderr; 

    // do nothing if it fails - what can we do?
  }

  return ms_pLogger; 
}

wxLog *wxLog::SetActiveTarget(wxLog *pLogger)
{ 
  // flush the old messages before changing
  if ( ms_pLogger != NULL )
    ms_pLogger->Flush();

  ms_bInitialized = TRUE;

  wxLog *pOldLogger = ms_pLogger; 
  ms_pLogger = pLogger; 
  return pOldLogger; 
}

void wxLog::DoLog(Level level, const char *szString)
{
  char szBuf[128];
  time_t timeNow;
  struct tm *ptmNow;

  time(&timeNow);
  ptmNow = localtime(&timeNow);

  strftime(szBuf, WXSIZEOF(szBuf), ms_szTimeFormat, ptmNow);
  wxString str = szBuf;

  switch ( level ) {
    case FatalError:
      DoLogString(str << _("Fatal error: ") << szString);
      DoLogString(_("Program aborted."));
      Flush();
      abort();
      break;

    case Error:
      DoLogString(str << _("Error: ") << szString);
      break;

    case Warning:
      DoLogString(str << _("Warning: ") << szString);
      break;

    case Info:
      if ( GetVerbose() )
    case Message:
        DoLogString(str + szString);
      // fall through

    case Status:
      // nothing to do
      break;

    case Trace:
    case Debug:
      #ifdef __DEBUG__
        #ifdef  __WIN32__
          // in addition to normal logging, also send the string to debugger
          // (don't prepend "Debug" here: it will go to debug window anyhow)
          ::OutputDebugString(str + szString + "\n\r");
        #endif  //Win32
        DoLogString(str << (level == Trace ? _("Trace") : _("Debug"))
                        << ": " << szString);
      #endif
            
      break;

    default:
      wxFAIL_MSG("unknown log level in wxLog::DoLog");
  }
}

void wxLog::DoLogString(const char *WXUNUSED(szString))
{
  wxFAIL_MSG("DoLogString must be overrided if it's called.");
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

void wxLogStderr::DoLogString(const char *szString)
{
  fputs(szString, m_fp);
  fputc('\n', m_fp);
  fflush(m_fp);
}

// ----------------------------------------------------------------------------
// wxLogStream implementation
// ----------------------------------------------------------------------------

wxLogStream::wxLogStream(ostream *ostr)
{
  if ( ostr == NULL )
    m_ostr = &cerr;
  else
    m_ostr = ostr;
}

void wxLogStream::DoLogString(const char *szString)
{
  (*m_ostr) << szString << endl << flush;
}

// ----------------------------------------------------------------------------
// wxLogTextCtrl implementation
// ----------------------------------------------------------------------------

/*
wxLogTextCtrl::wxLogTextCtrl(wxTextCtrl *pTextCtrl)
             : wxLogStream(new ostream(pTextCtrl))
{
}

wxLogTextCtrl::~wxLogTextCtrl()
{
  delete m_ostr;
}
*/

// ----------------------------------------------------------------------------
// wxLogGui implementation
// ----------------------------------------------------------------------------

#ifndef   WX_TEST_MINIMAL

wxLogGui::wxLogGui()
{
  m_bErrors = FALSE;
}

void wxLogGui::Flush()
{
  if ( !m_bHasMessages )
    return;

  // @@@ ugly...
  
  // concatenate all strings (but not too many to not overfill the msg box)
  wxString str;
  uint nLines    = 0, 
       nMsgCount = m_aMessages.Count();

  // start from the most recent message
  for ( uint n = nMsgCount; n > 0; n-- ) {
    // for Windows strings longer than this value are wrapped (NT 4.0)
    const uint nMsgLineWidth = 156;

    nLines += (m_aMessages[n - 1].Len() + nMsgLineWidth - 1) / nMsgLineWidth;

    if ( nLines > 25 )  // don't put too many lines in message box
      break;

    str << m_aMessages[n - 1] << "\n";
  }

  if ( m_bErrors ) {
    wxMessageBox(str, _("Error"), wxOK | wxICON_EXCLAMATION);
  }
  else {
    wxMessageBox(str, _("Information"), wxOK | wxICON_INFORMATION);
  }

  // no undisplayed messages whatsoever
  m_bHasMessages =
  m_bErrors      = FALSE;
  m_aMessages.Empty();
}

// the default behaviour is to discard all informational messages if there
// are any errors/warnings.
void wxLogGui::DoLog(Level level, const char *szString)
{
  switch ( level ) {
    case Info:
      if ( GetVerbose() )
    case Message:
        if ( !m_bErrors ) {
          m_aMessages.Add(szString);
          m_bHasMessages = TRUE;
        }
      break;

    case Status:
      {
        // find the top window and set it's status text if it has any
        wxWindow *pWin = wxTheApp->GetTopWindow();
        if ( pWin != NULL && pWin->IsKindOf(CLASSINFO(wxFrame)) ) {
          wxFrame *pFrame = (wxFrame *)pWin;
          pFrame->SetStatusText(szString);
        }
      }
      break;

    case Trace:
    case Debug:
      #ifdef __DEBUG__
        #ifdef  __WIN32__
          OutputDebugString(szString);
          OutputDebugString("\n\r");
        #else   //!WIN32
          // send them to stderr
          printf(stderr, level == Trace ? "Trace: %s\n" 
                                        : "Debug: %s\n", szString);
          fflush(stderr);
        #endif  // WIN32
      #endif
      break;

    case FatalError:
      // show this one immediately
      wxMessageBox(szString, "Fatal error", wxICON_HAND);
      break;

    case Error:
    case Warning:
      // discard earlier informational messages if this is the 1st error
      if ( !m_bErrors ) {
        m_aMessages.Empty();
        m_bHasMessages = TRUE;
        m_bErrors = TRUE;
      }

      m_aMessages.Add(szString);
      break;
    
    default:
      wxFAIL_MSG("unknown log level in wxLogGui::DoLog");
  }
}

#endif  //WX_TEST_MINIMAL

// ============================================================================
// Global functions/variables
// ============================================================================

// ----------------------------------------------------------------------------
// static variables
// ----------------------------------------------------------------------------
wxLog      *wxLog::ms_pLogger      = NULL;
bool        wxLog::ms_bInitialized = FALSE;
bool        wxLog::ms_bVerbose     = FALSE;
const char *wxLog::ms_szTimeFormat = "[%d/%b/%y %H:%M:%S] ";

// ----------------------------------------------------------------------------
// stdout error logging helper
// ----------------------------------------------------------------------------

// helper function: wraps the message and justifies it under given position
// (looks more pretty on the terminal). Also adds newline at the end.
//
// @@ this is now disabled until I find a portable way of determining the
//    terminal window size

#ifdef    LOG_PRETTY_WRAP
static void wxLogWrap(FILE *f, const char *pszPrefix, const char *psz)
{
  size_t nMax = 80; // @@@@
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
  #ifdef  __WINDOWS__
    #ifdef  __WIN32__
      return ::GetLastError();
    #else   //WIN16
      // @@@@ what to do on Windows 3.1?
      return 0;
    #endif  //WIN16/32
  #else   //Unix
    return errno;
  #endif  //Win/Unix
}

// get error message from system
const char *wxSysErrorMsg(unsigned long nErrCode)
{
  if ( nErrCode == 0 )
    nErrCode = wxSysErrorCode();

#ifdef  __WINDOWS__
#ifdef  __WIN32__
  static char s_szBuf[LOG_BUFFER_SIZE / 2];

  // get error message from system
  LPVOID lpMsgBuf;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                NULL, nErrCode, 
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR)&lpMsgBuf,
                0, NULL);

  // copy it to our buffer and free memory
  strncpy(s_szBuf, (const char *)lpMsgBuf, WXSIZEOF(s_szBuf) - 1);
  s_szBuf[WXSIZEOF(s_szBuf) - 1] = '\0';
  LocalFree(lpMsgBuf);

  // returned string is capitalized and ended with '\r\n' - bad
  s_szBuf[0] = (char)tolower(s_szBuf[0]);
  size_t len = strlen(s_szBuf);
  if ( len > 0 ) {
    // truncate string
    if ( s_szBuf[len - 2] == '\r' )
      s_szBuf[len - 2] = '\0';
  }

  return s_szBuf;
#else
  // TODO: Windows 3.1
  return NULL;
#endif
#else
  return strerror(nErrCode);
#endif
}

// ----------------------------------------------------------------------------
// debug helper
// ----------------------------------------------------------------------------

#ifdef  __DEBUG__

// this function is called when an assert fails
void wxOnAssert(const char *szFile, int nLine, const char *szMsg)
{
  // this variable can be set to true to suppress "assert failure" messages
  static s_bNoAsserts = FALSE;

  char szBuf[LOG_BUFFER_SIZE];
  sprintf(szBuf, _("Assert failed in file %s at line %d"), szFile, nLine);
  if ( szMsg != NULL ) {
    strcat(szBuf, ": ");
    strcat(szBuf, szMsg);
  }
  else {
    strcat(szBuf, ".");
  }

  // send it to the normal log destination
  wxLogDebug(szBuf);

  #ifdef  __WINDOWS__
    if ( !s_bNoAsserts ) {
      strcat(szBuf, _("\nDo you want to stop the program?"
                      "\nYou can also choose [Cancel] to suppress "
                      "further warnings."));

      switch ( ::MessageBox(NULL, szBuf, _("Debug"), 
                            MB_YESNOCANCEL | MB_ICONINFORMATION) ) {
        case IDYES:
          DebugBreak();
          break;

        case IDCANCEL:
          s_bNoAsserts = TRUE;
          break;
      }
    }
  #else   
    // @@@@ don't know how to start the debugger under generic Unix
    s_bNoAsserts = TRUE; // suppress 'unused var' warning
    abort();
  #endif
}

#endif  //DEBUG

