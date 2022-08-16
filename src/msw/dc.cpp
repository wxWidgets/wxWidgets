/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/dc.cpp
// Purpose:     wxDC class for MSW port
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
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


#ifndef WX_PRECOMP
    #include "wx/msw/wrapcdlg.h"
    #include "wx/image.h"
    #include "wx/window.h"
    #include "wx/utils.h"
    #include "wx/dialog.h"
    #include "wx/app.h"
    #include "wx/bitmap.h"
    #include "wx/dcmemory.h"
    #include "wx/log.h"
    #include "wx/math.h"
    #include "wx/icon.h"
    #include "wx/dcprint.h"
    #include "wx/module.h"
#endif

#include "wx/msw/dc.h"

#include "wx/scopedarray.h"
#include "wx/sysopt.h"

#ifdef wxHAS_RAW_BITMAP
    #include "wx/rawbmp.h"
#endif

#include <string.h>

#include "wx/msw/private/dc.h"
#include "wx/private/textmeasure.h"

#ifdef _MSC_VER
    // In the previous versions of wxWidgets, AlphaBlend() was dynamically
    // loaded from msimg32.dll during the run-time, so we didn't need to link
    // with this library. Now that we use the function statically, we do need
    // to link with it and we do it implicitly from here for MSVC users to
    // avoid breaking the build of the existing projects which didn't link with
    // this library before.
    #pragma comment(lib, "msimg32")

    // Indicate that we should just use the functions from gdi32.dll and
    // msimg32.dll directly.
    #define USE_STATIC_GDI_FUNCS
#else // !_MSC_VER
    // In other compilers, e.g. MinGW, we don't have anything similar to
    // #pragma comment(lib) used above, so we continue loading AlphaBlend()
    // dynamically, if possible.
    //
    // We also load some GDI functions not present in MinGW libraries
    // dynamically.
    #if wxUSE_DYNLIB_CLASS
        #include "wx/dynlib.h"

        #define USE_DYNAMIC_GDI_FUNCS
    #endif
#endif // _MSC_VER/!_MSC_VER

using namespace wxMSWImpl;

/* Quaternary raster codes */
#ifndef MAKEROP4
#define MAKEROP4(fore,back) (DWORD)((((back) << 8) & 0xFF000000) | (fore))
#endif

wxIMPLEMENT_ABSTRACT_CLASS(wxMSWDCImpl, wxDCImpl);

// ---------------------------------------------------------------------------
// constants
// ---------------------------------------------------------------------------

// The device space in Win32 GDI measures 2^27*2^27 , so we use 2^27-1 as the
// maximal possible view port extent.
static const int VIEWPORT_EXTENT = 134217727;

// ROPs which don't have standard names (see "Ternary Raster Operations" in the
// MSDN docs for how this and other numbers in wxDC::Blit() are obtained)
#define DSTCOPY 0x00AA0029      // a.k.a. NOP operation

// ----------------------------------------------------------------------------
// macros for logical <-> device coords conversion
// ----------------------------------------------------------------------------

/*
   We currently let Windows perform the scaling itself, as it might be more
   efficient than doing it in our own code, but we handle device origin
   translation ourselves as this allows us to use the full (at least 32-bit)
   range of wxCoord rather than being limited to 27-bit range of GDI functions.

   This means that instead of 2 coordinate systems -- physical and logical --
   we actually have 3 of them, as there is also a device coordinate system
   which is the same as logical one, but doesn't take the device origin into
   account (but does take into account the logical origin and scale). So this
   device coordinate system is simply shifted compared to the logical one.

   IOW:

        logical = (physical - deviceOrigin)/scale + logicalOrigin
        device  = physical/scale + logicalOrigin = logical + deviceOrigin/scale
 */

#define XLOG2DEV(x) ((x) + (m_deviceOriginX / m_scaleX))
#define YLOG2DEV(y) ((y) + (m_deviceOriginY / m_scaleY))
#define XDEV2LOG(x) ((x) - (m_deviceOriginX / m_scaleX))
#define YDEV2LOG(y) ((y) - (m_deviceOriginY / m_scaleY))

// ---------------------------------------------------------------------------
// private functions
// ---------------------------------------------------------------------------

#ifdef USE_DYNAMIC_GDI_FUNCS

namespace wxMSWImpl
{

// helper class to cache dynamically loaded libraries and not attempt reloading
// them if it fails
class wxOnceOnlyDLLLoader
{
public:
    // ctor argument must be a literal string as we don't make a copy of it!
    wxOnceOnlyDLLLoader(const wxChar *dllName)
        : m_dllName(dllName)
    {
    }

    // return the symbol with the given name or NULL if the DLL not loaded
    // or symbol not present
    void *GetSymbol(const wxChar *name)
    {
        // we're prepared to handle errors here
        wxLogNull noLog;

        if ( m_dllName )
        {
            m_dll.Load(m_dllName);

            // reset the name whether we succeeded or failed so that we don't
            // try again the next time
            m_dllName = NULL;
        }

        return m_dll.IsLoaded() ? m_dll.GetSymbol(name) : NULL;
    }

    void Unload()
    {
        if ( m_dll.IsLoaded() )
        {
            m_dll.Unload();
        }
    }

private:
    wxDynamicLibrary m_dll;
    const wxChar *m_dllName;
};

static wxOnceOnlyDLLLoader wxMSIMG32DLL(wxT("msimg32"));
static wxOnceOnlyDLLLoader wxGDI32DLL(wxT("gdi32.dll"));

// Note that originally these function pointers were local static members within
// functions, but we can't do that, because wxWidgets may be initialized,
// uninitialized, and reinitialized within the same program, and our dynamically
// loaded dll's may be unloaded and reloaded as part of that, almost certainly
// ending up at different base addresses due to address space layout
// randomization.
#ifdef USE_DYNAMIC_GDI_FUNCS
typedef BOOL (WINAPI *AlphaBlend_t)(HDC,int,int,int,int,
                                    HDC,int,int,int,int,
                                    BLENDFUNCTION);
static AlphaBlend_t gs_pfnAlphaBlend = NULL;
static bool gs_triedToLoadAlphaBlend = false;

typedef BOOL (WINAPI *GradientFill_t)(HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG);
static GradientFill_t gs_pfnGradientFill = NULL;
static bool gs_triedToLoadGradientFill = false;

typedef DWORD (WINAPI *GetLayout_t)(HDC);
static GetLayout_t gs_pfnGetLayout = NULL;
static bool gs_triedToLoadGetLayout = false;

typedef DWORD (WINAPI *SetLayout_t)(HDC, DWORD);
static SetLayout_t gs_pfnSetLayout = NULL;
static bool gs_triedToLoadSetLayout = false;
#endif // USE_DYNAMIC_GDI_FUNCS

// we must ensure that DLLs are unloaded before the static objects cleanup time
// because we may hit the notorious DllMain() dead lock in this case if wx is
// used as a DLL (attempting to unload another DLL from inside DllMain() hangs
// under Windows because it tries to reacquire the same lock)
class wxGDIDLLsCleanupModule : public wxModule
{
public:
    virtual bool OnInit() wxOVERRIDE { return true; }
    virtual void OnExit() wxOVERRIDE
    {
        wxMSIMG32DLL.Unload();
        wxGDI32DLL.Unload();
#ifdef USE_DYNAMIC_GDI_FUNCS
        gs_pfnGetLayout = NULL;
        gs_triedToLoadGetLayout = false;

        gs_pfnSetLayout = NULL;
        gs_triedToLoadSetLayout = false;

        gs_pfnAlphaBlend = NULL;
        gs_triedToLoadAlphaBlend = false;

        gs_pfnGradientFill = NULL;
        gs_triedToLoadGradientFill = false;
#endif // USE_DYNAMIC_GDI_FUNCS
    }

private:
    wxDECLARE_DYNAMIC_CLASS(wxGDIDLLsCleanupModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxGDIDLLsCleanupModule, wxModule);

} // namespace wxMSWImpl

#endif // USE_DYNAMIC_GDI_FUNCS

namespace wxDynLoadWrappers
{

#ifdef USE_DYNAMIC_GDI_FUNCS

// This is unfortunately necessary because GetLayout() is missing in MinGW
// libgdi32.a import library (at least up to w32api 4.0.3).
DWORD GetLayout(HDC hdc)
{
    if ( !gs_triedToLoadGetLayout )
    {
        gs_pfnGetLayout = (GetLayout_t)wxGDI32DLL.GetSymbol(wxT("GetLayout"));
        gs_triedToLoadGetLayout = true;
    }

    return gs_pfnGetLayout ? gs_pfnGetLayout(hdc) : GDI_ERROR;
}

// SetLayout is present in newer w32api versions but in older one (e.g. the one
// used by mingw32 4.2 Debian package), so load it dynamically too while we're
// at it.
DWORD SetLayout(HDC hdc, DWORD dwLayout)
{
    if ( !gs_triedToLoadSetLayout )
    {
        gs_pfnSetLayout = (SetLayout_t)wxGDI32DLL.GetSymbol(wxT("SetLayout"));
        gs_triedToLoadSetLayout = true;
    }

    return gs_pfnSetLayout ? gs_pfnSetLayout(hdc, dwLayout) : GDI_ERROR;
}

// AlphaBlend() requires linking with libmsimg32.a and we want to avoid this as
// it would break all the existing make/project files.
BOOL AlphaBlend(HDC hdcDest, int xDest, int yDest, int wDest, int hDest,
                HDC hdcSrc, int xSrc, int ySrc, int wSrc, int hSrc,
                BLENDFUNCTION bf)
{
    if ( !gs_triedToLoadAlphaBlend )
    {
        gs_pfnAlphaBlend = (AlphaBlend_t)wxMSIMG32DLL.GetSymbol(wxT("AlphaBlend"));
        gs_triedToLoadAlphaBlend = true;
    }

    if ( !gs_pfnAlphaBlend )
        return FALSE;

    return gs_pfnAlphaBlend(hdcDest, xDest, yDest, wDest, hDest,
                            hdcSrc, xSrc, ySrc, wSrc, hSrc,
                            bf);
}

// Just as AlphaBlend(), this one lives in msimg32.dll.
BOOL GradientFill(HDC hdc, PTRIVERTEX pVert, ULONG numVert,
                  PVOID pMesh, ULONG numMesh, ULONG mode)
{
    if ( !gs_triedToLoadGradientFill )
    {
        gs_pfnGradientFill = (GradientFill_t)wxMSIMG32DLL.GetSymbol(wxT("GradientFill"));
        gs_triedToLoadGradientFill = true;
    }

    if ( !gs_pfnGradientFill )
        return FALSE;

    return gs_pfnGradientFill(hdc, pVert, numVert, pMesh, numMesh, mode);
}

#elif defined(USE_STATIC_GDI_FUNCS)

DWORD GetLayout(HDC hdc)
{
    return ::GetLayout(hdc);
}

DWORD SetLayout(HDC hdc, DWORD dwLayout)
{
    return ::SetLayout(hdc, dwLayout);
}

BOOL AlphaBlend(HDC hdcDest, int xDest, int yDest, int wDest, int hDest,
                HDC hdcSrc, int xSrc, int ySrc, int wSrc, int hSrc,
                BLENDFUNCTION bf)
{
    return ::AlphaBlend(hdcDest, xDest, yDest, wDest, hDest,
                        hdcSrc, xSrc, ySrc, wSrc, hSrc,
                        bf);
}

BOOL GradientFill(HDC hdc, PTRIVERTEX pVert, ULONG numVert,
                  PVOID pMesh, ULONG numMesh, ULONG mode)
{
    return ::GradientFill(hdc, pVert, numVert, pMesh, numMesh, mode);
}

#else // Can't use the functions either statically or dynamically.

inline
DWORD GetLayout(HDC WXUNUSED(hdc))
{
    return GDI_ERROR;
}

inline
DWORD SetLayout(HDC WXUNUSED(hdc), DWORD WXUNUSED(dwLayout))
{
    return GDI_ERROR;
}

inline
BOOL AlphaBlend(HDC,int,int,int,int,
                HDC,int,int,int,int,
                BLENDFUNCTION)
{
    return FALSE;
}

inline
BOOL GradientFill(HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG)
{
    return FALSE;
}

#endif // USE_DYNAMIC_GDI_FUNCS/USE_STATIC_GDI_FUNCS

} // namespace wxDynLoadWrappers


// call AlphaBlend() to blit contents of hdcSrc to dcDst using alpha
//
// NB: bmpSrc is the bitmap selected in hdcSrc, it is not really needed
//     to pass it to this function but as we already have it at the point
//     of call anyhow we do
//
// return true if we could draw the bitmap in one way or the other, false
// otherwise
static bool AlphaBlt(wxMSWDCImpl* dcSrc,
                     int x, int y, int dstWidth, int dstHeight,
                     int srcX, int srcY,
                     int srcWidth, int srcHeight,
                     HDC hdcSrc,
                     const wxBitmap& bmpSrc);

namespace wxMSWImpl
{

// Create a compatible HDC and copy the layout of the source DC to it. This is
// necessary in order to draw bitmaps (which are usually blitted from a
// temporary compatible memory DC to the real target DC) using the same layout.
HDC CreateCompatibleDCWithLayout(HDC hdc);

} // namespace wxMSWImpl

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// instead of duplicating the same code which sets and then restores text
// colours in each wxDC method working with wxBRUSHSTYLE_STIPPLE_MASK_OPAQUE brushes,
// encapsulate this in a small helper class

// wxBrushAttrsSetter: changes the text colours in the ctor if required and
//                  restores them in the dtor
class wxBrushAttrsSetter : private wxBkModeChanger,
                           private wxTextColoursChanger
{
public:
    wxBrushAttrsSetter(wxMSWDCImpl& dc);

private:
    wxDECLARE_NO_COPY_CLASS(wxBrushAttrsSetter);
};

namespace
{

// this class sets the stretch blit mode to COLORONCOLOR during its lifetime
class StretchBltModeChanger
{
public:
    StretchBltModeChanger(HDC hdc)
        : m_hdc(hdc)
    {
        m_modeOld = ::SetStretchBltMode(m_hdc, COLORONCOLOR);
        if ( !m_modeOld )
        {
            wxLogLastError(wxT("SetStretchBltMode"));
        }
    }

    ~StretchBltModeChanger()
    {
        if ( !::SetStretchBltMode(m_hdc, m_modeOld) )
        {
            wxLogLastError(wxT("SetStretchBltMode"));
        }
    }

private:
    const HDC m_hdc;

    int m_modeOld;

    wxDECLARE_NO_COPY_CLASS(StretchBltModeChanger);
};

// This one changes polygon fill mode to the given one during its lifetime.
class PolyFillModeSetter
{
public:
    PolyFillModeSetter(HDC hdc, wxPolygonFillMode mode)
        : m_hdc(hdc)
    {
        m_modeOld = ::SetPolyFillMode(m_hdc, mode == wxODDEVEN_RULE ? ALTERNATE
                                                                    : WINDING);
    }

    ~PolyFillModeSetter()
    {
        ::SetPolyFillMode(m_hdc, m_modeOld);
    }

private:
    const HDC m_hdc;

    int m_modeOld;

    wxDECLARE_NO_COPY_CLASS(PolyFillModeSetter);
};

} // anonymous namespace

// ===========================================================================
// implementation
// ===========================================================================

// ----------------------------------------------------------------------------
// wxBrushAttrsSetter
// ----------------------------------------------------------------------------

wxBrushAttrsSetter::wxBrushAttrsSetter(wxMSWDCImpl& dc)
                  : wxBkModeChanger(GetHdcOf(dc)),
                    wxTextColoursChanger(GetHdcOf(dc))
{
    const wxBrush& brush = dc.GetBrush();
    if ( brush.IsOk() && brush.GetStyle() == wxBRUSHSTYLE_STIPPLE_MASK_OPAQUE )
    {
        // note that Windows convention is opposite to wxWidgets one, this is
        // why text colour becomes the background one and vice versa
        wxTextColoursChanger::Change(dc.GetTextBackground(),
                                     dc.GetTextForeground());

        wxBkModeChanger::Change(dc.GetBackgroundMode());
    }
}

// ----------------------------------------------------------------------------
// wxDC MSW-specific methods
// ----------------------------------------------------------------------------

WXHDC wxDC::GetHDC() const
{
    wxMSWDCImpl * const impl = wxDynamicCast(GetImpl(), wxMSWDCImpl);
    return impl ? impl->GetHDC() : 0;
}

// ---------------------------------------------------------------------------
// wxMSWDCImpl
// ---------------------------------------------------------------------------

void wxMSWDCImpl::InitWindow(wxWindow* window)
{
    m_window = window;
    if ( m_window )
        m_contentScaleFactor = m_window->GetDPIScaleFactor();
}

wxMSWDCImpl::wxMSWDCImpl( wxDC *owner, WXHDC hDC ) :
    wxDCImpl( owner )
{
    Init();
    m_hDC = hDC;
}

wxMSWDCImpl::~wxMSWDCImpl()
{
    if ( m_hDC != 0 )
    {
        SelectOldObjects(m_hDC);

        // if we own the HDC, we delete it, otherwise we just release it

        if ( m_bOwnsDC )
        {
            ::DeleteDC(GetHdc());
        }
        else // we don't own our HDC
        {
            if (m_window)
            {
                ::ReleaseDC(GetHwndOf(m_window), GetHdc());
            }
            else
            {
                // Must have been a wxScreenDC
                ::ReleaseDC((HWND) NULL, GetHdc());
            }
        }
    }
}

// This will select current objects out of the DC,
// which is what you have to do before deleting the
// DC.
void wxMSWDCImpl::SelectOldObjects(WXHDC dc)
{
    if (dc)
    {
        if (m_oldBitmap)
        {
            ::SelectObject((HDC) dc, (HBITMAP) m_oldBitmap);
            if (m_selectedBitmap.IsOk())
            {
                m_selectedBitmap.SetSelectedInto(NULL);
            }
        }
        m_oldBitmap = 0;
        if (m_oldPen)
        {
            ::SelectObject((HDC) dc, (HPEN) m_oldPen);
        }
        m_oldPen = 0;
        if (m_oldBrush)
        {
            ::SelectObject((HDC) dc, (HBRUSH) m_oldBrush);
        }
        m_oldBrush = 0;
        if (m_oldFont)
        {
            ::SelectObject((HDC) dc, (HFONT) m_oldFont);
        }
        m_oldFont = 0;

#if wxUSE_PALETTE
        if (m_oldPalette)
        {
            ::SelectPalette((HDC) dc, (HPALETTE) m_oldPalette, FALSE);
        }
        m_oldPalette = 0;
#endif // wxUSE_PALETTE
    }

    m_brush = wxNullBrush;
    m_pen = wxNullPen;
#if wxUSE_PALETTE
    m_palette = wxNullPalette;
#endif // wxUSE_PALETTE
    m_font = wxNullFont;
    m_backgroundBrush = wxNullBrush;
    m_selectedBitmap = wxNullBitmap;
}

// ---------------------------------------------------------------------------
// clipping
// ---------------------------------------------------------------------------

void wxMSWDCImpl::UpdateClipBox()
{
    RECT rect;
    ::GetClipBox(GetHdc(), &rect);

    // Don't shift by the device origin if the clipping box is empty.
    if ( rect.left == rect.right && rect.top == rect.bottom )
    {
        m_clipX1 =
        m_clipY1 =
        m_clipX2 =
        m_clipY2 = 0;
    }
    else
    {
        m_clipX1 = (wxCoord) XDEV2LOG(rect.left);
        m_clipY1 = (wxCoord) YDEV2LOG(rect.top);
        m_clipX2 = (wxCoord) XDEV2LOG(rect.right);
        m_clipY2 = (wxCoord) YDEV2LOG(rect.bottom);
    }

    m_isClipBoxValid = true;
}

bool wxMSWDCImpl::DoGetClippingRect(wxRect& rect) const
{
    // check if we should try to retrieve the clipping region possibly not set
    // by our SetClippingRegion() but preset or modified by Windows: this
    // can happen when we're associated with an existing HDC using SetHDC() or
    // when wxDC logical coordinates are transformed with SetDeviceOrigin(),
    // SetLogicalOrigin(), SetUserScale(), SetLogicalScale(),
    // SetTransformMatrix(), ResetTransformMatrix().
    if ( !m_isClipBoxValid )
    {
        wxMSWDCImpl *self = wxConstCast(this, wxMSWDCImpl);
        self->UpdateClipBox();
    }

    // Unfortunately we can't just call wxDCImpl::DoGetClippingRect() here
    // because it wouldn't return the correct result if there is no clipping
    // region and this DC has a world transform applied to it, as the base
    // class version doesn't know anything about world transforms and wouldn't
    // apply it to GetLogicalArea() that it returns in this case.
    //
    // We could solve this by overriding GetLogicalArea() in wxMSW and using
    // DPtoLP() to perform the conversion correctly ourselves, but it's even
    // simpler to just use the value returned by our UpdateClipBox() which is
    // already correct in any case, so just do this instead.
    rect = wxRect(m_clipX1,
                  m_clipY1,
                  m_clipX2 - m_clipX1,
                  m_clipY2 - m_clipY1);

    return m_clipping;
}

// common part of DoSetClippingRegion() and DoSetDeviceClippingRegion()
void wxMSWDCImpl::SetClippingHrgn(WXHRGN hrgn, bool doRtlOffset)
{
    wxCHECK_RET( hrgn, wxT("invalid clipping region") );

    HRGN hRgnRTL = NULL;
    // DC with enabled RTL layout needs a mirrored region
    // so we have to create such a region temporarily.
    if ( GetLayoutDirection() == wxLayout_RightToLeft )
    {
        DWORD bufLen = ::GetRegionData(hrgn, 0, NULL);  // Get the storage size
        wxScopedArray<char> pDataBuf(bufLen);
        RGNDATA* const rgndata = reinterpret_cast<RGNDATA*>(pDataBuf.get());
        if ( ::GetRegionData(hrgn, bufLen, rgndata) != bufLen )
        {
            wxLogLastError("GetRegionData");
            return;
        }
        int dcw, dch;
        DoGetSize(&dcw, &dch);
        XFORM tr;
        tr.eM11 = -1;
        tr.eM12 = 0;
        tr.eM21 = 0;
        tr.eM22 = 1;
        // For region created directly with device coordinates
        // (regions passed to DoSetDeviceClippingRegion) we have to
        // apply additional 1-pixel offset because original right edge
        // passed to e.g. CreateRectRgn() (in wxRegion) is actually
        // not included in the clipping area but this edge will become
        // a left edge after mirroring and therefore its x-coordinates
        // shoulde be adjusted.
        tr.eDx = doRtlOffset ? dcw : dcw-1; // max X
        tr.eDy = 0;
        hRgnRTL = ::ExtCreateRegion(&tr, bufLen, rgndata);
        if ( !hRgnRTL )
        {
            wxLogLastError("ExtCreateRegion");
            return;
        }
    }
    AutoHRGN rgnRTL(hRgnRTL);

    // note that we combine the new clipping region with the existing one: this
    // is compatible with what the other ports do and is the documented
    // behaviour now (starting with 2.3.3)
    if ( ::ExtSelectClipRgn(GetHdc(), (HRGN)rgnRTL ? (HRGN)rgnRTL : (HRGN)hrgn, RGN_AND) == ERROR )
    {
        wxLogLastError(wxT("ExtSelectClipRgn"));

        return;
    }

    m_clipping = true;

    UpdateClipBox();
}

void wxMSWDCImpl::DoSetClippingRegion(wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{
    // the region coords are always the device ones, so do the translation
    // manually
    //
    // FIXME: possible +/-1 error here, to check!
    // (x,y) has to represent the left-top corner of the region
    // so if size values are negative we need to recalculate
    // parameters of the region to get (x,y) at this corner.
    if ( w < 0 )
    {
        w = -w;
        x -= (w - 1);
    }
    if ( h < 0 )
    {
        h = -h;
        y -= (h - 1);
    }
    // Because world transform can be applied to HDC and its coordiante
    // system may be e.g. rotated we shouldn't assume that axis-aligned
    // clipping box in local coordinates will remain axis-aligned box in
    // device coordinates. Therefore we need to take into account all
    // 4 corners of the rectangle to create a polygonal clipping region
    // in device coordinates.
    POINT rect[4];
    wxPoint p = LogicalToDevice(x, y);
    rect[0].x = p.x;
    rect[0].y = p.y;
    p = LogicalToDevice(x + w, y);
    rect[1].x = p.x;
    rect[1].y = p.y;
    p = LogicalToDevice(x + w, y + h);
    rect[2].x = p.x;
    rect[2].y = p.y;
    p = LogicalToDevice(x, y + h);
    rect[3].x = p.x;
    rect[3].y = p.y;
    HRGN hrgn = ::CreatePolygonRgn(rect, WXSIZEOF(rect), WINDING);
    if ( !hrgn )
    {
        wxLogLastError(wxT("CreatePolygonRgn"));
    }
    else
    {
        SetClippingHrgn((WXHRGN)hrgn, false);

        ::DeleteObject(hrgn);
    }
}

void wxMSWDCImpl::DoSetDeviceClippingRegion(const wxRegion& region)
{
    SetClippingHrgn(region.GetHRGN(), true);
}

void wxMSWDCImpl::DestroyClippingRegion()
{
    if (m_clipping && m_hDC)
    {
#if 1
        // On a PocketPC device (not necessarily emulator), resetting
        // the clip region as per the old method causes bad display
        // problems. In fact setting a null region is probably OK
        // on desktop WIN32 also, since the WIN32 docs imply that the user
        // clipping region is independent from the paint clipping region.
        ::SelectClipRgn(GetHdc(), 0);
#else
        // TODO: this should restore the previous clipping region,
        //       so that OnPaint processing works correctly, and the update
        //       clipping region doesn't get destroyed after the first
        //       DestroyClippingRegion.
        HRGN rgn = CreateRectRgn(0, 0, 32000, 32000);
        ::SelectClipRgn(GetHdc(), rgn);
        ::DeleteObject(rgn);
#endif
    }

    wxDCImpl::DestroyClippingRegion();
    m_isClipBoxValid = false;
}

// ---------------------------------------------------------------------------
// query capabilities
// ---------------------------------------------------------------------------

bool wxMSWDCImpl::CanDrawBitmap() const
{
    return true;
}

bool wxMSWDCImpl::CanGetTextExtent() const
{
    // What sort of display is it?
    int technology = ::GetDeviceCaps(GetHdc(), TECHNOLOGY);

    return (technology == DT_RASDISPLAY) || (technology == DT_RASPRINTER);
}

int wxMSWDCImpl::GetDepth() const
{
    return (int)::GetDeviceCaps(GetHdc(), BITSPIXEL);
}

// ---------------------------------------------------------------------------
// drawing
// ---------------------------------------------------------------------------

void wxMSWDCImpl::Clear()
{
    if ( !m_window )
    {
        // No, I think we should simply ignore this if printing on e.g.
        // a printer DC.
        // wxCHECK_RET( m_selectedBitmap.IsOk(), wxT("this DC can't be cleared") );
        if (!m_selectedBitmap.IsOk())
            return;
    }

    HBRUSH hbr;
    if ( !m_backgroundBrush.IsOk() )
    {
        // By default, use the stock white brush for compatibility with the
        // previous wx versions.
        hbr = WHITE_BRUSH;
    }
    else if ( !m_backgroundBrush.IsTransparent() )
    {
        hbr = GetHbrushOf(m_backgroundBrush);
    }
    else // Using transparent background brush.
    {
        // Clearing with transparent brush doesn't do anything, just as drawing
        // with transparent pen doesn't.
        return;
    }

    RECT rect;
    ::GetClipBox(GetHdc(), &rect);
    // Inflate the box by 1 unit in each direction
    // to compensate rounding errors if DC is the subject
    // of complex transformation (is e.g. rotated).
    ::InflateRect(&rect, 1, 1);
    ::FillRect(GetHdc(), &rect, hbr);
}

bool wxMSWDCImpl::DoFloodFill(wxCoord x,
                       wxCoord y,
                       const wxColour& col,
                       wxFloodFillStyle style)
{
    bool success = (0 != ::ExtFloodFill(GetHdc(), XLOG2DEV(x), YLOG2DEV(y),
                         col.GetPixel(),
                         style == wxFLOOD_SURFACE ? FLOODFILLSURFACE
                                                  : FLOODFILLBORDER) ) ;
    if (!success)
    {
        // quoting from the MSDN docs:
        //
        //      Following are some of the reasons this function might fail:
        //
        //      * The filling could not be completed.
        //      * The specified point has the boundary color specified by the
        //        crColor parameter (if FLOODFILLBORDER was requested).
        //      * The specified point does not have the color specified by
        //        crColor (if FLOODFILLSURFACE was requested)
        //      * The point is outside the clipping region that is, it is not
        //        visible on the device.
        //
        wxLogLastError(wxT("ExtFloodFill"));
    }

    CalcBoundingBox(x, y);

    return success;
}

bool wxMSWDCImpl::DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const
{
    wxCHECK_MSG( col, false, wxT("NULL colour parameter in wxMSWDCImpl::GetPixel") );

    // get the color of the pixel
    COLORREF pixelcolor = ::GetPixel(GetHdc(), XLOG2DEV(x), YLOG2DEV(y));

    wxRGBToColour(*col, pixelcolor);

    return true;
}

// Check whether DC is not rotated and not scaled
static bool IsNonTransformedDC(HDC hdc)
{
    // Ensure DC is not rotated
    if ( ::GetGraphicsMode(hdc) == GM_ADVANCED )
        return false;
    // and not scaled
    SIZE devExt;
    ::GetViewportExtEx(hdc, &devExt);
    SIZE logExt;
    ::GetWindowExtEx(hdc, &logExt);
    return devExt.cx == logExt.cx && devExt.cy == logExt.cy;
}

void wxMSWDCImpl::DoCrossHair(wxCoord x, wxCoord y)
{
    RECT rect;
    ::GetClipBox(GetHdc(), &rect);
    // Inflate the box by 1 unit in each direction
    // to compensate rounding errors if DC is the subject
    // of complex transformation (is e.g. rotated).
    ::InflateRect(&rect, 1, 1);

    // We have optimized function to draw physical vertical or horizontal lines
    // with solid color and square ends so it can be used to draw a cross hair
    // for:
    // - Any shape of the lines ends because non-square line ends are drawn
    //   outside the view and visible line ends are always square.
    // - DC which coordinate system is not rotated (graphics mode
    //   of the DC != GM_ADVANCED) and not scaled.
    // - wxCOPY raster operation mode becaue it is based on ExtTextOut() API.
    if ( IsNonTransformedDC(GetHdc()) &&
         m_logicalFunction == wxCOPY &&
         m_pen.IsNonTransparent() && // this calls IsOk() too
         m_pen.GetStyle() == wxPENSTYLE_SOLID
       )
    {
        // Since we are drawing on a non-scaled DC, a 0-pixel width line
        // can be safely drawn as a 1-pixel wide one.
        int w = m_pen.GetWidth() > 0 ? m_pen.GetWidth() : 1;

        COLORREF color = wxColourToRGB(m_pen.GetColour());
        wxDrawHVLine(GetHdc(), XLOG2DEV(rect.left), YLOG2DEV(y), XLOG2DEV(rect.right), YLOG2DEV(y),
                     color, w);
        wxDrawHVLine(GetHdc(), XLOG2DEV(x), YLOG2DEV(rect.top), XLOG2DEV(x), YLOG2DEV(rect.bottom),
                     color, w);
    }
    else
    {
        wxDrawLine(GetHdc(), XLOG2DEV(rect.left), YLOG2DEV(y), XLOG2DEV(rect.right), YLOG2DEV(y));
        wxDrawLine(GetHdc(), XLOG2DEV(x), YLOG2DEV(rect.top), XLOG2DEV(x), YLOG2DEV(rect.bottom));
    }

    CalcBoundingBox(rect.left, rect.top, rect.right, rect.bottom);
}

void wxMSWDCImpl::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{
    // We have optimized function to draw physical vertical or horizontal lines
    // with solid color and square ends. It is based on ExtTextOut() API
    // so it can be used only with wxCOPY raster operation mode.
    // Because checking wheteher the line would be horizontal/vertical
    // in the device coordinate system is complex so we only check whether
    // the line is horizontal/vertical in the logical coordinates and use
    // optimized function only for DC which coordinate system is for sure
    // not rotated (graphics mode of the DC != GM_ADVANCED) and not scaled.
    if ( (x1 == x2 || y1 == y2) &&
         m_logicalFunction == wxCOPY &&
         IsNonTransformedDC(GetHdc()) &&
         m_pen.IsNonTransparent() && // this calls IsOk() too
         m_pen.GetStyle() == wxPENSTYLE_SOLID &&
         (m_pen.GetWidth() <= 1 || m_pen.GetCap() == wxCAP_BUTT)
       )
    {
        // Since we are drawing on a non-scaled DC, a 0-pixel width line
        // can be safely drawn as a 1-pixel wide one.
        wxDrawHVLine(GetHdc(), XLOG2DEV(x1), YLOG2DEV(y1), XLOG2DEV(x2), YLOG2DEV(y2),
                     wxColourToRGB(m_pen.GetColour()),
                     m_pen.GetWidth() > 0 ? m_pen.GetWidth() : 1);
    }
    else
    {
        wxDrawLine(GetHdc(), XLOG2DEV(x1), YLOG2DEV(y1), XLOG2DEV(x2), YLOG2DEV(y2));
    }

    CalcBoundingBox(x1, y1, x2, y2);
}

// Draws an arc of a circle, centred on (xc, yc), with starting point (x1, y1)
// and ending at (x2, y2)
void wxMSWDCImpl::DoDrawArc(wxCoord x1, wxCoord y1,
                     wxCoord x2, wxCoord y2,
                     wxCoord xc, wxCoord yc)
{
    double dx = xc - x1;
    double dy = yc - y1;
    wxCoord r = (wxCoord)sqrt(dx*dx + dy*dy);


    wxBrushAttrsSetter cc(*this); // needed for wxBRUSHSTYLE_STIPPLE_MASK_OPAQUE handling

    // treat the special case of full circle separately
    if ( x1 == x2 && y1 == y2 )
    {
        GetOwner()->DrawEllipse(xc - r, yc - r, 2*r, 2*r);
        return;
    }

    wxCoord xx1 = XLOG2DEV(x1);
    wxCoord yy1 = YLOG2DEV(y1);
    wxCoord xx2 = XLOG2DEV(x2);
    wxCoord yy2 = YLOG2DEV(y2);
    wxCoord xxc = XLOG2DEV(xc);
    wxCoord yyc = YLOG2DEV(yc);
    dx = xxc - xx1;
    dy = yyc - yy1;
    wxCoord ray = (wxCoord)sqrt(dx*dx + dy*dy);

    wxCoord xxx1 = (wxCoord) (xxc-ray);
    wxCoord yyy1 = (wxCoord) (yyc-ray);
    wxCoord xxx2 = (wxCoord) (xxc+ray);
    wxCoord yyy2 = (wxCoord) (yyc+ray);

    if ( m_brush.IsNonTransparent() )
    {
        // Have to add 1 to bottom-right corner of rectangle
        // to make semi-circles look right (crooked line otherwise).
        // Unfortunately this is not a reliable method, depends
        // on the size of shape.
        // TODO: figure out why this happens!
        Pie(GetHdc(),xxx1,yyy1,xxx2+1,yyy2+1, xx1,yy1,xx2,yy2);
    }
    else
    {
        Arc(GetHdc(),xxx1,yyy1,xxx2,yyy2, xx1,yy1,xx2,yy2);
    }

    CalcBoundingBox(xc - r, yc - r, xc + r, yc + r);
}

void wxMSWDCImpl::DoDrawPoint(wxCoord x, wxCoord y)
{
    COLORREF color = 0x00ffffff;
    if (m_pen.IsOk())
    {
        color = m_pen.GetColour().GetPixel();
    }

    SetPixel(GetHdc(), XLOG2DEV(x), YLOG2DEV(y), color);

    CalcBoundingBox(x, y);
}

void wxMSWDCImpl::DoDrawPolygon(int n,
                         const wxPoint points[],
                         wxCoord xoffset,
                         wxCoord yoffset,
                         wxPolygonFillMode fillStyle)
{
    wxBrushAttrsSetter cc(*this); // needed for wxBRUSHSTYLE_STIPPLE_MASK_OPAQUE handling

    PolyFillModeSetter polyfillModeSetter(GetHdc(), fillStyle);

    // Do things less efficiently if we have offsets or need to account for the
    // device origin offset
    if (xoffset != 0 || yoffset != 0 || m_deviceOriginX != 0 || m_deviceOriginY != 0)
    {
        wxScopedArray<POINT> cpoints(n);
        int i;
        for (i = 0; i < n; i++)
        {
            // First adjust the logical coordinates to include the offset.
            cpoints[i].x = points[i].x + xoffset;
            cpoints[i].y = points[i].y + yoffset;

            CalcBoundingBox(cpoints[i].x, cpoints[i].y);

            // Now convert them to the device coordinates that we need to use.
            cpoints[i].x += XLOG2DEV(0);
            cpoints[i].y += YLOG2DEV(0);
        }
        (void)Polygon(GetHdc(), cpoints.get(), n);
    }
    else
    {
        int i;
        for (i = 0; i < n; i++)
            CalcBoundingBox(points[i].x, points[i].y);

        Polygon(GetHdc(), reinterpret_cast<const POINT*>(points), n);
    }
}

void
wxMSWDCImpl::DoDrawPolyPolygon(int n,
                        const int count[],
                        const wxPoint points[],
                        wxCoord xoffset,
                        wxCoord yoffset,
                        wxPolygonFillMode fillStyle)
{
    wxBrushAttrsSetter cc(*this); // needed for wxBRUSHSTYLE_STIPPLE_MASK_OPAQUE handling
    int i, cnt;
    for (i = cnt = 0; i < n; i++)
        cnt += count[i];

    PolyFillModeSetter polyfillModeSetter(GetHdc(), fillStyle);

    // The code here is similar to the code in DoDrawPolygon() above.
    if (xoffset != 0 || yoffset != 0 || m_deviceOriginX != 0 || m_deviceOriginY != 0)
    {
        wxScopedArray<POINT> cpoints(cnt);
        for (i = 0; i < cnt; i++)
        {
            cpoints[i].x = points[i].x + xoffset;
            cpoints[i].y = points[i].y + yoffset;

            CalcBoundingBox(cpoints[i].x, cpoints[i].y);

            cpoints[i].x += XLOG2DEV(0);
            cpoints[i].y += YLOG2DEV(0);
        }
        (void)PolyPolygon(GetHdc(), cpoints.get(), count, n);
    }
    else
    {
        for (i = 0; i < cnt; i++)
            CalcBoundingBox(points[i].x, points[i].y);

        PolyPolygon(GetHdc(), reinterpret_cast<const POINT*>(points), count, n);
    }
}

void wxMSWDCImpl::DoDrawLines(int n, const wxPoint points[], wxCoord xoffset, wxCoord yoffset)
{
    // The logic here is similar to that of DoDrawPolygon() above.
    if (xoffset != 0 || yoffset != 0 || m_deviceOriginX != 0 || m_deviceOriginY != 0)
    {
        wxScopedArray<POINT> cpoints(n);
        int i;
        for (i = 0; i < n; i++)
        {
            cpoints[i].x = (int)(points[i].x + xoffset);
            cpoints[i].y = (int)(points[i].y + yoffset);

            CalcBoundingBox(cpoints[i].x, cpoints[i].y);

            cpoints[i].x += XLOG2DEV(0);
            cpoints[i].y += YLOG2DEV(0);
        }
        (void)Polyline(GetHdc(), cpoints.get(), n);
    }
    else
    {
        int i;
        for (i = 0; i < n; i++)
            CalcBoundingBox(points[i].x, points[i].y);

        Polyline(GetHdc(), reinterpret_cast<const POINT*>(points), n);
    }
}

void wxMSWDCImpl::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    wxBrushAttrsSetter cc(*this); // needed for wxBRUSHSTYLE_STIPPLE_MASK_OPAQUE handling

    wxCoord x2 = x + width;
    wxCoord y2 = y + height;

    wxCoord x1dev = XLOG2DEV(x),
            y1dev = YLOG2DEV(y),
            x2dev = XLOG2DEV(x2),
            y2dev = YLOG2DEV(y2);

    // Windows draws the filled rectangles without outline
    // (i.e. drawn with a transparent pen) one pixel smaller in both directions
    // and we want them to have the same size regardless of which pen is used
    if ( m_pen.IsTransparent() )
    {
        // Right edge to be extended is "displayed right edge"
        // and hence its device coordinates depend
        // on layout direction and can be either x1 or x2.
        if ( GetLayoutDirection() == wxLayout_RightToLeft )
            x1dev--;
        else
            x2dev++;
        y2dev++;
    }

    (void)Rectangle(GetHdc(), x1dev, y1dev, x2dev, y2dev);

    CalcBoundingBox(x, y, x2, y2);
}

void wxMSWDCImpl::DoDrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius)
{
    wxBrushAttrsSetter cc(*this); // needed for wxBRUSHSTYLE_STIPPLE_MASK_OPAQUE handling

    // Now, a negative radius value is interpreted to mean
    // 'the proportion of the smallest X or Y dimension'

    if (radius < 0.0)
    {
        double smallest = (width < height) ? width : height;
        radius = (- radius * smallest);
    }

    wxCoord x2 = (x+width);
    wxCoord y2 = (y+height);

    // Windows draws the filled rectangles without outline (i.e. drawn with a
    // transparent pen) one pixel smaller in both directions and we want them
    // to have the same size regardless of which pen is used - adjust
    if ( m_pen.IsTransparent() )
    {
        x2++;
        y2++;
    }

    (void)RoundRect(GetHdc(), XLOG2DEV(x), YLOG2DEV(y), XLOG2DEV(x2),
        YLOG2DEV(y2), (int) (2*radius), (int)( 2*radius));

    CalcBoundingBox(x, y, x2, y2);
}

void wxMSWDCImpl::DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    wxBrushAttrsSetter cc(*this); // needed for wxBRUSHSTYLE_STIPPLE_MASK_OPAQUE handling

    // +1 below makes the ellipse more similar to other platforms.
    // In particular, DoDrawEllipse(x,y,1,1) should draw one point.
    wxCoord x2 = x + width + 1;
    wxCoord y2 = y + height + 1;

    // Problem: Windows GDI Ellipse() with x2-x == y2-y == 3 and transparent
    // pen doesn't draw anything. Should we provide a workaround?

    ::Ellipse(GetHdc(), XLOG2DEV(x), YLOG2DEV(y), XLOG2DEV(x2), YLOG2DEV(y2));

    CalcBoundingBox(x, y, x2, y2);
}

#if wxUSE_SPLINES
void wxMSWDCImpl::DoDrawSpline(const wxPointList *points)
{
    // quadratic b-spline to cubic bezier spline conversion
    //
    // quadratic spline with control points P0,P1,P2
    // P(s) = P0*(1-s)^2 + P1*2*(1-s)*s + P2*s^2
    //
    // bezier spline with control points B0,B1,B2,B3
    // B(s) = B0*(1-s)^3 + B1*3*(1-s)^2*s + B2*3*(1-s)*s^2 + B3*s^3
    //
    // control points of bezier spline calculated from b-spline
    // B0 = P0
    // B1 = (2*P1 + P0)/3
    // B2 = (2*P1 + P2)/3
    // B3 = P2

    wxCHECK_RET( points, "NULL pointer to spline points?" );

    const size_t n_points = points->size();
    wxCHECK_RET( n_points >= 2 , "incomplete list of spline points?" );

    const size_t n_bezier_points = n_points * 3 + 1;
    wxScopedArray<POINT> lppt(n_bezier_points);
    size_t bezier_pos = 0;
    wxCoord x1, y1, x2, y2, cx1, cy1;

    wxPointList::const_iterator itPt = points->begin();
    wxPoint* p = *itPt; ++itPt;
    x1 = p->x;
    y1 = p->y;
    lppt[ bezier_pos ].x = XLOG2DEV(x1);
    lppt[ bezier_pos ].y = YLOG2DEV(y1);
    bezier_pos++;
    lppt[ bezier_pos ] = lppt[ bezier_pos-1 ];
    bezier_pos++;
    CalcBoundingBox(x1, y1);

    p = *itPt; ++itPt;
    x2 = p->x;
    y2 = p->y;
    cx1 = ( x1 + x2 ) / 2;
    cy1 = ( y1 + y2 ) / 2;
    lppt[ bezier_pos ].x = XLOG2DEV(cx1);
    lppt[ bezier_pos ].y = YLOG2DEV(cy1);
    bezier_pos++;
    lppt[ bezier_pos ] = lppt[ bezier_pos-1 ];
    bezier_pos++;
    CalcBoundingBox(x2, y2);

    while ( itPt != points->end() )
    {
        p = *itPt; ++itPt;
        x1 = x2;
        y1 = y2;
        x2 = p->x;
        y2 = p->y;
        int cx4 = (x1 + x2) / 2;
        int cy4 = (y1 + y2) / 2;
        // B0 is B3 of previous segment
        // B1:
        lppt[ bezier_pos ].x = XLOG2DEV((x1*2+cx1)/3);
        lppt[ bezier_pos ].y = YLOG2DEV((y1*2+cy1)/3);
        bezier_pos++;
        // B2:
        lppt[ bezier_pos ].x = XLOG2DEV((x1*2+cx4)/3);
        lppt[ bezier_pos ].y = YLOG2DEV((y1*2+cy4)/3);
        bezier_pos++;
        // B3:
        lppt[ bezier_pos ].x = XLOG2DEV(cx4);
        lppt[ bezier_pos ].y = YLOG2DEV(cy4);
        bezier_pos++;
        cx1 = cx4;
        cy1 = cy4;

        CalcBoundingBox(x2, y2);
    }

    lppt[ bezier_pos ] = lppt[ bezier_pos-1 ];
    bezier_pos++;
    lppt[ bezier_pos ].x = XLOG2DEV(x2);
    lppt[ bezier_pos ].y = YLOG2DEV(y2);
    bezier_pos++;
    lppt[ bezier_pos ] = lppt[ bezier_pos-1 ];
    bezier_pos++;

    ::PolyBezier( GetHdc(), lppt.get(), bezier_pos );
}
#endif // wxUSE_SPLINES

// Chris Breeze 20/5/98: first implementation of DrawEllipticArc on Windows
void wxMSWDCImpl::DoDrawEllipticArc(wxCoord x,wxCoord y,wxCoord w,wxCoord h,double sa,double ea)
{
    wxBrushAttrsSetter cc(*this); // needed for wxBRUSHSTYLE_STIPPLE_MASK_OPAQUE handling

    wxCoord x2 = x + w;
    wxCoord y2 = y + h;

    int rx1 = XLOG2DEV(x+w/2);
    int ry1 = YLOG2DEV(y+h/2);
    int rx2 = rx1;
    int ry2 = ry1;

    sa = wxDegToRad(sa);
    ea = wxDegToRad(ea);

    rx1 += (int)(100.0 * abs(w) * cos(sa));
    ry1 -= (int)(100.0 * abs(h) * m_signY * sin(sa));
    rx2 += (int)(100.0 * abs(w) * cos(ea));
    ry2 -= (int)(100.0 * abs(h) * m_signY * sin(ea));

    // draw pie with NULL_PEN first and then outline otherwise a line is
    // drawn from the start and end points to the centre
    HPEN hpenOld = (HPEN) ::SelectObject(GetHdc(), (HPEN) ::GetStockObject(NULL_PEN));
    if (m_signY > 0)
    {
        (void)Pie(GetHdc(), XLOG2DEV(x), YLOG2DEV(y), XLOG2DEV(x2)+1, YLOG2DEV(y2)+1,
                  rx1, ry1, rx2, ry2);
    }
    else
    {
        (void)Pie(GetHdc(), XLOG2DEV(x), YLOG2DEV(y)-1, XLOG2DEV(x2)+1, YLOG2DEV(y2),
                  rx1, ry1-1, rx2, ry2-1);
    }

    ::SelectObject(GetHdc(), hpenOld);

    (void)Arc(GetHdc(), XLOG2DEV(x), YLOG2DEV(y), XLOG2DEV(x2), YLOG2DEV(y2),
              rx1, ry1, rx2, ry2);

    CalcBoundingBox(x, y, x2, y2);
}

void wxMSWDCImpl::DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y)
{
    wxCHECK_RET( icon.IsOk(), wxT("invalid icon in DrawIcon") );

    // Check if we are printing: notice that it's not enough to just check for
    // DT_RASPRINTER as this is also the kind of print preview HDC, but its
    // type is OBJ_ENHMETADC while the type of the "real" printer DC is OBJ_DC.
    if ( ::GetDeviceCaps(GetHdc(), TECHNOLOGY) == DT_RASPRINTER &&
            ::GetObjectType(GetHdc()) == OBJ_DC )
    {
        // DrawIcon API doesn't work for printer DCs (printer DC is write-only
        // and DrawIcon requires DC supporting SRCINVERT ROP).
        // We need to convert icon to bitmap and use alternative API calls.
        wxBitmap bmp(icon);
        DoDrawBitmap(bmp, x, y, !bmp.HasAlpha() /* use mask */);
    }
    else
    {
        ::DrawIconEx(GetHdc(), XLOG2DEV(x), YLOG2DEV(y), GetHiconOf(icon), icon.GetWidth(), icon.GetHeight(), 0, NULL, DI_NORMAL);
    }

    CalcBoundingBox(wxPoint(x, y), icon.GetSize());
}

void wxMSWDCImpl::DoDrawBitmap( const wxBitmap &bmp, wxCoord x, wxCoord y, bool useMask )
{
    wxCHECK_RET( bmp.IsOk(), wxT("invalid bitmap in wxMSWDCImpl::DrawBitmap") );

    int width = bmp.GetWidth(),
        height = bmp.GetHeight();

    HBITMAP hbmpMask = 0;

#if wxUSE_PALETTE
    HPALETTE oldPal = 0;
#endif // wxUSE_PALETTE

    if ( bmp.HasAlpha() )
    {
        // Make a copy in case we would neeed to remove its mask.
        // If this will not be necessary, the copy is cheap as bitmaps are reference-counted.
        wxBitmap curBmp(bmp);

        // For bitmap with both alpha channel and mask we have to apply mask on our own
        // because MaskBlt() API doesn't work properly with 32 bpp RGBA bitmaps.
        // To do so we will create a temporary bitmap with copy of RGB data and with alpha channel
        // being a superposition of the original alpha values and the mask - for non-masked pixels
        // alpha channel values will remain intact and for masked pixels they will be set to the transparent value.
        if ( curBmp.GetMask() )
        {
            if ( useMask )
            {
                curBmp.MSWBlendMaskWithAlpha();
            }
            else
            {
                curBmp.SetMask(NULL);
            }
        }

        MemoryHDC hdcMem;
        SelectInHDC select(hdcMem, GetHbitmapOf(curBmp));

        if ( AlphaBlt(this, XLOG2DEV(x), YLOG2DEV(y), width, height,
                            0, 0, width, height, hdcMem, curBmp) )
        {
            CalcBoundingBox(wxPoint(x, y), bmp.GetSize());
            return;
        }
    }

    StretchBltModeChanger stretchModeChanger(GetHdc());

    if ( useMask )
    {
        wxMask *mask = bmp.GetMask();
        if ( mask )
            hbmpMask = (HBITMAP)mask->GetMaskBitmap();

        if ( !hbmpMask )
        {
            // don't give assert here because this would break existing
            // programs - just silently ignore useMask parameter
            useMask = false;
        }
    }
    if ( useMask )
    {
        // use MaskBlt() with ROP which doesn't do anything to dst in the mask
        // points
        bool ok = false;

#if wxUSE_SYSTEM_OPTIONS
        // On some systems, MaskBlt succeeds yet is much much slower
        // than the wxWidgets fall-back implementation. So we need
        // to be able to switch this on and off at runtime.
        //
        // NB: don't query the value of the option every time but do it only
        //     once as otherwise it can have real (and bad) performance
        //     implications (see #11172)
        static bool
            s_maskBltAllowed = wxSystemOptions::GetOptionInt("no-maskblt") == 0;
        if ( s_maskBltAllowed )
#endif // wxUSE_SYSTEM_OPTIONS
        {
            HDC cdc = GetHdc();
            HDC hdcMem = wxMSWImpl::CreateCompatibleDCWithLayout(cdc);
            HGDIOBJ hOldBitmap = ::SelectObject(hdcMem, GetHbitmapOf(bmp));
#if wxUSE_PALETTE
            wxPalette *pal = bmp.GetPalette();
            if ( pal && ::GetDeviceCaps(cdc,BITSPIXEL) <= 8 )
            {
                oldPal = ::SelectPalette(hdcMem, GetHpaletteOf(*pal), FALSE);
                ::RealizePalette(hdcMem);
            }
#endif // wxUSE_PALETTE

            ok = ::MaskBlt(cdc, XLOG2DEV(x), YLOG2DEV(y), width, height,
                            hdcMem, 0, 0,
                            hbmpMask, 0, 0,
                            MAKEROP4(SRCCOPY, DSTCOPY)) != 0;

#if wxUSE_PALETTE
            if (oldPal)
                ::SelectPalette(hdcMem, oldPal, FALSE);
#endif // wxUSE_PALETTE

            ::SelectObject(hdcMem, hOldBitmap);
            ::DeleteDC(hdcMem);
        }

        if ( !ok )
        {
            // Rather than reproduce wxMSWDCImpl::Blit, let's do it at the wxWin API
            // level
            wxMemoryDC memDC;

            memDC.SetLayoutDirection(GetLayoutDirection());
            memDC.SelectObjectAsSource(bmp);

            GetOwner()->Blit(x, y, width, height, &memDC, 0, 0, wxCOPY, useMask);
        }
    }
    else // no mask, just use BitBlt()
    {
        HDC cdc = GetHdc();
        HDC memdc = wxMSWImpl::CreateCompatibleDCWithLayout( cdc );
        HBITMAP hbitmap = (HBITMAP) bmp.GetHBITMAP( );

        wxASSERT_MSG( hbitmap, wxT("bitmap is ok but HBITMAP is NULL?") );

        wxTextColoursChanger textCol(GetHdc(), *this);

#if wxUSE_PALETTE
        wxPalette *pal = bmp.GetPalette();
        if ( pal && ::GetDeviceCaps(cdc,BITSPIXEL) <= 8 )
        {
            oldPal = ::SelectPalette(memdc, GetHpaletteOf(*pal), FALSE);
            ::RealizePalette(memdc);
        }
#endif // wxUSE_PALETTE

        HGDIOBJ hOldBitmap = ::SelectObject( memdc, hbitmap );
        ::BitBlt(cdc, XLOG2DEV(x), YLOG2DEV(y), width, height,
                 memdc, 0, 0, SRCCOPY);

#if wxUSE_PALETTE
        if (oldPal)
            ::SelectPalette(memdc, oldPal, FALSE);
#endif // wxUSE_PALETTE

        ::SelectObject( memdc, hOldBitmap );
        ::DeleteDC( memdc );
    }

    CalcBoundingBox(wxPoint(x, y), bmp.GetSize());
}

void wxMSWDCImpl::DoDrawText(const wxString& text, wxCoord x, wxCoord y)
{
    // For compatibility with other ports (notably wxGTK) and because it's
    // genuinely useful, we allow passing multiline strings to DrawText().
    // However there is no native MSW function to draw them directly so we
    // instead reuse the generic DrawLabel() method to render them. Of course,
    // DrawLabel() itself will call back to us but with single line strings
    // only so there won't be any infinite recursion here.
    if ( text.find('\n') != wxString::npos )
    {
        GetOwner()->DrawLabel(text, wxRect(x, y, 0, 0));
        return;
    }

    // prepare for drawing the text
    wxTextColoursChanger textCol(GetHdc(), *this);

    wxBkModeChanger bkMode(GetHdc(), m_backgroundMode);

    DrawAnyText(text, x, y);

    // update the bounding box
    CalcBoundingBox(wxPoint(x, y), GetOwner()->GetTextExtent(text));
}

void wxMSWDCImpl::DrawAnyText(const wxString& text, wxCoord x, wxCoord y)
{
    if ( ::ExtTextOut(GetHdc(), XLOG2DEV(x), YLOG2DEV(y), 0, NULL,
                   text.c_str(), text.length(), NULL) == 0 )
    {
        wxLogLastError(wxT("TextOut"));
    }
}

void wxMSWDCImpl::DoDrawRotatedText(const wxString& text,
                             wxCoord x, wxCoord y,
                             double angle)
{
    // we test that we have some font because otherwise we should still use the
    // "else" part below to avoid that DrawRotatedText(angle = 180) and
    // DrawRotatedText(angle = 0) use different fonts (we can't use the default
    // font for drawing rotated fonts unfortunately)
    if ( (angle == 0.0) && m_font.IsOk() )
    {
        DoDrawText(text, x, y);

        // Bounding box already updated by DoDrawText(), no need to do it again.
        return;
    }

    // NB: don't take DEFAULT_GUI_FONT (a.k.a. wxSYS_DEFAULT_GUI_FONT)
    //     because it's not TrueType and so can't have non zero
    //     orientation/escapement
    wxFont font;
    if ( m_font.IsOk() )
    {
        font = m_font;
    }
    else // Use default font appropriate for rotated text.
    {
        font = *wxSWISS_FONT;
        if ( m_window )
            font.WXAdjustToPPI(m_window->GetDPI());
    }

    LOGFONT lf;
    if ( ::GetObject(GetHfontOf(font), sizeof(lf), &lf) == 0 )
    {
        wxLogLastError(wxT("GetObject(hfont)"));
    }

    // GDI wants the angle in tenth of degree
    long angle10 = (long)(angle * 10);
    lf.lfEscapement = angle10;
    lf.lfOrientation = angle10;

    AutoHFONT hfont(lf);
    if ( !hfont )
    {
        wxLogLastError(wxT("CreateFont"));
        return;
    }

    SelectInHDC selRotatedFont(GetHdc(), hfont);

    // Get extent of whole text.
    wxCoord w, h, heightLine;
    GetOwner()->GetMultiLineTextExtent(text, &w, &h, &heightLine);

    // Prepare for drawing the text
    wxTextColoursChanger textCol(GetHdc(), *this);
    wxBkModeChanger bkMode(GetHdc(), m_backgroundMode);

    // Compute the shift for the origin of the next line.
    const double rad = wxDegToRad(angle);
    const double dx = heightLine * sin(rad);
    const double dy = heightLine * cos(rad);

    // Draw all text line by line
    const wxArrayString lines = wxSplit(text, '\n', '\0');
    for ( size_t lineNum = 0; lineNum < lines.size(); lineNum++ )
    {
        // Calculate origin for each line to avoid accumulation of
        // rounding errors.
        DrawAnyText(lines[lineNum],
                    x + wxRound(lineNum*dx),
                    y + wxRound(lineNum*dy));
    }


    // call the bounding box by adding all four vertices of the rectangle
    // containing the text to it (simpler and probably not slower than
    // determining which of them is really topmost/leftmost/...)

    // "upper left" and "upper right"
    CalcBoundingBox(x, y, x + wxCoord(w*cos(rad)), y - wxCoord(w*sin(rad)));

    // "bottom left" and "bottom right"
    x += (wxCoord)(h*sin(rad));
    y += (wxCoord)(h*cos(rad));
    CalcBoundingBox(x, y, x + wxCoord(w*cos(rad)), y - wxCoord(w*sin(rad)));
}

// ---------------------------------------------------------------------------
// set GDI objects
// ---------------------------------------------------------------------------

#if wxUSE_PALETTE

void wxMSWDCImpl::DoSelectPalette(bool realize)
{
    // Set the old object temporarily, in case the assignment deletes an object
    // that's not yet selected out.
    if (m_oldPalette)
    {
        ::SelectPalette(GetHdc(), (HPALETTE) m_oldPalette, FALSE);
        m_oldPalette = 0;
    }

    if ( m_palette.IsOk() )
    {
        HPALETTE oldPal = ::SelectPalette(GetHdc(),
                                          GetHpaletteOf(m_palette),
                                          false);
        if (!m_oldPalette)
            m_oldPalette = (WXHPALETTE) oldPal;

        if (realize)
            ::RealizePalette(GetHdc());
    }
}

void wxMSWDCImpl::SetPalette(const wxPalette& palette)
{
    if ( palette.IsOk() )
    {
        m_palette = palette;
        DoSelectPalette(true);
    }
}

void wxMSWDCImpl::InitializePalette()
{
    if ( wxDisplayDepth() <= 8 )
    {
        // look for any window or parent that has a custom palette. If any has
        // one then we need to use it in drawing operations
        wxWindow *win = m_window->GetAncestorWithCustomPalette();

        m_hasCustomPalette = win && win->HasCustomPalette();
        if ( m_hasCustomPalette )
        {
            m_palette = win->GetPalette();

            // turn on MSW translation for this palette
            DoSelectPalette();
        }
    }
}

#endif // wxUSE_PALETTE

void wxMSWDCImpl::SetFont(const wxFont& font)
{
    if ( font == m_font )
        return;

    if ( font.IsOk() )
    {
        wxFont f(font);
        if ( m_window )
            f.WXAdjustToPPI(m_window->GetDPI());

        HGDIOBJ hfont = ::SelectObject(GetHdc(), GetHfontOf(f));
        if ( hfont == HGDI_ERROR )
        {
            wxLogLastError(wxT("SelectObject(font)"));
        }
        else // selected ok
        {
            if ( !m_oldFont )
                m_oldFont = (WXHFONT)hfont;

            m_font = f;
        }
    }
    else // invalid font, reset the current font
    {
        if ( m_oldFont )
        {
            if ( ::SelectObject(GetHdc(), (HPEN) m_oldFont) == HGDI_ERROR )
            {
                wxLogLastError(wxT("SelectObject(old font)"));
            }

            m_oldFont = 0;
        }

        m_font = wxNullFont;
    }
}

void wxMSWDCImpl::SetPen(const wxPen& pen)
{
    if ( pen == m_pen )
        return;

    if ( pen.IsOk() )
    {
        HGDIOBJ hpen = ::SelectObject(GetHdc(), GetHpenOf(pen));
        if ( hpen == HGDI_ERROR )
        {
            wxLogLastError(wxT("SelectObject(pen)"));
        }
        else // selected ok
        {
            if ( !m_oldPen )
                m_oldPen = (WXHPEN)hpen;

            m_pen = pen;
        }
    }
    else // invalid pen, reset the current pen
    {
        if ( m_oldPen )
        {
            if ( ::SelectObject(GetHdc(), (HPEN) m_oldPen) == HGDI_ERROR )
            {
                wxLogLastError(wxT("SelectObject(old pen)"));
            }

            m_oldPen = 0;
        }

        m_pen = wxNullPen;
    }
}

void wxMSWDCImpl::SetBrush(const wxBrush& brush)
{
    if ( brush == m_brush )
        return;

    if ( brush.IsOk() )
    {
        // we must make sure the brush is aligned with the logical coordinates
        // before selecting it or using the same brush for the background of
        // different windows would result in discontinuities
        wxSize sizeBrushBitmap = wxDefaultSize;
        wxBitmap *stipple = brush.GetStipple();
        if ( stipple && stipple->IsOk() )
            sizeBrushBitmap = stipple->GetSize();
        else if ( brush.IsHatch() )
            sizeBrushBitmap = wxSize(8, 8);

        if ( sizeBrushBitmap.IsFullySpecified() )
        {
            if ( !::SetBrushOrgEx
                    (
                        GetHdc(),
                        m_deviceOriginX % sizeBrushBitmap.x,
                        m_deviceOriginY % sizeBrushBitmap.y,
                        NULL                    // [out] previous brush origin
                    ) )
            {
                wxLogLastError(wxT("SetBrushOrgEx()"));
            }
        }

        HGDIOBJ hbrush = ::SelectObject(GetHdc(), GetHbrushOf(brush));
        if ( hbrush == HGDI_ERROR )
        {
            wxLogLastError(wxT("SelectObject(brush)"));
        }
        else // selected ok
        {
            if ( !m_oldBrush )
                m_oldBrush = (WXHBRUSH)hbrush;

            m_brush = brush;
        }
    }
    else // invalid brush, reset the current brush
    {
        if ( m_oldBrush )
        {
            if ( ::SelectObject(GetHdc(), (HPEN) m_oldBrush) == HGDI_ERROR )
            {
                wxLogLastError(wxT("SelectObject(old brush)"));
            }

            m_oldBrush = 0;
        }

        m_brush = wxNullBrush;
    }
}

void wxMSWDCImpl::SetBackground(const wxBrush& brush)
{
    m_backgroundBrush = brush;

    if ( m_backgroundBrush.IsOk() )
    {
        (void)SetBkColor(GetHdc(), m_backgroundBrush.GetColour().GetPixel());
    }
}

void wxMSWDCImpl::SetBackgroundMode(int mode)
{
    m_backgroundMode = mode;

    // SetBackgroundColour now only refers to text background
    // and m_backgroundMode is used there
}

void wxMSWDCImpl::SetLogicalFunction(wxRasterOperationMode function)
{
    m_logicalFunction = function;

    SetRop(m_hDC);
}

void wxMSWDCImpl::SetRop(WXHDC dc)
{
    if ( !dc )
        return;

    int rop;

    switch (m_logicalFunction)
    {
        case wxCLEAR:        rop = R2_BLACK;         break;
        case wxXOR:          rop = R2_XORPEN;        break;
        case wxINVERT:       rop = R2_NOT;           break;
        case wxOR_REVERSE:   rop = R2_MERGEPENNOT;   break;
        case wxAND_REVERSE:  rop = R2_MASKPENNOT;    break;
        case wxCOPY:         rop = R2_COPYPEN;       break;
        case wxAND:          rop = R2_MASKPEN;       break;
        case wxAND_INVERT:   rop = R2_MASKNOTPEN;    break;
        case wxNO_OP:        rop = R2_NOP;           break;
        case wxNOR:          rop = R2_NOTMERGEPEN;   break;
        case wxEQUIV:        rop = R2_NOTXORPEN;     break;
        case wxSRC_INVERT:   rop = R2_NOTCOPYPEN;    break;
        case wxOR_INVERT:    rop = R2_MERGENOTPEN;   break;
        case wxNAND:         rop = R2_NOTMASKPEN;    break;
        case wxOR:           rop = R2_MERGEPEN;      break;
        case wxSET:          rop = R2_WHITE;         break;
        default:
            wxFAIL_MSG( wxS("unknown logical function") );
            return;
    }

    SetROP2(GetHdc(), rop);
}

bool wxMSWDCImpl::StartDoc(const wxString& WXUNUSED(message))
{
    // We might be previewing, so return true to let it continue.
    return true;
}

void wxMSWDCImpl::EndDoc()
{
}

void wxMSWDCImpl::StartPage()
{
}

void wxMSWDCImpl::EndPage()
{
}

// ---------------------------------------------------------------------------
// text metrics
// ---------------------------------------------------------------------------

wxCoord wxMSWDCImpl::GetCharHeight() const
{
    TEXTMETRIC lpTextMetric;

    GetTextMetrics(GetHdc(), &lpTextMetric);

    return lpTextMetric.tmHeight;
}

wxCoord wxMSWDCImpl::GetCharWidth() const
{
    TEXTMETRIC lpTextMetric;

    GetTextMetrics(GetHdc(), &lpTextMetric);

    return lpTextMetric.tmAveCharWidth;
}

void wxMSWDCImpl::DoGetFontMetrics(int *height,
                                   int *ascent,
                                   int *descent,
                                   int *internalLeading,
                                   int *externalLeading,
                                   int *averageWidth) const
{
    TEXTMETRIC tm;

    GetTextMetrics(GetHdc(), &tm);

    if ( height )
        *height = tm.tmHeight;
    if ( ascent )
        *ascent = tm.tmAscent;
    if ( descent )
        *descent = tm.tmDescent;
    if ( internalLeading )
        *internalLeading = tm.tmInternalLeading;
    if ( externalLeading )
        *externalLeading = tm.tmExternalLeading;
    if ( averageWidth )
        *averageWidth = tm.tmAveCharWidth;
}

void wxMSWDCImpl::DoGetTextExtent(const wxString& string, wxCoord *x, wxCoord *y,
                           wxCoord *descent, wxCoord *externalLeading,
                           const wxFont *font) const
{
    wxASSERT_MSG( !font || font->IsOk(), wxT("invalid font in wxMSWDCImpl::GetTextExtent") );

    wxTextMeasure txm(GetOwner(), font);
    txm.GetTextExtent(string, x, y, descent, externalLeading);
}


bool wxMSWDCImpl::DoGetPartialTextExtents(const wxString& text, wxArrayInt& widths) const
{
    wxTextMeasure txm(GetOwner(), NULL); // don't change the font
    return txm.GetPartialTextExtents(text, widths, 1.0);
}

namespace
{

void ApplyEffectiveScale(double scale, int sign, int *device, int *logical)
{
    // To reduce rounding errors as much as possible, we try to use the largest
    // possible extent (2^27-1) for the device space but we must also avoid
    // overflowing the int range i.e. ensure that logical extents are less than
    // 2^31 in magnitude. So the minimal scale we can use is 1/16 as for
    // anything smaller VIEWPORT_EXTENT/scale would overflow the int range.
    static const double MIN_LOGICAL_SCALE = 1./16;

    double physExtent = VIEWPORT_EXTENT;
    if ( scale < MIN_LOGICAL_SCALE )
    {
        physExtent *= scale/MIN_LOGICAL_SCALE;
        scale = MIN_LOGICAL_SCALE;
    }

    *device = wxRound(physExtent);
    *logical = sign*wxRound(VIEWPORT_EXTENT/scale);
}

} // anonymous namespace

void wxMSWDCImpl::RealizeScaleAndOrigin()
{
    // although it may seem wasteful to always use MM_ANISOTROPIC here instead
    // of using MM_TEXT if there is no scaling, benchmarking doesn't detect any
    // noticeable difference between these mapping modes
    ::SetMapMode(GetHdc(), MM_ANISOTROPIC);

    // wxWidgets API assumes that the coordinate space is "infinite" (i.e. only
    // limited by 2^32 range of the integer coordinates) but in MSW API we must
    // actually specify the extents that we use so compute them here.

    int devExtX, devExtY,   // Viewport, i.e. device space, extents.
        logExtX, logExtY;   // Window, i.e. logical coordinate space, extents.

    ApplyEffectiveScale(m_scaleX, m_signX, &devExtX, &logExtX);
    ApplyEffectiveScale(m_scaleY, m_signY, &devExtY, &logExtY);

    // In GDI anisotropic mode only devExt/logExt ratio is important
    // so we can reduce the fractions to avoid large numbers
    // which could cause arithmetic overflows inside Win API.
    int gcd = wxGCD(abs(devExtX), abs(logExtX));
    devExtX /= gcd;
    logExtX /= gcd;
    gcd = wxGCD(abs(devExtY), abs(logExtY));
    devExtY /= gcd;
    logExtY /= gcd;

    ::SetViewportExtEx(GetHdc(), devExtX, devExtY, NULL);
    ::SetWindowExtEx(GetHdc(), logExtX, logExtY, NULL);

    ::SetWindowOrgEx(GetHdc(), m_logicalOriginX, m_logicalOriginY, NULL);

    m_isClipBoxValid = false;
}

void wxMSWDCImpl::SetMapMode(wxMappingMode mode)
{
    m_mappingMode = mode;

    if ( mode == wxMM_TEXT )
    {
        m_logicalScaleX =
        m_logicalScaleY = 1.0;
    }
    else // need to do some calculations
    {
        int pixel_width = ::GetDeviceCaps(GetHdc(), HORZRES),
            pixel_height = ::GetDeviceCaps(GetHdc(), VERTRES),
            mm_width = ::GetDeviceCaps(GetHdc(), HORZSIZE),
            mm_height = ::GetDeviceCaps(GetHdc(), VERTSIZE);

        if ( (mm_width == 0) || (mm_height == 0) )
        {
            // we can't calculate mm2pixels[XY] then!
            return;
        }

        double mm2pixelsX = (double)pixel_width / mm_width,
               mm2pixelsY = (double)pixel_height / mm_height;

        switch (mode)
        {
            case wxMM_TWIPS:
                m_logicalScaleX = twips2mm * mm2pixelsX;
                m_logicalScaleY = twips2mm * mm2pixelsY;
                break;

            case wxMM_POINTS:
                m_logicalScaleX = pt2mm * mm2pixelsX;
                m_logicalScaleY = pt2mm * mm2pixelsY;
                break;

            case wxMM_METRIC:
                m_logicalScaleX = mm2pixelsX;
                m_logicalScaleY = mm2pixelsY;
                break;

            case wxMM_LOMETRIC:
                m_logicalScaleX = mm2pixelsX / 10.0;
                m_logicalScaleY = mm2pixelsY / 10.0;
                break;

            default:
                wxFAIL_MSG( wxT("unknown mapping mode in SetMapMode") );
        }
    }

    ComputeScaleAndOrigin();

    RealizeScaleAndOrigin();
}

void wxMSWDCImpl::SetUserScale(double x, double y)
{
    if ( x == m_userScaleX && y == m_userScaleY )
        return;

    wxDCImpl::SetUserScale(x,y);

    RealizeScaleAndOrigin();
}

void wxMSWDCImpl::SetAxisOrientation(bool xLeftRight,
                              bool yBottomUp)
{
    int signX = xLeftRight ? 1 : -1,
        signY = yBottomUp ? -1 : 1;

    if (signX == m_signX && signY == m_signY)
        return;

    wxDCImpl::SetAxisOrientation( xLeftRight, yBottomUp );

    RealizeScaleAndOrigin();
}

void wxMSWDCImpl::SetLogicalOrigin(wxCoord x, wxCoord y)
{
    if ( x == m_logicalOriginX && y == m_logicalOriginY )
        return;

    wxDCImpl::SetLogicalOrigin( x, y );

    RealizeScaleAndOrigin();
}

// For use by wxWidgets only, unless custom units are required.
void wxMSWDCImpl::SetLogicalScale(double x, double y)
{
    wxDCImpl::SetLogicalScale(x,y);

    RealizeScaleAndOrigin();
}

void wxMSWDCImpl::SetDeviceOrigin(wxCoord x, wxCoord y)
{
    if ( x == m_deviceOriginX && y == m_deviceOriginY )
        return;

    wxDCImpl::SetDeviceOrigin( x, y );

    // Do not call RealizeScaleAndOrigin(), we don't rely on Windows for the
    // device origin translation.

    m_isClipBoxValid = false;
}

wxPoint wxMSWDCImpl::DeviceToLogical(wxCoord x, wxCoord y) const
{
    POINT p;
    p.x = x;
    p.y = y;
    ::DPtoLP(GetHdc(), &p, 1);

    wxPoint pt(p.x, p.y);

    if ( m_deviceOriginX || m_deviceOriginY )
    {
        // Note the minus sign, we use it for convenience here as we actually
        // need the reverse translation below.
        const double dx = -m_deviceOriginX / m_scaleX;
        const double dy = -m_deviceOriginY / m_scaleY;

#if wxUSE_DC_TRANSFORM_MATRIX
        // In presence of a world transform we need to apply it to the device
        // origin shift too as it's not taken into account by the HDC itself.
        wxAffineMatrix2D m0 = GetTransformMatrix();
        if ( !m0.IsIdentity() )
        {
            // Compute m^(-1)*T*m where T is the translation matrix.
            wxAffineMatrix2D m = m0;
            m.Invert();
            m.Translate(dx, dy);
            m.Concat(m0);

            const wxPoint2DDouble dp = m.TransformPoint(pt);

            // We don't use rounding here because we don't use it elsewhere.
            pt.x = dp.m_x;
            pt.y = dp.m_y;
        }
        else
#endif // wxUSE_DC_TRANSFORM_MATRIX
        {
            // In this case things can be done much simpler.
            pt.x += dx;
            pt.y += dy;
        }
    }

    return pt;
}

wxPoint wxMSWDCImpl::LogicalToDevice(wxCoord x, wxCoord y) const
{
    POINT p;
    p.x = x;
    p.y = y;
    ::LPtoDP(GetHdc(), &p, 1);
    return wxPoint(p.x + m_deviceOriginX, p.y + m_deviceOriginY);
}

wxSize wxMSWDCImpl::DeviceToLogicalRel(int x, int y) const
{
    POINT p[2];
    p[0].x = 0;
    p[0].y = 0;
    p[1].x = x;
    p[1].y = y;
    ::DPtoLP(GetHdc(), p, WXSIZEOF(p));
    return wxSize(p[1].x-p[0].x, p[1].y-p[0].y);
}

wxSize wxMSWDCImpl::LogicalToDeviceRel(int x, int y) const
{
    POINT p[2];
    p[0].x = 0;
    p[0].y = 0;
    p[1].x = x;
    p[1].y = y;
    ::LPtoDP(GetHdc(), p, WXSIZEOF(p));
    return wxSize(p[1].x-p[0].x, p[1].y-p[0].y);
}

// ----------------------------------------------------------------------------
// Transform matrix
// ----------------------------------------------------------------------------

#if wxUSE_DC_TRANSFORM_MATRIX

bool wxMSWDCImpl::CanUseTransformMatrix() const
{
    return true;
}

bool wxMSWDCImpl::SetTransformMatrix(const wxAffineMatrix2D &matrix)
{
    if ( matrix.IsIdentity() )
    {
        ResetTransformMatrix();
        return true;
    }

    if ( !::SetGraphicsMode(GetHdc(), GM_ADVANCED) )
    {
        wxLogLastError(wxT("SetGraphicsMode"));
        return false;
    }

    wxMatrix2D mat;
    wxPoint2DDouble tr;
    matrix.Get(&mat, &tr);

    XFORM xform;
    xform.eM11 = mat.m_11;
    xform.eM12 = mat.m_12;
    xform.eM21 = mat.m_21;
    xform.eM22 = mat.m_22;
    xform.eDx = tr.m_x;
    xform.eDy = tr.m_y;

    if ( !::SetWorldTransform(GetHdc(), &xform) )
    {
        wxLogLastError(wxT("SetWorldTransform"));
        return false;
    }

    m_isClipBoxValid = false;
    return true;
}

wxAffineMatrix2D wxMSWDCImpl::GetTransformMatrix() const
{
    wxAffineMatrix2D transform;

    XFORM xform;
    if ( !::GetWorldTransform(GetHdc(), &xform) )
    {
        wxLogLastError(wxT("GetWorldTransform"));
        return transform;
    }

    wxMatrix2D m(xform.eM11, xform.eM12, xform.eM21, xform.eM22);
    wxPoint2DDouble p(xform.eDx, xform.eDy);
    transform.Set(m, p);

    return transform;
}

void wxMSWDCImpl::ResetTransformMatrix()
{
    ::ModifyWorldTransform(GetHdc(), NULL, MWT_IDENTITY);
    ::SetGraphicsMode(GetHdc(), GM_COMPATIBLE);
    m_isClipBoxValid = false;
}

#endif // wxUSE_DC_TRANSFORM_MATRIX

// ---------------------------------------------------------------------------
// bit blit
// ---------------------------------------------------------------------------

bool wxMSWDCImpl::DoBlit(wxCoord dstX, wxCoord dstY,
                  wxCoord dstWidth, wxCoord dstHeight,
                  wxDC *source,
                  wxCoord srcX, wxCoord srcY,
                  wxRasterOperationMode rop, bool useMask,
                  wxCoord srcMaskX, wxCoord srcMaskY)
{
    return DoStretchBlit(dstX, dstY, dstWidth, dstHeight, source, srcX, srcY, dstWidth, dstHeight, rop, useMask, srcMaskX, srcMaskY);
}

bool wxMSWDCImpl::DoStretchBlit(wxCoord xdest, wxCoord ydest,
                         wxCoord dstWidth, wxCoord dstHeight,
                         wxDC *source,
                         wxCoord xsrc, wxCoord ysrc,
                         wxCoord srcWidth, wxCoord srcHeight,
                         wxRasterOperationMode rop, bool useMask,
                         wxCoord xsrcMask, wxCoord ysrcMask)
{
    wxCHECK_MSG( source, false, wxT("wxMSWDCImpl::Blit(): NULL wxDC pointer") );

    wxMSWDCImpl *implSrc = wxDynamicCast( source->GetImpl(), wxMSWDCImpl );
    if ( !implSrc )
    {
        // TODO: Do we want to be able to blit from other DCs too?
        return false;
    }

    const wxRect bbox(xdest, ydest, dstWidth, dstHeight);

    // Take the device origin into account manually here, as none of the
    // functions used below would do it.
    xdest += XLOG2DEV(0);
    ydest += YLOG2DEV(0);

    const int xsrcOrig = xsrc;
    const int ysrcOrig = ysrc;

    // This does the same thing as XLOG2DEV() but for the source DC.
    xsrc += implSrc->m_deviceOriginX / implSrc->m_scaleX;
    ysrc += implSrc->m_deviceOriginY / implSrc->m_scaleY;

    HDC hdcSrc = GetHdcOf(*implSrc);

    // if either the source or destination has alpha channel, we must use
    // AlphaBlt() as other function don't handle it correctly
    const wxBitmap& bmpSrc = implSrc->GetSelectedBitmap();
    if ( bmpSrc.IsOk() && (bmpSrc.HasAlpha() ||
            (m_selectedBitmap.IsOk() && m_selectedBitmap.HasAlpha())) )
    {
        if ( AlphaBlt(this, xdest, ydest, dstWidth, dstHeight,
                      xsrc, ysrc, srcWidth, srcHeight, hdcSrc, bmpSrc) )
        {
            CalcBoundingBox(bbox);
            return true;
        }
    }

    wxMask *mask = NULL;
    if ( useMask )
    {
        mask = bmpSrc.GetMask();

        if ( !(bmpSrc.IsOk() && mask && mask->GetMaskBitmap()) )
        {
            // don't give assert here because this would break existing
            // programs - just silently ignore useMask parameter
            useMask = false;
        }
    }

    if (xsrcMask == -1 && ysrcMask == -1)
    {
        xsrcMask = xsrc; ysrcMask = ysrc;
    }

    wxTextColoursChanger textCol(GetHdc(), *this);

    DWORD dwRop;
    switch (rop)
    {
        case wxXOR:          dwRop = SRCINVERT;        break;
        case wxINVERT:       dwRop = DSTINVERT;        break;
        case wxOR_REVERSE:   dwRop = 0x00DD0228;       break;
        case wxAND_REVERSE:  dwRop = SRCERASE;         break;
        case wxCLEAR:        dwRop = BLACKNESS;        break;
        case wxSET:          dwRop = WHITENESS;        break;
        case wxOR_INVERT:    dwRop = MERGEPAINT;       break;
        case wxAND:          dwRop = SRCAND;           break;
        case wxOR:           dwRop = SRCPAINT;         break;
        case wxEQUIV:        dwRop = 0x00990066;       break;
        case wxNAND:         dwRop = 0x007700E6;       break;
        case wxAND_INVERT:   dwRop = 0x00220326;       break;
        case wxCOPY:         dwRop = SRCCOPY;          break;
        case wxNO_OP:        dwRop = DSTCOPY;          break;
        case wxSRC_INVERT:   dwRop = NOTSRCCOPY;       break;
        case wxNOR:          dwRop = NOTSRCCOPY;       break;
        default:
           wxFAIL_MSG( wxT("unsupported logical function") );
           return false;
    }

    // Most of the operations involve the source or the pattern, but a few of
    // them (and only those few, no other are possible) only use destination
    // HDC. For them we must not give a valid source HDC to MaskBlt() as it
    // still uses it, somehow, and the result is garbage.
    if ( dwRop == BLACKNESS || dwRop == WHITENESS ||
            dwRop == DSTINVERT || dwRop == DSTCOPY )
    {
        hdcSrc = NULL;
    }

    bool success = false;

    if (useMask)
    {
        // we want the part of the image corresponding to the mask to be
        // transparent, so use "DSTCOPY" ROP for the mask points (the usual
        // meaning of fg and bg is inverted which corresponds to wxWin notion
        // of the mask which is also contrary to the Windows one)

        // On some systems, MaskBlt succeeds yet is much much slower
        // than the wxWidgets fall-back implementation. So we need
        // to be able to switch this on and off at runtime.
#if wxUSE_SYSTEM_OPTIONS
        static bool s_maskBltAllowed = wxSystemOptions::GetOptionInt("no-maskblt") == 0;
        if ( s_maskBltAllowed )
#endif
        {
            if ( dstWidth == srcWidth && dstHeight == srcHeight )
            {
                success = ::MaskBlt
                            (
                            GetHdc(),
                            xdest, ydest, dstWidth, dstHeight,
                            hdcSrc,
                            xsrc, ysrc,
                            (HBITMAP)mask->GetMaskBitmap(),
                            xsrcMask, ysrcMask,
                            MAKEROP4(dwRop, DSTCOPY)
                            ) != 0;
            }
        }

        if ( !success )
        {
            // Blit bitmap with mask
            HDC dc_mask ;
            HDC  dc_buffer ;
            HBITMAP buffer_bmap ;

#if wxUSE_DC_CACHEING
            // create a temp buffer bitmap and DCs to access it and the mask
            wxDCCacheEntry* dcCacheEntry1 = FindDCInCache(NULL, hdcSrc);
            dc_mask = (HDC) dcCacheEntry1->m_dc;

            wxDCCacheEntry* dcCacheEntry2 = FindDCInCache(dcCacheEntry1, GetHDC());
            dc_buffer = (HDC) dcCacheEntry2->m_dc;

            wxDCCacheEntry* bitmapCacheEntry = FindBitmapInCache(GetHDC(),
                dstWidth, dstHeight);

            buffer_bmap = (HBITMAP) bitmapCacheEntry->m_bitmap;
#else // !wxUSE_DC_CACHEING
            // create a temp buffer bitmap and DCs to access it and the mask
            dc_mask = wxMSWImpl::CreateCompatibleDCWithLayout(hdcSrc);
            dc_buffer = wxMSWImpl::CreateCompatibleDCWithLayout(GetHdc());
            buffer_bmap = ::CreateCompatibleBitmap(GetHdc(), dstWidth, dstHeight);
#endif // wxUSE_DC_CACHEING/!wxUSE_DC_CACHEING
            HGDIOBJ hOldMaskBitmap = ::SelectObject(dc_mask, (HBITMAP) mask->GetMaskBitmap());
            HGDIOBJ hOldBufferBitmap = ::SelectObject(dc_buffer, buffer_bmap);

            // copy dest to buffer
            if ( !::BitBlt(dc_buffer, 0, 0, dstWidth, dstHeight,
                           GetHdc(), xdest, ydest, SRCCOPY) )
            {
                wxLogLastError(wxT("BitBlt"));
            }

            StretchBltModeChanger stretchModeChanger(GetHdc());

            // copy src to buffer using selected raster op
            if ( !::StretchBlt(dc_buffer, 0, 0, dstWidth, dstHeight,
                               hdcSrc, xsrc, ysrc, srcWidth, srcHeight, dwRop) )
            {
                wxLogLastError(wxT("StretchBlt"));
            }

            // set masked area in buffer to BLACK
            {
                wxTextColoursChanger textCol2(GetHdc(), *wxBLACK, *wxWHITE);
                if ( !::StretchBlt(dc_buffer, 0, 0, dstWidth, dstHeight,
                                   dc_mask, xsrcMask, ysrcMask,
                                   srcWidth, srcHeight, SRCAND) )
                {
                    wxLogLastError(wxT("StretchBlt"));
                }

                // set unmasked area in dest to BLACK
                ::SetBkColor(GetHdc(), RGB(0, 0, 0));
                ::SetTextColor(GetHdc(), RGB(255, 255, 255));
                if ( !::StretchBlt(GetHdc(), xdest, ydest, dstWidth, dstHeight,
                                   dc_mask, xsrcMask, ysrcMask,
                                   srcWidth, srcHeight, SRCAND) )
                {
                    wxLogLastError(wxT("StretchBlt"));
                }
            } // restore the original text and background colours

            // OR buffer to dest
            success = ::BitBlt(GetHdc(), xdest, ydest, dstWidth, dstHeight,
                               dc_buffer, 0, 0, SRCPAINT) != 0;
            if ( !success )
            {
                wxLogLastError(wxT("BitBlt"));
            }

            // tidy up temporary DCs and bitmap
            ::SelectObject(dc_mask, hOldMaskBitmap);
            ::SelectObject(dc_buffer, hOldBufferBitmap);

#if !wxUSE_DC_CACHEING
            {
                ::DeleteDC(dc_mask);
                ::DeleteDC(dc_buffer);
                ::DeleteObject(buffer_bmap);
            }
#endif
        }
    }
    else // no mask, just BitBlt() it
    {
        // if we already have a DIB, draw it using StretchDIBits(), otherwise
        // use StretchBlt() if available and finally fall back to BitBlt()

        // Notice that we can't use StretchDIBits() when source and destination
        // are the same as it can't, apparently, handle overlapping surfaces.
        const int caps = ::GetDeviceCaps(GetHdc(), RASTERCAPS);
        if ( bmpSrc.IsOk() && (GetHdc() != hdcSrc) && (caps & RC_STRETCHDIB) )
        {
            DIBSECTION ds;
            wxZeroMemory(ds);

            if ( ::GetObject(GetHbitmapOf(bmpSrc),
                             sizeof(ds),
                             &ds) == sizeof(ds) )
            {
                StretchBltModeChanger stretchModeChanger(GetHdc());

                // Unlike all the other functions used here (i.e. AlphaBlt(),
                // MaskBlt(), BitBlt() and StretchBlt()), StretchDIBits() does
                // not take into account the source DC logical coordinates
                // automatically as it doesn't even work with the source HDC.
                // So do this manually to ensure that the coordinates are
                // interpreted in the same way here as in all the other cases.
                xsrc = source->LogicalToDeviceX(xsrcOrig);
                ysrc = source->LogicalToDeviceY(ysrcOrig);
                srcWidth = source->LogicalToDeviceXRel(srcWidth);
                srcHeight = source->LogicalToDeviceYRel(srcHeight);

                // Figure out what co-ordinate system we're supposed to specify
                // ysrc in.
                const LONG hDIB = ds.dsBmih.biHeight;
                if ( hDIB > 0 )
                {
                    // reflect ysrc
                    ysrc = hDIB - (ysrc + srcHeight);
                }

                if ( ::StretchDIBits(GetHdc(),
                                     xdest, ydest,
                                     dstWidth, dstHeight,
                                     xsrc, ysrc,
                                     srcWidth, srcHeight,
                                     ds.dsBm.bmBits,
                                     (LPBITMAPINFO)&ds.dsBmih,
                                     DIB_RGB_COLORS,
                                     dwRop
                                     ) == (int)GDI_ERROR )
                {
                    wxLogLastError(wxT("StretchDIBits"));
                }
                else
                {
                    success = true;
                }
            }
        }

        if ( !success && (caps & RC_STRETCHBLT) )
        {
            StretchBltModeChanger stretchModeChanger(GetHdc());

            /*
            Workaround for #19190. See (reverted) 6614aa496d: "For some reason
            in RTL mode, source offset has to be -1, otherwise the right
            border (physical) remains unpainted." [note that the offset
            actually is applied not only to the source but also dest]

            Be conservative about using the offset and only do it for
            currently known failing cases: when xdest and xsrc are
            equal and only when not actually stretching.
            */
            const wxCoord ofs = (GetLayoutDirection() == wxLayout_RightToLeft
                                 && xdest == xsrc
                                 && srcWidth == dstWidth
                                 && srcHeight == dstHeight) ? -1 : 0;

            if ( !::StretchBlt
                    (
                        GetHdc(),
                        xdest + ofs, ydest, dstWidth, dstHeight,
                        hdcSrc,
                        xsrc + ofs, ysrc, srcWidth, srcHeight,
                        dwRop
                    ) )
            {
                wxLogLastError(wxT("StretchBlt"));
            }
            else
            {
                success = true;
            }
        }

        if ( !success )
        {
            if ( !::BitBlt(GetHdc(), xdest, ydest, dstWidth, dstHeight,
                           hdcSrc, xsrc, ysrc, dwRop) )
            {
                wxLogLastError(wxT("BitBlt"));
            }
            else
            {
                success = true;
            }
        }
    }

    if ( success )
        CalcBoundingBox(bbox);

    return success;
}

void wxMSWDCImpl::GetDeviceSize(int *width, int *height) const
{
    if ( width )
        *width = ::GetDeviceCaps(GetHdc(), HORZRES);
    if ( height )
        *height = ::GetDeviceCaps(GetHdc(), VERTRES);
}

void wxMSWDCImpl::DoGetSizeMM(int *w, int *h) const
{
    // if we implement it in terms of DoGetSize() instead of directly using the
    // results returned by GetDeviceCaps(HORZ/VERTSIZE) as was done before, it
    // will also work for wxWindowDC and wxClientDC even though their size is
    // not the same as the total size of the screen
    int wPixels, hPixels;
    DoGetSize(&wPixels, &hPixels);

    if ( w )
    {
        int wTotal = ::GetDeviceCaps(GetHdc(), HORZRES);

        wxCHECK_RET( wTotal, wxT("0 width device?") );

        *w = (wPixels * ::GetDeviceCaps(GetHdc(), HORZSIZE)) / wTotal;
    }

    if ( h )
    {
        int hTotal = ::GetDeviceCaps(GetHdc(), VERTRES);

        wxCHECK_RET( hTotal, wxT("0 height device?") );

        *h = (hPixels * ::GetDeviceCaps(GetHdc(), VERTSIZE)) / hTotal;
    }
}

wxSize wxMSWDCImpl::GetPPI() const
{
    // As documented by MSDN, GetDeviceCaps() returns the same value for all
    // HDCs on the system, and so can't be used to retrieve the correct value
    // for the HDCs associated with the windows on monitors other than the
    // primary one if they use different DPI. Hence prefer to get this
    // information from the associated window, if possible.
    wxSize ppi;
    if ( m_window )
    {
        ppi = m_window->GetDPI();
    }

    if ( !ppi.x || !ppi.y )
    {
        ppi = wxGetDPIofHDC(GetHdc());
    }

    return ppi;
}

// ----------------------------------------------------------------------------
// DC caching
// ----------------------------------------------------------------------------

#if wxUSE_DC_CACHEING

/*
 * This implementation is a bit ugly and uses the old-fashioned wxList class, so I will
 * improve it in due course, either using arrays, or simply storing pointers to one
 * entry for the bitmap, and two for the DCs. -- JACS
 */

wxObjectList wxMSWDCImpl::sm_bitmapCache;
wxObjectList wxMSWDCImpl::sm_dcCache;

wxDCCacheEntry::wxDCCacheEntry(WXHBITMAP hBitmap, int w, int h, int depth)
{
    m_bitmap = hBitmap;
    m_dc = 0;
    m_width = w;
    m_height = h;
    m_depth = depth;
}

wxDCCacheEntry::wxDCCacheEntry(WXHDC hDC, int depth)
{
    m_bitmap = 0;
    m_dc = hDC;
    m_width = 0;
    m_height = 0;
    m_depth = depth;
}

wxDCCacheEntry::~wxDCCacheEntry()
{
    if (m_bitmap)
        ::DeleteObject((HBITMAP) m_bitmap);
    if (m_dc)
        ::DeleteDC((HDC) m_dc);
}

wxDCCacheEntry* wxMSWDCImpl::FindBitmapInCache(WXHDC dc, int w, int h)
{
    int depth = ::GetDeviceCaps((HDC) dc, PLANES) * ::GetDeviceCaps((HDC) dc, BITSPIXEL);
    wxList::compatibility_iterator node = sm_bitmapCache.GetFirst();
    while (node)
    {
        wxDCCacheEntry* entry = (wxDCCacheEntry*) node->GetData();

        if (entry->m_depth == depth)
        {
            if (entry->m_width < w || entry->m_height < h)
            {
                ::DeleteObject((HBITMAP) entry->m_bitmap);
                entry->m_bitmap = (WXHBITMAP) ::CreateCompatibleBitmap((HDC) dc, w, h);
                if ( !entry->m_bitmap)
                {
                    wxLogLastError(wxT("CreateCompatibleBitmap"));
                }
                entry->m_width = w; entry->m_height = h;
                return entry;
            }
            return entry;
        }

        node = node->GetNext();
    }
    WXHBITMAP hBitmap = (WXHBITMAP) ::CreateCompatibleBitmap((HDC) dc, w, h);
    if ( !hBitmap)
    {
        wxLogLastError(wxT("CreateCompatibleBitmap"));
    }
    wxDCCacheEntry* entry = new wxDCCacheEntry(hBitmap, w, h, depth);
    AddToBitmapCache(entry);
    return entry;
}

wxDCCacheEntry* wxMSWDCImpl::FindDCInCache(wxDCCacheEntry* notThis, WXHDC dc)
{
    int depth = ::GetDeviceCaps((HDC) dc, PLANES) * ::GetDeviceCaps((HDC) dc, BITSPIXEL);
    wxList::compatibility_iterator node = sm_dcCache.GetFirst();
    while (node)
    {
        wxDCCacheEntry* entry = (wxDCCacheEntry*) node->GetData();

        // Don't return the same one as we already have
        if (!notThis || (notThis != entry))
        {
            if (entry->m_depth == depth)
            {
                return entry;
            }
        }

        node = node->GetNext();
    }
    WXHDC hDC = (WXHDC) wxMSWImpl::CreateCompatibleDCWithLayout((HDC) dc);
    if ( !hDC)
    {
        wxLogLastError(wxT("CreateCompatibleDC"));
    }
    wxDCCacheEntry* entry = new wxDCCacheEntry(hDC, depth);
    AddToDCCache(entry);
    return entry;
}

void wxMSWDCImpl::AddToBitmapCache(wxDCCacheEntry* entry)
{
    sm_bitmapCache.Append(entry);
}

void wxMSWDCImpl::AddToDCCache(wxDCCacheEntry* entry)
{
    sm_dcCache.Append(entry);
}

void wxMSWDCImpl::ClearCache()
{
    WX_CLEAR_LIST(wxList, sm_dcCache);
    WX_CLEAR_LIST(wxList, sm_bitmapCache);
}

// Clean up cache at app exit
class wxDCModule : public wxModule
{
public:
    virtual bool OnInit() wxOVERRIDE { return true; }
    virtual void OnExit() wxOVERRIDE { wxMSWDCImpl::ClearCache(); }

private:
    wxDECLARE_DYNAMIC_CLASS(wxDCModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxDCModule, wxModule);

#endif // wxUSE_DC_CACHEING

// ----------------------------------------------------------------------------
// alpha channel support
// ----------------------------------------------------------------------------

static bool AlphaBlt(wxMSWDCImpl* dcDst,
                     int x, int y, int dstWidth, int dstHeight,
                     int srcX, int srcY,
                     int srcWidth, int srcHeight,
                     HDC hdcSrc,
                     const wxBitmap& WXUNUSED_UNLESS_DEBUG(bmpSrc))
{
    wxASSERT_MSG( bmpSrc.IsOk() && bmpSrc.HasAlpha(),
                    wxT("AlphaBlt(): invalid bitmap") );
    wxASSERT_MSG( dcDst && hdcSrc, wxT("AlphaBlt(): invalid HDC") );

    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = 0xff;
    bf.AlphaFormat = AC_SRC_ALPHA;

    if ( !wxDynLoadWrappers::AlphaBlend
          (
            GetHdcOf(*dcDst), x, y, dstWidth, dstHeight,
            hdcSrc, srcX, srcY, srcWidth, srcHeight,
            bf
          ) )
    {
        wxLogLastError(wxT("AlphaBlend"));
        return false;
    }

    // There is an extra complication with drawing bitmaps with alpha
    // on bitmaps without alpha: AlphaBlt() modifies the alpha
    // component of the destination bitmap even if it hadn't had it
    // before which is not only unexpected but corrupts the bitmap as
    // all its pixels outside of the (x, y, dstWidth, dstHeight) area
    // are now fully transparent: they already had 0 value of alpha
    // before but it didn't count as long as all other pixels had 0
    // alpha as well, but now that some of them are not transparent any
    // more, the rest of pixels with 0 alpha is transparent. So undo
    // this to avoid "losing" the existing bitmap contents outside of
    // the area affected by AlphaBlt(), see #14403.
#ifdef wxHAS_RAW_BITMAP
    const wxBitmap& bmpDst = dcDst->GetSelectedBitmap();
    if ( bmpDst.IsOk() && !bmpDst.HasAlpha() && bmpDst.GetDepth() == 32 )
    {
        // We need to deselect the bitmap from the memory DC it is
        // currently selected into before modifying it.
        wxBitmap bmpOld = bmpDst;
        dcDst->DoSelect(wxNullBitmap);

        // Notice the extra block: we must destroy wxAlphaPixelData
        // before selecting the bitmap into the DC again.
        {
            // Since drawn bitmap can only partially overlap
            // with destination bitmap we need to calculate
            // efective drawing area location.
            const wxRect rectDst(bmpOld.GetSize());
            const wxRect rectDrawn(x, y, dstWidth, dstHeight);
            const wxRect r = rectDrawn.Intersect(rectDst);

            wxAlphaPixelData data(bmpOld);
            if ( data )
            {
                wxAlphaPixelData::Iterator p(data);

                p.Offset(data, r.GetLeft(), r.GetTop());
                for ( int old_y = 0; old_y < r.GetHeight(); old_y++ )
                {
                    wxAlphaPixelData::Iterator rowStart = p;
                    for ( int old_x = 0; old_x < r.GetWidth(); old_x++ )
                    {
                        // We choose to use wxALPHA_TRANSPARENT instead
                        // of perhaps more logical wxALPHA_OPAQUE here
                        // to ensure that the bitmap remains the same
                        // as before, i.e. without any alpha at all.
                        p.Alpha() = wxALPHA_TRANSPARENT;
                        ++p;
                    }

                    p = rowStart;
                    p.OffsetY(data, 1);
                }
            }
        }

        // Using wxAlphaPixelData sets the internal "has alpha" flag
        // which is usually what we need, but in this particular case
        // we use it to get rid of alpha, not set it, so reset it back.
        bmpOld.ResetAlpha();

        dcDst->DoSelect(bmpOld);
    }
#endif // wxHAS_RAW_BITMAP

    return true;
}


void wxMSWDCImpl::DoGradientFillLinear (const wxRect& rect,
                                 const wxColour& initialColour,
                                 const wxColour& destColour,
                                 wxDirection nDirection)
{
    GRADIENT_RECT grect;
    grect.UpperLeft = 0;
    grect.LowerRight = 1;

    // invert colours direction if not filling from left-to-right or
    // top-to-bottom
    int firstVertex = nDirection == wxNORTH || nDirection == wxWEST ? 1 : 0;

    // one vertex for upper left and one for upper-right
    TRIVERTEX vertices[2];

    vertices[0].x = XLOG2DEV(rect.GetLeft());
    vertices[0].y = YLOG2DEV(rect.GetTop());
    vertices[1].x = XLOG2DEV(rect.GetRight()) + 1;
    vertices[1].y = YLOG2DEV(rect.GetBottom()) + 1;

    vertices[firstVertex].Red = (COLOR16)(initialColour.Red() << 8);
    vertices[firstVertex].Green = (COLOR16)(initialColour.Green() << 8);
    vertices[firstVertex].Blue = (COLOR16)(initialColour.Blue() << 8);
    vertices[firstVertex].Alpha = 0;
    vertices[1 - firstVertex].Red = (COLOR16)(destColour.Red() << 8);
    vertices[1 - firstVertex].Green = (COLOR16)(destColour.Green() << 8);
    vertices[1 - firstVertex].Blue = (COLOR16)(destColour.Blue() << 8);
    vertices[1 - firstVertex].Alpha = 0;

    if ( wxDynLoadWrappers::GradientFill
         (
            GetHdc(),
            vertices,
            WXSIZEOF(vertices),
            &grect,
            1,
            nDirection == wxWEST || nDirection == wxEAST
                ? GRADIENT_FILL_RECT_H
                : GRADIENT_FILL_RECT_V
         ) )
    {
        CalcBoundingBox(rect);
    }
    else
    {
        wxLogLastError(wxT("GradientFill"));
    }
}

namespace wxMSWImpl
{

HDC CreateCompatibleDCWithLayout(HDC hdc)
{
    HDC hdcNew = ::CreateCompatibleDC(hdc);
    if ( hdcNew )
    {
        DWORD dwLayout = wxDynLoadWrappers::GetLayout(hdc);
        if ( dwLayout != GDI_ERROR )
            wxDynLoadWrappers::SetLayout(hdcNew, dwLayout);
    }

    return hdcNew;
}

} // namespace wxMSWImpl

wxLayoutDirection wxMSWDCImpl::GetLayoutDirection() const
{
    DWORD layout = wxDynLoadWrappers::GetLayout(GetHdc());

    if ( layout == GDI_ERROR )
        return wxLayout_Default;

    return layout & LAYOUT_RTL ? wxLayout_RightToLeft : wxLayout_LeftToRight;
}

void wxMSWDCImpl::SetLayoutDirection(wxLayoutDirection dir)
{
    if ( dir == wxLayout_Default )
    {
        dir = wxApp::MSWGetDefaultLayout(GetWindow());
        if ( dir == wxLayout_Default )
            return;
    }

    DWORD layout = wxDynLoadWrappers::GetLayout(GetHdc());
    if ( layout == GDI_ERROR )
        return;

    if ( dir == wxLayout_RightToLeft )
        layout |= LAYOUT_RTL;
    else
        layout &= ~LAYOUT_RTL;

    wxDynLoadWrappers::SetLayout(GetHdc(), layout);
}
