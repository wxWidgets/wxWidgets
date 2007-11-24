///////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/notifmsg.h
// Purpose:     generic implementation of wxNotificationMessage
// Author:      Vadim Zeitlin
// Created:     2007-11-24
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_NOTIFMSG_H_
#define _WX_GENERIC_NOTIFMSG_H_

class wxNotificationMessageDialog;

// ----------------------------------------------------------------------------
// wxNotificationMessage
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxNotificationMessage : public wxNotificationMessageBase
{
public:
    wxNotificationMessage() { Init(); }
    wxNotificationMessage(const wxString& title,
                          const wxString& message = wxString(),
                          wxWindow *parent = NULL)
        : wxNotificationMessageBase(title, message, parent)
    {
        Init();
    }

    virtual ~wxNotificationMessage();


    virtual bool Show(int timeout = Timeout_Auto);
    virtual bool Close();

    // generic implementation-specific methods

    // get/set the default timeout (used if Timeout_Auto is specified)
    static int GetDefaultTimeout() { return ms_timeout; }
    static void SetDefaultTimeout(int timeout);

private:
    void Init();


    // default timeout
    static int ms_timeout;

    // notification message is represented by a modeless dialog in this
    // implementation
    wxNotificationMessageDialog *m_dialog;


    DECLARE_NO_COPY_CLASS(wxNotificationMessage)
};

#endif // _WX_GENERIC_NOTIFMSG_H_

