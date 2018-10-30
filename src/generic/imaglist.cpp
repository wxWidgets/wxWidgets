/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/imaglist.cpp
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_IMAGLIST && !defined(wxHAS_NATIVE_IMAGELIST)

#include "wx/imaglist.h"

#ifndef WX_PRECOMP
    #include "wx/dc.h"
    #include "wx/icon.h"
    #include "wx/image.h"
#endif

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

bool wxGenericImageList::Create( int width, int height, bool WXUNUSED(mask), int WXUNUSED(initialCount) )
{
    m_size = wxSize(width, height);

    return true;
}

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
    else // We already have a fixed size, check that the bitmap conforms to it.
    {
        // There is a special case: a bitmap may contain more than one image,
        // in which case we're supposed to chop it in parts, just as Windows
        // ImageList_Add() does.
        if ( bitmapSize.x > m_size.x && (bitmapSize.x % m_size.x == 0) )
        {
            const int numImages = bitmapSize.x / m_size.x;
            for (int subIndex = 0; subIndex < numImages; subIndex++)
            {
                wxRect rect(m_size.x * subIndex, 0, m_size.x, m_size.y);
                Add(bitmap.GetSubBitmap(rect));
            }

            return GetImageCount() - 1;
        }

        wxASSERT_MSG( bitmapSize == m_size,
                      "All bitmaps in wxImageList must have the same size" );
    }

    m_images.push_back(bitmap);

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
    wxImage img = bitmap.ConvertToImage();
    img.SetMaskColour(maskColour.Red(), maskColour.Green(), maskColour.Blue());
    return Add(wxBitmap(img));
}

const wxBitmap *wxGenericImageList::DoGetPtr( int index ) const
{
    wxCHECK_MSG( index >= 0 && (size_t)index < m_images.size(),
                 NULL, wxT("wrong index in image list") );

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

    m_images[index] = bitmap;

    if ( mask.IsOk() )
        m_images[index].SetMask(new wxMask(mask));

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
