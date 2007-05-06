/////////////////////////////////////////////////////////////////////////////
// Name:        dc.cpp
// Purpose:
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dc.h"


#if wxUSE_NEW_DC

//----------------------------------------------------------------------------
// wxDCFactory
//----------------------------------------------------------------------------

wxDCFactory *wxDCFactory::m_factory = NULL;

void wxDCFactory::SetDCFactory( wxDCFactory *factory )
{
    if (wxDCFactory::m_factory)
        delete wxDCFactory::m_factory;

    wxDCFactory::m_factory = factory;
}

wxDCFactory *wxDCFactory::GetFactory()
{
    if (!wxDCFactory::m_factory)
        wxDCFactory::m_factory = new wxNativeDCFactory;

    return wxDCFactory::m_factory;
}

//-----------------------------------------------------------------------------
// wxNativeDCFactory
//-----------------------------------------------------------------------------

wxImplDC* wxNativeDCFactory::CreateWindowDC( wxWindow *window )
{ 
#if defined(__WXMSW__)
    return new wxWindowsWindowImplDC( window );
#elif defined(__WXGTK20__)
    return new wxGTKWindowImplDC( window );
#elif defined(__WXGTK__)
    return new wxGTKWindowImplDC( window );
#elif defined(__WXMAC__)
    return new wxMacWindowImplDC( window );
#elif defined(__WXCOCOA__)
    return new wxCocoaWindowImplDC( window );
#elif defined(__WXMOTIF__)
    return new wxMotifWindowImplDC( window );
#elif defined(__WXX11__)
    return new wxX11WindowImplDC( window );
#elif defined(__WXMGL__)
    return new wxMGLWindowImplDC( window );
#elif defined(__WXDFB__)
    return new wxDFBWindowImplDC( window );
#elif defined(__WXPM__)
    return new wxPMWindowImplDC( window );
#elif defined(__PALMOS__)
    return new wxPalmWindowImplDC( window );
#endif
}

wxImplDC* wxNativeDCFactory::CreateClientDC( wxWindow *window )
{
#if defined(__WXMSW__)
    return new wxWindowsClientImplDC( window );
#elif defined(__WXGTK20__)
    return new wxGTKClientImplDC( window );
#elif defined(__WXGTK__)
    return new wxGTKClientImplDC( window );
#elif defined(__WXMAC__)
    return new wxMacClientImplDC( window );
#elif defined(__WXCOCOA__)
    return new wxCocoaClientImplDC( window );
#elif defined(__WXMOTIF__)
    return new wxMotifClientImplDC( window );
#elif defined(__WXX11__)
    return new wxX11ClientImplDC( window );
#elif defined(__WXMGL__)
    return new wxMGLClientImplDC( window );
#elif defined(__WXDFB__)
    return new wxDFBClientImplDC( window );
#elif defined(__WXPM__)
    return new wxPMClientImplDC( window );
#elif defined(__PALMOS__)
    return new wxPalmClientImplDC( window );
#endif
}

wxImplDC* wxNativeDCFactory::CreatePaintDC( wxWindow *window )
{
#if defined(__WXMSW__)
    return new wxWindowsPaintImplDC( window );
#elif defined(__WXGTK20__)
    return new wxGTKPaintImplDC( window );
#elif defined(__WXGTK__)
    return new wxGTKPaintImplDC( window );
#elif defined(__WXMAC__)
    return new wxMacPaintImplDC( window );
#elif defined(__WXCOCOA__)
    return new wxCocoaPaintImplDC( window );
#elif defined(__WXMOTIF__)
    return new wxMotifPaintImplDC( window );
#elif defined(__WXX11__)
    return new wxX11PaintImplDC( window );
#elif defined(__WXMGL__)
    return new wxMGLPaintImplDC( window );
#elif defined(__WXDFB__)
    return new wxDFBPaintImplDC( window );
#elif defined(__WXPM__)
    return new wxPMPaintImplDC( window );
#elif defined(__PALMOS__)
    return new wxPalmPaintImplDC( window );
#endif
}

wxImplDC* wxNativeDCFactory::CreateMemoryDC()
{
#if defined(__WXMSW__)
    return new wxWindowsMemoryImplDC();
#elif defined(__WXGTK20__)
    return new wxGTKMemoryImplDC();
#elif defined(__WXGTK__)
    return new wxGTKMemoryImplDC();
#elif defined(__WXMAC__)
    return new wxMacMemoryImplDC();
#elif defined(__WXCOCOA__)
    return new wxCocoaMemoryImplDC();
#elif defined(__WXMOTIF__)
    return new wxMotifMemoryImplDC();
#elif defined(__WXX11__)
    return new wxX11MemoryImplDC();
#elif defined(__WXMGL__)
    return new wxMGLMemoryImplDC();
#elif defined(__WXDFB__)
    return new wxDFBMemoryImplDC();
#elif defined(__WXPM__)
    return new wxPMMemoryImplDC();
#elif defined(__PALMOS__)
    return new wxPalmMemoryImplDC();
#endif
}

wxImplDC* wxNativeDCFactory::CreateMemoryDC( wxBitmap &bitmap )
{
#if defined(__WXMSW__)
    return new wxWindowsMemoryImplDC( bitmap );
#elif defined(__WXGTK20__)
    return new wxGTKMemoryImplDC( bitmap );
#elif defined(__WXGTK__)
    return new wxGTKMemoryImplDC( bitmap );
#elif defined(__WXMAC__)
    return new wxMacMemoryImplDC( bitmap );
#elif defined(__WXCOCOA__)
    return new wxCocoaMemoryImplDC( bitmap );
#elif defined(__WXMOTIF__)
    return new wxMotifMemoryImplDC( bitmap );
#elif defined(__WXX11__)
    return new wxX11MemoryImplDC( bitmap );
#elif defined(__WXMGL__)
    return new wxMGLMemoryImplDC( bitmap );
#elif defined(__WXDFB__)
    return new wxDFBMemoryImplDC( bitmap );
#elif defined(__WXPM__)
    return new wxPMMemoryImplDC( bitmap );
#elif defined(__PALMOS__)
    return new wxPalmMemoryImplDC( bitmap );
#endif
}

wxImplDC* wxNativeDCFactory::CreateMemoryDC( wxDC *dc )
{
#if defined(__WXMSW__)
    return new wxWindowsMemoryImplDC( dc );
#elif defined(__WXGTK20__)
    return new wxGTKMemoryImplDC( dc );
#elif defined(__WXGTK__)
    return new wxGTKMemoryImplDC( dc );
#elif defined(__WXMAC__)
    return new wxMacMemoryImplDC( dc );
#elif defined(__WXCOCOA__)
    return new wxCocoaMemoryImplDC( dc );
#elif defined(__WXMOTIF__)
    return new wxMotifMemoryImplDC( dc );
#elif defined(__WXX11__)
    return new wxX11MemoryImplDC( dc );
#elif defined(__WXMGL__)
    return new wxMGLMemoryImplDC( dc );
#elif defined(__WXDFB__)
    return new wxDFBMemoryImplDC( dc );
#elif defined(__WXPM__)
    return new wxPMMemoryImplDC( dc );
#elif defined(__PALMOS__)
    return new wxPalmMemoryImplDC( dc );
#endif
}

//-----------------------------------------------------------------------------
// wxImplDC
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxImplDC, wxObject)

wxImplDC::wxImplDC( wxDC *owner )
        : m_colour(wxColourDisplay())
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

wxImplDC::~wxImplDC()
{
}

#if WXWIN_COMPATIBILITY_2_8
    // for compatibility with the old code when wxCoord was long everywhere
void wxImplDC::GetTextExtent(const wxString& string,
                       long *x, long *y,
                       long *descent,
                       long *externalLeading,
                       const wxFont *theFont) const
    {
        wxCoord x2, y2, descent2, externalLeading2;
        DoGetTextExtent(string, &x2, &y2,
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

void wxImplDC::GetLogicalOrigin(long *x, long *y) const 
    {
        wxCoord x2, y2;
        DoGetLogicalOrigin(&x2, &y2);
        if ( x )
            *x = x2;
        if ( y )
            *y = y2;
    }

void wxImplDC::GetDeviceOrigin(long *x, long *y) const 
    {
        wxCoord x2, y2;
        DoGetDeviceOrigin(&x2, &y2);
        if ( x )
            *x = x2;
        if ( y )
            *y = y2;
    } 
    
void wxImplDC::GetClippingBox(long *x, long *y, long *w, long *h) const 
    {
        wxCoord xx,yy,ww,hh;
        DoGetClippingBox(&xx, &yy, &ww, &hh);
        if (x) *x = xx;
        if (y) *y = yy;
        if (w) *w = ww;
        if (h) *h = hh;
    } 
#endif  // WXWIN_COMPATIBILITY_2_8



// ----------------------------------------------------------------------------
// coordinate conversions and transforms
// ----------------------------------------------------------------------------

wxCoord wxImplDC::DeviceToLogicalX(wxCoord x) const
{
    return wxRound((double)(x - m_deviceOriginX - m_deviceLocalOriginX) / m_scaleX) * m_signX + m_logicalOriginX;
}

wxCoord wxImplDC::DeviceToLogicalY(wxCoord y) const
{
    return wxRound((double)(y - m_deviceOriginY - m_deviceLocalOriginY) / m_scaleY) * m_signY + m_logicalOriginY;
}

wxCoord wxImplDC::DeviceToLogicalXRel(wxCoord x) const
{
    return wxRound((double)(x) / m_scaleX);
}

wxCoord wxImplDC::DeviceToLogicalYRel(wxCoord y) const
{
    return wxRound((double)(y) / m_scaleY);
}

wxCoord wxImplDC::LogicalToDeviceX(wxCoord x) const
{
    return wxRound((double)(x - m_logicalOriginX) * m_scaleX) * m_signX + m_deviceOriginX + m_deviceLocalOriginX;
}

wxCoord wxImplDC::LogicalToDeviceY(wxCoord y) const
{
    return wxRound((double)(y - m_logicalOriginY) * m_scaleY) * m_signY + m_deviceOriginY + m_deviceLocalOriginY;
}

wxCoord wxImplDC::LogicalToDeviceXRel(wxCoord x) const
{
    return wxRound((double)(x) * m_scaleX);
}

wxCoord wxImplDC::LogicalToDeviceYRel(wxCoord y) const
{
    return wxRound((double)(y) * m_scaleY);
}

void wxImplDC::ComputeScaleAndOrigin()
{
    m_scaleX = m_logicalScaleX * m_userScaleX;
    m_scaleY = m_logicalScaleY * m_userScaleY;
}

void wxImplDC::SetMapMode( int mode )
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

void wxImplDC::SetUserScale( double x, double y )
{
    // allow negative ? -> no
    m_userScaleX = x;
    m_userScaleY = y;
    ComputeScaleAndOrigin();
}

void wxImplDC::SetLogicalScale( double x, double y )
{
    // allow negative ?
    m_logicalScaleX = x;
    m_logicalScaleY = y;
    ComputeScaleAndOrigin();
}

void wxImplDC::SetLogicalOrigin( wxCoord x, wxCoord y )
{
    m_logicalOriginX = x * m_signX;
    m_logicalOriginY = y * m_signY;
    ComputeScaleAndOrigin();
}

void wxImplDC::SetDeviceOrigin( wxCoord x, wxCoord y )
{
    m_deviceOriginX = x;
    m_deviceOriginY = y;
    ComputeScaleAndOrigin();
}

void wxImplDC::SetDeviceLocalOrigin( wxCoord x, wxCoord y )
{
    m_deviceLocalOriginX = x;
    m_deviceLocalOriginY = y;
    ComputeScaleAndOrigin();
}

void wxImplDC::SetAxisOrientation( bool xLeftRight, bool yBottomUp )
{
    // only wxPostScripDC has m_signX = -1, we override SetAxisOrientation there
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

bool wxImplDC::DoGetPartialTextExtents(const wxString& text, wxArrayInt& widths) const
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
            GetTextExtent(c, &w, &h);
            if (c_int < FWC_SIZE)
                s_fontWidthCache.m_widths[c_int] = w;
        }

        totalWidth += w;
        widths[i] = totalWidth;
    }

    return true;
}

void wxImplDC::GetMultiLineTextExtent(const wxString& text,
                                      wxCoord *x,
                                      wxCoord *y,
                                      wxCoord *h,
                                      const wxFont *font) const
{
    wxCoord widthTextMax = 0, widthLine,
            heightTextTotal = 0, heightLineDefault = 0, heightLine = 0;

    wxString curLine;
    for ( const wxChar *pc = text; ; pc++ )
    {
        if ( *pc == _T('\n') || *pc == _T('\0') )
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

            if ( *pc == _T('\n') )
            {
               curLine.clear();
            }
            else
            {
               // the end of string
               break;
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

void wxImplDC::DoDrawCheckMark(wxCoord x1, wxCoord y1,
                               wxCoord width, wxCoord height)
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    wxCoord x2 = x1 + width,
            y2 = y1 + height;

    // the pen width is calibrated to give 3 for width == height == 10
    wxDCPenChanger pen(m_owner, wxPen(GetTextForeground(), (width + height + 1)/7));

    // we're drawing a scaled version of wx/generic/tick.xpm here
    wxCoord x3 = x1 + (4*width) / 10,   // x of the tick bottom
            y3 = y1 + height / 2;       // y of the left tick branch
    DoDrawLine(x1, y3, x3, y2);
    DoDrawLine(x3, y2, x2, y1);

    CalcBoundingBox(x1, y1);
    CalcBoundingBox(x2, y2);
}

bool
wxImplDC::DoStretchBlit(wxCoord xdest, wxCoord ydest,
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

void wxImplDC::DrawLines(const wxList *list, wxCoord xoffset, wxCoord yoffset)
{
    int n = list->GetCount();
    wxPoint *points = new wxPoint[n];

    int i = 0;
    for ( wxList::compatibility_iterator node = list->GetFirst(); node; node = node->GetNext(), i++ )
    {
        wxPoint *point = (wxPoint *)node->GetData();
        points[i].x = point->x;
        points[i].y = point->y;
    }

    DoDrawLines(n, points, xoffset, yoffset);

    delete [] points;
}

void wxImplDC::DrawPolygon(const wxList *list,
                           wxCoord xoffset, wxCoord yoffset,
                           int fillStyle)
{
    int n = list->GetCount();
    wxPoint *points = new wxPoint[n];

    int i = 0;
    for ( wxList::compatibility_iterator node = list->GetFirst(); node; node = node->GetNext(), i++ )
    {
        wxPoint *point = (wxPoint *)node->GetData();
        points[i].x = point->x;
        points[i].y = point->y;
    }

    DoDrawPolygon(n, points, xoffset, yoffset, fillStyle);

    delete [] points;
}

void
wxImplDC::DoDrawPolyPolygon(int n,
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

// TODO: this API needs fixing (wxPointList, why (!const) "wxList *"?)
void wxImplDC::DrawSpline(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2, wxCoord x3, wxCoord y3)
{
    wxList point_list;

    wxPoint *point1 = new wxPoint;
    point1->x = x1; point1->y = y1;
    point_list.Append((wxObject*)point1);

    wxPoint *point2 = new wxPoint;
    point2->x = x2; point2->y = y2;
    point_list.Append((wxObject*)point2);

    wxPoint *point3 = new wxPoint;
    point3->x = x3; point3->y = y3;
    point_list.Append((wxObject*)point3);

    DrawSpline(&point_list);

    for( wxList::compatibility_iterator node = point_list.GetFirst(); node; node = node->GetNext() )
    {
        wxPoint *p = (wxPoint *)node->GetData();
        delete p;
    }
}

void wxImplDC::DrawSpline(int n, wxPoint points[])
{
    wxList list;
    for (int i =0; i < n; i++)
    {
        list.Append((wxObject*)&points[i]);
    }

    DrawSpline(&list);
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
static void wx_spline_draw_point_array(wxDCBase *dc);

wxList wx_spline_point_list;

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
  wxPoint *point = new wxPoint ;
  point->x = (int) x;
  point->y = (int) y;
  wx_spline_point_list.Append((wxObject*)point);
  return true;
}

static void wx_spline_draw_point_array(wxDC *dc)
{
  dc->DrawLines(&wx_spline_point_list, 0, 0 );
  wxList::compatibility_iterator node = wx_spline_point_list.GetFirst();
  while (node)
  {
    wxPoint *point = (wxPoint *)node->GetData();
    delete point;
    wx_spline_point_list.Erase(node);
    node = wx_spline_point_list.GetFirst();
  }
}

void wxImplDC::DoDrawSpline( wxList *points )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    wxPoint *p;
    double           cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4;
    double           x1, y1, x2, y2;

    wxList::compatibility_iterator node = points->GetFirst();
    if (!node)
        // empty list
        return;

    p = (wxPoint *)node->GetData();

    x1 = p->x;
    y1 = p->y;

    node = node->GetNext();
    p = (wxPoint *)node->GetData();

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
        p = (wxPoint *)node->GetData();
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


void wxImplDC::DrawLabel(const wxString& text,
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
        DoDrawBitmap(bitmap, x, y, true /* use mask */);

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
                    m_owner->GetTextExtent(curLine, &widthLine, NULL);

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

                DoDrawText(curLine, xRealStart, y);
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

        DoDrawLine(startUnderscore, yUnderscore, endUnderscore, yUnderscore);
    }

    // return bounding rect if requested
    if ( rectBounding )
    {
        *rectBounding = wxRect(x, y - heightText, widthText, heightText);
    }

    CalcBoundingBox(x0, y0);
    CalcBoundingBox(x0 + width0, y0 + height);
}


void wxImplDC::DoGradientFillLinear(const wxRect& rect,
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
                DoDrawRectangle(rect.GetRight()-x-xDelta, rect.GetTop(),
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
                DoDrawRectangle(rect.GetLeft(), rect.GetBottom()-y-yDelta,
                        rect.GetWidth(), yDelta);
        }
    }

    SetPen(oldPen);
    SetBrush(oldBrush);
}

void wxImplDC::DoGradientFillConcentric(const wxRect& rect,
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
            DoDrawPoint(wxPoint(x + rect.GetLeft(), y + rect.GetTop()));
        }
    }
    //return old pen color
    m_pen.SetColour(oldPenColour);
}

//-----------------------------------------------------------------------------
// wxDC
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxDC, wxObject)

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


#endif  // wxUSE_NEW_DC

//-----------------------------------------------------------------------------
// wxDC
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxDC, wxDCBase)

wxDC::wxDC()
{
    m_ok = FALSE;

    m_pen = *wxBLACK_PEN;
    m_font = *wxNORMAL_FONT;
    m_brush = *wxWHITE_BRUSH;
}

void wxDC::DoSetClippingRegion( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    m_clipping = TRUE;
    m_clipX1 = x;
    m_clipY1 = y;
    m_clipX2 = x + width;
    m_clipY2 = y + height;
}

// ---------------------------------------------------------------------------
// get DC capabilities
// ---------------------------------------------------------------------------

void wxDC::DoGetSizeMM( int* width, int* height ) const
{
    int w = 0;
    int h = 0;
    GetSize( &w, &h );
    if (width) *width = int( double(w) / (m_userScaleX*m_mm_to_pix_x) );
    if (height) *height = int( double(h) / (m_userScaleY*m_mm_to_pix_y) );
}

// Resolution in pixels per logical inch
wxSize wxDC::GetPPI() const
{
    // TODO (should probably be pure virtual)
    return wxSize(0, 0);
}
