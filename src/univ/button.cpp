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

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// default margins around the image
static const wxCoord DEFAULT_BTN_MARGIN_X = 0;
static const wxCoord DEFAULT_BTN_MARGIN_Y = 0;

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
                      const wxBitmap& bitmap,
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
        style |= wxALIGN_CENTRE_HORIZONTAL | wxALIGN_CENTRE_VERTICAL;
    }

    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return FALSE;

    SetLabel(label);
    SetImageLabel(bitmap);
    SetBestSize(size);

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

    if ( m_bitmap.Ok() )
    {
        // allocate extra space for the bitmap
        wxCoord heightBmp = m_bitmap.GetHeight() + 2*m_marginBmpY;
        if ( height < heightBmp )
            height = heightBmp;

        width += m_bitmap.GetWidth() + 2*m_marginBmpX;
    }

    return AdjustSize(wxSize(width, height));
}

// ----------------------------------------------------------------------------
// drawing
// ----------------------------------------------------------------------------

void wxButton::DoDraw(wxControlRenderer *renderer)
{
    renderer->DrawButtonBorder();
    renderer->DrawLabel(m_bitmap, m_marginBmpX, m_marginBmpY);
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
    if ( m_isPressed )
        Release();
    else
        Press();

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
// misc
// ----------------------------------------------------------------------------

void wxButton::SetImageLabel(const wxBitmap& bitmap)
{
    m_bitmap = bitmap;
    m_marginBmpX = DEFAULT_BTN_MARGIN_X;
    m_marginBmpY = DEFAULT_BTN_MARGIN_Y;
}

void wxButton::SetImageMargins(wxCoord x, wxCoord y)
{
    m_marginBmpX = x;
    m_marginBmpY = y;
}

void wxButton::SetDefault()
{
    m_isDefault = TRUE;
}

#endif // wxUSE_BUTTON

