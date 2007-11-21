/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk1/dcmemory.cpp
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

void wxMemoryDC::Init()
{
    m_ok = false;

    m_cmap = gtk_widget_get_default_colormap();
}

wxMemoryDC::wxMemoryDC( wxDC *WXUNUSED(dc) )
          : wxWindowDC()
{
    Init();
}

wxMemoryDC::~wxMemoryDC()
{
}

void wxMemoryDC::DoSelect( const wxBitmap& bitmap )
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
            m_selected.GetBitmap() &&
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
            m_selected.GetBitmap() &&
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
            m_selected.GetBitmap() &&
                (brush != *wxTRANSPARENT_BRUSH) )
    {
        brush.SetColour( brush.GetColour() == *wxWHITE ? *wxBLACK : *wxWHITE );
    }

    wxWindowDC::SetBackground( brush );
}

void wxMemoryDC::SetTextForeground( const wxColour& col )
{
    if ( m_selected.Ok() && m_selected.GetBitmap() )
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
    if (m_selected.Ok() && m_selected.GetBitmap())
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
