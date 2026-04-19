/////////////////////////////////////////////////////////////////////////////
// Name:        wx/svggc.h
// Purpose:     wxGraphicsContext/Renderer backed by wxSVGFileDC
// Author:      Blake Madden
// Copyright:   (c) Blake Madden
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SVGGC_H_
#define _WX_SVGGC_H_

#include "wx/defs.h"

#if wxUSE_SVG && wxUSE_GRAPHICS_CONTEXT

#include "wx/graphics.h"
#include "wx/dcsvg.h"
#include "wx/private/graphics.h"

#include <stack>
#include <vector>

class wxSVGFileDCImpl;

// Represents a single command in an SVG path (Move, Line, etc.).
// We store these instead of a raw string so that we can apply
// transformations to the path after it has been recorded.
struct wxSVGPathSegment
{
    enum Type { MoveSegment, LineSegment, CurveSegment, ArcSegment, CloseSegment, QuadCurveSegment } type;
    wxDouble x, y;
    wxDouble x1, y1, x2, y2;
    wxDouble r;
    wxDouble startAngle, endAngle;
    bool clockwise = true;

    static wxSVGPathSegment MoveTo(wxDouble x, wxDouble y);
    static wxSVGPathSegment LineTo(wxDouble x, wxDouble y);
    static wxSVGPathSegment CurveTo(wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2, wxDouble x, wxDouble y);
    static wxSVGPathSegment QuadCurveTo(wxDouble cx, wxDouble cy, wxDouble x, wxDouble y);
    static wxSVGPathSegment Arc(wxDouble x, wxDouble y, wxDouble r, wxDouble startAngle, wxDouble endAngle, bool clockwise);
    static wxSVGPathSegment Close();
};

// ----------------------------------------------------------------------------
// wxSVGGraphicsMatrixData - standard 2D affine transformation matrix
// ----------------------------------------------------------------------------
class wxSVGGraphicsMatrixData : public wxGraphicsMatrixData
{
public:
    wxSVGGraphicsMatrixData(wxGraphicsRenderer* renderer,
                            wxDouble a = 1.0, wxDouble b = 0.0,
                            wxDouble c = 0.0, wxDouble d = 1.0,
                            wxDouble tx = 0.0, wxDouble ty = 0.0);

    wxGraphicsObjectRefData* Clone() const override;

    void Concat(const wxGraphicsMatrixData* t) override;

    void Set(wxDouble a = 1.0, wxDouble b = 0.0, wxDouble c = 0.0, wxDouble d = 1.0,
             wxDouble tx = 0.0, wxDouble ty = 0.0) override;

    void Get(wxDouble* a = nullptr, wxDouble* b = nullptr, wxDouble* c = nullptr,
             wxDouble* d = nullptr, wxDouble* tx = nullptr, wxDouble* ty = nullptr) const override;

    void Invert() override;

    bool IsEqual(const wxGraphicsMatrixData* t) const override;

    bool IsIdentity() const override;

    void Translate(wxDouble dx, wxDouble dy) override;

    void Scale(wxDouble xScale, wxDouble yScale) override;

    void Rotate(wxDouble angle) override;

    void TransformPoint(wxDouble* x, wxDouble* y) const override;

    void TransformDistance(wxDouble* dx, wxDouble* dy) const override;

    void* GetNativeMatrix() const override { return nullptr; }

    wxDouble m_a, m_b, m_c, m_d, m_tx, m_ty;
};

// ----------------------------------------------------------------------------
// wxSVGGraphicsPathData - records path commands for deferred SVG generation
// ----------------------------------------------------------------------------
class wxSVGGraphicsPathData : public wxGraphicsPathData
{
public:
    explicit wxSVGGraphicsPathData(wxGraphicsRenderer* renderer);

    wxGraphicsObjectRefData* Clone() const override;

    void MoveToPoint(wxDouble x, wxDouble y) override;

    void AddLineToPoint(wxDouble x, wxDouble y) override;

    void AddCurveToPoint(wxDouble cx1, wxDouble cy1,
                         wxDouble cx2, wxDouble cy2,
                         wxDouble x, wxDouble y) override;

    void AddQuadCurveToPoint(wxDouble cx, wxDouble cy,
                             wxDouble x, wxDouble y) override;

    void AddRectangle(wxDouble x, wxDouble y, wxDouble w, wxDouble h) override;

    void AddCircle(wxDouble x, wxDouble y, wxDouble r) override;

    void AddArcToPoint(wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2, wxDouble r) override;

    void AddEllipse(wxDouble x, wxDouble y, wxDouble w, wxDouble h) override;

    void AddRoundedRectangle(wxDouble x, wxDouble y, wxDouble w, wxDouble h, wxDouble radius) override;

    void AddPath(const wxGraphicsPathData* path) override;

    void CloseSubpath() override;

    void GetCurrentPoint(wxDouble* x, wxDouble* y) const override;

    void AddArc(wxDouble x, wxDouble y, wxDouble r,
                wxDouble startAngle, wxDouble endAngle, bool clockwise) override;

    void* GetNativePath() const override;

    void UnGetNativePath(void*) const override {}

    void Transform(const wxGraphicsMatrixData* matrix) override;

    void GetBox(wxDouble* x, wxDouble* y, wxDouble* w, wxDouble* h) const override;

    bool Contains(wxDouble x, wxDouble y,
                  wxPolygonFillMode fillStyle = wxODDEVEN_RULE) const override;

    // Generates the SVG "d" attribute string from recorded segments.
    wxString GetDString() const;

private:
    void Extend(wxDouble x, wxDouble y);

    // Approximates a circular arc with a series of cubic Beziers.
    void AddArcToCubics(wxDouble xc, wxDouble yc, wxDouble r,
                        wxDouble startAngle, wxDouble endAngle, bool clockwise);

    std::vector<wxSVGPathSegment> m_segments;
    wxString m_d;
    wxDouble m_curX, m_curY;
    bool m_hasPoints;
    wxDouble m_minX, m_minY, m_maxX, m_maxY;
};

// ----------------------------------------------------------------------------
// wxSVGGraphicsPenData / BrushData / FontData / BitmapData
// ----------------------------------------------------------------------------

class wxSVGGraphicsPenData : public wxGraphicsObjectRefData
{
public:
    wxSVGGraphicsPenData(wxGraphicsRenderer* renderer,
                         const wxGraphicsPenInfo& info);

    const wxPen& GetPen() const { return m_pen; }
    const wxGraphicsPenInfo& GetInfo() const { return m_info; }

private:
    wxGraphicsPenInfo m_info;
    wxPen m_pen;
};

class wxSVGGraphicsBrushData : public wxGraphicsObjectRefData
{
public:
    wxSVGGraphicsBrushData(wxGraphicsRenderer* renderer, const wxBrush& brush);
    wxSVGGraphicsBrushData(wxGraphicsRenderer* renderer,
                           wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2,
                           const wxGraphicsGradientStops& stops,
                           const wxGraphicsMatrix& matrix);
    wxSVGGraphicsBrushData(wxGraphicsRenderer* renderer,
                           wxDouble startX, wxDouble startY, wxDouble endX, wxDouble endY,
                           wxDouble radius,
                           const wxGraphicsGradientStops& stops,
                           const wxGraphicsMatrix& matrix);

    const wxBrush& GetBrush() const { return m_brush; }

    bool IsGradient() const { return m_isGradient; }
    bool IsRadial() const { return m_isRadial; }

    void GetLinearParameters(wxDouble* x1, wxDouble* y1, wxDouble* x2, wxDouble* y2) const
    {
        if ( x1 ) *x1 = m_x1; if ( y1 ) *y1 = m_y1;
        if ( x2 ) *x2 = m_x2; if ( y2 ) *y2 = m_y2;
    }
    void GetRadialParameters(wxDouble* startX, wxDouble* startY, wxDouble* endX, wxDouble* endY, wxDouble* radius) const
    {
        if ( startX ) *startX = m_x1; if ( startY ) *startY = m_y1;
        if ( endX ) *endX = m_x2; if ( endY ) *endY = m_y2;
        if ( radius ) *radius = m_radius;
    }
    const wxGraphicsGradientStops& GetStops() const { return m_stops; }
    const wxGraphicsMatrix& GetMatrix() const { return m_matrix; }

private:
    wxBrush m_brush;
    bool m_isGradient = false;
    bool m_isRadial = false;
    wxDouble m_x1 = 0, m_y1 = 0, m_x2 = 0, m_y2 = 0, m_radius = 0;
    wxGraphicsGradientStops m_stops;
    wxGraphicsMatrix m_matrix;
};

class wxSVGGraphicsFontData : public wxGraphicsObjectRefData
{
public:
    wxSVGGraphicsFontData(wxGraphicsRenderer* renderer,
                          const wxFont& font, const wxColour& colour);

    const wxFont& GetFont() const { return m_font; }
    const wxColour& GetColour() const { return m_colour; }

private:
    wxFont m_font;
    wxColour m_colour;
};

class wxSVGGraphicsBitmapData : public wxGraphicsObjectRefData
{
public:
    wxSVGGraphicsBitmapData(wxGraphicsRenderer* renderer, const wxBitmap& bmp);

    wxGraphicsObjectRefData* Clone() const override;

    const wxBitmap& GetBitmap() const { return m_bitmap; }

private:
    wxBitmap m_bitmap;
};

// Singleton renderer producing wxSVGGraphicsContext and its supporting
// handle/data objects. This renderer does not know how to create contexts
// from arbitrary DCs or windows - the SVG graphics context is only ever
// constructed indirectly via wxSVGFileDCImpl::GetGraphicsContext().
class WXDLLIMPEXP_CORE wxSVGGraphicsRenderer : public wxGraphicsRenderer
{
public:
    static wxGraphicsRenderer* Get();

    virtual wxGraphicsContext* CreateContext(const wxWindowDC& dc) override;
    virtual wxGraphicsContext* CreateContext(const wxMemoryDC& dc) override;
#if wxUSE_PRINTING_ARCHITECTURE
    virtual wxGraphicsContext* CreateContext(const wxPrinterDC& dc) override;
#endif
#ifdef __WXMSW__
#if wxUSE_ENH_METAFILE
    virtual wxGraphicsContext* CreateContext(const wxEnhMetaFileDC& dc) override;
#endif
#endif

    virtual wxGraphicsContext* CreateContextFromNativeContext(void* context) override;
    virtual wxGraphicsContext* CreateContextFromNativeWindow(void* window) override;
#ifdef __WXMSW__
    virtual wxGraphicsContext* CreateContextFromNativeHDC(WXHDC dc) override;
#endif
    virtual wxGraphicsContext* CreateContext(wxWindow* window) override;

#if wxUSE_IMAGE
    virtual wxGraphicsContext* CreateContextFromImage(wxImage& image) override;
#endif

    virtual wxGraphicsContext* CreateMeasuringContext() override;

    virtual wxGraphicsPath CreatePath() override;

    virtual wxGraphicsMatrix CreateMatrix(wxDouble a = 1.0, wxDouble b = 0.0,
                                          wxDouble c = 0.0, wxDouble d = 1.0,
                                          wxDouble tx = 0.0, wxDouble ty = 0.0) override;

    virtual wxGraphicsPen CreatePen(const wxGraphicsPenInfo& info) override;
    virtual wxGraphicsBrush CreateBrush(const wxBrush& brush) override;

    virtual wxGraphicsBrush
    CreateLinearGradientBrush(wxDouble x1, wxDouble y1,
                              wxDouble x2, wxDouble y2,
                              const wxGraphicsGradientStops& stops,
                              const wxGraphicsMatrix& matrix = wxNullGraphicsMatrix) override;

    virtual wxGraphicsBrush
    CreateRadialGradientBrush(wxDouble startX, wxDouble startY,
                              wxDouble endX, wxDouble endY,
                              wxDouble radius,
                              const wxGraphicsGradientStops& stops,
                              const wxGraphicsMatrix& matrix = wxNullGraphicsMatrix) override;

    virtual wxGraphicsFont CreateFont(const wxFont& font,
                                      const wxColour& col = *wxBLACK) override;
    virtual wxGraphicsFont CreateFont(double sizeInPixels,
                                      const wxString& facename,
                                      int flags = wxFONTFLAG_DEFAULT,
                                      const wxColour& col = *wxBLACK) override;
    virtual wxGraphicsFont CreateFontAtDPI(const wxFont& font,
                                           const wxRealPoint& dpi,
                                           const wxColour& col = *wxBLACK) override;

    virtual wxGraphicsBitmap CreateBitmap(const wxBitmap& bitmap) override;
#if wxUSE_IMAGE
    virtual wxGraphicsBitmap CreateBitmapFromImage(const wxImage& image) override;
    virtual wxImage CreateImageFromBitmap(const wxGraphicsBitmap& bmp) override;
#endif
    virtual wxGraphicsBitmap CreateBitmapFromNativeBitmap(void* bitmap) override;
    virtual wxGraphicsBitmap CreateSubBitmap(const wxGraphicsBitmap& bitmap,
                                             wxDouble x, wxDouble y,
                                             wxDouble w, wxDouble h) override;

    virtual wxString GetName() const override;
    virtual void GetVersion(int* major, int* minor = nullptr, int* micro = nullptr) const override;

private:
    wxSVGGraphicsRenderer() = default;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxSVGGraphicsRenderer);
};

// wxGraphicsContext specialisation that writes SVG drawing commands into
// the shared SVG buffer owned by wxSVGFileDCImpl. Constructed only via
// wxSVGFileDCImpl::GetGraphicsContext(); state (pen/brush/font) is mirrored
// back onto the owning DC so NewGraphicsIfNeeded() can keep the output
// well-formed regardless of whether drawing came through the DC or the GC.
class WXDLLIMPEXP_CORE wxSVGGraphicsContext : public wxGraphicsContext
{
public:
    explicit wxSVGGraphicsContext(wxSVGFileDCImpl* impl);
    virtual ~wxSVGGraphicsContext();

    // State stack
    virtual void PushState() override;
    virtual void PopState() override;

    // Clipping
    virtual void Clip(const wxRegion& region) override;
    virtual void Clip(wxDouble x, wxDouble y, wxDouble w, wxDouble h) override;
    virtual void ResetClip() override;
    virtual void GetClipBox(wxDouble* x, wxDouble* y, wxDouble* w, wxDouble* h) override;

    virtual void* GetNativeContext() override;

    virtual bool SetAntialiasMode(wxAntialiasMode antialias) override;
    virtual bool SetInterpolationQuality(wxInterpolationQuality interpolation) override;
    virtual bool SetCompositionMode(wxCompositionMode op) override;

    virtual void BeginLayer(wxDouble opacity) override;
    virtual void EndLayer() override;

    // Transforms
    virtual void Translate(wxDouble dx, wxDouble dy) override;
    virtual void Scale(wxDouble xScale, wxDouble yScale) override;
    virtual void Rotate(wxDouble angle) override;
    virtual void ConcatTransform(const wxGraphicsMatrix& matrix) override;
    virtual void SetTransform(const wxGraphicsMatrix& matrix) override;
    virtual wxGraphicsMatrix GetTransform() const override;

    // Paint state
    virtual void SetPen(const wxGraphicsPen& pen) override;
    virtual void SetBrush(const wxGraphicsBrush& brush) override;
    virtual void SetFont(const wxGraphicsFont& font) override;

    // Paths
    virtual void StrokePath(const wxGraphicsPath& path) override;
    virtual void FillPath(const wxGraphicsPath& path,
                          wxPolygonFillMode fillStyle = wxODDEVEN_RULE) override;

    // Text
    virtual void GetTextExtent(const wxString& text, wxDouble* width, wxDouble* height,
                               wxDouble* descent = nullptr,
                               wxDouble* externalLeading = nullptr) const override;
    virtual void GetPartialTextExtents(const wxString& text,
                                       wxArrayDouble& widths) const override;

    // Images
    virtual void DrawBitmap(const wxGraphicsBitmap& bmp,
                            wxDouble x, wxDouble y, wxDouble w, wxDouble h) override;
    virtual void DrawBitmap(const wxBitmap& bmp,
                            wxDouble x, wxDouble y, wxDouble w, wxDouble h) override;
    virtual void DrawIcon(const wxIcon& icon,
                          wxDouble x, wxDouble y, wxDouble w, wxDouble h) override;

#ifdef __WXMSW__
    virtual WXHDC GetNativeHDC() override;
    virtual void ReleaseNativeHDC(WXHDC hdc) override;
#endif

protected:
    virtual void DoDrawText(const wxString& str, wxDouble x, wxDouble y) override;

private:
    // Mirror a pen/brush onto the parent DC so its NewGraphicsIfNeeded()
    // machinery closes/reopens the current <g> group before the next write.
    void SyncPenToDC(const wxPen& pen);
    void SyncBrushToDC(const wxBrush& brush);

    // Mirror the current transformation matrix to the parent DC.
    // This is required because text and bitmap drawing is delegated to the
    // DC, which needs to know the current GC transform to position them
    // correctly in the SVG output.
    void SyncTransformToDC();

    // Returns the current top-of-stack matrix (or identity if empty) as an
    // SVG "transform=\"matrix(a,b,c,d,tx,ty)\"" attribute, or empty if the
    // matrix is the identity.
    wxString GetCurrentTransformAttr() const;

    // Push the path's local-coordinate bounding box through the current
    // transform and feed the four corners to the parent DC's bounding box,
    // so wxSVGFileDC::MaxX/MaxY include GC-rendered geometry.
    void AccumulatePathBounds(wxGraphicsPathData* data);

    wxSVGFileDCImpl* m_impl;
    wxPen m_currentPen;
    wxBrush m_currentBrush;
    wxFont m_currentFont;
    wxColour m_currentTextColour;

    // Running current-transform-matrix plus save-state stack.
    wxGraphicsMatrix m_transform;

    struct State
    {
        wxGraphicsMatrix transform;
        size_t clipNestingLevel;
        wxCompositionMode compositionMode;
    };
    std::stack<State> m_stateStack;

    wxDECLARE_NO_COPY_CLASS(wxSVGGraphicsContext);
};

#endif // wxUSE_SVG && wxUSE_GRAPHICS_CONTEXT

#endif // _WX_SVGGC_H_
