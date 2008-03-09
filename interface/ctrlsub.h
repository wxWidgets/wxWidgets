/////////////////////////////////////////////////////////////////////////////
// Name:        ctrlsub.h
// Purpose:     documentation for wxControlWithItems class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxControlWithItems
    @wxheader{ctrlsub.h}

    This class is an abstract base class for some wxWidgets controls which contain
    several items, such as wxListBox and
    wxCheckListBox derived from it,
    wxChoice and wxComboBox.

    It defines the methods for accessing the controls items and although each of
    the derived classes implements them differently, they still all conform to the
    same interface.

    The items in a wxControlWithItems have (non-empty) string labels and,
    optionally, client data associated with them. Client data may be of two
    different kinds: either simple untyped (@c void *) pointers which are simply
    stored by the control but not used in any way by it, or typed pointers
    (@c wxClientData *) which are owned by the control meaning that the typed
    client data (and only it) will be deleted when an item is
    @ref wxControlWithItems::delete deleted or the entire control is
    @ref wxControlWithItems::clear cleared (which also happens when it is
    destroyed). Finally note that in the same control all items must have client
    data of the same type (typed or untyped), if any. This type is determined by
    the first call to wxControlWithItems::Append (the version with
    client data pointer) or wxControlWithItems::SetClientData.

    @library{wxcore}
    @category{FIXME}

    @seealso
    wxControlWithItems::Clear
*/
class wxControlWithItems : public wxControl
{
public:
    //@{
    /**
        Appends several items at once to the control. Notice that calling this method
        is usually much faster than appending them one by one if you need to add a lot
        of items.
        
        @param item
            String to add.
        @param stringsArray
            Contains items to append to the control.
        @param strings
            Array of strings of size n.
        @param n
            Number of items in the strings array.
        @param clientData
            Array of client data pointers of size n to associate with the new items.
        
        @returns When appending a single item, the return value is the index of
                 the newly added item which may be different from the
                 last one if the control is sorted (e.g. has wxLB_SORT
                 or wxCB_SORT style).
    */
    int Append(const wxString& item);
    int Append(const wxString& item, void* clientData);
    int Append(const wxString& item, wxClientData* clientData);
    void Append(const wxArrayString& strings);
    void Append(unsigned int n, const wxString* strings);
    void Append(unsigned int n, const wxString* strings,
                void** clientData);
    void Append(unsigned int n, const wxString* strings,
                wxClientData** clientData);
    //@}

    /**
        Removes all items from the control.
        @e Clear() also deletes the client data of the existing items if it is owned
        by the control.
    */
    void Clear();

    /**
        Deletes an item from the control. The client data associated with the item
        will be also deleted if it is owned by the control.
        Note that it is an error (signalled by an assert failure in debug builds) to
        remove an item with the index negative or greater or equal than the number of
        items in the control.
        
        @param n
            The zero-based item index.
        
        @see Clear()
    */
    void Delete(unsigned int n);

    /**
        Finds an item whose label matches the given string.
        
        @param string
            String to find.
        @param caseSensitive
            Whether search is case sensitive (default is not).
        
        @returns The zero-based position of the item, or wxNOT_FOUND if the
                 string was not found.
    */
    int FindString(const wxString& string,
                   bool caseSensitive = false);

    /**
        Returns a pointer to the client data associated with the given item (if any).
        It is an error to call this function for a control which doesn't have untyped
        client data at all although it is ok to call it even if the given item doesn't
        have any client data associated with it (but other items do).
        
        @param n
            The zero-based position of the item.
        
        @returns A pointer to the client data, or @NULL if not present.
    */
    void* GetClientData(unsigned int n) const;

    /**
        Returns a pointer to the client data associated with the given item (if any).
        It is an error to call this function for a control which doesn't have typed
        client data at all although it is ok to call it even if the given item doesn't
        have any client data associated with it (but other items do).
        
        @param n
            The zero-based position of the item.
        
        @returns A pointer to the client data, or @NULL if not present.
    */
    wxClientData* GetClientObject(unsigned int n) const;

    /**
        Returns the number of items in the control.
        
        @see IsEmpty()
    */
    unsigned int GetCount() const;

    /**
        Returns the index of the selected item or @c wxNOT_FOUND if no item is
        selected.
        
        @returns The position of the current selection.
        
        @remarks This method can be used with single selection list boxes only,
                 you should use wxListBox::GetSelections for the list
                 boxes with wxLB_MULTIPLE style.
        
        @see SetSelection(), GetStringSelection()
    */
    int GetSelection() const;

    /**
        Returns the label of the item with the given index.
        
        @param n
            The zero-based index.
        
        @returns The label of the item or an empty string if the position was
                 invalid.
    */
    wxString GetString(unsigned int n) const;

    /**
        Returns the label of the selected item or an empty string if no item is
        selected.
        
        @see GetSelection()
    */
    wxString GetStringSelection() const;

    /**
        Returns the array of the labels of all items in the control.
    */
    wxArrayString GetStrings() const;

    //@{
    /**
        Inserts several items at once into the control. Notice that calling this method
        is usually much faster than inserting them one by one if you need to insert a
        lot
        of items.
        
        @param item
            String to add.
        @param pos
            Position to insert item before, zero based.
        @param stringsArray
            Contains items to insert into the control content
        @param strings
            Array of strings of size n.
        @param n
            Number of items in the strings array.
        @param clientData
            Array of client data pointers of size n to associate with the new items.
        
        @returns The return value is the index of the newly inserted item. If the
                 insertion failed for some reason, -1 is returned.
    */
    int Insert(const wxString& item, unsigned int pos);
    int Insert(const wxString& item, unsigned int pos,
               void* clientData);
    int Insert(const wxString& item, unsigned int pos,
               wxClientData* clientData);
    void Insert(const wxArrayString& strings, unsigned int pos);
    void Insert(const wxArrayString& strings, unsigned int pos);
    void Insert(unsigned int n, const wxString* strings,
                unsigned int pos);
    void Insert(unsigned int n, const wxString* strings,
                unsigned int pos,
                void** clientData);
    void Insert(unsigned int n, const wxString* strings,
                unsigned int pos,
                wxClientData** clientData);
    //@}

    /**
        Returns @true if the control is empty or @false if it has some items.
        
        @see GetCount()
    */
    bool IsEmpty() const;

    /**
        This is the same as SetSelection() and
        exists only because it is slightly more natural for controls which support
        multiple selection.
    */
    void Select(int n);

    //@{
    /**
        Replaces the current control contents with the given items. Notice that calling
        this method is much faster than appending the items one by one if you need to
        append a lot of them.
        
        @param item
            The single item to insert into the control.
        @param stringsArray
            Contains items to set as control content.
        @param strings
            Raw C++ array of strings. Only used in conjunction with 'n'.
        @param n
            Number of items passed in 'strings'. Only used in conjunction with
        'strings'.
        @param clientData
            Client data to associate with the item(s).
        
        @returns When the control is sorted (e.g. has wxLB_SORT or wxCB_SORT
                 style) the return value could be different from
                 (GetCount() - 1). When setting a single item to the
                 container, the return value is the index of the newly
                 added item which may be different from the last one if
                 the control is sorted (e.g. has wxLB_SORT or wxCB_SORT
                 style).
    */
    int Set(const wxString& item);
    int Set(const wxString& item, void* clientData);
    int Set(const wxString& item, wxClientData* clientData);
    void Set(const wxArrayString& stringsArray);
    void Set(unsigned int n, const wxString* strings);
    void Set(unsigned int n, const wxString* strings,
             void** clientData);
    void Set(unsigned int n, const wxString* strings,
             wxClientData** clientData);
    //@}

    /**
        Associates the given untyped client data pointer with the given item. Note that
        it is an error to call this function if any typed client data pointers had been
        associated with the control items before.
        
        @param n
            The zero-based item index.
        @param data
            The client data to associate with the item.
    */
    void SetClientData(unsigned int n, void* data);

    /**
        Associates the given typed client data pointer with the given item: the
        @a data object will be deleted when the item is deleted (either explicitly
        by using @ref delete() Deletes or implicitly when the
        control itself is destroyed).
        Note that it is an error to call this function if any untyped client data
        pointers had been associated with the control items before.
        
        @param n
            The zero-based item index.
        @param data
            The client data to associate with the item.
    */
    void SetClientObject(unsigned int n, wxClientData* data);

    /**
        Sets the selection to the given item @a n or removes the selection entirely
        if @a n == @c wxNOT_FOUND.
        Note that this does not cause any command events to be emitted nor does it
        deselect any other items in the controls which support multiple selections.
        
        @param n
            The string position to select, starting from zero.
        
        @see SetString(), SetStringSelection()
    */
    void SetSelection(int n);

    /**
        Sets the label for the given item.
        
        @param n
            The zero-based item index.
        @param string
            The label to set.
    */
    void SetString(unsigned int n, const wxString& string);

    /**
        Selects the item with the specified string in the control. This doesn't cause
        any command events to be emitted.
        
        @param string
            The string to select.
        
        @returns @true if the specified string has been selected, @false if it
                 wasn't found in the control.
    */
    bool SetStringSelection(const wxString& string);
};
