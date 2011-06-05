/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/generic/listbox.h
// Purpose:     wxMoListBox class
// Author:      Julian Smart
// Modified by:
// Created:     2009-06-07
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWidgets Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_GENERIC_LISTBOX_H_
#define _WX_MOBILE_GENERIC_LISTBOX_H_

// ----------------------------------------------------------------------------
// headers and constants
// ----------------------------------------------------------------------------

#include "wx/mobile/generic/wheelsctrl.h"

/**
    @class wxMoListBox

    A listbox implementation using a wheels control.
    This may change, since the picker control should normally
    be at the bottom of the screen. We could instead use
    a text control and a button to indicate there are further
    values, and make it similar to a combobox control.

    @category{wxMobile}
*/

class WXDLLEXPORT wxMoListBox: public wxMoWheelsCtrl
{
public:
    /// Default constructor.
    wxMoListBox() { Init(); }

    /// Constructor.
    wxMoListBox(wxWindow *parent,
               wxWindowID id = wxID_ANY,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               const wxArrayString& strings = wxArrayString(),
               long style = wxLB_SINGLE,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxMoVListBoxNameStr)
    {
        Init();

        (void)Create(parent, id, pos, size, strings, style, validator, name);
    }

    /// Creation function.
    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                const wxArrayString& strings = wxArrayString(),
                long style = wxLB_SINGLE,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxMoVListBoxNameStr);

    virtual ~wxMoListBox () {}

    /// Appends a string to the end of the list box.
    void Append(const wxString& str);

    /// Inserts a string in front of the given item.
    int Insert(const wxString& str, int pos);

    /// Clears and sets the strings in the listbox.
    void Set(const wxArrayString& strings);

    /// Clears the listbox.
    void Clear();

    /// Gets the string corresponding to the selection, if in single-selection mode.
    wxString GetStringSelection() const;

    /// Sets the selection by string, if in single-selection mode.
    void SetStringSelection(const wxString& str);

    /// Gets the string at the given position.
    wxString GetString(int n) const;

    /// Sets the string at the given position.
    void SetString(int n, const wxString& str);

    /// Gets all the strings in the listbox.
    wxArrayString GetStrings() const;

    /// Returns the number of strings in the listbox.
    int GetCount() const;

    /// Inserts items into the listbox at the given position.
    void InsertItems(const wxArrayString& items, int pos);

    /// Gets the selection indices if the listbox is in multiple selection mode.
    int GetSelections(wxArrayInt& selections) const;

protected:
    void OnWheelSelected(wxWheelsCtrlEvent& event);

    virtual wxSize DoGetBestSize() const;

    wxMoWheelsTextDataSource* GetTextDataSource() const;

    // common part of all ctors
    void Init();

    DECLARE_CLASS(wxMoListBox)
    DECLARE_EVENT_TABLE()
};

#endif // _WX_MOBILE_GENERIC_LISTBOX_H_

