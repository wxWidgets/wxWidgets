/////////////////////////////////////////////////////////////////////////////
// Name:        prntbase.h
// Purpose:     Base classes for printing framework
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __PRNTBASEH__
#define __PRNTBASEH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/event.h"
#include "wx/cmndata.h"
#include "wx/panel.h"
#include "wx/scrolwin.h"
#include "wx/dialog.h"

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

 protected:
  wxPrintData printData;
  wxPrintout *currentPrintout;
 public:
  static  wxWindow *abortWindow;
  static  bool abortIt;

  wxPrinterBase(wxPrintData *data = NULL);
  ~wxPrinterBase(void);

  virtual wxWindow *CreateAbortWindow(wxWindow *parent, wxPrintout *printout);
  virtual void ReportError(wxWindow *parent, wxPrintout *printout, char *message);
  inline wxPrintData& GetPrintData(void) { return printData; };
  inline bool GetAbort(void) { return abortIt; }

  ///////////////////////////////////////////////////////////////////////////
  // OVERRIDES

  virtual bool Setup(wxWindow *parent) = 0;
  virtual bool Print(wxWindow *parent, wxPrintout *printout, bool prompt = TRUE) = 0;
  virtual bool PrintDialog(wxWindow *parent) = 0;
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

 private:
   char *printoutTitle;
   wxDC *printoutDC;

   int pageWidthPixels;
   int pageHeightPixels;

   int pageWidthMM;
   int pageHeightMM;

   int PPIScreenX;
   int PPIScreenY;
   int PPIPrinterX;
   int PPIPrinterY;

   bool isPreview;
 public:
  wxPrintout(char *title = "Printout");
  ~wxPrintout(void);

  virtual bool OnBeginDocument(int startPage, int endPage);
  virtual void OnEndDocument(void);
  virtual void OnBeginPrinting(void);
  virtual void OnEndPrinting(void);

  // Guaranteed to be before any other functions are called
  inline virtual void OnPreparePrinting(void) { }

  virtual bool HasPage(int page);
  virtual bool OnPrintPage(int page) = 0;
  virtual void GetPageInfo(int *minPage, int *maxPage, int *pageFrom, int *pageTo);

  inline virtual char *GetTitle(void) { return printoutTitle; }

  inline wxDC *GetDC(void) { return printoutDC; }
  inline void SetDC(wxDC *dc) { printoutDC = dc; }
  inline void SetPageSizePixels(int w, int  h) { pageWidthPixels = w; pageHeightPixels = h; }
  inline void GetPageSizePixels(int *w, int  *h) { *w = pageWidthPixels; *h = pageHeightPixels; }
  inline void SetPageSizeMM(int w, int  h) { pageWidthMM = w; pageHeightMM = h; }
  inline void GetPageSizeMM(int *w, int  *h) { *w = pageWidthMM; *h = pageHeightMM; }

  inline void SetPPIScreen(int x, int y) { PPIScreenX = x; PPIScreenY = y; }
  inline void GetPPIScreen(int *x, int *y) { *x = PPIScreenX; *y = PPIScreenY; }
  inline void SetPPIPrinter(int x, int y) { PPIPrinterX = x; PPIPrinterY = y; }
  inline void GetPPIPrinter(int *x, int *y) { *x = PPIPrinterX; *y = PPIPrinterY; }

  inline virtual bool IsPreview(void) { return isPreview; }

  inline virtual void SetIsPreview(bool p) { isPreview = p; }
};

/*
 * wxPreviewCanvas
 * Canvas upon which a preview is drawn.
 */
 
class WXDLLEXPORT wxPreviewCanvas: public wxScrolledWindow
{
  DECLARE_CLASS(wxPreviewCanvas)

 private:
  wxPrintPreviewBase *printPreview;
 public:
  wxPreviewCanvas(wxPrintPreviewBase *preview, wxWindow *parent,
    const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
    long style = 0, const wxString& name = "canvas");
  ~wxPreviewCanvas(void);

  void OnPaint(wxPaintEvent& event);

  // Responds to colour changes
  void OnSysColourChanged(wxSysColourChangedEvent& event);

DECLARE_EVENT_TABLE()
};

/*
 * wxPreviewFrame
 * Default frame for showing preview.
 */

class WXDLLEXPORT wxPreviewFrame: public wxFrame
{
  DECLARE_CLASS(wxPreviewFrame)

 protected:
  wxWindow *previewCanvas;
  wxPreviewControlBar *controlBar;
  wxPrintPreviewBase *printPreview;
 public:
  wxPreviewFrame(wxPrintPreviewBase *preview, wxFrame *parent, const wxString& title = "Print Preview",
    const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
    long style = wxDEFAULT_FRAME, const wxString& name = "frame");
  ~wxPreviewFrame(void);

  bool OnClose(void);
  virtual void Initialize(void);
  virtual void CreateCanvas(void);
  virtual void CreateControlBar(void);
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

 protected:
  wxPrintPreviewBase *printPreview;
  wxButton *closeButton;
  wxButton *nextPageButton;
  wxButton *previousPageButton;
  wxButton *printButton;
  wxChoice *zoomControl;
  static wxFont *buttonFont;
  long buttonFlags;
 public:
  wxPreviewControlBar(wxPrintPreviewBase *preview, long buttons,
    wxWindow *parent, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
    long style = 0, const wxString& name = "panel");
  ~wxPreviewControlBar(void);

  virtual void CreateButtons(void);
  virtual void SetZoomControl(int zoom);
  virtual int GetZoomControl(void);
  inline virtual wxPrintPreviewBase *GetPrintPreview(void) { return printPreview; }

  void OnPrint(wxCommandEvent& event);
  void OnClose(wxCommandEvent& event);
  void OnNext(wxCommandEvent& event);
  void OnPrevious(wxCommandEvent& event);
  void OnZoom(wxCommandEvent& event);
  void OnPaint(wxPaintEvent& event);

DECLARE_EVENT_TABLE()
};

/*
 * wxPrintPreview
 * Programmer creates an object of this class to preview a wxPrintout.
 */
 
class WXDLLEXPORT wxPrintPreviewBase: public wxObject
{
  DECLARE_CLASS(wxPrintPreviewBase)

 protected:
  wxPrintData printData;
  wxWindow *previewCanvas;
  wxFrame *previewFrame;
  wxBitmap *previewBitmap;
  wxPrintout *previewPrintout;
  wxPrintout *printPrintout;
  int currentPage;
  int currentZoom;
  float previewScale;
  int topMargin;
  int leftMargin;
  int pageWidth;
  int pageHeight;
  int minPage;
  int maxPage;
 protected:
  bool isOk;
 public:
  wxPrintPreviewBase(wxPrintout *printout, wxPrintout *printoutForPrinting = NULL, wxPrintData *data = NULL);
  ~wxPrintPreviewBase(void);

  virtual bool SetCurrentPage(int pageNum);
  inline int GetCurrentPage(void) { return currentPage; };

  inline void SetPrintout(wxPrintout *printout) { previewPrintout = printout; };
  inline wxPrintout *GetPrintout(void) { return previewPrintout; };
  inline wxPrintout *GetPrintoutForPrinting(void) { return printPrintout; };

  inline void SetFrame(wxFrame *frame) { previewFrame = frame; };
  inline void SetCanvas(wxWindow *canvas) { previewCanvas = canvas; };

  inline virtual wxFrame *GetFrame(void) { return previewFrame; }
  inline virtual wxWindow *GetCanvas(void) { return previewCanvas; }

  // The preview canvas should call this from OnPaint
  virtual bool PaintPage(wxWindow *canvas, wxDC& dc);

  // This draws a blank page onto the preview canvas
  virtual bool DrawBlankPage(wxWindow *canvas, wxDC& dc);

  // This is called by wxPrintPreview to render a page into
  // a wxMemoryDC.
  virtual bool RenderPage(int pageNum);

  inline wxPrintData& GetPrintData(void) { return printData; }

  virtual void SetZoom(int percent);
  int GetZoom(void) { return currentZoom; };

  inline int GetMaxPage(void) { return maxPage; }
  inline int GetMinPage(void) { return minPage; }
  
  inline bool Ok(void) { return isOk; }
  inline void SetOk(bool ok) { isOk = ok; }

  ///////////////////////////////////////////////////////////////////////////
  // OVERRIDES

  // If we own a wxPrintout that can be used for printing, this
  // will invoke the actual printing procedure. Called
  // by the wxPreviewControlBar.
  virtual bool Print(bool interactive) = 0;

  // Calculate scaling that needs to be done to get roughly
  // the right scaling for the screen pretending to be
  // the currently selected printer.
  virtual void DetermineScaling(void) = 0;
};

/*
 * Abort dialog
 */

class WXDLLEXPORT wxPrintAbortDialog: public wxDialog
{
public:
    void OnCancel(wxCommandEvent& event);

  wxPrintAbortDialog(wxWindow *parent,
    const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
    long style = 0, const wxString& name = "dialog"):
   wxDialog(parent, -1, title, pos, size, style, name)
  {
  }

  DECLARE_EVENT_TABLE()
};

#endif
    // __PRNTBASEH__
