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

    IMPLEMENT_ABSTRACT_CLASS(wxDC, wxObject)

// ---------------------------------------------------------------------------
// constants
// ---------------------------------------------------------------------------

static const int VIEWPORT_EXTENT = 1000;

static const int MM_POINTS = 9;
static const int MM_METRIC = 10;

// usually this is defined in math.h
#ifndef M_PI
    static const double M_PI = 3.14159265358979323846;
#endif // M_PI

// ---------------------------------------------------------------------------
// private functions
// ---------------------------------------------------------------------------

// convert degrees to radians
static inline double DegToRad(double deg) { return (deg * M_PI) / 180.0; }

int SetTextColor(
  HPS                               hPS
, int                               nForegroundColour
)
{
    CHARBUNDLE                      vCbnd;

    vCbnd.lColor =  nForegroundColour;
    ::GpiSetAttrs( hPS       // presentation-space handle
                  ,PRIM_CHAR // Char primitive.
                  ,CBB_COLOR // sets color.
                  ,0         //
                  ,&vCbnd    // buffer for attributes.
                 );
    return 0;
}

int QueryTextBkColor(
  HPS                               hPS
)
{
    CHARBUNDLE                      vCbnd;

    ::GpiQueryAttrs( hPS            // presentation-space handle
                    ,PRIM_CHAR      // Char primitive.
                    ,CBB_BACK_COLOR // Background color.
                    ,&vCbnd         // buffer for attributes.
                   );
    return vCbnd.lBackColor;
}


int SetTextBkColor(
  HPS                               hPS
, int                               nBackgroundColour
)
{
    CHARBUNDLE                      vCbnd;
    int                             rc;

    rc =  QueryTextBkColor(hPS);

    vCbnd.lBackColor = nBackgroundColour;
    ::GpiSetAttrs(hPS,            // presentation-space handle
                  PRIM_CHAR,      // Char primitive.
                  CBB_BACK_COLOR, // sets color.
                  0,
                  &vCbnd          // buffer for attributes.
                 );
    return rc;
}

int SetBkMode(
  HPS                               hPS
, int                               nBackgroundMode
)
{
    if(nBackgroundMode == wxTRANSPARENT)
        ::GpiSetBackMix( hPS
                        ,BM_LEAVEALONE
                       );
    else
        // the background of the primitive takes  over whatever is underneath.
        ::GpiSetBackMix( hPS
                        ,BM_OVERPAINT
                        );
    return 0;
}

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// wxDC
// ---------------------------------------------------------------------------

wxDC::wxDC(void)
{
    m_pCanvas      = NULL;

    m_hOldBitmap   = 0;
    m_hOldPen      = 0;
    m_hOldBrush    = 0;
    m_hOldFont     = 0;
    m_hOldPalette  = 0;

    m_bOwnsDC      = FALSE;
    m_hDC          = 0;
    m_nDCCount     = 0;
    m_hOldPS       = NULL;
    m_hPS          = NULL;
    m_bIsPaintTime = FALSE; // True at Paint Time
    m_brush.GetColour().Set("WHITE");
}

wxDC::~wxDC(void)
{
}

// This will select current objects out of the DC,
// which is what you have to do before deleting the
// DC.
void wxDC::SelectOldObjects(
  WXHDC                             hPS
)
{
    if (hPS)
    {
        if (m_hOldBitmap)
        {
            ::GpiSetBitmap(hPS, (HBITMAP) m_hOldBitmap);
            if (m_vSelectedBitmap.Ok())
            {
                m_vSelectedBitmap.SetSelectedInto(NULL);
            }
        }
        m_hOldBitmap = 0;
        //
        // OS/2 has no other native GDI objects to set in a PS/DC like windows
        //
        m_hOldPen = 0;
        m_hOldBrush = 0;
        m_hOldFont = 0;
        m_hOldPalette = 0;
    }

    m_brush           = wxNullBrush;
    m_pen             = wxNullPen;
    m_palette         = wxNullPalette;
    m_font            = wxNullFont;
    m_backgroundBrush = wxNullBrush;
    m_vSelectedBitmap = wxNullBitmap;
}

// ---------------------------------------------------------------------------
// clipping
// ---------------------------------------------------------------------------

#define DO_SET_CLIPPING_BOX()                    \
{                                                \
    RECTL rect;                                  \
                                                 \
    ::GpiQueryClipBox(m_hPS, &rect);             \
                                                 \
    m_clipX1 = (wxCoord) XDEV2LOG(rect.xLeft);   \
    m_clipY1 = (wxCoord) YDEV2LOG(rect.yTop);    \
    m_clipX2 = (wxCoord) XDEV2LOG(rect.xRight);  \
    m_clipY2 = (wxCoord) YDEV2LOG(rect.yBottom); \
}

void wxDC::DoSetClippingRegion(
  wxCoord                           x
, wxCoord                           y
, wxCoord                           width
, wxCoord                           height
)
{
    RECTL                           vRect;

    m_clipping    = TRUE;
    vRect.xLeft   = XLOG2DEV(x);
    vRect.yTop    = YLOG2DEV(m_vRclPaint.yTop - y);
    vRect.xRight  = XLOG2DEV(x + width);
    vRect.yBottom = YLOG2DEV(m_vRclPaint.yTop - (y + height));
    ::GpiIntersectClipRectangle(m_hPS, &vRect);
    DO_SET_CLIPPING_BOX()
} // end of wxDC::DoSetClippingRegion

void wxDC::DoSetClippingRegionAsRegion(
  const wxRegion&                   rRegion
)
{
     wxCHECK_RET(rRegion.GetHRGN(), wxT("invalid clipping region"));
     HRGN                           hRgnOld;

     m_clipping = TRUE;
     ::GpiSetClipRegion( m_hPS
                        ,(HRGN)rRegion.GetHRGN()
                        ,&hRgnOld
                       );
    DO_SET_CLIPPING_BOX()
} // end of wxDC::DoSetClippingRegionAsRegion

void wxDC::DestroyClippingRegion(void)
{
    if (m_clipping && m_hPS)
    {
         HRGN                       hRgnOld;
         RECTL                      vRect;

         // TODO: this should restore the previous clipped region
         //       so that OnPaint processing works correctly, and
         //       the update doesn't get destroyed after the first
         //       DestroyClippingRegion
         vRect.xLeft   = XLOG2DEV(0);
         vRect.yTop    = YLOG2DEV(32000);
         vRect.xRight  = XLOG2DEV(32000);
         vRect.yBottom = YLOG2DEV(0);

         HRGN                       hRgn = ::GpiCreateRegion(m_hPS, 1, &vRect);

         ::GpiSetClipRegion(m_hPS, hRgn, &hRgnOld);
     }
      m_clipping = FALSE;
} // end of wxDC::DestroyClippingRegion

// ---------------------------------------------------------------------------
// query capabilities
// ---------------------------------------------------------------------------

bool wxDC::CanDrawBitmap() const
{
    return TRUE;
}

bool wxDC::CanGetTextExtent() const
{
    LONG                            lTechnology = 0L;

    ::DevQueryCaps(GetHDC(), CAPS_TECHNOLOGY, 1L, &lTechnology);
    return (lTechnology == CAPS_TECH_RASTER_DISPLAY) || (lTechnology == CAPS_TECH_RASTER_PRINTER);
} // end of wxDC::CanGetTextExtent

int wxDC::GetDepth() const
{
    LONG                            lArray[CAPS_COLOR_BITCOUNT];
    int                             nBitsPerPixel;

    if(::DevQueryCaps( GetHDC()
                      ,CAPS_FAMILY
                      ,CAPS_COLOR_BITCOUNT
                      ,lArray
                     ))
    {
        nBitsPerPixel = (int)lArray[CAPS_COLOR_BITCOUNT];
    }
    return nBitsPerPixel;
} // end of wxDC::GetDepth

// ---------------------------------------------------------------------------
// drawing
// ---------------------------------------------------------------------------

void wxDC::Clear()
{
    ::GpiErase(m_hPS);
}

void wxDC::DoFloodFill(
  wxCoord                           vX
, wxCoord                           vY
, const wxColour&                   rCol
, int                               nStyle
)
{
    POINTL                          vPtlPos;
    LONG                            lColor;
    LONG                            lOptions;

    vPtlPos.x = vX;             // Loads x-coordinate
    vPtlPos.y = vY;             // Loads y-coordinate
    ::GpiMove(m_hPS, &vPtlPos); // Sets current position
    lColor = rCol.GetPixel();
    lOptions = FF_BOUNDARY;
    if(wxFLOOD_SURFACE == nStyle)
        lOptions = FF_SURFACE;

    ::GpiFloodFill(m_hPS, lOptions, lColor);
}

bool wxDC::DoGetPixel(
  wxCoord                           vX
, wxCoord                           vY
, wxColour*                         pCol
) const
{
    POINTL                          vPoint;
    LONG                            lColor;

    vPoint.x = vX;
    vPoint.y = vY;
    lColor = ::GpiSetPel(m_hPS, &vPoint);
    pCol->Set((unsigned long)lColor);
    if(lColor>= 0)
        return(TRUE);
    else
        return(FALSE);
}

void wxDC::DoCrossHair(
  wxCoord                           vX
, wxCoord                           vY
)
{
    wxCoord                         vX1 = vX - VIEWPORT_EXTENT;
    wxCoord                         vY1 = vY - VIEWPORT_EXTENT;
    wxCoord                         vX2 = vX + VIEWPORT_EXTENT;
    wxCoord                         vY2 = vY + VIEWPORT_EXTENT;
    POINTL                          vPoint[4];

    vPoint[0].x = vX1;
    vPoint[0].y = m_vRclPaint.yTop - vY;

    vPoint[1].x = vX2;
    vPoint[1].y = m_vRclPaint.yTop - vY;

    ::GpiMove(m_hPS, &vPoint[0]);
    ::GpiLine(m_hPS, &vPoint[1]);

    vPoint[2].x = vX;
    vPoint[2].y = m_vRclPaint.yTop - vY1;

    vPoint[3].x = vX;
    vPoint[3].y = m_vRclPaint.yTop - vY2;

    ::GpiMove(m_hPS, &vPoint[2]);
    ::GpiLine(m_hPS, &vPoint[3]);
} // end of wxDC::DoCrossHair

void wxDC::DoDrawLine(
  wxCoord                           vX1
, wxCoord                           vY1
, wxCoord                           vX2
, wxCoord                           vY2
)
{
    POINTL                          vPoint[2];

    vPoint[0].x = vX1;
    vPoint[0].y = m_vRclPaint.yTop - vY1;
    vPoint[1].x = vX2;
    vPoint[1].y = m_vRclPaint.yTop - vY2;
    ::GpiMove(m_hPS, &vPoint[0]);
    ::GpiLine(m_hPS, &vPoint[1]);
} // end of wxDC::DoDrawLine

//////////////////////////////////////////////////////////////////////////////
// Draws an arc of a circle, centred on (xc, yc), with starting point (x1, y1)
// and ending at (x2, y2). The current pen is used for the outline and the
// current brush for filling the shape. The arc is drawn in an anticlockwise
// direction from the start point to the end point.
//////////////////////////////////////////////////////////////////////////////
void wxDC::DoDrawArc(
  wxCoord                           vX1
, wxCoord                           vY1
, wxCoord                           vX2
, wxCoord                           vY2
, wxCoord                           vXc
, wxCoord                           vYc
)
{
     POINTL                         vPtlPos;
     POINTL                         vPtlArc[2]; // Structure for current position
     int                            nDx;
     int                            nDy;
     double                         dRadius;
     double                         dAngl1;
     double                         dAngl2;
     double                         dAnglmid;
     wxCoord                        vXm;
     wxCoord                        vYm;
     ARCPARAMS                      vArcp; // Structure for arc parameters

    if((vX1 == vXc && vY1 == vXc) || (vX2 == vXc && vY2 == vXc))
        return; // Draw point ??
    dRadius = 0.5 * ( hypot( (double)(vY1 - vYc)
                            ,(double)(vX1 - vXc)
                           ) +
                      hypot( (double)(vY2 - vYc)
                            ,(double)(vX2 - vXc)
                           )
                     );

    dAngl1 = atan2( (double)(vY1 - vYc)
                   ,(double)(vX1 - vXc)
                  );
    dAngl2 = atan2( (double)(vY2 - vYc)
                   ,(double)(vX2 - vXc)
                  );
    if(dAngl2 < dAngl1)
        dAngl2 += M_PI * 2;

    //
    // GpiPointArc can't draw full arc
    //
     if(dAngl2 == dAngl1 || (vX1 == vX2 && vY1 == vY2) )
     {
        //
        // Medium point
        //
        dAnglmid = (dAngl1 + dAngl2)/2. + M_PI;
        vXm      = vXc + dRadius * cos(dAnglmid);
        vYm      = vYc + dRadius * sin(dAnglmid);
        DoDrawArc( vX1, vY1
                  ,vXm, vYm
                  ,vXc, vYc
                 );
        DoDrawArc( vXm, vYm
                  ,vX2, vY2
                  ,vXc, vYc
                 );
        return;
    }

    //
    // Medium point
    //
    dAnglmid = (dAngl1 + dAngl2)/2.;
    vXm      = vXc + dRadius * cos(dAnglmid);
    vYm      = vYc + dRadius * sin(dAnglmid);

    //
    // Ellipse main axis (r,q), (p,s) with center at (0,0) */
    //
    vArcp.lR = 0;
    vArcp.lQ = 1;
    vArcp.lP = 1;
    vArcp.lS = 0;
    ::GpiSetArcParams(m_hPS, &vArcp); // Sets parameters to default

    vPtlPos.x = vX1; // Loads x-coordinate
    vPtlPos.y = vY1; // Loads y-coordinate
    ::GpiMove(m_hPS, &vPtlPos); // Sets current position
    vPtlArc[0].x = vXm;
    vPtlArc[0].y = vYm;
    vPtlArc[1].x = vX2;
    vPtlArc[1].y = vY2;
    ::GpiPointArc(m_hPS, vPtlArc); // Draws the arc
} // end of wxDC::DoDrawArc

void wxDC::DoDrawCheckMark(
  wxCoord                           vX1
, wxCoord                           vY1
, wxCoord                           vWidth
, wxCoord                           vHeight
)
{
    POINTL                          vPoint[2];

    vPoint[0].x = vX1;
    vPoint[0].y = vY1;
    vPoint[1].x = vX1 + vWidth;
    vPoint[1].y = vY1 + vHeight;

    ::GpiMove(m_hPS, &vPoint[0]);
    ::GpiBox( m_hPS       // handle to a presentation space
             ,DRO_OUTLINE // draw the box outline ? or ?
             ,&vPoint[1]  // address of the corner
             ,0L          // horizontal corner radius
             ,0L          // vertical corner radius
            );
    if(vWidth > 4 && vHeight > 4)
    {
        int                         nTmp;

        vPoint[0].x += 2; vPoint[0].y += 2;
        vPoint[1].x -= 2; vPoint[1].y -= 2;
        ::GpiMove(m_hPS, &vPoint[0]);
        ::GpiLine(m_hPS, &vPoint[1]);
        nTmp = vPoint[0].x;
        vPoint[0].x = vPoint[1].x;
        vPoint[1].x = nTmp;
        ::GpiMove(m_hPS, &vPoint[0]);
        ::GpiLine(m_hPS, &vPoint[1]);
    }
} // end of wxDC::DoDrawCheckMark

void wxDC::DoDrawPoint(
  wxCoord                           vX
, wxCoord                           vY
)
{
    POINTL                          vPoint;

    vPoint.x = vX;
    vPoint.y = m_vRclPaint.yTop - vY;
    ::GpiSetPel(m_hPS, &vPoint);
} // end of wxDC::DoDrawPoint

void wxDC::DoDrawPolygon(
  int                               n
, wxPoint                           vPoints[]
, wxCoord                           vXoffset
, wxCoord                           vYoffset
, int                               nFillStyle
)
{
    ULONG                           ulCount = 1;    // Number of polygons.
    POLYGON                         vPlgn;          // polygon.
    ULONG                           flOptions = 0L; // Drawing options.

//////////////////////////////////////////////////////////////////////////////
// This contains fields of option bits... to draw boundary lines as well as
// the area interior.
//
// Drawing boundary lines:
//   POLYGON_NOBOUNDARY              Does not draw boundary lines.
//   POLYGON_BOUNDARY                Draws boundary lines (the default).
//
// Construction of the area interior:
//   POLYGON_ALTERNATE               Constructs interior in alternate mode
//                                   (the default).
//   POLYGON_WINDING                 Constructs interior in winding mode.
//////////////////////////////////////////////////////////////////////////////

    ULONG                           flModel = 0L; // Drawing model.

//////////////////////////////////////////////////////////////////////////////
// Drawing model.
//   POLYGON_INCL  Fill is inclusive of bottom right (the default).
//   POLYGON_EXCL  Fill is exclusive of bottom right.
//       This is provided to aid migration from other graphics models.
//////////////////////////////////////////////////////////////////////////////

    LONG                            lHits = 0L; // Correlation/error indicator.
    POINTL                          vPoint;
    int                             i;
    int                             nIsTRANSPARENT = 0;
    LONG                            lBorderColor = 0L;
    LONG                            lColor = 0L;

    lBorderColor = m_pen.GetColour().GetPixel();
    lColor       = m_brush.GetColour().GetPixel();
    if(m_brush.GetStyle() == wxTRANSPARENT)
        nIsTRANSPARENT = 1;

    vPlgn.ulPoints = n;
    vPlgn.aPointl = (POINTL*) calloc( n + 1
                                     ,sizeof(POINTL)
                                    ); // well, new will call malloc

    for(i = 0; i < n; i++)
    {
        vPlgn.aPointl[i].x = vPoints[i].x; // +xoffset;
        vPlgn.aPointl[i].y = vPoints[i].y; // +yoffset;
    }
    flModel = POLYGON_BOUNDARY;
    if(nFillStyle == wxWINDING_RULE)
        flModel |= POLYGON_WINDING;
    else
        flModel |= POLYGON_ALTERNATE;

    vPoint.x = vXoffset;
    vPoint.y = vYoffset;

    ::GpiSetColor(m_hPS, lBorderColor);
    ::GpiMove(m_hPS, &vPoint);
    lHits = ::GpiPolygons(m_hPS, ulCount, &vPlgn, flOptions, flModel);
    free(vPlgn.aPointl);
} // end of wxDC::DoDrawPolygon

void wxDC::DoDrawLines(
  int                               n
, wxPoint                           vPoints[]
, wxCoord                           vXoffset
, wxCoord                           vYoffset
)
{
    int                             i;
    POINTL                          vPoint;

    vPoint.x = vPoints[0].x + vXoffset;
    vPoint.y = vPoints[0].y + vYoffset;
    ::GpiMove(m_hPS, &vPoint);

    LONG                            lBorderColor = m_pen.GetColour().GetPixel();

    ::GpiSetColor(m_hPS, lBorderColor);
    for(i = 1; i < n; i++)
    {
        vPoint.x = vPoints[0].x + vXoffset;
        vPoint.y = vPoints[0].y + vYoffset;
        ::GpiLine(m_hPS, &vPoint);
    }
} // end of wxDC::DoDrawLines

void wxDC::DoDrawRectangle(
  wxCoord                           vX
, wxCoord                           vY
, wxCoord                           vWidth
, wxCoord                           vHeight
)
{
    POINTL                          vPoint[2];
    LONG                            lControl;
    LONG                            lColor;
    LONG                            lBorderColor;
    int                             nIsTRANSPARENT = 0;

    vPoint[0].x = vX;
    vPoint[0].y = m_vRclPaint.yTop - (vY + vHeight);
    vPoint[1].x = vX + vWidth;
    vPoint[1].y = m_vRclPaint.yTop - vY;
    ::GpiMove(m_hPS, &vPoint[0]);
    lColor       = m_brush.GetColour().GetPixel();
    lBorderColor = m_pen.GetColour().GetPixel();
    if (m_brush.GetStyle() == wxTRANSPARENT)
        nIsTRANSPARENT = 1;
    if(lColor == lBorderColor || nIsTRANSPARENT)
    {
        lControl = DRO_OUTLINEFILL; //DRO_FILL;
        if(m_brush.GetStyle() == wxTRANSPARENT)
            lControl = DRO_OUTLINE;

        ::GpiSetColor(m_hPS, lColor);
        ::GpiBox( m_hPS       // handle to a presentation space
                 ,lControl   // draw the box outline ? or ?
                 ,&vPoint[1]  // address of the corner
                 ,0L          // horizontal corner radius
                 ,0L          // vertical corner radius
                );
    }
    else
    {
        lControl = DRO_OUTLINE;
        ::GpiSetColor( m_hPS
                      ,lBorderColor
                     );
        ::GpiBox( m_hPS
                 ,lControl
                 ,&vPoint[1]
                 ,0L
                 ,0L
                );
        lControl = DRO_FILL;
        ::GpiSetColor( m_hPS
                      ,lColor
                     );
        vPoint[0].x = vX + 1;
        vPoint[0].y = m_vRclPaint.yTop - (vY + vHeight) + 1;
        vPoint[1].x = vX + vWidth - 2;
        vPoint[1].y = m_vRclPaint.yTop - (vY + 2);
        ::GpiMove(m_hPS, &vPoint[0]);
        ::GpiBox( m_hPS
                 ,lControl
                 ,&vPoint[1]
                 ,0L
                 ,0L
                );
    }
} // end of wxDC::DoDrawRectangle

void wxDC::DoDrawRoundedRectangle(
  wxCoord                           vX
, wxCoord                           vY
, wxCoord                           vWidth
, wxCoord                           vHeight
, double                            dRadius
)
{
    POINTL                          vPoint[2];
    LONG                            lControl;

    vPoint[0].x = vX;
    vPoint[0].y = YLOG2DEV(vY) - vHeight;
    vPoint[1].x = vX + vWidth;
    vPoint[1].y = vY;
    ::GpiMove(m_hPS, &vPoint[0]);

    lControl = DRO_OUTLINEFILL; //DRO_FILL;
    if (m_brush.GetStyle() == wxTRANSPARENT)
        lControl = DRO_OUTLINE;
    ::GpiBox( m_hPS         // handle to a presentation space
             ,DRO_OUTLINE   // draw the box outline ? or ?
             ,&vPoint[1]    // address of the corner
             ,(LONG)dRadius // horizontal corner radius
             ,(LONG)dRadius // vertical corner radius
            );
} // end of wxDC::DoDrawRoundedRectangle

// Draw Ellipse within box (x,y) - (x+width, y+height)
void wxDC::DoDrawEllipse(
  wxCoord                           vX
, wxCoord                           vY
, wxCoord                           vWidth
, wxCoord                           vHeight
)
{
    POINTL                          vPtlPos; // Structure for current position
    FIXED                           vFxMult; // Multiplier for ellipse
    ARCPARAMS                       vArcp;   // Structure for arc parameters

    vArcp.lR = 0;
    vArcp.lQ = vHeight/2;
    vArcp.lP = vWidth/2;
    vArcp.lS = 0;
    ::GpiSetArcParams( m_hPS
                      ,&vArcp
                     ); // Sets parameters to default
    vPtlPos.x = vX + vWidth/2;  // Loads x-coordinate
    vPtlPos.y = vY + vHeight/2; // Loads y-coordinate
    ::GpiMove( m_hPS
              ,&vPtlPos
             ); // Sets current position
    vFxMult = MAKEFIXED(1, 0);             /* Sets multiplier            */

    //
    // DRO_FILL, DRO_OTLINEFILL - where to get
    //
    ::GpiFullArc( m_hPS
                 ,DRO_OUTLINE
                 ,vFxMult
                ); // Draws full arc with center at current position
} // end of wxDC::DoDrawEllipse

void wxDC::DoDrawEllipticArc(
  wxCoord                           vX
, wxCoord                           vY
, wxCoord                           vWidth
, wxCoord                           vHeight
, double                            dSa
, double                            dEa
)
{
    POINTL                          vPtlPos; // Structure for current position
    FIXED                           vFxMult; // Multiplier for ellipse
    ARCPARAMS                       vArcp;   // Structure for arc parameters
    FIXED                           vFSa;
    FIXED                           vFSweepa; // Start angle, sweep angle
    double                          dIntPart;
    double                          dFractPart;
    double                          dRadius;

    dFractPart = modf(dSa,&dIntPart);
    vFSa = MAKEFIXED((int)dIntPart, (int)(dFractPart * 0xffff) );
    dFractPart = modf(dEa - dSa, &dIntPart);
    vFSweepa = MAKEFIXED((int)dIntPart, (int)(dFractPart * 0xffff) );

    //
    // Ellipse main axis (r,q), (p,s) with center at (0,0)
    //
    vArcp.lR = 0;
    vArcp.lQ = vHeight/2;
    vArcp.lP = vWidth/2;
    vArcp.lS = 0;
    ::GpiSetArcParams(m_hPS, &vArcp); // Sets parameters to default
    vPtlPos.x = vX + vWidth/2  * (1. + cos(DegToRad(dSa))); // Loads x-coordinate
    vPtlPos.y = vY + vHeight/2 * (1. + sin(DegToRad(dSa))); // Loads y-coordinate
    ::GpiMove(m_hPS, &vPtlPos); // Sets current position

    //
    // May be not to the center ?
    //
    vPtlPos.x = vX + vWidth/2 ; // Loads x-coordinate
    vPtlPos.y = vY + vHeight/2; // Loads y-coordinate
    vFxMult = MAKEFIXED(1, 0);  // Sets multiplier

    //
    // DRO_FILL, DRO_OTLINEFILL - where to get
    //
    ::GpiPartialArc( m_hPS
                    ,&vPtlPos
                    ,vFxMult
                    ,vFSa
                    ,vFSweepa
                   );
} // end of wxDC::DoDrawEllipticArc

void wxDC::DoDrawIcon(
  const wxIcon&                     rIcon
, wxCoord                           vX
, wxCoord                           vY
)
{
    wxCHECK_RET( rIcon.Ok(), wxT("invalid icon in DrawIcon") );

    ::WinDrawPointer( GetHPS()
                     ,vX
                     ,vY
                     ,(HPOINTER)GetHiconOf(rIcon)
                     ,DP_NORMAL
                    );
} // end of wxDC::DoDrawIcon

void wxDC::DoDrawBitmap(
  const wxBitmap&                   rBmp
, wxCoord                           vX
, wxCoord                           vY
, bool                              bUseMask
)
{
    POINTL                          vPoint = {vX, vY};

    ::WinDrawBitmap( GetHPS()
                    ,(HBITMAP)GetHbitmapOf(rBmp)
                    ,NULL
                    ,&vPoint
                    ,0L
                    ,0L
                    ,DBM_NORMAL
                   );
} // end of wxDC::DoDrawBitmap

void wxDC::DoDrawText(
  const wxString&                   rsText
, wxCoord                           vX
, wxCoord                           vY
)
{
    DrawAnyText( rsText
                ,vX
                ,vY
               );
}

void wxDC::DrawAnyText(
  const wxString&                   rsText
, wxCoord                           vX
, wxCoord                           vY
)
{
    int                             nOldBackground = 0;
    POINTL                          vPtlStart;
    LONG                            lHits;

    //
    // prepare for drawing the text
    //

    //
    // Set text color attributes
    //
    if (m_textForegroundColour.Ok())
    {
        SetTextColor( m_hPS
                     ,(int)m_textForegroundColour.GetPixel()
                    );
    }

    if (m_textBackgroundColour.Ok())
    {
        nOldBackground = SetTextBkColor( m_hPS
                                        ,(int)m_textBackgroundColour.GetPixel()
                                       );
    }
    SetBkMode( m_hPS
              ,m_backgroundMode
             );
    vPtlStart.x = vX;
    vPtlStart.y = vY;

    lHits = ::GpiCharStringAt( m_hPS
                              ,&vPtlStart
                              ,rsText.length()
                              ,(PCH)rsText.c_str()
                             );
    if (lHits != GPI_OK)
    {
        wxLogLastError(wxT("TextOut"));
    }

    //
    // Restore the old parameters (text foreground colour may be left because
    // it never is set to anything else, but background should remain
    // transparent even if we just drew an opaque string)
    //
    if (m_textBackgroundColour.Ok())
            SetTextBkColor( m_hPS
                           ,nOldBackground
                          );
    SetBkMode( m_hPS
              ,wxTRANSPARENT
             );
}

void wxDC::DoDrawRotatedText(
  const wxString&                   rsText
, wxCoord                           vX
, wxCoord                           vY
, double                            dAngle
)
{
    if (dAngle == 0.0)
    {
        DoDrawText( rsText
                   ,vX
                   ,vY
                  );
    }

   // TODO:
   /*
    if ( angle == 0.0 )
    {
        DoDrawText(text, x, y);
    }
    else
    {
        LOGFONT lf;
        wxFillLogFont(&lf, &m_font);

        // GDI wants the angle in tenth of degree
        long angle10 = (long)(angle * 10);
        lf.lfEscapement = angle10;
        lf. lfOrientation = angle10;

        HFONT hfont = ::CreateFontIndirect(&lf);
        if ( !hfont )
        {
            wxLogLastError("CreateFont");
        }
        else
        {
            HFONT hfontOld = ::SelectObject(GetHdc(), hfont);

            DrawAnyText(text, x, y);

            (void)::SelectObject(GetHdc(), hfontOld);
        }

        // call the bounding box by adding all four vertices of the rectangle
        // containing the text to it (simpler and probably not slower than
        // determining which of them is really topmost/leftmost/...)
        wxCoord w, h;
        GetTextExtent(text, &w, &h);

        double rad = DegToRad(angle);

        // "upper left" and "upper right"
        CalcBoundingBox(x, y);
        CalcBoundingBox(x + w*cos(rad), y - h*sin(rad));
        CalcBoundingBox(x + h*sin(rad), y + h*cos(rad));

        // "bottom left" and "bottom right"
        x += (wxCoord)(h*sin(rad));
        y += (wxCoord)(h*cos(rad));
        CalcBoundingBox(x, y);
        CalcBoundingBox(x + h*sin(rad), y + h*cos(rad));
    }
*/
}

// ---------------------------------------------------------------------------
// set GDI objects
// ---------------------------------------------------------------------------

void wxDC::SetPalette(const wxPalette& palette)
{
   // TODO
}

void wxDC::SetFont(
  const wxFont&                     rFont
)
{
    //
    // Set the old object temporarily, in case the assignment deletes an object
    // that's not yet selected out.
    //
    if (m_hOldFont)
    {
        m_hOldFont = 0;
    }
    m_font = rFont;
    if (!rFont.Ok())
    {
        m_hOldFont = 0;
    }

    m_font.SetPS(m_hPS); // this will realize the font

    if (m_font.Ok())
    {
        HFONT                       hFont = m_font.GetResourceHandle();
        if (hFont == (HFONT) NULL)
        {
            wxLogDebug(wxT("::SelectObject failed in wxDC::SetFont."));
        }
        if (!m_hOldFont)
            m_hOldFont = (WXHFONT) hFont;
    }
} // end of wxDC::SetFont

void wxDC::SetPen(
  const wxPen&                      rPen
)
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (m_pen == rPen)
        return;
    m_pen = rPen;
    if (!m_pen.Ok())
        return;

    if (m_hOldPen)
        m_hOldPen = 0L;
    m_pen = rPen;

    if (!m_pen.Ok())
    {
        if (m_hOldPen)
        {
            m_pen.SetPS((HPS)m_hOldPen);
        }
        m_hOldPen = 0L;
    }

    if (m_pen.Ok())
    {
        if (m_pen.GetResourceHandle())
        {
            m_pen.SetPS(m_hPS);
            if (!m_hOldPen)
                m_hOldPen = m_pen.GetPS();
        }
    }
}

void wxDC::SetBrush(
  const wxBrush&                    rBrush
)
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (m_brush == rBrush)
        return;
    m_brush = rBrush;
    if (!m_brush.Ok())
        return;

    if (m_hOldBrush)
        m_hOldBrush = 0L;
    m_brush = rBrush;

    if (!m_brush.Ok())
    {
        if (m_hOldBrush)
        {
            m_brush.SetPS((HPS)m_hOldBrush);
        }
        m_hOldBrush = 0L;
    }

    if (m_brush.Ok())
    {
        if (m_brush.GetResourceHandle())
        {
            m_brush.SetPS(m_hPS);
            if (!m_hOldBrush)
                m_hOldBrush = m_brush.GetPS();
        }
    }
} // end of wxDC::SetBrush

void wxDC::SetBackground(const wxBrush& brush)
{
   // TODO
}

void wxDC::SetBackgroundMode(
  int                               nMode
)
{
    m_backgroundMode = nMode;
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

wxCoord wxDC::GetCharHeight() const
{
    FONTMETRICS                     vFM; // metrics structure

    ::GpiQueryFontMetrics( m_hPS
                          ,sizeof(FONTMETRICS)
                          ,&vFM
                         );
    return YDEV2LOGREL(vFM.lXHeight);
}

wxCoord wxDC::GetCharWidth() const
{
    FONTMETRICS                     vFM; // metrics structure

    ::GpiQueryFontMetrics( m_hPS
                          ,sizeof(FONTMETRICS)
                          ,&vFM
                         );
    return XDEV2LOGREL(vFM.lAveCharWidth);
}

void wxDC::DoGetTextExtent(
  const wxString&                   rsString
, wxCoord*                          pvX
, wxCoord*                          pvY
, wxCoord*                          pvDescent
, wxCoord*                          pvExternalLeading
, wxFont*                           pTheFont
) const
{
    POINTL                          avPoint[TXTBOX_COUNT];
    POINTL                          vPtMin;
    POINTL                          vPtMax;
    int                             i;
    int                             l;
    FONTMETRICS                     vFM; // metrics structure
    BOOL                            bRc;
    char*                           pStr;
    ERRORID                         vErrorCode; // last error id code
    wxFont*                         pFontToUse = (wxFont*)pTheFont;

    char                            zMsg[128]; // DEBUG
    wxString                        sError;

    if (!pFontToUse)
        pFontToUse = (wxFont*)&m_font;
    l = rsString.length();
    pStr = (PCH) rsString.c_str();

    //
    // In world coordinates.
    //
    bRc = ::GpiQueryTextBox( m_hPS
                            ,l
                            ,pStr
                            ,TXTBOX_COUNT // return maximum information
                            ,avPoint      // array of coordinates points
                           );
    if(!bRc)
    {
       vErrorCode = ::WinGetLastError(wxGetInstance());
       sError = wxPMErrorToStr(vErrorCode);
       // DEBUG
       sprintf(zMsg, "GpiQueryTextBox for %s: failed with Error: %x - %s", pStr, vErrorCode, sError.c_str());
       (void)wxMessageBox( "wxWindows Menu sample"
                          ,zMsg
                          ,wxICON_INFORMATION
                         );
    }

    vPtMin.x = avPoint[0].x;
    vPtMax.x = avPoint[0].x;
    vPtMin.y = avPoint[0].y;
    vPtMax.y = avPoint[0].y;
    for (i = 1; i < 4; i++)
    {
        if(vPtMin.x > avPoint[i].x) vPtMin.x = avPoint[i].x;
        if(vPtMin.y > avPoint[i].y) vPtMin.y = avPoint[i].y;
        if(vPtMax.x < avPoint[i].x) vPtMax.x = avPoint[i].x;
        if(vPtMax.y < avPoint[i].y) vPtMax.y = avPoint[i].y;
    }
    ::GpiQueryFontMetrics( m_hPS
                          ,sizeof(FONTMETRICS)
                          ,&vFM
                         );

    if (pvX)
        *pvX = (wxCoord)(vPtMax.x - vPtMin.x + 1);
    if (pvY)
        *pvY = (wxCoord)(vPtMax.y - vPtMin.y + 1);
    if (pvDescent)
        *pvDescent = vFM.lMaxDescender;
    if (pvExternalLeading)
        *pvExternalLeading = vFM.lExternalLeading;
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

void wxDC::SetLogicalOrigin( wxCoord x, wxCoord y )
{
    // TODO:
};

void wxDC::SetDeviceOrigin(
  wxCoord                           x
, wxCoord                           y
)
{
    RECTL                           vRect;

    m_deviceOriginX = x;
    m_deviceOriginY = y;
    ::GpiQueryPageViewport( m_hPS
                           ,&vRect
                          );
    vRect.xLeft += x;
    vRect.xRight += x;
    vRect.yBottom -= y;
    vRect.yTop -= y;
    ::GpiSetPageViewport( m_hPS
                         ,&vRect
                        );
};

// ---------------------------------------------------------------------------
// coordinates transformations
// ---------------------------------------------------------------------------

wxCoord wxDCBase::DeviceToLogicalX(wxCoord x) const
{
    return (wxCoord) (((x) - m_deviceOriginX)/(m_logicalScaleX*m_userScaleX*m_signX*m_scaleX) - m_logicalOriginX);
}

wxCoord wxDCBase::DeviceToLogicalXRel(wxCoord x) const
{
    return (wxCoord) ((x)/(m_logicalScaleX*m_userScaleX*m_signX*m_scaleX));
}

wxCoord wxDCBase::DeviceToLogicalY(wxCoord y) const
{
    return (wxCoord) (((y) - m_deviceOriginY)/(m_logicalScaleY*m_userScaleY*m_signY*m_scaleY) - m_logicalOriginY);
}

wxCoord wxDCBase::DeviceToLogicalYRel(wxCoord y) const
{
    return (wxCoord) ((y)/(m_logicalScaleY*m_userScaleY*m_signY*m_scaleY));
}

wxCoord wxDCBase::LogicalToDeviceX(wxCoord x) const
{
    return (wxCoord) ((x - m_logicalOriginX)*m_logicalScaleX*m_userScaleX*m_signX*m_scaleX + m_deviceOriginX);
}

wxCoord wxDCBase::LogicalToDeviceXRel(wxCoord x) const
{
    return (wxCoord) (x*m_logicalScaleX*m_userScaleX*m_signX*m_scaleX);
}

wxCoord wxDCBase::LogicalToDeviceY(wxCoord y) const
{
    return (wxCoord) ((y - m_logicalOriginY)*m_logicalScaleY*m_userScaleY*m_signY*m_scaleY + m_deviceOriginY);
}

wxCoord wxDCBase::LogicalToDeviceYRel(wxCoord y) const
{
    return (wxCoord) (y*m_logicalScaleY*m_userScaleY*m_signY*m_scaleY);
}

// ---------------------------------------------------------------------------
// bit blit
// ---------------------------------------------------------------------------

bool wxDC::DoBlit(
  wxCoord                           vXdest
, wxCoord                           vYdest
, wxCoord                           vWidth
, wxCoord                           vHeight
, wxDC*                             pSource
, wxCoord                           vXsrc
, wxCoord                           vYsrc
, int                               nRop
, bool                              bUseMask
)
{
    wxMask*                         pMask = NULL;
    CHARBUNDLE                      vCbnd;
    COLORREF                        vOldTextColor;
    COLORREF                        vOldBackground = ::GpiQueryBackColor(m_hPS);

    if (bUseMask)
    {
        const wxBitmap&             rBmp = pSource->m_vSelectedBitmap;

        pMask = rBmp.GetMask();
        if (!(rBmp.Ok() && pMask && pMask->GetMaskBitmap()))
        {
            bUseMask = FALSE;
        }
    }

    ::GpiQueryAttrs( m_hPS
                    ,PRIM_CHAR
                    ,CBB_COLOR
                    ,&vCbnd
                   );
    vOldTextColor = (COLORREF)vCbnd.lColor;

    if (m_textForegroundColour.Ok())
    {
        vCbnd.lColor = (LONG)m_textForegroundColour.GetPixel();
        ::GpiSetAttrs( m_hPS           // presentation-space handle
                      ,PRIM_CHAR       // Char primitive.
                      ,CBB_COLOR       // sets color.
                      ,0
                      ,&vCbnd          // buffer for attributes.
                     );
    }
    if (m_textBackgroundColour.Ok())
    {
        ::GpiSetBackColor(m_hPS, (LONG)m_textBackgroundColour.GetPixel());
    }

    LONG                            lRop = ROP_SRCCOPY;

    switch (nRop)
    {
        case wxXOR:          lRop = ROP_SRCINVERT;        break;
        case wxINVERT:       lRop = ROP_DSTINVERT;        break;
        case wxOR_REVERSE:   lRop = 0x00DD0228;           break;
        case wxAND_REVERSE:  lRop = ROP_SRCERASE;         break;
        case wxCLEAR:        lRop = ROP_ZERO;             break;
        case wxSET:          lRop = ROP_ONE;              break;
        case wxOR_INVERT:    lRop = ROP_MERGEPAINT;       break;
        case wxAND:          lRop = ROP_SRCAND;           break;
        case wxOR:           lRop = ROP_SRCPAINT;         break;
        case wxEQUIV:        lRop = 0x00990066;           break;
        case wxNAND:         lRop = 0x007700E6;           break;
        case wxAND_INVERT:   lRop = 0x00220326;           break;
        case wxCOPY:         lRop = ROP_SRCCOPY;          break;
        case wxNO_OP:        lRop = ROP_NOTSRCERASE;      break;
        case wxSRC_INVERT:   lRop = ROP_SRCINVERT;        break;
        case wxNOR:          lRop = ROP_NOTSRCCOPY;       break;
        default:
           wxFAIL_MSG( wxT("unsupported logical function") );
           return FALSE;
    }

    bool                            bSuccess;

    if (bUseMask)
    {
        //
        // Blit bitmap with mask
        //

        //
        // Create a temp buffer bitmap and DCs/PSs to access it and the mask
        //
        HDC                             hDCMask;
        HDC                             hDCBuffer;
        HPS                             hPSMask;
        HPS                             hPSBuffer;
        DEVOPENSTRUC                    vDOP = {0L, "DISPLAY", NULL, 0L, 0L, 0L, 0L, 0L, 0L};
        BITMAPINFOHEADER2               vBmpHdr;
        SIZEL                           vSize = {0, 0};
        LONG                            rc;

        hDCMask = ::DevOpenDC(vHabmain, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&vDOP, NULLHANDLE);
        hDCBuffer = ::DevOpenDC(vHabmain, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&vDOP, NULLHANDLE);
        hPSMask = ::GpiCreatePS(vHabmain, hDCMask, &vSize, PU_PELS | GPIT_MICRO | GPIA_ASSOC);
        hPSBuffer = ::GpiCreatePS(vHabmain, hDCBuffer, &vSize, PU_PELS | GPIT_MICRO | GPIA_ASSOC);

        memset(&vBmpHdr, 0, sizeof(BITMAPINFOHEADER2));
        vBmpHdr.cbFix     = sizeof(BITMAPINFOHEADER2);
        vBmpHdr.cx        = vWidth;
        vBmpHdr.cy        = vHeight;
        vBmpHdr.cPlanes   = 1;
        vBmpHdr.cBitCount = 24;

        HBITMAP                         hBufBitmap = ::GpiCreateBitmap(GetHPS(), &vBmpHdr, 0L, NULL, NULL);
        POINTL                          aPoint1[4] = { 0, 0
                                                      ,vWidth, vHeight
                                                      ,vXdest, vYdest
                                                      ,vXdest + vWidth, vYdest + vHeight
                                                     };
        POINTL                          aPoint2[4] = { 0, 0
                                                      ,vWidth, vHeight
                                                      ,vXsrc, vYsrc
                                                      ,vXsrc + vWidth, vYsrc + vHeight
                                                     };
        POINTL                          aPoint3[4] = { vXdest, vYdest
                                                      ,vXdest + vWidth, vYdest + vHeight
                                                      ,vXsrc, vYsrc
                                                      ,vXsrc + vWidth, vYsrc + vHeight
                                                     };
        POINTL                          aPoint4[4] = { vXdest, vYdest
                                                      ,vXdest + vWidth, vYdest + vHeight
                                                      ,0, 0
                                                      ,vWidth, vHeight
                                                     };
        ::GpiSetBitmap(hPSMask, (HBITMAP) pMask->GetMaskBitmap());
        ::GpiSetBitmap(hPSBuffer, (HBITMAP) hBufBitmap);

        //
        // Copy dest to buffer
        //
        rc = ::GpiBitBlt( hPSBuffer
                         ,GetHPS()
                         ,4L
                         ,aPoint1
                         ,ROP_SRCCOPY
                         ,BBO_IGNORE
                        );
        if (rc == GPI_ERROR)
        {
            wxLogLastError(wxT("BitBlt"));
        }

        //
        // Copy src to buffer using selected raster op
        //
        rc = ::GpiBitBlt( hPSBuffer
                         ,GetHPS()
                         ,4L
                         ,aPoint2
                         ,lRop
                         ,BBO_IGNORE
                        );
        if (rc == GPI_ERROR)
        {
            wxLogLastError(wxT("BitBlt"));
        }

        //
        // Set masked area in buffer to BLACK (pixel value 0)
        //
        COLORREF                        vPrevBkCol = ::GpiQueryBackColor(GetHPS());
        COLORREF                        vPrevCol = ::GpiQueryColor(GetHPS());

        ::GpiSetBackColor(GetHPS(), OS2RGB(255, 255, 255));
        ::GpiSetColor(GetHPS(), OS2RGB(0, 0, 0));

        rc = ::GpiBitBlt( hPSBuffer
                         ,hPSMask
                         ,4L
                         ,aPoint2
                         ,ROP_SRCAND
                         ,BBO_IGNORE
                        );
        if (rc == GPI_ERROR)
        {
            wxLogLastError(wxT("BitBlt"));
        }

        //
        // Set unmasked area in dest to BLACK
        //
        ::GpiSetBackColor(GetHPS(), OS2RGB(0, 0, 0));
        ::GpiSetColor(GetHPS(), OS2RGB(255, 255, 255));
        rc = ::GpiBitBlt( GetHPS()
                         ,hPSMask
                         ,4L
                         ,aPoint3
                         ,ROP_SRCAND
                         ,BBO_IGNORE
                        );
        if (rc == GPI_ERROR)
        {
            wxLogLastError(wxT("BitBlt"));
        }

        //
        // Restore colours to original values
        //
        ::GpiSetBackColor(GetHPS(), vPrevBkCol);
        ::GpiSetColor(GetHPS(), vPrevCol);

        //
        // OR buffer to dest
        //
        rc = ::GpiBitBlt( GetHPS()
                         ,hPSMask
                         ,4L
                         ,aPoint4
                         ,ROP_SRCPAINT
                         ,BBO_IGNORE
                        );
        if (rc == GPI_ERROR)
        {
            bSuccess = FALSE;
            wxLogLastError(wxT("BitBlt"));
        }

        //
        // Tidy up temporary DCs and bitmap
        //
        ::GpiSetBitmap(hPSMask, NULLHANDLE);
        ::GpiSetBitmap(hPSBuffer, NULLHANDLE);
        ::GpiDestroyPS(hPSMask);
        ::GpiDestroyPS(hPSBuffer);
        ::DevCloseDC(hDCMask);
        ::DevCloseDC(hDCBuffer);
        ::GpiDeleteBitmap(hBufBitmap);
        bSuccess = TRUE;
    }
    else // no mask, just BitBlt() it
    {
        POINTL                          aPoint[4] = { vXdest, vYdest
                                                     ,vXdest + vWidth, vYdest + vHeight
                                                     ,vXsrc, vYsrc
                                                     ,vXsrc + vWidth, vYsrc + vHeight
                                                    };

        bSuccess = (::GpiBitBlt( m_hPS
                                ,pSource->GetHPS()
                                ,4L
                                ,aPoint
                                ,lRop
                                ,BBO_IGNORE
                               ) != GPI_ERROR);
        if (!bSuccess )
        {
            wxLogLastError(wxT("BitBlt"));
        }
    }
    vCbnd.lColor = (LONG)vOldTextColor;
    ::GpiSetAttrs( m_hPS           // presentation-space handle
                  ,PRIM_CHAR       // Char primitive.
                  ,CBB_COLOR       // sets color.
                  ,0
                  ,&vCbnd          // buffer for attributes.
                 );
    ::GpiSetBackColor(m_hPS, (LONG)vOldBackground);
    return bSuccess;
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
    wxCoord x1, y1, descent1, externalLeading1;
    GetTextExtent(string, & x1, & y1, & descent1, & externalLeading1, theFont, use16bit);
    *x = x1; *y = y1;
    if (descent)
        *descent = descent1;
    if (externalLeading)
        *externalLeading = externalLeading1;
}
#endif

