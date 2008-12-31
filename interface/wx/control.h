/////////////////////////////////////////////////////////////////////////////
// Name:        control.h
// Purpose:     interface of wxControl
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    The different ellipsization modes supported by the
    wxControl::Ellipsize function.
*/
enum wxEllipsizeMode
{
    wxELLIPSIZE_START,
    wxELLIPSIZE_MIDDLE,
    wxELLIPSIZE_END
};

/**
    @class wxControl

    This is the base class for a control or "widget".

    A control is generally a small window which processes user input and/or
    displays one or more item of data.

    @library{wxcore}
    @category{ctrl}

    @see wxValidator
*/
class wxControl : public wxWindow
{
public:
    /**
        Simulates the effect of the user issuing a command to the item.

        @see wxCommandEvent
    */
    virtual void Command(wxCommandEvent& event);

    /**
        Replaces parts of the @a label string with ellipsis, if needed, so
        that it doesn't exceed @a maxWidth.

        @param label
            The string to ellipsize
        @param dc
            The DC used to retrieve the character widths through the
            wxDC::GetPartialTextExtents() function.
        @param mode
            The ellipsization modes. See ::wxEllipsizeMode.
        @param maxWidth
            The maximum width of the returned string in pixels.
    */
    static wxString Ellipsize(const wxString& label, const wxDC& dc,
                              wxEllipsizeMode mode, int maxWidth);

    /**
        Returns the control's text.

        @note The returned string contains mnemonics ("&" characters) if it has
              any, use GetLabelText() if they are undesired.
    */
    wxString GetLabel() const;

    /**
        Returns the control's label without mnemonics.
    */
    wxString GetLabelText() const;

    /**
        Returns the given @a label string without mnemonics ("&" characters).
    */
    static wxString GetLabelText(const wxString& label);

    /**
        Removes the mnemonics ("&" characters) from the given string.
    */
    static wxString RemoveMnemonics(const wxString& str);

    /**
        Escape the special mnemonics characters ("&") in the given string.

        This function can be helpful if you need to set the controls label to a
        user-provided string. If the string contains ampersands, they wouldn't
        appear on the display but be used instead to indicate that the
        character following the first of them can be used as a control mnemonic.
        While this can sometimes be desirable (e.g. to allow the user to
        configure mnemonics of the controls), more often you will want to use
        this function before passing a user-defined string to SetLabel().
        Alternatively, if the label is entirely user-defined, you can just call
        SetLabelText() directly -- but this function must be used if the label
        is a combination of a part defined by program containing the control
        mnemonics and a user-defined part.

        @param text
            The string such as it should appear on the display.
        @return
            The same string with the ampersands in it doubled.
     */
    static wxString EscapeMnemonics(const wxString& text);

    /**
        Sets the item's text.

        Any "&" characters in the @a label are special and indicate that the
        following character is a @e mnemonic for this control and can be used to
        activate it from the keyboard (typically by using @e Alt key in
        combination with it). To insert a literal ampersand character, you need
        to double it, i.e. use use "&&". If this behaviour is undesirable, use
        SetLabelText() instead.
    */
    void SetLabel(const wxString& label);

    /**
        Sets the item's text to exactly the given string.

        Unlike SetLabel(), this function shows exactly the @a text passed to it
        in the control, without interpreting ampersands in it in any way.
        Notice that it means that the control can't have any mnemonic defined
        for it using this function.

        @see EscapeMnemonics()
     */
    void SetLabelText(const wxString& text);
};

