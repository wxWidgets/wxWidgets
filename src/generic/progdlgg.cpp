/////////////////////////////////////////////////////////////////////////////
// Name:        progdlgg.h
// Purpose:     wxProgressDialog class
// Author:      Karsten Ballüder
// Modified by:
// Created:     09.05.1999
// RCS-ID:      $Id$
// Copyright:   (c) Karsten Ballüder
// Licence:     wxWindows license
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

#if !USE_SHARED_LIBRARY
    BEGIN_EVENT_TABLE(wxProgressDialog, wxDialog)
       EVT_BUTTON(wxID_CANCEL, wxProgressDialog::OnCancel)
       EVT_CLOSE(wxProgressDialog::OnClose)
    END_EVENT_TABLE()

    IMPLEMENT_CLASS(wxProgressDialog, wxDialog)
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxProgressDialog
// ----------------------------------------------------------------------------

wxProgressDialog::wxProgressDialog(wxString const &title,
                                   wxString const &message,
                                   int maximum,
                                   wxWindow *parent,
                                   int style)
                : wxDialog(parent, -1, title)
{
    bool hasAbortButton = (style & wxPD_CAN_ABORT) != 0;
    m_state = hasAbortButton ? Continue : Uncancelable;
    m_disableParentOnly = (style & wxPD_APP_MODAL) == 0;
    m_AutoHide = (style & wxPD_AUTO_HIDE) != 0;
    m_parent = parent;
    m_maximum = maximum;

    wxLayoutConstraints *c;

    wxClientDC dc(this);
    dc.SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));
    long widthText;
    dc.GetTextExtent(message, &widthText, NULL);

    m_msg = new wxStaticText(this, -1, message);
    c = new wxLayoutConstraints;
    c->left.SameAs(this, wxLeft, 2*LAYOUT_X_MARGIN);
    c->top.SameAs(this, wxTop, 2*LAYOUT_Y_MARGIN);
    c->width.AsIs();
    c->height.AsIs();
    m_msg->SetConstraints(c);

    wxSize sizeDlg, sizeLabel = m_msg->GetSize();
    sizeDlg.y = 2*LAYOUT_Y_MARGIN + sizeLabel.y;

    wxWindow *lastWindow = m_msg;

    if ( maximum > 0 )
    {
        m_gauge = new wxGauge(this, -1, maximum,
                wxDefaultPosition, wxDefaultSize,
                wxGA_HORIZONTAL | wxRAISED_BORDER);
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
    m_elapsed = m_estimated = m_remaining = NULL;

    int nTimeLabels = 0;
    if ( style & wxPD_ELAPSED_TIME )
    {
        nTimeLabels++;

        m_elapsed = CreateLabel(_T("Elapsed time : "), &lastWindow);
    }

    if ( style & wxPD_ESTIMATED_TIME )
    {
        nTimeLabels++;

        m_estimated = CreateLabel(_T("Estimated time : "), &lastWindow);
    }

    if ( style & wxPD_REMAINING_TIME )
    {
        nTimeLabels++;

        m_remaining = CreateLabel(_T("Remaining time : "), &lastWindow);
    }

    if ( nTimeLabels > 0 )
    {
        // set it to the current time
        m_timeStart = wxGetCurrentTime();

        sizeDlg.y += nTimeLabels * (sizeLabel.y + LAYOUT_Y_MARGIN);
    }

    if ( hasAbortButton )
    {
        m_btnAbort = new wxButton(this, wxID_CANCEL, _("Cancel"));
        c = new wxLayoutConstraints;

        // Windows dialogs usually have buttons in the lower right corner
#ifdef __WXMSW__
        c->right.SameAs(this, wxRight, 2*LAYOUT_X_MARGIN);
#else // !MSW
        c->centreX.SameAs(this, wxCentreX);
#endif // MSW/!MSW
        c->bottom.SameAs(this, wxBottom, 2*LAYOUT_Y_MARGIN);

        wxSize sizeBtn = wxButton::GetDefaultSize();
        c->width.Absolute(sizeBtn.x);
        c->height.Absolute(sizeBtn.y);

        m_btnAbort->SetConstraints(c);

        sizeDlg.y += 2*LAYOUT_Y_MARGIN + sizeBtn.y;
    }
    else
        m_btnAbort = (wxButton *)NULL;

    SetAutoLayout(TRUE);
    Layout();

    sizeDlg.y += 2*LAYOUT_Y_MARGIN;

    // try to make the dialog not square but rectangular of reasonabel width
    sizeDlg.x = wxMax(widthText, 4*sizeDlg.y/3);
    sizeDlg.x *= 3;
    sizeDlg.x /= 2;
    SetClientSize(sizeDlg);

    Centre(wxCENTER_FRAME | wxBOTH);

    if (m_disableParentOnly )
    {
        if ( m_parent )
            m_parent->Enable(FALSE);
    }
    else
    {
        wxEnableTopLevelWindows(FALSE);
    }

    Show(TRUE);
    Enable(TRUE); // enable this window
}

wxStaticText *wxProgressDialog::CreateLabel(const wxString& text,
                                            wxWindow **lastWindow)
{
    wxLayoutConstraints *c;

    wxStaticText *label = new wxStaticText(this, -1, _T("unknown"));
    c = new wxLayoutConstraints;

    // VZ: I like the labels be centered - if the others don't mind, you may
    //     remove "#ifdef __WXMSW__" and use it for all ports
#ifdef __WXMSW__
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

bool
wxProgressDialog::Update(int value, const wxString& newmsg)
{
   wxASSERT_MSG( value == -1 || m_gauge, _T("cannot update non existent dialog") );
   wxASSERT_MSG( value <= m_maximum, _T("invalid progress value") );


   if( m_gauge )
      m_gauge->SetValue(value + 1);

   if( !newmsg.IsEmpty() )
      m_msg->SetLabel(newmsg);

   if ( (m_elapsed || m_remaining || m_estimated) && (value != 0) )
   {
      unsigned long elapsed = wxGetCurrentTime() - m_timeStart;
      unsigned long estimated = elapsed * m_maximum / value;
      unsigned long remaining = estimated - elapsed;

      SetTimeLabel(elapsed, m_elapsed);
      SetTimeLabel(estimated, m_estimated);
      SetTimeLabel(remaining, m_remaining);
   }

   if ( (value == m_maximum ) && !m_AutoHide )
   {
       if ( m_btnAbort )
       {
           // tell the user what he should do...
           m_btnAbort->SetLabel(_("Close"));
       }

       if ( !newmsg )
       {
           // also provide the finishing message if the application didn't
           m_msg->SetLabel(_("Done."));
       }

       // so that we return TRUE below and that out [Cancel] handler knew what
       // to do
       m_state = Finished;

       wxYield();

       (void)ShowModal();
   }
   else
   {
       // update the display
       wxYield();
   }

   return m_state != Canceled;
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
    }
}

void wxProgressDialog::OnClose(wxCloseEvent& event)
{
    if ( m_state == Uncancelable )
        event.Veto(TRUE);
    else
        m_state = Canceled;
}

// ----------------------------------------------------------------------------
// destruction
// ----------------------------------------------------------------------------

wxProgressDialog::~wxProgressDialog()
{
    if ( m_disableParentOnly )
    {
        if ( m_parent )
            m_parent->Enable(TRUE);
    }
    else
    {
        wxEnableTopLevelWindows(TRUE);
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
        s.Printf(_T("%lu:%02lu:%02lu"), hours, minutes, seconds);

        if ( s != label->GetLabel() )
            label->SetLabel(s);
    }
}

#endif // wxUSE_PROGRESSDLG
