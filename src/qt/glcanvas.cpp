/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/glcanvas.cpp
// Author:      Sean D'Epagnier
// Copyright:   (c) Sean D'Epagnier 2014
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_GLCANVAS

#include "wx/qt/private/winevent.h"
#include "wx/glcanvas.h"

#include <QOpenGLWidget>
#include <QSurfaceFormat>
#include <QtWidgets/QGestureRecognizer>
#include <QtWidgets/QGestureEvent>

wxGCC_WARNING_SUPPRESS(unused-parameter)

class wxQtGLWidget : public wxQtEventSignalHandler< QOpenGLWidget, wxGLCanvas >
{
public:
    wxQtGLWidget(wxWindow *parent, wxGLCanvas *handler, QSurfaceFormat format)
        : wxQtEventSignalHandler<QOpenGLWidget, wxGLCanvas>(parent, handler)
    {
        setFormat(format);
        setFocusPolicy(Qt::StrongFocus);
    }

protected:
    virtual void resizeEvent ( QResizeEvent * event ) override;
    virtual void paintEvent ( QPaintEvent * event ) override;

    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;
};

void wxQtGLWidget::resizeEvent ( QResizeEvent * event )
{
    QOpenGLWidget::resizeEvent(event);
}

void wxQtGLWidget::paintEvent ( QPaintEvent * event )
{
    QOpenGLWidget::paintEvent(event);
}

void wxQtGLWidget::resizeGL(int w, int h)
{
    QResizeEvent qevt(QSize(w, h), QSize(w, h));
    GetHandler()->QtHandleResizeEvent(this, &qevt);
}

void wxQtGLWidget::paintGL()
{
    wxPaintEvent event( GetHandler() );
    EmitEvent(event);
}

// ----------------------------------------------------------------------------
// wxGLContextAttrs: OpenGL rendering context attributes
// ----------------------------------------------------------------------------
// GLX specific values

wxGLContextAttrs& wxGLContextAttrs::CoreProfile()
{
//    AddAttribBits(GLX_CONTEXT_PROFILE_MASK_ARB,
//                  GLX_CONTEXT_CORE_PROFILE_BIT_ARB);
    AddAttribute(WX_GL_COMPAT_PROFILE);
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::MajorVersion(int val)
{
    if ( val > 0 )
    {
        AddAttribute(WX_GL_MAJOR_VERSION);
        AddAttribute(val);

        if ( val >= 3 )
            SetNeedsARB();
    }
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::MinorVersion(int val)
{
    if ( val >= 0 )
    {
        AddAttribute(WX_GL_MINOR_VERSION);
        AddAttribute(val);
    }
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::CompatibilityProfile()
{
    AddAttribute(WX_GL_COMPAT_PROFILE);
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::ForwardCompatible()
{
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::ES2()
{
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::DebugCtx()
{
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::Robust()
{
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::NoResetNotify()
{
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::LoseOnReset()
{
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::ResetIsolation()
{
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::ReleaseFlush(int val)
{
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::PlatformDefaults()
{
    renderTypeRGBA = true;
    return *this;
}

void wxGLContextAttrs::EndList()
{
    AddAttribute(0);
}

// ----------------------------------------------------------------------------
// wxGLAttributes: Visual/FBconfig attributes
// ----------------------------------------------------------------------------
// GLX specific values

//   Different versions of GLX API use rather different attributes lists, see
//   the following URLs:
//
//   - <= 1.2: http://www.opengl.org/sdk/docs/man/xhtml/glXChooseVisual.xml
//   - >= 1.3: http://www.opengl.org/sdk/docs/man/xhtml/glXChooseFBConfig.xml
//
//   Notice in particular that
//   - GLX_RGBA is boolean attribute in the old version of the API but a
//     value of GLX_RENDER_TYPE in the new one
//   - Boolean attributes such as GLX_DOUBLEBUFFER don't take values in the
//     old version but must be followed by True or False in the new one.

wxGLAttributes& wxGLAttributes::RGBA()
{
    AddAttribute(WX_GL_RGBA);
    return *this;
}

wxGLAttributes& wxGLAttributes::BufferSize(int val)
{
    if ( val >= 0 )
    {
        AddAttribute(WX_GL_BUFFER_SIZE);
        AddAttribute(val);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::Level(int val)
{
    AddAttribute(WX_GL_LEVEL);
    AddAttribute(val);
    return *this;
}

wxGLAttributes& wxGLAttributes::DoubleBuffer()
{
    AddAttribute(WX_GL_DOUBLEBUFFER);
    return *this;
}

wxGLAttributes& wxGLAttributes::Stereo()
{
    AddAttribute(WX_GL_STEREO);
    return *this;
}

wxGLAttributes& wxGLAttributes::AuxBuffers(int val)
{
    if ( val >= 0 )
    {
        AddAttribute(WX_GL_AUX_BUFFERS);
        AddAttribute(val);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::MinRGBA(int mRed, int mGreen, int mBlue, int mAlpha)
{
    if ( mRed >= 0)
    {
        AddAttribute(WX_GL_MIN_RED);
        AddAttribute(mRed);
    }
    if ( mGreen >= 0)
    {
        AddAttribute(WX_GL_MIN_GREEN);
        AddAttribute(mGreen);
    }
    if ( mBlue >= 0)
    {
        AddAttribute(WX_GL_MIN_BLUE);
        AddAttribute(mBlue);
    }
    if ( mAlpha >= 0)
    {
        AddAttribute(WX_GL_MIN_ALPHA);
        AddAttribute(mAlpha);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::Depth(int val)
{
    if ( val >= 0 )
    {
        AddAttribute(WX_GL_DEPTH_SIZE);
        AddAttribute(val);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::Stencil(int val)
{
    if ( val >= 0 )
    {
        AddAttribute(WX_GL_DEPTH_SIZE);
        AddAttribute(val);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::MinAcumRGBA(int mRed, int mGreen, int mBlue, int mAlpha)
{
    if ( mRed >= 0)
    {
        AddAttribute(WX_GL_MIN_ACCUM_RED);
        AddAttribute(mRed);
    }
    if ( mGreen >= 0)
    {
        AddAttribute(WX_GL_MIN_ACCUM_GREEN);
        AddAttribute(mGreen);
    }
    if ( mBlue >= 0)
    {
        AddAttribute(WX_GL_MIN_ACCUM_BLUE);
        AddAttribute(mBlue);
    }
    if ( mAlpha >= 0)
    {
        AddAttribute(WX_GL_MIN_ACCUM_ALPHA);
        AddAttribute(mAlpha);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::SampleBuffers(int val)
{
    if ( val >= 0 )
    {
        AddAttribute(WX_GL_SAMPLE_BUFFERS);
        AddAttribute(val);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::Samplers(int val)
{
    if ( val >= 0 )
    {
        AddAttribute(WX_GL_SAMPLES);
        AddAttribute(val);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::FrameBuffersRGB()
{
//    AddAttribute(GLX_FRAMEBUFFER_SRGB_CAPABLE_ARB);
//    AddAttribute(True);
    return *this;
}

void wxGLAttributes::EndList()
{
    AddAttribute(0);
}

wxGLAttributes& wxGLAttributes::PlatformDefaults()
{
    // No Qt specific values
    return *this;
}


//---------------------------------------------------------------------------
// wxGlContext
//---------------------------------------------------------------------------

wxIMPLEMENT_CLASS(wxGLContext, wxWindow);

wxGLContext::wxGLContext(wxGLCanvas *win,
                         const wxGLContext *other,
                         const wxGLContextAttrs *ctxAttrs)
{
    m_isOk = true;
}

bool wxGLContext::SetCurrent(const wxGLCanvas& win) const
{
    QOpenGLWidget *qglWidget = static_cast<QOpenGLWidget *>(win.GetHandle());
    QOpenGLContext *context = qglWidget->context();

    if (!m_glContext)
        const_cast<wxGLContext *>(this)->m_glContext = context;

    if (context != m_glContext)
    {
        // I think I must destroy and recreate the QOpenGLWidget to change the context?
        wxLogDebug("Calling wxGLContext::SetCurrent with a different canvas is not supported in wxQt");
        return false;
    }

    qglWidget->makeCurrent();
    return true;
}

//---------------------------------------------------------------------------
// PanGestureRecognizer - helper class for wxGLCanvas
//---------------------------------------------------------------------------

class PanGestureRecognizer : public QGestureRecognizer
{
private:
    static const int MINIMUM_DISTANCE = 10;

    typedef QGestureRecognizer parent;

    bool IsValidMove(int dx, int dy);

    virtual QGesture* create(QObject* pTarget);

    virtual QGestureRecognizer::Result recognize(QGesture* pGesture, QObject *pWatched, QEvent *pEvent);

    void reset (QGesture *pGesture);

    QPointF m_startPoint;
    QPointF m_lastPoint;
};

//---------------------------------------------------------------------------
// wxGlCanvas
//---------------------------------------------------------------------------

wxIMPLEMENT_CLASS(wxGLCanvas, wxWindow);

wxGLCanvas::wxGLCanvas(wxWindow *parent,
                       const wxGLAttributes& dispAttrs,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name,
                       const wxPalette& palette)
{
    Create(parent, dispAttrs, id, pos, size, style, name, palette);
}

wxGLCanvas::wxGLCanvas(wxWindow *parent,
                       wxWindowID id,
                       const int *attribList,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name,
                       const wxPalette& palette)
{
    Create(parent, id, pos, size, style, name, attribList, palette);
}

wxGLCanvas::~wxGLCanvas()
{
    // Avoid sending further signals (i.e. if deleting the current page)
    m_qtWindow->blockSignals(true);
}

bool wxGLCanvas::Create(wxWindow *parent,
                        const wxGLAttributes& dispAttrs,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name,
                        const wxPalette& palette)
{
    const int* attrsList = dispAttrs.GetGLAttrs();

    wxCHECK_MSG(attrsList, false, "wxGLAttributes object is empty.");

    return Create(parent, id, pos, size, style, name, attrsList, palette);
}

bool wxGLCanvas::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name,
                        const int *attribList,
                        const wxPalette& palette)
{
#if wxUSE_PALETTE
    wxASSERT_MSG( !palette.IsOk(), wxT("palettes not supported") );
#endif // wxUSE_PALETTE
    wxUnusedVar(palette); // Unused when wxDEBUG_LEVEL==0

    // Separate display/context attributes, set defaults.
    wxGLAttributes dispAttrs;
    wxGLContextAttrs ctxAttrs;
    if (!ParseAttribList(attribList, dispAttrs, &ctxAttrs))
        return false;

    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    if (!wxGLCanvas::ConvertWXAttrsToQtGL(dispAttrs, ctxAttrs, format))
        return false;

    // Return false if any attribute is unsupported
    if ( !IsDisplaySupported(attribList) )
    {
        wxFAIL_MSG("Can't find a pixel format for the requested attributes");
        return false;
    }

    m_qtWindow = new wxQtGLWidget(parent, this, format);

    // Create and register a custom pan recognizer, available to all instances of this class.
    QGestureRecognizer* pPanRecognizer = new PanGestureRecognizer();
    QGestureRecognizer::registerRecognizer(pPanRecognizer);

    if ( !wxWindow::Create( parent, id, pos, size, style, name ) )
        return false;

    SetBackgroundStyle(wxBG_STYLE_PAINT);

    return true;
}

bool wxGLCanvas::SwapBuffers()
{
    // Not possible
    return true;
}

bool wxGLCanvas::QtCanPaintWithoutActivePainter() const
{
    return true;
}

/* static */
bool wxGLCanvas::ConvertWXAttrsToQtGL(const wxGLAttributes &wxGLAttrs, const wxGLContextAttrs wxCtxAttrs, QSurfaceFormat &format)
{
    const int *glattrs = wxGLAttrs.GetGLAttrs();
    const int *ctxattrs = wxCtxAttrs.GetGLAttrs();

    // set default parameters to false
    format.setDepthBufferSize(0);
    format.setAlphaBufferSize(0);
    format.setStencilBufferSize(0);

    for (int arg = 0; glattrs && glattrs[arg] != 0; arg++)
    {
        // indicates whether we have a boolean attribute
        bool isBoolAttr = false;

        int v = glattrs[arg+1];
        switch ( glattrs[arg] )
        {
            // Pixel format attributes

            case WX_GL_BUFFER_SIZE:
                // Not supported
                return false;

            case WX_GL_LEVEL:
                // Not supported
                return false;

            case WX_GL_RGBA:
                // Non-RGBA is not supported
                isBoolAttr = true;
                break;

            case WX_GL_DOUBLEBUFFER:
                // Since QOpenGLWidget copies the framebuffer data to a
                // texture, we already have tear-free behaviour.
                // Using SwapBehavior::DoubleBuffer just increases latency.
                isBoolAttr = true;
                break;

            case WX_GL_STEREO:
                format.setStereo(true);
                isBoolAttr = true;
                break;

            case WX_GL_AUX_BUFFERS:
                // don't know how to implement
                return false;

            case WX_GL_MIN_RED:
                format.setRedBufferSize(v);
                break;

            case WX_GL_MIN_GREEN:
                format.setGreenBufferSize(v);
                break;

            case WX_GL_MIN_BLUE:
                format.setBlueBufferSize(v);
                break;

            case WX_GL_MIN_ALPHA:
                format.setAlphaBufferSize(v);
                break;

            case WX_GL_DEPTH_SIZE:
                format.setDepthBufferSize(v);
                break;

            case WX_GL_STENCIL_SIZE:
                format.setStencilBufferSize(v);
                break;

            case WX_GL_MIN_ACCUM_RED:
            case WX_GL_MIN_ACCUM_GREEN:
            case WX_GL_MIN_ACCUM_BLUE:
            case WX_GL_MIN_ACCUM_ALPHA:
                // Not supported
                return false;

            case WX_GL_SAMPLE_BUFFERS:
                format.setSamples(v > 0 ? std::max(4, format.samples()) : -1);
                // can we somehow indicate if it's not supported?
                break;

            case WX_GL_SAMPLES:
                format.setSamples(v);
                // can we somehow indicate if it's not supported?
                break;

            default:
                wxLogDebug(wxT("Unsupported OpenGL attribute %d"),
                           glattrs[arg]);
                continue;
        }

        if ( !isBoolAttr )
        {
            if ( !v )
                return false; // zero parameter
            arg++;
        }
    }

    for (int arg = 0; ctxattrs && ctxattrs[arg] != 0; arg++)
    {
        // indicates whether we have a boolean attribute
        bool isBoolAttr = false;

        int v = ctxattrs[arg+1];
        switch ( ctxattrs[arg] )
        {
            // Context attributes

            case WX_GL_MAJOR_VERSION:
                format.setVersion ( v, format.minorVersion() );
                break;

            case WX_GL_MINOR_VERSION:
                format.setVersion ( format.majorVersion(), v );
                break;

            case WX_GL_CORE_PROFILE:
                format.setProfile(QSurfaceFormat::CoreProfile);
                break;

            case WX_GL_COMPAT_PROFILE:
                format.setProfile(QSurfaceFormat::CompatibilityProfile);
                break;

            default:
                wxLogDebug(wxT("Unsupported OpenGL attribute %d"),
                           ctxattrs[arg]);
                continue;
        }

        if ( !isBoolAttr )
        {
            if ( !v )
                return false; // zero parameter
            arg++;
        }
    }

    return true;
}

/* static */
bool wxGLCanvasBase::IsDisplaySupported(const wxGLAttributes& dispAttrs)
{
    const int* attrsList = dispAttrs.GetGLAttrs();

    wxCHECK_MSG(attrsList, false, "wxGLAttributes object is empty.");

    return IsDisplaySupported(attrsList);
}

/* static */
bool wxGLCanvasBase::IsDisplaySupported(const int *attribList)
{
    // Separate display/context attributes, set defaults.
    wxGLAttributes dispAttrs;
    wxGLContextAttrs ctxAttrs;
    if (!ParseAttribList(attribList, dispAttrs, &ctxAttrs))
        return false;

    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    if (!wxGLCanvas::ConvertWXAttrsToQtGL(dispAttrs, ctxAttrs, format))
        return false;

    return true;
}

// ----------------------------------------------------------------------------
// wxGLApp
// ----------------------------------------------------------------------------

bool wxGLApp::InitGLVisual(const int *attribList)
{
    wxLogError("Missing implementation of " + wxString(__func__));
    return false;
}

// -----------------------------------------------------------------------------------------
//  We want a private pan gesture recognizer for GL canvas,
//  since the Qt standard recognizers do not function well for this window.
// -----------------------------------------------------------------------------------------

bool
PanGestureRecognizer::IsValidMove(int dx, int dy)
{
   // The moved distance is to small to count as not just a glitch.
   if ((qAbs(dx) < MINIMUM_DISTANCE) && (qAbs(dy) < MINIMUM_DISTANCE))
      return false;

   return true;
}


// virtual
QGesture*
PanGestureRecognizer::create(QObject* pTarget)
{
   return new QPanGesture(pTarget);
}


// virtual
QGestureRecognizer::Result
PanGestureRecognizer::recognize(QGesture* pGesture, QObject *pWatched, QEvent *pEvent)
{
    wxUnusedVar(pWatched);
    QGestureRecognizer::Result result = QGestureRecognizer::Ignore;
    QPanGesture *pPan = static_cast<QPanGesture*>(pGesture);

    const QTouchEvent *ev = static_cast<const QTouchEvent *>(pEvent);

    switch (pEvent->type())
    {
        case QEvent::TouchBegin:
            {
                QTouchEvent::TouchPoint p1 = ev->touchPoints().at(0);
                m_startPoint = p1.startScreenPos().toPoint();
                m_lastPoint = m_startPoint;

                pPan->setLastOffset(QPointF(0,0));
                pPan->setOffset(QPointF(0,0));

                result = QGestureRecognizer::MayBeGesture;
            }
            break;

        case QEvent::TouchEnd:
            {
                QTouchEvent::TouchPoint p1 = ev->touchPoints().at(0);
                QPointF endPoint = p1.screenPos().toPoint();

                pPan->setLastOffset(pPan->offset());
                pPan->setOffset(QPointF(p1.pos().x() - p1.startPos().x(),
                                        p1.pos().y() - p1.startPos().y()));

                pPan->setHotSpot(p1.startScreenPos());

                // process distance and direction
                int dx = endPoint.x() - m_startPoint.x();
                int dy = endPoint.y() - m_startPoint.y();

                if (!IsValidMove(dx, dy))
                {
                    // Just a click, so no gesture.
                    result = QGestureRecognizer::Ignore;
                }
                else
                {
                    result = QGestureRecognizer::FinishGesture;
                }

            }
            break;

        case QEvent::TouchUpdate:
            {
                QTouchEvent::TouchPoint p1 = ev->touchPoints().at(0);
                QPointF upPoint = p1.screenPos().toPoint();

                pPan->setLastOffset(pPan->offset());
                pPan->setOffset(QPointF(p1.pos().x() - p1.startPos().x(),
                                        p1.pos().y() - p1.startPos().y()));

                pPan->setHotSpot(p1.startScreenPos());

                int dx = upPoint.x() - m_lastPoint.x();
                int dy = upPoint.y() - m_lastPoint.y();

                if( (dx > 2) || (dx < -2) || (dy > 2) || (dy < -2))
                {
                    result = QGestureRecognizer::TriggerGesture;

                }
                else
                {
                    result = QGestureRecognizer::Ignore;
                }

                m_lastPoint = upPoint;
            }
            break;

        default:
            break;
    }

    return result;
}

void
PanGestureRecognizer::reset(QGesture *pGesture)
{
    pGesture->setProperty("startPoint", QVariant(QVariant::Invalid));
    parent::reset(pGesture);
}

#endif // wxUSE_GLCANVAS
