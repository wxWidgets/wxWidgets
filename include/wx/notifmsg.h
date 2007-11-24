///////////////////////////////////////////////////////////////////////////////
// Name:        wx/notifmsg.h
// Purpose:     class allowing to show notification messages to the user
// Author:      Vadim Zeitlin
// Created:     2007-11-19
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_NOTIFMSG_H_
#define _WX_NOTIFMSG_H_

#include "wx/event.h"

#if wxUSE_NOTIFICATION_MESSAGE

// ----------------------------------------------------------------------------
// wxNotificationMessage: allows to show the user a message non intrusively
// ----------------------------------------------------------------------------

// notice that this class is not a window and so doesn't derive from wxWindow

class WXDLLIMPEXP_ADV wxNotificationMessageBase : public wxEvtHandler
{
public:
    // ctors and initializers
    // ----------------------

    // default ctor, use setters below to initialize it later
    wxNotificationMessageBase() { }

    // create a notification object with the given title and message (the
    // latter may be empty in which case only the title will be shown)
    wxNotificationMessageBase(const wxString& title,
                              const wxString& message = wxString(),
                              wxWindow *parent = NULL)
        : m_title(title),
          m_message(message),
          m_parent(parent)
    {
    }

    // note that the setters must be called before Show()

    // set the title: short string, markup not allowed
    void SetTitle(const wxString& title) { m_title = title; }

    // set the text of the message: this is a longer string than the title and
    // some platforms allow simple HTML-like markup in it
    void SetMessage(const wxString& message) { m_message = message; }

    // set the parent for this notification: we'll be associated with the top
    // level parent of this window or, if this method is not called, with the
    // main application window by default
    void SetParent(wxWindow *parent) { m_parent = parent; }


    // showing and hiding
    // ------------------

    // possible values for Show() timeout
    enum
    {
        Timeout_Auto = -1,  // notification will be hidden automatically
        Timeout_Never = 0   // notification will never time out
    };

    // show the notification to the user and hides it after timeout seconds
    // pass (special values Timeout_Auto and Timeout_Never can be used)
    //
    // returns false if an error occurred
    virtual bool Show(int timeout = Timeout_Auto) = 0;

    // hide the notification, returns true if it was hidden or false if it
    // couldn't be done (e.g. on some systems automatically hidden
    // notifications can't be hidden manually)
    virtual bool Close() = 0;

protected:
    // accessors for the derived classes
    const wxString& GetTitle() const { return m_title; }
    const wxString& GetMessage() const { return m_message; }
    wxWindow *GetParent() const { return m_parent; }

    // return the concatenation of title and message separated by a new line,
    // this is suitable for simple implementation which have no support for
    // separate title and message parts of the notification
    wxString GetFullMessage() const
    {
        wxString text(m_title);
        if ( !m_message.empty() )
        {
            text << "\n\n" << m_message;
        }

        return text;
    }

private:
    wxString m_title,
             m_message;

    wxWindow *m_parent;

    DECLARE_NO_COPY_CLASS(wxNotificationMessageBase)
};

#define wxUSE_GENERIC_NOTIFICATION_MESSAGE 1

#if defined(__WXGTK__) && wxUSE_LIBHILDON
    // we always use the native implementation in Hildon while the other ports
    // will fall back to the generic one even if they have a native version too
    #undef wxUSE_GENERIC_NOTIFICATION_MESSAGE
    #define wxUSE_GENERIC_NOTIFICATION_MESSAGE 0

    #include "wx/gtk/hildon/notifmsg.h"
/*
    TODO: provide support for
        - libnotify (Gnome)
        - Snarl (http://www.fullphat.net/, Win32)
        - Growl (http://growl.info/, OS X)
 */
#else
    #include "wx/generic/notifmsg.h"
#endif

#endif // wxUSE_NOTIFICATION_MESSAGE

#endif // _WX_NOTIFMSG_H_

