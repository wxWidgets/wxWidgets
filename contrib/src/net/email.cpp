/////////////////////////////////////////////////////////////////////////////
// Name:        email.h
// Purpose:     wxEmail: portable email client class
// Author:      Julian Smart
// Modified by:
// Created:     2001-08-21
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "email.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/string.h"
#include "wx/net/email.h"

#ifdef __WXMSW__
#include "wx/net/smapi.h"
#endif

// Send a message.
// Specify profile, or leave it to wxWindows to find the current user name

#ifdef __WXMSW__
bool wxEmail::Send(wxMailMessage& message, const wxString& profileName)
{
    wxASSERT (message.m_to.GetCount() > 0) ;

    wxString profile(profileName);
    if (profile.IsEmpty())
        profile = wxGetUserName();

    wxMapiSession session;

    if (!session.MapiInstalled())
        return FALSE;
    if (!session.Logon(profile))
        return FALSE;

    return session.Send(message);
}
#else
#error Send not yet implemented for this platform.
#endif

