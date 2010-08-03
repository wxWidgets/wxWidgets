/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dcmemory.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most, Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dcmemory.h"
#include "wx/qt/dcmemory.h"

wxMemoryDCImpl::wxMemoryDCImpl( wxMemoryDC *owner )
    : wxQtDCImpl( owner )
{
    m_qtImage = NULL;
    m_ok = false;
}

wxMemoryDCImpl::wxMemoryDCImpl( wxMemoryDC *owner, wxBitmap& bitmap )
    : wxQtDCImpl( owner )
{
    m_qtImage = NULL;
    m_ok = false;
    DoSelect( bitmap );
}

wxMemoryDCImpl::wxMemoryDCImpl( wxMemoryDC *owner, wxDC *dc )
    : wxQtDCImpl( owner )
{
    m_qtImage = NULL;
    m_ok = false;
}

wxMemoryDCImpl::~wxMemoryDCImpl()
{
    DoSelect( wxNullBitmap );
}

void wxMemoryDCImpl::DoSelect( const wxBitmap& bitmap )
{
    if ( IsOk() )
    {
        // Copy image to bitmap
        m_qtPainter.end();

        m_qtPainter.begin( m_pixmap );
        m_qtPainter.drawImage( QPoint( 0, 0 ), *m_qtImage );
        m_qtPainter.end();

        m_ok = false;
        m_pixmap = NULL;
    }

    if ( m_qtImage )
    {
        delete m_qtImage;
        m_qtImage = NULL;
    }

    if ( bitmap.IsOk() && !bitmap.GetHandle()->isNull() ) {
        m_pixmap = bitmap.GetHandle();

        m_qtImage = new QImage( m_pixmap->toImage() );

        m_ok = m_qtPainter.begin( m_qtImage );
    }
}
