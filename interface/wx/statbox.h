/////////////////////////////////////////////////////////////////////////////
// Name:        statbox.h
// Purpose:     interface of wxStaticBox
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxStaticBox

    A static box is a rectangle drawn around other windows to denote
    a logical grouping of items.

    Note that since wxWidgets 2.9.0 you are encouraged to build the windows which are
    placed inside wxStaticBoxes as children of the wxStaticBox itself:
    @code
        ...
        wxStaticBox *stbox = new wxStaticBox(parentWindow, wxID_ANY, "StaticBox");

        new wxStaticText(stbox, "This window is a child of the staticbox");
        ...
    @endcode
    
    Creating the windows which are placed inside wxStaticBoxes as siblings of the
    wxStaticBox is still allowed but it's deprecated as it gives some problems
    (e.g. relative to tooltips) on some ports.
    
    Also note that there is a specialized wxSizer class (wxStaticBoxSizer) which can
    be used as an easier way to pack items into a static box.

    @library{wxcore}
    @category{ctrl}
    @appearance{staticbox.png}

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
            Window style. See wxStaticBox.
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
};

