/////////////////////////////////////////////////////////////////////////////
// Name:        commandlinkbutton.h
// Purpose:     interface of wxCommandLinkButton
// Author:      wxWidgets team
// RCS-ID:      
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxCommandLinkButton

    Command link buttons are used to give more detailed explanations about
    the command to be performed after clicking the button and are typically
    used together with other command link buttons when a choice needs
    to be made.
    A command link button consists of a main label which serves as the heading
    and followed with a descriptive text underneath it called the note.
    Furthermore the button displays an arrow next to it if no other bitmap
    is set to indicate that an action will be taken.

    The main label and note of the button can be both be set and retrieved
    individually as well as at the same time, using the SetLabel() and
    GetLabel() methods.

    It may be placed on a @ref wxDialog "dialog box" or on a @ref wxPanel panel,
    or indeed on almost any other window.

    @since 2.9.2

    @library{wxcore}
    @category{ctrl}
    @appearance{commandlinkbutton.png}

    @see wxButton, wxBitmapButton
*/
class wxCommandLinkButton : public wxGenericCommandLinkButton
{
public:
    /**
        Default ctor.
    */
    wxCommandLinkButton();

    /**
        Constructor, creating and showing a command link button.

        The button will be decorated with stock icons under GTK+ 2.

        @param parent
            Parent window. Must not be @NULL.
        @param id
            Button identifier. A value of wxID_ANY indicates a default value.
        @param mainLabel
            First line of text on the button, typically the label of an action
            that will be made when the button is pressed.
        @param note
            Second line of text describing the action performed when the button
            is pressed.
        @param pos
            Button position.
        @param size
            Button size. If the default size is specified then the button is sized
            appropriately for the text.
        @param style
            Window style. See wxButton class description.
        @param validator
            Window validator.
        @param name
            Window name.

        @see Create(), wxValidator
    */
    wxCommandLinkButton(wxWindow* parent, wxWindowID id,
                        const wxString& mainLabel = wxEmptyString,
                        const wxString& note = wxEmptyString,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxValidator& validator = wxDefaultValidator,
                        const wxString& name = wxButtonNameStr);

    /**
        Button creation function for two-step creation.
        For more details, see wxCommandLinkButton().
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxString& mainLabel = wxEmptyString,
                const wxString& note = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxButtonNameStr);

    /**
        Sets a new main label and note for the button.

        @param mainLabel
            New main label to use.
        @param note
            New note to use.
    */
    void SetMainLabelAndNote(const wxString& mainLabel, const wxString& note);

    /**
        Sets the string label and note for the button.

        @param label
            The label and note to set, with the two separated
            by the first newline or none to set a blank note.
    */
    virtual void SetLabel(const wxString& label);

    /**
        Returns the string label for the button.

        @see SetLabel()

        @return
            A string with the main label and note concatenated
            together with a newline separating them.
    */
    wxString GetLabel() const;

    /**
        Changes the main label.

        @param mainLabel
            New main label to use.
    */
    void SetMainLabel(const wxString &mainLabel);

    /**
        Changes the note.

        @param note
            New note to use.
    */
    void SetNote(const wxString &note);

    /**
        Gets the current main label.

        @return
            Main label currently displayed.
    */
    wxString GetMainLabel() const;

    /**
        Gets the current note.

        @return
            Note currently displayed.
    */
    wxString GetNote() const;
};

