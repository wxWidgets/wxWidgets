/////////////////////////////////////////////////////////////////////////////
// Name:        wx/richmsgdlg.h
// Purpose:     interface of wxRichMessageDialog
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxRichMessageDialog

    This class is an extension of wxMessageDialog.
    Unlike the base class it's only native under MSW when using
    Windows Vista/7.

    A checkbox and a user-expandable detailed text can be added
    to the dialog.
    If the checkbox is present, it is initially unchecked.
    Call SetCheckBoxValue() before calling ShowModal() to change this.

    The dialog supports all styles supported by wxMessageDialog.

    @library{wxcore}
    @category{cmndlg}

    @see @ref overview_cmndlg_msg
*/
class wxRichMessageDialog : public wxRichMessageDialogBase
{
public:
    /**
        Constructor specifying the rich message dialog properties.
        Works just like the constructor for wxMessageDialog.
    */
    wxRichMessageDialog(wxWindow* parent, const wxString& message,
                        const wxString& caption = wxMessageBoxCaptionStr,
                        long style = wxOK | wxCENTRE,
                        const wxPoint& pos = wxDefaultPosition);

    /**
        Shows a checkbox with a given label or hides it.

        @param checkBoxText
            If the parameter is non-empty a checkbox will be shown with that
            label, otherwise it will be hidden.
        @param checked
            The initial state of the checkbox.
     */
    void ShowCheckBox(const wxString &checkBoxText, bool checked = false);


    /**
        Retrieves the label for the checkbox.

        @return
            The label for the checkbox, will be the empty string if no
            checkbox is used.
    */
    wxString GetCheckBoxText() const;

    /**
        Shows or hides a detailed text and an expander that is used to
        show or hide the detailed text.

        @param detailedText
            The detailed text that can be expanded when the dialog is shown,
            if empty no detailed text will be used.
    */
    void ShowDetailedText(const wxString &detailedText);

    /**
        Retrieves the detailed text.

        @return
            The detailed text or empty if detailed text is not used.
     */
    wxString GetDetailedText() const;

    /**
        Retrieves the state of the checkbox. The returned value is both
        affected by calls to SetCheckBoxValue() and any changes made to it when
        the dialog is shown.

        @return
            True if the checkbox was checked and false if not.
            If no checkbox was used or the dialog hasn't been shown the value
            will be the one set by a call to SetCheckBoxValue() or false if
            none have been made.
    */
    bool IsCheckBoxChecked() const;

    /**
        Shows the dialog, returning one of wxID_OK, wxID_CANCEL, wxID_YES, wxID_NO.

        IsCheckBoxChecked() can be called afterwards to retrieve the value of the
        check box if one was used.
    */
    virtual int ShowModal();
};
