/////////////////////////////////////////////////////////////////////////////
// Name:        msw/radiobut.cpp
// Purpose:     wxRadioButton
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "radiobut.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_RADIOBTN

#ifndef WX_PRECOMP
    #include "wx/radiobut.h"
    #include "wx/settings.h"
    #include "wx/brush.h"
#endif

#include "wx/msw/private.h"

// ============================================================================
// wxRadioButton implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxRadioButton creation
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxRadioButton, wxControl)

void wxRadioButton::Init()
{
    m_focusJustSet = FALSE;
}

bool wxRadioButton::Create(wxWindow *parent,
                           wxWindowID id,
                           const wxString& label,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style,
                           const wxValidator& validator,
                           const wxString& name)
{
    if ( !CreateControl(parent, id, pos, size, style, validator, name) )
        return FALSE;

    long msStyle = HasFlag(wxRB_GROUP) ? WS_GROUP : 0;

    msStyle |= BS_AUTORADIOBUTTON;

    if ( HasFlag(wxCLIP_SIBLINGS) )
        msStyle |= WS_CLIPSIBLINGS;

    if ( !MSWCreateControl(_T("BUTTON"), msStyle, pos, size, label, 0) )
        return FALSE;

    // for compatibility with wxGTK, the first radio button in a group is
    // always checked (this makes sense anyhow as you need to ensure that at
    // least one button in the group is checked and this is the simlpest way to
    // do it)
    if ( HasFlag(wxRB_GROUP) )
        SetValue(TRUE);

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxRadioButton functions
// ----------------------------------------------------------------------------

void wxRadioButton::SetValue(bool value)
{
    // BST_CHECKED is defined as 1, BST_UNCHECKED as 0, so we can just pass
    // value as is (we don't sue BST_XXX here as they're not defined for Win16)
    (void)::SendMessage(GetHwnd(), BM_SETCHECK, (WPARAM)value, 0L);
}

bool wxRadioButton::GetValue() const
{
    // NB: this will also return TRUE for BST_INDETERMINATE value if we ever
    //     have 3-state radio buttons
    return ::SendMessage(GetHwnd(), BM_GETCHECK, 0, 0L) != 0;
}

// ----------------------------------------------------------------------------
// wxRadioButton event processing
// ----------------------------------------------------------------------------

void wxRadioButton::Command (wxCommandEvent& event)
{
    SetValue(event.m_commandInt != 0);
    ProcessCommand(event);
}

void wxRadioButton::SetFocus()
{
    // when the radio button receives a WM_SETFOCUS message it generates a
    // BN_CLICKED which is totally unexpected and leads to catastrophic results
    // if you pop up a dialog from the radio button event handler as, when the
    // dialog is dismissed, the focus is returned to the radio button which
    // generates BN_CLICKED which leads to showing another dialog and so on
    // without end!
    //
    // to aviod this, we drop the pseudo BN_CLICKED events generated when the
    // button gains focus
    m_focusJustSet = TRUE;

    wxControl::SetFocus();
}

bool wxRadioButton::MSWCommand(WXUINT param, WXWORD WXUNUSED(id))
{
    if ( param != BN_CLICKED )
        return FALSE;

    if ( m_focusJustSet )
    {
        // see above: we want to ignore this event
        m_focusJustSet = FALSE;
    }
    else // a real clicked event
    {
        wxCommandEvent event(wxEVT_COMMAND_RADIOBUTTON_SELECTED, GetId());
        event.SetEventObject( this );
        event.SetInt( GetValue() );

        ProcessCommand(event);
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxRadioButton geometry
// ----------------------------------------------------------------------------

wxSize wxRadioButton::DoGetBestSize() const
{
    static int s_radioSize = 0;

    if ( !s_radioSize )
    {
        wxScreenDC dc;
        dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));

        s_radioSize = dc.GetCharHeight();
    }

    wxString str = GetLabel();

    int wRadio, hRadio;
    if ( !str.empty() )
    {
        GetTextExtent(str, &wRadio, &hRadio);
        wRadio += s_radioSize + GetCharWidth();

        if ( hRadio < s_radioSize )
            hRadio = s_radioSize;
    }
    else
    {
        wRadio = s_radioSize;
        hRadio = s_radioSize;
    }

    return wxSize(wRadio, hRadio);
}

#endif // wxUSE_RADIOBTN
