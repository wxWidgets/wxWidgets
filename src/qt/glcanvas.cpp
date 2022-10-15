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

#include <QtOpenGL/QGLWidget>
#include <QtWidgets/QGestureRecognizer>
#include <QtWidgets/QGestureEvent>

#if defined(__VISUALC__)
    #pragma message("OpenGL support is not implemented in wxQt")
#else
    #warning "OpenGL support is not implemented in wxQt"
#endif
wxGCC_WARNING_SUPPRESS(unused-parameter)

class wxQtGLWidget : public wxQtEventSignalHandler< QGLWidget, wxGLCanvas >
{
public:
    wxQtGLWidget(wxWindow *parent, wxGLCanvas *handler, QGLFormat format)
        : wxQtEventSignalHandler<QGLWidget,wxGLCanvas>(parent, handler)
        {
            setFormat(format);
            setAutoBufferSwap( false );
        }

protected:
    virtual void showEvent ( QShowEvent * event ) override;
    virtual void hideEvent ( QHideEvent * event ) override;
    virtual void resizeEvent ( QResizeEvent * event ) override;
    virtual void paintEvent ( QPaintEvent * event ) override;

    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;
};

void wxQtGLWidget::showEvent ( QShowEvent * event )
{
    QGLWidget::showEvent( event );
}

void wxQtGLWidget::hideEvent ( QHideEvent * event )
{
    QGLWidget::hideEvent( event );
}

void wxQtGLWidget::resizeEvent ( QResizeEvent * event )
{
    QGLWidget::resizeEvent(event);
}

void wxQtGLWidget::paintEvent ( QPaintEvent * event )
{
    QGLWidget::paintEvent(event);
}

void wxQtGLWidget::resizeGL(int w, int h)
{
    wxSizeEvent event( wxSize(w, h) );
    EmitEvent(event);
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
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::MajorVersion(int val)
{
    if ( val > 0 )
    {
        if ( val >= 3 )
            SetNeedsARB();
    }
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::MinorVersion(int val)
{
    if ( val >= 0 )
    {
    }
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::CompatibilityProfile()
{
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
//    AddAttribute(None);
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
    return *this;
}

wxGLAttributes& wxGLAttributes::BufferSize(int val)
{
    if ( val >= 0 )
    {
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::Level(int val)
{
//    AddAttribute(GLX_LEVEL);
    AddAttribute(val);
    return *this;
}

wxGLAttributes& wxGLAttributes::DoubleBuffer()
{
    return *this;
}

wxGLAttributes& wxGLAttributes::Stereo()
{
    return *this;
}

wxGLAttributes& wxGLAttributes::AuxBuffers(int val)
{
    if ( val >= 0 )
    {
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::MinRGBA(int mRed, int mGreen, int mBlue, int mAlpha)
{
    if ( mRed >= 0)
    {
    }
    if ( mGreen >= 0)
    {
    }
    if ( mBlue >= 0)
    {
    }
    if ( mAlpha >= 0)
    {
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::Depth(int val)
{
    if ( val >= 0 )
    {
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::Stencil(int val)
{
    if ( val >= 0 )
    {
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::MinAcumRGBA(int mRed, int mGreen, int mBlue, int mAlpha)
{
    if ( mRed >= 0)
    {
    }
    if ( mGreen >= 0)
    {
    }
    if ( mBlue >= 0)
    {
    }
    if ( mAlpha >= 0)
    {
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::SampleBuffers(int val)
{
#ifdef GLX_SAMPLE_BUFFERS_ARB
    if ( val >= 0 && wxGLCanvasX11::IsGLXMultiSampleAvailable() )
    {
        AddAttribute(GLX_SAMPLE_BUFFERS_ARB);
        AddAttribute(val);
    }
#endif
    return *this;
}

wxGLAttributes& wxGLAttributes::Samplers(int val)
{
#ifdef GLX_SAMPLES_ARB
    if ( val >= 0 && wxGLCanvasX11::IsGLXMultiSampleAvailable() )
    {
        AddAttribute(GLX_SAMPLES_ARB);
        AddAttribute(val);
    }
#endif
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
}

wxGLAttributes& wxGLAttributes::PlatformDefaults()
{
    // No GLX specific values
    return *this;
}

wxGLAttributes& wxGLAttributes::Defaults()
{
    RGBA().DoubleBuffer();
//    if ( wxGLCanvasX11::GetGLXVersion() < 13 )
//        Depth(1).MinRGBA(1, 1, 1, 0);
//    else
        Depth(16).SampleBuffers(1).Samplers(4);
    return *this;
}


//---------------------------------------------------------------------------
// wxGlContext
//---------------------------------------------------------------------------

wxIMPLEMENT_CLASS(wxGLContext, wxWindow);

wxGLContext::wxGLContext(wxGLCanvas *WXUNUSED(win), const wxGLContext* WXUNUSED(other), const wxGLContextAttrs *WXUNUSED(ctxAttrs))
{
}

bool wxGLContext::SetCurrent(const wxGLCanvas&) const
{
// I think I must destroy and recreate the QGLWidget to change the context?
//    win->GetHandle()->makeCurrent();
    return false;
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
    wxLogError("Missing implementation of " + wxString(__FUNCTION__));
    return false;
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

    QGLFormat format;
    if (!wxGLCanvas::ConvertWXAttrsToQtGL(attribList, format))
        return false;

    m_qtWindow = new wxQtGLWidget(parent, this, format);

    // Create and register a custom pan recognizer, available to all instances of this class.
    QGestureRecognizer* pPanRecognizer = new PanGestureRecognizer();
    QGestureRecognizer::registerRecognizer(pPanRecognizer);

    return wxWindow::Create( parent, id, pos, size, style, name );
}

bool wxGLCanvas::SwapBuffers()
{
    static_cast<QGLWidget *>(m_qtWindow)->swapBuffers();
    return true;
}

/* static */
bool wxGLCanvas::ConvertWXAttrsToQtGL(const int *wxattrs, QGLFormat &format)
{
    if (!wxattrs)
        return true;

    // set default parameters to false
    format.setDoubleBuffer(false);
    format.setDepth(false);
    format.setAlpha(false);
    format.setStencil(false);

    for ( int arg = 0; wxattrs[arg] != 0; arg++ )
    {
        // indicates whether we have a boolean attribute
        bool isBoolAttr = false;

        int v = wxattrs[arg+1];
        switch ( wxattrs[arg] )
        {
            case WX_GL_BUFFER_SIZE:
                format.setRgba(false);
                // I do not know how to set the buffer size, so fail
                return false;

            case WX_GL_LEVEL:
                format.setPlane(v);
                break;

            case WX_GL_RGBA:
                format.setRgba(true);
                isBoolAttr = true;
                break;

            case WX_GL_DOUBLEBUFFER:
                format.setDoubleBuffer(true);
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
                format.setRedBufferSize(v*8);
                break;

            case WX_GL_MIN_GREEN:
                format.setGreenBufferSize(v);
                break;

            case WX_GL_MIN_BLUE:
                format.setBlueBufferSize(v);
                break;

            case WX_GL_MIN_ALPHA:
                format.setAlpha(true);
                format.setAlphaBufferSize(v);
                break;

            case WX_GL_DEPTH_SIZE:
                format.setDepth(true);
                format.setDepthBufferSize(v);
                break;

            case WX_GL_STENCIL_SIZE:
                format.setStencil(true);
                format.setStencilBufferSize(v);
                break;

            case WX_GL_MIN_ACCUM_RED:
            case WX_GL_MIN_ACCUM_GREEN:
            case WX_GL_MIN_ACCUM_BLUE:
            case WX_GL_MIN_ACCUM_ALPHA:
                format.setAccumBufferSize(v);
                break;

            case WX_GL_SAMPLE_BUFFERS:
                format.setSampleBuffers(v);
                // can we somehow indicate if it's not supported?
                break;

            case WX_GL_SAMPLES:
                format.setSamples(v);
                // can we somehow indicate if it's not supported?
                break;

            case WX_GL_MAJOR_VERSION:
                 format.setVersion ( v,0 );
                 break;

            default:
                wxLogDebug(wxT("Unsupported OpenGL attribute %d"),
                           wxattrs[arg]);
                continue;
        }

        if ( !isBoolAttr ) {
            if ( !v )
                return false; // zero parameter
            arg++;
        }
    }

    return true;
}

/* static */
bool
wxGLCanvasBase::IsDisplaySupported(const int *attribList)
{
    QGLFormat format;

    if (!wxGLCanvas::ConvertWXAttrsToQtGL(attribList, format))
        return false;

    return QGLWidget(format).isValid();
}

/* static */
bool
wxGLCanvasBase::IsDisplaySupported(const wxGLAttributes& dispAttrs)
{
    wxLogError("Missing implementation of " + wxString(__FUNCTION__));
    return false;
}

// ----------------------------------------------------------------------------
// wxGLApp
// ----------------------------------------------------------------------------

bool wxGLApp::InitGLVisual(const int *attribList)
{
    wxLogError("Missing implementation of " + wxString(__FUNCTION__));
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
