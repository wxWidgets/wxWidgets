// Copyright (c) 2007 John Wilmes

#include "wx/app.h"
#include "wx/defs.h"
#include "wx/dcclient.h"
#include "wx/gdicmn.h"
#include "wx/log.h"

IMPLEMENT_DYNAMIC_CLASS(wxWindowDC, wxDC)

wxWindowDC::wxWindowDC() {
}

wxWindowDC::~wxWindowDC() {
}

void wxWindowDC::ClientEval(const wxString& cmd) {
    //TODO prepend cmd with "dc->" or something similar,
    //      append with ";\n"
    m_evalBuf.Append(cmd);
}

bool wxWindowDC::FlushEvalBuffer() {
    // The flush event should be sent exactly once per request, so we can assume
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

bool wxWindowDC::DoFloodFill(wxCoord x, wxCoord y, const wxColour& col,
                             int style) {
    wxString cmd;
    cmd.Printf(wxT("FloodFill(%d, %d, '%s', %d)"),
               x, y, col.GetAsString(), style);
    ClientEval(cmd);
    return true;
}

// Currently there is no way to get this info from the client
// Eventually we will probably have to duplicate all drawing code in a bitmap
// on the server-side to implement this properly
bool wxWindowDC::DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const {
    col = new wxColour;
    return false;
}

void wxWindowDC::DoDrawPoint(wxCoord x, wxCoord y) {
    wxString cmd;
    cmd.Printf(wxT("DrawPoint(%d, %d)"), x, y);
    ClientEval(cmd);
}

void wxWindowDC::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2) {
    wxString cmd;
    cmd.Printf(wxT("DrawLine(%d, %d, %d, %d)"), x1, y1, x2, y2);
    ClientEval(cmd);
}

void wxWindowDC::DoDrawArc(wxCoord x1, wxCoord y1,
                           wxCoord x2, wxCoord y2,
                           wxCoord xc, wxCoord yc) {
    wxString cmd;
    cmd.Printf(wxT("DrawLine(%d, %d, %d, %d, %d, %d)"),
               x1, y1, x2, y2, xc, yc);
    ClientEval(cmd);
}

void wxWindowDC::DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                                   double sa, double ea) {
    wxString cmd;
    cmd.Printf(wxT("DrawEllipticArc(%d, %d, %d, %d, %f, %f)"),
               x, y, w, h, sa, ea);
    ClientEval(cmd);
}

void wxWindowDC::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord w, wxCoord h) {
    wxString cmd;
    cmd.Printf(wxT("DrawRectangle(%d, %d, %d, %d)"), x, y, w, h);
    ClientEval(cmd);
}

void wxWindowDC::DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                        wxCoord w, wxCoord h,
                                        double r) {
    wxString cmd;
    cmd.Printf(wxT("DrawRoundedRectangle(%d, %d, %d, %d, %f)"),
               x, y, w, h, r);
    ClientEval(cmd);
}

void wxWindowDC::DoDrawEllipse(wxCoord x, wxCoord y, wxCoord w, wxCoord h) {
    wxString cmd;
    cmd.Printf(wxT("DrawEllipse(%d, %d, %d, %d)"), x, y, w, h);
    ClientEval(cmd);
}

void wxWindowDC::DoCrossHair(wxCoord x, wxCoord y) {
    wxString cmd;
    cmd.Printf(wxT("CrossHair(%d, %d)"), x, y);
    ClientEval(cmd);
}

void wxWindowDC::DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y) {
    //TODO We'll probably have to write the icon out to a file, and send the URL
    wxString cmd;
    wxString url = "DrawIcon unimplemented";
    cmd.Printf(wxT("DrawIcon('%s', %d, %d)"), url, x, y);
    ClientEval(cmd);
}

void wxWindowDC::DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y,
                              bool useMask) {
    //TODO We'll probably have to write the bmp out to a file, and send the URL
    wxString cmd;
    wxString url = "DrawBitmap unimplemented";
    cmd.Printf(wxT("DrawBitmap('%s', %d, %d)"), url, x, y);
    ClientEval(cmd);
}

void wxWindowDC::DoDrawText(const wxString& text, wxCoord x, wxCoord y) {
    wxString cmd;
    cmd.Printf(wxT("DrawText('%s', %d, %d)"), text, x, y);
    ClientEval(cmd);
}

void wxWindowDC::DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y,
                                   double angle) {
    wxString cmd;
    cmd.Printf(wxT("DrawRotatedText('%s', %d, %d, %d)"), text, x, y, angle);
    ClientEval(cmd);
}

bool wxWindowDC::DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
                        wxDC *source, wxCoord xsrc, wxCoord ysrc,
                        int rop, bool useMask, wxCoord, wxCoord) {
    //TODO If this is a wxMemoryDC, we could try the DrawBitmap approach above
    // If it's not a wxMemoryDC, I'm not really sure how we would do this
    return false;
}

void wxWindowDC::DoSetClippingRegionAsRegion(const wxRegion& region) {
}

void wxWindowDC::DoSetClippingRegion(wxCoord x, wxCoord y,
                                     wxCoord width, wxCoord height) {
}

void wxWindowDC::DoGetSize(int *width, int *height) const {
}

void wxWindowDC::DoGetSizeMM(int* width, int* height) const {
}

void wxWindowDC::DoDrawLines(int n, wxPoint points[],
                             wxCoord xoffset, wxCoord yoffset) {
    //TODO convert points to JSON array representation, store in array
    wxString array;
    wxString cmd;
    cmd.Printf(wxT("DrawLines(%d, %s, %d, %d)"),
               n, array, xoffset, yoffset);
    ClientEval(cmd);
}

void wxWindowDC::DoDrawPolygon(int n, wxPoint points[],
                               wxCoord xoffset, wxCoord yoffset,
                               int fillStyle) {
    //TODO convert points to JSON array representation, store in array
    wxString array;
    wxString cmd;
    cmd.Printf(wxT("DrawPolygon(%d, %s, %d, %d, %d)"),
               n, array, xoffset, yoffset, fillStyle);
    ClientEval(cmd);
}

void wxWindowDC::DoGetTextExtent(const wxString&,
                                 wxCoord* x, wxCoord* y,
                                 wxCoord* descent,
                                 wxCoord* externalLeading,
                                 const wxFont* theFont) const {
}
