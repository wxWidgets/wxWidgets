/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dcprint.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dcprint.h"
#include "wx/qt/dcprint.h"

wxPrinterDCImpl::wxPrinterDCImpl( wxPrinterDC *owner, const wxPrintData & )
    : wxDCImpl( owner )
{
}


bool wxPrinterDCImpl::CanDrawBitmap() const
{
    return false;
}

bool wxPrinterDCImpl::CanGetTextExtent() const
{
    return false;
}

void wxPrinterDCImpl::DoGetSize(int *width, int *height) const
{
}

void wxPrinterDCImpl::DoGetSizeMM(int* width, int* height) const
{
}

int wxPrinterDCImpl::GetDepth() const
{
    return 0;
}

wxSize wxPrinterDCImpl::GetPPI() const
{
    return wxSize();
}

void wxPrinterDCImpl::SetFont(const wxFont& font)
{
}

void wxPrinterDCImpl::SetPen(const wxPen& pen)
{
}

void wxPrinterDCImpl::SetBrush(const wxBrush& brush)
{
}

void wxPrinterDCImpl::SetBackground(const wxBrush& brush)
{
}

void wxPrinterDCImpl::SetBackgroundMode(int mode)
{
}


#if wxUSE_PALETTE
void wxPrinterDCImpl::SetPalette(const wxPalette& palette)
{
}
#endif // wxUSE_PALETTE

void wxPrinterDCImpl::SetLogicalFunction(wxRasterOperationMode function)
{
}

wxCoord wxPrinterDCImpl::GetCharHeight() const
{
    return wxCoord();
}

wxCoord wxPrinterDCImpl::GetCharWidth() const
{
    return wxCoord();
}

void wxPrinterDCImpl::DoGetTextExtent(const wxString& string,
                             wxCoord *x, wxCoord *y,
                             wxCoord *descent,
                             wxCoord *externalLeading,
                             const wxFont *theFont ) const
{
}

void wxPrinterDCImpl::Clear()
{
}

void wxPrinterDCImpl::DoSetClippingRegion(wxCoord x, wxCoord y,
                                 wxCoord width, wxCoord height)
{
}

void wxPrinterDCImpl::DoSetDeviceClippingRegion(const wxRegion& region)
{
}

bool wxPrinterDCImpl::DoFloodFill(wxCoord x, wxCoord y, const wxColour& col,
                         wxFloodFillStyle style )
{
    return false;
}

bool wxPrinterDCImpl::DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const
{
    return false;
}

void wxPrinterDCImpl::DoDrawPoint(wxCoord x, wxCoord y)
{
}

void wxPrinterDCImpl::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{
}

void wxPrinterDCImpl::DoDrawArc(wxCoord x1, wxCoord y1,
                       wxCoord x2, wxCoord y2,
                       wxCoord xc, wxCoord yc)
{
}

void wxPrinterDCImpl::DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                               double sa, double ea)
{
}

void wxPrinterDCImpl::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
}

void wxPrinterDCImpl::DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                    wxCoord width, wxCoord height,
                                    double radius)
{
}

void wxPrinterDCImpl::DoDrawEllipse(wxCoord x, wxCoord y,
                           wxCoord width, wxCoord height)
{
}

void wxPrinterDCImpl::DoCrossHair(wxCoord x, wxCoord y)
{
}

void wxPrinterDCImpl::DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y)
{
}

void wxPrinterDCImpl::DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y,
                          bool useMask)
{
}

void wxPrinterDCImpl::DoDrawText(const wxString& text, wxCoord x, wxCoord y)
{
}

void wxPrinterDCImpl::DoDrawRotatedText(const wxString& text,
                               wxCoord x, wxCoord y, double angle)
{
}

bool wxPrinterDCImpl::DoBlit(wxCoord xdest, wxCoord ydest,
                    wxCoord width, wxCoord height,
                    wxDC *source,
                    wxCoord xsrc, wxCoord ysrc,
                    wxRasterOperationMode rop,
                    bool useMask,
                    wxCoord xsrcMask,
                    wxCoord ysrcMask)
{
    return false;
}

void wxPrinterDCImpl::DoDrawLines(int n, wxPoint points[],
                         wxCoord xoffset, wxCoord yoffset )
{
}

void wxPrinterDCImpl::DoDrawPolygon(int n, wxPoint points[],
                       wxCoord xoffset, wxCoord yoffset,
                       wxPolygonFillMode fillStyle )
{
}


