/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/bitmap.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/bitmap.h"
#include "wx/image.h"

void wxBitmap::InitStandardHandlers()
{
}


wxBitmap::wxBitmap()
{
}

wxBitmap::wxBitmap(const wxBitmap& bmp)
{
}

wxBitmap::wxBitmap(const char bits[], int width, int height, int depth )
{
}

wxBitmap::wxBitmap(int width, int height, int depth)
{
}

wxBitmap::wxBitmap(const wxSize& sz, int depth )
{
}

// wxBitmap::wxBitmap(const char* const* bits)
// {
// }

wxBitmap::wxBitmap(const wxString &filename, wxBitmapType type )
{
}

wxBitmap::wxBitmap(const wxImage& image, int depth )
{
}


bool wxBitmap::Create(int width, int height, int depth )
{
    return 0;
}

bool wxBitmap::Create(const wxSize& sz, int depth )
{
    return 0;
}


int wxBitmap::GetHeight() const
{
    return 0;
}

int wxBitmap::GetWidth() const
{
    return 0;
}

int wxBitmap::GetDepth() const
{
    return 0;
}


#if wxUSE_IMAGE
wxImage wxBitmap::ConvertToImage() const
{
    return 0;
}

#endif // wxUSE_IMAGE

wxMask *wxBitmap::GetMask() const
{
    return 0;
}

void wxBitmap::SetMask(wxMask *mask)
{
}


wxBitmap wxBitmap::GetSubBitmap(const wxRect& rect) const
{
    return 0;
}


bool wxBitmap::SaveFile(const wxString &name, wxBitmapType type,
              const wxPalette *palette ) const
{
    return 0;
}

bool wxBitmap::LoadFile(const wxString &name, wxBitmapType type)
{
    return 0;
}


#if wxUSE_PALETTE
wxPalette *wxBitmap::GetPalette() const
{
    return 0;
}

void wxBitmap::SetPalette(const wxPalette& palette)
{
}

#endif // wxUSE_PALETTE

// copies the contents and mask of the given (colour) icon to the bitmap
bool wxBitmap::CopyFromIcon(const wxIcon& icon)
{
    return 0;
}


    // implementation:
void wxBitmap::SetHeight(int height)
{
}

void wxBitmap::SetWidth(int width)
{
}

void wxBitmap::SetDepth(int depth)
{
}


void *wxBitmap::GetRawData(wxPixelDataBase& data, int bpp)
{
    return 0;
}

void wxBitmap::UngetRawData(wxPixelDataBase& data)
{
}


wxGDIRefData *wxBitmap::CreateGDIRefData() const
{
    return 0;
}

wxGDIRefData *wxBitmap::CloneGDIRefData(const wxGDIRefData *data) const
{
    return 0;
}

//#############################################################################

IMPLEMENT_DYNAMIC_CLASS( wxMask, wxObject )

wxMask::wxMask()
{
}

wxMask::wxMask(const wxMask &mask)
{
}

wxMask::wxMask(const wxBitmap& bitmap, const wxColour& colour)
{
}

wxMask::wxMask(const wxBitmap& bitmap, int paletteIndex)
{
}

wxMask::wxMask(const wxBitmap& bitmap)
{
}


wxMask::~wxMask()
{
}

bool wxMask::Create(const wxBitmap& bitmap, const wxColour& colour)
{
    return false;
}

bool wxMask::Create(const wxBitmap& bitmap, int paletteIndex)
{
    return false;
}

bool wxMask::Create(const wxBitmap& bitmap)
{
    return false;
}

