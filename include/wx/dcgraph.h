/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dcgraph.h
// Purpose:     graphics context device bridge header
// Author:      Stefan Csomor
// Modified by:
// Created:
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GRAPHICS_DC_H_
#define _WX_GRAPHICS_DC_H_

#if wxUSE_GRAPHICS_CONTEXT

#include "wx/dc.h"
#include "wx/geometry.h"
#include "wx/graphics.h"

class WXDLLIMPEXP_FWD_CORE wxWindowDC;


class WXDLLIMPEXP_CORE wxGCDC: public wxDC
{
public:
    wxGCDC( const wxWindowDC& dc );
    wxGCDC( const wxMemoryDC& dc );
#if wxUSE_PRINTING_ARCHITECTURE
    wxGCDC( const wxPrinterDC& dc );
#endif
#if defined(__WXMSW__) && wxUSE_ENH_METAFILE
    wxGCDC( const wxEnhMetaFileDC& dc );
#endif
    wxGCDC(wxGraphicsContext* context);

    wxGCDC();
    virtual ~wxGCDC();

#ifdef __WXMSW__
    // override wxDC virtual functions to provide access to HDC associated with
    // underlying wxGraphicsContext
    virtual WXHDC AcquireHDC() override;
    virtual void ReleaseHDC(WXHDC hdc) override;
#endif // __WXMSW__

private:
    wxDECLARE_DYNAMIC_CLASS(wxGCDC);
    wxDECLARE_NO_COPY_CLASS(wxGCDC);
};


class WXDLLIMPEXP_CORE wxGCDCImpl: public wxDCImpl
{
public:
    wxGCDCImpl( wxDC *owner, const wxWindowDC& dc );
    wxGCDCImpl( wxDC *owner, const wxMemoryDC& dc );
#if wxUSE_PRINTING_ARCHITECTURE
    wxGCDCImpl( wxDC *owner, const wxPrinterDC& dc );
#endif
#if defined(__WXMSW__) && wxUSE_ENH_METAFILE
    wxGCDCImpl( wxDC *owner, const wxEnhMetaFileDC& dc );
#endif

    // Ctor using an existing graphics context given to wxGCDC ctor.
    wxGCDCImpl(wxDC *owner, wxGraphicsContext* context);

    wxGCDCImpl( wxDC *owner );

    virtual ~wxGCDCImpl();

    // implement base class pure virtuals
    // ----------------------------------

    virtual void Clear() override;

    virtual bool StartDoc( const wxString& message ) override;
    virtual void EndDoc() override;

    virtual void StartPage() override;
    virtual void EndPage() override;

    // flushing the content of this dc immediately onto screen
    virtual void Flush() override;

    virtual void SetFont(const wxFont& font) override;
    virtual void SetPen(const wxPen& pen) override;
    virtual void SetBrush(const wxBrush& brush) override;
    virtual void SetBackground(const wxBrush& brush) override;
    virtual void SetBackgroundMode(int mode) override;

#if wxUSE_PALETTE
    virtual void SetPalette(const wxPalette& palette) override;
#endif

    virtual void DestroyClippingRegion() override;

    virtual wxCoord GetCharHeight() const override;
    virtual wxCoord GetCharWidth() const override;

    virtual bool CanDrawBitmap() const override;
    virtual bool CanGetTextExtent() const override;
    virtual int GetDepth() const override;
    virtual wxSize GetPPI() const override;

    virtual void SetLogicalFunction(wxRasterOperationMode function) override;

    virtual void SetTextForeground(const wxColour& colour) override;
    virtual void SetTextBackground(const wxColour& colour) override;

    virtual void ComputeScaleAndOrigin() override;

    wxGraphicsContext* GetGraphicsContext() const override { return m_graphicContext; }
    virtual void SetGraphicsContext( wxGraphicsContext* ctx ) override;

    virtual void* GetHandle() const override;

#if wxUSE_DC_TRANSFORM_MATRIX
    virtual bool CanUseTransformMatrix() const override;
    virtual bool SetTransformMatrix(const wxAffineMatrix2D& matrix) override;
    virtual wxAffineMatrix2D GetTransformMatrix() const override;
    virtual void ResetTransformMatrix() override;
#endif // wxUSE_DC_TRANSFORM_MATRIX

    // coordinates conversions and transforms
    virtual wxPoint DeviceToLogical(wxCoord x, wxCoord y) const override;
    virtual wxPoint LogicalToDevice(wxCoord x, wxCoord y) const override;
    virtual wxSize DeviceToLogicalRel(int x, int y) const override;
    virtual wxSize LogicalToDeviceRel(int x, int y) const override;

    // the true implementations
    virtual bool DoFloodFill(wxCoord x, wxCoord y, const wxColour& col,
                             wxFloodFillStyle style = wxFLOOD_SURFACE) override;

    virtual void DoGradientFillLinear(const wxRect& rect,
        const wxColour& initialColour,
        const wxColour& destColour,
        wxDirection nDirection = wxEAST) override;

    virtual void DoGradientFillConcentric(const wxRect& rect,
        const wxColour& initialColour,
        const wxColour& destColour,
        const wxPoint& circleCenter) override;

    virtual bool DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const override;

    virtual void DoDrawPoint(wxCoord x, wxCoord y) override;

#if wxUSE_SPLINES
    virtual void DoDrawSpline(const wxPointList *points) override;
#endif

    virtual void DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2) override;

    virtual void DoDrawArc(wxCoord x1, wxCoord y1,
        wxCoord x2, wxCoord y2,
        wxCoord xc, wxCoord yc) override;

    virtual void DoDrawCheckMark(wxCoord x, wxCoord y,
        wxCoord width, wxCoord height) override;

    virtual void DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
        double sa, double ea) override;

    virtual void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height) override;
    virtual void DoDrawRoundedRectangle(wxCoord x, wxCoord y,
        wxCoord width, wxCoord height,
        double radius) override;
    virtual void DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height) override;

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
                        wxCoord xsrcMask = -1, wxCoord ysrcMask = -1) override;

    virtual bool DoStretchBlit(wxCoord xdest, wxCoord ydest,
                               wxCoord dstWidth, wxCoord dstHeight,
                               wxDC *source,
                               wxCoord xsrc, wxCoord ysrc,
                               wxCoord srcWidth, wxCoord srcHeight,
                               wxRasterOperationMode = wxCOPY, bool useMask = false,
                               wxCoord xsrcMask = wxDefaultCoord, wxCoord ysrcMask = wxDefaultCoord) override;

    virtual void DoGetSize(int *,int *) const override;
    virtual void DoGetSizeMM(int* width, int* height) const override;

    virtual void DoDrawLines(int n, const wxPoint points[],
        wxCoord xoffset, wxCoord yoffset) override;
    virtual void DoDrawPolygon(int n, const wxPoint points[],
                               wxCoord xoffset, wxCoord yoffset,
                               wxPolygonFillMode fillStyle = wxODDEVEN_RULE) override;
    virtual void DoDrawPolyPolygon(int n, const int count[], const wxPoint points[],
                                   wxCoord xoffset, wxCoord yoffset,
                                   wxPolygonFillMode fillStyle) override;

    virtual void DoSetDeviceClippingRegion(const wxRegion& region) override;
    virtual void DoSetClippingRegion(wxCoord x, wxCoord y,
        wxCoord width, wxCoord height) override;
    virtual bool DoGetClippingRect(wxRect& rect) const override;

    virtual void DoGetTextExtent(const wxString& string,
        wxCoord *x, wxCoord *y,
        wxCoord *descent = nullptr,
        wxCoord *externalLeading = nullptr,
        const wxFont *theFont = nullptr) const override;

    virtual bool DoGetPartialTextExtents(const wxString& text, wxArrayInt& widths) const override;

#ifdef __WXMSW__
    virtual wxRect MSWApplyGDIPlusTransform(const wxRect& r) const override;
#endif // __WXMSW__

    // update the internal clip box variables
    void UpdateClipBox();

protected:
    // unused int parameter distinguishes this version, which does not create a
    // wxGraphicsContext, in the expectation that the derived class will do it
    wxGCDCImpl(wxDC* owner, int);

#ifdef __WXOSX__
#if WXWIN_COMPATIBILITY_3_2
    wxDEPRECATED_MSG("Don't use OSXGetOrigin()")
    virtual wxPoint OSXGetOrigin() const { return wxPoint(); }
#endif // WXWIN_COMPATIBILITY_3_2
#endif

    // scaling variables
    bool m_logicalFunctionSupported;
    wxGraphicsMatrix m_matrixOriginal;
    wxGraphicsMatrix m_matrixCurrent;
    wxGraphicsMatrix m_matrixCurrentInv;
#if wxUSE_DC_TRANSFORM_MATRIX
    wxAffineMatrix2D m_matrixExtTransform;
#endif // wxUSE_DC_TRANSFORM_MATRIX

    wxGraphicsContext* m_graphicContext;

    bool m_isClipBoxValid;

private:
    // This method only initializes trivial fields.
    void CommonInit();

    // This method initializes all fields (including those initialized by
    // CommonInit() as it calls it) and the given context, if non-null, which
    // is assumed to be newly created.
    void Init(wxGraphicsContext*);

    // This method initializes m_graphicContext, m_ok and m_matrixOriginal
    // fields, returns true if the context was valid.
    bool DoInitContext(wxGraphicsContext* ctx);

    // Another convenient wrapper for CalcBoundingBox().
    // This is not an overload in order to avoid hiding the base class ones.
    void CalcBoundingBoxForBox(const wxRect2DDouble& box)
    {
        CalcBoundingBox(wxRound(box.m_x), wxRound(box.m_y));
        CalcBoundingBox(wxRound(box.m_x + box.m_width),
                        wxRound(box.m_y + box.m_height));
    }

    wxDECLARE_CLASS(wxGCDCImpl);
    wxDECLARE_NO_COPY_CLASS(wxGCDCImpl);
};

#endif // wxUSE_GRAPHICS_CONTEXT
#endif // _WX_GRAPHICS_DC_H_
