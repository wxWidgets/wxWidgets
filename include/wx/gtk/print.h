/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/print.h
// Author:      Anthony Bretaudeau
// Purpose:     GTK printing support
// Created:     2007-08-25
// Copyright:   (c) Anthony Bretaudeau
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_PRINT_H_
#define _WX_GTK_PRINT_H_

#include "wx/defs.h"

#if wxUSE_GTKPRINT

#include "wx/print.h"
#include "wx/printdlg.h"
#include "wx/prntbase.h"
#include "wx/dc.h"

typedef struct _GtkPrintOperation GtkPrintOperation;
typedef struct _GtkPrintContext GtkPrintContext;
typedef struct _GtkPrintSettings GtkPrintSettings;
typedef struct _GtkPageSetup GtkPageSetup;

typedef struct _cairo cairo_t;

//----------------------------------------------------------------------------
// wxGtkPrintFactory
//----------------------------------------------------------------------------

class wxGtkPrintFactory: public wxPrintFactory
{
public:
    virtual wxPrinterBase *CreatePrinter( wxPrintDialogData *data ) override;

    virtual wxPrintPreviewBase *CreatePrintPreview( wxPrintout *preview,
                                                    wxPrintout *printout = nullptr,
                                                    wxPrintDialogData *data = nullptr ) override;
    virtual wxPrintPreviewBase *CreatePrintPreview( wxPrintout *preview,
                                                    wxPrintout *printout,
                                                    wxPrintData *data ) override;

    virtual wxPrintDialogBase *CreatePrintDialog( wxWindow *parent,
                                                  wxPrintDialogData *data = nullptr ) override;
    virtual wxPrintDialogBase *CreatePrintDialog( wxWindow *parent,
                                                  wxPrintData *data ) override;

    virtual wxPageSetupDialogBase *CreatePageSetupDialog( wxWindow *parent,
                                                          wxPageSetupDialogData * data = nullptr ) override;

    virtual wxDCImpl* CreatePrinterDCImpl( wxPrinterDC *owner, const wxPrintData& data ) override;

    virtual bool HasPrintSetupDialog() override;
    virtual wxDialog *CreatePrintSetupDialog( wxWindow *parent, wxPrintData *data ) override;
    virtual bool HasOwnPrintToFile() override;
    virtual bool HasPrinterLine() override;
    virtual wxString CreatePrinterLine() override;
    virtual bool HasStatusLine() override;
    virtual wxString CreateStatusLine() override;

    virtual wxPrintNativeDataBase *CreatePrintNativeData() override;
};

//----------------------------------------------------------------------------
// wxGtkPrintDialog
//----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGtkPrintDialog: public wxPrintDialogBase
{
public:
    wxGtkPrintDialog( wxWindow *parent,
                         wxPrintDialogData* data = nullptr );
    wxGtkPrintDialog( wxWindow *parent, wxPrintData* data);
    virtual ~wxGtkPrintDialog();

    wxPrintData& GetPrintData() override
        { return m_printDialogData.GetPrintData(); }
    wxPrintDialogData& GetPrintDialogData() override
        { return m_printDialogData; }

    wxDC *GetPrintDC() override;

    virtual int ShowModal() override;

    virtual bool Validate() override { return true; }
    virtual bool TransferDataToWindow() override { return true; }
    virtual bool TransferDataFromWindow() override { return true; }

    void SetShowDialog(bool show) { m_showDialog = show; }
    bool GetShowDialog() { return m_showDialog; }

protected:
    // Implement some base class methods to do nothing to avoid asserts and
    // GTK warnings, since this is not a real wxDialog.
    virtual void DoSetSize(int WXUNUSED(x), int WXUNUSED(y),
                           int WXUNUSED(width), int WXUNUSED(height),
                           int WXUNUSED(sizeFlags) = wxSIZE_AUTO) override {}
    virtual void DoMoveWindow(int WXUNUSED(x), int WXUNUSED(y),
                              int WXUNUSED(width), int WXUNUSED(height)) override {}

private:
    wxPrintDialogData    m_printDialogData;
    wxWindow            *m_parent;
    bool                 m_showDialog;

    wxDECLARE_DYNAMIC_CLASS(wxGtkPrintDialog);
};

//----------------------------------------------------------------------------
// wxGtkPageSetupDialog
//----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGtkPageSetupDialog: public wxPageSetupDialogBase
{
public:
    wxGtkPageSetupDialog( wxWindow *parent,
                            wxPageSetupDialogData* data = nullptr );
    virtual ~wxGtkPageSetupDialog();

    virtual wxPageSetupDialogData& GetPageSetupDialogData() override { return m_pageDialogData; }

    virtual int ShowModal() override;

    virtual bool Validate() override { return true; }
    virtual bool TransferDataToWindow() override { return true; }
    virtual bool TransferDataFromWindow() override { return true; }

protected:
    // Implement some base class methods to do nothing to avoid asserts and
    // GTK warnings, since this is not a real wxDialog.
    virtual void DoSetSize(int WXUNUSED(x), int WXUNUSED(y),
                           int WXUNUSED(width), int WXUNUSED(height),
                           int WXUNUSED(sizeFlags) = wxSIZE_AUTO) override {}
    virtual void DoMoveWindow(int WXUNUSED(x), int WXUNUSED(y),
                              int WXUNUSED(width), int WXUNUSED(height)) override {}

private:
    wxPageSetupDialogData    m_pageDialogData;
    wxWindow                *m_parent;

    wxDECLARE_DYNAMIC_CLASS(wxGtkPageSetupDialog);
};

//----------------------------------------------------------------------------
// wxGtkPrinter
//----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGtkPrinter : public wxPrinterBase
{
public:
    wxGtkPrinter(wxPrintDialogData *data = nullptr);
    virtual ~wxGtkPrinter();

    virtual bool Print(wxWindow *parent,
                       wxPrintout *printout,
                       bool prompt = true) override;
    virtual wxDC* PrintDialog(wxWindow *parent) override;
    virtual bool Setup(wxWindow *parent) override;

    GtkPrintContext *GetPrintContext() { return m_gpc; }
    void SetPrintContext(GtkPrintContext *context) {m_gpc = context;}
    void BeginPrint(wxPrintout *printout, GtkPrintOperation *operation, GtkPrintContext *context);
    void DrawPage(wxPrintout *printout, GtkPrintOperation *operation, GtkPrintContext *context, int page_nr);

private:
    GtkPrintContext *m_gpc;
    wxDC            *m_dc;

    wxDECLARE_DYNAMIC_CLASS(wxGtkPrinter);
    wxDECLARE_NO_COPY_CLASS(wxGtkPrinter);
};

//----------------------------------------------------------------------------
// wxGtkPrintNativeData
//----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGtkPrintNativeData : public wxPrintNativeDataBase
{
public:
    wxGtkPrintNativeData();
    virtual ~wxGtkPrintNativeData();

    virtual bool TransferTo( wxPrintData &data ) override;
    virtual bool TransferFrom( const wxPrintData &data ) override;

    virtual bool Ok() const override { return IsOk(); }
    virtual bool IsOk() const override { return true; }

    GtkPrintSettings* GetPrintConfig() { return m_config; }
    void SetPrintConfig( GtkPrintSettings * config );

    GtkPrintOperation* GetPrintJob() { return m_job; }
    void SetPrintJob(GtkPrintOperation *job);

    GtkPrintContext *GetPrintContext() { return m_context; }
    void SetPrintContext(GtkPrintContext *context) {m_context = context; }

private:
    // NB: m_config is created and owned by us, but the other objects are not
    //     and their accessors don't change their ref count.
    GtkPrintSettings    *m_config;
    GtkPrintOperation   *m_job;
    GtkPrintContext     *m_context;

    wxDECLARE_DYNAMIC_CLASS(wxGtkPrintNativeData);
};

//-----------------------------------------------------------------------------
// wxGtkPrinterDC
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGtkPrinterDCImpl : public wxDCImpl
{
public:
    wxGtkPrinterDCImpl( wxPrinterDC *owner, const wxPrintData& data );
    virtual ~wxGtkPrinterDCImpl();

    bool Ok() const { return IsOk(); }
    bool IsOk() const override;

    virtual void* GetCairoContext() const override;
    virtual void* GetHandle() const override;

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
    wxSize GetPPI() const override;
    virtual int GetDepth() const override { return 24; }
    void SetBackgroundMode(int mode) override;
#if wxUSE_PALETTE
    void SetPalette(const wxPalette& WXUNUSED(palette)) override { }
#endif
    void SetResolution(int ppi);

    // overridden for wxPrinterDC Impl
    virtual int GetResolution() const override;
    virtual wxRect GetPaperRect() const override;

protected:
    bool DoFloodFill(wxCoord x1, wxCoord y1, const wxColour &col,
                     wxFloodFillStyle style=wxFLOOD_SURFACE ) override;
    void DoGradientFillConcentric(const wxRect& rect, const wxColour& initialColour, const wxColour& destColour, const wxPoint& circleCenter) override;
    void DoGradientFillLinear(const wxRect& rect, const wxColour& initialColour, const wxColour& destColour, wxDirection nDirection = wxEAST) override;
    bool DoGetPixel(wxCoord x1, wxCoord y1, wxColour *col) const override;
    void DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2) override;
    void DoCrossHair(wxCoord x, wxCoord y) override;
    void DoDrawArc(wxCoord x1,wxCoord y1,wxCoord x2,wxCoord y2,wxCoord xc,wxCoord yc) override;
    void DoDrawEllipticArc(wxCoord x,wxCoord y,wxCoord w,wxCoord h,double sa,double ea) override;
    void DoDrawPoint(wxCoord x, wxCoord y) override;
    void DoDrawLines(int n, const wxPoint points[], wxCoord xoffset = 0, wxCoord yoffset = 0) override;
    void DoDrawPolygon(int n, const wxPoint points[], wxCoord xoffset = 0, wxCoord yoffset = 0, wxPolygonFillMode fillStyle=wxODDEVEN_RULE) override;
    void DoDrawPolyPolygon(int n, const int count[], const wxPoint points[], wxCoord xoffset = 0, wxCoord yoffset = 0, wxPolygonFillMode fillStyle=wxODDEVEN_RULE) override;
    void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height) override;
    void DoDrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius = 20.0) override;
    void DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height) override;
#if wxUSE_SPLINES
    void DoDrawSpline(const wxPointList *points) override;
#endif
    bool DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
            wxDC *source, wxCoord xsrc, wxCoord ysrc,
            wxRasterOperationMode rop = wxCOPY, bool useMask = false,
            wxCoord xsrcMask = wxDefaultCoord, wxCoord ysrcMask = wxDefaultCoord) override;
    void DoDrawIcon( const wxIcon& icon, wxCoord x, wxCoord y ) override;
    void DoDrawBitmap( const wxBitmap& bitmap, wxCoord x, wxCoord y, bool useMask = false  ) override;
    void DoDrawText(const wxString& text, wxCoord x, wxCoord y ) override;
    void DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y, double angle) override;
    void DoSetClippingRegion(wxCoord x, wxCoord y, wxCoord width, wxCoord height) override;
    void DoSetDeviceClippingRegion( const wxRegion &WXUNUSED(clip) ) override
    {
        wxFAIL_MSG( "not implemented" );
    }
    void DoGetTextExtent(const wxString& string, wxCoord *x, wxCoord *y,
                     wxCoord *descent = nullptr,
                     wxCoord *externalLeading = nullptr,
                     const wxFont *theFont = nullptr ) const override;
    bool DoGetPartialTextExtents(const wxString& text, wxArrayInt& widths) const override;
    void DoGetSize(int* width, int* height) const override;
    void DoGetSizeMM(int *width, int *height) const override;

    wxPrintData& GetPrintData() { return m_printData; }
    void SetPrintData(const wxPrintData& data);

private:
    // Call cairo_set_source_rgba() if necessary.
    void SetSourceColour(const wxColour& col);

    wxPrintData             m_printData;
    PangoContext           *m_context;
    PangoLayout            *m_layout;
    PangoFontDescription   *m_fontdesc;
    cairo_t                *m_cairo;

    // Last colour passed to cairo_set_source_rgba().
    mutable wxColour        m_currentSourceColour;

    GtkPrintContext        *m_gpc;
    int                     m_resolution;
    double                  m_PS2DEV;
    double                  m_DEV2PS;

    friend class wxGtkPrinterDCContextSaver;

    wxDECLARE_DYNAMIC_CLASS(wxGtkPrinterDCImpl);
    wxDECLARE_NO_COPY_CLASS(wxGtkPrinterDCImpl);
};

// ----------------------------------------------------------------------------
// wxGtkPrintPreview: programmer creates an object of this class to preview a
// wxPrintout.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGtkPrintPreview : public wxPrintPreviewBase
{
public:
    wxGtkPrintPreview(wxPrintout *printout,
                             wxPrintout *printoutForPrinting = nullptr,
                             wxPrintDialogData *data = nullptr);
    wxGtkPrintPreview(wxPrintout *printout,
                             wxPrintout *printoutForPrinting,
                             wxPrintData *data);

    virtual ~wxGtkPrintPreview();

    virtual bool Print(bool interactive) override;
    virtual void DetermineScaling() override;

private:
    void Init(wxPrintout *printout,
              wxPrintout *printoutForPrinting,
              wxPrintData *data);

    // resolution to use in DPI
    int m_resolution;

    wxDECLARE_CLASS(wxGtkPrintPreview);
};

#endif // wxUSE_GTKPRINT

#endif // _WX_GTK_PRINT_H_
