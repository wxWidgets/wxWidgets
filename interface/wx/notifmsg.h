/////////////////////////////////////////////////////////////////////////////
// Name:        notifmsg.h
// Purpose:     interface of wxNotificationMessage
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxNotificationMessage

    This class allows to show the user a message non intrusively. Currently it is
    implemented natively only for the Maemo platform and uses (non-modal) dialogs
    for the display of the notifications under the other platforms but it will be
    extended to use the platform-specific notifications in the other ports in the
    future.

    Notice that this class is not a window and so doesn't derive from wxWindow.

    @library{wxbase}
    @category{FIXME}
*/
class wxNotificationMessage : public wxEvtHandler
{
public:
    //@{
    /**
        , @b wxWindow*@e parent = @NULL, @b int@e flags = @c wxICON_INFORMATION)
        Create a notification object with the given attributes.
        See SetTitle(),
        SetMessage(),
        SetParent() and
        SetFlags() for the description of the
        corresponding parameters.
    */
    wxNotificationMessage();
    wxNotificationMessage(const wxString& title);
    //@}

    /**
        Hides the notification.
        Returns @true if it was hidden or @false if it couldn't be done (e.g. on
        some
        systems automatically hidden notifications can't be hidden manually)
    */
    virtual bool Close();

    /**
        This parameter can be currently used to specify the icon to show in the
        notification. Valid values are @c wxICON_INFORMATION,
        @c wxICON_WARNING and @c wxICON_ERROR (notice that
        @c wxICON_QUESTION is not allowed here).
        Some implementations of this class may not support the icons.
    */
    void SetFlags(int flags);

    /**
        Set the main text of the notification. This should be a more detailed
        description than the title but still limited to reasonable length (not more
        than 256 characters).
    */
    void SetMessage(const wxString& message);

    /**
        Set the parent for this notification: the notification will be associated with
        the top level parent of this window or, if this method is not called, with the
        main application window by default
    */
    void SetParent(wxWindow* parent);

    /**
        Set the title, it must be a concise string (not more than 64 characters), use
        SetMessage() to give the user more
        details.
    */
    void SetTitle(const wxString& title);

    /**
        Show the notification to the user and hides it after timeout seconds
        pass. Special values @c Timeout_Auto and @c Timeout_Never can be
        used here, notice that you shouldn't rely on @a timeout being exactly
        respected because the current platform may only support default timeout value
        and also because the user may be able to close the notification.
        Returns @false if an error occurred.
    */
    virtual bool Show(int timeout = Timeout_Auto);
};

