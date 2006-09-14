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

#warning "move this to common"
#include "wx/xpmdecod.h"

#include "wx/dfb/private.h"

//-----------------------------------------------------------------------------
// helpers
//-----------------------------------------------------------------------------

// Convert wxColour into it's quantized value in lower-precision
// pixel format (needed for masking by colour).
static wxColour wxQuantizeColour(const wxColour& clr, const wxBitmap& bmp)
{
#if 0
    pixel_format_t *pf = bmp.GetMGLbitmap_t()->pf;

    if ( pf->redAdjust == 0 && pf->greenAdjust == 0 && pf->blueAdjust == 0 )
        return clr;
    else
        return wxColour((unsigned char)((clr.Red() >> pf->redAdjust) << pf->redAdjust),
                        (unsigned char)((clr.Green() >> pf->greenAdjust) << pf->greenAdjust),
                        (unsigned char)((clr.Blue() >> pf->blueAdjust) << pf->blueAdjust));
#endif
}


//-----------------------------------------------------------------------------
// wxMask
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMask, wxObject)

wxMask::wxMask() : m_bitmap(NULL)
{
}

wxMask::wxMask(const wxBitmap& bitmap)
    : m_bitmap(NULL)
{
    Create(bitmap);
}

wxMask::wxMask(const wxBitmap& bitmap, const wxColour& colour)
    : m_bitmap(NULL)
{
    Create(bitmap, colour);
}

#if wxUSE_PALETTE
wxMask::wxMask(const wxBitmap& bitmap, int paletteIndex)
    : m_bitmap(NULL)
{
    Create(bitmap, paletteIndex);
}
#endif // wxUSE_PALETTE

wxMask::wxMask(const wxMask& mask)
{
    m_bitmap = mask.m_bitmap ? new wxBitmap(*mask.m_bitmap) : NULL;
}

wxMask::~wxMask()
{
    delete m_bitmap;
}

#warning "move this to common code"
bool wxMask::Create(const wxBitmap& bitmap, const wxColour& colour)
{
    delete m_bitmap;
    m_bitmap = NULL;

    wxColour clr(wxQuantizeColour(colour, bitmap));

    wxImage imgSrc(bitmap.ConvertToImage());
    imgSrc.SetMask(false);
    wxImage image(imgSrc.ConvertToMono(clr.Red(), clr.Green(), clr.Blue()));
    if ( !image.Ok() )
        return false;

    m_bitmap = new wxBitmap(image, 1);

    return m_bitmap->Ok();
}

#if wxUSE_PALETTE
bool wxMask::Create(const wxBitmap& bitmap, int paletteIndex)
{
    unsigned char r,g,b;
    wxPalette *pal = bitmap.GetPalette();

    wxCHECK_MSG( pal, false, wxT("Cannot create mask from bitmap without palette") );

    pal->GetRGB(paletteIndex, &r, &g, &b);

    return Create(bitmap, wxColour(r, g, b));
}
#endif // wxUSE_PALETTE

bool wxMask::Create(const wxBitmap& bitmap)
{
    delete m_bitmap;
    m_bitmap = NULL;

    wxCHECK_MSG( bitmap.Ok(), false, wxT("Invalid bitmap") );
    wxCHECK_MSG( bitmap.GetDepth() == 1, false, wxT("Cannot create mask from colour bitmap") );

    m_bitmap = new wxBitmap(bitmap);
    return true;
}

const wxBitmap& wxMask::GetBitmap() const
{
    return m_bitmap ? *m_bitmap : wxNullBitmap;
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
        if ( data.m_surface )
            m_surface = data.m_surface->Clone();

        m_mask = data.m_mask ? new wxMask(*data.m_mask) : NULL;
#if wxUSE_PALETTE
        m_palette = data.m_palette ? new wxPalette(*data.m_palette) : NULL;
#endif
    }

    ~wxBitmapRefData()
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

IMPLEMENT_ABSTRACT_CLASS(wxBitmapHandler, wxObject)
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

    DFBSurfaceDescription desc;
    desc.flags = (DFBSurfaceDescriptionFlags)(
                        DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT);
    desc.caps = DSCAPS_NONE;
    desc.width = width;
    desc.height = height;

    return Create(wxIDirectFB::Get()->CreateSurface(&desc));
}

#warning "FIXME: move this to common code"
bool wxBitmap::CreateFromXpm(const char **bits)
{
    wxCHECK_MSG( bits != NULL, false, wxT("invalid bitmap data") );

#if wxUSE_IMAGE && wxUSE_XPM
    wxXPMDecoder decoder;
    wxImage img = decoder.ReadData(bits);
    wxCHECK_MSG( img.Ok(), false, wxT("invalid bitmap data") );

    *this = wxBitmap(img);

    return true;
#else
    wxFAIL_MSG( _T("creating bitmaps from XPMs not supported") );
    return false;
#endif // wxUSE_IMAGE && wxUSE_XPM
}

#if wxUSE_IMAGE
wxBitmap::wxBitmap(const wxImage& image, int depth)
{
    wxCHECK_RET( image.Ok(), wxT("invalid image") );
}

wxImage wxBitmap::ConvertToImage() const
{
    wxCHECK_MSG( Ok(), wxNullImage, wxT("invalid bitmap") );

    return wxNullImage; // FIXME
}
#endif // wxUSE_IMAGE

wxBitmap::wxBitmap(const wxString &filename, wxBitmapType type)
{
    LoadFile(filename, type);
}

wxBitmap::wxBitmap(const char bits[], int width, int height, int depth)
{
    wxCHECK_RET( depth == 1, wxT("can only create mono bitmap from XBM data") );
}

bool wxBitmap::Ok() const
{
    return (m_refData != NULL && M_BITMAP->m_surface);
}

bool wxBitmap::operator==(const wxBitmap& bmp) const
{
    // FIXME: is this the right way to compare bitmaps?
    return (m_refData == bmp.m_refData);
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
            wxLogError("no bitmap handler for type %d defined.", type);
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
            wxLogError("no bitmap handler for type %d defined.", type);
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

    delete M_BITMAP->m_palette;
    M_BITMAP->m_palette = NULL;

    if ( !palette.Ok() ) return;

    M_BITMAP->m_palette = new wxPalette(palette);
}
#endif // wxUSE_PALETTE

void wxBitmap::SetHeight(int height)
{
    AllocExclusive();
#warning "todo"
}

void wxBitmap::SetWidth(int width)
{
    AllocExclusive();
#warning "todo"
}

void wxBitmap::SetDepth(int depth)
{
    AllocExclusive();
#warning "todo"
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
