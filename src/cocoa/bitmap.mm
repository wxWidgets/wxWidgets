/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/bitmap.cpp
// Purpose:     wxBitmap
// Author:      David Elliott
// Modified by:
// Created:     2003/07/19
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/setup.h"
#include "wx/utils.h"
#include "wx/palette.h"
#include "wx/bitmap.h"
#include "wx/icon.h"
#include "wx/log.h"
#include "wx/image.h"
#include "wx/xpmdecod.h"

#include "wx/cocoa/autorelease.h"
#include "wx/cocoa/string.h"

#import <AppKit/NSBitmapImageRep.h>
#import <AppKit/NSGraphics.h>

// ========================================================================
// wxBitmapRefData
// ========================================================================
class wxBitmapRefData: public wxGDIRefData
{
    friend class wxBitmap;
public:
    wxBitmapRefData();
    wxBitmapRefData( const wxBitmapRefData& data );
    virtual ~wxBitmapRefData();

protected:
    int                 m_width;
    int                 m_height;
    int                 m_depth;
    bool                m_ok;
    int                 m_numColors;
    wxPalette           m_bitmapPalette;
    int                 m_quality;
    WX_NSBitmapImageRep m_cocoaNSBitmapImageRep;
    wxMask             *m_bitmapMask; // Optional mask
};

#define M_BITMAPDATA ((wxBitmapRefData *)m_refData)

wxBitmapRefData::wxBitmapRefData()
{
    m_ok = FALSE;
    m_width = 0;
    m_height = 0;
    m_depth = 0;
    m_quality = 0;
    m_numColors = 0;
    m_cocoaNSBitmapImageRep = nil;
    m_bitmapMask = NULL;
}

wxBitmapRefData::wxBitmapRefData( const wxBitmapRefData& data)
{
    m_width = data.m_width;
    m_height = data.m_height;
    m_depth = data.m_depth;
    m_ok = data.m_ok;
    m_numColors = data.m_numColors;
    m_bitmapPalette = data.m_bitmapPalette;
    m_quality = data.m_quality;
    m_cocoaNSBitmapImageRep = data.m_cocoaNSBitmapImageRep;
    [m_cocoaNSBitmapImageRep retain];
    m_bitmapMask = data.m_bitmapMask?new wxMask(*data.m_bitmapMask):NULL;
}

wxBitmapRefData::~wxBitmapRefData()
{
    [m_cocoaNSBitmapImageRep release];
    m_cocoaNSBitmapImageRep = NULL;

    delete m_bitmapMask;
    m_bitmapMask = NULL;
}

// ========================================================================
// wxBitmap
// ========================================================================
IMPLEMENT_DYNAMIC_CLASS(wxBitmap, wxGDIObject)

wxBitmap::wxBitmap()
{
    m_refData = NULL;

    if ( wxTheBitmapList )
        wxTheBitmapList->AddBitmap(this);
}

wxBitmap::~wxBitmap()
{
    if (wxTheBitmapList)
        wxTheBitmapList->DeleteObject(this);
}

wxBitmap::wxBitmap(const char bits[], int the_width, int the_height, int no_bits)
{
    m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_width = the_width ;
    M_BITMAPDATA->m_height = the_height ;
    M_BITMAPDATA->m_depth = no_bits ;
    M_BITMAPDATA->m_numColors = 0;

    /* TODO: create the bitmap from data */

    if ( wxTheBitmapList )
        wxTheBitmapList->AddBitmap(this);
}

wxBitmap::wxBitmap(int w, int h, int d)
{
    (void)Create(w, h, d);

    if ( wxTheBitmapList )
        wxTheBitmapList->AddBitmap(this);
}

wxBitmap::wxBitmap(void *data, wxBitmapType type, int width, int height, int depth)
{
    (void) Create(data, type, width, height, depth);

    if ( wxTheBitmapList )
        wxTheBitmapList->AddBitmap(this);
}

wxBitmap::wxBitmap(const wxString& filename, wxBitmapType type)
{
    LoadFile(filename, type);

    if ( wxTheBitmapList )
        wxTheBitmapList->AddBitmap(this);
}

wxObjectRefData *wxBitmap::CreateRefData() const
{
    return new wxBitmapRefData;
}

wxObjectRefData *wxBitmap::CloneRefData(const wxObjectRefData *data) const
{
    return new wxBitmapRefData(*(wxBitmapRefData*)data);
}

WX_NSBitmapImageRep wxBitmap::GetNSBitmapImageRep()
{
    if(!M_BITMAPDATA)
        return NULL;
    return M_BITMAPDATA->m_cocoaNSBitmapImageRep;
}

void wxBitmap::SetWidth(int w)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_width = w;
}

void wxBitmap::SetHeight(int h)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_height = h;
}

void wxBitmap::SetDepth(int d)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_depth = d;
}

void wxBitmap::SetQuality(int q)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_quality = q;
}

void wxBitmap::SetOk(bool isOk)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_ok = isOk;
}

void wxBitmap::SetPalette(const wxPalette& palette)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_bitmapPalette = palette ;
}

void wxBitmap::SetMask(wxMask *mask)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_bitmapMask = mask ;
}

bool wxBitmap::Ok() const
{
    return m_refData && M_BITMAPDATA->m_ok;
}

wxPalette* wxBitmap::GetPalette() const
{
    if(!m_refData)
        return NULL;
    return &M_BITMAPDATA->m_bitmapPalette;
}

wxMask* wxBitmap::GetMask() const
{
    if(!m_refData)
        return NULL;
    return M_BITMAPDATA->m_bitmapMask;
}

int wxBitmap::GetDepth() const
{
    if(!m_refData)
        return 0;
    return M_BITMAPDATA->m_depth;
}

int wxBitmap::GetWidth() const
{
    if(!m_refData)
        return 0;
    return M_BITMAPDATA->m_width;
}

int wxBitmap::GetHeight() const
{
    if(!m_refData)
        return 0;
    return M_BITMAPDATA->m_height;
}

bool wxBitmap::Create(int w, int h, int d)
{
    UnRef();

    m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_width = w;
    M_BITMAPDATA->m_height = h;
    M_BITMAPDATA->m_depth = d;

    /* TODO: create new bitmap */

    return M_BITMAPDATA->m_ok;
}

bool wxBitmap::LoadFile(const wxString& filename, wxBitmapType type)
{
    wxAutoNSAutoreleasePool pool;
    UnRef();

    m_refData = new wxBitmapRefData;

    NSBitmapImageRep *imageRep = [NSBitmapImageRep
        imageRepWithContentsOfFile:wxNSStringWithWxString(filename)];

    if(imageRep)
    {
        M_BITMAPDATA->m_width = [imageRep pixelsWide];
        M_BITMAPDATA->m_height = [imageRep pixelsHigh];
        M_BITMAPDATA->m_depth = 24; // FIXME
        M_BITMAPDATA->m_ok = true;
        M_BITMAPDATA->m_numColors = 0;
        M_BITMAPDATA->m_quality = 0;
        M_BITMAPDATA->m_cocoaNSBitmapImageRep = [imageRep retain];
        M_BITMAPDATA->m_bitmapMask = NULL;
        return true;
    }
    wxImage image;
    if(!image.LoadFile(filename,type))
        return false;
    if(!image.Ok())
        return false;
    *this = wxBitmap(image);
    return true;
}

bool wxBitmap::Create(void *data, wxBitmapType type, int width, int height, int depth)
{
    UnRef();

    m_refData = new wxBitmapRefData;

    return false;
}

bool wxBitmap::SaveFile(const wxString& filename, wxBitmapType type, const wxPalette *palette) const
{
    return false;
}

bool wxBitmap::CopyFromIcon(const wxIcon& icno)
{
    return false;
}

wxBitmap wxBitmap::GetSubBitmap(wxRect const&) const
{
    return wxNullBitmap;
}

wxImage wxBitmap::ConvertToImage() const
{
    if(!M_BITMAPDATA->m_ok)
        return wxImage(5,5)/*wxNullImage*/;
    return wxImage(M_BITMAPDATA->m_width,M_BITMAPDATA->m_height);
}

bool wxBitmap::CreateFromXpm(const char **xpm)
{
#if wxUSE_IMAGE && wxUSE_XPM
    UnRef();

    wxCHECK_MSG( xpm, false, wxT("invalid XPM data") )

    wxXPMDecoder decoder;
    wxImage img = decoder.ReadData(xpm);
    wxCHECK_MSG( img.Ok(), false, wxT("invalid XPM data") )

    *this = wxBitmap(img);
    return true;
#else
    return false;
#endif
}

bool wxBitmap::CreateFromImage(const wxImage& image, int depth)
{
    UnRef();

    wxCHECK_MSG(image.Ok(), false, wxT("invalid image"));
    wxCHECK_MSG(depth == -1 || depth == 1, false, wxT("invalid bitmap depth"));

    m_refData = new wxBitmapRefData();

    M_BITMAPDATA->m_width = image.GetWidth();
    M_BITMAPDATA->m_height = image.GetHeight();
    NSBitmapImageRep *bitmapImage = [[NSBitmapImageRep alloc]
            initWithBitmapDataPlanes: NULL
            pixelsWide: image.GetWidth()
            pixelsHigh: image.GetHeight()
            bitsPerSample: 8
            samplesPerPixel: 3
            hasAlpha: NO
            isPlanar: NO
            colorSpaceName: NSCalibratedRGBColorSpace
            bytesPerRow: 0
            bitsPerPixel: 0];

    const int numBytes = image.GetWidth()*image.GetHeight()*3;
    memcpy([bitmapImage bitmapData], image.GetData(), numBytes);
    // TODO: Alpha and convert to desired depth
    M_BITMAPDATA->m_depth = 24;
    M_BITMAPDATA->m_ok = true;
    M_BITMAPDATA->m_numColors = 0;
    M_BITMAPDATA->m_quality = 0;
    M_BITMAPDATA->m_cocoaNSBitmapImageRep = bitmapImage;
    M_BITMAPDATA->m_bitmapMask = NULL;
    return true;
}

// ========================================================================
// wxMask
// ========================================================================

IMPLEMENT_DYNAMIC_CLASS(wxMask, wxObject)

wxMask::wxMask()
{
/* TODO
    m_maskBitmap = 0;
*/
}

// Construct a mask from a bitmap and a colour indicating
// the transparent area
wxMask::wxMask(const wxBitmap& bitmap, const wxColour& colour)
{
/* TODO
    m_maskBitmap = 0;
*/
    Create(bitmap, colour);
}

// Construct a mask from a bitmap and a palette index indicating
// the transparent area
wxMask::wxMask(const wxBitmap& bitmap, int paletteIndex)
{
/* TODO
    m_maskBitmap = 0;
*/

    Create(bitmap, paletteIndex);
}

// Construct a mask from a mono bitmap (copies the bitmap).
wxMask::wxMask(const wxBitmap& bitmap)
{
/* TODO
    m_maskBitmap = 0;
*/

    Create(bitmap);
}

wxMask::~wxMask()
{
// TODO: delete mask bitmap
}

// Create a mask from a mono bitmap (copies the bitmap).
bool wxMask::Create(const wxBitmap& bitmap)
{
// TODO
    return FALSE;
}

// Create a mask from a bitmap and a palette index indicating
// the transparent area
bool wxMask::Create(const wxBitmap& bitmap, int paletteIndex)
{
// TODO
    return FALSE;
}

// Create a mask from a bitmap and a colour indicating
// the transparent area
bool wxMask::Create(const wxBitmap& bitmap, const wxColour& colour)
{
// TODO
    return FALSE;
}

