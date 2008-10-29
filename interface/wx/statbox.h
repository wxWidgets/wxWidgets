/////////////////////////////////////////////////////////////////////////////
// Name:        statbox.h
// Purpose:     interface of wxStaticBox
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxStaticBox

    A static box is a rectangle drawn around other panel items to denote
    a logical grouping of items.

    Please note that a static box should @b not be used as the parent for the
    controls it contains, instead they should be siblings of each other. Although
    using a static box as a parent might work in some versions of wxWidgets, it
    results in a crash under, for example, wxGTK.

    Also, please note that because of this, the order in which you create new
    controls is important. Create your wxStaticBox control @b before any
    siblings that are to appear inside the wxStaticBox in order to preserve the
    correct Z-Order of controls.

    @library{wxcore}
    @category{ctrl}
    @appearance{staticbox.png}

    @see wxStaticText
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
            If wxDefaultPosition is specified then a default position is chosen.
        @param size
            Checkbox size.
            If wxDefaultSize is specified then a default size is chosen.
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
                const wxString& name = "staticBox");

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

