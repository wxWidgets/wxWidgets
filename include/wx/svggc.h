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

#include <stack>

class wxSVGFileDCImpl;
class WXDLLIMPEXP_FWD_CORE wxSVGWriter;

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

    // Create a context that draws into the given SVG DC. The DC must outlive
    // the returned context.
    wxGraphicsContext* CreateContext(wxSVGFileDC& dc);

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
    explicit wxSVGGraphicsContext(wxSVGFileDC& dc);
    virtual ~wxSVGGraphicsContext();

    // Construct a graphics context that draws into the given SVG DC. The DC
    // must outlive the returned context.
    static wxGraphicsContext* Create(wxSVGFileDC& dc);

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
    wxSVGWriter* const m_writer;
    wxPen m_currentPen;
    wxBrush m_currentBrush;
    wxFont m_currentFont;
    wxColour m_currentTextColour;

    // Running current-transform-matrix plus save-state stack.
    wxGraphicsMatrix m_transform;

    struct State
    {
        wxGraphicsMatrix transform;
        int clipNestingLevel;
        wxCompositionMode compositionMode;
    };
    std::stack<State> m_stateStack;

    wxDECLARE_NO_COPY_CLASS(wxSVGGraphicsContext);
};

#endif // wxUSE_SVG && wxUSE_GRAPHICS_CONTEXT

#endif // _WX_SVGGC_H_
