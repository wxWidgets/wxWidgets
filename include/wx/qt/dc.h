/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/dc.h
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_DC_H_
#define _WX_QT_DC_H_

class QPainter;
class QImage;

class WXDLLIMPEXP_FWD_CORE wxRegion;

class WXDLLIMPEXP_CORE wxQtDCImpl : public wxDCImpl
{
public:
    wxQtDCImpl( wxDC *owner );
    ~wxQtDCImpl();

    virtual bool CanDrawBitmap() const wxOVERRIDE;
    virtual bool CanGetTextExtent() const wxOVERRIDE;

    virtual void DoGetSize(int *width, int *height) const wxOVERRIDE;
    virtual void DoGetSizeMM(int* width, int* height) const wxOVERRIDE;

    virtual int GetDepth() const wxOVERRIDE;
    virtual wxSize GetPPI() const wxOVERRIDE;

    virtual void SetFont(const wxFont& font) wxOVERRIDE;
    virtual void SetPen(const wxPen& pen) wxOVERRIDE;
    virtual void SetBrush(const wxBrush& brush) wxOVERRIDE;
    virtual void SetBackground(const wxBrush& brush) wxOVERRIDE;
    virtual void SetBackgroundMode(int mode) wxOVERRIDE;

#if wxUSE_PALETTE
    virtual void SetPalette(const wxPalette& palette) wxOVERRIDE;
#endif // wxUSE_PALETTE

    virtual void SetLogicalFunction(wxRasterOperationMode function) wxOVERRIDE;

    virtual wxCoord GetCharHeight() const wxOVERRIDE;
    virtual wxCoord GetCharWidth() const wxOVERRIDE;
    virtual void DoGetTextExtent(const wxString& string,
                                 wxCoord *x, wxCoord *y,
                                 wxCoord *descent = NULL,
                                 wxCoord *externalLeading = NULL,
                                 const wxFont *theFont = NULL) const wxOVERRIDE;

    virtual void Clear() wxOVERRIDE;

    virtual void DoSetClippingRegion(wxCoord x, wxCoord y,
                                     wxCoord width, wxCoord height) wxOVERRIDE;

    virtual void DoSetDeviceClippingRegion(const wxRegion& region) wxOVERRIDE;
    virtual void DestroyClippingRegion() wxOVERRIDE;

    virtual bool DoFloodFill(wxCoord x, wxCoord y, const wxColour& col,
                             wxFloodFillStyle style = wxFLOOD_SURFACE) wxOVERRIDE;
    virtual bool DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const wxOVERRIDE;

    virtual void DoDrawPoint(wxCoord x, wxCoord y) wxOVERRIDE;
    virtual void DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2) wxOVERRIDE;

    virtual void DoDrawArc(wxCoord x1, wxCoord y1,
                           wxCoord x2, wxCoord y2,
                           wxCoord xc, wxCoord yc) wxOVERRIDE;

    virtual void DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                                   double sa, double ea) wxOVERRIDE;

    virtual void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height) wxOVERRIDE;
    virtual void DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                        wxCoord width, wxCoord height,
                                        double radius) wxOVERRIDE;
    virtual void DoDrawEllipse(wxCoord x, wxCoord y,
                               wxCoord width, wxCoord height) wxOVERRIDE;

    virtual void DoCrossHair(wxCoord x, wxCoord y) wxOVERRIDE;

    virtual void DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y) wxOVERRIDE;
    virtual void DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y,
                              bool useMask = false) wxOVERRIDE;

    virtual void DoDrawText(const wxString& text, wxCoord x, wxCoord y) wxOVERRIDE;
    virtual void DoDrawRotatedText(const wxString& text,
                                   wxCoord x, wxCoord y, double angle) wxOVERRIDE;

    virtual bool DoBlit(wxCoord xdest, wxCoord ydest,
                        wxCoord width, wxCoord height,
                        wxDC *source,
                        wxCoord xsrc, wxCoord ysrc,
                        wxRasterOperationMode rop = wxCOPY,
                        bool useMask = false,
                        wxCoord xsrcMask = wxDefaultCoord,
                        wxCoord ysrcMask = wxDefaultCoord) wxOVERRIDE;

    virtual void DoDrawLines(int n, const wxPoint points[],
                             wxCoord xoffset, wxCoord yoffset ) wxOVERRIDE;

    virtual void DoDrawPolygon(int n, const wxPoint points[],
                           wxCoord xoffset, wxCoord yoffset,
                           wxPolygonFillMode fillStyle = wxODDEVEN_RULE) wxOVERRIDE;

    // Use Qt transformations, as they automatically scale pen widths, text...
    virtual void ComputeScaleAndOrigin() wxOVERRIDE;

    void QtPreparePainter();

    virtual void* GetHandle() const wxOVERRIDE { return (void*) m_qtPainter; }

protected:
    virtual QPixmap *GetQPixmap() { return m_qtPixmap; }

    QPainter *m_qtPainter;
    QPixmap *m_qtPixmap;

    wxRegion m_clippingRegion;
private:
    enum wxQtRasterColourOp
    {
        wxQtNONE,
        wxQtWHITE,
        wxQtBLACK,
        wxQtINVERT
    };
    wxQtRasterColourOp m_rasterColourOp;
    QColor *m_qtPenColor;
    QColor *m_qtBrushColor;
    void ApplyRasterColourOp();

    wxDECLARE_CLASS(wxQtDCImpl);
    wxDECLARE_NO_COPY_CLASS(wxQtDCImpl);

};

#endif // _WX_QT_DC_H_
