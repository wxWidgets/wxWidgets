/////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/svggc.h
// Purpose:     internal data classes for wxSVGGraphicsContext/Renderer
// Author:      Blake Madden
// Copyright:   (c) Blake Madden
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_SVGGC_H_
#define _WX_PRIVATE_SVGGC_H_

#include "wx/defs.h"

#if wxUSE_SVG && wxUSE_GRAPHICS_CONTEXT

#include "wx/bitmap.h"
#include "wx/brush.h"
#include "wx/font.h"
#include "wx/graphics.h"
#include "wx/pen.h"
#include "wx/private/graphics.h"

#include <vector>

// A single command in an SVG path (Move, Line, etc.).
// Stored instead of a raw string so that transformations can be applied
// to the path after it has been recorded.
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
        if ( x1 )
            *x1 = m_x1;
        if ( y1 )
            *y1 = m_y1;
        if ( x2 )
            *x2 = m_x2;
        if ( y2 )
            *y2 = m_y2;
    }
    void GetRadialParameters(wxDouble* startX, wxDouble* startY, wxDouble* endX, wxDouble* endY, wxDouble* radius) const
    {
        if ( startX )
            *startX = m_x1;
        if ( startY )
            *startY = m_y1;
        if ( endX )
            *endX = m_x2;
        if ( endY )
            *endY = m_y2;
        if ( radius )
            *radius = m_radius;
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

#endif // wxUSE_SVG && wxUSE_GRAPHICS_CONTEXT

#endif // _WX_PRIVATE_SVGGC_H_
