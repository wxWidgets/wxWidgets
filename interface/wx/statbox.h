/////////////////////////////////////////////////////////////////////////////
// Name:        statbox.h
// Purpose:     interface of wxStaticBox
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxStaticBox

    A static box is a rectangle drawn around other windows to denote
    a logical grouping of items.

    Note that while the previous versions required that windows appearing
    inside a static box be created as its siblings (i.e. use the same parent as
    the static box itself), since wxWidgets 2.9.1 it is also possible to create
    them as children of wxStaticBox itself and you are actually encouraged to
    do it like this if compatibility with the previous versions is not
    important.

    So the new recommended way to create static box is:
    @code
        void MyFrame::CreateControls()
        {
            wxPanel *panel = new wxPanel(this);
            wxStaticBox *box = new wxStaticBox(panel, wxID_ANY, "StaticBox");

            new wxStaticText(box, wxID_ANY "This window is a child of the staticbox");
            ...
        }
    @endcode

    While the compatible -- and now deprecated -- way is
    @code
            wxStaticBox *box = new wxStaticBox(panel, wxID_ANY, "StaticBox");

            new wxStaticText(panel, wxID_ANY "This window is a child of the panel");
            ...
    @endcode

    Also note that there is a specialized wxSizer class (wxStaticBoxSizer) which can
    be used as an easier way to pack items into a static box.

    @library{wxcore}
    @category{ctrl}
    @appearance{staticbox}

    @see wxStaticText, wxStaticBoxSizer
*/
class wxStaticBox : public wxControl
{
public:
    /**
      Default constructor
    */
    wxStaticBox();

    /**
        Constructor, creating and showing a static box.

        @param parent
            Parent window. Must not be @NULL.
        @param id
            Window identifier. The value wxID_ANY indicates a default value.
        @param label
            Text to be displayed in the static box, the empty string for no label.
        @param pos
            Window position.
            If ::wxDefaultPosition is specified then a default position is chosen.
        @param size
            Checkbox size.
            If ::wxDefaultSize is specified then a default size is chosen.
        @param style
            Window style. There are no wxStaticBox-specific styles, but generic
            ::wxALIGN_LEFT, ::wxALIGN_CENTRE_HORIZONTAL and ::wxALIGN_RIGHT can
            be used here to change the position of the static box label when
            using wxGTK (these styles are ignored under the other platforms
            currently).
        @param name
            Window name.

        @see Create()
    */
    wxStaticBox(wxWindow* parent, wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxStaticBoxNameStr);

    /**
        Constructor for a static box using the given window as label.

        This constructor takes a pointer to an arbitrary window (although
        usually a wxCheckBox or a wxRadioButton) instead of just the usual text
        label and puts this window at the top of the box at the place where the
        label would be shown.

        The @a label window must be a non-null, fully created window and will
        become a child of this wxStaticBox, i.e. it will be owned by this
        control and will be deleted when the wxStaticBox itself is deleted.

        An example of creating a wxStaticBox with window as a label:
        @code
        void MyFrame::CreateControls()
        {
            wxPanel* panel = new wxPanel(this);
            wxCheckBox* checkbox = new wxCheckBox(panel, wxID_ANY, "Box checkbox");
            wxStaticBox* box = new wxStaticBox(panel, wxID_ANY, checkbox);
            ...
        }
        @endcode

        Currently this constructor is only available in wxGTK, use
        @c wxHAS_WINDOW_LABEL_IN_STATIC_BOX to check whether it can be used at
        compile-time.

        @since 3.1.1
     */
    wxStaticBox(wxWindow* parent, wxWindowID id,
                wxWindow* label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxStaticBoxNameStr);

    /**
        Destructor, destroying the group box.
    */
    virtual ~wxStaticBox();

    /**
        Creates the static box for two-step construction.
        See wxStaticBox() for further details.
    */
    bool Create(wxWindow* parent, wxWindowID id, const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0,
                const wxString& name = wxStaticBoxNameStr);

    /**
        Creates the static box with the window as a label.

        This method can only be called for an object created using its default
        constructor.

        See the constructor documentation for more details.

        Currently this overload is only available in wxGTK, use
        @c wxHAS_WINDOW_LABEL_IN_STATIC_BOX to check whether it can be used at
        compile-time.

        @since 3.1.1
     */
    wxStaticBox(wxWindow* parent, wxWindowID id,
                wxWindow* label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxStaticBoxNameStr);
};
