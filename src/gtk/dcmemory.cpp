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

#if wxUSE_NEW_DC
IMPLEMENT_ABSTRACT_CLASS(wxGTKMemoryImplDC, wxGTKWindowImplDC)
#else
IMPLEMENT_ABSTRACT_CLASS(wxMemoryDC,wxWindowDC)
#endif

#if wxUSE_NEW_DC
wxGTKMemoryImplDC::wxGTKMemoryImplDC( wxMemoryDC *owner ) 
  : wxGTKWindowImplDC( owner )
{ 
    Init(); 
}

wxGTKMemoryImplDC::wxGTKMemoryImplDC( wxMemoryDC *owner, wxBitmap& bitmap) 
  : wxGTKWindowImplDC( owner )
{ 
    Init(); 
    owner->SelectObject(bitmap); 
}

wxGTKMemoryImplDC::wxGTKMemoryImplDC( wxMemoryDC *owner, wxDC *WXUNUSED(dc) )
  : wxGTKWindowImplDC( owner )
{
    Init();
}
#else
wxMemoryDC::wxMemoryDC() 
{ 
    Init(); 
}

wxMemoryDC::wxMemoryDC(wxBitmap& bitmap) 
{ 
    Init(); 
    SelectObject(bitmap); 
}

wxMemoryDC::wxMemoryDC( wxDC *WXUNUSED(dc) )
  : wxWindowDC()
{
    Init();
}
#endif

wxGTKMemoryImplDC::~wxGTKMemoryImplDC()
{
    g_object_unref(m_context);
}

void wxGTKMemoryImplDC::Init()
{
    m_ok = false;

    m_cmap = gtk_widget_get_default_colormap();

    m_context = gdk_pango_context_get();
    // Note: The Sun customised version of Pango shipping with Solaris 10
    // crashes if the language is left NULL (see bug 1374114)
    pango_context_set_language( m_context, gtk_get_default_language() );
    m_layout = pango_layout_new( m_context );
    m_fontdesc = pango_font_description_copy( pango_context_get_font_description( m_context ) );
}

void wxGTKMemoryImplDC::DoSelect( const wxBitmap& bitmap )
{
    Destroy();

    m_selected = bitmap;
    if (m_selected.Ok())
    {
        m_window = m_selected.GetPixmap();

        m_selected.PurgeOtherRepresentations(wxBitmap::Pixmap);

        SetUpDC( true );
    }
    else
    {
        m_ok = false;
        m_window = (GdkWindow *) NULL;
    }
}

void wxGTKMemoryImplDC::SetPen( const wxPen& penOrig )
{
    wxPen pen( penOrig );
    if ( m_selected.Ok() &&
            m_selected.GetDepth() == 1 &&
                (pen != *wxTRANSPARENT_PEN) )
    {
        pen.SetColour( pen.GetColour() == *wxWHITE ? *wxBLACK : *wxWHITE );
    }

    wxGTKWindowImplDC::SetPen( pen );
}

void wxGTKMemoryImplDC::SetBrush( const wxBrush& brushOrig )
{
    wxBrush brush( brushOrig );
    if ( m_selected.Ok() &&
            m_selected.GetDepth() == 1 &&
                (brush != *wxTRANSPARENT_BRUSH) )
    {
        brush.SetColour( brush.GetColour() == *wxWHITE ? *wxBLACK : *wxWHITE);
    }

    wxGTKWindowImplDC::SetBrush( brush );
}

void wxGTKMemoryImplDC::SetBackground( const wxBrush& brushOrig )
{
    wxBrush brush(brushOrig);

    if ( m_selected.Ok() &&
            m_selected.GetDepth() == 1 &&
                (brush != *wxTRANSPARENT_BRUSH) )
    {
        brush.SetColour( brush.GetColour() == *wxWHITE ? *wxBLACK : *wxWHITE );
    }

    wxGTKWindowImplDC::SetBackground( brush );
}

void wxGTKMemoryImplDC::SetTextForeground( const wxColour& col )
{
    if ( m_selected.Ok() && m_selected.GetDepth() == 1 )
    {
        wxGTKWindowImplDC::SetTextForeground( col == *wxWHITE ? *wxBLACK : *wxWHITE);
    }
    else
    {
        wxGTKWindowImplDC::SetTextForeground( col );
    }
}

void wxGTKMemoryImplDC::SetTextBackground( const wxColour &col )
{
    if (m_selected.Ok() && m_selected.GetDepth() == 1)
    {
        wxGTKWindowImplDC::SetTextBackground( col == *wxWHITE ? *wxBLACK : *wxWHITE );
    }
    else
    {
        wxGTKWindowImplDC::SetTextBackground( col );
    }
}

void wxGTKMemoryImplDC::DoGetSize( int *width, int *height ) const
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

wxBitmap wxGTKMemoryImplDC::DoGetAsBitmap(const wxRect *subrect) const
{
    wxBitmap bmp = GetSelectedBitmap();
    return subrect ? bmp.GetSubBitmap(*subrect) : bmp;
}

const wxBitmap& wxGTKMemoryImplDC::DoGetSelectedBitmap() const
{
    return m_selected;
}

wxBitmap& wxGTKMemoryImplDC::DoGetSelectedBitmap()
{
    return m_selected;
}

