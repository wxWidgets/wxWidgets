/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dcclient.cpp
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include <QtGui/QPicture>
#include <QtWidgets/QWidget>

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/dcclient.h"
#include "wx/qt/dcclient.h"
#include "wx/qt/private/converter.h"

#include <QtGui/QPainter>

//##############################################################################

wxIMPLEMENT_CLASS(wxWindowDCImpl,wxQtDCImpl);

wxWindowDCImpl::wxWindowDCImpl( wxDC *owner )
    : wxQtDCImpl( owner )
{
    m_window = nullptr;
    m_ok = false;
    m_qtPainter = new QPainter();
}

wxWindowDCImpl::wxWindowDCImpl( wxDC *owner, wxWindow *win )
    : wxQtDCImpl( owner )
{
    m_window = win;
    m_qtPainter = m_window->QtGetPainter(); // guaranteed to be non-null

    if ( m_qtPainter->isActive() )
    {
        m_isWindowPainter = true;
    }
    else
    {
        m_qtPainter = new QPainter();
        m_pict.reset( new QPicture() );
        m_ok = m_qtPainter->begin( m_pict.get() );
        QtPreparePainter();
    }
}

wxWindowDCImpl::~wxWindowDCImpl()
{
    /* Paint to a QPicture that will then be painted in the next
     * paint event of that window (a paint event will be generated
     * when this wxClientDC is done). */
    if ( m_ok )
    {
        if ( m_isWindowPainter )
            m_qtPainter = nullptr; // do not destroy in base class as it is owned by the window
        else
            m_qtPainter->end();

        if ( m_window )
        {
            if ( m_pict && !m_pict->isNull() )
            {
                // force paint event if there is something to replay and
                // if not currently inside a paint event (to avoid recursion)
                wxRect rect = wxQtConvertRect(m_pict->boundingRect());
                if ( !m_window->GetHandle()->paintingActive() && !rect.IsEmpty() )
                {
                    m_window->QtSetPicture( m_pict.release() );
                    // only force the update of the rect affected by the DC
                    m_window->Refresh(true, &rect);
                }
                else // Do we really need the else branch? we are in the dtor anyhow!
                {
                    // Not drawing anything, reset picture to avoid issues in handler
                    m_pict.reset();
                }
            }

            // let destroy the m_qtPainter (see inherited classes destructors)
            m_window = nullptr;
        }
    }

    // Painter will be deleted by base class if we own it
}

void wxWindowDCImpl::DoGetSize(int *width, int *height) const
{
    wxCHECK_RET( m_window, "wxWindowDCImpl without a window?" );

    m_window->GetSize(width, height);
}

//##############################################################################

wxIMPLEMENT_CLASS(wxClientDCImpl,wxWindowDCImpl);

wxClientDCImpl::wxClientDCImpl( wxDC *owner )
    : wxWindowDCImpl( owner )
{
}

wxClientDCImpl::wxClientDCImpl( wxDC *owner, wxWindow *win )
    : wxWindowDCImpl( owner, win )
{
     if ( m_ok )
     {
        m_qtPainter->setClipRect( wxQtConvertRect(win->GetClientRect()),
                                  m_clipping ? Qt::IntersectClip : Qt::ReplaceClip );

        m_qtPainter->translate( wxQtConvertPoint(win->GetClientAreaOrigin()) );
    }
}

void wxClientDCImpl::DoGetSize(int *width, int *height) const
{
    wxCHECK_RET( m_window, "wxClientDCImpl without a window?" );

    m_window->GetClientSize(width, height);
}

//##############################################################################

wxIMPLEMENT_CLASS(wxPaintDCImpl,wxWindowDCImpl);

wxPaintDCImpl::wxPaintDCImpl( wxDC *owner )
    : wxWindowDCImpl( owner )
{
}

wxPaintDCImpl::wxPaintDCImpl( wxDC *owner, wxWindow *win )
    : wxWindowDCImpl( owner, win )
{
    wxCHECK_RET( m_isWindowPainter || win->QtCanPaintWithoutActivePainter(),
                 "wxPaintDC can't be created outside wxEVT_PAINT handler" );

    m_qtPainter->translate( wxQtConvertPoint(win->GetClientAreaOrigin()) );
}

