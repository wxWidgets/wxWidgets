///////////////////////////////////////////////////////////////////////////////
// Name:        common/ctrlsub.cpp
// Purpose:     wxItemContainer implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     22.10.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "controlwithitems.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_CONTROLS

#ifndef WX_PRECOMP
    #include "wx/ctrlsub.h"
    #include "wx/arrstr.h"
#endif

// ============================================================================
// implementation
// ============================================================================

wxItemContainer::~wxItemContainer()
{
    // this destructor is required for Darwin
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------

wxString wxItemContainer::GetStringSelection() const
{
    wxString s;
    int sel = GetSelection();
    if ( sel != -1 )
        s = GetString(sel);

    return s;
}

wxArrayString wxItemContainer::GetStrings() const
{
    wxArrayString result ;
    size_t count = GetCount() ;
    for ( size_t n = 0 ; n < count ; n++ )
        result.Add(GetString(n));
    return result ;
}

// ----------------------------------------------------------------------------
// appending items
// ----------------------------------------------------------------------------

void wxItemContainer::Append(const wxArrayString& strings)
{
    size_t count = strings.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        Append(strings[n]);
    }
}

int wxItemContainer::Insert(const wxString& item, int pos, void *clientData)
{
    int n = DoInsert(item, pos);
    if ( n != wxNOT_FOUND )
        SetClientData(n, clientData);

    return n;
}

int
wxItemContainer::Insert(const wxString& item, int pos, wxClientData *clientData)
{
    int n = DoInsert(item, pos);
    if ( n != wxNOT_FOUND )
        SetClientObject(n, clientData);

    return n;
}

// ----------------------------------------------------------------------------
// client data
// ----------------------------------------------------------------------------

void wxItemContainer::SetClientObject(int n, wxClientData *data)
{
    wxASSERT_MSG( m_clientDataItemsType != wxClientData_Void,
                  wxT("can't have both object and void client data") );

    // when we call SetClientObject() for the first time, m_clientDataItemsType
    // is still wxClientData_None and so calling DoGetItemClientObject() would
    // fail (in addition to being useless) - don't do it
    if ( m_clientDataItemsType == wxClientData_Object )
    {
        wxClientData *clientDataOld = DoGetItemClientObject(n);
        if ( clientDataOld )
            delete clientDataOld;
    }
    else // m_clientDataItemsType == wxClientData_None
    {
        // now we have object client data
        m_clientDataItemsType = wxClientData_Object;
    }

    DoSetItemClientObject(n, data);
}

wxClientData *wxItemContainer::GetClientObject(int n) const
{
    wxASSERT_MSG( m_clientDataItemsType == wxClientData_Object,
                  wxT("this window doesn't have object client data") );

    return DoGetItemClientObject(n);
}

void wxItemContainer::SetClientData(int n, void *data)
{
    wxASSERT_MSG( m_clientDataItemsType != wxClientData_Object,
                  wxT("can't have both object and void client data") );

    DoSetItemClientData(n, data);
    m_clientDataItemsType = wxClientData_Void;
}

void *wxItemContainer::GetClientData(int n) const
{
    wxASSERT_MSG( m_clientDataItemsType == wxClientData_Void,
                  wxT("this window doesn't have void client data") );

    return DoGetItemClientData(n);
}

wxControlWithItems::~wxControlWithItems()
{
    // this destructor is required for Darwin
}

#if WXWIN_COMPATIBILITY_2_2

int wxItemContainer::Number() const
{
    return GetCount();
}

#endif // WXWIN_COMPATIBILITY_2_2

#endif // wxUSE_CONTROLS
