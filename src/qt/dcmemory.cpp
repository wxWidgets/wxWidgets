/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dcmemory.cpp
// Author:      Peter Most
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
    m_qtPainter = new QPainter();
}

wxMemoryDCImpl::wxMemoryDCImpl( wxMemoryDC *owner, wxBitmap& bitmap )
    : wxQtDCImpl( owner )
{
    m_qtImage = NULL;
    m_ok = false;
    m_qtPainter = new QPainter();
    DoSelect( bitmap );
}

wxMemoryDCImpl::wxMemoryDCImpl( wxMemoryDC *owner, wxDC *WXUNUSED(dc) )
    : wxQtDCImpl( owner )
{
    m_qtImage = NULL;
    m_ok = false;
}

wxMemoryDCImpl::~wxMemoryDCImpl()
{
    // dont deselect the bitmap here as it can be already deleted
//    DoSelect( wxNullBitmap );     // TODO FIX (move to wxBitmap?)
}

void wxMemoryDCImpl::DoSelect( const wxBitmap& bitmap )
{
    m_selected = bitmap;
    if ( IsOk() )
    {
        // Copy image to bitmap
        m_qtPainter->end();

        m_qtPainter->begin( m_pixmap );
        m_qtPainter->drawImage( QPoint( 0, 0 ), *m_qtImage );
        m_qtPainter->end();

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

        m_ok = m_qtPainter->begin( m_qtImage );
    }
}

const wxBitmap& wxMemoryDCImpl::GetSelectedBitmap() const
{
    return m_selected;
}

wxBitmap& wxMemoryDCImpl::GetSelectedBitmap()
{
    return m_selected;
}
