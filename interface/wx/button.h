/////////////////////////////////////////////////////////////////////////////
// Name:        button.h
// Purpose:     interface of wxButton
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxButton

    A button is a control that contains a text string, and is one of the most
    common elements of a GUI.

    It may be placed on a @ref wxDialog "dialog box" or on a @ref wxPanel panel,
    or indeed on almost any other window.

    @beginStyleTable
    @style{wxBU_LEFT}
           Left-justifies the label. Windows and GTK+ only.
    @style{wxBU_TOP}
           Aligns the label to the top of the button. Windows and GTK+ only.
    @style{wxBU_RIGHT}
           Right-justifies the bitmap label. Windows and GTK+ only.
    @style{wxBU_BOTTOM}
           Aligns the label to the bottom of the button. Windows and GTK+ only.
    @style{wxBU_EXACTFIT}
           Creates the button as small as possible instead of making it of the
           standard size (which is the default behaviour ).
    @style{wxBORDER_NONE}
           Creates a flat button. Windows and GTK+ only.
    @endStyleTable

    @beginEventEmissionTable{wxCommandEvent}
    @event{EVT_BUTTON(id, func)}
           Process a wxEVT_COMMAND_BUTTON_CLICKED event, when the button is clicked.
    @endEventTable

    @library{wxcore}
    @category{ctrl}
    @appearance{button.png}

    @see wxBitmapButton
*/
class wxButton : public wxControl
{
public:
    /**
        Default ctor.
    */
    wxButton();

    /**
        Constructor, creating and showing a button.

        The preferred way to create standard buttons is to use default value of
        @a label. If no label is supplied and @a id is one of standard IDs from
        @ref page_stockitems "this list", a standard label will be used.

        In addition to that, the button will be decorated with stock icons under GTK+ 2.

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
            Window style. See wxButton class description.
        @param validator
            Window validator.
        @param name
            Window name.

        @see Create(), wxValidator
    */
    wxButton(wxWindow* parent, wxWindowID id,
             const wxString& label = wxEmptyString,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxButtonNameStr);

    /**
        Destructor, destroying the button.
    */
    virtual ~wxButton();

    /**
        Button creation function for two-step creation.
        For more details, see wxButton().
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxString& label = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxButtonNameStr);

    /**
        Returns the default size for the buttons. It is advised to make all the dialog
        buttons of the same size and this function allows to retrieve the (platform and
        current font dependent size) which should be the best suited for this.
    */
    static wxSize GetDefaultSize();

    /**
        Returns the string label for the button.

        @see SetLabel()
    */
    wxString GetLabel() const;

    /**
        This sets the button to be the default item in its top-level window
        (e.g. the panel or the dialog box containing it).

        As normal, pressing return causes the default button to be depressed when
        the return key is pressed.

        See also wxWindow::SetFocus() which sets the keyboard focus for windows
        and text panel items, and wxTopLevelWindow::SetDefaultItem().

        @remarks Under Windows, only dialog box buttons respond to this function.

        @return the old default item (possibly NULL)
    */
    virtual wxWindow* SetDefault();

    /**
        Sets the string label for the button.

        @param label
            The label to set.
    */
    void SetLabel(const wxString& label);
};

