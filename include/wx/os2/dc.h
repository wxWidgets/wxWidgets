/////////////////////////////////////////////////////////////////////////////
// Name:        dc.h
// Purpose:     wxDC class
// Author:      David Webster
// Modified by:
// Created:     08/26/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DC_H_
#define _WX_DC_H_

#include "wx/defs.h"
#include "wx/dc.h"

// ---------------------------------------------------------------------------
// macros
// ---------------------------------------------------------------------------

// Logical to device
// Absolute
#define XLOG2DEV(x) (x)
#define YLOG2DEV(y) (y)

// Relative
#define XLOG2DEVREL(x) (x)
#define YLOG2DEVREL(y) (y)

// Device to logical
// Absolute
#define XDEV2LOG(x) (x)

#define YDEV2LOG(y) (y)

// Relative
#define XDEV2LOGREL(x) (x)
#define YDEV2LOGREL(y) (y)

/*
 * Have the same macros as for XView but not for every operation:
 * just for calculating window/viewport extent (a better way of scaling).
 */

// Logical to device
// Absolute
#define MS_XLOG2DEV(x) LogicalToDevice(x)

#define MS_YLOG2DEV(y) LogicalToDevice(y)

// Relative
#define MS_XLOG2DEVREL(x) LogicalToDeviceXRel(x)
#define MS_YLOG2DEVREL(y) LogicalToDeviceYRel(y)

// Device to logical
// Absolute
#define MS_XDEV2LOG(x) DeviceToLogicalX(x)

#define MS_YDEV2LOG(y) DeviceToLogicalY(y)

// Relative
#define MS_XDEV2LOGREL(x) DeviceToLogicalXRel(x)
#define MS_YDEV2LOGREL(y) DeviceToLogicalYRel(y)

#define YSCALE(y) (yorigin - (y))

#define     wx_round(a)    (int)((a)+.5)

class WXDLLEXPORT wxDC : public wxDCBase
{
    DECLARE_DYNAMIC_CLASS(wxDC)

public:
    wxDC();
    ~wxDC();

    // implement base class pure virtuals
    // ----------------------------------

    virtual void Clear(void);

    virtual bool    StartDoc(const wxString& rsMessage);
    virtual void    EndDoc(void);

    virtual void    StartPage(void);
    virtual void    EndPage(void);

    virtual void    SetFont(const wxFont& rFont);
    virtual void    SetPen(const wxPen& rPen);
    virtual void    SetBrush(const wxBrush& rBrush);
    virtual void    SetBackground(const wxBrush& rBrush);
    virtual void    SetBackgroundMode(int nMode);
    virtual void    SetPalette(const wxPalette& rPalette);

    virtual void    DestroyClippingRegion(void);

    virtual wxCoord GetCharHeight(void) const;
    virtual wxCoord GetCharWidth(void) const;
    virtual void    DoGetTextExtent( const wxString& rsString
                                    ,wxCoord*        pX
                                    ,wxCoord*        pY
                                    ,wxCoord*        pDescent = NULL
                                    ,wxCoord*        pExternalLeading = NULL
                                    ,wxFont*         pTheFont = NULL
                                   ) const;
    virtual bool    CanDrawBitmap(void) const;
    virtual bool    CanGetTextExtent(void) const;
    virtual int     GetDepth(void) const;
    virtual wxSize  GetPPI(void) const;

    virtual void    SetMapMode(int nMode);
    virtual void    SetUserScale( double dX
                                 ,double dY
                                );
    virtual void    SetSystemScale( double dX
                                   ,double dY
                                  );
    virtual void    SetLogicalScale( double dX
                                    ,double dY
                                   );
    virtual void    SetLogicalOrigin( wxCoord vX
                                     ,wxCoord vY
                                    );
    virtual void    SetDeviceOrigin( wxCoord vX
                                    ,wxCoord vY
                                   );
    virtual void    SetAxisOrientation( bool bXLeftRight
                                       ,bool bYBottomUp
                                      );
    virtual void    SetLogicalFunction(int nFunction);

    // implementation from now on
    // --------------------------

    virtual void    SetRop(WXHDC hCdc);
    virtual void    SelectOldObjects(WXHDC hDc);

    wxWindow*       GetWindow(void) const { return m_pCanvas; }
    void            SetWindow(wxWindow* pWin) { m_pCanvas = pWin; }

    WXHDC           GetHDC(void) const { return m_hDC; }
    void            SetHDC( WXHDC hDc
                           ,bool  bOwnsDC = FALSE
                          )
    {
        m_hDC = hDc;
        m_bOwnsDC = bOwnsDC;
    }

    const wxBitmap& GetSelectedBitmap(void) const { return m_vSelectedBitmap; }
    wxBitmap&       GetSelectedBitmap(void) { return m_vSelectedBitmap; }

protected:
    virtual void DoFloodFill( wxCoord         vX
                             ,wxCoord         vY
                             ,const wxColour& rCol
                             ,int             nStyle = wxFLOOD_SURFACE
                            );

    virtual bool DoGetPixel( wxCoord   vX
                            ,wxCoord   vY
                            ,wxColour* pCol
                           ) const;

    virtual void DoDrawPoint( wxCoord vX
                             ,wxCoord vY
                            );
    virtual void DoDrawLine( wxCoord vX1
                            ,wxCoord vY1
                            ,wxCoord vX2
                            ,wxCoord vY2
                           );

    virtual void DoDrawArc( wxCoord vX1
                           ,wxCoord vY1
                           ,wxCoord vX2
                           ,wxCoord vY2
                           ,wxCoord vXc
                           ,wxCoord vYc
                          );
    virtual void DoDrawCheckMark( wxCoord vX
                                 ,wxCoord vY
                                 ,wxCoord vWidth
                                 ,wxCoord vHeight
                                );
    virtual void DoDrawEllipticArc( wxCoord vX
                                   ,wxCoord vY
                                   ,wxCoord vW
                                   ,wxCoord vH
                                   ,double  dSa
                                   ,double  dEa
                                  );

    virtual void DoDrawRectangle( wxCoord vX
                                 ,wxCoord vY
                                 ,wxCoord vWidth
                                 ,wxCoord vHeight
                                );
    virtual void DoDrawRoundedRectangle( wxCoord vX
                                        ,wxCoord vY
                                        ,wxCoord vWidth
                                        ,wxCoord vHeight
                                        ,double  dRadius
                                       );
    virtual void DoDrawEllipse( wxCoord vX
                               ,wxCoord vY
                               ,wxCoord vWidth
                               ,wxCoord vHeight
                              );

    virtual void DoCrossHair( wxCoord vX
                             ,wxCoord vY
                            );

    virtual void DoDrawIcon( const wxIcon& rIcon
                            ,wxCoord       vX
                            ,wxCoord       vY
                           );
    virtual void DoDrawBitmap( const wxBitmap& rBmp
                              ,wxCoord         vX
                              ,wxCoord         vY
                              ,bool            bUseMask = FALSE
                             );

    virtual void DoDrawText( const wxString& rsText
                            ,wxCoord         vX
                            ,wxCoord         vY
                           );
    virtual void DoDrawRotatedText( const wxString& rsText
                                   ,wxCoord         vX
                                   ,wxCoord         vY
                                   ,double          dAngle
                                  );

    virtual bool DoBlit( wxCoord vXdest
                        ,wxCoord vYdest
                        ,wxCoord vWidth
                        ,wxCoord vHeight
                        ,wxDC*   pSource
                        ,wxCoord vXsrc
                        ,wxCoord vYsrc
                        ,int     nRop = wxCOPY
                        ,bool    bUseMask = FALSE
                       );

    virtual void DoSetClippingRegionAsRegion(const wxRegion& rRegion);
    virtual void DoSetClippingRegion( wxCoord vX
                                     ,wxCoord vY
                                     ,wxCoord vWidth
                                     ,wxCoord vHeight
                                    );
    virtual void DoGetClippingRegion( wxCoord* pX
                                     ,wxCoord* pY
                                     ,wxCoord* pWidth
                                     ,wxCoord* pHeight)
    {
        GetClippingBox( pX
                       ,pY
                       ,pWidth
                       ,pHeight
                      );
    }

    virtual void DoGetSize( int* pWidth
                           ,int* pHeight
                          ) const;
    virtual void DoGetSizeMM( int* pWidth
                             ,int* pHeight
                            ) const;

    virtual void DoDrawLines( int     n
                             ,wxPoint vaPoints[]
                             ,wxCoord vXoffset
                             ,wxCoord yYoffset
                            );
    virtual void DoDrawPolygon( int     n
                               ,wxPoint vaPoints[]
                               ,wxCoord vXoffset
                               ,wxCoord vYoffset
                               ,int     nFillStyle = wxODDEVEN_RULE
                              );

#if wxUSE_SPLINES
    virtual void DoDrawSpline(wxList* pPoints);
#endif // wxUSE_SPLINES

    //
    // common part of DoDrawText() and DoDrawRotatedText()
    //
    void DrawAnyText( const wxString& rsText
                     ,wxCoord         vX
                     ,wxCoord         vY
                    );

    // OS2-specific member variables ?? do we even need this under OS/2?
    int                             m_nWindowExtX;
    int                             m_nWindowExtY;

    //
    // the window associated with this DC (may be NULL)
    //
    wxWindow*                       m_pCanvas;
    wxBitmap                        m_vSelectedBitmap;

    //
    // TRUE => DeleteDC() in dtor, FALSE => only ReleaseDC() it
    //
    bool                            m_bOwnsDC:1;

    //
    // our HDC and its usage count: we only free it when the usage count drops
    // to 0
    //
    WXHDC                           m_hDC;
    int                             m_nDCCount;

    //
    // Store all old GDI objects when do a SelectObject, so we can select them
    // back in (this unselecting user's objects) so we can safely delete the
    // DC.
    //
    WXHBITMAP                       m_hOldBitmap;
    WXHPEN                          m_hOldPen;
    WXHBRUSH                        m_hOldBrush;
    WXHFONT                         m_hOldFont;
    WXHPALETTE                      m_hOldPalette;
};
#endif
    // _WX_DC_H_
