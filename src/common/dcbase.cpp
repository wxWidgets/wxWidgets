/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/dcbase.cpp
// Purpose:     generic methods of the wxDC Class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     05/25/99
// RCS-ID:      $Id$
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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/dc.h"
#include "wx/dcclient.h"
#include "wx/dcmemory.h"
#include "wx/dcscreen.h"
#include "wx/dcprint.h"
#include "wx/prntbase.h"

#ifndef WX_PRECOMP
    #include "wx/math.h"
    #include "wx/module.h"
#endif

#ifdef __WXMSW__
    #include "wx/msw/dcclient.h"
    #include "wx/msw/dcmemory.h"
    #include "wx/msw/dcscreen.h"
#endif

#ifdef __WXGTK__
    #include "wx/gtk/dcclient.h"
    #include "wx/gtk/dcmemory.h"
    #include "wx/gtk/dcscreen.h"
#endif

#ifdef __WXMAC__
    #include "wx/mac/dcclient.h"
    #include "wx/mac/dcmemory.h"
    #include "wx/mac/dcscreen.h"
#endif

#ifdef __WXCOCOA__
    #include "wx/cocoa/dcclient.h"
    #include "wx/cocoa/dcmemory.h"
    #include "wx/cocoa/dcscreen.h"
#endif

#ifdef __WXX11__
    #include "wx/x11/dcclient.h"
    #include "wx/x11/dcmemory.h"
    #include "wx/x11/dcscreen.h"
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
    virtual bool OnInit() { return true; }
    virtual void OnExit() { wxDCFactory::Set(NULL); }

private:
    DECLARE_DYNAMIC_CLASS(wxDCFactoryCleanupModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxDCFactoryCleanupModule, wxModule)

//-----------------------------------------------------------------------------
// wxNativeDCFactory
//-----------------------------------------------------------------------------

wxDCImpl* wxNativeDCFactory::CreateWindowDC( wxWindowDC *owner )
{
    return new wxWindowDCImpl( owner );
}

wxDCImpl* wxNativeDCFactory::CreateWindowDC( wxWindowDC *owner, wxWindow *window )
{
    return new wxWindowDCImpl( owner, window );
}

wxDCImpl* wxNativeDCFactory::CreateClientDC( wxClientDC *owner )
{
    return new wxClientDCImpl( owner );
}

wxDCImpl* wxNativeDCFactory::CreateClientDC( wxClientDC *owner, wxWindow *window )
{
    return new wxClientDCImpl( owner, window );
}

wxDCImpl* wxNativeDCFactory::CreatePaintDC( wxPaintDC *owner )
{
    return new wxPaintDCImpl( owner );
}

wxDCImpl* wxNativeDCFactory::CreatePaintDC( wxPaintDC *owner, wxWindow *window )
{
    return new wxPaintDCImpl( owner, window );
}

wxDCImpl* wxNativeDCFactory::CreateMemoryDC( wxMemoryDC *owner )
{
    return new wxMemoryDCImpl( owner );
}

wxDCImpl* wxNativeDCFactory::CreateMemoryDC( wxMemoryDC *owner, wxBitmap &bitmap )
{
    return new wxMemoryDCImpl( owner, bitmap );
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

IMPLEMENT_ABSTRACT_CLASS(wxWindowDC, wxDC)

wxWindowDC::wxWindowDC(wxWindow *win)
          : wxDC(wxDCFactory::Get()->CreateWindowDC(this, win))
{
}

//-----------------------------------------------------------------------------
// wxClientDC
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxClientDC, wxWindowDC)

wxClientDC::wxClientDC(wxWindow *win)
          : wxWindowDC(wxDCFactory::Get()->CreateClientDC(this, win))
{
}

//-----------------------------------------------------------------------------
// wxMemoryDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMemoryDC, wxDC)

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

IMPLEMENT_ABSTRACT_CLASS(wxPaintDC, wxClientDC)

wxPaintDC::wxPaintDC(wxWindow *win)
         : wxClientDC(wxDCFactory::Get()->CreatePaintDC(this, win))
{
}

//-----------------------------------------------------------------------------
// wxScreenDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxScreenDC, wxWindowDC)

wxScreenDC::wxScreenDC()
          : wxDC(wxDCFactory::Get()->CreateScreenDC(this))
{
}

//-----------------------------------------------------------------------------
// wxPrinterDC
//-----------------------------------------------------------------------------

#if wxUSE_PRINTING_ARCHITECTURE

IMPLEMENT_DYNAMIC_CLASS(wxPrinterDC, wxDC)

wxPrinterDC::wxPrinterDC()
           : wxDC(wxDCFactory::Get()->CreatePrinterDC(this, wxPrintData()))
{
}

wxPrinterDC::wxPrinterDC(const wxPrintData& data)
           : wxDC(wxDCFactory::Get()->CreatePrinterDC(this, data))
{
}

wxRect wxPrinterDC::GetPaperRect()
{
    return GetImpl()->GetPaperRect();
}

int wxPrinterDC::GetResolution()
{
    return GetImpl()->GetResolution();
}

#endif // wxUSE_PRINTING_ARCHITECTURE

//-----------------------------------------------------------------------------
// wxDCImpl
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxDCImpl, wxObject)

wxDCImpl::wxDCImpl( wxDC *owner )
        : m_window(NULL)
        , m_colour(wxColourDisplay())
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
        , m_minX(0), m_minY(0), m_maxX(0), m_maxY(0)
        , m_clipX1(0), m_clipY1(0), m_clipX2(0), m_clipY2(0)
        , m_logicalFunction(wxCOPY)
        , m_backgroundMode(wxTRANSPARENT)
        , m_mappingMode(wxMM_TEXT)
        , m_pen()
        , m_brush()
        , m_backgroundBrush(*wxTRANSPARENT_BRUSH)
        , m_textForegroundColour(*wxBLACK)
        , m_textBackgroundColour(*wxWHITE)
        , m_font()
#if wxUSE_PALETTE
        , m_palette()
        , m_hasCustomPalette(false)
#endif // wxUSE_PALETTE
{
    m_owner = owner;

    m_mm_to_pix_x = (double)wxGetDisplaySize().GetWidth() /
                    (double)wxGetDisplaySizeMM().GetWidth();
    m_mm_to_pix_y = (double)wxGetDisplaySize().GetHeight() /
                    (double)wxGetDisplaySizeMM().GetHeight();

    ResetBoundingBox();
    ResetClipping();
}

wxDCImpl::~wxDCImpl()
{
}

// ----------------------------------------------------------------------------
// coordinate conversions and transforms
// ----------------------------------------------------------------------------

wxCoord wxDCImpl::DeviceToLogicalX(wxCoord x) const
{
    return wxRound((double)(x - m_deviceOriginX - m_deviceLocalOriginX) / m_scaleX) * m_signX + m_logicalOriginX;
}

wxCoord wxDCImpl::DeviceToLogicalY(wxCoord y) const
{
    return wxRound((double)(y - m_deviceOriginY - m_deviceLocalOriginY) / m_scaleY) * m_signY + m_logicalOriginY;
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
    return wxRound((double)(x - m_logicalOriginX) * m_scaleX) * m_signX + m_deviceOriginX * m_signY + m_deviceLocalOriginX;
}

wxCoord wxDCImpl::LogicalToDeviceY(wxCoord y) const
{
    return wxRound((double)(y - m_logicalOriginY) * m_scaleY) * m_signY + m_deviceOriginY * m_signY + m_deviceLocalOriginY;
}

wxCoord wxDCImpl::LogicalToDeviceXRel(wxCoord x) const
{
    return wxRound((double)(x) * m_scaleX);
}

wxCoord wxDCImpl::LogicalToDeviceYRel(wxCoord y) const
{
    return wxRound((double)(y) * m_scaleY);
}

void wxDCImpl::ComputeScaleAndOrigin()
{
    m_scaleX = m_logicalScaleX * m_userScaleX;
    m_scaleY = m_logicalScaleY * m_userScaleY;
}

void wxDCImpl::SetMapMode( int mode )
{
    switch (mode)
    {
        case wxMM_TWIPS:
          SetLogicalScale( twips2mm*m_mm_to_pix_x, twips2mm*m_mm_to_pix_y );
          break;
        case wxMM_POINTS:
          SetLogicalScale( pt2mm*m_mm_to_pix_x, pt2mm*m_mm_to_pix_y );
          break;
        case wxMM_METRIC:
          SetLogicalScale( m_mm_to_pix_x, m_mm_to_pix_y );
          break;
        case wxMM_LOMETRIC:
          SetLogicalScale( m_mm_to_pix_x/10.0, m_mm_to_pix_y/10.0 );
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


// Each element of the widths array will be the width of the string up to and
// including the corresponding character in text.  This is the generic
// implementation, the port-specific classes should do this with native APIs
// if available and if faster.  Note: pango_layout_index_to_pos is much slower
// than calling GetTextExtent!!

#define FWC_SIZE 256

class FontWidthCache
{
public:
    FontWidthCache() : m_scaleX(1), m_widths(NULL) { }
    ~FontWidthCache() { delete []m_widths; }

    void Reset()
    {
        if (!m_widths)
            m_widths = new int[FWC_SIZE];

        memset(m_widths, 0, sizeof(int)*FWC_SIZE);
    }

    wxFont m_font;
    double m_scaleX;
    int *m_widths;
};

static FontWidthCache s_fontWidthCache;

bool wxDCImpl::DoGetPartialTextExtents(const wxString& text, wxArrayInt& widths) const
{
    int totalWidth = 0;

    const size_t len = text.length();
    widths.Empty();
    widths.Add(0, len);

    // reset the cache if font or horizontal scale have changed
    if ( !s_fontWidthCache.m_widths ||
         !wxIsSameDouble(s_fontWidthCache.m_scaleX, m_scaleX) ||
         (s_fontWidthCache.m_font != GetFont()) )
    {
        s_fontWidthCache.Reset();
        s_fontWidthCache.m_font = GetFont();
        s_fontWidthCache.m_scaleX = m_scaleX;
    }

    // Calculate the position of each character based on the widths of
    // the previous characters
    int w, h;
    for ( size_t i = 0; i < len; i++ )
    {
        const wxChar c = text[i];
        unsigned int c_int = (unsigned int)c;

        if ((c_int < FWC_SIZE) && (s_fontWidthCache.m_widths[c_int] != 0))
        {
            w = s_fontWidthCache.m_widths[c_int];
        }
        else
        {
            DoGetTextExtent(c, &w, &h);
            if (c_int < FWC_SIZE)
                s_fontWidthCache.m_widths[c_int] = w;
        }

        totalWidth += w;
        widths[i] = totalWidth;
    }

    return true;
}

void wxDCImpl::GetMultiLineTextExtent(const wxString& text,
                                      wxCoord *x,
                                      wxCoord *y,
                                      wxCoord *h,
                                      const wxFont *font) const
{
    wxCoord widthTextMax = 0, widthLine,
            heightTextTotal = 0, heightLineDefault = 0, heightLine = 0;

    wxString curLine;
    for ( wxString::const_iterator pc = text.begin(); ; ++pc )
    {
        if ( pc == text.end() || *pc == _T('\n') )
        {
            if ( curLine.empty() )
            {
                // we can't use GetTextExtent - it will return 0 for both width
                // and height and an empty line should count in height
                // calculation

                // assume that this line has the same height as the previous
                // one
                if ( !heightLineDefault )
                    heightLineDefault = heightLine;

                if ( !heightLineDefault )
                {
                    // but we don't know it yet - choose something reasonable
                    DoGetTextExtent(_T("W"), NULL, &heightLineDefault,
                                  NULL, NULL, font);
                }

                heightTextTotal += heightLineDefault;
            }
            else
            {
                DoGetTextExtent(curLine, &widthLine, &heightLine,
                              NULL, NULL, font);
                if ( widthLine > widthTextMax )
                    widthTextMax = widthLine;
                heightTextTotal += heightLine;
            }

            if ( pc == text.end() )
            {
               break;
            }
            else // '\n'
            {
               curLine.clear();
            }
        }
        else
        {
            curLine += *pc;
        }
    }

    if ( x )
        *x = widthTextMax;
    if ( y )
        *y = heightTextTotal;
    if ( h )
        *h = heightLine;
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

    CalcBoundingBox(x1, y1);
    CalcBoundingBox(x2, y2);
}

bool
wxDCImpl::DoStretchBlit(wxCoord xdest, wxCoord ydest,
                        wxCoord dstWidth, wxCoord dstHeight,
                        wxDC *source,
                        wxCoord xsrc, wxCoord ysrc,
                        wxCoord srcWidth, wxCoord srcHeight,
                        int rop,
                        bool useMask,
                        wxCoord xsrcMask,
                        wxCoord ysrcMask)
{
    wxCHECK_MSG( srcWidth && srcHeight && dstWidth && dstHeight, false,
                 _T("invalid blit size") );

    // emulate the stretching by modifying the DC scale
    double xscale = (double)srcWidth/dstWidth,
           yscale = (double)srcHeight/dstHeight;

    double xscaleOld, yscaleOld;
    GetUserScale(&xscaleOld, &yscaleOld);
    SetUserScale(xscaleOld/xscale, yscaleOld/yscale);

    bool rc = DoBlit(wxCoord(xdest*xscale), wxCoord(ydest*yscale),
                     wxCoord(dstWidth*xscale), wxCoord(dstHeight*yscale),
                     source,
                     xsrc, ysrc, rop, useMask, xsrcMask, ysrcMask);

    SetUserScale(xscaleOld, yscaleOld);

    return rc;
}

void wxDCImpl::DrawLines(const wxPointList *list, wxCoord xoffset, wxCoord yoffset)
{
    int n = list->GetCount();
    wxPoint *points = new wxPoint[n];

    int i = 0;
    for ( wxPointList::compatibility_iterator node = list->GetFirst(); node; node = node->GetNext(), i++ )
    {
        wxPoint *point = node->GetData();
        points[i].x = point->x;
        points[i].y = point->y;
    }

    DoDrawLines(n, points, xoffset, yoffset);

    delete [] points;
}

void wxDCImpl::DrawPolygon(const wxPointList *list,
                           wxCoord xoffset, wxCoord yoffset,
                           int fillStyle)
{
    int n = list->GetCount();
    wxPoint *points = new wxPoint[n];

    int i = 0;
    for ( wxPointList::compatibility_iterator node = list->GetFirst(); node; node = node->GetNext(), i++ )
    {
        wxPoint *point = node->GetData();
        points[i].x = point->x;
        points[i].y = point->y;
    }

    DoDrawPolygon(n, points, xoffset, yoffset, fillStyle);

    delete [] points;
}

void
wxDCImpl::DoDrawPolyPolygon(int n,
                            int count[],
                            wxPoint points[],
                            wxCoord xoffset, wxCoord yoffset,
                            int fillStyle)
{
    if ( n == 1 )
    {
        DoDrawPolygon(count[0], points, xoffset, yoffset, fillStyle);
        return;
    }

    int      i, j, lastOfs;
    wxPoint* pts;
    wxPen    pen;

    for (i = j = lastOfs = 0; i < n; i++)
    {
        lastOfs = j;
        j      += count[i];
    }
    pts = new wxPoint[j+n-1];
    for (i = 0; i < j; i++)
        pts[i] = points[i];
    for (i = 2; i <= n; i++)
    {
        lastOfs -= count[n-i];
        pts[j++] = pts[lastOfs];
    }

    pen = GetPen();
    SetPen(wxPen(*wxBLACK, 0, wxTRANSPARENT));
    DoDrawPolygon(j, pts, xoffset, yoffset, fillStyle);
    SetPen(pen);
    for (i = j = 0; i < n; i++)
    {
        DoDrawLines(count[i], pts+j, xoffset, yoffset);
        j += count[i];
    }
    delete[] pts;
}

#if wxUSE_SPLINES

void wxDCImpl::DoDrawSpline(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2, wxCoord x3, wxCoord y3)
{
    wxPointList point_list;

    wxPoint *point1 = new wxPoint;
    point1->x = x1; point1->y = y1;
    point_list.Append( point1 );

    wxPoint *point2 = new wxPoint;
    point2->x = x2; point2->y = y2;
    point_list.Append( point2 );

    wxPoint *point3 = new wxPoint;
    point3->x = x3; point3->y = y3;
    point_list.Append( point3 );

    DoDrawSpline(&point_list);

    for( wxPointList::compatibility_iterator node = point_list.GetFirst(); node; node = node->GetNext() )
    {
        wxPoint *p = node->GetData();
        delete p;
    }
}

void wxDCImpl::DoDrawSpline(int n, wxPoint points[])
{
    wxPointList list;
    for (int i =0; i < n; i++)
        list.Append( &points[i] );

    DoDrawSpline(&list);
}

// ----------------------------------- spline code ----------------------------------------

void wx_quadratic_spline(double a1, double b1, double a2, double b2,
                         double a3, double b3, double a4, double b4);
void wx_clear_stack();
int wx_spline_pop(double *x1, double *y1, double *x2, double *y2, double *x3,
        double *y3, double *x4, double *y4);
void wx_spline_push(double x1, double y1, double x2, double y2, double x3, double y3,
          double x4, double y4);
static bool wx_spline_add_point(double x, double y);
static void wx_spline_draw_point_array(wxDC *dc);

wxPointList wx_spline_point_list;

#define                half(z1, z2)        ((z1+z2)/2.0)
#define                THRESHOLD        5

/* iterative version */

void wx_quadratic_spline(double a1, double b1, double a2, double b2, double a3, double b3, double a4,
                 double b4)
{
    register double  xmid, ymid;
    double           x1, y1, x2, y2, x3, y3, x4, y4;

    wx_clear_stack();
    wx_spline_push(a1, b1, a2, b2, a3, b3, a4, b4);

    while (wx_spline_pop(&x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4)) {
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

#define         SPLINE_STACK_DEPTH             20
static Stack    wx_spline_stack[SPLINE_STACK_DEPTH];
static Stack   *wx_stack_top;
static int      wx_stack_count;

void wx_clear_stack()
{
    wx_stack_top = wx_spline_stack;
    wx_stack_count = 0;
}

void wx_spline_push(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{
    wx_stack_top->x1 = x1;
    wx_stack_top->y1 = y1;
    wx_stack_top->x2 = x2;
    wx_stack_top->y2 = y2;
    wx_stack_top->x3 = x3;
    wx_stack_top->y3 = y3;
    wx_stack_top->x4 = x4;
    wx_stack_top->y4 = y4;
    wx_stack_top++;
    wx_stack_count++;
}

int wx_spline_pop(double *x1, double *y1, double *x2, double *y2,
                  double *x3, double *y3, double *x4, double *y4)
{
    if (wx_stack_count == 0)
        return (0);
    wx_stack_top--;
    wx_stack_count--;
    *x1 = wx_stack_top->x1;
    *y1 = wx_stack_top->y1;
    *x2 = wx_stack_top->x2;
    *y2 = wx_stack_top->y2;
    *x3 = wx_stack_top->x3;
    *y3 = wx_stack_top->y3;
    *x4 = wx_stack_top->x4;
    *y4 = wx_stack_top->y4;
    return (1);
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

    wxPoint *p;
    double           cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4;
    double           x1, y1, x2, y2;

    wxPointList::compatibility_iterator node = points->GetFirst();
    if (!node)
        // empty list
        return;

    p = (wxPoint *)node->GetData();

    x1 = p->x;
    y1 = p->y;

    node = node->GetNext();
    p = node->GetData();

    x2 = p->x;
    y2 = p->y;
    cx1 = (double)((x1 + x2) / 2);
    cy1 = (double)((y1 + y2) / 2);
    cx2 = (double)((cx1 + x2) / 2);
    cy2 = (double)((cy1 + y2) / 2);

    wx_spline_add_point(x1, y1);

    while ((node = node->GetNext())
#if !wxUSE_STL
           != NULL
#endif // !wxUSE_STL
          )
    {
        p = node->GetData();
        x1 = x2;
        y1 = y2;
        x2 = p->x;
        y2 = p->y;
        cx4 = (double)(x1 + x2) / 2;
        cy4 = (double)(y1 + y2) / 2;
        cx3 = (double)(x1 + cx4) / 2;
        cy3 = (double)(y1 + cy4) / 2;

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
            SetPen(wxPen(colour, 1, wxSOLID));
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
            SetPen(wxPen(colour, 1, wxSOLID));
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
    //save the old pen color
    wxColour oldPenColour = m_pen.GetColour();

    wxUint8 nR1 = destColour.Red();
    wxUint8 nG1 = destColour.Green();
    wxUint8 nB1 = destColour.Blue();
    wxUint8 nR2 = initialColour.Red();
    wxUint8 nG2 = initialColour.Green();
    wxUint8 nB2 = initialColour.Blue();
    wxUint8 nR, nG, nB;


    //Radius
    wxInt32 cx = rect.GetWidth() / 2;
    wxInt32 cy = rect.GetHeight() / 2;
    wxInt32 nRadius;
    if (cx < cy)
        nRadius = cx;
    else
        nRadius = cy;

    //Offset of circle
    wxInt32 nCircleOffX = circleCenter.x - (rect.GetWidth() / 2);
    wxInt32 nCircleOffY = circleCenter.y - (rect.GetHeight() / 2);

    for ( wxInt32 x = 0; x < rect.GetWidth(); x++ )
    {
        for ( wxInt32 y = 0; y < rect.GetHeight(); y++ )
        {
            //get color difference
            wxInt32 nGradient = ((nRadius -
                                  (wxInt32)sqrt(
                                    pow((double)(x - cx - nCircleOffX), 2) +
                                    pow((double)(y - cy - nCircleOffY), 2)
                                  )) * 100) / nRadius;

            //normalize Gradient
            if (nGradient < 0 )
                nGradient = 0;

            //get dest colors
            nR = (wxUint8)(nR1 + ((nR2 - nR1) * nGradient / 100));
            nG = (wxUint8)(nG1 + ((nG2 - nG1) * nGradient / 100));
            nB = (wxUint8)(nB1 + ((nB2 - nB1) * nGradient / 100));

            //set the pixel
            m_pen.SetColour(wxColour(nR,nG,nB));
            DoDrawPoint(x + rect.GetLeft(), y + rect.GetTop());
        }
    }
    //return old pen color
    m_pen.SetColour(oldPenColour);
}

//-----------------------------------------------------------------------------
// wxDC
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxDC, wxObject)

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
    if ( bitmap.Ok() )
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
        x = rect.GetRight() - width;
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
        y = rect.GetBottom() - height;
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
    if ( bitmap.Ok() )
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
    wxString curLine;
    for ( wxString::const_iterator pc = text.begin(); ; ++pc )
    {
        if ( *pc == _T('\n') || pc == text.end() )
        {
            int xRealStart = x; // init it here to avoid compielr warnings

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
                // remeber to draw underscore here
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
        SetPen(wxPen(GetTextForeground(), 0, wxSOLID));

        yUnderscore--;

        DrawLine(startUnderscore, yUnderscore, endUnderscore, yUnderscore);
    }

    // return bounding rect if requested
    if ( rectBounding )
    {
        *rectBounding = wxRect(x, y - heightText, widthText, heightText);
    }

    CalcBoundingBox(x0, y0);
    CalcBoundingBox(x0 + width0, y0 + height);
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
        wxCoord xx,yy,ww,hh;
        m_pimpl->DoGetClippingBox(&xx, &yy, &ww, &hh);
        if (x) *x = xx;
        if (y) *y = yy;
        if (w) *w = ww;
        if (h) *h = hh;
    }

#endif  // WXWIN_COMPATIBILITY_2_8
