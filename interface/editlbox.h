/////////////////////////////////////////////////////////////////////////////
// Name:        editlbox.h
// Purpose:     documentation for wxEditableListBox class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxEditableListBox
    @wxheader{editlbox.h}

    An editable listbox is composite control that lets the
    user easily enter, delete and reorder a list of strings.

    @beginStyleTable
    @style{wxEL_ALLOW_NEW}:
           Allows the user to enter new strings.
    @style{wxEL_ALLOW_EDIT}:
           Allows the user to edit existing strings.
    @style{wxEL_ALLOW_DELETE}:
           Allows the user to delete existing strings.
    @style{wxEL_NO_REORDER}:
           Does not allow the user to reorder the strings.
    @style{wxEL_DEFAULT_STYLE}:
           wxEL_ALLOW_NEW|wxEL_ALLOW_EDIT|wxEL_ALLOW_DELETE
    @endStyleTable

    @library{wxadv}
    @category{FIXME}

    @seealso
    wxListBox
*/
class wxEditableListBox : public wxPanel
{
public:
    //@{
    /**
        Constructor, creating and showing a list box.
        
        @param parent
            Parent window. Must not be @NULL.
        @param id
            Window identifier. The value wxID_ANY indicates a default value.
        @param label
            The text shown just before the list control.
        @param pos
            Window position.
        @param size
            Window size. If wxDefaultSize is specified then the window is
        sized
            appropriately.
        @param style
            Window style. See wxEditableListBox.
        @param name
            Window name.
        
        @see Create()
    */
    wxEditableListBox();
    wxEditableListBox(wxWindow* parent, wxWindowID id,
                      const wxString& label,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = wxEL_DEFAULT_STYLE,
                      const wxString& name = "editableListBox");
    //@}

    /**
        Destructor, destroying the list box.
    */
    ~wxEditableListBox();

    /**
        Creates the editable listbox for two-step construction. See wxEditableListBox()
        for further details.
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxEL_DEFAULT_STYLE,
                const wxString& name = "editableListBox");

    /**
        Replaces current contents with given strings.
    */
    void SetStrings(const wxArrayString& strings);
};
