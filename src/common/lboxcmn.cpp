///////////////////////////////////////////////////////////////////////////////
// Name:        common/lboxcmn.cpp
// Purpose:     wxListBox class methods common to all platforms
// Author:      Vadim Zeitlin
// Modified by:
// Created:     22.10.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:   	wxWindows licence
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

wxString wxListBoxBase::GetStringSelection () const
{
    wxString s;
    int sel = GetSelection();
    if ( sel != -1 )
        s = GetString(sel);

    return s;
}

bool wxListBoxBase::SetStringSelection(const wxString& s, bool select)
{
    int sel = FindString(s);
    wxCHECK_MSG( sel != -1, FALSE,
                 wxT("invalid string in wxListBox::SetStringSelection") );

    SetSelection(sel, select);

    return TRUE;
}

// ----------------------------------------------------------------------------
// client data
// ----------------------------------------------------------------------------

void wxListBoxBase::SetClientObject(int n, wxClientData *data)
{
    wxASSERT_MSG( m_clientDataItemsType != ClientData_Void,
                  wxT("can't have both object and void client data") );

    wxClientData *clientDataOld = DoGetClientObject(n);
    if ( clientDataOld )
        delete clientDataOld;

    DoSetClientObject(n, data);
    m_clientDataItemsType = ClientData_Object;
}

wxClientData *wxListBoxBase::GetClientObject(int n) const
{
    wxASSERT_MSG( m_clientDataItemsType == ClientData_Object,
                  wxT("this window doesn't have object client data") );

    return DoGetClientObject(n);
}

void wxListBoxBase::SetClientData(int n, void *data)
{
    wxASSERT_MSG( m_clientDataItemsType != ClientData_Object,
                  wxT("can't have both object and void client data") );

    DoSetClientData(n, data);
    m_clientDataItemsType = ClientData_Void;
}

void *wxListBoxBase::GetClientData(int n) const
{
    wxASSERT_MSG( m_clientDataItemsType == ClientData_Void,
                  wxT("this window doesn't have void client data") );

    return DoGetClientData(n);
}

// ----------------------------------------------------------------------------
// misc
// ----------------------------------------------------------------------------

void wxListBoxBase::SetFirstItem(const wxString& s)
{
    int n = FindString(s);

    wxCHECK_RET( n != -1, wxT("invalid string in wxListBox::SetFirstItem") );

    DoSetFirstItem(n);
}

void wxListBoxBase::Command(wxCommandEvent & event)
{
    SetSelection(event.m_commandInt, event.m_extraLong);
    (void)ProcessEvent(event);
}

