/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/tglbtn.cpp
// Purpose:     Definition of the wxToggleButton class, which implements a
//              toggle button under wxMSW.
// Author:      John Norris, minor changes by Axel Schlueter
//              and William Gallafent.
// Modified by:
// Created:     08.02.01
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Johnny C. Norris II
// License:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_TOGGLEBTN

#include "wx/tglbtn.h"

#ifndef WX_PRECOMP
    #include "wx/button.h"
    #include "wx/brush.h"
    #include "wx/dcscreen.h"
    #include "wx/settings.h"

    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/msw/private.h"
#include "wx/msw/private/button.h"

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
bool wxToggleButton::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxString& label,
                            const wxPoint& pos,
                            const wxSize& size, long style,
                            const wxValidator& validator,
                            const wxString& name)
{
    if ( !CreateControl(parent, id, pos, size, style, validator, name) )
        return false;

    // if the label contains several lines we must explicitly tell the button
    // about it or it wouldn't draw it correctly ("\n"s would just appear as
    // black boxes)
    //
    // NB: we do it here and not in MSWGetStyle() because we need the label
    //     value and the label is not set yet when MSWGetStyle() is called
    WXDWORD exstyle;
    WXDWORD msStyle = MSWGetStyle(style, &exstyle);
    msStyle |= wxMSWButton::GetMultilineStyle(label);

    return MSWCreateControl(_T("BUTTON"), msStyle, pos, size, label, exstyle);
}

WXDWORD wxToggleButton::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    WXDWORD msStyle = wxControl::MSWGetStyle(style, exstyle);

    msStyle |= BS_AUTOCHECKBOX | BS_PUSHLIKE | WS_TABSTOP;

    if ( style & wxBU_LEFT )
      msStyle |= BS_LEFT;
    if ( style & wxBU_RIGHT )
      msStyle |= BS_RIGHT;
    if ( style & wxBU_TOP )
      msStyle |= BS_TOP;
    if ( style & wxBU_BOTTOM )
      msStyle |= BS_BOTTOM;

    return msStyle;
}

wxSize wxToggleButton::DoGetBestSize() const
{
    return wxMSWButton::ComputeBestSize(const_cast<wxToggleButton *>(this));
}

void wxToggleButton::SetLabel(const wxString& label)
{
    wxMSWButton::UpdateMultilineStyle(GetHwnd(), label);

    wxToggleButtonBase::SetLabel(label);
}

void wxToggleButton::SetValue(bool val)
{
   ::SendMessage(GetHwnd(), BM_SETCHECK, val, 0);
}

bool wxToggleButton::GetValue() const
{
    return ::SendMessage(GetHwnd(), BM_GETCHECK, 0, 0) == BST_CHECKED;
}

void wxToggleButton::Command(wxCommandEvent& event)
{
    SetValue(event.GetInt() != 0);
    ProcessCommand(event);
}

bool wxToggleButton::MSWCommand(WXUINT WXUNUSED(param), WXWORD WXUNUSED(id))
{
    wxCommandEvent event(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, m_windowId);
    event.SetInt(GetValue());
    event.SetEventObject(this);
    ProcessCommand(event);
    return true;
}

#endif // wxUSE_TOGGLEBTN
