/////////////////////////////////////////////////////////////////////////////
// Name:        control.h
// Purpose:     interface of wxControl
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxControl
    @wxheader{control.h}

    This is the base class for a control or "widget".

    A control is generally a small window which processes user input and/or
    displays one or more item of data.

    @library{wxcore}
    @category{ctrl}
    @appearance{control.png}

    @see wxValidator
*/
class wxControl : public wxWindow
{
public:
    /**
        Simulates the effect of the user issuing a command to the item. See
        wxCommandEvent.
    */
    void Command(wxCommandEvent& event);

    /**
        Returns the control's text.
        Note that the returned string contains the mnemonics (@c  characters) if
        any, use GetLabelText() if they are
        undesired.
    */
    wxString GetLabel() const;

    //@{
    /**
        Returns the control's label, or the given @a label string for the static
        version, without the mnemonics characters.
    */
    const wxString GetLabelText();
    const static wxString  GetLabelText(const wxString& label);
    //@}

    /**
        Sets the item's text.
        The @c  characters in the @a label are special and indicate that the
        following character is a mnemonic for this control and can be used to activate
        it from the keyboard (typically by using @e Alt key in combination with
        it). To insert a literal ampersand character, you need to double it, i.e. use
        @c "".
    */
    void SetLabel(const wxString& label);
};

