/////////////////////////////////////////////////////////////////////////////
// Name:        dcmemory.cpp
// Purpose:
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dcmemory.h"
#endif

#include "wx/dcmemory.h"

#include <gdk/gdk.h>
#include <gtk/gtk.h>

//-----------------------------------------------------------------------------
// wxMemoryDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMemoryDC,wxWindowDC)

wxMemoryDC::wxMemoryDC() : wxWindowDC()
{
    m_ok = FALSE;

    m_cmap = gtk_widget_get_default_colormap();
}

wxMemoryDC::wxMemoryDC( wxDC *WXUNUSED(dc) )
  : wxWindowDC()
{
    m_ok = FALSE;

    m_cmap = gtk_widget_get_default_colormap();
}

wxMemoryDC::~wxMemoryDC()
{
}

void wxMemoryDC::SelectObject( const wxBitmap& bitmap )
{
    Destroy();
    m_selected = bitmap;
    if (m_selected.Ok())
    {
        if (m_selected.GetPixmap())
        {
            m_window = m_selected.GetPixmap();
        }
        else
        {
            m_window = m_selected.GetBitmap();
        }

        SetUpDC();

        m_isMemDC = TRUE;
    }
    else
    {
        m_ok = FALSE;
        m_window = (GdkWindow *) NULL;
    }
}

void wxMemoryDC::SetPen( const wxPen &pen )
{
    if (m_selected.Ok() && m_selected.GetBitmap() && (*wxTRANSPARENT_PEN != pen))
    {
        if (*wxWHITE_PEN == pen)
            wxWindowDC::SetPen( *wxBLACK_PEN );
        else
            wxWindowDC::SetPen( *wxWHITE_PEN );
    }
    else
    {
        wxWindowDC::SetPen( pen );
    }
}

void wxMemoryDC::SetBrush( const wxBrush &brush )
{
    if (m_selected.Ok() && m_selected.GetBitmap() && (*wxTRANSPARENT_BRUSH != brush))
    {
        if (*wxWHITE_BRUSH == brush)
            wxWindowDC::SetBrush( *wxBLACK_BRUSH );
        else
            wxWindowDC::SetBrush( *wxWHITE_BRUSH );
    }
    else
    {
        wxWindowDC::SetBrush( brush );
    }
}

void wxMemoryDC::SetTextForeground( const wxColour &col )
{
    if (m_selected.Ok() && m_selected.GetBitmap())
    {
        if (col == *wxWHITE)
            wxWindowDC::SetTextForeground( *wxBLACK );
        else
            wxWindowDC::SetTextForeground( *wxWHITE );
    }
    else
    {
        wxWindowDC::SetTextForeground( col );
    }
}

void wxMemoryDC::SetTextBackground( const wxColour &col )
{
    if (m_selected.Ok() && m_selected.GetBitmap())
    {
        if (col == *wxWHITE)
            wxWindowDC::SetTextBackground( *wxBLACK );
        else
            wxWindowDC::SetTextBackground( *wxWHITE );
    }
    else
    {
        wxWindowDC::SetTextBackground( col );
    }
}

void wxMemoryDC::DoGetSize( int *width, int *height ) const
{
    if (m_selected.Ok())
    {
        if (width) (*width) = m_selected.GetWidth();
        if (height) (*height) = m_selected.GetHeight();
    }
    else
    {
        if (width) (*width) = 0;
        if (height) (*height) = 0;
    }
}


