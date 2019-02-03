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

    virtual bool CanDrawBitmap() const;
    virtual bool CanGetTextExtent() const;

    virtual void DoGetSize(int *width, int *height) const;
    virtual void DoGetSizeMM(int* width, int* height) const;

    virtual int GetDepth() const;
    virtual wxSize GetPPI() const;

    virtual void SetFont(const wxFont& font);
    virtual void SetPen(const wxPen& pen);
    virtual void SetBrush(const wxBrush& brush);
    virtual void SetBackground(const wxBrush& brush);
    virtual void SetBackgroundMode(int mode);

#if wxUSE_PALETTE
    virtual void SetPalette(const wxPalette& palette);
#endif // wxUSE_PALETTE

    virtual void SetLogicalFunction(wxRasterOperationMode function);

    virtual wxCoord GetCharHeight() const;
    virtual wxCoord GetCharWidth() const;
    virtual void DoGetTextExtent(const wxString& string,
                                 wxCoord *x, wxCoord *y,
                                 wxCoord *descent = NULL,
                                 wxCoord *externalLeading = NULL,
                                 const wxFont *theFont = NULL) const;

    virtual void Clear();

    virtual void DoSetClippingRegion(wxCoord x, wxCoord y,
                                     wxCoord width, wxCoord height);

    virtual void DoSetDeviceClippingRegion(const wxRegion& region);
    virtual void DestroyClippingRegion();

    virtual bool DoFloodFill(wxCoord x, wxCoord y, const wxColour& col,
                             wxFloodFillStyle style = wxFLOOD_SURFACE);
    virtual bool DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const;

    virtual void DoDrawPoint(wxCoord x, wxCoord y);
    virtual void DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2);

    virtual void DoDrawArc(wxCoord x1, wxCoord y1,
                           wxCoord x2, wxCoord y2,
                           wxCoord xc, wxCoord yc);

    virtual void DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                                   double sa, double ea);

    virtual void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    virtual void DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                        wxCoord width, wxCoord height,
                                        double radius);
    virtual void DoDrawEllipse(wxCoord x, wxCoord y,
                               wxCoord width, wxCoord height);

    virtual void DoCrossHair(wxCoord x, wxCoord y);

    virtual void DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y);
    virtual void DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y,
                              bool useMask = false);

    virtual void DoDrawText(const wxString& text, wxCoord x, wxCoord y);
    virtual void DoDrawRotatedText(const wxString& text,
                                   wxCoord x, wxCoord y, double angle);

    virtual bool DoBlit(wxCoord xdest, wxCoord ydest,
                        wxCoord width, wxCoord height,
                        wxDC *source,
                        wxCoord xsrc, wxCoord ysrc,
                        wxRasterOperationMode rop = wxCOPY,
                        bool useMask = false,
                        wxCoord xsrcMask = wxDefaultCoord,
                        wxCoord ysrcMask = wxDefaultCoord);

    virtual void DoDrawLines(int n, const wxPoint points[],
                             wxCoord xoffset, wxCoord yoffset );

    virtual void DoDrawPolygon(int n, const wxPoint points[],
                           wxCoord xoffset, wxCoord yoffset,
                           wxPolygonFillMode fillStyle = wxODDEVEN_RULE);

    // Use Qt transformations, as they automatically scale pen widths, text...
    virtual void ComputeScaleAndOrigin();

    void QtPreparePainter();

    virtual void* GetHandle() const { return (void*) m_qtPainter; }

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
