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
  #include  <wx/event.h>
  #include  <wx/app.h>
  #include  <wx/string.h>
  #include  <wx/intl.h>
  #include  <wx/menu.h>
  #include  <wx/frame.h>
  #include  <wx/msgdlg.h>
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
  // Redefines OutputDebugString if necessary
  #include  "wx/msw/private.h"
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

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

// we use a global variable to store the frame pointer for wxLogStatus - bad,
// but it's he easiest way
static wxFrame *gs_pFrame; // FIXME MT-unsafe

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

// static buffer for error messages (FIXME MT-unsafe)
static wxChar s_szBuf[LOG_BUFFER_SIZE];

// generic log function
void wxLogGeneric(wxLogLevel level, const wxChar *szFormat, ...)
{
  if ( wxLog::GetActiveTarget() != NULL ) {
    va_list argptr;
    va_start(argptr, szFormat);
    wxVsprintf(s_szBuf, szFormat, argptr);
    va_end(argptr);

    wxLog::OnLog(level, s_szBuf, time(NULL));
  }
}

#define IMPLEMENT_LOG_FUNCTION(level)                             \
  void wxLog##level(const wxChar *szFormat, ...)                  \
  {                                                               \
    if ( wxLog::GetActiveTarget() != NULL ) {                     \
      va_list argptr;                                             \
      va_start(argptr, szFormat);                                 \
      wxVsprintf(s_szBuf, szFormat, argptr);                      \
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

// accepts an additional argument which tells to which frame the output should
// be directed
void wxLogStatus(wxFrame *pFrame, const wxChar *szFormat, ...)
{
  wxLog *pLog = wxLog::GetActiveTarget();
  if ( pLog != NULL ) {
    va_list argptr;
    va_start(argptr, szFormat);
    wxVsprintf(s_szBuf, szFormat, argptr);
    va_end(argptr);

    wxASSERT( gs_pFrame == NULL ); // should be reset!
    gs_pFrame = pFrame;
    wxLog::OnLog(wxLOG_Status, s_szBuf, time(NULL));
    gs_pFrame = (wxFrame *) NULL;
  }
}

// same as info, but only if 'verbose' mode is on
void wxLogVerbose(const wxChar *szFormat, ...)
{
  wxLog *pLog = wxLog::GetActiveTarget();
  if ( pLog != NULL && pLog->GetVerbose() ) {
    va_list argptr;
    va_start(argptr, szFormat);
    wxVsprintf(s_szBuf, szFormat, argptr);
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
      va_list argptr;                                             \
      va_start(argptr, szFormat);                                 \
      wxVsprintf(s_szBuf, szFormat, argptr);                      \
      va_end(argptr);                                             \
                                                                  \
      wxLog::OnLog(wxLOG_##level, s_szBuf, time(NULL));           \
    }                                                             \
  }

  void wxLogTrace(const wxChar *mask, const wxChar *szFormat, ...)
  {
    wxLog *pLog = wxLog::GetActiveTarget();

    if ( pLog != NULL && wxLog::IsAllowedTraceMask(mask) ) {
      va_list argptr;
      va_start(argptr, szFormat);
      wxVsprintf(s_szBuf, szFormat, argptr);
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
      va_list argptr;
      va_start(argptr, szFormat);
      wxVsprintf(s_szBuf, szFormat, argptr);
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
    wxSprintf(szErrMsg, _(" (error %ld: %s)"), lErrCode, wxSysErrorMsg(lErrCode));
    wxStrncat(s_szBuf, szErrMsg, WXSIZEOF(s_szBuf) - wxStrlen(s_szBuf));

    wxLog::OnLog(wxLOG_Error, s_szBuf, time(NULL));
}

void WXDLLEXPORT wxLogSysError(const wxChar *szFormat, ...)
{
    va_list argptr;
    va_start(argptr, szFormat);
    wxVsprintf(s_szBuf, szFormat, argptr);
    va_end(argptr);

    wxLogSysErrorHelper(wxSysErrorCode());
}

void WXDLLEXPORT wxLogSysError(long lErrCode, const wxChar *szFormat, ...)
{
    va_list argptr;
    va_start(argptr, szFormat);
    wxVsprintf(s_szBuf, szFormat, argptr);
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

#ifdef  wxUSE_NOGUI
            ms_pLogger = new wxLogStderr;
#else
            // ask the application to create a log target for us
            if ( wxTheApp != NULL )
                ms_pLogger = wxTheApp->CreateLogTarget();
            else
                ms_pLogger = new wxLogStderr;
#endif

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

void wxLog::DoLog(wxLogLevel level, const wxChar *szString, time_t t)
{
    wxString str;

    switch ( level ) {
        case wxLOG_FatalError:
            DoLogString(str << _("Fatal error: ") << szString, t);
            DoLogString(_("Program aborted."), t);
            Flush();
            abort();
            break;

        case wxLOG_Error:
            DoLogString(str << _("Error: ") << szString, t);
            break;

        case wxLOG_Warning:
            DoLogString(str << _("Warning: ") << szString, t);
            break;

        case wxLOG_Info:
        case wxLOG_Message:
            if ( GetVerbose() )
                DoLogString(str + szString, t);
            // fall through

        case wxLOG_Status:
            // nothing to do
            break;

        case wxLOG_Trace:
        case wxLOG_Debug:
#ifdef __WXDEBUG__
            DoLogString(szString, t);
#endif

            break;

        default:
            wxFAIL_MSG(_("unknown log level in wxLog::DoLog"));
    }
}

void wxLog::DoLogString(const wxChar *WXUNUSED(szString), time_t t)
{
    wxFAIL_MSG(_T("DoLogString must be overriden if it's called."));
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

void wxLogStderr::DoLogString(const wxChar *szString, time_t t)
{
    wxString str(szString);
    str << _T('\n');

    fputs(str.mb_str(), m_fp);
    fflush(m_fp);

    // under Windows, programs usually don't have stderr at all, so make show the
    // messages also under debugger
#ifdef __WXMSW__
    OutputDebugString(str + _T('\r'));
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

void wxLogStream::DoLogString(const wxChar *szString, time_t t)
{
    (*m_ostr) << szString << endl << flush;
}
#endif // wxUSE_STD_IOSTREAM

#ifndef wxUSE_NOGUI

// ----------------------------------------------------------------------------
// wxLogTextCtrl implementation
// ----------------------------------------------------------------------------

#if wxUSE_STD_IOSTREAM
wxLogTextCtrl::wxLogTextCtrl(wxTextCtrl *pTextCtrl)
#if !defined(NO_TEXT_WINDOW_STREAM)
: wxLogStream(new ostream(pTextCtrl))
#endif
{
}

wxLogTextCtrl::~wxLogTextCtrl()
{
    delete m_ostr;
}
#endif // wxUSE_STD_IOSTREAM

// ----------------------------------------------------------------------------
// wxLogGui implementation (FIXME MT-unsafe)
// ----------------------------------------------------------------------------

wxLogGui::wxLogGui()
{
    Clear();
}

void wxLogGui::Clear()
{
    m_bErrors = m_bWarnings = FALSE;
    m_aMessages.Empty();
    m_aTimes.Empty();
}

void wxLogGui::Flush()
{
    if ( !m_bHasMessages )
        return;

    // do it right now to block any new calls to Flush() while we're here
    m_bHasMessages = FALSE;

    // concatenate all strings (but not too many to not overfill the msg box)
    wxString str;
    size_t nLines = 0,
    nMsgCount = m_aMessages.Count();

    // start from the most recent message
    for ( size_t n = nMsgCount; n > 0; n-- ) {
        // for Windows strings longer than this value are wrapped (NT 4.0)
        const size_t nMsgLineWidth = 156;

        nLines += (m_aMessages[n - 1].Len() + nMsgLineWidth - 1) / nMsgLineWidth;

        if ( nLines > 25 )  // don't put too many lines in message box
            break;

        str << m_aMessages[n - 1] << _T("\n");
    }

    const wxChar *title;
    long style;

    if ( m_bErrors ) {
        title = _("Error");
        style = wxICON_STOP;
    }
    else if ( m_bWarnings ) {
        title = _("Warning");
        style = wxICON_EXCLAMATION;
    }
    else {
        title = _("Information");
        style = wxICON_INFORMATION;
    }

    wxMessageBox(str, title, wxOK | style);

    // no undisplayed messages whatsoever
    Clear();
}

// the default behaviour is to discard all informational messages if there
// are any errors/warnings.
void wxLogGui::DoLog(wxLogLevel level, const wxChar *szString, time_t t)
{
    switch ( level ) {
        case wxLOG_Info:
            if ( GetVerbose() )
        case wxLOG_Message:
                if ( !m_bErrors ) {
                    m_aMessages.Add(szString);
                    m_aTimes.Add((long)t);
                    m_bHasMessages = TRUE;
                }
                break;

        case wxLOG_Status:
                {
                    // find the top window and set it's status text if it has any
                    wxFrame *pFrame = gs_pFrame;
                    if ( pFrame == NULL ) {
                        wxWindow *pWin = wxTheApp->GetTopWindow();
                        if ( pWin != NULL && pWin->IsKindOf(CLASSINFO(wxFrame)) ) {
                            pFrame = (wxFrame *)pWin;
                        }
                    }

                    if ( pFrame != NULL )
                        pFrame->SetStatusText(szString);
                }
                break;

        case wxLOG_Trace:
        case wxLOG_Debug:
                #ifdef __WXDEBUG__
                {
                    #ifdef __WXMSW__
                        // don't prepend debug/trace here: it goes to the
                        // debug window anyhow, but do put a timestamp
                        OutputDebugString(wxString(szString) + _T("\n\r"));
                    #else
                        // send them to stderr
                        fprintf(stderr, "%s: %s\n",
                                level == wxLOG_Trace ? "Trace" : "Debug",
                                (const char*)wxConv_libc.cWX2MB(szString));
                        fflush(stderr);
                    #endif
                }
                #endif // __WXDEBUG__

                break;

        case wxLOG_FatalError:
                // show this one immediately
                wxMessageBox(szString, _("Fatal error"), wxICON_HAND);
                break;

        case wxLOG_Error:
                // discard earlier informational messages if this is the 1st
                // error because they might not make sense any more
                if ( !m_bErrors ) {
                    m_aMessages.Empty();
                    m_aTimes.Empty();
                    m_bHasMessages = TRUE;
                    m_bErrors = TRUE;
                }
                // fall through

        case wxLOG_Warning:
                if ( !m_bErrors ) {
                    // for the warning we don't discard the info messages
                    m_bWarnings = TRUE;
                }

                m_aMessages.Add(szString);
                m_aTimes.Add((long)t);
                break;

        default:
                wxFAIL_MSG(_("unknown log level in wxLogGui::DoLog"));
    }
}

// ----------------------------------------------------------------------------
// wxLogWindow and wxLogFrame implementation
// ----------------------------------------------------------------------------

// log frame class
// ---------------
class wxLogFrame : public wxFrame
{
public:
    // ctor & dtor
    wxLogFrame(wxFrame *pParent, wxLogWindow *log, const wxChar *szTitle);
    virtual ~wxLogFrame();

    // menu callbacks
    void OnClose(wxCommandEvent& event);
    void OnCloseWindow(wxCloseEvent& event);
    void OnSave (wxCommandEvent& event);
    void OnClear(wxCommandEvent& event);

    void OnIdle(wxIdleEvent&);

    // accessors
    wxTextCtrl *TextCtrl() const { return m_pTextCtrl; }

private:
    enum
    {
        Menu_Close = 100,
        Menu_Save,
        Menu_Clear
    };

    // instead of closing just hide the window to be able to Show() it later
    void DoClose() { Show(FALSE); }

    wxTextCtrl  *m_pTextCtrl;
    wxLogWindow *m_log;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxLogFrame, wxFrame)
    // wxLogWindow menu events
    EVT_MENU(Menu_Close, wxLogFrame::OnClose)
    EVT_MENU(Menu_Save,  wxLogFrame::OnSave)
    EVT_MENU(Menu_Clear, wxLogFrame::OnClear)

    EVT_CLOSE(wxLogFrame::OnCloseWindow)
END_EVENT_TABLE()

wxLogFrame::wxLogFrame(wxFrame *pParent, wxLogWindow *log, const wxChar *szTitle)
          : wxFrame(pParent, -1, szTitle)
{
    m_log = log;

    m_pTextCtrl = new wxTextCtrl(this, -1, wxEmptyString, wxDefaultPosition,
            wxDefaultSize,
            wxTE_MULTILINE  |
            wxHSCROLL       |
            wxTE_READONLY);

    // create menu
    wxMenuBar *pMenuBar = new wxMenuBar;
    wxMenu *pMenu = new wxMenu;
    pMenu->Append(Menu_Save,  _("&Save..."), _("Save log contents to file"));
    pMenu->Append(Menu_Clear, _("C&lear"), _("Clear the log contents"));
    pMenu->AppendSeparator();
    pMenu->Append(Menu_Close, _("&Close"), _("Close this window"));
    pMenuBar->Append(pMenu, _("&Log"));
    SetMenuBar(pMenuBar);

    // status bar for menu prompts
    CreateStatusBar();

    m_log->OnFrameCreate(this);
}

void wxLogFrame::OnClose(wxCommandEvent& WXUNUSED(event))
{
    DoClose();
}

void wxLogFrame::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    DoClose();
}

void wxLogFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
    // get the file name
    // -----------------
    const wxChar *szFileName = wxSaveFileSelector(_T("log"), _T("txt"), _T("log.txt"));
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
    int nLines = m_pTextCtrl->GetNumberOfLines();
    for ( int nLine = 0; bOk && nLine < nLines; nLine++ ) {
        bOk = file.Write(m_pTextCtrl->GetLineText(nLine) +
                // we're not going to pull in the whole wxTextFile if all we need is this...
#if wxUSE_TEXTFILE
                wxTextFile::GetEOL()
#else // !wxUSE_TEXTFILE
                '\n'
#endif // wxUSE_TEXTFILE
                        );
    }

    if ( bOk )
        bOk = file.Close();

    if ( !bOk ) {
        wxLogError(_("Can't save log contents to file."));
    }
    else {
        wxLogStatus(this, _("Log saved to the file '%s'."), szFileName);
    }
}

void wxLogFrame::OnClear(wxCommandEvent& WXUNUSED(event))
{
    m_pTextCtrl->Clear();
}

wxLogFrame::~wxLogFrame()
{
    m_log->OnFrameDelete(this);
}

// wxLogWindow
// -----------
wxLogWindow::wxLogWindow(wxFrame *pParent,
        const wxChar *szTitle,
        bool bShow,
        bool bDoPass)
{
    m_bPassMessages = bDoPass;

    m_pLogFrame = new wxLogFrame(pParent, this, szTitle);
    m_pOldLog = wxLog::SetActiveTarget(this);

    if ( bShow )
        m_pLogFrame->Show(TRUE);
}

void wxLogWindow::Show(bool bShow)
{
    m_pLogFrame->Show(bShow);
}

void wxLogWindow::Flush()
{
    if ( m_pOldLog != NULL )
        m_pOldLog->Flush();

    m_bHasMessages = FALSE;
}

void wxLogWindow::DoLog(wxLogLevel level, const wxChar *szString, time_t t)
{
    // first let the previous logger show it
    if ( m_pOldLog != NULL && m_bPassMessages ) {
        // FIXME why can't we access protected wxLog method from here (we derive
        //       from wxLog)? gcc gives "DoLog is protected in this context", what
        //       does this mean? Anyhow, the cast is harmless and let's us do what
        //       we want.
        ((wxLogWindow *)m_pOldLog)->DoLog(level, szString, t);
    }

    if ( m_pLogFrame ) {
        switch ( level ) {
            case wxLOG_Status:
                // by default, these messages are ignored by wxLog, so process
                // them ourselves
                {
                    wxString str;
                    str << _("Status: ") << szString;
                    DoLogString(str, t);
                }
                break;

                // don't put trace messages in the text window for 2 reasons:
                // 1) there are too many of them
                // 2) they may provoke other trace messages thus sending a program
                //    into an infinite loop
            case wxLOG_Trace:
                break;

            default:
                // and this will format it nicely and call our DoLogString()
                wxLog::DoLog(level, szString, t);
        }
    }

    m_bHasMessages = TRUE;
}

void wxLogWindow::DoLogString(const wxChar *szString, time_t t)
{
    // put the text into our window
    wxTextCtrl *pText = m_pLogFrame->TextCtrl();

    // remove selection (WriteText is in fact ReplaceSelection)
#ifdef __WXMSW__
    long nLen = pText->GetLastPosition();
    pText->SetSelection(nLen, nLen);
#endif // Windows

    pText->WriteText(szString);
    pText->WriteText(_T("\n")); // "\n" ok here (_not_ "\r\n")

    // TODO ensure that the line can be seen
}

wxFrame *wxLogWindow::GetFrame() const
{
    return m_pLogFrame;
}

void wxLogWindow::OnFrameCreate(wxFrame * WXUNUSED(frame))
{
}

void wxLogWindow::OnFrameDelete(wxFrame * WXUNUSED(frame))
{
    m_pLogFrame = (wxLogFrame *)NULL;
}

wxLogWindow::~wxLogWindow()
{
    delete m_pOldLog;

    // may be NULL if log frame already auto destroyed itself
    delete m_pLogFrame;
}

#endif  //wxUSE_NOGUI

// ============================================================================
// Global functions/variables
// ============================================================================

// ----------------------------------------------------------------------------
// static variables
// ----------------------------------------------------------------------------

wxLog          *wxLog::ms_pLogger      = (wxLog *)NULL;
bool            wxLog::ms_doLog        = TRUE;
bool            wxLog::ms_bAutoCreate  = TRUE;
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
    s_szBuf[WXSIZEOF(s_szBuf) - 1] = _T('\0');
    LocalFree(lpMsgBuf);

    // returned string is capitalized and ended with '\r\n' - bad
    s_szBuf[0] = (wxChar)wxTolower(s_szBuf[0]);
    size_t len = wxStrlen(s_szBuf);
    if ( len > 0 ) {
        // truncate string
        if ( s_szBuf[len - 2] == _T('\r') )
            s_szBuf[len - 2] = _T('\0');
    }

    return s_szBuf;
#else   //Win16
    // TODO
    return NULL;
#endif // Win16/32
#else   // Unix
#if wxUSE_UNICODE
    static wxChar s_szBuf[LOG_BUFFER_SIZE / 2];
    wxConv_libc.MB2WC(s_szBuf, strerror(nErrCode), WXSIZEOF(s_szBuf) -1);
    return s_szBuf;
#else
    return strerror(nErrCode);
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
void wxOnAssert(const char *szFile, int nLine, const wxChar *szMsg)
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
#ifdef __VISUALC__
    sprintf(szBuf, _T("%s(%d): assert failed"), szFile, nLine);
#else  // !VC++
    // make the error message more clear for all the others
#ifdef wxSprintf
    wxSprintf(szBuf, _T("Assert failed in file %s at line %d"), szFile, nLine);
#else
    wxSprintf(szBuf, _T("Assert failed in file %hs at line %d"), szFile, nLine);
#endif
#endif // VC/!VC

    if ( szMsg != NULL ) {
        wxStrcat(szBuf, _T(": "));
        wxStrcat(szBuf, szMsg);
    }
    else {
        wxStrcat(szBuf, _T("."));
    }

    if ( !s_bNoAsserts ) {
        // send it to the normal log destination
        wxLogDebug(szBuf);

#if wxUSE_NOGUI
        Trap();
#else
        // this message is intentionally not translated - it is for
        // developpers only
        wxStrcat(szBuf, _T("\nDo you want to stop the program?"
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
#endif // USE_NOGUI
    }

    s_bInAssert = FALSE;
}

#endif  //WXDEBUG

