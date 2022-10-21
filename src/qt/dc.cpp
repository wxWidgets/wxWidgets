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

        if (m_clipping)
        {
            wxRegionIterator ri(m_clippingRegion);
            bool append = false;
            while (ri.HaveRects())
            {
                wxRect r = ri.GetRect();
                m_qtPainter->setClipRect( r.x, r.y, r.width, r.height,
                                          append ? Qt::IntersectClip : Qt::ReplaceClip );
                append = true;
                ++ri;
            }
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

    m_qtPainter->setPen(pen.GetHandle());

    ApplyRasterColourOp();
}

void wxQtDCImpl::SetBrush(const wxBrush& brush)
{
    m_brush = brush;

    if (brush.GetStyle() == wxBRUSHSTYLE_STIPPLE_MASK_OPAQUE)
    {
        // Use a monochrome mask: use foreground color for the mask
        QBrush b(brush.GetHandle());
        b.setColor(m_textForegroundColour.GetQColor());
        b.setTexture(b.texture().mask());
        m_qtPainter->setBrush(b);
    }
    else if (brush.GetStyle() == wxBRUSHSTYLE_STIPPLE)
    {
        //Don't use the mask
        QBrush b(brush.GetHandle());

        QPixmap p = b.texture();
        p.setMask(QBitmap());
        b.setTexture(p);

        m_qtPainter->setBrush(b);
    }
    else
    {
        m_qtPainter->setBrush(brush.GetHandle());
    }

    ApplyRasterColourOp();
}

void wxQtDCImpl::SetBackground(const wxBrush& brush)
{
    m_backgroundBrush = brush;

    if (m_qtPainter->isActive())
        m_qtPainter->setBackground(brush.GetHandle());
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
    wxMISSING_IMPLEMENTATION(__FUNCTION__);
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
    QFontMetrics metrics(m_qtPainter->font());
    return wxCoord( metrics.height() );
}

wxCoord wxQtDCImpl::GetCharWidth() const
{
    //FIXME: Returning max width, instead of average
    QFontMetrics metrics(m_qtPainter->font());
    return wxCoord( metrics.maxWidth() );
}

void wxQtDCImpl::DoGetTextExtent(const wxString& string,
                             wxCoord *x, wxCoord *y,
                             wxCoord *descent,
                             wxCoord *externalLeading,
                             const wxFont *theFont ) const
{
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

    m_qtPainter->eraseRect(QRect(0, 0, width, height));
}

void wxQtDCImpl::DoSetClippingRegion(wxCoord x, wxCoord y,
                                 wxCoord width, wxCoord height)
{
    // Special case: Empty region -> DestroyClippingRegion()
    if ( width == 0 && height == 0 )
    {
        DestroyClippingRegion();
    }
    else
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

        if (m_qtPainter->isActive())
        {
            // Set QPainter clipping (intersection if not the first one)
            m_qtPainter->setClipRect( x, y, width, height,
                                      m_clipping ? Qt::IntersectClip : Qt::ReplaceClip );
        }

        // Set internal state for getters
        /* Note: Qt states that QPainter::clipRegion() may be slow, so we
         * keep the region manually, which should be faster */
        if ( !m_clipping || m_clippingRegion.IsEmpty() )
        {
            int dcwidth, dcheight;
            DoGetSize(&dcwidth, &dcheight);

            m_clippingRegion = wxRegion(0, 0, dcwidth, dcheight);
        }
        m_clippingRegion.Intersect( wxRect(x, y, width, height) );

        wxRect clipRect = m_clippingRegion.GetBox();

        m_clipX1 = clipRect.GetLeft();
        m_clipX2 = clipRect.GetRight() + 1;
        m_clipY1 = clipRect.GetTop();
        m_clipY2 = clipRect.GetBottom() + 1;
        m_clipping = true;
    }
}

void wxQtDCImpl::DoSetDeviceClippingRegion(const wxRegion& region)
{
    if ( region.IsEmpty() )
    {
        DestroyClippingRegion();
    }
    else
    {
        QRegion qregion = region.GetHandle();
        // Save current origin / scale (logical coordinates)
        QTransform qtrans = m_qtPainter->worldTransform();
        // Reset transofrmation to match device coordinates
        m_qtPainter->setWorldTransform( QTransform() );
        // Set QPainter clipping (intersection if not the first one)
        m_qtPainter->setClipRegion( qregion,
                                 m_clipping ? Qt::IntersectClip : Qt::ReplaceClip );

        // Restore the transformation (translation / scale):
        m_qtPainter->setWorldTransform( qtrans );

        // Set internal state for getters
        /* Note: Qt states that QPainter::clipRegion() may be slow, so we
        * keep the region manually, which should be faster */
        if ( m_clipping )
            m_clippingRegion.Union( region );
        else
            m_clippingRegion.Intersect( region );

        wxRect clipRect = m_clippingRegion.GetBox();

        m_clipX1 = clipRect.GetLeft();
        m_clipX2 = clipRect.GetRight() + 1;
        m_clipY1 = clipRect.GetTop();
        m_clipY2 = clipRect.GetBottom() + 1;
        m_clipping = true;
    }
}

void wxQtDCImpl::DestroyClippingRegion()
{
    wxDCImpl::DestroyClippingRegion();
    m_clippingRegion.Clear();

    if (m_qtPainter->isActive())
        m_qtPainter->setClipping( false );
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
    m_qtPainter->drawPoint(x, y);
}

void wxQtDCImpl::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{
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

    qreal penWidth = m_qtPainter->pen().width();
    qreal lenRadius = l1.length() - penWidth / 2;
    QPointF centerToCorner( lenRadius, lenRadius );

    QRect rectangle = QRectF( center - centerToCorner, center + centerToCorner ).toRect();

    // Calculate the angles
    int startAngle = (int)( l1.angle() * 16 );
    int endAngle = (int)( l2.angle() * 16 );
    int spanAngle = endAngle - startAngle;
    if ( spanAngle < 0 )
    {
        spanAngle = -spanAngle;
    }

    if ( spanAngle == 0 )
        m_qtPainter->drawEllipse( rectangle );
    else
        m_qtPainter->drawPie( rectangle, startAngle, spanAngle );
}

void wxQtDCImpl::DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                               double sa, double ea)
{
    int penWidth = m_qtPainter->pen().width();
    x += penWidth / 2;
    y += penWidth / 2;
    w -= penWidth;
    h -= penWidth;

    double spanAngle = sa - ea;
    if (spanAngle < -180)
        spanAngle += 360;
    if (spanAngle > 180)
        spanAngle -= 360;

    if ( spanAngle == 0 )
        m_qtPainter->drawEllipse( x, y, w, h );
    else
        m_qtPainter->drawPie( x, y, w, h, (int)( sa * 16 ), (int)( ( ea - sa ) * 16 ) );
}

void wxQtDCImpl::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    int penWidth = m_qtPainter->pen().width();
    x += penWidth / 2;
    y += penWidth / 2;
    width -= penWidth;
    height -= penWidth;

    m_qtPainter->drawRect( x, y, width, height );
}

void wxQtDCImpl::DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                    wxCoord width, wxCoord height,
                                    double radius)
{
    int penWidth = m_qtPainter->pen().width();
    x += penWidth / 2;
    y += penWidth / 2;
    width -= penWidth;
    height -= penWidth;

    m_qtPainter->drawRoundedRect( x, y, width, height, radius, radius );
}

void wxQtDCImpl::DoDrawEllipse(wxCoord x, wxCoord y,
                           wxCoord width, wxCoord height)
{
    const int penWidth = m_qtPainter->pen().width();
    x += penWidth / 2;
    y += penWidth / 2;
    width -= penWidth;
    height -= penWidth;

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
        m_qtPainter->drawText(x, y, 1, 1, Qt::TextDontClip, wxQtConvertString(text));

        //Restore saved settings
        m_qtPainter->setBackground(savedBrush);

        m_qtPainter->setBackgroundMode(Qt::TransparentMode);
    }
    else
        m_qtPainter->drawText(x, y, 1, 1, Qt::TextDontClip, wxQtConvertString(text));

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

        m_qtPainter->translate(xoffset, yoffset);

        QBrush savebrush = m_qtPainter->brush();
        m_qtPainter->setBrush(Qt::NoBrush);
        m_qtPainter->drawPath(path);
        m_qtPainter->setBrush(savebrush);

        // Reset transform
        ComputeScaleAndOrigin();
    }
}

void wxQtDCImpl::DoDrawPolygon(int n, const wxPoint points[],
                       wxCoord xoffset, wxCoord yoffset,
                       wxPolygonFillMode fillStyle )
{
    QPolygon qtPoints;
    for (int i = 0; i < n; i++) {
        qtPoints << wxQtConvertPoint(points[i]);
    }

    Qt::FillRule fill = (fillStyle == wxWINDING_RULE) ? Qt::WindingFill : Qt::OddEvenFill;

    m_qtPainter->translate(xoffset, yoffset);
    m_qtPainter->drawPolygon(qtPoints, fill);
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
    t.translate( m_logicalOriginX, m_logicalOriginY );

    // Apply transform to QPainter, overwriting the previous one
    m_qtPainter->setWorldTransform(t, false);
}
