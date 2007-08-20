/////////////////////////////////////////////////////////////////////////////
// Name:        src/web/dcclient.cpp
// Purpose:     wxClientDC class
// Author:      Julian Smart, Robert Roebling, John Wilmes
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id: dcclient.cpp 45851 2007-05-05 21:34:26Z VZ $
// Copyright:   (c) Julian Smart, Robert Roebling, John Wilmes
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
// wxWindowDC
// ----------------------------------------------------------------------------

/*TODO determine our subregion of the toplevel window
 * on flush, surround drawing commands with translation/scaling/clipping to the
 * appropriate region
 */
IMPLEMENT_DYNAMIC_CLASS(wxWindowDC, wxMemoryDC)

wxWindowDC::wxWindowDC() {
    //TODO -> should this be rendered? where?
    Create(NULL);
}

wxWindowDC::wxWindowDC(wxWindow* window) {
    Create(window);
}

bool wxWindowDC::Create(wxWindow* window) {
    if (window) {
        m_window = window;
        return true;
    }
    return false;
}

wxWindowDC::~wxWindowDC() {
    //TODO guarantee we flush
}

void wxWindowDC::SetBackground(const wxBrush& brush) {
    wxString cmd;
    cmd.Printf(wxT("SetBackground('%s')"), GetJsonBrushString(brush));
    m_window->EvalInClient(BuildClientCommand(cmd));
    wxMemoryDC::SetBackground(brush);
}

void wxWindowDC::SetBackgroundMode(int mode) {
    wxString cmd;
    cmd.Printf(wxT("SetBackgroundMode('%d')"), mode);
    m_window->EvalInClient(BuildClientCommand(cmd));
    wxMemoryDC::SetBackgroundMode(mode);
}

void wxWindowDC::SetBrush(const wxBrush& brush) {
    wxString cmd;
    cmd.Printf(wxT("SetBrush('%s')"), GetJsonBrushString(brush));
    m_window->EvalInClient(BuildClientCommand(cmd));
    wxMemoryDC::SetBrush(brush);
}

void wxWindowDC::SetFont(const wxFont& font) {
    wxString cmd;
    cmd.Printf(wxT("SetFont('%s')"), GetJsonFontString(font));
    m_window->EvalInClient(BuildClientCommand(cmd));
    wxMemoryDC::SetFont(font);
}

void wxWindowDC::SetLogicalFunction(int function) {
    wxString cmd;
    cmd.Printf(wxT("SetLogicalFunction('%d')"), function);
    m_window->EvalInClient(BuildClientCommand(cmd));
    wxMemoryDC::SetLogicalFunction(function);
}

void wxWindowDC::SetPen(const wxPen& pen) {
    wxString cmd;
    cmd.Printf(wxT("SetPen('%s')"), GetJsonPenString(pen));
    m_window->EvalInClient(BuildClientCommand(cmd));
    wxMemoryDC::SetPen(pen);
}

wxString wxWindowDC::BuildClientCommand(const wxString& cmd) {
    wxString clcmd = "App.getDC().";
    clcmd += cmd;
    clcmd += ";\n";
    return clcmd;
}

void wxWindowDC::Clear() {
    wxString cmd;
    cmd.Printf(wxT("Clear()"));
    m_window->EvalInClient(BuildClientCommand(cmd));
    wxMemoryDC::Clear();
}

bool wxWindowDC::DoFloodFill(wxCoord x, wxCoord y, const wxColour& col,
                             int style) {
    wxString cmd;
    cmd.Printf(wxT("FloodFill(%d, %d, '%s', %d)"),
               x, y, GetJsonColourString(col), style);
    m_window->EvalInClient(BuildClientCommand(cmd));
    return true;
}

void wxWindowDC::DoDrawPoint(wxCoord x, wxCoord y) {
    wxString cmd;
    cmd.Printf(wxT("DrawPoint(%d, %d)"), x, y);
    m_window->EvalInClient(BuildClientCommand(cmd));
    wxMemoryDC::DoDrawPoint(x, y);
}

void wxWindowDC::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2) {
    wxString cmd;
    cmd.Printf(wxT("DrawLine(%d, %d, %d, %d)"), x1, y1, x2, y2);
    m_window->EvalInClient(BuildClientCommand(cmd));
    wxMemoryDC::DoDrawLine(x1, y1, x2, y2);
}

void wxWindowDC::DoDrawArc(wxCoord x1, wxCoord y1,
                           wxCoord x2, wxCoord y2,
                           wxCoord xc, wxCoord yc) {
    wxString cmd;
    cmd.Printf(wxT("DrawLine(%d, %d, %d, %d, %d, %d)"),
               x1, y1, x2, y2, xc, yc);
    m_window->EvalInClient(BuildClientCommand(cmd));
    wxMemoryDC::DoDrawArc(x1, y1, x2, y2, xc, yc);
}

void wxWindowDC::DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                                   double sa, double ea) {
    wxString cmd;
    cmd.Printf(wxT("DrawEllipticArc(%d, %d, %d, %d, %f, %f)"),
               x, y, w, h, sa, ea);
    m_window->EvalInClient(BuildClientCommand(cmd));
    wxMemoryDC::DoDrawEllipticArc(x, y, w, h, sa, ea);
}

void wxWindowDC::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord w, wxCoord h) {
    wxString cmd;
    cmd.Printf(wxT("DrawRectangle(%d, %d, %d, %d)"), x, y, w, h);
    m_window->EvalInClient(BuildClientCommand(cmd));
    wxMemoryDC::DoDrawRectangle(x, y, w, h);
}

void wxWindowDC::DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                        wxCoord w, wxCoord h,
                                        double r) {
    wxString cmd;
    cmd.Printf(wxT("DrawRoundedRectangle(%d, %d, %d, %d, %f)"),
               x, y, w, h, r);
    m_window->EvalInClient(BuildClientCommand(cmd));
    wxMemoryDC::DoDrawRoundedRectangle(x, y, w, h, r);
}

void wxWindowDC::DoDrawEllipse(wxCoord x, wxCoord y, wxCoord w, wxCoord h) {
    wxString cmd;
    cmd.Printf(wxT("DrawEllipse(%d, %d, %d, %d)"), x, y, w, h);
    m_window->EvalInClient(BuildClientCommand(cmd));
    wxMemoryDC::DoDrawEllipse(x, y, w, h);
}

void wxWindowDC::DoCrossHair(wxCoord x, wxCoord y) {
    wxString cmd;
    cmd.Printf(wxT("CrossHair(%d, %d)"), x, y);
    m_window->EvalInClient(BuildClientCommand(cmd));
    wxMemoryDC::DoCrossHair(x, y);
}

void wxWindowDC::DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y) {
    wxString file = wxTheApp->GetResourceFile();
    icon.SaveFile(wxTheApp->GetResourcePath() + file, wxBITMAP_TYPE_BMP);
    wxString cmd;
    cmd.Printf(wxT("DrawBitmap('%s', %d, %d)"), wxTheApp->GetResourceUrl() + file, x, y);
    m_window->EvalInClient(BuildClientCommand(cmd));
    wxMemoryDC::DoDrawIcon(icon, x, y);
}

void wxWindowDC::DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y,
                              bool useMask) {
    wxString file = wxTheApp->GetResourceFile();
    bmp.SaveFile(wxTheApp->GetResourcePath() + file, wxBITMAP_TYPE_BMP);
    wxString cmd;
    cmd.Printf(wxT("DrawBitmap('%s', %d, %d)"), wxTheApp->GetResourceUrl() + file, x, y);
    m_window->EvalInClient(BuildClientCommand(cmd));
    wxMemoryDC::DoDrawBitmap(bmp, x, y, useMask);
}

void wxWindowDC::DoDrawText(const wxString& text, wxCoord x, wxCoord y) {
    wxString cmd;
    cmd.Printf(wxT("DrawText('%s', %d, %d)"), text, x, y);
    m_window->EvalInClient(BuildClientCommand(cmd));
    wxMemoryDC::DoDrawText(text, x, y);
}

void wxWindowDC::DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y,
                                   double angle) {
    wxString cmd;
    cmd.Printf(wxT("DrawRotatedText('%s', %d, %d, %d)"), text, x, y, angle);
    m_window->EvalInClient(BuildClientCommand(cmd));
    wxMemoryDC::DoDrawRotatedText(text, x, y, angle);
}

bool wxWindowDC::DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
                        wxDC *source, wxCoord xsrc, wxCoord ysrc,
                        int rop, bool useMask, wxCoord xsrcMask, wxCoord ysrcMask) {
    wxMemoryDC *memdc;
    try {
        memdc = dynamic_cast<wxMemoryDC*>(source);
    } catch (...) {
        return false;
    }
    wxString file = wxTheApp->GetResourceFile();
    wxString path = wxTheApp->GetResourcePath() + file;
    Magick::Image save = *memdc->m_image;
    save.modifyImage();
    save.crop(Magick::Geometry(width, height, xsrc, ysrc));
    save.magick(GetMagick(wxBITMAP_TYPE_PNG));
    save.write((const char*)path.c_str());

    wxString cmd;
    cmd.Printf(wxT("Blit(%s, %d, %d, %d)"), wxTheApp->GetResourceUrl() + file, xdest, ydest, rop);
    m_window->EvalInClient(BuildClientCommand(cmd));

    wxMemoryDC::DoBlit(xdest, ydest, width, height, source, xsrc, ysrc, rop, useMask,  xsrcMask, ysrcMask);
    return true;
}

void wxWindowDC::DoDrawLines(int n, wxPoint points[],
                             wxCoord xoffset, wxCoord yoffset) {
    wxString cmd;
    cmd.Printf(wxT("DrawLines(%d, %s, %d, %d)"),
               n, GetJsonPointsString(n, points), xoffset, yoffset);
    m_window->EvalInClient(BuildClientCommand(cmd));
    wxMemoryDC::DoDrawLines(n, points, xoffset, yoffset);
}

void wxWindowDC::DoDrawPolygon(int n, wxPoint points[],
                               wxCoord xoffset, wxCoord yoffset,
                               int fillStyle) {
    wxString cmd;
    cmd.Printf(wxT("DrawPolygon(%d, %s, %d, %d, %d)"),
               n, GetJsonPointsString(n, points), xoffset, yoffset, fillStyle);
    m_window->EvalInClient(BuildClientCommand(cmd));
    wxMemoryDC::DoDrawPolygon(n, points, xoffset, yoffset, fillStyle);
}

void wxWindowDC::DoSetClippingRegionAsRegion(const wxRegion& region) {
    wxCoord x, y, width, height;
    region.GetBox(x, y, width, height);
    DoSetClippingRegion(x, y, width, height);
}

void wxWindowDC::DoSetClippingRegion(wxCoord x, wxCoord y,
                                     wxCoord width, wxCoord height) {
    wxString cmd;
    cmd.Printf(wxT("SetClippingRegion(%d, %d, %d, %d)"), x, y, width, height);
    m_window->EvalInClient(BuildClientCommand(cmd));
    wxMemoryDC::DoSetClippingRegion(x, y, width, height);
}

//-----------------------------------------------------------------------------
// wxClientDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxClientDC, wxWindowDC)

wxClientDC::wxClientDC() {
}

wxClientDC::wxClientDC( wxWindow *window )
          : wxWindowDC( window )
{
    wxCHECK_RET(window, _T("NULL window in wxClientDC::wxClientDC"));
}

wxClientDC::~wxClientDC() {
}

// ----------------------------------------------------------------------------
// wxPaintDC
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxPaintDC, wxClientDC)

wxPaintDC::wxPaintDC() {
}

wxPaintDC::wxPaintDC(wxWindow* window)
  : wxClientDC(window)
{
}

wxPaintDC::~wxPaintDC() {
}
