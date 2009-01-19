/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/bitmap.cpp
// Purpose:
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/bitmap.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/palette.h"
    #include "wx/icon.h"
    #include "wx/math.h"
    #include "wx/image.h"
    #include "wx/colour.h"
#endif

#include "wx/rawbmp.h"

#include "wx/gtk/private/object.h"

#include <gtk/gtk.h>

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern GtkWidget *wxGetRootWindow();

//-----------------------------------------------------------------------------
// wxMask
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMask,wxObject)

wxMask::wxMask()
{
    m_bitmap = NULL;
}

wxMask::wxMask( const wxBitmap& bitmap, const wxColour& colour )
{
    m_bitmap = NULL;
    Create( bitmap, colour );
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
    Create( bitmap );
}

wxMask::~wxMask()
{
    if (m_bitmap)
        g_object_unref (m_bitmap);
}

bool wxMask::Create( const wxBitmap& bitmap,
                     const wxColour& colour )
{
    if (m_bitmap)
    {
        g_object_unref (m_bitmap);
        m_bitmap = NULL;
    }

    const int w = bitmap.GetWidth();
    const int h = bitmap.GetHeight();

    // create mask as XBM format bitmap

    // one bit per pixel, each row starts on a byte boundary
    const size_t out_size = size_t((w + 7) / 8) * unsigned(h);
    wxByte* out = new wxByte[out_size];
    // set bits are unmasked
    memset(out, 0xff, out_size);
    unsigned bit_index = 0;
    if (bitmap.HasPixbuf())
    {
        const wxByte r_mask = colour.Red();
        const wxByte g_mask = colour.Green();
        const wxByte b_mask = colour.Blue();
        GdkPixbuf* pixbuf = bitmap.GetPixbuf();
        const wxByte* in = gdk_pixbuf_get_pixels(pixbuf);
        const int inc = 3 + int(gdk_pixbuf_get_has_alpha(pixbuf) != 0);
        const int rowpadding = gdk_pixbuf_get_rowstride(pixbuf) - inc * w;
        for (int y = 0; y < h; y++, in += rowpadding)
        {
            for (int x = 0; x < w; x++, in += inc, bit_index++)
                if (in[0] == r_mask && in[1] == g_mask && in[2] == b_mask)
                    out[bit_index >> 3] ^= 1 << (bit_index & 7);
            // move index to next byte boundary
            bit_index = (bit_index + 7) & ~7u;
        }
    }
    else
    {
        GdkImage* image = gdk_drawable_get_image(bitmap.GetPixmap(), 0, 0, w, h);
        GdkColormap* colormap = gdk_image_get_colormap(image);
        guint32 mask_pixel;
        if (colormap == NULL)
            // mono bitmap, white is pixel value 0
            mask_pixel = guint32(colour.Red() != 255 || colour.Green() != 255 || colour.Blue() != 255);
        else
        {
            wxColor c(colour);
            c.CalcPixel(colormap);
            mask_pixel = c.GetPixel();
        }
        for (int y = 0; y < h; y++)
        {
            for (int x = 0; x < w; x++, bit_index++)
                if (gdk_image_get_pixel(image, x, y) == mask_pixel)
                    out[bit_index >> 3] ^= 1 << (bit_index & 7);
            bit_index = (bit_index + 7) & ~7u;
        }
        g_object_unref(image);
    }
    m_bitmap = gdk_bitmap_create_from_data(wxGetRootWindow()->window, (char*)out, w, h);
    delete[] out;
    return true;
}

#if wxUSE_PALETTE
bool wxMask::Create( const wxBitmap& bitmap, int paletteIndex )
{
    unsigned char r,g,b;
    wxPalette *pal = bitmap.GetPalette();

    wxCHECK_MSG( pal, false, wxT("Cannot create mask from bitmap without palette") );

    pal->GetRGB(paletteIndex, &r, &g, &b);

    return Create(bitmap, wxColour(r, g, b));
}
#endif // wxUSE_PALETTE

bool wxMask::Create( const wxBitmap& bitmap )
{
    if (m_bitmap)
    {
        g_object_unref (m_bitmap);
        m_bitmap = NULL;
    }

    if (!bitmap.IsOk()) return false;

    wxCHECK_MSG( bitmap.GetDepth() == 1, false, wxT("Cannot create mask from colour bitmap") );

    m_bitmap = gdk_pixmap_new( wxGetRootWindow()->window, bitmap.GetWidth(), bitmap.GetHeight(), 1 );

    if (!m_bitmap) return false;

    wxGtkObject<GdkGC> gc(gdk_gc_new( m_bitmap ));
    gdk_gc_set_function(gc, GDK_COPY_INVERT);
    gdk_draw_drawable(m_bitmap, gc, bitmap.GetPixmap(), 0, 0, 0, 0, bitmap.GetWidth(), bitmap.GetHeight());

    return true;
}

GdkBitmap *wxMask::GetBitmap() const
{
    return m_bitmap;
}

//-----------------------------------------------------------------------------
// wxBitmap
//-----------------------------------------------------------------------------

class wxBitmapRefData: public wxGDIRefData
{
public:
    wxBitmapRefData();
    virtual ~wxBitmapRefData();

    virtual bool IsOk() const { return m_pixmap || m_pixbuf; }

    GdkPixmap      *m_pixmap;
    GdkPixbuf      *m_pixbuf;
    wxMask         *m_mask;
    int             m_width;
    int             m_height;
    int             m_bpp;
#if wxUSE_PALETTE
    wxPalette      *m_palette;
#endif // wxUSE_PALETTE
};

wxBitmapRefData::wxBitmapRefData()
{
    m_pixmap = NULL;
    m_pixbuf = NULL;
    m_mask = NULL;
    m_width = 0;
    m_height = 0;
    m_bpp = 0;
#if wxUSE_PALETTE
    m_palette = NULL;
#endif // wxUSE_PALETTE
}

wxBitmapRefData::~wxBitmapRefData()
{
    if (m_pixmap)
        g_object_unref (m_pixmap);
    if (m_pixbuf)
        g_object_unref (m_pixbuf);
    delete m_mask;
#if wxUSE_PALETTE
    delete m_palette;
#endif // wxUSE_PALETTE
}

//-----------------------------------------------------------------------------

#define M_BMPDATA static_cast<wxBitmapRefData*>(m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxBitmap,wxGDIObject)

wxBitmap::wxBitmap(int width, int height, int depth)
{
    Create(width, height, depth);
}

wxBitmap::wxBitmap(const wxString &filename, wxBitmapType type)
{
    LoadFile(filename, type);
}

wxBitmap::wxBitmap(const char bits[], int width, int height, int depth)
{
    wxASSERT(depth == 1);
    if (width > 0 && height > 0 && depth == 1)
    {
        SetPixmap(gdk_bitmap_create_from_data(wxGetRootWindow()->window, bits, width, height));
    }
}

wxBitmap::wxBitmap(const char* const* bits)
{
    wxCHECK2_MSG(bits != NULL, return, wxT("invalid bitmap data"));

    GdkBitmap* mask = NULL;
    SetPixmap(gdk_pixmap_create_from_xpm_d(wxGetRootWindow()->window, &mask, NULL, const_cast<char**>(bits)));
    if (!M_BMPDATA)
        return;

    if (M_BMPDATA->m_pixmap != NULL && mask != NULL)
    {
        M_BMPDATA->m_mask = new wxMask;
        M_BMPDATA->m_mask->m_bitmap = mask;
    }
}

wxBitmap::~wxBitmap()
{
}

bool wxBitmap::Create( int width, int height, int depth )
{
    UnRef();

    if ( width <= 0 || height <= 0 )
    {
        return false;
    }

    const GdkVisual* visual = wxTheApp->GetGdkVisual();

    if (depth == 32)
    {
        SetPixbuf(gdk_pixbuf_new(GDK_COLORSPACE_RGB, true, 8, width, height), 32);

        if (!M_BMPDATA)
            return false;

        // must initialize alpha, otherwise GetPixmap()
        // will create a mask out of garbage
        gdk_pixbuf_fill(M_BMPDATA->m_pixbuf, 0x000000ff);
    }
    else if (depth == 24)
    {
        if (visual->depth == depth)
            SetPixmap(gdk_pixmap_new(wxGetRootWindow()->window, width, height, depth));
        else
            SetPixbuf(gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8, width, height), 24);
    }
    else
    {
        if (depth != 1)
        {
            if (depth == -1)
                depth = visual->depth;
        }
        SetPixmap(gdk_pixmap_new(wxGetRootWindow()->window, width, height, depth));
    }

    return IsOk();
}

#if wxUSE_IMAGE

bool wxBitmap::CreateFromImage(const wxImage& image, int depth)
{
    UnRef();

    wxCHECK_MSG( image.IsOk(), false, wxT("invalid image") );
    wxCHECK_MSG( depth == -1 || depth == 1, false, wxT("invalid bitmap depth") );

    if (image.GetWidth() <= 0 || image.GetHeight() <= 0)
        return false;

    // create pixbuf if image has alpha and requested depth is compatible
    if (image.HasAlpha() && (depth == -1 || depth == 32))
        return CreateFromImageAsPixbuf(image);

    // otherwise create pixmap, if alpha is present it will be converted to mask
    return CreateFromImageAsPixmap(image, depth);
}

bool wxBitmap::CreateFromImageAsPixmap(const wxImage& image, int depth)
{
    const int w = image.GetWidth();
    const int h = image.GetHeight();
    if (depth == 1)
    {
        // create XBM format bitmap

        // one bit per pixel, each row starts on a byte boundary
        const size_t out_size = size_t((w + 7) / 8) * unsigned(h);
        wxByte* out = new wxByte[out_size];
        // set bits are black
        memset(out, 0xff, out_size);
        const wxByte* in = image.GetData();
        unsigned bit_index = 0;
        for (int y = 0; y < h; y++)
        {
            for (int x = 0; x < w; x++, in += 3, bit_index++)
                if (in[0] == 255 && in[1] == 255 && in[2] == 255)
                    out[bit_index >> 3] ^= 1 << (bit_index & 7);
            // move index to next byte boundary
            bit_index = (bit_index + 7) & ~7u;
        }
        SetPixmap(gdk_bitmap_create_from_data(wxGetRootWindow()->window, (char*)out, w, h));
        delete[] out;

        if (!M_BMPDATA)     // SetPixmap may have failed
            return false;
    }
    else
    {
        SetPixmap(gdk_pixmap_new(wxGetRootWindow()->window, w, h, depth));
        if (!M_BMPDATA)
            return false;

        wxGtkObject<GdkGC> gc(gdk_gc_new(M_BMPDATA->m_pixmap));
        gdk_draw_rgb_image(
            M_BMPDATA->m_pixmap, gc,
            0, 0, w, h,
            GDK_RGB_DITHER_NONE, image.GetData(), w * 3);
    }

    const wxByte* alpha = image.GetAlpha();
    if (alpha != NULL || image.HasMask())
    {
        // create mask as XBM format bitmap

        const size_t out_size = size_t((w + 7) / 8) * unsigned(h);
        wxByte* out = new wxByte[out_size];
        memset(out, 0xff, out_size);
        unsigned bit_index = 0;
        if (alpha != NULL)
        {
            for (int y = 0; y < h; y++)
            {
                for (int x = 0; x < w; x++, bit_index++)
                    if (*alpha++ < wxIMAGE_ALPHA_THRESHOLD)
                        out[bit_index >> 3] ^= 1 << (bit_index & 7);
                bit_index = (bit_index + 7) & ~7u;
            }
        }
        else
        {
            const wxByte r_mask = image.GetMaskRed();
            const wxByte g_mask = image.GetMaskGreen();
            const wxByte b_mask = image.GetMaskBlue();
            const wxByte* in = image.GetData();
            for (int y = 0; y < h; y++)
            {
                for (int x = 0; x < w; x++, in += 3, bit_index++)
                    if (in[0] == r_mask && in[1] == g_mask && in[2] == b_mask)
                        out[bit_index >> 3] ^= 1 << (bit_index & 7);
                bit_index = (bit_index + 7) & ~7u;
            }
        }
        wxMask* mask = new wxMask;
        mask->m_bitmap = gdk_bitmap_create_from_data(M_BMPDATA->m_pixmap, (char*)out, w, h);
        SetMask(mask);
        delete[] out;
    }
    return IsOk();
}

bool wxBitmap::CreateFromImageAsPixbuf(const wxImage& image)
{
    wxASSERT(image.HasAlpha());

    int width = image.GetWidth();
    int height = image.GetHeight();

    Create(width, height, 32);
    GdkPixbuf* pixbuf = M_BMPDATA->m_pixbuf;
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
    else
    {
        GdkPixmap* pixmap = GetPixmap();
        GdkPixmap* pixmap_invert = NULL;
        if (GetDepth() == 1)
        {
            // mono bitmaps are inverted, i.e. 0 is white
            pixmap_invert = gdk_pixmap_new(pixmap, w, h, 1);
            wxGtkObject<GdkGC> gc(gdk_gc_new(pixmap_invert));
            gdk_gc_set_function(gc, GDK_COPY_INVERT);
            gdk_draw_drawable(pixmap_invert, gc, pixmap, 0, 0, 0, 0, w, h);
            pixmap = pixmap_invert;
        }
        // create a pixbuf which shares data with the wxImage
        GdkPixbuf* pixbuf = gdk_pixbuf_new_from_data(
            data, GDK_COLORSPACE_RGB, false, 8, w, h, 3 * w, NULL, NULL);

        gdk_pixbuf_get_from_drawable(pixbuf, pixmap, NULL, 0, 0, 0, 0, w, h);

        g_object_unref(pixbuf);
        if (pixmap_invert != NULL)
            g_object_unref(pixmap_invert);
    }
    // convert mask, unless there is already alpha
    if (GetMask() && !image.HasAlpha())
    {
        // we hard code the mask colour for now but we could also make an
        // effort (and waste time) to choose a colour not present in the
        // image already to avoid having to fudge the pixels below --
        // whether it's worth to do it is unclear however
        const int MASK_RED = 1;
        const int MASK_GREEN = 2;
        const int MASK_BLUE = 3;
        const int MASK_BLUE_REPLACEMENT = 2;

        image.SetMaskColour(MASK_RED, MASK_GREEN, MASK_BLUE);
        GdkImage* image_mask = gdk_drawable_get_image(GetMask()->GetBitmap(), 0, 0, w, h);

        for (int y = 0; y < h; y++)
        {
            for (int x = 0; x < w; x++, data += 3)
            {
                if (gdk_image_get_pixel(image_mask, x, y) == 0)
                {
                    data[0] = MASK_RED;
                    data[1] = MASK_GREEN;
                    data[2] = MASK_BLUE;
                }
                else if (data[0] == MASK_RED && data[1] == MASK_GREEN && data[2] == MASK_BLUE)
                {
                    // we have to fudge the colour a bit to prevent
                    // this pixel from appearing transparent
                    data[2] = MASK_BLUE_REPLACEMENT;
                }
            }
        }
        g_object_unref(image_mask);
    }

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
    wxCHECK_MSG(rect.x >= 0 && rect.y >= 0 &&
                rect.x + rect.width <= M_BMPDATA->m_width &&
                rect.y + rect.height <= M_BMPDATA->m_height,
                ret, wxT("invalid bitmap region"));

    if (HasPixbuf() || M_BMPDATA->m_bpp == 32)
    {
        GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB,
                                           gdk_pixbuf_get_has_alpha(GetPixbuf()),
                                           8, rect.width, rect.height);
        if (!pixbuf)
            return ret;

        ret.SetPixbuf(pixbuf, M_BMPDATA->m_bpp);
        gdk_pixbuf_copy_area(GetPixbuf(),
                             rect.x, rect.y, rect.width, rect.height,
                             pixbuf, 0, 0);
    }
    else
    {
        ret.Create(rect.width, rect.height, M_BMPDATA->m_bpp);
        wxGtkObject<GdkGC> gc(gdk_gc_new( ret.GetPixmap() ));
        gdk_draw_drawable( ret.GetPixmap(), gc, GetPixmap(), rect.x, rect.y, 0, 0, rect.width, rect.height );
    }
    // make mask, unless there is already alpha
    if (GetMask() && !HasAlpha())
    {
        wxMask *mask = new wxMask;
        mask->m_bitmap = gdk_pixmap_new( wxGetRootWindow()->window, rect.width, rect.height, 1 );

        wxGtkObject<GdkGC> gc(gdk_gc_new( mask->m_bitmap ));
        gdk_draw_drawable(mask->m_bitmap, gc, M_BMPDATA->m_mask->m_bitmap, rect.x, rect.y, 0, 0, rect.width, rect.height);

        ret.SetMask( mask );
    }

    return ret;
}

bool wxBitmap::SaveFile( const wxString &name, wxBitmapType type, const wxPalette *WXUNUSED(palette) ) const
{
    wxCHECK_MSG( IsOk(), false, wxT("invalid bitmap") );

#if wxUSE_IMAGE
    // Try to save the bitmap via wxImage handlers:
    wxImage image = ConvertToImage();
    return image.Ok() && image.SaveFile(name, type);
#else // !wxUSE_IMAGE
    wxUnusedVar(name);
    wxUnusedVar(type);

    return false;
#endif // wxUSE_IMAGE
}

bool wxBitmap::LoadFile( const wxString &name, wxBitmapType type )
{
    UnRef();

    if (type == wxBITMAP_TYPE_XPM)
    {
        GdkBitmap *mask = NULL;
        SetPixmap(gdk_pixmap_create_from_xpm(wxGetRootWindow()->window, &mask, NULL, name.fn_str()));
        if (!M_BMPDATA)
            return false;   // do not set the mask

        if (mask)
        {
            M_BMPDATA->m_mask = new wxMask;
            M_BMPDATA->m_mask->m_bitmap = mask;
        }
    }
#if wxUSE_IMAGE
    else // try if wxImage can load it
    {
        wxImage image;
        if (image.LoadFile(name, type) && image.Ok())
            CreateFromImage(image, -1);
    }
#endif // wxUSE_IMAGE

    return IsOk();
}

#if wxUSE_PALETTE
wxPalette *wxBitmap::GetPalette() const
{
    wxCHECK_MSG(IsOk(), NULL, wxT("invalid bitmap"));

    return M_BMPDATA->m_palette;
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

void wxBitmap::SetPixmap( GdkPixmap *pixmap )
{
    if (!pixmap)
        return;

    if (!m_refData)
        m_refData = new wxBitmapRefData;

    // AllocExclusive should not be needed for this internal function
    wxASSERT(m_refData->GetRefCount() == 1);
    wxASSERT(M_BMPDATA->m_pixmap == NULL);
    M_BMPDATA->m_pixmap = pixmap;
    gdk_drawable_get_size(pixmap, &M_BMPDATA->m_width, &M_BMPDATA->m_height);
    M_BMPDATA->m_bpp = gdk_drawable_get_depth(pixmap);
    PurgeOtherRepresentations(Pixmap);
}

GdkPixmap *wxBitmap::GetPixmap() const
{
    wxCHECK_MSG( IsOk(), NULL, wxT("invalid bitmap") );

    // create the pixmap on the fly if we use Pixbuf representation:
    if (M_BMPDATA->m_pixmap == NULL)
    {
        GdkPixmap** pmask = NULL;
        if (gdk_pixbuf_get_has_alpha(M_BMPDATA->m_pixbuf))
        {
            // make new mask from alpha
            delete M_BMPDATA->m_mask;
            M_BMPDATA->m_mask = new wxMask;
            pmask = &M_BMPDATA->m_mask->m_bitmap;
        }
        gdk_pixbuf_render_pixmap_and_mask(M_BMPDATA->m_pixbuf,
                                          &M_BMPDATA->m_pixmap,
                                          pmask,
                                          0x80 /* alpha threshold */);
    }

    return M_BMPDATA->m_pixmap;
}

bool wxBitmap::HasPixmap() const
{
    wxCHECK_MSG( IsOk(), false, wxT("invalid bitmap") );

    return M_BMPDATA->m_pixmap != NULL;
}

GdkPixbuf *wxBitmap::GetPixbuf() const
{
    wxCHECK_MSG( IsOk(), NULL, wxT("invalid bitmap") );

    if (M_BMPDATA->m_pixbuf == NULL)
    {

        int width = GetWidth();
        int height = GetHeight();

        GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB,
                                           GetMask() != NULL,
                                           8, width, height);
        M_BMPDATA->m_pixbuf = pixbuf;
        gdk_pixbuf_get_from_drawable(pixbuf, M_BMPDATA->m_pixmap, NULL,
                                     0, 0, 0, 0, width, height);
        // apply the mask to created pixbuf:
        if (M_BMPDATA->m_pixbuf && M_BMPDATA->m_mask)
        {
            GdkPixbuf *pmask =
                gdk_pixbuf_get_from_drawable(NULL,
                                             M_BMPDATA->m_mask->GetBitmap(),
                                             NULL,
                                             0, 0, 0, 0, width, height);
            if (pmask)
            {
                guchar *bmp = gdk_pixbuf_get_pixels(pixbuf);
                guchar *mask = gdk_pixbuf_get_pixels(pmask);
                int bmprowinc = gdk_pixbuf_get_rowstride(pixbuf) - 4 * width;
                int maskrowinc = gdk_pixbuf_get_rowstride(pmask) - 3 * width;

                for (int y = 0; y < height;
                     y++, bmp += bmprowinc, mask += maskrowinc)
                {
                    for (int x = 0; x < width; x++, bmp += 4, mask += 3)
                    {
                        if (mask[0] == 0 /*black pixel*/)
                            bmp[3] = 0;
                    }
                }

                g_object_unref (pmask);
            }
        }
    }

    return M_BMPDATA->m_pixbuf;
}

bool wxBitmap::HasPixbuf() const
{
    wxCHECK_MSG( IsOk(), false, wxT("invalid bitmap") );

    return M_BMPDATA->m_pixbuf != NULL;
}

void wxBitmap::SetPixbuf(GdkPixbuf* pixbuf, int depth)
{
    if (!pixbuf)
        return;

    if (!m_refData)
        m_refData = new wxBitmapRefData;

    // AllocExclusive should not be needed for this internal function
    wxASSERT(m_refData->GetRefCount() == 1);
    wxASSERT(M_BMPDATA->m_pixbuf == NULL);
    M_BMPDATA->m_pixbuf = pixbuf;
    M_BMPDATA->m_width = gdk_pixbuf_get_width(pixbuf);
    M_BMPDATA->m_height = gdk_pixbuf_get_height(pixbuf);
    // if depth specified
    if (depth != 0)
        M_BMPDATA->m_bpp = depth;
    else if (M_BMPDATA->m_bpp == 0)
        // use something reasonable
        M_BMPDATA->m_bpp = wxTheApp->GetGdkVisual()->depth;
    PurgeOtherRepresentations(Pixbuf);
}

void wxBitmap::PurgeOtherRepresentations(wxBitmap::Representation keep)
{
    if (keep == Pixmap && HasPixbuf())
    {
        g_object_unref (M_BMPDATA->m_pixbuf);
        M_BMPDATA->m_pixbuf = NULL;
    }
    if (keep == Pixbuf && HasPixmap())
    {
        g_object_unref (M_BMPDATA->m_pixmap);
        M_BMPDATA->m_pixmap = NULL;
    }
}

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

bool wxBitmap::HasAlpha() const
{
    return m_refData != NULL && M_BMPDATA->m_pixbuf != NULL &&
        gdk_pixbuf_get_has_alpha(M_BMPDATA->m_pixbuf);
}

wxGDIRefData* wxBitmap::CreateGDIRefData() const
{
    return new wxBitmapRefData;
}

wxGDIRefData* wxBitmap::CloneGDIRefData(const wxGDIRefData* data) const
{
    const wxBitmapRefData* oldRef = static_cast<const wxBitmapRefData*>(data);
    wxBitmapRefData* newRef = new wxBitmapRefData;
    newRef->m_width = oldRef->m_width;
    newRef->m_height = oldRef->m_height;
    newRef->m_bpp = oldRef->m_bpp;
    if (oldRef->m_pixmap != NULL)
    {
        newRef->m_pixmap = gdk_pixmap_new(
            oldRef->m_pixmap, oldRef->m_width, oldRef->m_height,
            // use pixmap depth, m_bpp may not match
            gdk_drawable_get_depth(oldRef->m_pixmap));
        wxGtkObject<GdkGC> gc(gdk_gc_new(newRef->m_pixmap));
        gdk_draw_drawable(
            newRef->m_pixmap, gc, oldRef->m_pixmap, 0, 0, 0, 0, -1, -1);
    }
    if (oldRef->m_pixbuf != NULL)
    {
        newRef->m_pixbuf = gdk_pixbuf_copy(oldRef->m_pixbuf);
    }
    if (oldRef->m_mask != NULL)
    {
        newRef->m_mask = new wxMask;
        newRef->m_mask->m_bitmap = gdk_pixmap_new(
            oldRef->m_mask->m_bitmap, oldRef->m_width, oldRef->m_height, 1);
        wxGtkObject<GdkGC> gc(gdk_gc_new(newRef->m_mask->m_bitmap));
        gdk_draw_drawable(newRef->m_mask->m_bitmap,
            gc, oldRef->m_mask->m_bitmap, 0, 0, 0, 0, -1, -1);
    }
#if wxUSE_PALETTE
    // implement this if SetPalette is ever implemented
    wxASSERT(oldRef->m_palette == NULL);
#endif

    return newRef;
}

/* static */ void wxBitmap::InitStandardHandlers()
{
    // TODO: Insert handler based on GdkPixbufs handler later
}
