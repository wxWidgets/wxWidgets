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

    virtual wxCoord GetCharHeight() const;
    virtual wxCoord GetCharWidth() const;
    virtual void DoGetTextExtent(const wxString& string,
                                 wxCoord *x, wxCoord *y,
                                 wxCoord *descent = NULL,
                                 wxCoord *externalLeading = NULL,
                                 wxFont *theFont = NULL) const;

    virtual bool CanDrawBitmap() const;
    virtual bool CanGetTextExtent() const;
    virtual int GetDepth() const;
    virtual wxSize GetPPI() const;

    virtual void SetMapMode(int mode);
    virtual void SetUserScale(double x, double y);
    virtual void SetSystemScale(double x, double y);
    virtual void SetLogicalScale(double x, double y);
    virtual void SetLogicalOrigin(wxCoord x, wxCoord y);
    virtual void SetDeviceOrigin(wxCoord x, wxCoord y);
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

    const wxBitmap& GetSelectedBitmap() const { return m_selectedBitmap; }
    wxBitmap& GetSelectedBitmap() { return m_selectedBitmap; }

protected:
    virtual void DoFloodFill(wxCoord x, wxCoord y, const wxColour& col,
                             int style = wxFLOOD_SURFACE);

    virtual bool DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const;

    virtual void DoDrawPoint(wxCoord x, wxCoord y);
    virtual void DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2);

    virtual void DoDrawArc(wxCoord x1, wxCoord y1,
                           wxCoord x2, wxCoord y2,
                           wxCoord xc, wxCoord yc);
    virtual void DoDrawCheckMark(wxCoord x, wxCoord y,
                                 wxCoord width, wxCoord height);
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
                              bool useMask = FALSE);

    virtual void DoDrawText(const wxString& text, wxCoord x, wxCoord y);
    virtual void DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y,
                                   double angle);

    virtual bool DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
                        wxDC *source, wxCoord xsrc, wxCoord ysrc,
                        int rop = wxCOPY, bool useMask = FALSE);

    // this is gnarly - we can't even call this function DoSetClippingRegion()
    // because of virtual function hiding
    virtual void DoSetClippingRegionAsRegion(const wxRegion& region);
    virtual void DoSetClippingRegion(wxCoord x, wxCoord y,
                                     wxCoord width, wxCoord height);
    virtual void DoGetClippingRegion(wxCoord *x, wxCoord *y,
                                     wxCoord *width, wxCoord *height)
    {
        GetClippingBox(x, y, width, height);
    }

    virtual void DoGetSize(int *width, int *height) const;
    virtual void DoGetSizeMM(int* width, int* height) const;

    virtual void DoDrawLines(int n, wxPoint points[],
                             wxCoord xoffset, wxCoord yoffset);
    virtual void DoDrawPolygon(int n, wxPoint points[],
                               wxCoord xoffset, wxCoord yoffset,
                               int fillStyle = wxODDEVEN_RULE);

#if wxUSE_SPLINES
    virtual void DoDrawSpline(wxList *points);
#endif // wxUSE_SPLINES

    // common part of DoDrawText() and DoDrawRotatedText()
    void DrawAnyText(const wxString& text, wxCoord x, wxCoord y);

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
