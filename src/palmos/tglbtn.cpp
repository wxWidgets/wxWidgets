/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/tglbtn.cpp
// Purpose:     Definition of the wxToggleButton class, which implements a
//              toggle button.
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by: Wlodzimierz ABX Skiba - native implementation
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne, Wlodzimierz Skiba
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declatations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_TOGGLEBTN

#ifndef WX_PRECOMP
    #include "wx/button.h"
    #include "wx/brush.h"
    #include "wx/dcscreen.h"
    #include "wx/settings.h"

    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/tglbtn.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxToggleButton, wxControl)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxToggleButton
// ----------------------------------------------------------------------------

// Single check box item
bool wxToggleButton::Create(wxWindow *parent, wxWindowID id,
                            const wxString& label,
                            const wxPoint& pos,
                            const wxSize& size, long style,
                            const wxValidator& validator,
                            const wxString& name)
{
    wxControl::PalmCreateControl(pushButtonCtl, parent, id, label, pos, size);
    return true;
}

wxBorder wxToggleButton::GetDefaultBorder() const
{
    return wxBORDER_NONE;
}

wxSize wxToggleButton::DoGetBestSize() const
{
   return wxSize(0,0);
}

void wxToggleButton::SetValue(bool val)
{
}

bool wxToggleButton::GetValue() const
{
    return false;
}

void wxToggleButton::Command(wxCommandEvent & event)
{
}

#endif // wxUSE_TOGGLEBTN

