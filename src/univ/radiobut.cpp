/////////////////////////////////////////////////////////////////////////////
// Name:        univ/radiobut.cpp
// Purpose:     wxRadioButton implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     10.09.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "univradiobut.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_RADIOBTN

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/radiobut.h"
    #include "wx/validate.h"
#endif

#include "wx/univ/theme.h"
#include "wx/univ/renderer.h"
#include "wx/univ/inphand.h"
#include "wx/univ/colschem.h"

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxRadioButton, wxControl)

// ----------------------------------------------------------------------------
// wxRadioButton
// ----------------------------------------------------------------------------

bool wxRadioButton::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString &label,
                        const wxPoint &pos,
                        const wxSize &size,
                        long style,
                        const wxValidator& validator,
                        const wxString &name)
{
    if ( !wxCheckBox::Create(parent, id, label, pos, size, style,
                             wxDefaultValidator, name) )
    {
        return FALSE;
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// radio button methods
// ----------------------------------------------------------------------------

void wxRadioButton::ChangeValue(bool value)
{
    if ( !IsChecked() && value )
    {
        // TODO clear all all others in our group!

        SetValue(TRUE);

        SendEvent();
    }
    //else: either nothign chanegd or an attempt was made to clear a checked
    //      radio button which can't be done
}

void wxRadioButton::ClearValue()
{
    if ( IsChecked() )
    {
        SetValue(FALSE);
    }
}

void wxRadioButton::SendEvent()
{
    wxCommandEvent event(wxEVT_COMMAND_RADIOBUTTON_SELECTED, GetId());
    InitCommandEvent(event);
    event.SetInt(IsChecked());
    Command(event);
}

// ----------------------------------------------------------------------------
// indicator bitmaps
// ----------------------------------------------------------------------------

wxBitmap wxRadioButton::GetBitmap(State state, Status status) const
{
    // radio buttons don't have "undetermined" status
    wxASSERT_MSG( status == Status_Checked || status == Status_Unchecked,
                  _T("invalid status for a radio button") );

    wxBitmap bmp = DoGetBitmap(state, status);
    if ( !bmp.Ok() )
        bmp = DoGetBitmap(State_Normal, status);
    if ( !bmp.Ok() )
        bmp = wxTheme::Get()->GetColourScheme()->GetRadioBitmap(state, status);

    return bmp;
}

#endif // wxUSE_RADIOBTN

