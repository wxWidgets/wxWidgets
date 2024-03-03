/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/dcpsg.h
// Purpose:     wxPostScriptDC class
// Author:      Julian Smart and others
// Copyright:   (c) Julian Smart and Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCPSG_H_
#define _WX_DCPSG_H_

#include "wx/defs.h"

#if wxUSE_PRINTING_ARCHITECTURE && wxUSE_POSTSCRIPT

#include "wx/dc.h"
#include "wx/dcprint.h"
#include "wx/dialog.h"
#include "wx/module.h"
#include "wx/cmndata.h"
#include "wx/strvararg.h"

//-----------------------------------------------------------------------------
// wxPostScriptDC
//-----------------------------------------------------------------------------


class WXDLLIMPEXP_CORE wxPostScriptDC : public wxDC
{
public:
    wxPostScriptDC();

    // Recommended constructor
    wxPostScriptDC(const wxPrintData& printData);

private:
    wxDECLARE_DYNAMIC_CLASS(wxPostScriptDC);
};

class WXDLLIMPEXP_CORE wxPostScriptDCImpl : public wxDCImpl
{
public:
    wxPostScriptDCImpl( wxPrinterDC *owner );
    wxPostScriptDCImpl( wxPrinterDC *owner, const wxPrintData& data );
    wxPostScriptDCImpl( wxPostScriptDC *owner );
    wxPostScriptDCImpl( wxPostScriptDC *owner, const wxPrintData& data );

    void Init();

    virtual ~wxPostScriptDCImpl();

    virtual bool Ok() const { return IsOk(); }
    virtual bool IsOk() const override;

    bool CanDrawBitmap() const override { return true; }

    void Clear() override;
    void SetFont( const wxFont& font ) override;
    void SetPen( const wxPen& pen ) override;
    void SetBrush( const wxBrush& brush ) override;
    void SetLogicalFunction( wxRasterOperationMode function ) override;
    void SetBackground( const wxBrush& brush ) override;

    void DestroyClippingRegion() override;

    bool StartDoc(const wxString& message) override;
    void EndDoc() override;
    void StartPage() override;
    void EndPage() override;

    wxCoord GetCharHeight() const override;
    wxCoord GetCharWidth() const override;
    bool CanGetTextExtent() const override { return true; }

    // Resolution in pixels per logical inch
    wxSize GetPPI() const override;

    virtual wxSize FromDIP(const wxSize& sz) const override;

    virtual wxSize ToDIP(const wxSize& sz) const override;

    virtual void ComputeScaleAndOrigin() override;

    void SetBackgroundMode(int WXUNUSED(mode)) override { }
#if wxUSE_PALETTE
    void SetPalette(const wxPalette& WXUNUSED(palette)) override { }
#endif

    void SetPrintData(const wxPrintData& data);
    wxPrintData& GetPrintData() { return m_printData; }

    virtual int GetDepth() const override { return 24; }

    void PsPrint( const wxString& psdata );

    // Overridden for wxPrinterDC Impl

    virtual int GetResolution() const override;
    virtual wxRect GetPaperRect() const override;

    virtual void* GetHandle() const override { return nullptr; }

protected:
    bool DoFloodFill(wxCoord x1, wxCoord y1, const wxColour &col,
                     wxFloodFillStyle style = wxFLOOD_SURFACE) override;
    bool DoGetPixel(wxCoord x1, wxCoord y1, wxColour *col) const override;
    void DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2) override;
    void DoCrossHair(wxCoord x, wxCoord y) override ;
    void DoDrawArc(wxCoord x1,wxCoord y1,wxCoord x2,wxCoord y2,wxCoord xc,wxCoord yc) override;
    void DoDrawEllipticArc(wxCoord x,wxCoord y,wxCoord w,wxCoord h,double sa,double ea) override;
    void DoDrawPoint(wxCoord x, wxCoord y) override;
    void DoDrawLines(int n, const wxPoint points[], wxCoord xoffset = 0, wxCoord yoffset = 0) override;
    void DoDrawPolygon(int n, const wxPoint points[],
                       wxCoord xoffset = 0, wxCoord yoffset = 0,
                       wxPolygonFillMode fillStyle = wxODDEVEN_RULE) override;
    void DoDrawPolyPolygon(int n, const int count[], const wxPoint points[],
                           wxCoord xoffset = 0, wxCoord yoffset = 0,
                           wxPolygonFillMode fillStyle = wxODDEVEN_RULE) override;
    void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height) override;
    void DoDrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius = 20) override;
    void DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height) override;
#if wxUSE_SPLINES
    void DoDrawSpline(const wxPointList *points) override;
#endif
    bool DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
                wxDC *source, wxCoord xsrc, wxCoord ysrc,
                wxRasterOperationMode rop = wxCOPY, bool useMask = false,
                wxCoord xsrcMask = wxDefaultCoord, wxCoord ysrcMask = wxDefaultCoord) override;
    void DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y) override;
    void DoDrawBitmap(const wxBitmap& bitmap, wxCoord x, wxCoord y, bool useMask = false) override;
    void DoDrawText(const wxString& text, wxCoord x, wxCoord y) override;
    void DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y, double angle) override;
    void DoSetClippingRegion(wxCoord x, wxCoord y, wxCoord width, wxCoord height) override;
    void DoSetDeviceClippingRegion( const wxRegion &WXUNUSED(clip)) override
    {
        wxFAIL_MSG( "not implemented" );
    }
    void DoGetTextExtent(const wxString& string, wxCoord *x, wxCoord *y,
                         wxCoord *descent = nullptr,
                         wxCoord *externalLeading = nullptr,
                         const wxFont *theFont = nullptr) const override;
    void DoGetSize(int* width, int* height) const override;
    void DoGetSizeMM(int *width, int *height) const override;

    // Common part of DoDrawText() and DoDrawRotatedText()
    void DrawAnyText(const wxWX2MBbuf& textbuf, wxCoord testDescent, double lineHeight);
    // Actually set PostScript font
    void SetPSFont();
    // Set PostScript color
    void SetPSColour(const wxColour& col);

    FILE*             m_pstream;    // PostScript output stream
    unsigned char     m_currentRed;
    unsigned char     m_currentGreen;
    unsigned char     m_currentBlue;
    int               m_pageNumber;
    bool              m_clipping;
    mutable double    m_underlinePosition;
    mutable double    m_underlineThickness;
    wxPrintData       m_printData;
    double            m_pageHeight;
    wxArrayString     m_definedPSFonts;
    bool              m_isFontChanged;

private:
    wxDECLARE_DYNAMIC_CLASS(wxPostScriptDCImpl);
};

#endif
    // wxUSE_POSTSCRIPT && wxUSE_PRINTING_ARCHITECTURE

#endif
        // _WX_DCPSG_H_
