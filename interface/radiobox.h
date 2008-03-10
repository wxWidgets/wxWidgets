/////////////////////////////////////////////////////////////////////////////
// Name:        radiobox.h
// Purpose:     interface of wxRadioBox
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxRadioBox
    @wxheader{radiobox.h}

    A radio box item is used to select one of number of mutually exclusive
    choices.  It is displayed as a vertical column or horizontal row of
    labelled buttons.

    @beginStyleTable
    @style{wxRA_SPECIFY_ROWS}:
           The major dimension parameter refers to the maximum number of rows.
    @style{wxRA_SPECIFY_COLS}:
           The major dimension parameter refers to the maximum number of
           columns.
    @style{wxRA_USE_CHECKBOX}:
           Use of the checkbox controls instead of radio buttons (currently
           supported only on PalmOS)
    @endStyleTable

    @beginEventTable
    @event{EVT_RADIOBOX(id, func)}:
           Process a wxEVT_COMMAND_RADIOBOX_SELECTED event, when a radiobutton
           is clicked.
    @endEventTable

    @library{wxcore}
    @category{ctrl}
    @appearance{radiobox.png}

    @see @ref overview_eventhandlingoverview, wxRadioButton, wxCheckBox
*/
class wxRadioBox : public wxControlWithItems
{
public:
    //@{
    /**
        Constructor, creating and showing a radiobox.
        
        @param parent
            Parent window. Must not be @NULL.
        @param id
            Window identifier. The value wxID_ANY indicates a default value.
        @param label
            Label for the static box surrounding the radio buttons.
        @param pos
            Window position. If wxDefaultPosition is specified then a default
        position is chosen.
        @param size
            Window size. If wxDefaultSize is specified then a default size
        is chosen.
        @param n
            Number of choices with which to initialize the radiobox.
        @param choices
            An array of choices with which to initialize the radiobox.
        @param majorDimension
            Specifies the maximum number of rows (if style contains wxRA_SPECIFY_ROWS)
        or columns (if style contains wxRA_SPECIFY_COLS) for a two-dimensional
            radiobox.
        @param style
            Window style. See wxRadioBox.
        @param validator
            Window validator.
        @param name
            Window name.
        
        @see Create(), wxValidator
    */
    wxRadioBox();
    wxRadioBox(wxWindow* parent, wxWindowID id,
               const wxString& label,
               const wxPoint& point = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int n = 0,
               const wxString choices[] = NULL,
               int majorDimension = 0,
               long style = wxRA_SPECIFY_COLS,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = "radioBox");
    wxRadioBox(wxWindow* parent, wxWindowID id,
               const wxString& label,
               const wxPoint& point,
               const wxSize& size,
               const wxArrayString& choices,
               int majorDimension = 0,
               long style = wxRA_SPECIFY_COLS,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = "radioBox");
    //@}

    /**
        Destructor, destroying the radiobox item.
    */
    ~wxRadioBox();

    //@{
    /**
        Creates the radiobox for two-step construction. See wxRadioBox()
        for further details.
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxString& label,
                const wxPoint& point = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0,
                const wxString choices[] = NULL,
                int majorDimension = 0,
                long style = wxRA_SPECIFY_COLS,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = "radioBox");
    bool Create(wxWindow* parent, wxWindowID id,
                const wxString& label,
                const wxPoint& point,
                const wxSize& size,
                const wxArrayString& choices,
                int majorDimension = 0,
                long style = wxRA_SPECIFY_COLS,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = "radioBox");
    //@}

    //@{
    /**
        Enables or disables an individual button in the radiobox.
        
        @param enable
            @true to enable, @false to disable.
        @param n
            The zero-based button to enable or disable.
        
        @see wxWindow::Enable
    */
    virtual bool Enable(bool enable = true);
    virtual bool Enable(unsigned int n, bool enable = true);
    //@}

    /**
        Finds a button matching the given string, returning the position if found, or
        -1 if not found.
        
        @param string
            The string to find.
    */
    int FindString(const wxString& string) const;

    /**
        Returns the number of columns in the radiobox.
    */
    unsigned int GetColumnCount() const;

    /**
        Returns a radio box item under the point, a zero-based item index, or @c
        wxNOT_FOUND if no item is under the point.
        
        @param pt
            Point in client coordinates.
    */
    int GetItemFromPoint(const wxPoint pt) const;

    /**
        Returns the helptext associated with the specified @a item if any or @c
        wxEmptyString.
        
        @param item
            The zero-based item index.
        
        @see SetItemHelpText()
    */
    wxString GetItemHelpText(unsigned int item) const;

    /**
        Returns the tooltip associated with the specified @a item if any or @NULL.
        
        @see SetItemToolTip(), wxWindow::GetToolTip
    */
    wxToolTip* GetItemToolTip(unsigned int item) const;

    /**
        Returns the radiobox label.
        
        @param n
            The zero-based button index.
        
        @see SetLabel()
    */
    wxString GetLabel() const;

    /**
        Returns the number of rows in the radiobox.
    */
    unsigned int GetRowCount() const;

    /**
        Returns the zero-based position of the selected button.
    */
    int GetSelection() const;

    /**
        Returns the label for the button at the given position.
        
        @param n
            The zero-based button position.
    */
    wxString GetString(unsigned int n) const;

    /**
        Returns the selected string.
    */
    wxString GetStringSelection() const;

    /**
        Returns @true if the item is enabled or @false if it was disabled using
        @ref enable() "Enable(n, @false)".
        @b Platform note: Currently only implemented in wxMSW, wxGTK and wxUniversal
        and always returns @true in the other ports.
        
        @param n
            The zero-based button position.
    */
    bool IsItemEnabled(unsigned int n) const;

    /**
        Returns @true if the item is currently shown or @false if it was hidden
        using
        @ref show() "Show(n, @false)".
        Note that this function returns @true for an item which hadn't been hidden
        even
        if the entire radiobox is not currently shown.
        @b Platform note: Currently only implemented in wxMSW, wxGTK and wxUniversal
        and always returns @true in the other ports.
        
        @param n
            The zero-based button position.
    */
    bool IsItemShown(unsigned int n) const;

    /**
        Sets the helptext for an item. Empty string erases any existing helptext.
        
        @param item
            The zero-based item index.
        @param helptext
            The help text to set for the item.
        
        @see GetItemHelpText()
    */
    void SetItemHelpText(unsigned int item, const wxString& helptext);

    /**
        Sets the tooltip text for the specified item in the radio group.
        @b Platform note: Currently only implemented in wxMSW and wxGTK2 and does
        nothing in the other ports.
        
        @param item
            Index of the item the tooltip will be shown for.
        @param text
            Tooltip text for the item, the tooltip is removed if empty.
        
        @see GetItemToolTip(), wxWindow::SetToolTip
    */
    void SetItemToolTip(unsigned int item, const wxString& text);

    /**
        Sets the radiobox label.
        
        @param label
            The label to set.
        @param n
            The zero-based button index.
    */
    void SetLabel(const wxString& label);

    /**
        Sets a button by passing the desired string position. This does not cause
        a wxEVT_COMMAND_RADIOBOX_SELECTED event to get emitted.
        
        @param n
            The zero-based button position.
    */
    void SetSelection(int n);

    /**
        Sets the selection to a button by passing the desired string. This does not
        cause
        a wxEVT_COMMAND_RADIOBOX_SELECTED event to get emitted.
        
        @param string
            The label of the button to select.
    */
    void SetStringSelection(const wxString& string);
};

