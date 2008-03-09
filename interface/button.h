/////////////////////////////////////////////////////////////////////////////
// Name:        button.h
// Purpose:     documentation for wxButton class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxButton
    @wxheader{button.h}

    A button is a control that contains a text string,
    and is one of the most common elements of a GUI. It may be placed on a
    @ref overview_wxdialog "dialog box" or panel, or indeed
    almost any other window.

    @beginStyleTable
    @style{wxBU_LEFT}:
           Left-justifies the label. Windows and GTK+ only.
    @style{wxBU_TOP}:
           Aligns the label to the top of the button. Windows and GTK+ only.
    @style{wxBU_RIGHT}:
           Right-justifies the bitmap label. Windows and GTK+ only.
    @style{wxBU_BOTTOM}:
           Aligns the label to the bottom of the button. Windows and GTK+ only.
    @style{wxBU_EXACTFIT}:
           Creates the button as small as possible instead of making it of the
           standard size (which is the default behaviour ).
    @style{wxBORDER_NONE}:
           Creates a flat button. Windows and GTK+ only.
    @endStyleTable

    @beginEventTable
    @event{EVT_BUTTON(id, func)}:
           Process a wxEVT_COMMAND_BUTTON_CLICKED event, when the button is
           clicked.
    @endEventTable

    @library{wxcore}
    @category{ctrl}
    @appearance{button.png}

    @seealso
    wxBitmapButton
*/
class wxButton : public wxControl
{
public:
    //@{
    /**
        Constructor, creating and showing a button.
        The preferred way to create standard buttons is to use default value of
        @e label. If no label is supplied and @a id is one of standard IDs from
        @ref overview_stockitems "this list", standard label will be used. In addition
        to
        that, the button will be decorated with stock icons under GTK+ 2.
        
        @param parent
            Parent window. Must not be @NULL.
        @param id
            Button identifier. A value of wxID_ANY indicates a default value.
        @param label
            Text to be displayed on the button.
        @param pos
            Button position.
        @param size
            Button size. If the default size is specified then the button is sized
            appropriately for the text.
        @param style
            Window style. See wxButton.
        @param validator
            Window validator.
        @param name
            Window name.
        
        @see Create(), wxValidator
    */
    wxButton();
    wxButton(wxWindow* parent, wxWindowID id,
             const wxString& label = wxEmptyString,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = "button");
    //@}

    /**
        Destructor, destroying the button.
    */
    ~wxButton();

    /**
        Button creation function for two-step creation. For more details, see
        wxButton().
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxString& label = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator,
                const wxString& name = "button");

    /**
        Returns the default size for the buttons. It is advised to make all the dialog
        buttons of the same size and this function allows to retrieve the (platform and
        current font dependent size) which should be the best suited for this.
    */
    wxSize GetDefaultSize();

    /**
        Returns the string label for the button.
        
        @returns The button's label.
        
        @see SetLabel()
    */
    wxString GetLabel() const;

    /**
        This sets the button to be the default item for the panel or dialog
        box.
        
        @remarks Under Windows, only dialog box buttons respond to this function.
                  As normal under Windows and Motif, pressing return
                 causes the default button to be depressed when the
                 return key is pressed. See also wxWindow::SetFocus
                 which sets the keyboard focus for windows and text
                 panel items, and wxTopLevelWindow::SetDefaultItem.
    */
    void SetDefault();

    /**
        Sets the string label for the button.
        
        @param label
            The label to set.
    */
    void SetLabel(const wxString& label);
};
