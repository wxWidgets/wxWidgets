/////////////////////////////////////////////////////////////////////////////
// Name:        common/choiccmn.cpp
// Purpose:     common (to all ports) wxChoice functions
// Author:      Vadim Zeitlin
// Modified by:
// Created:     26.07.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "choicebase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/choice.h"
    #include "wx/log.h"
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// events
// ----------------------------------------------------------------------------

void wxChoiceBase::Command(wxCommandEvent &event)
{
    SetSelection(event.GetInt());
    (void)ProcessEvent(event);
}

// ----------------------------------------------------------------------------
// string selection management
// ----------------------------------------------------------------------------

wxString wxChoiceBase::GetStringSelection() const
{
    int sel = GetSelection();
    wxString str;
    wxCHECK_MSG( sel != wxNOT_FOUND, str, _T("no selection, hence no string") );

    str = GetString(sel);
    return str;
}

bool wxChoiceBase::SetStringSelection(const wxString& sel)
{
    int selIndex = FindString(sel);
    wxCHECK_MSG( selIndex != wxNOT_FOUND, FALSE,
                 _T("can't set selection to string not in the control") );

    SetSelection(selIndex);

    return TRUE;
}

// ----------------------------------------------------------------------------
// client data
// ----------------------------------------------------------------------------

void wxChoiceBase::SetClientObject(int n, wxClientData *data)
{
    wxASSERT_MSG( m_clientDataItemsType != ClientData_Void,
                  _T("can't have both object and void client data") );

    wxClientData *clientDataOld = DoGetClientObject(n);
    if ( clientDataOld )
        delete clientDataOld;

    DoSetClientObject(n, data);
    m_clientDataItemsType = ClientData_Object;
}

wxClientData *wxChoiceBase::GetClientObject(int n) const
{
    wxASSERT_MSG( m_clientDataItemsType == ClientData_Object,
                  _T("this window doesn't have object client data") );

    return DoGetClientObject(n);
}

void wxChoiceBase::SetClientData(int n, void *data)
{
    wxASSERT_MSG( m_clientDataItemsType != ClientData_Object,
                  _T("can't have both object and void client data") );

    DoSetClientData(n, data);
    m_clientDataItemsType = ClientData_Void;
}

void *wxChoiceBase::GetClientData(int n) const
{
    wxASSERT_MSG( m_clientDataItemsType == ClientData_Void,
                  _T("this window doesn't have void client data") );

    return DoGetClientData(n);
}


