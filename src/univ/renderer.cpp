///////////////////////////////////////////////////////////////////////////////
// Name:        univ/renderer.cpp
// Purpose:     wxControlRenderer implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     15.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "renderer.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/univ/renderer.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxControlRenderer
// ----------------------------------------------------------------------------

wxControlRenderer::wxControlRenderer(wxControl *control,
                                   wxDC& dc,
                                   wxRenderer *renderer)
                : m_dc(dc)
{
    m_ctrl = control;
    m_renderer = renderer;

    wxSize size = m_ctrl->GetSize();
    m_rect.x =
    m_rect.y = 0;
    m_rect.width = size.x;
    m_rect.height = size.y;
}

int wxControlRenderer::GetStateFlags() const
{
    int flags = 0;
    if ( m_ctrl->IsEnabled() )
        flags |= wxRENDER_ENABLED;

    // the following states are only possible if our application is active - if
    // it is not, even our default/focused controls shouldn't appear as such
    if ( wxTheApp->IsActive() )
    {
        if ( m_ctrl->IsFocused() )
            flags |= wxRENDER_FOCUSED;
        if ( m_ctrl->IsPressed() )
            flags |= wxRENDER_PRESSED;
        if ( m_ctrl->IsDefault() )
            flags |= wxRENDER_DEFAULT;
    }

    return flags;
}

void wxControlRenderer::PaintBackground()
{
    wxBrush brush(m_ctrl->GetBackgroundColour(), wxSOLID);
    m_dc.SetBrush(brush);
    m_dc.SetPen(*wxTRANSPARENT_PEN);
    m_dc.DrawRectangle(m_rect);
}

void wxControlRenderer::DrawBorder()
{
    // draw outline
    m_renderer->DrawBorder(m_dc, m_ctrl->GetBorder(),
                           m_rect, GetStateFlags(), &m_rect);

    // fill the inside
    PaintBackground();
}

void wxControlRenderer::DrawLabel()
{
    m_dc.SetFont(m_ctrl->GetFont());
    m_dc.SetTextForeground(m_ctrl->GetForegroundColour());

    // shift the label if a button is pressed
    wxRect rectLabel = m_rect;
    if ( GetStateFlags() & wxRENDER_PRESSED )
    {
        rectLabel.x++;
        rectLabel.y++;
    }

    m_renderer->DrawLabel(m_dc,
                          m_ctrl->GetLabel(),
                          rectLabel,
                          GetStateFlags(),
                          m_ctrl->GetAlignment(),
                          m_ctrl->GetAccelIndex());
}

void wxControlRenderer::DrawFrame()
{
    m_dc.SetFont(m_ctrl->GetFont());
    m_dc.SetTextForeground(m_ctrl->GetForegroundColour());
    m_dc.SetTextBackground(m_ctrl->GetBackgroundColour());

    m_renderer->DrawFrame(m_dc,
                          m_ctrl->GetLabel(),
                          m_rect,
                          GetStateFlags(),
                          m_ctrl->GetAlignment(),
                          m_ctrl->GetAccelIndex());
}

void wxControlRenderer::DrawButtonBorder()
{
    m_renderer->DrawButtonBorder(m_dc, m_rect, GetStateFlags(), &m_rect);

    PaintBackground();
}
