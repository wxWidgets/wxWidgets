/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/bitmap.cpp
// Purpose:
// Author:      Robert Roebling
// RCS-ID:      $Id: bitmap.cpp 66372 2010-12-14 18:43:32Z VZ $
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/bitmap.h"

#ifndef WX_PRECOMP
    #include "wx/icon.h"
    #include "wx/image.h"
    #include "wx/colour.h"
#endif

#include "wx/rawbmp.h"

#include "wx/gtk/private/object.h"

#include <gtk/gtk.h>

extern GtkWidget *wxGetRootWindow();

static void CairoSurfaceToPixbuf(cairo_surface_t* surface, GdkPixbuf* pixbuf, int w, int h)
{
    pixbuf = gdk_pixbuf_get_from_surface(surface, 0, 0, w, h);
}

static void MaskToAlpha(cairo_surface_t* mask, GdkPixbuf* pixbuf, int w, int h)
{
    GdkPixbuf* mask_pixbuf = gdk_pixbuf_get_from_surface(mask, 0, 0, w, h);
    guchar* p = gdk_pixbuf_get_pixels(pixbuf) + 3;
    const guchar* mask_data = gdk_pixbuf_get_pixels(mask_pixbuf);
    const int rowpad = gdk_pixbuf_get_rowstride(pixbuf) - w * 4;
    const int mask_rowpad = gdk_pixbuf_get_rowstride(mask_pixbuf) - w * 3;
    for (int y = h; y; y--, p += rowpad, mask_data += mask_rowpad)
    {
        for (int x = w; x; x--, p += 4, mask_data += 3)
        {
            *p = 255;
            // no need to test all 3 components,
            //   pixels are either (0,0,0) or (0xff,0xff,0xff)
            if (mask_data[0] == 0)
                *p = 0;
        }
    }
    g_object_unref(mask_pixbuf);
}

//-----------------------------------------------------------------------------
// wxMask
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMask, wxMaskBase)

wxMask::wxMask()
{
    m_bitmap = NULL;
}

wxMask::wxMask(const wxMask& mask)
{
    if ( !mask.m_bitmap )
    {
        m_bitmap = NULL;
        return;
    }

    // create a copy of an existing mask
    m_bitmap = gdk_pixbuf_copy(mask.m_bitmap);
}

wxMask::wxMask( const wxBitmap& bitmap, const wxColour& colour )
{
    m_bitmap = NULL;
    InitFromColour(bitmap, colour);
}

#if wxUSE_PALETTE
wxMask::wxMask( const wxBitmap& bitmap, int paletteIndex )
{
    m_bitmap = NULL;
    Create( bitmap, paletteIndex );
}
#endif // wxUSE_PALETTE

wxMask::wxMask( const wxBitmap& bitmap )
{
    m_bitmap = NULL;
    InitFromMonoBitmap(bitmap);
}

wxMask::~wxMask()
{
    if (m_bitmap)
        g_object_unref (m_bitmap);
}

void wxMask::FreeData()
{
    if (m_bitmap)
    {
        g_object_unref (m_bitmap);
        m_bitmap = NULL;
    }
}

bool wxMask::InitFromColour(const wxBitmap& bitmap, const wxColour& colour)
{
    // const int w = bitmap.GetWidth();
    // const int h = bitmap.GetHeight();

    m_bitmap = gdk_pixbuf_copy(bitmap.GetPixbuf());
    return true;
}

bool wxMask::InitFromMonoBitmap(const wxBitmap& bitmap)
{
    if (!bitmap.IsOk()) return false;

    wxCHECK_MSG( bitmap.GetDepth() == 1, false, wxT("Cannot create mask from colour bitmap") );

    m_bitmap = gdk_pixbuf_copy(bitmap.GetPixbuf());

    if (!m_bitmap) return false;

    return true;
}

GdkPixbuf *wxMask::GetBitmap() const
{
    return m_bitmap;
}

//-----------------------------------------------------------------------------
// wxBitmapRefData
//-----------------------------------------------------------------------------

class wxBitmapRefData: public wxGDIRefData
{
public:
    wxBitmapRefData(int width, int height, int depth);
    virtual ~wxBitmapRefData();

    virtual bool IsOk() const;

    // GdkPixmap      *m_cairo_surface;
    cairo_surface_t      *m_cairo_surface;
    GdkPixbuf      *m_pixbuf;
    wxMask         *m_mask;
    int             m_width;
    int             m_height;
    int             m_bpp;
    bool m_alphaRequested;

private:
    // We don't provide a copy ctor as copying m_cairo_surface and m_pixbuf properly
    // is expensive and we don't want to do it implicitly (and possibly
    // accidentally). wxBitmap::CloneGDIRefData() which does need to do it does
    // it explicitly itself.
    wxDECLARE_NO_COPY_CLASS(wxBitmapRefData);
};

wxBitmapRefData::wxBitmapRefData(int width, int height, int depth)
{
    // m_cairo_surface = NULL;
    m_cairo_surface = NULL;
    m_pixbuf = NULL;
    m_mask = NULL;
    m_width = width;
    m_height = height;
    m_bpp = depth;
    // if (m_bpp < 0)
    //     m_bpp = gdk_drawable_get_depth(wxGetRootWindow()->window);
    m_alphaRequested = depth == 32;
}

wxBitmapRefData::~wxBitmapRefData()
{
    // if (m_cairo_surface)
    //     g_object_unref (m_cairo_surface);
    if (m_cairo_surface)
        g_object_unref (m_cairo_surface);
    if (m_pixbuf)
        g_object_unref (m_pixbuf);
    delete m_mask;
}

bool wxBitmapRefData::IsOk() const
{
    return m_bpp != 0;
}

//-----------------------------------------------------------------------------
// wxBitmap
//-----------------------------------------------------------------------------

#define M_BMPDATA static_cast<wxBitmapRefData*>(m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxBitmap,wxGDIObject)

wxBitmap::wxBitmap(const wxString &filename, wxBitmapType type)
{
    LoadFile(filename, type);
}

wxBitmap::wxBitmap(const char bits[], int width, int height, int depth)
{
    wxASSERT(depth == 1);
    if (width > 0 && height > 0 && depth == 1)
    {
        // FIXME The arguments need to be fixed.
        SetPixbuf(gdk_pixbuf_new_from_data((guchar*)bits, GDK_COLORSPACE_RGB, 0, 1, width, height, 1, NULL, NULL));
    }
}

wxBitmap::wxBitmap(const char* const* bits)
{
    // FIXME 
    // wxCHECK2_MSG(bits != NULL, return, wxT("invalid bitmap data"));

    // GdkBitmap* mask = NULL;
    // SetPixmap(gdk_pixmap_create_from_xpm_d(wxGetRootWindow()->window, &mask, NULL, const_cast<char**>(bits)));
    // if (!M_BMPDATA)
    //     return;
}

wxBitmap::~wxBitmap()
{
}

bool wxBitmap::Create( int width, int height, int depth )
{
    UnRef();
    wxCHECK_MSG(width >= 0 && height >= 0, false, "invalid bitmap size");
    m_refData = new wxBitmapRefData(width, height, depth);
    return true;
}

#if wxUSE_IMAGE

bool wxBitmap::CreateFromImage(const wxImage& image, int depth)
{
    UnRef();

    wxCHECK_MSG( image.IsOk(), false, wxT("invalid image") );
    wxCHECK_MSG( depth == -1 || depth == 1, false, wxT("invalid bitmap depth") );

    if (image.GetWidth() <= 0 || image.GetHeight() <= 0)
        return false;

    // FIXME Always use CreateFromImageAsPixbuf
    // create pixbuf if image has alpha and requested depth is compatible
    if (image.HasAlpha() && (depth == -1 || depth == 32))
        return CreateFromImageAsPixbuf(image);
}

bool wxBitmap::CreateFromImageAsPixbuf(const wxImage& image)
{
    wxASSERT(image.HasAlpha());

    int width = image.GetWidth();
    int height = image.GetHeight();

    Create(width, height, 32);
    GdkPixbuf* pixbuf = GetPixbuf();
    if (!pixbuf)
        return false;

    // Copy the data:
    const unsigned char* in = image.GetData();
    unsigned char *out = gdk_pixbuf_get_pixels(pixbuf);
    unsigned char *alpha = image.GetAlpha();

    int rowpad = gdk_pixbuf_get_rowstride(pixbuf) - 4 * width;

    for (int y = 0; y < height; y++, out += rowpad)
    {
        for (int x = 0; x < width; x++, alpha++, out += 4, in += 3)
        {
            out[0] = in[0];
            out[1] = in[1];
            out[2] = in[2];
            out[3] = *alpha;
        }
    }

    return true;
}

wxImage wxBitmap::ConvertToImage() const
{
    wxCHECK_MSG( IsOk(), wxNullImage, wxT("invalid bitmap") );

    const int w = GetWidth();
    const int h = GetHeight();
    wxImage image(w, h, false);
    unsigned char *data = image.GetData();

    wxCHECK_MSG(data != NULL, wxNullImage, wxT("couldn't create image") );

    // FIXME Always use Pixbuf
    // prefer pixbuf if available, it will preserve alpha and should be quicker
    if (HasPixbuf())
    {
        GdkPixbuf *pixbuf = GetPixbuf();
        unsigned char* alpha = NULL;
        if (gdk_pixbuf_get_has_alpha(pixbuf))
        {
            image.SetAlpha();
            alpha = image.GetAlpha();
        }
        const unsigned char* in = gdk_pixbuf_get_pixels(pixbuf);
        unsigned char *out = data;
        const int inc = 3 + int(alpha != NULL);
        const int rowpad = gdk_pixbuf_get_rowstride(pixbuf) - inc * w;

        for (int y = 0; y < h; y++, in += rowpad)
        {
            for (int x = 0; x < w; x++, in += inc, out += 3)
            {
                out[0] = in[0];
                out[1] = in[1];
                out[2] = in[2];
                if (alpha != NULL)
                    *alpha++ = in[3];
            }
        }
    }
    // FIXME Deal with mask
    // // convert mask, unless there is already alpha
    // if (GetMask() && !image.HasAlpha())
    // {
    //     // we hard code the mask colour for now but we could also make an
    //     // effort (and waste time) to choose a colour not present in the
    //     // image already to avoid having to fudge the pixels below --
    //     // whether it's worth to do it is unclear however
    //     const int MASK_RED = 1;
    //     const int MASK_GREEN = 2;
    //     const int MASK_BLUE = 3;
    //     const int MASK_BLUE_REPLACEMENT = 2;

    //     image.SetMaskColour(MASK_RED, MASK_GREEN, MASK_BLUE);
    //     GdkImage* image_mask = gdk_drawable_get_image(GetMask()->GetBitmap(), 0, 0, w, h);

    //     for (int y = 0; y < h; y++)
    //     {
    //         for (int x = 0; x < w; x++, data += 3)
    //         {
    //             if (gdk_image_get_pixel(image_mask, x, y) == 0)
    //             {
    //                 data[0] = MASK_RED;
    //                 data[1] = MASK_GREEN;
    //                 data[2] = MASK_BLUE;
    //             }
    //             else if (data[0] == MASK_RED && data[1] == MASK_GREEN && data[2] == MASK_BLUE)
    //             {
    //                 // we have to fudge the colour a bit to prevent
    //                 // this pixel from appearing transparent
    //                 data[2] = MASK_BLUE_REPLACEMENT;
    //             }
    //         }
    //     }
    //     g_object_unref(image_mask);
    // }

    return image;
}

#endif // wxUSE_IMAGE

int wxBitmap::GetHeight() const
{
    wxCHECK_MSG( IsOk(), -1, wxT("invalid bitmap") );

    return M_BMPDATA->m_height;
}

int wxBitmap::GetWidth() const
{
    wxCHECK_MSG( IsOk(), -1, wxT("invalid bitmap") );

    return M_BMPDATA->m_width;
}

int wxBitmap::GetDepth() const
{
    wxCHECK_MSG( IsOk(), -1, wxT("invalid bitmap") );

    return M_BMPDATA->m_bpp;
}

wxMask *wxBitmap::GetMask() const
{
    wxCHECK_MSG( IsOk(), NULL, wxT("invalid bitmap") );

    return M_BMPDATA->m_mask;
}

void wxBitmap::SetMask( wxMask *mask )
{
    wxCHECK_RET( IsOk(), wxT("invalid bitmap") );

    AllocExclusive();
    delete M_BMPDATA->m_mask;
    M_BMPDATA->m_mask = mask;
}

bool wxBitmap::CopyFromIcon(const wxIcon& icon)
{
    *this = icon;
    return IsOk();
}

wxBitmap wxBitmap::GetSubBitmap( const wxRect& rect) const
{
    wxBitmap ret;

    wxCHECK_MSG(IsOk(), ret, wxT("invalid bitmap"));

    const int w = rect.width;
    const int h = rect.height;
    const wxBitmapRefData* bmpData = M_BMPDATA;

    wxCHECK_MSG(rect.x >= 0 && rect.y >= 0 &&
                rect.x + w <= bmpData->m_width &&
                rect.y + h <= bmpData->m_height,
                ret, wxT("invalid bitmap region"));

    wxBitmapRefData * const newRef = new wxBitmapRefData(w, h, bmpData->m_bpp);
    ret.m_refData = newRef;

    if (bmpData->m_pixbuf)
    {
        GdkPixbuf* pixbuf =
            gdk_pixbuf_new_subpixbuf(bmpData->m_pixbuf, rect.x, rect.y, w, h);
        newRef->m_pixbuf = gdk_pixbuf_copy(pixbuf);
        g_object_unref(pixbuf);
    }
    // FIXME Deal with mask
    // if (bmpData->m_mask && bmpData->m_mask->m_bitmap)
    // {
    //     GdkPixmap* sub_mask = gdk_pixmap_new(bmpData->m_mask->m_bitmap, w, h, 1);
    //     newRef->m_mask = new wxMask;
    //     newRef->m_mask->m_bitmap = sub_mask;
    //     GdkGC* gc = gdk_gc_new(sub_mask);
    //     gdk_draw_drawable(
    //         sub_mask, gc, bmpData->m_mask->m_bitmap, rect.x, rect.y, 0, 0, w, h);
    //     g_object_unref(gc);
    // }

    return ret;
}

bool wxBitmap::SaveFile( const wxString &name, wxBitmapType type, const wxPalette *WXUNUSED(palette) ) const
{
    wxCHECK_MSG( IsOk(), false, wxT("invalid bitmap") );

#if wxUSE_IMAGE
    wxImage image = ConvertToImage();
    if (image.IsOk() && image.SaveFile(name, type))
        return true;
#endif
    const char* type_name = NULL;
    switch (type)
    {
        case wxBITMAP_TYPE_BMP:  type_name = "bmp";  break;
        case wxBITMAP_TYPE_ICO:  type_name = "ico";  break;
        case wxBITMAP_TYPE_JPEG: type_name = "jpeg"; break;
        case wxBITMAP_TYPE_PNG:  type_name = "png";  break;
        default: break;
    }
    return type_name &&
        gdk_pixbuf_save(GetPixbuf(), name.fn_str(), type_name, NULL, NULL);
}

bool wxBitmap::LoadFile( const wxString &name, wxBitmapType type )
{
#if wxUSE_IMAGE
    wxImage image;
    if (image.LoadFile(name, type) && image.IsOk())
        *this = wxBitmap(image);
    else
#endif
    {
        wxUnusedVar(type); // The type is detected automatically by GDK.

        UnRef();
        GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(name.fn_str(), NULL);
        if (pixbuf)
            SetPixbuf(pixbuf);
    }

    return IsOk();
}

#if wxUSE_PALETTE
wxPalette *wxBitmap::GetPalette() const
{
    return NULL;
}

void wxBitmap::SetPalette(const wxPalette& WXUNUSED(palette))
{
    // TODO
}
#endif // wxUSE_PALETTE

void wxBitmap::SetHeight( int height )
{
    AllocExclusive();
    M_BMPDATA->m_height = height;
}

void wxBitmap::SetWidth( int width )
{
    AllocExclusive();
    M_BMPDATA->m_width = width;
}

void wxBitmap::SetDepth( int depth )
{
    AllocExclusive();
    M_BMPDATA->m_bpp = depth;
}

GdkPixbuf *wxBitmap::GetPixbuf() const
{
    wxCHECK_MSG( IsOk(), NULL, wxT("invalid bitmap") );

    wxBitmapRefData* bmpData = M_BMPDATA;
    if (bmpData->m_pixbuf)
        return bmpData->m_pixbuf;

    const int w = bmpData->m_width;
    const int h = bmpData->m_height;
    GdkPixbuf* mask = NULL;
    if (bmpData->m_mask)
        mask = bmpData->m_mask->m_bitmap;
    const bool useAlpha = bmpData->m_alphaRequested || mask;
    bmpData->m_pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, useAlpha, 8, w, h);
    if (bmpData->m_cairo_surface)
        CairoSurfaceToPixbuf(bmpData->m_cairo_surface, bmpData->m_pixbuf, w, h);
    // FIXME Uncomment to see the error
    // if (mask)
    //     MaskToAlpha(mask, bmpData->m_pixbuf, w, h);
    return bmpData->m_pixbuf;
}

bool wxBitmap::HasPixbuf() const
{
    wxCHECK_MSG( IsOk(), false, wxT("invalid bitmap") );

    return M_BMPDATA->m_pixbuf != NULL;
}

void wxBitmap::SetPixbuf(GdkPixbuf* pixbuf)
{
    UnRef();

    if (!pixbuf)
        return;

    int depth = -1;
    if (gdk_pixbuf_get_has_alpha(pixbuf))
        depth = 32;
    m_refData = new wxBitmapRefData(
        gdk_pixbuf_get_width(pixbuf), gdk_pixbuf_get_height(pixbuf), depth);

    M_BMPDATA->m_pixbuf = pixbuf;
}

void wxBitmap::PurgeOtherRepresentations(wxBitmap::Representation keep)
{
    if (keep == Pixmap && HasPixbuf())
    {
        g_object_unref (M_BMPDATA->m_pixbuf);
        M_BMPDATA->m_pixbuf = NULL;
    }
}

#ifdef wxHAS_RAW_BITMAP
void *wxBitmap::GetRawData(wxPixelDataBase& data, int bpp)
{
    void* bits = NULL;
    GdkPixbuf *pixbuf = GetPixbuf();
    const bool hasAlpha = HasAlpha();

    // allow access if bpp is valid and matches existence of alpha
    if ( pixbuf && ((bpp == 24 && !hasAlpha) || (bpp == 32 && hasAlpha)) )
    {
        data.m_height = gdk_pixbuf_get_height( pixbuf );
        data.m_width = gdk_pixbuf_get_width( pixbuf );
        data.m_stride = gdk_pixbuf_get_rowstride( pixbuf );
        bits = gdk_pixbuf_get_pixels(pixbuf);
    }
    return bits;
}

void wxBitmap::UngetRawData(wxPixelDataBase& WXUNUSED(data))
{
}
#endif // wxHAS_RAW_BITMAP

bool wxBitmap::HasAlpha() const
{
    const wxBitmapRefData* bmpData = M_BMPDATA;
    return bmpData && (bmpData->m_alphaRequested ||
        (bmpData->m_pixbuf && gdk_pixbuf_get_has_alpha(bmpData->m_pixbuf)));
}

wxGDIRefData* wxBitmap::CreateGDIRefData() const
{
    return new wxBitmapRefData(0, 0, 0);
}

wxGDIRefData* wxBitmap::CloneGDIRefData(const wxGDIRefData* data) const
{
    const wxBitmapRefData* oldRef = static_cast<const wxBitmapRefData*>(data);
    wxBitmapRefData * const newRef = new wxBitmapRefData(oldRef->m_width,
                                                         oldRef->m_height,
                                                         oldRef->m_bpp);
    if (oldRef->m_pixbuf != NULL)
    {
        newRef->m_pixbuf = gdk_pixbuf_copy(oldRef->m_pixbuf);
    }
    if (oldRef->m_mask != NULL)
    {
        newRef->m_mask = new wxMask(*oldRef->m_mask);
    }

    return newRef;
}

/* static */ void wxBitmap::InitStandardHandlers()
{
    // TODO: Insert handler based on GdkPixbufs handler later
}
