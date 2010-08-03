/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dcclient.cpp
// Author:      Peter Most, Javier Torres
// Id:          $Id$
// Copyright:   (c) Peter Most, Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dcclient.h"
#include "wx/qt/dcclient.h"

#include <QtGui/QPicture>

//##############################################################################

wxWindowDCImpl::wxWindowDCImpl( wxDC *owner )
    : wxQtDCImpl( owner )
{
    m_window = NULL;
    m_ok = false;
}

wxWindowDCImpl::wxWindowDCImpl( wxDC *owner, wxWindow *win )
    : wxQtDCImpl( owner )
{
    m_window = win;

    m_qtImage = win->QtGetPaintBuffer();
    m_ok = m_qtPainter.begin( m_qtImage );
}

wxWindowDCImpl::~wxWindowDCImpl()
{
    if ( m_ok )
    {
        m_qtPainter.end();
        m_qtImage = NULL;
        m_ok = false;
    }
}

//##############################################################################


wxClientDCImpl::wxClientDCImpl( wxDC *owner )
    : wxWindowDCImpl( owner )
{
    m_window = NULL;
}

wxClientDCImpl::wxClientDCImpl( wxDC *owner, wxWindow *win )
    : wxWindowDCImpl( owner )
{
    m_window = win;

    QPicture *pic = win->QtGetPicture();
    int w, h;
    win->GetClientSize( &w, &h );

    pic->setBoundingRect( QRect( 0, 0, w, h ) );
    m_ok = m_qtPainter.begin( pic );
}

wxClientDCImpl::~wxClientDCImpl()
{
    /* Paint to a QPicture that will then be painted in the next
     * paint event of that window (a paint event will be generated
     * when this wxClientDC is done). */
    if ( m_ok )
    {
        m_qtPainter.end();
        m_ok = false;

        if ( m_window != NULL )
            m_window->GetHandle()->repaint();
    }
}

//##############################################################################

wxPaintDCImpl::wxPaintDCImpl( wxDC *owner )
    : wxWindowDCImpl( owner )
{
}

wxPaintDCImpl::wxPaintDCImpl( wxDC *owner, wxWindow *win )
    : wxWindowDCImpl( owner, win )
{
}

