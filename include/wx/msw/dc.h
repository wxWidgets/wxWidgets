/////////////////////////////////////////////////////////////////////////////
// Name:        dc.h
// Purpose:     wxDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DC_H_
#define _WX_DC_H_

#ifdef __GNUG__
    #pragma interface "dc.h"
#endif

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

    virtual void Clear();

    virtual bool StartDoc(const wxString& message);
    virtual void EndDoc();

    virtual void StartPage();
    virtual void EndPage();

    virtual void SetFont(const wxFont& font);
    virtual void SetPen(const wxPen& pen);
    virtual void SetBrush(const wxBrush& brush);
    virtual void SetBackground(const wxBrush& brush);
    virtual void SetBackgroundMode(int mode);
    virtual void SetPalette(const wxPalette& palette);

    virtual void DestroyClippingRegion();

    virtual long GetCharHeight() const;
    virtual long GetCharWidth() const;
    virtual void GetTextExtent(const wxString& string,
                               long *x, long *y,
                               long *descent = NULL,
                               long *externalLeading = NULL,
                               wxFont *theFont = NULL) const;

    virtual bool CanDrawBitmap() const;
    virtual bool CanGetTextExtent() const;
    virtual int GetDepth() const;
    virtual wxSize GetPPI() const;

    virtual void SetMapMode(int mode);
    virtual void SetUserScale(double x, double y);
    virtual void SetSystemScale(double x, double y);
    virtual void SetLogicalScale(double x, double y);
    virtual void SetLogicalOrigin(long x, long y);
    virtual void SetDeviceOrigin(long x, long y);
    virtual void SetAxisOrientation(bool xLeftRight, bool yBottomUp);
    virtual void SetLogicalFunction(int function);

    // implementation from now on
    // --------------------------

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

protected:
    virtual void DoFloodFill(long x, long y, const wxColour& col,
                             int style = wxFLOOD_SURFACE);

    virtual bool DoGetPixel(long x, long y, wxColour *col) const;

    virtual void DoDrawPoint(long x, long y);
    virtual void DoDrawLine(long x1, long y1, long x2, long y2);

    virtual void DoDrawArc(long x1, long y1,
                           long x2, long y2,
                           long xc, long yc);
    virtual void DoDrawEllipticArc(long x, long y, long w, long h,
                                   double sa, double ea);

    virtual void DoDrawRectangle(long x, long y, long width, long height);
    virtual void DoDrawRoundedRectangle(long x, long y,
                                        long width, long height,
                                        double radius);
    virtual void DoDrawEllipse(long x, long y, long width, long height);

    virtual void DoCrossHair(long x, long y);

    virtual void DoDrawIcon(const wxIcon& icon, long x, long y);
    virtual void DoDrawBitmap(const wxBitmap &bmp, long x, long y,
                              bool useMask = FALSE);

    virtual void DoDrawText(const wxString& text, long x, long y);

    virtual bool DoBlit(long xdest, long ydest, long width, long height,
                        wxDC *source, long xsrc, long ysrc,
                        int rop = wxCOPY, bool useMask = FALSE);

    // this is gnarly - we can't even call this function DoSetClippingRegion()
    // because of virtual function hiding
    virtual void DoSetClippingRegionAsRegion(const wxRegion& region);
    virtual void DoSetClippingRegion(long x, long y,
                                     long width, long height);
    virtual void DoGetClippingRegion(long *x, long *y,
                                     long *width, long *height)
    {
        GetClippingBox(x, y, width, height);
    }

    virtual void DoGetSize(int *width, int *height) const;
    virtual void DoGetSizeMM(int* width, int* height) const;

    virtual void DoDrawLines(int n, wxPoint points[],
                             long xoffset, long yoffset);
    virtual void DoDrawPolygon(int n, wxPoint points[],
                               long xoffset, long yoffset,
                               int fillStyle = wxODDEVEN_RULE);

#if wxUSE_SPLINES
    virtual void DoDrawSpline(wxList *points);
#endif // wxUSE_SPLINES

    // MSW-specific member variables
    int               m_windowExtX;
    int               m_windowExtY;

    // the window associated with this DC (may be NULL)
    wxWindow         *m_canvas;

    wxBitmap          m_selectedBitmap;

    // TRUE => DeleteDC() in dtor, FALSE => only ReleaseDC() it
    bool              m_bOwnsDC:1;

    // our HDC and its usage count: we only free it when the usage count drops
    // to 0
    WXHDC             m_hDC;
    int               m_hDCCount;

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
