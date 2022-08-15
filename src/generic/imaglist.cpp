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

wxGenericImageList::wxGenericImageList()
{
    Create(0, 0, false);
}

wxGenericImageList::wxGenericImageList( int width, int height, bool mask, int initialCount )
{
    (void)Create(width, height, mask, initialCount);
}

void wxGenericImageList::Destroy()
{
    (void)RemoveAll();

    // Make it invalid.
    m_size = wxSize(0, 0);
}

wxGenericImageList::~wxGenericImageList()
{
}

int wxGenericImageList::GetImageCount() const
{
    wxASSERT_MSG( m_size != wxSize(0, 0), "Invalid image list" );

    return static_cast<int>(m_images.size());
}

bool wxGenericImageList::Create( int width, int height, bool mask, int WXUNUSED(initialCount) )
{
    // Prevent from storing negative dimensions
    m_size = wxSize(wxMax(width, 0), wxMax(height, 0));
    m_useMask = mask;

    // Images must have proper size
    return m_size != wxSize(0, 0);
}

wxBitmap wxGenericImageList::GetImageListBitmap(const wxBitmap& bitmap) const
{
    wxBitmap bmp(bitmap);

    // If we have neither mask nor alpha and were asked to use a mask,
    // create a default one.
    if ( m_useMask && !bmp.GetMask() && !bmp.HasAlpha() )
    {
        // Like for wxMSW, use the light grey from standard colour map as transparent colour.
        wxColour col = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
        bmp.SetMask(new wxMask(bmp, col));
    }

    // Ensure image size is the same as the size of the images on the image list.
    wxBitmap bmpResized;
    const wxSize sz = bmp.GetLogicalSize();
    if ( sz.x == m_size.x && sz.y == m_size.y )
    {
        bmpResized = bmp;
    }
    else if ( sz.x > m_size.x && sz.y > m_size.y )
    {
        wxRect r(0, 0, m_size.x, m_size.y);
        bmpResized = bmp.GetSubBitmap(r);
    }
    else
    {
#if wxUSE_IMAGE
        wxImage img = bmp.ConvertToImage();
        // We need image with new physical size
        const double scaleFactor = bmp.GetScaleFactor();
        wxImage imgResized = img.Size(scaleFactor * m_size, wxPoint(0, 0), 0, 0, 0);
        bmpResized = wxBitmap(imgResized, -1, scaleFactor);
#else
        bmpResized = bmp;
#endif // wxUSE_IMAGE
    }

    return bmpResized;
}

int wxGenericImageList::Add( const wxBitmap &bitmap )
{
    // Cannot add image to invalid list
    if ( m_size == wxSize(0, 0) )
        return -1;

    // We use the logical size here as image list images size is specified in
    // logical pixels, just as window coordinates and sizes are.
    const wxSize bitmapSize = bitmap.GetLogicalSize();

    // There is a special case: a bitmap may contain more than one image,
    // in which case we're supposed to chop it in parts, just as Windows
    // ImageList_Add() does.
    if ( bitmapSize.x == m_size.x )
    {
        m_images.push_back(GetImageListBitmap(bitmap));
    }
    else if ( bitmapSize.x > m_size.x )
    {
        const int numImages = bitmapSize.x / m_size.x;
        for (int subIndex = 0; subIndex < numImages; subIndex++)
        {
            wxRect rect(m_size.x * subIndex, 0, m_size.x, m_size.y);
            m_images.push_back(GetImageListBitmap(bitmap.GetSubBitmap(rect)));
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

    m_images[index] = GetImageListBitmap(bmp);

    return true;
}

bool wxGenericImageList::Remove( int index )
{
    if ( index < 0 || (size_t)index >= m_images.size() )
        return false;

    m_images.erase(m_images.begin() + index);

    return true;
}

bool wxGenericImageList::RemoveAll()
{
    m_images.clear();

    return true;
}

bool wxGenericImageList::GetSize( int WXUNUSED(index), int &width, int &height ) const
{
    width = m_size.x;
    height = m_size.y;

    wxCHECK_MSG( m_size != wxSize(0, 0), false, "Invalid image list" );

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
