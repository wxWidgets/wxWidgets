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

    virtual bool CanDrawBitmap() const override;
    virtual bool CanGetTextExtent() const override;

    virtual void DoGetSize(int *width, int *height) const override;
    virtual void DoGetSizeMM(int* width, int* height) const override;

    virtual int GetDepth() const override;
    virtual wxSize GetPPI() const override;

    virtual void SetFont(const wxFont& font) override;
    virtual void SetPen(const wxPen& pen) override;
    virtual void SetBrush(const wxBrush& brush) override;
    virtual void SetBackground(const wxBrush& brush) override;
    virtual void SetBackgroundMode(int mode) override;

#if wxUSE_PALETTE
    virtual void SetPalette(const wxPalette& palette) override;
#endif // wxUSE_PALETTE

    virtual void SetLogicalFunction(wxRasterOperationMode function) override;

    virtual wxCoord GetCharHeight() const override;
    virtual wxCoord GetCharWidth() const override;
    virtual void DoGetTextExtent(const wxString& string,
                                 wxCoord *x, wxCoord *y,
                                 wxCoord *descent = nullptr,
                                 wxCoord *externalLeading = nullptr,
                                 const wxFont *theFont = nullptr) const override;

    virtual void Clear() override;

    void UpdateClipBox();

    virtual bool DoGetClippingRect(wxRect& rect) const override;
    virtual void DoSetClippingRegion(wxCoord x, wxCoord y,
                                     wxCoord width, wxCoord height) override;

    virtual void DoSetDeviceClippingRegion(const wxRegion& region) override;
    virtual void DestroyClippingRegion() override;

    virtual bool DoFloodFill(wxCoord x, wxCoord y, const wxColour& col,
                             wxFloodFillStyle style = wxFLOOD_SURFACE) override;
    virtual bool DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const override;

    virtual void DoDrawPoint(wxCoord x, wxCoord y) override;
    virtual void DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2) override;

    virtual void DoDrawArc(wxCoord x1, wxCoord y1,
                           wxCoord x2, wxCoord y2,
                           wxCoord xc, wxCoord yc) override;

    virtual void DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                                   double sa, double ea) override;

    virtual void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height) override;
    virtual void DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                        wxCoord width, wxCoord height,
                                        double radius) override;
    virtual void DoDrawEllipse(wxCoord x, wxCoord y,
                               wxCoord width, wxCoord height) override;

    virtual void DoCrossHair(wxCoord x, wxCoord y) override;

    virtual void DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y) override;
    virtual void DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y,
                              bool useMask = false) override;

    virtual void DoDrawText(const wxString& text, wxCoord x, wxCoord y) override;
    virtual void DoDrawRotatedText(const wxString& text,
                                   wxCoord x, wxCoord y, double angle) override;

    virtual bool DoBlit(wxCoord xdest, wxCoord ydest,
                        wxCoord width, wxCoord height,
                        wxDC *source,
                        wxCoord xsrc, wxCoord ysrc,
                        wxRasterOperationMode rop = wxCOPY,
                        bool useMask = false,
                        wxCoord xsrcMask = wxDefaultCoord,
                        wxCoord ysrcMask = wxDefaultCoord) override;

    virtual void DoDrawLines(int n, const wxPoint points[],
                             wxCoord xoffset, wxCoord yoffset ) override;

    virtual void DoDrawPolygon(int n, const wxPoint points[],
                           wxCoord xoffset, wxCoord yoffset,
                           wxPolygonFillMode fillStyle = wxODDEVEN_RULE) override;

    virtual void DoDrawPolyPolygon(int n, const int count[], const wxPoint points[],
                                   wxCoord xoffset, wxCoord yoffset,
                                   wxPolygonFillMode fillStyle) override;

    // Use Qt transformations, as they automatically scale pen widths, text...
    virtual void ComputeScaleAndOrigin() override;

    void QtPreparePainter();

    virtual void* GetHandle() const override { return (void*) m_qtPainter; }

protected:
    virtual QPixmap *GetQPixmap() { return m_qtPixmap; }

    QPainter *m_qtPainter;
    QPixmap *m_qtPixmap;

    wxRegion m_clippingRegion;

    bool m_isClipBoxValid = false;

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
