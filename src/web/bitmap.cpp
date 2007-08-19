/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/bitmap.cpp
// Purpose:
// Author:      Robert Roebling
// RCS-ID:      $Id: bitmap.cpp 46059 2007-05-16 07:14:53Z PC $
// Copyright:   (c) 1998 Robert Roebling, John Wilmes
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

#include "wx/web/private/utils.h"

#include <Magick++.h>

//-----------------------------------------------------------------------------
// wxMask
//-----------------------------------------------------------------------------

//TODO wxMask is not yet implemented
IMPLEMENT_DYNAMIC_CLASS(wxMask,wxObject)

wxMask::wxMask()
{
}

wxMask::wxMask( const wxBitmap& bitmap, const wxColour& colour )
{
}

wxMask::wxMask( const wxBitmap& bitmap )
{
}

wxMask::~wxMask()
{
}

bool wxMask::Create( const wxBitmap& bitmap,
                     const wxColour& colour )
{
}

bool wxMask::Create( const wxBitmap& bitmap )
{
}

//-----------------------------------------------------------------------------
// wxBitmap
//-----------------------------------------------------------------------------

class wxBitmapRefData: public wxObjectRefData
{
public:
    wxBitmapRefData();
    virtual ~wxBitmapRefData();

    Magick::Image  m_image;
    wxMask         *m_mask;
    int             m_width;
    int             m_height;
    int             m_bpp;
};

wxBitmapRefData::wxBitmapRefData()
{
    m_mask = (wxMask *) NULL;
    m_width = 0;
    m_height = 0;
    m_bpp = 0;
}

wxBitmapRefData::~wxBitmapRefData()
{
    if (m_mask) {
        delete m_mask;
    }
}

//-----------------------------------------------------------------------------

#define M_BMPDATA wx_static_cast(wxBitmapRefData*, m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxBitmap,wxGDIObject)

wxBitmap::wxBitmap() {
}

wxBitmap::wxBitmap(int width, int height, int depth)
{
    Create(width, height, depth);
}

wxBitmap::wxBitmap(const wxString &filename, wxBitmapType type)
{
    LoadFile(filename, type);
}

wxBitmap::wxBitmap(const wxImage& image, int depth) {
}

wxBitmap::wxBitmap(const char bits[], int width, int height, int depth)
{
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
    
    SetMagickImage(Magick::Image(Magick::Geometry(width, height), "white"));

    return Ok();
}

wxImage wxBitmap::ConvertToImage() const
{
}

bool wxBitmap::IsOk() const
{
    return (m_refData != NULL) && (M_BMPDATA->m_width > 0);
}

int wxBitmap::GetHeight() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid bitmap") );

    return M_BMPDATA->m_height;
}

int wxBitmap::GetWidth() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid bitmap") );

    return M_BMPDATA->m_width;
}

int wxBitmap::GetDepth() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid bitmap") );

    return M_BMPDATA->m_bpp;
}

wxMask *wxBitmap::GetMask() const
{
    wxCHECK_MSG( Ok(), (wxMask *) NULL, wxT("invalid bitmap") );

    return M_BMPDATA->m_mask;
}

void wxBitmap::SetMask( wxMask *mask )
{
    wxCHECK_RET( Ok(), wxT("invalid bitmap") );

    AllocExclusive();
    if (M_BMPDATA->m_mask) {
        delete M_BMPDATA->m_mask;
    }
    M_BMPDATA->m_mask = mask;
}

bool wxBitmap::CopyFromIcon(const wxIcon& icon)
{
    *this = icon;
    return Ok();
}

wxBitmap wxBitmap::GetSubBitmap( const wxRect& rect) const
{
    wxBitmap ret;

    wxCHECK_MSG(Ok(), ret, wxT("invalid bitmap"));
    wxCHECK_MSG(rect.x >= 0 && rect.y >= 0 &&
                rect.x + rect.width <= M_BMPDATA->m_width &&
                rect.y + rect.height <= M_BMPDATA->m_height,
                ret, wxT("invalid bitmap region"));

    Magick::Image subImg(M_BMPDATA->m_image);
    subImg.modifyImage();
    subImg.crop(Magick::Geometry(rect.GetWidth(), rect.GetHeight(), rect.GetX(), rect.GetY()));
    ret.SetMagickImage(subImg);

    return ret;
}

bool wxBitmap::SaveFile( const wxString &name, wxBitmapType type, const wxPalette *WXUNUSED(palette) ) const
{
    wxCHECK_MSG( Ok(), false, wxT("invalid bitmap") );

    Magick::Image save = M_BMPDATA->m_image;
    save.modifyImage();
    save.magick(GetMagick(type));
    save.write((const char*)name.c_str());
    return true;
}

bool wxBitmap::LoadFile(const wxString &name, wxBitmapType type)
{
    UnRef();

    Magick::Image img((const char*)name.c_str());
    SetMagickImage(img);

    return Ok();
}

void wxBitmap::SetHeight(int height)
{
    AllocExclusive();
    M_BMPDATA->m_height = height;
    M_BMPDATA->m_image.modifyImage();
    M_BMPDATA->m_image.size(Magick::Geometry(M_BMPDATA->m_width, M_BMPDATA->m_height));
}

void wxBitmap::SetWidth(int width)
{
    AllocExclusive();
    M_BMPDATA->m_width = width;
    M_BMPDATA->m_image.modifyImage();
    M_BMPDATA->m_image.size(Magick::Geometry(M_BMPDATA->m_width, M_BMPDATA->m_height));
}

void wxBitmap::SetDepth(int depth)
{
    AllocExclusive();
    M_BMPDATA->m_image.modifyImage();
    M_BMPDATA->m_image.depth(depth);
    M_BMPDATA->m_bpp = M_BMPDATA->m_image.depth();
}

Magick::Image wxBitmap::GetMagickImage() const {
    return M_BMPDATA->m_image;
}

void wxBitmap::SetMagickImage(const Magick::Image& image)
{
    if (!m_refData)
        m_refData = new wxBitmapRefData;

    // AllocExclusive should not be needed for this internal function
    wxASSERT(m_refData->GetRefCount() == 1);
    M_BMPDATA->m_image = image;
    Magick::Geometry dim = image.geometry();
    M_BMPDATA->m_width = dim.width();
    M_BMPDATA->m_height = dim.height();
    M_BMPDATA->m_bpp = image.depth();
}

wxObjectRefData* wxBitmap::CreateRefData() const
{
    return new wxBitmapRefData;
}

wxObjectRefData* wxBitmap::CloneRefData(const wxObjectRefData* data) const
{
    const wxBitmapRefData* oldRef = wx_static_cast(const wxBitmapRefData*, data);
    wxBitmapRefData* newRef = new wxBitmapRefData;
    newRef->m_width = oldRef->m_width;
    newRef->m_height = oldRef->m_height;
    newRef->m_bpp = oldRef->m_bpp;
    newRef->m_image = oldRef->m_image;

    return newRef;
}

//-----------------------------------------------------------------------------
// wxBitmapHandler
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxBitmapHandler, wxBitmapHandlerBase)

/* static */ void wxBitmap::InitStandardHandlers()
{
    // TODO
}
