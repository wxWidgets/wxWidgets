/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/palette.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/palette.h"

wxPalette::wxPalette()
{
}

wxPalette::wxPalette(int n, unsigned char *red, unsigned char *green, unsigned char *blue)
{
}

bool wxPalette::Create(int n, unsigned char *red, unsigned char *green, unsigned char *blue)
{
    return false;
}

bool wxPalette::GetRGB(int pixel, unsigned char *red, unsigned char *green, unsigned char *blue) const
{
    return false;
}

int wxPalette::GetPixel(unsigned char red, unsigned char green, unsigned char blue) const
{
    return false;
}


wxGDIRefData *wxPalette::CreateGDIRefData() const
{
    return NULL;
}

wxGDIRefData *wxPalette::CloneGDIRefData(const wxGDIRefData *data) const
{
    return NULL;
}

