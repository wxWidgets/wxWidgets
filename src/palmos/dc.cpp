/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/dc.cpp
// Purpose:     wxDC class
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/window.h"
    #include "wx/dc.h"
    #include "wx/utils.h"
    #include "wx/dialog.h"
    #include "wx/app.h"
    #include "wx/bitmap.h"
    #include "wx/dcmemory.h"
    #include "wx/log.h"
    #include "wx/icon.h"
    #include "wx/dcprint.h"
    #include "wx/module.h"
#endif

#include "wx/sysopt.h"
#include "wx/dynload.h"

#ifdef wxHAVE_RAW_BITMAP
#include "wx/rawbmp.h"
#endif

#include <string.h>

#ifndef AC_SRC_ALPHA
#define AC_SRC_ALPHA 1
#endif

/* Quaternary raster codes */
#ifndef MAKEROP4
#define MAKEROP4(fore,back) (DWORD)((((back) << 8) & 0xFF000000) | (fore))
#endif

IMPLEMENT_ABSTRACT_CLASS(wxDC, wxDCBase)

// ---------------------------------------------------------------------------
// constants
// ---------------------------------------------------------------------------

static const int VIEWPORT_EXTENT = 1000;

static const int MM_POINTS = 9;
static const int MM_METRIC = 10;

#define DSTCOPY 0x00AA0029

// ---------------------------------------------------------------------------
// private functions
// ---------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// wxDC
// ---------------------------------------------------------------------------

// Default constructor
wxDC::wxDC()
{
}

wxDC::~wxDC()
{
}

// This will select current objects out of the DC,
// which is what you have to do before deleting the
// DC.
void wxDC::SelectOldObjects(WXHDC dc)
{
}

// ---------------------------------------------------------------------------
// clipping
// ---------------------------------------------------------------------------

void wxDC::UpdateClipBox()
{
}

void
wxDC::DoGetClippingBox(wxCoord *x, wxCoord *y, wxCoord *w, wxCoord *h) const
{
}

// common part of DoSetClippingRegion() and DoSetClippingRegionAsRegion()
void wxDC::SetClippingHrgn(WXHRGN hrgn)
{
}

void wxDC::DoSetClippingRegion(wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{
}

void wxDC::DoSetClippingRegionAsRegion(const wxRegion& region)
{
}

void wxDC::DestroyClippingRegion()
{
}

// ---------------------------------------------------------------------------
// query capabilities
// ---------------------------------------------------------------------------

bool wxDC::CanDrawBitmap() const
{
    return false;
}

bool wxDC::CanGetTextExtent() const
{
    return false;
}

int wxDC::GetDepth() const
{
    return 0;
}

// ---------------------------------------------------------------------------
// drawing
// ---------------------------------------------------------------------------

void wxDC::Clear()
{
}

bool wxDC::DoFloodFill(wxCoord x, wxCoord y, const wxColour& col, int style)
{
    return false;
}

bool wxDC::DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const
{
    return false;
}

void wxDC::DoCrossHair(wxCoord x, wxCoord y)
{
}

void wxDC::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{
}

// Draws an arc of a circle, centred on (xc, yc), with starting point (x1, y1)
// and ending at (x2, y2)
void wxDC::DoDrawArc(wxCoord x1, wxCoord y1,
                     wxCoord x2, wxCoord y2,
                     wxCoord xc, wxCoord yc)
{
}

void wxDC::DoDrawCheckMark(wxCoord x1, wxCoord y1,
                           wxCoord width, wxCoord height)
{
}

void wxDC::DoDrawPoint(wxCoord x, wxCoord y)
{
}

void wxDC::DoDrawPolygon(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset,int fillStyle)
{
}

void
wxDC::DoDrawPolyPolygon(int n,
                        int count[],
                        wxPoint points[],
                        wxCoord xoffset,
                        wxCoord yoffset,
                        int fillStyle)
{
}

void wxDC::DoDrawLines(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset)
{
}

void wxDC::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
}

void wxDC::DoDrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius)
{
}

void wxDC::DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
}

void wxDC::DoDrawEllipticArc(wxCoord x,wxCoord y,wxCoord w,wxCoord h,double sa,double ea)
{
}

void wxDC::DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y)
{
}

void wxDC::DoDrawBitmap( const wxBitmap &bmp, wxCoord x, wxCoord y, bool useMask )
{
}

void wxDC::DoDrawText(const wxString& text, wxCoord x, wxCoord y)
{
}

void wxDC::DrawAnyText(const wxString& text, wxCoord x, wxCoord y)
{
}

void wxDC::DoDrawRotatedText(const wxString& text,
                             wxCoord x, wxCoord y,
                             double angle)
{
}

// ---------------------------------------------------------------------------
// set GDI objects
// ---------------------------------------------------------------------------

#if wxUSE_PALETTE

void wxDC::DoSelectPalette(bool realize)
{
}

void wxDC::SetPalette(const wxPalette& palette)
{
}

void wxDC::InitializePalette()
{
}

#endif // wxUSE_PALETTE

void wxDC::SetFont(const wxFont& font)
{
}

void wxDC::SetPen(const wxPen& pen)
{
}

void wxDC::SetBrush(const wxBrush& brush)
{
}

void wxDC::SetBackground(const wxBrush& brush)
{
}

void wxDC::SetBackgroundMode(int mode)
{
}

void wxDC::SetLogicalFunction(int function)
{
}

void wxDC::SetRop(WXHDC dc)
{
}

bool wxDC::StartDoc(const wxString& WXUNUSED(message))
{
    return true;
}

void wxDC::EndDoc()
{
}

void wxDC::StartPage()
{
}

void wxDC::EndPage()
{
}

// ---------------------------------------------------------------------------
// text metrics
// ---------------------------------------------------------------------------

wxCoord wxDC::GetCharHeight() const
{
    return 0;
}

wxCoord wxDC::GetCharWidth() const
{
    return 0;
}

void wxDC::DoGetTextExtent(const wxString& string, wxCoord *x, wxCoord *y,
                           wxCoord *descent, wxCoord *externalLeading,
                           const wxFont *font) const
{
}


bool wxDC::DoGetPartialTextExtents(const wxString& text, wxArrayInt& widths) const
{
    return false;
}




void wxDC::SetMapMode(int mode)
{
}

void wxDC::SetUserScale(double x, double y)
{
}

void wxDC::SetAxisOrientation(bool xLeftRight, bool yBottomUp)
{
}

void wxDC::SetSystemScale(double x, double y)
{
}

void wxDC::SetLogicalOrigin(wxCoord x, wxCoord y)
{
}

void wxDC::SetDeviceOrigin(wxCoord x, wxCoord y)
{
}

// ---------------------------------------------------------------------------
// coordinates transformations
// ---------------------------------------------------------------------------

wxCoord wxDC::DeviceToLogicalX(wxCoord x) const
{
    return 0;
}

wxCoord wxDC::DeviceToLogicalXRel(wxCoord x) const
{
    return 0;
}

wxCoord wxDC::DeviceToLogicalY(wxCoord y) const
{
    return 0;
}

wxCoord wxDC::DeviceToLogicalYRel(wxCoord y) const
{
    return 0;
}

wxCoord wxDC::LogicalToDeviceX(wxCoord x) const
{
    return 0;
}

wxCoord wxDC::LogicalToDeviceXRel(wxCoord x) const
{
    return 0;
}

wxCoord wxDC::LogicalToDeviceY(wxCoord y) const
{
    return 0;
}

wxCoord wxDC::LogicalToDeviceYRel(wxCoord y) const
{
    return 0;
}

// ---------------------------------------------------------------------------
// bit blit
// ---------------------------------------------------------------------------

bool wxDC::DoBlit(wxCoord xdest, wxCoord ydest,
                  wxCoord width, wxCoord height,
                  wxDC *source, wxCoord xsrc, wxCoord ysrc,
                  int rop, bool useMask,
                  wxCoord xsrcMask, wxCoord ysrcMask)
{
    return false;
}

void wxDC::DoGetSize(int *w, int *h) const
{
}

void wxDC::DoGetSizeMM(int *w, int *h) const
{
}

wxSize wxDC::GetPPI() const
{
    return wxSize(0, 0);
}

void wxDC::SetLogicalScale(double x, double y)
{
}

// ----------------------------------------------------------------------------
// DC caching
// ----------------------------------------------------------------------------

#if wxUSE_DC_CACHEING

wxList wxDC::sm_bitmapCache;
wxList wxDC::sm_dcCache;

wxDCCacheEntry::wxDCCacheEntry(WXHBITMAP hBitmap, int w, int h, int depth)
{
}

wxDCCacheEntry::wxDCCacheEntry(WXHDC hDC, int depth)
{
}

wxDCCacheEntry::~wxDCCacheEntry()
{
}

wxDCCacheEntry* wxDC::FindBitmapInCache(WXHDC dc, int w, int h)
{
    return NULL;
}

wxDCCacheEntry* wxDC::FindDCInCache(wxDCCacheEntry* notThis, WXHDC dc)
{
    return NULL;
}

void wxDC::AddToBitmapCache(wxDCCacheEntry* entry)
{
}

void wxDC::AddToDCCache(wxDCCacheEntry* entry)
{
}

void wxDC::ClearCache()
{
}

class wxDCModule : public wxModule
{
public:
    virtual bool OnInit() { return true; }
    virtual void OnExit() { wxDC::ClearCache(); }

private:
    DECLARE_DYNAMIC_CLASS(wxDCModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxDCModule, wxModule)

#endif // wxUSE_DC_CACHEING
