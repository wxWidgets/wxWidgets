/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dcscreen.cpp
// Author:      Sean D'Epagnier
// Copyright:   (c) Sean D'Epagnier
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dcscreen.h"
#include "wx/qt/dcscreen.h"

#include <QtWidgets/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QPicture>
#include <QtGui/QPixmap>
#include <QtGui/QScreen>

#if QT_VERSION_MAJOR < 6
#include <QDesktopWidget>
#endif

wxIMPLEMENT_ABSTRACT_CLASS(wxScreenDCImpl, wxQtDCImpl);

wxScreenDCImpl::wxScreenDCImpl( wxScreenDC *owner )
    : wxWindowDCImpl( owner )
{
    m_pict.reset(new QPicture());
    m_ok = m_qtPainter->begin( m_pict.get() );

    QtPreparePainter();
}

wxScreenDCImpl::~wxScreenDCImpl( )
{
    delete m_qtPixmap;
}

void wxScreenDCImpl::DoGetSize(int *width, int *height) const
{
    wxDisplaySize(width, height);
}

// defered allocation for blit
QPixmap *wxScreenDCImpl::GetQPixmap()
{
    if (!m_qtPixmap)
    {
        WId winId = 0;

#if QT_VERSION_MAJOR < 6
        winId = QApplication::desktop()->winId();
#endif

        m_qtPixmap = new QPixmap(QApplication::primaryScreen()->grabWindow(winId));
    }
    return m_qtPixmap;
}
