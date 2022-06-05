/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/imaglist.cpp
// Purpose:     wxImageList implementation for Win32
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/msw/wrapcctl.h" // include <commctrl.h> "properly"
    #include "wx/window.h"
    #include "wx/icon.h"
    #include "wx/dc.h"
    #include "wx/string.h"
    #include "wx/dcmemory.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/image.h"
    #include <stdio.h>
#endif

#include "wx/imaglist.h"
#include "wx/dc.h"
#include "wx/msw/dc.h"
#include "wx/msw/dib.h"
#include "wx/msw/private.h"

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxImageList, wxObject);

#define GetHImageList()     ((HIMAGELIST)m_hImageList)

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// returns the default transparent colour to use for creating the mask
static wxColour GetDefaultMaskColour();

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxImageList creation/destruction
// ----------------------------------------------------------------------------

// Creates an image list
bool wxImageList::Create(int width, int height, bool mask, int initial)
{
    wxASSERT_MSG( m_hImageList == NULL, "Recreating existing wxImageList?" );

    // Prevent from storing negative dimensions
    m_size = wxSize(wxMax(width, 0), wxMax(height, 0));
    UINT flags = 0;

    // as we want to be able to use 32bpp bitmaps in the image lists, we always
    // use ILC_COLOR32, even if the display resolution is less -- the system
    // will make the best effort to show the bitmap if we do this resulting in
    // quite acceptable display while using a lower depth ILC_COLOR constant
    // (e.g. ILC_COLOR16) shows completely broken bitmaps
    flags |= ILC_COLOR32;

    m_useMask = mask;

    // For comctl32.dll < 6 always use masks as it doesn't support alpha.
    //
    // We also have to use masks when we don't have wxImage and wxDIB that are
    // needed to handle alpha.
#if wxUSE_WXDIB && wxUSE_IMAGE
    if ( wxApp::GetComCtl32Version() < 600 )
#endif
        flags |= ILC_MASK;

    // Grow by 1, I guess this is reasonable behaviour most of the time
    m_hImageList = (WXHIMAGELIST) ImageList_Create(width, height, flags,
                                                   initial, 1);
    if ( !m_hImageList )
    {
        wxLogLastError(wxT("ImageList_Create()"));
    }

    return m_hImageList != 0;
}

void wxImageList::Destroy()
{
    if ( m_hImageList )
    {
        ImageList_Destroy(GetHImageList());
        m_hImageList = 0;
    }
}

wxImageList::~wxImageList()
{
    Destroy();
}

// ----------------------------------------------------------------------------
// wxImageList attributes
// ----------------------------------------------------------------------------

// Returns the number of images in the image list.
int wxImageList::GetImageCount() const
{
    wxASSERT_MSG( m_hImageList, wxT("invalid image list") );

    return ImageList_GetImageCount(GetHImageList());
}

// Returns the size (same for all images) of the images in the list
bool wxImageList::GetSize(int WXUNUSED(index), int &width, int &height) const
{
    wxASSERT_MSG( m_hImageList, wxT("invalid image list") );

    return ImageList_GetIconSize(GetHImageList(), &width, &height) != 0;
}

// ----------------------------------------------------------------------------
// wxImageList operations
// ----------------------------------------------------------------------------

class wxImageList::wxMSWBitmaps
{
public:
    wxMSWBitmaps() : hbmp(NULL) { }

#if wxUSE_WXDIB && wxUSE_IMAGE
    void InitFromImageWithAlpha(const wxImage& img)
    {
        hbmp = wxDIB(img, wxDIB::PixelFormat_NotPreMultiplied).Detach();
        hbmpRelease.Init(hbmp);
    }
#endif // wxUSE_WXDIB && wxUSE_IMAGE

    // These fields are filled by GetImageListBitmaps().
    HBITMAP hbmp;
    AutoHBITMAP hbmpMask;

private:
    // This one is only used to delete a temporary bitmap, if necessary, and
    // shouldn't be used otherwise, so it's private.
    AutoHBITMAP hbmpRelease;

    wxDECLARE_NO_COPY_CLASS(wxMSWBitmaps);
};

void
wxImageList::GetImageListBitmaps(wxMSWBitmaps& bitmaps,
                                 const wxBitmap& bitmap, const wxBitmap& mask)
{
    // This can be overwritten below if we need to modify the bitmap, but it
    // doesn't cost anything to initialize the bitmap with this HBITMAP.
    bitmaps.hbmp = GetHbitmapOf(bitmap);

#if wxUSE_WXDIB && wxUSE_IMAGE
    if ( wxApp::GetComCtl32Version() >= 600 )
    {
        wxBitmap bmp(bitmap);

        if ( mask.IsOk() )
        {
            // Explicitly specified mask overrides the mask associated with the
            // bitmap, if any.
            bmp.SetMask(new wxMask(mask));
        }

        if ( bmp.GetMask() )
        {
            // Get rid of the mask by converting it to alpha.
            if ( bmp.HasAlpha() )
                bmp.MSWBlendMaskWithAlpha();
        }
        else if ( m_useMask )
        {
            // Create the mask from the default transparent colour if we have
            // nothing else.
            if ( !bmp.HasAlpha() )
                bmp.SetMask(new wxMask(bmp, GetDefaultMaskColour()));
        }
        else
        {
            // We actually don't have to do anything at all and can just use
            // the original bitmap as is.
            return;
        }

        // wxBitmap normally stores alpha in pre-multiplied format but
        // ImageList_Draw() does pre-multiplication internally so we need to undo
        // the pre-multiplication here. Converting back and forth like this is, of
        // course, very inefficient but it's better than wrong appearance so we do
        // this for now until a better way can be found.
        wxImage img = bmp.ConvertToImage();
        if ( !img.HasAlpha() )
            img.InitAlpha();

        // There is a very special, but important in practice, case of fully
        // transparent bitmaps: they're used to allow not specifying any image
        // for some items. Unfortunately the native image list simply ignores
        // alpha channel if it only contains 0 values, apparently considering
        // the bitmap to be 24bpp in this case. And there doesn't seem to be
        // any way to avoid this, i.e. tell it to really not draw anything, so
        // we use this horrible hack to force it to take alpha into account by
        // setting at least one pixel to a non-0 value.
        unsigned char* alpha = img.GetAlpha();
        unsigned char* const end = alpha + img.GetWidth()*img.GetHeight();
        for ( ; alpha < end; ++alpha )
        {
            if ( *alpha != wxALPHA_TRANSPARENT )
                break;
        }

        if ( alpha == end )
        {
            // We haven't found any non-transparent pixels, so make one of them
            // (we arbitrarily choose the bottom right one) almost, but not
            // quite, transparent.
            alpha[-1] = 1; // As transparent as possible, but not transparent.
        }

        bitmaps.InitFromImageWithAlpha(img);

        // In any case we'll never use mask at the native image list level as
        // it's incompatible with alpha and we need to use alpha.
    }
    else
#endif // wxUSE_WXDIB && wxUSE_IMAGE
    {
        wxMask maskToUse;

        HBITMAP hbmpMask = NULL;

        // Always use mask if it is specified.
        if ( mask.IsOk() )
        {
            hbmpMask = GetHbitmapOf(mask);
        }
        else if ( bitmap.GetMask() )
        {
            hbmpMask = bitmap.GetMask()->GetMaskBitmap();
        }
#if wxUSE_WXDIB && wxUSE_IMAGE
        // We can also use alpha, but we have to convert it to a mask as it is
        // not supported by this comctl32.dll version.
        else if ( bitmap.HasAlpha() )
        {
            wxImage img = bitmap.ConvertToImage();
            img.ConvertAlphaToMask();
            bitmaps.InitFromImageWithAlpha(img);

            maskToUse.MSWCreateFromImageMask(img);
        }
#endif // wxUSE_WXDIB && wxUSE_IMAGE
        // We don't have neither mask nor alpha, only force creating the
        // mask from colour if requested to do it.
        else if ( m_useMask )
        {
            maskToUse.Create(bitmap, GetDefaultMaskColour());
        }

        if ( !hbmpMask )
            hbmpMask = maskToUse.GetMaskBitmap();

        if ( hbmpMask )
        {
            // windows mask convention is opposite to the wxWidgets one
            bitmaps.hbmpMask.Init(wxInvertMask(hbmpMask));
        }
    }
}

// Adds a bitmap, and optionally a mask bitmap.
// Note that wxImageList creates new bitmaps, so you may delete
// 'bitmap' and 'mask'.
int wxImageList::Add(const wxBitmap& bitmap, const wxBitmap& mask)
{
    wxMSWBitmaps bitmaps;
    GetImageListBitmaps(bitmaps, bitmap, mask);

    int index = ImageList_Add(GetHImageList(), bitmaps.hbmp, bitmaps.hbmpMask);
    if ( index == -1 )
    {
        wxLogError(_("Couldn't add an image to the image list."));
    }

    return index;
}

// Adds a bitmap, using the specified colour to create the mask bitmap
// Note that wxImageList creates new bitmaps, so you may delete
// 'bitmap'.
int wxImageList::Add(const wxBitmap& bitmap, const wxColour& maskColour)
{
    wxMSWBitmaps bitmaps;
    wxMask mask(bitmap, maskColour);
    GetImageListBitmaps(bitmaps, bitmap, mask.GetBitmap());

    int index = ImageList_AddMasked(GetHImageList(),
                                    bitmaps.hbmp,
                                    wxColourToRGB(maskColour));
    if ( index == -1 )
    {
        wxLogError(_("Couldn't add an image to the image list."));
    }

    return index;
}

// Adds a bitmap and mask from an icon.
int wxImageList::Add(const wxIcon& icon)
{
    // We don't use ImageList_AddIcon() here as this only works for icons with
    // masks when using ILC_MASK, which we usually don't do, so reuse the
    // bitmap function instead -- even if it's slightly less efficient due to
    // extra conversions, it's simpler than handling all the various cases here.
    return Add(wxBitmap(icon));
}

// Replaces a bitmap, optionally passing a mask bitmap.
// Note that wxImageList creates new bitmaps, so you may delete
// 'bitmap' and 'mask'.
bool wxImageList::Replace(int index,
                          const wxBitmap& bitmap,
                          const wxBitmap& mask)
{
    wxMSWBitmaps bitmaps;
    GetImageListBitmaps(bitmaps, bitmap, mask);

    if ( !ImageList_Replace(GetHImageList(), index, bitmaps.hbmp, bitmaps.hbmpMask) )
    {
        wxLogLastError(wxT("ImageList_Replace()"));
        return false;
    }

    return true;
}

// Replaces a bitmap and mask from an icon.
bool wxImageList::Replace(int i, const wxIcon& icon)
{
    // Same as in Add() above, just reuse the existing function for simplicity
    // even if it means an extra conversion.
    return Replace(i, wxBitmap(icon));
}

// Removes the image at the given index.
bool wxImageList::Remove(int index)
{
    bool ok = index >= 0 && ImageList_Remove(GetHImageList(), index) != FALSE;
    if ( !ok )
    {
        wxLogLastError(wxT("ImageList_Remove()"));
    }

    return ok;
}

// Remove all images
bool wxImageList::RemoveAll()
{
    // don't use ImageList_RemoveAll() because mingw32 headers don't have it
    bool ok = ImageList_Remove(GetHImageList(), -1) != FALSE;
    if ( !ok )
    {
        wxLogLastError(wxT("ImageList_Remove()"));
    }

    return ok;
}

// Draws the given image on a dc at the specified position.
// If 'solidBackground' is true, Draw sets the image list background
// colour to the background colour of the wxDC, to speed up
// drawing by eliminating masked drawing where possible.
bool wxImageList::Draw(int index,
                       wxDC& dc,
                       int x, int y,
                       int flags,
                       bool solidBackground)
{
    wxDCImpl *impl = dc.GetImpl();
    wxMSWDCImpl *msw_impl = wxDynamicCast( impl, wxMSWDCImpl );
    if (!msw_impl)
       return false;

    HDC hDC = GetHdcOf(*msw_impl);
    wxCHECK_MSG( hDC, false, wxT("invalid wxDC in wxImageList::Draw") );

    COLORREF clr = CLR_NONE;    // transparent by default
    if ( solidBackground )
    {
        const wxBrush& brush = dc.GetBackground();
        if ( brush.IsOk() )
        {
            clr = wxColourToRGB(brush.GetColour());
        }
    }

    ImageList_SetBkColor(GetHImageList(), clr);

    UINT style = 0;
    if ( flags & wxIMAGELIST_DRAW_NORMAL )
        style |= ILD_NORMAL;
    if ( flags & wxIMAGELIST_DRAW_TRANSPARENT )
        style |= ILD_TRANSPARENT;
    if ( flags & wxIMAGELIST_DRAW_SELECTED )
        style |= ILD_SELECTED;
    if ( flags & wxIMAGELIST_DRAW_FOCUSED )
        style |= ILD_FOCUS;

    bool ok = ImageList_Draw(GetHImageList(), index, hDC, x, y, style) != 0;
    if ( !ok )
    {
        wxLogLastError(wxT("ImageList_Draw()"));
    }

    return ok;
}

// Get the bitmap
wxBitmap wxImageList::GetBitmap(int index) const
{
    int bmp_width = 0, bmp_height = 0;
    GetSize(index, bmp_width, bmp_height);

    wxBitmap bitmap(bmp_width, bmp_height);

#if wxUSE_WXDIB && wxUSE_IMAGE
    wxMemoryDC dc;
    dc.SelectObject(bitmap);

    IMAGEINFO ii;
    ImageList_GetImageInfo(GetHImageList(), index, &ii);
    if ( ii.hbmMask )
    {
        // draw it the first time to find a suitable mask colour
        if ( !const_cast<wxImageList*>(this)->Draw(index, dc, 0, 0, wxIMAGELIST_DRAW_TRANSPARENT) )
            return wxNullBitmap;

        dc.SelectObject(wxNullBitmap);

        // find the suitable mask colour
        wxImage image = bitmap.ConvertToImage();
        unsigned char r = 0, g = 0, b = 0;
        image.FindFirstUnusedColour(&r, &g, &b);

        // redraw whole image and bitmap in the mask colour
        image.Create(bmp_width, bmp_height);
        image.Replace(0, 0, 0, r, g, b);
        bitmap = wxBitmap(image);

        // redraw icon over the mask colour to actually draw it
        dc.SelectObject(bitmap);
        const_cast<wxImageList*>(this)->Draw(index, dc, 0, 0, wxIMAGELIST_DRAW_TRANSPARENT);
        dc.SelectObject(wxNullBitmap);

        // get the image, set the mask colour and convert back to get transparent bitmap
        image = bitmap.ConvertToImage();
        image.SetMaskColour(r, g, b);
        bitmap = wxBitmap(image);
    }
    else // no mask
    {
        // Just draw it normally.
        if ( !const_cast<wxImageList*>(this)->Draw(index, dc, 0, 0, wxIMAGELIST_DRAW_NORMAL) )
            return wxNullBitmap;

        dc.SelectObject(wxNullBitmap);

        // And adjust its alpha flag as the destination bitmap would get it if
        // the source one had it.
        //
        // Note that perhaps we could just call UseAlpha() which would set the
        // "has alpha" flag unconditionally as it doesn't seem to do any harm,
        // but for now only do it if necessary, just to be on the safe side,
        // even if it requires more work (and takes more time).
        bitmap.MSWUpdateAlpha();
    }
#endif
    return bitmap;
}

// Get the icon
wxIcon wxImageList::GetIcon(int index) const
{
    HICON hIcon = ImageList_ExtractIcon(0, GetHImageList(), index);
    if (hIcon)
    {
        wxIcon icon;

        int iconW, iconH;
        GetSize(index, iconW, iconH);
        icon.InitFromHICON((WXHICON)hIcon, iconW, iconH);

        return icon;
    }
    else
        return wxNullIcon;
}

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

static wxColour GetDefaultMaskColour()
{
    // use the light grey count as transparent: the trouble here is
    // that the light grey might have been changed by Windows behind
    // our back, so use the standard colour map to get its real value
    wxCOLORMAP *cmap = wxGetStdColourMap();
    wxColour col;
    wxRGBToColour(col, cmap[wxSTD_COL_BTNFACE].from);

    return col;
}
