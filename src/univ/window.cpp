///////////////////////////////////////////////////////////////////////////////
// Name:        univ/window.cpp
// Purpose:     wxUniversalWindow implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     06.08.00
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
    #pragma implementation "univwindow.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// drawing
// ----------------------------------------------------------------------------

// the event handler executed when the window must be repainted
void wxUniversalWindow::OnPaint(wxPaintEvent& event)
{
    // get the renderer to use
    wxRenderer *renderer = wxTheTheme->GetRenderer();

    // draw the control
    wxPaintDC dc(this);
    DoDraw(dc, renderer);
}

// draw the border
void DoDrawBorder(wxRenderer *renderer, wxDC& dc, const wxRect& rect)
{
    if ( !(m_windowStyle & wxNO_BORDER) )
    {
        renderer->DrawBorder(dc, this);
    }
}

