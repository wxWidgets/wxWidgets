/////////////////////////////////////////////////////////////////////////////
// Name:        choice.h
// Purpose:     documentation for wxChoice class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxChoice
    @wxheader{choice.h}

    A choice item is used to select one of a list of strings. Unlike a
    listbox, only the selection is visible until the user pulls down the
    menu of choices.

    @beginStyleTable
    @style{wxCB_SORT}:
           Sorts the entries alphabetically.
    @endStyleTable

    @beginEventTable
    @event{EVT_CHOICE(id, func)}:
           Process a wxEVT_COMMAND_CHOICE_SELECTED event, when an item on the
           list is selected.
    @endEventTable

    @library{wxcore}
    @category{ctrl}
    @appearance{choice.png}

    @seealso
    wxListBox, wxComboBox, wxCommandEvent
*/
class wxChoice : public wxControlWithItems
{
public:
    //@{
    /**
        Constructor, creating and showing a choice.
        
        @param parent
            Parent window. Must not be @NULL.
        @param id
            Window identifier. The value wxID_ANY indicates a default value.
        @param pos
            Window position.
        @param size
            Window size. If wxDefaultSize is specified then the choice is
        sized
            appropriately.
        @param n
            Number of strings with which to initialise the choice control.
        @param choices
            An array of strings with which to initialise the choice control.
        @param style
            Window style. See wxChoice.
        @param validator
            Window validator.
        @param name
            Window name.
        
        @see Create(), wxValidator
    */
    wxChoice();
    wxChoice(wxWindow* parent, wxWindowID id,
             const wxPoint& pos,
             const wxSize& size, int n,
             const wxString choices[],
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = "choice");
    wxChoice(wxWindow* parent, wxWindowID id,
             const wxPoint& pos,
             const wxSize& size,
             const wxArrayString& choices,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = "choice");
    //@}

    /**
        Destructor, destroying the choice item.
    */
    ~wxChoice();

    //@{
    /**
        Creates the choice for two-step construction. See wxChoice().
    */
    bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos,
                const wxSize& size, int n,
                const wxString choices[],
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = "choice");
    bool Create(wxWindow* parent, wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = "choice");
    //@}

    /**
        Gets the number of columns in this choice item.
        
        @remarks This is implemented for Motif only and always returns 1 for the
                 other platforms.
    */
    int GetColumns();

    /**
        Unlike wxControlWithItems::GetSelection which only
        returns the accepted selection value, i.e. the selection in the control once
        the user closes the dropdown list, this function returns the current selection.
        That is, while the dropdown list is shown, it returns the currently selected
        item in it. When it is not shown, its result is the same as for the other
        function.
        This function is new since wxWidgets version 2.6.2 (before this version
        wxControlWithItems::GetSelection itself behaved like
        this).
    */
    int GetCurrentSelection();

    /**
        Sets the number of columns in this choice item.
        
        @param n
            Number of columns.
    */
    void SetColumns(int n = 1);
};
