/////////////////////////////////////////////////////////////////////////////
// Name:        src/retro68/bitmap.cpp
// Purpose:     wxBitmap implementation using offscreen GWorlds
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/bitmap.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/colour.h"
    #include "wx/image.h"
    #include "wx/log.h"
#endif

#include "wx/rawbmp.h"

#include "wx/retro68/dc.h"
#include "wx/retro68/private/qd.h"

#include <string.h>

// ----------------------------------------------------------------------------
// wxBitmapRefData
// ----------------------------------------------------------------------------

class wxBitmapRefData : public wxGDIRefData
{
public:
    wxBitmapRefData() = default;

    wxBitmapRefData(const wxBitmapRefData& data)
        : wxGDIRefData()
    {
        if ( !Create(data.m_width, data.m_height, data.m_depth) )
            return;

        m_hasAlpha = data.m_hasAlpha;
        m_scaleFactor = data.m_scaleFactor;

        int rowBytesSrc, rowBytesDst;
        const unsigned char* src = data.Lock(&rowBytesSrc);
        unsigned char* dst = Lock(&rowBytesDst);

        for ( int y = 0; y < m_height; y++ )
            memcpy(dst + y * rowBytesDst, src + y * rowBytesSrc, m_width * 4);

        Unlock();
        data.Unlock();

        if ( data.m_mask )
            m_mask = new wxMask(*data.m_mask);
    }

    virtual ~wxBitmapRefData()
    {
        delete m_mask;

        if ( m_gworld )
        {
            wxRetro68DCImpl::InvalidateCachedPortState();
            DisposeGWorld(m_gworld);
        }
    }

    virtual bool IsOk() const override { return m_gworld != nullptr; }

    bool Create(int width, int height, int depth)
    {
        Rect r = { 0, 0, static_cast<short>(height), static_cast<short>(width) };

        QDErr err = NewGWorld(&m_gworld, 32, &r, nullptr, nullptr, 0);
        if ( err != noErr )
        {
            // Try using the temporary memory if there is not enough memory in
            // the application heap.
            err = NewGWorld(&m_gworld, 32, &r, nullptr, nullptr, useTempMem);
        }

        if ( err != noErr )
        {
            m_gworld = nullptr;
            return false;
        }

        m_width = width;
        m_height = height;
        m_depth = depth == 1 ? 1 : depth == 32 ? 32 : 24;

        // Initialize the bitmap to black and fully transparent.
        int rowBytes;
        unsigned char* const pixels = Lock(&rowBytes);
        if ( pixels )
        {
            for ( int y = 0; y < m_height; y++ )
                memset(pixels + y * rowBytes, 0, m_width * 4);
        }
        Unlock();

        return true;
    }

    unsigned char* Lock(int* rowBytes) const
    {
        PixMapHandle pm = GetGWorldPixMap(m_gworld);
        if ( m_lockCount++ == 0 )
            ::LockPixels(pm);

        if ( rowBytes )
            *rowBytes = (**pm).rowBytes & 0x3fff;

        return reinterpret_cast<unsigned char*>(GetPixBaseAddr(pm));
    }

    void Unlock() const
    {
        wxCHECK_RET( m_lockCount > 0, "unbalanced bitmap unlock" );

        if ( --m_lockCount == 0 )
            ::UnlockPixels(GetGWorldPixMap(m_gworld));
    }

    GWorldPtr m_gworld = nullptr;
    int m_width = 0;
    int m_height = 0;
    int m_depth = 0;
    bool m_hasAlpha = false;
    double m_scaleFactor = 1.0;
    wxMask *m_mask = nullptr;

private:
    mutable int m_lockCount = 0;
};

#define M_BITMAP ((wxBitmapRefData *)m_refData)

//-----------------------------------------------------------------------------
// wxBitmap
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxBitmap, wxBitmapBase);

bool wxBitmap::Create(int width, int height, int depth)
{
    UnRef();

    wxCHECK_MSG( width > 0 && height > 0, false, wxT("invalid bitmap size") );

    wxBitmapRefData* const data = new wxBitmapRefData;
    if ( !data->Create(width, height, depth) )
    {
        delete data;
        return false;
    }

    m_refData = data;

    return true;
}

wxBitmap::wxBitmap(const char bits[], int width, int height, int depth)
{
    wxCHECK_RET( depth == 1, wxT("can only create mono bitmap from XBM data") );

    if ( !Create(width, height, 1) )
        return;

    // XBM data: rows padded to byte boundary, least significant bit first,
    // with set bits corresponding to black (foreground) pixels.
    int rowBytes;
    unsigned char* const pixels = LockPixels(&rowBytes);

    const int srcRowBytes = (width + 7) / 8;
    for ( int y = 0; y < height; y++ )
    {
        const unsigned char* const src =
            reinterpret_cast<const unsigned char*>(bits) + y * srcRowBytes;
        unsigned char* dst = pixels + y * rowBytes;

        for ( int x = 0; x < width; x++, dst += 4 )
        {
            const bool set = (src[x / 8] & (1 << (x % 8))) != 0;
            const unsigned char v = set ? 0 : 255;
            dst[0] = 255;
            dst[1] = dst[2] = dst[3] = v;
        }
    }

    UnlockPixels();
}

wxBitmap::wxBitmap(const wxString &filename, wxBitmapType type)
{
    LoadFile(filename, type);
}

#if wxUSE_IMAGE

void wxBitmap::InitFromImage(const wxImage& image, int depth, double scale)
{
    wxCHECK_RET( image.IsOk(), wxT("invalid image") );

    const int width = image.GetWidth();
    const int height = image.GetHeight();

    if ( !Create(width, height, depth == 1 ? 1 : 32) )
        return;

    M_BITMAP->m_scaleFactor = scale;

    const unsigned char* src = image.GetData();
    const unsigned char* alpha = image.GetAlpha();

    int rowBytes;
    unsigned char* const pixels = LockPixels(&rowBytes);

    for ( int y = 0; y < height; y++ )
    {
        unsigned char* dst = pixels + y * rowBytes;
        for ( int x = 0; x < width; x++, dst += 4, src += 3 )
        {
            if ( depth == 1 )
            {
                // Convert to black and white.
                const unsigned lum = (src[0] * 30 + src[1] * 59 + src[2] * 11) / 100;
                const unsigned char v = lum < 128 ? 0 : 255;
                dst[1] = dst[2] = dst[3] = v;
            }
            else
            {
                dst[1] = src[0];
                dst[2] = src[1];
                dst[3] = src[2];
            }

            dst[0] = alpha ? *alpha++ : 255;
        }
    }

    UnlockPixels();

    if ( alpha && depth != 1 )
        M_BITMAP->m_hasAlpha = true;

    if ( image.HasMask() )
    {
        wxColour maskColour(image.GetMaskRed(),
                            image.GetMaskGreen(),
                            image.GetMaskBlue());
        SetMask(new wxMask(*this, maskColour));
    }
}

wxImage wxBitmap::ConvertToImage() const
{
    wxCHECK_MSG( IsOk(), wxNullImage, wxT("invalid bitmap") );

    const int width = GetWidth();
    const int height = GetHeight();

    wxImage img(width, height, false);
    unsigned char* dst = img.GetData();

    unsigned char* alpha = nullptr;
    if ( HasAlpha() )
    {
        img.SetAlpha();
        alpha = img.GetAlpha();
    }

    int rowBytes;
    const unsigned char* const pixels = LockPixels(&rowBytes);

    for ( int y = 0; y < height; y++ )
    {
        const unsigned char* src = pixels + y * rowBytes;
        for ( int x = 0; x < width; x++, src += 4, dst += 3 )
        {
            dst[0] = src[1];
            dst[1] = src[2];
            dst[2] = src[3];

            if ( alpha )
                *alpha++ = src[0];
        }
    }

    UnlockPixels();

    // Also convert the mask, if any.
    if ( GetMask() && GetMask()->GetBitmap().IsOk() )
    {
        const wxBitmap& bmpMask = GetMask()->GetBitmap();

        // Find a colour which is not used in the image to use for the mask.
        unsigned char r, g, b;
        if ( !img.FindFirstUnusedColour(&r, &g, &b) )
        {
            r = 1;
            g = 2;
            b = 3;
        }

        int rowBytesMask;
        const unsigned char* const pixMask = bmpMask.LockPixels(&rowBytesMask);
        unsigned char* data = img.GetData();
        for ( int y = 0; y < height; y++ )
        {
            const unsigned char* m = pixMask + y * rowBytesMask;
            for ( int x = 0; x < width; x++, m += 4, data += 3 )
            {
                // Black pixels of the mask are masked out.
                if ( !m[1] )
                {
                    data[0] = r;
                    data[1] = g;
                    data[2] = b;
                }
            }
        }
        bmpMask.UnlockPixels();

        img.SetMaskColour(r, g, b);
    }

    return img;
}

#endif // wxUSE_IMAGE

int wxBitmap::GetHeight() const
{
    wxCHECK_MSG( IsOk(), -1, wxT("invalid bitmap") );

    return M_BITMAP->m_height;
}

int wxBitmap::GetWidth() const
{
    wxCHECK_MSG( IsOk(), -1, wxT("invalid bitmap") );

    return M_BITMAP->m_width;
}

int wxBitmap::GetDepth() const
{
    wxCHECK_MSG( IsOk(), -1, wxT("invalid bitmap") );

    return M_BITMAP->m_depth;
}

wxMask *wxBitmap::GetMask() const
{
    wxCHECK_MSG( IsOk(), nullptr, wxT("invalid bitmap") );

    return M_BITMAP->m_mask;
}

void wxBitmap::SetMask(wxMask *mask)
{
    wxCHECK_RET( IsOk(), wxT("invalid bitmap") );

    AllocExclusive();
    delete M_BITMAP->m_mask;
    M_BITMAP->m_mask = mask;
}

bool wxBitmap::HasAlpha() const
{
    return IsOk() && M_BITMAP->m_hasAlpha;
}

bool wxBitmap::UseAlpha(bool use)
{
    wxCHECK_MSG( IsOk(), false, wxT("invalid bitmap") );

    if ( use == M_BITMAP->m_hasAlpha )
        return true;

    AllocExclusive();
    M_BITMAP->m_hasAlpha = use;

    if ( use )
    {
        M_BITMAP->m_depth = 32;
    }
    else
    {
        // Reset the alpha byte to opaque, as some code may rely on it.
        int rowBytes;
        unsigned char* const pixels = LockPixels(&rowBytes);
        for ( int y = 0; y < GetHeight(); y++ )
        {
            unsigned char* p = pixels + y * rowBytes;
            for ( int x = 0; x < GetWidth(); x++, p += 4 )
                p[0] = 255;
        }
        UnlockPixels();
    }

    return true;
}

wxBitmap wxBitmap::GetSubBitmap(const wxRect& rect) const
{
    wxCHECK_MSG( IsOk(), wxNullBitmap, wxT("invalid bitmap") );
    wxCHECK_MSG( rect.x >= 0 && rect.y >= 0 &&
                    rect.GetRight() < GetWidth() && rect.GetBottom() < GetHeight(),
                 wxNullBitmap, wxT("invalid bitmap region") );

    wxBitmap bmp;
    if ( !bmp.Create(rect.width, rect.height, GetDepth()) )
        return wxNullBitmap;

    int rowBytesSrc, rowBytesDst;
    const unsigned char* const src = LockPixels(&rowBytesSrc);
    unsigned char* const dst = bmp.LockPixels(&rowBytesDst);

    for ( int y = 0; y < rect.height; y++ )
    {
        memcpy(dst + y * rowBytesDst,
               src + (rect.y + y) * rowBytesSrc + rect.x * 4,
               rect.width * 4);
    }

    bmp.UnlockPixels();
    UnlockPixels();

    if ( HasAlpha() )
        bmp.UseAlpha();

    if ( GetMask() )
    {
        const wxBitmap bmpMask = GetMask()->GetBitmap().GetSubBitmap(rect);
        bmp.SetMask(new wxMask(bmpMask));
    }

    return bmp;
}

bool wxBitmap::SaveFile(const wxString& name, wxBitmapType type,
                        const wxPalette *WXUNUSED(palette)) const
{
    wxCHECK_MSG( IsOk(), false, wxT("invalid bitmap") );

#if wxUSE_IMAGE
    wxImage image = ConvertToImage();
    return image.SaveFile(name, type);
#else
    wxUnusedVar(name);
    wxUnusedVar(type);
    return false;
#endif
}

bool wxBitmap::LoadFile(const wxString &name, wxBitmapType type)
{
    UnRef();

#if wxUSE_IMAGE
    wxImage image;
    if ( !image.LoadFile(name, type) || !image.IsOk() )
        return false;

    *this = wxBitmap(image);
    return IsOk();
#else
    wxUnusedVar(name);
    wxUnusedVar(type);
    return false;
#endif
}

#if wxUSE_PALETTE

wxPalette *wxBitmap::GetPalette() const
{
    return nullptr;
}

void wxBitmap::SetPalette(const wxPalette& WXUNUSED(palette))
{
}

#endif // wxUSE_PALETTE

#if WXWIN_COMPATIBILITY_3_0

void wxBitmap::SetHeight(int height)
{
    AllocExclusive();
    M_BITMAP->m_height = height;
}

void wxBitmap::SetWidth(int width)
{
    AllocExclusive();
    M_BITMAP->m_width = width;
}

void wxBitmap::SetDepth(int depth)
{
    AllocExclusive();
    M_BITMAP->m_depth = depth;
}

#endif // WXWIN_COMPATIBILITY_3_0

GWorldPtr wxBitmap::GetGWorld() const
{
    wxCHECK_MSG( IsOk(), nullptr, wxT("invalid bitmap") );

    return M_BITMAP->m_gworld;
}

unsigned char* wxBitmap::LockPixels(int* rowBytes) const
{
    wxCHECK_MSG( IsOk(), nullptr, wxT("invalid bitmap") );

    return M_BITMAP->Lock(rowBytes);
}

void wxBitmap::UnlockPixels() const
{
    wxCHECK_RET( IsOk(), wxT("invalid bitmap") );

    M_BITMAP->Unlock();
}

void *wxBitmap::GetRawData(wxPixelDataBase& data, int bpp)
{
    wxCHECK_MSG( IsOk(), nullptr, wxT("invalid bitmap") );

    // We always use 32bpp internally.
    if ( bpp != 32 )
        return nullptr;

    AllocExclusive();

    int rowBytes;
    unsigned char* const pixels = LockPixels(&rowBytes);

    data.m_width = GetWidth();
    data.m_height = GetHeight();
    data.m_stride = rowBytes;

    return pixels;
}

void wxBitmap::UngetRawData(wxPixelDataBase& WXUNUSED(data))
{
    UnlockPixels();
}

wxGDIRefData *wxBitmap::CreateGDIRefData() const
{
    return new wxBitmapRefData;
}

wxGDIRefData *wxBitmap::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxBitmapRefData(*static_cast<const wxBitmapRefData *>(data));
}

/* static */
void wxBitmap::InitStandardHandlers()
{
    // not wxBitmap handlers, we rely on wxImage
}
