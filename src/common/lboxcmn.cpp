///////////////////////////////////////////////////////////////////////////////
// Name:        common/lboxcmn.cpp
// Purpose:     wxListBox class methods common to all platforms
// Author:      Vadim Zeitlin
// Modified by:
// Created:     22.10.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "listboxbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_LISTBOX

#ifndef WX_PRECOMP
    #include "wx/listbox.h"
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// adding items
// ----------------------------------------------------------------------------

void wxListBoxBase::InsertItems(int nItems, const wxString *items, int pos)
{
    wxArrayString aItems;
    for ( int n = 0; n < nItems; n++ )
    {
        aItems.Add(items[n]);
    }

    DoInsertItems(aItems, pos);
}


void wxListBoxBase::Set(int nItems, const wxString* items, void **clientData)
{
    wxArrayString aItems;
    for ( int n = 0; n < nItems; n++ )
    {
        aItems.Add(items[n]);
    }

    DoSetItems(aItems, clientData);
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------

bool wxListBoxBase::SetStringSelection(const wxString& s, bool select)
{
    int sel = FindString(s);
    wxCHECK_MSG( sel != -1, FALSE,
                 wxT("invalid string in SetStringSelection") );

    SetSelection(sel, select);

    return TRUE;
}

void wxListBoxBase::DeselectAll(int itemToLeaveSelected)
{
    if ( HasMultipleSelection() )
    {
        wxArrayInt selections;
        GetSelections(selections);

        size_t count = selections.GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            int item = selections[n];
            if ( item != itemToLeaveSelected )
                Deselect(item);
        }
    }
    else // single selection
    {
        int sel = GetSelection();
        if ( sel != -1 && sel != itemToLeaveSelected )
        {
            Deselect(sel);
        }
    }
}

// ----------------------------------------------------------------------------
// misc
// ----------------------------------------------------------------------------

void wxListBoxBase::Command(wxCommandEvent& event)
{
    SetSelection(event.m_commandInt, event.m_extraLong != 0);
    (void)ProcessEvent(event);
}

void wxListBoxBase::SetFirstItem(const wxString& s)
{
    int n = FindString(s);

    wxCHECK_RET( n != -1, wxT("invalid string in wxListBox::SetFirstItem") );

    DoSetFirstItem(n);
}

#endif // wxUSE_LISTBOX
