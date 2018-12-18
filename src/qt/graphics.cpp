#include "wx/qt/graphics.h"
#include <QPainter>


#if wxUSE_GRAPHICS_CONTEXT
#include "wx/graphics.h"
#include "wx/private/graphics.h"

namespace
{
    double RadiansToDegrees(double angle_in_radians)
    {
        return angle_in_radians * 180 / M_PI;
    }
}

class WXDLLIMPEXP_CORE wxQtBrushData : public wxGraphicsObjectRefData
{
public:
    wxQtBrushData(wxGraphicsRenderer* renderer)
        : wxGraphicsObjectRefData(renderer)
    {
    }

	wxQtBrushData(wxGraphicsRenderer* renderer, const wxBrush& wxbrush)
		: wxGraphicsObjectRefData(renderer), brush(wxbrush.GetHandle())
	{
	}

    void CreateLinearGradientBrush(wxDouble x1, wxDouble y1,
        wxDouble x2, wxDouble y2,
        const wxGraphicsGradientStops& stops)
    {
        QLinearGradient gradient(x1, y1, x2, y2);
        QGradientStops qstops;
        for (size_t i = 0; i < stops.GetCount(); ++i)
        {
            const wxGraphicsGradientStop stop = stops.Item(i);
            qstops.append(QGradientStop(stop.GetPosition(), stop.GetColour().GetQColor()));
        }

        gradient.setStops(qstops);
        brush = QBrush(gradient);
    }

    const QBrush& getBrush() const
	{
        return brush;
	}

private:
	QBrush brush;
};

class WXDLLIMPEXP_CORE wxQtPenData : public wxGraphicsObjectRefData
{
public:
    wxQtPenData(wxGraphicsRenderer* renderer, const wxGraphicsPenInfo& info)
        : wxGraphicsObjectRefData(renderer), pen(createPenFromInfo(info))
    {
    }

    const QPen& getPen() const
    {
        return pen;
    }

private:
    static QPen createPenFromInfo(const wxGraphicsPenInfo& info)
    {
        wxPen wxpen(info.GetColour(), info.GetWidth(), info.GetStyle());
        wxpen.SetDashes(info.GetDashCount(), info.GetDash());
        wxpen.SetJoin(info.GetJoin());
        wxpen.SetCap(info.GetCap());
        switch (info.GetStyle())
        {
            case wxPENSTYLE_STIPPLE:
            case wxPENSTYLE_STIPPLE_MASK:
            case wxPENSTYLE_STIPPLE_MASK_OPAQUE:
            {
                wxpen.SetStipple(info.GetStipple());
                break;
            }
            default:
                break;
        }

        return wxpen.GetHandle();
    }
    QPen pen;
};

class WXDLLIMPEXP_CORE wxQtMatrixData : public wxGraphicsMatrixData
{
public:
	explicit wxQtMatrixData(wxGraphicsRenderer* renderer)
		: wxGraphicsMatrixData(renderer), m_transform(new QTransform)
	{
	}

    wxQtMatrixData(wxGraphicsRenderer* renderer, const QTransform& transform)
        : wxGraphicsMatrixData(renderer), m_transform(new QTransform(transform))
    {
    }
    
    virtual ~wxQtMatrixData()
	{
		delete m_transform;
	}

    virtual wxGraphicsObjectRefData* Clone() const wxOVERRIDE
    {
        wxQtMatrixData* new_matrix = new wxQtMatrixData(m_renderer);
        *new_matrix->m_transform = *m_transform;
        return new_matrix;
    }

	// concatenates the matrix
	virtual void Concat(const wxGraphicsMatrixData *t) wxOVERRIDE
	{
        const wxQtMatrixData* rhs = static_cast<const wxQtMatrixData*>(t);
        *m_transform = *rhs->m_transform * (*m_transform);
	}

	// sets the matrix to the respective values
	virtual void Set(wxDouble a = 1.0, wxDouble b = 0.0, wxDouble c = 0.0, wxDouble d = 1.0,
	wxDouble tx = 0.0, wxDouble ty = 0.0) wxOVERRIDE
	{
        m_transform->setMatrix(
            a, b, 0.0,
            c, d, 0.0,
            tx, ty, 1.0);
	}

	// gets the component valuess of the matrix
	virtual void Get(wxDouble* a = NULL, wxDouble* b = NULL, wxDouble* c = NULL,
	wxDouble* d = NULL, wxDouble* tx = NULL, wxDouble* ty = NULL) const wxOVERRIDE
	{
        if (a) *a = m_transform->m11();
        if (b) *b = m_transform->m12();
        if (c) *c = m_transform->m21();
        if (d) *d = m_transform->m22();
        if (tx) *tx = m_transform->m31();
        if (ty) *ty = m_transform->m32();
    }

	// makes this the inverse matrix
	virtual void Invert() wxOVERRIDE
	{
        bool invertible = false;
        const QTransform inverted_transform = m_transform->inverted(&invertible);
        if (invertible)
        {
            *m_transform = inverted_transform;
        }
	}

	// returns true if the elements of the transformation matrix are equal ?
	virtual bool IsEqual(const wxGraphicsMatrixData* t) const wxOVERRIDE
	{
        const wxQtMatrixData* rhs = static_cast<const wxQtMatrixData*>(t);
        return *m_transform == *rhs->m_transform;
	}

	// return true if this is the identity matrix
	virtual bool IsIdentity() const wxOVERRIDE
	{
        return m_transform->isIdentity();
	}

	//
	// transformation
	//

	// add the translation to this matrix
	virtual void Translate(wxDouble dx, wxDouble dy) wxOVERRIDE
	{
        m_transform->translate(dx, dy);
	}

	// add the scale to this matrix
	virtual void Scale(wxDouble xScale, wxDouble yScale) wxOVERRIDE
	{
        m_transform->scale(xScale, yScale);
    }

	// add the rotation to this matrix (radians)
	virtual void Rotate(wxDouble angle) wxOVERRIDE
	{
        m_transform->rotateRadians(angle);
	}

	//
	// apply the transforms
	//

	// applies that matrix to the point
	virtual void TransformPoint(wxDouble *x, wxDouble *y) const wxOVERRIDE
	{
        qreal transformed_x, transformed_y;
        m_transform->map(static_cast<qreal>(*x), static_cast<qreal>(*y), &transformed_x, &transformed_y);
        *x = transformed_x;
        *y = transformed_y;
	}

	// applies the matrix except for translations
	virtual void TransformDistance(wxDouble *dx, wxDouble *dy) const wxOVERRIDE
	{
	}

	// returns the native representation
	virtual void * GetNativeMatrix() const wxOVERRIDE
	{
		return static_cast<void*>(m_transform);
	}

    const QTransform& GetQTransform() const
	{
        return *m_transform;
	}
private:
	QTransform* m_transform;
};

class wxQtFontData : public wxGraphicsObjectRefData
{
public:
    wxQtFontData(wxGraphicsRenderer* renderer, const wxFont &font, const wxColour &col)
        : wxGraphicsObjectRefData(renderer),
          m_font(font.GetHandle()),
          m_color(col.GetQColor())
    {
    }

    const QFont& GetFont() const
    {
        return m_font;
    }

    const QColor& GetColor() const
    {
        return m_color;
    }

private:
    QFont m_font;
    QColor m_color;
};

//-----------------------------------------------------------------------------
// wxQtGraphicsPathData
//-----------------------------------------------------------------------------


class WXDLLIMPEXP_CORE wxQtGraphicsPathData : public wxGraphicsPathData
{
public:
	wxQtGraphicsPathData(wxGraphicsRenderer* renderer)
		: wxGraphicsPathData(renderer),
          m_path(new QPainterPath()),
        m_current_subpath_start(-1)
	{
	}
	~wxQtGraphicsPathData()
	{
		delete m_path;
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
        m_path->moveTo(x, y);
        m_current_subpath_start = m_path->elementCount() - 1;
	}

	// adds a straight line from the current point to (x,y)
    // if there is no current path, it is equivalent to a moveTo.
	virtual void AddLineToPoint(wxDouble x, wxDouble y) wxOVERRIDE
	{
        if (!hasCurrentSubpath())
            MoveToPoint(x, y);
        else
            m_path->lineTo(x, y);
	}

	// adds a cubic Bezier curve from the current point, using two control points and an end point
	virtual void AddCurveToPoint(wxDouble cx1, wxDouble cy1, wxDouble cx2, wxDouble cy2, wxDouble x, wxDouble y) wxOVERRIDE
	{
	    if (!hasCurrentSubpath())
            MoveToPoint(cx1, cy1);
        m_path->cubicTo(QPointF(cx1, cy1), QPointF(cx2, cy2), QPointF(x, y));
	}

	// adds an arc of a circle centering at (x,y) with radius (r) from startAngle to endAngle
	virtual void AddArc(wxDouble x, wxDouble y, wxDouble r, wxDouble startAngle, wxDouble endAngle, bool clockwise) wxOVERRIDE
	{
        if (!hasCurrentSubpath())
            MoveToPoint(x, y);

        qreal arc_length;
        if (clockwise)
        {
            if (endAngle < startAngle)
                endAngle += 2 * M_PI;
            arc_length = -RadiansToDegrees(endAngle - startAngle);
        }
        else
        {
            if (endAngle > startAngle)
                endAngle -= 2 * M_PI;
            arc_length = -RadiansToDegrees(endAngle - startAngle);
        }
        m_path->arcTo(x-r, y-r, r*2, r*2, -RadiansToDegrees(startAngle), arc_length);
	}

	// gets the last point of the current path, (0,0) if not yet set
	virtual void GetCurrentPoint(wxDouble* x, wxDouble* y) const wxOVERRIDE
	{
		QPointF position = m_path->currentPosition();
		*x = position.x();
		*y = position.y();
	}

	// adds another path
	virtual void AddPath(const wxGraphicsPathData* path) wxOVERRIDE
	{
        const wxQtGraphicsPathData* new_path = static_cast<const wxQtGraphicsPathData*>(path);
        m_path->addPath(*new_path->m_path);
	}

	// closes the current sub-path
	virtual void CloseSubpath() wxOVERRIDE
	{
        // Current position must be end of last path after close, not (0,0) as Qt sets.
        if (!hasCurrentSubpath())
            return;

        const QPainterPath::Element first_element = m_path->elementAt(m_current_subpath_start);

        m_path->closeSubpath();
        MoveToPoint(first_element.x, first_element.y);
	}

	//
	// These are convenience functions which - if not available natively will be assembled
	// using the primitives from above
	//

    // appends a circle as a new closed subpath
	virtual void AddCircle(wxDouble x, wxDouble y, wxDouble r) wxOVERRIDE
	{
        m_path->addEllipse(x - r, y - r, r*2, r*2);
    }

	// appends an ellipse as a new closed subpath fitting the passed rectangle
	virtual void AddEllipse(wxDouble x, wxDouble y, wxDouble w, wxDouble h) wxOVERRIDE
	{
        m_path->addEllipse(x, y, w, h);
	}

	/*
	// draws a an arc to two tangents connecting (current) to (x1,y1) and (x1,y1) to (x2,y2), also a straight line from (current) to (x1,y1)
	virtual void AddArcToPoint( wxDouble x1, wxDouble y1 , wxDouble x2, wxDouble y2, wxDouble r )  ;
	*/

	// returns the native path
	virtual void * GetNativePath() const wxOVERRIDE
	{
		return reinterpret_cast<void*>(m_path);
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
        QRectF bounding_rect = m_path->controlPointRect();
        if (!bounding_rect.isValid())
            bounding_rect = QRectF();
        if (x) *x = bounding_rect.left();
        if (y) *y = bounding_rect.top();
        if (w) *w = bounding_rect.width();
        if (h) *h = bounding_rect.height();
    }

	virtual bool Contains(wxDouble x, wxDouble y, wxPolygonFillMode fillStyle = wxWINDING_RULE) const wxOVERRIDE
	{
        return m_path->contains(QPointF(x, y));
	}

private:
    bool hasCurrentSubpath() const
    {
        return m_current_subpath_start != -1;
    }
	QPainterPath* m_path;
    int m_current_subpath_start;
};

class WXDLLIMPEXP_CORE wxQtGraphicsContext : public wxGraphicsContext
{
    void initFromDC(const wxDC& dc)
    {
        QPainter* painter = reinterpret_cast<QPainter*>(dc.GetHandle());
        m_qtPainter = new QPainter(painter->device());
        wxSize sz = dc.GetSize();
        m_width = sz.x;
        m_height = sz.y;
    }
public:
	wxQtGraphicsContext(wxGraphicsRenderer* renderer, const wxWindowDC& dc)
		: wxGraphicsContext(renderer)
	{
        initFromDC(dc);
	}
	wxQtGraphicsContext(wxGraphicsRenderer* renderer, const wxMemoryDC& dc)
		: wxGraphicsContext(renderer)
	{
        initFromDC(dc);
    }
#if wxUSE_PRINTING_ARCHITECTURE
	wxQtGraphicsContext(wxGraphicsRenderer* renderer, const wxPrinterDC& dc)
		: wxGraphicsContext(renderer)
	{
        initFromDC(dc);
    }
#endif
	wxQtGraphicsContext(wxGraphicsRenderer* renderer, wxWindow *window)
		: wxGraphicsContext(renderer)
	{
		QPainter* painter = reinterpret_cast<QPainter*>(window->QtGetPainter());
		m_qtPainter = new QPainter(painter->device());

        wxSize sz = window->GetClientSize();
        m_width = sz.x;
        m_height = sz.y;
    }

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
        if (m_pen.IsNull())
        {
            return;
        }

        QPainterPath* path_data = reinterpret_cast<QPainterPath*>(p.GetNativePath());
        const QPen& pen = ((wxQtPenData*)m_pen.GetRefData())->getPen();
        m_qtPainter->strokePath(*path_data, pen);
    }
	virtual void FillPath(const wxGraphicsPath& p, wxPolygonFillMode fillStyle = wxWINDING_RULE) wxOVERRIDE
	{
		if (m_brush.IsNull())
		{
			return;
		}

		QPainterPath* path_data = reinterpret_cast<QPainterPath*>(p.GetNativePath());
		const QBrush& brush = ((wxQtBrushData*)m_brush.GetRefData())->getBrush();
		m_qtPainter->fillPath(*path_data, brush);
	}
	virtual void ClearRectangle(wxDouble x, wxDouble y, wxDouble w, wxDouble h) wxOVERRIDE
	{
        m_qtPainter->fillRect(x, y, w, h, QBrush(QColor(0, 0, 0, 0)));
	}

	virtual void Translate(wxDouble dx, wxDouble dy) wxOVERRIDE
	{
		m_qtPainter->translate(dx, dy);
	}
	virtual void Scale(wxDouble xScale, wxDouble yScale) wxOVERRIDE
	{
        m_qtPainter->scale(xScale, yScale);
	}

	virtual void Rotate(wxDouble angle) wxOVERRIDE
	{
        // wx angle is in radians. Qt angle is in degrees.
		m_qtPainter->rotate(RadiansToDegrees(angle));
	}

	// concatenates this transform with the current transform of this context
	virtual void ConcatTransform(const wxGraphicsMatrix& matrix) wxOVERRIDE
	{
        wxGraphicsMatrix currentMatrix = GetTransform();
        currentMatrix.Concat(matrix);
        SetTransform(currentMatrix);
	}

	// sets the transform of this context
	virtual void SetTransform(const wxGraphicsMatrix& matrix) wxOVERRIDE
	{
        const wxQtMatrixData* qmatrix = static_cast<const wxQtMatrixData*>(matrix.GetRefData());
        m_qtPainter->setTransform(qmatrix->GetQTransform());
	}

	// gets the matrix of this context
	virtual wxGraphicsMatrix GetTransform() const wxOVERRIDE
	{
        QTransform transform = m_qtPainter->transform();
        wxGraphicsMatrix m;
	    m.SetRefData(new wxQtMatrixData(GetRenderer(), transform));
        return m;
	}

	virtual void DrawBitmap(const wxGraphicsBitmap &bmp, wxDouble x, wxDouble y, wxDouble w, wxDouble h) wxOVERRIDE
	{
	}

    void DoDrawBitmap(const wxBitmap &bmp, wxDouble x, wxDouble y, wxDouble w, wxDouble h, bool useMask) const
	{
        QPixmap pix = *bmp.GetHandle();

        if (useMask && bmp.GetMask() && bmp.GetMask()->GetHandle())
            pix.setMask(*bmp.GetMask()->GetHandle());

        m_qtPainter->drawPixmap(x, y, w, h, pix);
	}

	virtual void DrawBitmap(const wxBitmap &bmp, wxDouble x, wxDouble y, wxDouble w, wxDouble h) wxOVERRIDE
	{
        DoDrawBitmap(bmp, x, y, w, h, false);
    }

    virtual void DrawIcon(const wxIcon &icon, wxDouble x, wxDouble y, wxDouble w, wxDouble h) wxOVERRIDE
	{
        DoDrawBitmap(icon, x, y, w, h, true);
	}

    virtual void PushState() wxOVERRIDE
	{
        m_qtPainter->save();
	}

    virtual void PopState() wxOVERRIDE
	{
        m_qtPainter->restore();
    }

    virtual void Flush() wxOVERRIDE
	{
	}

	virtual void GetTextExtent(const wxString &str, wxDouble *width, wxDouble *height,
		wxDouble *descent, wxDouble *externalLeading) const wxOVERRIDE
	{
        wxCHECK_RET(!m_font.IsNull(), wxT("wxQtContext::DrawText - no valid font set"));

        wxQtFontData* font_data = static_cast<wxQtFontData*>(m_font.GetRefData());

        m_qtPainter->setFont(font_data->GetFont());

        QFontMetrics metrics = m_qtPainter->fontMetrics();
        QRect bounding_rect = metrics.boundingRect(QString(str));

        if (width) *width = bounding_rect.width();
        if (height) *height = bounding_rect.height();
        if (descent) *descent = metrics.descent();
        if (externalLeading) *externalLeading = metrics.leading() - (metrics.ascent() + metrics.descent());
    }

	virtual void GetPartialTextExtents(const wxString& text, wxArrayDouble& widths) const wxOVERRIDE
	{
	}

protected:
	virtual void DoDrawText(const wxString &str, wxDouble x, wxDouble y) wxOVERRIDE
	{
        wxCHECK_RET(!m_font.IsNull(), wxT("wxQtContext::DrawText - no valid font set"));

        wxQtFontData* font_data = static_cast<wxQtFontData*>(m_font.GetRefData());

        m_qtPainter->setFont(font_data->GetFont());
        m_qtPainter->setPen(QPen(font_data->GetColor()));
        QFontMetrics metrics = m_qtPainter->fontMetrics();

        m_qtPainter->drawText(x, y+metrics.ascent(), QString(str));
	}

	enum ApplyTransformMode { Apply_directly, Apply_scaled_dev_origin };

#ifdef __WXQT__
	QPainter* m_qtPainter;
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

#if wxUSE_IMAGE
wxGraphicsContext * wxQtGraphicsRenderer::CreateContextFromImage(wxImage& image)
{
//	return new wxCairoImageContext(this, image);
	return NULL;
}
#endif // wxUSE_IMAGE

wxGraphicsContext * wxQtGraphicsRenderer::CreateMeasuringContext()
{
	//return new wxCairoMeasuringContext(this);
	return NULL;
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
	wxQtMatrixData* data = new wxQtMatrixData(this);
	data->Set(a, b, c, d, tx, ty);
	m.SetRefData(data);
	return m;
}

wxGraphicsPen wxQtGraphicsRenderer::CreatePen(const wxGraphicsPenInfo& info)
{
	wxGraphicsPen p;
	if (info.GetStyle() != wxPENSTYLE_TRANSPARENT)
	{
		p.SetRefData(new wxQtPenData(this, info));
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
	wxQtBrushData* d = new wxQtBrushData(this);
	d->CreateLinearGradientBrush(x1, y1, x2, y2, stops);
	p.SetRefData(d);
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
		p.SetRefData(new wxQtFontData(this, font, col));
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
	if (major) *major = 1;
    if (minor) *minor = 0;
    if (micro) *micro = 0;
}

static wxQtGraphicsRenderer gs_qtGraphicsRenderer;

// MSW and OS X have their own native default renderers, but the other ports
// use Cairo by default
wxGraphicsRenderer* wxGraphicsRenderer::GetDefaultRenderer()
{
	return &gs_qtGraphicsRenderer;
}


#endif // wxUSE_GRAPHICS_CONTEXT
