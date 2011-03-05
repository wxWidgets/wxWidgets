/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/checkbox.cpp
// Purpose:     wxCheckBox
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by: Wlodzimierz ABX Skiba - native implementation
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne, Wlodzimierz Skiba
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_CHECKBOX

#include "wx/checkbox.h"

#ifndef WX_PRECOMP
    #include "wx/brush.h"
    #include "wx/dcscreen.h"
    #include "wx/settings.h"
#endif

#include <Control.h>

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxCheckBox
// ----------------------------------------------------------------------------

bool wxCheckBox::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& label,
                        const wxPoint& pos,
                        const wxSize& size, long style,
                        const wxValidator& validator,
                        const wxString& name)
{
    if(!wxControl::Create(parent, id, pos, size, style, validator, name))
        return false;

    m_state = wxCHK_UNCHECKED;
    return wxControl::PalmCreateControl(checkboxCtl, label, pos, size);
}

wxSize wxCheckBox::DoGetBestSize() const
{
    return wxSize(0,0);
}

void wxCheckBox::SetValue(bool val)
{
    SetBoolValue(val);
}

bool wxCheckBox::GetValue() const
{
    return GetBoolValue();
}

bool wxCheckBox::SendClickEvent()
{
    wxCommandEvent event(wxEVT_COMMAND_CHECKBOX_CLICKED, GetId());
    event.SetInt(GetValue());
    event.SetEventObject(this);
    return ProcessCommand(event);
}

void wxCheckBox::Command(wxCommandEvent& event)
{
    int state = event.GetInt();
    wxCHECK_RET( (state == wxCHK_UNCHECKED) || (state == wxCHK_CHECKED)
        || (state == wxCHK_UNDETERMINED),
        wxT("event.GetInt() returned an invalid checkbox state") );

    Set3StateValue((wxCheckBoxState) state);
    ProcessCommand(event);
}

void wxCheckBox::DoSet3StateValue(wxCheckBoxState state)
{
    Int16 newValue;
    ControlType *control = (ControlType *)GetObjectPtr();
    if(NULL == control) {
        return;
    }
    m_state = state;
    switch (state) {
    case wxCHK_UNCHECKED:
        newValue = 0;
        break;
    case wxCHK_CHECKED:
        newValue = 1;
        break;
    case wxCHK_UNDETERMINED:
    default:
        return;
        break;
    }
    CtlSetValue (control, newValue);
}

wxCheckBoxState wxCheckBox::DoGet3StateValue() const
{
    return m_state;
}

#endif // wxUSE_CHECKBOX
