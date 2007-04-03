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

#include "wx/dfb/private.h"

//-----------------------------------------------------------------------------
// helpers
//-----------------------------------------------------------------------------

// pitch = stride = # of bytes between the start of N-th line and (N+1)-th line
static void CopyPixelsAndSwapRGB(unsigned w, unsigned h,
                                 const unsigned char *src,
                                 unsigned src_pitch,
                                 unsigned char *dst,
                                 unsigned dst_pitch)
{
    unsigned src_advance = src_pitch - 3 * w;
    unsigned dst_advance = dst_pitch - 3 * w;
    for ( unsigned y = 0; y < h; y++, src += src_advance, dst += dst_advance )
    {
        for ( unsigned x = 0; x < w; x++, src += 3, dst += 3 )
        {
            // copy with RGB -> BGR translation:
            dst[0] = src[2];
            dst[1] = src[1];
            dst[2] = src[0];
        }
    }
}

static void CopySurfaceToImage(const wxIDirectFBSurfacePtr& surface,
                               const wxImage& image)
{
    wxCHECK_RET( surface->GetPixelFormat() == DSPF_RGB24,
                 _T("unexpected pixel format") );

    wxIDirectFBSurface::Locked locked(surface, DSLF_READ);
    wxCHECK_RET( locked.ptr, _T("failed to lock surface") );

    CopyPixelsAndSwapRGB(image.GetWidth(), image.GetHeight(),
                         (unsigned char*)locked.ptr, locked.pitch,
                         image.GetData(), image.GetWidth() * 3);
}

static void CopyImageToSurface(const wxImage& image,
                               const wxIDirectFBSurfacePtr& surface)
{
    wxCHECK_RET( surface->GetPixelFormat() == DSPF_RGB24,
                 _T("unexpected pixel format") );

    wxIDirectFBSurface::Locked locked(surface, DSLF_WRITE);
    wxCHECK_RET( locked.ptr, _T("failed to lock surface") );

    CopyPixelsAndSwapRGB(image.GetWidth(), image.GetHeight(),
                         image.GetData(), image.GetWidth() * 3,
                         (unsigned char*)locked.ptr, locked.pitch);
}

// Creates a surface that will use wxImage's pixel data (RGB only)
static wxIDirectFBSurfacePtr CreateSurfaceForImage(const wxImage& image)
{
    wxCHECK_MSG( image.Ok(), NULL, _T("invalid image") );
    // FIXME_DFB: implement alpha handling by merging alpha buffer with RGB
    //            into a temporary RGBA surface
    wxCHECK_MSG( !image.HasAlpha(), NULL, _T("alpha channel not supported") );

    // NB: wxImage uses RGB order of bytes while DirectFB uses BGR, so we
    //     cannot use preallocated surface that shares data with wxImage, we
    //     have to copy the data to temporary surface instead
    DFBSurfaceDescription desc;
    desc.flags = (DFBSurfaceDescriptionFlags)
        (DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT);
    desc.caps = DSCAPS_NONE;
    desc.width = image.GetWidth();
    desc.height = image.GetHeight();
    desc.pixelformat = DSPF_RGB24;

    return wxIDirectFB::Get()->CreateSurface(&desc);
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

    wxCHECK_MSG( surface, false, _T("invalid surface") );

    m_refData = new wxBitmapRefData();
    M_BITMAP->m_surface = surface;
    return true;
}

bool wxBitmap::Create(int width, int height, int depth)
{
    UnRef();

    wxCHECK_MSG( width > 0 && height > 0, false, wxT("invalid bitmap size") );
    wxCHECK_MSG( depth == -1, false, wxT("only default depth supported now") );

    DFBSurfaceDescription desc;
    desc.flags = (DFBSurfaceDescriptionFlags)(
                        DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT);
    desc.caps = DSCAPS_NONE;
    desc.width = width;
    desc.height = height;

    return Create(wxIDirectFB::Get()->CreateSurface(&desc));
}

#if wxUSE_IMAGE
wxBitmap::wxBitmap(const wxImage& image, int depth)
{
    wxCHECK_RET( image.Ok(), wxT("invalid image") );

    // create surface in screen's format:
    if ( !Create(image.GetWidth(), image.GetHeight(), depth) )
        return;

    // then copy the image to it:
    wxIDirectFBSurfacePtr dst = M_BITMAP->m_surface;

    if ( dst->GetPixelFormat() == DSPF_RGB24 )
    {
        CopyImageToSurface(image, dst);
    }
    else
    {
        // wxBitmap uses different pixel format, so we have to use a temporary
        // surface and blit to the bitmap via it:
        wxIDirectFBSurfacePtr src(CreateSurfaceForImage(image));
        CopyImageToSurface(image, src);

        if ( !dst->SetBlittingFlags(DSBLIT_NOFX) )
            return;
        if ( !dst->Blit(src->GetRaw(), NULL, 0, 0) )
            return;
    }

    // FIXME: implement mask creation from image's mask (or alpha channel?)
    wxASSERT_MSG( !image.HasMask(), _T("image masks are ignored for now") );
}

wxImage wxBitmap::ConvertToImage() const
{
    wxCHECK_MSG( Ok(), wxNullImage, wxT("invalid bitmap") );

    wxImage img(GetWidth(), GetHeight());
    wxIDirectFBSurfacePtr src = M_BITMAP->m_surface;

    if ( src->GetPixelFormat() == DSPF_RGB24 )
    {
        CopySurfaceToImage(src, img);
    }
    else
    {
        // wxBitmap uses different pixel format, so we have to use a temporary
        // surface and blit to the bitmap via it:
        wxIDirectFBSurfacePtr dst(CreateSurfaceForImage(img));

        if ( !dst->SetBlittingFlags(DSBLIT_NOFX) )
            return wxNullImage;
        if ( !dst->Blit(src->GetRaw(), NULL, 0, 0) )
            return wxNullImage;

        CopySurfaceToImage(dst, img);
    }

    // FIXME: implement mask setting in the image
    wxASSERT_MSG( GetMask() == NULL, _T("bitmap masks are ignored for now") );

    return img;
}
#endif // wxUSE_IMAGE

wxBitmap::wxBitmap(const wxString &filename, wxBitmapType type)
{
    LoadFile(filename, type);
}

wxBitmap::wxBitmap(const char bits[], int width, int height, int depth)
{
    wxCHECK_RET( depth == 1, wxT("can only create mono bitmap from XBM data") );

    wxFAIL_MSG( _T("not implemented") );
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

    wxFAIL_MSG( _T("SetHeight not implemented") );
}

void wxBitmap::SetWidth(int width)
{
    AllocExclusive();

    wxFAIL_MSG( _T("SetWidth not implemented") );
}

void wxBitmap::SetDepth(int depth)
{
    AllocExclusive();

    wxFAIL_MSG( _T("SetDepth not implemented") );
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
