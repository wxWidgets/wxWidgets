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
    m_bIsPaintTime = FALSE;// True at Paint Time
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
        if (m_hOldBitmap)
        {
//            ::SelectObject((HDC) dc, (HBITMAP) m_oldBitmap);
            if (m_vSelectedBitmap.Ok())
            {
                m_vSelectedBitmap.SetSelectedInto(NULL);
            }
        }
        m_hOldBitmap = 0;
        if (m_hOldPen)
        {
//            ::SelectObject((HDC) dc, (HPEN) m_oldPen);
        }
        m_hOldPen = 0;
        if (m_hOldBrush)
        {
//            ::SelectObject((HDC) dc, (HBRUSH) m_oldBrush);
        }
        m_hOldBrush = 0;
        if (m_hOldFont)
        {
//            ::SelectObject((HDC) dc, (HFONT) m_oldFont);
        }
        m_hOldFont = 0;
        if (m_hOldPalette)
        {
//            ::SelectPalette((HDC) dc, (HPALETTE) m_oldPalette, TRUE);
        }
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

#define DO_SET_CLIPPING_BOX()                   \
{                                               \
    RECT rect;                                  \
                                                \
    GetClipBox(GetHdc(), &rect);                \
                                                \
    m_clipX1 = (wxCoord) XDEV2LOG(rect.left);   \
    m_clipY1 = (wxCoord) YDEV2LOG(rect.top);    \
    m_clipX2 = (wxCoord) XDEV2LOG(rect.right);  \
    m_clipY2 = (wxCoord) YDEV2LOG(rect.bottom); \
}

void wxDC::DoSetClippingRegion( wxCoord x, wxCoord y
                               ,wxCoord width, wxCoord height
                              )
{
   // TODO
}

void wxDC::DoSetClippingRegionAsRegion(const wxRegion& region)
{
   // TODO
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
//    *pCol.Set(lColor);
    if(lColor>= 0)
        return(TRUE);
   else
        return(FALSE);
}

void wxDC::DoCrossHair(wxCoord x, wxCoord y)
{
   // TODO
}

void wxDC::DoDrawLine(
  wxCoord                           vX1
, wxCoord                           vY1
, wxCoord                           vX2
, wxCoord                           vY2
)
{
    POINTL                          vPoint[2];

    vPoint[0].x = vX1;
    vPoint[0].y = vY1;
    vPoint[1].x = vX2;
    vPoint[1].y = vY2;
    // ::GpiSetColor(m_hPS,CLR_RED); //DEbug
    ::GpiMove(m_hPS, &vPoint[0]);
    ::GpiLine(m_hPS, &vPoint[1]);
}

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
        DoDrawArc( vX1
                  ,vY1
                  ,vXm
                  ,vYm
                  ,vXc
                  ,vYc
                 );
        DoDrawArc( vXm
                  ,vYm
                  ,vX2
                  ,vY2
                  ,vXc
                  ,vYc
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
    vPtlArc[0].x =  vXm;
    vPtlArc[0].y =  vYm;
    vPtlArc[1].x = vX2;
    vPtlArc[1].y = vY2;
    ::GpiPointArc(m_hPS, vPtlArc); // Draws the arc
}

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
}

void wxDC::DoDrawPoint(
  wxCoord                           vX
, wxCoord                           vY
)
{
    POINTL                          vPoint;

    vPoint.x = vX;
    vPoint.y = vY;
    ::GpiSetPel(m_hPS, &vPoint);
}

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
}

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
}

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
    vPoint[0].y = vY;
    vPoint[1].x = vX + vWidth;
    vPoint[1].y = vY - vHeight;      //mustdie !!! ??
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

//EK    ::GpiSetColor(m_hPS,lBorderColor);
        ::GpiSetColor(m_hPS,CLR_GREEN);
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
        ::GpiBox( m_hPS
                 ,lControl
                 ,&vPoint[1]
                 ,0L
                 ,0L
                );
    }
}

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
    vPoint[0].y = vY;
    vPoint[1].x = vX + vWidth;
    vPoint[1].y = vY + vHeight;
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
}

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
}

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
}

void wxDC::DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y)
{
    // TODO:
}

void wxDC::DoDrawBitmap( const wxBitmap &bmp
                        ,wxCoord x, wxCoord y
                        ,bool useMask
                       )
{
   // TODO
}

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
//        ::SelectObject(GetHdc(), (HFONT) m_hOldFont);
        m_hOldFont = 0;
    }

    m_font = rFont;

    if (!rFont.Ok())
    {
        if (m_hOldFont)
//            ::SelectObject(GetHdc(), (HFONT) m_hOldFont);
        m_hOldFont = 0;
    }

    if (m_font.Ok() && m_font.GetResourceHandle())
    {
        HFONT                       hFont = (HFONT)0; //::SelectObject(GetHdc(), (HFONT) m_font.GetResourceHandle());
        if (hFont == (HFONT) NULL)
        {
            wxLogDebug(wxT("::SelectObject failed in wxDC::SetFont."));
        }
        if (!m_hOldFont)
            m_hOldFont = (WXHFONT) hFont;
    }
}

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

    int                             nWidth = m_pen.GetWidth();

    if (nWidth <= 0)
    {
        nWidth = 1;
    }
    else
    {
        double                      dW = 0.5 +
                                       ( fabs((double) XLOG2DEVREL(nWidth)) +
                                         fabs((double) YLOG2DEVREL(nWidth))
                                       ) / 2.0;
        nWidth = (int)dW;
    }
    wxColour                        vColor = m_pen.GetColour();

    ::GpiSetColor( m_hPS
                  ,vColor.GetPixel()
                 ); //DEbug ??

    int                             nLinetype;
    int                             nStyle = m_pen.GetStyle();

    nLinetype = LINETYPE_DEFAULT;
    switch(nStyle)
    {
        case wxDOT:
            nLinetype = LINETYPE_DOT;
            break;

        case wxLONG_DASH:
            nLinetype = LINETYPE_LONGDASH;
            break;

        case wxSHORT_DASH:
            nLinetype = LINETYPE_SHORTDASH;
            break;

        case wxDOT_DASH:
            nLinetype = LINETYPE_DASHDOT;
            break;

        case wxTRANSPARENT:
            nLinetype = LINETYPE_INVISIBLE;
            break;

        case wxSOLID:
            nLinetype = LINETYPE_SOLID;
            break;
    }
    ::GpiSetLineType( m_hPS
                     ,nLinetype
                    );

    nWidth =  m_pen.GetWidth();
    ::GpiSetLineWidth( m_hPS
                      ,MAKEFIXED( nWidth
                                 ,0
                                )
                     );
}

void wxDC::SetBrush(
  const wxBrush&                    rBrush
)
{
   // TODO
}

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
    // TODO
    return(8);
}

wxCoord wxDC::GetCharWidth() const
{
    // TODO
    return(8);
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

void wxDC::SetDeviceOrigin( wxCoord x, wxCoord y )
{
    // TODO:
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

bool wxDC::DoBlit( wxCoord xdest
                  ,wxCoord ydest
                  ,wxCoord width
                  ,wxCoord height
                  ,wxDC *source
                  ,wxCoord xsrc
                  ,wxCoord ysrc
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
    wxCoord x1, y1, descent1, externalLeading1;
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

