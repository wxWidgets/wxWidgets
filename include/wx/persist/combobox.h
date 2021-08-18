///////////////////////////////////////////////////////////////////////////////
// Name:        wx/persist/combobox.h
// Purpose:     Persistence adapter for wxComboBox
// Author:      Vadim Zeitlin
// Created:     2020-11-19
// Copyright:   (c) 2020 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PERSIST_COMBOBOX_H_
#define _WX_PERSIST_COMBOBOX_H_

#include "wx/persist/window.h"

#if wxUSE_COMBOBOX

#include "wx/combobox.h"

#define wxPERSIST_COMBOBOX_KIND wxASCII_STR("Combobox")

// Stores semicolon-separated list of combobox entries (real semicolons are
// escaped using backslash), i.e. "first;second;...".
#define wxPERSIST_COMBOBOX_ITEMS wxASCII_STR("Items")

#define wxPERSIST_COMBOBOX_ITEMS_SEP ';'

// ----------------------------------------------------------------------------
// wxPersistentComboBox: supports saving/restoring combobox items
// ----------------------------------------------------------------------------

class wxPersistentComboBox : public wxPersistentWindow<wxComboBox>
{
public:
    // The maximum number of items to save is currently hard-coded.
    //
    // Notice that we must have some limit, as otherwise the length of the
    // items string in the config would be unbounded, which certainly wouldn't
    // be a good idea.
    enum { MaxSavedItemsCount = 10 };

    explicit wxPersistentComboBox(wxComboBox* combobox)
        : wxPersistentWindow<wxComboBox>(combobox)
    {
    }

    virtual void Save() const wxOVERRIDE
    {
        const wxComboBox* const combobox = Get();

        wxArrayString items = combobox->GetStrings();

        const wxString value = combobox->GetValue();
        if ( !value.empty() )
        {
            wxArrayString::iterator it;
            for ( it = items.begin(); it != items.end(); ++it )
            {
                if ( *it == value )
                {
                    // There is no need to add the text of an item already
                    // present in the combobox again, but do move it to the top
                    // of it to indicate that it was the last one used.
                    wxSwap(*items.begin(), *it);
                    break;
                }
            }

            if ( it == items.end() )
            {
                // This is a genuinely new item, so just insert it front.
                items.insert(items.begin(), value);

                if ( items.size() > MaxSavedItemsCount )
                    items.erase(items.begin() + MaxSavedItemsCount, items.end());
            }
        }

        SaveValue(wxPERSIST_COMBOBOX_ITEMS,
                  wxJoin(items, wxPERSIST_COMBOBOX_ITEMS_SEP));
    }

    virtual bool Restore() wxOVERRIDE
    {
        wxString items;
        if ( !RestoreValue(wxPERSIST_COMBOBOX_ITEMS, &items) )
            return false;

        Get()->Set(wxSplit(items, wxPERSIST_COMBOBOX_ITEMS_SEP));

        return true;
    }

    virtual wxString GetKind() const wxOVERRIDE { return wxPERSIST_COMBOBOX_KIND; }
};

inline wxPersistentObject *wxCreatePersistentObject(wxComboBox* combobox)
{
    return new wxPersistentComboBox(combobox);
}

#endif // wxUSE_COMBOBOX

#endif // _WX_PERSIST_COMBOBOX_H_
