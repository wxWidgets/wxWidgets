#include "wx/qt/graphics.h"
#include <QPainter>


#if wxUSE_GRAPHICS_CONTEXT
#include "wx/graphics.h"
#include "wx/private/graphics.h"

class WXDLLIMPEXP_CORE wxQtBrushData : public wxGraphicsObjectRefData
{
public:
	wxQtBrushData(wxGraphicsRenderer* renderer, const wxBrush& wxbrush)
		: wxGraphicsObjectRefData(renderer), brush(wxbrush.GetHandle())
	{
	}
	QBrush brush;
};

//-----------------------------------------------------------------------------
// wxQtGraphicsPathData
//-----------------------------------------------------------------------------


class WXDLLIMPEXP_CORE wxQtGraphicsPathData : public wxGraphicsPathData
{
public:
	wxQtGraphicsPathData(wxGraphicsRenderer* renderer)
		: wxGraphicsPathData(renderer), path(new QPainterPath())
	{
	}
	~wxQtGraphicsPathData()
	{
		delete path;
	}

	virtual wxGraphicsObjectRefData *Clone() const wxOVERRIDE
	{
		return NULL;
	}

	//
	// These are the path primitives from which everything else can be constructed
	//

	// begins a new subpath at (x,y)
	virtual void MoveToPoint(wxDouble x, wxDouble y) wxOVERRIDE
	{
		path->moveTo(x, y);
	}

		// adds a straight line from the current point to (x,y)
		virtual void AddLineToPoint(wxDouble x, wxDouble y) wxOVERRIDE
	{
		path->lineTo(x, y);
	}

	// adds a cubic Bezier curve from the current point, using two control points and an end point
	virtual void AddCurveToPoint(wxDouble cx1, wxDouble cy1, wxDouble cx2, wxDouble cy2, wxDouble x, wxDouble y) wxOVERRIDE
	{
	}


	// adds an arc of a circle centering at (x,y) with radius (r) from startAngle to endAngle
	virtual void AddArc(wxDouble x, wxDouble y, wxDouble r, wxDouble startAngle, wxDouble endAngle, bool clockwise) wxOVERRIDE
	{
	}

	// gets the last point of the current path, (0,0) if not yet set
	virtual void GetCurrentPoint(wxDouble* x, wxDouble* y) const wxOVERRIDE
	{
		QPointF position = path->currentPosition();
		*x = position.x();
		*y = position.y();
	}

		// adds another path
		virtual void AddPath(const wxGraphicsPathData* path) wxOVERRIDE
	{
	}

	// closes the current sub-path
	virtual void CloseSubpath() wxOVERRIDE
	{
		path->closeSubpath();
	}

	//
	// These are convenience functions which - if not available natively will be assembled
	// using the primitives from above
	//

	// appends a rectangle as a new closed subpath
	virtual void AddRectangle(wxDouble x, wxDouble y, wxDouble w, wxDouble h) wxOVERRIDE
	{
	}

	// appends a circle as a new closed subpath
	virtual void AddCircle(wxDouble x, wxDouble y, wxDouble r) wxOVERRIDE
	{
		path->addEllipse(x - r, y - r, r, r);
		path->closeSubpath();
	}

	// appends an ellipse as a new closed subpath fitting the passed rectangle
	virtual void AddEllipse(wxDouble x, wxDouble y, wxDouble w, wxDouble h) wxOVERRIDE
	{
		path->addEllipse(x, y, w, h);
		path->closeSubpath();
	}

	/*
	// draws a an arc to two tangents connecting (current) to (x1,y1) and (x1,y1) to (x2,y2), also a straight line from (current) to (x1,y1)
	virtual void AddArcToPoint( wxDouble x1, wxDouble y1 , wxDouble x2, wxDouble y2, wxDouble r )  ;
	*/

	// returns the native path
	virtual void * GetNativePath() const wxOVERRIDE
	{
		return reinterpret_cast<void*>(path);
	}

	// give the native path returned by GetNativePath() back (there might be some deallocations necessary)
	virtual void UnGetNativePath(void *p) const wxOVERRIDE
	{
	}

	// transforms each point of this path by the matrix
	virtual void Transform(const wxGraphicsMatrixData* matrix) wxOVERRIDE
	{
	}

	// gets the bounding box enclosing all points (possibly including control points)
	virtual void GetBox(wxDouble *x, wxDouble *y, wxDouble *w, wxDouble *h) const wxOVERRIDE
	{
	}

	virtual bool Contains(wxDouble x, wxDouble y, wxPolygonFillMode fillStyle = wxWINDING_RULE) const wxOVERRIDE
	{
		return false;
	}

private:
	QPainterPath* path;
};

class WXDLLIMPEXP_CORE wxQtGraphicsContext : public wxGraphicsContext
{
public:
	wxQtGraphicsContext(wxGraphicsRenderer* renderer, const wxWindowDC& dc)
		: wxGraphicsContext(renderer)
	{
		QPainter* painter = reinterpret_cast<QPainter*>(dc.GetHandle());
		m_qtPainter = new QPainter(painter->device());
		//m_qtPainter->fillRect(-100, -100, 200, 200, QBrush(QColor(192, 192, 192)));
	}
	wxQtGraphicsContext(wxGraphicsRenderer* renderer, const wxMemoryDC& dc)
		: wxGraphicsContext(renderer)
	{
		QPainter* painter = reinterpret_cast<QPainter*>(dc.GetHandle());
		m_qtPainter = new QPainter(painter->device());
	}
#if wxUSE_PRINTING_ARCHITECTURE
	wxQtGraphicsContext(wxGraphicsRenderer* renderer, const wxPrinterDC& dc)
		: wxGraphicsContext(renderer)
	{
		QPainter* painter = reinterpret_cast<QPainter*>(dc.GetHandle());
		m_qtPainter = new QPainter(painter->device());
	}
#endif
	wxQtGraphicsContext(wxGraphicsRenderer* renderer, wxWindow *window)
		: wxGraphicsContext(renderer)
	{
		QPainter* painter = reinterpret_cast<QPainter*>(window->QtGetPainter());
		m_qtPainter = new QPainter(painter->device());
	}

	// If this ctor is used, Init() must be called by the derived class later.
	//wxQtGraphicsContext(wxGraphicsRenderer* renderer)
	//	: wxGraphicsContext(renderer))
	//{
	//}

	virtual ~wxQtGraphicsContext()
	{
		delete m_qtPainter;
	}

	virtual bool ShouldOffset() const wxOVERRIDE
	{
		return false;
	}

	virtual void Clip(const wxRegion &region) wxOVERRIDE
	{
	}

	// clips drawings to the rect
	virtual void Clip(wxDouble x, wxDouble y, wxDouble w, wxDouble h) wxOVERRIDE
	{
	}

	// resets the clipping to original extent
	virtual void ResetClip() wxOVERRIDE
	{
	}

	// returns bounding box of the clipping region
	virtual void GetClipBox(wxDouble* x, wxDouble* y, wxDouble* w, wxDouble* h) wxOVERRIDE
	{
	}

	virtual void * GetNativeContext() wxOVERRIDE
	{
		return NULL;
	}

	virtual bool SetAntialiasMode(wxAntialiasMode antialias) wxOVERRIDE
	{
		return true;
	}

	virtual bool SetInterpolationQuality(wxInterpolationQuality interpolation) wxOVERRIDE
	{
		return true;
	}

	virtual bool SetCompositionMode(wxCompositionMode op) wxOVERRIDE
	{
		return true;
	}

	virtual void BeginLayer(wxDouble opacity) wxOVERRIDE
	{
	}

	virtual void EndLayer() wxOVERRIDE
	{
	}

	virtual void StrokePath(const wxGraphicsPath& p) wxOVERRIDE
	{
	}
	virtual void FillPath(const wxGraphicsPath& p, wxPolygonFillMode fillStyle = wxWINDING_RULE) wxOVERRIDE
	{
		if (m_brush.IsNull())
		{
			return;
		}

		QPainterPath* path_data = reinterpret_cast<QPainterPath*>(p.GetNativePath());
		const QBrush& brush = ((wxQtBrushData*)m_brush.GetRefData())->brush;
		m_qtPainter->fillPath(*path_data, brush);
	}
	virtual void ClearRectangle(wxDouble x, wxDouble y, wxDouble w, wxDouble h) wxOVERRIDE
	{
	}
	virtual void DrawRectangle(wxDouble x, wxDouble y, wxDouble w, wxDouble h) wxOVERRIDE
	{
	}

	virtual void Translate(wxDouble dx, wxDouble dy) wxOVERRIDE
	{
		m_qtPainter->translate(dx, dy);
	}
	virtual void Scale(wxDouble xScale, wxDouble yScale) wxOVERRIDE
	{
	}
	virtual void Rotate(wxDouble angle) wxOVERRIDE
	{
        // wx angle is in radians. Qt angle is in degrees.
		m_qtPainter->rotate(angle*180/M_PI);
	}

	// concatenates this transform with the current transform of this context
	virtual void ConcatTransform(const wxGraphicsMatrix& matrix) wxOVERRIDE
	{
	}

	// sets the transform of this context
	virtual void SetTransform(const wxGraphicsMatrix& matrix) wxOVERRIDE
	{
	}

	// gets the matrix of this context
	virtual wxGraphicsMatrix GetTransform() const wxOVERRIDE
	{
		return wxGraphicsMatrix();
	}

	virtual void DrawBitmap(const wxGraphicsBitmap &bmp, wxDouble x, wxDouble y, wxDouble w, wxDouble h) wxOVERRIDE
	{
	}
	virtual void DrawBitmap(const wxBitmap &bmp, wxDouble x, wxDouble y, wxDouble w, wxDouble h) wxOVERRIDE
	{
	}
	virtual void DrawIcon(const wxIcon &icon, wxDouble x, wxDouble y, wxDouble w, wxDouble h) wxOVERRIDE
	{
	}
	virtual void PushState() wxOVERRIDE
	{
	}
	virtual void PopState() wxOVERRIDE
	{
	}
	virtual void Flush() wxOVERRIDE
	{
	}

	virtual void GetTextExtent(const wxString &str, wxDouble *width, wxDouble *height,
		wxDouble *descent, wxDouble *externalLeading) const wxOVERRIDE
	{
	}
	virtual void GetPartialTextExtents(const wxString& text, wxArrayDouble& widths) const wxOVERRIDE
	{
	}

protected:
	virtual void DoDrawText(const wxString &str, wxDouble x, wxDouble y) wxOVERRIDE
	{
	}

	enum ApplyTransformMode { Apply_directly, Apply_scaled_dev_origin };

#ifdef __WXQT__
	QPainter* m_qtPainter;
	QImage* m_qtImage;
//	cairo_surface_t* m_qtSurface;
#endif

private:
	//cairo_t* m_context;
	//cairo_matrix_t m_internalTransform;

	wxVector<float> m_layerOpacities;

	wxDECLARE_NO_COPY_CLASS(wxQtGraphicsContext);
};

//-----------------------------------------------------------------------------
// wxQtGraphicsRenderer declaration
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxQtGraphicsRenderer : public wxGraphicsRenderer
{
public:
	wxQtGraphicsRenderer() {}

	virtual ~wxQtGraphicsRenderer() {}

	// Context

	virtual wxGraphicsContext * CreateContext(const wxWindowDC& dc) wxOVERRIDE;
	virtual wxGraphicsContext * CreateContext(const wxMemoryDC& dc) wxOVERRIDE;
#if wxUSE_PRINTING_ARCHITECTURE
	virtual wxGraphicsContext * CreateContext(const wxPrinterDC& dc) wxOVERRIDE;
#endif

	virtual wxGraphicsContext * CreateContextFromNativeContext(void * context) wxOVERRIDE;

	virtual wxGraphicsContext * CreateContextFromNativeWindow(void * window) wxOVERRIDE;

#if wxUSE_IMAGE
	virtual wxGraphicsContext * CreateContextFromImage(wxImage& image) wxOVERRIDE;
#endif // wxUSE_IMAGE

	virtual wxGraphicsContext * CreateContext(wxWindow* window) wxOVERRIDE;

	virtual wxGraphicsContext * CreateMeasuringContext() wxOVERRIDE;

    // Path

	virtual wxGraphicsPath CreatePath() wxOVERRIDE;

	// Matrix

	virtual wxGraphicsMatrix CreateMatrix(wxDouble a = 1.0, wxDouble b = 0.0, wxDouble c = 0.0, wxDouble d = 1.0,
		wxDouble tx = 0.0, wxDouble ty = 0.0) wxOVERRIDE;


	virtual wxGraphicsPen CreatePen(const wxGraphicsPenInfo& info) wxOVERRIDE;

	virtual wxGraphicsBrush CreateBrush(const wxBrush& brush) wxOVERRIDE;

	virtual wxGraphicsBrush
		CreateLinearGradientBrush(wxDouble x1, wxDouble y1,
		wxDouble x2, wxDouble y2,
		const wxGraphicsGradientStops& stops) wxOVERRIDE;

	virtual wxGraphicsBrush
		CreateRadialGradientBrush(wxDouble xo, wxDouble yo,
		wxDouble xc, wxDouble yc,
		wxDouble radius,
		const wxGraphicsGradientStops& stops) wxOVERRIDE;

	// sets the font
	virtual wxGraphicsFont CreateFont(const wxFont &font, const wxColour &col = *wxBLACK) wxOVERRIDE;
	virtual wxGraphicsFont CreateFont(double sizeInPixels,
		const wxString& facename,
		int flags = wxFONTFLAG_DEFAULT,
		const wxColour& col = *wxBLACK) wxOVERRIDE;

	// create a native bitmap representation
	virtual wxGraphicsBitmap CreateBitmap(const wxBitmap &bitmap) wxOVERRIDE;
#if wxUSE_IMAGE
	virtual wxGraphicsBitmap CreateBitmapFromImage(const wxImage& image) wxOVERRIDE;
	virtual wxImage CreateImageFromBitmap(const wxGraphicsBitmap& bmp) wxOVERRIDE;
#endif // wxUSE_IMAGE

	// create a graphics bitmap from a native bitmap
	virtual wxGraphicsBitmap CreateBitmapFromNativeBitmap(void* bitmap) wxOVERRIDE;

	// create a subimage from a native image representation
	virtual wxGraphicsBitmap CreateSubBitmap(const wxGraphicsBitmap &bitmap, wxDouble x, wxDouble y, wxDouble w, wxDouble h) wxOVERRIDE;

	virtual wxString GetName() const wxOVERRIDE;
	virtual void GetVersion(int *major, int *minor, int *micro) const wxOVERRIDE;

	wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxQtGraphicsRenderer);
};

//-----------------------------------------------------------------------------
// wxQtGraphicsRenderer implementation
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxQtGraphicsRenderer, wxGraphicsRenderer);

static wxQtGraphicsRenderer gs_cairoGraphicsRenderer;

wxGraphicsContext * wxQtGraphicsRenderer::CreateContext(const wxWindowDC& dc)
{
	return new wxQtGraphicsContext(this, dc);
}

wxGraphicsContext * wxQtGraphicsRenderer::CreateContext(const wxMemoryDC& dc)
{
	return new wxQtGraphicsContext(this, dc);
}

#if wxUSE_PRINTING_ARCHITECTURE
wxGraphicsContext * wxQtGraphicsRenderer::CreateContext(const wxPrinterDC& dc)
{
	return new wxQtGraphicsContext(this, dc);
}
#endif

#if defined(__WXMSW__) && wxUSE_ENH_METAFILE
wxGraphicsContext * wxQtGraphicsRenderer::CreateContext(const wxEnhMetaFileDC& dc)
{
	ENSURE_LOADED_OR_RETURN(NULL);
	return new wxQtGraphicsContext(this, dc);
}
#endif // __WXMSW__ && wxUSE_ENH_METAFILE

wxGraphicsContext * wxQtGraphicsRenderer::CreateContextFromNativeContext(void * context)
{
#ifdef __WXMSW__
	DWORD objType = ::GetObjectType((HGDIOBJ)context);
	if (objType == 0)
		return new wxQtGraphicsContext(this, (cairo_t*)context);

	if (objType == OBJ_DC || objType == OBJ_MEMDC)
		return new wxQtGraphicsContext(this, (HDC)context);

	return NULL;
#else
	return NULL;
#endif
}


wxGraphicsContext * wxQtGraphicsRenderer::CreateContextFromNativeWindow(void * window)
{
#ifdef __WXGTK__
	return new wxQtGraphicsContext(this, static_cast<GdkWindow*>(window));
#elif defined(__WXMSW__)
	return new wxQtGraphicsContext(this, static_cast<HWND>(window));
#else
	wxUnusedVar(window);
	return NULL;
#endif
}

#ifdef __WXMSW__
wxGraphicsContext * wxQtGraphicsRenderer::CreateContextFromNativeHDC(WXHDC dc)
{
	return new wxQtGraphicsContext(this, (HDC)dc);
}
#endif

#if wxUSE_IMAGE
wxGraphicsContext * wxQtGraphicsRenderer::CreateContextFromImage(wxImage& image)
{
//	return new wxCairoImageContext(this, image);
	return NULL;
}
#endif // wxUSE_IMAGE

wxGraphicsContext * wxQtGraphicsRenderer::CreateMeasuringContext()
{
#ifdef __WXGTK__
	return CreateContextFromNativeWindow(gdk_get_default_root_window());
#elif defined(__WXMSW__)
	return new wxCairoMeasuringContext(this);
#else
	return NULL;
	// TODO
#endif
}

wxGraphicsContext * wxQtGraphicsRenderer::CreateContext(wxWindow* window)
{
	return new wxQtGraphicsContext(this, window);
}

// Path

wxGraphicsPath wxQtGraphicsRenderer::CreatePath()
{
	wxGraphicsPath path;
	path.SetRefData(new wxQtGraphicsPathData(this));
	return path;
}


// Matrix

wxGraphicsMatrix wxQtGraphicsRenderer::CreateMatrix(wxDouble a, wxDouble b, wxDouble c, wxDouble d,
	wxDouble tx, wxDouble ty)

{
	wxGraphicsMatrix m;
//	wxCairoMatrixData* data = new wxCairoMatrixData(this);
//	data->Set(a, b, c, d, tx, ty);
//	m.SetRefData(data);
	return m;
}

wxGraphicsPen wxQtGraphicsRenderer::CreatePen(const wxGraphicsPenInfo& info)
{
	wxGraphicsPen p;
	if (info.GetStyle() != wxPENSTYLE_TRANSPARENT)
	{
//		p.SetRefData(new wxCairoPenData(this, info));
	}
	return p;
}

wxGraphicsBrush wxQtGraphicsRenderer::CreateBrush(const wxBrush& brush)
{
	wxGraphicsBrush p;
	if (brush.IsOk() && brush.GetStyle() != wxBRUSHSTYLE_TRANSPARENT)
	{
		p.SetRefData(new wxQtBrushData(this, brush));
	}
	return p;
}

wxGraphicsBrush
wxQtGraphicsRenderer::CreateLinearGradientBrush(wxDouble x1, wxDouble y1,
wxDouble x2, wxDouble y2,
const wxGraphicsGradientStops& stops)
{
	wxGraphicsBrush p;
	//wxCairoBrushData* d = new wxCairoBrushData(this);
	//d->CreateLinearGradientBrush(x1, y1, x2, y2, stops);
	//p.SetRefData(d);
	return p;
}

wxGraphicsBrush
wxQtGraphicsRenderer::CreateRadialGradientBrush(wxDouble xo, wxDouble yo,
wxDouble xc, wxDouble yc, wxDouble r,
const wxGraphicsGradientStops& stops)
{
	wxGraphicsBrush p;
	//wxCairoBrushData* d = new wxCairoBrushData(this);
	//d->CreateRadialGradientBrush(xo, yo, xc, yc, r, stops);
	//p.SetRefData(d);
	return p;
}

wxGraphicsFont wxQtGraphicsRenderer::CreateFont(const wxFont &font, const wxColour &col)
{
	wxGraphicsFont p;
	if (font.IsOk())
	{
		//p.SetRefData(new wxCairoFontData(this, font, col));
	}
	return p;
}

wxGraphicsFont
wxQtGraphicsRenderer::CreateFont(double sizeInPixels,
const wxString& facename,
int flags,
const wxColour& col)
{
	wxGraphicsFont font;
	//font.SetRefData(new wxCairoFontData(this, sizeInPixels, facename, flags, col));
	return font;
}

wxGraphicsBitmap wxQtGraphicsRenderer::CreateBitmap(const wxBitmap& bmp)
{
	wxGraphicsBitmap p;
	if (bmp.IsOk())
	{
		//p.SetRefData(new wxCairoBitmapData(this, bmp));
	}
	return p;
}

#if wxUSE_IMAGE

wxGraphicsBitmap wxQtGraphicsRenderer::CreateBitmapFromImage(const wxImage& image)
{
	wxGraphicsBitmap bmp;

	if (image.IsOk())
	{
		//bmp.SetRefData(new wxCairoBitmapData(this, image));
	}

	return bmp;
}

wxImage wxQtGraphicsRenderer::CreateImageFromBitmap(const wxGraphicsBitmap& bmp)
{
	wxImage image;

	//const wxCairoBitmapData* const
	//	data = static_cast<wxCairoBitmapData*>(bmp.GetGraphicsData());
	//if (data)
	//	image = data->ConvertToImage();

	return image;
}

#endif // wxUSE_IMAGE


wxGraphicsBitmap wxQtGraphicsRenderer::CreateBitmapFromNativeBitmap(void* bitmap)
{
	wxGraphicsBitmap p;
	if (bitmap != NULL)
	{
		//p.SetRefData(new wxCairoBitmapData(this, (cairo_surface_t*)bitmap));
	}
	return p;
}

wxGraphicsBitmap
wxQtGraphicsRenderer::CreateSubBitmap(const wxGraphicsBitmap& bitmap,
wxDouble x, wxDouble y,
wxDouble w, wxDouble h)
{
//	wxCHECK_MSG(!bitmap.IsNull(), wxNullGraphicsBitmap, wxS("Invalid bitmap"));
//
//	wxCairoBitmapData* dataSrc = static_cast<wxCairoBitmapData*>(bitmap.GetRefData());
//	cairo_surface_t* srcSurface = dataSrc->GetCairoSurface();
//	wxCHECK_MSG(srcSurface, wxNullGraphicsBitmap, wxS("Invalid bitmap"));
//
//	int srcWidth = cairo_image_surface_get_width(srcSurface);
//	int srcHeight = cairo_image_surface_get_height(srcSurface);
//
//	int dstWidth = wxRound(w);
//	int dstHeight = wxRound(h);
//
//	wxCHECK_MSG(x >= 0.0 && y >= 0.0 && dstWidth > 0 && dstHeight > 0 &&
//		x + dstWidth <= srcWidth && y + dstHeight <= srcHeight,
//		wxNullGraphicsBitmap, wxS("Invalid bitmap region"));
//
//	cairo_surface_t* dstSurface;
//#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 12, 0)
//	if (cairo_version() >= CAIRO_VERSION_ENCODE(1, 12, 0))
//	{
//		dstSurface = cairo_surface_create_similar_image(srcSurface,
//			cairo_image_surface_get_format(srcSurface),
//			dstWidth, dstHeight);
//	}
//	else
//#endif // Cairo 1.12
//	{
//		dstSurface = cairo_surface_create_similar(srcSurface,
//			CAIRO_CONTENT_COLOR_ALPHA,
//			dstWidth, dstHeight);
//	}
//
//	cairo_t* cr = cairo_create(dstSurface);
//	cairo_set_source_surface(cr, srcSurface, -x, -y);
//
//	cairo_rectangle(cr, 0.0, 0.0, dstWidth, dstHeight);
//	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
//	cairo_fill(cr);
//	cairo_destroy(cr);
//
	wxGraphicsBitmap bmpRes;
	//bmpRes.SetRefData(new wxCairoBitmapData(this, dstSurface));
	return bmpRes;
}

wxString wxQtGraphicsRenderer::GetName() const
{
	return "qt";
}

void wxQtGraphicsRenderer::GetVersion(int *major, int *minor, int *micro) const
{
	if (major)
		*major = 1;
}

static wxQtGraphicsRenderer gs_qtGraphicsRenderer;

// MSW and OS X have their own native default renderers, but the other ports
// use Cairo by default
wxGraphicsRenderer* wxGraphicsRenderer::GetDefaultRenderer()
{
	return &gs_qtGraphicsRenderer;
}


#endif // wxUSE_GRAPHICS_CONTEXT
