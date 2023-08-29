/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dcprint.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dcprint.h"
#include "wx/qt/dcprint.h"

wxIMPLEMENT_CLASS(wxPrinterDCImpl,wxDCImpl)

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

void wxPrinterDCImpl::DoGetSize(int *WXUNUSED(width), int *WXUNUSED(height)) const
{
}

void wxPrinterDCImpl::DoGetSizeMM(int* WXUNUSED(width), int* WXUNUSED(height)) const
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

void wxPrinterDCImpl::SetFont(const wxFont& WXUNUSED(font))
{
}

void wxPrinterDCImpl::SetPen(const wxPen& WXUNUSED(pen))
{
}

void wxPrinterDCImpl::SetBrush(const wxBrush& WXUNUSED(brush))
{
}

void wxPrinterDCImpl::SetBackground(const wxBrush& WXUNUSED(brush))
{
}

void wxPrinterDCImpl::SetBackgroundMode(int WXUNUSED(mode))
{
}


#if wxUSE_PALETTE
void wxPrinterDCImpl::SetPalette(const wxPalette& WXUNUSED(palette))
{
}
#endif // wxUSE_PALETTE

void wxPrinterDCImpl::SetLogicalFunction(wxRasterOperationMode WXUNUSED(function))
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

void wxPrinterDCImpl::DoGetTextExtent(const wxString& WXUNUSED(string),
                             wxCoord *WXUNUSED(x), wxCoord *WXUNUSED(y),
                             wxCoord *WXUNUSED(descent),
                             wxCoord *WXUNUSED(externalLeading),
                             const wxFont *WXUNUSED(theFont) ) const
{
}

void wxPrinterDCImpl::Clear()
{
}

void wxPrinterDCImpl::DoSetClippingRegion(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y),
                                 wxCoord WXUNUSED(width), wxCoord WXUNUSED(height))
{
}

void wxPrinterDCImpl::DoSetDeviceClippingRegion(const wxRegion& WXUNUSED(region))
{
}

bool wxPrinterDCImpl::DoFloodFill(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), const wxColour& WXUNUSED(col),
                         wxFloodFillStyle WXUNUSED(style) )
{
    return false;
}

bool wxPrinterDCImpl::DoGetPixel(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), wxColour *WXUNUSED(col)) const
{
    return false;
}

void wxPrinterDCImpl::DoDrawPoint(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y))
{
}

void wxPrinterDCImpl::DoDrawLine(wxCoord WXUNUSED(x1), wxCoord WXUNUSED(y1), wxCoord WXUNUSED(x2), wxCoord WXUNUSED(y2))
{
}

void wxPrinterDCImpl::DoDrawArc(wxCoord WXUNUSED(x1), wxCoord WXUNUSED(y1),
                       wxCoord WXUNUSED(x2), wxCoord WXUNUSED(y2),
                       wxCoord WXUNUSED(xc), wxCoord WXUNUSED(yc))
{
}

void wxPrinterDCImpl::DoDrawEllipticArc(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), wxCoord WXUNUSED(w), wxCoord WXUNUSED(h),
                               double WXUNUSED(sa), double WXUNUSED(ea))
{
}

void wxPrinterDCImpl::DoDrawRectangle(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), wxCoord WXUNUSED(width), wxCoord WXUNUSED(height))
{
}

void wxPrinterDCImpl::DoDrawRoundedRectangle(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y),
                                    wxCoord WXUNUSED(width), wxCoord WXUNUSED(height),
                                    double WXUNUSED(radius))
{
}

void wxPrinterDCImpl::DoDrawEllipse(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y),
                           wxCoord WXUNUSED(width), wxCoord WXUNUSED(height))
{
}

void wxPrinterDCImpl::DoCrossHair(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y))
{
}

void wxPrinterDCImpl::DoDrawIcon(const wxIcon& WXUNUSED(icon), wxCoord WXUNUSED(x), wxCoord WXUNUSED(y))
{
}

void wxPrinterDCImpl::DoDrawBitmap(const wxBitmap &WXUNUSED(bmp), wxCoord WXUNUSED(x), wxCoord WXUNUSED(y),
                          bool WXUNUSED(useMask))
{
}

void wxPrinterDCImpl::DoDrawText(const wxString& WXUNUSED(text), wxCoord WXUNUSED(x), wxCoord WXUNUSED(y))
{
}

void wxPrinterDCImpl::DoDrawRotatedText(const wxString& WXUNUSED(text),
                               wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), double WXUNUSED(angle))
{
}

bool wxPrinterDCImpl::DoBlit(wxCoord WXUNUSED(xdest), wxCoord WXUNUSED(ydest),
                    wxCoord WXUNUSED(width), wxCoord WXUNUSED(height),
                    wxDC *WXUNUSED(source),
                    wxCoord WXUNUSED(xsrc), wxCoord WXUNUSED(ysrc),
                    wxRasterOperationMode WXUNUSED(rop),
                    bool WXUNUSED(useMask),
                    wxCoord WXUNUSED(xsrcMask),
                    wxCoord WXUNUSED(ysrcMask))
{
    return false;
}

void wxPrinterDCImpl::DoDrawLines(int WXUNUSED(n), const wxPoint WXUNUSED(points)[],
                         wxCoord WXUNUSED(xoffset), wxCoord WXUNUSED(yoffset) )
{
}

void wxPrinterDCImpl::DoDrawPolygon(int WXUNUSED(n), const wxPoint WXUNUSED(points)[],
                       wxCoord WXUNUSED(xoffset), wxCoord WXUNUSED(yoffset),
                       wxPolygonFillMode WXUNUSED(fillStyle) )
{
}


