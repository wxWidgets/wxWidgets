/////////////////////////////////////////////////////////////////////////////
// Name:        textdlg.h
// Purpose:     interface of wxPasswordEntryDialog
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    Default text dialog style.
*/
#define wxTextEntryDialogStyle (wxOK | wxCANCEL | wxCENTRE)

/// Default text dialog caption.
const char wxGetTextFromUserPromptStr[] = "Input Text";

/// Default password dialog caption.
const char wxGetPasswordFromUserPromptStr[] = "Enter Password";


/**
    @class wxPasswordEntryDialog

    This class represents a dialog that requests a one-line password string from
    the user.

    It is implemented as a generic wxWidgets dialog.

    @library{wxcore}
    @category{cmndlg}

    @see @ref overview_cmndlg_password
*/
class wxPasswordEntryDialog : public wxTextEntryDialog
{
public:
    /**
        Constructor.

        Use wxTextEntryDialog::ShowModal to show the dialog.

        @param parent
            Parent window.
        @param message
            Message to show on the dialog.
        @param caption
            The caption of the dialog.
        @param defaultValue
            The default value, which may be the empty string.
        @param style
            A dialog style, specifying the buttons (wxOK, wxCANCEL) and an
            optional wxCENTRE style. You do not need to specify the wxTE_PASSWORD style,
            it is always applied.
        @param pos
            Dialog position.
    */
    wxPasswordEntryDialog(wxWindow* parent, const wxString& message,
                          const wxString& caption = wxGetPasswordFromUserPromptStr,
                          const wxString& defaultValue = wxEmptyString,
                          long style = wxTextEntryDialogStyle,
                          const wxPoint& pos = wxDefaultPosition);
};



/**
    @class wxTextEntryDialog

    This class represents a dialog that requests a one-line text string from the user.
    It is implemented as a generic wxWidgets dialog.

    @library{wxcore}
    @category{cmndlg}

    @see @ref overview_cmndlg_textentry
*/
class wxTextEntryDialog : public wxDialog
{
public:
    /**
        Default constructor.

        Call Create() to really create the dialog later.

        @since 2.9.5
     */
    wxTextEntryDialog();

    /**
        Constructor.

        Use ShowModal() to show the dialog.

        See Create() method for parameter description.
    */
    wxTextEntryDialog(wxWindow* parent, const wxString& message,
                      const wxString& caption = wxGetTextFromUserPromptStr,
                      const wxString& value = wxEmptyString,
                      long style = wxTextEntryDialogStyle,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize sz = wxDefaultSize);

    /**
        @param parent
            Parent window.
        @param message
            Message to show on the dialog.
        @param caption
            The caption of the dialog.
        @param value
            The default value, which may be the empty string.
        @param style
            A dialog style, specifying the buttons (wxOK, wxCANCEL)
            and an optional wxCENTRE style. Additionally, most wxTextCtrl
            styles (such as @c wxTE_PASSWORD or @c wxTE_MULTILINE) may be
            specified here, but @c wxTE_READONLY may not be used, as it doesn't
            make sense for this dialog, used for text input.
        @param pos
            Dialog position.
        @param sz
            The size of the dialog (this parameter is only available since wxWidgets 3.3.0).

        @since 2.9.5
    */
    bool Create(wxWindow* parent, const wxString& message,
                      const wxString& caption = wxGetTextFromUserPromptStr,
                      const wxString& value = wxEmptyString,
                      long style = wxTextEntryDialogStyle,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize sz = wxDefaultSize);

    /**
        Destructor.
    */
    virtual ~wxTextEntryDialog();

    /**
        Returns the text that the user has entered if the user has pressed OK, or the
        original value if the user has pressed Cancel.
    */
    wxString GetValue() const;

    /**
        Associate a validator with the text control used by the dialog.

        These methods can be used to limit the user entry to only some
        characters, e.g.
        @code
            wxTextEntryDialog dlg(this, ...);
            dlg.SetTextValidator(wxFILTER_ALPHA);
            if ( dlg.ShowModal() == wxID_OK )
            {
                // We can be certain that this string contains letters only.
                wxString value = dlg.GetValue();
            }
        @endcode

        The first overload uses the provided @a validator which can be of a
        custom class derived from wxTextValidator while the second one creates
        a wxTextValidator with the specified @a style.
     */
    ///@{
    void SetTextValidator(const wxTextValidator& validator);
    void SetTextValidator(wxTextValidatorStyle style = wxFILTER_NONE);
    ///@}

    /**
        This function sets the maximum number of characters the user can enter
        into this dialog.

        @see wxTextEntry::SetMaxLength()

        @since 2.9.5
    */
    void SetMaxLength(unsigned long len);

    /**
        Sets the default text value.
    */
    void SetValue(const wxString& value);

    /**
        Sets the hint shown in the empty text control.

        @see wxTextEntry::SetHint()

        @since 3.3.2
    */
    void SetHint(const wxString &hint);

    /**
        Convert all text entered into the text control used by the dialog to upper case.

        Call this method to ensure that all text entered into the text control
        used by the dialog is converted on the fly to upper case. If the text
        control is not empty, its existing contents is also converted to upper
        case.

        @see wxTextEntry::ForceUpper()

        @since 3.1.0
     */
    void ForceUpper();

    /**
        Shows the dialog, returning wxID_OK if the user pressed OK, and wxID_CANCEL
        otherwise.

        Call GetValue() to retrieve the values of the string entered by the
        user after showing the dialog.
    */
    int ShowModal();
};



// ============================================================================
// Global functions/macros
// ============================================================================

/** @addtogroup group_funcmacro_dialog */
///@{

/**
    Pop up a dialog box with title set to @e caption, @c message, and a
    @c default_value. The user may type in text and press OK to return this
    text, or press Cancel to return the empty string.

    If both @a x and @a y are specified, i.e. are not wxDefaultCoord, the dialog
    is shown at this position and @a centre is ignored. It is an error to
    specify only one of them. Otherwise, the dialog is centred on the screen or
    on its parent window if @a centre is @true (which is the default) or shown
    at the default position if it is @false.

    The second overload also allows to specify the @a size of the dialog, which
    is then made at least this big, but never smaller than the size needed to
    show its contents. As the dialog can only show a single line of text, its
    height is normally determined by its contents and only the width of @a size
    is useful, so the height should usually be left as wxDefaultCoord.

    This function is a wrapper around wxTextEntryDialog and while it is usually
    more convenient to use, using the dialog directly is more flexible, e.g. it
    allows you to specify the @c wxTE_MULTILINE to allow the user enter
    multiple lines of text while this function is limited to single line entry
    only.

    @note Before wxWidgets 3.3.4, the position was ignored if @a centre was
        @true, i.e. by default, and the second overload didn't exist.

    @header{wx/textdlg.h}
*/
wxString wxGetTextFromUser(const wxString& message,
                           const wxString& caption = wxGetTextFromUserPromptStr,
                           const wxString& default_value = wxEmptyString,
                           wxWindow* parent = nullptr,
                           int x = wxDefaultCoord,
                           int y = wxDefaultCoord,
                           bool centre = true);

wxString wxGetTextFromUser(const wxString& message,
                           const wxString& caption,
                           const wxString& default_value,
                           wxWindow* parent,
                           const wxPoint& pos,
                           const wxSize& size = wxDefaultSize,
                           bool centre = true);

/**
    Similar to wxGetTextFromUser() but the text entered in the dialog is not
    shown on screen but replaced with stars. This is intended to be used for
    entering passwords as the function name implies.

    @header{wx/textdlg.h}
*/
wxString wxGetPasswordFromUser(const wxString& message,
                               const wxString& caption = wxGetPasswordFromUserPromptStr,
                               const wxString& default_value = wxEmptyString,
                               wxWindow* parent = nullptr,
                               int x = wxDefaultCoord,
                               int y = wxDefaultCoord,
                               bool centre = true);

///@}

