/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/colour.cpp
// Purpose:     wxColourImpl class for wxQt
// Author:      Kolya Kosenko
// Created:     2010-05-12
// Copyright:   (c) 2010 Kolya Kosenko
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/colour.h"
#endif // WX_PRECOMP

#include "wx/colour.h"
#include "wx/qt/private/utils.h"

#include <QtGui/QColor>

wxColourImpl::wxColourImpl(const QColor& color)
{
    InitRGBA(color.red(), color.green(), color.blue(), color.alpha());
}

bool wxColourImpl::operator==(const wxColourImpl& color) const
{
    return m_red == color.m_red && m_green == color.m_green && m_blue == color.m_blue && m_alpha == color.m_alpha;
}

bool wxColourImpl::operator!=(const wxColourImpl& color) const
{
    return !(*this == color);
}

int wxColourImpl::GetPixel() const
{
    wxMISSING_IMPLEMENTATION( "wxColourImpl::GetPixel" );
    return 0;
}

QColor wxColourImpl::GetQColor() const
{
    if ( m_valid )
        return QColor(m_red, m_green, m_blue, m_alpha);
    return QColor();
}

void wxColourImpl::InitRGBA(ChannelType r, ChannelType g, ChannelType b, ChannelType a)
{
    m_red = r; m_green = g; m_blue = b; m_alpha = a;
    m_valid = true;
}
