///////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/private/notifmsg.h
// Purpose:     wxGenericNotificationMessage declarations
// Author:      Tobias Taschner
// Created:     2015-08-04
// Copyright:   (c) 2015 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_PRIVATE_NOTIFMSG_H_
#define _WX_GENERIC_PRIVATE_NOTIFMSG_H_

#include "wx/private/notifmsg.h"

class wxGenericNotificationMessageImpl : public wxNotificationMessageImpl
{
public:
    wxGenericNotificationMessageImpl(wxNotificationMessageBase* notification);

    virtual ~wxGenericNotificationMessageImpl();

    virtual bool Show(int timeout) override;

    virtual bool Close() override;

    virtual void SetTitle(const wxString& title) override;

    virtual void SetMessage(const wxString& message) override;

    virtual void SetParent(wxWindow *parent) override;

    virtual void SetFlags(int flags) override;

    virtual void SetIcon(const wxIcon& icon) override;

    virtual bool AddAction(wxWindowID actionid, const wxString &label) override;

    // get/set the default timeout (used if Timeout_Auto is specified)
    static int GetDefaultTimeout() { return ms_timeout; }
    static void SetDefaultTimeout(int timeout);

private:
    // default timeout
    static int ms_timeout;

    // notification message is represented by a frame in this implementation
    class wxNotificationMessageWindow *m_window;

    wxDECLARE_NO_COPY_CLASS(wxGenericNotificationMessageImpl);
};

#endif // _WX_GENERIC_PRIVATE_NOTIFMSG_H_
