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
  #include  <wx/app.h>
  #include  <wx/string.h>
  #include  <wx/intl.h>
  #include  <wx/menu.h>

  #include  <wx/generic/msgdlgg.h>
  #include  <wx/filedlg.h>
  #include  <wx/textctrl.h>
#endif //WX_PRECOMP

#include  <wx/file.h>
#include  <wx/textfile.h>
#include  <wx/utils.h>
#include  <wx/log.h>

// other standard headers
#include  <errno.h>
#include  <stdlib.h>
#include  <time.h>

#ifdef  __WXMSW__
  #include  <windows.h>
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
void wxLogGeneric(wxLogLevel level, const char *szFormat, ...)
{
  if ( wxLog::GetActiveTarget() != NULL ) {
    va_list argptr;
    va_start(argptr, szFormat);
    vsprintf(s_szBuf, szFormat, argptr);
    va_end(argptr);

    wxLog::OnLog(level, s_szBuf);
  }
}

#define IMPLEMENT_LOG_FUNCTION(level)                             \
  void wxLog##level(const char *szFormat, ...)                    \
  {                                                               \
    if ( wxLog::GetActiveTarget() != NULL ) {                     \
      va_list argptr;                                             \
      va_start(argptr, szFormat);                                 \
      vsprintf(s_szBuf, szFormat, argptr);                        \
      va_end(argptr);                                             \
                                                                  \
      wxLog::OnLog(wxLOG_##level, s_szBuf);                       \
    }                                                             \
  }

IMPLEMENT_LOG_FUNCTION(FatalError)
IMPLEMENT_LOG_FUNCTION(Error)
IMPLEMENT_LOG_FUNCTION(Warning)
IMPLEMENT_LOG_FUNCTION(Message)
IMPLEMENT_LOG_FUNCTION(Info)
IMPLEMENT_LOG_FUNCTION(Status)

// same as info, but only if 'verbose' mode is on
void wxLogVerbose(const char *szFormat, ...)
{
  wxLog *pLog = wxLog::GetActiveTarget();
  if ( pLog != NULL && pLog->GetVerbose() ) {
    va_list argptr;
    va_start(argptr, szFormat);
    vsprintf(s_szBuf, szFormat, argptr);
    va_end(argptr);

    wxLog::OnLog(wxLOG_Info, s_szBuf);
  }
}

// debug functions
#ifdef __WXDEBUG__
#define IMPLEMENT_LOG_DEBUG_FUNCTION(level)                       \
  void wxLog##level(const char *szFormat, ...)                    \
  {                                                               \
    if ( wxLog::GetActiveTarget() != NULL ) {                     \
      va_list argptr;                                             \
      va_start(argptr, szFormat);                                 \
      vsprintf(s_szBuf, szFormat, argptr);                        \
      va_end(argptr);                                             \
                                                                  \
      wxLog::OnLog(wxLOG_##level, s_szBuf);                       \
    }                                                             \
  }

  void wxLogTrace(wxTraceMask mask, const char *szFormat, ...)
  {
    wxLog *pLog = wxLog::GetActiveTarget();

    // we check that all of mask bits are set in the current mask, so
    // that wxLogTrace(wxTraceRefCount | wxTraceOle) will only do something
    // if both bits are set.
    if ( pLog != NULL && ((pLog->GetTraceMask() & mask) == mask) ) {
      va_list argptr;
      va_start(argptr, szFormat);
      vsprintf(s_szBuf, szFormat, argptr);
      va_end(argptr);

      wxLog::OnLog(wxLOG_Trace, s_szBuf);
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
  char szErrMsg[LOG_BUFFER_SIZE / 2];
  sprintf(szErrMsg, _(" (error %ld: %s)"), lErrCode, wxSysErrorMsg(lErrCode));
  strncat(s_szBuf, szErrMsg, WXSIZEOF(s_szBuf) - strlen(s_szBuf));

  wxLog::OnLog(wxLOG_Error, s_szBuf);
}

void WXDLLEXPORT wxLogSysError(const char *szFormat, ...)
{
  va_list argptr;
  va_start(argptr, szFormat);
  vsprintf(s_szBuf, szFormat, argptr);
  va_end(argptr);

  wxLogSysErrorHelper(wxSysErrorCode());
}

void WXDLLEXPORT wxLogSysError(long lErrCode, const char *szFormat, ...)
{
  va_list argptr;
  va_start(argptr, szFormat);
  vsprintf(s_szBuf, szFormat, argptr);
  va_end(argptr);

  wxLogSysErrorHelper(lErrCode);
}

// ----------------------------------------------------------------------------
// wxLog class implementation
// ----------------------------------------------------------------------------

wxLog::wxLog()
{
  m_bHasMessages = FALSE;
  m_bVerbose     = FALSE;
  m_szTimeFormat = "[%d/%b/%y %H:%M:%S] ";
}

wxLog *wxLog::GetActiveTarget()
{
  if ( ms_bAutoCreate && ms_pLogger == NULL ) {
    // prevent infinite recursion if someone calls wxLogXXX() from
    // wxApp::CreateLogTarget()
    static bool s_bInGetActiveTarget = FALSE;
    if ( !s_bInGetActiveTarget ) {
      s_bInGetActiveTarget = TRUE;

      #ifdef  WX_TEST_MINIMAL
        ms_pLogger = new wxLogStderr;
      #else
        // ask the application to create a log target for us
        if ( wxTheApp != NULL )
          ms_pLogger = wxTheApp->CreateLogTarget();
      #endif

      // do nothing if it fails - what can we do?
    }
  }

  return ms_pLogger;
}

wxLog *wxLog::SetActiveTarget(wxLog *pLogger)
{
  // flush the old messages before changing
  if ( ms_pLogger != NULL )
    ms_pLogger->Flush();

  wxLog *pOldLogger = ms_pLogger;
  ms_pLogger = pLogger;
  return pOldLogger;
}

void wxLog::DoLog(wxLogLevel level, const char *szString)
{
  wxString str;

  // prepend a timestamp if not disabled
  if ( !IsEmpty(m_szTimeFormat) ) {
    char szBuf[128];
    time_t timeNow;
    struct tm *ptmNow;

    time(&timeNow);
    ptmNow = localtime(&timeNow);

    strftime(szBuf, WXSIZEOF(szBuf), m_szTimeFormat, ptmNow);
    str = szBuf;
  }

  switch ( level ) {
    case wxLOG_FatalError:
      DoLogString(str << _("Fatal error: ") << szString);
      DoLogString(_("Program aborted."));
      Flush();
      abort();
      break;

    case wxLOG_Error:
      DoLogString(str << _("Error: ") << szString);
      break;

    case wxLOG_Warning:
      DoLogString(str << _("Warning: ") << szString);
      break;

    case wxLOG_Info:
      if ( GetVerbose() )
    case wxLOG_Message:
        DoLogString(str + szString);
      // fall through

    case wxLOG_Status:
      // nothing to do
      break;

    case wxLOG_Trace:
    case wxLOG_Debug:
      #ifdef __WXDEBUG__
        #ifdef  __WIN32__
          // in addition to normal logging, also send the string to debugger
          // (don't prepend "Debug" here: it will go to debug window anyhow)
          ::OutputDebugString(str + szString + "\n\r");
        #endif  //Win32
        DoLogString(str << (level == wxLOG_Trace ? _("Trace") : _("Debug"))
                        << ": " << szString);
      #endif

      break;

    default:
      wxFAIL_MSG(_("unknown log level in wxLog::DoLog"));
  }
}

void wxLog::DoLogString(const char *WXUNUSED(szString))
{
  wxFAIL_MSG(_("DoLogString must be overrided if it's called."));
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
wxLogTextCtrl::wxLogTextCtrl(wxTextCtrl *pTextCtrl)
// @@@ TODO: in wxGTK wxTextCtrl doesn't derive from streambuf

// Also, in DLL mode in wxMSW, can't use it.
#if defined(NO_TEXT_WINDOW_STREAM)
#else
             : wxLogStream(new ostream(pTextCtrl))
#endif
{
}

wxLogTextCtrl::~wxLogTextCtrl()
{
  delete m_ostr;
}

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
void wxLogGui::DoLog(wxLogLevel level, const char *szString)
{
  switch ( level ) {
    case wxLOG_Info:
      if ( GetVerbose() )
    case wxLOG_Message:
        if ( !m_bErrors ) {
          m_aMessages.Add(szString);
          m_bHasMessages = TRUE;
        }
      break;

    case wxLOG_Status:
      {
        // find the top window and set it's status text if it has any
        wxWindow *pWin = wxTheApp->GetTopWindow();
        if ( pWin != NULL && pWin->IsKindOf(CLASSINFO(wxFrame)) ) {
          wxFrame *pFrame = (wxFrame *)pWin;
          pFrame->SetStatusText(szString);
        }
      }
      break;

    case wxLOG_Trace:
    case wxLOG_Debug:
      #ifdef __WXDEBUG__
        #ifdef  __WIN32__
          OutputDebugString(szString);
          OutputDebugString("\n\r");
        #else   //!WIN32
          // send them to stderr
          fprintf(stderr, "%s: %s\n",
                  level == wxLOG_Trace ? _("Trace") : _("Debug"), szString);
          fflush(stderr);
        #endif  // WIN32
      #endif
      break;

    case wxLOG_FatalError:
      // show this one immediately
      wxMessageBox(szString, _("Fatal error"), wxICON_HAND);
      break;

    case wxLOG_Error:
    case wxLOG_Warning:
      // discard earlier informational messages if this is the 1st error
      if ( !m_bErrors ) {
        m_aMessages.Empty();
        m_bHasMessages = TRUE;
        m_bErrors = TRUE;
      }

      m_aMessages.Add(szString);
      break;

    default:
      wxFAIL_MSG(_("unknown log level in wxLogGui::DoLog"));
  }
}

// ----------------------------------------------------------------------------
// wxLogWindow implementation
// ----------------------------------------------------------------------------

// log frame class
class wxLogFrame : public wxFrame
{
public:
  // ctor
  wxLogFrame(const char *szTitle);

  // menu callbacks
  void OnClose(wxCommandEvent& event);
  void OnCloseWindow(wxCloseEvent& event);
  void OnSave (wxCommandEvent& event);
  void OnClear(wxCommandEvent& event);

  // accessors
  wxTextCtrl *TextCtrl() const { return m_pTextCtrl; }

private:
  enum
  {
    Menu_Close = 100,
    Menu_Save,
    Menu_Clear
  };

  wxTextCtrl *m_pTextCtrl;

  DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxLogFrame, wxFrame)
  // wxLogWindow menu events
  EVT_MENU(Menu_Close, wxLogFrame::OnClose)
  EVT_MENU(Menu_Save,  wxLogFrame::OnSave)
  EVT_MENU(Menu_Clear, wxLogFrame::OnClear)

  EVT_CLOSE(wxLogFrame::OnCloseWindow)
END_EVENT_TABLE()

wxLogFrame::wxLogFrame(const char *szTitle)
          : wxFrame(NULL, -1, szTitle)
{
  // we don't want to be a top-level frame because it would prevent the
  // application termination when all other frames are closed
  wxTopLevelWindows.DeleteObject(this);

  // @@ kludge: wxSIMPLE_BORDER is simply to prevent wxWindows from creating
  //    a rich edit control instead of a normal one we want
  m_pTextCtrl = new wxTextCtrl(this, -1, wxEmptyString, wxDefaultPosition,
                               wxDefaultSize,
                               wxSIMPLE_BORDER |
                               wxTE_MULTILINE  |
                               wxHSCROLL       |
                               wxTE_READONLY);
  /*
  m_pTextCtrl->SetEditable(FALSE);
  m_pTextCtrl->SetRichEdit(FALSE);
  */

  // create menu
  wxMenuBar *pMenuBar = new wxMenuBar;
  wxMenu *pMenu = new wxMenu;
  pMenu->Append(Menu_Save,  _("&Save..."));
  pMenu->Append(Menu_Clear, _("C&lear"));
  pMenu->AppendSeparator();
  pMenu->Append(Menu_Close, _("&Close"));
  pMenuBar->Append(pMenu, _("&Log"));
  SetMenuBar(pMenuBar);

  // @@ what about status bar? needed (for menu prompts)?
}

void wxLogFrame::OnClose(wxCommandEvent& WXUNUSED(event))
{
  // just hide the window
  Show(FALSE);
}

void wxLogFrame::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
  // just hide the window
  Show(FALSE);
}

void wxLogFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
  // get the file name
  // -----------------
  const char *szFileName = wxSaveFileSelector("log", "txt", "log.txt");
  if ( szFileName == NULL ) {
    // cancelled
    return;
  }

  // open file
  // ---------
  wxFile file;
  bool bOk = FALSE; 
  if ( wxFile::Exists(szFileName) ) {
    bool bAppend = FALSE;
    wxString strMsg;
    strMsg.Printf(_("Append log to file '%s' "
                    "(choosing [No] will overwrite it)?"), szFileName);
    switch ( wxMessageBox(strMsg, _("Question"), wxYES_NO | wxCANCEL) ) {
      case wxYES:
        bAppend = TRUE;
        break;

      case wxNO:
        bAppend = FALSE;
        break;

      case wxCANCEL:
        return;

      default:
        wxFAIL_MSG(_("invalid message box return value"));
    }

    if ( bAppend ) {
      bOk = file.Open(szFileName, wxFile::write_append);
    }
    else {
      bOk = file.Create(szFileName, TRUE /* overwrite */);
    }
  }
  else {
    bOk = file.Create(szFileName);
  }

  // retrieve text and save it
  // -------------------------
#ifdef __WXGTK__
  // @@@@ TODO: no GetNumberOfLines and GetLineText in wxGTK yet
  wxLogError(_("Sorry, this function is not implemented under GTK"));
#else
  int nLines = m_pTextCtrl->GetNumberOfLines();
  for ( int nLine = 0; bOk && nLine < nLines; nLine++ ) {
    bOk = file.Write(m_pTextCtrl->GetLineText(nLine) + wxTextFile::GetEOL());
  }
#endif //GTK

  if ( bOk )
    bOk = file.Close();

  if ( !bOk ) {
    wxLogError(_("Can't save log contents to file."));
    return;
  }
}

void wxLogFrame::OnClear(wxCommandEvent& WXUNUSED(event))
{
  m_pTextCtrl->Clear();
}

wxLogWindow::wxLogWindow(const char *szTitle, bool bShow, bool bDoPass)
{
  m_bPassMessages = bDoPass;

  m_pLogFrame = new wxLogFrame(szTitle);
  m_pOldLog = wxLog::SetActiveTarget(this);

  if ( bShow )
    m_pLogFrame->Show(TRUE);
}

void wxLogWindow::Show(bool bShow)
{
  m_pLogFrame->Show(bShow);
}

wxFrame *wxLogWindow::GetFrame() const
{
  return m_pLogFrame;
}

void wxLogWindow::DoLog(wxLogLevel level, const char *szString)
{
  // first let the previous logger show it
  if ( m_pOldLog != NULL && m_bPassMessages ) {
    // @@@ why can't we access protected wxLog method from here (we derive
    // from wxLog)? gcc gives "DoLog is protected in this context", what
    // does this mean? Anyhow, the cast is harmless and let's us do what
    // we want.
    ((wxLogWindow *)m_pOldLog)->DoLog(level, szString);
  }

  // and this will format it nicely and call our DoLogString()
  wxLog::DoLog(level, szString);
}

void wxLogWindow::DoLogString(const char *szString)
{
  // put the text into our window
  wxTextCtrl *pText = m_pLogFrame->TextCtrl();

  // remove selection (WriteText is in fact ReplaceSelection)
  #ifdef __WXMSW__
    long nLen = pText->GetLastPosition();
    pText->SetSelection(nLen, nLen);
  #endif // Windows

  pText->WriteText(szString);
  pText->WriteText("\n"); // "\n" ok here (_not_ "\r\n")

  // ensure that the line can be seen
  // @@@ TODO
}

wxLogWindow::~wxLogWindow()
{
  m_pLogFrame->Close(TRUE);
}

#endif  //WX_TEST_MINIMAL

// ============================================================================
// Global functions/variables
// ============================================================================

// ----------------------------------------------------------------------------
// static variables
// ----------------------------------------------------------------------------
wxLog      *wxLog::ms_pLogger      = NULL;
bool        wxLog::ms_bAutoCreate  = TRUE;
wxTraceMask wxLog::ms_ulTraceMask  = (wxTraceMask)0;

// ----------------------------------------------------------------------------
// stdout error logging helper
// ----------------------------------------------------------------------------

// helper function: wraps the message and justifies it under given position
// (looks more pretty on the terminal). Also adds newline at the end.
//
// @@ this is now disabled until I find a portable way of determining the
//    terminal window size (ok, I found it but does anybody really cares?)
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
  #ifdef  __WXMSW__
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

  #ifdef  __WXMSW__
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
      s_szBuf[0] = (char)wxToLower(s_szBuf[0]);
      size_t len = strlen(s_szBuf);
      if ( len > 0 ) {
        // truncate string
        if ( s_szBuf[len - 2] == '\r' )
          s_szBuf[len - 2] = '\0';
      }

      return s_szBuf;
    #else   //Win16
      // TODO @@@@
      return NULL;
    #endif // Win16/32
  #else   // Unix
    return strerror(nErrCode);
  #endif  // Win/Unix
}

// ----------------------------------------------------------------------------
// debug helper
// ----------------------------------------------------------------------------

#ifdef  __WXDEBUG__

void Trap()
{
  #ifdef __WXMSW__
    DebugBreak();
  #else // Unix
    raise(SIGTRAP);
  #endif // Win/Unix
}

// this function is called when an assert fails
void wxOnAssert(const char *szFile, int nLine, const char *szMsg)
{
  // this variable can be set to true to suppress "assert failure" messages
  static bool s_bNoAsserts = FALSE;
  static bool s_bInAssert = FALSE;

  if ( s_bInAssert ) {
    // He-e-e-e-elp!! we're trapped in endless loop
    Trap();
  }

  s_bInAssert = TRUE;

  char szBuf[LOG_BUFFER_SIZE];
  sprintf(szBuf, _("Assert failed in file %s at line %d"), szFile, nLine);
  if ( szMsg != NULL ) {
    strcat(szBuf, ": ");
    strcat(szBuf, szMsg);
  }
  else {
    strcat(szBuf, ".");
  }

  if ( !s_bNoAsserts ) {
    // send it to the normal log destination
    wxLogDebug(szBuf);

    strcat(szBuf, _("\nDo you want to stop the program?"
                    "\nYou can also choose [Cancel] to suppress "
                    "further warnings."));

    switch ( wxMessageBox(szBuf, _("Debug"),
                          wxYES_NO | wxCANCEL | wxICON_STOP ) ) {
      case wxYES:
        Trap();
        break;

      case wxCANCEL:
        s_bNoAsserts = TRUE;
        break;

      //case wxNO: nothing to do
    }
  }

  s_bInAssert = FALSE;
}

#endif  //WXDEBUG

