/////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/svg.h
// Purpose:     Private SVG helpers shared by wxSVGFileDC and wxSVGGraphicsContext
// Author:      Blake Madden
// Copyright:   (c) Blake Madden
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_SVG_H_
#define _WX_PRIVATE_SVG_H_

#include "wx/defs.h"

#if wxUSE_SVG

#include "wx/string.h"
#include "wx/colour.h"
#include "wx/pen.h"
#include "wx/brush.h"
#include "wx/font.h"
#include "wx/dcmemory.h"
#include "wx/dcsvg.h"

#if wxUSE_GRAPHICS_CONTEXT
#include "wx/graphics.h"
#endif

#include <memory>

class WXDLLIMPEXP_FWD_CORE wxBitmap;

namespace wxSVG
{

// Formats a double in C locale with 2-digit precision.
wxString NumStr(double f);
wxString NumStr(float f);

// Returns the colour as "#rrggbb" and optionally its alpha as 0..1 opacity.
wxString Col2SVG(wxColour c, float* opacity = nullptr);

// Returns a "stroke=... stroke-opacity=..." attribute fragment.
wxString GetPenStroke(const wxColour& c, int style = wxPENSTYLE_SOLID);

// Returns a "fill=... fill-opacity=..." attribute fragment.
wxString GetBrushFill(const wxColour& c, int style = wxBRUSHSTYLE_SOLID);

// Returns a <pattern> element definition for hatched brushes, or empty.
wxString CreateBrushFill(const wxBrush& brush, wxSVGShapeRenderingMode mode);

// Returns a "shape-rendering=..." attribute fragment.
wxString GetRenderMode(wxSVGShapeRenderingMode style);

// Returns a "fill=url(#...)" attribute for a hatched brush, or empty.
wxString GetBrushPattern(const wxBrush& brush);

// Returns the unique pattern name (id) for a hatched brush, or empty.
wxString GetBrushStyleName(const wxBrush& brush);

// Returns "stroke-width=... stroke-linecap=... stroke-linejoin=..." fragment.
wxString GetPenStyle(const wxPen& pen);

// Returns a "stroke-dasharray=..." attribute fragment for dashed pens, or empty.
wxString GetPenPattern(const wxPen& pen);

// Returns a wxMemoryDC configured with the given font, scaled so that its
// metrics match the SVG's DPI-independent coordinate system.
wxMemoryDC GetTextMetricDC(const wxFont& font);

} // namespace wxSVG

// ---------------------------------------------------------------------------
// wxSVGWriter
//
// Owns the SVG output buffer and the shared bookkeeping (nesting counters,
// rendering mode, bitmap handler, graphics-change flag, gradient/clip IDs)
// that both wxSVGFileDC and wxSVGGraphicsContext need to coordinate on.
//
// The writer is stateless about coordinate transforms, pen/brush colour, and
// font — those remain owned by the DC (via wxDCImpl) and the GC, which pass
// them in when they need to open a new <g> group. The writer's job is purely
// to serialize whatever its owner emits.
// ---------------------------------------------------------------------------
class WXDLLIMPEXP_CORE wxSVGWriter
{
public:
    wxSVGWriter(const wxString& filename,
                int width, int height,
                double dpi,
                const wxString& title);

    // Append a raw fragment to the output buffer.
    void Write(const wxString& s);

    // Returns the document with any still-open groups (clip, layer, pen/brush,
    // accessible) closed and the root </svg> appended.
    wxString GetDocument() const;

    // Writes the document to the configured filename. Returns true on success,
    // or if the document was already saved.
    bool Save();

    bool IsOk() const { return !m_writeError; }
    void SetWriteError() { m_writeError = true; }

    const wxString& GetFilename() const { return m_filename; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    double GetDPI() const { return m_dpi; }

    // Pen/brush change flag controlling when a new <g> group is opened.
    void MarkGraphicsChanged() { m_graphicsChanged = true; }
    bool IsGraphicsChanged() const { return m_graphicsChanged; }
    void ClearGraphicsChanged() { m_graphicsChanged = false; }

    wxSVGShapeRenderingMode GetShapeRenderingMode() const { return m_renderingMode; }
    void SetShapeRenderingMode(wxSVGShapeRenderingMode mode) { m_renderingMode = mode; }

    // Takes ownership. Pass nullptr to reset to the default (file) handler.
    void SetBitmapHandler(wxSVGBitmapHandler* handler);

    // Writes the bitmap via the current handler (creating a default file
    // handler on first use). Sets the write-error flag on failure.
    void WriteBitmap(const wxBitmap& bmp, wxCoord x, wxCoord y);

    // Returns the next gradient/clip id and increments the shared counter.
    size_t GetNextGradientId() { return ms_gradientUniqueId++; }
    size_t GetNextClipId() { return ms_clipUniqueId++; }

    size_t GetAccessibleGroupDepth() const { return m_accessibleGroupDepth; }
    void IncrementAccessibleGroupDepth() { ++m_accessibleGroupDepth; }
    void DecrementAccessibleGroupDepth() { --m_accessibleGroupDepth; }

    size_t GetLayerDepth() const { return m_layerDepth; }
    void IncrementLayerDepth() { ++m_layerDepth; }
    void DecrementLayerDepth() { --m_layerDepth; }

    size_t GetClipNestingLevel() const { return m_clipNestingLevel; }
    void IncrementClipNestingLevel() { ++m_clipNestingLevel; }
    void SetClipNestingLevel(size_t n) { m_clipNestingLevel = n; }

#if wxUSE_GRAPHICS_CONTEXT
    const wxGraphicsBrush& GetGraphicsBrush() const { return m_graphicsBrush; }
    void SetGraphicsBrush(const wxGraphicsBrush& brush) { m_graphicsBrush = brush; }

    const wxGraphicsPen& GetGraphicsPen() const { return m_graphicsPen; }
    void SetGraphicsPen(const wxGraphicsPen& pen) { m_graphicsPen = pen; }

    wxCompositionMode GetCompositionMode() const { return m_compositionMode; }
    // Returns true and marks graphics changed if the mode differs from the
    // current one; returns false otherwise.
    bool SetCompositionMode(wxCompositionMode mode);

    const wxString& GetGCTransform() const { return m_gcTransform; }
    void SetGCTransform(const wxString& transform) { m_gcTransform = transform; }

    // Writes a <defs>/<linearGradient>/<radialGradient> block if the brush is
    // a gradient, and returns the corresponding fill="url(#...)" fragment. If
    // the brush is not a gradient, returns an empty string and writes nothing.
    wxString WriteGraphicsBrushFill(const wxGraphicsBrush& brush);

    // As above, for pens with a gradient stroke; returns a stroke="url(#...)"
    // fragment.
    wxString WriteGraphicsPenStroke(const wxGraphicsPen& pen);
#endif // wxUSE_GRAPHICS_CONTEXT

private:
    void WriteHeader(const wxString& title);

    wxString m_svgDocument;
    wxString m_filename;
    bool m_writeError = false;
    bool m_saved = false;
    bool m_graphicsChanged = true;
    int m_width;
    int m_height;
    double m_dpi;
    std::unique_ptr<wxSVGBitmapHandler> m_bmpHandler;
    wxSVGShapeRenderingMode m_renderingMode = wxSVG_SHAPE_RENDERING_AUTO;

    size_t m_accessibleGroupDepth = 0;
    size_t m_layerDepth = 0;
    size_t m_clipNestingLevel = 0;

    static size_t ms_clipUniqueId;
    static size_t ms_gradientUniqueId;

#if wxUSE_GRAPHICS_CONTEXT
    wxGraphicsBrush m_graphicsBrush;
    wxGraphicsPen m_graphicsPen;
    wxCompositionMode m_compositionMode = wxCOMPOSITION_INVALID;
    wxString m_gcTransform;
#endif

    wxDECLARE_NO_COPY_CLASS(wxSVGWriter);
};

#endif // wxUSE_SVG

#endif // _WX_PRIVATE_SVG_H_
