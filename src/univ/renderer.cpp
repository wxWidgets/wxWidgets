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
    #include "wx/app.h"
    #include "wx/control.h"
    #include "wx/dc.h"
#endif // WX_PRECOMP

#include "wx/image.h"

#include "wx/univ/renderer.h"

// ============================================================================
// implementation
// ============================================================================

wxRenderer::~wxRenderer()
{
}

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

void wxControlRenderer::DrawBorder()
{
    int flags = m_ctrl->GetStateFlags();

    // draw outline
    m_renderer->DrawBorder(m_dc, m_ctrl->GetBorder(),
                           m_rect, flags, &m_rect);

    // fill the inside (TODO: query the theme for bg bitmap)
    m_renderer->DrawBackground(m_dc,
                               m_ctrl->GetBackgroundColour(), m_rect, flags);
}

void wxControlRenderer::DrawLabel()
{
    m_dc.SetFont(m_ctrl->GetFont());
    m_dc.SetTextForeground(m_ctrl->GetForegroundColour());

    m_renderer->DrawLabel(m_dc,
                          m_ctrl->GetLabel(),
                          m_rect,
                          m_ctrl->GetStateFlags(),
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
                          m_ctrl->GetStateFlags(),
                          m_ctrl->GetAlignment(),
                          m_ctrl->GetAccelIndex());
}

void wxControlRenderer::DrawButtonBorder()
{
    int flags = m_ctrl->GetStateFlags();

    m_renderer->DrawButtonBorder(m_dc, m_rect, flags, &m_rect);

    m_renderer->DrawBackground(m_dc, m_ctrl->GetBackgroundColour(),
                               m_rect, flags);
}

void wxControlRenderer::DrawBackgroundBitmap()
{
    // get the bitmap and the flags
    int alignment;
    wxStretch stretch;
    wxBitmap bmp = m_ctrl->GetBackgroundBitmap(&alignment, &stretch);
    if ( !bmp.Ok() )
        return;

    int width = bmp.GetWidth(),
        height = bmp.GetHeight();

    wxCoord x = 0,
            y = 0;
    if ( stretch & wxTILE )
    {
        // tile the bitmap
        for ( ; x < m_rect.width; x += width )
        {
            for ( y = 0; y < m_rect.height; y += height )
            {
                m_dc.DrawBitmap(bmp, x, y);
            }
        }
    }
    else if ( stretch & wxEXPAND )
    {
        // stretch bitmap to fill the entire control
        bmp = wxImage(bmp).Scale(m_rect.width, m_rect.height).ConvertToBitmap();
    }
    else // not stretched, not tiled
    {
        if ( alignment & wxALIGN_RIGHT )
        {
            x = m_rect.GetRight() - width;
        }
        else if ( alignment & wxALIGN_CENTRE )
        {
            x = (m_rect.GetLeft() + m_rect.GetRight() - width) / 2;
        }
        else // alignment & wxALIGN_LEFT
        {
            x = m_rect.GetLeft();
        }

        if ( alignment & wxALIGN_BOTTOM )
        {
            y = m_rect.GetBottom() - height;
        }
        else if ( alignment & wxALIGN_CENTRE_VERTICAL )
        {
            y = (m_rect.GetTop() + m_rect.GetBottom() - height) / 2;
        }
        else // alignment & wxALIGN_TOP
        {
            y = m_rect.GetTop();
        }
    }

    // do draw it
    m_dc.DrawBitmap(bmp, x, y);
}

void wxControlRenderer::DrawScrollbar(int thumbStart, int thumbEnd)
{
    m_renderer->DrawScrollbar(m_dc,
                              m_ctrl->GetWindowStyle() & wxVERTICAL
                                ? wxVERTICAL
                                : wxHORIZONTAL,
                              thumbStart, thumbEnd, m_rect,
                              m_ctrl->GetStateFlags());
}
