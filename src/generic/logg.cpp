/////////////////////////////////////////////////////////////////////////////
// Name:        logg.cpp
// Purpose:     wxLog-derived classes which need GUI support (the rest is in
//              src/common/log.cpp)
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.09.99 (extracted from src/common/log.cpp)
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

// no #pragma implementation "log.h" because it's already in src/common/log.cpp

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if !wxUSE_GUI
    #error "This file can't be compiled without GUI!"
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/menu.h"
    #include "wx/frame.h"
    #include "wx/filedlg.h"
    #include "wx/msgdlg.h"
    #include "wx/textctrl.h"
#endif // WX_PRECOMP

#include "wx/file.h"
#include "wx/textfile.h"

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
// global functions
// ----------------------------------------------------------------------------

// accepts an additional argument which tells to which frame the output should
// be directed
void wxLogStatus(wxFrame *pFrame, const wxChar *szFormat, ...)
{
  wxString msg;

  wxLog *pLog = wxLog::GetActiveTarget();
  if ( pLog != NULL ) {
    va_list argptr;
    va_start(argptr, szFormat);
    msg.PrintfV(szFormat, argptr);
    va_end(argptr);

    wxASSERT( gs_pFrame == NULL ); // should be reset!
    gs_pFrame = pFrame;
    wxLog::OnLog(wxLOG_Status, msg, time(NULL));
    gs_pFrame = (wxFrame *) NULL;
  }
}

// ----------------------------------------------------------------------------
// wxLogTextCtrl implementation
// ----------------------------------------------------------------------------

wxLogTextCtrl::wxLogTextCtrl(wxTextCtrl *pTextCtrl)
{
    m_pTextCtrl = pTextCtrl;
}

void wxLogTextCtrl::DoLogString(const wxChar *szString, time_t WXUNUSED(t))
{
    wxString msg;
    TimeStamp(&msg);
    msg << szString << T('\n');

    m_pTextCtrl->AppendText(msg);
}

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

        str << m_aMessages[n - 1] << T("\n");
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
    
    // do it here again
    m_bHasMessages = FALSE;
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
#if wxUSE_STATUSBAR
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
#endif // wxUSE_STATUSBAR
                break;

        case wxLOG_Trace:
        case wxLOG_Debug:
                #ifdef __WXDEBUG__
                {
                    #ifdef __WXMSW__
                        // don't prepend debug/trace here: it goes to the
                        // debug window anyhow, but do put a timestamp
                        wxString str;
                        TimeStamp(&str);
                        str << szString << T("\n\r");
                        OutputDebugString(str);
                    #else
                        // send them to stderr
                        wxFprintf(stderr, T("%s: %s\n"),
                                  level == wxLOG_Trace ? T("Trace")
                                                       : T("Debug"),
                                  szString);
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
                m_bHasMessages = TRUE;
                break;
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
#if wxUSE_FILE
    void OnSave (wxCommandEvent& event);
#endif // wxUSE_FILE
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
#if wxUSE_FILE
    EVT_MENU(Menu_Save,  wxLogFrame::OnSave)
#endif // wxUSE_FILE
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
#if wxUSE_FILE
    pMenu->Append(Menu_Save,  _("&Save..."), _("Save log contents to file"));
#endif // wxUSE_FILE
    pMenu->Append(Menu_Clear, _("C&lear"), _("Clear the log contents"));
    pMenu->AppendSeparator();
    pMenu->Append(Menu_Close, _("&Close"), _("Close this window"));
    pMenuBar->Append(pMenu, _("&Log"));
    SetMenuBar(pMenuBar);

#if wxUSE_STATUSBAR
    // status bar for menu prompts
    CreateStatusBar();
#endif // wxUSE_STATUSBAR

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

#if wxUSE_FILE
void wxLogFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
    // get the file name
    // -----------------
    const wxChar *szFileName = wxSaveFileSelector(T("log"), T("txt"), T("log.txt"));
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
#endif // wxUSE_FILE

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
                if ( !wxIsEmpty(szString) )
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

void wxLogWindow::DoLogString(const wxChar *szString, time_t WXUNUSED(t))
{
    // put the text into our window
    wxTextCtrl *pText = m_pLogFrame->TextCtrl();

    // remove selection (WriteText is in fact ReplaceSelection)
#ifdef __WXMSW__
    long nLen = pText->GetLastPosition();
    pText->SetSelection(nLen, nLen);
#endif // Windows

    wxString msg;
    TimeStamp(&msg);
    msg << szString << T('\n');

    pText->AppendText(msg);

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

