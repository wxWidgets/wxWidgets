/////////////////////////////////////////////////////////////////////////////
// Name:        bitmap.cpp
// Purpose:     wxBitmap
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "bitmap.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/setup.h"
#include "wx/list.h"
#include "wx/utils.h"
#include "wx/app.h"
#include "wx/palette.h"
#include "wx/dcmemory.h"
#include "wx/bitmap.h"
#include "wx/icon.h"
#endif

#include "wx/msw/private.h"
#include "wx/log.h"

#include "assert.h"

#include "wx/msw/dib.h"

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxBitmap, wxGDIObject)
IMPLEMENT_DYNAMIC_CLASS(wxMask, wxObject)
#endif

wxBitmapRefData::wxBitmapRefData(void)
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

wxBitmapRefData::~wxBitmapRefData(void)
{
  if (m_selectedInto)
  {
    char buf[200];
    sprintf(buf, "Bitmap was deleted without selecting out of wxMemoryDC %X.", (unsigned int) m_selectedInto);
    wxFatalError(buf);
  }
  if (m_hBitmap)
  {
    DeleteObject((HBITMAP) m_hBitmap);
  }
  m_hBitmap = 0 ;

  if (m_bitmapMask)
    delete m_bitmapMask;
  m_bitmapMask = NULL;

}

wxList wxBitmap::sm_handlers;

wxBitmap::wxBitmap(void)
{
  if ( wxTheBitmapList )
    wxTheBitmapList->AddBitmap(this);
}

wxBitmap::~wxBitmap(void)
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
    char buf[200];
    sprintf(buf, "Bitmap %X was deleted without selecting out of wxMemoryDC %X.", (unsigned int) this, (unsigned int) M_BITMAPDATA->m_selectedInto);
    wxFatalError(buf);
  }
  if (M_BITMAPDATA->m_hBitmap)
  {
    DeleteObject((HBITMAP) M_BITMAPDATA->m_hBitmap);
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

  M_BITMAPDATA->m_hBitmap = (WXHBITMAP) CreateBitmap(the_width, the_height, 1, no_bits, bits);

  if (M_BITMAPDATA->m_hBitmap)
    M_BITMAPDATA->m_ok = TRUE;
  else
    M_BITMAPDATA->m_ok = FALSE;

  M_BITMAPDATA->m_selectedInto = NULL;

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

  if (d > 0)
  {
    M_BITMAPDATA->m_hBitmap = (WXHBITMAP) CreateBitmap(w, h, 1, d, NULL);
  }
  else
  {
    HDC dc = GetDC((HWND) NULL);
    M_BITMAPDATA->m_hBitmap = (WXHBITMAP) CreateCompatibleBitmap(dc, w, h);
    ReleaseDC((HWND) NULL, dc);
    M_BITMAPDATA->m_depth = wxDisplayDepth();
  }
  if (M_BITMAPDATA->m_hBitmap)
    M_BITMAPDATA->m_ok = TRUE;
  else
    M_BITMAPDATA->m_ok = FALSE;
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
    if ( (handler->GetName().Cmp(name) == 0) )
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
    if ( (handler->GetExtension().Cmp(extension) == 0) &&
          (bitmapType == -1 || (handler->GetType() == bitmapType)) )
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

// New Create/FreeDIB functions since ones in dibutils.cpp are confusing
static long createDIB(long xSize, long ySize, long bitsPerPixel,
                       HPALETTE hPal, LPBITMAPINFO* lpDIBHeader);
static long freeDIB(LPBITMAPINFO lpDIBHeader);

// Creates a bitmap that matches the device context, from
// an arbitray bitmap. At present, the original bitmap must have an
// associated palette. TODO: use a default palette if no palette exists.
// Contributed by Frederic Villeneuve <frederic.villeneuve@natinst.com>
wxBitmap wxBitmap::GetBitmapForDC(wxDC& dc) const
{
    wxMemoryDC      memDC;
    wxBitmap        tmpBitmap(this->GetWidth(), this->GetHeight(), dc.GetDepth());
    HPALETTE        hPal = (HPALETTE) NULL;
    LPBITMAPINFO    lpDib;
    void            *lpBits = (void*) NULL;

/*
    wxASSERT( this->GetPalette() && this->GetPalette()->Ok() && (this->GetPalette()->GetHPALETTE() != 0) );

    tmpBitmap.SetPalette(this->GetPalette());
    memDC.SelectObject(tmpBitmap);
    memDC.SetPalette(this->GetPalette());

    hPal = (HPALETTE) this->GetPalette()->GetHPALETTE();
*/
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
    return (tmpBitmap);
}

/*
 * wxMask
 */

wxMask::wxMask(void)
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

wxMask::~wxMask(void)
{
    if ( m_maskBitmap )
        ::DeleteObject((HBITMAP) m_maskBitmap);
}

// Create a mask from a mono bitmap (copies the bitmap).
bool wxMask::Create(const wxBitmap& bitmap)
{
    if ( m_maskBitmap )
  {
    ::DeleteObject((HBITMAP) m_maskBitmap);
    m_maskBitmap = 0;
  }
  if (!bitmap.Ok() || bitmap.GetDepth() != 1)
  {
    return FALSE;
  }
  m_maskBitmap = (WXHBITMAP) CreateBitmap(
          bitmap.GetWidth(),
          bitmap.GetHeight(),
          1, 1, 0
          );
  HDC srcDC = CreateCompatibleDC(0);
  SelectObject(srcDC, (HBITMAP) bitmap.GetHBITMAP());
  HDC destDC = CreateCompatibleDC(0);
  SelectObject(destDC, (HBITMAP) m_maskBitmap);
  BitBlt(destDC, 0, 0, bitmap.GetWidth(), bitmap.GetHeight(), srcDC, 0, 0, SRCCOPY);
  SelectObject(srcDC, 0);
  DeleteDC(srcDC);
  SelectObject(destDC, 0);
  DeleteDC(destDC);
  return TRUE;
}

// Create a mask from a bitmap and a palette index indicating
// the transparent area
bool wxMask::Create(const wxBitmap& bitmap, int paletteIndex)
{
    if ( m_maskBitmap )
  {
    ::DeleteObject((HBITMAP) m_maskBitmap);
    m_maskBitmap = 0;
  }
  if (bitmap.Ok() && bitmap.GetPalette()->Ok())
  {
    unsigned char red, green, blue;
    if (bitmap.GetPalette()->GetRGB(paletteIndex, &red, &green, &blue))
    {
      wxColour transparentColour(red, green, blue);
      return Create(bitmap, transparentColour);
    }
  }
  return FALSE;
}

// Create a mask from a bitmap and a colour indicating
// the transparent area
bool wxMask::Create(const wxBitmap& bitmap, const wxColour& colour)
{
    if ( m_maskBitmap )
  {
    ::DeleteObject((HBITMAP) m_maskBitmap);
    m_maskBitmap = 0;
  }
  if (!bitmap.Ok())
  {
    return FALSE;
  }

  // scan the bitmap for the transparent colour and set
  // the corresponding pixels in the mask to BLACK and
  // the rest to WHITE
  COLORREF maskColour = RGB(colour.Red(), colour.Green(), colour.Blue());
  m_maskBitmap = (WXHBITMAP) ::CreateBitmap(
          bitmap.GetWidth(),
          bitmap.GetHeight(),
          1, 1, 0
          );
  HDC srcDC = ::CreateCompatibleDC(0);
  ::SelectObject(srcDC, (HBITMAP) bitmap.GetHBITMAP());
  HDC destDC = ::CreateCompatibleDC(0);
  ::SelectObject(destDC, (HBITMAP) m_maskBitmap);

  // this is not very efficient, but I can't think
  // of a better way of doing it
  for (int w = 0; w < bitmap.GetWidth(); w++)
  {
    for (int h = 0; h < bitmap.GetHeight(); h++)
    {
      COLORREF col = GetPixel(srcDC, w, h);
      if (col == maskColour)
      {
        ::SetPixel(destDC, w, h, RGB(0, 0, 0));
      }
      else
      {
        ::SetPixel(destDC, w, h, RGB(255, 255, 255));
      }
    }
  }
  ::SelectObject(srcDC, 0);
  ::DeleteDC(srcDC);
  ::SelectObject(destDC, 0);
  ::DeleteDC(destDC);
  return TRUE;
}

/*
 * wxBitmapHandler
 */

IMPLEMENT_DYNAMIC_CLASS(wxBitmapHandler, wxObject)

bool wxBitmapHandler::Create(wxBitmap *WXUNUSED(bitmap), void *WXUNUSED(data), long WXUNUSED(type), int WXUNUSED(width), int WXUNUSED(height), int WXUNUSED(depth))
{
  return FALSE;
}

bool wxBitmapHandler::LoadFile(wxBitmap *WXUNUSED(bitmap), const wxString& WXUNUSED(name), long WXUNUSED(type),
    int WXUNUSED(desiredWidth), int WXUNUSED(desiredHeight))
{
  return FALSE;
}

bool wxBitmapHandler::SaveFile(wxBitmap *WXUNUSED(bitmap), const wxString& WXUNUSED(name), int WXUNUSED(type), const wxPalette *WXUNUSED(palette))
{
  return FALSE;
}

/*
 * Standard handlers
 */

class WXDLLEXPORT wxBMPResourceHandler: public wxBitmapHandler
{
  DECLARE_DYNAMIC_CLASS(wxBMPResourceHandler)
public:
  inline wxBMPResourceHandler(void)
  {
  m_name = "Windows bitmap resource";
  m_extension = "";
  m_type = wxBITMAP_TYPE_BMP_RESOURCE;
  };

  virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
      int desiredWidth, int desiredHeight);
};
IMPLEMENT_DYNAMIC_CLASS(wxBMPResourceHandler, wxBitmapHandler)

bool wxBMPResourceHandler::LoadFile(wxBitmap *bitmap, const wxString& name, long WXUNUSED(flags),
    int WXUNUSED(desiredWidth), int WXUNUSED(desiredHeight))
{
    // TODO: load colourmap.
    M_BITMAPHANDLERDATA->m_hBitmap = (WXHBITMAP) ::LoadBitmap(wxGetInstance(), name);
    if (M_BITMAPHANDLERDATA->m_hBitmap)
    {
        M_BITMAPHANDLERDATA->m_ok = TRUE;
        BITMAP bm;
        GetObject((HBITMAP) M_BITMAPHANDLERDATA->m_hBitmap, sizeof(BITMAP), (LPSTR) &bm);
        M_BITMAPHANDLERDATA->m_width = bm.bmWidth;
        M_BITMAPHANDLERDATA->m_height = bm.bmHeight;
        M_BITMAPHANDLERDATA->m_depth = bm.bmBitsPixel;

        if ( bitmap->IsKindOf(CLASSINFO(wxIcon)) )
        {
        }

        return TRUE;
    }

  // it's probably not found
  wxLogError("Can't load bitmap '%s' from resources! Check .rc file.", name.c_str());

  return FALSE;
}

class WXDLLEXPORT wxBMPFileHandler: public wxBitmapHandler
{
  DECLARE_DYNAMIC_CLASS(wxBMPFileHandler)
public:
  inline wxBMPFileHandler(void)
  {
  m_name = "Windows bitmap file";
  m_extension = "bmp";
  m_type = wxBITMAP_TYPE_BMP;
  };

  virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
      int desiredWidth, int desiredHeight);
  virtual bool SaveFile(wxBitmap *bitmap, const wxString& name, int type, const wxPalette *palette = NULL);
};
IMPLEMENT_DYNAMIC_CLASS(wxBMPFileHandler, wxBitmapHandler)

bool wxBMPFileHandler::LoadFile(wxBitmap *bitmap, const wxString& name, long WXUNUSED(flags),
    int WXUNUSED(desiredWidth), int WXUNUSED(desiredHeight))
{
#if wxUSE_IMAGE_LOADING_IN_MSW
    wxPalette *palette = NULL;
    bool success = FALSE;
/*
    if (type & wxBITMAP_DISCARD_COLOURMAP)
      success = wxLoadIntoBitmap(WXSTRINGCAST name, bitmap);
    else
*/
    success = (wxLoadIntoBitmap(WXSTRINGCAST name, bitmap, &palette) != 0);
    if (!success && palette)
    {
      delete palette;
      palette = NULL;
    }
    if (palette)
    {
        M_BITMAPHANDLERDATA->m_bitmapPalette = *palette;
        delete palette;
    }
    return success;
#else
  return FALSE;
#endif
}

bool wxBMPFileHandler::SaveFile(wxBitmap *bitmap, const wxString& name, int WXUNUSED(type), const wxPalette *pal)
{
#if wxUSE_IMAGE_LOADING_IN_MSW
    wxPalette *actualPalette = (wxPalette *)pal;
    if (!actualPalette && (!M_BITMAPHANDLERDATA->m_bitmapPalette.IsNull()))
      actualPalette = & (M_BITMAPHANDLERDATA->m_bitmapPalette);
    return (wxSaveBitmap(WXSTRINGCAST name, bitmap, actualPalette) != 0);
#else
  return FALSE;
#endif
}

void wxBitmap::CleanUpHandlers(void)
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

void wxBitmap::InitStandardHandlers(void)
{
  AddHandler(new wxBMPResourceHandler);
  AddHandler(new wxBMPFileHandler);

  // Not added by default: include xpmhand.h in your app
  // and call these in your wxApp::OnInit.
//  AddHandler(new wxXPMFileHandler);
//  AddHandler(new wxXPMDataHandler);

  AddHandler(new wxICOResourceHandler);
  AddHandler(new wxICOFileHandler);
}

static long createDIB(long xSize, long ySize, long bitsPerPixel,
                       HPALETTE hPal, LPBITMAPINFO* lpDIBHeader)
{
   unsigned long   i, headerSize;
   LPBITMAPINFO    lpDIBheader = NULL;
   LPPALETTEENTRY  lpPe = NULL;


   // Allocate space for a DIB header
   headerSize = (sizeof(BITMAPINFOHEADER) + (256 * sizeof(PALETTEENTRY)));
   lpDIBheader = (BITMAPINFO *) malloc(headerSize);
   lpPe = (PALETTEENTRY *)((BYTE*)lpDIBheader + sizeof(BITMAPINFOHEADER));

   GetPaletteEntries(hPal, 0, 256, lpPe);


   memset(lpDIBheader, 0x00, sizeof(BITMAPINFOHEADER));


   // Fill in the static parts of the DIB header
   lpDIBheader->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   lpDIBheader->bmiHeader.biWidth = xSize;
   lpDIBheader->bmiHeader.biHeight = ySize;
   lpDIBheader->bmiHeader.biPlanes = 1;

   // this value must be 1, 4, 8 or 24 so PixelDepth can only be
   lpDIBheader->bmiHeader.biBitCount = (WORD)(bitsPerPixel);
   lpDIBheader->bmiHeader.biCompression = BI_RGB;
   lpDIBheader->bmiHeader.biSizeImage = xSize * abs(ySize) * bitsPerPixel >>
3;
   lpDIBheader->bmiHeader.biClrUsed = 256;


   // Initialize the DIB palette
   for (i = 0; i < 256; i++) {
      lpDIBheader->bmiColors[i].rgbReserved = lpPe[i].peFlags;
      lpDIBheader->bmiColors[i].rgbRed = lpPe[i].peRed;
      lpDIBheader->bmiColors[i].rgbGreen = lpPe[i].peGreen;
      lpDIBheader->bmiColors[i].rgbBlue = lpPe[i].peBlue;
   }

   *lpDIBHeader = lpDIBheader;


   return (0);

}



static long freeDIB(LPBITMAPINFO lpDIBHeader)
{

   if (lpDIBHeader != NULL) {
      free(lpDIBHeader);
   }

   return (0);
}


