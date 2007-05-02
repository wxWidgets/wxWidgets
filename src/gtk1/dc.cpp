/////////////////////////////////////////////////////////////////////////////
// Name:        dc.cpp
// Purpose:
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dc.h"

#include <gdk/gdk.h>
#include <gtk/gtk.h>

//-----------------------------------------------------------------------------
// wxDC
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxDC, wxDCBase)

wxDC::wxDC()
{
    m_ok = FALSE;

    m_mm_to_pix_x = (double)wxGetDisplaySize().GetWidth() /
                    (double)wxGetDisplaySizeMM().GetWidth();
    m_mm_to_pix_y = (double)wxGetDisplaySize().GetHeight() /
                    (double)wxGetDisplaySizeMM().GetHeight();

    m_needComputeScaleX = FALSE; /* not used yet */
    m_needComputeScaleY = FALSE; /* not used yet */

    m_logicalFunction = wxCOPY;

    m_pen = *wxBLACK_PEN;
    m_font = *wxNORMAL_FONT;
    m_brush = *wxWHITE_BRUSH;
}

void wxDC::DoSetClippingRegion( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    m_clipping = TRUE;
    m_clipX1 = x;
    m_clipY1 = y;
    m_clipX2 = x + width;
    m_clipY2 = y + height;
}

// ---------------------------------------------------------------------------
// get DC capabilities
// ---------------------------------------------------------------------------

void wxDC::DoGetSizeMM( int* width, int* height ) const
{
    int w = 0;
    int h = 0;
    GetSize( &w, &h );
    if (width) *width = int( double(w) / (m_userScaleX*m_mm_to_pix_x) );
    if (height) *height = int( double(h) / (m_userScaleY*m_mm_to_pix_y) );
}

// Resolution in pixels per logical inch
wxSize wxDC::GetPPI() const
{
    // TODO (should probably be pure virtual)
    return wxSize(0, 0);
}

