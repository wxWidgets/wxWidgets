///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/notifmsgcmn.cpp
// Purpose:     wxNotificationMessageBase implementation
// Author:      Tobias Taschner
// Created:     2015-08-04
// Copyright:   (c) 2015 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////


// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_NOTIFICATION_MESSAGE

#include "wx/notifmsg.h"

#include "wx/private/notifmsg.h"

#ifndef wxHAS_NATIVE_NOTIFICATION_MESSAGE
#include "wx/generic/private/notifmsg.h"
#endif

wxDEFINE_EVENT( wxEVT_NOTIFICATION_MESSAGE_CLICK, wxCommandEvent );
wxDEFINE_EVENT( wxEVT_NOTIFICATION_MESSAGE_DISMISSED, wxCommandEvent );
wxDEFINE_EVENT( wxEVT_NOTIFICATION_MESSAGE_ACTION, wxCommandEvent );

// ----------------------------------------------------------------------------
// wxNotificationMessageBase
// ----------------------------------------------------------------------------

wxNotificationMessageBase::~wxNotificationMessageBase()
{
    delete m_impl;
}

bool wxNotificationMessageBase::Show(int timeout)
{
    return m_impl->Show(timeout);
}

bool wxNotificationMessageBase::Close()
{
    return m_impl->Close();
}

void wxNotificationMessageBase::SetTitle(const wxString& title)
{
    m_impl->SetTitle(title);
}

void wxNotificationMessageBase::SetMessage(const wxString& message)
{
    m_impl->SetMessage(message);
}

void wxNotificationMessageBase::SetParent(wxWindow *parent)
{
    m_impl->SetParent(parent);
}

void wxNotificationMessageBase::SetFlags(int flags)
{
    wxASSERT_MSG(flags == wxICON_INFORMATION ||
        flags == wxICON_WARNING || flags == wxICON_ERROR ||
        flags == 0,
        "Invalid icon flags specified");

    m_impl->SetFlags(flags);
}

void wxNotificationMessageBase::SetIcon(const wxIcon& icon)
{
    m_impl->SetIcon(icon);
}

bool wxNotificationMessageBase::AddAction(wxWindowID actionid, const wxString &label)
{
    return m_impl->AddAction(actionid, label);
}

//
// wxNotificationMessage
//

#ifndef wxHAS_NATIVE_NOTIFICATION_MESSAGE

void wxNotificationMessage::Init()
{
    // This is only used when we derive from wxGenericNotificationMessage,
    // which already initializes its m_impl correctly, so there is nothing to
    // do here (but we still need to have this method for consistency with the
    // native implementations).
}

#endif

#endif // wxUSE_NOTIFICATION_MESSAGE
