///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/listboxitem.h
// Purpose:     Declaration of the wxListBoxItem class.
// Author:      Vadim Zeitlin
// Created:     2024-10-27 (extracted from src/msw/listbox.cpp)
// Copyright:   (c) 2024 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_LISTBOXITEM_H_
#define _WX_MSW_PRIVATE_LISTBOXITEM_H_

#include "wx/ownerdrw.h"

// ----------------------------------------------------------------------------
// wxListBoxItemBase: base class for wxListBoxItem and wxCheckListBoxItem
// ----------------------------------------------------------------------------

// The template parameter is the control containing these items.
template <typename T>
class wxListBoxItemBase : public wxOwnerDrawn
{
public:
    using Control = T;

    explicit wxListBoxItemBase(Control *parent)
        { m_parent = parent; }

    Control *GetParent() const
        { return m_parent; }

    int GetIndex() const
        { return m_parent->GetItemIndex(const_cast<wxListBoxItemBase*>(this)); }

    wxString GetName() const override
        { return m_parent->GetString(GetIndex()); }

protected:
    void
    GetColourToUse(wxODStatus stat,
                   wxColour& colText,
                   wxColour& colBack) const override
    {
        wxOwnerDrawn::GetColourToUse(stat, colText, colBack);

        // Default background colour for the owner drawn items is the menu one,
        // but it's not appropriate for the listboxes, so override it here.
        if ( !(stat & wxODSelected) && !GetBackgroundColour().IsOk() )
            colBack = wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX);
    }

private:
    Control *m_parent;
};

#endif // _WX_MSW_PRIVATE_LISTBOXITEM_H_
