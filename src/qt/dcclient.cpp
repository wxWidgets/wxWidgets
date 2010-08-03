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
    
    // Paint to an image
    PrepareQPainter( wxQtConvertSize( win->GetClientSize() ) );
}

wxWindowDCImpl::~wxWindowDCImpl()
{
    // Copy the image to the window
    if ( m_ok )
    {
        m_qtPainter.end();

        m_qtPainter.begin( m_window->QtGetContainer() );
        m_qtPainter.drawImage( QPoint( 0, 0 ), *m_qtImage );
        m_qtPainter.end();
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
    
    // Paint to an image
    PrepareQPainter( wxQtConvertSize( win->GetClientSize() ) );
}

wxClientDCImpl::~wxClientDCImpl()
{
    /* Paint to a QPicture that will then be painted in the next
     * paint event of that window (a paint event will be generated
     * when this wxClientDC is done). */
    if ( m_ok )
    {
        m_qtPainter.end();
        
        m_qtPainter.begin( m_window->QtGetPicture() );
        m_qtPainter.drawImage( QPoint( 0, 0 ), *m_qtImage );
        m_qtPainter.end();
        m_ok = false;

        if ( m_window != NULL )
            m_window->Refresh();
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

