/////////////////////////////////////////////////////////////////////////////
// Name:        dc.cpp
// Purpose:     wxDC class
// Author:      David Webster
// Modified by:
// Created:     10/14/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

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
#endif

#include "wx/dcprint.h"

#include <string.h>
#include <math.h>

#include "wx/os2/private.h"

#if !USE_SHARED_LIBRARY
    IMPLEMENT_ABSTRACT_CLASS(wxDC, wxObject)
#endif

// ---------------------------------------------------------------------------
// constants
// ---------------------------------------------------------------------------

static const int VIEWPORT_EXTENT = 1000;

static const int MM_POINTS = 9;
static const int MM_METRIC = 10;

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// wxDC
// ---------------------------------------------------------------------------

wxDC::wxDC(void)
{
    m_canvas  = NULL;

    m_oldBitmap  = 0;
    m_oldPen     = 0;
    m_oldBrush   = 0;
    m_oldFont    = 0;
    m_oldPalette = 0;

    m_bOwnsDC = FALSE;
    m_hDC        = 0;
    m_hDCCount   = 0;
};

wxDC::~wxDC(void)
{
    // TODO:
};

// This will select current objects out of the DC,
// which is what you have to do before deleting the
// DC.
void wxDC::SelectOldObjects(WXHDC dc)
{
    if (dc)
    {
        if (m_oldBitmap)
        {
//            ::SelectObject((HDC) dc, (HBITMAP) m_oldBitmap);
            if (m_selectedBitmap.Ok())
            {
                m_selectedBitmap.SetSelectedInto(NULL);
            }
        }
        m_oldBitmap = 0;
        if (m_oldPen)
        {
//            ::SelectObject((HDC) dc, (HPEN) m_oldPen);
        }
        m_oldPen = 0;
        if (m_oldBrush)
        {
//            ::SelectObject((HDC) dc, (HBRUSH) m_oldBrush);
        }
        m_oldBrush = 0;
        if (m_oldFont)
        {
//            ::SelectObject((HDC) dc, (HFONT) m_oldFont);
        }
        m_oldFont = 0;
        if (m_oldPalette)
        {
//            ::SelectPalette((HDC) dc, (HPALETTE) m_oldPalette, TRUE);
        }
        m_oldPalette = 0;
    }

    m_brush = wxNullBrush;
    m_pen = wxNullPen;
    m_palette = wxNullPalette;
    m_font = wxNullFont;
    m_backgroundBrush = wxNullBrush;
    m_selectedBitmap = wxNullBitmap;
}

// ---------------------------------------------------------------------------
// clipping
// ---------------------------------------------------------------------------

void wxDC::DoSetClippingRegionAsRegion(const wxRegion& region)
{
   // TODO
}

void wxDC::DoSetClippingRegion( long x, long y
                               ,long width, long height
                              )
{
   // TODO
}

void wxDC::DoClipping(WXHDC dc)
{
    if (m_clipping && dc)
    {
//      TODO:
//      IntersectClipRect((HDC) dc, XLOG2DEV(m_clipX1), YLOG2DEV(m_clipY1),
//                                  XLOG2DEV(m_clipX2), YLOG2DEV(m_clipY2));
    }
}

void wxDC::DestroyClippingRegion(void)
{
    // TODO:
};

// ---------------------------------------------------------------------------
// query capabilities
// ---------------------------------------------------------------------------

bool wxDC::CanDrawBitmap() const
{
    return TRUE;
}

bool wxDC::CanGetTextExtent() const
{
    // What sort of display is it?
    int technology = 0; // TODO:  ::GetDeviceCaps(GetHdc(), TECHNOLOGY);

    // TODO: return (technology == DT_RASDISPLAY) || (technology == DT_RASPRINTER);
    return FALSE;
}

int wxDC::GetDepth() const
{
   // TODO:
   return (1);
}

// ---------------------------------------------------------------------------
// drawing
// ---------------------------------------------------------------------------

void wxDC::Clear()
{
   // TODO
}

void wxDC::DoFloodFill( long x
                       ,long y
                       ,const wxColour& col
                       ,int style
                      )
{
   // TODO
}

bool wxDC::DoGetPixel(long x, long y, wxColour *col) const
{
   // TODO
   return(TRUE);
}

void wxDC::DoCrossHair(long x, long y)
{
   // TODO
}

void wxDC::DoDrawLine(long x1, long y1, long x2, long y2)
{
   // TODO
}

void wxDC::DoDrawArc( long x1, long y1
                     ,long x2, long y2
                     ,long xc, long yc
                    )
{
   // TODO
}

void wxDC::DoDrawPoint(long x, long y)
{
   // TODO
}

void wxDC::DoDrawPolygon(int n, wxPoint points[]
                         ,long xoffset, long yoffset
                         ,int fillStyle
                        )
{
   // TODO
}

void wxDC::DoDrawLines( int n, wxPoint points[]
                       ,long xoffset, long yoffset
                      )
{
   // TODO
}

void wxDC::DoDrawRectangle(long x, long y, long width, long height)
{
   // TODO
}

void wxDC::DoDrawRoundedRectangle( long x, long y
                                  ,long width, long height
                                  ,double radius
                                 )
{
   // TODO
}

void wxDC::DoDrawEllipse(long x, long y, long width, long height)
{
   // TODO
}

void wxDC::DoDrawEllipticArc( long x
                             ,long y
                             ,long w
                             ,long h
                             ,double sa
                             ,double ea
                            )
{
   // TODO
}

void wxDC::DoDrawIcon(const wxIcon& icon, long x, long y)
{
   // TODO
}

void wxDC::DoDrawBitmap( const wxBitmap &bmp
                        ,long x, long y
                        ,bool useMask
                       )
{
   // TODO
}

void wxDC::DoDrawText(const wxString& text, long x, long y)
{
   // TODO
}

// ---------------------------------------------------------------------------
// set GDI objects
// ---------------------------------------------------------------------------

void wxDC::SetPalette(const wxPalette& palette)
{
   // TODO
}

void wxDC::SetFont(const wxFont& font)
{
   // TODO
}

void wxDC::SetPen(const wxPen& pen)
{
   // TODO
}
void wxDC::SetBrush(const wxBrush& brush)
{
   // TODO
}

void wxDC::SetBackground(const wxBrush& brush)
{
   // TODO
}

void wxDC::SetBackgroundMode(int mode)
{
   // TODO
}

void wxDC::SetLogicalFunction(int function)
{
   // TODO
}

void wxDC::SetRop(WXHDC dc)
{
    if (!dc || m_logicalFunction < 0)
        return;

    int c_rop;
    // These may be wrong
    switch (m_logicalFunction)
    {
// TODO: Figure this stuff out
        //    case wxXOR: c_rop = R2_XORPEN; break;
//    case wxXOR: c_rop = R2_NOTXORPEN; break;
//    case wxINVERT: c_rop = R2_NOT; break;
//    case wxOR_REVERSE: c_rop = R2_MERGEPENNOT; break;
//    case wxAND_REVERSE: c_rop = R2_MASKPENNOT; break;
//    case wxCLEAR: c_rop = R2_WHITE; break;
//    case wxSET: c_rop = R2_BLACK; break;
//    case wxSRC_INVERT: c_rop = R2_NOTCOPYPEN; break;
//    case wxOR_INVERT: c_rop = R2_MERGENOTPEN; break;
//    case wxAND: c_rop = R2_MASKPEN; break;
//    case wxOR: c_rop = R2_MERGEPEN; break;
//    case wxAND_INVERT: c_rop = R2_MASKNOTPEN; break;
//    case wxEQUIV:
//    case wxNAND:
//    case wxCOPY:
    default:
//      c_rop = R2_COPYPEN;
        break;
    }
//    SetROP2((HDC) dc, c_rop);
}

bool wxDC::StartDoc(const wxString& message)
{
    // We might be previewing, so return TRUE to let it continue.
    return TRUE;
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

long wxDC::GetCharHeight() const
{
    // TODO
    return(1);
}

long wxDC::GetCharWidth() const
{
    // TODO
    return(1);
}

void wxDC::GetTextExtent( const wxString& string
                         ,long* x
                         ,long* y
                         ,long* decent
                         ,long* externalLeading
                         ,wxFont* theFont
                        ) const
{
   // TODO:
}

void wxDC::SetMapMode( int mode )
{
    // TODO:
};

void wxDC::SetUserScale(double x, double y)
{
    m_userScaleX = x;
    m_userScaleY = y;

    SetMapMode(m_mappingMode);
}

void wxDC::SetAxisOrientation(bool xLeftRight, bool yBottomUp)
{
    m_signX = xLeftRight ? 1 : -1;
    m_signY = yBottomUp ? -1 : 1;

    SetMapMode(m_mappingMode);
}

void wxDC::SetSystemScale(double x, double y)
{
    m_scaleX = x;
    m_scaleY = y;

    SetMapMode(m_mappingMode);
}

void wxDC::SetLogicalOrigin( long x, long y )
{
    // TODO:
};

void wxDC::SetDeviceOrigin( long x, long y )
{
    // TODO:
};

// ---------------------------------------------------------------------------
// coordinates transformations
// ---------------------------------------------------------------------------

long wxDCBase::DeviceToLogicalX(long x) const
{
    long new_x = x - m_deviceOriginX;
    if (new_x > 0)
        return (long)((double)(new_x) / m_scaleX + 0.5) * m_signX + m_logicalOriginX;
    else
        return (long)((double)(new_x) / m_scaleX - 0.5) * m_signX + m_logicalOriginX;
};

long wxDCBase::DeviceToLogicalXRel(long x) const
{
    if (x > 0)
        return (long)((double)(x) / m_scaleX + 0.5);
    else
        return (long)((double)(x) / m_scaleX - 0.5);
};

long wxDCBase::DeviceToLogicalY(long y) const
{
    long new_y = y - m_deviceOriginY;
    if (new_y > 0)
        return (long)((double)(new_y) / m_scaleY + 0.5) * m_signY + m_logicalOriginY;
    else
        return (long)((double)(new_y) / m_scaleY - 0.5) * m_signY + m_logicalOriginY;
};

long wxDCBase::DeviceToLogicalYRel(long y) const
{
    if (y > 0)
        return (long)((double)(y) / m_scaleY + 0.5);
    else
        return (long)((double)(y) / m_scaleY - 0.5);
};

long wxDCBase::LogicalToDeviceX(long x) const
{
    long new_x = x - m_logicalOriginX;
    if (new_x > 0)
        return (long)((double)(new_x) * m_scaleX + 0.5) * m_signX + m_deviceOriginX;
    else
    return (long)((double)(new_x) * m_scaleX - 0.5) * m_signX + m_deviceOriginX;
};

long wxDCBase::LogicalToDeviceXRel(long x) const
{
    if (x > 0)
        return (long)((double)(x) * m_scaleX + 0.5);
    else
        return (long)((double)(x) * m_scaleX - 0.5);
};

long wxDCBase::LogicalToDeviceY(long y) const
{
    long new_y = y - m_logicalOriginY;
    if (new_y > 0)
        return (long)((double)(new_y) * m_scaleY + 0.5) * m_signY + m_deviceOriginY;
    else
        return (long)((double)(new_y) * m_scaleY - 0.5) * m_signY + m_deviceOriginY;
};

long wxDCBase::LogicalToDeviceYRel(long y) const
{
    if (y > 0)
        return (long)((double)(y) * m_scaleY + 0.5);
    else
        return (long)((double)(y) * m_scaleY - 0.5);
};

// ---------------------------------------------------------------------------
// bit blit
// ---------------------------------------------------------------------------

bool wxDC::DoBlit( long xdest
                  ,long ydest
                  ,long width
                  ,long height
                  ,wxDC *source
                  ,long xsrc
                  ,long ysrc
                  ,int  rop
                  ,bool useMask
                 )
{
   // TODO
   return(TRUE);
}

void wxDC::DoGetSize( int* width, int* height ) const
{
    // TODO:
};

void wxDC::DoGetSizeMM( int* width, int* height ) const
{
    // TODO:
};

wxSize wxDC::GetPPI() const
{
   int x = 1;
   int y = 1;
   // TODO:
   return (wxSize(x,y));
}

void wxDC::SetLogicalScale( double x, double y )
{
    // TODO:
};

#if WXWIN_COMPATIBILITY
void wxDC::DoGetTextExtent(const wxString& string, float *x, float *y,
                         float *descent, float *externalLeading,
                         wxFont *theFont, bool use16bit) const
{
    long x1, y1, descent1, externalLeading1;
    GetTextExtent(string, & x1, & y1, & descent1, & externalLeading1, theFont, use16bit);
    *x = x1; *y = y1;
    if (descent)
        *descent = descent1;
    if (externalLeading)
        *externalLeading = externalLeading1;
}
#endif

// ---------------------------------------------------------------------------
// spline drawing code
// ---------------------------------------------------------------------------

#if wxUSE_SPLINES

class wxSpline: public wxObject
{
public:
    int type;
    wxList *points;

    wxSpline(wxList *list);
    void DeletePoints();

    // Doesn't delete points
    ~wxSpline();
};

void wx_draw_open_spline(wxDC *dc, wxSpline *spline);

void wx_quadratic_spline(double a1, double b1, double a2, double b2,
                         double a3, double b3, double a4, double b4);
void wx_clear_stack();
int wx_spline_pop(double *x1, double *y1, double *x2, double *y2, double *x3,
                  double *y3, double *x4, double *y4);
void wx_spline_push(double x1, double y1, double x2, double y2, double x3, double y3,
                    double x4, double y4);
static bool wx_spline_add_point(double x, double y);
static void wx_spline_draw_point_array(wxDC *dc);
wxSpline *wx_make_spline(int x1, int y1, int x2, int y2, int x3, int y3);

void wxDC::DoDrawSpline(wxList *list)
{
    wxSpline spline(list);

    wx_draw_open_spline(this, &spline);
}

wxList wx_spline_point_list;

void wx_draw_open_spline(wxDC *dc, wxSpline *spline)
{
    wxPoint *p;
    double           cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4;
    double           x1, y1, x2, y2;

    wxNode *node = spline->points->First();
    p = (wxPoint *)node->Data();

    x1 = p->x;
    y1 = p->y;

    node = node->Next();
    p = (wxPoint *)node->Data();

    x2 = p->x;
    y2 = p->y;
    cx1 = (double)((x1 + x2) / 2);
    cy1 = (double)((y1 + y2) / 2);
    cx2 = (double)((cx1 + x2) / 2);
    cy2 = (double)((cy1 + y2) / 2);

    wx_spline_add_point(x1, y1);

    while ((node = node->Next()) != NULL)
    {
        p = (wxPoint *)node->Data();
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

    wx_spline_add_point((double)wx_round(cx1), (double)wx_round(cy1));
    wx_spline_add_point(x2, y2);

    wx_spline_draw_point_array(dc);

}

/********************* CURVES FOR SPLINES *****************************

  The following spline drawing routine is from

    "An Algorithm for High-Speed Curve Generation"
    by George Merrill Chaikin,
    Computer Graphics and Image Processing, 3, Academic Press,
    1974, 346-349.

      and

        "On Chaikin's Algorithm" by R. F. Riesenfeld,
        Computer Graphics and Image Processing, 4, Academic Press,
        1975, 304-310.

***********************************************************************/

#define     half(z1, z2)    ((z1+z2)/2.0)
#define     THRESHOLD   5

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
            wx_spline_add_point((double)wx_round(x1), (double)wx_round(y1));
            wx_spline_add_point((double)wx_round(xmid), (double)wx_round(ymid));
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
}
Stack;

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
    wxPoint *point = new wxPoint;
    point->x = (int) x;
    point->y = (int) y;
    wx_spline_point_list.Append((wxObject*)point);
    return TRUE;
}

static void wx_spline_draw_point_array(wxDC *dc)
{
    dc->DrawLines(&wx_spline_point_list, 0, 0);
    wxNode *node = wx_spline_point_list.First();
    while (node)
    {
        wxPoint *point = (wxPoint *)node->Data();
        delete point;
        delete node;
        node = wx_spline_point_list.First();
    }
}

wxSpline::wxSpline(wxList *list)
{
    points = list;
}

wxSpline::~wxSpline()
{
}

void wxSpline::DeletePoints()
{
    for(wxNode *node = points->First(); node; node = points->First())
    {
        wxPoint *point = (wxPoint *)node->Data();
        delete point;
        delete node;
    }
    delete points;
}


#endif // wxUSE_SPLINES

