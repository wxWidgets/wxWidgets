/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dcmemory.cpp
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dcmemory.h"
#include "wx/qt/dcmemory.h"

#include <QtGui/QPainter>

wxMemoryDCImpl::wxMemoryDCImpl( wxMemoryDC *owner )
    : wxQtDCImpl( owner )
{
    m_ok = false;
    m_qtPainter = new QPainter();
}

wxMemoryDCImpl::wxMemoryDCImpl( wxMemoryDC *owner, wxBitmap& bitmap )
    : wxQtDCImpl( owner )
{
    m_ok = false;
    m_qtPainter = new QPainter();
    DoSelect( bitmap );
}

wxMemoryDCImpl::wxMemoryDCImpl( wxMemoryDC *owner, wxDC *WXUNUSED(dc) )
    : wxQtDCImpl( owner )
{
    m_ok = false;
    m_qtPainter = new QPainter();
}

wxMemoryDCImpl::~wxMemoryDCImpl()
{
    // Deselect the bitmap, if any (warning: it can be already deleted)
    DoSelect( wxNullBitmap );
}

void wxMemoryDCImpl::DoSelect( const wxBitmap& bitmap )
{
    if ( IsOk() )
    {
        // Finish the painting in the intermediate image device:
        m_qtPainter->end();
        m_ok = false;
    }

    m_selected = bitmap;
    if ( bitmap.IsOk() && !bitmap.GetHandle()->isNull() ) {
        // start drawing on the intermediary device:
        m_ok = m_qtPainter->begin( m_selected.GetHandle() );
        SetPen(m_pen);
        SetBrush(m_brush);
        SetFont(m_font);
    }
}

bool wxMemoryDCImpl::DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const
{
    if ( m_selected.IsNull())
        return false;

    QColor pixel = m_selected.GetHandle()->pixel( x, y );
    col->Set( pixel.red(), pixel.green(), pixel.blue(), pixel.alpha() );
    return true;
}

wxBitmap wxMemoryDCImpl::DoGetAsBitmap(const wxRect *subrect) const
{
    if ( !subrect )
        return m_selected;
    return m_selected.GetSubBitmap(*subrect);
}

const wxBitmap& wxMemoryDCImpl::GetSelectedBitmap() const
{
    return m_selected;
}

wxBitmap& wxMemoryDCImpl::GetSelectedBitmap()
{
    return m_selected;
}
