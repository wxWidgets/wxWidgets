/////////////////////////////////////////////////////////////////////////////
// Name:        msgdlg.h
// Purpose:     interface of wxMessageDialog
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxMessageDialog
    @wxheader{msgdlg.h}

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
        Constructor. Use ShowModal() to show the dialog.

        @param parent
            Parent window.
        @param message
            Message to show on the dialog.
        @param caption
            The dialog caption.
        @param style
            A dialog style (bitlist) containing flags chosen from the following:

            wxOK

            Show an OK button.

            wxCANCEL

            Show a Cancel button.

            wxYES_NO

            Show Yes and No buttons.

            wxYES_DEFAULT

            Used with wxYES_NO, makes Yes button the default - which is the default
        behaviour.

            wxNO_DEFAULT

            Used with wxYES_NO, makes No button the default.

            wxICON_EXCLAMATION

            Shows an exclamation mark icon.

            wxICON_HAND

            Shows an error icon.

            wxICON_ERROR

            Shows an error icon - the same as wxICON_HAND.

            wxICON_QUESTION

            Shows a question mark icon.

            wxICON_INFORMATION

            Shows an information (i) icon.

            wxSTAY_ON_TOP

            The message box stays on top of all other window, even those of the other
        applications (Windows only).
        @param pos
            Dialog position. Not Windows.
    */
    wxMessageDialog(wxWindow* parent, const wxString& message,
                    const wxString& caption = "Message box",
                    long style = wxOK | wxCANCEL,
                    const wxPoint& pos = wxDefaultPosition);

    /**
        Destructor.
    */
    ~wxMessageDialog();

    /**
        Sets the extended message for the dialog: this message is usually an extension
        of the short message specified in the constructor or set with
        SetMessage(). If it is set, the main message
        appears highlighted -- if supported -- and this message appears beneath it in
        normal font. On the platforms which don't support extended messages, it is
        simply appended to the normal message with a new line separating them.
    */
    void SetExtendedMessage(const wxString exMsg);

    /**
        Sets the message shown by the dialog.
    */
    void SetMessage(const wxString msg);

    /**
        Overrides the default labels of the OK and Cancel buttons.
        Please see the remarks in
        SetYesNoLabels() documentation.
    */
    bool SetOKCancelLabels(const wxString ok, const wxString cancel);

    /**
        Overrides the default label of the OK button.
        Please see the remarks in
        SetYesNoLabels() documentation.
    */
    bool SetOKLabel(const wxString ok);

    /**
        Overrides the default labels of the Yes, No and Cancel buttons.
        Please see the remarks in
        SetYesNoLabels() documentation.
    */
    bool SetYesNoCancelLabels(const wxString yes, const wxString no,
                              const wxString cancel);

    /**
        Overrides the default labels of the Yes and No buttons.
        Notice that this function is not currently available on all platforms, so it
        may return @false to indicate that the labels couldn't be changed. If it
        returns @true (currently only under wxMac), the labels were set successfully.
        Typically, if the function was used successfully, the main dialog message may
        need to be changed, e.g.:
    */
    bool SetYesNoLabels(const wxString yes, const wxString no);

    /**
        Shows the dialog, returning one of wxID_OK, wxID_CANCEL, wxID_YES, wxID_NO.
    */
    int ShowModal();
};



// ============================================================================
// Global functions/macros
// ============================================================================

/** @ingroup group_funcmacro_dialog */
//@{

/**
    General purpose message dialog. @c style may be a bit list of the
    following identifiers:

    @beginStyleTable
    @style{wxYES_NO}
        Puts Yes and No buttons on the message box. May be combined with
        wxCANCEL.
    @style{wxCANCEL}
        Puts a Cancel button on the message box. May only be combined with
        wxYES_NO or wxOK.
    @style{wxOK}
        Puts an Ok button on the message box. May be combined with wxCANCEL.
    @style{wxICON_EXCLAMATION}
        Displays an exclamation mark symbol.
    @style{wxICON_HAND}
        Displays an error symbol.
    @style{wxICON_ERROR}
        Displays an error symbol - the same as wxICON_HAND.
    @style{wxICON_QUESTION}
        Displays a question mark symbol.
    @style{wxICON_INFORMATION}
        Displays an information symbol.

    The return value is one of: wxYES, wxNO, wxCANCEL, wxOK. For example:

    @code
    int answer = wxMessageBox("Quit program?", "Confirm",
                              wxYES_NO | wxCANCEL, main_frame);
    if (answer == wxYES)
        main_frame->Close();
    @endcode

    @a message may contain newline characters, in which case the message will
    be split into separate lines, to cater for large messages.

    @header{wx/msgdlg.h}
*/
int wxMessageBox(const wxString& message,
                 const wxString& caption = "Message",
                 int style = wxOK,
                 wxWindow* parent = NULL,
                 int x = -1, int y = -1);

//@}

