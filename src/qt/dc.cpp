/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dc.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dc.h"
#include "wx/qt/dc.h"

wxQtDCImpl::wxQtDCImpl( wxDC *owner )
    : wxDCImpl( owner )
{
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
}

void wxQtDCImpl::DoGetSizeMM(int* width, int* height) const
{
}

int wxQtDCImpl::GetDepth() const
{
    return 0;
}

wxSize wxQtDCImpl::GetPPI() const
{
    return wxSize();
}

void wxQtDCImpl::SetFont(const wxFont& font)
{
}

void wxQtDCImpl::SetPen(const wxPen& pen)
{
}

void wxQtDCImpl::SetBrush(const wxBrush& brush)
{
}

void wxQtDCImpl::SetBackground(const wxBrush& brush)
{
}

void wxQtDCImpl::SetBackgroundMode(int mode)
{
}


#if wxUSE_PALETTE
void wxQtDCImpl::SetPalette(const wxPalette& palette)
{
}
#endif // wxUSE_PALETTE

void wxQtDCImpl::SetLogicalFunction(wxRasterOperationMode function)
{
}


wxCoord wxQtDCImpl::GetCharHeight() const
{
    return wxCoord();
}

wxCoord wxQtDCImpl::GetCharWidth() const
{
    return wxCoord();
}

void wxQtDCImpl::DoGetTextExtent(const wxString& string,
                             wxCoord *x, wxCoord *y,
                             wxCoord *descent,
                             wxCoord *externalLeading,
                             const wxFont *theFont ) const
{
}

void wxQtDCImpl::Clear()
{
}

void wxQtDCImpl::DoSetClippingRegion(wxCoord x, wxCoord y,
                                 wxCoord width, wxCoord height)
{
}

void wxQtDCImpl::DoSetDeviceClippingRegion(const wxRegion& region)
{
}

bool wxQtDCImpl::DoFloodFill(wxCoord x, wxCoord y, const wxColour& col,
                         wxFloodFillStyle style )
{
    return false;
}

bool wxQtDCImpl::DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const
{
    return false;
}

void wxQtDCImpl::DoDrawPoint(wxCoord x, wxCoord y)
{
}

void wxQtDCImpl::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{
}


void wxQtDCImpl::DoDrawArc(wxCoord x1, wxCoord y1,
                       wxCoord x2, wxCoord y2,
                       wxCoord xc, wxCoord yc)
{
}

void wxQtDCImpl::DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                               double sa, double ea)
{
}

void wxQtDCImpl::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
}

void wxQtDCImpl::DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                    wxCoord width, wxCoord height,
                                    double radius)
{
}

void wxQtDCImpl::DoDrawEllipse(wxCoord x, wxCoord y,
                           wxCoord width, wxCoord height)
{
}

void wxQtDCImpl::DoCrossHair(wxCoord x, wxCoord y)
{
}

void wxQtDCImpl::DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y)
{
}

void wxQtDCImpl::DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y,
                          bool useMask )
{
}

void wxQtDCImpl::DoDrawText(const wxString& text, wxCoord x, wxCoord y)
{
}

void wxQtDCImpl::DoDrawRotatedText(const wxString& text,
                               wxCoord x, wxCoord y, double angle)
{
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
    return false;
}

void wxQtDCImpl::DoDrawLines(int n, wxPoint points[],
                         wxCoord xoffset, wxCoord yoffset )
{
}

void wxQtDCImpl::DoDrawPolygon(int n, wxPoint points[],
                       wxCoord xoffset, wxCoord yoffset,
                       wxPolygonFillMode fillStyle )
{
}


