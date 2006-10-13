/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/dcmemory.cpp
// Purpose:
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dcmemory.h"

#include <gdk/gdk.h>
#include <gtk/gtk.h>

//-----------------------------------------------------------------------------
// wxMemoryDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMemoryDC,wxWindowDC)

wxMemoryDC::wxMemoryDC( const wxBitmap& bitmap )
   : wxWindowDC()
{
    m_ok = false;

    m_cmap = gtk_widget_get_default_colormap();

    m_context = gdk_pango_context_get();
    // Note: The Sun customised version of Pango shipping with Solaris 10
    // crashes if the language is left NULL (see bug 1374114)
    pango_context_set_language( m_context, gtk_get_default_language() );
    m_layout = pango_layout_new( m_context );
    m_fontdesc = pango_font_description_copy( pango_context_get_font_description( m_context ) );

    if ( bitmap.IsOk() )
        SelectObject(bitmap);
}

wxMemoryDC::wxMemoryDC( wxDC *WXUNUSED(dc) )
  : wxWindowDC()
{
    m_ok = false;

    m_cmap = gtk_widget_get_default_colormap();

    m_context = gdk_pango_context_get();
    pango_context_set_language( m_context, gtk_get_default_language() );
    m_layout = pango_layout_new( m_context );
    m_fontdesc = pango_font_description_copy( pango_context_get_font_description( m_context ) );
}

wxMemoryDC::~wxMemoryDC()
{
    g_object_unref(m_context);
}

void wxMemoryDC::SelectObject( const wxBitmap& bitmap )
{
    Destroy();
    m_selected = bitmap;
    if (m_selected.Ok())
    {
        m_window = m_selected.GetPixmap();

        m_selected.PurgeOtherRepresentations(wxBitmap::Pixmap);

        m_isMemDC = true;

        SetUpDC();
    }
    else
    {
        m_ok = false;
        m_window = (GdkWindow *) NULL;
    }
}

void wxMemoryDC::SetPen( const wxPen& penOrig )
{
    wxPen pen( penOrig );
    if ( m_selected.Ok() &&
            m_selected.GetDepth() == 1 &&
                (pen != *wxTRANSPARENT_PEN) )
    {
        pen.SetColour( pen.GetColour() == *wxWHITE ? *wxBLACK : *wxWHITE );
    }

    wxWindowDC::SetPen( pen );
}

void wxMemoryDC::SetBrush( const wxBrush& brushOrig )
{
    wxBrush brush( brushOrig );
    if ( m_selected.Ok() &&
            m_selected.GetDepth() == 1 &&
                (brush != *wxTRANSPARENT_BRUSH) )
    {
        brush.SetColour( brush.GetColour() == *wxWHITE ? *wxBLACK : *wxWHITE);
    }

    wxWindowDC::SetBrush( brush );
}

void wxMemoryDC::SetBackground( const wxBrush& brushOrig )
{
    wxBrush brush(brushOrig);

    if ( m_selected.Ok() &&
            m_selected.GetDepth() == 1 &&
                (brush != *wxTRANSPARENT_BRUSH) )
    {
        brush.SetColour( brush.GetColour() == *wxWHITE ? *wxBLACK : *wxWHITE );
    }

    wxWindowDC::SetBackground( brush );
}

void wxMemoryDC::SetTextForeground( const wxColour& col )
{
    if ( m_selected.Ok() && m_selected.GetDepth() == 1 )
    {
        wxWindowDC::SetTextForeground( col == *wxWHITE ? *wxBLACK : *wxWHITE);
    }
    else
    {
        wxWindowDC::SetTextForeground( col );
    }
}

void wxMemoryDC::SetTextBackground( const wxColour &col )
{
    if (m_selected.Ok() && m_selected.GetDepth() == 1)
    {
        wxWindowDC::SetTextBackground( col == *wxWHITE ? *wxBLACK : *wxWHITE );
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
