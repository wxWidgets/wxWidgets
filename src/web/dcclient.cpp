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

IMPLEMENT_DYNAMIC_CLASS(wxWindowDC, wxMemoryDC)

wxWindowDC::wxWindowDC() {
}

wxWindowDC::wxWindowDC(wxWindow* window) {
    m_window = window;
}

wxWindowDC::~wxWindowDC() {
}

void wxWindowDC::SetBackground(const wxBrush& brush) {
    wxString cmd;
    cmd.Printf(wxT("SetBackground('%s')"), GetJsonBrush(brush));
    ClientEval(cmd);
    wxMemoryDC::SetBackground(brush);
}

void wxWindowDC::SetBackgroundMode(int mode) {
    wxString cmd;
    cmd.Printf(wxT("SetBackgroundMode('%d')"), mode);
    ClientEval(cmd);
    wxMemoryDC::SetBackgroundMode(mode);
}

void wxWindowDC::SetBrush(const wxBrush& brush) {
    wxString cmd;
    cmd.Printf(wxT("SetBrush('%s')"), GetJsonBrush(brush));
    ClientEval(cmd);
    wxMemoryDC::SetBrush(brush);
}

void wxWindowDC::SetFont(const wxFont& font) {
    wxString cmd;
    cmd.Printf(wxT("SetFont('%s')"), GetJsonFont(font));
    ClientEval(cmd);
    wxMemoryDC::SetFont(font);
}

void wxWindowDC::SetLogicalFunction(int function) {
    wxString cmd;
    cmd.Printf(wxT("SetLogicalFunction('%d')"), function);
    ClientEval(cmd);
    wxMemoryDC::SetLogicalFunction(function);
}

void wxWindowDC::SetPen(const wxPen& pen) {
    wxString cmd;
    cmd.Printf(wxT("SetPen('%s')"), GetJsonPen(pen));
    ClientEval(cmd);
    wxMemoryDC::SetPen(pen);
}

void wxWindowDC::ClientEval(const wxString& cmd) {
    m_evalBuf.Append("App.getDC().");
    m_evalBuf.Append(cmd);
    m_evalBuf.Append(";\n");
}

bool wxWindowDC::FlushEvalBuffer() {
    // This method should be called exactly once per request, so we can assume
    // that someone is listening on the other end of the FIFO, and not bother
    // with open/O_NONBLOCK
    FILE* fd = fopen(wxTheApp->m_responseFifoPath, "w");
    if (NULL == fd) {
        //can't open response FIFO, even though we should block until someone
        //reads
#if wxUSE_LOG
        wxLogSysError("Unable to open response FIFO to flush eval buffer at '%s'",
                     wxTheApp->m_responseFifoPath);
#endif //wxUSE_LOG
        return false;
    }
    if (EOF == fputs(m_evalBuf.fn_str(), fd)) {
        //can't write to response FIFO, even though it was successfully opened
#if wxUSE_LOG
        wxLogSysError("Unable to write to response FIFO to flush eval buffer at '%s'",
                     wxTheApp->m_responseFifoPath);
#endif //wxUSE_LOG
        return false;
    }
    m_evalBuf.Empty(); //Don't clear, we'll need the memory soon enough
    if (EOF == fclose(fd)) {
        //can't close response FIFO
#if wxUSE_LOG
        wxLogSysError("Unable to close response FIFO to flush eval buffer at '%s'",
                     wxTheApp->m_responseFifoPath);
#endif //wxUSE_LOG
    }
    return true;
}

void wxWindowDC::Clear() {
    wxString cmd;
    cmd.Printf(wxT("Clear()"));
    ClientEval(cmd);
    wxMemoryDC::Clear();
}

bool wxWindowDC::DoFloodFill(wxCoord x, wxCoord y, const wxColour& col,
                             int style) {
    wxString cmd;
    cmd.Printf(wxT("FloodFill(%d, %d, '%s', %d)"),
               x, y, col.GetAsString(), style);
    ClientEval(cmd);
    return true;
}

void wxWindowDC::DoDrawPoint(wxCoord x, wxCoord y) {
    wxString cmd;
    cmd.Printf(wxT("DrawPoint(%d, %d)"), x, y);
    ClientEval(cmd);
    wxMemoryDC::DoDrawPoint(x, y);
}

void wxWindowDC::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2) {
    wxString cmd;
    cmd.Printf(wxT("DrawLine(%d, %d, %d, %d)"), x1, y1, x2, y2);
    ClientEval(cmd);
    wxMemoryDC::DoDrawLine(x1, y1, x2, y2);
}

void wxWindowDC::DoDrawArc(wxCoord x1, wxCoord y1,
                           wxCoord x2, wxCoord y2,
                           wxCoord xc, wxCoord yc) {
    wxString cmd;
    cmd.Printf(wxT("DrawLine(%d, %d, %d, %d, %d, %d)"),
               x1, y1, x2, y2, xc, yc);
    ClientEval(cmd);
    wxMemoryDC::DoDrawArc(x1, y1, x2, y2, xc, yc);
}

void wxWindowDC::DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                                   double sa, double ea) {
    wxString cmd;
    cmd.Printf(wxT("DrawEllipticArc(%d, %d, %d, %d, %f, %f)"),
               x, y, w, h, sa, ea);
    ClientEval(cmd);
    wxMemoryDC::DoDrawEllipticArc(x, y, w, h, sa, ea);
}

void wxWindowDC::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord w, wxCoord h) {
    wxString cmd;
    cmd.Printf(wxT("DrawRectangle(%d, %d, %d, %d)"), x, y, w, h);
    ClientEval(cmd);
    wxMemoryDC::DoDrawRectangle(x, y, w, h);
}

void wxWindowDC::DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                        wxCoord w, wxCoord h,
                                        double r) {
    wxString cmd;
    cmd.Printf(wxT("DrawRoundedRectangle(%d, %d, %d, %d, %f)"),
               x, y, w, h, r);
    ClientEval(cmd);
    wxMemoryDC::DoDrawRoundedRectangle(x, y, w, h, r);
}

void wxWindowDC::DoDrawEllipse(wxCoord x, wxCoord y, wxCoord w, wxCoord h) {
    wxString cmd;
    cmd.Printf(wxT("DrawEllipse(%d, %d, %d, %d)"), x, y, w, h);
    ClientEval(cmd);
    wxMemoryDC::DoDrawEllipse(x, y, w, h);
}

void wxWindowDC::DoCrossHair(wxCoord x, wxCoord y) {
    wxString cmd;
    cmd.Printf(wxT("CrossHair(%d, %d)"), x, y);
    ClientEval(cmd);
    wxMemoryDC::DoCrossHair(x, y);
}

void wxWindowDC::DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y) {
    wxString file = wxString::Format("%d.bmp", wxNewId());
    icon.SaveFile(wxTheApp->GetResourcePath() + file, wxBITMAP_TYPE_BMP);
    wxString cmd;
    cmd.Printf(wxT("DrawBitmap('%s', %d, %d)"), wxTheApp->GetResourceUrl() + file, x, y);
    ClientEval(cmd);
    wxMemoryDC::DoDrawIcon(icon, x, y);
}

void wxWindowDC::DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y,
                              bool useMask) {
    wxString file = wxString::Format("%d.bmp", wxNewId());
    bmp.SaveFile(wxTheApp->GetResourcePath() + file, wxBITMAP_TYPE_BMP);
    wxString cmd;
    cmd.Printf(wxT("DrawBitmap('%s', %d, %d)"), wxTheApp->GetResourceUrl() + file, x, y);
    ClientEval(cmd);
    wxMemoryDC::DoDrawBitmap(bmp, x, y, useMask);
}

void wxWindowDC::DoDrawText(const wxString& text, wxCoord x, wxCoord y) {
    wxString cmd;
    cmd.Printf(wxT("DrawText('%s', %d, %d)"), text, x, y);
    ClientEval(cmd);
    wxMemoryDC::DoDrawText(text, x, y);
}

void wxWindowDC::DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y,
                                   double angle) {
    wxString cmd;
    cmd.Printf(wxT("DrawRotatedText('%s', %d, %d, %d)"), text, x, y, angle);
    ClientEval(cmd);
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
    wxString file = wxString::Format("%d.png", wxNewId());
    wxString path = wxTheApp->GetResourcePath() + file;
    Magick::Image save = memdc->m_image;
    save.modifyImage();
    save.crop(Magick::Geometry(width, height, xsrc, ysrc));
    save.magick(GetMagick(wxBITMAP_TYPE_PNG));
    save.write((const char*)path.c_str());

    wxString cmd;
    cmd.Printf(wxT("Blit(%s, %d, %d, %d)"), wxTheApp->GetResourceUrl() + file, xdest, ydest, rop);
    ClientEval(cmd);

    wxMemoryDC::DoBlit(xdest, ydest, width, height, source, xsrc, ysrc, rop, useMask,  xsrcMask, ysrcMask);
    return true;
}

void wxWindowDC::DoDrawLines(int n, wxPoint points[],
                             wxCoord xoffset, wxCoord yoffset) {
    wxString cmd;
    cmd.Printf(wxT("DrawLines(%d, %s, %d, %d)"),
               n, GetJsonPoints(n, points), xoffset, yoffset);
    ClientEval(cmd);
    wxMemoryDC::DoDrawLines(n, points, xoffset, yoffset);
}

void wxWindowDC::DoDrawPolygon(int n, wxPoint points[],
                               wxCoord xoffset, wxCoord yoffset,
                               int fillStyle) {
    wxString cmd;
    cmd.Printf(wxT("DrawPolygon(%d, %s, %d, %d, %d)"),
               n, GetJsonPoints(n, points), xoffset, yoffset, fillStyle);
    ClientEval(cmd);
    wxMemoryDC::DoDrawPolygon(n, points, xoffset, yoffset, fillStyle);
}

void wxWindowDC::DoSetClippingRegionAsRegion(const wxRegion& region) {
    //TODO
    wxMemoryDC::DoSetClippingRegionAsRegion(region);
}

void wxWindowDC::DoSetClippingRegion(wxCoord x, wxCoord y,
                                     wxCoord width, wxCoord height) {
    //TODO
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
