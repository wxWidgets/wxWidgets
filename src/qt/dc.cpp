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

wxQTDCImpl::wxQTDCImpl( wxDC *owner )
    : wxDCImpl( owner )
{
}

bool wxQTDCImpl::CanDrawBitmap() const
{
    return false;
}

bool wxQTDCImpl::CanGetTextExtent() const
{
    return false;
}

void wxQTDCImpl::DoGetSize(int *width, int *height) const
{
}

void wxQTDCImpl::DoGetSizeMM(int* width, int* height) const
{
}

int wxQTDCImpl::GetDepth() const
{
    return 0;
}

wxSize wxQTDCImpl::GetPPI() const
{
    return wxSize();
}

void wxQTDCImpl::SetFont(const wxFont& font)
{
}

void wxQTDCImpl::SetPen(const wxPen& pen)
{
}

void wxQTDCImpl::SetBrush(const wxBrush& brush)
{
}

void wxQTDCImpl::SetBackground(const wxBrush& brush)
{
}

void wxQTDCImpl::SetBackgroundMode(int mode)
{
}


#if wxUSE_PALETTE
void wxQTDCImpl::SetPalette(const wxPalette& palette)
{
}
#endif // wxUSE_PALETTE

void wxQTDCImpl::SetLogicalFunction(wxRasterOperationMode function)
{
}


wxCoord wxQTDCImpl::GetCharHeight() const
{
    return wxCoord();
}

wxCoord wxQTDCImpl::GetCharWidth() const
{
    return wxCoord();
}

void wxQTDCImpl::DoGetTextExtent(const wxString& string,
                             wxCoord *x, wxCoord *y,
                             wxCoord *descent,
                             wxCoord *externalLeading,
                             const wxFont *theFont ) const
{
}

void wxQTDCImpl::Clear()
{
}

void wxQTDCImpl::DoSetClippingRegion(wxCoord x, wxCoord y,
                                 wxCoord width, wxCoord height)
{
}

void wxQTDCImpl::DoSetDeviceClippingRegion(const wxRegion& region)
{
}

bool wxQTDCImpl::DoFloodFill(wxCoord x, wxCoord y, const wxColour& col,
                         wxFloodFillStyle style )
{
    return false;
}

bool wxQTDCImpl::DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const
{
    return false;
}

void wxQTDCImpl::DoDrawPoint(wxCoord x, wxCoord y)
{
}

void wxQTDCImpl::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{
}


void wxQTDCImpl::DoDrawArc(wxCoord x1, wxCoord y1,
                       wxCoord x2, wxCoord y2,
                       wxCoord xc, wxCoord yc)
{
}

void wxQTDCImpl::DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                               double sa, double ea)
{
}

void wxQTDCImpl::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
}

void wxQTDCImpl::DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                    wxCoord width, wxCoord height,
                                    double radius)
{
}

void wxQTDCImpl::DoDrawEllipse(wxCoord x, wxCoord y,
                           wxCoord width, wxCoord height)
{
}

void wxQTDCImpl::DoCrossHair(wxCoord x, wxCoord y)
{
}

void wxQTDCImpl::DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y)
{
}

void wxQTDCImpl::DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y,
                          bool useMask )
{
}

void wxQTDCImpl::DoDrawText(const wxString& text, wxCoord x, wxCoord y)
{
}

void wxQTDCImpl::DoDrawRotatedText(const wxString& text,
                               wxCoord x, wxCoord y, double angle)
{
}

bool wxQTDCImpl::DoBlit(wxCoord xdest, wxCoord ydest,
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

void wxQTDCImpl::DoDrawLines(int n, wxPoint points[],
                         wxCoord xoffset, wxCoord yoffset )
{
}

void wxQTDCImpl::DoDrawPolygon(int n, wxPoint points[],
                       wxCoord xoffset, wxCoord yoffset,
                       wxPolygonFillMode fillStyle )
{
}


