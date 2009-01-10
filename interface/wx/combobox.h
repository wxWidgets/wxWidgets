/////////////////////////////////////////////////////////////////////////////
// Name:        combobox.h
// Purpose:     interface of wxComboBox
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxComboBox

    A combobox is like a combination of an edit control and a listbox. It can
    be displayed as static list with editable or read-only text field; or a
    drop-down list with text field; or a drop-down list without a text field.

    A combobox permits a single selection only. Combobox items are numbered
    from zero.

    If you need a customized combobox, have a look at wxComboCtrl,
    wxOwnerDrawnComboBox, wxComboPopup and the ready-to-use wxBitmapComboBox.

    @beginStyleTable
    @style{wxCB_SIMPLE}
           Creates a combobox with a permanently displayed list. Windows only.
    @style{wxCB_DROPDOWN}
           Creates a combobox with a drop-down list.
    @style{wxCB_READONLY}
           Same as wxCB_DROPDOWN but only the strings specified as the combobox
           choices can be selected, it is impossible to select (even from a
           program) a string which is not in the choices list.
    @style{wxCB_SORT}
           Sorts the entries in the list alphabetically.
    @style{wxTE_PROCESS_ENTER}
           The control will generate the event wxEVT_COMMAND_TEXT_ENTER
           (otherwise pressing Enter key is either processed internally by the
           control or used for navigation between dialog controls). Windows
           only.
    @endStyleTable

    @beginEventTable{wxCommandEvent}
    @event{EVT_COMBOBOX(id, func)}
           Process a wxEVT_COMMAND_COMBOBOX_SELECTED event, when an item on
           the list is selected. Note that calling GetValue() returns the new
           value of selection.
    @event{EVT_TEXT(id, func)}
           Process a wxEVT_COMMAND_TEXT_UPDATED event, when the combobox text
           changes.
    @event{EVT_TEXT_ENTER(id, func)}
           Process a wxEVT_COMMAND_TEXT_ENTER event, when RETURN is pressed in
           the combobox (notice that the combobox must have been created with
           wxTE_PROCESS_ENTER style to receive this event).
    @endEventTable

    @library{wxcore}
    @category{ctrl}
    @appearance{combobox.png}

    @see wxListBox, wxTextCtrl, wxChoice, wxCommandEvent
*/
class wxComboBox : public wxControl, public wxItemContainer
{
public:
    /**
        Default constructor.
    */
    wxComboBox();

    //@{
    /**
        Constructor, creating and showing a combobox.

        @param parent
            Parent window. Must not be @NULL.
        @param id
            Window identifier. The value wxID_ANY indicates a default value.
        @param value
            Initial selection string. An empty string indicates no selection.
        @param pos
            Window position.
        @param size
            Window size. If wxDefaultSize is specified then the window is sized
            appropriately.
        @param n
            Number of strings with which to initialise the control.
        @param choices
            An array of strings with which to initialise the control.
        @param style
            Window style. See wxComboBox.
        @param validator
            Window validator.
        @param name
            Window name.

        @beginWxPythonOnly
        The wxComboBox constructor in wxPython reduces the @a n and @a choices
        arguments are to a single argument, which is a list of strings.
        @endWxPythonOnly

        @see Create(), wxValidator
    */
    wxComboBox(wxWindow* parent, wxWindowID id,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int n = 0,
               const wxString choices[] = NULL,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = "comboBox");
    /**
        Constructor, creating and showing a combobox.

        @param parent
            Parent window. Must not be @NULL.
        @param id
            Window identifier. The value wxID_ANY indicates a default value.
        @param value
            Initial selection string. An empty string indicates no selection.
        @param pos
            Window position.
        @param size
            Window size. If wxDefaultSize is specified then the window is sized
            appropriately.
        @param choices
            An array of strings with which to initialise the control.
        @param style
            Window style. See wxComboBox.
        @param validator
            Window validator.
        @param name
            Window name.

        @beginWxPythonOnly
        The wxComboBox constructor in wxPython reduces the @a n and @a choices
        arguments are to a single argument, which is a list of strings.
        @endWxPythonOnly

        @see Create(), wxValidator
    */
    wxComboBox(wxWindow* parent, wxWindowID id,
               const wxString& value,
               const wxPoint& pos,
               const wxSize& size,
               const wxArrayString& choices,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = "comboBox");
    //@}

    /**
        Destructor, destroying the combobox.
    */
    virtual ~wxComboBox();

    //@{
    /**
        Creates the combobox for two-step construction. Derived classes should
        call or replace this function. See wxComboBox() for further details.
    */
    bool Create(wxWindow *parent, wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0, const wxString choices[] = (const wxString *) NULL,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr);
    bool Create(wxWindow *parent, wxWindowID id,
                const wxString& value,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr);
    //@}

    /**
        Returns @true if the combobox is editable and there is a text selection
        to copy to the clipboard. Only available on Windows.
    */
    virtual bool CanCopy() const;

    /**
        Returns @true if the combobox is editable and there is a text selection
        to copy to the clipboard. Only available on Windows.
    */
    virtual bool CanCut() const;

    /**
        Returns @true if the combobox is editable and there is text on the
        clipboard that can be pasted into the text field. Only available on
        Windows.
    */
    virtual bool CanPaste() const;

    /**
        Returns @true if the combobox is editable and the last undo can be
        redone. Only available on Windows.
    */
    virtual bool CanRedo() const;

    /**
        Returns @true if the combobox is editable and the last edit can be
        undone. Only available on Windows.
    */
    virtual bool CanUndo() const;

    /**
        Copies the selected text to the clipboard.
    */
    virtual void Copy();

    /**
        Copies the selected text to the clipboard and removes the selection.
    */
    virtual void Cut();

    /**
        This function does the same things as wxChoice::GetCurrentSelection()
        and returns the item currently selected in the dropdown list if it's
        open or the same thing as wxControlWithItems::GetSelection() otherwise.
    */
    virtual int GetCurrentSelection() const;

    /**
        Returns the insertion point for the combobox's text field.

        @note Under wxMSW, this function always returns 0 if the combobox
              doesn't have the focus.
    */
    virtual long GetInsertionPoint() const;

    /**
        Returns the last position in the combobox text field.
    */
    virtual long GetLastPosition() const;

    /**
        This is the same as wxTextCtrl::GetSelection() for the text control
        which is part of the combobox. Notice that this is a different method
        from wxControlWithItems::GetSelection().

        Currently this method is only implemented in wxMSW and wxGTK.
    */
    virtual void GetSelection(long* from, long* to) const;

    /**
        Returns the current value in the combobox text field.
    */
    virtual wxString GetValue() const;

    /**
        Pastes text from the clipboard to the text field.
    */
    virtual void Paste();

    /**
        Redoes the last undo in the text field. Windows only.
    */
    virtual void Redo();

    /**
        Removes the text between the two positions in the combobox text field.

        @param from
            The first position.
        @param to
            The last position.
    */
    virtual void Remove(long from, long to);

    /**
        Replaces the text between two positions with the given text, in the
        combobox text field.

        @param from
            The first position.
        @param to
            The second position.
        @param text
            The text to insert.
    */
    virtual void Replace(long from, long to, const wxString& text);

    /**
        Sets the insertion point in the combobox text field.

        @param pos
            The new insertion point.
    */
    virtual void SetInsertionPoint(long pos);

    /**
        Sets the insertion point at the end of the combobox text field.
    */
    virtual void SetInsertionPointEnd();

    /**
        Selects the text between the two positions, in the combobox text field.

        @param from
            The first position.
        @param to
            The second position.

        @beginWxPythonOnly
        This method is called SetMark() in wxPython, "SetSelection" is kept for
        wxControlWithItems::SetSelection().
        @endWxPythonOnly
    */
    virtual void SetSelection(long from, long to);

    /**
        Sets the text for the combobox text field.

        @note For a combobox with @c wxCB_READONLY style the string must be in
              the combobox choices list, otherwise the call to SetValue() is
              ignored.

        @param text
            The text to set.
    */
    virtual void SetValue(const wxString& text);

    /**
        Undoes the last edit in the text field. Windows only.
    */
    virtual void Undo();
};

