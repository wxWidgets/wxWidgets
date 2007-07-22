// Copyright (c) 2007 John Wilmes

#include "wx/bitmap.h"
#include "wx/image.h"

IMPLEMENT_DYNAMIC_CLASS(wxMask, wxObject)

wxMask::wxMask()
{
}

wxMask::wxMask(const wxBitmap& bitmap, const wxColour& colour)
{
    Create(bitmap, colour);
}

wxMask::wxMask(const wxBitmap& bitmap, int paletteIndex)
{
    Create(bitmap, paletteIndex);
}

wxMask::wxMask(const wxBitmap& bitmap)
{
    Create(bitmap);
}

wxMask::~wxMask()
{
}

bool wxMask::Create(const wxBitmap& bitmap,
                    const wxColour& colour)
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

IMPLEMENT_DYNAMIC_CLASS(wxBitmap, wxGDIObject)

wxBitmap::wxBitmap()
{
}

wxBitmap::wxBitmap(int width, int height, int depth)
{
    Create(width, height, depth);
}

wxBitmap::wxBitmap( const char bits[], int width, int height, int depth )
{
}

wxBitmap::wxBitmap(const wxString &filename, wxBitmapType type)
{
}

#if wxUSE_IMAGE
wxBitmap::wxBitmap(const wxImage& image, int depth) {
}
#endif //wxUSE_IMAGE

wxBitmap::~wxBitmap()
{
}

bool wxBitmap::Create(int width, int height, int depth)
{
    wxCHECK_MSG((width > 0) && (height > 0), false, wxT("invalid bitmap size"));
    return true;
}

#if wxUSE_IMAGE
wxImage wxBitmap::ConvertToImage() const
{
    wxImage image;

    return image;
}
#endif //wxUSE_IMAGE

bool wxBitmap::IsOk() const
{
    return true;
}

int wxBitmap::GetHeight() const
{
    wxCHECK_MSG(Ok(), -1, wxT("invalid bitmap"));
    return 0;
}

int wxBitmap::GetWidth() const
{
    wxCHECK_MSG(Ok(), -1, wxT("invalid bitmap"));
    return 0;
}

int wxBitmap::GetDepth() const
{
    wxCHECK_MSG(Ok(), -1, wxT("invalid bitmap"));
    return 0;
}

wxMask *wxBitmap::GetMask() const
{
    wxCHECK_MSG(Ok(), (wxMask *) NULL, wxT("invalid bitmap"));
    return new wxMask();
}

void wxBitmap::SetMask(wxMask *mask)
{
}

bool wxBitmap::CopyFromIcon(const wxIcon& icon)
{
    return true;
}

wxBitmap wxBitmap::GetSubBitmap(const wxRect& rect) const
{
    wxBitmap ret(rect.width, rect.height, 0);
    return ret;
}

bool wxBitmap::SaveFile(const wxString &name, wxBitmapType type,
                        const wxPalette *palette) const
{
    wxCHECK_MSG(Ok(), false, wxT("invalid bitmap"));
    return false;
}

bool wxBitmap::LoadFile(const wxString &name, wxBitmapType type )
{
    return false;
}

void wxBitmap::SetHeight(int height)
{
}

void wxBitmap::SetWidth(int width)
{
}

void wxBitmap::SetDepth(int depth)
{
}

void wxBitmap::InitStandardHandlers() {
}

IMPLEMENT_ABSTRACT_CLASS(wxBitmapHandler, wxBitmapHandlerBase)

wxBitmapHandler::wxBitmapHandler() {
}
