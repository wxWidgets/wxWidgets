/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/imaglist.cpp
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_IMAGLIST && !defined(wxHAS_NATIVE_IMAGELIST)

#include "wx/imaglist.h"

#ifndef WX_PRECOMP
    #include "wx/dc.h"
    #include "wx/icon.h"
    #include "wx/image.h"
#endif

#include "wx/settings.h"

//-----------------------------------------------------------------------------
//  wxImageList
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxGenericImageList, wxObject);
wxIMPLEMENT_DYNAMIC_CLASS(wxImageList, wxGenericImageList);

wxGenericImageList::wxGenericImageList( int width, int height, bool mask, int initialCount )
{
    (void)Create(width, height, mask, initialCount);
}

wxGenericImageList::~wxGenericImageList()
{
    (void)RemoveAll();
}

int wxGenericImageList::GetImageCount() const
{
    return static_cast<int>(m_images.size());
}

bool wxGenericImageList::Create( int width, int height, bool mask, int WXUNUSED(initialCount) )
{
    m_size = wxSize(width, height);
    m_useMask = mask;

    return true;
}

namespace
{
wxBitmap GetImageListBitmap(const wxBitmap& bitmap, bool useMask, const wxSize& imgSize)
{
    wxBitmap bmp(bitmap);
    if ( useMask )
    {
        if ( bmp.GetMask() )
        {
            if ( bmp.HasAlpha() )
            {
                // We need to remove alpha channel for compatibility with
                // native-based wxMSW wxImageList where stored images are not allowed
                // to have both mask and alpha channel.
#if wxUSE_IMAGE
                wxImage img = bmp.ConvertToImage();
                img.ClearAlpha();
                bmp = img;
#endif // wxUSE_IMAGE
            }
        }
        else
        {
            if ( bmp.HasAlpha() )
            {
                // Convert alpha channel to mask.
#if wxUSE_IMAGE
                wxImage img = bmp.ConvertToImage();
                img.ConvertAlphaToMask();
                bmp = img;
#endif // wxUSE_IMAGE
            }
            else
            {
                // Like for wxMSW, use the light grey from standard colour map as transparent colour.
                wxColour col = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
                bmp.SetMask(new wxMask(bmp, col));
            }
        }
    }
    else
    {
        if ( bmp.GetMask() )
        {
            if ( bmp.HasAlpha() )
            {
                // TODO: It would be better to blend a mask with existing alpha values.
                bmp.SetMask(NULL);
            }
            else
            {
                // Convert a mask to alpha values.
#if wxUSE_IMAGE
                wxImage img = bmp.ConvertToImage();
                img.InitAlpha();
                bmp = img;
#else
                bmp.SetMask(NULL);
#endif // wxUSE_IMAGE
            }
        }
    }

    // Ensure image size is the same as the size of the images on the image list.
    wxBitmap bmpResized;
    const wxSize sz = bmp.GetSize();
    if ( sz.x == imgSize.x && sz.y == imgSize.y )
    {
        bmpResized = bmp;
    }
    else if ( sz.x > imgSize.x && sz.y > imgSize.y )
    {
        wxRect r(0, 0, imgSize.x, imgSize.y);
        bmpResized = bmp.GetSubBitmap(r);
    }
    else
    {
#if wxUSE_IMAGE
        wxImage img = bmp.ConvertToImage();
        wxImage imgResized = img.Size(imgSize, wxPoint(0, 0), 0, 0, 0);
        bmpResized = imgResized;
#else
        bmpResized = bmp;
#endif // wxUSE_IMAGE
    }

    return bmpResized;
}
};

int wxGenericImageList::Add( const wxBitmap &bitmap )
{
    // We use the scaled, i.e. logical, size here as image list images size is
    // specified in logical pixels, just as window coordinates and sizes are.
    const wxSize bitmapSize = bitmap.GetScaledSize();

    if ( m_size == wxSize(0, 0) )
    {
        // This is the first time Add() is called and we hadn't had any fixed
        // size: adopt the size of our first bitmap as image size.
        m_size = bitmapSize;
    }

    // There is a special case: a bitmap may contain more than one image,
    // in which case we're supposed to chop it in parts, just as Windows
    // ImageList_Add() does.
    if ( bitmapSize.x == m_size.x )
    {
        m_images.push_back(GetImageListBitmap(bitmap, m_useMask, m_size));
    }
    else if ( bitmapSize.x > m_size.x )
    {
        const int numImages = bitmapSize.x / m_size.x;
        for (int subIndex = 0; subIndex < numImages; subIndex++)
        {
            wxRect rect(m_size.x * subIndex, 0, m_size.x, m_size.y);
            m_images.push_back(GetImageListBitmap(bitmap.GetSubBitmap(rect), m_useMask, m_size));
        }
    }
    else
    {
        return -1;
    }

    return GetImageCount() - 1;
}

int wxGenericImageList::Add( const wxBitmap& bitmap, const wxBitmap& mask )
{
    wxBitmap bmp(bitmap);
    if (mask.IsOk())
        bmp.SetMask(new wxMask(mask));
    return Add(bmp);
}

int wxGenericImageList::Add( const wxBitmap& bitmap, const wxColour& maskColour )
{
    wxBitmap bmp(bitmap);
    bmp.SetMask(new wxMask(bitmap, maskColour));
    return Add(bmp);
}

const wxBitmap *wxGenericImageList::DoGetPtr( int index ) const
{
    if ( index < 0 || (size_t)index >= m_images.size() )
        return NULL;

    return &m_images[index];
}

// Get the bitmap
wxBitmap wxGenericImageList::GetBitmap(int index) const
{
    const wxBitmap* bmp = DoGetPtr(index);
    if (!bmp)
        return wxNullBitmap;

    return *bmp;
}

// Get the icon
wxIcon wxGenericImageList::GetIcon(int index) const
{
    const wxBitmap* bmp = DoGetPtr(index);
    if (!bmp)
        return wxNullIcon;

    wxIcon icon;
    icon.CopyFromBitmap(*bmp);
    return icon;
}

bool
wxGenericImageList::Replace(int index,
                            const wxBitmap& bitmap,
                            const wxBitmap& mask)
{
    // Call DoGetPtr() just to check the index validity.
    if ( !DoGetPtr(index) )
        return false;

    wxBitmap bmp(bitmap);
    if ( mask.IsOk() )
        bmp.SetMask(new wxMask(mask));

    m_images[index] = GetImageListBitmap(bmp, m_useMask, m_size);

    return true;
}

bool wxGenericImageList::Remove( int index )
{
    m_images.erase(m_images.begin() + index);

    return true;
}

bool wxGenericImageList::RemoveAll()
{
    m_images.clear();

    return true;
}

bool wxGenericImageList::GetSize( int index, int &width, int &height ) const
{
    const wxBitmap* bmp = DoGetPtr(index);
    if ( !bmp )
    {
        width = 0;
        height = 0;
        return false;
    }

    width = bmp->GetScaledWidth();
    height = bmp->GetScaledHeight();

    return true;
}

bool wxGenericImageList::Draw( int index, wxDC &dc, int x, int y,
                        int flags, bool WXUNUSED(solidBackground) )
{
    const wxBitmap* bmp = DoGetPtr(index);
    if ( !bmp )
        return false;

    dc.DrawBitmap(*bmp, x, y, (flags & wxIMAGELIST_DRAW_TRANSPARENT) != 0);

    return true;
}

#endif // wxUSE_IMAGLIST
