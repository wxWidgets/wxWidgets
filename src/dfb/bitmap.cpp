/////////////////////////////////////////////////////////////////////////////
// Name:        src/dfb/bitmap.cpp
// Purpose:     wxBitmap implementation
// Author:      Vaclav Slavik
// Created:     2006-08-04
// RCS-ID:      $Id$
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"
#endif

#include "wx/bitmap.h"
#include "wx/colour.h"
#include "wx/image.h"
#include "wx/rawbmp.h"

#include "wx/dfb/private.h"

//-----------------------------------------------------------------------------
// helpers
//-----------------------------------------------------------------------------

// NB: Most of this conversion code is needed because of differences between
//     wxImage and wxDFB's wxBitmap representations:
//     (1) wxImage uses RGB order, while DirectFB uses BGR
//     (2) wxImage has alpha channel in a separate plane, while DirectFB puts
//         all components into single BGRA plane

// pitch = stride = # of bytes between the start of N-th line and (N+1)-th line
// {Src,Dst}PixSize = # of bytes used to represent one pixel
template<int SrcPixSize, int DstPixSize>
static void CopyPixelsAndSwapRGB(unsigned w, unsigned h,
                                 const unsigned char *src,
                                 unsigned src_pitch,
                                 unsigned char *dst,
                                 unsigned dst_pitch)
{
    unsigned src_advance = src_pitch - SrcPixSize * w;
    unsigned dst_advance = dst_pitch - DstPixSize * w;
    for ( unsigned y = 0; y < h; y++, src += src_advance, dst += dst_advance )
    {
        for ( unsigned x = 0; x < w; x++, src += SrcPixSize, dst += DstPixSize )
        {
            // copy with RGB -> BGR translation:
            dst[0] = src[2];
            dst[1] = src[1];
            dst[2] = src[0];
        }
    }
}

static void CopySurfaceToImage(const wxIDirectFBSurfacePtr& surface,
                               wxImage& image)
{
    wxIDirectFBSurface::Locked locked(surface, DSLF_READ);
    wxCHECK_RET( locked.ptr, "failed to lock surface" );

    const unsigned width = image.GetWidth();
    const unsigned height = image.GetHeight();
    const DFBSurfacePixelFormat format = surface->GetPixelFormat();

    // copy RGB data from the surface:
    switch ( format )
    {
        case DSPF_RGB24:
            CopyPixelsAndSwapRGB<3,3>
            (
                width, height,
                (unsigned char*)locked.ptr, locked.pitch,
                image.GetData(), width * 3
            );
            break;

        case DSPF_RGB32:
        case DSPF_ARGB:
            CopyPixelsAndSwapRGB<4,3>
            (
                width, height,
                (unsigned char*)locked.ptr, locked.pitch,
                image.GetData(), width * 3
            );
            break;

        default:
            wxFAIL_MSG( "unexpected pixel format" );
            return;
    }

    // extract alpha channel if the bitmap has it:
    if ( format == DSPF_ARGB )
    {
        // create alpha plane:
        image.SetAlpha();

        // and copy alpha data to it:
        const unsigned advance = locked.pitch - 4 * width;
        unsigned char *alpha = image.GetAlpha();
        // NB: "+3" is to get pointer to alpha component
        const unsigned char *src = ((unsigned char*)locked.ptr) + 3;

        for ( unsigned y = 0; y < height; y++, src += advance )
            for ( unsigned x = 0; x < width; x++, src += 4 )
                *(alpha++) = *src;
    }
}

static void CopyImageToSurface(const wxImage& image,
                               const wxIDirectFBSurfacePtr& surface)
{
    wxIDirectFBSurface::Locked locked(surface, DSLF_WRITE);
    wxCHECK_RET( locked.ptr, "failed to lock surface" );

    const unsigned width = image.GetWidth();
    const unsigned height = image.GetHeight();
    const DFBSurfacePixelFormat format = surface->GetPixelFormat();

    // copy RGB data to the surface:
    switch ( format )
    {
        case DSPF_RGB24:
            CopyPixelsAndSwapRGB<3,3>
            (
               width, height,
               image.GetData(), width * 3,
               (unsigned char*)locked.ptr, locked.pitch
            );
            break;

        case DSPF_RGB32:
        case DSPF_ARGB:
            CopyPixelsAndSwapRGB<3,4>
            (
               width, height,
               image.GetData(), width * 3,
               (unsigned char*)locked.ptr, locked.pitch
            );
            break;

        default:
            wxFAIL_MSG( "unexpected pixel format" );
            return;
    }

    // if the image has alpha channel, merge it in:
    if ( format == DSPF_ARGB )
    {
        wxCHECK_RET( image.HasAlpha(), "logic error - ARGB, but no alpha" );

        const unsigned advance = locked.pitch - 4 * width;
        const unsigned char *alpha = image.GetAlpha();
        // NB: "+3" is to get pointer to alpha component
        unsigned char *dest = ((unsigned char*)locked.ptr) + 3;

        for ( unsigned y = 0; y < height; y++, dest += advance )
            for ( unsigned x = 0; x < width; x++, dest += 4 )
                *dest = *(alpha++);
    }
}

static wxIDirectFBSurfacePtr
CreateSurfaceWithFormat(int w, int h, DFBSurfacePixelFormat format)
{
    DFBSurfaceDescription desc;
    desc.flags = (DFBSurfaceDescriptionFlags)
        (DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT);
    desc.caps = DSCAPS_NONE;
    desc.width = w;
    desc.height = h;

    if ( format != DSPF_UNKNOWN )
    {
        desc.flags = (DFBSurfaceDescriptionFlags)(
                            desc.flags | DSDESC_PIXELFORMAT);
        desc.pixelformat = format;
    }

    return wxIDirectFB::Get()->CreateSurface(&desc);
}

// Creates a surface that will use wxImage's pixel data (RGB only)
static wxIDirectFBSurfacePtr CreateSurfaceForImage(const wxImage& image)
{
    wxCHECK_MSG( image.Ok(), NULL, "invalid image" );
    // FIXME_DFB: implement alpha handling by merging alpha buffer with RGB
    //            into a temporary RGBA surface
    wxCHECK_MSG( !image.HasAlpha(), NULL, "alpha channel not supported" );

    // NB: wxImage uses RGB order of bytes while DirectFB uses BGR, so we
    //     cannot use preallocated surface that shares data with wxImage, we
    //     have to copy the data to temporary surface instead
    return CreateSurfaceWithFormat(image.GetWidth(), image.GetHeight(),
                                   DSPF_RGB24);
}

static bool ConvertSurfaceToFormat(wxIDirectFBSurfacePtr& surface,
                                   DFBSurfacePixelFormat format)
{
    if ( surface->GetPixelFormat() == format )
        return true;

    int w, h;
    surface->GetSize(&w, &h);
    wxIDirectFBSurfacePtr s = CreateSurfaceWithFormat(w, h, format);
    if ( !s )
        return false;

    if ( !s->SetBlittingFlags(DSBLIT_NOFX) )
        return false;
    if ( !s->Blit(surface->GetRaw(), NULL, 0, 0) )
        return false;

    surface = s;
    return true;
}

static DFBSurfacePixelFormat DepthToFormat(int depth)
{
    switch ( depth )
    {
        case 24:
            return DSPF_RGB24;
        case 32:
            // NB: we treat depth=32 as requesting ARGB for consistency with
            //     other ports
            return DSPF_ARGB;
        default:
            wxFAIL_MSG( "unsupported depth requested" );
            // fall through
        case -1:
            return DSPF_UNKNOWN;
    }
}

//-----------------------------------------------------------------------------
// wxBitmapRefData
//-----------------------------------------------------------------------------

class wxBitmapRefData: public wxObjectRefData
{
public:
    wxBitmapRefData()
    {
        m_mask = NULL;
#if wxUSE_PALETTE
        m_palette = NULL;
#endif
    }

    wxBitmapRefData(const wxBitmapRefData& data)
    {
        m_surface = data.m_surface ? data.m_surface->Clone() : NULL;

        m_mask = data.m_mask ? new wxMask(*data.m_mask) : NULL;
#if wxUSE_PALETTE
        m_palette = data.m_palette ? new wxPalette(*data.m_palette) : NULL;
#endif
    }

    virtual ~wxBitmapRefData()
    {
        delete m_mask;
#if wxUSE_PALETTE
        delete m_palette;
#endif
    }

    wxIDirectFBSurfacePtr m_surface;
    wxMask               *m_mask;
#if wxUSE_PALETTE
    wxPalette            *m_palette;
#endif
};

#define M_BITMAP ((wxBitmapRefData *)m_refData)

//-----------------------------------------------------------------------------
// wxBitmap
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxBitmapHandler, wxBitmapHandlerBase)
IMPLEMENT_DYNAMIC_CLASS(wxBitmap, wxBitmapBase)

wxBitmap::wxBitmap(int width, int height, int depth)
{
    Create(width, height, depth);
}

bool wxBitmap::Create(const wxIDirectFBSurfacePtr& surface)
{
    UnRef();

    wxCHECK_MSG( surface, false, "invalid surface" );

    m_refData = new wxBitmapRefData();
    M_BITMAP->m_surface = surface;
    return true;
}

bool wxBitmap::Create(int width, int height, int depth)
{
    return CreateWithFormat(width, height, DepthToFormat(depth));
}

bool wxBitmap::CreateWithFormat(int width, int height, int dfbFormat)
{
    UnRef();

    wxCHECK_MSG( width > 0 && height > 0, false, wxT("invalid bitmap size") );

    return Create(CreateSurfaceWithFormat(width, height,
                                          DFBSurfacePixelFormat(dfbFormat)));
}

#if wxUSE_IMAGE
wxBitmap::wxBitmap(const wxImage& image, int depth)
{
    wxCHECK_RET( image.Ok(), wxT("invalid image") );

    DFBSurfacePixelFormat format = DepthToFormat(depth);
    if ( format == DSPF_UNKNOWN && image.HasAlpha() )
        format = DSPF_ARGB;

    // create surface in screen's format (unless we need alpha channel,
    // in which case use ARGB):
    if ( !CreateWithFormat(image.GetWidth(), image.GetHeight(), format) )
        return;

    // then copy the image to it:
    wxIDirectFBSurfacePtr dst = M_BITMAP->m_surface;

    switch ( dst->GetPixelFormat() )
    {
        case DSPF_RGB24:
        case DSPF_RGB32:
        case DSPF_ARGB:
            CopyImageToSurface(image, dst);
            break;

        default:
        {
            // wxBitmap uses different pixel format, so we have to use a
            // temporary surface and blit to the bitmap via it:
            wxIDirectFBSurfacePtr src(CreateSurfaceForImage(image));
            CopyImageToSurface(image, src);

            if ( !dst->SetBlittingFlags(DSBLIT_NOFX) )
                return;
            if ( !dst->Blit(src->GetRaw(), NULL, 0, 0) )
                return;
        }
    }

    // FIXME: implement mask creation from image's mask (or alpha channel?)
    wxASSERT_MSG( !image.HasMask(), "image masks are ignored for now" );
}

wxImage wxBitmap::ConvertToImage() const
{
    wxCHECK_MSG( Ok(), wxNullImage, wxT("invalid bitmap") );

    wxImage img(GetWidth(), GetHeight());
    wxIDirectFBSurfacePtr src = M_BITMAP->m_surface;

    switch ( src->GetPixelFormat() )
    {
        case DSPF_RGB24:
        case DSPF_RGB32:
        case DSPF_ARGB:
            CopySurfaceToImage(src, img);
            break;
        default:
        {
            // wxBitmap uses different pixel format, so we have to use a
            // temporary surface and blit to the bitmap via it:
            wxIDirectFBSurfacePtr dst(CreateSurfaceForImage(img));

            if ( !dst->SetBlittingFlags(DSBLIT_NOFX) )
                return wxNullImage;
            if ( !dst->Blit(src->GetRaw(), NULL, 0, 0) )
                return wxNullImage;

            CopySurfaceToImage(dst, img);
        }
    }

    // FIXME: implement mask setting in the image
    wxASSERT_MSG( GetMask() == NULL, "bitmap masks are ignored for now" );

    return img;
}
#endif // wxUSE_IMAGE

void *wxBitmap::GetRawData(wxPixelDataBase& data, int bpp)
{
    wxCHECK_MSG( Ok(), NULL, "invalid bitmap" );

    AllocExclusive();

    DFBSurfacePixelFormat format;
    if ( bpp == 32 )
        format = DSPF_ARGB;
    else
        format = DSPF_RGB24;

    // convert the bitmap into format compatible with requested raw access;
    // note that we don't bother converting the bitmap back in UngetRawData(),
    // as unpacked formats (RGB24, RGB32) are the common case and converting
    // between them while blitting is fast enough (FIXME?)
    if ( !ConvertSurfaceToFormat(M_BITMAP->m_surface, format) )
        return NULL;

    void *bits = NULL;
    if ( !M_BITMAP->m_surface->Lock
                               (
                                 (DFBSurfaceLockFlags)(DSLF_READ | DSLF_WRITE),
                                 &bits,
                                 &data.m_stride
                               ) )
        return NULL;

    M_BITMAP->m_surface->GetSize(&data.m_width, &data.m_height);

    return bits;
}

void wxBitmap::UngetRawData(wxPixelDataBase& WXUNUSED(data))
{
    M_BITMAP->m_surface->Unlock();
}

bool wxBitmap::HasAlpha() const
{
    wxCHECK_MSG( Ok(), false, "invalid bitmap" );

    return M_BITMAP->m_surface->GetPixelFormat() == DSPF_ARGB;
}

wxBitmap::wxBitmap(const wxString &filename, wxBitmapType type)
{
    LoadFile(filename, type);
}

wxBitmap::wxBitmap(const char bits[], int width, int height, int depth)
{
    wxCHECK_RET( depth == 1, wxT("can only create mono bitmap from XBM data") );

    wxFAIL_MSG( "not implemented" );
}

bool wxBitmap::IsOk() const
{
    return (m_refData != NULL && M_BITMAP->m_surface);
}

int wxBitmap::GetHeight() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid bitmap") );

    int h = -1;
    M_BITMAP->m_surface->GetSize(NULL, &h);
    return h;
}

int wxBitmap::GetWidth() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid bitmap") );

    int w = -1;
    M_BITMAP->m_surface->GetSize(&w, NULL);
    return w;
}

int wxBitmap::GetDepth() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid bitmap") );

    return M_BITMAP->m_surface->GetDepth();
}

wxMask *wxBitmap::GetMask() const
{
    wxCHECK_MSG( Ok(), NULL, wxT("invalid bitmap") );

    return M_BITMAP->m_mask;
}

void wxBitmap::SetMask(wxMask *mask)
{
    wxCHECK_RET( Ok(), wxT("invalid bitmap") );

    AllocExclusive();
    delete M_BITMAP->m_mask;
    M_BITMAP->m_mask = mask;
}

bool wxBitmap::CopyFromIcon(const wxIcon& icon)
{
    *this = *((wxBitmap*)(&icon));
    return true;
}

wxBitmap wxBitmap::GetSubBitmap(const wxRect& rect) const
{
    wxCHECK_MSG( Ok() &&
                 rect.x >= 0 && rect.y >= 0 &&
                 rect.x+rect.width <= GetWidth() &&
                 rect.y+rect.height <= GetHeight(),
                 wxNullBitmap,
                 wxT("invalid bitmap or bitmap region") );

    // NB: DirectFB subsurfaces share the same pixels buffer, so we must
    //     clone the obtained subsurface
    DFBRectangle r = { rect.x, rect.y, rect.width, rect.height };
    return wxBitmap(M_BITMAP->m_surface->GetSubSurface(&r)->Clone());
}

#warning "to common code"
bool wxBitmap::LoadFile(const wxString &name, wxBitmapType type)
{
    UnRef();

    wxBitmapHandler *handler = FindHandler(type);

    if ( handler == NULL )
    {
        wxImage image;
        if ( !image.LoadFile(name, type) || !image.Ok() )
        {
            wxLogError(_("No bitmap handler for type %d defined."), type);
            return false;
        }
        else
        {
            *this = wxBitmap(image);
            return true;
        }
    }

    m_refData = new wxBitmapRefData();

    return handler->LoadFile(this, name, type, -1, -1);
}

#warning "to common code"
bool wxBitmap::SaveFile(const wxString& filename, wxBitmapType type, const wxPalette *palette) const
{
    wxCHECK_MSG( Ok(), false, wxT("invalid bitmap") );

    wxBitmapHandler *handler = FindHandler(type);

    if ( handler == NULL )
    {
        wxImage image = ConvertToImage();
#if wxUSE_PALETTE
        if ( palette )
            image.SetPalette(*palette);
#endif // wxUSE_PALETTE

        if ( image.Ok() )
            return image.SaveFile(filename, type);
        else
        {
            wxLogError(_("No bitmap handler for type %d defined."), type);
            return false;
        }
    }

    return handler->SaveFile(this, filename, type, palette);
}

#if wxUSE_PALETTE
wxPalette *wxBitmap::GetPalette() const
{
    wxCHECK_MSG( Ok(), NULL, wxT("invalid bitmap") );

    return M_BITMAP->m_palette;
}

void wxBitmap::SetPalette(const wxPalette& palette)
{
    wxCHECK_RET( Ok(), wxT("invalid bitmap") );
    wxCHECK_RET( GetDepth() > 1 && GetDepth() <= 8, wxT("cannot set palette for bitmap of this depth") );

    AllocExclusive();
    delete M_BITMAP->m_palette;
    M_BITMAP->m_palette = NULL;

    if ( !palette.Ok() ) return;

    M_BITMAP->m_palette = new wxPalette(palette);
}
#endif // wxUSE_PALETTE

void wxBitmap::SetHeight(int height)
{
    AllocExclusive();

    wxFAIL_MSG( "SetHeight not implemented" );
}

void wxBitmap::SetWidth(int width)
{
    AllocExclusive();

    wxFAIL_MSG( "SetWidth not implemented" );
}

void wxBitmap::SetDepth(int depth)
{
    DFBSurfacePixelFormat format = DepthToFormat(depth);
    if ( M_BITMAP->m_surface->GetPixelFormat() == format )
        return;

    AllocExclusive();

    int w, h;
    M_BITMAP->m_surface->GetSize(&w, &h);
    wxIDirectFBSurfacePtr s = CreateSurfaceWithFormat(w, h, format);
    if ( !s )
        return;
    if ( !s->SetBlittingFlags(DSBLIT_NOFX) )
        return;
    if ( !s->Blit(M_BITMAP->m_surface->GetRaw(), NULL, 0, 0) )
        return;

    M_BITMAP->m_surface = s;
}

wxIDirectFBSurfacePtr wxBitmap::GetDirectFBSurface() const
{
    wxCHECK_MSG( Ok(), NULL, wxT("invalid bitmap") );

    return M_BITMAP->m_surface;
}

wxObjectRefData *wxBitmap::CreateRefData() const
{
    return new wxBitmapRefData;
}

wxObjectRefData *wxBitmap::CloneRefData(const wxObjectRefData *data) const
{
    return new wxBitmapRefData(*(wxBitmapRefData *)data);
}


/*static*/
void wxBitmap::InitStandardHandlers()
{
    // not wxBitmap handlers, we rely on wxImage
}
