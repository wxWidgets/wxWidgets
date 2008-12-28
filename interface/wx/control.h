/////////////////////////////////////////////////////////////////////////////
// Name:        control.h
// Purpose:     interface of wxControl
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

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
        Sets the item's text.

        Any "&" characters in the @a label are special and indicate that the
        following character is a @e mnemonic for this control and can be used to
        activate it from the keyboard (typically by using @e Alt key in
        combination with it).
        To insert a literal ampersand character, you need to double it, i.e. use "&&".
    */
    void SetLabel(const wxString& label);
};

