/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dc.cpp
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2009 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include <QtGui/QBitmap>
#include <QtGui/QPen>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>

#ifndef WX_PRECOMP
    #include "wx/icon.h"
    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/dc.h"
#include "wx/qt/dc.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/compat.h"

#include <QtGui/QScreen>
#include <QtWidgets/QApplication>

static void SetPenColour( QPainter *qtPainter, QColor col )
{
    QPen p = qtPainter->pen();
    p.setColor( col );
    qtPainter->setPen( p );
}

static void SetBrushColour( QPainter *qtPainter, QColor col )
{
    QBrush b = qtPainter->brush();
    b.setColor( col );
    qtPainter->setBrush( b );
}

class QtDCOffsetHelper
{
public:
    QtDCOffsetHelper(QPainter *qpainter)
    {
        m_shouldOffset = ShouldOffset(qpainter->pen());
        if (!m_shouldOffset)
            return;

        m_qp = qpainter;
        m_offset = 0.5;

        m_qp->translate(m_offset, m_offset);
    }

    ~QtDCOffsetHelper()
    {
        if (m_shouldOffset)
            m_qp->translate(-m_offset, -m_offset);
    }

    bool ShouldOffset(const QPen &pen) const
    {
        if (pen.style() == Qt::NoPen)
            return false;

        double width = pen.widthF();

        // always offset for 1-pixel width
        if (width <= 0)
            return true;

        // offset if pen width is odd integer
        const int w = int(width);
        return (w & 1) && wxIsSameDouble(width, w);
    }

private:
    QPainter *m_qp;
    double m_offset;
    bool m_shouldOffset;
};

wxIMPLEMENT_CLASS(wxQtDCImpl,wxDCImpl);

wxQtDCImpl::wxQtDCImpl( wxDC *owner )
    : wxDCImpl( owner )
{
    m_qtPixmap = nullptr;
    m_qtPainter = nullptr;
    m_rasterColourOp = wxQtNONE;
    m_qtPenColor = new QColor;
    m_qtBrushColor = new QColor;
    m_ok = true;
}

wxQtDCImpl::~wxQtDCImpl()
{
    if ( m_qtPainter )
    {
        if( m_qtPainter->isActive() )
        {
            m_qtPainter->end();
        }
        delete m_qtPainter;
    }

    delete m_qtPenColor;
    delete m_qtBrushColor;
}

void wxQtDCImpl::QtPreparePainter( )
{
    //Do here all QPainter initialization (called after each begin())
    if ( m_qtPainter == nullptr )
    {
        wxLogDebug(wxT("wxQtDCImpl::QtPreparePainter is null!!!"));
    }
    else if ( m_qtPainter->isActive() )
    {
        m_qtPainter->setPen( wxPen().GetHandle() );
        m_qtPainter->setBrush( wxBrush().GetHandle() );
        m_qtPainter->setFont( wxFont().GetHandle() );

        if (m_qtPainter->device()->depth() > 1)
        {
            m_qtPainter->setRenderHints(QPainter::Antialiasing,
                                        true);
        }

        if (m_clipping)
        {
            m_qtPainter->setClipRegion( m_clippingRegion.GetHandle() );
        }
    }
    else
    {
//        wxLogDebug(wxT("wxQtDCImpl::QtPreparePainter not active!"));
    }
}

bool wxQtDCImpl::CanDrawBitmap() const
{
    return true;
}

bool wxQtDCImpl::CanGetTextExtent() const
{
    return true;
}

void wxQtDCImpl::DoGetSize(int *width, int *height) const
{
    QPaintDevice *pDevice = m_qtPainter->device();

    int deviceWidth;
    int deviceHeight;

    if ( pDevice )
    {
        deviceWidth = pDevice->width();
        deviceHeight = pDevice->height();
    }
    else
    {
        deviceWidth = 0;
        deviceHeight = 0;

    }
    if ( width )
        *width = deviceWidth;
    if ( height )
        *height = deviceHeight;
}

void wxQtDCImpl::DoGetSizeMM(int* width, int* height) const
{
    QPaintDevice *pDevice = m_qtPainter->device();

    int deviceWidthMM;
    int deviceHeightMM;

    if ( pDevice )
    {
        deviceWidthMM = pDevice->widthMM();
        deviceHeightMM = pDevice->heightMM();
    }
    else
    {
        deviceWidthMM = 0;
        deviceHeightMM = 0;
    }

    if ( width )
        *width = deviceWidthMM;
    if ( height )
        *height = deviceHeightMM;
}

int wxQtDCImpl::GetDepth() const
{
    return m_qtPainter->device()->depth();
}

wxSize wxQtDCImpl::GetPPI() const
{
    QScreen *srn = QApplication::screens().at(0);
    if (!srn)
        return wxSize(m_qtPainter->device()->logicalDpiX(), m_qtPainter->device()->logicalDpiY());
    qreal dotsPerInch = srn->logicalDotsPerInch();
    return wxSize(round(dotsPerInch), round(dotsPerInch));
}

void wxQtDCImpl::SetFont(const wxFont& font)
{
    m_font = font;

    if (m_qtPainter->isActive())
        m_qtPainter->setFont(font.GetHandle());
}

void wxQtDCImpl::SetPen(const wxPen& pen)
{
    m_pen = pen;

    if ( !m_pen.IsOk() ) return;

    m_qtPainter->setPen(pen.GetHandle());

    ApplyRasterColourOp();
}

void wxQtDCImpl::SetBrush(const wxBrush& brush)
{
    m_brush = brush;

    if ( !m_brush.IsOk() ) return;

    if (brush.GetStyle() == wxBRUSHSTYLE_STIPPLE_MASK_OPAQUE)
    {
        // Use a monochrome mask: use foreground color for the mask
        m_brush.SetColour(m_textForegroundColour);
    }

    m_qtPainter->setBrush(m_brush.GetHandle());

    ApplyRasterColourOp();
}

void wxQtDCImpl::SetBackground(const wxBrush& brush)
{
    m_backgroundBrush = brush;

    // For consistency with the other ports: clearing the dc with
    // invalid brush (Qt::NoBrush) should use white colour (which
    // happens to be the default colour in Qt too) instead of no
    // colour at all.
    if (!m_backgroundBrush.IsOk())
        m_backgroundBrush = *wxWHITE_BRUSH;

    if (m_qtPainter->isActive())
    {
        m_qtPainter->setBackground(m_backgroundBrush.GetHandle());
    }
}

void wxQtDCImpl::SetBackgroundMode(int mode)
{
    /* Do not change QPainter, as wx uses this background mode
     * only for drawing text, where Qt uses it for everything.
     * Always let QPainter mode to transparent, and change it
     * when needed */
    m_backgroundMode = mode;
}

#include <QtGui/QPen>
#include <QtGui/QPainter>
#include <QtGui/QScreen>
#include <QtWidgets/QApplication>

#if wxUSE_PALETTE
void wxQtDCImpl::SetPalette(const wxPalette& WXUNUSED(palette))
{
    wxMISSING_IMPLEMENTATION(__func__);
}
#endif // wxUSE_PALETTE

void wxQtDCImpl::SetLogicalFunction(wxRasterOperationMode function)
{
    m_logicalFunction = function;

    wxQtRasterColourOp rasterColourOp = wxQtNONE;
    switch ( function )
    {
        case wxCLEAR:       // 0
            m_qtPainter->setCompositionMode( QPainter::CompositionMode_SourceOver );
            rasterColourOp = wxQtBLACK;
            break;
        case wxXOR:         // src XOR dst
            m_qtPainter->setCompositionMode( QPainter::RasterOp_SourceXorDestination );
            break;
        case wxINVERT:      // NOT dst => dst XOR WHITE
            m_qtPainter->setCompositionMode( QPainter::RasterOp_SourceXorDestination );
            rasterColourOp = wxQtWHITE;
            break;
        case wxOR_REVERSE:  // src OR (NOT dst) => (NOT (NOT src)) OR (NOT dst)
            m_qtPainter->setCompositionMode( QPainter::RasterOp_NotSourceOrNotDestination );
            rasterColourOp = wxQtINVERT;
            break;
        case wxAND_REVERSE: // src AND (NOT dst)
            m_qtPainter->setCompositionMode( QPainter::RasterOp_SourceAndNotDestination );
            break;
        case wxCOPY:        // src
            m_qtPainter->setCompositionMode( QPainter::CompositionMode_SourceOver );
            break;
        case wxAND:         // src AND dst
            m_qtPainter->setCompositionMode( QPainter::RasterOp_SourceAndDestination );
            break;
        case wxAND_INVERT:  // (NOT src) AND dst
            m_qtPainter->setCompositionMode( QPainter::RasterOp_NotSourceAndDestination );
            break;
        case wxNO_OP:       // dst
            m_qtPainter->setCompositionMode( QPainter::CompositionMode_DestinationOver );
            break;
        case wxNOR:         // (NOT src) AND (NOT dst)
            m_qtPainter->setCompositionMode( QPainter::RasterOp_NotSourceAndNotDestination );
            break;
        case wxEQUIV:       // (NOT src) XOR dst
            m_qtPainter->setCompositionMode( QPainter::RasterOp_NotSourceXorDestination );
            break;
        case wxSRC_INVERT:  // (NOT src)
            m_qtPainter->setCompositionMode( QPainter::RasterOp_NotSource );
            break;
        case wxOR_INVERT:   // (NOT src) OR dst
            m_qtPainter->setCompositionMode( QPainter::RasterOp_SourceOrDestination );
            rasterColourOp = wxQtINVERT;
            break;
        case wxNAND:        // (NOT src) OR (NOT dst)
            m_qtPainter->setCompositionMode( QPainter::RasterOp_NotSourceOrNotDestination );
            break;
        case wxOR:          // src OR dst
            m_qtPainter->setCompositionMode( QPainter::RasterOp_SourceOrDestination );
            break;
        case wxSET:          // 1
            m_qtPainter->setCompositionMode( QPainter::CompositionMode_SourceOver );
            rasterColourOp = wxQtWHITE;
            break;
    }

    if ( rasterColourOp != m_rasterColourOp )
    {
        // Source colour mode changed
        m_rasterColourOp = rasterColourOp;

        // Restore original colours and apply new mode
        SetPenColour( m_qtPainter, *m_qtPenColor );
        SetBrushColour( m_qtPainter, *m_qtPenColor );

        ApplyRasterColourOp();
    }
}

void wxQtDCImpl::ApplyRasterColourOp()
{
    // Save colours
    *m_qtPenColor = m_qtPainter->pen().color();
    *m_qtBrushColor = m_qtPainter->brush().color();

    // Apply op
    switch ( m_rasterColourOp )
    {
        case wxQtWHITE:
            SetPenColour( m_qtPainter, QColor( Qt::white ) );
            SetBrushColour( m_qtPainter, QColor( Qt::white ) );
            break;
        case wxQtBLACK:
            SetPenColour( m_qtPainter, QColor( Qt::black ) );
            SetBrushColour( m_qtPainter, QColor( Qt::black ) );
            break;
        case wxQtINVERT:
            SetPenColour( m_qtPainter, QColor( ~m_qtPenColor->rgb() ) );
            SetBrushColour( m_qtPainter, QColor( ~m_qtBrushColor->rgb() ) );
            break;
        case wxQtNONE:
            // No op
            break;
    }
}

wxCoord wxQtDCImpl::GetCharHeight() const
{
    QFontMetrics metrics(m_qtPainter->isActive() ?
        m_qtPainter->font() : QApplication::font());
    return wxCoord( metrics.height() );
}

wxCoord wxQtDCImpl::GetCharWidth() const
{
    //FIXME: Returning max width, instead of average
    QFontMetrics metrics(m_qtPainter->isActive() ?
        m_qtPainter->font() : QApplication::font());
    return wxCoord( metrics.maxWidth() );
}

void wxQtDCImpl::DoGetTextExtent(const wxString& string,
                             wxCoord *x, wxCoord *y,
                             wxCoord *descent,
                             wxCoord *externalLeading,
                             const wxFont *theFont ) const
{
    if ( x )
        *x = 0;
    if ( y )
        *y = 0;
    if ( descent )
        *descent = 0;
    if ( externalLeading )
        *externalLeading = 0;

    // We can skip computing the string width and height if it is empty, but
    // not its descent and/or external leading, which still needs to be
    // returned even for an empty string.
    if ( string.empty() && !descent && !externalLeading )
        return;

    QFont f;
    if (theFont != nullptr)
        f = theFont->GetHandle();
    else
        f = m_font.GetHandle();

    QFontMetrics metrics(f);
    if (x != nullptr || y != nullptr)
    {
        // note that boundingRect doesn't return "advance width" for spaces
        if (x != nullptr)
            *x = wxQtGetWidthFromMetrics(metrics, wxQtConvertString(string));
        if (y != nullptr)
            *y = metrics.height();
    }

    if (descent != nullptr)
        *descent = metrics.descent();

    if (externalLeading != nullptr)
        *externalLeading = metrics.leading();
}

void wxQtDCImpl::Clear()
{
    int width, height;
    DoGetSize(&width, &height);

    m_qtPainter->eraseRect( DeviceToLogicalX(0),
                            DeviceToLogicalY(0),
                            DeviceToLogicalXRel(width),
                            DeviceToLogicalYRel(height) );
}

void wxQtDCImpl::UpdateClipBox()
{
    if ( !m_qtPainter->isActive() )
        return;

    if ( m_clippingRegion.IsEmpty() )
    {
        int dcwidth, dcheight;
        DoGetSize(&dcwidth, &dcheight);

        m_qtPainter->setClipRect(DeviceToLogicalX(0),
                                 DeviceToLogicalY(0),
                                 DeviceToLogicalXRel(dcwidth),
                                 DeviceToLogicalYRel(dcheight),
                                 m_clipping ? Qt::IntersectClip : Qt::ReplaceClip);
    }

    /* Note: Qt states that QPainter::clipRegion() may be slow, so we
     *       keep the region manually, which should be faster. A comment in
     *       QPainter::clipBoundingRect() source says: This is not 100% precise,
     *       but it fits within the guarantee and it is reasonably fast.
     */
    m_clippingRegion.QtSetRegion(
        QRegion(m_qtPainter->clipBoundingRect().toRect()) );

    wxRect clipRect = m_clippingRegion.GetBox();

    m_clipX1 = clipRect.GetLeft();
    m_clipX2 = clipRect.GetRight() + 1;
    m_clipY1 = clipRect.GetTop();
    m_clipY2 = clipRect.GetBottom() + 1;

    m_isClipBoxValid = true;
}

bool wxQtDCImpl::DoGetClippingRect(wxRect& rect) const
{
    // Check if we should try to retrieve the clipping region possibly not set
    // by our SetClippingRegion() but preset or modified by application: this
    // can happen when wxDC logical coordinates are transformed with
    // SetDeviceOrigin(), SetLogicalOrigin(), SetUserScale(), SetLogicalScale().
    if ( !m_isClipBoxValid )
    {
        wxQtDCImpl *self = wxConstCast(this, wxQtDCImpl);
        self->UpdateClipBox();
    }

    return wxDCImpl::DoGetClippingRect(rect);
}

void wxQtDCImpl::DoSetClippingRegion(wxCoord x, wxCoord y,
                                 wxCoord width, wxCoord height)
{
    if ( width < 0 )
    {
        width = -width;
        x -= width - 1;
    }
    if ( height < 0 )
    {
        height = -height;
        y -= height - 1;
    }

    if ( m_qtPainter->isActive() )
    {
        // Set QPainter clipping (intersection if not the first one)
        m_qtPainter->setClipRect( x, y, width, height,
                                  m_clipping ? Qt::IntersectClip : Qt::ReplaceClip );

        m_clipping = true;
    }

    UpdateClipBox();
}

void wxQtDCImpl::DoSetDeviceClippingRegion(const wxRegion& region)
{
    if ( m_qtPainter->isActive() )
    {
        // Disable the matrix transformations to match device coordinates
        m_qtPainter->setWorldMatrixEnabled(false);
        // Enable clipping explicitly as QPainter::setClipRegion() doesn't
        // do that for us
        m_qtPainter->setClipping( true );
        // Set QPainter clipping (intersection if not the first one)
        m_qtPainter->setClipRegion( region.GetHandle(),
                                    m_clipping ? Qt::IntersectClip : Qt::ReplaceClip );

        m_qtPainter->setWorldMatrixEnabled(true);

        m_clipping = true;
    }

    UpdateClipBox();
}

void wxQtDCImpl::DestroyClippingRegion()
{
    wxDCImpl::DestroyClippingRegion();
    m_clippingRegion.Clear();

    if (m_qtPainter->isActive())
        m_qtPainter->setClipping( false );

    m_isClipBoxValid = false;
}

bool wxQtDCImpl::DoFloodFill(wxCoord x, wxCoord y, const wxColour& col,
                         wxFloodFillStyle style )
{
#if wxUSE_IMAGE
    extern bool wxDoFloodFill(wxDC *dc, wxCoord x, wxCoord y,
                              const wxColour & col, wxFloodFillStyle style);

    return wxDoFloodFill( GetOwner(), x, y, col, style);
#else
    wxUnusedVar(x);
    wxUnusedVar(y);
    wxUnusedVar(col);
    wxUnusedVar(style);

    return false;
#endif
}

bool wxQtDCImpl::DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const
{
    wxCHECK_MSG( m_qtPainter->isActive(), false, "Invalid wxDC" );

    if ( col )
    {
        wxCHECK_MSG( m_qtPixmap != nullptr, false, "This DC doesn't support GetPixel()" );
        QPixmap pixmap1px = m_qtPixmap->copy( x, y, 1, 1 );
        QImage image = pixmap1px.toImage();
        QColor pixel = image.pixel( 0, 0 );
        col->Set( pixel.red(), pixel.green(), pixel.blue(), pixel.alpha() );

        return true;
    }
    else
    {
        return false;
    }
}

void wxQtDCImpl::DoDrawPoint(wxCoord x, wxCoord y)
{
    QtDCOffsetHelper helper( m_qtPainter );

    m_qtPainter->drawPoint(x, y);
}

void wxQtDCImpl::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{
    QtDCOffsetHelper helper( m_qtPainter );

    m_qtPainter->drawLine(x1, y1, x2, y2);
}


void wxQtDCImpl::DoDrawArc(wxCoord x1, wxCoord y1,
                       wxCoord x2, wxCoord y2,
                       wxCoord xc, wxCoord yc)
{
    // Calculate the rectangle that contains the circle
    QLineF l1( xc, yc, x1, y1 );
    QLineF l2( xc, yc, x2, y2 );
    QPointF center( xc, yc );

    qreal lenRadius = l1.length();
    QPointF centerToCorner( lenRadius, lenRadius );

    QRect rectangle = QRectF( center - centerToCorner, center + centerToCorner ).toRect();

    // Calculate the angles
    int startAngle = (int)(l1.angle() * 16);
    int endAngle = (int)(l2.angle() * 16);

    while(endAngle < startAngle)
        endAngle += 360 * 16;

    int spanAngle = endAngle - startAngle;

    QtDCOffsetHelper helper( m_qtPainter );

    if ( spanAngle == 0 )
        m_qtPainter->drawEllipse( rectangle );
    else if (m_qtPainter->brush().style() != Qt::NoBrush)
        m_qtPainter->drawPie( rectangle, startAngle, spanAngle );
    else
        m_qtPainter->drawArc( rectangle, startAngle, spanAngle );
}

void wxQtDCImpl::DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                               double sa, double ea)
{
    int startAngle = (int)( sa * 16 );
    int endAngle = (int)( ea * 16 );

    while(endAngle < startAngle)
        endAngle += 360 * 16;

    int spanAngle = endAngle - startAngle;

    QRect rectangle(x, y, w, h);

    QtDCOffsetHelper helper( m_qtPainter );

    if (spanAngle == 0)
    {
        m_qtPainter->drawEllipse( rectangle );
    }
    else
    {
        if (m_qtPainter->brush().style() != Qt::NoBrush)
        {
            QPen savedPen = m_qtPainter->pen();
            m_qtPainter->setPen( QPen( Qt::NoPen ) );
            m_qtPainter->drawPie( rectangle, startAngle, spanAngle );
            m_qtPainter->setPen( savedPen );
        }

        m_qtPainter->drawArc( rectangle, startAngle, spanAngle );
    }
}

void wxQtDCImpl::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    if (m_qtPainter->pen().style() != Qt::NoPen)
    {
        width -= 1;
        height -= 1;
    }

    QtDCOffsetHelper helper( m_qtPainter );

    m_qtPainter->drawRect( x, y, width, height );
}

void wxQtDCImpl::DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                    wxCoord width, wxCoord height,
                                    double radius)
{
    if (m_qtPainter->pen().style() != Qt::NoPen)
    {
        width -= 1;
        height -= 1;
    }

    QtDCOffsetHelper helper( m_qtPainter );

    m_qtPainter->drawRoundedRect( x, y, width, height, radius, radius );
}

void wxQtDCImpl::DoDrawEllipse(wxCoord x, wxCoord y,
                           wxCoord width, wxCoord height)
{
    QtDCOffsetHelper helper( m_qtPainter );

    m_qtPainter->drawEllipse( x, y, width, height );
}

void wxQtDCImpl::DoCrossHair(wxCoord x, wxCoord y)
{
    int w, h;
    DoGetSize( &w, &h );

    // Map width and height back (inverted transform)
    QTransform inv = m_qtPainter->transform().inverted();
    int left, top, right, bottom;
    inv.map( w, h, &right, &bottom );
    inv.map( 0, 0, &left, &top );

    QtDCOffsetHelper helper( m_qtPainter );

    m_qtPainter->drawLine( left, y, right, y );
    m_qtPainter->drawLine( x, top, x, bottom );
}

void wxQtDCImpl::DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y)
{
    DoDrawBitmap( icon, x, y, true );
}

void wxQtDCImpl::DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y,
                          bool useMask )
{
    QPixmap pix = *bmp.GetHandle();
    if (pix.depth() == 1) {
        //Monochrome bitmap, draw using text fore/background

        //Save pen/brush
        QBrush savedBrush = m_qtPainter->background();
        QPen savedPen = m_qtPainter->pen();

        //Use text colors
        m_qtPainter->setBackground(QBrush(m_textBackgroundColour.GetQColor()));
        m_qtPainter->setPen(QPen(m_textForegroundColour.GetQColor()));

        //Draw
        m_qtPainter->drawPixmap(x, y, pix);

        //Restore saved settings
        m_qtPainter->setBackground(savedBrush);
        m_qtPainter->setPen(savedPen);
    }
    else
    {
            if ( useMask && bmp.GetMask() && bmp.GetMask()->GetHandle() )
                pix.setMask(*bmp.GetMask()->GetHandle());
            m_qtPainter->drawPixmap(x, y, pix);
    }
}

void wxQtDCImpl::DoDrawText(const wxString& text, wxCoord x, wxCoord y)
{
    QPen savedPen = m_qtPainter->pen();
    m_qtPainter->setPen(QPen(m_textForegroundColour.GetQColor()));

    // Disable logical function
    QPainter::CompositionMode savedOp = m_qtPainter->compositionMode();
    m_qtPainter->setCompositionMode( QPainter::CompositionMode_SourceOver );

    if (m_backgroundMode == wxBRUSHSTYLE_SOLID)
    {
        m_qtPainter->setBackgroundMode(Qt::OpaqueMode);

        //Save pen/brush
        QBrush savedBrush = m_qtPainter->background();

        //Use text colors
        m_qtPainter->setBackground(QBrush(m_textBackgroundColour.GetQColor()));

        //Draw
        m_qtPainter->drawText(x, y, 1, 1, Qt::TextDontClip, wxQtConvertString(text));

        //Restore saved settings
        m_qtPainter->setBackground(savedBrush);


        m_qtPainter->setBackgroundMode(Qt::TransparentMode);
    }
    else
        m_qtPainter->drawText(x, y, 1, 1, Qt::TextDontClip, wxQtConvertString(text));

    m_qtPainter->setPen(savedPen);
    m_qtPainter->setCompositionMode( savedOp );
}

void wxQtDCImpl::DoDrawRotatedText(const wxString& text,
                               wxCoord x, wxCoord y, double angle)
{
    if (m_backgroundMode == wxBRUSHSTYLE_SOLID)
        m_qtPainter->setBackgroundMode(Qt::OpaqueMode);

    //Move and rotate (reverse angle direction in Qt and wx)
    m_qtPainter->translate(x, y);
    m_qtPainter->rotate(-angle);

    QPen savedPen = m_qtPainter->pen();
    m_qtPainter->setPen(QPen(m_textForegroundColour.GetQColor()));

    // Disable logical function
    QPainter::CompositionMode savedOp = m_qtPainter->compositionMode();
    m_qtPainter->setCompositionMode( QPainter::CompositionMode_SourceOver );

    if (m_backgroundMode == wxBRUSHSTYLE_SOLID)
    {
        m_qtPainter->setBackgroundMode(Qt::OpaqueMode);

        //Save pen/brush
        QBrush savedBrush = m_qtPainter->background();

        //Use text colors
        m_qtPainter->setBackground(QBrush(m_textBackgroundColour.GetQColor()));

        //Draw
        m_qtPainter->drawText(0, 0, 1, 1, Qt::TextDontClip, wxQtConvertString(text));

        //Restore saved settings
        m_qtPainter->setBackground(savedBrush);

        m_qtPainter->setBackgroundMode(Qt::TransparentMode);
    }
    else
        m_qtPainter->drawText(0, 0, 1, 1, Qt::TextDontClip, wxQtConvertString(text));

    //Reset to default
    ComputeScaleAndOrigin();
    m_qtPainter->setPen(savedPen);
    m_qtPainter->setCompositionMode( savedOp );
}

bool wxQtDCImpl::DoBlit(wxCoord xdest, wxCoord ydest,
                    wxCoord width, wxCoord height,
                    wxDC *source,
                    wxCoord xsrc, wxCoord ysrc,
                    wxRasterOperationMode rop,
                    bool useMask,
                    wxCoord WXUNUSED(xsrcMask),
                    wxCoord WXUNUSED(ysrcMask) )
{
    wxQtDCImpl *implSource = (wxQtDCImpl*)source->GetImpl();

    QPixmap *qtSource = implSource->GetQPixmap();

    // Not a CHECK on purpose
    if ( !qtSource )
        return false;

    // Change logical function
    wxRasterOperationMode savedMode = GetLogicalFunction();
    SetLogicalFunction( rop );

    if ( useMask )
    {
        m_qtPainter->drawPixmap( QRect( xdest, ydest, width, height ),
                                *qtSource,
                                QRect( xsrc, ysrc, width, height ) );
    }
    else
    {
        QImage qtSourceConverted = qtSource->toImage();
        qtSourceConverted = qtSourceConverted.convertToFormat(QImage::Format_RGB32);

        m_qtPainter->drawImage( QRect( xdest, ydest, width, height ),
                                qtSourceConverted,
                                QRect( xsrc, ysrc, width, height ) );
    }

    SetLogicalFunction( savedMode );

    return true;
}

void wxQtDCImpl::DoDrawLines(int n, const wxPoint points[],
                         wxCoord xoffset, wxCoord yoffset )
{
    if (n > 0)
    {
        QPainterPath path(wxQtConvertPoint(points[0]));
        for (int i = 1; i < n; i++)
        {
            path.lineTo(wxQtConvertPoint(points[i]));
        }

        {
            QtDCOffsetHelper helper(m_qtPainter);

            m_qtPainter->translate(xoffset, yoffset);

            QBrush savebrush = m_qtPainter->brush();
            m_qtPainter->setBrush(Qt::NoBrush);
            m_qtPainter->drawPath(path);
            m_qtPainter->setBrush(savebrush);
        }

        // Reset transform
        ComputeScaleAndOrigin();
    }
}

void wxQtDCImpl::DoDrawPolygon(int n, const wxPoint points[],
                       wxCoord xoffset, wxCoord yoffset,
                       wxPolygonFillMode fillStyle )
{
    Qt::FillRule fill = (fillStyle == wxWINDING_RULE) ? Qt::WindingFill : Qt::OddEvenFill;

    QPolygon qtPoints;
    for (int i = 0; i < n; i++) {
        qtPoints << wxQtConvertPoint(points[i]);
    }

    {
        QtDCOffsetHelper helper(m_qtPainter);

        m_qtPainter->translate(xoffset, yoffset);
        m_qtPainter->drawPolygon(qtPoints, fill);
    }

    // Reset transform
    ComputeScaleAndOrigin();
}

void wxQtDCImpl::DoDrawPolyPolygon(int n,
                                   const int count[],
                                   const wxPoint points[],
                                   wxCoord xoffset,
                                   wxCoord yoffset,
                                   wxPolygonFillMode fillStyle)
{
    if ( n == 1 )
    {
        DoDrawPolygon(count[0], points, xoffset, yoffset, fillStyle);
        return;
    }

    QPainterPath path;

    int i = 0;
    for ( int j = 0; j < n; ++j )
    {
        wxPoint start = points[i];
        path.moveTo(start.x + xoffset, start.y + yoffset);

        ++i;
        int l = count[j];
        for ( int k = 1; k < l; ++k )
        {
            path.lineTo(points[i].x + xoffset, points[i].y + yoffset);
            ++i;
        }
        // close the polygon
        if ( start != points[i-1] )
            path.lineTo(start.x + xoffset, start.y + yoffset);
    }

    QtDCOffsetHelper helper(m_qtPainter);

    m_qtPainter->fillPath(path, m_qtPainter->brush());
    m_qtPainter->strokePath(path, m_qtPainter->pen());

    // Reset transform
    ComputeScaleAndOrigin();
}

void wxQtDCImpl::ComputeScaleAndOrigin()
{
    QTransform t;

    // First apply device origin
    t.translate( m_deviceOriginX + m_deviceLocalOriginX,
                 m_deviceOriginY + m_deviceLocalOriginY );

    // Second, scale
    m_scaleX = m_logicalScaleX * m_userScaleX;
    m_scaleY = m_logicalScaleY * m_userScaleY;
    t.scale( m_scaleX * m_signX, m_scaleY * m_signY );

    // Finally, logical origin
    t.translate( -m_logicalOriginX, -m_logicalOriginY );

    // Apply transform to QPainter, overwriting the previous one
    m_qtPainter->setWorldTransform(t, false);

    m_isClipBoxValid = false;
}
