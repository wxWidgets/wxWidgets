/////////////////////////////////////////////////////////////////////////////
// Name:        listbox.h
// Purpose:     interface of wxListBox
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxListBox

    A listbox is used to select one or more of a list of strings.

    The strings are displayed in a scrolling box, with the selected string(s)
    marked in reverse video. A listbox can be single selection (if an item is
    selected, the previous selection is removed) or multiple selection
    (clicking an item toggles the item on or off independently of other
    selections).

    List box elements are numbered from zero and while the maximal number of
    elements is unlimited, it is usually better to use a virtual control, not
    requiring to add all the items to it at once, such as wxDataViewCtrl or
    wxListCtrl with @c wxLC_VIRTUAL style, once more than a few hundreds items
    need to be displayed because this control is not optimized, neither from
    performance nor from user interface point of view, for large number of
    items.

    Notice that currently @c TAB characters in list box items text are not
    handled consistently under all platforms, so they should be replaced by
    spaces to display strings properly everywhere. The list box doesn't
    support any other control characters at all.

    @beginStyleTable
    @style{wxLB_SINGLE}
        Single-selection list.
    @style{wxLB_MULTIPLE}
        Multiple-selection list: the user can toggle multiple items on and off.
        This is the same as wxLB_EXTENDED in wxGTK2 port.
    @style{wxLB_EXTENDED}
        Extended-selection list: the user can extend the selection by using
        @c SHIFT or @c CTRL keys together with the cursor movement keys or
        the mouse.
    @style{wxLB_HSCROLL}
        Create horizontal scrollbar if contents are too wide (Windows only).
    @style{wxLB_ALWAYS_SB}
        Always show a vertical scrollbar.
    @style{wxLB_NEEDED_SB}
        Only create a vertical scrollbar if needed.
    @style{wxLB_NO_SB}
        Don't create vertical scrollbar (wxMSW only).
    @style{wxLB_SORT}
        The listbox contents are sorted in alphabetical order.
    @endStyleTable

    Note that @c wxLB_SINGLE, @c wxLB_MULTIPLE and @c wxLB_EXTENDED styles are
    mutually exclusive and you can specify at most one of them (single selection
    is the default). See also @ref overview_windowstyles.

    @beginEventEmissionTable{wxCommandEvent}
    @event{EVT_LISTBOX(id, func)}
        Process a @c wxEVT_LISTBOX event, when an item on the
        list is selected or the selection changes.
    @event{EVT_LISTBOX_DCLICK(id, func)}
        Process a @c wxEVT_LISTBOX_DCLICK event, when the listbox
        is double-clicked.
    @endEventTable

    @library{wxcore}
    @category{ctrl}
    @appearance{listbox}

    @see wxEditableListBox, wxChoice, wxComboBox, wxListCtrl, wxCommandEvent
*/
class wxListBox : public wxControl,
                  public wxItemContainer
{
public:
    /**
        Default constructor.
    */
    wxListBox();

    /**
        Constructor, creating and showing a list box.

        @param parent
            The parent window.
        @param id
            The ID of this control. A value of @c wxID_ANY indicates a default value.
        @param pos
            The initial position.
            If ::wxDefaultPosition is specified then a default position is chosen.
        @param size
            The initial size.
            If ::wxDefaultSize is specified then the window is sized appropriately.
        @param n
            Number of strings with which to initialise the control.
        @param choices
            The strings to use to initialize the control.
        @param style
            Window style. See wxListBox.
        @param validator
            The validator for this control.
        @param name
            The name of this class.

        @beginWxPerlOnly
        Not supported by wxPerl.
        @endWxPerlOnly
    */

    wxListBox(wxWindow* parent, wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              int n = 0,
              const wxString choices[] = NULL,
              long style = 0,
              const wxValidator& validator = wxDefaultValidator,
              const wxString& name = wxListBoxNameStr);

    /**
        Constructor, creating and showing a list box.

        See the other wxListBox() constructor; the only difference is that
        this overload takes a wxArrayString instead of a pointer to an array
        of wxString.

        @beginWxPerlOnly
        Use an array reference for the @a choices parameter.
        @endWxPerlOnly
    */

    wxListBox(wxWindow* parent, wxWindowID id,
              const wxPoint& pos,
              const wxSize& size,
              const wxArrayString& choices,
              long style = 0,
              const wxValidator& validator = wxDefaultValidator,
              const wxString& name = wxListBoxNameStr);

    /**
        Destructor, destroying the list box.
    */
    virtual ~wxListBox();

    //@{
    /**
        Creates the listbox for two-step construction.
        See wxListBox() for further details.
    */
    bool Create(wxWindow *parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0, const wxString choices[] = NULL,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxListBoxNameStr);
    bool Create(wxWindow *parent, wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxListBoxNameStr);
    //@}

    /**
        Deselects an item in the list box.

        @param n
            The zero-based item to deselect.

        @remarks This applies to multiple selection listboxes only.
    */
    void Deselect(int n);

    virtual void SetSelection(int n);
    
    virtual int GetSelection() const;
    
    virtual bool SetStringSelection(const wxString& s, bool select);
    virtual bool SetStringSelection(const wxString& s);

    /**
        Fill an array of ints with the positions of the currently selected items.

        @param selections
            A reference to an wxArrayInt instance that is used to store the result of
            the query.

        @return The number of selections.

        @remarks Use this with a multiple selection listbox.

        @beginWxPerlOnly
        In wxPerl this method takes no parameters and return the
        selected items as a list.
        @endWxPerlOnly

        @see wxControlWithItems::GetSelection, wxControlWithItems::GetStringSelection,
             wxControlWithItems::SetSelection
    */
    virtual int GetSelections(wxArrayInt& selections) const;

    /**
        Returns the item located at @a point, or @c wxNOT_FOUND if there
        is no item located at @a point.

        It is currently implemented for wxMSW, wxMac and wxGTK2 ports.

        @param point
            Point of item (in client coordinates) to obtain

        @return Item located at point, or wxNOT_FOUND if unimplemented or the
                item does not exist.

        @since 2.7.0
    */
    int HitTest(const wxPoint& point) const;

    /**
        @overload
    */
    int HitTest(int x, int y) const;

    /**
        Insert the given number of strings before the specified position.

        @param nItems
            Number of items in the array items
        @param items
            Labels of items to be inserted
        @param pos
            Position before which to insert the items: if pos is 0 the
            items will be inserted in the beginning of the listbox

        @beginWxPerlOnly
        Not supported by wxPerl.
        @endWxPerlOnly
    */
    void InsertItems(unsigned int nItems, const wxString *items,
                     unsigned int pos);

    /**
        Insert the given number of strings before the specified position.

        @param items
            Labels of items to be inserted
        @param pos
            Position before which to insert the items: if pos is @c 0 the
            items will be inserted in the beginning of the listbox

        @beginWxPerlOnly
        Use an array reference for the @a items parameter.
        @endWxPerlOnly
    */
    void InsertItems(const wxArrayString& items,
                     unsigned int pos);

    /**
        Determines whether an item is selected.

        @param n
            The zero-based item index.

        @return @true if the given item is selected, @false otherwise.
    */
    virtual bool IsSelected(int n) const;

    /**
        Set the specified item to be the first visible item.

        @param n
            The zero-based item index that should be visible.
    */
    void SetFirstItem(int n);

    /**
        Set the specified item to be the first visible item.

        @param string
            The string that should be visible.
    */
    void SetFirstItem(const wxString& string);

    /**
        Ensure that the item with the given index is currently shown.

        Scroll the listbox if necessary.

        This method is currently only implemented in wxGTK and wxOSX and does
        nothing in other ports.

        @see SetFirstItem()
     */
    virtual void EnsureVisible(int n);

    /**
        Return true if the listbox has ::wxLB_SORT style.

        This method is mostly meant for internal use only.
     */
    virtual bool IsSorted() const;


    // NOTE: Phoenix needs to see the implementation of pure virtuals so it
    // knows that this class is not abstract.
    virtual unsigned int GetCount() const; 
    virtual wxString GetString(unsigned int n) const; 
    virtual void SetString(unsigned int n, const wxString& s); 
    virtual int FindString(const wxString& s, bool bCase = false) const;     
};

