/////////////////////////////////////////////////////////////////////////////
// Name:        prntbase.h
// Purpose:     Base classes for printing framework
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRNTBASEH__
#define _WX_PRNTBASEH__

#ifdef __GNUG__
    #pragma interface "prntbase.h"
#endif

#include "wx/defs.h"
#include "wx/event.h"
#include "wx/cmndata.h"
#include "wx/panel.h"
#include "wx/scrolwin.h"
#include "wx/dialog.h"
#include "wx/frame.h"

class WXDLLEXPORT wxDC;
class WXDLLEXPORT wxButton;
class WXDLLEXPORT wxChoice;
class WXDLLEXPORT wxPrintout;
class WXDLLEXPORT wxPrinterBase;
class WXDLLEXPORT wxPrintDialog;
class WXDLLEXPORT wxPrintPreviewBase;
class WXDLLEXPORT wxPreviewCanvas;
class WXDLLEXPORT wxPreviewControlBar;
class WXDLLEXPORT wxPreviewFrame;

/*
 * Represents the printer: manages printing a wxPrintout object
 */

class WXDLLEXPORT wxPrinterBase: public wxObject
{
    DECLARE_CLASS(wxPrinterBase)

public:
    wxPrinterBase(wxPrintDialogData *data = (wxPrintDialogData *) NULL);
    virtual ~wxPrinterBase();

    virtual wxWindow *CreateAbortWindow(wxWindow *parent, wxPrintout *printout);
    virtual void ReportError(wxWindow *parent, wxPrintout *printout, char *message);

    wxPrintDialogData& GetPrintDialogData() const
        { return (wxPrintDialogData&) m_printDialogData; }
    bool GetAbort() const { return sm_abortIt; }

    ///////////////////////////////////////////////////////////////////////////
    // OVERRIDES

    virtual bool Setup(wxWindow *parent) = 0;
    virtual bool Print(wxWindow *parent, wxPrintout *printout, bool prompt = TRUE) = 0;
    virtual wxDC* PrintDialog(wxWindow *parent) = 0;

protected:
    wxPrintDialogData     m_printDialogData;
    wxPrintout*           m_currentPrintout;

public:
    static wxWindow*      sm_abortWindow;
    static bool           sm_abortIt;

};

/*
 * wxPrintout
 * Represents an object via which a document may be printed.
 * The programmer derives from this, overrides (at least) OnPrintPage,
 * and passes it to a wxPrinter object for printing, or a wxPrintPreview
 * object for previewing.
 */

class WXDLLEXPORT wxPrintout: public wxObject
{
DECLARE_ABSTRACT_CLASS(wxPrintout)

public:
    wxPrintout(const wxString& title = "Printout");
    virtual ~wxPrintout();

    virtual bool OnBeginDocument(int startPage, int endPage);
    virtual void OnEndDocument();
    virtual void OnBeginPrinting();
    virtual void OnEndPrinting();

    // Guaranteed to be before any other functions are called
    virtual void OnPreparePrinting() { }

    virtual bool HasPage(int page);
    virtual bool OnPrintPage(int page) = 0;
    virtual void GetPageInfo(int *minPage, int *maxPage, int *pageFrom, int *pageTo);

    virtual wxString GetTitle() const { return m_printoutTitle; }

    wxDC *GetDC() const { return m_printoutDC; }
    void SetDC(wxDC *dc) { m_printoutDC = dc; }
    void SetPageSizePixels(int w, int  h) { m_pageWidthPixels = w; m_pageHeightPixels = h; }
    void GetPageSizePixels(int *w, int  *h) const { *w = m_pageWidthPixels; *h = m_pageHeightPixels; }
    void SetPageSizeMM(int w, int  h) { m_pageWidthMM = w; m_pageHeightMM = h; }
    void GetPageSizeMM(int *w, int  *h) const { *w = m_pageWidthMM; *h = m_pageHeightMM; }

    void SetPPIScreen(int x, int y) { m_PPIScreenX = x; m_PPIScreenY = y; }
    void GetPPIScreen(int *x, int *y) const { *x = m_PPIScreenX; *y = m_PPIScreenY; }
    void SetPPIPrinter(int x, int y) { m_PPIPrinterX = x; m_PPIPrinterY = y; }
    void GetPPIPrinter(int *x, int *y) const { *x = m_PPIPrinterX; *y = m_PPIPrinterY; }

    virtual bool IsPreview() const { return m_isPreview; }

    virtual void SetIsPreview(bool p) { m_isPreview = p; }

private:
    wxString         m_printoutTitle;
    wxDC*            m_printoutDC;

    int              m_pageWidthPixels;
    int              m_pageHeightPixels;

    int              m_pageWidthMM;
    int              m_pageHeightMM;

    int              m_PPIScreenX;
    int              m_PPIScreenY;
    int              m_PPIPrinterX;
    int              m_PPIPrinterY;

    bool             m_isPreview;
};

/*
 * wxPreviewCanvas
 * Canvas upon which a preview is drawn.
 */

class WXDLLEXPORT wxPreviewCanvas: public wxScrolledWindow
{
    DECLARE_CLASS(wxPreviewCanvas)

public:
    wxPreviewCanvas(wxPrintPreviewBase *preview,
                    wxWindow *parent,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0,
                    const wxString& name = "canvas");
    ~wxPreviewCanvas();

    void OnPaint(wxPaintEvent& event);

    // Responds to colour changes
    void OnSysColourChanged(wxSysColourChangedEvent& event);

private:
    wxPrintPreviewBase* m_printPreview;

    DECLARE_EVENT_TABLE()
};

/*
 * wxPreviewFrame
 * Default frame for showing preview.
 */

class WXDLLEXPORT wxPreviewFrame: public wxFrame
{
    DECLARE_CLASS(wxPreviewFrame)

public:
    wxPreviewFrame(wxPrintPreviewBase *preview,
                   wxFrame *parent,
                   const wxString& title = "Print Preview",
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxDEFAULT_FRAME_STYLE,
                   const wxString& name = "frame");
    ~wxPreviewFrame();

    void OnCloseWindow(wxCloseEvent& event);
    virtual void Initialize();
    virtual void CreateCanvas();
    virtual void CreateControlBar();

protected:
    wxWindow*             m_previewCanvas;
    wxPreviewControlBar*  m_controlBar;
    wxPrintPreviewBase*   m_printPreview;

private:
    DECLARE_EVENT_TABLE()
};

/*
 * wxPreviewControlBar
 * A panel with buttons for controlling a print preview.
 * The programmer may wish to use other means for controlling
 * the print preview.
 */

#define wxPREVIEW_PRINT        1
#define wxPREVIEW_PREVIOUS     2
#define wxPREVIEW_NEXT         4
#define wxPREVIEW_ZOOM         8

#define wxPREVIEW_DEFAULT      wxPREVIEW_PREVIOUS|wxPREVIEW_NEXT|wxPREVIEW_ZOOM

// Ids for controls
#define wxID_PREVIEW_CLOSE      1
#define wxID_PREVIEW_NEXT       2
#define wxID_PREVIEW_PREVIOUS   3
#define wxID_PREVIEW_PRINT      4
#define wxID_PREVIEW_ZOOM       5

class WXDLLEXPORT wxPreviewControlBar: public wxPanel
{
    DECLARE_CLASS(wxPreviewControlBar)

public:
    wxPreviewControlBar(wxPrintPreviewBase *preview,
                        long buttons,
                        wxWindow *parent,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxString& name = "panel");
    ~wxPreviewControlBar();

    virtual void CreateButtons();
    virtual void SetZoomControl(int zoom);
    virtual int GetZoomControl();
    virtual wxPrintPreviewBase *GetPrintPreview() const
        { return m_printPreview; }

    void OnPrint(wxCommandEvent& event);
    void OnWindowClose(wxCommandEvent& event);
    void OnNext(wxCommandEvent& event);
    void OnPrevious(wxCommandEvent& event);
    void OnZoom(wxCommandEvent& event);
    void OnPaint(wxPaintEvent& event);

protected:
    wxPrintPreviewBase*   m_printPreview;
    wxButton*             m_closeButton;
    wxButton*             m_nextPageButton;
    wxButton*             m_previousPageButton;
    wxButton*             m_printButton;
    wxChoice*             m_zoomControl;
    long                  m_buttonFlags;

private:
    DECLARE_EVENT_TABLE()
};

/*
 * wxPrintPreview
 * Programmer creates an object of this class to preview a wxPrintout.
 */

class WXDLLEXPORT wxPrintPreviewBase: public wxObject
{
    DECLARE_CLASS(wxPrintPreviewBase)

public:
    wxPrintPreviewBase(wxPrintout *printout,
                       wxPrintout *printoutForPrinting = (wxPrintout *) NULL,
                       wxPrintDialogData *data = (wxPrintDialogData *) NULL);
    wxPrintPreviewBase(wxPrintout *printout,
                       wxPrintout *printoutForPrinting,
                       wxPrintData *data);
    virtual ~wxPrintPreviewBase();

    virtual bool SetCurrentPage(int pageNum);
    int GetCurrentPage() const { return m_currentPage; };

    void SetPrintout(wxPrintout *printout) { m_previewPrintout = printout; };
    wxPrintout *GetPrintout() const { return m_previewPrintout; };
    wxPrintout *GetPrintoutForPrinting() const { return m_printPrintout; };

    void SetFrame(wxFrame *frame) { m_previewFrame = frame; };
    void SetCanvas(wxWindow *canvas) { m_previewCanvas = canvas; };

    virtual wxFrame *GetFrame() const { return m_previewFrame; }
    virtual wxWindow *GetCanvas() const { return m_previewCanvas; }

    // The preview canvas should call this from OnPaint
    virtual bool PaintPage(wxWindow *canvas, wxDC& dc);

    // This draws a blank page onto the preview canvas
    virtual bool DrawBlankPage(wxWindow *canvas, wxDC& dc);

    // This is called by wxPrintPreview to render a page into a wxMemoryDC.
    virtual bool RenderPage(int pageNum);

    wxPrintDialogData& GetPrintDialogData() { return m_printDialogData; }

    virtual void SetZoom(int percent);
    int GetZoom() const { return m_currentZoom; };

    int GetMaxPage() const { return m_maxPage; }
    int GetMinPage() const { return m_minPage; }

    bool Ok() const { return m_isOk; }
    void SetOk(bool ok) { m_isOk = ok; }

    ///////////////////////////////////////////////////////////////////////////
    // OVERRIDES

    // If we own a wxPrintout that can be used for printing, this
    // will invoke the actual printing procedure. Called
    // by the wxPreviewControlBar.
    virtual bool Print(bool interactive) = 0;

    // Calculate scaling that needs to be done to get roughly
    // the right scaling for the screen pretending to be
    // the currently selected printer.
    virtual void DetermineScaling() = 0;

protected:
    wxPrintDialogData m_printDialogData;
    wxWindow*         m_previewCanvas;
    wxFrame*          m_previewFrame;
    wxBitmap*         m_previewBitmap;
    wxPrintout*       m_previewPrintout;
    wxPrintout*       m_printPrintout;
    int               m_currentPage;
    int               m_currentZoom;
    float             m_previewScale;
    int               m_topMargin;
    int               m_leftMargin;
    int               m_pageWidth;
    int               m_pageHeight;
    int               m_minPage;
    int               m_maxPage;

    bool              m_isOk;

private:
    void Init(wxPrintout *printout, wxPrintout *printoutForPrinting);
};

/*
 * Abort dialog
 */

class WXDLLEXPORT wxPrintAbortDialog: public wxDialog
{
public:
    wxPrintAbortDialog(wxWindow *parent,
                       const wxString& title,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = 0,
                       const wxString& name = "dialog")
        : wxDialog(parent, -1, title, pos, size, style, name)
        {
        }

    void OnCancel(wxCommandEvent& event);

private:
    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_PRNTBASEH__
