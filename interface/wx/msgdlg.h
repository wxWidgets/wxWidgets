/////////////////////////////////////////////////////////////////////////////
// Name:        msgdlg.h
// Purpose:     interface of wxMessageDialog
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxMessageDialog

    This class represents a dialog that shows a single or multi-line message,
    with a choice of OK, Yes, No and Cancel buttons.

    @library{wxcore}
    @category{cmndlg}

    @see @ref overview_wxmessagedialogoverview "wxMessageDialog overview"
*/
class wxMessageDialog : public wxDialog
{
public:
    /**
        Constructor specifying the message box properties.

        Use ShowModal() to show the dialog.

        @a style may be a bit list of the following identifiers:

        @beginStyleTable
        @style{wxOK}
            Puts an Ok button in the message box. May be combined with @c
            wxCANCEL.
        @style{wxCANCEL}
            Puts a Cancel button in the message box. Must be combined with
            either @c wxOK or @c wxYES_NO.
        @style{wxYES_NO}
            Puts Yes and No buttons in the message box. May be combined with
            @c wxCANCEL.
        @style{wxNO_DEFAULT}
            Makes the "No" button default, can only be used with @c wxYES_NO.
        @style{wxYES_DEFAULT}
            Makes the "Yes" button default, this is the default behaviour and
            this flag exists solely for symmetry with @c wxNO_DEFAULT.
        @style{wxICON_EXCLAMATION}
            Displays an exclamation mark symbol.
        @style{wxICON_ERROR}
            Displays an error symbol.
        @style{wxICON_HAND}
            Displays an error symbol, this is a MSW-inspired synonym for @c
            wxICON_ERROR.
        @style{wxICON_QUESTION}
            Displays a question mark symbol. This icon is automatically used
            with @c wxYES_NO so it's usually unnecessary to specify it
            explicitly.
        @style{wxICON_INFORMATION}
            Displays an information symbol. This icon is used by default if @c
            wxYES_NO is not given so it is usually unnecessary to specify it
            explicitly.
        @style{wxSTAY_ON_TOP}
            Makes the message box stay on top of all other windows (currently
            implemented only under MSW).
        @endStyleTable

        @param parent
            Parent window.
        @param message
            Message to show in the dialog.
        @param caption
            The dialog title.
        @param style
            Combination of style flags described above.
        @param pos
            Dialog position (ignored under MSW).
    */
    wxMessageDialog(wxWindow* parent, const wxString& message,
                    const wxString& caption = "Message box",
                    long style = wxOK | wxCANCEL,
                    const wxPoint& pos = wxDefaultPosition);

    /**
        Sets the extended message for the dialog: this message is usually an
        extension of the short message specified in the constructor or set with
        SetMessage().

        If it is set, the main message appears highlighted -- if supported --
        and this message appears beneath it in normal font. On the platforms
        which don't support extended messages, it is simply appended to the
        normal message with a new line separating them.
    */
    void SetExtendedMessage(const wxString extendedMessage);

    /**
        Sets the message shown by the dialog.
    */
    void SetMessage(const wxString msg);

    /**
        Overrides the default labels of the OK and Cancel buttons.

        Please see the remarks in SetYesNoLabels() documentation.
    */
    bool SetOKCancelLabels(const wxString ok, const wxString cancel);

    /**
        Overrides the default label of the OK button.

        Please see the remarks in SetYesNoLabels() documentation.
    */
    bool SetOKLabel(const wxString ok);

    /**
        Overrides the default labels of the Yes, No and Cancel buttons.

        Please see the remarks in SetYesNoLabels() documentation.
    */
    bool SetYesNoCancelLabels(const wxString yes, const wxString no,
                              const wxString cancel);

    /**
        Overrides the default labels of the Yes and No buttons.

        Notice that this function is not currently available on all platforms,
        so it may return @false to indicate that the labels couldn't be
        changed. If it returns @true (currently only under wxMac), the labels
        were set successfully. Typically, if the function was used
        successfully, the main dialog message may need to be changed, e.g.:
        @code
        wxMessageDialog dlg(...);
        if ( dlg.SetYesNoLabels(_("&Quit"), _("&Don't quit")) )
            dlg.SetMessage(_("What do you want to do?"));
        else // buttons have standard "Yes"/"No" values, so rephrase the question
            dlg.SetMessage(_("Do you really want to quit?"));
        @endcode
    */
    bool SetYesNoLabels(const wxString yes, const wxString no);

    /**
        Shows the dialog, returning one of wxID_OK, wxID_CANCEL, wxID_YES,
        wxID_NO.

        Notice that this method returns the identifier of the button which was
        clicked unlike wxMessageBox() function.
    */
    int ShowModal();
};



// ============================================================================
// Global functions/macros
// ============================================================================

/** @ingroup group_funcmacro_dialog */
//@{

/**
    Show a general purpose message dialog.

    This is a convenient function which is usually used instead of using
    wxMessageDialog directly. Notice however that some of the features, such as
    extended text and custom labels for the message box buttons, are not
    provided by this function but only by wxMessageDialog.

    The return value is one of: @c wxYES, @c wxNO, @c wxCANCEL or @c wxOK
    (notice that this return value is @b different from the return value of
    wxMessageDialog::ShowModal()).

    For example:
    @code
    int answer = wxMessageBox("Quit program?", "Confirm",
                              wxYES_NO | wxCANCEL, main_frame);
    if (answer == wxYES)
        main_frame->Close();
    @endcode

    @a message may contain newline characters, in which case the message will
    be split into separate lines, to cater for large messages.

    @param message
        Message to show in the dialog.
    @param caption
        The dialog title.
    @param parent
        Parent window.
    @param style
        Combination of style flags described in wxMessageDialog documentation.
    @param x
        Horizontal dialog position (ignored under MSW). Use @c wxDefaultCoord
        for @a x and @a y to let the system position the window.
    @param y
        Vertical dialog position (ignored under MSW).
    @header{wx/msgdlg.h}
*/
int wxMessageBox(const wxString& message,
                 const wxString& caption = "Message",
                 int style = wxOK,
                 wxWindow* parent = NULL,
                 int x = wxDefaultCoord,
                 int y = wxDefaultCoord);

//@}

