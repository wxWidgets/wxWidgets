/////////////////////////////////////////////////////////////////////////////
// Name:        dc.h
// Purpose:     wxDC class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DC_H_
#define _WX_DC_H_

#ifdef __GNUG__
#pragma interface "dc.h"
#endif

#include "wx/window.h"
#include "wx/pen.h"
#include "wx/brush.h"
#include "wx/icon.h"
#include "wx/font.h"
#include "wx/gdicmn.h"

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

#ifndef MM_TEXT
#define MM_TEXT               0
#define MM_ISOTROPIC          1
#define MM_ANISOTROPIC        2
#define MM_LOMETRIC           3
#define MM_HIMETRIC           4
#define MM_TWIPS              5
#define MM_POINTS             6
#define MM_METRIC             7
#endif

//-----------------------------------------------------------------------------
// global variables
//-----------------------------------------------------------------------------

extern int wxPageNumber;

//-----------------------------------------------------------------------------
// wxDC
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxDC: public wxDCBase
{
  DECLARE_ABSTRACT_CLASS(wxDC)

  public:

    wxDC(void);
    ~wxDC(void);

    virtual void BeginDrawing(void) { };
    virtual void EndDrawing(void) { };

//-----------------------------------------------------------------------------
// Pure virtuals from wxDCBase that need defining
//-----------------------------------------------------------------------------
    virtual bool   CanDrawBitmap(void) const { return TRUE; };
    virtual bool   CanGetTextExtent(void) const { return TRUE; };
    virtual int    GetDepth(void) const;
    virtual wxSize GetPPI(void) const;
    virtual void   GetTextExtent( const wxString& string
                                 ,long*           width
                                 ,long*           height
                                 ,long*           descent = NULL
                                 ,long*           externalLeading = NULL
                                 ,wxFont*         theFont = NULL
                                ) const;
    virtual long   GetCharWidth(void) const;
    virtual long   GetCharHeight(void) const;
    virtual void   Clear(void);
    virtual void   SetFont(const wxFont& font);
    virtual void   SetPen(const wxPen& pen);
    virtual void   SetBrush(const wxBrush& brush);
    virtual void   SetBackground(const wxBrush& brush);
    virtual void   SetLogicalFunction(int function);
    virtual void   SetBackgroundMode(int mode);
    virtual void   SetPalette(const wxPalette& palette);
    virtual void   SetMapMode(int mode);
    virtual void   DestroyClippingRegion(void);
    virtual void   SetUserScale(double x, double y);
    virtual void   SetLogicalOrigin(long x, long y);
    virtual void   SetDeviceOrigin(long x, long y);
    virtual void   SetAxisOrientation(bool xLeftRight, bool yBottomUp);

    virtual void   DoFloodFill( long x
                               ,long y
                               ,const wxColour& col
                               ,int style = wxFLOOD_SURFACE
                              );
    virtual bool   DoGetPixel(long x, long y, wxColour *col) const;
    virtual void   DoDrawPoint(long x, long y);
    virtual void   DoDrawLine(long x1, long y1, long x2, long y2);
    virtual void   DoDrawArc(long x1, long y1,
                             long x2, long y2,
                             long xc, long yc
                            );
    virtual void   DoDrawEllipticArc( long x, long y
                                     ,long w, long h
                                     ,double sa, double ea
                                    );
    virtual void   DoDrawRectangle(long x, long y, long width, long height);
    virtual void   DoDrawRoundedRectangle( long x, long y
                                          ,long width, long height
                                          ,double radius
                                         );
    virtual void   DoDrawEllipse(long x, long y, long width, long height);
    virtual void   DoCrossHair(long x, long y);
    virtual void   DoDrawIcon(const wxIcon& icon, long x, long y);
    virtual void   DoDrawBitmap( const wxBitmap &bmp
                                ,long x, long y
                                ,bool useMask = FALSE
                               );
    virtual void   DoDrawText(const wxString& text, long x, long y);
    virtual bool   DoBlit( long xdest, long ydest
                          ,long width, long height
                          ,wxDC *source
                          ,long xsrc, long ysrc
                          ,int rop = wxCOPY, bool useMask = FALSE
                         );
    virtual void   DoGetSize(int *width, int *height) const;
    virtual void   DoGetSizeMM(int* width, int* height) const;
    virtual void   DoDrawLines( int n, wxPoint points[]
                               ,long xoffset, long yoffset
                              );
    virtual void   DoDrawPolygon( int n, wxPoint points[]
                                 ,long xoffset, long yoffset
                                 ,int fillStyle = wxODDEVEN_RULE
                                );
    virtual void   DoSetClippingRegionAsRegion(const wxRegion& region);
    virtual void   DoSetClippingRegion( long x, long y
                                       ,long width, long height
                                      );
#if wxUSE_SPLINES
    virtual void DoDrawSpline(wxList *points);
#endif

//-----------------------------------------------------------------------------
// Other virtuals from wxDCBase that are overridden here
//-----------------------------------------------------------------------------

    virtual void SetLogicalScale( double x, double y );

//-----------------------------------------------------------------------------
// Our own new stuff
//-----------------------------------------------------------------------------
    virtual void SetInternalDeviceOrigin( long x, long y );
    virtual void GetInternalDeviceOrigin( long *x, long *y );

    virtual void SetRop(WXHDC cdc);
    virtual void DoClipping(WXHDC cdc);
    virtual void SelectOldObjects(WXHDC dc);

    wxWindow *GetWindow() const { return m_canvas; }
    void SetWindow(wxWindow *win) { m_canvas = win; }

    WXHDC GetHDC() const { return m_hDC; }
    void SetHDC(WXHDC dc, bool bOwnsDC = FALSE)
    {
        m_hDC = dc;
        m_bOwnsDC = bOwnsDC;
    }

private:

#if WXWIN_COMPATIBILITY
    // function hiding warning supression
    virtual void   GetTextExtent( const wxString& string
                                 ,long*           width
                                 ,long*           height
                                 ,long*           descent = NULL
                                 ,long*           externalLeading = NULL
                                 ,wxFont*         theFont = NULL
                                 ,bool            use16 = FALSE
                                ) const
    { wxDCBase::GetTextExtent(string, width, height, descent, externalLeading, theFont, use16); };
#endif

protected:

    // ------------------------------------------------------------------------
    // Some additional data we need
    // ------------------------------------------------------------------------

    // Owner canvas and selected in bitmap (if bitmap is GDI object selected)
    wxWindow*         m_canvas;
    wxBitmap          m_selectedBitmap;

    // our HDC and its usage count: we only free it when the usage count drops
    // to 0
    WXHDC             m_hDC;
    int               m_hDCCount;
    bool              m_bOwnsDC:1;

    // Store all old GDI objects when do a SelectObject, so we can select them
    // back in (this unselecting user's objects) so we can safely delete the
    // DC.
    WXHBITMAP         m_oldBitmap;
    WXHPEN            m_oldPen;
    WXHBRUSH          m_oldBrush;
    WXHFONT           m_oldFont;
    WXHPALETTE        m_oldPalette;
};

#endif
    // _WX_DC_H_
