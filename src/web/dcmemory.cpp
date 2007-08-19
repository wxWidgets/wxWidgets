/////////////////////////////////////////////////////////////////////////////
// Name:        src/web/dcmemory.cpp
// Purpose:     wxMemoryDC class
// Author:      John Wilmes
// Modified by:
// Created:     08/16/07
// RCS-ID:      $Id: dcclient.cpp 45851 2007-05-05 21:34:26Z VZ $
// Copyright:   (c) John Wilmes
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dcclient.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/window.h"
    #include "wx/dcmemory.h"
    #include "wx/math.h"
    #include "wx/image.h"
    #include "wx/module.h"
#endif

#include "wx/web/private/utils.h"

// ----------------------------------------------------------------------------
// wxMemoryDC
// ----------------------------------------------------------------------------

//TODO cache draw commands and execute in STL list when needed for better
//  performance

IMPLEMENT_DYNAMIC_CLASS(wxMemoryDC, wxDC)

wxMemoryDC::wxMemoryDC() {
}

wxMemoryDC::~wxMemoryDC() {
}

bool wxMemoryDC::CanDrawBitmap() const {
    return true;
}

bool wxMemoryDC::CanGetTextExtent() const {
    return true;
}

wxCoord wxMemoryDC::GetCharHeight() const {
    //TODO
    return 0;
}

wxCoord wxMemoryDC::GetCharWidth() const {
    //TODO
    return 0;
}

int wxMemoryDC::GetDepth() const {
    //TODO
    return 0;
}

wxSize wxMemoryDC::GetPPI() const {
    //TODO
    return wxSize();
}

void wxMemoryDC::SetBackground(const wxBrush& brush) {
    m_background = brush;
    m_image.backgroundColor(GetMagickColor(brush.GetColour()));
}

void wxMemoryDC::SetBackgroundMode(int mode) {
    //TODO unimplemented
}

void wxMemoryDC::SetBrush(const wxBrush& brush) {
    m_brush = brush;
}

void wxMemoryDC::SetFont(const wxFont& font) {
    m_font = font;
}

void wxMemoryDC::SetLogicalFunction(int function) {
    //TODO unimplemented
}

void wxMemoryDC::SetPen(const wxPen& pen) {
    m_pen = pen;
    m_image.strokeColor(GetMagickColor(m_pen.GetColour()));
    m_image.strokeLineCap(GetMagickCap(m_pen.GetCap()));
    m_image.strokeLineJoin(GetMagickJoin(m_pen.GetJoin()));
    m_image.strokeWidth(m_pen.GetWidth());
}

void wxMemoryDC::Clear() {
    m_image.erase();
}

bool wxMemoryDC::DoFloodFill(wxCoord x, wxCoord y, const wxColour& col,
                             int style) {
    m_image.modifyImage();
    if (style == wxFLOOD_SURFACE) {
        wxColour current;
        wxCHECK_MSG(GetPixel(x, y, &current), false, "invalid target");
        if (current == col) {
            m_image.floodFillColor(x, y, GetMagickColor(m_brush.GetColour()));
        }
    } else if (style == wxFLOOD_BORDER) {
        m_image.floodFillColor(x, y, GetMagickColor(m_brush.GetColour()), GetMagickColor(col));
    }
    return true;
}

bool wxMemoryDC::DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const {
    //TODO do bounds checking?
    col = new wxColour(GetWxColour(m_image.pixelColor(x, y)));
    return true;
}

void wxMemoryDC::DoDrawPoint(wxCoord x, wxCoord y) {
    m_image.modifyImage();
    m_image.pixelColor(x, y, GetMagickColor(m_pen.GetColour()));
}

void wxMemoryDC::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2) {
    m_image.modifyImage();
    Magick::DrawableLine line(x1, y1, x2, y2);
    m_image.draw(line);
}

void wxMemoryDC::DoDrawArc(wxCoord x1, wxCoord y1,
                           wxCoord x2, wxCoord y2,
                           wxCoord xc, wxCoord yc) {
    //TODO convert to start/end angle
}

void wxMemoryDC::DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                                   double sa, double ea) {
    //TODO unimplemented
}

void wxMemoryDC::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord w, wxCoord h) {
    m_image.modifyImage();
    Magick::DrawableRectangle rect(x, y, x+w, y-h);
    m_image.draw(rect);
}

void wxMemoryDC::DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                        wxCoord w, wxCoord h,
                                        double r) {
    //TODO unimplemented
}

void wxMemoryDC::DoDrawEllipse(wxCoord x, wxCoord y, wxCoord w, wxCoord h) {
    //TODO unimplemented
}

void wxMemoryDC::DoCrossHair(wxCoord x, wxCoord y) {
    //TODO unimplemented
}

void wxMemoryDC::DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y) {
    m_image.modifyImage();
    m_image.composite(icon.GetMagickImage(), x, y, Magick::OverCompositeOp);
}

void wxMemoryDC::DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y,
                              bool useMask) {
    m_image.modifyImage();
    m_image.composite(bmp.GetMagickImage(), x, y, Magick::OverCompositeOp);
}

void wxMemoryDC::DoDrawText(const wxString& text, wxCoord x, wxCoord y) {
    m_image.modifyImage();
    Magick::DrawableText dta(x, y, (const char*)text.c_str());
    m_image.draw(dta);
}

void wxMemoryDC::DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y,
                                   double angle) {
    Magick::DrawableRotation rot1(angle);
    Magick::DrawableText dta(x, y, (const char*)text.c_str());
    Magick::DrawableRotation rot2(-1.0 * angle);
    m_image.modifyImage();
    m_image.draw(rot1);
    m_image.draw(dta);
    m_image.draw(rot2);
}

bool wxMemoryDC::DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
                        wxDC *source, wxCoord xsrc, wxCoord ysrc,
                        int rop, bool useMask, wxCoord, wxCoord) {
    //TODO perform mask operations
    wxMemoryDC *memdc;
    try {
        memdc = dynamic_cast<wxMemoryDC*>(source);
    } catch (...) {
        return false;
    }
    Magick::Image comp = memdc->m_image;
    comp.modifyImage();
    comp.crop(Magick::Geometry(width, height, xsrc, ysrc));
    m_image.composite(comp, xdest, ydest, GetMagickCompositeOp(rop));
    return true;
}

void wxMemoryDC::DestroyClippingRegion() {
    //TODO unimplemented
}

void wxMemoryDC::DoSetClippingRegionAsRegion(const wxRegion& region) {
}

void wxMemoryDC::DoSetClippingRegion(wxCoord x, wxCoord y,
                                     wxCoord width, wxCoord height) {
}

void wxMemoryDC::DoGetSize(int *width, int *height) const {
}

void wxMemoryDC::DoGetSizeMM(int* width, int* height) const {
}

void wxMemoryDC::DoDrawLines(int n, wxPoint points[],
                             wxCoord xoffset, wxCoord yoffset) {
    //TODO unimplemented
}

void wxMemoryDC::DoDrawPolygon(int n, wxPoint points[],
                               wxCoord xoffset, wxCoord yoffset,
                               int fillStyle) {
    //TODO unimplemented
}

void wxMemoryDC::DoGetTextExtent(const wxString&,
                                 wxCoord* x, wxCoord* y,
                                 wxCoord* descent,
                                 wxCoord* externalLeading,
                                 const wxFont* theFont) const {
    //TODO unimplemented
}

void wxMemoryDC::DoSelect(const wxBitmap& bmp) {
    //TODO unimplemented
}
