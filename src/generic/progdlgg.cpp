/////////////////////////////////////////////////////////////////////////////
// Name:        progdlgg.h
// Purpose:     wxProgressDialog class
// Author:      Karsten Ballüder
// Modified by:
// Created:     09.05.1999
// RCS-ID:      $Id$
// Copyright:   (c) Karsten Ballüder
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "progdlgg.h"
#endif

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
    #include "wx/layout.h"
    #include "wx/event.h"
    #include "wx/gauge.h"
    #include "wx/intl.h"
    #include "wx/settings.h"
    #include "wx/dcclient.h"
    #include "wx/timer.h"
#endif

#include "wx/generic/progdlgg.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#define LAYOUT_X_MARGIN 8
#define LAYOUT_Y_MARGIN 8

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// update the label to show the given time (in seconds)
static void SetTimeLabel(unsigned long val, wxStaticText *label);

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxProgressDialog, wxDialog)
    EVT_BUTTON(wxID_CANCEL, wxProgressDialog::OnCancel)

    EVT_CLOSE(wxProgressDialog::OnClose)
END_EVENT_TABLE()

IMPLEMENT_CLASS(wxProgressDialog, wxDialog)

// ============================================================================
// wxProgressDialog implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxProgressDialog creation
// ----------------------------------------------------------------------------

wxProgressDialog::wxProgressDialog(wxString const &title,
                                   wxString const &message,
                                   int maximum,
                                   wxWindow *parent,
                                   int style)
                : wxDialog(parent, -1, title)
{
    // we may disappear at any moment, let the others know about it
    SetExtraStyle(GetExtraStyle() | wxWS_EX_TRANSIENT);

    m_windowStyle |= style;

    bool hasAbortButton = (style & wxPD_CAN_ABORT) != 0;

#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
    // we have to remove the "Close" button from the title bar then as it is
    // confusing to have it - it doesn't work anyhow
    //
    // FIXME: should probably have a (extended?) window style for this
    if ( !hasAbortButton )
    {
        EnableCloseButton(FALSE);
    }
#endif // wxMSW

    m_state = hasAbortButton ? Continue : Uncancelable;
    m_maximum = maximum;

#if defined(__WXMSW__) || defined(__WXPM__)
    // we can't have values > 65,536 in the progress control under Windows, so
    // scale everything down
    m_factor = m_maximum / 65536 + 1;
    m_maximum /= m_factor;
#endif // __WXMSW__

    m_parentTop = parent;
    while ( m_parentTop && m_parentTop->GetParent() )
    {
        m_parentTop = m_parentTop->GetParent();
    }

    wxLayoutConstraints *c;

    wxClientDC dc(this);
    dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
    long widthText;
    dc.GetTextExtent(message, &widthText, NULL, NULL, NULL, NULL);

    m_msg = new wxStaticText(this, -1, message);
    c = new wxLayoutConstraints;
    c->left.SameAs(this, wxLeft, 2*LAYOUT_X_MARGIN);
    c->top.SameAs(this, wxTop, 2*LAYOUT_Y_MARGIN);
    c->width.AsIs();
    c->height.AsIs();
    m_msg->SetConstraints(c);

    wxSize sizeDlg,
           sizeLabel = m_msg->GetSize();
    sizeDlg.y = 2*LAYOUT_Y_MARGIN + sizeLabel.y;

    wxWindow *lastWindow = m_msg;

    if ( maximum > 0 )
    {
        // note that we can't use wxGA_SMOOTH because it happens to also mean
        // wxDIALOG_MODAL and will cause the dialog to be modal. Have an extra
        // style argument to wxProgressDialog, perhaps.
        m_gauge = new wxGauge(this, -1, m_maximum,
                              wxDefaultPosition, wxDefaultSize,
                              wxGA_HORIZONTAL);

        c = new wxLayoutConstraints;
        c->left.SameAs(this, wxLeft, 2*LAYOUT_X_MARGIN);
        c->top.Below(m_msg, 2*LAYOUT_Y_MARGIN);
        c->right.SameAs(this, wxRight, 2*LAYOUT_X_MARGIN);
        c->height.AsIs();
        m_gauge->SetConstraints(c);
        m_gauge->SetValue(0);
        lastWindow = m_gauge;

        wxSize sizeGauge = m_gauge->GetSize();
        sizeDlg.y += 2*LAYOUT_Y_MARGIN + sizeGauge.y;
    }
    else
        m_gauge = (wxGauge *)NULL;

    // create the estimated/remaining/total time zones if requested
    m_elapsed = m_estimated = m_remaining = (wxStaticText*)NULL;

    // if we are going to have at least one label, remmeber it in this var
    wxStaticText *label = NULL;

    // also count how many labels we really have
    size_t nTimeLabels = 0;

    if ( style & wxPD_ELAPSED_TIME )
    {
        nTimeLabels++;

        label =
        m_elapsed = CreateLabel(_("Elapsed time : "), &lastWindow);
    }

    if ( style & wxPD_ESTIMATED_TIME )
    {
        nTimeLabels++;

        label =
        m_estimated = CreateLabel(_("Estimated time : "), &lastWindow);
    }

    if ( style & wxPD_REMAINING_TIME )
    {
        nTimeLabels++;

        label =
        m_remaining = CreateLabel(_("Remaining time : "), &lastWindow);
    }

    if ( nTimeLabels > 0 )
    {
        // set it to the current time
        m_timeStart = wxGetCurrentTime();
        sizeDlg.y += nTimeLabels * (label->GetSize().y + LAYOUT_Y_MARGIN);
    }

    if ( hasAbortButton )
    {
        m_btnAbort = new wxButton(this, wxID_CANCEL, _("Cancel"));
        c = new wxLayoutConstraints;

        // Windows dialogs usually have buttons in the lower right corner
#if defined(__WXMSW__) || defined(__WXPM__)
        c->right.SameAs(this, wxRight, 2*LAYOUT_X_MARGIN);
#else // !MSW
        c->centreX.SameAs(this, wxCentreX);
#endif // MSW/!MSW
        c->bottom.SameAs(this, wxBottom, 2*LAYOUT_Y_MARGIN);

        c->width.AsIs();
        c->height.AsIs();

        m_btnAbort->SetConstraints(c);

        sizeDlg.y += 2*LAYOUT_Y_MARGIN + wxButton::GetDefaultSize().y;
    }
    else // no "Cancel" button
    {
        m_btnAbort = (wxButton *)NULL;
    }

    SetAutoLayout(TRUE);
    Layout();

    sizeDlg.y += 2*LAYOUT_Y_MARGIN;

    // try to make the dialog not square but rectangular of reasonabel width
    sizeDlg.x = (wxCoord)wxMax(widthText, 4*sizeDlg.y/3);
    sizeDlg.x *= 3;
    sizeDlg.x /= 2;
    SetClientSize(sizeDlg);

    Centre(wxCENTER_FRAME | wxBOTH);

    if ( style & wxPD_APP_MODAL )
    {
        m_winDisabler = new wxWindowDisabler(this);
    }
    else
    {
        if ( m_parentTop )
            m_parentTop->Enable(FALSE);
        m_winDisabler = NULL;
    }

    Show(TRUE);
    Enable(TRUE); // enable this window

    // this one can be initialized even if the others are unknown for now
    //
    // NB: do it after calling Layout() to keep the labels correctly aligned
    if ( m_elapsed )
    {
        SetTimeLabel(0, m_elapsed);
    }

    // Update the display (especially on X, GTK)
    wxYield();

#ifdef __WXMAC__
    MacUpdateImmediately();
#endif
}

wxStaticText *wxProgressDialog::CreateLabel(const wxString& text,
                                            wxWindow **lastWindow)
{
    wxLayoutConstraints *c;

    wxStaticText *label = new wxStaticText(this, -1, _("unknown"));
    c = new wxLayoutConstraints;

    // VZ: I like the labels be centered - if the others don't mind, you may
    //     remove "#ifdef __WXMSW__" and use it for all ports
#if defined(__WXMSW__) || defined(__WXPM__)
    c->left.SameAs(this, wxCentreX, LAYOUT_X_MARGIN);
#else // !MSW
    c->right.SameAs(this, wxRight, 2*LAYOUT_X_MARGIN);
#endif // MSW/!MSW
    c->top.Below(*lastWindow, LAYOUT_Y_MARGIN);
    c->width.AsIs();
    c->height.AsIs();
    label->SetConstraints(c);

    wxStaticText *dummy = new wxStaticText(this, -1, text);
    c = new wxLayoutConstraints;
    c->right.LeftOf(label);
    c->top.SameAs(label, wxTop, 0);
    c->width.AsIs();
    c->height.AsIs();
    dummy->SetConstraints(c);

    *lastWindow = label;

    return label;
}

// ----------------------------------------------------------------------------
// wxProgressDialog operations
// ----------------------------------------------------------------------------

bool
wxProgressDialog::Update(int value, const wxString& newmsg)
{
    wxASSERT_MSG( value == -1 || m_gauge, wxT("cannot update non existent dialog") );

#ifdef __WXMSW__
    value /= m_factor;
#endif // __WXMSW__

    wxASSERT_MSG( value <= m_maximum, wxT("invalid progress value") );

    if ( m_gauge && value < m_maximum )
    {
        m_gauge->SetValue(value + 1);
    }

    if ( !newmsg.IsEmpty() )
    {
        m_msg->SetLabel(newmsg);

        wxYield();
    }

    if ( (m_elapsed || m_remaining || m_estimated) && (value != 0) )
    {
        unsigned long elapsed = wxGetCurrentTime() - m_timeStart;
        unsigned long estimated = (unsigned long)(( (double) elapsed * m_maximum ) / ((double)value)) ;
        unsigned long remaining = estimated - elapsed;

        SetTimeLabel(elapsed, m_elapsed);
        SetTimeLabel(estimated, m_estimated);
        SetTimeLabel(remaining, m_remaining);
    }

    if ( value == m_maximum )
    {
        // so that we return TRUE below and that out [Cancel] handler knew what
        // to do
        m_state = Finished;
        if( !(GetWindowStyle() & wxPD_AUTO_HIDE) )
        {
            if ( m_btnAbort )
            {
                // tell the user what he should do...
                m_btnAbort->SetLabel(_("Close"));
            }
#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
            else // enable the button to give the user a way to close the dlg
            {
                EnableCloseButton(TRUE);
            }
#endif // __WXMSW__

            if ( !newmsg )
            {
                // also provide the finishing message if the application didn't
                m_msg->SetLabel(_("Done."));
            }

            wxYield();

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
    else
    {
        // update the display
        wxYield();
    }

#ifdef __WXMAC__
    MacUpdateImmediately();
#endif

    return m_state != Canceled;
}

void wxProgressDialog::Resume()
{
    m_state = Continue;

    // it may have been disabled by OnCancel(), so enable it back to let the
    // user interrupt us again if needed
    m_btnAbort->Enable();
}

bool wxProgressDialog::Show( bool show )
{
    // reenable other windows before hiding this one because otherwise
    // Windows wouldn't give the focus back to the window which had
    // been previously focused because it would still be disabled
    if(!show)
        ReenableOtherWindows();

    return wxDialog::Show(show);
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void wxProgressDialog::OnCancel(wxCommandEvent& event)
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

        // update the button state immediately so that the user knows that the
        // request has been noticed
        m_btnAbort->Disable();
    }
}

void wxProgressDialog::OnClose(wxCloseEvent& event)
{
    if ( m_state == Uncancelable )
    {
        // can't close this dialog
        event.Veto(TRUE);
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
    }
}

// ----------------------------------------------------------------------------
// destruction
// ----------------------------------------------------------------------------

wxProgressDialog::~wxProgressDialog()
{
    // normally this should have been already done, but just in case
    ReenableOtherWindows();
}

void wxProgressDialog::ReenableOtherWindows()
{
    if ( GetWindowStyle() & wxPD_APP_MODAL )
    {
        delete m_winDisabler;
        m_winDisabler = (wxWindowDisabler *)NULL;
    }
    else
    {
        if ( m_parentTop )
            m_parentTop->Enable(TRUE);
    }
}

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static void SetTimeLabel(unsigned long val, wxStaticText *label)
{
    if ( label )
    {
        wxString s;
        unsigned long hours = val / 3600;
        unsigned long minutes = (val % 3600) / 60;
        unsigned long seconds = val % 60;
        s.Printf(wxT("%lu:%02lu:%02lu"), hours, minutes, seconds);

        if ( s != label->GetLabel() )
            label->SetLabel(s);
    }
}

#endif // wxUSE_PROGRESSDLG
