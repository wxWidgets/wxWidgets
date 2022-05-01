/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/dcbase.cpp
// Purpose:     generic methods of the wxDC Class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     05/25/99
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/dc.h"
#include "wx/dcclient.h"
#include "wx/dcmemory.h"
#include "wx/dcscreen.h"
#include "wx/dcprint.h"
#include "wx/prntbase.h"
#include "wx/scopedarray.h"
#include "wx/scopeguard.h"
#include "wx/stack.h"

#ifndef WX_PRECOMP
    #include "wx/math.h"
    #include "wx/module.h"
    #include "wx/window.h"
#endif

#include "wx/private/textmeasure.h"
#include "wx/private/rescale.h"
#include "wx/display.h"

#ifdef __WXMSW__
    #include "wx/msw/dcclient.h"
    #include "wx/msw/dcmemory.h"
    #include "wx/msw/dcscreen.h"
#endif

#ifdef __WXGTK3__
    #include "wx/gtk/dc.h"
#elif defined __WXGTK20__
    #include "wx/gtk/dcclient.h"
    #include "wx/gtk/dcmemory.h"
    #include "wx/gtk/dcscreen.h"
#elif defined(__WXGTK__)
    #include "wx/gtk1/dcclient.h"
    #include "wx/gtk1/dcmemory.h"
    #include "wx/gtk1/dcscreen.h"
#endif

#ifdef __WXMAC__
    #include "wx/osx/dcclient.h"
    #include "wx/osx/dcmemory.h"
    #include "wx/osx/dcscreen.h"
#endif

#ifdef __WXMOTIF__
    #include "wx/motif/dcclient.h"
    #include "wx/motif/dcmemory.h"
    #include "wx/motif/dcscreen.h"
#endif

#ifdef __WXX11__
    #include "wx/x11/dcclient.h"
    #include "wx/x11/dcmemory.h"
    #include "wx/x11/dcscreen.h"
#endif

#ifdef __WXDFB__
    #include "wx/dfb/dcclient.h"
    #include "wx/dfb/dcmemory.h"
    #include "wx/dfb/dcscreen.h"
#endif

#ifdef __WXQT__
    #include "wx/qt/dcclient.h"
    #include "wx/qt/dcmemory.h"
    #include "wx/qt/dcscreen.h"
#endif
//----------------------------------------------------------------------------
// wxDCFactory
//----------------------------------------------------------------------------

wxDCFactory *wxDCFactory::m_factory = NULL;

void wxDCFactory::Set(wxDCFactory *factory)
{
    delete m_factory;

    m_factory = factory;
}

wxDCFactory *wxDCFactory::Get()
{
    if ( !m_factory )
        m_factory = new wxNativeDCFactory;

    return m_factory;
}

class wxDCFactoryCleanupModule : public wxModule
{
public:
    virtual bool OnInit() wxOVERRIDE { return true; }
    virtual void OnExit() wxOVERRIDE { wxDCFactory::Set(NULL); }

private:
    wxDECLARE_DYNAMIC_CLASS(wxDCFactoryCleanupModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxDCFactoryCleanupModule, wxModule);

//-----------------------------------------------------------------------------
// wxNativeDCFactory
//-----------------------------------------------------------------------------

wxDCImpl* wxNativeDCFactory::CreateWindowDC( wxWindowDC *owner, wxWindow *window )
{
    wxDCImpl * const impl = new wxWindowDCImpl( owner, window );
    impl->InheritAttributes(window);
    return impl;
}

wxDCImpl* wxNativeDCFactory::CreateClientDC( wxClientDC *owner, wxWindow *window )
{
    wxDCImpl * const impl = new wxClientDCImpl( owner, window );
    impl->InheritAttributes(window);
    return impl;
}

wxDCImpl* wxNativeDCFactory::CreatePaintDC( wxPaintDC *owner, wxWindow *window )
{
    wxDCImpl * const impl = new wxPaintDCImpl( owner, window );
    impl->InheritAttributes(window);
    return impl;
}

wxDCImpl* wxNativeDCFactory::CreateMemoryDC( wxMemoryDC *owner )
{
    return new wxMemoryDCImpl( owner );
}

wxDCImpl* wxNativeDCFactory::CreateMemoryDC(wxMemoryDC *owner, wxBitmap& bitmap)
{
    // the bitmap may be modified when it's selected into a memory DC so make
    // sure changing this bitmap doesn't affect any other shallow copies of it
    // (see wxMemoryDC::SelectObject())
    //
    // notice that we don't provide any ctor equivalent to SelectObjectAsSource
    // method because this should be rarely needed and easy to work around by
    // using the default ctor and calling SelectObjectAsSource itself
    if ( bitmap.IsOk() )
        bitmap.UnShare();

    return new wxMemoryDCImpl(owner, bitmap);
}

wxDCImpl* wxNativeDCFactory::CreateMemoryDC( wxMemoryDC *owner, wxDC *dc )
{
    return new wxMemoryDCImpl( owner, dc );
}

wxDCImpl* wxNativeDCFactory::CreateScreenDC( wxScreenDC *owner )
{
    return new wxScreenDCImpl( owner );
}

#if wxUSE_PRINTING_ARCHITECTURE
wxDCImpl *wxNativeDCFactory::CreatePrinterDC( wxPrinterDC *owner, const wxPrintData &data )
{
    wxPrintFactory *factory = wxPrintFactory::GetFactory();
    return factory->CreatePrinterDCImpl( owner, data );
}
#endif

//-----------------------------------------------------------------------------
// wxWindowDC
//-----------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxWindowDC, wxDC);

wxWindowDC::wxWindowDC(wxWindow *win)
          : wxDC(wxDCFactory::Get()->CreateWindowDC(this, win))
{
}

//-----------------------------------------------------------------------------
// wxClientDC
//-----------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxClientDC, wxWindowDC);

wxClientDC::wxClientDC(wxWindow *win)
          : wxWindowDC(wxDCFactory::Get()->CreateClientDC(this, win))
{
}

//-----------------------------------------------------------------------------
// wxMemoryDC
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxMemoryDC, wxDC);

wxMemoryDC::wxMemoryDC()
          : wxDC(wxDCFactory::Get()->CreateMemoryDC(this))
{
}

wxMemoryDC::wxMemoryDC(wxBitmap& bitmap)
          : wxDC(wxDCFactory::Get()->CreateMemoryDC(this, bitmap))
{
}

wxMemoryDC::wxMemoryDC(wxDC *dc)
          : wxDC(wxDCFactory::Get()->CreateMemoryDC(this, dc))
{
}

void wxMemoryDC::SelectObject(wxBitmap& bmp)
{
    if ( bmp.IsSameAs(GetSelectedBitmap()) )
    {
        // Nothing to do, this bitmap is already selected.
        return;
    }

    // make sure that the given wxBitmap is not sharing its data with other
    // wxBitmap instances as its contents will be modified by any drawing
    // operation done on this DC
    if (bmp.IsOk())
        bmp.UnShare();

    GetImpl()->DoSelect(bmp);
}

void wxMemoryDC::SelectObjectAsSource(const wxBitmap& bmp)
{
    GetImpl()->DoSelect(bmp);
}

const wxBitmap& wxMemoryDC::GetSelectedBitmap() const
{
    return GetImpl()->GetSelectedBitmap();
}

wxBitmap& wxMemoryDC::GetSelectedBitmap()
{
    return GetImpl()->GetSelectedBitmap();
}


//-----------------------------------------------------------------------------
// wxPaintDC
//-----------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxPaintDC, wxClientDC);

wxPaintDC::wxPaintDC(wxWindow *win)
         : wxClientDC(wxDCFactory::Get()->CreatePaintDC(this, win))
{
}

//-----------------------------------------------------------------------------
// wxScreenDC
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxScreenDC, wxWindowDC);

wxScreenDC::wxScreenDC()
          : wxDC(wxDCFactory::Get()->CreateScreenDC(this))
{
}

//-----------------------------------------------------------------------------
// wxPrinterDC
//-----------------------------------------------------------------------------

#if wxUSE_PRINTING_ARCHITECTURE

wxIMPLEMENT_DYNAMIC_CLASS(wxPrinterDC, wxDC);

wxPrinterDC::wxPrinterDC()
           : wxDC(wxDCFactory::Get()->CreatePrinterDC(this, wxPrintData()))
{
}

wxPrinterDC::wxPrinterDC(const wxPrintData& data)
           : wxDC(wxDCFactory::Get()->CreatePrinterDC(this, data))
{
}

wxRect wxPrinterDC::GetPaperRect() const
{
    return GetImpl()->GetPaperRect();
}

int wxPrinterDC::GetResolution() const
{
    return GetImpl()->GetResolution();
}

#endif // wxUSE_PRINTING_ARCHITECTURE

//-----------------------------------------------------------------------------
// wxDCImpl
//-----------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxDCImpl, wxObject);

wxDCImpl::wxDCImpl( wxDC *owner )
        : m_window(NULL)
        , m_colour(true)
        , m_ok(true)
        , m_clipping(false)
        , m_isInteractive(0)
        , m_isBBoxValid(false)
        , m_logicalOriginX(0), m_logicalOriginY(0)
        , m_deviceOriginX(0), m_deviceOriginY(0)
        , m_deviceLocalOriginX(0), m_deviceLocalOriginY(0)
        , m_logicalScaleX(1.0), m_logicalScaleY(1.0)
        , m_userScaleX(1.0), m_userScaleY(1.0)
        , m_scaleX(1.0), m_scaleY(1.0)
        , m_signX(1), m_signY(1)
        , m_contentScaleFactor(1)
        , m_mm_to_pix_x(0.0), m_mm_to_pix_y(0.0)
        , m_minX(0), m_minY(0), m_maxX(0), m_maxY(0)
        , m_clipX1(0), m_clipY1(0), m_clipX2(0), m_clipY2(0)
        , m_logicalFunction(wxCOPY)
        , m_backgroundMode(wxBRUSHSTYLE_TRANSPARENT)
        , m_mappingMode(wxMM_TEXT)
        , m_pen()
        , m_brush()
        , m_backgroundBrush()
        , m_textForegroundColour(*wxBLACK)
        , m_textBackgroundColour(*wxWHITE)
        , m_font()
#if wxUSE_PALETTE
        , m_palette()
        , m_hasCustomPalette(false)
#endif // wxUSE_PALETTE
        , m_devClipX1(0), m_devClipY1(0), m_devClipX2(0), m_devClipY2(0)
        , m_useDevClipCoords(false)
{
    m_owner = owner;
}

wxDCImpl::~wxDCImpl()
{
}

// ----------------------------------------------------------------------------
// clipping
// ----------------------------------------------------------------------------

void wxDCImpl::DoSetClippingRegion(wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{
    wxASSERT_MSG( w >= 0 && h >= 0,
                  wxS("Clipping box size values cannot be negative") );

    // If we set clipping box with this method we can operate on device coordinates
    // and calculate clipping box properly also when transformations were applied to DC.
    m_useDevClipCoords = true;
    wxRect clipRegion(LogicalToDevice(x, y), LogicalToDeviceRel(w, h));

    if ( m_clipping )
    {
        // New clipping box is an intersection
        // of required clipping box and the current one.
        wxRect curRegion(m_devClipX1, m_devClipY1, m_devClipX2 - m_devClipX1, m_devClipY2 - m_devClipY1);
        clipRegion.Intersect(curRegion);
    }
    else
    {
        // Effective clipping box is an intersection
        // of required clipping box and DC surface.
        int dcWidth, dcHeight;
        DoGetSize(&dcWidth, &dcHeight);
        wxRect dcRect(0, 0, dcWidth, dcHeight);
        clipRegion.Intersect(dcRect);

        m_clipping = true;
    }

    if ( clipRegion.IsEmpty() )
    {
        m_devClipX1 = m_devClipY1 = m_devClipX2 = m_devClipY2 = 0;
    }
    else
    {
        m_devClipX1 = clipRegion.GetLeft();
        m_devClipY1 = clipRegion.GetTop();
        m_devClipX2 = clipRegion.GetRight() + 1;
        m_devClipY2 = clipRegion.GetBottom() + 1;
    }
}

wxRect wxDCImpl::GetLogicalArea() const
{
    const wxSize size = GetSize();
    return wxRect(DeviceToLogical(0, 0), DeviceToLogicalRel(size.x, size.y));
}

bool wxDCImpl::DoGetClippingRect(wxRect& rect) const
{
#if WXWIN_COMPATIBILITY_3_0
    // Call the old function for compatibility.
    DoGetClippingBox(&rect.x, &rect.y, &rect.width, &rect.height);
    if ( rect != wxRect(-1, -1, 0, 0) )
    {
        // Custom overridden version of DoGetClippingBox() was called, we need
        // to check if there is an actual clipping region or not. Normally the
        // function is supposed to return the whole DC area (in logical
        // coordinates) in this case, but also check that the clipping region
        // is not empty because some implementations seem to do this instead.
        return !rect.IsEmpty() && rect != GetLogicalArea();
    }
#endif // WXWIN_COMPATIBILITY_3_0

    if ( m_clipping )
    {
        if ( m_useDevClipCoords )
        {
            if ( m_devClipX1 == m_devClipX2 || m_devClipY1 == m_devClipY2 )
                rect = wxRect(); // empty clip region
            else
                rect = wxRect(DeviceToLogical(m_devClipX1, m_devClipY1), DeviceToLogicalRel(m_devClipX2 - m_devClipX1, m_devClipY2 - m_devClipY1));
        }
        else
        {
            // When derived class set coordinates in logical units directly...
            rect = wxRect(m_clipX1, m_clipY1, m_clipX2 - m_clipX1, m_clipY2 - m_clipY1);
        }
        return true;
    }
    else // No active clipping region.
    {
        rect = GetLogicalArea();

        return false;
    }
}

#if WXWIN_COMPATIBILITY_3_0
void wxDCImpl::DoGetClippingBox(wxCoord *x, wxCoord *y,
                                wxCoord *w, wxCoord *h) const
{
    // Dummy implementation just to allow DoGetClippingRect() above to
    // determine if this version was called or not.
    if ( x )
        *x = -1;
    if ( y )
        *y = -1;
    if ( w )
        *w = 0;
    if ( h )
        *h = 0;
}
#endif // WXWIN_COMPATIBILITY_3_0

// ----------------------------------------------------------------------------
// coordinate conversions and transforms
// ----------------------------------------------------------------------------

wxCoord wxDCImpl::DeviceToLogicalX(wxCoord x) const
{
    return wxRound( (double)((x - m_deviceOriginX - m_deviceLocalOriginX) * m_signX) / m_scaleX ) + m_logicalOriginX ;
}

wxCoord wxDCImpl::DeviceToLogicalY(wxCoord y) const
{
    return wxRound( (double)((y - m_deviceOriginY - m_deviceLocalOriginY) * m_signY) / m_scaleY ) + m_logicalOriginY ;
}

wxCoord wxDCImpl::DeviceToLogicalXRel(wxCoord x) const
{
    return wxRound((double)(x) / m_scaleX);
}

wxCoord wxDCImpl::DeviceToLogicalYRel(wxCoord y) const
{
    return wxRound((double)(y) / m_scaleY);
}

wxCoord wxDCImpl::LogicalToDeviceX(wxCoord x) const
{
    return wxRound( (double)((x - m_logicalOriginX) * m_signX) * m_scaleX) + m_deviceOriginX + m_deviceLocalOriginX;
}

wxCoord wxDCImpl::LogicalToDeviceY(wxCoord y) const
{
    return wxRound( (double)((y - m_logicalOriginY) * m_signY) * m_scaleY) + m_deviceOriginY + m_deviceLocalOriginY;
}

wxCoord wxDCImpl::LogicalToDeviceXRel(wxCoord x) const
{
    return wxRound((double)(x) * m_scaleX);
}

wxCoord wxDCImpl::LogicalToDeviceYRel(wxCoord y) const
{
    return wxRound((double)(y) * m_scaleY);
}

wxPoint wxDCImpl::DeviceToLogical(wxCoord x, wxCoord y) const
{
    return wxPoint(DeviceToLogicalX(x), DeviceToLogicalY(y));
}

wxPoint wxDCImpl::LogicalToDevice(wxCoord x, wxCoord y) const
{
    return wxPoint(LogicalToDeviceX(x), LogicalToDeviceY(y));
}

wxSize wxDCImpl::DeviceToLogicalRel(int x, int y) const
{
    return wxSize(DeviceToLogicalXRel(x), DeviceToLogicalYRel(y));
}

wxSize wxDCImpl::LogicalToDeviceRel(int x, int y) const
{
    return wxSize(LogicalToDeviceXRel(x), LogicalToDeviceYRel(y));
}

void wxDCImpl::ComputeScaleAndOrigin()
{
    m_scaleX = m_logicalScaleX * m_userScaleX;
    m_scaleY = m_logicalScaleY * m_userScaleY;
}

void wxDCImpl::SetMapMode( wxMappingMode mode )
{
    switch (mode)
    {
        case wxMM_TWIPS:
          SetLogicalScale( twips2mm*GetMMToPXx(), twips2mm*GetMMToPXy() );
          break;
        case wxMM_POINTS:
          SetLogicalScale( pt2mm*GetMMToPXx(), pt2mm*GetMMToPXy() );
          break;
        case wxMM_METRIC:
          SetLogicalScale( GetMMToPXx(), GetMMToPXy() );
          break;
        case wxMM_LOMETRIC:
          SetLogicalScale( GetMMToPXx()/10.0, GetMMToPXy()/10.0 );
          break;
        default:
        case wxMM_TEXT:
          SetLogicalScale( 1.0, 1.0 );
          break;
    }
    m_mappingMode = mode;
}

void wxDCImpl::SetUserScale( double x, double y )
{
    // allow negative ? -> no
    m_userScaleX = x;
    m_userScaleY = y;
    ComputeScaleAndOrigin();
}

void wxDCImpl::SetLogicalScale( double x, double y )
{
    // allow negative ?
    m_logicalScaleX = x;
    m_logicalScaleY = y;
    ComputeScaleAndOrigin();
}

void wxDCImpl::SetLogicalOrigin( wxCoord x, wxCoord y )
{
    m_logicalOriginX = x * m_signX;
    m_logicalOriginY = y * m_signY;
    ComputeScaleAndOrigin();
}

void wxDCImpl::SetDeviceOrigin( wxCoord x, wxCoord y )
{
    m_deviceOriginX = x;
    m_deviceOriginY = y;
    ComputeScaleAndOrigin();
}

void wxDCImpl::SetDeviceLocalOrigin( wxCoord x, wxCoord y )
{
    m_deviceLocalOriginX = x;
    m_deviceLocalOriginY = y;
    ComputeScaleAndOrigin();
}

void wxDCImpl::SetAxisOrientation( bool xLeftRight, bool yBottomUp )
{
    // only wxPostScripDC has m_signX = -1, we override SetAxisOrientation there
    // wxWidgets 2.9: no longer override it
    m_signX = (xLeftRight ?  1 : -1);
    m_signY = (yBottomUp  ? -1 :  1);
    ComputeScaleAndOrigin();
}

wxSize wxDCImpl::FromDIP(const wxSize& sz) const
{
#ifdef wxHAS_DPI_INDEPENDENT_PIXELS
    return sz;
#else
    const wxSize dpi = GetPPI();
    const wxSize baseline = wxDisplay::GetStdPPI();
    return wxRescaleCoord(sz).From(baseline).To(dpi);
#endif // wxHAS_DPI_INDEPENDENT_PIXELS
}

wxSize wxDCImpl::ToDIP(const wxSize& sz) const
{
#ifdef wxHAS_DPI_INDEPENDENT_PIXELS
    return sz;
#else
    const wxSize dpi = GetPPI();
    const wxSize baseline = wxDisplay::GetStdPPI();
    return wxRescaleCoord(sz).From(dpi).To(baseline);
#endif // wxHAS_DPI_INDEPENDENT_PIXELS
}

bool wxDCImpl::DoGetPartialTextExtents(const wxString& text, wxArrayInt& widths) const
{
    wxTextMeasure tm(GetOwner(), &m_font);
    return tm.GetPartialTextExtents(text, widths, m_scaleX);
}

void wxDCImpl::GetMultiLineTextExtent(const wxString& text,
                                      wxCoord *x,
                                      wxCoord *y,
                                      wxCoord *h,
                                      const wxFont *font) const
{
    wxTextMeasure tm(GetOwner(), font && font->IsOk() ? font : &m_font);
    tm.GetMultiLineTextExtent(text, x, y, h);
}

void wxDCImpl::DoDrawCheckMark(wxCoord x1, wxCoord y1,
                               wxCoord width, wxCoord height)
{
    wxCHECK_RET( IsOk(), wxT("invalid window dc") );

    wxCoord x2 = x1 + width,
            y2 = y1 + height;

    // the pen width is calibrated to give 3 for width == height == 10
    wxDCPenChanger pen( *m_owner, wxPen(GetTextForeground(), (width + height + 1)/7));

    // we're drawing a scaled version of wx/generic/tick.xpm here
    wxCoord x3 = x1 + (4*width) / 10,   // x of the tick bottom
            y3 = y1 + height / 2;       // y of the left tick branch
    DoDrawLine(x1, y3, x3, y2);
    DoDrawLine(x3, y2, x2, y1);

    CalcBoundingBox(x1, y1, x2, y2);
}

bool
wxDCImpl::DoStretchBlit(wxCoord xdest, wxCoord ydest,
                        wxCoord dstWidth, wxCoord dstHeight,
                        wxDC *source,
                        wxCoord xsrc, wxCoord ysrc,
                        wxCoord srcWidth, wxCoord srcHeight,
                        wxRasterOperationMode rop,
                        bool useMask,
                        wxCoord xsrcMask,
                        wxCoord ysrcMask)
{
    wxCHECK_MSG( srcWidth && srcHeight && dstWidth && dstHeight, false,
                 wxT("invalid blit size") );

    // emulate the stretching by modifying the DC scale
    double xscale = (double)srcWidth/dstWidth,
           yscale = (double)srcHeight/dstHeight;

    // Shift origin to avoid imprecision of integer destination coordinates
    const int deviceOriginX = m_deviceOriginX;
    const int deviceOriginY = m_deviceOriginY;
    const int deviceLocalOriginX = m_deviceLocalOriginX;
    const int deviceLocalOriginY = m_deviceLocalOriginY;
    const int logicalOriginX = m_logicalOriginX;
    const int logicalOriginY = m_logicalOriginY;
    m_deviceOriginX = LogicalToDeviceX(xdest);
    m_deviceOriginY = LogicalToDeviceY(ydest);
    m_deviceLocalOriginX = 0;
    m_deviceLocalOriginY = 0;
    m_logicalOriginX = 0;
    m_logicalOriginY = 0;

    double xscaleOld, yscaleOld;
    GetUserScale(&xscaleOld, &yscaleOld);
    SetUserScale(xscaleOld/xscale, yscaleOld/yscale);

    bool rc = DoBlit(0, 0, srcWidth, srcHeight,
                     source,
                     xsrc, ysrc, rop, useMask, xsrcMask, ysrcMask);

    m_deviceOriginX = deviceOriginX;
    m_deviceOriginY = deviceOriginY;
    m_deviceLocalOriginX = deviceLocalOriginX;
    m_deviceLocalOriginY = deviceLocalOriginY;
    m_logicalOriginX = logicalOriginX;
    m_logicalOriginY = logicalOriginY;
    SetUserScale(xscaleOld, yscaleOld);

    return rc;
}

void wxDCImpl::DrawLines(const wxPointList *list, wxCoord xoffset, wxCoord yoffset)
{
    int n = list->GetCount();
    wxScopedArray<wxPoint> points(n);

    int i = 0;
    for ( wxPointList::compatibility_iterator node = list->GetFirst(); node; node = node->GetNext(), i++ )
    {
        wxPoint *point = node->GetData();
        points[i].x = point->x;
        points[i].y = point->y;
    }

    DoDrawLines(n, points.get(), xoffset, yoffset);
}

void wxDCImpl::DrawPolygon(const wxPointList *list,
                           wxCoord xoffset, wxCoord yoffset,
                           wxPolygonFillMode fillStyle)
{
    int n = list->GetCount();
    wxScopedArray<wxPoint> points(n);

    int i = 0;
    for ( wxPointList::compatibility_iterator node = list->GetFirst(); node; node = node->GetNext(), i++ )
    {
        wxPoint *point = node->GetData();
        points[i].x = point->x;
        points[i].y = point->y;
    }

    DoDrawPolygon(n, points.get(), xoffset, yoffset, fillStyle);
}

void
wxDCImpl::DoDrawPolyPolygon(int n,
                            const int count[],
                            const wxPoint points[],
                            wxCoord xoffset, wxCoord yoffset,
                            wxPolygonFillMode fillStyle)
{
    if ( n == 1 )
    {
        DoDrawPolygon(count[0], points, xoffset, yoffset, fillStyle);
        return;
    }

    int      i, j, lastOfs;

    for (i = j = lastOfs = 0; i < n; i++)
    {
        lastOfs = j;
        j      += count[i];
    }
    wxScopedArray<wxPoint> pts(j+n-1);
    for (i = 0; i < j; i++)
        pts[i] = points[i];
    for (i = 2; i <= n; i++)
    {
        lastOfs -= count[n-i];
        pts[j++] = pts[lastOfs];
    }

    {
        wxDCPenChanger setTransp(*m_owner, *wxTRANSPARENT_PEN);
        DoDrawPolygon(j, pts.get(), xoffset, yoffset, fillStyle);
    }

    for (i = j = 0; i < n; i++)
    {
        DoDrawLines(count[i], pts.get()+j, xoffset, yoffset);
        j += count[i];
    }
}

#if wxUSE_SPLINES

void wxDCImpl::DrawSpline(wxCoord x1, wxCoord y1,
                          wxCoord x2, wxCoord y2,
                          wxCoord x3, wxCoord y3)
{
    wxPoint points[] = { wxPoint(x1, y1), wxPoint(x2, y2), wxPoint(x3, y3) };
    DrawSpline(WXSIZEOF(points), points);
}

void wxDCImpl::DrawSpline(int n, const wxPoint points[])
{
    wxPointList list;
    for ( int i = 0; i < n; i++ )
        list.Append(const_cast<wxPoint*>(&points[i]));

    DrawSpline(&list);
}

// ----------------------------------- spline code ----------------------------------------

static
void wx_quadratic_spline(double a1, double b1, double a2, double b2,
                         double a3, double b3, double a4, double b4);
static
void wx_clear_stack();
static
bool wx_spline_pop(double *x1, double *y1, double *x2, double *y2, double *x3,
        double *y3, double *x4, double *y4);
static
void wx_spline_push(double x1, double y1, double x2, double y2, double x3, double y3,
          double x4, double y4);
static bool wx_spline_add_point(double x, double y);
static void wx_spline_draw_point_array(wxDC *dc);

static wxPointList wx_spline_point_list;

#define                half(z1, z2)        ((z1+z2)/2.0)
#define                THRESHOLD        5

/* iterative version */

void wx_quadratic_spline(double a1, double b1, double a2, double b2, double a3, double b3, double a4,
                 double b4)
{
    double x1, y1, x2, y2, x3, y3, x4, y4;

    wx_clear_stack();
    wx_spline_push(a1, b1, a2, b2, a3, b3, a4, b4);

    while (wx_spline_pop(&x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4)) {
        double xmid, ymid;
        xmid = (double)half(x2, x3);
        ymid = (double)half(y2, y3);
        if (fabs(x1 - xmid) < THRESHOLD && fabs(y1 - ymid) < THRESHOLD &&
            fabs(xmid - x4) < THRESHOLD && fabs(ymid - y4) < THRESHOLD) {
            wx_spline_add_point( x1, y1 );
            wx_spline_add_point( xmid, ymid );
        } else {
            wx_spline_push(xmid, ymid, (double)half(xmid, x3), (double)half(ymid, y3),
                 (double)half(x3, x4), (double)half(y3, y4), x4, y4);
            wx_spline_push(x1, y1, (double)half(x1, x2), (double)half(y1, y2),
                 (double)half(x2, xmid), (double)half(y2, ymid), xmid, ymid);
        }
    }
}

/* utilities used by spline drawing routines */

typedef struct wx_spline_stack_struct {
    double           x1, y1, x2, y2, x3, y3, x4, y4;
} Stack;

static wxStack<Stack> gs_wx_spline_stack;

void wx_clear_stack()
{
    gs_wx_spline_stack = wxStack<Stack>();
}

void wx_spline_push(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{
    Stack rec;
    rec.x1 = x1;
    rec.y1 = y1;
    rec.x2 = x2;
    rec.y2 = y2;
    rec.x3 = x3;
    rec.y3 = y3;
    rec.x4 = x4;
    rec.y4 = y4;
    gs_wx_spline_stack.push(rec);
}

bool wx_spline_pop(double *x1, double *y1, double *x2, double *y2,
                   double *x3, double *y3, double *x4, double *y4)
{
    if ( gs_wx_spline_stack.empty() )
        return false;

    const Stack& top = gs_wx_spline_stack.top();
    *x1 = top.x1;
    *y1 = top.y1;
    *x2 = top.x2;
    *y2 = top.y2;
    *x3 = top.x3;
    *y3 = top.y3;
    *x4 = top.x4;
    *y4 = top.y4;
    gs_wx_spline_stack.pop();
    return true;
}

static bool wx_spline_add_point(double x, double y)
{
    wxPoint *point = new wxPoint( wxRound(x), wxRound(y) );
    wx_spline_point_list.Append(point );
    return true;
}

static void wx_spline_draw_point_array(wxDC *dc)
{
    dc->DrawLines(&wx_spline_point_list, 0, 0 );
    wxPointList::compatibility_iterator node = wx_spline_point_list.GetFirst();
    while (node)
    {
        wxPoint *point = node->GetData();
        delete point;
        wx_spline_point_list.Erase(node);
        node = wx_spline_point_list.GetFirst();
    }
}

void wxDCImpl::DoDrawSpline( const wxPointList *points )
{
    wxCHECK_RET( IsOk(), wxT("invalid window dc") );
    wxCHECK_RET(points, "NULL pointer to spline points?");
    wxCHECK_RET(points->size() >= 2, "incomplete list of spline points?");

    const wxPoint *p;
    double           x1, y1, x2, y2;

    wxPointList::const_iterator itPt= points->begin();
    p = *itPt; ++itPt;
    x1 = p->x;
    y1 = p->y;

    p = *itPt; ++itPt;
    x2 = p->x;
    y2 = p->y;
    double cx1 = (double)((x1 + x2) / 2);
    double cy1 = (double)((y1 + y2) / 2);
    double cx2 = (double)((cx1 + x2) / 2);
    double cy2 = (double)((cy1 + y2) / 2);

    wx_spline_add_point(x1, y1);

    while ( itPt != points->end() )
    {
        x1 = x2;
        y1 = y2;
        p = *itPt; ++itPt;
        x2 = p->x;
        y2 = p->y;
        double cx4 = (double)(x1 + x2) / 2;
        double cy4 = (double)(y1 + y2) / 2;
        double cx3 = (double)(x1 + cx4) / 2;
        double cy3 = (double)(y1 + cy4) / 2;

        wx_quadratic_spline(cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4);

        cx1 = cx4;
        cy1 = cy4;
        cx2 = (double)(cx1 + x2) / 2;
        cy2 = (double)(cy1 + y2) / 2;
    }

    wx_spline_add_point( cx1, cy1 );
    wx_spline_add_point( x2, y2 );

    wx_spline_draw_point_array( m_owner );
}

#endif // wxUSE_SPLINES



void wxDCImpl::DoGradientFillLinear(const wxRect& rect,
                                    const wxColour& initialColour,
                                    const wxColour& destColour,
                                    wxDirection nDirection)
{
    // save old pen
    wxPen oldPen = m_pen;
    wxBrush oldBrush = m_brush;

    wxUint8 nR1 = initialColour.Red();
    wxUint8 nG1 = initialColour.Green();
    wxUint8 nB1 = initialColour.Blue();
    wxUint8 nR2 = destColour.Red();
    wxUint8 nG2 = destColour.Green();
    wxUint8 nB2 = destColour.Blue();
    wxUint8 nR, nG, nB;

    if ( nDirection == wxEAST || nDirection == wxWEST )
    {
        wxInt32 x = rect.GetWidth();
        wxInt32 w = x;              // width of area to shade
        wxInt32 xDelta = w/256;     // height of one shade bend
        if (xDelta < 1)
            xDelta = 1;

        while (x >= xDelta)
        {
            x -= xDelta;
            if (nR1 > nR2)
                nR = nR1 - (nR1-nR2)*(w-x)/w;
            else
                nR = nR1 + (nR2-nR1)*(w-x)/w;

            if (nG1 > nG2)
                nG = nG1 - (nG1-nG2)*(w-x)/w;
            else
                nG = nG1 + (nG2-nG1)*(w-x)/w;

            if (nB1 > nB2)
                nB = nB1 - (nB1-nB2)*(w-x)/w;
            else
                nB = nB1 + (nB2-nB1)*(w-x)/w;

            wxColour colour(nR,nG,nB);
            SetPen(wxPen(colour, 1, wxPENSTYLE_SOLID));
            SetBrush(wxBrush(colour));
            if(nDirection == wxEAST)
                DoDrawRectangle(rect.GetRight()-x-xDelta+1, rect.GetTop(),
                        xDelta, rect.GetHeight());
            else //nDirection == wxWEST
                DoDrawRectangle(rect.GetLeft()+x, rect.GetTop(),
                        xDelta, rect.GetHeight());
        }
    }
    else  // nDirection == wxNORTH || nDirection == wxSOUTH
    {
        wxInt32 y = rect.GetHeight();
        wxInt32 w = y;              // height of area to shade
        wxInt32 yDelta = w/255;     // height of one shade bend
        if (yDelta < 1)
            yDelta = 1;

        while (y > 0)
        {
            y -= yDelta;
            if (nR1 > nR2)
                nR = nR1 - (nR1-nR2)*(w-y)/w;
            else
                nR = nR1 + (nR2-nR1)*(w-y)/w;

            if (nG1 > nG2)
                nG = nG1 - (nG1-nG2)*(w-y)/w;
            else
                nG = nG1 + (nG2-nG1)*(w-y)/w;

            if (nB1 > nB2)
                nB = nB1 - (nB1-nB2)*(w-y)/w;
            else
                nB = nB1 + (nB2-nB1)*(w-y)/w;

            wxColour colour(nR,nG,nB);
            SetPen(wxPen(colour, 1, wxPENSTYLE_SOLID));
            SetBrush(wxBrush(colour));
            if(nDirection == wxNORTH)
                DoDrawRectangle(rect.GetLeft(), rect.GetTop()+y,
                        rect.GetWidth(), yDelta);
            else //nDirection == wxSOUTH
                DoDrawRectangle(rect.GetLeft(), rect.GetBottom()-y-yDelta+1,
                        rect.GetWidth(), yDelta);
        }
    }

    SetPen(oldPen);
    SetBrush(oldBrush);
}

void wxDCImpl::DoGradientFillConcentric(const wxRect& rect,
                                      const wxColour& initialColour,
                                      const wxColour& destColour,
                                      const wxPoint& circleCenter)
{
    // save the old pen and ensure it is restored on exit
    const wxPen penOrig = m_pen;
    wxON_BLOCK_EXIT_SET(m_pen, penOrig);

    wxUint8 nR1 = destColour.Red();
    wxUint8 nG1 = destColour.Green();
    wxUint8 nB1 = destColour.Blue();
    wxUint8 nR2 = initialColour.Red();
    wxUint8 nG2 = initialColour.Green();
    wxUint8 nB2 = initialColour.Blue();
    wxUint8 nR, nG, nB;


    //Radius
    double cx = rect.GetWidth() / 2;
    double cy = rect.GetHeight() / 2;
    double dRadius;
    if (cx < cy)
        dRadius = cx;
    else
        dRadius = cy;

    //Offset of circle
    double ptX, ptY;
    ptX = circleCenter.x;
    ptY = circleCenter.y;
    double nCircleOffX = ptX - cx;
    double nCircleOffY = ptY - cy;

    double dGradient;
    double dx, dy;

    for ( wxInt32 x = 0; x < rect.GetWidth(); x++ )
    {
        for ( wxInt32 y = 0; y < rect.GetHeight(); y++ )
        {
            //get color difference
            dx = x;
            dy = y;

            dGradient = ((dRadius - sqrt(  (dx - cx - nCircleOffX) * (dx - cx - nCircleOffX)
                                          +(dy - cy - nCircleOffY) * (dy - cy - nCircleOffY)
                                         )
                         ) * 100
                        ) / dRadius;

            //normalize Gradient
            if (dGradient < 0)
                dGradient = 0.0;

            //get dest colors
            nR = (wxUint8)(nR1 + ((nR2 - nR1) * dGradient / 100));
            nG = (wxUint8)(nG1 + ((nG2 - nG1) * dGradient / 100));
            nB = (wxUint8)(nB1 + ((nB2 - nB1) * dGradient / 100));

            //set the pixel
            SetPen(wxColour(nR,nG,nB));
            DoDrawPoint(x + rect.GetLeft(), y + rect.GetTop());
        }
    }
}

void wxDCImpl::InheritAttributes(wxWindow *win)
{
    wxCHECK_RET( win, "window can't be NULL" );

    SetFont(win->GetFont());
    SetTextForeground(win->GetForegroundColour());
    SetTextBackground(win->GetBackgroundColour());
    SetBackground(win->GetBackgroundColour());
    SetLayoutDirection(win->GetLayoutDirection());
}

void wxDCImpl::DoGetFontMetrics(int *height,
                                int *ascent,
                                int *descent,
                                int *internalLeading,
                                int *externalLeading,
                                int *averageWidth) const
{
    // Average width is typically the same as width of 'x'.
    wxCoord h, d;
    DoGetTextExtent("x", averageWidth, &h, &d, externalLeading);

    if ( height )
        *height = h;
    if ( ascent )
        *ascent = h - d;
    if ( descent )
        *descent = d;
    if ( internalLeading )
        *internalLeading = 0;
}

//-----------------------------------------------------------------------------
// wxDC
//-----------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxDC, wxObject);

void wxDC::CopyAttributes(const wxDC& dc)
{
    SetFont(dc.GetFont());
    SetTextForeground(dc.GetTextForeground());
    SetTextBackground(dc.GetTextBackground());
    SetBackground(dc.GetBackground());
    SetLayoutDirection(dc.GetLayoutDirection());
}

void wxDC::DrawLabel(const wxString& text,
                         const wxBitmap& bitmap,
                         const wxRect& rect,
                         int alignment,
                         int indexAccel,
                         wxRect *rectBounding)
{
    // find the text position
    wxCoord widthText, heightText, heightLine;
    GetMultiLineTextExtent(text, &widthText, &heightText, &heightLine);

    wxCoord width, height;
    if ( bitmap.IsOk() )
    {
        width = widthText + bitmap.GetWidth();
        height = bitmap.GetHeight();
    }
    else // no bitmap
    {
        width = widthText;
        height = heightText;
    }

    wxCoord x, y;
    if ( alignment & wxALIGN_RIGHT )
    {
        x = rect.GetRight() - width + 1;
    }
    else if ( alignment & wxALIGN_CENTRE_HORIZONTAL )
    {
        x = (rect.GetLeft() + rect.GetRight() + 1 - width) / 2;
    }
    else // alignment & wxALIGN_LEFT
    {
        x = rect.GetLeft();
    }

    if ( alignment & wxALIGN_BOTTOM )
    {
        y = rect.GetBottom() - height + 1;
    }
    else if ( alignment & wxALIGN_CENTRE_VERTICAL )
    {
        y = (rect.GetTop() + rect.GetBottom() + 1 - height) / 2;
    }
    else // alignment & wxALIGN_TOP
    {
        y = rect.GetTop();
    }

    // draw the bitmap first
    wxCoord x0 = x,
            y0 = y,
            width0 = width;
    if ( bitmap.IsOk() )
    {
        DrawBitmap(bitmap, x, y, true /* use mask */);

        wxCoord offset = bitmap.GetWidth() + 4;
        x += offset;
        width -= offset;

        y += (height - heightText) / 2;
    }

    // we will draw the underscore under the accel char later
    wxCoord startUnderscore = 0,
            endUnderscore = 0,
            yUnderscore = 0;

    // split the string into lines and draw each of them separately
    //
    // NB: while wxDC::DrawText() on some platforms supports drawing multi-line
    //     strings natively, this is not the case for all of them, notably not
    //     wxMSW which uses this function for multi-line texts, so we may only
    //     call DrawText() for single-line strings from here to avoid infinite
    //     recursion.
    wxString curLine;
    for ( wxString::const_iterator pc = text.begin(); ; ++pc )
    {
        if ( pc == text.end() || *pc == '\n' )
        {
            int xRealStart = x; // init it here to avoid compiler warnings

            if ( !curLine.empty() )
            {
                // NB: can't test for !(alignment & wxALIGN_LEFT) because
                //     wxALIGN_LEFT is 0
                if ( alignment & (wxALIGN_RIGHT | wxALIGN_CENTRE_HORIZONTAL) )
                {
                    wxCoord widthLine;
                    GetTextExtent(curLine, &widthLine, NULL);

                    if ( alignment & wxALIGN_RIGHT )
                    {
                        xRealStart += width - widthLine;
                    }
                    else // if ( alignment & wxALIGN_CENTRE_HORIZONTAL )
                    {
                        xRealStart += (width - widthLine) / 2;
                    }
                }
                //else: left aligned, nothing to do

                DrawText(curLine, xRealStart, y);
            }

            y += heightLine;

            // do we have underscore in this line? we can check yUnderscore
            // because it is set below to just y + heightLine if we do
            if ( y == yUnderscore )
            {
                // adjust the horz positions to account for the shift
                startUnderscore += xRealStart;
                endUnderscore += xRealStart;
            }

            if ( pc == text.end() )
                break;

            curLine.clear();
        }
        else // not end of line
        {
            if ( pc - text.begin() == indexAccel )
            {
                // remember to draw underscore here
                GetTextExtent(curLine, &startUnderscore, NULL);
                curLine += *pc;
                GetTextExtent(curLine, &endUnderscore, NULL);

                yUnderscore = y + heightLine;
            }
            else
            {
                curLine += *pc;
            }
        }
    }

    // draw the underscore if found
    if ( startUnderscore != endUnderscore )
    {
        // it should be of the same colour as text
        SetPen(wxPen(GetTextForeground(), 0, wxPENSTYLE_SOLID));

        // This adjustment is relatively arbitrary: we need to draw the
        // underline slightly higher to avoid overflowing the character cell
        // but whether we should do it 1, 2 or 3 pixels higher is not clear.
        //
        // The currently used value seems to be compatible with native MSW
        // behaviour, i.e. it results in the same appearance of the owner-drawn
        // and normal labels.
        yUnderscore -= 2;

        DrawLine(startUnderscore, yUnderscore, endUnderscore, yUnderscore);
    }

    // return bounding rect if requested
    if ( rectBounding )
    {
        *rectBounding = wxRect(x, y - heightText, widthText, heightText);
    }

    m_pimpl->CalcBoundingBox(wxPoint(x0, y0), wxSize(width0, height));
}

#if WXWIN_COMPATIBILITY_2_8
    // for compatibility with the old code when wxCoord was long everywhere
void wxDC::GetTextExtent(const wxString& string,
                       long *x, long *y,
                       long *descent,
                       long *externalLeading,
                       const wxFont *theFont) const
    {
        wxCoord x2, y2, descent2, externalLeading2;
        m_pimpl->DoGetTextExtent(string, &x2, &y2,
                        &descent2, &externalLeading2,
                        theFont);
        if ( x )
            *x = x2;
        if ( y )
            *y = y2;
        if ( descent )
            *descent = descent2;
        if ( externalLeading )
            *externalLeading = externalLeading2;
    }

void wxDC::GetLogicalOrigin(long *x, long *y) const
    {
        wxCoord x2, y2;
        m_pimpl->DoGetLogicalOrigin(&x2, &y2);
        if ( x )
            *x = x2;
        if ( y )
            *y = y2;
    }

void wxDC::GetDeviceOrigin(long *x, long *y) const
    {
        wxCoord x2, y2;
        m_pimpl->DoGetDeviceOrigin(&x2, &y2);
        if ( x )
            *x = x2;
        if ( y )
            *y = y2;
    }

void wxDC::GetClippingBox(long *x, long *y, long *w, long *h) const
    {
        wxRect r;
        m_pimpl->DoGetClippingRect(r);
        if (x) *x = r.x;
        if (y) *y = r.y;
        if (w) *w = r.width;
        if (h) *h = r.height;
    }

void wxDC::DrawObject(wxDrawObject* drawobject)
{
    drawobject->Draw(*this);
    CalcBoundingBox(drawobject->MinX(),drawobject->MinY(),
                    drawobject->MaxX(),drawobject->MaxY());
}

#endif  // WXWIN_COMPATIBILITY_2_8

/*
Notes for wxWidgets DrawEllipticArcRot(...)

wxDCBase::DrawEllipticArcRot(...) draws a rotated elliptic arc or an ellipse.
It uses wxDCBase::CalculateEllipticPoints(...) and wxDCBase::Rotate(...),
which are also new.

All methods are generic, so they can be implemented in wxDCBase.

CalculateEllipticPoints(...) fills a given list of wxPoints with some points
of an elliptic arc. The algorithm is pixel-based: In every row (in flat
parts) or every column (in steep parts) only one pixel is calculated.
Trigonometric calculation (sin, cos, tan, atan) is only done if the
starting angle is not equal to the ending angle. The calculation of the
pixels is done using simple arithmetic only and should perform not too
bad even on devices without floating point processor. I didn't test this yet.

Rotate(...) rotates a list of point pixel-based, you will see rounding errors.
For instance: an ellipse rotated 180 degrees is drawn
slightly different from the original.

The points are then moved to an array and used to draw a polyline and/or polygon
(with center added, the pie).
The result looks quite similar to the native ellipse, only e few pixels differ.

The performance on a desktop system (Athlon 1800, WinXP) is about 7 times
slower as DrawEllipse(...), which calls the native API.
An rotated ellipse outside the clipping region takes nearly the same time,
while an native ellipse outside takes nearly no time to draw.

If you draw an arc with this new method, you will see the starting and ending angles
are calculated properly.
If you use DrawEllipticArc(...), you will see they are only correct for circles
and not properly calculated for ellipses.

Peter Lenhard
p.lenhard@t-online.de
*/

float wxDCImpl::GetFontPointSizeAdjustment(float dpi)
{
    // wxMSW has long-standing bug where wxFont point size is interpreted as
    // "pixel size corresponding to given point size *on screen*". In other
    // words, on a typical 600dpi printer and a typical 96dpi screen, fonts
    // are ~6 times smaller when printing. Unfortunately, this bug is so severe
    // that *all* printing code has to account for it and consequently, other
    // ports need to emulate this bug too:
    return float(wxDisplay::GetStdPPIValue()) / dpi;
}

double wxDCImpl::GetMMToPXx() const
{
    if ( wxIsNullDouble(m_mm_to_pix_x) )
    {
        m_mm_to_pix_x = (double)wxGetDisplaySize().GetWidth() /
                        (double)wxGetDisplaySizeMM().GetWidth();
    }

    return m_mm_to_pix_x;
}

double wxDCImpl::GetMMToPXy() const
{
    if ( wxIsNullDouble(m_mm_to_pix_y) )
    {
        m_mm_to_pix_y = (double)wxGetDisplaySize().GetHeight() /
                        (double)wxGetDisplaySizeMM().GetHeight();
    }

    return m_mm_to_pix_y;
}
