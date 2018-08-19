/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/progdlgg.cpp
// Purpose:     wxGenericProgressDialog class
// Author:      Karsten Ballueder
// Modified by:
// Created:     09.05.1999
// Copyright:   (c) Karsten Ballueder
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

#if wxUSE_PROGRESSDLG

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/frame.h"
    #include "wx/button.h"
    #include "wx/stattext.h"
    #include "wx/sizer.h"
    #include "wx/event.h"
    #include "wx/gauge.h"
    #include "wx/intl.h"
    #include "wx/dcclient.h"
    #include "wx/timer.h"
    #include "wx/settings.h"
    #include "wx/app.h"
#endif

#include "wx/progdlg.h"
#include "wx/evtloop.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#define LAYOUT_MARGIN 8

static const int wxID_SKIP = 32000;  // whatever

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(wxGenericProgressDialog, wxDialog)
    EVT_BUTTON(wxID_CANCEL, wxGenericProgressDialog::OnCancel)
    EVT_BUTTON(wxID_SKIP, wxGenericProgressDialog::OnSkip)

    EVT_CLOSE(wxGenericProgressDialog::OnClose)
wxEND_EVENT_TABLE()

// ============================================================================
// wxGenericProgressDialog implementation
// ============================================================================

wxIMPLEMENT_CLASS(wxProgressDialog, wxDialog)

// ----------------------------------------------------------------------------
// wxGenericProgressDialog creation
// ----------------------------------------------------------------------------

void wxGenericProgressDialog::Init()
{
    // we may disappear at any moment, let the others know about it
    SetExtraStyle(GetExtraStyle() | wxWS_EX_TRANSIENT);

    // Initialize all our members that we always use (even when we don't
    // create a valid window in this class).

    m_pdStyle = 0;
    m_parentTop = NULL;

    m_gauge = NULL;
    m_msg = NULL;
    m_elapsed =
    m_estimated =
    m_remaining = NULL;

    m_state = Uncancelable;
    m_maximum = 0;

    m_timeStart = wxGetCurrentTime();
    m_timeStop = (unsigned long)-1;
    m_break = 0;

    m_skip = false;

    m_btnAbort =
    m_btnSkip = NULL;

    m_display_estimated =
    m_last_timeupdate =
    m_ctdelay = 0;

    m_delay = 3;

    m_winDisabler = NULL;
    m_tempEventLoop = NULL;

    SetWindowStyle(wxDEFAULT_DIALOG_STYLE);
}

wxGenericProgressDialog::wxGenericProgressDialog()
                       : wxDialog()
{
    Init();
}

wxGenericProgressDialog::wxGenericProgressDialog(const wxString& title,
                                                 const wxString& message,
                                                 int maximum,
                                                 wxWindow *parent,
                                                 int style)
                       : wxDialog()
{
    Init();

    Create( title, message, maximum, parent, style );
}

void wxGenericProgressDialog::SetTopParent(wxWindow* parent)
{
    m_parent = parent;

    // Notice that we intentionally do not use GetParentForModalDialog() here
    // as we don't want to disable the main application window if null parent
    // was given (and GetParentForModalDialog() would fall back to it in this
    // case). This allows creating modeless progress dialog, which can be
    // useful even though it is also dangerous because it can easily result in
    // unwanted reentrancies.
    m_parentTop = parent;
}

bool wxGenericProgressDialog::Create( const wxString& title,
                                      const wxString& message,
                                      int maximum,
                                      wxWindow *parent,
                                      int style )
{
    SetTopParent(parent);

    m_pdStyle = style;

    if (!wxDialog::Create(m_parentTop, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, GetWindowStyle()))
        return false;

    SetMaximum(maximum);

    // We need a running event loop in order to update the dialog and be able
    // to process clicks on its buttons, so ensure that there is one running
    // even if this means we have to start it ourselves (this happens most
    // commonly during the program initialization, e.g. for the progress
    // dialogs shown from overridden wxApp::OnInit()).
    EnsureActiveEventLoopExists();

#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
    // we have to remove the "Close" button from the title bar then as it is
    // confusing to have it - it doesn't work anyhow
    //
    // FIXME: should probably have a (extended?) window style for this
    if ( !HasPDFlag(wxPD_CAN_ABORT) )
    {
        EnableCloseButton(false);
    }
#endif // wxMSW

    m_state = HasPDFlag(wxPD_CAN_ABORT) ? Continue : Uncancelable;

    // top-level sizerTop
    wxSizer * const sizerTop = new wxBoxSizer(wxVERTICAL);

    m_msg = new wxStaticText(this, wxID_ANY, message);
    sizerTop->Add(m_msg, 0, wxLEFT | wxRIGHT | wxTOP, 2*LAYOUT_MARGIN);

    int gauge_style = wxGA_HORIZONTAL;
    if ( style & wxPD_SMOOTH )
        gauge_style |= wxGA_SMOOTH;
    gauge_style |= wxGA_PROGRESS;

#ifdef __WXMSW__
    maximum /= m_factor;
#endif

    m_gauge = new wxGauge
                  (
                    this,
                    wxID_ANY,
                    maximum,
                    wxDefaultPosition,
                    // make the progress bar sufficiently long
                    wxSize(wxMin(wxGetClientDisplayRect().width/3, 300), -1),
                    gauge_style
                  );

    sizerTop->Add(m_gauge, 0, wxLEFT | wxRIGHT | wxTOP | wxEXPAND, 2*LAYOUT_MARGIN);
    m_gauge->SetValue(0);

    // create the estimated/remaining/total time zones if requested
    m_elapsed =
    m_estimated =
    m_remaining = NULL;

    // also count how many labels we really have
    size_t nTimeLabels = 0;

    wxSizer * const sizerLabels = new wxFlexGridSizer(2);

    if ( style & wxPD_ELAPSED_TIME )
    {
        nTimeLabels++;

        m_elapsed = CreateLabel(GetElapsedLabel(), sizerLabels);
    }

    if ( style & wxPD_ESTIMATED_TIME )
    {
        nTimeLabels++;

        m_estimated = CreateLabel(GetEstimatedLabel(), sizerLabels);
    }

    if ( style & wxPD_REMAINING_TIME )
    {
        nTimeLabels++;

        m_remaining = CreateLabel(GetRemainingLabel(), sizerLabels);
    }
    sizerTop->Add(sizerLabels, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, LAYOUT_MARGIN);

    wxStdDialogButtonSizer *buttonSizer = wxDialog::CreateStdDialogButtonSizer(0);

    const int borderFlags = wxALL;

    wxSizerFlags sizerFlags
        = wxSizerFlags().Border(borderFlags, LAYOUT_MARGIN);

    if ( HasPDFlag(wxPD_CAN_SKIP) )
    {
        m_btnSkip = new wxButton(this, wxID_SKIP, _("&Skip"));

        buttonSizer->SetNegativeButton(m_btnSkip);
    }

    if ( HasPDFlag(wxPD_CAN_ABORT) )
    {
        m_btnAbort = new wxButton(this, wxID_CANCEL);

        buttonSizer->SetCancelButton(m_btnAbort);
    }

    if ( !HasPDFlag(wxPD_CAN_SKIP | wxPD_CAN_ABORT) )
        buttonSizer->AddSpacer(LAYOUT_MARGIN);

    buttonSizer->Realize();

    sizerTop->Add(buttonSizer, sizerFlags.Expand());

    SetSizerAndFit(sizerTop);

    Centre(wxCENTER_FRAME | wxBOTH);

    DisableOtherWindows();

    Show();
    Enable();

    // this one can be initialized even if the others are unknown for now
    //
    // NB: do it after calling Layout() to keep the labels correctly aligned
    if ( m_elapsed )
    {
        SetTimeLabel(0, m_elapsed);
    }

    Update();
    return true;
}

void wxGenericProgressDialog::UpdateTimeEstimates(int value,
                                                  unsigned long &elapsedTime,
                                                  unsigned long &estimatedTime,
                                                  unsigned long &remainingTime)
{
    unsigned long elapsed = wxGetCurrentTime() - m_timeStart;
    if ( value != 0 && (m_last_timeupdate < elapsed || value == m_maximum) )
    {
        m_last_timeupdate = elapsed;
        unsigned long estimated = m_break +
                (unsigned long)(( (double) (elapsed-m_break) * m_maximum ) / ((double)value)) ;
        if (    estimated > m_display_estimated
                && m_ctdelay >= 0
            )
        {
            ++m_ctdelay;
        }
        else if (    estimated < m_display_estimated
                    && m_ctdelay <= 0
                )
        {
            --m_ctdelay;
        }
        else
        {
            m_ctdelay = 0;
        }
        if (    m_ctdelay >= m_delay          // enough confirmations for a higher value
                || m_ctdelay <= (m_delay*-1)     // enough confirmations for a lower value
                || value == m_maximum            // to stay consistent
                || elapsed > m_display_estimated // to stay consistent
                || ( elapsed > 0 && elapsed < 4 ) // additional updates in the beginning
            )
        {
            m_display_estimated = estimated;
            m_ctdelay = 0;
        }
    }

    if ( value != 0 )
    {
        long display_remaining = m_display_estimated - elapsed;
        if ( display_remaining < 0 )
        {
            display_remaining = 0;
        }

        estimatedTime = m_display_estimated;
        remainingTime = display_remaining;
    }

    elapsedTime = elapsed;
}

// static
wxString wxGenericProgressDialog::GetFormattedTime(unsigned long timeInSec)
{
    wxString timeAsHMS;

    if ( timeInSec == (unsigned long)-1 )
    {
        timeAsHMS = _("Unknown");
    }
    else
    {
        unsigned hours = timeInSec / 3600;
        unsigned minutes = (timeInSec % 3600) / 60;
        unsigned seconds = timeInSec % 60;
        timeAsHMS.Printf("%u:%02u:%02u", hours, minutes, seconds);
    }

    return timeAsHMS;
}

void wxGenericProgressDialog::EnsureActiveEventLoopExists()
{
    if ( !wxEventLoopBase::GetActive() )
    {
        m_tempEventLoop = new wxEventLoop;
        wxEventLoop::SetActive(m_tempEventLoop);
    }
}

wxStaticText *
wxGenericProgressDialog::CreateLabel(const wxString& text, wxSizer *sizer)
{
    wxStaticText *label = new wxStaticText(this, wxID_ANY, text);
    wxStaticText *value = new wxStaticText(this, wxID_ANY, _("unknown"));

    // select placement most native or nice on target GUI
#if defined(__WXMSW__) || defined(__WXMAC__) || defined(__WXGTK20__)
    // value and time centered in one row
    sizer->Add(label, 1, wxALIGN_RIGHT | wxTOP | wxRIGHT, LAYOUT_MARGIN);
    sizer->Add(value, 1, wxALIGN_LEFT | wxTOP, LAYOUT_MARGIN);
#else
    // value and time to the right in one row
    sizer->Add(label);
    sizer->Add(value, 0, wxLEFT, LAYOUT_MARGIN);
#endif

    return value;
}

// ----------------------------------------------------------------------------
// wxGenericProgressDialog operations
// ----------------------------------------------------------------------------

bool
wxGenericProgressDialog::Update(int value, const wxString& newmsg, bool *skip)
{
    if ( !DoBeforeUpdate(skip) )
        return false;

    wxCHECK_MSG( m_gauge, false, "dialog should be fully created" );

#ifdef __WXMSW__
    value /= m_factor;
#endif // __WXMSW__

    wxASSERT_MSG( value <= m_maximum, wxT("invalid progress value") );

    m_gauge->SetValue(value);

    UpdateMessage(newmsg);

    if ( (m_elapsed || m_remaining || m_estimated) && (value != 0) )
    {
        unsigned long elapsed;
        unsigned long display_remaining;

        UpdateTimeEstimates( value,
                             elapsed,
                             m_display_estimated,
                             display_remaining );

        SetTimeLabel(elapsed, m_elapsed);
        SetTimeLabel(m_display_estimated, m_estimated);
        SetTimeLabel(display_remaining, m_remaining);
    }

    if ( value == m_maximum )
    {
        if ( m_state == Finished )
        {
            // ignore multiple calls to Update(m_maximum): it may sometimes be
            // troublesome to ensure that Update() is not called twice with the
            // same value (e.g. because of the rounding errors) and if we don't
            // return now we're going to generate asserts below
            return true;
        }

        // so that we return true below and that out [Cancel] handler knew what
        // to do
        m_state = Finished;
        if( !HasPDFlag(wxPD_AUTO_HIDE) )
        {
            EnableClose();
            DisableSkip();
#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
            EnableCloseButton();
#endif // __WXMSW__

            if ( newmsg.empty() )
            {
                // also provide the finishing message if the application didn't
                m_msg->SetLabel(_("Done."));
            }

            // allow the window to repaint:
            // NOTE: since we yield only for UI events with this call, there
            //       should be no side-effects
            wxEventLoopBase::GetActive()->YieldFor(wxEVT_CATEGORY_UI);

            // NOTE: this call results in a new event loop being created
            //       and to a call to ProcessPendingEvents() (which may generate
            //       unwanted re-entrancies).
            (void)ShowModal();
        }
        else // auto hide
        {
            // reenable other windows before hiding this one because otherwise
            // Windows wouldn't give the focus back to the window which had
            // been previously focused because it would still be disabled
            ReenableOtherWindows();

            Hide();
        }
    }
    else // not at maximum yet
    {
        DoAfterUpdate();
    }

    // update the display in case yielding above didn't do it
    Update();

    return m_state != Canceled;
}

bool wxGenericProgressDialog::Pulse(const wxString& newmsg, bool *skip)
{
    if ( !DoBeforeUpdate(skip) )
        return false;

    wxCHECK_MSG( m_gauge, false, "dialog should be fully created" );

    // show a bit of progress
    m_gauge->Pulse();

    UpdateMessage(newmsg);

    if (m_elapsed || m_remaining || m_estimated)
    {
        unsigned long elapsed = wxGetCurrentTime() - m_timeStart;

        SetTimeLabel(elapsed, m_elapsed);
        SetTimeLabel((unsigned long)-1, m_estimated);
        SetTimeLabel((unsigned long)-1, m_remaining);
    }

    DoAfterUpdate();

    return m_state != Canceled;
}

bool wxGenericProgressDialog::DoBeforeUpdate(bool *skip)
{
    // we have to yield because not only we want to update the display but
    // also to process the clicks on the cancel and skip buttons
    // NOTE: using YieldFor() this call shouldn't give re-entrancy problems
    //       for event handlers not interested to UI/user-input events.
    wxEventLoopBase::GetActive()->YieldFor(wxEVT_CATEGORY_UI|wxEVT_CATEGORY_USER_INPUT);

    Update();

    if ( m_skip && skip && !*skip )
    {
        *skip = true;
        m_skip = false;
        EnableSkip();
    }

    return m_state != Canceled;
}

void wxGenericProgressDialog::DoAfterUpdate()
{
    // allow the window to repaint:
    // NOTE: since we yield only for UI events with this call, there
    //       should be no side-effects
    wxEventLoopBase::GetActive()->YieldFor(wxEVT_CATEGORY_UI);
}

void wxGenericProgressDialog::Resume()
{
    m_state = Continue;
    m_ctdelay = m_delay; // force an update of the elapsed/estimated/remaining time
    m_break += wxGetCurrentTime()-m_timeStop;

    EnableAbort();
    EnableSkip();
    m_skip = false;
}

bool wxGenericProgressDialog::Show( bool show )
{
    // reenable other windows before hiding this one because otherwise
    // Windows wouldn't give the focus back to the window which had
    // been previously focused because it would still be disabled
    if(!show)
        ReenableOtherWindows();

    return wxDialog::Show(show);
}

int wxGenericProgressDialog::GetValue() const
{
    wxCHECK_MSG( m_gauge, -1, "dialog should be fully created" );

    return m_gauge->GetValue();
}

int wxGenericProgressDialog::GetRange() const
{
    return m_maximum;
}

wxString wxGenericProgressDialog::GetMessage() const
{
    return m_msg->GetLabel();
}

void wxGenericProgressDialog::SetRange(int maximum)
{
    wxCHECK_RET( m_gauge, "dialog should be fully created" );

    wxCHECK_RET( maximum > 0, "Invalid range" );

    m_gauge->SetRange(maximum);

    SetMaximum(maximum);
}

void wxGenericProgressDialog::SetMaximum(int maximum)
{
    m_maximum = maximum;

#if defined(__WXMSW__)
    // we can't have values > 65,536 in the progress control under Windows, so
    // scale everything down
    m_factor = m_maximum / 65536 + 1;
#endif // __WXMSW__
}


bool wxGenericProgressDialog::WasCancelled() const
{
    return HasPDFlag(wxPD_CAN_ABORT) && m_state == Canceled;
}

bool wxGenericProgressDialog::WasSkipped() const
{
    return HasPDFlag(wxPD_CAN_SKIP) && m_skip;
}

// static
void wxGenericProgressDialog::SetTimeLabel(unsigned long val,
                                           wxStaticText *label)
{
    if ( label )
    {
        wxString s;

        if (val != (unsigned long)-1)
        {
            s = GetFormattedTime(val);
        }
        else
        {
            s = _("Unknown");
        }

        if ( s != label->GetLabel() )
            label->SetLabel(s);
    }
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void wxGenericProgressDialog::OnCancel(wxCommandEvent& event)
{
    if ( m_state == Finished )
    {
        // this means that the count down is already finished and we're being
        // shown as a modal dialog - so just let the default handler do the job
        event.Skip();
    }
    else
    {
        // request to cancel was received, the next time Update() is called we
        // will handle it
        m_state = Canceled;

        // update the buttons state immediately so that the user knows that the
        // request has been noticed
        DisableAbort();
        DisableSkip();

        // save the time when the dialog was stopped
        m_timeStop = wxGetCurrentTime();
    }
}

void wxGenericProgressDialog::OnSkip(wxCommandEvent& WXUNUSED(event))
{
    DisableSkip();
    m_skip = true;
}

void wxGenericProgressDialog::OnClose(wxCloseEvent& event)
{
    if ( m_state == Uncancelable )
    {
        // can't close this dialog
        event.Veto();
    }
    else if ( m_state == Finished )
    {
        // let the default handler close the window as we already terminated
        event.Skip();
    }
    else
    {
        // next Update() will notice it
        m_state = Canceled;
        DisableAbort();
        DisableSkip();

        m_timeStop = wxGetCurrentTime();
    }
}

// ----------------------------------------------------------------------------
// destruction
// ----------------------------------------------------------------------------

wxGenericProgressDialog::~wxGenericProgressDialog()
{
    // normally this should have been already done, but just in case
    ReenableOtherWindows();

    if ( m_tempEventLoop )
    {
        // If another event loop has been installed as active during the life
        // time of this object, we shouldn't deactivate it, but we also can't
        // delete our m_tempEventLoop in this case because it risks leaving the
        // new event loop with a dangling pointer, which it will set back as
        // the active loop when it exits, resulting in a crash. So we have no
        // choice but to just leak this pointer then, which is, of course, bad
        // and usually easily avoidable by just destroying the progress dialog
        // sooner, so warn the programmer about it.
        wxCHECK_RET
        (
            wxEventLoopBase::GetActive() == m_tempEventLoop,
            "current event loop must not be changed during "
            "wxGenericProgressDialog lifetime"
        );

        wxEventLoopBase::SetActive(NULL);
        delete m_tempEventLoop;
    }
}

void wxGenericProgressDialog::DisableOtherWindows()
{
    if ( HasPDFlag(wxPD_APP_MODAL) )
    {
        m_winDisabler = new wxWindowDisabler(this);
    }
    else
    {
        if ( m_parentTop )
            m_parentTop->Disable();
        m_winDisabler = NULL;
    }
}

void wxGenericProgressDialog::ReenableOtherWindows()
{
    if ( HasPDFlag(wxPD_APP_MODAL) )
    {
        wxDELETE(m_winDisabler);
    }
    else
    {
        if ( m_parentTop )
            m_parentTop->Enable();
    }
}

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

void wxGenericProgressDialog::EnableSkip(bool enable)
{
    if ( HasPDFlag(wxPD_CAN_SKIP) )
    {
        if(m_btnSkip)
            m_btnSkip->Enable(enable);
    }
}

void wxGenericProgressDialog::EnableAbort(bool enable)
{
    if( HasPDFlag(wxPD_CAN_ABORT) )
    {
        if(m_btnAbort)
            m_btnAbort->Enable(enable);
    }
}

void wxGenericProgressDialog::EnableClose()
{
    if(HasPDFlag(wxPD_CAN_ABORT))
    {
        if(m_btnAbort)
        {
            m_btnAbort->Enable();
            m_btnAbort->SetLabel(_("Close"));
        }
    }
}

void wxGenericProgressDialog::UpdateMessage(const wxString &newmsg)
{
    if ( !newmsg.empty() && newmsg != m_msg->GetLabel() )
    {
        const wxSize sizeOld = m_msg->GetSize();

        m_msg->SetLabel(newmsg);

        if ( m_msg->GetSize().x > sizeOld.x )
        {
            // Resize the dialog to fit its new, longer contents instead of
            // just truncating it.
            Fit();
        }

        // allow the window to repaint:
        // NOTE: since we yield only for UI events with this call, there
        //       should be no side-effects
        wxEventLoopBase::GetActive()->YieldFor(wxEVT_CATEGORY_UI);
    }
}

#endif // wxUSE_PROGRESSDLG
