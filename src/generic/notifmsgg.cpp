///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/notifmsgg.cpp
// Purpose:     generic implementation of wxNotificationMessage
// Author:      Vadim Zeitlin
// Created:     2007-11-24
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// include this before the test below, wxUSE_GENERIC_NOTIFICATION_MESSAGE is
// defined in this header
#include "wx/notifmsg.h"

#if wxUSE_GENERIC_NOTIFICATION_MESSAGE

#ifndef WX_PRECOMP
    #include "wx/dialog.h"
    #include "wx/timer.h"
#endif //WX_PRECOMP

// ----------------------------------------------------------------------------
// wxNotificationMessageDialog
// ----------------------------------------------------------------------------

class wxNotificationMessageDialog : public wxDialog
{
public:
    wxNotificationMessageDialog(wxWindow *parent,
                                const wxString& text,
                                int timeout);

    void Set(wxWindow *parent,
             const wxString& text,
             int timeout);

    bool IsAutomatic() const { return m_timer.IsRunning(); }
    void SetDeleteOnHide() { m_deleteOnHide = true; }

private:
    void OnClose(wxCloseEvent& event);
    void OnTimer(wxTimerEvent& event);

    // if true, delete the dialog when it should disappear, otherwise just hide
    // it (initially false)
    bool m_deleteOnHide;

    // timer which will hide this dialog when it expires, if it's not running
    // it means we were created without timeout
    wxTimer m_timer;


    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(wxNotificationMessageDialog)
};

// ============================================================================
// wxNotificationMessageDialog implementation
// ============================================================================

BEGIN_EVENT_TABLE(wxNotificationMessageDialog, wxDialog)
    EVT_CLOSE(wxNotificationMessageDialog::OnClose)

    EVT_TIMER(wxID_ANY, wxNotificationMessageDialog::OnTimer)
END_EVENT_TABLE()

wxNotificationMessageDialog::wxNotificationMessageDialog(wxWindow *parent,
                                                         const wxString& text,
                                                         int timeout)
                           : wxDialog(parent, wxID_ANY, _("Notice"),
                                      wxDefaultPosition, wxDefaultSize,
                                      0 /* no caption, no border styles */),
                             m_timer(this)
{
    m_deleteOnHide = false;

    Set(parent, text, timeout);
}

void
wxNotificationMessageDialog::Set(wxWindow * WXUNUSED(parent),
                                 const wxString& text,
                                 int timeout)
{
    wxSizer *sizer = CreateTextSizer(text);
    SetSizerAndFit(sizer);

    if ( timeout != wxNotificationMessage::Timeout_Never )
    {
        // wxTimer uses ms, timeout is in seconds
        m_timer.Start(timeout*1000, true /* one shot only */);
    }
    else if ( m_timer.IsRunning() )
    {
        m_timer.Stop();
    }
}

void wxNotificationMessageDialog::OnClose(wxCloseEvent& event)
{
    if ( m_deleteOnHide )
    {
        // we don't need to keep this dialog alive any more
        Destroy();
    }
    else // don't really close, just hide, as we can be shown again later
    {
        event.Veto();

        Hide();
    }
}

void wxNotificationMessageDialog::OnTimer(wxTimerEvent& WXUNUSED(event))
{
    if ( m_deleteOnHide )
        Destroy();
    else
        Hide();
}

// ============================================================================
// wxNotificationMessage implementation
// ============================================================================

int wxNotificationMessage::ms_timeout = 10;

/* static */ void wxNotificationMessage::SetDefaultTimeout(int timeout)
{
    wxASSERT_MSG( timeout > 0,
                "negative or zero default timeout doesn't make sense" );

    ms_timeout = timeout;
}

void wxNotificationMessage::Init()
{
    m_dialog = NULL;
}

wxNotificationMessage::~wxNotificationMessage()
{
    if ( m_dialog->IsAutomatic() )
    {
        // we want to allow the user to create an automatically hidden
        // notification just by creating a local wxNotificationMessage object
        // and so we shouldn't hide the notification when this object goes out
        // of scope
        m_dialog->SetDeleteOnHide();
    }
    else // manual dialog, hide it immediately
    {
        // OTOH for permanently shown dialogs only the code can hide them and
        // if the object is deleted, we must do it now as it won't be
        // accessible programmatically any more
        delete m_dialog;
    }
}

bool wxNotificationMessage::Show(int timeout)
{
    if ( timeout == Timeout_Auto )
    {
        timeout = GetDefaultTimeout();
    }

    if ( !m_dialog )
    {
        m_dialog = new wxNotificationMessageDialog
                       (
                        GetParent(),
                        GetFullMessage(),
                        timeout
                       );
    }
    else // update the existing dialog
    {
        m_dialog->Set(GetParent(), GetFullMessage(), timeout);
    }

    m_dialog->Show();

    return true;
}

bool wxNotificationMessage::Close()
{
    if ( !m_dialog )
        return false;

    m_dialog->Hide();

    return true;
}

#endif // wxUSE_GENERIC_NOTIFICATION_MESSAGE
