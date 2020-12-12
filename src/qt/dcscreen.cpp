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

wxIMPLEMENT_ABSTRACT_CLASS(wxScreenDCImpl, wxQtDCImpl);

wxScreenDCImpl::wxScreenDCImpl( wxScreenDC *owner )
    : wxWindowDCImpl( owner )
{
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
    if ( !m_qtPixmap )
        m_qtPixmap = new QPixmap(QApplication::primaryScreen()->grabWindow(QApplication::desktop()->winId()));
    return m_qtPixmap;
}
