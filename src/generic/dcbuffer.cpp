/////////////////////////////////////////////////////////////////////////////
// Name:        dcbuffer.cpp
// Purpose:     wxBufferedDC class
// Author:      Ron Lee <ron@debian.org>
// Modified by:
// Created:     16/03/02
// RCS-ID:      $Id$
// Copyright:   (c) Ron Lee
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
    #pragma implementation "dcbuffer.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/window.h"
#endif

#include "wx/dcbuffer.h"


// ==============================================================
//   Double buffering helper.
// --------------------------------------------------------------

wxBufferedDC::wxBufferedDC( wxDC *dc, const wxBitmap &buffer )
    : m_dc( dc )
    , m_buffer( buffer )
{
    SelectObject( m_buffer );
}

wxBufferedDC::wxBufferedDC( wxDC *dc, const wxSize &area )
    : m_dc( dc )
    , m_buffer( area.GetWidth(), area.GetHeight() )
{
    SelectObject( m_buffer );
}

wxBufferedDC::~wxBufferedDC()
{
    if( m_dc != 0 )
    {
        UnMask();
    }
}

void wxBufferedDC::Init( wxDC *dc, const wxBitmap &buffer )
{
    wxASSERT_MSG( m_dc == 0 && m_buffer == wxNullBitmap,
                  _T("wxBufferedDC already initialised") );

    m_dc = dc;
    m_buffer = buffer;
    SelectObject( m_buffer );
}

void wxBufferedDC::Init( wxDC *dc, const wxSize &area )
{
    wxASSERT_MSG( m_dc == 0 && m_buffer == wxNullBitmap,
                  _T("wxBufferedDC already initialised") );

    m_dc = dc;
    m_buffer = wxBitmap( area.GetWidth(), area.GetHeight() );
    SelectObject( m_buffer );
}

void wxBufferedDC::UnMask()
{
    wxASSERT_MSG( m_dc != 0, _T("No low level DC associated with buffer (anymore)") );

    m_dc->Blit( 0, 0, m_buffer.GetWidth(), m_buffer.GetHeight(), this, 0, 0 );
    m_dc = 0;
}


// ==============================================================
//   Double buffered PaintDC.
// --------------------------------------------------------------

wxBufferedPaintDC::wxBufferedPaintDC( wxWindow *window, const wxBitmap &buffer )
    : m_paintdc( window )
{
    window->PrepareDC( m_paintdc );

    if( buffer != wxNullBitmap )
        Init( &m_paintdc, buffer );
    else
        Init( &m_paintdc, window->GetClientSize() );
}

wxBufferedPaintDC::~wxBufferedPaintDC()
{
        // We must UnMask here, else by the time the base class
        // does it, the PaintDC will have already been destroyed.

    UnMask();
}


// vi:sts=4:sw=4:et
