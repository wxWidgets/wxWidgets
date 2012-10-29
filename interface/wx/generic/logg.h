/////////////////////////////////////////////////////////////////////////////
// Name:        logg.h
// Purpose:     interface of GUI wxLog* classes
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


/**
    @class wxLogWindow

    This class represents a background log window: to be precise, it collects all
    log messages in the log frame which it manages but also passes them on to the
    log target which was active at the moment of its creation. This allows you, for
    example, to show all the log messages in a frame but still continue to process
    them normally by showing the standard log dialog.

    @library{wxcore}
    @category{logging}
    @header{wx/log.h}

    @see wxLogTextCtrl
*/
class wxLogWindow : public wxLogInterposer
{
public:
    /**
        Creates the log frame window and starts collecting the messages in it.

        @param pParent
            The parent window for the log frame, may be @NULL
        @param szTitle
            The title for the log frame
        @param show
            @true to show the frame initially (default), otherwise
            Show() must be called later.
        @param passToOld
            @true to process the log messages normally in addition to logging them
            in the log frame (default), @false to only log them in the log frame.
            Note that if no targets were set using wxLog::SetActiveTarget() then
            wxLogWindow simply becomes the active one and messages won't be passed
            to other targets.
    */
    wxLogWindow(wxWindow* pParent, const wxString& szTitle, bool show = true,
                bool passToOld = true);

    /**
        Returns the associated log frame window. This may be used to position or resize
        it but use Show() to show or hide it.
    */
    wxFrame* GetFrame() const;

    /**
        Called if the user closes the window interactively, will not be
        called if it is destroyed for another reason (such as when program
        exits).

        Return @true from here to allow the frame to close, @false to
        prevent this from happening.

        @see OnFrameDelete()
    */
    virtual bool OnFrameClose(wxFrame* frame);

    /**
        Called immediately after the log frame creation allowing for
        any extra initializations.
    */
    virtual void OnFrameCreate(wxFrame* frame);

    /**
        Called right before the log frame is going to be deleted: will
        always be called unlike OnFrameClose().
    */
    virtual void OnFrameDelete(wxFrame* frame);

    /**
        Shows or hides the frame.
    */
    void Show(bool show = true);
};



/**
    @class wxLogGui

    This is the default log target for the GUI wxWidgets applications.

    Please see @ref overview_log_customize for explanation of how to change the
    default log target.

    An object of this class is used by default to show the log messages created
    by using wxLogMessage(), wxLogError() and other logging functions. It
    doesn't display the messages logged by them immediately however but
    accumulates all messages logged during an event handler execution and then
    shows them all at once when its Flush() method is called during the idle
    time processing. This has the important advantage of showing only a single
    dialog to the user even if several messages were logged because of a single
    error as it often happens (e.g. a low level function could log a message
    because it failed to open a file resulting in its caller logging another
    message due to the failure of higher level operation requiring the use of
    this file). If you need to force the display of all previously logged
    messages immediately you can use wxLog::FlushActive() to force the dialog
    display.

    Also notice that if an error message is logged when several informative
    messages had been already logged before, the informative messages are
    discarded on the assumption that they are not useful -- and may be
    confusing and hence harmful -- any more after the error. The warning
    and error messages are never discarded however and any informational
    messages logged after the first error one are also kept (as they may
    contain information about the error recovery). You may override DoLog()
    method to change this behaviour.

    At any rate, it is possible that that several messages were accumulated
    before this class Flush() method is called. If this is the case, Flush()
    uses a custom dialog which shows the last message directly and allows the
    user to view the previously logged ones by expanding the "Details"
    wxCollapsiblePane inside it. This custom dialog also provides the buttons
    for copying the log messages to the clipboard and saving them to a file.

    However if only a single message is present when Flush() is called, just a
    wxMessageBox() is used to show it. This has the advantage of being closer
    to the native behaviour but it doesn't give the user any possibility to
    copy or save the message (except for the recent Windows versions where @c
    Ctrl-C may be pressed in the message box to copy its contents to the
    clipboard) so you may want to override DoShowSingleLogMessage() to
    customize wxLogGui -- the dialogs sample shows how to do this.

    @library{wxcore}
    @category{logging}
    @header{wx/log.h}
*/
class wxLogGui : public wxLog
{
public:
    /**
        Default constructor.
    */
    wxLogGui();

    /**
        Presents the accumulated log messages, if any, to the user.

        This method is called during the idle time and should show any messages
        accumulated in wxLogGui#m_aMessages field to the user.
     */
    virtual void Flush();

protected:
    /**
        Returns the appropriate title for the dialog.

        The title is constructed from wxApp::GetAppDisplayName() and the
        severity string (e.g. "error" or "warning") appropriate for the current
        wxLogGui#m_bErrors and wxLogGui#m_bWarnings values.
     */
    wxString GetTitle() const;

    /**
        Returns wxICON_ERROR, wxICON_WARNING or wxICON_INFORMATION depending on
        the current maximal severity.

        This value is suitable to be used in the style parameter of
        wxMessageBox() function.
     */
    int GetSeverityIcon() const;

    /**
        Forgets all the currently stored messages.

        If you override Flush() (and don't call the base class version), you
        must call this method to avoid messages being logged over and over
        again.
     */
    void Clear();


    /**
        All currently accumulated messages.

        This array may be empty if no messages were logged.

        @see m_aSeverity, m_aTimes
     */
    wxArrayString m_aMessages;

    /**
        The severities of each logged message.

        This array is synchronized with wxLogGui#m_aMessages, i.e. the n-th
        element of this array corresponds to the severity of the n-th message.
        The possible severity values are @c wxLOG_XXX constants, e.g.
        wxLOG_Error, wxLOG_Warning, wxLOG_Message etc.
     */
    wxArrayInt m_aSeverity;

    /**
        The time stamps of each logged message.

        The elements of this array are time_t values corresponding to the time
        when the message was logged.
     */
    wxArrayLong m_aTimes;

    /**
        True if there any error messages.
     */
    bool m_bErrors;

    /**
        True if there any warning messages.

        If both wxLogGui#m_bErrors and this member are false, there are only
        informational messages to be shown.
     */
    bool m_bWarnings;

    /**
        True if there any messages to be shown to the user.

        This variable is used instead of simply checking whether
        wxLogGui#m_aMessages array is empty to allow blocking further calls to
        Flush() while a log dialog is already being shown, even if the messages
        array hasn't been emptied yet.
     */
    bool m_bHasMessages;

private:
    /**
        Method called by Flush() to show a single log message.

        This function can be overridden to show the message in a different way.
        By default a simple wxMessageBox() call is used.

        @param message
            The message to show (it can contain multiple lines).
        @param title
            The suggested title for the dialog showing the message, see
            GetTitle().
        @param style
            One of @c wxICON_XXX constants, see GetSeverityIcon().
     */
    virtual void DoShowSingleLogMessage(const wxString& message,
                                        const wxString& title,
                                        int style);

    /**
        Method called by Flush() to show multiple log messages.

        This function can be overridden to show the messages in a different way.
        By default a special log dialog showing the most recent message and
        allowing the user to expand it to view the previously logged ones is
        used.

        @param messages
            Array of messages to show; it contains more than one element.
        @param severities
            Array of message severities containing @c wxLOG_XXX values.
        @param times
            Array of time_t values indicating when each message was logged.
        @param title
            The suggested title for the dialog showing the message, see
            GetTitle().
        @param style
            One of @c wxICON_XXX constants, see GetSeverityIcon().
     */
    virtual void DoShowMultipleLogMessages(const wxArrayString& messages,
                                           const wxArrayInt& severities,
                                           const wxArrayLong& times,
                                           const wxString& title,
                                           int style);
};



/**
    @class wxLogTextCtrl

    Using these target all the log messages can be redirected to a text control.
    The text control must have been created with @c wxTE_MULTILINE style by the
    caller previously.

    @library{wxcore}
    @category{logging}
    @header{wx/log.h}

    @see wxTextCtrl, wxStreamToTextRedirector
*/
class wxLogTextCtrl : public wxLog
{
public:
    /**
        Constructs a log target which sends all the log messages to the given text
        control. The @a textctrl parameter cannot be @NULL.
    */
    wxLogTextCtrl(wxTextCtrl* pTextCtrl);
};
