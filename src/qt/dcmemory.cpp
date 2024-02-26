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

wxIMPLEMENT_CLASS(wxMemoryDCImpl,wxQtDCImpl);

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

    // clean up the intermediate image device:
    m_selected = bitmap;
    m_qtPixmap = bitmap.GetHandle();
    if ( bitmap.IsOk() && !m_qtPixmap->isNull() )
    {
        // apply mask before drawing
        wxMask *mask = bitmap.GetMask();
        if ( mask && mask->GetHandle() )
            m_qtPixmap->setMask(*mask->GetHandle());

        // start drawing on the intermediary device:
        m_ok = m_qtPainter->begin( m_qtPixmap );

        if (m_qtPainter->device()->depth() > 1)
        {
            m_qtPainter->setRenderHints(QPainter::Antialiasing,
                                        true);
        }

        SetPen(m_pen);
        SetBrush(m_brush);
        SetFont(m_font);
    }
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
