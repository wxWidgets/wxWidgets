/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dcclient.cpp
// Author:      Peter Most, Javier Torres
// Copyright:   (c) Peter Most, Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dcclient.h"
#include "wx/log.h"
#include "wx/qt/dcclient.h"

#include <QtGui/QPicture>

//##############################################################################

wxWindowDCImpl::wxWindowDCImpl( wxDC *owner )
    : wxQtDCImpl( owner )
{
    m_window = NULL;
    m_ok = false;
    m_qtPainter = new QPainter();
}

wxWindowDCImpl::wxWindowDCImpl( wxDC *owner, wxWindow *win )
    : wxQtDCImpl( owner )
{
    m_window = win;
    m_qtPainter = m_window->QtGetPainter();
    // if we're not inside a Paint event, painter will invalid
    m_ok = m_qtPainter != NULL;
}

wxWindowDCImpl::~wxWindowDCImpl()
{
    if ( m_ok )
    {
        m_ok = false;
    }
    if ( m_window )
    {
        // do not destroy as it is owned by the window
        m_qtPainter = NULL;
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
    m_ok = m_qtPainter->begin( pic );
    QtPreparePainter();
}

wxClientDCImpl::~wxClientDCImpl()
{
    /* Paint to a QPicture that will then be painted in the next
     * paint event of that window (a paint event will be generated
     * when this wxClientDC is done). */
    if ( m_ok )
    {
        m_qtPainter->end();
        m_ok = false;
        QPicture *pict = m_window->QtGetPicture();

        if ( m_window != NULL )
        {
            // get the inner widget in scroll areas:
            QWidget *widget;
            if ( m_window->QtGetScrollBarsContainer() )
            {
                widget = m_window->QtGetScrollBarsContainer()->viewport();
            } else {
                widget = m_window->GetHandle();
            }
            // force paint event if there is something to replay and
            // if not currently inside a paint event (to avoid recursion)
            QRect rect = pict->boundingRect();
            if ( !pict->isNull() && !widget->paintingActive() && !rect.isEmpty() )
            {
                // only force the update of the rect affected by the DC
                widget->repaint( rect );
                wxLogDebug( wxT("wxClientDC Repainting %s (%d %d %d %d)"),
                           (const char*) m_window->GetName(),
                           rect.left(), rect.top(), rect.width(), rect.height());
            }
            else
            {
                // Not drawing anything, reset picture to avoid issues in handler
                pict->setData( NULL, 0 );
            }
            // let destroy the m_qtPainter (see inherited classes destructors)
            m_window = NULL;
        }
    }
}

//##############################################################################

wxPaintDCImpl::wxPaintDCImpl( wxDC *owner )
    : wxWindowDCImpl( owner )
{
    if ( m_ok )
    {
        QtPreparePainter();
    }
}

wxPaintDCImpl::wxPaintDCImpl( wxDC *owner, wxWindow *win )
    : wxWindowDCImpl( owner, win )
{
    if ( m_ok )
    {
        QtPreparePainter();
    }
}

