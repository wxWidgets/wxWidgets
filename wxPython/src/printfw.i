/////////////////////////////////////////////////////////////////////////////
// Name:        printfw.i
// Purpose:     Printing Framework classes
//
// Author:      Robin Dunn
//
// Created:     7-May-1999
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module printfw

%{
#include "helpers.h"
#include <wx/print.h>
#include <wx/printdlg.h>
#include <wx/dcps.h>

#include "printfw.h"
%}

//----------------------------------------------------------------------

%{
    // Put some wx default wxChar* values into wxStrings.
    static const wxChar* wxPrintoutTitleStr = wxT("Printout");
    DECLARE_DEF_STRING(PrintoutTitleStr);
    static const wxChar* wxPreviewCanvasNameStr = wxT("previewcanvas");
    DECLARE_DEF_STRING(PreviewCanvasNameStr);

    DECLARE_DEF_STRING(FrameNameStr);
    DECLARE_DEF_STRING(PanelNameStr);
    DECLARE_DEF_STRING(DialogNameStr);

%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import misc.i
%import windows.i
%import gdi.i
%import cmndlgs.i
%import frames.i


%pragma(python) code = "import wx"


//----------------------------------------------------------------------

enum wxPrintMode
{
    wxPRINT_MODE_NONE =    0,
    wxPRINT_MODE_PREVIEW = 1,   // Preview in external application
    wxPRINT_MODE_FILE =    2,   // Print to file
    wxPRINT_MODE_PRINTER = 3    // Send to printer
};



class wxPrintData : public wxObject {
public:
    wxPrintData();
    ~wxPrintData();

    int GetNoCopies();
    bool GetCollate();
    int  GetOrientation();

    bool Ok();

    const wxString& GetPrinterName();
    bool GetColour();
    wxDuplexMode GetDuplex();
    wxPaperSize GetPaperId();
    const wxSize& GetPaperSize();

    wxPrintQuality GetQuality();

    void SetNoCopies(int v);
    void SetCollate(bool flag);
    void SetOrientation(int orient);

    void SetPrinterName(const wxString& name);
    void SetColour(bool colour);
    void SetDuplex(wxDuplexMode duplex);
    void SetPaperId(wxPaperSize sizeId);
    void SetPaperSize(const wxSize& sz);
    void SetQuality(wxPrintQuality quality);

    // PostScript-specific data
    const wxString& GetPrinterCommand();
    const wxString& GetPrinterOptions();
    const wxString& GetPreviewCommand();
    const wxString& GetFilename();
    const wxString& GetFontMetricPath();
    double GetPrinterScaleX();
    double GetPrinterScaleY();
    long GetPrinterTranslateX();
    long GetPrinterTranslateY();
    wxPrintMode GetPrintMode();

    void SetPrinterCommand(const wxString& command);
    void SetPrinterOptions(const wxString& options);
    void SetPreviewCommand(const wxString& command);
    void SetFilename(const wxString& filename);
    void SetFontMetricPath(const wxString& path);
    void SetPrinterScaleX(double x);
    void SetPrinterScaleY(double y);
    void SetPrinterScaling(double x, double y);
    void SetPrinterTranslateX(long x);
    void SetPrinterTranslateY(long y);
    void SetPrinterTranslation(long x, long y);
    void SetPrintMode(wxPrintMode printMode);

    %pragma(python) addtoclass = "def __nonzero__(self): return self.Ok()"
};

//----------------------------------------------------------------------

#ifdef __WXMSW__
class  wxPrinterDC : public wxDC {
public:
    wxPrinterDC(const wxPrintData& printData);
    %name(wxPrinterDC2) wxPrinterDC(const wxString& driver,
                                    const wxString& device,
                                    const wxString& output,
                                    bool interactive = TRUE,
                                    int orientation = wxPORTRAIT);
};
#endif

//---------------------------------------------------------------------------

class wxPostScriptDC : public wxDC {
public:
    wxPostScriptDC(const wxPrintData& printData);
//     %name(wxPostScriptDC2)wxPostScriptDC(const wxString& output,
//                                          bool interactive = TRUE,
//                                          wxWindow* parent = NULL);

    wxPrintData& GetPrintData();
    void SetPrintData(const wxPrintData& data);

    static void SetResolution(int ppi);
    static int GetResolution();
};

//---------------------------------------------------------------------------

class wxPageSetupDialogData : public wxObject {
public:
    wxPageSetupDialogData();
    ~wxPageSetupDialogData();

    void EnableHelp(bool flag);
    void EnableMargins(bool flag);
    void EnableOrientation(bool flag);
    void EnablePaper(bool flag);
    void EnablePrinter(bool flag);
    bool GetDefaultMinMargins();
    bool GetEnableMargins();
    bool GetEnableOrientation();
    bool GetEnablePaper();
    bool GetEnablePrinter();
    bool GetEnableHelp();
    bool GetDefaultInfo();
    wxPoint GetMarginTopLeft();
    wxPoint GetMarginBottomRight();
    wxPoint GetMinMarginTopLeft();
    wxPoint GetMinMarginBottomRight();
    wxPaperSize GetPaperId();
    wxSize GetPaperSize();
    %addmethods {
        %new wxPrintData* GetPrintData() {
            return new wxPrintData(self->GetPrintData());  // force a copy
        }
    }

    bool Ok();

    void SetDefaultInfo(bool flag);
    void SetDefaultMinMargins(bool flag);
    void SetMarginTopLeft(const wxPoint& pt);
    void SetMarginBottomRight(const wxPoint& pt);
    void SetMinMarginTopLeft(const wxPoint& pt);
    void SetMinMarginBottomRight(const wxPoint& pt);
    void SetPaperId(wxPaperSize id);
    void SetPaperSize(const wxSize& size);
    void SetPrintData(const wxPrintData& printData);

    %pragma(python) addtoclass = "def __nonzero__(self): return self.Ok()"
};


class wxPageSetupDialog : public wxDialog {
public:
    wxPageSetupDialog(wxWindow* parent, wxPageSetupDialogData* data = NULL);

    %pragma(python) addtomethod = "__init__:#wx._StdDialogCallbacks(self)"

    wxPageSetupDialogData& GetPageSetupData();
    int ShowModal();
};

//----------------------------------------------------------------------


class wxPrintDialogData : public wxObject {
public:
    wxPrintDialogData();
    ~wxPrintDialogData();

    void EnableHelp(bool flag);
    void EnablePageNumbers(bool flag);
    void EnablePrintToFile(bool flag);
    void EnableSelection(bool flag);
    bool GetAllPages();
    bool GetCollate();
    int GetFromPage();
    int GetMaxPage();
    int GetMinPage();
    int GetNoCopies();
    bool GetSelection();
    %addmethods {
        %new wxPrintData* GetPrintData() {
            return new wxPrintData(self->GetPrintData());  // force a copy
        }
    }
    bool GetPrintToFile();
    int GetToPage();

    bool Ok();

    void SetCollate(bool flag);
    void SetAllPages(bool flag);
    void SetFromPage(int page);
    void SetMaxPage(int page);
    void SetMinPage(int page);
    void SetNoCopies(int n);
    void SetPrintData(const wxPrintData& printData);
    void SetPrintToFile(bool flag);
    void SetSelection(bool flag);
    void SetSetupDialog(bool flag);
    void SetToPage(int page);

    %pragma(python) addtoclass = "def __nonzero__(self): return self.Ok()"
};


class wxPrintDialog : public wxDialog {
public:
    wxPrintDialog(wxWindow* parent, wxPrintDialogData* data = NULL);

    %pragma(python) addtomethod = "__init__:#wx._StdDialogCallbacks(self)"

    wxPrintDialogData& GetPrintDialogData();
    %new wxDC* GetPrintDC();
    int ShowModal();
};

//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Custom wxPrintout class that knows how to call python
%{


// Since this one would be tough and ugly to do with the Macros...
void wxPyPrintout::GetPageInfo(int *minPage, int *maxPage, int *pageFrom, int *pageTo) {
    bool hadErr = FALSE;
    bool found;

    wxPyBeginBlockThreads();
    if ((found = m_myInst.findCallback("GetPageInfo"))) {
        PyObject* result = m_myInst.callCallbackObj(Py_BuildValue("()"));
        if (result && PyTuple_Check(result) && PyTuple_Size(result) == 4) {
            PyObject* val;

            val = PyTuple_GetItem(result, 0);
            if (PyInt_Check(val))    *minPage = PyInt_AsLong(val);
            else hadErr = TRUE;

            val = PyTuple_GetItem(result, 1);
            if (PyInt_Check(val))    *maxPage = PyInt_AsLong(val);
            else hadErr = TRUE;

            val = PyTuple_GetItem(result, 2);
            if (PyInt_Check(val))    *pageFrom = PyInt_AsLong(val);
            else hadErr = TRUE;

            val = PyTuple_GetItem(result, 3);
            if (PyInt_Check(val))    *pageTo = PyInt_AsLong(val);
            else hadErr = TRUE;
        }
        else
            hadErr = TRUE;

        if (hadErr) {
            PyErr_SetString(PyExc_TypeError, "GetPageInfo should return a tuple of 4 integers.");
            PyErr_Print();
        }
        Py_DECREF(result);
    }
    wxPyEndBlockThreads();
    if (! found)
        wxPrintout::GetPageInfo(minPage, maxPage, pageFrom, pageTo);
}

void wxPyPrintout::base_GetPageInfo(int *minPage, int *maxPage, int *pageFrom, int *pageTo) {
    wxPrintout::GetPageInfo(minPage, maxPage, pageFrom, pageTo);
}


IMP_PYCALLBACK_BOOL_INTINT(wxPyPrintout, wxPrintout, OnBeginDocument);
IMP_PYCALLBACK__(wxPyPrintout, wxPrintout, OnEndDocument);
IMP_PYCALLBACK__(wxPyPrintout, wxPrintout, OnBeginPrinting);
IMP_PYCALLBACK__(wxPyPrintout, wxPrintout, OnEndPrinting);
IMP_PYCALLBACK__(wxPyPrintout, wxPrintout, OnPreparePrinting);
IMP_PYCALLBACK_BOOL_INT_pure(wxPyPrintout, wxPrintout, OnPrintPage);
IMP_PYCALLBACK_BOOL_INT(wxPyPrintout, wxPrintout, HasPage);


%}


// Now define the custom class for SWIGging
%name(wxPrintout) class wxPyPrintout  : public wxObject {
public:
    wxPyPrintout(const wxString& title = wxPyPrintoutTitleStr);

    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxPrintout)"

    %addmethods {
        void Destroy() { delete self; }
    }

    wxDC* GetDC();
    void GetPageSizeMM(int *OUTPUT, int *OUTPUT);
    void GetPageSizePixels(int *OUTPUT, int *OUTPUT);
    void GetPPIPrinter(int *OUTPUT, int *OUTPUT);
    void GetPPIScreen(int *OUTPUT, int *OUTPUT);
    bool IsPreview();

    bool base_OnBeginDocument(int startPage, int endPage);
    void base_OnEndDocument();
    void base_OnBeginPrinting();
    void base_OnEndPrinting();
    void base_OnPreparePrinting();
    void base_GetPageInfo(int *OUTPUT, int *OUTPUT, int *OUTPUT, int *OUTPUT);
    bool base_HasPage(int page);
};

//----------------------------------------------------------------------

enum wxPrinterError
{
    wxPRINTER_NO_ERROR = 0,
    wxPRINTER_CANCELLED,
    wxPRINTER_ERROR
};


class wxPrinter : public wxObject {
public:
    wxPrinter(wxPrintDialogData* data = NULL);
    ~wxPrinter();

    void CreateAbortWindow(wxWindow* parent, wxPyPrintout* printout);
    wxPrintDialogData& GetPrintDialogData();
    bool Print(wxWindow *parent, wxPyPrintout *printout, int prompt=TRUE);
    wxDC* PrintDialog(wxWindow *parent);
    void ReportError(wxWindow *parent, wxPyPrintout *printout, const wxString& message);
    bool Setup(wxWindow *parent);
    bool GetAbort();

    static wxPrinterError GetLastError();
};

//----------------------------------------------------------------------

class wxPrintAbortDialog: public wxDialog
{
public:
    wxPrintAbortDialog(wxWindow *parent,
                       const wxString& title,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = 0,
                       const wxString& name = wxPyDialogNameStr);

};

//----------------------------------------------------------------------

class wxPrintPreview : public wxObject {
public:
    wxPrintPreview(wxPyPrintout* printout,
                   wxPyPrintout* printoutForPrinting,
                   wxPrintData* data=NULL);

    virtual bool SetCurrentPage(int pageNum);
    int GetCurrentPage();

    void SetPrintout(wxPyPrintout *printout);
    wxPyPrintout *GetPrintout();
    wxPyPrintout *GetPrintoutForPrinting();

    void SetFrame(wxFrame *frame);
    void SetCanvas(wxWindow *canvas);

    virtual wxFrame *GetFrame();
    virtual wxWindow *GetCanvas();

    // The preview canvas should call this from OnPaint
    virtual bool PaintPage(wxWindow *canvas, wxDC& dc);

    // This draws a blank page onto the preview canvas
    virtual bool DrawBlankPage(wxWindow *canvas, wxDC& dc);

    // This is called by wxPrintPreview to render a page into a wxMemoryDC.
    virtual bool RenderPage(int pageNum);

    wxPrintDialogData& GetPrintDialogData();

    virtual void SetZoom(int percent);
    int GetZoom();

    int GetMaxPage();
    int GetMinPage();

    bool Ok();
    void SetOk(bool ok);

    virtual bool Print(bool interactive);
    virtual void DetermineScaling();

    %pragma(python) addtoclass = "def __nonzero__(self): return self.Ok()"
};


class wxPreviewFrame : public wxFrame {
public:
    wxPreviewFrame(wxPrintPreview* preview, wxFrame* parent, const wxString& title,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize&  size = wxDefaultSize,
                   long style = wxDEFAULT_FRAME_STYLE,
                   const wxString& name = wxPyFrameNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

    void Initialize();
    void CreateControlBar();
    void CreateCanvas();
};


class wxPreviewCanvas: public wxScrolledWindow
{
public:
    wxPreviewCanvas(wxPrintPreview *preview,
                    wxWindow *parent,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0,
                    const wxString& name = wxPyPreviewCanvasNameStr);
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
};



enum {
    wxPREVIEW_PRINT,
    wxPREVIEW_PREVIOUS,
    wxPREVIEW_NEXT,
    wxPREVIEW_ZOOM,
    wxPREVIEW_FIRST,
    wxPREVIEW_LAST,
    wxPREVIEW_GOTO,
    wxPREVIEW_DEFAULT,

    wxID_PREVIEW_CLOSE,
    wxID_PREVIEW_NEXT,
    wxID_PREVIEW_PREVIOUS,
    wxID_PREVIEW_PRINT,
    wxID_PREVIEW_ZOOM,
    wxID_PREVIEW_FIRST,
    wxID_PREVIEW_LAST,
    wxID_PREVIEW_GOTO
};

class wxPreviewControlBar: public wxPanel
{
public:
    wxPreviewControlBar(wxPrintPreview *preview,
                        long buttons,
                        wxWindow *parent,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxString& name = wxPyPanelNameStr);
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

    int GetZoomControl();
    void SetZoomControl(int zoom);
    wxPrintPreview* GetPrintPreview();

    void OnNext();
    void OnPrevious();
    void OnFirst();
    void OnLast();
    void OnGoto();
};


//----------------------------------------------------------------------
// Python-derivable versions of the above preview classes

%{
class wxPyPrintPreview : public wxPrintPreview
{
    DECLARE_CLASS(wxPyPrintPreview)
public:
    wxPyPrintPreview(wxPyPrintout* printout,
                     wxPyPrintout* printoutForPrinting,
                     wxPrintData* data=NULL)
        : wxPrintPreview(printout, printoutForPrinting, data)
    {}

    DEC_PYCALLBACK_BOOL_INT(SetCurrentPage);
    DEC_PYCALLBACK_BOOL_WXWINDC(PaintPage);
    DEC_PYCALLBACK_BOOL_WXWINDC(DrawBlankPage);
    DEC_PYCALLBACK_BOOL_INT(RenderPage);
    DEC_PYCALLBACK_VOID_INT(SetZoom);
    DEC_PYCALLBACK_BOOL_BOOL(Print);
    DEC_PYCALLBACK_VOID_(DetermineScaling);

    PYPRIVATE;
};

// Stupid renamed classes...  Fix this in 2.5...
#if defined(__WXMSW__)
IMPLEMENT_CLASS( wxPyPrintPreview, wxWindowsPrintPreview );
#elif defined(__WXMAC__)
IMPLEMENT_CLASS( wxPyPrintPreview, wxMacPrintPreview );
#else
IMPLEMENT_CLASS( wxPyPrintPreview, wxPrintPreview );
#endif

IMP_PYCALLBACK_BOOL_INT    (wxPyPrintPreview, wxPrintPreview, SetCurrentPage);
IMP_PYCALLBACK_BOOL_WXWINDC(wxPyPrintPreview, wxPrintPreview, PaintPage);
IMP_PYCALLBACK_BOOL_WXWINDC(wxPyPrintPreview, wxPrintPreview, DrawBlankPage);
IMP_PYCALLBACK_BOOL_INT    (wxPyPrintPreview, wxPrintPreview, RenderPage);
IMP_PYCALLBACK_VOID_INT    (wxPyPrintPreview, wxPrintPreview, SetZoom);
IMP_PYCALLBACK_BOOL_BOOL   (wxPyPrintPreview, wxPrintPreview, Print);
IMP_PYCALLBACK_VOID_       (wxPyPrintPreview, wxPrintPreview, DetermineScaling);
%}


class wxPyPrintPreview : public wxPrintPreview
{
public:
    wxPyPrintPreview(wxPyPrintout* printout,
                     wxPyPrintout* printoutForPrinting,
                     wxPrintData* data=NULL);

    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxPyPrintPreview)"

    bool base_SetCurrentPage(int pageNum);
    bool base_PaintPage(wxWindow *canvas, wxDC& dc);
    bool base_DrawBlankPage(wxWindow *canvas, wxDC& dc);
    bool base_RenderPage(int pageNum);
    void base_SetZoom(int percent);
    bool base_Print(bool interactive);
    void base_DetermineScaling();
};




%{
class wxPyPreviewFrame : public wxPreviewFrame
{
    DECLARE_CLASS(wxPyPreviewFrame);
public:
    wxPyPreviewFrame(wxPrintPreview* preview, wxFrame* parent,
                     const wxString& title,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize&  size = wxDefaultSize,
                     long style = wxDEFAULT_FRAME_STYLE,
                     const wxString& name = wxPyFrameNameStr)
        : wxPreviewFrame(preview, parent, title, pos, size, style, name)
    {}

    void SetPreviewCanvas(wxWindow* canvas) { m_previewCanvas = canvas; }
    void SetControlBar(wxPreviewControlBar* bar) { m_controlBar = bar; }

    DEC_PYCALLBACK_VOID_(Initialize);
    DEC_PYCALLBACK_VOID_(CreateCanvas);
    DEC_PYCALLBACK_VOID_(CreateControlBar);

    PYPRIVATE;
};

IMPLEMENT_CLASS(wxPyPreviewFrame, wxPreviewFrame);

IMP_PYCALLBACK_VOID_(wxPyPreviewFrame, wxPreviewFrame, Initialize);
IMP_PYCALLBACK_VOID_(wxPyPreviewFrame, wxPreviewFrame, CreateCanvas);
IMP_PYCALLBACK_VOID_(wxPyPreviewFrame, wxPreviewFrame, CreateControlBar);
%}

class wxPyPreviewFrame : public wxPreviewFrame
{
public:
    wxPyPreviewFrame(wxPrintPreview* preview, wxFrame* parent,
                     const wxString& title,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize&  size = wxDefaultSize,
                     long style = wxDEFAULT_FRAME_STYLE,
                     const wxString& name = wxPyFrameNameStr);

    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxPyPreviewFrame)"
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

    void SetPreviewCanvas(wxWindow* canvas);
    void SetControlBar(wxPreviewControlBar* bar);

    void base_Initialize();
    void base_CreateCanvas();
    void base_CreateControlBar();
};




%{
class wxPyPreviewControlBar : public wxPreviewControlBar
{
    DECLARE_CLASS(wxPyPreviewControlBar);
public:
    wxPyPreviewControlBar(wxPrintPreview *preview,
                          long buttons,
                          wxWindow *parent,
                          const wxPoint& pos = wxDefaultPosition,
                          const wxSize& size = wxDefaultSize,
                          long style = 0,
                          const wxString& name = wxPyPanelNameStr)
        : wxPreviewControlBar(preview, buttons, parent, pos, size, style, name)
    {}

    void SetPrintPreview(wxPrintPreview* preview) { m_printPreview = preview; }

    DEC_PYCALLBACK_VOID_(CreateButtons);
    DEC_PYCALLBACK_VOID_INT(SetZoomControl);

    PYPRIVATE;
};

IMPLEMENT_CLASS(wxPyPreviewControlBar, wxPreviewControlBar);
IMP_PYCALLBACK_VOID_(wxPyPreviewControlBar, wxPreviewControlBar, CreateButtons);
IMP_PYCALLBACK_VOID_INT(wxPyPreviewControlBar, wxPreviewControlBar, SetZoomControl);
%}

class wxPyPreviewControlBar : public wxPreviewControlBar
{
public:
    wxPyPreviewControlBar(wxPrintPreview *preview,
                          long buttons,
                          wxWindow *parent,
                          const wxPoint& pos = wxDefaultPosition,
                          const wxSize& size = wxDefaultSize,
                          long style = 0,
                          const wxString& name = wxPyPanelNameStr);

    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxPyPreviewControlBar)"
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

    void SetPrintPreview(wxPrintPreview* preview);

    void base_CreateButtons();
    void base_SetZoomControl(int zoom);
};


//----------------------------------------------------------------------

%init %{
    wxPyPtrTypeMap_Add("wxPrintout", "wxPyPrintout");
%}

//----------------------------------------------------------------------
//----------------------------------------------------------------------




