/////////////////////////////////////////////////////////////////////////////
// Name:        bitmap.cpp
// Purpose:     wxBitmap
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "bitmap.h"
#endif

#include "wx/setup.h"
#include "wx/utils.h"
#include "wx/palette.h"
#include "wx/bitmap.h"
#include "wx/icon.h"
#include "wx/log.h"
#include "wx/image.h"

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxBitmap, wxGDIObject)
IMPLEMENT_DYNAMIC_CLASS(wxMask, wxObject)
#endif

wxBitmapRefData::wxBitmapRefData()
{
    m_ok = FALSE;
    m_width = 0;
    m_height = 0;
    m_depth = 0;
    m_quality = 0;
    m_numColors = 0;
    m_bitmapMask = NULL;
}

wxBitmapRefData::~wxBitmapRefData()
{
    /*
     * TODO: delete the bitmap data here.
     */

    if (m_bitmapMask)
        delete m_bitmapMask;
    m_bitmapMask = NULL;
}

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

wxBitmap::wxBitmap(const wxImage& image, int depth)
{
}

wxBitmap::wxBitmap(char **bits)
{
}

/* TODO: maybe allow creation from XPM
// Create from data
wxBitmap::wxBitmap(const char **data)
{
    (void) Create((void *)data, wxBITMAP_TYPE_XPM_DATA, 0, 0, 0);
}
*/

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
    UnRef();

    m_refData = new wxBitmapRefData;

    wxBitmapHandler *handler = FindHandler(type);

    if ( handler == NULL ) {
        wxLogWarning("no bitmap handler for type %d defined.", type);

        return FALSE;
    }

    return handler->LoadFile(this, filename, type, -1, -1);
}

bool wxBitmap::Create(void *data, wxBitmapType type, int width, int height, int depth)
{
    UnRef();

    m_refData = new wxBitmapRefData;

    wxBitmapHandler *handler = FindHandler(type);

    if ( handler == NULL ) {
        wxLogWarning("no bitmap handler for type %d defined.", type);

        return FALSE;
    }

    return handler->Create(this, data, type, width, height, depth);
}

bool wxBitmap::SaveFile(const wxString& filename, wxBitmapType type, const wxPalette *palette) const
{
    wxBitmapHandler *handler = FindHandler(type);

    if ( handler == NULL ) {
        wxLogWarning("no bitmap handler for type %d defined.", type);

        return FALSE;
  }

  return handler->SaveFile(this, filename, type, palette);
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

/*
 * wxMask
 */

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

/*
 * wxBitmapHandler
 */

IMPLEMENT_DYNAMIC_CLASS(wxBitmapHandler, wxObject)

wxBitmapHandler::~wxBitmapHandler()
{
}

bool wxBitmapHandler::Create(wxBitmap *bitmap, void *data, long type, int width, int height, int depth)
{
    return FALSE;
}

bool wxBitmapHandler::LoadFile(wxBitmap *bitmap, const wxString& name, long type,
        int desiredWidth, int desiredHeight)
{
    return FALSE;
}

bool wxBitmapHandler::SaveFile(const wxBitmap *bitmap, const wxString& name, int type, const wxPalette *palette)
{
    return FALSE;
}

/*
 * Standard handlers
 */

/* TODO: bitmap handlers, a bit like this:
class WXDLLEXPORT wxBMPResourceHandler: public wxBitmapHandler
{
    DECLARE_DYNAMIC_CLASS(wxBMPResourceHandler)
public:
    inline wxBMPResourceHandler()
    {
        m_name = "Windows bitmap resource";
        m_extension = "";
        m_type = wxBITMAP_TYPE_BMP_RESOURCE;
    };

    virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
          int desiredWidth, int desiredHeight);
};
IMPLEMENT_DYNAMIC_CLASS(wxBMPResourceHandler, wxBitmapHandler)
*/

void wxBitmap::InitStandardHandlers()
{
/* TODO: initialize all standard bitmap or derive class handlers here.
    AddHandler(new wxBMPResourceHandler);
    AddHandler(new wxBMPFileHandler);
    AddHandler(new wxXPMFileHandler);
    AddHandler(new wxXPMDataHandler);
    AddHandler(new wxICOResourceHandler);
    AddHandler(new wxICOFileHandler);
*/
}

bool wxBitmap::CopyFromIcon(const wxIcon& icno)
{
    return false;
}

wxPalette* wxBitmap::GetPalette() const
{
    return NULL;
}

wxBitmap wxBitmap::GetSubBitmap(wxRect const&) const
{
    return wxNullBitmap;
}

wxImage wxBitmap::ConvertToImage() const
{
    return wxNullImage;
}

bool wxBitmap::Ok() const
{
    return FALSE;
}

wxMask* wxBitmap::GetMask() const
{
    return NULL;
}

int wxBitmap::GetDepth() const
{
    return 0;
}

int wxBitmap::GetWidth() const
{
    return 0;
}

int wxBitmap::GetHeight() const
{
    return 0;
}


bool wxBitmap::CreateFromXpm(const char **)
{
    return false;
}

// vim:sts=4:sw=4:syn=cpp:et
