///////////////////////////////////////////////////////////////////////////////
// Name:        common/ctrlsub.cpp
// Purpose:     wxItemContainer implementation
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
#endif

// ============================================================================
// implementation
// ============================================================================

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

// ----------------------------------------------------------------------------
// client data
// ----------------------------------------------------------------------------

void wxItemContainer::SetClientObject(int n, wxClientData *data)
{
    wxASSERT_MSG( m_clientDataItemsType != wxClientData_Void,
                  wxT("can't have both object and void client data") );

    wxClientData *clientDataOld = DoGetItemClientObject(n);
    if ( clientDataOld )
        delete clientDataOld;

    DoSetItemClientObject(n, data);
    m_clientDataItemsType = wxClientData_Object;
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

#endif // wxUSE_CONTROLS
