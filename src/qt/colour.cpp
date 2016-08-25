/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/colour.h
// Purpose:     wxColour class implementation for wxQt
// Author:      Kolya Kosenko
// Created:     2010-05-12
// Copyright:   (c) 2010 Kolya Kosenko
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

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

bool wxColour::IsOk() const { return valid; }

unsigned char wxColour::Red() const { return red;   }
unsigned char wxColour::Green() const { return green; }
unsigned char wxColour::Blue()  const { return blue;  }
unsigned char wxColour::Alpha() const { return alpha; }

bool wxColour::operator==(const wxColour& color) const
{
    return red == color.red && green == color.green && blue == color.blue && alpha == color.alpha;
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
    if( valid )
        return QColor(red, green, blue, alpha);
    return QColor();
}

void wxColour::Init()
{
    valid = false;
}

void wxColour::InitRGBA(ChannelType r, ChannelType g, ChannelType b, ChannelType a)
{
    red = r, green = g, blue = b, alpha = a;
    valid = true;
}
