/////////////////////////////////////////////////////////////////////////////
// Name:        notifmsg.h
// Purpose:     interface of wxNotificationMessage
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxNotificationMessage

    This class allows to show the user a message non intrusively.

    Currently it is implemented natively for Windows and GTK and uses
    (non-modal) dialogs for the display of the notifications under the other
    platforms.

    Notice that this class is not a window and so doesn't derive from wxWindow.

    @library{wxadv}
    @category{misc}
*/
class wxNotificationMessage : public wxEvtHandler
{
public:
    /// Possible values for Show() timeout.
    enum
    {
        Timeout_Auto = -1,  ///< Notification will be hidden automatically.
        Timeout_Never = 0   ///< Notification will never time out.
    };

    /**
        Default constructor, use SetParent(), SetTitle() and SetMessage() to
        initialize the object before showing it.
    */
    wxNotificationMessage();

    /**
        Create a notification object with the given attributes.

        See SetTitle(), SetMessage(), SetParent() and SetFlags() for the
        description of the corresponding parameters.
    */
    wxNotificationMessage(const wxString& title, const wxString& message = wxEmptyString,
                          wxWindow* parent = NULL, int flags = wxICON_INFORMATION);

    /**
        Destructor does not hide the notification.

        The notification can continue to be shown even after the C++ object was
        destroyed, call Close() explicitly if it needs to be hidden.
     */
    virtual ~wxNotificationMessage();

    /**
        Hides the notification.

        Returns @true if it was hidden or @false if it couldn't be done
        (e.g. on some systems automatically hidden notifications can't be
        hidden manually).
    */
    virtual bool Close();

    /**
        This parameter can be currently used to specify the icon to show in the
        notification.

        Valid values are @c wxICON_INFORMATION, @c wxICON_WARNING and
        @c wxICON_ERROR (notice that @c wxICON_QUESTION is not allowed here).
        Some implementations of this class may not support the icons.
    */
    void SetFlags(int flags);

    /**
        Set the main text of the notification.

        This should be a more detailed description than the title but still limited
        to reasonable length (not more than 256 characters).
    */
    void SetMessage(const wxString& message);

    /**
        Set the parent for this notification: the notification will be associated with
        the top level parent of this window or, if this method is not called, with the
        main application window by default.
    */
    void SetParent(wxWindow* parent);

    /**
        Set the title, it must be a concise string (not more than 64 characters), use
        SetMessage() to give the user more details.
    */
    void SetTitle(const wxString& title);

    /**
        Show the notification to the user and hides it after @a timeout seconds
        are elapsed.

        Special values @c Timeout_Auto and @c Timeout_Never can be used here,
        notice that you shouldn't rely on @a timeout being exactly respected
        because the current platform may only support default timeout value
        and also because the user may be able to close the notification.

        @note When using native notifications in wxGTK, the timeout is ignored
            for the notifications with @c wxICON_WARNING or @c wxICON_ERROR
            flags, they always remain shown unless they're explicitly hidden by
            the user, i.e. behave as if Timeout_Auto were given.

        @return @false if an error occurred.
    */
    virtual bool Show(int timeout = Timeout_Auto);
};

