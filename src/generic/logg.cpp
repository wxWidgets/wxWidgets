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
    #include "wx/sizer.h"
    #include "wx/statbmp.h"
#endif // WX_PRECOMP

#include "wx/file.h"
#include "wx/textfile.h"

#ifdef  __WXMSW__
  // for OutputDebugString()
  #include  "wx/msw/private.h"
#endif // Windows

// may be defined to 0 for old behavior (using wxMessageBox) - shouldn't be
// changed normally (that's why it's here and not in setup.h)
#define wxUSE_LOG_DIALOG 1

#if wxUSE_LOG_DIALOG
    #include "wx/datetime.h"
    #include "wx/listctrl.h"
    #include "wx/image.h"
#else // !wxUSE_TEXTFILE
    #include "wx/msgdlg.h"
#endif // wxUSE_LOG_DIALOG/!wxUSE_LOG_DIALOG

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

#if wxUSE_LOG_DIALOG

class wxLogDialog : public wxDialog
{
public:
    wxLogDialog(wxWindow *parent,
                const wxArrayString& messages,
                const wxArrayInt& severity,
                const wxArrayLong& timess,
                const wxString& caption,
                long style);
    virtual ~wxLogDialog();

    // event handlers
    void OnOk(wxCommandEvent& event);
    void OnDetails(wxCommandEvent& event);
    void OnListSelect(wxListEvent& event);

private:
    // the data for the listctrl
    wxArrayString m_messages;
    wxArrayInt m_severity;
    wxArrayLong m_times;

    // the "toggle" button and its state
    wxButton *m_btnDetails;
    bool      m_showingDetails;

    // the listctrl (not shown initially)
    wxListCtrl *m_listctrl;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxLogDialog, wxDialog)
    EVT_BUTTON(wxID_OK,     wxLogDialog::OnOk)
    EVT_BUTTON(wxID_MORE,   wxLogDialog::OnDetails)
    EVT_LIST_ITEM_SELECTED(-1, wxLogDialog::OnListSelect)
END_EVENT_TABLE()

#endif // wxUSE_LOG_DIALOG

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
    msg << szString << wxT('\n');

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
    m_aSeverity.Empty();
    m_aTimes.Empty();
}

void wxLogGui::Flush()
{
    if ( !m_bHasMessages )
        return;

    // do it right now to block any new calls to Flush() while we're here
    m_bHasMessages = FALSE;

    wxString title = wxTheApp->GetAppName();
    if ( !!title )
    {
        title[0u] = wxToupper(title[0u]);
        title += _T(' ');
    }

    long style;
    if ( m_bErrors ) {
        title += _("Error");
        style = wxICON_STOP;
    }
    else if ( m_bWarnings ) {
        title += _("Warning");
        style = wxICON_EXCLAMATION;
    }
    else {
        title += _("Information");
        style = wxICON_INFORMATION;
    }

    // this is the best we can do here
    wxWindow *parent = wxTheApp->GetTopWindow();

    size_t nMsgCount = m_aMessages.Count();

    wxString str;
    if ( nMsgCount == 1 )
    {
        str = m_aMessages[0];
    }
    else // more than one message
    {
#if wxUSE_LOG_DIALOG
        wxLogDialog dlg(parent,
                        m_aMessages, m_aSeverity, m_aTimes,
                        title, style);

        // clear the message list before showing the dialog because while it's
        // shown some new messages may appear
        Clear();

        (void)dlg.ShowModal();
#else // !wxUSE_LOG_DIALOG
        // concatenate all strings (but not too many to not overfill the msg box)
        size_t nLines = 0;

        // start from the most recent message
        for ( size_t n = nMsgCount; n > 0; n-- ) {
            // for Windows strings longer than this value are wrapped (NT 4.0)
            const size_t nMsgLineWidth = 156;

            nLines += (m_aMessages[n - 1].Len() + nMsgLineWidth - 1) / nMsgLineWidth;

            if ( nLines > 25 )  // don't put too many lines in message box
                break;

            str << m_aMessages[n - 1] << wxT("\n");
        }
#endif // wxUSE_LOG_DIALOG/!wxUSE_LOG_DIALOG
    }

    // this catches both cases of 1 message with wxUSE_LOG_DIALOG and any
    // situation without it
    if ( !!str )
    {
        wxMessageBox(str, title, wxOK | style, parent);

        // no undisplayed messages whatsoever
        Clear();
    }

    // do it here again
    m_bHasMessages = FALSE;
}

// log all kinds of messages
void wxLogGui::DoLog(wxLogLevel level, const wxChar *szString, time_t t)
{
    switch ( level ) {
        case wxLOG_Info:
            if ( GetVerbose() )
        case wxLOG_Message:
            {
                if ( !m_bErrors ) {
                    m_aMessages.Add(szString);
                    m_aSeverity.Add(wxLOG_Message);
                    m_aTimes.Add((long)t);
                    m_bHasMessages = TRUE;
                }
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

                if ( pFrame && pFrame->GetStatusBar() )
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
                    str << szString << wxT("\n\r");
                    OutputDebugString(str);
                #else
                    // send them to stderr
                    wxFprintf(stderr, wxT("%s: %s\n"),
                              level == wxLOG_Trace ? wxT("Trace")
                                                   : wxT("Debug"),
                              szString);
                    fflush(stderr);
                #endif
            }
            #endif // __WXDEBUG__

            break;

        case wxLOG_FatalError:
            // show this one immediately
            wxMessageBox(szString, _("Fatal error"), wxICON_HAND);
            wxExit();
            break;

        case wxLOG_Error:
            if ( !m_bErrors ) {
#if !wxUSE_LOG_DIALOG
                // discard earlier informational messages if this is the 1st
                // error because they might not make sense any more and showing
                // them in a message box might be confusing
                m_aMessages.Empty();
                m_aSeverity.Empty();
                m_aTimes.Empty();
#endif // wxUSE_LOG_DIALOG
                m_bErrors = TRUE;
            }
            // fall through

        case wxLOG_Warning:
            if ( !m_bErrors ) {
                // for the warning we don't discard the info messages
                m_bWarnings = TRUE;
            }

            m_aMessages.Add(szString);
            m_aSeverity.Add((int)level);
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
    // use standard ids for our commands!
    enum
    {
        Menu_Close = wxID_CLOSE,
        Menu_Save  = wxID_SAVE,
        Menu_Clear = wxID_CLEAR
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
    const wxChar *szFileName = wxSaveFileSelector(wxT("log"), wxT("txt"), wxT("log.txt"));
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
        // bogus cast just to access protected DoLog
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
    msg << szString << wxT('\n');

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

// ----------------------------------------------------------------------------
// wxLogDialog
// ----------------------------------------------------------------------------

#if wxUSE_LOG_DIALOG

static const size_t MARGIN = 10;

wxLogDialog::wxLogDialog(wxWindow *parent,
                         const wxArrayString& messages,
                         const wxArrayInt& severity,
                         const wxArrayLong& times,
                         const wxString& caption,
                         long style)
           : wxDialog(parent, -1, caption )
{
    size_t count = messages.GetCount();
    m_messages.Alloc(count);
    m_severity.Alloc(count);
    m_times.Alloc(count);

    for ( size_t n = 0; n < count; n++ )
    {
        wxString msg = messages[n];
        do
        {
            m_messages.Add(msg.BeforeFirst(_T('\n')));
            msg = msg.AfterFirst(_T('\n'));

            m_severity.Add(severity[n]);
            m_times.Add(times[n]);
        }
        while ( !!msg );
    }

    m_showingDetails = FALSE; // not initially
    m_listctrl = (wxListCtrl *)NULL;

    // create the controls which are always shown and layout them: we use
    // sizers even though our window is not resizeable to calculate the size of
    // the dialog properly
    wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *sizerButtons = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *sizerAll = new wxBoxSizer(wxHORIZONTAL);

    wxButton *btnOk = new wxButton(this, wxID_OK, _T("OK"));
    sizerButtons->Add(btnOk, 0, wxCENTRE|wxBOTTOM, MARGIN/2);
    m_btnDetails = new wxButton(this, wxID_MORE, _T("&Details >>"));
    sizerButtons->Add(m_btnDetails, 0, wxCENTRE|wxTOP, MARGIN/2 - 1);

    wxIcon icon = wxTheApp->GetStdIcon((int)(style & wxICON_MASK));
    sizerAll->Add(new wxStaticBitmap(this, -1, icon), 0, wxCENTRE);
    const wxString& message = messages.Last();
    sizerAll->Add(CreateTextSizer(message), 0, wxCENTRE|wxLEFT|wxRIGHT, MARGIN);
    sizerAll->Add(sizerButtons, 0, wxALIGN_RIGHT|wxLEFT, MARGIN);

    sizerTop->Add(sizerAll, 0, wxCENTRE|wxALL, MARGIN);

    SetAutoLayout(TRUE);
    SetSizer(sizerTop);

    sizerTop->SetSizeHints(this);
    sizerTop->Fit(this);

    btnOk->SetFocus();

    // this can't happen any more as we don't use this dialog in this case
#if 0
    if ( count == 1 )
    {
        // no details... it's easier to disable a button than to change the
        // dialog layout depending on whether we have details or not
        m_btnDetails->Disable();
    }
#endif // 0

    Centre();
}

void wxLogDialog::OnListSelect(wxListEvent& event)
{
    // we can't just disable the control because this looks ugly under Windows
    // (wrong bg colour, no scrolling...), but we still want to disable
    // selecting items - it makes no sense here
    m_listctrl->SetItemState(event.GetIndex(), 0, wxLIST_STATE_SELECTED);
}

void wxLogDialog::OnOk(wxCommandEvent& WXUNUSED(event))
{
    EndModal(wxID_OK);
}

void wxLogDialog::OnDetails(wxCommandEvent& WXUNUSED(event))
{
    wxSizer *sizer = GetSizer();

    if ( m_showingDetails )
    {
        m_btnDetails->SetLabel(_T("&Details >>"));

        sizer->Remove(m_listctrl);
    }
    else // show details now
    {
        m_btnDetails->SetLabel(_T("<< &Details"));

        if ( !m_listctrl )
        {
            // create it now
            m_listctrl = new wxListCtrl(this, -1,
                                        wxDefaultPosition, wxDefaultSize,
                                        wxSUNKEN_BORDER |
                                        wxLC_REPORT |
                                        wxLC_NO_HEADER |
                                        wxLC_SINGLE_SEL);
            m_listctrl->InsertColumn(0, _("Message"));
            m_listctrl->InsertColumn(1, _("Time"));

            // prepare the imagelist
            static const int ICON_SIZE = 16;
            wxImageList *imageList = new wxImageList(ICON_SIZE, ICON_SIZE);

            // order should be the same as in the switch below!
            static const int icons[] =
            {
                wxICON_ERROR,
                wxICON_EXCLAMATION,
                wxICON_INFORMATION
            };

            for ( size_t icon = 0; icon < WXSIZEOF(icons); icon++ )
            {
                wxBitmap bmp = wxTheApp->GetStdIcon(icons[icon]);
                imageList->Add(wxImage(bmp).
                                Rescale(ICON_SIZE, ICON_SIZE).
                                 ConvertToBitmap());
            }

            m_listctrl->SetImageList(imageList, wxIMAGE_LIST_SMALL);

            // and fill it
            wxString fmt = wxLog::GetTimestamp();
            if ( !fmt )
            {
                // default format
                fmt = _T("%X");
            }

            size_t count = m_messages.GetCount();
            for ( size_t n = 0; n < count; n++ )
            {
                int image;
                switch ( m_severity[n] )
                {
                    case wxLOG_Error:
                        image = 0;
                        break;

                    case wxLOG_Warning:
                        image = 1;
                        break;

                    default:
                        image = 2;
                }

                m_listctrl->InsertItem(n, m_messages[n], image);
                m_listctrl->SetItem(n, 1,
                                    wxDateTime((time_t)m_times[n]).Format(fmt));
            }

            // let the columns size themselves
            m_listctrl->SetColumnWidth(0, wxLIST_AUTOSIZE);
            m_listctrl->SetColumnWidth(1, wxLIST_AUTOSIZE);

            // get the approx height of the listctrl
            wxFont font = GetFont();
            if ( !font.Ok() )
                font = *wxSWISS_FONT;

            int y;
            GetTextExtent(_T("H"), (int*)NULL, &y, (int*)NULL, (int*)NULL, &font);
            int height = wxMin(y*(count + 3), 100);
            m_listctrl->SetSize(-1, height);
        }

        sizer->Add(m_listctrl, 1, wxEXPAND|(wxALL & ~wxTOP), MARGIN);
    }

    m_showingDetails = !m_showingDetails;

    // in any case, our size changed - update
    sizer->SetSizeHints(this);
    sizer->Fit(this);
}

wxLogDialog::~wxLogDialog()
{
    if ( m_listctrl )
    {
        delete m_listctrl->GetImageList(wxIMAGE_LIST_SMALL);
    }
}

#endif // wxUSE_LOG_DIALOG
