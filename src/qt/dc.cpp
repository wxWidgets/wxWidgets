/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dc.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most, Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dc.h"
#include "wx/qt/dc.h"
#include "wx/qt/converter.h"
#include "wx/qt/utils.h"

wxQtDCImpl::wxQtDCImpl( wxDC *owner )
    : wxDCImpl( owner )
{
}

void wxQtDCImpl::PrepareQPainter()
{
    //Do here all QPainter initialization (called after each begin())
}

bool wxQtDCImpl::CanDrawBitmap() const
{
    return false;
}

bool wxQtDCImpl::CanGetTextExtent() const
{
    return false;
}

void wxQtDCImpl::DoGetSize(int *width, int *height) const
{
    *width  = m_qtPainter.device()->width();
    *height = m_qtPainter.device()->height();
}

void wxQtDCImpl::DoGetSizeMM(int* width, int* height) const
{
    *width  = m_qtPainter.device()->widthMM();
    *height = m_qtPainter.device()->heightMM();
}

int wxQtDCImpl::GetDepth() const
{
    return m_qtPainter.device()->depth();
}

wxSize wxQtDCImpl::GetPPI() const
{
    return wxSize(m_qtPainter.device()->logicalDpiX(), m_qtPainter.device()->logicalDpiY());
}

void wxQtDCImpl::SetFont(const wxFont& font)
{
    wxMISSING_IMPLEMENTATION(__FUNCTION__);
}

void wxQtDCImpl::SetPen(const wxPen& pen)
{
    m_qtPainter.setPen(pen.GetHandle());
}

void wxQtDCImpl::SetBrush(const wxBrush& brush)
{
    m_qtPainter.setBrush(brush.GetHandle());
}

void wxQtDCImpl::SetBackground(const wxBrush& brush)
{
    m_qtPainter.setBackground(brush.GetHandle());
}

void wxQtDCImpl::SetBackgroundMode(int mode)
{
    switch (mode) {
        case wxSOLID:
            m_qtPainter.setBackgroundMode(Qt::OpaqueMode);
            break;
        case wxTRANSPARENT:
            m_qtPainter.setBackgroundMode(Qt::TransparentMode);
            break;
        default:
            wxFAIL_MSG( "Unknown wxDC background mode" );
    }
}


#if wxUSE_PALETTE
void wxQtDCImpl::SetPalette(const wxPalette& palette)
{
    wxMISSING_IMPLEMENTATION(__FUNCTION__);
}
#endif // wxUSE_PALETTE

void wxQtDCImpl::SetLogicalFunction(wxRasterOperationMode function)
{
    wxMISSING_IMPLEMENTATION(__FUNCTION__);
}


wxCoord wxQtDCImpl::GetCharHeight() const
{
    wxMISSING_IMPLEMENTATION(__FUNCTION__);
    return wxCoord();
}

wxCoord wxQtDCImpl::GetCharWidth() const
{
    wxMISSING_IMPLEMENTATION(__FUNCTION__);
    return wxCoord();
}

void wxQtDCImpl::DoGetTextExtent(const wxString& string,
                             wxCoord *x, wxCoord *y,
                             wxCoord *descent,
                             wxCoord *externalLeading,
                             const wxFont *theFont ) const
{
    wxMISSING_IMPLEMENTATION(__FUNCTION__);
}

void wxQtDCImpl::Clear()
{
    int width, height;
    DoGetSize(&width, &height);
    
    m_qtPainter.eraseRect(QRect(0, 0, width, height));
}

void wxQtDCImpl::DoSetClippingRegion(wxCoord x, wxCoord y,
                                 wxCoord width, wxCoord height)
{
    wxMISSING_IMPLEMENTATION(__FUNCTION__);
}

void wxQtDCImpl::DoSetDeviceClippingRegion(const wxRegion& region)
{
    wxMISSING_IMPLEMENTATION(__FUNCTION__);
}

bool wxQtDCImpl::DoFloodFill(wxCoord x, wxCoord y, const wxColour& col,
                         wxFloodFillStyle style )
{
    m_qtPainter.device()->logicalDpiX();
    return false;
}

bool wxQtDCImpl::DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const
{
    m_qtPainter.device()->logicalDpiX();
    return false;
}

void wxQtDCImpl::DoDrawPoint(wxCoord x, wxCoord y)
{
    m_qtPainter.drawPoint(x, y);
}

void wxQtDCImpl::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{
    m_qtPainter.drawLine(x1, y1, x2, y2);
}


void wxQtDCImpl::DoDrawArc(wxCoord x1, wxCoord y1,
                       wxCoord x2, wxCoord y2,
                       wxCoord xc, wxCoord yc)
{
    wxMISSING_IMPLEMENTATION(__FUNCTION__);
}

void wxQtDCImpl::DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                               double sa, double ea)
{
    m_qtPainter.device()->logicalDpiX();
}

void wxQtDCImpl::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    m_qtPainter.drawRect(x, y, width, height);
}

void wxQtDCImpl::DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                    wxCoord width, wxCoord height,
                                    double radius)
{
    m_qtPainter.drawRoundedRect(x, y, width, height, radius, radius);
}

void wxQtDCImpl::DoDrawEllipse(wxCoord x, wxCoord y,
                           wxCoord width, wxCoord height)
{
    m_qtPainter.drawEllipse(x, y, width, height);
}

void wxQtDCImpl::DoCrossHair(wxCoord x, wxCoord y)
{
    wxMISSING_IMPLEMENTATION(__FUNCTION__);
}

void wxQtDCImpl::DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y)
{
    wxMISSING_IMPLEMENTATION(__FUNCTION__);
}

void wxQtDCImpl::DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y,
                          bool useMask )
{
    //TODO: Don't use mask if useMask is false

    QPixmap pix = *bmp.GetHandle();
    if (pix.depth() == 1) {
        //Monochrome bitmap, draw using text fore/background
        
        //Save pen/brush
        QBrush savedBrush = m_qtPainter.background();
        QPen savedPen = m_qtPainter.pen();
        Qt::BGMode savedMode = m_qtPainter.backgroundMode();

        
        
        //Use text colors
        m_qtPainter.setBackground(QBrush(m_textBackgroundColour.GetHandle()));
        m_qtPainter.setPen(QPen(m_textForegroundColour.GetHandle()));
        m_qtPainter.setBackgroundMode(Qt::OpaqueMode);
        
        //Draw
        m_qtPainter.drawPixmap(x, y, pix);
        
        //Restore saved settings
        m_qtPainter.setBackground(savedBrush);
        m_qtPainter.setPen(savedPen);
        m_qtPainter.setBackgroundMode(savedMode);
    } else {
        m_qtPainter.drawPixmap(x, y, pix);
    }
}

void wxQtDCImpl::DoDrawText(const wxString& text, wxCoord x, wxCoord y)
{
    //Save pen/brush
    QBrush savedBrush = m_qtPainter.background();
    QPen savedPen = m_qtPainter.pen();
    
    //Use text colors
    m_qtPainter.setBackground(QBrush(m_textBackgroundColour.GetHandle()));
    m_qtPainter.setPen(QPen(m_textForegroundColour.GetHandle()));

    //Draw
    m_qtPainter.drawText(x, y, 1, 1, Qt::TextDontClip, wxQtConvertString(text));

    //Restore saved settings
    m_qtPainter.setBackground(savedBrush);
    m_qtPainter.setPen(savedPen);
}

void wxQtDCImpl::DoDrawRotatedText(const wxString& text,
                               wxCoord x, wxCoord y, double angle)
{
    //Move and rotate
    m_qtPainter.translate(x, y);
    m_qtPainter.rotate(angle);

    //Save pen/brush
    QBrush savedBrush = m_qtPainter.background();
    QPen savedPen = m_qtPainter.pen();
    
    //Use text colors
    m_qtPainter.setBackground(QBrush(m_textBackgroundColour.GetHandle()));
    m_qtPainter.setPen(QPen(m_textForegroundColour.GetHandle()));
    
    //Draw
    m_qtPainter.drawText(0, 0, wxQtConvertString(text));
    
    //Restore saved settings
    m_qtPainter.setBackground(savedBrush);
    m_qtPainter.setPen(savedPen);

    //Reset to default
    m_qtPainter.resetTransform();
}

bool wxQtDCImpl::DoBlit(wxCoord xdest, wxCoord ydest,
                    wxCoord width, wxCoord height,
                    wxDC *source,
                    wxCoord xsrc, wxCoord ysrc,
                    wxRasterOperationMode rop,
                    bool useMask,
                    wxCoord xsrcMask,
                    wxCoord ysrcMask )
{
    wxMISSING_IMPLEMENTATION(__FUNCTION__);
    return false;
}

void wxQtDCImpl::DoDrawLines(int n, wxPoint points[],
                         wxCoord xoffset, wxCoord yoffset )
{
    QVector<QPoint> qtPoints;
    for (int i = 0; i < n; i++) {
        qtPoints << wxQtConvertPoint(points[i]);
    }

    m_qtPainter.translate(xoffset, yoffset);
    m_qtPainter.drawLines(qtPoints);
    m_qtPainter.resetTransform();
}

void wxQtDCImpl::DoDrawPolygon(int n, wxPoint points[],
                       wxCoord xoffset, wxCoord yoffset,
                       wxPolygonFillMode fillStyle )
{
    QPolygon qtPoints;
    for (int i = 0; i < n; i++) {
        qtPoints << wxQtConvertPoint(points[i]);
    }

    Qt::FillRule fill = (fillStyle == wxWINDING_RULE) ? Qt::WindingFill : Qt::OddEvenFill;
    
    m_qtPainter.translate(xoffset, yoffset);
    m_qtPainter.drawPolygon(qtPoints, fill);
    m_qtPainter.resetTransform();
}


