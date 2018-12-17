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

#include <QtWidgets/QDesktopWidget>
#include <QtGui/QScreen>
#include <QtWidgets/QApplication>
#include <QtGui/QPixmap>

wxIMPLEMENT_ABSTRACT_CLASS(wxScreenDCImpl, wxWindowDCImpl);

wxScreenDCImpl::wxScreenDCImpl( wxScreenDC *owner )
    : wxWindowDCImpl( owner )
{
    m_qtImage = NULL;
}

wxScreenDCImpl::~wxScreenDCImpl( )
{
    delete m_qtImage;
}

void wxScreenDCImpl::DoGetSize(int *width, int *height) const
{
    wxDisplaySize(width, height);
}

// defered allocation for blit
QImage *wxScreenDCImpl::GetQImage()
{
    if ( !m_qtImage )
        m_qtImage = new QImage(QApplication::primaryScreen()->grabWindow(QApplication::desktop()->winId()).toImage());
    return m_qtImage;
}
