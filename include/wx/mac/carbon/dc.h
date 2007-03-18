/////////////////////////////////////////////////////////////////////////////
// Name:        dc.h
// Purpose:     wxDC class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DC_H_
#define _WX_DC_H_

#include "wx/pen.h"
#include "wx/brush.h"
#include "wx/icon.h"
#include "wx/font.h"
#include "wx/gdicmn.h"

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

#ifndef MM_TEXT
#define MM_TEXT            0
#define MM_ISOTROPIC    1
#define MM_ANISOTROPIC    2
#define MM_LOMETRIC        3
#define MM_HIMETRIC        4
#define MM_TWIPS        5
#define MM_POINTS        6
#define MM_METRIC        7
#endif


class wxMacPortStateHelper;

#if !wxUSE_GRAPHICS_CONTEXT

class WXDLLEXPORT wxGraphicPath
{
public :
    virtual ~wxGraphicPath() {}

    virtual void MoveToPoint( wxCoord x1, wxCoord y1 ) = 0;

    virtual void AddLineToPoint( wxCoord x1, wxCoord y1 ) = 0;

    virtual void AddQuadCurveToPoint( wxCoord cx1, wxCoord cy1, wxCoord x1, wxCoord y1 ) = 0;

    virtual void AddRectangle( wxCoord x, wxCoord y, wxCoord w, wxCoord h ) = 0;
    
    virtual void AddCircle( wxCoord x, wxCoord y, wxCoord r ) = 0;
    
    // draws a an arc to two tangents connecting (current) to (x1,y1) and (x1,y1) to (x2,y2), also a straight line from (current) to (x1,y1)
    virtual void AddArcToPoint( wxCoord x1, wxCoord y1 , wxCoord x2, wxCoord y2, wxCoord r ) = 0 ;

    virtual void AddArc( wxCoord x, wxCoord y, wxCoord r, double startAngle, double endAngle, bool clockwise ) = 0 ;

    virtual void CloseSubpath() = 0;
};

class WXDLLEXPORT wxGraphicContext
{
public:
    virtual ~wxGraphicContext() {}

    virtual wxGraphicPath * CreatePath() = 0;
        
    virtual void PushState() = 0 ;
    
    virtual void PopState() = 0 ;

    virtual void Clip( const wxRegion &region ) = 0;

    virtual void SetPen( const wxPen &pen ) = 0;

    virtual void SetBrush( const wxBrush &brush ) = 0;
    
    virtual void SetFont( const wxFont &font ) = 0 ;
    
    virtual void SetTextColor( const wxColour &col ) = 0 ;

    virtual void StrokePath( const wxGraphicPath *path ) = 0;

    virtual void DrawPath( const wxGraphicPath *path, int fillStyle = wxWINDING_RULE ) = 0;

    virtual void FillPath( const wxGraphicPath *path, const wxColor &fillColor, int fillStyle = wxWINDING_RULE ) = 0;

    virtual void DrawBitmap( const wxBitmap &bmp, wxCoord x, wxCoord y, wxCoord w, wxCoord h ) = 0 ;
    
    virtual void DrawIcon( const wxIcon &icon, wxCoord x, wxCoord y, wxCoord w, wxCoord h ) = 0 ;
    
    virtual void DrawText( const wxString &str, wxCoord x, wxCoord y, double angle ) = 0 ;
    
    virtual void GetTextExtent( const wxString &text, wxCoord *width, wxCoord *height,
                            wxCoord *descent, wxCoord *externalLeading ) const  = 0 ;
    
    virtual void GetPartialTextExtents(const wxString& text, wxArrayInt& widths) const = 0 ;
    
    virtual void Translate( wxCoord dx , wxCoord dy ) = 0 ;
    
    virtual void Scale( wxCoord xScale , wxCoord yScale ) = 0 ;
};

class WXDLLEXPORT wxDC: public wxDCBase
{
    DECLARE_DYNAMIC_CLASS(wxDC)
    DECLARE_NO_COPY_CLASS(wxDC)

public:
    wxDC();
    virtual ~wxDC();

    // implement base class pure virtuals
    // ----------------------------------

    virtual void Clear();

    virtual bool StartDoc( const wxString& WXUNUSED(message) ) { return true; }
    virtual void EndDoc(void) {}

    virtual void StartPage(void) {}
    virtual void EndPage(void) {}

    virtual void SetFont(const wxFont& font);
    virtual void SetPen(const wxPen& pen);
    virtual void SetBrush(const wxBrush& brush);
    virtual void SetBackground(const wxBrush& brush);
    virtual void SetBackgroundMode(int mode);
    virtual void SetPalette(const wxPalette& palette);

    virtual void DestroyClippingRegion();

    virtual wxCoord GetCharHeight() const;
    virtual wxCoord GetCharWidth() const;

    virtual bool CanDrawBitmap() const;
    virtual bool CanGetTextExtent() const;
    virtual int GetDepth() const;
    virtual wxSize GetPPI() const;

    virtual void SetMapMode(int mode);
    virtual void SetUserScale(double x, double y);

    virtual void SetLogicalScale(double x, double y);
    virtual void SetLogicalOrigin(wxCoord x, wxCoord y);
    virtual void SetDeviceOrigin(wxCoord x, wxCoord y);
    virtual void SetAxisOrientation(bool xLeftRight, bool yBottomUp);
    virtual void SetLogicalFunction(int function);

    virtual void SetTextForeground(const wxColour& colour);
    virtual void SetTextBackground(const wxColour& colour);

    virtual void ComputeScaleAndOrigin();

public:
    wxCoord XDEV2LOG(wxCoord x) const
    {
        return wxRound((double)(x - m_deviceOriginX) / m_scaleX) * m_signX + m_logicalOriginX;
    }
    wxCoord XDEV2LOGREL(wxCoord x) const
    {
        return wxRound((double)(x) / m_scaleX);
    }
    wxCoord YDEV2LOG(wxCoord y) const
    {
        return wxRound((double)(y - m_deviceOriginY) / m_scaleY) * m_signY + m_logicalOriginY;
    }
    wxCoord YDEV2LOGREL(wxCoord y) const
    {
        return wxRound((double)(y) / m_scaleY);
    }
    wxCoord XLOG2DEV(wxCoord x) const
    {
        return wxRound((double)(x - m_logicalOriginX) * m_scaleX) * m_signX + m_deviceOriginX;
    }
    wxCoord XLOG2DEVREL(wxCoord x) const
    {
        return wxRound((double)(x) * m_scaleX);
    }
    wxCoord YLOG2DEV(wxCoord y) const
    {
        return wxRound((double)(y - m_logicalOriginY) * m_scaleY) * m_signY + m_deviceOriginY;
    }
    wxCoord YLOG2DEVREL(wxCoord y) const
    {
        return wxRound((double)(y) * m_scaleY);
    }

    wxCoord XLOG2DEVMAC(wxCoord x) const
    {
        return wxRound((double)(x - m_logicalOriginX) * m_scaleX) * m_signX + m_deviceOriginX + m_macLocalOrigin.x;
    }

    wxCoord YLOG2DEVMAC(wxCoord y) const
    {
        return wxRound((double)(y - m_logicalOriginY) * m_scaleY) * m_signY + m_deviceOriginY + m_macLocalOrigin.y;
    }

#if wxMAC_USE_CORE_GRAPHICS
    wxGraphicsContext* GetGraphicsContext() { return m_graphicContext; }
#else
    WXHRGN MacGetCurrentClipRgn() { return m_macCurrentClipRgn; }
    static void MacSetupBackgroundForCurrentPort(const wxBrush& background );
#endif

protected:
    virtual void DoGetTextExtent(const wxString& string,
        wxCoord *x, wxCoord *y,
        wxCoord *descent = NULL,
        wxCoord *externalLeading = NULL,
        wxFont *theFont = NULL) const;

    virtual bool DoGetPartialTextExtents(const wxString& text, wxArrayInt& widths) const;

    virtual bool DoFloodFill(wxCoord x, wxCoord y, const wxColour& col,
                             int style = wxFLOOD_SURFACE);

    virtual bool DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const;

    virtual void DoDrawPoint(wxCoord x, wxCoord y);

#if wxMAC_USE_CORE_GRAPHICS && wxUSE_SPLINES
    virtual void DoDrawSpline(wxList *points);
#endif

    virtual void DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2);

    virtual void DoDrawArc(wxCoord x1, wxCoord y1,
                           wxCoord x2, wxCoord y2,
                           wxCoord xc, wxCoord yc);

    virtual void DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                                   double sa, double ea);

    virtual void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    virtual void DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                        wxCoord width, wxCoord height,
                                        double radius);
    virtual void DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height);

    virtual void DoCrossHair(wxCoord x, wxCoord y);

    virtual void DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y);
    virtual void DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y,
                              bool useMask = false);

    virtual void DoDrawText(const wxString& text, wxCoord x, wxCoord y);
    virtual void DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y,
                                   double angle);

    virtual bool DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
                        wxDC *source, wxCoord xsrc, wxCoord ysrc,
                        int rop = wxCOPY, bool useMask = false, wxCoord xsrcMask = -1, wxCoord ysrcMask = -1);

    virtual bool DoStretchBlit(wxCoord xdest, wxCoord ydest,
                               wxCoord dstWidth, wxCoord dstHeight,
                               wxDC *source,
                               wxCoord xsrc, wxCoord ysrc,
                               wxCoord srcWidth, wxCoord srcHeight,
                               int rop = wxCOPY, bool useMask = false,
                               wxCoord xsrcMask = wxDefaultCoord, wxCoord ysrcMask = wxDefaultCoord);

    // this is gnarly - we can't even call this function DoSetClippingRegion()
    // because of virtual function hiding

    virtual void DoSetClippingRegionAsRegion(const wxRegion& region);
    virtual void DoSetClippingRegion(wxCoord x, wxCoord y,
                                     wxCoord width, wxCoord height);

    virtual void DoGetSizeMM(int *width, int *height) const;

    virtual void DoDrawLines(int n, wxPoint points[],
                             wxCoord xoffset, wxCoord yoffset);
    virtual void DoDrawPolygon(int n, wxPoint points[],
                               wxCoord xoffset, wxCoord yoffset,
                               int fillStyle = wxODDEVEN_RULE);

protected:
    // scaling variables
    double       m_mm_to_pix_x, m_mm_to_pix_y;

     // To be set using SetDeviceOrigin()
     // by external classes such as wxScrolledWindow
    long         m_externalDeviceOriginX, m_externalDeviceOriginY;

#if !wxMAC_USE_CORE_GRAPHICS
    // If un-scrolled is non-zero or d.o. changes with scrolling.
    // Set using SetInternalDeviceOrigin().
    long         m_internalDeviceOriginX, m_internalDeviceOriginY;

    WXHBITMAP            m_macMask;
#endif

    // not yet used
    bool         m_needComputeScaleX, m_needComputeScaleY;

public:
    // implementation
    void                    MacInstallFont() const;

    // in order to preserve the const inheritance of the virtual functions,
    // we have to use mutable variables starting from CWPro 5
    wxPoint                         m_macLocalOrigin;
    mutable void                   *m_macATSUIStyle;

    WXHDC                           m_macPort;

#if wxMAC_USE_CORE_GRAPHICS
    wxGraphicsContext              *m_graphicContext;
#else
    void                            MacInstallPen() const;
    void                            MacInstallBrush() const;

    void                            MacSetupPort( wxMacPortStateHelper *ph ) const;
    void                            MacCleanupPort( wxMacPortStateHelper *ph ) const;

    mutable wxMacPortStateHelper   *m_macCurrentPortStateHelper;

    mutable bool                    m_macFontInstalled;
    mutable bool                    m_macPenInstalled;
    mutable bool                    m_macBrushInstalled;

    WXHRGN                          m_macBoundaryClipRgn;
    WXHRGN                          m_macCurrentClipRgn;
    mutable bool                    m_macFormerAliasState;
    mutable short                   m_macFormerAliasSize;
    mutable bool                    m_macAliasWasEnabled;
    mutable void                   *m_macForegroundPixMap;
    mutable void                   *m_macBackgroundPixMap;
#endif
};

#endif

#endif // _WX_DC_H_
