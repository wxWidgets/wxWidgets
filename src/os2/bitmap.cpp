/////////////////////////////////////////////////////////////////////////////
// Name:        bitmap.cpp
// Purpose:     wxBitmap
// Author:      David Webster
// Modified by:
// Created:     08/08/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include <stdio.h>

    #include "wx/list.h"
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/palette.h"
    #include "wx/dcmemory.h"
    #include "wx/bitmap.h"
    #include "wx/icon.h"
#endif

#include "wx/os2/private.h"
#include "wx/log.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

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
    m_hBitmap = 0 ;
    m_selectedInto = NULL;
    m_numColors = 0;
    m_bitmapMask = NULL;
}

wxBitmapRefData::~wxBitmapRefData()
{
  if (m_selectedInto)
  {
    wxChar buf[200];
    wxSprintf(buf, wxT("Bitmap was deleted without selecting out of wxMemoryDC %lX."), (unsigned long) m_selectedInto);
    wxFatalError(buf);
  }
  if (m_hBitmap)
  {
// TODO:    DeleteObject((HBITMAP) m_hBitmap);
  }
  m_hBitmap = 0 ;

    if (m_bitmapMask)
        delete m_bitmapMask;
    m_bitmapMask = NULL;
}

wxList wxBitmap::sm_handlers;

wxBitmap::wxBitmap()
{
    m_refData = NULL;

    if ( wxTheBitmapList )
        wxTheBitmapList->AddBitmap(this);
}

wxBitmap::wxBitmap(const wxBitmap& bitmap)
{
// TODO:
/*
    wxIcon *icon = wxDynamicCast(&bitmap, wxIcon);
    if ( icon )
    {
        HDC hdc = ::CreateCompatibleDC(NULL);   // screen DC
        HBITMAP hbitmap = ::CreateCompatibleBitmap(hdc,
                                                   icon->GetWidth(),
                                                   icon->GetHeight());
        ::SelectObject(hdc, hbitmap);
        ::DrawIcon(hdc, 0, 0, (HICON)icon->GetHICON());

        ::DeleteDC(hdc);

        SetHBITMAP((WXHBITMAP)hbitmap);
    }
    else
    {
        Ref(bitmap);
    }

    if ( wxTheBitmapList )
        wxTheBitmapList->AddBitmap(this);
*/
}

wxBitmap::~wxBitmap()
{
    if (wxTheBitmapList)
        wxTheBitmapList->DeleteObject(this);
}

bool wxBitmap::FreeResource(bool WXUNUSED(force))
{
  if ( !M_BITMAPDATA )
  return FALSE;

  if (M_BITMAPDATA->m_selectedInto)
  {
    wxChar buf[200];
    wxSprintf(buf, wxT("Bitmap %lX was deleted without selecting out of wxMemoryDC %lX."), (unsigned long) this, (unsigned long) M_BITMAPDATA->m_selectedInto);
    wxFatalError(buf);
  }
  if (M_BITMAPDATA->m_hBitmap)
  {
// TODO:    DeleteObject((HBITMAP) M_BITMAPDATA->m_hBitmap);
  }
  M_BITMAPDATA->m_hBitmap = 0 ;

/*
  if (M_BITMAPDATA->m_bitmapPalette)
    delete M_BITMAPDATA->m_bitmapPalette;

  M_BITMAPDATA->m_bitmapPalette = NULL ;
*/

  return TRUE;
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

// Create from XPM data
wxBitmap::wxBitmap(char **data, wxControl *WXUNUSED(anItem))
{
  (void) Create((void *)data, wxBITMAP_TYPE_XPM_DATA, 0, 0, 0);
}

wxBitmap::wxBitmap(int w, int h, int d)
{
    (void)Create(w, h, d);

    if ( wxTheBitmapList )
        wxTheBitmapList->AddBitmap(this);
}

wxBitmap::wxBitmap(void *data, long type, int width, int height, int depth)
{
    (void) Create(data, type, width, height, depth);

    if ( wxTheBitmapList )
        wxTheBitmapList->AddBitmap(this);
}

wxBitmap::wxBitmap(const wxString& filename, long type)
{
    LoadFile(filename, (int)type);

    if ( wxTheBitmapList )
        wxTheBitmapList->AddBitmap(this);
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

bool wxBitmap::LoadFile(const wxString& filename, long type)
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

bool wxBitmap::Create(void *data, long type, int width, int height, int depth)
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

bool wxBitmap::SaveFile(const wxString& filename, int type, const wxPalette *palette)
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

void wxBitmap::SetHBITMAP(WXHBITMAP bmp)
{
  if (!M_BITMAPDATA)
  m_refData = new wxBitmapRefData;

  M_BITMAPDATA->m_hBitmap = bmp;
}

void wxBitmap::AddHandler(wxBitmapHandler *handler)
{
    sm_handlers.Append(handler);
}

void wxBitmap::InsertHandler(wxBitmapHandler *handler)
{
    sm_handlers.Insert(handler);
}

bool wxBitmap::RemoveHandler(const wxString& name)
{
    wxBitmapHandler *handler = FindHandler(name);
    if ( handler )
    {
        sm_handlers.DeleteObject(handler);
        return TRUE;
    }
    else
        return FALSE;
}

wxBitmapHandler *wxBitmap::FindHandler(const wxString& name)
{
    wxNode *node = sm_handlers.First();
    while ( node )
    {
        wxBitmapHandler *handler = (wxBitmapHandler *)node->Data();
        if ( handler->GetName() == name )
            return handler;
        node = node->Next();
    }
    return NULL;
}

wxBitmapHandler *wxBitmap::FindHandler(const wxString& extension, long bitmapType)
{
    wxNode *node = sm_handlers.First();
    while ( node )
    {
        wxBitmapHandler *handler = (wxBitmapHandler *)node->Data();
        if ( handler->GetExtension() == extension &&
                    (bitmapType == -1 || handler->GetType() == bitmapType) )
            return handler;
        node = node->Next();
    }
    return NULL;
}

wxBitmapHandler *wxBitmap::FindHandler(long bitmapType)
{
    wxNode *node = sm_handlers.First();
    while ( node )
    {
        wxBitmapHandler *handler = (wxBitmapHandler *)node->Data();
        if (handler->GetType() == bitmapType)
            return handler;
        node = node->Next();
    }
    return NULL;
}

// Creates a bitmap that matches the device context, from
// an arbitray bitmap. At present, the original bitmap must have an
// associated palette. TODO: use a default palette if no palette exists.
// Contributed by Frederic Villeneuve <frederic.villeneuve@natinst.com>
wxBitmap wxBitmap::GetBitmapForDC(wxDC& dc) const
{
    wxBitmap        tmpBitmap(this->GetWidth(), this->GetHeight(), dc.GetDepth());
// TODO:
/*
    wxMemoryDC      memDC;
    HPALETTE        hPal = (HPALETTE) NULL;
    LPBITMAPINFO    lpDib;
    void            *lpBits = (void*) NULL;


    wxASSERT( this->GetPalette() && this->GetPalette()->Ok() && (this->GetPalette()->GetHPALETTE() != 0) );

    tmpBitmap.SetPalette(this->GetPalette());
    memDC.SelectObject(tmpBitmap);
    memDC.SetPalette(this->GetPalette());

    hPal = (HPALETTE) this->GetPalette()->GetHPALETTE();

    if( this->GetPalette() && this->GetPalette()->Ok() && (this->GetPalette()->GetHPALETTE() != 0) )
    {
        tmpBitmap.SetPalette(* this->GetPalette());
        memDC.SelectObject(tmpBitmap);
        memDC.SetPalette(* this->GetPalette());
        hPal = (HPALETTE) this->GetPalette()->GetHPALETTE();
    }
    else
    {
        hPal = (HPALETTE) ::GetStockObject(DEFAULT_PALETTE);
        wxPalette palette;
        palette.SetHPALETTE( (WXHPALETTE)hPal );
        tmpBitmap.SetPalette( palette );
        memDC.SelectObject(tmpBitmap);
        memDC.SetPalette( palette );
    }

    // set the height negative because in a DIB the order of the lines is reversed
    createDIB(this->GetWidth(), -this->GetHeight(), this->GetDepth(), hPal, &lpDib);

    lpBits = malloc(lpDib->bmiHeader.biSizeImage);

    ::GetBitmapBits((HBITMAP)GetHBITMAP(), lpDib->bmiHeader.biSizeImage, lpBits);

    ::SetDIBitsToDevice((HDC) memDC.GetHDC(), 0, 0, this->GetWidth(), this->GetHeight(),
                        0, 0, 0, this->GetHeight(), lpBits, lpDib, DIB_RGB_COLORS);

    free(lpBits);

    freeDIB(lpDib);
*/
    return (tmpBitmap);
}

/*
 * wxMask
 */

wxMask::wxMask()
{
    m_maskBitmap = 0;
}

// Construct a mask from a bitmap and a colour indicating
// the transparent area
wxMask::wxMask(const wxBitmap& bitmap, const wxColour& colour)
{
    m_maskBitmap = 0;
    Create(bitmap, colour);
}

// Construct a mask from a bitmap and a palette index indicating
// the transparent area
wxMask::wxMask(const wxBitmap& bitmap, int paletteIndex)
{
    m_maskBitmap = 0;
    Create(bitmap, paletteIndex);
}

// Construct a mask from a mono bitmap (copies the bitmap).
wxMask::wxMask(const wxBitmap& bitmap)
{
    m_maskBitmap = 0;
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

bool wxBitmapHandler::Create(wxBitmap *bitmap, void *data, long type, int width, int height, int depth)
{
    return FALSE;
}

bool wxBitmapHandler::LoadFile(wxBitmap *bitmap, const wxString& name, long type,
        int desiredWidth, int desiredHeight)
{
    return FALSE;
}

bool wxBitmapHandler::SaveFile(wxBitmap *bitmap, const wxString& name, int type, const wxPalette *palette)
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

void wxBitmap::CleanUpHandlers()
{
    wxNode *node = sm_handlers.First();
    while ( node )
    {
        wxBitmapHandler *handler = (wxBitmapHandler *)node->Data();
        wxNode *next = node->Next();
        delete handler;
        delete node;
        node = next;
    }
}

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

