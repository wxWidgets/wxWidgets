///////////////////////////////////////////////////////////////////////////////
// Name:        univ/window.cpp
// Purpose:     implementation of extra wxWindow methods for wxUniv port
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
    #include "wx/window.h"
    #include "wx/dc.h"
    #include "wx/dcclient.h"
    #include "wx/event.h"

    #include "wx/univ/renderer.h"
    #include "wx/univ/theme.h"
#endif // WX_PRECOMP

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// we don't have any objects of type wxWindowBase so this cast is always safe
#define self    ((wxWindow *)this)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// drawing
// ----------------------------------------------------------------------------

// the event handler executed when the window must be repainted
void wxWindowBase::OnPaint(wxPaintEvent& event)
{
    // get the renderer and the DC to use
    wxRenderer *renderer = wxTheme::Get()->GetRenderer();
    wxPaintDC dc(self);

    // draw the border
    DoDrawBorder(dc, renderer);

    // draw the control
    DoDraw(dc, renderer);
}

// draw the border
void wxWindowBase::DoDrawBorder(wxDC& dc, wxRenderer *renderer)
{
    if ( !(m_windowStyle & wxNO_BORDER) )
    {
        renderer->DrawBorder(dc, self);
    }
}

void wxWindowBase::DoDraw(wxDC& dc, wxRenderer *renderer)
{
}
