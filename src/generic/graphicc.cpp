/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/graphicc.cpp
// Purpose:     cairo device context class
// Author:      Stefan Csomor
// Modified by:
// Created:     2006-10-03
// Copyright:   (c) 2006 Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"


#if wxUSE_GRAPHICS_CONTEXT

#include "wx/graphics.h"

#if wxUSE_CAIRO

#ifndef __WXGTK20__
// keep cairo.h from defining dllimport as we're defining the symbols inside
// the wx dll in order to load them dynamically.
#define cairo_public
#endif

#include <cairo.h>
#include <float.h>

bool wxCairoInit();

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
    #include "wx/icon.h"
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
    #include "wx/dcprint.h"
    #include "wx/window.h"
#endif

#include "wx/private/graphics.h"
#include "wx/rawbmp.h"
#include "wx/vector.h"
#include "wx/display.h"
#ifdef __WXMSW__
    #include "wx/msw/enhmeta.h"
#endif

using namespace std;

//-----------------------------------------------------------------------------
// device context implementation
//
// more and more of the dc functionality should be implemented by calling
// the appropricate wxCairoContext, but we will have to do that step by step
// also coordinate conversions should be moved to native matrix ops
//-----------------------------------------------------------------------------

// we always stock two context states, one at entry, to be able to preserve the
// state we were called with, the other one after changing to HI Graphics orientation
// (this one is used for getting back clippings etc)

//-----------------------------------------------------------------------------
// wxGraphicsPath implementation
//-----------------------------------------------------------------------------

#ifdef __WXMSW__
// TODO remove this dependency (gdiplus needs the macros)

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#include <cairo-win32.h>
// Notice that the order is important: cairo-win32.h includes windows.h which
// pollutes the global name space with macros so include our own header which
// #undefines them after it.
#include "wx/msw/private.h"
#endif

#ifdef __WXGTK__
#ifdef __WXGTK20__
#include "wx/gtk/private/wrapgtk.h"
#else // GTK+ 1.x
#include <gtk/gtk.h>
#endif
#include "wx/fontutil.h"
#ifndef __WXGTK3__
#include "wx/gtk/dc.h"
#endif
#include "wx/gtk/private/object.h"
#endif

#ifdef __WXQT__
#include <QtGui/QPainter>
#include "wx/qt/dc.h"
#endif

#ifdef __WXMAC__
#include "wx/osx/private.h"
#include <cairo-quartz.h>
#endif

// Helper functions for dealing with alpha pre-multiplication.
namespace
{

    inline unsigned char Premultiply(unsigned char alpha, unsigned char data)
    {
        return alpha ? (data * alpha) / 0xff : data;
    }

    inline unsigned char Unpremultiply(unsigned char alpha, unsigned char data)
    {
        return alpha ? (data * 0xff) / alpha : data;
    }

} // anonymous namespace

class WXDLLIMPEXP_CORE wxCairoPathData : public wxGraphicsPathData
{
public :
    wxCairoPathData(wxGraphicsRenderer* renderer, cairo_t* path = NULL);
    ~wxCairoPathData();

    virtual wxGraphicsObjectRefData *Clone() const wxOVERRIDE;

    //
    // These are the path primitives from which everything else can be constructed
    //

    // begins a new subpath at (x,y)
    virtual void MoveToPoint( wxDouble x, wxDouble y ) wxOVERRIDE;

    // adds a straight line from the current point to (x,y)
    virtual void AddLineToPoint( wxDouble x, wxDouble y ) wxOVERRIDE;

    // adds a cubic Bezier curve from the current point, using two control points and an end point
    virtual void AddCurveToPoint( wxDouble cx1, wxDouble cy1, wxDouble cx2, wxDouble cy2, wxDouble x, wxDouble y ) wxOVERRIDE;


    // adds an arc of a circle centering at (x,y) with radius (r) from startAngle to endAngle
    virtual void AddArc( wxDouble x, wxDouble y, wxDouble r, wxDouble startAngle, wxDouble endAngle, bool clockwise ) wxOVERRIDE ;

    // gets the last point of the current path, (0,0) if not yet set
    virtual void GetCurrentPoint( wxDouble* x, wxDouble* y) const wxOVERRIDE;

    // adds another path
    virtual void AddPath( const wxGraphicsPathData* path ) wxOVERRIDE;

    // closes the current sub-path
    virtual void CloseSubpath() wxOVERRIDE;

    //
    // These are convenience functions which - if not available natively will be assembled
    // using the primitives from above
    //

    // appends a rectangle as a new closed subpath
    virtual void AddRectangle(wxDouble x, wxDouble y, wxDouble w, wxDouble h) wxOVERRIDE;

    // appends a circle as a new closed subpath
    virtual void AddCircle(wxDouble x, wxDouble y, wxDouble r) wxOVERRIDE;

    // appends an ellipse as a new closed subpath fitting the passed rectangle
    virtual void AddEllipse(wxDouble x, wxDouble y, wxDouble w, wxDouble h) wxOVERRIDE;

    /*
    // draws a an arc to two tangents connecting (current) to (x1,y1) and (x1,y1) to (x2,y2), also a straight line from (current) to (x1,y1)
    virtual void AddArcToPoint( wxDouble x1, wxDouble y1 , wxDouble x2, wxDouble y2, wxDouble r )  ;
    */

    // returns the native path
    virtual void * GetNativePath() const wxOVERRIDE ;

    // give the native path returned by GetNativePath() back (there might be some deallocations necessary)
    virtual void UnGetNativePath(void *p) const wxOVERRIDE;

    // transforms each point of this path by the matrix
    virtual void Transform( const wxGraphicsMatrixData* matrix ) wxOVERRIDE ;

    // gets the bounding box enclosing all points (possibly including control points)
    virtual void GetBox(wxDouble *x, wxDouble *y, wxDouble *w, wxDouble *h) const wxOVERRIDE;

    virtual bool Contains( wxDouble x, wxDouble y, wxPolygonFillMode fillStyle = wxWINDING_RULE) const wxOVERRIDE;

private :
    cairo_t* m_pathContext;
};

class WXDLLIMPEXP_CORE wxCairoMatrixData : public wxGraphicsMatrixData
{
public :
    wxCairoMatrixData(wxGraphicsRenderer* renderer, const cairo_matrix_t* matrix = NULL ) ;
    virtual ~wxCairoMatrixData() ;

    virtual wxGraphicsObjectRefData *Clone() const wxOVERRIDE ;

    // concatenates the matrix
    virtual void Concat( const wxGraphicsMatrixData *t ) wxOVERRIDE;

    // sets the matrix to the respective values
    virtual void Set(wxDouble a=1.0, wxDouble b=0.0, wxDouble c=0.0, wxDouble d=1.0,
        wxDouble tx=0.0, wxDouble ty=0.0) wxOVERRIDE;

    // gets the component valuess of the matrix
    virtual void Get(wxDouble* a=NULL, wxDouble* b=NULL,  wxDouble* c=NULL,
                     wxDouble* d=NULL, wxDouble* tx=NULL, wxDouble* ty=NULL) const wxOVERRIDE;

    // makes this the inverse matrix
    virtual void Invert() wxOVERRIDE;

    // returns true if the elements of the transformation matrix are equal ?
    virtual bool IsEqual( const wxGraphicsMatrixData* t) const wxOVERRIDE ;

    // return true if this is the identity matrix
    virtual bool IsIdentity() const wxOVERRIDE;

    //
    // transformation
    //

    // add the translation to this matrix
    virtual void Translate( wxDouble dx , wxDouble dy ) wxOVERRIDE;

    // add the scale to this matrix
    virtual void Scale( wxDouble xScale , wxDouble yScale ) wxOVERRIDE;

    // add the rotation to this matrix (radians)
    virtual void Rotate( wxDouble angle ) wxOVERRIDE;

    //
    // apply the transforms
    //

    // applies that matrix to the point
    virtual void TransformPoint( wxDouble *x, wxDouble *y ) const wxOVERRIDE;

    // applies the matrix except for translations
    virtual void TransformDistance( wxDouble *dx, wxDouble *dy ) const wxOVERRIDE;

    // returns the native representation
    virtual void * GetNativeMatrix() const wxOVERRIDE;
private:
    cairo_matrix_t m_matrix ;
} ;

// Common base class for pens and brushes.
class wxCairoPenBrushBaseData : public wxGraphicsObjectRefData
{
public:
    wxCairoPenBrushBaseData(wxGraphicsRenderer* renderer,
                            const wxColour& col,
                            bool isTransparent);
    virtual ~wxCairoPenBrushBaseData();

    virtual void Apply( wxGraphicsContext* context );

    void CreateLinearGradientPattern(wxDouble x1, wxDouble y1,
                                     wxDouble x2, wxDouble y2,
                                     const wxGraphicsGradientStops& stops,
                                     const wxGraphicsMatrix& matrix = wxNullGraphicsMatrix);
    void CreateRadialGradientPattern(wxDouble startX, wxDouble startY,
                                     wxDouble endX, wxDouble endY, wxDouble radius,
                                     const wxGraphicsGradientStops& stops,
                                     const wxGraphicsMatrix& matrix = wxNullGraphicsMatrix);

protected:
    // Call this to use the given bitmap as stipple. Bitmap must be non-null
    // and valid.
    void InitStipple(wxBitmap* bmp);

    // Call this to use the given hatch style. Hatch style must be valid.
    void InitHatch(wxHatchStyle hatchStyle);

    // common part of Create{Linear,Radial}GradientPattern()
    void AddGradientStops(const wxGraphicsGradientStops& stops);

    double m_red;
    double m_green;
    double m_blue;
    double m_alpha;

    cairo_pattern_t* m_pattern;
    class wxCairoBitmapData* m_bmpdata;

private:
    // Called once to allocate m_pattern if needed.
    void InitHatchPattern(cairo_t* ctext);

    wxHatchStyle m_hatchStyle;

    wxDECLARE_NO_COPY_CLASS(wxCairoPenBrushBaseData);
};

class WXDLLIMPEXP_CORE wxCairoPenData : public wxCairoPenBrushBaseData
{
public:
    wxCairoPenData( wxGraphicsRenderer* renderer, const wxGraphicsPenInfo &info );
    ~wxCairoPenData();

    void Init();

    virtual void Apply( wxGraphicsContext* context ) wxOVERRIDE;
    wxDouble GetWidth() { return m_width; }

private :
    double m_width;

    cairo_line_cap_t m_cap;
    cairo_line_join_t m_join;

    int m_count;
    const double *m_lengths;
    double *m_userLengths;

    wxDECLARE_NO_COPY_CLASS(wxCairoPenData);
};

class WXDLLIMPEXP_CORE wxCairoBrushData : public wxCairoPenBrushBaseData
{
public:
    wxCairoBrushData( wxGraphicsRenderer* renderer );
    wxCairoBrushData( wxGraphicsRenderer* renderer, const wxBrush &brush );

protected:
    void Init();
};

class wxCairoFontData : public wxGraphicsObjectRefData
{
public:
    wxCairoFontData( wxGraphicsRenderer* renderer,
                     const wxFont &font,
                     const wxRealPoint& dpi,
                     const wxColour& col );
    wxCairoFontData(wxGraphicsRenderer* renderer,
                    double sizeInPixels,
                    const wxString& facename,
                    int flags,
                    const wxColour& col);
    ~wxCairoFontData();

    void Apply( wxGraphicsContext* context );
#ifdef __WXGTK__
    const wxFont& GetFont() const { return m_wxfont; }
#endif
private :
    void InitColour(const wxColour& col);
    void InitFontComponents(const wxString& facename,
                            cairo_font_slant_t slant,
                            cairo_font_weight_t weight);

    double m_size;
    double m_red;
    double m_green;
    double m_blue;
    double m_alpha;
#ifdef __WXMAC__
    cairo_font_face_t *m_font;
#elif defined(__WXGTK__)
    wxFont m_wxfont;
#endif

    // These members are used when the font is created from its face name and
    // flags (and not from wxFont) and also even when creating it from wxFont
    // on the platforms not covered above.
    //
    // Notice that we can't use cairo_font_face_t instead of storing those,
    // even though it would be simpler and need less #ifdefs, because
    // cairo_toy_font_face_create() that we'd need to create it is only
    // available in Cairo 1.8 and we require just 1.2 currently. If we do drop
    // support for < 1.8 versions in the future it would be definitely better
    // to use cairo_toy_font_face_create() instead.
    wxCharBuffer m_fontName;
    cairo_font_slant_t m_slant;
    cairo_font_weight_t m_weight;
};

class wxCairoBitmapData : public wxGraphicsBitmapData
{
public:
    wxCairoBitmapData( wxGraphicsRenderer* renderer, const wxBitmap& bmp );
#if wxUSE_IMAGE
    wxCairoBitmapData(wxGraphicsRenderer* renderer, const wxImage& image);
#endif // wxUSE_IMAGE
    wxCairoBitmapData( wxGraphicsRenderer* renderer, cairo_surface_t* bitmap );
    ~wxCairoBitmapData();

    cairo_surface_t* GetCairoSurface() { return m_surface; }
    cairo_pattern_t* GetCairoPattern() { return m_pattern; }
    void* GetNativeBitmap() const wxOVERRIDE { return m_surface; }
    wxSize GetSize() { return wxSize(m_width, m_height); }

#if wxUSE_IMAGE
    wxImage ConvertToImage() const;
#endif // wxUSE_IMAGE

private :
    // Allocate m_buffer for the bitmap of the given size in the given format.
    //
    // Returns the stride used for the buffer.
    int InitBuffer(int width, int height, cairo_format_t format);

    // Really create the surface using the buffer (which was supposed to be
    // filled since InitBuffer() call).
    void InitSurface(cairo_format_t format, int stride);


    cairo_surface_t* m_surface;
    cairo_pattern_t* m_pattern;
    int m_width;
    int m_height;
    unsigned char* m_buffer;
};

class WXDLLIMPEXP_CORE wxCairoContext : public wxGraphicsContext
{
public:
    wxCairoContext( wxGraphicsRenderer* renderer, const wxWindowDC& dc );
    wxCairoContext( wxGraphicsRenderer* renderer, const wxMemoryDC& dc );
#if wxUSE_PRINTING_ARCHITECTURE
    wxCairoContext( wxGraphicsRenderer* renderer, const wxPrinterDC& dc );
#endif
#ifdef __WXGTK__
    wxCairoContext( wxGraphicsRenderer* renderer, GdkWindow *window );
#endif
#ifdef __WXMSW__
#if wxUSE_ENH_METAFILE
    wxCairoContext(wxGraphicsRenderer* renderer, const wxEnhMetaFileDC& dc);
#endif // wxUSE_ENH_METAFILE
    wxCairoContext( wxGraphicsRenderer* renderer, HDC context );
    wxCairoContext(wxGraphicsRenderer* renderer, HWND hWnd);
#endif
    wxCairoContext( wxGraphicsRenderer* renderer, cairo_t *context );
    wxCairoContext( wxGraphicsRenderer* renderer, wxWindow *window);

    // If this ctor is used, Init() must be called by the derived class later.
    wxCairoContext( wxGraphicsRenderer* renderer );

    virtual ~wxCairoContext();

    virtual bool ShouldOffset() const wxOVERRIDE
    {
        if (!m_enableOffset || m_pen.IsNull())
            return false;

        const double width = static_cast<wxCairoPenData*>(m_pen.GetRefData())->GetWidth();

        // always offset for 1-pixel width
        if (width <= 0)
            return true;

        // no offset if overall scale is not odd integer
        double x = GetContentScaleFactor(), y = x;
        cairo_user_to_device_distance(m_context, &x, &y);
        if (!wxIsSameDouble(fmod(wxMin(fabs(x), fabs(y)), 2.0), 1.0))
            return false;

        // offset if pen width is odd integer
        return wxIsSameDouble(fmod(width, 2.0), 1.0);
    }

    virtual void Clip( const wxRegion &region ) wxOVERRIDE;

    // clips drawings to the rect
    virtual void Clip( wxDouble x, wxDouble y, wxDouble w, wxDouble h ) wxOVERRIDE;

    // resets the clipping to original extent
    virtual void ResetClip() wxOVERRIDE;

    // returns bounding box of the clipping region
    virtual void GetClipBox(wxDouble* x, wxDouble* y, wxDouble* w, wxDouble* h) wxOVERRIDE;

    virtual void * GetNativeContext() wxOVERRIDE;

    virtual bool SetAntialiasMode(wxAntialiasMode antialias) wxOVERRIDE;

    virtual bool SetInterpolationQuality(wxInterpolationQuality interpolation) wxOVERRIDE;

    virtual bool SetCompositionMode(wxCompositionMode op) wxOVERRIDE;

    virtual void BeginLayer(wxDouble opacity) wxOVERRIDE;

    virtual void EndLayer() wxOVERRIDE;

    virtual void StrokePath( const wxGraphicsPath& p ) wxOVERRIDE;
    virtual void FillPath( const wxGraphicsPath& p , wxPolygonFillMode fillStyle = wxWINDING_RULE ) wxOVERRIDE;
    virtual void ClearRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h ) wxOVERRIDE;
    virtual void DrawRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h) wxOVERRIDE;

    virtual void Translate( wxDouble dx , wxDouble dy ) wxOVERRIDE;
    virtual void Scale( wxDouble xScale , wxDouble yScale ) wxOVERRIDE;
    virtual void Rotate( wxDouble angle ) wxOVERRIDE;

    // concatenates this transform with the current transform of this context
    virtual void ConcatTransform( const wxGraphicsMatrix& matrix ) wxOVERRIDE;

    // sets the transform of this context
    virtual void SetTransform( const wxGraphicsMatrix& matrix ) wxOVERRIDE;

    // gets the matrix of this context
    virtual wxGraphicsMatrix GetTransform() const wxOVERRIDE;

    virtual void DrawBitmap( const wxGraphicsBitmap &bmp, wxDouble x, wxDouble y, wxDouble w, wxDouble h ) wxOVERRIDE;
    virtual void DrawBitmap( const wxBitmap &bmp, wxDouble x, wxDouble y, wxDouble w, wxDouble h ) wxOVERRIDE;
    virtual void DrawIcon( const wxIcon &icon, wxDouble x, wxDouble y, wxDouble w, wxDouble h ) wxOVERRIDE;
    virtual void PushState() wxOVERRIDE;
    virtual void PopState() wxOVERRIDE;
    virtual void Flush() wxOVERRIDE;

    void GetDPI(wxDouble* dpiX, wxDouble* dpiY) const wxOVERRIDE;

    virtual void GetTextExtent( const wxString &str, wxDouble *width, wxDouble *height,
                                wxDouble *descent, wxDouble *externalLeading ) const wxOVERRIDE;
    virtual void GetPartialTextExtents(const wxString& text, wxArrayDouble& widths) const wxOVERRIDE;

#ifdef __WXMSW__
    virtual WXHDC GetNativeHDC() wxOVERRIDE;
    virtual void ReleaseNativeHDC(WXHDC WXUNUSED(hdc)) wxOVERRIDE;
#endif

protected:
    virtual void DoDrawText( const wxString &str, wxDouble x, wxDouble y ) wxOVERRIDE;

    void Init(cairo_t *context);

    enum ApplyTransformMode { Apply_directly, Apply_scaled_dev_origin };
    void ApplyTransformFromDC(const wxDC& dc, ApplyTransformMode mode = Apply_directly);

#ifdef __WXQT__
    QPainter* m_qtPainter;
    QImage* m_qtImage;
    cairo_surface_t* m_qtSurface;
#endif
#ifdef __WXMSW__
    cairo_surface_t* m_mswSurface;
    WindowHDC m_mswWindowHDC;
    int m_mswStateSavedDC;
#endif
#ifdef __WXGTK__
    // Tiny helper actually applying the font. It's convenient because it can
    // be called with a temporary wxFont, as we're going to make a copy of its
    // Pango font description inside this function before the font object is
    // destroyed.
    //
    // It's also all we need for GTK < 3.
    static void DoApplyFont(PangoLayout* layout, const wxFont& font)
    {
        pango_layout_set_font_description
        (
            layout,
            font.GetNativeFontInfo()->description
        );
    }

#ifdef __WXGTK3__
    // This factor must be applied to the font before actually using it, for
    // consistency with the text drawn by GTK itself.
    float m_fontScalingFactor;

    // Function applying the Pango font description for the given font scaled by
    // the font scaling factor if necessary to the specified layout.
    void ApplyFont(PangoLayout* layout, const wxFont& font) const
    {
        // Only scale the font if we really need to do it.
        DoApplyFont(layout, m_fontScalingFactor == 1.0f
                                ? font
                                : font.Scaled(m_fontScalingFactor));
    }
#else // GTK < 3
    // Provide the same function even if it does nothing in this case to keep
    // the same code for all GTK versions.
    void ApplyFont(PangoLayout* layout, const wxFont& font) const
    {
        DoApplyFont(layout, font);
    }
#endif // __WXGTK3__
#endif // __WXGTK__

private:
    cairo_t* m_context;
    cairo_matrix_t m_internalTransform;

    wxVector<float> m_layerOpacities;

    wxDECLARE_NO_COPY_CLASS(wxCairoContext);
};

#if wxUSE_IMAGE
// ----------------------------------------------------------------------------
// wxCairoImageContext: context associated with a wxImage.
// ----------------------------------------------------------------------------

class wxCairoImageContext : public wxCairoContext
{
public:
    wxCairoImageContext(wxGraphicsRenderer* renderer, wxImage& image) :
        wxCairoContext(renderer),
        m_image(image),
        m_data(renderer, image)
    {
        Init(cairo_create(m_data.GetCairoSurface()));
        m_width = image.GetWidth();
        m_height = image.GetHeight();
    }

    virtual ~wxCairoImageContext()
    {
        Flush();
    }

    virtual void Flush() wxOVERRIDE
    {
        m_image = m_data.ConvertToImage();
    }

private:
    wxImage& m_image;
    wxCairoBitmapData m_data;

    wxDECLARE_NO_COPY_CLASS(wxCairoImageContext);
};
#endif // wxUSE_IMAGE

#ifdef __WXMSW__
class wxCairoMeasuringContext : public wxCairoContext
{
public:
    wxCairoMeasuringContext(wxGraphicsRenderer* renderer)
        : wxCairoContext(renderer, m_hdc = ::GetDC(NULL))
    {
    }

    virtual ~wxCairoMeasuringContext()
    {
        ::ReleaseDC(NULL, m_hdc);
    }

private:
    HDC m_hdc;
};
#endif // __WXMSW__

// ----------------------------------------------------------------------------
// wxCairoPenBrushBaseData implementation
//-----------------------------------------------------------------------------

wxCairoPenBrushBaseData::wxCairoPenBrushBaseData(wxGraphicsRenderer* renderer,
                                                 const wxColour& col,
                                                 bool isTransparent)
    : wxGraphicsObjectRefData(renderer)
{
    m_hatchStyle = wxHATCHSTYLE_INVALID;
    m_pattern = NULL;
    m_bmpdata = NULL;

    if ( isTransparent || !col.IsOk() )
    {
        m_red =
        m_green =
        m_blue =
        m_alpha = 0;
    }
    else // non-transparent
    {
        m_red = col.Red()/255.0;
        m_green = col.Green()/255.0;
        m_blue = col.Blue()/255.0;
        m_alpha = col.Alpha()/255.0;
    }
}

wxCairoPenBrushBaseData::~wxCairoPenBrushBaseData()
{
    if (m_bmpdata)
    {
        // Deleting the bitmap data also deletes the pattern referenced by
        // m_pattern, so set it to NULL to avoid deleting it twice.
        delete m_bmpdata;
        m_pattern = NULL;
    }
    if (m_pattern)
        cairo_pattern_destroy(m_pattern);
}

void wxCairoPenBrushBaseData::InitHatchPattern(cairo_t* ctext)
{
    cairo_surface_t* const
        surface = cairo_surface_create_similar(
                    cairo_get_target(ctext), CAIRO_CONTENT_COLOR_ALPHA, 10, 10
                );

    cairo_t* const cr = cairo_create(surface);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_SQUARE);
    cairo_set_line_width(cr, 1);
    cairo_set_line_join(cr,CAIRO_LINE_JOIN_MITER);

    switch ( m_hatchStyle )
    {
        case wxHATCHSTYLE_CROSS:
            cairo_move_to(cr, 5, 0);
            cairo_line_to(cr, 5, 10);
            cairo_move_to(cr, 0, 5);
            cairo_line_to(cr, 10, 5);
            break;

        case wxHATCHSTYLE_BDIAGONAL:
            cairo_move_to(cr, 0, 10);
            cairo_line_to(cr, 10, 0);
            break;

        case wxHATCHSTYLE_FDIAGONAL:
            cairo_move_to(cr, 0, 0);
            cairo_line_to(cr, 10, 10);
            break;

        case wxHATCHSTYLE_CROSSDIAG:
            cairo_move_to(cr, 0, 0);
            cairo_line_to(cr, 10, 10);
            cairo_move_to(cr, 10, 0);
            cairo_line_to(cr, 0, 10);
            break;

        case wxHATCHSTYLE_HORIZONTAL:
            cairo_move_to(cr, 0, 5);
            cairo_line_to(cr, 10, 5);
            break;

        case wxHATCHSTYLE_VERTICAL:
            cairo_move_to(cr, 5, 0);
            cairo_line_to(cr, 5, 10);
            break;

        default:
            wxFAIL_MSG(wxS("Invalid hatch pattern style."));
    }

    cairo_set_source_rgba(cr, m_red, m_green, m_blue, m_alpha);
    cairo_stroke(cr);

    cairo_destroy(cr);

    m_pattern = cairo_pattern_create_for_surface(surface);
    cairo_surface_destroy(surface);
    cairo_pattern_set_extend(m_pattern, CAIRO_EXTEND_REPEAT);
}

void wxCairoPenBrushBaseData::InitStipple(wxBitmap* bmp)
{
    wxCHECK_RET( bmp && bmp->IsOk(), wxS("Invalid stippled bitmap") );

    m_bmpdata = new wxCairoBitmapData(GetRenderer(), *bmp);
    m_pattern = m_bmpdata->GetCairoPattern();
    cairo_pattern_set_extend(m_pattern, CAIRO_EXTEND_REPEAT);
}

void wxCairoPenBrushBaseData::InitHatch(wxHatchStyle hatchStyle)
{
    // We can't create m_pattern right now as we don't have the Cairo context
    // needed for it, so just remember that we need to do it.
    m_hatchStyle = hatchStyle;
}

void wxCairoPenBrushBaseData::Apply( wxGraphicsContext* context )
{
    cairo_t* const ctext = (cairo_t*) context->GetNativeContext();

    if ( m_hatchStyle != wxHATCHSTYLE_INVALID && !m_pattern )
        InitHatchPattern(ctext);

    if ( m_pattern )
        cairo_set_source(ctext, m_pattern);
    else
        cairo_set_source_rgba(ctext, m_red, m_green, m_blue, m_alpha);
}

void wxCairoPenBrushBaseData::AddGradientStops(const wxGraphicsGradientStops& stops)
{
    // loop over all the stops, they include the beginning and ending ones
    const unsigned numStops = stops.GetCount();
    for ( unsigned n = 0; n < numStops; n++ )
    {
        const wxGraphicsGradientStop stop = stops.Item(n);

        const wxColour col = stop.GetColour();

        cairo_pattern_add_color_stop_rgba
        (
            m_pattern,
            double(stop.GetPosition()),
            col.Red()/255.0,
            col.Green()/255.0,
            col.Blue()/255.0,
            col.Alpha()/255.0
        );
    }

    wxASSERT_MSG(cairo_pattern_status(m_pattern) == CAIRO_STATUS_SUCCESS,
                 wxT("Couldn't create cairo pattern"));
}

void
wxCairoPenBrushBaseData::CreateLinearGradientPattern(wxDouble x1, wxDouble y1,
                                                     wxDouble x2, wxDouble y2,
                                                     const wxGraphicsGradientStops& stops,
                                                     const wxGraphicsMatrix& matrix)
{
    m_pattern = cairo_pattern_create_linear(x1,y1,x2,y2);

    if ( !matrix.IsNull() )
    {
        cairo_matrix_t m = *((cairo_matrix_t*) matrix.GetNativeMatrix());
        cairo_pattern_set_matrix(m_pattern, &m);
    }

    AddGradientStops(stops);
}

void
wxCairoPenBrushBaseData::CreateRadialGradientPattern(wxDouble startX, wxDouble startY,
                                                     wxDouble endX, wxDouble endY,
                                                     wxDouble radius,
                                                     const wxGraphicsGradientStops& stops,
                                                     const wxGraphicsMatrix& matrix)
{
    m_pattern = cairo_pattern_create_radial(startX,startY,0.0,endX,endY,radius);

    if ( !matrix.IsNull() )
    {
        cairo_matrix_t m = *((cairo_matrix_t*) matrix.GetNativeMatrix());
        cairo_pattern_set_matrix(m_pattern, &m);
    }

    AddGradientStops(stops);
}

//-----------------------------------------------------------------------------
// wxCairoPenData implementation
//-----------------------------------------------------------------------------

wxCairoPenData::~wxCairoPenData()
{
    delete[] m_userLengths;
}

void wxCairoPenData::Init()
{
    m_pattern = NULL;
    m_lengths = NULL;
    m_userLengths = NULL;
    m_width = 0;
    m_count = 0;
}

wxCairoPenData::wxCairoPenData( wxGraphicsRenderer* renderer, const wxGraphicsPenInfo &info )
    : wxCairoPenBrushBaseData(renderer, info.GetColour(), info.IsTransparent())
{
    Init();
    m_width = info.GetWidth();

    switch ( info.GetCap() )
    {
    case wxCAP_ROUND :
        m_cap = CAIRO_LINE_CAP_ROUND;
        break;

    case wxCAP_PROJECTING :
        m_cap = CAIRO_LINE_CAP_SQUARE;
        break;

    case wxCAP_BUTT :
        m_cap = CAIRO_LINE_CAP_BUTT;
        break;

    default :
        m_cap = CAIRO_LINE_CAP_BUTT;
        break;
    }

    switch ( info.GetJoin() )
    {
    case wxJOIN_BEVEL :
        m_join = CAIRO_LINE_JOIN_BEVEL;
        break;

    case wxJOIN_MITER :
        m_join = CAIRO_LINE_JOIN_MITER;
        break;

    case wxJOIN_ROUND :
        m_join = CAIRO_LINE_JOIN_ROUND;
        break;

    default :
        m_join = CAIRO_LINE_JOIN_MITER;
        break;
    }

    const double dashUnit = m_width < 1.0 ? 1.0 : m_width;
    const double dotted[] =
    {
        dashUnit , dashUnit + 2.0
    };
    static const double short_dashed[] =
    {
        9.0 , 6.0
    };
    static const double dashed[] =
    {
        19.0 , 9.0
    };
    static const double dotted_dashed[] =
    {
        9.0 , 6.0 , 3.0 , 3.0
    };

    switch ( info.GetStyle() )
    {
    case wxPENSTYLE_SOLID :
        // Non-RGB colours, which may e.g. appear under wxOSX for wxColours
        // with NSColor backend, are not supported by Cairo and have to be
        // handled in a special way.
        if ( !info.GetColour().IsSolid() )
        {
#if defined(__WXOSX_COCOA__)
            // Under wxOSX, non-solid NSColors are actually represented
            // by pattern images and therefore a wxPen with non-solid
            // colour and wxPENSTYLE_SOLID style can be converted
            // to a stiple (a surface pattern).

            // Create a stiple bitmap from NSColor's pattern image
            wxBitmap bmp(info.GetColour().OSXGetNSPatternImage());
            InitStipple(&bmp);
#else
            wxFAIL_MSG( "Pen with non-solid colour is not supported." );
#endif
        }

        break;

    case wxPENSTYLE_DOT :
        m_count = WXSIZEOF(dotted);
        m_userLengths = new double[ m_count ] ;
        memcpy( m_userLengths, dotted, sizeof(dotted) );
        m_lengths = m_userLengths;
        break;

    case wxPENSTYLE_LONG_DASH :
        m_lengths = dashed ;
        m_count = WXSIZEOF(dashed);
        break;

    case wxPENSTYLE_SHORT_DASH :
        m_lengths = short_dashed ;
        m_count = WXSIZEOF(short_dashed);
        break;

    case wxPENSTYLE_DOT_DASH :
        m_lengths = dotted_dashed ;
        m_count = WXSIZEOF(dotted_dashed);
        break;

    case wxPENSTYLE_USER_DASH :
        {
            wxDash *wxdashes ;
            m_count = info.GetDashes( &wxdashes ) ;
            if ((wxdashes != NULL) && (m_count > 0))
            {
                m_userLengths = new double[m_count] ;
                for ( int i = 0 ; i < m_count ; ++i )
                {
                    m_userLengths[i] = wxdashes[i] * dashUnit ;

                    if ( i % 2 == 1 && m_userLengths[i] < dashUnit + 2.0 )
                        m_userLengths[i] = dashUnit + 2.0 ;
                    else if ( i % 2 == 0 && m_userLengths[i] < dashUnit )
                        m_userLengths[i] = dashUnit ;
                }
            }
            m_lengths = m_userLengths ;
        }
        break;

    case wxPENSTYLE_STIPPLE :
    case wxPENSTYLE_STIPPLE_MASK :
    case wxPENSTYLE_STIPPLE_MASK_OPAQUE :
        {
            wxBitmap stipple = info.GetStipple();
            InitStipple(&stipple);
        }
        break;

    default :
        if ( info.GetStyle() >= wxPENSTYLE_FIRST_HATCH
            && info.GetStyle() <= wxPENSTYLE_LAST_HATCH )
        {
            wxASSERT_MSG( info.GetColour().IsSolid(),
                          "Pen with non-solid colour is not supported." );
            InitHatch(static_cast<wxHatchStyle>(info.GetStyle()));
        }
        break;
    }

    switch ( info.GetGradientType() )
    {
    case wxGRADIENT_NONE:
        break;

    case wxGRADIENT_LINEAR:
        CreateLinearGradientPattern(info.GetX1(), info.GetY1(),
                                    info.GetX2(), info.GetY2(),
                                    info.GetStops(),
                                    info.GetMatrix());
        break;

    case wxGRADIENT_RADIAL:
        CreateRadialGradientPattern(info.GetStartX(), info.GetStartY(),
                                    info.GetEndX(), info.GetEndY(),
                                    info.GetRadius(),
                                    info.GetStops(),
                                    info.GetMatrix());
        break;
    }
}

void wxCairoPenData::Apply( wxGraphicsContext* context )
{
    wxCairoPenBrushBaseData::Apply(context);

    cairo_t * ctext = (cairo_t*) context->GetNativeContext();
    double width = m_width;
    if (width <= 0)
    {
        double x = context->GetContentScaleFactor(), y = x;
        cairo_user_to_device_distance(ctext, &x, &y);
        width = 1 / wxMin(fabs(x), fabs(y));
    }
    cairo_set_line_width(ctext, width);
    cairo_set_line_cap(ctext,m_cap);
    cairo_set_line_join(ctext,m_join);
    cairo_set_dash(ctext, m_lengths, m_count, 0);
}

//-----------------------------------------------------------------------------
// wxCairoBrushData implementation
//-----------------------------------------------------------------------------

wxCairoBrushData::wxCairoBrushData( wxGraphicsRenderer* renderer )
    : wxCairoPenBrushBaseData(renderer, wxColour(), true /* transparent */)
{
    Init();
}

wxCairoBrushData::wxCairoBrushData( wxGraphicsRenderer* renderer,
                                    const wxBrush &brush )
    : wxCairoPenBrushBaseData(renderer, brush.GetColour(), brush.IsTransparent())
{
    Init();

    switch ( brush.GetStyle() )
    {
        case wxBRUSHSTYLE_STIPPLE:
        case wxBRUSHSTYLE_STIPPLE_MASK:
        case wxBRUSHSTYLE_STIPPLE_MASK_OPAQUE:
            InitStipple(brush.GetStipple());
            break;

        case wxBRUSHSTYLE_SOLID:
            // Non-RGB colours, which may e.g. appear under wxOSX for wxColours
            // with NSColor backend, are not supported by Cairo and have to be
            // handled in a special way.
            if ( !brush.GetColour().IsSolid() )
            {
#if defined(__WXOSX_COCOA__)
                // Under wxOSX, non-solid NSColors are actually represented
                // by pattern images and therefore a wxBrush with non-solid
                // colour and wxBRUSHSTYLE_SOLID style can be converted
                // to a stiple (a surface pattern).

                // Create a stiple bitmap from NSColor's pattern image
                wxBitmap bmp(brush.GetColour().OSXGetNSPatternImage());
                InitStipple(&bmp);
#else
                wxFAIL_MSG( "Brush with non-solid colour is not supported." );
#endif
            }
            break;

        default:
            if ( brush.IsHatch() )
            {
                wxASSERT_MSG( brush.GetColour().IsSolid(),
                              "Brush with non-solid colour is not supported." );
                InitHatch(static_cast<wxHatchStyle>(brush.GetStyle()));
            }
            break;
    }
}

void wxCairoBrushData::Init()
{
    m_pattern = NULL;
    m_bmpdata = NULL;
}

//-----------------------------------------------------------------------------
// wxCairoFontData implementation
//-----------------------------------------------------------------------------

void wxCairoFontData::InitColour(const wxColour& col)
{
    m_red = col.Red()/255.0;
    m_green = col.Green()/255.0;
    m_blue = col.Blue()/255.0;
    m_alpha = col.Alpha()/255.0;
}

void
wxCairoFontData::InitFontComponents(const wxString& facename,
                                    cairo_font_slant_t slant,
                                    cairo_font_weight_t weight)
{
    m_fontName = facename.mb_str(wxConvUTF8);
    m_slant = slant;
    m_weight = weight;
}

wxCairoFontData::wxCairoFontData( wxGraphicsRenderer* renderer, const wxFont &font,
                                  const wxRealPoint& dpi, const wxColour& col )
    : wxGraphicsObjectRefData(renderer)
#ifdef __WXGTK__
    , m_wxfont(font)
#endif
{
    InitColour(col);

#ifdef __WXMSW__
    // Font is 72 DPI, screen is 96-based, and font needs a correction
    // for screens with higher DPI (similar to gdi+ and d2d renderers).
    m_size = !dpi.y
    ? double(font.GetPixelSize().GetHeight())
    : (font.GetFractionalPointSize() * dpi.y / 72);
#else
    // On macOS, font and screen both are 72 DPI, and macOS font size does
    // not need a correction for retina displays.
    // GTK does not use m_size, but initialize it anyway and mark dpi as unused
    // so we don't get any compiler warnings.
    wxUnusedVar(dpi);
    m_size = font.GetFractionalPointSize() * wxDisplay::GetStdPPIValue() / 72;
#ifdef __WXMAC__
    m_font = cairo_quartz_font_face_create_for_cgfont(font.OSXGetCGFont());
#endif
#endif

    InitFontComponents
    (
        font.GetFaceName(),
        font.GetStyle() == wxFONTSTYLE_ITALIC ? CAIRO_FONT_SLANT_ITALIC
                                              : CAIRO_FONT_SLANT_NORMAL,
        font.GetWeight() == wxFONTWEIGHT_BOLD ? CAIRO_FONT_WEIGHT_BOLD
                                              : CAIRO_FONT_WEIGHT_NORMAL
    );
}

wxCairoFontData::wxCairoFontData(wxGraphicsRenderer* renderer,
                                 double sizeInPixels,
                                 const wxString& facename,
                                 int flags,
                                 const wxColour& col) :
    wxGraphicsObjectRefData(renderer)
#ifdef __WXGTK__
    , m_wxfont(wxFontInfo(wxSize(int(sizeInPixels), int(sizeInPixels)))
                .AllFlags(flags).FaceName(facename))
#endif
{
    InitColour(col);

    // Resolution for Cairo image surfaces is 72 DPI meaning that the sizes in
    // points and pixels are identical, so we can just pass the size in pixels
    // directly to cairo_set_font_size().
    m_size = sizeInPixels;

#if defined(__WXMAC__)
    m_font = NULL;
#endif

    // There is no need to set m_underlined under wxGTK in this case, it can
    // only be used if m_font != NULL.

    InitFontComponents
    (
        facename,
        flags & wxFONTFLAG_ITALIC ? CAIRO_FONT_SLANT_ITALIC
                                  : CAIRO_FONT_SLANT_NORMAL,
        flags & wxFONTFLAG_BOLD ? CAIRO_FONT_WEIGHT_BOLD
                                : CAIRO_FONT_WEIGHT_NORMAL
    );
}

wxCairoFontData::~wxCairoFontData()
{
#ifdef __WXMAC__
    if ( m_font )
        cairo_font_face_destroy( m_font );
#endif
}

void wxCairoFontData::Apply( wxGraphicsContext* context )
{
    cairo_t * ctext = (cairo_t*) context->GetNativeContext();
    cairo_set_source_rgba(ctext,m_red,m_green, m_blue,m_alpha);
#ifdef __WXGTK__
    if (m_wxfont.IsOk())
    {
        // Nothing to do, the caller uses Pango layout functions to do
        // everything.
        return;
    }
#elif defined(__WXMAC__)
    if ( m_font )
    {
        cairo_set_font_face(ctext, m_font);
        cairo_set_font_size(ctext, m_size );
        return;
    }
#endif

    // If we get here, we must be on a platform without native font support or
    // we're using toy Cairo API even under wxGTK/wxMac.
    cairo_select_font_face(ctext, m_fontName, m_slant, m_weight );
    cairo_set_font_size(ctext, m_size );
}

//-----------------------------------------------------------------------------
// wxCairoPathData implementation
//-----------------------------------------------------------------------------

wxCairoPathData::wxCairoPathData( wxGraphicsRenderer* renderer, cairo_t* pathcontext)
    : wxGraphicsPathData(renderer)
{
    if (pathcontext)
    {
        m_pathContext = pathcontext;
    }
    else
    {
        cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,1,1);
        m_pathContext = cairo_create(surface);
        cairo_surface_destroy (surface);
    }
}

wxCairoPathData::~wxCairoPathData()
{
    cairo_destroy(m_pathContext);
}

wxGraphicsObjectRefData *wxCairoPathData::Clone() const
{
    cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,1,1);
    cairo_t* pathcontext = cairo_create(surface);
    cairo_surface_destroy (surface);

    cairo_path_t* path = cairo_copy_path(m_pathContext);
    cairo_append_path(pathcontext, path);
    cairo_path_destroy(path);
    return new wxCairoPathData( GetRenderer() ,pathcontext);
}


void* wxCairoPathData::GetNativePath() const
{
    return cairo_copy_path(m_pathContext) ;
}

void wxCairoPathData::UnGetNativePath(void *p) const
{
    cairo_path_destroy((cairo_path_t*)p);
}

//
// The Primitives
//

void wxCairoPathData::MoveToPoint( wxDouble x , wxDouble y )
{
    cairo_move_to(m_pathContext,x,y);
}

void wxCairoPathData::AddLineToPoint( wxDouble x , wxDouble y )
{
    cairo_line_to(m_pathContext,x,y);
}

void wxCairoPathData::AddPath( const wxGraphicsPathData* path )
{
    cairo_path_t* p = (cairo_path_t*)path->GetNativePath();
    cairo_append_path(m_pathContext, p);
    UnGetNativePath(p);
}

void wxCairoPathData::CloseSubpath()
{
    cairo_close_path(m_pathContext);
}

void wxCairoPathData::AddCurveToPoint( wxDouble cx1, wxDouble cy1, wxDouble cx2, wxDouble cy2, wxDouble x, wxDouble y )
{
    cairo_curve_to(m_pathContext,cx1,cy1,cx2,cy2,x,y);
}

// gets the last point of the current path, (0,0) if not yet set
void wxCairoPathData::GetCurrentPoint( wxDouble* x, wxDouble* y) const
{
    double dx,dy;
    cairo_get_current_point(m_pathContext,&dx,&dy);
    if (x)
        *x = dx;
    if (y)
        *y = dy;
}

void wxCairoPathData::AddArc( wxDouble x, wxDouble y, wxDouble r, double startAngle, double endAngle, bool clockwise )
{
    // as clockwise means positive in our system (y pointing downwards)
    // TODO make this interpretation dependent of the
    // real device trans
    if ( clockwise||(endAngle-startAngle)>=2*M_PI)
        cairo_arc(m_pathContext,x,y,r,startAngle,endAngle);
    else
        cairo_arc_negative(m_pathContext,x,y,r,startAngle,endAngle);
}

// transforms each point of this path by the matrix
void wxCairoPathData::Transform( const wxGraphicsMatrixData* matrix )
{
    // as we don't have a true path object, we have to apply the inverse
    // matrix to the context
    cairo_matrix_t m = *((cairo_matrix_t*) matrix->GetNativeMatrix());
    cairo_matrix_invert( &m );
    cairo_transform(m_pathContext,&m);
}

// gets the bounding box enclosing all points (possibly including control points)
void wxCairoPathData::GetBox(wxDouble *x, wxDouble *y, wxDouble *w, wxDouble *h) const
{
    double x1,y1,x2,y2;

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    if ( cairo_version() >= CAIRO_VERSION_ENCODE(1, 6, 0) )
    {
        cairo_path_extents(m_pathContext, &x1, &y1, &x2, &y2);
    }
    else
#endif
    {
        cairo_stroke_extents(m_pathContext, &x1, &y1, &x2, &y2);
    }

    if ( x2 < x1 )
    {
        *x = x2;
        *w = x1-x2;
    }
    else
    {
        *x = x1;
        *w = x2-x1;
    }

    if( y2 < y1 )
    {
        *y = y2;
        *h = y1-y2;
    }
    else
    {
        *y = y1;
        *h = y2-y1;
    }
}

bool wxCairoPathData::Contains( wxDouble x, wxDouble y, wxPolygonFillMode fillStyle ) const
{
    cairo_set_fill_rule(m_pathContext,fillStyle==wxODDEVEN_RULE ? CAIRO_FILL_RULE_EVEN_ODD : CAIRO_FILL_RULE_WINDING);
    return cairo_in_fill( m_pathContext, x, y) != 0;
}

// Convenience functions

void wxCairoPathData::AddRectangle(wxDouble x, wxDouble y, wxDouble w, wxDouble h)
{
    cairo_rectangle(m_pathContext, x, y, w, h);
}

void wxCairoPathData::AddCircle(wxDouble x, wxDouble y, wxDouble r)
{
    cairo_move_to(m_pathContext, x+r, y);
    cairo_arc(m_pathContext, x, y, r, 0.0, 2*M_PI);
    cairo_close_path(m_pathContext);
}

void wxCairoPathData::AddEllipse(wxDouble x, wxDouble y, wxDouble w, wxDouble h)
{
    if (w <= 0 || h <= 0)
        return;

    cairo_move_to(m_pathContext, x+w, y+h/2.0);
    w /= 2.0;
    h /= 2.0;
    cairo_save(m_pathContext);
    cairo_translate(m_pathContext, x+w, y+h);
    cairo_scale(m_pathContext, w, h);
    cairo_arc(m_pathContext, 0.0, 0.0, 1.0, 0.0, 2*M_PI);
    cairo_restore(m_pathContext);
    cairo_close_path(m_pathContext);
}

//-----------------------------------------------------------------------------
// wxCairoMatrixData implementation
//-----------------------------------------------------------------------------

wxCairoMatrixData::wxCairoMatrixData(wxGraphicsRenderer* renderer, const cairo_matrix_t* matrix )
    : wxGraphicsMatrixData(renderer)
{
    if ( matrix )
        m_matrix = *matrix;
}

wxCairoMatrixData::~wxCairoMatrixData()
{
    // nothing to do
}

wxGraphicsObjectRefData *wxCairoMatrixData::Clone() const
{
    return new wxCairoMatrixData(GetRenderer(),&m_matrix);
}

// concatenates the matrix
void wxCairoMatrixData::Concat( const wxGraphicsMatrixData *t )
{
    // The parameter matrix (t) is the multiplicand.
    cairo_matrix_multiply(&m_matrix, (cairo_matrix_t*) t->GetNativeMatrix(), &m_matrix);
}

// sets the matrix to the respective values
void wxCairoMatrixData::Set(wxDouble a, wxDouble b, wxDouble c, wxDouble d,
                        wxDouble tx, wxDouble ty)
{
    cairo_matrix_init( &m_matrix, a, b, c, d, tx, ty);
}

// gets the component valuess of the matrix
void wxCairoMatrixData::Get(wxDouble* a, wxDouble* b,  wxDouble* c,
                            wxDouble* d, wxDouble* tx, wxDouble* ty) const
{
    if (a)  *a = m_matrix.xx;
    if (b)  *b = m_matrix.yx;
    if (c)  *c = m_matrix.xy;
    if (d)  *d = m_matrix.yy;
    if (tx) *tx= m_matrix.x0;
    if (ty) *ty= m_matrix.y0;
}

// makes this the inverse matrix
void wxCairoMatrixData::Invert()
{
    cairo_matrix_invert( &m_matrix );
}

// returns true if the elements of the transformation matrix are equal ?
bool wxCairoMatrixData::IsEqual( const wxGraphicsMatrixData* t) const
{
    const cairo_matrix_t* tm = (cairo_matrix_t*) t->GetNativeMatrix();
    return (
        m_matrix.xx == tm->xx &&
        m_matrix.yx == tm->yx &&
        m_matrix.xy == tm->xy &&
        m_matrix.yy == tm->yy &&
        m_matrix.x0 == tm->x0 &&
        m_matrix.y0 == tm->y0 ) ;
}

// return true if this is the identity matrix
bool wxCairoMatrixData::IsIdentity() const
{
    return ( m_matrix.xx == 1 && m_matrix.yy == 1 &&
        m_matrix.yx == 0 && m_matrix.xy == 0 && m_matrix.x0 == 0 && m_matrix.y0 == 0);
}

//
// transformation
//

// add the translation to this matrix
void wxCairoMatrixData::Translate( wxDouble dx , wxDouble dy )
{
    cairo_matrix_translate( &m_matrix, dx, dy) ;
}

// add the scale to this matrix
void wxCairoMatrixData::Scale( wxDouble xScale , wxDouble yScale )
{
    cairo_matrix_scale( &m_matrix, xScale, yScale) ;
}

// add the rotation to this matrix (radians)
void wxCairoMatrixData::Rotate( wxDouble angle )
{
    cairo_matrix_rotate( &m_matrix, angle) ;
}

//
// apply the transforms
//

// applies that matrix to the point
void wxCairoMatrixData::TransformPoint( wxDouble *x, wxDouble *y ) const
{
    double lx = *x, ly = *y ;
    cairo_matrix_transform_point( &m_matrix, &lx, &ly);
    *x = lx;
    *y = ly;
}

// applies the matrix except for translations
void wxCairoMatrixData::TransformDistance( wxDouble *dx, wxDouble *dy ) const
{
    double lx = *dx, ly = *dy ;
    cairo_matrix_transform_distance( &m_matrix, &lx, &ly);
    *dx = lx;
    *dy = ly;
}

// returns the native representation
void * wxCairoMatrixData::GetNativeMatrix() const
{
    return const_cast<cairo_matrix_t*>(&m_matrix);
}

// ----------------------------------------------------------------------------
// wxCairoBitmap implementation
// ----------------------------------------------------------------------------

int wxCairoBitmapData::InitBuffer(int width, int height, cairo_format_t format)
{
    wxUnusedVar(format); // Only really unused with Cairo < 1.6.

    // Determine the stride: use cairo_format_stride_for_width() if available
    // but fall back to 4*width for the earlier versions as this is what that
    // function always returns, even in latest Cairo, anyhow.
    int stride;
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    if ( cairo_version() >= CAIRO_VERSION_ENCODE(1, 6, 0) )
    {
        stride = cairo_format_stride_for_width(format, width);

        // All our code would totally break if stride were not a multiple of 4
        // so ensure this is the case.
        if ( stride % 4 )
        {
            wxFAIL_MSG("Unexpected Cairo image surface stride.");

            stride += 4 - stride % 4;
        }
    }
    else
#endif
        stride = 4*width;

    m_width = width;
    m_height = height;
    m_buffer = new unsigned char[height*stride];

    return stride;
}

void wxCairoBitmapData::InitSurface(cairo_format_t format, int stride)
{
    m_surface = cairo_image_surface_create_for_data(
                            m_buffer, format, m_width, m_height, stride);
    m_pattern = cairo_pattern_create_for_surface(m_surface);
}

wxCairoBitmapData::wxCairoBitmapData( wxGraphicsRenderer* renderer, cairo_surface_t* bitmap ) :
    wxGraphicsBitmapData(renderer)
{
    m_surface = bitmap;
    m_pattern = cairo_pattern_create_for_surface(m_surface);

    m_width = cairo_image_surface_get_width(m_surface);
    m_height = cairo_image_surface_get_height(m_surface);
    m_buffer = NULL;
}

wxCairoBitmapData::wxCairoBitmapData( wxGraphicsRenderer* renderer, const wxBitmap& bmp )
    : wxGraphicsBitmapData(renderer)
{
    m_surface = NULL;
    m_pattern = NULL;
    m_buffer = NULL;
    wxCHECK_RET( bmp.IsOk(), wxT("Invalid bitmap in wxCairoContext::DrawBitmap"));

#ifdef wxHAS_RAW_BITMAP
    // Create a surface object and copy the bitmap pixel data to it.  if the
    // image has alpha (or a mask represented as alpha) then we'll use a
    // different format and iterator than if it doesn't...
    const bool isSrcBpp32 = bmp.GetDepth() == 32;

#if defined(__WXMSW__) || defined(__WXOSX__)
    // Under MSW and OSX we can have 32 bpp xRGB bitmaps (without alpha).
    const bool hasAlpha = bmp.HasAlpha();
#endif

    cairo_format_t bufferFormat =
    // Under MSW and OSX we can have 32 bpp xRGB bitmaps (without alpha).
#if defined(__WXMSW__) || defined(__WXOSX__)
        (isSrcBpp32 && hasAlpha) || bmp.GetMask() != NULL
#else
        isSrcBpp32 || bmp.GetMask() != NULL
#endif
        ? CAIRO_FORMAT_ARGB32 : CAIRO_FORMAT_RGB24;

    int stride = InitBuffer(bmp.GetWidth(), bmp.GetHeight(), bufferFormat);

    wxBitmap bmpSource = bmp;  // we need a non-const instance
    wxUint32* data = (wxUint32*)m_buffer;

    if ( isSrcBpp32 )
    {
        {
            // use the bitmap's alpha
            wxAlphaPixelData pixData(bmpSource);
            wxCHECK_RET( pixData, wxT("Failed to gain raw access to bitmap data."));

            wxAlphaPixelData::Iterator p(pixData);
            for (int y=0; y < pixData.GetHeight(); y++)
            {
                wxAlphaPixelData::Iterator rowStart = p;
                wxUint32* const rowStartDst = data;
                for (int x=0; x < pixData.GetWidth(); x++)
                {
                    // Each pixel in CAIRO_FORMAT_ARGB32 is a 32-bit quantity,
                    // with alpha in the upper 8 bits, then red, then green, then
                    // blue. The 32-bit quantities are stored native-endian.
                    // Pre-multiplied alpha is used.
#if defined (__WXMSW__) || defined(__WXOSX__)
                    unsigned char alpha = hasAlpha ? p.Alpha() : wxALPHA_OPAQUE;
                    // MSW and OSX bitmap pixel bits are already premultiplied.
                    *data = (alpha << 24 | p.Red() << 16 | p.Green() << 8 | p.Blue());
#else // !__WXMSW__ , !__WXOSX__
                    // We always have alpha, but we need to premultiply it.
                    unsigned char alpha = p.Alpha();
                    if (alpha == wxALPHA_TRANSPARENT)
                        *data = 0;
                    else
                        *data = (alpha << 24
                            | Premultiply(alpha, p.Red()) << 16
                            | Premultiply(alpha, p.Green()) << 8
                            | Premultiply(alpha, p.Blue()));
#endif // __WXMSW__, __WXOSX__ / !__WXMSW__, !__WXOSX__
                    ++data;
                    ++p;
                }

                data = rowStartDst + stride / 4;
                p = rowStart;
                p.OffsetY(pixData, 1);
            }
        }

#if defined(__WXMSW__) || defined(__WXOSX__)
        // Using wxAlphaPixelData sets (on wxMSW and wxOSX) the internal
        // "has alpha" flag but we want to leave it unchanged, so we need
        // to reset "has alpha" flag back.
        // (wxBitmap::UseAlpha() is used only on wxMSW and wxOSX.)
        bmpSource.UseAlpha(hasAlpha);
#endif // __WXMSW__ || __WXOSX__
    }
    else  // no alpha
    {
        wxNativePixelData pixData(bmpSource);
        wxCHECK_RET( pixData, wxT("Failed to gain raw access to bitmap data."));

        wxNativePixelData::Iterator p(pixData);
        for (int y=0; y < pixData.GetHeight(); y++)
        {
            wxNativePixelData::Iterator rowStart = p;
            wxUint32* const rowStartDst = data;
            for (int x=0; x < pixData.GetWidth(); x++)
            {
                // Each pixel in CAIRO_FORMAT_RGB24 is a 32-bit quantity, with
                // the upper 8 bits unused. Red, Green, and Blue are stored in
                // the remaining 24 bits in that order.  The 32-bit quantities
                // are stored native-endian.
                *data = (wxALPHA_OPAQUE << 24 | p.Red() << 16 | p.Green() << 8 | p.Blue() );
                ++data;
                ++p;
            }

            data = rowStartDst + stride / 4;
            p = rowStart;
            p.OffsetY(pixData, 1);
        }
    }

    // if there is a mask, set the alpha bytes in the target buffer to
    // fully transparent or retain original value
    if (bmp.GetMask() != NULL)
    {
        wxBitmap bmpMask = bmp.GetMask()->GetBitmap();
        data = (wxUint32*)m_buffer;
        wxNativePixelData pixData(bmpMask);
        wxCHECK_RET( pixData, wxT("Failed to gain raw access to mask data."));

        wxNativePixelData::Iterator p(pixData);
        for (int y=0; y < pixData.GetHeight(); y++)
        {
            wxNativePixelData::Iterator rowStart = p;
            wxUint32* const rowStartDst = data;
            for (int x=0; x < pixData.GetWidth(); x++)
            {
                if (p.Red()+p.Green()+p.Blue() == 0)
                    *data = 0;

                ++data;
                ++p;
            }

            data = rowStartDst + stride / 4;
            p = rowStart;
            p.OffsetY(pixData, 1);
        }
    }

    InitSurface(bufferFormat, stride);
#endif // wxHAS_RAW_BITMAP
}

#if wxUSE_IMAGE

wxCairoBitmapData::wxCairoBitmapData(wxGraphicsRenderer* renderer,
                                     const wxImage& image)
    : wxGraphicsBitmapData(renderer)
{
    const cairo_format_t bufferFormat = image.HasAlpha() || image.HasMask()
                                            ? CAIRO_FORMAT_ARGB32
                                            : CAIRO_FORMAT_RGB24;

    int stride = InitBuffer(image.GetWidth(), image.GetHeight(), bufferFormat);

    // Copy wxImage data into the buffer. Notice that we work with wxUint32
    // values and not bytes becase Cairo always works with buffers in native
    // endianness.
    wxUint32* dst = reinterpret_cast<wxUint32*>(m_buffer);
    const unsigned char* src = image.GetData();

    if ( bufferFormat == CAIRO_FORMAT_ARGB32 )
    {
        const unsigned char* alpha = image.GetAlpha();

        for ( int y = 0; y < m_height; y++ )
        {
            wxUint32* const rowStartDst = dst;

            for ( int x = 0; x < m_width; x++ )
            {
                const unsigned char a = alpha ? *alpha : wxALPHA_OPAQUE;

                *dst++ = a                    << 24 |
                         ((a * src[0]) / 255) << 16 |
                         ((a * src[1]) / 255) <<  8 |
                         ((a * src[2]) / 255);
                src += 3;

                if ( alpha )
                    alpha++;
            }

            dst = rowStartDst + stride / 4;
        }
    }
    else // RGB
    {
        for ( int y = 0; y < m_height; y++ )
        {
            wxUint32* const rowStartDst = dst;

            for ( int x = 0; x < m_width; x++ )
            {
                *dst++ = src[0] << 16 |
                         src[1] <<  8 |
                         src[2];
                src += 3;
            }

            dst = rowStartDst + stride / 4;
        }
    }

    // if there is a mask, set the alpha bytes in the target buffer to
    // fully transparent or retain original value
    if ( image.HasMask() )
    {
        unsigned char mr = image.GetMaskRed();
        unsigned char mg = image.GetMaskGreen();
        unsigned char mb = image.GetMaskBlue();

        dst = reinterpret_cast<wxUint32*>(m_buffer);
        src = image.GetData();

        if ( bufferFormat == CAIRO_FORMAT_ARGB32 )
        {
            for ( int y = 0; y < m_height; y++ )
            {
                wxUint32* const rowStartDst = dst;

                for ( int x = 0; x < m_width; x++ )
                {
                    if ( src[0] == mr && src[1] == mg && src[2] == mb )
                        *dst = 0;

                    dst++;
                    src += 3;
                }

                dst = rowStartDst + stride / 4;
            }
        }
    }

    InitSurface(bufferFormat, stride);
}

wxImage wxCairoBitmapData::ConvertToImage() const
{
    wxImage image(m_width, m_height, false /* don't clear */);

    // Get the surface type and format.
    wxCHECK_MSG( cairo_surface_get_type(m_surface) == CAIRO_SURFACE_TYPE_IMAGE,
                 wxNullImage,
                 wxS("Can't convert non-image surface to image.") );

    switch ( cairo_image_surface_get_format(m_surface) )
    {
        case CAIRO_FORMAT_ARGB32:
            image.SetAlpha();
            break;

        case CAIRO_FORMAT_RGB24:
            // Nothing to do, we don't use alpha by default.
            break;

        case CAIRO_FORMAT_A8:
        case CAIRO_FORMAT_A1:
            wxFAIL_MSG(wxS("Unsupported Cairo image surface type."));
            return wxNullImage;

        default:
            wxFAIL_MSG(wxS("Unknown Cairo image surface type."));
            return wxNullImage;
    }

    // Prepare for copying data.
    cairo_surface_flush(m_surface);
    const wxUint32* src = (wxUint32*)cairo_image_surface_get_data(m_surface);
    wxCHECK_MSG( src, wxNullImage, wxS("Failed to get Cairo surface data.") );

    int stride = cairo_image_surface_get_stride(m_surface);
    wxCHECK_MSG( stride > 0, wxNullImage,
                 wxS("Failed to get Cairo surface stride.") );

    // As we work with wxUint32 pointers and not char ones, we need to adjust
    // the stride accordingly. This should be lossless as the stride must be a
    // multiple of pixel size.
    wxASSERT_MSG( !(stride % sizeof(wxUint32)), wxS("Unexpected stride.") );
    stride /= sizeof(wxUint32);

    unsigned char* dst = image.GetData();
    unsigned char *alpha = image.GetAlpha();
    if ( alpha )
    {
        // We need to also copy alpha and undo the pre-multiplication as Cairo
        // stores pre-multiplied values in this format while wxImage does not.
        for ( int y = 0; y < m_height; y++ )
        {
            const wxUint32* const rowStart = src;
            for ( int x = 0; x < m_width; x++ )
            {
                const wxUint32 argb = *src++;

                const unsigned char a = argb >> 24;
                *alpha++ = a;

                // Copy the RGB data undoing the pre-multiplication.
                *dst++ = Unpremultiply(a, argb >> 16);
                *dst++ = Unpremultiply(a, argb >>  8);
                *dst++ = Unpremultiply(a, argb);
            }

            src = rowStart + stride;
        }
    }
    else // RGB
    {
        // Things are pretty simple in this case, just copy RGB bytes.
        for ( int y = 0; y < m_height; y++ )
        {
            const wxUint32* const rowStart = src;
            for ( int x = 0; x < m_width; x++ )
            {
                const wxUint32 argb = *src++;

                *dst++ = (argb & 0x00ff0000) >> 16;
                *dst++ = (argb & 0x0000ff00) >>  8;
                *dst++ = (argb & 0x000000ff);
            }

            src = rowStart + stride;
        }
    }

    return image;
}

#endif // wxUSE_IMAGE

wxCairoBitmapData::~wxCairoBitmapData()
{
    if (m_pattern)
        cairo_pattern_destroy(m_pattern);

    if (m_surface)
        cairo_surface_destroy(m_surface);

    delete [] m_buffer;
}

//-----------------------------------------------------------------------------
// wxCairoContext implementation
//-----------------------------------------------------------------------------

class wxCairoOffsetHelper
{
public :
    wxCairoOffsetHelper(cairo_t* ctx, double scaleFactor, bool offset)
    {
        m_ctx = ctx;
        m_offset = 0;
        if (offset)
        {
             double x = scaleFactor, y = x;
             cairo_user_to_device_distance(ctx, &x, &y);
             m_offset = 0.5 / wxMin(fabs(x), fabs(y));
             cairo_translate(m_ctx, m_offset, m_offset);
        }
    }
    ~wxCairoOffsetHelper( )
    {
        if (m_offset > 0)
            cairo_translate(m_ctx, -m_offset, -m_offset);
    }
private:
    cairo_t* m_ctx;
    double m_offset;
} ;

#if wxUSE_PRINTING_ARCHITECTURE
wxCairoContext::wxCairoContext( wxGraphicsRenderer* renderer, const wxPrinterDC& dc )
: wxGraphicsContext(renderer)
{
#ifdef __WXMSW__
    // wxMSW contexts always use MM_ANISOTROPIC, which messes up
    // text rendering when printing using Cairo. Switch it to MM_TEXT
    // map mode to avoid this problem.
    HDC hdc = (HDC)dc.GetHDC();
    m_mswStateSavedDC = ::SaveDC(hdc);
    ::SetMapMode(hdc, MM_TEXT);
    m_mswSurface = cairo_win32_printing_surface_create(hdc);
    Init( cairo_create(m_mswSurface) );

    wxSize sz = dc.GetSize();
    m_width = sz.x;
    m_height = sz.y;
    // Transfer transformation settings from source DC to Cairo context on our own.
    // Since we switched from MM_ANISOTROPIC to MM_TEXT mapping mode
    // we have to apply rescaled DC's device origin to Cairo context.
    ApplyTransformFromDC(dc, Apply_scaled_dev_origin);
#elif defined(__WXGTK20__)
    const wxDCImpl *impl = dc.GetImpl();
    cairo_t* cr = static_cast<cairo_t*>(impl->GetCairoContext());
    Init(cr ? cairo_reference(cr) : NULL);

    wxSize sz = dc.GetSize();
    m_width = sz.x;
    m_height = sz.y;

    // Transfer transformation settings from source DC to Cairo context.
    ApplyTransformFromDC(dc);
#else
    #warning "Constructing wxCairoContext from wxPrinterDC not implemented."
    wxUnusedVar(dc);
#endif
}
#endif

wxCairoContext::wxCairoContext( wxGraphicsRenderer* renderer, const wxWindowDC& dc )
: wxGraphicsContext(renderer, dc.GetWindow())
{
    int width, height;
    dc.GetSize( &width, &height );
    m_width = width;
    m_height = height;

    EnableOffset();

#ifdef __WXMSW__
    HDC hdc = (HDC)dc.GetHDC();
    m_mswStateSavedDC = ::SaveDC(hdc);
    m_mswSurface = cairo_win32_surface_create(hdc);
    Init( cairo_create(m_mswSurface) );
#endif

#ifdef __WXGTK3__
    cairo_t* cr = static_cast<cairo_t*>(dc.GetImpl()->GetCairoContext());
    Init(cr ? cairo_reference(cr) : NULL);
#elif defined __WXGTK20__
    const wxGTKDCImpl* impldc = static_cast<const wxGTKDCImpl*>(dc.GetImpl());
    Init( gdk_cairo_create( impldc->GetGDKWindow() ) );

    // Transfer transformation settings from source DC to Cairo context on our own.
    ApplyTransformFromDC(dc);
#endif // __WXGTK3__ || __WXGTK20__

#ifdef __WXX11__
    cairo_t* cr = static_cast<cairo_t*>(dc.GetImpl()->GetCairoContext());
    if ( cr )
        Init(cairo_reference(cr));
#elif defined(__WXMAC__)
    CGContextRef cgcontext = (CGContextRef)dc.GetWindow()->MacGetCGContextRef();
    cairo_surface_t* surface = cairo_quartz_surface_create_for_cg_context(cgcontext, width, height);
    Init( cairo_create( surface ) );
    cairo_surface_destroy( surface );
#endif

#ifdef __WXQT__
    m_qtPainter = static_cast<QPainter*>(dc.GetHandle());
    // create a internal buffer (fallback if cairo_qt_surface is missing)
    m_qtImage = new QImage(width, height, QImage::Format_ARGB32_Premultiplied);
    // clear the buffer to be painted over the current contents
    m_qtImage->fill(Qt::transparent);
    m_qtSurface = cairo_image_surface_create_for_data(m_qtImage->bits(),
                                                      CAIRO_FORMAT_ARGB32,
                                                      width, height,
                                                      m_qtImage->bytesPerLine());
    Init( cairo_create( m_qtSurface ) );
#endif
}

wxCairoContext::wxCairoContext( wxGraphicsRenderer* renderer, const wxMemoryDC& dc )
: wxGraphicsContext(renderer)
{
    int width, height;
    dc.GetSize( &width, &height );
    m_width = width;
    m_height = height;

    SetContentScaleFactor(dc.GetContentScaleFactor());

#ifdef __WXMSW__
    wxBitmap bmp = dc.GetSelectedBitmap();
    wxASSERT_MSG(bmp.IsOk(),
                 wxS("Should select a bitmap before creating wxCairoContext"));

    HDC hdc = (HDC)dc.GetHDC();
    m_mswStateSavedDC = ::SaveDC(hdc);
    bool hasBitmap = false;      // To signal that Cairo context is created
                                 // from raw bitmap and not from DC.
    bool adjustTransformFromDC = false; // To signal that we have to transfer
                                        // transformation settings from source
                                        // wxDC to Cairo context on our own.

    // Prior to 1.15.4 creating surface from HDC could be done only with
    // cairo_win32_surface_create() supporting only 24 bpp RGB surface.
    // So, in case of a 32 bpp ARGB bitmap, it was necessary to create
    // a 32 bpp surface directly from bitmap data with cairo_image_surface_create_for_data().
    // New API cairo_win32_surface_create_with_format() introduced in 1.15.4
    // supports creating also 32 bpp ARGB surface so this function
    // can be used to create a surface from both RGB and ARGB bitmaps.
    // For 0RGB bitmaps, surface still has to be created in the old way, from
    // bitmap data.
    if ( bmp.GetDepth() == 32 &&
         (cairo_version() < CAIRO_VERSION_ENCODE(1, 15, 4) || !bmp.HasAlpha()) )
    {
#if wxUSE_WXDIB
        // We need to convert the currently selected bitmap to a DIB
        // because:
        // 1. We need to correct alpha values if bitmap
        //    doesn't contain real ARGB data.
        // 2. We need to pass location of its bit values to Cairo function.

        // We need to temporarily deselect this bitmap
        // from the memory DC before modifying it.
        const_cast<wxMemoryDC&>(dc).SelectObject(wxNullBitmap);

        bmp.ConvertToDIB(); // Does nothing if already a DIB.

        if ( !bmp.HasAlpha() )
        {
            // Initialize alpha channel, even if we don't have any alpha yet,
            // we should have correct (opaque) alpha values in it for Cairo
            // functions to work correctly.
            {
                wxAlphaPixelData data(bmp);
                if (data)
                {
                    wxAlphaPixelData::Iterator p(data);
                    for (int y = 0; y < data.GetHeight(); y++)
                    {
                        wxAlphaPixelData::Iterator rowStart = p;

                        for (int x = 0; x < data.GetWidth(); x++)
                        {
                            p.Alpha() = wxALPHA_OPAQUE;
                            ++p;
                        }

                        p = rowStart;
                        p.OffsetY(data, 1);
                    }
                }
            } // End of block modifying the bitmap.

            // Using wxAlphaPixelData sets the internal "has alpha" flag but we
            // don't really have any alpha yet, so reset it back for now.
            bmp.ResetAlpha();
        }

        // Undo SelectObject() at the beginning of this block.
        const_cast<wxMemoryDC&>(dc).SelectObjectAsSource(bmp);
#endif // wxUSE_WXDIB

        // We need to pass a pointer to the location
        // of the bit values to Cairo function.
        BITMAP info;
        if ( ::GetObject(bmp.GetHBITMAP(), sizeof(info), &info) == sizeof(info) )
        {
            if( info.bmBits )
            {
                m_mswSurface = cairo_image_surface_create_for_data((unsigned char*)info.bmBits,
                                               bmp.HasAlpha() ?
                                               CAIRO_FORMAT_ARGB32 : CAIRO_FORMAT_RGB24,
                                               info.bmWidth,
                                               info.bmHeight,
                                               info.bmWidthBytes);
                if ( cairo_surface_status(m_mswSurface) == CAIRO_STATUS_SUCCESS )
                {
                    hasBitmap = true;
                    // We will have to transfer transformation settings
                    // from source wxDC to Cairo context on our own
                    // because in this case they are not inherited
                    // implicitly from underlying DC.
                    adjustTransformFromDC = true;
                }
                else
                {
                    cairo_surface_destroy(m_mswSurface);
                    m_mswSurface = NULL;
                }
            }
            else
            {
                wxFAIL_MSG( wxS("Invalid bitmap") );
            }
        }
        else
        {
            wxLogLastError( wxS("wxCairoContext ctor - GetObject") );
        }
    }

    // Fallback if Cairo surface hasn't been created from bitmap data.
    if( !hasBitmap )
    {
        // When x- or y-coordinate of DC origin > 0 then surface
        // created from DC is not fully operational (for some Cairo
        // operations memory access violation errors occur - see Cairo
        // bug 96482) so in this case we would need to pass non-transformed
        // DC to Cairo and to apply original DC transformation to the Cairo
        // context operations on our own.
        // Bug 96482 was fixed in Cairo 1.15.12 so this workaround needs
        // to be applied only for older Cairo versions.
        if ( cairo_version() < CAIRO_VERSION_ENCODE(1, 15, 12) )
        {
            wxCoord orgX, orgY;
            dc.GetDeviceOrigin(&orgX, &orgY);
            if ( orgX > 0 || orgY > 0 )
            {
                ::SetViewportOrgEx(hdc, 0, 0, NULL);
                ::SetViewportExtEx(hdc, 1, 1, NULL);
                ::SetWindowOrgEx(hdc, 0, 0, NULL);
                ::SetWindowExtEx(hdc, 1, 1, NULL);
                adjustTransformFromDC = true;
            }
        }

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 15, 4)
        if ( cairo_version() >= CAIRO_VERSION_ENCODE(1, 15, 4) )
        {
            m_mswSurface = cairo_win32_surface_create_with_format(hdc,
                             bmp.HasAlpha() ? CAIRO_FORMAT_ARGB32 : CAIRO_FORMAT_RGB24);
        }
        else
#endif // Cairo >= 1.15.4
        {
            m_mswSurface = cairo_win32_surface_create(hdc);
        }
        wxASSERT_MSG( cairo_surface_status(m_mswSurface) == CAIRO_STATUS_SUCCESS,
                      wxS("wxCairoContext ctor - Error creating Cairo surface") );
    }

    Init( cairo_create(m_mswSurface) );
    if (hasBitmap)
    {
        // If we've created an image surface we need to flip the Y axis
        // so that all drawing will appear right side up.
        // We have to remember these operations as an internal transformation
        // which is not going to be exposed through e.g. GetTransform().
        cairo_matrix_init(&m_internalTransform, 1.0, 0.0, 0.0, -1.0, 0.0, height);
    }
    // Transfer transformation settings from source wxDC
    // to Cairo context on our own, if required.
    if ( adjustTransformFromDC )
        ApplyTransformFromDC(dc);
#endif // __WXMSW__

#ifdef __WXGTK3__
    cairo_t* cr = static_cast<cairo_t*>(dc.GetImpl()->GetCairoContext());
    Init(cr ? cairo_reference(cr) : NULL);
#elif defined __WXGTK20__
    const wxGTKDCImpl* impldc = static_cast<const wxGTKDCImpl*>(dc.GetImpl());
    Init( gdk_cairo_create( impldc->GetGDKWindow() ) );

    // Transfer transformation settings from source DC to Cairo context on our own.
    ApplyTransformFromDC(dc);
#endif // __WXGTK3__ || __WXGTK20__

#ifdef __WXX11__
    cairo_t* cr = static_cast<cairo_t*>(dc.GetImpl()->GetCairoContext());
    if ( cr )
        Init(cairo_reference(cr));
#endif

#ifdef __WXMAC__
    CGContextRef cgcontext = (CGContextRef)dc.GetGraphicsContext()->GetNativeContext();
    cairo_surface_t* surface = cairo_quartz_surface_create_for_cg_context(cgcontext, width, height);
    Init( cairo_create( surface ) );
    cairo_surface_destroy( surface );
#endif

#ifdef __WXQT__
    m_qtPainter = static_cast<QPainter*>(dc.GetHandle());
    // create a internal buffer (fallback if cairo_qt_surface is missing)
    m_qtImage = new QImage(width, height, QImage::Format_ARGB32_Premultiplied);
    // clear the buffer to be painted over the current contents
    m_qtImage->fill(Qt::transparent);
    m_qtSurface = cairo_image_surface_create_for_data(m_qtImage->bits(),
                                                      CAIRO_FORMAT_ARGB32,
                                                      width, height,
                                                      m_qtImage->bytesPerLine());
    Init( cairo_create( m_qtSurface ) );
#endif
}

#ifdef __WXGTK20__
wxCairoContext::wxCairoContext( wxGraphicsRenderer* renderer, GdkWindow *window )
: wxGraphicsContext(renderer)
{
    Init( gdk_cairo_create( window ) );

#ifdef __WXGTK3__
    m_width = gdk_window_get_width(window);
    m_height = gdk_window_get_height(window);
#else
    int width, height;
    gdk_drawable_get_size(window, &width, &height);
    m_width = width;
    m_height = height;
#endif
}
#endif

#ifdef __WXMSW__

#if wxUSE_ENH_METAFILE
wxCairoContext::wxCairoContext(wxGraphicsRenderer* renderer, const wxEnhMetaFileDC& dc)
: wxGraphicsContext(renderer)
{
    // wxMSW contexts always use MM_ANISOTROPIC, which messes up
    // text rendering when printing using Cairo. Switch it to MM_TEXT
    // map mode to avoid this problem.
    HDC hdc = (HDC)dc.GetHDC();
    m_mswStateSavedDC = ::SaveDC(hdc);
    ::SetMapMode(hdc, MM_TEXT);
    m_mswSurface = cairo_win32_printing_surface_create(hdc);
    Init( cairo_create(m_mswSurface) );

    wxSize sz = dc.GetSize();
    m_width = sz.x;
    m_height = sz.y;
    // Transfer transformation settings from source DC to Cairo context on our own.
    // Since we switched from MM_ANISOTROPIC to MM_TEXT mapping mode
    // we have to apply rescaled DC's device origin to Cairo context.
    ApplyTransformFromDC(dc, Apply_scaled_dev_origin);
}
#endif // wxUSE_ENH_METAFILE

wxCairoContext::wxCairoContext( wxGraphicsRenderer* renderer, HDC handle )
: wxGraphicsContext(renderer)
{
    m_mswStateSavedDC = ::SaveDC(handle);

    bool adjustTransformFromDC = false; // To signal that we have to transfer
                                        // transformation settings from source
                                        // wxDC to Cairo context on our own.
    cairo_matrix_t dcTransform;
    cairo_matrix_init_identity(&dcTransform);
    // When x- or y-coordinate of DC origin > 0 then surface
    // created from DC is not fully operational (for some Cairo
    // operations memory access violation errors occur - see Cairo
    // bug 96482) so in this case we would need to pass non-transformed
    // DC to Cairo and to apply original DC transformation to the Cairo
    // context operations on our own.
    if ( cairo_version() < CAIRO_VERSION_ENCODE(1, 15, 12) )
    {
        POINT devOrg;
        ::GetViewportOrgEx(handle, &devOrg);
        if ( devOrg.x > 0 || devOrg.y > 0 )
        {
            SIZE devExt;
            ::GetViewportExtEx(handle, &devExt);
            POINT logOrg;
            ::GetWindowOrgEx(handle, &logOrg);
            SIZE logExt;
            ::GetWindowExtEx(handle, &logExt);

            double sx = (double)devExt.cx / logExt.cx;
            double sy = (double)devExt.cy / logExt.cy;

            cairo_matrix_translate(&dcTransform, devOrg.x, devOrg.y);
            cairo_matrix_scale(&dcTransform, sx, sy);
            cairo_matrix_translate(&dcTransform, -logOrg.x, -logOrg.y);

            ::SetViewportOrgEx(handle, 0, 0, NULL);
            ::SetViewportExtEx(handle, 1, 1, NULL);
            ::SetWindowOrgEx(handle, 0, 0, NULL);
            ::SetWindowExtEx(handle, 1, 1, NULL);

            adjustTransformFromDC = true;
        }
    }
    m_mswSurface = cairo_win32_surface_create(handle);
    Init( cairo_create(m_mswSurface) );
    if ( adjustTransformFromDC )
    {
        cairo_matrix_multiply(&m_internalTransform,
                              &dcTransform, &m_internalTransform);
        cairo_set_matrix(m_context, &m_internalTransform);
    }

    m_width = 0;
    m_height = 0;
    // Try to determine DC size.
    if ( m_context )
    {
        RECT r;
        if( ::GetClipBox(handle, &r) != ERROR )
        {
            m_width = r.right - r.left;
            m_height = r.bottom - r.top;
        }
    }
}

wxCairoContext::wxCairoContext(wxGraphicsRenderer* renderer, HWND hWnd)
    : wxGraphicsContext(renderer)
    , m_mswWindowHDC(hWnd)
{
    // See remarks for wxWindowBase::GetContentScaleFactor
    double scaleY = ::GetDeviceCaps((HDC)m_mswWindowHDC, LOGPIXELSY) / 96.0f;
    SetContentScaleFactor(scaleY);

    m_mswStateSavedDC = 0;
    m_mswSurface = cairo_win32_surface_create((HDC)m_mswWindowHDC);

    Init(cairo_create(m_mswSurface));
    m_width = 0;
    m_height = 0;
    if ( m_context )
    {
        RECT r = wxGetWindowRect(hWnd);
        m_width = r.right - r.left;
        m_height = r.bottom - r.top;
    }
}
#endif // __WXMSW__


wxCairoContext::wxCairoContext( wxGraphicsRenderer* renderer, cairo_t *context )
: wxGraphicsContext(renderer)
{
#ifdef __WXQT__
    m_qtPainter = NULL;
    m_qtImage = NULL;
    m_qtSurface = NULL;
#endif
#ifdef __WXMSW__
    m_mswSurface = NULL;
    m_mswStateSavedDC = 0;
#endif // __WXMSW__
    Init( cairo_reference(context) );
    m_width = 0;
    m_height = 0;
}

wxCairoContext::wxCairoContext( wxGraphicsRenderer* renderer, wxWindow *window)
    : wxGraphicsContext(renderer, window)
#ifdef __WXMSW__
    , m_mswWindowHDC(GetHwndOf(window))
#endif
{
    EnableOffset();
#ifdef __WXGTK__
    // something along these lines (copied from dcclient)

    // Some controls don't have m_wxwindow - like wxStaticBox, but the user
    // code should still be able to create wxClientDCs for them, so we will
    // use the parent window here then.
    if (window->m_wxwindow == NULL)
    {
        window = window->GetParent();
    }

    wxASSERT_MSG( window->m_wxwindow, wxT("wxCairoContext needs a widget") );

    Init(gdk_cairo_create(window->GTKGetDrawingWindow()));

    wxSize sz = window->GetSize();
    m_width = sz.x;
    m_height = sz.y;
#endif

#ifdef __WXMSW__
    m_mswStateSavedDC = 0;
    m_mswSurface = cairo_win32_surface_create((HDC)m_mswWindowHDC);
    Init(cairo_create(m_mswSurface));

    wxSize sz = window->GetSize();
    m_width = sz.x;
    m_height = sz.y;
#endif //  __WXMSW__

#ifdef __WXQT__
    // direct m_qtSurface is not being used yet (this needs cairo qt surface)
#endif
}

wxCairoContext::wxCairoContext(wxGraphicsRenderer* renderer) :
    wxGraphicsContext(renderer)
{
#ifdef __WXQT__
    m_qtPainter = NULL;
    m_qtImage = NULL;
    m_qtSurface = NULL;
#endif
#ifdef __WXMSW__
    m_mswSurface = NULL;
    m_mswStateSavedDC = 0;
#endif // __WXMSW__
    Init(NULL);
    m_width = 0;
    m_height = 0;
}

wxCairoContext::~wxCairoContext()
{
    if ( m_context )
    {
        PopState();
        PopState();
        cairo_destroy(m_context);
    }
#ifdef __WXMSW__
    if ( m_mswSurface )
    {
        HDC hdc = cairo_win32_surface_get_dc(m_mswSurface);

        cairo_surface_destroy(m_mswSurface);

        if ( hdc && m_mswStateSavedDC != 0 )
            ::RestoreDC(hdc, m_mswStateSavedDC);
    }
#endif
#ifdef __WXQT__
    if ( m_qtPainter != NULL )
    {
        // draw the internal buffered image to the widget
        cairo_surface_flush(m_qtSurface);
        m_qtPainter->drawImage( 0,0, *m_qtImage );
        delete m_qtImage;
        cairo_surface_destroy( m_qtSurface );
    }
#endif

}

void wxCairoContext::Init(cairo_t *context)
{
#ifdef __WXGTK3__
    // Attempt to find the system font scaling parameter (e.g. "Fonts->Scaling
    // Factor" in Gnome Tweaks, "Force font DPI" in KDE System Settings or
    // GDK_DPI_SCALE environment variable).
    GdkScreen* screen = gdk_screen_get_default();
    m_fontScalingFactor = screen ? float(gdk_screen_get_resolution(screen) / 96.0) : 1.0f;
#endif

    m_context = context;
    if ( m_context )
    {
        // Store initial transformation settings
        // of the underlying source context.
        cairo_get_matrix(m_context, &m_internalTransform);

        PushState();
        PushState();
    }
    else
    {
        cairo_matrix_init_identity(&m_internalTransform);
    }
}

void wxCairoContext::ApplyTransformFromDC(const wxDC& dc, ApplyTransformMode mode)
{
    // Transfer transformation settings from source wxDC
    // to Cairo context and store them as an internal transformation
    // (which is not going to be exposed).
    double sx, sy;
    dc.GetUserScale(&sx, &sy);
    double lsx, lsy;
    dc.GetLogicalScale(&lsx, &lsy);
    sx *= lsx;
    sy *= lsy;

    wxPoint org = dc.GetDeviceOrigin();
    if ( mode == Apply_scaled_dev_origin )
        // This is used when mapping mode has been changed
        // under wxMSW from MM_ANISOTROPIC to MM_TEXT.
        cairo_matrix_translate(&m_internalTransform, org.x/sx, org.y/sy);
    else
        cairo_matrix_translate(&m_internalTransform, org.x, org.y);

    cairo_matrix_scale(&m_internalTransform, sx, sy);

    org = dc.GetLogicalOrigin();
    cairo_matrix_translate(&m_internalTransform, -org.x, -org.y);

    cairo_set_matrix(m_context, &m_internalTransform);
}

void wxCairoContext::Clip( const wxRegion& region )
{
    // Create a path with all the rectangles in the region
    wxGraphicsPath path = GetRenderer()->CreatePath();
    wxRegionIterator ri(region);
    while (ri)
    {
        path.AddRectangle(ri.GetX(), ri.GetY(), ri.GetW(), ri.GetH());
        ++ri;
    }

    // Put it in the context
    cairo_path_t* cp = (cairo_path_t*) path.GetNativePath() ;
    cairo_append_path(m_context, cp);

    // clip to that path
    cairo_clip(m_context);
    path.UnGetNativePath(cp);
}

void wxCairoContext::Clip( wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
    // Create a path with this rectangle
    wxGraphicsPath path = GetRenderer()->CreatePath();
    path.AddRectangle(x,y,w,h);

    // Put it in the context
    cairo_path_t* cp = (cairo_path_t*) path.GetNativePath() ;
    cairo_append_path(m_context, cp);

    // clip to that path
    cairo_clip(m_context);
    path.UnGetNativePath(cp);
}

void wxCairoContext::ResetClip()
{
    cairo_reset_clip(m_context);
}

void wxCairoContext::GetClipBox(wxDouble* x, wxDouble* y, wxDouble* w, wxDouble* h)
{
    double x1, y1, x2, y2;
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 4, 0)
    if ( cairo_version() >= CAIRO_VERSION_ENCODE(1, 4, 0) )
    {
        cairo_clip_extents(m_context, &x1, &y1, &x2, &y2);
        // Check if we have an empty clipping box.
        if ( x2 - x1 <= DBL_MIN || y2 - y1 <= DBL_MIN )
            x1 = x2 = y1 = y2 = 0.0;
    }
    else
#endif // Cairo >= 1.4
    {
        // There doesn't seem to be any way to get the clipping box with this
        // ancient version.
        x1 = x2 = y1 = y2 = 0.0;
    }

    if ( x )
        *x = x1;
    if ( y )
        *y = y1;
    if ( w )
        *w = x2 - x1;
    if ( h )
        *h = y2 - y1;
}

void wxCairoContext::StrokePath( const wxGraphicsPath& path )
{
    if ( !m_pen.IsNull() )
    {
        wxCairoOffsetHelper helper(m_context, GetContentScaleFactor(), ShouldOffset());
        cairo_path_t* cp = (cairo_path_t*) path.GetNativePath() ;
        cairo_append_path(m_context,cp);
        ((wxCairoPenData*)m_pen.GetRefData())->Apply(this);
        cairo_stroke(m_context);
        path.UnGetNativePath(cp);
    }
}

void wxCairoContext::FillPath( const wxGraphicsPath& path , wxPolygonFillMode fillStyle )
{
    if ( !m_brush.IsNull() )
    {
        wxCairoOffsetHelper helper(m_context, GetContentScaleFactor(), ShouldOffset());
        cairo_path_t* cp = (cairo_path_t*) path.GetNativePath() ;
        cairo_append_path(m_context,cp);
        ((wxCairoBrushData*)m_brush.GetRefData())->Apply(this);
        cairo_set_fill_rule(m_context,fillStyle==wxODDEVEN_RULE ? CAIRO_FILL_RULE_EVEN_ODD : CAIRO_FILL_RULE_WINDING);
        cairo_fill(m_context);
        path.UnGetNativePath(cp);
    }
}

void wxCairoContext::ClearRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
    cairo_save(m_context);
    cairo_set_operator(m_context, CAIRO_OPERATOR_CLEAR);
    cairo_rectangle (m_context, x, y, w, h);
    cairo_fill (m_context);
    cairo_restore(m_context);
}

void wxCairoContext::DrawRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
    if ( !m_brush.IsNull() )
    {
        ((wxCairoBrushData*)m_brush.GetRefData())->Apply(this);
        cairo_rectangle(m_context, x, y, w, h);
        cairo_fill(m_context);
    }
    if ( !m_pen.IsNull() )
    {
        wxCairoOffsetHelper helper(m_context, GetContentScaleFactor(), ShouldOffset());
        ((wxCairoPenData*)m_pen.GetRefData())->Apply(this);
        cairo_rectangle(m_context, x, y, w, h);
        cairo_stroke(m_context);
    }
}

void wxCairoContext::Rotate( wxDouble angle )
{
    cairo_rotate(m_context,angle);
}

void wxCairoContext::Translate( wxDouble dx , wxDouble dy )
{
    cairo_translate(m_context,dx,dy);
}

void wxCairoContext::Scale( wxDouble xScale , wxDouble yScale )
{
    cairo_scale(m_context,xScale,yScale);
}

// concatenates this transform with the current transform of this context
void wxCairoContext::ConcatTransform( const wxGraphicsMatrix& matrix )
{
    cairo_transform(m_context,(const cairo_matrix_t *) matrix.GetNativeMatrix());
}

// sets the transform of this context
void wxCairoContext::SetTransform( const wxGraphicsMatrix& matrix )
{
    // To get actual transformation we need to concatenate
    // given transformation with internal transformation.
    cairo_set_matrix(m_context, &m_internalTransform);
    cairo_transform(m_context, (const cairo_matrix_t*)matrix.GetNativeMatrix());
}

// gets the matrix of this context
wxGraphicsMatrix wxCairoContext::GetTransform() const
{
    wxGraphicsMatrix matrix = CreateMatrix();
    cairo_matrix_t* transformMatrix = (cairo_matrix_t*)matrix.GetNativeMatrix();
    cairo_get_matrix(m_context, transformMatrix);

    // Don't expose internal transformations.
    cairo_matrix_t intTransformMatrixRev = m_internalTransform;
    if ( cairo_matrix_invert(&intTransformMatrixRev) == CAIRO_STATUS_SUCCESS )
        cairo_matrix_multiply(transformMatrix, transformMatrix, &intTransformMatrixRev);

    return matrix;
}



void wxCairoContext::PushState()
{
    cairo_save(m_context);
}

void wxCairoContext::PopState()
{
    cairo_restore(m_context);
}

void wxCairoContext::Flush()
{
#ifdef __WXMSW__
    if ( m_mswSurface )
    {
        cairo_surface_flush(m_mswSurface);
    }
#endif
#ifdef __WXQT__
    if ( m_qtSurface )
    {
        cairo_surface_flush(m_qtSurface);
        m_qtPainter->drawImage( 0,0, *m_qtImage );
    }
#endif
}

void wxCairoContext::GetDPI(wxDouble* dpiX, wxDouble* dpiY) const
{
    const wxSize dpi = GetWindow() ? GetWindow()->GetDPI() :
                       (wxDisplay::GetStdPPI() * GetContentScaleFactor());

    if  (dpiX )
        *dpiX = dpi.x;
    if ( dpiY )
        *dpiY = dpi.y;
}

void wxCairoContext::DrawBitmap( const wxBitmap &bmp, wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
    wxGraphicsBitmap bitmap = GetRenderer()->CreateBitmap(bmp);
    DrawBitmap(bitmap, x, y, w, h);

}

void wxCairoContext::DrawBitmap(const wxGraphicsBitmap &bmp, wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
    PushState();

    // In case we're scaling the image by using a width and height different
    // than the bitmap's size create a pattern transformation on the surface and
    // draw the transformed pattern.
    wxCairoBitmapData* data = static_cast<wxCairoBitmapData*>(bmp.GetRefData());
    cairo_pattern_t* pattern = data->GetCairoPattern();
    wxSize size = data->GetSize();

    wxDouble scaleX = w / size.GetWidth();
    wxDouble scaleY = h / size.GetHeight();

    // prepare to draw the image
    cairo_translate(m_context, x, y);
    cairo_scale(m_context, scaleX, scaleY);
    cairo_set_source(m_context, pattern);
    // use the original size here since the context is scaled already...
    cairo_rectangle(m_context, 0, 0, size.GetWidth(), size.GetHeight());
    // fill the rectangle using the pattern
    cairo_fill(m_context);

    PopState();
}

void wxCairoContext::DrawIcon( const wxIcon &icon, wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
    // An icon is a bitmap on wxGTK, so do this the easy way.  When we want to
    // start using the Cairo backend on other platforms then we may need to
    // fiddle with this...
    DrawBitmap(icon, x, y, w, h);
}


void wxCairoContext::DoDrawText(const wxString& str, wxDouble x, wxDouble y)
{
    wxCHECK_RET( !m_font.IsNull(),
                 wxT("wxCairoContext::DrawText - no valid font set") );

    if ( str.empty())
        return;

    const wxCharBuffer data = str.utf8_str();
    if ( !data )
        return;

    wxCairoFontData* const
        fontData = static_cast<wxCairoFontData*>(m_font.GetRefData());

    fontData->Apply(this);

#ifdef __WXGTK__
    const wxFont& font = fontData->GetFont();
    if ( font.IsOk() )
    {
        wxGtkObject<PangoLayout> layout(pango_cairo_create_layout (m_context));
        ApplyFont(layout, font);
        pango_layout_set_text(layout, data, data.length());

        // Note that Pango attributes don't depend on font size, so we don't
        // need to use the scaled font here.
        font.GTKSetPangoAttrs(layout);

        cairo_move_to(m_context, x, y);
        pango_cairo_show_layout (m_context, layout);

        // Don't use Cairo text API, we already did everything.
        return;
    }
#endif // __WXGTK__

    // Cairo's x,y for drawing text is at the baseline, so we need to adjust
    // the position we move to by the ascent.
    cairo_font_extents_t fe;
    cairo_font_extents(m_context, &fe);
    cairo_move_to(m_context, x, y+fe.ascent);

    cairo_show_text(m_context, data);
}

void wxCairoContext::GetTextExtent( const wxString &str, wxDouble *width, wxDouble *height,
                                    wxDouble *descent, wxDouble *externalLeading ) const
{
    wxCHECK_RET( !m_font.IsNull(), wxT("wxCairoContext::GetTextExtent - no valid font set") );

    if ( width )
        *width = 0;
    if ( height )
        *height = 0;
    if ( descent )
        *descent = 0;
    if ( externalLeading )
        *externalLeading = 0;

    // We can skip computing the string width and height if it is empty, but
    // not its descent and/or external leading, which still needs to be
    // returned even for an empty string.
    if ( str.empty() && !descent && !externalLeading )
        return;

    wxCairoFontData* const
        fontData = static_cast<wxCairoFontData*>(m_font.GetRefData());

#ifdef __WXGTK__
    // Use Pango instead of Cairo toy font API if we have the font.
    const wxFont& font = fontData->GetFont();
    if ( font.IsOk() )
    {
        // Note that there is no need to call Apply() at all in this case, it
        // just sets the text colour, but we don't care about this when
        // measuring its extent.
        int w, h;

        wxGtkObject<PangoLayout> layout(pango_cairo_create_layout (m_context));
        ApplyFont(layout, font);
        const wxCharBuffer data = str.utf8_str();
        if ( !data )
        {
            return;
        }
        pango_layout_set_text(layout, data, data.length());
        pango_layout_get_pixel_size (layout, &w, &h);
        if ( width )
            *width = w;
        if ( height )
            *height = h;
        if (descent)
        {
            PangoLayoutIter *iter = pango_layout_get_iter(layout);
            int baseline = pango_layout_iter_get_baseline(iter);
            pango_layout_iter_free(iter);
            *descent = h - PANGO_PIXELS(baseline);
        }
        return;
    }
#endif // __WXGTK__

    fontData->Apply(const_cast<wxCairoContext*>(this));

    if (width)
    {
        const wxWX2MBbuf buf(str.mb_str(wxConvUTF8));
        cairo_text_extents_t te;
        cairo_text_extents(m_context, buf, &te);
        *width = te.width;
    }

    if (height || descent || externalLeading)
    {
        cairo_font_extents_t fe;
        cairo_font_extents(m_context, &fe);

        // some backends have negative descents

        if ( fe.descent < 0 )
            fe.descent = -fe.descent;

        if ( fe.height < (fe.ascent + fe.descent ) )
        {
            // some backends are broken re height ... (eg currently ATSUI)
            fe.height = fe.ascent + fe.descent;
        }

        if (height && !str.empty())
            *height = fe.height;
        if ( descent )
            *descent = fe.descent;
        if ( externalLeading )
            *externalLeading = wxMax(0, fe.height - (fe.ascent + fe.descent));
    }
}

void wxCairoContext::GetPartialTextExtents(const wxString& text, wxArrayDouble& widths) const
{
    widths.Empty();
    wxCHECK_RET( !m_font.IsNull(), wxT("wxCairoContext::GetPartialTextExtents - no valid font set") );
#ifdef __WXGTK__
    const wxCharBuffer data = text.utf8_str();
    int w = 0;
    if (data.length())
    {
        wxGtkObject<PangoLayout> layout(pango_cairo_create_layout(m_context));
        const wxFont& font = static_cast<wxCairoFontData*>(m_font.GetRefData())->GetFont();

        ApplyFont(layout, font);
        pango_layout_set_text(layout, data, data.length());
        PangoLayoutIter* iter = pango_layout_get_iter(layout);
        PangoRectangle rect;
        do {
            pango_layout_iter_get_cluster_extents(iter, NULL, &rect);
            w += rect.width;
            widths.Add(PANGO_PIXELS(w));
        } while (pango_layout_iter_next_cluster(iter));
        pango_layout_iter_free(iter);
    }
    size_t i = widths.GetCount();
    const size_t len = text.length();
    while (i++ < len)
        widths.Add(PANGO_PIXELS(w));
#else
    for (size_t i = 0; i < text.length(); i++)
    {
        const wxCharBuffer data = text.SubString(0, i).utf8_str();

        cairo_text_extents_t te;
        cairo_text_extents(m_context, (const char*)data, &te);

        widths.push_back(te.width);
    }
#endif
}

void * wxCairoContext::GetNativeContext()
{
    return m_context;
}

bool wxCairoContext::SetAntialiasMode(wxAntialiasMode antialias)
{
    if (m_antialias == antialias)
        return true;

    m_antialias = antialias;

    cairo_antialias_t antialiasMode;
    switch (antialias)
    {
        case wxANTIALIAS_DEFAULT:
            antialiasMode = CAIRO_ANTIALIAS_DEFAULT;
            break;
        case wxANTIALIAS_NONE:
            antialiasMode = CAIRO_ANTIALIAS_NONE;
            break;
        default:
            return false;
    }
    cairo_set_antialias(m_context, antialiasMode);

    cairo_font_options_t* options = cairo_font_options_create();
    cairo_get_font_options(m_context, options);
    cairo_font_options_set_antialias(options, antialiasMode);
    cairo_set_font_options(m_context, options);
    cairo_font_options_destroy(options);

    return true;
}

bool wxCairoContext::SetInterpolationQuality(wxInterpolationQuality WXUNUSED(interpolation))
{
    // placeholder
    return false;
}

bool wxCairoContext::SetCompositionMode(wxCompositionMode op)
{
    if ( m_composition == op )
        return true;

    m_composition = op;
    cairo_operator_t cop;
    switch (op)
    {
        case wxCOMPOSITION_CLEAR:
            cop = CAIRO_OPERATOR_CLEAR;
            break;
        case wxCOMPOSITION_SOURCE:
            cop = CAIRO_OPERATOR_SOURCE;
            break;
        case wxCOMPOSITION_OVER:
            cop = CAIRO_OPERATOR_OVER;
            break;
        case wxCOMPOSITION_IN:
            cop = CAIRO_OPERATOR_IN;
            break;
        case wxCOMPOSITION_OUT:
            cop = CAIRO_OPERATOR_OUT;
            break;
        case wxCOMPOSITION_ATOP:
            cop = CAIRO_OPERATOR_ATOP;
            break;
        case wxCOMPOSITION_DEST:
            cop = CAIRO_OPERATOR_DEST;
            break;
        case wxCOMPOSITION_DEST_OVER:
            cop = CAIRO_OPERATOR_DEST_OVER;
            break;
        case wxCOMPOSITION_DEST_IN:
            cop = CAIRO_OPERATOR_DEST_IN;
            break;
        case wxCOMPOSITION_DEST_OUT:
            cop = CAIRO_OPERATOR_DEST_OUT;
            break;
        case wxCOMPOSITION_DEST_ATOP:
            cop = CAIRO_OPERATOR_DEST_ATOP;
            break;
        case wxCOMPOSITION_XOR:
            cop = CAIRO_OPERATOR_XOR;
            break;
        case wxCOMPOSITION_ADD:
            cop = CAIRO_OPERATOR_ADD;
            break;
        case wxCOMPOSITION_DIFF:
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 10, 0)
            if ( cairo_version() >= CAIRO_VERSION_ENCODE(1, 10, 0) )
            {
                cop = CAIRO_OPERATOR_DIFFERENCE;
                break;
            }
            wxFALLTHROUGH;
#endif // Cairo 1.10
        default:
            return false;
    }
    cairo_set_operator(m_context, cop);
    return true;
}

void wxCairoContext::BeginLayer(wxDouble opacity)
{
    m_layerOpacities.push_back(float(opacity));
    cairo_push_group(m_context);
}

void wxCairoContext::EndLayer()
{
    float opacity = m_layerOpacities.back();
    m_layerOpacities.pop_back();
    cairo_pop_group_to_source(m_context);
    cairo_paint_with_alpha(m_context, double(opacity));
}

#ifdef __WXMSW__
WXHDC wxCairoContext::GetNativeHDC()
{
    wxCHECK_MSG(m_mswSurface, NULL, "Can't get HDC from Cairo context");
    cairo_surface_flush(m_mswSurface);
    return cairo_win32_surface_get_dc(m_mswSurface);
};

void wxCairoContext::ReleaseNativeHDC(WXHDC WXUNUSED(hdc))
{
    wxCHECK_RET(m_mswSurface, "Can't release HDC for Cairo context");
    cairo_surface_mark_dirty(m_mswSurface);
};
#endif // __WXMSW__

//-----------------------------------------------------------------------------
// wxCairoRenderer declaration
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxCairoRenderer : public wxGraphicsRenderer
{
public :
    wxCairoRenderer() {}

    virtual ~wxCairoRenderer() {}

    // Context

    virtual wxGraphicsContext * CreateContext( const wxWindowDC& dc) wxOVERRIDE;
    virtual wxGraphicsContext * CreateContext( const wxMemoryDC& dc) wxOVERRIDE;
#if wxUSE_PRINTING_ARCHITECTURE
    virtual wxGraphicsContext * CreateContext( const wxPrinterDC& dc) wxOVERRIDE;
#endif

    virtual wxGraphicsContext * CreateContextFromNativeContext( void * context ) wxOVERRIDE;

    virtual wxGraphicsContext * CreateContextFromNativeWindow( void * window ) wxOVERRIDE;

#ifdef __WXMSW__
    virtual wxGraphicsContext * CreateContextFromNativeHDC(WXHDC dc) wxOVERRIDE;
#endif

#if wxUSE_IMAGE
    virtual wxGraphicsContext * CreateContextFromImage(wxImage& image) wxOVERRIDE;
#endif // wxUSE_IMAGE

    virtual wxGraphicsContext * CreateContext( wxWindow* window ) wxOVERRIDE;

    virtual wxGraphicsContext * CreateMeasuringContext() wxOVERRIDE;
#ifdef __WXMSW__
#if wxUSE_ENH_METAFILE
    virtual wxGraphicsContext * CreateContext( const wxEnhMetaFileDC& dc);
#endif
#endif
    // Path

    virtual wxGraphicsPath CreatePath() wxOVERRIDE;

    // Matrix

    virtual wxGraphicsMatrix CreateMatrix( wxDouble a=1.0, wxDouble b=0.0, wxDouble c=0.0, wxDouble d=1.0,
        wxDouble tx=0.0, wxDouble ty=0.0) wxOVERRIDE;


    virtual wxGraphicsPen CreatePen(const wxGraphicsPenInfo& info) wxOVERRIDE ;

    virtual wxGraphicsBrush CreateBrush(const wxBrush& brush ) wxOVERRIDE ;

    virtual wxGraphicsBrush
    CreateLinearGradientBrush(wxDouble x1, wxDouble y1,
                              wxDouble x2, wxDouble y2,
                              const wxGraphicsGradientStops& stops,
                              const wxGraphicsMatrix& matrix = wxNullGraphicsMatrix) wxOVERRIDE;

    virtual wxGraphicsBrush
    CreateRadialGradientBrush(wxDouble startX, wxDouble startY,
                              wxDouble endX, wxDouble endY,
                              wxDouble radius,
                              const wxGraphicsGradientStops& stops,
                              const wxGraphicsMatrix& matrix = wxNullGraphicsMatrix) wxOVERRIDE;

    // sets the font
    virtual wxGraphicsFont CreateFont( const wxFont &font , const wxColour &col = *wxBLACK ) wxOVERRIDE ;
    virtual wxGraphicsFont CreateFont(double sizeInPixels,
                                      const wxString& facename,
                                      int flags = wxFONTFLAG_DEFAULT,
                                      const wxColour& col = *wxBLACK) wxOVERRIDE;
    virtual wxGraphicsFont CreateFontAtDPI(const wxFont& font,
                                           const wxRealPoint& dpi,
                                           const wxColour& col) wxOVERRIDE;

    // create a native bitmap representation
    virtual wxGraphicsBitmap CreateBitmap( const wxBitmap &bitmap ) wxOVERRIDE;
#if wxUSE_IMAGE
    virtual wxGraphicsBitmap CreateBitmapFromImage(const wxImage& image) wxOVERRIDE;
    virtual wxImage CreateImageFromBitmap(const wxGraphicsBitmap& bmp) wxOVERRIDE;
#endif // wxUSE_IMAGE

    // create a graphics bitmap from a native bitmap
    virtual wxGraphicsBitmap CreateBitmapFromNativeBitmap( void* bitmap ) wxOVERRIDE;

    // create a subimage from a native image representation
    virtual wxGraphicsBitmap CreateSubBitmap( const wxGraphicsBitmap &bitmap, wxDouble x, wxDouble y, wxDouble w, wxDouble h  ) wxOVERRIDE;

    virtual wxString GetName() const wxOVERRIDE;
    virtual void GetVersion(int *major, int *minor, int *micro) const wxOVERRIDE;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxCairoRenderer);
} ;

//-----------------------------------------------------------------------------
// wxCairoRenderer implementation
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxCairoRenderer,wxGraphicsRenderer);

static wxCairoRenderer gs_cairoGraphicsRenderer;

#ifdef __WXGTK__
    #define ENSURE_LOADED_OR_RETURN(returnOnFail)
#else
    #define ENSURE_LOADED_OR_RETURN(returnOnFail)  \
        if (!wxCairoInit())                        \
            return returnOnFail
#endif

wxGraphicsContext * wxCairoRenderer::CreateContext( const wxWindowDC& dc)
{
    ENSURE_LOADED_OR_RETURN(NULL);
    return new wxCairoContext(this,dc);
}

wxGraphicsContext * wxCairoRenderer::CreateContext( const wxMemoryDC& dc)
{
    ENSURE_LOADED_OR_RETURN(NULL);
    return new wxCairoContext(this,dc);
}

#if wxUSE_PRINTING_ARCHITECTURE
wxGraphicsContext * wxCairoRenderer::CreateContext( const wxPrinterDC& dc)
{
    ENSURE_LOADED_OR_RETURN(NULL);
    return new wxCairoContext(this, dc);
}
#endif

#if defined(__WXMSW__) && wxUSE_ENH_METAFILE
wxGraphicsContext * wxCairoRenderer::CreateContext(const wxEnhMetaFileDC& dc)
{
    ENSURE_LOADED_OR_RETURN(NULL);
    return new wxCairoContext(this, dc);
}
#endif // __WXMSW__ && wxUSE_ENH_METAFILE

wxGraphicsContext * wxCairoRenderer::CreateContextFromNativeContext(void * context)
{
    ENSURE_LOADED_OR_RETURN(NULL);
#ifdef __WXMSW__
    DWORD objType = ::GetObjectType((HGDIOBJ)context);
    if (objType == 0)
        return new wxCairoContext(this, (cairo_t*)context);

    if (objType == OBJ_DC || objType == OBJ_MEMDC)
        return new wxCairoContext(this, (HDC)context);

    return NULL;
#else
    return new wxCairoContext(this,(cairo_t*)context);
#endif
}


wxGraphicsContext * wxCairoRenderer::CreateContextFromNativeWindow( void * window )
{
    ENSURE_LOADED_OR_RETURN(NULL);
#ifdef __WXGTK__
    return new wxCairoContext(this, static_cast<GdkWindow*>(window));
#elif defined(__WXMSW__)
    return new wxCairoContext(this, static_cast<HWND>(window));
#else
    wxUnusedVar(window);
    return NULL;
#endif
}

#ifdef __WXMSW__
wxGraphicsContext * wxCairoRenderer::CreateContextFromNativeHDC(WXHDC dc)
{
    ENSURE_LOADED_OR_RETURN(NULL);
    return new wxCairoContext(this, (HDC)dc);
}
#endif

#if wxUSE_IMAGE
wxGraphicsContext * wxCairoRenderer::CreateContextFromImage(wxImage& image)
{
    ENSURE_LOADED_OR_RETURN(NULL);
    return new wxCairoImageContext(this, image);
}
#endif // wxUSE_IMAGE

wxGraphicsContext * wxCairoRenderer::CreateMeasuringContext()
{
#ifdef __WXGTK__
    return CreateContextFromNativeWindow(gdk_get_default_root_window());
#elif defined(__WXMSW__)
    ENSURE_LOADED_OR_RETURN(NULL);
    return new wxCairoMeasuringContext(this);
#else
    return NULL;
    // TODO
#endif
}

wxGraphicsContext * wxCairoRenderer::CreateContext( wxWindow* window )
{
    ENSURE_LOADED_OR_RETURN(NULL);
    return new wxCairoContext(this, window );
}

// Path

wxGraphicsPath wxCairoRenderer::CreatePath()
{
    wxGraphicsPath path;
    ENSURE_LOADED_OR_RETURN(path);
    path.SetRefData( new wxCairoPathData(this) );
    return path;
}


// Matrix

wxGraphicsMatrix wxCairoRenderer::CreateMatrix( wxDouble a, wxDouble b, wxDouble c, wxDouble d,
                                                wxDouble tx, wxDouble ty)

{
    wxGraphicsMatrix m;
    ENSURE_LOADED_OR_RETURN(m);
    wxCairoMatrixData* data = new wxCairoMatrixData( this );
    data->Set( a,b,c,d,tx,ty ) ;
    m.SetRefData(data);
    return m;
}

wxGraphicsPen wxCairoRenderer::CreatePen(const wxGraphicsPenInfo& info)
{
    wxGraphicsPen p;
    ENSURE_LOADED_OR_RETURN(p);
    if (info.GetStyle() != wxPENSTYLE_TRANSPARENT)
    {
        p.SetRefData(new wxCairoPenData( this, info ));
    }
    return p;
}

wxGraphicsBrush wxCairoRenderer::CreateBrush(const wxBrush& brush )
{
    wxGraphicsBrush p;
    ENSURE_LOADED_OR_RETURN(p);
    if (brush.IsOk() && brush.GetStyle() != wxBRUSHSTYLE_TRANSPARENT)
    {
        p.SetRefData(new wxCairoBrushData( this, brush ));
    }
    return p;
}

wxGraphicsBrush
wxCairoRenderer::CreateLinearGradientBrush(wxDouble x1, wxDouble y1,
                                           wxDouble x2, wxDouble y2,
                                           const wxGraphicsGradientStops& stops,
                                           const wxGraphicsMatrix& matrix)
{
    wxGraphicsBrush p;
    ENSURE_LOADED_OR_RETURN(p);
    wxCairoBrushData* d = new wxCairoBrushData( this );
    d->CreateLinearGradientPattern(x1, y1, x2, y2, stops, matrix);
    p.SetRefData(d);
    return p;
}

wxGraphicsBrush
wxCairoRenderer::CreateRadialGradientBrush(wxDouble startX, wxDouble startY,
                                           wxDouble endX, wxDouble endY, wxDouble r,
                                           const wxGraphicsGradientStops& stops,
                                           const wxGraphicsMatrix& matrix)
{
    wxGraphicsBrush p;
    ENSURE_LOADED_OR_RETURN(p);
    wxCairoBrushData* d = new wxCairoBrushData( this );
    d->CreateRadialGradientPattern(startX, startY, endX, endY, r, stops, matrix);
    p.SetRefData(d);
    return p;
}


wxGraphicsFont wxCairoRenderer::CreateFont( const wxFont &font , const wxColour &col )
{
    return CreateFontAtDPI(font, wxRealPoint(), col);
}

wxGraphicsFont
wxCairoRenderer::CreateFont(double sizeInPixels,
                            const wxString& facename,
                            int flags,
                            const wxColour& col)
{
    wxGraphicsFont font;
    ENSURE_LOADED_OR_RETURN(font);
    font.SetRefData(new wxCairoFontData(this, sizeInPixels, facename, flags, col));
    return font;
}

wxGraphicsFont
wxCairoRenderer::CreateFontAtDPI(const wxFont& font,
                                 const wxRealPoint& dpi,
                                 const wxColour& col)
{
    wxGraphicsFont p;
    ENSURE_LOADED_OR_RETURN(p);
    if ( font.IsOk() )
    {
        p.SetRefData(new wxCairoFontData( this, font, dpi, col ));
    }
    return p;
}

wxGraphicsBitmap wxCairoRenderer::CreateBitmap( const wxBitmap& bmp )
{
    wxGraphicsBitmap p;
    ENSURE_LOADED_OR_RETURN(p);
    if ( bmp.IsOk() )
    {
        p.SetRefData(new wxCairoBitmapData( this , bmp ));
    }
    return p;
}

#if wxUSE_IMAGE

wxGraphicsBitmap wxCairoRenderer::CreateBitmapFromImage(const wxImage& image)
{
    wxGraphicsBitmap bmp;

    ENSURE_LOADED_OR_RETURN(bmp);

    if ( image.IsOk() )
    {
        bmp.SetRefData(new wxCairoBitmapData(this, image));
    }

    return bmp;
}

wxImage wxCairoRenderer::CreateImageFromBitmap(const wxGraphicsBitmap& bmp)
{
    wxImage image;
    ENSURE_LOADED_OR_RETURN(image);

    const wxCairoBitmapData* const
        data = static_cast<wxCairoBitmapData*>(bmp.GetGraphicsData());
    if (data)
        image = data->ConvertToImage();

    return image;
}

#endif // wxUSE_IMAGE


wxGraphicsBitmap wxCairoRenderer::CreateBitmapFromNativeBitmap( void* bitmap )
{
    wxGraphicsBitmap p;
    ENSURE_LOADED_OR_RETURN(p);
    if ( bitmap != NULL )
    {
        p.SetRefData(new wxCairoBitmapData( this , (cairo_surface_t*) bitmap ));
    }
    return p;
}

wxGraphicsBitmap
wxCairoRenderer::CreateSubBitmap(const wxGraphicsBitmap& bitmap,
                                 wxDouble x, wxDouble y,
                                 wxDouble w, wxDouble h)
{
    ENSURE_LOADED_OR_RETURN(wxNullGraphicsBitmap);

    wxCHECK_MSG(!bitmap.IsNull(), wxNullGraphicsBitmap, wxS("Invalid bitmap"));

    wxCairoBitmapData* dataSrc = static_cast<wxCairoBitmapData*>(bitmap.GetRefData());
    cairo_surface_t* srcSurface = dataSrc->GetCairoSurface();
    wxCHECK_MSG(srcSurface, wxNullGraphicsBitmap, wxS("Invalid bitmap"));

    int srcWidth = cairo_image_surface_get_width(srcSurface);
    int srcHeight = cairo_image_surface_get_height(srcSurface);

    int dstWidth = wxRound(w);
    int dstHeight = wxRound(h);

    wxCHECK_MSG( x >= 0.0 && y >= 0.0 && dstWidth > 0 && dstHeight > 0 &&
                 x + dstWidth <= srcWidth && y + dstHeight <= srcHeight,
                 wxNullGraphicsBitmap, wxS("Invalid bitmap region"));

    cairo_surface_t* dstSurface;
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 12, 0)
    if ( cairo_version() >= CAIRO_VERSION_ENCODE(1, 12, 0) )
    {
        dstSurface = cairo_surface_create_similar_image(srcSurface,
            cairo_image_surface_get_format(srcSurface),
            dstWidth, dstHeight);
    }
    else
#endif // Cairo 1.12
    {
        dstSurface = cairo_surface_create_similar(srcSurface,
            CAIRO_CONTENT_COLOR_ALPHA,
            dstWidth, dstHeight);
    }

    cairo_t* cr = cairo_create(dstSurface);
    cairo_set_source_surface(cr, srcSurface, -x, -y);

    cairo_rectangle(cr, 0.0, 0.0, dstWidth, dstHeight);
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_fill(cr);
    cairo_destroy(cr);

    wxGraphicsBitmap bmpRes;
    bmpRes.SetRefData(new wxCairoBitmapData(this, dstSurface));
    return bmpRes;
}

wxString wxCairoRenderer::GetName() const
{
    return "cairo";
}

void wxCairoRenderer::GetVersion(int *major, int *minor, int *micro) const
{
    int dummy;
    sscanf(cairo_version_string(), "%d.%d.%d",
           major ? major : &dummy,
           minor ? minor : &dummy,
           micro ? micro : &dummy);
}

wxGraphicsRenderer* wxGraphicsRenderer::GetCairoRenderer()
{
    return &gs_cairoGraphicsRenderer;
}

#else // !wxUSE_CAIRO

wxGraphicsRenderer* wxGraphicsRenderer::GetCairoRenderer()
{
    return NULL;
}

#endif  // wxUSE_CAIRO/!wxUSE_CAIRO

// MSW and OS X and Qt on Windows have their own native default renderers, but the other ports
// use Cairo by default.
#if !(defined(__WXMSW__) || defined(__WXOSX__) || (defined(__WXQT__) && defined(__WIN32__)))
wxGraphicsRenderer* wxGraphicsRenderer::GetDefaultRenderer()
{
    return GetCairoRenderer();
}
#endif // !(__WXMSW__ || __WXOSX__ || (defined(__WXQT__) && defined(__WIN32__)))

#endif // wxUSE_GRAPHICS_CONTEXT
