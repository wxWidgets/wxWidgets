/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/graphics.cpp
// Purpose:     wxGCDC class
// Author:      Stefan Csomor
// Modified by:
// Created:     2006-09-30
// Copyright:   (c) 2006 Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/dc.h"

#if wxUSE_GRAPHICS_GDIPLUS

#ifndef WX_PRECOMP
    #include "wx/msw/wrapcdlg.h"
    #include "wx/image.h"
    #include "wx/window.h"
    #include "wx/utils.h"
    #include "wx/dialog.h"
    #include "wx/app.h"
    #include "wx/bitmap.h"
    #include "wx/log.h"
    #include "wx/icon.h"
    #include "wx/math.h"
    #include "wx/module.h"
    // include all dc types that are used as a param
    #include "wx/dc.h"
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
    #include "wx/dcprint.h"
#endif

#include "wx/stack.h"

#include "wx/private/graphics.h"
#include "wx/msw/wrapgdip.h"
#include "wx/msw/dc.h"
#include "wx/msw/enhmeta.h"
#include "wx/dcgraph.h"
#include "wx/rawbmp.h"

#include "wx/msw/private.h" // needs to be before #include <commdlg.h>

#if wxUSE_COMMON_DIALOGS
#include <commdlg.h>
#endif
#include <float.h> // for FLT_MAX, FLT_MIN

// Define REAL_MAX, REAL_MIN
// if it isn't done in GDI+ header(s).
#ifndef REAL_MAX
    #define REAL_MAX    FLT_MAX
#endif // REAL_MAX
#ifndef REAL_MIN
    #define REAL_MIN    FLT_MIN
#endif // REAL_MIN

namespace
{

//-----------------------------------------------------------------------------
// Local functions
//-----------------------------------------------------------------------------

inline double dmin(double a, double b) { return a < b ? a : b; }
inline double dmax(double a, double b) { return a > b ? a : b; }

// translate a wxColour to a Color
inline Color wxColourToColor(const wxColour& col)
{
    return Color(col.Alpha(), col.Red(), col.Green(), col.Blue());
}

// Do not use this pointer directly, it's only used by
// GetDrawTextStringFormat() and the cleanup code in wxGDIPlusRendererModule.
StringFormat* gs_drawTextStringFormat = NULL;

// Get the string format used for the text drawing and measuring functions:
// notice that it must be the same one for all of them, otherwise the drawn
// text might be of different size than what measuring it returned.
inline StringFormat* GetDrawTextStringFormat()
{
    if ( !gs_drawTextStringFormat )
    {
        gs_drawTextStringFormat = new StringFormat(StringFormat::GenericTypographic());

        // This doesn't make any difference for DrawText() actually but we want
        // this behaviour when measuring text.
        gs_drawTextStringFormat->SetFormatFlags
        (
            gs_drawTextStringFormat->GetFormatFlags()
                | StringFormatFlagsMeasureTrailingSpaces
        );
    }

    return gs_drawTextStringFormat;
}

} // anonymous namespace

//-----------------------------------------------------------------------------
// device context implementation
//
// more and more of the dc functionality should be implemented by calling
// the appropricate wxGDIPlusContext, but we will have to do that step by step
// also coordinate conversions should be moved to native matrix ops
//-----------------------------------------------------------------------------

// we always stock two context states, one at entry, to be able to preserve the
// state we were called with, the other one after changing to HI Graphics orientation
// (this one is used for getting back clippings etc)

//-----------------------------------------------------------------------------
// wxGraphicsPath implementation
//-----------------------------------------------------------------------------

class wxGDIPlusContext;

class wxGDIPlusPathData : public wxGraphicsPathData
{
public :
    wxGDIPlusPathData(wxGraphicsRenderer* renderer, GraphicsPath* path = NULL);
    ~wxGDIPlusPathData();

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
    virtual void AddArc( wxDouble x, wxDouble y, wxDouble r, wxDouble startAngle, wxDouble endAngle, bool clockwise ) wxOVERRIDE;

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
    virtual void * GetNativePath() const wxOVERRIDE { return m_path; }

    // give the native path returned by GetNativePath() back (there might be some deallocations necessary)
    virtual void UnGetNativePath(void * WXUNUSED(path)) const wxOVERRIDE {}

    // transforms each point of this path by the matrix
    virtual void Transform( const wxGraphicsMatrixData* matrix ) wxOVERRIDE;

    // gets the bounding box enclosing all points (possibly including control points)
    virtual void GetBox(wxDouble *x, wxDouble *y, wxDouble *w, wxDouble *h) const wxOVERRIDE;

    virtual bool Contains( wxDouble x, wxDouble y, wxPolygonFillMode fillStyle = wxODDEVEN_RULE) const wxOVERRIDE;

private :
    GraphicsPath* m_path;
    bool m_logCurrentPointSet;
    PointF m_logCurrentPoint;
    bool m_figureOpened;
    PointF m_figureStart;
};

class wxGDIPlusMatrixData : public wxGraphicsMatrixData
{
public :
    wxGDIPlusMatrixData(wxGraphicsRenderer* renderer, Matrix* matrix = NULL) ;
    virtual ~wxGDIPlusMatrixData() ;

    virtual wxGraphicsObjectRefData* Clone() const wxOVERRIDE;

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
    virtual bool IsEqual( const wxGraphicsMatrixData* t) const wxOVERRIDE;

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
    Matrix* m_matrix ;
} ;

class wxGDIPlusPenData : public wxGraphicsObjectRefData
{
public:
    wxGDIPlusPenData( wxGraphicsRenderer* renderer, const wxGraphicsPenInfo &info );
    ~wxGDIPlusPenData();

    void Init();

    virtual wxDouble GetWidth() { return m_width; }
    virtual Pen* GetGDIPlusPen() { return m_pen; }

protected :
    Pen* m_pen;
    Image* m_penImage;
    Brush* m_penBrush;

    wxDouble m_width;
};

class wxGDIPlusBrushData : public wxGraphicsObjectRefData
{
public:
    wxGDIPlusBrushData( wxGraphicsRenderer* renderer );
    wxGDIPlusBrushData( wxGraphicsRenderer* renderer, const wxBrush &brush );
    ~wxGDIPlusBrushData ();

    void CreateLinearGradientBrush(wxDouble x1, wxDouble y1,
                                   wxDouble x2, wxDouble y2,
                                   const wxGraphicsGradientStops& stops);
    void CreateRadialGradientBrush(wxDouble xo, wxDouble yo,
                                   wxDouble xc, wxDouble yc,
                                   wxDouble radius,
                                   const wxGraphicsGradientStops& stops);

    virtual Brush* GetGDIPlusBrush() { return m_brush; }

protected:
    virtual void Init();

private:
    // common part of Create{Linear,Radial}GradientBrush()
    template <typename T>
    void SetGradientStops(T *brush,
                          const wxGraphicsGradientStops& stops,
                          bool reversed = false);

    Brush* m_brush;
    Image* m_brushImage;
    GraphicsPath* m_brushPath;
};

class WXDLLIMPEXP_CORE wxGDIPlusBitmapData : public wxGraphicsBitmapData
{
public:
    wxGDIPlusBitmapData( wxGraphicsRenderer* renderer, Bitmap* bitmap );
    wxGDIPlusBitmapData( wxGraphicsRenderer* renderer, const wxBitmap &bmp );
    ~wxGDIPlusBitmapData ();

    virtual Bitmap* GetGDIPlusBitmap() { return m_bitmap; }
    virtual void* GetNativeBitmap() const wxOVERRIDE { return m_bitmap; }

#if wxUSE_IMAGE
    wxImage ConvertToImage() const;
#endif // wxUSE_IMAGE

private :
    Bitmap* m_bitmap;
    Bitmap* m_helper;
};

class wxGDIPlusFontData : public wxGraphicsObjectRefData
{
public:
    wxGDIPlusFontData( wxGraphicsRenderer* renderer,
                       const wxFont &font,
                       const wxColour& col );
    wxGDIPlusFontData(wxGraphicsRenderer* renderer,
                      const wxString& name,
                      REAL size,
                      int style,
                      const wxColour& col);

    // This ctor takes ownership of the brush.
    wxGDIPlusFontData(wxGraphicsRenderer* renderer,
                      const wxString& name,
                      REAL size,
                      int style,
                      Brush* textBrush);

    ~wxGDIPlusFontData();

    virtual Brush* GetGDIPlusBrush() { return m_textBrush; }
    virtual Font* GetGDIPlusFont() { return m_font; }

private :
    // Common part of all ctors, flags here is a combination of values of
    // FontStyle GDI+ enum.
    void Init(const wxString& name,
              REAL size,
              int style,
              Brush* textBrush);

    // Common part of ctors taking wxColour.
    void Init(const wxString& name,
              REAL size,
              int style,
              const wxColour& col)
    {
        Init(name, size, style, new SolidBrush(wxColourToColor(col)));
    }

    Brush* m_textBrush;
    Font* m_font;
};

class wxGDIPlusContext : public wxGraphicsContext
{
public:
    wxGDIPlusContext( wxGraphicsRenderer* renderer, const wxDC& dc );
    wxGDIPlusContext( wxGraphicsRenderer* renderer, HDC hdc, wxDouble width, wxDouble height );
    wxGDIPlusContext( wxGraphicsRenderer* renderer, HWND hwnd, wxWindow* window = NULL);
    wxGDIPlusContext( wxGraphicsRenderer* renderer, Graphics* gr);
    wxGDIPlusContext(wxGraphicsRenderer* renderer);

    virtual ~wxGDIPlusContext();

    virtual void Clip( const wxRegion &region ) wxOVERRIDE;
    // clips drawings to the rect
    virtual void Clip( wxDouble x, wxDouble y, wxDouble w, wxDouble h ) wxOVERRIDE;

    // resets the clipping to original extent
    virtual void ResetClip() wxOVERRIDE;

    // returns bounding box of the clipping region
    virtual void GetClipBox(wxDouble* x, wxDouble* y, wxDouble* w, wxDouble* h) wxOVERRIDE;

    virtual void * GetNativeContext() wxOVERRIDE;

    virtual void StrokePath( const wxGraphicsPath& p ) wxOVERRIDE;
    virtual void FillPath( const wxGraphicsPath& p , wxPolygonFillMode fillStyle = wxODDEVEN_RULE ) wxOVERRIDE;

    virtual void DrawRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h ) wxOVERRIDE;

    // stroke lines connecting each of the points
    virtual void StrokeLines( size_t n, const wxPoint2DDouble *points) wxOVERRIDE;

    // We don't have any specific implementation for this one in wxMSW but
    // override it just to avoid warnings about hiding the base class virtual.
    virtual void StrokeLines( size_t n, const wxPoint2DDouble *beginPoints, const wxPoint2DDouble *endPoints) wxOVERRIDE
    {
        wxGraphicsContext::StrokeLines(n, beginPoints, endPoints);
    }

    // draws a polygon
    virtual void DrawLines( size_t n, const wxPoint2DDouble *points, wxPolygonFillMode fillStyle = wxODDEVEN_RULE ) wxOVERRIDE;

    virtual bool SetAntialiasMode(wxAntialiasMode antialias) wxOVERRIDE;

    virtual bool SetInterpolationQuality(wxInterpolationQuality interpolation) wxOVERRIDE;

    virtual bool SetCompositionMode(wxCompositionMode op) wxOVERRIDE;

    virtual void BeginLayer(wxDouble opacity) wxOVERRIDE;

    virtual void EndLayer() wxOVERRIDE;

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

    virtual void GetTextExtent( const wxString &str, wxDouble *width, wxDouble *height,
        wxDouble *descent, wxDouble *externalLeading ) const wxOVERRIDE;
    virtual void GetPartialTextExtents(const wxString& text, wxArrayDouble& widths) const wxOVERRIDE;
    virtual bool ShouldOffset() const wxOVERRIDE;
    virtual void GetSize( wxDouble* width, wxDouble *height );

    Graphics* GetGraphics() const { return m_context; }

protected:
    // Used from ctors (including those in the derived classes) and takes
    // ownership of the graphics pointer that must be non-NULL.
    void Init(Graphics* graphics, int width, int height);

private:
    virtual void DoDrawText(const wxString& str, wxDouble x, wxDouble y) wxOVERRIDE;

    Graphics* m_context;
    wxStack<GraphicsState> m_stateStack;
    GraphicsState m_state1;
    GraphicsState m_state2;

    wxDECLARE_NO_COPY_CLASS(wxGDIPlusContext);
};

#if wxUSE_IMAGE

class wxGDIPlusImageContext : public wxGDIPlusContext
{
public:
    wxGDIPlusImageContext(wxGraphicsRenderer* renderer, wxImage& image) :
        wxGDIPlusContext(renderer),
        m_image(image),
        m_bitmap(renderer, image)
    {
        Init
        (
            new Graphics(m_bitmap.GetGDIPlusBitmap()),
            image.GetWidth(),
            image.GetHeight()
        );
    }

    virtual ~wxGDIPlusImageContext()
    {
        Flush();
    }

    virtual void Flush() wxOVERRIDE
    {
        m_image = m_bitmap.ConvertToImage();
    }

private:
    wxImage& m_image;
    wxGDIPlusBitmapData m_bitmap;

    wxDECLARE_NO_COPY_CLASS(wxGDIPlusImageContext);
};

#endif // wxUSE_IMAGE

class wxGDIPlusMeasuringContext : public wxGDIPlusContext
{
public:
    wxGDIPlusMeasuringContext( wxGraphicsRenderer* renderer ) : wxGDIPlusContext( renderer , m_hdc = GetDC(NULL), 1000, 1000 )
    {
    }

    virtual ~wxGDIPlusMeasuringContext()
    {
        ReleaseDC( NULL, m_hdc );
    }

private:
    HDC m_hdc ;
} ;

class wxGDIPlusPrintingContext : public wxGDIPlusContext
{
public:
    wxGDIPlusPrintingContext( wxGraphicsRenderer* renderer, const wxDC& dc );

    // Override to scale the font proportionally to the DPI.
    virtual void SetFont(const wxGraphicsFont& font) wxOVERRIDE
    {
        // The casts here are safe because we're only supposed to be passed
        // fonts created by this renderer.
        Font* const f = static_cast<wxGDIPlusFontData*>(font.GetRefData())->GetGDIPlusFont();
        Brush* const b = static_cast<wxGDIPlusFontData*>(font.GetRefData())->GetGDIPlusBrush();

        // To scale the font, we need to create a new one which means
        // retrieving all the parameters originally used to create the font.
        FontFamily ffamily;
        f->GetFamily(&ffamily);

        WCHAR familyName[LF_FACESIZE];
        ffamily.GetFamilyName(familyName);

        wxGraphicsFont fontScaled;
        fontScaled.SetRefData(new wxGDIPlusFontData
                                  (
                                    GetRenderer(),
                                    familyName,
                                    f->GetSize() / m_fontScaleRatio,
                                    f->GetStyle(),
                                    b->Clone()
                                  ));
        wxGDIPlusContext::SetFont(fontScaled);
    }

private:
    // This is logically const ratio between this context DPI and the standard
    // one which is used for scaling the fonts used with this context: without
    // this, the fonts wouldn't have the correct size, even though we
    // explicitly create them using UnitPoint units.
    wxDouble m_fontScaleRatio;
};

//-----------------------------------------------------------------------------
// wxGDIPlusRenderer declaration
//-----------------------------------------------------------------------------

class wxGDIPlusRenderer : public wxGraphicsRenderer
{
public :
    wxGDIPlusRenderer()
    {
        m_loaded = -1;
        m_gditoken = 0;
    }

    virtual ~wxGDIPlusRenderer()
    {
        if ( m_loaded == 1 )
        {
            Unload();
        }
    }

    // Context

    virtual wxGraphicsContext * CreateContext( const wxWindowDC& dc) wxOVERRIDE;

    virtual wxGraphicsContext * CreateContext( const wxMemoryDC& dc) wxOVERRIDE;

#if wxUSE_PRINTING_ARCHITECTURE
    virtual wxGraphicsContext * CreateContext( const wxPrinterDC& dc) wxOVERRIDE;
#endif

#if wxUSE_ENH_METAFILE
    virtual wxGraphicsContext * CreateContext( const wxEnhMetaFileDC& dc) wxOVERRIDE;
#endif

    virtual wxGraphicsContext * CreateContextFromNativeContext( void * context ) wxOVERRIDE;

    virtual wxGraphicsContext * CreateContextFromNativeWindow( void * window ) wxOVERRIDE;

    virtual wxGraphicsContext * CreateContextFromNativeHDC(WXHDC dc) wxOVERRIDE;

    virtual wxGraphicsContext * CreateContext( wxWindow* window ) wxOVERRIDE;

#if wxUSE_IMAGE
    virtual wxGraphicsContext * CreateContextFromImage(wxImage& image) wxOVERRIDE;
#endif // wxUSE_IMAGE

    virtual wxGraphicsContext * CreateMeasuringContext() wxOVERRIDE;

    // Path

    virtual wxGraphicsPath CreatePath() wxOVERRIDE;

    // Matrix

    virtual wxGraphicsMatrix CreateMatrix( wxDouble a=1.0, wxDouble b=0.0, wxDouble c=0.0, wxDouble d=1.0,
        wxDouble tx=0.0, wxDouble ty=0.0) wxOVERRIDE;


    virtual wxGraphicsPen CreatePen(const wxGraphicsPenInfo& pen) wxOVERRIDE;

    virtual wxGraphicsBrush CreateBrush(const wxBrush& brush ) wxOVERRIDE;

    virtual wxGraphicsBrush
    CreateLinearGradientBrush(wxDouble x1, wxDouble y1,
                              wxDouble x2, wxDouble y2,
                              const wxGraphicsGradientStops& stops) wxOVERRIDE;

    virtual wxGraphicsBrush
    CreateRadialGradientBrush(wxDouble xo, wxDouble yo,
                              wxDouble xc, wxDouble yc,
                              wxDouble radius,
                              const wxGraphicsGradientStops& stops) wxOVERRIDE;

    // create a native bitmap representation
    virtual wxGraphicsBitmap CreateBitmap( const wxBitmap &bitmap ) wxOVERRIDE;
#if wxUSE_IMAGE
    virtual wxGraphicsBitmap CreateBitmapFromImage(const wxImage& image) wxOVERRIDE;
    virtual wxImage CreateImageFromBitmap(const wxGraphicsBitmap& bmp) wxOVERRIDE;
#endif // wxUSE_IMAGE

    virtual wxGraphicsFont CreateFont( const wxFont& font,
                                       const wxColour& col) wxOVERRIDE;

    virtual wxGraphicsFont CreateFont(double size,
                                      const wxString& facename,
                                      int flags = wxFONTFLAG_DEFAULT,
                                      const wxColour& col = *wxBLACK) wxOVERRIDE;

    // create a graphics bitmap from a native bitmap
    virtual wxGraphicsBitmap CreateBitmapFromNativeBitmap( void* bitmap ) wxOVERRIDE;

    // create a subimage from a native image representation
    virtual wxGraphicsBitmap CreateSubBitmap( const wxGraphicsBitmap &bitmap, wxDouble x, wxDouble y, wxDouble w, wxDouble h  ) wxOVERRIDE;

    virtual wxString GetName() const wxOVERRIDE;
    virtual void GetVersion(int *major, int *minor, int *micro) const wxOVERRIDE;

protected :
    bool EnsureIsLoaded();
    void Load();
    void Unload();
    friend class wxGDIPlusRendererModule;

private :
    int m_loaded;
    ULONG_PTR m_gditoken;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxGDIPlusRenderer);
} ;

//-----------------------------------------------------------------------------
// wxGDIPlusPen implementation
//-----------------------------------------------------------------------------

wxGDIPlusPenData::~wxGDIPlusPenData()
{
    delete m_pen;
    delete m_penImage;
    delete m_penBrush;
}

void wxGDIPlusPenData::Init()
{
    m_pen = NULL ;
    m_penImage = NULL;
    m_penBrush = NULL;
}

wxGDIPlusPenData::wxGDIPlusPenData( wxGraphicsRenderer* renderer,
                                    const wxGraphicsPenInfo &info )
    : wxGraphicsObjectRefData(renderer)
{
    Init();
    m_width = info.GetWidth();
    if (m_width <= 0.0)
        m_width = 0.1;

    m_pen = new Pen(wxColourToColor(info.GetColour()), m_width );

    LineCap cap;
    switch ( info.GetCap() )
    {
    case wxCAP_ROUND :
        cap = LineCapRound;
        break;

    case wxCAP_PROJECTING :
        cap = LineCapSquare;
        break;

    case wxCAP_BUTT :
        cap = LineCapFlat; // TODO verify
        break;

    default :
        cap = LineCapFlat;
        break;
    }
    m_pen->SetLineCap(cap,cap, DashCapFlat);

    LineJoin join;
    switch ( info.GetJoin() )
    {
    case wxJOIN_BEVEL :
        join = LineJoinBevel;
        break;

    case wxJOIN_MITER :
        join = LineJoinMiter;
        break;

    case wxJOIN_ROUND :
        join = LineJoinRound;
        break;

    default :
        join = LineJoinMiter;
        break;
    }

    m_pen->SetLineJoin(join);

    m_pen->SetDashStyle(DashStyleSolid);

    DashStyle dashStyle = DashStyleSolid;
    switch ( info.GetStyle() )
    {
    case wxPENSTYLE_SOLID :
        break;

    case wxPENSTYLE_DOT :
        dashStyle = DashStyleDot;
        break;

    case wxPENSTYLE_LONG_DASH :
        dashStyle = DashStyleDash; // TODO verify
        break;

    case wxPENSTYLE_SHORT_DASH :
        dashStyle = DashStyleDash;
        break;

    case wxPENSTYLE_DOT_DASH :
        dashStyle = DashStyleDashDot;
        break;
    case wxPENSTYLE_USER_DASH :
        {
            dashStyle = DashStyleCustom;
            wxDash *dashes;
            int count = info.GetDashes( &dashes );
            if ((dashes != NULL) && (count > 0))
            {
                REAL *userLengths = new REAL[count];
                for ( int i = 0; i < count; ++i )
                {
                    userLengths[i] = dashes[i];
                }
                m_pen->SetDashPattern( userLengths, count);
                delete[] userLengths;
            }
        }
        break;
    case wxPENSTYLE_STIPPLE :
        {
            wxBitmap bmp = info.GetStipple();
            if ( bmp.IsOk() )
            {
                m_penImage = Bitmap::FromHBITMAP((HBITMAP)bmp.GetHBITMAP(),
#if wxUSE_PALETTE
                    (HPALETTE)bmp.GetPalette()->GetHPALETTE()
#else
                    NULL
#endif
                );
                m_penBrush = new TextureBrush(m_penImage);
                m_pen->SetBrush( m_penBrush );
            }

        }
        break;
    default :
        if ( info.GetStyle() >= wxPENSTYLE_FIRST_HATCH &&
             info.GetStyle() <= wxPENSTYLE_LAST_HATCH )
        {
            HatchStyle style;
            switch( info.GetStyle() )
            {
            case wxPENSTYLE_BDIAGONAL_HATCH :
                style = HatchStyleBackwardDiagonal;
                break ;
            case wxPENSTYLE_CROSSDIAG_HATCH :
                style = HatchStyleDiagonalCross;
                break ;
            case wxPENSTYLE_FDIAGONAL_HATCH :
                style = HatchStyleForwardDiagonal;
                break ;
            case wxPENSTYLE_CROSS_HATCH :
                style = HatchStyleCross;
                break ;
            case wxPENSTYLE_HORIZONTAL_HATCH :
                style = HatchStyleHorizontal;
                break ;
            case wxPENSTYLE_VERTICAL_HATCH :
                style = HatchStyleVertical;
                break ;
            default:
                style = HatchStyleHorizontal;
            }
            m_penBrush = new HatchBrush
                             (
                                style,
                                wxColourToColor(info.GetColour()),
                                Color::Transparent
                             );
            m_pen->SetBrush( m_penBrush );
        }
        break;
    }
    if ( dashStyle != DashStyleSolid )
        m_pen->SetDashStyle(dashStyle);
}

//-----------------------------------------------------------------------------
// wxGDIPlusBrush implementation
//-----------------------------------------------------------------------------

wxGDIPlusBrushData::wxGDIPlusBrushData( wxGraphicsRenderer* renderer )
: wxGraphicsObjectRefData(renderer)
{
    Init();
}

wxGDIPlusBrushData::wxGDIPlusBrushData( wxGraphicsRenderer* renderer , const wxBrush &brush )
: wxGraphicsObjectRefData(renderer)
{
    Init();
    if ( brush.GetStyle() == wxBRUSHSTYLE_SOLID)
    {
        m_brush = new SolidBrush(wxColourToColor( brush.GetColour()));
    }
    else if ( brush.IsHatch() )
    {
        HatchStyle style;
        switch( brush.GetStyle() )
        {
        case wxBRUSHSTYLE_BDIAGONAL_HATCH :
            style = HatchStyleBackwardDiagonal;
            break ;
        case wxBRUSHSTYLE_CROSSDIAG_HATCH :
            style = HatchStyleDiagonalCross;
            break ;
        case wxBRUSHSTYLE_FDIAGONAL_HATCH :
            style = HatchStyleForwardDiagonal;
            break ;
        case wxBRUSHSTYLE_CROSS_HATCH :
            style = HatchStyleCross;
            break ;
        case wxBRUSHSTYLE_HORIZONTAL_HATCH :
            style = HatchStyleHorizontal;
            break ;
        case wxBRUSHSTYLE_VERTICAL_HATCH :
            style = HatchStyleVertical;
            break ;
        default:
            style = HatchStyleHorizontal;
        }
        m_brush = new HatchBrush
                      (
                        style,
                        wxColourToColor(brush.GetColour()),
                        Color::Transparent
                      );
    }
    else
    {
        wxBitmap* bmp = brush.GetStipple();
        if ( bmp && bmp->IsOk() )
        {
            wxDELETE( m_brushImage );
            m_brushImage = Bitmap::FromHBITMAP((HBITMAP)bmp->GetHBITMAP(),
#if wxUSE_PALETTE
                (HPALETTE)bmp->GetPalette()->GetHPALETTE()
#else
                NULL
#endif
            );
            m_brush = new TextureBrush(m_brushImage);
        }
    }
}

wxGDIPlusBrushData::~wxGDIPlusBrushData()
{
    delete m_brush;
    delete m_brushImage;
    delete m_brushPath;
};

void wxGDIPlusBrushData::Init()
{
    m_brush = NULL;
    m_brushImage= NULL;
    m_brushPath= NULL;
}

template <typename T>
void
wxGDIPlusBrushData::SetGradientStops(T *brush,
        const wxGraphicsGradientStops& stops,
        bool reversed)
{
    const unsigned numStops = stops.GetCount();
    if ( numStops <= 2 )
    {
        // initial and final colours are set during the brush creation, nothing
        // more to do
        return;
    }

    wxVector<Color> colors(numStops);
    wxVector<REAL> positions(numStops);

    if ( reversed )
    {
        for ( unsigned i = 0; i < numStops; i++ )
        {
            wxGraphicsGradientStop stop = stops.Item(numStops - i - 1);

            colors[i] = wxColourToColor(stop.GetColour());
            positions[i] = 1.0 - stop.GetPosition();
        }
    }
    else
    {
        for ( unsigned i = 0; i < numStops; i++ )
        {
            wxGraphicsGradientStop stop = stops.Item(i);

            colors[i] = wxColourToColor(stop.GetColour());
            positions[i] = stop.GetPosition();
        }
    }

    brush->SetInterpolationColors(&colors[0], &positions[0], numStops);
}

void
wxGDIPlusBrushData::CreateLinearGradientBrush(wxDouble x1, wxDouble y1,
                                              wxDouble x2, wxDouble y2,
                                              const wxGraphicsGradientStops& stops)
{
    LinearGradientBrush * const
        brush = new LinearGradientBrush(PointF(x1, y1) , PointF(x2, y2),
                                        wxColourToColor(stops.GetStartColour()),
                                        wxColourToColor(stops.GetEndColour()));
    m_brush =  brush;

    SetGradientStops(brush, stops);
}

void
wxGDIPlusBrushData::CreateRadialGradientBrush(wxDouble xo, wxDouble yo,
                                              wxDouble xc, wxDouble yc,
                                              wxDouble radius,
                                              const wxGraphicsGradientStops& stops)
{
    m_brushPath = new GraphicsPath();
    m_brushPath->AddEllipse( (REAL)(xc-radius), (REAL)(yc-radius),
                             (REAL)(2*radius), (REAL)(2*radius));

    PathGradientBrush * const brush = new PathGradientBrush(m_brushPath);
    m_brush = brush;
    brush->SetCenterPoint(PointF(xo, yo));
    brush->SetCenterColor(wxColourToColor(stops.GetStartColour()));

    const Color col(wxColourToColor(stops.GetEndColour()));
    int count = 1;
    brush->SetSurroundColors(&col, &count);

    // Because the GDI+ API draws radial gradients from outside towards the
    // center we have to reverse the order of the gradient stops.
    SetGradientStops(brush, stops, true);
}

//-----------------------------------------------------------------------------
// Support for adding private fonts
//-----------------------------------------------------------------------------

#if wxUSE_PRIVATE_FONTS

namespace
{

Gdiplus::PrivateFontCollection* gs_privateFonts = NULL;
Gdiplus::FontFamily* gs_pFontFamily = NULL;

} // anonymous namespace

// This function is defined in src/msw/font.cpp.
extern const wxArrayString& wxGetPrivateFontFileNames();

#endif // wxUSE_PRIVATE_FONTS

//-----------------------------------------------------------------------------
// wxGDIPlusFont implementation
//-----------------------------------------------------------------------------

void
wxGDIPlusFontData::Init(const wxString& name,
                        REAL size,
                        int style,
                        Brush* textBrush)
{
#if wxUSE_PRIVATE_FONTS
    // If the user has registered any private fonts, they should be used in
    // preference to any system-wide ones.
    m_font = NULL;
    if ( gs_privateFonts )
    {
        const int count = gs_privateFonts->GetFamilyCount();

        // We should find all the families, i.e. "found" should be "count".
        int found = 0;
        gs_privateFonts->GetFamilies(count, gs_pFontFamily, &found);

        for ( int j = 0 ; j < found; j++ )
        {
            wchar_t familyName[LF_FACESIZE];
            int rc = gs_pFontFamily[j].GetFamilyName(familyName);
            if ( rc == 0 && name == familyName )
            {
                m_font = new Font(&gs_pFontFamily[j], size, style, UnitPoint);
                break;
            }
        }
    }

    if ( !m_font )
#endif // wxUSE_PRIVATE_FONTS
    {
        m_font = new Font(name.wc_str(), size, style, UnitPoint);
    }

    m_textBrush = textBrush;
}

wxGDIPlusFontData::wxGDIPlusFontData( wxGraphicsRenderer* renderer,
                                      const wxFont &font,
                                      const wxColour& col )
    : wxGraphicsObjectRefData( renderer )
{
    int style = FontStyleRegular;
    if ( font.GetStyle() == wxFONTSTYLE_ITALIC )
        style |= FontStyleItalic;
    if ( font.GetUnderlined() )
        style |= FontStyleUnderline;
    if ( font.GetStrikethrough() )
        style |= FontStyleStrikeout;
    if ( font.GetWeight() == wxFONTWEIGHT_BOLD )
        style |= FontStyleBold;

    Init(font.GetFaceName(), font.GetFractionalPointSize(), style, col);
}

wxGDIPlusFontData::wxGDIPlusFontData(wxGraphicsRenderer* renderer,
                                     const wxString& name,
                                     REAL size,
                                     int style,
                                     const wxColour& col) :
    wxGraphicsObjectRefData(renderer)
{
    Init(name, size, style, col);
}

wxGDIPlusFontData::wxGDIPlusFontData(wxGraphicsRenderer* renderer,
                                     const wxString& name,
                                     REAL size,
                                     int style,
                                     Brush* brush)
    : wxGraphicsObjectRefData(renderer)
{
    Init(name, size, style, brush);
}

wxGDIPlusFontData::~wxGDIPlusFontData()
{
    delete m_textBrush;
    delete m_font;
}

// the built-in conversions functions create non-premultiplied bitmaps, while GDIPlus needs them in the
// premultiplied format, therefore in the failing cases we create a new bitmap using the non-premultiplied
// bytes as parameter, since there is no real copying of the data going in, only references are stored
// m_helper has to be kept alive as well

//-----------------------------------------------------------------------------
// wxGDIPlusBitmapData implementation
//-----------------------------------------------------------------------------

wxGDIPlusBitmapData::wxGDIPlusBitmapData( wxGraphicsRenderer* renderer, Bitmap* bitmap ) :
    wxGraphicsBitmapData( renderer ), m_bitmap( bitmap )
{
    m_helper = NULL;
}

wxGDIPlusBitmapData::wxGDIPlusBitmapData( wxGraphicsRenderer* renderer,
                        const wxBitmap &bmp) : wxGraphicsBitmapData( renderer )
{
    m_bitmap = NULL;
    m_helper = NULL;

    Bitmap* image = NULL;
    if ( bmp.GetMask() )
    {
        Bitmap interim((HBITMAP)bmp.GetHBITMAP(),
#if wxUSE_PALETTE
            (HPALETTE)bmp.GetPalette()->GetHPALETTE()
#else
            NULL
#endif
        );

        size_t width = interim.GetWidth();
        size_t height = interim.GetHeight();
        Rect bounds(0,0,width,height);

        image = new Bitmap(width,height,PixelFormat32bppPARGB) ;

        Bitmap interimMask((HBITMAP)bmp.GetMask()->GetMaskBitmap(),NULL);
        wxASSERT(interimMask.GetPixelFormat() == PixelFormat1bppIndexed);

        BitmapData dataMask ;
        interimMask.LockBits(&bounds,ImageLockModeRead,
            interimMask.GetPixelFormat(),&dataMask);


        BitmapData imageData ;
        image->LockBits(&bounds,ImageLockModeWrite, PixelFormat32bppPARGB, &imageData);

        BYTE maskPattern = 0 ;
        BYTE maskByte = 0;
        size_t maskIndex ;

        for ( size_t y = 0 ; y < height ; ++y)
        {
            maskIndex = 0 ;
            for( size_t x = 0 ; x < width; ++x)
            {
                if ( x % 8 == 0)
                {
                    maskPattern = 0x80;
                    maskByte = *((BYTE*)dataMask.Scan0 + dataMask.Stride*y + maskIndex);
                    maskIndex++;
                }
                else
                    maskPattern = maskPattern >> 1;

                ARGB *dest = (ARGB*)((BYTE*)imageData.Scan0 + imageData.Stride*y + x*4);
                if ( (maskByte & maskPattern) == 0 )
                    *dest = 0x00000000;
                else
                {
                    Color c ;
                    interim.GetPixel(x,y,&c) ;
                    *dest = (c.GetValue() | Color::AlphaMask);
                }
            }
        }

        image->UnlockBits(&imageData);

        interimMask.UnlockBits(&dataMask);
        interim.UnlockBits(&dataMask);
    }
    else
    {
        image = Bitmap::FromHBITMAP((HBITMAP)bmp.GetHBITMAP(),
#if wxUSE_PALETTE
            (HPALETTE)bmp.GetPalette()->GetHPALETTE()
#else
            NULL
#endif
        );
        if ( bmp.HasAlpha() && GetPixelFormatSize(image->GetPixelFormat()) == 32 )
        {
            size_t width = image->GetWidth();
            size_t height = image->GetHeight();
            Rect bounds(0,0,width,height);
            static BitmapData data ;

            m_helper = image ;
            image = NULL ;
            m_helper->LockBits(&bounds, ImageLockModeRead,
                m_helper->GetPixelFormat(),&data);

            image = new Bitmap(data.Width, data.Height, data.Stride,
                PixelFormat32bppPARGB , (BYTE*) data.Scan0);

            m_helper->UnlockBits(&data);
        }
    }
    if ( image )
        m_bitmap = image;
}

#if wxUSE_IMAGE

wxImage wxGDIPlusBitmapData::ConvertToImage() const
{
    // We need to use Bitmap::LockBits() to convert bitmap to wxImage
    // because this way we can retrieve also alpha channel data.
    // Alternative way by retrieving bitmap handle with Bitmap::GetHBITMAP
    // (to pass it to wxBitmap) doesn't preserve real alpha channel data.
    const UINT w = m_bitmap->GetWidth();
    const UINT h = m_bitmap->GetHeight();

    wxImage img(w, h);
    // Set up wxImage buffer for alpha channel values
    // only if bitmap contains alpha channel.
    if ( IsAlphaPixelFormat(m_bitmap->GetPixelFormat()) )
    {
        img.InitAlpha();
    }

    BitmapData bitmapData;
    Rect rect(0, 0, w, h);
    m_bitmap->LockBits(&rect, ImageLockModeRead, PixelFormat32bppARGB, &bitmapData);

    unsigned char *imgRGB = img.GetData();    // destination RGB buffer
    unsigned char *imgAlpha = img.GetAlpha(); // destination alpha buffer
    const BYTE* pixels = static_cast<const BYTE*>(bitmapData.Scan0);
    for( UINT y = 0; y < h; y++ )
    {
        for( UINT x = 0; x < w; x++ )
        {
            ARGB c = reinterpret_cast<const ARGB*>(pixels)[x];
            *imgRGB++ = (c >> 16) & 0xFF;  // R
            *imgRGB++ = (c >> 8) & 0xFF;   // G
            *imgRGB++ = (c >> 0) & 0xFF;   // B
            if ( imgAlpha )
                *imgAlpha++ = (c >> 24) & 0xFF;
        }

        pixels += bitmapData.Stride;
    }
    m_bitmap->UnlockBits(&bitmapData);

    return img;
}

#endif // wxUSE_IMAGE

wxGDIPlusBitmapData::~wxGDIPlusBitmapData()
{
    delete m_bitmap;
    delete m_helper;
}

//-----------------------------------------------------------------------------
// wxGDIPlusPath implementation
//-----------------------------------------------------------------------------

wxGDIPlusPathData::wxGDIPlusPathData(wxGraphicsRenderer* renderer, GraphicsPath* path ) : wxGraphicsPathData(renderer)
    , m_logCurrentPointSet(false)
    , m_logCurrentPoint(0.0, 0.0)
    , m_figureOpened(false)
    , m_figureStart(0.0, 0.0)
{
    if ( path )
        m_path = path;
    else
        m_path = new GraphicsPath();
}

wxGDIPlusPathData::~wxGDIPlusPathData()
{
    delete m_path;
}

wxGraphicsObjectRefData* wxGDIPlusPathData::Clone() const
{
    wxGDIPlusPathData* newPathData =
                     new wxGDIPlusPathData(GetRenderer(), m_path->Clone());
    newPathData->m_logCurrentPointSet = m_logCurrentPointSet;
    newPathData->m_logCurrentPoint = m_logCurrentPoint;
    newPathData->m_figureOpened = m_figureOpened;
    newPathData->m_figureStart = m_figureStart;

    return newPathData;
}

//
// The Primitives
//

void wxGDIPlusPathData::MoveToPoint( wxDouble x , wxDouble y )
{
    m_path->StartFigure();
    m_figureOpened = true;
    m_figureStart = PointF((REAL)x, (REAL)y);
    // Since native current point is not updated in any way
    // we have to maintain current point location on our own in this case.
    m_logCurrentPoint = m_figureStart;
    m_logCurrentPointSet = true;
}

void wxGDIPlusPathData::AddLineToPoint( wxDouble x , wxDouble y )
{
    PointF start;
    if ( m_logCurrentPointSet )
    {
        start = m_logCurrentPoint;
    }
    else
    {
        Status st = m_path->GetLastPoint(&start);
        // If current point is not yet set then
        // this function should behave as MoveToPoint.
        if ( st != Ok )
        {
            MoveToPoint(x, y);
            return;
        }
    }
    m_path->AddLine(start.X, start.Y, (REAL)x, (REAL)y);
    // After calling AddLine() the native current point will be updated and can be used.
    m_logCurrentPointSet = false;
}

void wxGDIPlusPathData::CloseSubpath()
{
    if( m_figureOpened )
    {
        // Ensure that sub-path being closed contains at least one point.
        if ( m_logCurrentPointSet )
            m_path->AddLine(m_logCurrentPoint, m_logCurrentPoint);

        m_path->CloseFigure();
        m_figureOpened = false;
        // Since native GDI+ renderer doesn't move its current point
        // to the starting point of the figure we need to maintain
        // it on our own in this case.
        MoveToPoint(m_figureStart.X, m_figureStart.Y);
    }
}

void wxGDIPlusPathData::AddCurveToPoint( wxDouble cx1, wxDouble cy1, wxDouble cx2, wxDouble cy2, wxDouble x, wxDouble y )
{
    PointF c1(cx1,cy1);
    PointF c2(cx2,cy2);
    PointF end(x,y);
    PointF start;
    // If no current point is set then this function should behave
    // as if preceded by a call to MoveToPoint(cx1, cy1).
    if ( m_logCurrentPointSet )
    {
        start = m_logCurrentPoint;
    }
    else
    {
        if( m_path->GetLastPoint(&start) != Ok )
        {
            MoveToPoint(cx1, cy1);
            start = c1;
        }
    }
    m_path->AddBezier(start,c1,c2,end);
    // After calling AddBezier() the native current point will be updated and can be used.
    m_logCurrentPointSet = false;
}

// gets the last point of the current path, (0,0) if not yet set
void wxGDIPlusPathData::GetCurrentPoint( wxDouble* x, wxDouble* y) const
{
    PointF start;
    if ( m_logCurrentPointSet )
        start = m_logCurrentPoint;
    else
        m_path->GetLastPoint(&start);

    *x = start.X ;
    *y = start.Y ;
}

void wxGDIPlusPathData::AddArc( wxDouble x, wxDouble y, wxDouble r, double startAngle, double endAngle, bool clockwise )
{
    double angle;

    // For the sake of compatibility normalize angles the same way
    // as it is done in Cairo.
    if ( clockwise )
    {
        // If endAngle < startAngle it needs to be progressively
        // increased by 2*M_PI until endAngle > startAngle.
        if ( endAngle < startAngle )
        {
            while ( endAngle <= startAngle )
            {
                endAngle += 2.0*M_PI;
            }
        }

        angle = endAngle - startAngle;
    }
    else
    {
        // If endAngle > startAngle it needs to be progressively
        // decreased by 2*M_PI until endAngle < startAngle.
        if ( endAngle > startAngle )
        {
            while ( endAngle >= startAngle )
            {
                endAngle -= 2.0*M_PI;
            }
        }

        angle = startAngle - endAngle;
    }

    // To ensure compatibility with Cairo an initial
    // line segment to the beginning of the arc needs
    // to be added to the path.
    AddLineToPoint(r*cos(startAngle) + x, r*sin(startAngle) + y);

    // Native GraphicsPath.AddArc() does nothing
    // (even current point is not updated)
    // when sweep angle equals 0 so we can skip
    // any further actions.
    if ( angle == 0 )
    {
        return;
    }

    REAL x0 = (REAL)(x-r);
    REAL y0 = (REAL)(y-r);
    REAL dim = (REAL)(2*r);
    if ( angle >= 2.0*M_PI )
    {
        // In addition to arc we need to draw full circle(s).
        // Remarks:
        // 1. Parity of the number of the circles has to be
        // preserved because this matters when path would be
        // filled with wxODDEVEN_RULE flag set (using
        // FillModeAlternate mode) when number of the edges
        // is counted.
        // 2. With GraphicsPath.AddEllipse() we cannot
        // control the start point of the drawn circle
        // so we need to construct it from two arcs (halves).
        int numCircles = (int)(angle / (2.0*M_PI));
        numCircles = (numCircles - 1) % 2 + 1;
        for( int i = 0; i < numCircles; i++ )
        {
            m_path->AddArc(x0, y0, dim, dim,
                           wxRadToDeg(startAngle), clockwise ? 180 : -180);
            m_path->AddArc(x0, y0, dim, dim,
                           wxRadToDeg(startAngle+M_PI), clockwise ? 180 : -180);
        }
        // We need to reduce the angle to [0..2*M_PI) range
        angle = fmod(angle, 2.0*M_PI);
    }

    m_path->AddArc(x0, y0, dim, dim, wxRadToDeg(startAngle),
                   wxRadToDeg(clockwise ? angle : -angle));
   // After calling AddArc() the native current point will be updated and can be used.
   m_logCurrentPointSet = false;
}

void wxGDIPlusPathData::AddRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
    m_path->AddRectangle(RectF(x,y,w,h));
    // Drawn rectangle is an intrinsically closed shape but native
    // current point is not moved to the starting point of the figure
    // (the same case as with CloseFigure) so we need to maintain it
    // on our own in this case.
    MoveToPoint(x, y);
}

void wxGDIPlusPathData::AddCircle(wxDouble x, wxDouble y, wxDouble r)
{
    m_path->AddEllipse((REAL)(x-r), (REAL)(y-r), (REAL)(2.0*r), (REAL)(2.0*r));
    // After calling AddEllipse() the native current point will be updated and can be used.
    m_logCurrentPointSet = false;
}

void wxGDIPlusPathData::AddEllipse(wxDouble x, wxDouble y, wxDouble w, wxDouble h)
{
    m_path->AddEllipse((REAL)x, (REAL)y, (REAL)w, (REAL)h);
    // After calling AddEllipse() the native current point will be updated and can be used.
    m_logCurrentPointSet = false;
}

void wxGDIPlusPathData::AddPath( const wxGraphicsPathData* path )
{
    const wxGDIPlusPathData* pathData = static_cast<const wxGDIPlusPathData*>(path);
    const GraphicsPath* grPath = static_cast<const GraphicsPath*>(pathData->GetNativePath());

    m_path->AddPath(grPath, FALSE);
    // Copy auxiliary data if appended path is non-empty.
    if( grPath->GetPointCount() > 0 || pathData->m_logCurrentPointSet || pathData->m_figureOpened )
    {
        m_logCurrentPointSet = pathData->m_logCurrentPointSet;
        m_logCurrentPoint = pathData->m_logCurrentPoint;
        m_figureOpened = pathData->m_figureOpened;
        m_figureStart = pathData->m_figureStart;
    }
}

// transforms each point of this path by the matrix
void wxGDIPlusPathData::Transform( const wxGraphicsMatrixData* matrix )
{
    const Matrix* m = static_cast<const Matrix*>(matrix->GetNativeMatrix());
    m_path->Transform(m);
    // Transform also auxiliary points.
    if ( m_logCurrentPointSet )
        m->TransformPoints(&m_logCurrentPoint, 1);
    if ( m_figureOpened )
        m->TransformPoints(&m_figureStart, 1);
}

// gets the bounding box enclosing all points (possibly including control points)
void wxGDIPlusPathData::GetBox(wxDouble *x, wxDouble *y, wxDouble *w, wxDouble *h) const
{
    RectF bounds;
    m_path->GetBounds( &bounds, NULL, NULL) ;
    *x = bounds.X;
    *y = bounds.Y;
    *w = bounds.Width;
    *h = bounds.Height;
}

bool wxGDIPlusPathData::Contains( wxDouble x, wxDouble y, wxPolygonFillMode fillStyle ) const
{
    m_path->SetFillMode( fillStyle == wxODDEVEN_RULE ? FillModeAlternate : FillModeWinding);
    return m_path->IsVisible( (FLOAT) x,(FLOAT) y) == TRUE ;
}

//-----------------------------------------------------------------------------
// wxGDIPlusMatrixData implementation
//-----------------------------------------------------------------------------

wxGDIPlusMatrixData::wxGDIPlusMatrixData(wxGraphicsRenderer* renderer, Matrix* matrix )
    : wxGraphicsMatrixData(renderer)
{
    if ( matrix )
        m_matrix = matrix ;
    else
        m_matrix = new Matrix();
}

wxGDIPlusMatrixData::~wxGDIPlusMatrixData()
{
    delete m_matrix;
}

wxGraphicsObjectRefData *wxGDIPlusMatrixData::Clone() const
{
    return new wxGDIPlusMatrixData( GetRenderer(), m_matrix->Clone());
}

// concatenates the matrix
void wxGDIPlusMatrixData::Concat( const wxGraphicsMatrixData *t )
{
    m_matrix->Multiply( (Matrix*) t->GetNativeMatrix());
}

// sets the matrix to the respective values
void wxGDIPlusMatrixData::Set(wxDouble a, wxDouble b, wxDouble c, wxDouble d,
                 wxDouble tx, wxDouble ty)
{
    m_matrix->SetElements(a,b,c,d,tx,ty);
}

// gets the component valuess of the matrix
void wxGDIPlusMatrixData::Get(wxDouble* a, wxDouble* b,  wxDouble* c,
                              wxDouble* d, wxDouble* tx, wxDouble* ty) const
{
    REAL elements[6];
    m_matrix->GetElements(elements);
    if (a)  *a = elements[0];
    if (b)  *b = elements[1];
    if (c)  *c = elements[2];
    if (d)  *d = elements[3];
    if (tx) *tx= elements[4];
    if (ty) *ty= elements[5];
}

// makes this the inverse matrix
void wxGDIPlusMatrixData::Invert()
{
    m_matrix->Invert();
}

// returns true if the elements of the transformation matrix are equal ?
bool wxGDIPlusMatrixData::IsEqual( const wxGraphicsMatrixData* t) const
{
    return m_matrix->Equals((Matrix*) t->GetNativeMatrix())== TRUE ;
}

// return true if this is the identity matrix
bool wxGDIPlusMatrixData::IsIdentity() const
{
    return m_matrix->IsIdentity() == TRUE ;
}

//
// transformation
//

// add the translation to this matrix
void wxGDIPlusMatrixData::Translate( wxDouble dx , wxDouble dy )
{
    m_matrix->Translate(dx,dy);
}

// add the scale to this matrix
void wxGDIPlusMatrixData::Scale( wxDouble xScale , wxDouble yScale )
{
    m_matrix->Scale(xScale,yScale);
}

// add the rotation to this matrix (radians)
void wxGDIPlusMatrixData::Rotate( wxDouble angle )
{
    m_matrix->Rotate( wxRadToDeg(angle) );
}

//
// apply the transforms
//

// applies that matrix to the point
void wxGDIPlusMatrixData::TransformPoint( wxDouble *x, wxDouble *y ) const
{
    PointF pt(*x,*y);
    m_matrix->TransformPoints(&pt);
    *x = pt.X;
    *y = pt.Y;
}

// applies the matrix except for translations
void wxGDIPlusMatrixData::TransformDistance( wxDouble *dx, wxDouble *dy ) const
{
    PointF pt(*dx,*dy);
    m_matrix->TransformVectors(&pt);
    *dx = pt.X;
    *dy = pt.Y;
}

// returns the native representation
void * wxGDIPlusMatrixData::GetNativeMatrix() const
{
    return m_matrix;
}

//-----------------------------------------------------------------------------
// wxGDIPlusContext implementation
//-----------------------------------------------------------------------------

class wxGDIPlusOffsetHelper
{
public :
    wxGDIPlusOffsetHelper( Graphics* gr , bool offset )
    {
        m_gr = gr;
        m_offset = offset;
        if ( m_offset )
            m_gr->TranslateTransform( 0.5, 0.5 );
    }
    ~wxGDIPlusOffsetHelper( )
    {
        if ( m_offset )
            m_gr->TranslateTransform( -0.5, -0.5 );
    }
public :
    Graphics* m_gr;
    bool m_offset;
} ;

wxGDIPlusContext::wxGDIPlusContext( wxGraphicsRenderer* renderer, HDC hdc, wxDouble width, wxDouble height   )
    : wxGraphicsContext(renderer)
{
    Init(new Graphics(hdc), width, height);
}

wxGDIPlusContext::wxGDIPlusContext( wxGraphicsRenderer* renderer, const wxDC& dc )
    : wxGraphicsContext(renderer, dc.GetWindow())
{
    wxMSWDCImpl *msw = wxDynamicCast( dc.GetImpl() , wxMSWDCImpl );
    HDC hdc = (HDC) msw->GetHDC();
    wxSize sz = dc.GetSize();

    Init(new Graphics(hdc), sz.x, sz.y);
}

wxGDIPlusContext::wxGDIPlusContext( wxGraphicsRenderer* renderer,
                                    HWND hwnd,
                                    wxWindow* window )
    : wxGraphicsContext(renderer, window)
{
    RECT rect = wxGetWindowRect(hwnd);
    Init(new Graphics(hwnd), rect.right - rect.left, rect.bottom - rect.top);
    m_enableOffset = true;
}

wxGDIPlusContext::wxGDIPlusContext( wxGraphicsRenderer* renderer, Graphics* gr  )
    : wxGraphicsContext(renderer)
{
    Init(gr, 0, 0);
}

wxGDIPlusContext::wxGDIPlusContext(wxGraphicsRenderer* renderer)
    : wxGraphicsContext(renderer)
{
    // Derived class must call Init() later but just set m_context to NULL for
    // safety to avoid crashing in our dtor if Init() ends up not being called.
    m_context = NULL;
}

void wxGDIPlusContext::Init(Graphics* graphics, int width, int height)
{
    m_context = graphics;
    m_state1 = 0;
    m_state2 = 0;
    m_width = width;
    m_height = height;

    m_context->SetTextRenderingHint(TextRenderingHintSystemDefault);
    m_context->SetPixelOffsetMode(PixelOffsetModeHalf);
    m_context->SetSmoothingMode(SmoothingModeHighQuality);

    m_state1 = m_context->Save();
    m_state2 = m_context->Save();
}

wxGDIPlusContext::~wxGDIPlusContext()
{
    if ( m_context )
    {
        m_context->Restore( m_state2 );
        m_context->Restore( m_state1 );
        delete m_context;
    }
}


void wxGDIPlusContext::Clip( const wxRegion &region )
{
    Region rgn((HRGN)region.GetHRGN());
    m_context->SetClip(&rgn,CombineModeIntersect);
}

void wxGDIPlusContext::Clip( wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
    m_context->SetClip(RectF(x,y,w,h),CombineModeIntersect);
}

void wxGDIPlusContext::ResetClip()
{
    m_context->ResetClip();
}

void wxGDIPlusContext::GetClipBox(wxDouble* x, wxDouble* y, wxDouble* w, wxDouble* h)
{
    RectF r;
    m_context->SetPixelOffsetMode(PixelOffsetModeNone);
    m_context->GetVisibleClipBounds(&r);
    m_context->SetPixelOffsetMode(PixelOffsetModeHalf);
    // Check if we have an empty clipping box.
    if ( r.Width <= REAL_MIN || r.Height <= REAL_MIN )
        r.X = r.Y = r.Width = r.Height = 0.0F;

    if ( x )
        *x = r.X;
    if ( y )
        *y = r.Y;
    if ( w )
        *w = r.Width;
    if ( h )
        *h = r.Height;
}

void wxGDIPlusContext::DrawRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
    if (m_composition == wxCOMPOSITION_DEST)
        return;

    wxGDIPlusOffsetHelper helper( m_context , ShouldOffset() );
    Brush *brush = m_brush.IsNull() ? NULL : ((wxGDIPlusBrushData*)m_brush.GetRefData())->GetGDIPlusBrush();
    Pen *pen = m_pen.IsNull() ? NULL : ((wxGDIPlusPenData*)m_pen.GetGraphicsData())->GetGDIPlusPen();

    if ( w < 0 )
    {
        x += w;
        w = -w;
    }

    if ( h < 0 )
    {
        y += h;
        h = -h;
    }

    if ( brush )
    {
        // the offset is used to fill only the inside of the rectangle and not paint underneath
        // its border which may influence a transparent Pen
        REAL offset = 0;
        if ( pen )
             offset = pen->GetWidth();
        m_context->FillRectangle( brush, (REAL)x + offset/2, (REAL)y + offset/2, (REAL)w - offset, (REAL)h - offset);
    }

    if ( pen )
    {
        m_context->DrawRectangle( pen, (REAL)x, (REAL)y, (REAL)w, (REAL)h );
    }
}

void wxGDIPlusContext::StrokeLines( size_t n, const wxPoint2DDouble *points)
{
   if (m_composition == wxCOMPOSITION_DEST)
        return;

   if ( !m_pen.IsNull() )
   {
       wxGDIPlusOffsetHelper helper( m_context , ShouldOffset() );
       PointF *cpoints = new PointF[n];
       for (size_t i = 0; i < n; i++)
       {
           cpoints[i].X = static_cast<REAL>(points[i].m_x);
           cpoints[i].Y = static_cast<REAL>(points[i].m_y);

       } // for (size_t i = 0; i < n; i++)
       m_context->DrawLines( ((wxGDIPlusPenData*)m_pen.GetGraphicsData())->GetGDIPlusPen() , cpoints , n ) ;
       delete[] cpoints;
   }
}

void wxGDIPlusContext::DrawLines( size_t n, const wxPoint2DDouble *points, wxPolygonFillMode WXUNUSED(fillStyle) )
{
   if (m_composition == wxCOMPOSITION_DEST)
        return;

    wxGDIPlusOffsetHelper helper( m_context , ShouldOffset() );
    PointF *cpoints = new PointF[n];
    for (size_t i = 0; i < n; i++)
    {
        cpoints[i].X = static_cast<REAL>(points[i].m_x);
        cpoints[i].Y = static_cast<REAL>(points[i].m_y);

    } // for (int i = 0; i < n; i++)
    if ( !m_brush.IsNull() )
        m_context->FillPolygon( ((wxGDIPlusBrushData*)m_brush.GetRefData())->GetGDIPlusBrush() , cpoints , n ) ;
    if ( !m_pen.IsNull() )
        m_context->DrawLines( ((wxGDIPlusPenData*)m_pen.GetGraphicsData())->GetGDIPlusPen() , cpoints , n ) ;
    delete[] cpoints;
}

void wxGDIPlusContext::StrokePath( const wxGraphicsPath& path )
{
   if (m_composition == wxCOMPOSITION_DEST)
        return;

    if ( !m_pen.IsNull() )
    {
        wxGDIPlusOffsetHelper helper( m_context , ShouldOffset() );
        m_context->DrawPath( ((wxGDIPlusPenData*)m_pen.GetGraphicsData())->GetGDIPlusPen() , (GraphicsPath*) path.GetNativePath() );
    }
}

void wxGDIPlusContext::FillPath( const wxGraphicsPath& path , wxPolygonFillMode fillStyle )
{
   if (m_composition == wxCOMPOSITION_DEST)
        return;

    if ( !m_brush.IsNull() )
    {
        wxGDIPlusOffsetHelper helper( m_context , ShouldOffset() );
        ((GraphicsPath*) path.GetNativePath())->SetFillMode( fillStyle == wxODDEVEN_RULE ? FillModeAlternate : FillModeWinding);
        m_context->FillPath( ((wxGDIPlusBrushData*)m_brush.GetRefData())->GetGDIPlusBrush() ,
            (GraphicsPath*) path.GetNativePath());
    }
}

bool wxGDIPlusContext::SetAntialiasMode(wxAntialiasMode antialias)
{
    if (m_antialias == antialias)
        return true;

    // MinGW currently doesn't provide InterpolationModeInvalid in its headers,
    // so use our own definition.
    static const SmoothingMode
        wxSmoothingModeInvalid = static_cast<SmoothingMode>(-1);

    SmoothingMode antialiasMode = wxSmoothingModeInvalid;
    switch (antialias)
    {
        case wxANTIALIAS_DEFAULT:
            antialiasMode = SmoothingModeHighQuality;
            break;

        case wxANTIALIAS_NONE:
            antialiasMode = SmoothingModeNone;
            break;
    }

    wxCHECK_MSG( antialiasMode != wxSmoothingModeInvalid, false,
                 wxS("Unknown antialias mode") );

    if ( m_context->SetSmoothingMode(antialiasMode) != Gdiplus::Ok )
        return false;

    m_antialias = antialias;

    return true;
}

bool wxGDIPlusContext::SetInterpolationQuality(wxInterpolationQuality interpolation)
{
    if (m_interpolation == interpolation)
        return true;

    // MinGW currently doesn't provide InterpolationModeInvalid in its headers,
    // so use our own definition.
    static const InterpolationMode
        wxInterpolationModeInvalid = static_cast<InterpolationMode>(-1);

    InterpolationMode interpolationMode = wxInterpolationModeInvalid;
    switch (interpolation)
    {
        case wxINTERPOLATION_DEFAULT:
            interpolationMode = InterpolationModeDefault;
            break;

        case wxINTERPOLATION_NONE:
            interpolationMode = InterpolationModeNearestNeighbor;
            break;

        case wxINTERPOLATION_FAST:
            interpolationMode = InterpolationModeLowQuality;
            break;

        case wxINTERPOLATION_GOOD:
            interpolationMode = InterpolationModeHighQuality;
            break;

        case wxINTERPOLATION_BEST:
            interpolationMode = InterpolationModeHighQualityBicubic;
            break;
    }

    wxCHECK_MSG( interpolationMode != wxInterpolationModeInvalid, false,
                 wxS("Unknown interpolation mode") );

    if ( m_context->SetInterpolationMode(interpolationMode) != Gdiplus::Ok )
        return false;

    m_interpolation = interpolation;

    return true;
}

bool wxGDIPlusContext::SetCompositionMode(wxCompositionMode op)
{
    if ( m_composition == op )
        return true;

    m_composition = op;

    if (m_composition == wxCOMPOSITION_DEST)
        return true;

    CompositingMode cop;
    switch (op)
    {
        case wxCOMPOSITION_SOURCE:
            cop = CompositingModeSourceCopy;
            break;
        case wxCOMPOSITION_OVER:
            cop = CompositingModeSourceOver;
            break;
        default:
            return false;
    }

    m_context->SetCompositingMode(cop);
    return true;
}

void wxGDIPlusContext::BeginLayer(wxDouble /* opacity */)
{
    // TODO
}

void wxGDIPlusContext::EndLayer()
{
    // TODO
}

void wxGDIPlusContext::Rotate( wxDouble angle )
{
    m_context->RotateTransform( wxRadToDeg(angle) );
}

void wxGDIPlusContext::Translate( wxDouble dx , wxDouble dy )
{
    m_context->TranslateTransform( dx , dy );
}

void wxGDIPlusContext::Scale( wxDouble xScale , wxDouble yScale )
{
    m_context->ScaleTransform(xScale,yScale);
}

void wxGDIPlusContext::PushState()
{
    GraphicsState state = m_context->Save();
    m_stateStack.push(state);
}

void wxGDIPlusContext::PopState()
{
    wxCHECK_RET( !m_stateStack.empty(), wxT("No state to pop") );

    GraphicsState state = m_stateStack.top();
    m_stateStack.pop();
    m_context->Restore(state);
}

void wxGDIPlusContext::DrawBitmap( const wxGraphicsBitmap &bmp, wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
   if (m_composition == wxCOMPOSITION_DEST)
        return;

    Bitmap* image = static_cast<wxGDIPlusBitmapData*>(bmp.GetRefData())->GetGDIPlusBitmap();
    if ( image )
    {
        if( image->GetWidth() != (UINT) w || image->GetHeight() != (UINT) h )
        {
            Rect drawRect((REAL) x, (REAL)y, (REAL)w, (REAL)h);
            m_context->SetPixelOffsetMode( PixelOffsetModeNone );
            m_context->DrawImage(image, drawRect, 0 , 0 , image->GetWidth(), image->GetHeight(), UnitPixel ) ;
            m_context->SetPixelOffsetMode( PixelOffsetModeHalf );
        }
        else
            m_context->DrawImage(image,(REAL) x,(REAL) y,(REAL) w,(REAL) h) ;
    }
}

void wxGDIPlusContext::DrawBitmap( const wxBitmap &bmp, wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
    wxGraphicsBitmap bitmap = GetRenderer()->CreateBitmap(bmp);
    DrawBitmap(bitmap, x, y, w, h);
}

void wxGDIPlusContext::DrawIcon( const wxIcon &icon, wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
   if (m_composition == wxCOMPOSITION_DEST)
        return;

    // the built-in conversion fails when there is alpha in the HICON (eg XP style icons), we can only
    // find out by looking at the bitmap data whether there really was alpha in it
    HICON hIcon = (HICON)icon.GetHICON();
    AutoIconInfo iconInfo ;
    if (!iconInfo.GetFrom(hIcon))
        return;

    Bitmap interim(iconInfo.hbmColor,NULL);

    Bitmap* image = NULL ;

    // if it's not 32 bit, it doesn't have an alpha channel, note that since the conversion doesn't
    // work correctly, asking IsAlphaPixelFormat at this point fails as well
    if( GetPixelFormatSize(interim.GetPixelFormat())!= 32 )
    {
        image = Bitmap::FromHICON(hIcon);
    }
    else
    {
        size_t width = interim.GetWidth();
        size_t height = interim.GetHeight();
        Rect bounds(0,0,width,height);
        BitmapData data ;

        interim.LockBits(&bounds, ImageLockModeRead,
            interim.GetPixelFormat(),&data);

        bool hasAlpha = false;
        for ( size_t yy = 0 ; yy < height && !hasAlpha ; ++yy)
        {
            for( size_t xx = 0 ; xx < width && !hasAlpha; ++xx)
            {
                ARGB *dest = (ARGB*)((BYTE*)data.Scan0 + data.Stride*yy + xx*4);
                if ( ( *dest & Color::AlphaMask ) != 0 )
                    hasAlpha = true;
            }
        }

        if ( hasAlpha )
        {
        image = new Bitmap(data.Width, data.Height, data.Stride,
            PixelFormat32bppARGB , (BYTE*) data.Scan0);
        }
        else
        {
            image = Bitmap::FromHICON(hIcon);
        }

        interim.UnlockBits(&data);
    }

    m_context->DrawImage(image,(REAL) x,(REAL) y,(REAL) w,(REAL) h) ;

    delete image ;
}

void wxGDIPlusContext::DoDrawText(const wxString& str,
                                        wxDouble x, wxDouble y )
{
   if (m_composition == wxCOMPOSITION_DEST)
        return;

    wxCHECK_RET( !m_font.IsNull(),
                 wxT("wxGDIPlusContext::DrawText - no valid font set") );

    if ( str.IsEmpty())
        return ;

    wxGDIPlusFontData * const
        fontData = (wxGDIPlusFontData *)m_font.GetRefData();

    m_context->DrawString
               (
                    str.wc_str(*wxConvUI),  // string to draw, always Unicode
                    -1,                     // length: string is NUL-terminated
                    fontData->GetGDIPlusFont(),
                    PointF(x, y),
                    GetDrawTextStringFormat(),
                    fontData->GetGDIPlusBrush()
               );
}

void wxGDIPlusContext::GetTextExtent( const wxString &str, wxDouble *width, wxDouble *height,
                                     wxDouble *descent, wxDouble *externalLeading ) const
{
    wxCHECK_RET( !m_font.IsNull(), wxT("wxGDIPlusContext::GetTextExtent - no valid font set") );

    wxWCharBuffer s = str.wc_str( *wxConvUI );
    Font* f = ((wxGDIPlusFontData*)m_font.GetRefData())->GetGDIPlusFont();

    // Get the font metrics if we actually need them.
    if ( descent || externalLeading || (height && str.empty()) )
    {
        FontFamily ffamily ;
        f->GetFamily(&ffamily) ;

        // Notice that we must use the real font style or the results would be
        // incorrect for italic/bold fonts.
        const INT style = f->GetStyle();
        const REAL size = f->GetSize();
        const REAL emHeight = ffamily.GetEmHeight(style);
        REAL rDescent = ffamily.GetCellDescent(style) * size / emHeight;
        REAL rAscent = ffamily.GetCellAscent(style) * size / emHeight;
        REAL rHeight = ffamily.GetLineSpacing(style) * size / emHeight;

        if ( height && str.empty() )
            *height = rHeight;
        if ( descent )
            *descent = rDescent;
        if ( externalLeading )
            *externalLeading = rHeight - rAscent - rDescent;
    }

    // measuring empty strings is not guaranteed, so do it by hand
    if ( str.IsEmpty())
    {
        if ( width )
            *width = 0 ;

        // Height already assigned above if necessary.
    }
    else
    {
        RectF layoutRect(0,0, 100000.0f, 100000.0f);

        RectF bounds ;
        m_context->MeasureString((const wchar_t *) s , wcslen(s) , f, layoutRect, GetDrawTextStringFormat(), &bounds ) ;
        if ( width )
            *width = bounds.Width;
        if ( height )
            *height = bounds.Height;
    }
}

void wxGDIPlusContext::GetPartialTextExtents(const wxString& text, wxArrayDouble& widths) const
{
    widths.Empty();
    widths.Add(0, text.length());

    wxCHECK_RET( !m_font.IsNull(), wxT("wxGDIPlusContext::GetPartialTextExtents - no valid font set") );

    if (text.empty())
        return;

    Font* f = ((wxGDIPlusFontData*)m_font.GetRefData())->GetGDIPlusFont();
    wxWCharBuffer ws = text.wc_str( *wxConvUI );
    size_t len = wcslen( ws ) ;
    wxASSERT_MSG(text.length() == len , wxT("GetPartialTextExtents not yet implemented for multichar situations"));

    RectF layoutRect(0,0, 100000.0f, 100000.0f);
    StringFormat strFormat( GetDrawTextStringFormat() );

    size_t startPosition = 0;
    size_t remainder = len;
    const size_t maxSpan = 32;
    CharacterRange* ranges = new CharacterRange[maxSpan] ;
    Region* regions = new Region[maxSpan];

    while( remainder > 0 )
    {
        size_t span = wxMin( maxSpan, remainder );

        for( size_t i = 0 ; i < span ; ++i)
        {
            ranges[i].First = 0 ;
            ranges[i].Length = startPosition+i+1 ;
        }
        strFormat.SetMeasurableCharacterRanges(span,ranges);
        m_context->MeasureCharacterRanges(ws, -1 , f,layoutRect, &strFormat,span,regions) ;

        RectF bbox ;
        for ( size_t i = 0 ; i < span ; ++i)
        {
            regions[i].GetBounds(&bbox,m_context);
            widths[startPosition+i] = bbox.Width;
        }
        remainder -= span;
        startPosition += span;
    }

    delete[] ranges;
    delete[] regions;
}

bool wxGDIPlusContext::ShouldOffset() const
{
    if ( !m_enableOffset )
        return false;

    int penwidth = 0 ;
    if ( !m_pen.IsNull() )
    {
        penwidth = (int)((wxGDIPlusPenData*)m_pen.GetRefData())->GetWidth();
        if ( penwidth == 0 )
            penwidth = 1;
    }
    return ( penwidth % 2 ) == 1;
}

void* wxGDIPlusContext::GetNativeContext()
{
    return m_context;
}

// concatenates this transform with the current transform of this context
void wxGDIPlusContext::ConcatTransform( const wxGraphicsMatrix& matrix )
{
    m_context->MultiplyTransform((Matrix*) matrix.GetNativeMatrix());
}

// sets the transform of this context
void wxGDIPlusContext::SetTransform( const wxGraphicsMatrix& matrix )
{
    m_context->SetTransform((Matrix*) matrix.GetNativeMatrix());
}

// gets the matrix of this context
wxGraphicsMatrix wxGDIPlusContext::GetTransform() const
{
    wxGraphicsMatrix matrix = CreateMatrix();
    m_context->GetTransform((Matrix*) matrix.GetNativeMatrix());
    return matrix;
}

void wxGDIPlusContext::GetSize( wxDouble* width, wxDouble *height )
{
    *width = m_width;
    *height = m_height;
}

//-----------------------------------------------------------------------------
// wxGDIPlusPrintingContext implementation
//-----------------------------------------------------------------------------

wxGDIPlusPrintingContext::wxGDIPlusPrintingContext( wxGraphicsRenderer* renderer,
                                                    const wxDC& dc )
    : wxGDIPlusContext(renderer, dc)
{
    Graphics* context = GetGraphics();

    //m_context->SetPageUnit(UnitDocument);

    // Setup page scale, based on DPI ratio.
    // Antecedent should be 100dpi when the default page unit
    // (UnitDisplay) is used. Page unit UnitDocument would require 300dpi
    // instead. Note that calling SetPageScale() does not have effect on
    // non-printing DCs (that is, any other than wxPrinterDC or
    // wxEnhMetaFileDC).
    REAL dpiRatio = 100.0 / context->GetDpiY();
    context->SetPageScale(dpiRatio);

    // We use this modifier when measuring fonts. It is needed because the
    // page scale is modified above.
    m_fontScaleRatio = context->GetDpiY() / 96.0;
}

//-----------------------------------------------------------------------------
// wxGDIPlusRenderer implementation
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxGDIPlusRenderer, wxGraphicsRenderer);

static wxGDIPlusRenderer gs_GDIPlusRenderer;

wxGraphicsRenderer* wxGraphicsRenderer::GetGDIPlusRenderer()
{
    return &gs_GDIPlusRenderer;
}

wxGraphicsRenderer* wxGraphicsRenderer::GetDefaultRenderer()
{
    return wxGraphicsRenderer::GetGDIPlusRenderer();
}

bool wxGDIPlusRenderer::EnsureIsLoaded()
{
    // load gdiplus.dll if not yet loaded, but don't bother doing it again
    // if we already tried and failed (we don't want to spend lot of time
    // returning NULL from wxGraphicsContext::Create(), which may be called
    // relatively frequently):
    if ( m_loaded == -1 )
    {
        Load();
    }

    return m_loaded == 1;
}

// call EnsureIsLoaded() and return returnOnFail value if it fails
#define ENSURE_LOADED_OR_RETURN(returnOnFail)  \
    if ( !EnsureIsLoaded() )                   \
        return (returnOnFail)


void wxGDIPlusRenderer::Load()
{
    GdiplusStartupInput input;
    GdiplusStartupOutput output;
    if ( GdiplusStartup(&m_gditoken,&input,&output) == Gdiplus::Ok )
    {
        wxLogTrace("gdiplus", "successfully initialized GDI+");
        m_loaded = 1;

#if wxUSE_PRIVATE_FONTS
        // Make private fonts available to GDI+, if any.
        const wxArrayString& privateFonts = wxGetPrivateFontFileNames();
        const size_t n = privateFonts.size();
        if ( n )
        {
            gs_privateFonts = new Gdiplus::PrivateFontCollection();
            for ( size_t i = 0 ; i < n; i++ )
            {
                const wxString& fname = privateFonts[i];
                gs_privateFonts->AddFontFile(fname.wc_str());
            }

            gs_pFontFamily = new Gdiplus::FontFamily[n];
        }
#endif // wxUSE_PRIVATE_FONTS
    }
    else
    {
        wxLogTrace("gdiplus", "failed to initialize GDI+, missing gdiplus.dll?");
        m_loaded = 0;
    }
}

void wxGDIPlusRenderer::Unload()
{
    if ( m_gditoken )
    {
        GdiplusShutdown(m_gditoken);
        m_gditoken = 0;

#if wxUSE_PRIVATE_FONTS
        if ( gs_privateFonts )
        {
            delete gs_privateFonts;
            gs_privateFonts = NULL;

            delete[] gs_pFontFamily;
            gs_pFontFamily = NULL;
        }
#endif // wxUSE_PRIVATE_FONTS
    }
    m_loaded = -1; // next Load() will try again
}

wxGraphicsContext * wxGDIPlusRenderer::CreateContext( const wxWindowDC& dc)
{
    ENSURE_LOADED_OR_RETURN(NULL);
    wxGDIPlusContext* context = new wxGDIPlusContext(this, dc);
    context->EnableOffset(true);
    return context;
}

#if wxUSE_PRINTING_ARCHITECTURE
wxGraphicsContext * wxGDIPlusRenderer::CreateContext( const wxPrinterDC& dc)
{
    ENSURE_LOADED_OR_RETURN(NULL);
    wxGDIPlusContext* context = new wxGDIPlusPrintingContext(this, dc);
    return context;
}
#endif

#if wxUSE_ENH_METAFILE
wxGraphicsContext * wxGDIPlusRenderer::CreateContext( const wxEnhMetaFileDC& dc)
{
    ENSURE_LOADED_OR_RETURN(NULL);
    wxGDIPlusContext* context = new wxGDIPlusPrintingContext(this, dc);
    return context;
}
#endif

wxGraphicsContext * wxGDIPlusRenderer::CreateContext( const wxMemoryDC& dc)
{
    ENSURE_LOADED_OR_RETURN(NULL);
#if wxUSE_WXDIB
    // It seems that GDI+ sets invalid values for alpha channel when used with
    // a compatible bitmap (DDB). So we need to convert the currently selected
    // bitmap to a DIB before using it with any GDI+ functions to ensure that
    // we get the correct alpha channel values in it at the end.

    wxBitmap bmp = dc.GetSelectedBitmap();
    wxASSERT_MSG( bmp.IsOk(), "Should select a bitmap before creating wxGCDC" );

    // We don't need to convert it if it can't have alpha at all (any depth but
    // 32) or is already a DIB with alpha.
    if ( bmp.GetDepth() == 32 && (!bmp.IsDIB() || !bmp.HasAlpha()) )
    {
        // We need to temporarily deselect this bitmap from the memory DC
        // before modifying it.
        const_cast<wxMemoryDC&>(dc).SelectObject(wxNullBitmap);

        bmp.ConvertToDIB(); // Does nothing if already a DIB.

        if( !bmp.HasAlpha() )
        {
            // Initialize alpha channel, even if we don't have any alpha yet,
            // we should have correct (opaque) alpha values in it for GDI+
            // functions to work correctly.
            {
                wxAlphaPixelData data(bmp);
                if ( data )
                {
                    wxAlphaPixelData::Iterator p(data);
                    for ( int y = 0; y < data.GetHeight(); y++ )
                    {
                        wxAlphaPixelData::Iterator rowStart = p;

                        for ( int x = 0; x < data.GetWidth(); x++ )
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
    }
#endif // wxUSE_WXDIB

    wxGDIPlusContext* context = new wxGDIPlusContext(this, dc);
    context->EnableOffset(true);
    return context;
}

#if wxUSE_IMAGE
wxGraphicsContext * wxGDIPlusRenderer::CreateContextFromImage(wxImage& image)
{
    ENSURE_LOADED_OR_RETURN(NULL);
    wxGDIPlusContext* context = new wxGDIPlusImageContext(this, image);
    context->EnableOffset(true);
    return context;
}

#endif // wxUSE_IMAGE

wxGraphicsContext * wxGDIPlusRenderer::CreateMeasuringContext()
{
    ENSURE_LOADED_OR_RETURN(NULL);
    return new wxGDIPlusMeasuringContext(this);
}

wxGraphicsContext * wxGDIPlusRenderer::CreateContextFromNativeContext( void * context )
{
    ENSURE_LOADED_OR_RETURN(NULL);
    return new wxGDIPlusContext(this,(Graphics*) context);
}


wxGraphicsContext * wxGDIPlusRenderer::CreateContextFromNativeWindow( void * window )
{
    ENSURE_LOADED_OR_RETURN(NULL);
    return new wxGDIPlusContext(this,(HWND) window);
}

wxGraphicsContext * wxGDIPlusRenderer::CreateContextFromNativeHDC(WXHDC dc)
{
    ENSURE_LOADED_OR_RETURN(NULL);
    return new wxGDIPlusContext(this, new Graphics((HDC)dc));
}

wxGraphicsContext * wxGDIPlusRenderer::CreateContext( wxWindow* window )
{
    ENSURE_LOADED_OR_RETURN(NULL);
    return new wxGDIPlusContext(this, (HWND) window->GetHWND(), window );
}

// Path

wxGraphicsPath wxGDIPlusRenderer::CreatePath()
{
    ENSURE_LOADED_OR_RETURN(wxNullGraphicsPath);
    wxGraphicsPath m;
    m.SetRefData( new wxGDIPlusPathData(this));
    return m;
}


// Matrix

wxGraphicsMatrix wxGDIPlusRenderer::CreateMatrix( wxDouble a, wxDouble b, wxDouble c, wxDouble d,
                                                           wxDouble tx, wxDouble ty)

{
    ENSURE_LOADED_OR_RETURN(wxNullGraphicsMatrix);
    wxGraphicsMatrix m;
    wxGDIPlusMatrixData* data = new wxGDIPlusMatrixData( this );
    data->Set( a,b,c,d,tx,ty ) ;
    m.SetRefData(data);
    return m;
}

wxGraphicsPen wxGDIPlusRenderer::CreatePen(const wxGraphicsPenInfo& info)
{
    ENSURE_LOADED_OR_RETURN(wxNullGraphicsPen);
    if ( info.GetStyle() == wxPENSTYLE_TRANSPARENT )
        return wxNullGraphicsPen;
    else
    {
        wxGraphicsPen p;
        p.SetRefData(new wxGDIPlusPenData( this, info ));
        return p;
    }
}

wxGraphicsBrush wxGDIPlusRenderer::CreateBrush(const wxBrush& brush )
{
    ENSURE_LOADED_OR_RETURN(wxNullGraphicsBrush);
    if ( !brush.IsOk() || brush.GetStyle() == wxBRUSHSTYLE_TRANSPARENT )
        return wxNullGraphicsBrush;
    else
    {
        wxGraphicsBrush p;
        p.SetRefData(new wxGDIPlusBrushData( this, brush ));
        return p;
    }
}

wxGraphicsBrush
wxGDIPlusRenderer::CreateLinearGradientBrush(wxDouble x1, wxDouble y1,
                                             wxDouble x2, wxDouble y2,
                                             const wxGraphicsGradientStops& stops)
{
    ENSURE_LOADED_OR_RETURN(wxNullGraphicsBrush);
    wxGraphicsBrush p;
    wxGDIPlusBrushData* d = new wxGDIPlusBrushData( this );
    d->CreateLinearGradientBrush(x1, y1, x2, y2, stops);
    p.SetRefData(d);
    return p;
 }

wxGraphicsBrush
wxGDIPlusRenderer::CreateRadialGradientBrush(wxDouble xo, wxDouble yo,
                                             wxDouble xc, wxDouble yc,
                                             wxDouble radius,
                                             const wxGraphicsGradientStops& stops)
{
    ENSURE_LOADED_OR_RETURN(wxNullGraphicsBrush);
    wxGraphicsBrush p;
    wxGDIPlusBrushData* d = new wxGDIPlusBrushData( this );
    d->CreateRadialGradientBrush(xo,yo,xc,yc,radius,stops);
    p.SetRefData(d);
    return p;
}

wxGraphicsFont
wxGDIPlusRenderer::CreateFont( const wxFont &font,
                               const wxColour &col )
{
    ENSURE_LOADED_OR_RETURN(wxNullGraphicsFont);
    if ( font.IsOk() )
    {
        wxGraphicsFont p;
        p.SetRefData(new wxGDIPlusFontData( this, font, col ));
        return p;
    }
    else
        return wxNullGraphicsFont;
}

wxGraphicsFont
wxGDIPlusRenderer::CreateFont(double size,
                              const wxString& facename,
                              int flags,
                              const wxColour& col)
{
    ENSURE_LOADED_OR_RETURN(wxNullGraphicsFont);

    // Convert wxFont flags to GDI+ style:
    int style = FontStyleRegular;
    if ( flags & wxFONTFLAG_ITALIC )
        style |= FontStyleItalic;
    if ( flags & wxFONTFLAG_UNDERLINED )
        style |= FontStyleUnderline;
    if ( flags & wxFONTFLAG_BOLD )
        style |= FontStyleBold;
    if ( flags & wxFONTFLAG_STRIKETHROUGH )
        style |= FontStyleStrikeout;


    wxGraphicsFont f;
    f.SetRefData(new wxGDIPlusFontData(this, facename, size, style, col));
    return f;
}

wxGraphicsBitmap wxGDIPlusRenderer::CreateBitmap( const wxBitmap &bitmap )
{
    ENSURE_LOADED_OR_RETURN(wxNullGraphicsBitmap);
    if ( bitmap.IsOk() )
    {
        wxGraphicsBitmap p;
        p.SetRefData(new wxGDIPlusBitmapData( this , bitmap ));
        return p;
    }
    else
        return wxNullGraphicsBitmap;
}

#if wxUSE_IMAGE

wxGraphicsBitmap wxGDIPlusRenderer::CreateBitmapFromImage(const wxImage& image)
{
    ENSURE_LOADED_OR_RETURN(wxNullGraphicsBitmap);
    if ( image.IsOk() )
    {
        // Notice that we rely on conversion from wxImage to wxBitmap here but
        // we could probably do it more efficiently by converting from wxImage
        // to GDI+ Bitmap directly, i.e. copying wxImage pixels to the buffer
        // returned by Bitmap::LockBits(). However this would require writing
        // code specific for this task while like this we can reuse existing
        // code (see also wxGDIPlusBitmapData::ConvertToImage()).
        wxGraphicsBitmap gb;
        gb.SetRefData(new wxGDIPlusBitmapData(this, image));
        return gb;
    }
    else
        return wxNullGraphicsBitmap;
}


wxImage wxGDIPlusRenderer::CreateImageFromBitmap(const wxGraphicsBitmap& bmp)
{
    ENSURE_LOADED_OR_RETURN(wxNullImage);
    const wxGDIPlusBitmapData* const
        data = static_cast<wxGDIPlusBitmapData*>(bmp.GetGraphicsData());

    return data ? data->ConvertToImage() : wxNullImage;
}

#endif // wxUSE_IMAGE


wxGraphicsBitmap wxGDIPlusRenderer::CreateBitmapFromNativeBitmap( void *bitmap )
{
    ENSURE_LOADED_OR_RETURN(wxNullGraphicsBitmap);
    if ( bitmap != NULL )
    {
        wxGraphicsBitmap p;
        p.SetRefData(new wxGDIPlusBitmapData( this , (Bitmap*) bitmap ));
        return p;
    }
    else
        return wxNullGraphicsBitmap;
}

wxGraphicsBitmap wxGDIPlusRenderer::CreateSubBitmap( const wxGraphicsBitmap &bitmap, wxDouble x, wxDouble y, wxDouble w, wxDouble h  )
{
    ENSURE_LOADED_OR_RETURN(wxNullGraphicsBitmap);

    wxCHECK_MSG(!bitmap.IsNull(), wxNullGraphicsBitmap, wxS("Invalid bitmap"));

    Bitmap* image = static_cast<wxGDIPlusBitmapData*>(bitmap.GetRefData())->GetGDIPlusBitmap();
    if ( image )
    {
        wxCHECK_MSG( x >= 0.0 && y >= 0.0 && w > 0.0 && h > 0.0 &&
                     x + w <= image->GetWidth() && y + h <= image->GetHeight(),
                     wxNullGraphicsBitmap, wxS("Invalid bitmap region"));

        wxGraphicsBitmap p;
        p.SetRefData(new wxGDIPlusBitmapData( this , image->Clone( (REAL) x , (REAL) y , (REAL) w , (REAL) h , PixelFormat32bppPARGB) ));
        return p;
    }
    else
        return wxNullGraphicsBitmap;
}

wxString wxGDIPlusRenderer::GetName() const
{
    return "gdiplus";
}

void wxGDIPlusRenderer::GetVersion(int *major, int *minor, int *micro) const
{
    if ( major )
        *major = wxPlatformInfo::Get().GetOSMajorVersion();
    if ( minor )
        *minor = wxPlatformInfo::Get().GetOSMinorVersion();
    if ( micro )
        *micro = 0;
}

// Shutdown GDI+ at app exit, before possible dll unload
class wxGDIPlusRendererModule : public wxModule
{
public:
    wxGDIPlusRendererModule()
    {
        // We must be uninitialized before GDI+ DLL itself is unloaded.
        AddDependency("wxGdiPlusModule");
    }

    virtual bool OnInit() wxOVERRIDE { return true; }
    virtual void OnExit() wxOVERRIDE
    {
        wxDELETE(gs_drawTextStringFormat);

        gs_GDIPlusRenderer.Unload();
    }

private:
    wxDECLARE_DYNAMIC_CLASS(wxGDIPlusRendererModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxGDIPlusRendererModule, wxModule);

// ----------------------------------------------------------------------------
// wxMSW-specific parts of wxGCDC
// ----------------------------------------------------------------------------

WXHDC wxGCDC::AcquireHDC()
{
    wxGraphicsContext * const gc = GetGraphicsContext();
    if ( !gc )
        return NULL;

#if wxUSE_CAIRO
    // we can't get the HDC if it is not a GDI+ context
    wxGraphicsRenderer* r1 = gc->GetRenderer();
    wxGraphicsRenderer* r2 = wxGraphicsRenderer::GetCairoRenderer();
    if (r1 == r2)
        return NULL;
#endif

    Graphics * const g = static_cast<Graphics *>(gc->GetNativeContext());
    return g ? g->GetHDC() : NULL;
}

void wxGCDC::ReleaseHDC(WXHDC hdc)
{
    if ( !hdc )
        return;

    wxGraphicsContext * const gc = GetGraphicsContext();
    wxCHECK_RET( gc, "can't release HDC because there is no wxGraphicsContext" );

#if wxUSE_CAIRO
    // we can't get the HDC if it is not a GDI+ context
    wxGraphicsRenderer* r1 = gc->GetRenderer();
    wxGraphicsRenderer* r2 = wxGraphicsRenderer::GetCairoRenderer();
    if (r1 == r2)
        return;
#endif

    Graphics * const g = static_cast<Graphics *>(gc->GetNativeContext());
    wxCHECK_RET( g, "can't release HDC because there is no Graphics" );

    g->ReleaseHDC((HDC)hdc);
}

#endif // wxUSE_GRAPHICS_GDIPLUS
