///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/sizer.h
// Purpose:     Private sizer-related helpers
// Author:      Vadim Zeitlin
// Created:     2026-11-13
// Copyright:   (c) 2026 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_SIZER_H_
#define _WX_PRIVATE_SIZER_H_

#include "wx/sizer.h"

#include <vector>

// ----------------------------------------------------------------------------
// wxSizerPretendAllItemsAreShown: make sizer take hidden items into account
// ----------------------------------------------------------------------------

// Sizers ignore the hidden items, which is usually what we want, but sometimes
// it is necessary to compute the size that the sizer would have if all of its
// items were shown, e.g. when the windows are hidden only temporarily.
//
// Creating an object of this class makes the given sizer, as well as all the
// sizers nested inside it, consider all of their items as shown until it is
// destroyed, without actually showing them. This is done by temporarily adding
// wxRESERVE_SPACE_EVEN_IF_HIDDEN to the flags of all items.
//
// Note that the sizer items must not be added or removed while the object of
// this class exists.
class wxSizerPretendAllItemsAreShown
{
public:
    explicit wxSizerPretendAllItemsAreShown(wxSizer* sizer)
    {
        DoApply(sizer);
    }

    ~wxSizerPretendAllItemsAreShown()
    {
        for ( const ItemFlags& itemFlags : m_origFlags )
            itemFlags.item->SetFlag(itemFlags.flag);
    }

private:
    // Original flag of a sizer item modified by this object.
    struct ItemFlags
    {
        wxSizerItem* item;
        int flag;
    };

    void DoApply(wxSizer* sizer)
    {
        for ( wxSizerItem* item : sizer->GetChildren() )
        {
            const int flag = item->GetFlag();
            m_origFlags.push_back({item, flag});
            item->SetFlag(flag | wxRESERVE_SPACE_EVEN_IF_HIDDEN);

            if ( wxSizer* const subsizer = item->GetSizer() )
                DoApply(subsizer);
        }
    }

    // Original flags of all the items we modified.
    std::vector<ItemFlags> m_origFlags;

    wxDECLARE_NO_COPY_CLASS(wxSizerPretendAllItemsAreShown);
};

#endif // _WX_PRIVATE_SIZER_H_
