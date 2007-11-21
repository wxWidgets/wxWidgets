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

#ifndef _WX_EMAIL_H_
#define _WX_EMAIL_H_

#include "wx/net/msg.h"

/*
 * wxEmail
 * Miscellaneous email functions
 */

class WXDLLIMPEXP_NETUTILS wxEmail
{
public:
//// Ctor/dtor
    wxEmail() {};

//// Operations

    // Send a message.
    // Specify profile, or leave it to wxWidgets to find the current user name
    static bool Send(wxMailMessage& message, const wxString& profileName = wxEmptyString,
        const wxString& sendMail = wxT("/usr/sbin/sendmail -t"));
    
protected:
};


#endif //_WX_EMAIL_H_

