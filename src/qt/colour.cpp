/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/colour.cpp
// Purpose:     wxColour class implementation for wxQt
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

wxColour::wxColour(const QColor& color)
{
    InitRGBA(color.red(), color.green(), color.blue(), color.alpha());
}

bool wxColour::operator==(const wxColour& color) const
{
    return m_red == color.m_red && m_green == color.m_green && m_blue == color.m_blue && m_alpha == color.m_alpha;
}

bool wxColour::operator!=(const wxColour& color) const
{
    return !(*this == color);
}

int wxColour::GetPixel() const
{
    wxMISSING_IMPLEMENTATION( "wxColour::GetPixel" );
    return 0;
}

QColor wxColour::GetQColor() const
{
    if ( m_valid )
        return QColor(m_red, m_green, m_blue, m_alpha);
    return QColor();
}

void wxColour::Init()
{
    m_red = m_green = m_blue = m_alpha = 0;
    m_valid = false;
}

void wxColour::InitRGBA(ChannelType r, ChannelType g, ChannelType b, ChannelType a)
{
    m_red = r; m_green = g; m_blue = b; m_alpha = a;
    m_valid = true;
}
