/////////////////////////////////////////////////////////////////////////////
// Name:        univ/button.cpp
// Purpose:     wxButton
// Author:      Vadim Zeitlin
// Modified by:
// Created:     14.08.00
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
    #pragma implementation "univbutton.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_BUTTON

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/button.h"
    #include "wx/validate.h"
#endif

#include "wx/univ/renderer.h"
#include "wx/univ/inphand.h"
#include "wx/univ/theme.h"

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxButton, wxControl)

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

void wxButton::Init()
{
    m_isPressed =
    m_isDefault = FALSE;
}

bool wxButton::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxString &label,
                      const wxPoint &pos,
                      const wxSize &size,
                      long style,
                      const wxValidator& validator,
                      const wxString &name)
{
    // center label by default
    if ( !(style & wxALIGN_MASK) )
    {
        style |= wxALIGN_CENTRE | wxALIGN_CENTRE_VERTICAL;
    }

    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return FALSE;

    SetLabel(label);

    if ( size.x == -1 || size.y == -1 )
    {
        wxSize sizeBest = DoGetBestSize();
        SetSize(size.x == -1 ? sizeBest.x : size.x,
                size.y == -1 ? sizeBest.y : size.y);
    }

    return TRUE;
}

wxButton::~wxButton()
{
}

// ----------------------------------------------------------------------------
// size management
// ----------------------------------------------------------------------------

wxSize wxButton::DoGetBestSize() const
{
    wxClientDC dc(wxConstCast(this, wxButton));
    wxCoord width, height;
    dc.GetMultiLineTextExtent(GetLabel(), &width, &height);

    wxSize sz(width, height);
    wxTheme::Get()->GetRenderer()->AdjustSize(&sz, this);
    return sz;
}

// ----------------------------------------------------------------------------
// drawing
// ----------------------------------------------------------------------------

void wxButton::DoDraw(wxControlRenderer *renderer)
{
    renderer->DrawButtonBorder();
    renderer->DrawLabel();
}

// ----------------------------------------------------------------------------
// input processing
// ----------------------------------------------------------------------------

void wxButton::Press()
{
    m_isPressed = TRUE;
}

void wxButton::Release()
{
    m_isPressed = FALSE;
}

void wxButton::Toggle()
{
    m_isPressed = !m_isPressed;

    if ( !m_isPressed )
    {
        // releasing button after it had been pressed generates a click event
        Click();
    }
}

void wxButton::Click()
{
    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
    InitCommandEvent(event);
    Command(event);
}

bool wxButton::PerformAction(const wxControlAction& action,
                             const wxEvent& event)
{
    bool wasPressed = IsPressed();

    if ( action == wxACTION_BUTTON_TOGGLE )
        Toggle();
    else if ( action == wxACTION_BUTTON_CLICK )
        Click();
    else if ( action == wxACTION_BUTTON_PRESS )
        Press();
    else if ( action == wxACTION_BUTTON_RELEASE )
        Release();
    else
        return wxControl::PerformAction(action, event);

    return wasPressed != IsPressed();
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

void wxButton::SetDefault()
{
    m_isDefault = TRUE;
}

#endif // wxUSE_BUTTON

