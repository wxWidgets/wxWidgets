/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/dc.h
// Purpose:     wxDC class
// Author:      Julian Smart
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_DC_H_
#define _WX_MSW_DC_H_

#include "wx/defs.h"
#include "wx/dc.h"

// ---------------------------------------------------------------------------
// macros
// ---------------------------------------------------------------------------

#if wxUSE_DC_CACHEING
/*
 * Cached blitting, maintaining a cache
 * of bitmaps required for transparent blitting
 * instead of constant creation/deletion
 */

class wxDCCacheEntry: public wxObject
{
public:
    wxDCCacheEntry(WXHBITMAP hBitmap, int w, int h, int depth);
    wxDCCacheEntry(WXHDC hDC, int depth);
    virtual ~wxDCCacheEntry();

    WXHBITMAP   m_bitmap;
    WXHDC       m_dc;
    int         m_width;
    int         m_height;
    int         m_depth;
};
#endif

// this is an ABC: use one of the derived classes to create a DC associated
// with a window, screen, printer and so on
class WXDLLIMPEXP_CORE wxMSWDCImpl: public wxDCImpl
{
public:
    wxMSWDCImpl(wxDC *owner, WXHDC hDC);
    virtual ~wxMSWDCImpl();

    // implement base class pure virtuals
    // ----------------------------------

    virtual void Clear() override;

    virtual bool StartDoc(const wxString& message) override;
    virtual void EndDoc() override;

    virtual void StartPage() override;
    virtual void EndPage() override;

    virtual void SetFont(const wxFont& font) override;
    virtual void SetPen(const wxPen& pen) override;
    virtual void SetBrush(const wxBrush& brush) override;
    virtual void SetBackground(const wxBrush& brush) override;
    virtual void SetBackgroundMode(int mode) override;
#if wxUSE_PALETTE
    virtual void SetPalette(const wxPalette& palette) override;
#endif // wxUSE_PALETTE

    virtual void DestroyClippingRegion() override;

    virtual wxCoord GetCharHeight() const override;
    virtual wxCoord GetCharWidth() const override;

    virtual bool CanDrawBitmap() const override;
    virtual bool CanGetTextExtent() const override;
    virtual int GetDepth() const override;
    virtual wxSize GetPPI() const override;

    virtual void SetMapMode(wxMappingMode mode) override;
    virtual void SetUserScale(double x, double y) override;
    virtual void SetLogicalScale(double x, double y) override;
    virtual void SetLogicalOrigin(wxCoord x, wxCoord y) override;
    virtual void SetDeviceOrigin(wxCoord x, wxCoord y) override;
    virtual void SetAxisOrientation(bool xLeftRight, bool yBottomUp) override;

    virtual wxPoint DeviceToLogical(wxCoord x, wxCoord y) const override;
    virtual wxPoint LogicalToDevice(wxCoord x, wxCoord y) const override;
    virtual wxSize DeviceToLogicalRel(int x, int y) const override;
    virtual wxSize LogicalToDeviceRel(int x, int y) const override;

#if wxUSE_DC_TRANSFORM_MATRIX
    virtual bool CanUseTransformMatrix() const override;
    virtual bool SetTransformMatrix(const wxAffineMatrix2D& matrix) override;
    virtual wxAffineMatrix2D GetTransformMatrix() const override;
    virtual void ResetTransformMatrix() override;
#endif // wxUSE_DC_TRANSFORM_MATRIX

    virtual void SetLogicalFunction(wxRasterOperationMode function) override;

    // implementation from now on
    // --------------------------

    virtual void SetRop(WXHDC cdc);
    virtual void SelectOldObjects(WXHDC dc);

    void SetWindow(wxWindow *win)
    {
        m_window = win;

#if wxUSE_PALETTE
        // if we have palettes use the correct one for this window
        InitializePalette();
#endif // wxUSE_PALETTE
    }

    WXHDC GetHDC() const { return m_hDC; }
    void SetHDC(WXHDC dc, bool bOwnsDC = false)
    {
        m_hDC = dc;
        m_bOwnsDC = bOwnsDC;

        // we might have a pre existing clipping region, make sure that we
        // return it if asked -- but avoid calling ::GetClipBox() right now as
        // it could be unnecessary wasteful
        m_clipping = true;
        m_isClipBoxValid = false;
    }

    void* GetHandle() const override { return (void*)GetHDC(); }

    const wxBitmap& GetSelectedBitmap() const override { return m_selectedBitmap; }
    wxBitmap& GetSelectedBitmap() override { return m_selectedBitmap; }

    // update the internal clip box variables
    void UpdateClipBox();

#if wxUSE_DC_CACHEING
    static wxDCCacheEntry* FindBitmapInCache(WXHDC hDC, int w, int h);
    static wxDCCacheEntry* FindDCInCache(wxDCCacheEntry* notThis, WXHDC hDC);

    static void AddToBitmapCache(wxDCCacheEntry* entry);
    static void AddToDCCache(wxDCCacheEntry* entry);
    static void ClearCache();
#endif

    // RTL related functions
    // ---------------------

    // get or change the layout direction (LTR or RTL) for this dc,
    // wxLayout_Default is returned if layout direction is not supported
    virtual wxLayoutDirection GetLayoutDirection() const override;
    virtual void SetLayoutDirection(wxLayoutDirection dir) override;

protected:
    void Init()
    {
        m_bOwnsDC = false;
        m_hDC = nullptr;

        m_oldBitmap = nullptr;
        m_oldPen = nullptr;
        m_oldBrush = nullptr;
        m_oldFont = nullptr;

#if wxUSE_PALETTE
        m_oldPalette = nullptr;
#endif // wxUSE_PALETTE
        m_isClipBoxValid = false;
    }

    // Unlike the public SetWindow(), this one doesn't call InitializePalette().
    void InitWindow(wxWindow* window);

    // create an uninitialized DC: this should be only used by the derived
    // classes
    wxMSWDCImpl( wxDC *owner ) : wxDCImpl( owner ) { Init(); }

    void RealizeScaleAndOrigin();

public:
    virtual void DoGetFontMetrics(int *height,
                                  int *ascent,
                                  int *descent,
                                  int *internalLeading,
                                  int *externalLeading,
                                  int *averageWidth) const override;
    virtual void DoGetTextExtent(const wxString& string,
                                 wxCoord *x, wxCoord *y,
                                 wxCoord *descent = nullptr,
                                 wxCoord *externalLeading = nullptr,
                                 const wxFont *theFont = nullptr) const override;
    virtual bool DoGetPartialTextExtents(const wxString& text, wxArrayInt& widths) const override;

    virtual bool DoFloodFill(wxCoord x, wxCoord y, const wxColour& col,
                             wxFloodFillStyle style = wxFLOOD_SURFACE) override;

    virtual void DoGradientFillLinear(const wxRect& rect,
                                      const wxColour& initialColour,
                                      const wxColour& destColour,
                                      wxDirection nDirection = wxEAST) override;

    virtual bool DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const override;

    virtual void DoDrawPoint(wxCoord x, wxCoord y) override;
    virtual void DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2) override;

    virtual void DoDrawArc(wxCoord x1, wxCoord y1,
                           wxCoord x2, wxCoord y2,
                           wxCoord xc, wxCoord yc) override;
    virtual void DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                                   double sa, double ea) override;

    virtual void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height) override;
    virtual void DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                        wxCoord width, wxCoord height,
                                        double radius) override;
    virtual void DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height) override;

#if wxUSE_SPLINES
    virtual void DoDrawSpline(const wxPointList *points) override;
#endif

    virtual void DoCrossHair(wxCoord x, wxCoord y) override;

    virtual void DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y) override;
    virtual void DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y,
                              bool useMask = false) override;

    virtual void DoDrawText(const wxString& text, wxCoord x, wxCoord y) override;
    virtual void DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y,
                                   double angle) override;

    virtual bool DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
                        wxDC *source, wxCoord xsrc, wxCoord ysrc,
                        wxRasterOperationMode rop = wxCOPY, bool useMask = false,
                        wxCoord xsrcMask = wxDefaultCoord, wxCoord ysrcMask = wxDefaultCoord) override;

    virtual bool DoStretchBlit(wxCoord xdest, wxCoord ydest,
                               wxCoord dstWidth, wxCoord dstHeight,
                               wxDC *source,
                               wxCoord xsrc, wxCoord ysrc,
                               wxCoord srcWidth, wxCoord srcHeight,
                               wxRasterOperationMode rop = wxCOPY, bool useMask = false,
                               wxCoord xsrcMask = wxDefaultCoord, wxCoord ysrcMask = wxDefaultCoord) override;

    virtual void DoSetClippingRegion(wxCoord x, wxCoord y,
                                     wxCoord width, wxCoord height) override;
    virtual void DoSetDeviceClippingRegion(const wxRegion& region) override;
    virtual bool DoGetClippingRect(wxRect& rect) const override;

    virtual void DoGetSizeMM(int* width, int* height) const override;

    virtual void DoDrawLines(int n, const wxPoint points[],
                             wxCoord xoffset, wxCoord yoffset) override;
    virtual void DoDrawPolygon(int n, const wxPoint points[],
                               wxCoord xoffset, wxCoord yoffset,
                               wxPolygonFillMode fillStyle = wxODDEVEN_RULE) override;
    virtual void DoDrawPolyPolygon(int n, const int count[], const wxPoint points[],
                                   wxCoord xoffset, wxCoord yoffset,
                                   wxPolygonFillMode fillStyle = wxODDEVEN_RULE) override;
    virtual wxBitmap DoGetAsBitmap(const wxRect *subrect) const override
    {
        return subrect == nullptr ? GetSelectedBitmap()
                               : GetSelectedBitmap().GetSubBitmap(*subrect);
    }


#if wxUSE_PALETTE
    // MSW specific, select a logical palette into the HDC
    // (tell windows to translate pixel from other palettes to our custom one
    // and vice versa)
    // Realize tells it to also reset the system palette to this one.
    void DoSelectPalette(bool realize = false);

    // Find out what palette our parent window has, then select it into the dc
    void InitializePalette();
#endif // wxUSE_PALETTE

protected:
    // common part of DoDrawText() and DoDrawRotatedText()
    void DrawAnyText(const wxString& text, wxCoord x, wxCoord y);

    // common part of DoSetClippingRegion() and DoSetDeviceClippingRegion()
    void SetClippingHrgn(WXHRGN hrgn, bool doRtlOffset = false);

    // implementation of DoGetSize() for wxScreen/PrinterDC: this simply
    // returns the size of the entire device this DC is associated with
    //
    // notice that we intentionally put it in a separate function instead of
    // DoGetSize() itself because we want it to remain pure virtual both
    // because each derived class should take care to define it as needed (this
    // implementation is not at all always appropriate) and because we want
    // wxDC to be an ABC to prevent it from being created directly
    void GetDeviceSize(int *width, int *height) const;


    // MSW-specific member variables
    // -----------------------------

    wxBitmap          m_selectedBitmap;

    // TRUE => DeleteDC() in dtor, FALSE => only ReleaseDC() it
    bool              m_bOwnsDC:1;

    // our HDC
    WXHDC             m_hDC;

    // Store all old GDI objects when do a SelectObject, so we can select them
    // back in (this unselecting user's objects) so we can safely delete the
    // DC.
    WXHBITMAP         m_oldBitmap;
    WXHPEN            m_oldPen;
    WXHBRUSH          m_oldBrush;
    WXHFONT           m_oldFont;

#if wxUSE_PALETTE
    WXHPALETTE        m_oldPalette;
#endif // wxUSE_PALETTE

#if wxUSE_DC_CACHEING
    static wxObjectList     sm_bitmapCache;
    static wxObjectList     sm_dcCache;
#endif

    bool m_isClipBoxValid;

    wxDECLARE_CLASS(wxMSWDCImpl);
    wxDECLARE_NO_COPY_CLASS(wxMSWDCImpl);
};

// ----------------------------------------------------------------------------
// wxDCTemp: a wxDC which doesn't free the given HDC (used by wxWidgets
// only/mainly)
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxDCTempImpl : public wxMSWDCImpl
{
public:
    // construct a temporary DC with the specified HDC and size (it should be
    // specified whenever we know it for this HDC)
    wxDCTempImpl(wxDC *owner, WXHDC hdc, const wxSize& size )
        : wxMSWDCImpl( owner, hdc ),
          m_size(size)
    {
    }

    virtual ~wxDCTempImpl()
    {
        // prevent base class dtor from freeing it
        SetHDC((WXHDC)nullptr);
    }

    virtual void DoGetSize(int *w, int *h) const override
    {
        wxASSERT_MSG( m_size.IsFullySpecified(),
                      wxT("size of this DC hadn't been set and is unknown") );

        if ( w )
            *w = m_size.x;
        if ( h )
            *h = m_size.y;
    }

private:
    // size of this DC must be explicitly set by SetSize() as we have no way to
    // find it ourselves
    const wxSize m_size;

    wxDECLARE_NO_COPY_CLASS(wxDCTempImpl);
};

class WXDLLIMPEXP_CORE wxDCTemp : public wxDC
{
public:
    wxDCTemp(WXHDC hdc, const wxSize& size = wxDefaultSize)
        : wxDC(new wxDCTempImpl(this, hdc, size))
    {
    }
};

#endif // _WX_MSW_DC_H_

