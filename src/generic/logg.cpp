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
    #include "wx/button.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/menu.h"
    #include "wx/frame.h"
    #include "wx/filedlg.h"
    #include "wx/msgdlg.h"
    #include "wx/textctrl.h"
    #include "wx/sizer.h"
    #include "wx/statbmp.h"
    #include "wx/button.h"
#endif // WX_PRECOMP

#include "wx/file.h"
#include "wx/textfile.h"
#include "wx/statline.h"

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
    #include "wx/imaglist.h"
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
#if wxUSE_FILE
    void OnSave(wxCommandEvent& event);
#endif // wxUSE_FILE
    void OnListSelect(wxListEvent& event);

private:
    // create controls needed for the details display
    void CreateDetailsControls();

    // the data for the listctrl
    wxArrayString m_messages;
    wxArrayInt m_severity;
    wxArrayLong m_times;

    // the "toggle" button and its state
    wxButton *m_btnDetails;
    bool      m_showingDetails;

    // the controls which are not shown initially (but only when details
    // button is pressed)
    wxListCtrl *m_listctrl;
#if wxUSE_STATLINE
    wxStaticLine *m_statline;
#endif // wxUSE_STATLINE
#if wxUSE_FILE
    wxButton *m_btnSave;
#endif // wxUSE_FILE

    // the translated "Details" string
    static wxString ms_details;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxLogDialog, wxDialog)
    EVT_BUTTON(wxID_CANCEL, wxLogDialog::OnOk)
    EVT_BUTTON(wxID_MORE,   wxLogDialog::OnDetails)
#if wxUSE_FILE
    EVT_BUTTON(wxID_SAVE,   wxLogDialog::OnSave)
#endif // wxUSE_FILE
    EVT_LIST_ITEM_SELECTED(-1, wxLogDialog::OnListSelect)
END_EVENT_TABLE()

#endif // wxUSE_LOG_DIALOG

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

#if wxUSE_FILE

// pass an uninitialized file object, the function will ask the user for the
// filename and try to open it, returns TRUE on success (file was opened),
// FALSE if file couldn't be opened/created and -1 if the file selection
// dialog was cancelled
static int OpenLogFile(wxFile& file, wxString *filename = NULL);

#endif // wxUSE_FILE

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
#ifdef __WXMAC__
    msg << szString << wxT('\r');
#else
    msg << szString << wxT('\n');
#endif

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
    m_bErrors =
    m_bWarnings =
    m_bHasMessages = FALSE;

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

    wxString appName = wxTheApp->GetAppName();
    if ( !!appName )
        appName[0u] = wxToupper(appName[0u]);

    long style;
    wxString titleFormat;
    if ( m_bErrors ) {
        titleFormat = _("%s Error");
        style = wxICON_STOP;
    }
    else if ( m_bWarnings ) {
        titleFormat = _("%s Warning");
        style = wxICON_EXCLAMATION;
    }
    else {
        titleFormat = _("%s Information");
        style = wxICON_INFORMATION;
    }
    
    wxString title;
    title.Printf(titleFormat, appName.c_str());

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
                    str << szString << wxT("\r\n");
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

    // common part of OnClose() and OnCloseWindow()
    void DoClose();

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

void wxLogFrame::DoClose()
{
    if ( m_log->OnFrameClose(this) )
    {
        // instead of closing just hide the window to be able to Show() it
        // later
        Show(FALSE);
    }
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
    wxString filename;
    wxFile file;
    int rc = OpenLogFile(file, &filename);
    if ( rc == -1 )
    {
        // cancelled
        return;
    }

    bool bOk = rc != 0;

    // retrieve text and save it
    // -------------------------
    int nLines = m_pTextCtrl->GetNumberOfLines();
    for ( int nLine = 0; bOk && nLine < nLines; nLine++ ) {
        bOk = file.Write(m_pTextCtrl->GetLineText(nLine) +
                         wxTextFile::GetEOL());
    }

    if ( bOk )
        bOk = file.Close();

    if ( !bOk ) {
        wxLogError(_("Can't save log contents to file."));
    }
    else {
        wxLogStatus(this, _("Log saved to the file '%s'."), filename.c_str());
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

bool wxLogWindow::OnFrameClose(wxFrame * WXUNUSED(frame))
{
    // allow to close
    return TRUE;
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

wxString wxLogDialog::ms_details;

wxLogDialog::wxLogDialog(wxWindow *parent,
                         const wxArrayString& messages,
                         const wxArrayInt& severity,
                         const wxArrayLong& times,
                         const wxString& caption,
                         long style)
           : wxDialog(parent, -1, caption)
{
    if ( ms_details.IsEmpty() )
    {
        // ensure that we won't loop here if wxGetTranslation()
        // happens to pop up a Log message while translating this :-)
        ms_details = wxTRANSLATE("&Details");
        ms_details = wxGetTranslation(ms_details);
    }

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

#if wxUSE_STATLINE
    m_statline = (wxStaticLine *)NULL;
#endif // wxUSE_STATLINE

#if wxUSE_FILE
    m_btnSave = (wxButton *)NULL;
#endif // wxUSE_FILE

    // create the controls which are always shown and layout them: we use
    // sizers even though our window is not resizeable to calculate the size of
    // the dialog properly
    wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *sizerButtons = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *sizerAll = new wxBoxSizer(wxHORIZONTAL);

    // this "Ok" button has wxID_CANCEL id - not very logical, but this allows
    // to close the log dialog with <Esc> which wouldn't work otherwise (as it
    // translates into click on cancel button)
    wxButton *btnOk = new wxButton(this, wxID_CANCEL, _("OK"));
    sizerButtons->Add(btnOk, 0, wxCENTRE|wxBOTTOM, MARGIN/2);
    m_btnDetails = new wxButton(this, wxID_MORE, ms_details + _T(" >>"));
    sizerButtons->Add(m_btnDetails, 0, wxCENTRE|wxTOP, MARGIN/2 - 1);

#ifndef __WIN16__
    wxIcon icon = wxTheApp->GetStdIcon((int)(style & wxICON_MASK));
    sizerAll->Add(new wxStaticBitmap(this, -1, icon), 0, wxCENTRE);
#endif // !Win16

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

void wxLogDialog::CreateDetailsControls()
{
    // create the save button and separator line if possible
#if wxUSE_FILE
    m_btnSave = new wxButton(this, wxID_SAVE, _("&Save..."));
#endif // wxUSE_FILE

#if wxUSE_STATLINE
    m_statline = new wxStaticLine(this, -1);
#endif // wxUSE_STATLINE

    // create the list ctrl now
    m_listctrl = new wxListCtrl(this, -1,
                                wxDefaultPosition, wxDefaultSize,
                                wxSUNKEN_BORDER |
                                wxLC_REPORT |
                                wxLC_NO_HEADER |
                                wxLC_SINGLE_SEL);

    // no need to translate these strings as they're not shown to the
    // user anyhow (we use wxLC_NO_HEADER style)
    m_listctrl->InsertColumn(0, _T("Message"));
    m_listctrl->InsertColumn(1, _T("Time"));

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

    bool loadedIcons = TRUE;

#ifndef __WIN16__
    for ( size_t icon = 0; icon < WXSIZEOF(icons); icon++ )
    {
        wxBitmap bmp = wxTheApp->GetStdIcon(icons[icon]);

        // This may very well fail if there are insufficient
        // colours available. Degrade gracefully.

        if (!bmp.Ok())
            loadedIcons = FALSE;
        else
            imageList->Add(wxImage(bmp).
                        Rescale(ICON_SIZE, ICON_SIZE).
                         ConvertToBitmap());
    }

    m_listctrl->SetImageList(imageList, wxIMAGE_LIST_SMALL);
#endif // !Win16

    // and fill it
    wxString fmt = wxLog::GetTimestamp();
    if ( !fmt )
    {
        // default format
        fmt = _T("%c");
    }

    size_t count = m_messages.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        int image = -1;
#ifndef __WIN16__
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
#endif // !Win16

        if (!loadedIcons)
            image = -1;

        if (image > -1)
            m_listctrl->InsertItem(n, m_messages[n], image);
        else
            m_listctrl->InsertItem(n, m_messages[n]);

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
    int height = wxMax(y*(count + 3), 100);
    m_listctrl->SetSize(-1, height);
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

#if wxUSE_FILE

void wxLogDialog::OnSave(wxCommandEvent& WXUNUSED(event))
{
    wxFile file;
    int rc = OpenLogFile(file);
    if ( rc == -1 )
    {
        // cancelled
        return;
    }

    bool ok = rc != 0;

    wxString fmt = wxLog::GetTimestamp();
    if ( !fmt )
    {
        // default format
        fmt = _T("%c");
    }

    size_t count = m_messages.GetCount();
    for ( size_t n = 0; ok && (n < count); n++ )
    {
        wxString line;
        line << wxDateTime((time_t)m_times[n]).Format(fmt)
             << _T(": ")
             << m_messages[n]
             << wxTextFile::GetEOL();

        ok = file.Write(line);
    }

    if ( ok )
        ok = file.Close();

    if ( !ok )
        wxLogError(_("Can't save log contents to file."));
}

#endif // wxUSE_FILE

void wxLogDialog::OnDetails(wxCommandEvent& WXUNUSED(event))
{
    wxSizer *sizer = GetSizer();

    if ( m_showingDetails )
    {
        m_btnDetails->SetLabel(ms_details + _T(">>"));

        sizer->Remove(m_listctrl);

#if wxUSE_STATLINE
        sizer->Remove(m_statline);
#endif // wxUSE_STATLINE

#if wxUSE_FILE
        sizer->Remove(m_btnSave);
#endif // wxUSE_FILE
    }
    else // show details now
    {
        m_btnDetails->SetLabel(wxString(_T("<< ")) + ms_details);

        if ( !m_listctrl )
        {
            CreateDetailsControls();
        }

#if wxUSE_STATLINE
        sizer->Add(m_statline, 0, wxEXPAND | (wxALL & ~wxTOP), MARGIN);
#endif // wxUSE_STATLINE

        sizer->Add(m_listctrl, 1, wxEXPAND | (wxALL & ~wxTOP), MARGIN);

#if wxUSE_FILE
        sizer->Add(m_btnSave, 0, wxALIGN_RIGHT | (wxALL & ~wxTOP), MARGIN);
#endif // wxUSE_FILE
    }

    m_showingDetails = !m_showingDetails;

    // in any case, our size changed - update
    sizer->SetSizeHints(this);
    sizer->Fit(this);

#ifdef __WXGTK__
    // VS: this is neccessary in order to force frame redraw under
    // WindowMaker or fvwm2 (and probably other broken WMs).
    // Otherwise, detailed list wouldn't be displayed.
    Show(TRUE);
#endif // wxGTK
}

wxLogDialog::~wxLogDialog()
{
    if ( m_listctrl )
    {
        delete m_listctrl->GetImageList(wxIMAGE_LIST_SMALL);
    }
}

#endif // wxUSE_LOG_DIALOG

#if wxUSE_FILE

// pass an uninitialized file object, the function will ask the user for the
// filename and try to open it, returns TRUE on success (file was opened),
// FALSE if file couldn't be opened/created and -1 if the file selection
// dialog was cancelled
static int OpenLogFile(wxFile& file, wxString *pFilename)
{
    // get the file name
    // -----------------
    wxString filename = wxSaveFileSelector(wxT("log"), wxT("txt"), wxT("log.txt"));
    if ( !filename ) {
        // cancelled
        return -1;
    }

    // open file
    // ---------
    bool bOk = FALSE;
    if ( wxFile::Exists(filename) ) {
        bool bAppend = FALSE;
        wxString strMsg;
        strMsg.Printf(_("Append log to file '%s' (choosing [No] will overwrite it)?"),
                      filename.c_str());
        switch ( wxMessageBox(strMsg, _("Question"),
                              wxICON_QUESTION | wxYES_NO | wxCANCEL) ) {
            case wxYES:
                bAppend = TRUE;
                break;

            case wxNO:
                bAppend = FALSE;
                break;

            case wxCANCEL:
                return -1;

            default:
                wxFAIL_MSG(_("invalid message box return value"));
        }

        if ( bAppend ) {
            bOk = file.Open(filename, wxFile::write_append);
        }
        else {
            bOk = file.Create(filename, TRUE /* overwrite */);
        }
    }
    else {
        bOk = file.Create(filename);
    }

    if ( pFilename )
        *pFilename = filename;

    return bOk;
}

#endif // wxUSE_FILE

