/////////////////////////////////////////////////////////////////////////////
// Name:        _????.i
// Purpose:     SWIG interface for wx????
//
// Author:      Robin Dunn
//
// Created:     9-Aug-2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
%newgroup

%{
#include "wx/wxPython/printfw.h"

%}

MAKE_CONST_WXSTRING2(PrintoutTitleStr, wxT("Printout"));
MAKE_CONST_WXSTRING2(PreviewCanvasNameStr, wxT("previewcanvas"));


//---------------------------------------------------------------------------

enum wxPrintMode
{
    wxPRINT_MODE_NONE =    0,
    wxPRINT_MODE_PREVIEW = 1,   // Preview in external application
    wxPRINT_MODE_FILE =    2,   // Print to file
    wxPRINT_MODE_PRINTER = 3,   // Send to printer
    wxPRINT_MODE_STREAM = 4     // Send postscript data into a stream 
};

enum wxPrintBin
{
    wxPRINTBIN_DEFAULT,

    wxPRINTBIN_ONLYONE,
    wxPRINTBIN_LOWER,
    wxPRINTBIN_MIDDLE,
    wxPRINTBIN_MANUAL,
    wxPRINTBIN_ENVELOPE,
    wxPRINTBIN_ENVMANUAL,
    wxPRINTBIN_AUTO,
    wxPRINTBIN_TRACTOR,
    wxPRINTBIN_SMALLFMT,
    wxPRINTBIN_LARGEFMT,
    wxPRINTBIN_LARGECAPACITY,
    wxPRINTBIN_CASSETTE,
    wxPRINTBIN_FORMSOURCE,

    wxPRINTBIN_USER,
};


class wxPrintData : public wxObject {
public:
    %nokwargs wxPrintData;
    wxPrintData();
    wxPrintData(const wxPrintData& data);  // for making copies
    
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

    int GetQuality();
    wxPrintBin GetBin();
    wxPrintMode GetPrintMode() const;
    
    void SetNoCopies(int v);
    void SetCollate(bool flag);
    void SetOrientation(int orient);

    void SetPrinterName(const wxString& name);
    void SetColour(bool colour);
    void SetDuplex(wxDuplexMode duplex);
    void SetPaperId(wxPaperSize sizeId);
    void SetPaperSize(const wxSize& sz);
    void SetQuality(int quality);
    void SetBin(wxPrintBin bin);
    void SetPrintMode(wxPrintMode printMode);
 
    wxString GetFilename() const;
    void SetFilename( const wxString &filename );

    %pythoncode { def __nonzero__(self): return self.Ok() }


    // NOTE: These are now inside of #if WXWIN_COMPATIBILITY_2_4, so be
    //       prepared to remove them...
    
    // PostScript-specific data
    const wxString& GetPrinterCommand();
    const wxString& GetPrinterOptions();
    const wxString& GetPreviewCommand();
    const wxString& GetFontMetricPath();
    double GetPrinterScaleX();
    double GetPrinterScaleY();
    long GetPrinterTranslateX();
    long GetPrinterTranslateY();
    void SetPrinterCommand(const wxString& command);
    void SetPrinterOptions(const wxString& options);
    void SetPreviewCommand(const wxString& command);
    void SetFontMetricPath(const wxString& path);
    void SetPrinterScaleX(double x);
    void SetPrinterScaleY(double y);
    void SetPrinterScaling(double x, double y);
    void SetPrinterTranslateX(long x);
    void SetPrinterTranslateY(long y);
    void SetPrinterTranslation(long x, long y);

};

//---------------------------------------------------------------------------

class wxPageSetupDialogData : public wxObject {
public:
    %nokwargs wxPageSetupDialogData;
    wxPageSetupDialogData();
    wxPageSetupDialogData(const wxPageSetupDialogData& data);  // for making copies
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

    wxPrintData& GetPrintData();
//     %addmethods {
//         %new wxPrintData* GetPrintData() {
//             return new wxPrintData(self->GetPrintData());  // force a copy
//         }
//     }

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

    %pythoncode { def __nonzero__(self): return self.Ok() }
};



// NOTE: Contrary to it's name, this class doesn't derive from wxDialog.  It
// is a facade in front of a platform-specific (native dialog) provided by the
// wxPrintFactory.

MustHaveApp(wxPageSetupDialog);

class wxPageSetupDialog : public wxObject
{
public:
    wxPageSetupDialog(wxWindow* parent, wxPageSetupDialogData* data = NULL);

    wxPageSetupDialogData& GetPageSetupData();
    wxPageSetupDialogData& GetPageSetupDialogData();
    int ShowModal();
};

//---------------------------------------------------------------------------


class wxPrintDialogData : public wxObject {
public:
    %nokwargs wxPrintDialogData;
    wxPrintDialogData();
    wxPrintDialogData(const wxPrintData& printData);  // for making copies
    ~wxPrintDialogData();

    int GetFromPage() const;
    int GetToPage() const;
    int GetMinPage() const;
    int GetMaxPage() const;
    int GetNoCopies() const;
    bool GetAllPages() const;
    bool GetSelection() const;
    bool GetCollate() const;
    bool GetPrintToFile() const;

    // WXWIN_COMPATIBILITY_2_4
    bool GetSetupDialog() const;
    void SetSetupDialog(bool flag);

    void SetFromPage(int v);
    void SetToPage(int v);
    void SetMinPage(int v);
    void SetMaxPage(int v);
    void SetNoCopies(int v);
    void SetAllPages(bool flag);
    void SetSelection(bool flag);
    void SetCollate(bool flag);
    void SetPrintToFile(bool flag);

    void EnablePrintToFile(bool flag);
    void EnableSelection(bool flag);
    void EnablePageNumbers(bool flag);
    void EnableHelp(bool flag);

    bool GetEnablePrintToFile() const;
    bool GetEnableSelection() const;
    bool GetEnablePageNumbers() const;
    bool GetEnableHelp() const;

    // Is this data OK for showing the print dialog?
    bool Ok() const;

    
    wxPrintData& GetPrintData();
    void SetPrintData(const wxPrintData& printData);

    %pythoncode { def __nonzero__(self): return self.Ok() }
};



MustHaveApp(wxPrintDialog);


// NOTE: Contrary to it's name, this class doesn't derive from wxDialog.  It
// is a facade in front of a platform-specific (native dialog) provided by the
// wxPrintFactory.

class wxPrintDialog : public wxObject {
public:
    wxPrintDialog(wxWindow* parent, wxPrintDialogData* data = NULL);

    // TODO?: wxPrintDialog(wxWindow *parent, wxPrintData* data);
    
    virtual int ShowModal();

    virtual wxPrintDialogData& GetPrintDialogData();
    virtual wxPrintData& GetPrintData();

    %newobject GetPrintDC;
    virtual wxDC *GetPrintDC();

};


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


enum wxPrinterError
{
    wxPRINTER_NO_ERROR = 0,
    wxPRINTER_CANCELLED,
    wxPRINTER_ERROR
};


MustHaveApp(wxPrinter);

class wxPrinter : public wxObject {
public:
    wxPrinter(wxPrintDialogData* data = NULL);
    ~wxPrinter();

    virtual wxWindow *CreateAbortWindow(wxWindow *parent, wxPyPrintout *printout);
    virtual void ReportError(wxWindow *parent, wxPyPrintout *printout, const wxString& message);

    virtual bool Setup(wxWindow *parent);
    virtual bool Print(wxWindow *parent, wxPyPrintout *printout, bool prompt = true);
    virtual wxDC* PrintDialog(wxWindow *parent);
    
    virtual wxPrintDialogData& GetPrintDialogData() const;

    bool GetAbort();
    static wxPrinterError GetLastError();
};


//---------------------------------------------------------------------------
// Custom wxPrintout class that knows how to call python, See implementation in
// include/sx/wxPython/printfw.h

%{

IMPLEMENT_ABSTRACT_CLASS(wxPyPrintout, wxPrintout);

// Since this one would be tough and ugly to do with the Macros...
void wxPyPrintout::GetPageInfo(int *minPage, int *maxPage, int *pageFrom, int *pageTo) {
    bool hadErr = false;
    bool found;

    bool blocked = wxPyBeginBlockThreads();
    if ((found = wxPyCBH_findCallback(m_myInst, "GetPageInfo"))) {
        PyObject* result = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));
        if (result && PyTuple_Check(result) && PyTuple_Size(result) == 4) {
            PyObject* val;

            val = PyTuple_GetItem(result, 0);
            if (PyInt_Check(val))    *minPage = PyInt_AsLong(val);
            else hadErr = true;

            val = PyTuple_GetItem(result, 1);
            if (PyInt_Check(val))    *maxPage = PyInt_AsLong(val);
            else hadErr = true;

            val = PyTuple_GetItem(result, 2);
            if (PyInt_Check(val))    *pageFrom = PyInt_AsLong(val);
            else hadErr = true;

            val = PyTuple_GetItem(result, 3);
            if (PyInt_Check(val))    *pageTo = PyInt_AsLong(val);
            else hadErr = true;
        }
        else
            hadErr = true;

        if (hadErr) {
            PyErr_SetString(PyExc_TypeError, "GetPageInfo should return a tuple of 4 integers.");
            PyErr_Print();
        }
        Py_DECREF(result);
    }
    wxPyEndBlockThreads(blocked);
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


MustHaveApp(wxPyPrintout);

// Now define the custom class for SWIGging
%name(Printout) class wxPyPrintout  : public wxObject {
public:
    %pythonAppend wxPyPrintout   "self._setCallbackInfo(self, Printout)"

    wxPyPrintout(const wxString& title = wxPyPrintoutTitleStr);
    //~wxPyPrintout();      wxPrintPreview object takes ownership...
    
    void _setCallbackInfo(PyObject* self, PyObject* _class);

    
    wxString GetTitle() const;
    wxDC* GetDC();
    void SetDC(wxDC *dc);

    void SetPageSizePixels(int w, int  h);
    DocDeclA(
        void, GetPageSizePixels(int *OUTPUT, int *OUTPUT),
        "GetPageSizePixels() -> (w, h)");

    void SetPageSizeMM(int w, int  h);
    DocDeclA(
        void, GetPageSizeMM(int *OUTPUT, int *OUTPUT),
        "GetPageSizeMM() -> (w, h)");
    
    void SetPPIScreen(int x, int y);
    DocDeclA(
        void, GetPPIScreen(int *OUTPUT, int *OUTPUT),
        "GetPPIScreen() -> (x,y)");
    
    void SetPPIPrinter(int x, int y);
    DocDeclA(
        void, GetPPIPrinter(int *OUTPUT, int *OUTPUT),
        "GetPPIPrinter() -> (x,y)");

    bool IsPreview();
    void SetIsPreview(bool p);

    
    bool base_OnBeginDocument(int startPage, int endPage);
    void base_OnEndDocument();
    void base_OnBeginPrinting();
    void base_OnEndPrinting();
    void base_OnPreparePrinting();
    bool base_HasPage(int page);
    DocDeclA(
        void, base_GetPageInfo(int *OUTPUT, int *OUTPUT, int *OUTPUT, int *OUTPUT),
        "base_GetPageInfo() -> (minPage, maxPage, pageFrom, pageTo)");
};

//---------------------------------------------------------------------------



MustHaveApp(wxPreviewCanvas);

class wxPreviewCanvas: public wxScrolledWindow
{
public:
    %pythonAppend wxPreviewCanvas   "self._setOORInfo(self)"

    wxPreviewCanvas(wxPrintPreview *preview,
                    wxWindow *parent,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0,
                    const wxString& name = wxPyPreviewCanvasNameStr);
};


MustHaveApp(wxPreviewFrame);

class wxPreviewFrame : public wxFrame {
public:
    %pythonAppend wxPreviewFrame   "self._setOORInfo(self)"

    wxPreviewFrame(wxPrintPreview* preview, wxFrame* parent, const wxString& title,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize&  size = wxDefaultSize,
                   long style = wxDEFAULT_FRAME_STYLE,
                   const wxString& name = wxPyFrameNameStr);

    void Initialize();
    void CreateControlBar();
    void CreateCanvas();

    wxPreviewControlBar* GetControlBar() const;
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

MustHaveApp(wxPreviewControlBar);

class wxPreviewControlBar: public wxPanel
{
public:
    %pythonAppend wxPreviewControlBar   "self._setOORInfo(self)"

    wxPreviewControlBar(wxPrintPreview *preview,
                        long buttons,
                        wxWindow *parent,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxTAB_TRAVERSAL,
                        const wxString& name = wxPyPanelNameStr);

    int GetZoomControl();
    void SetZoomControl(int zoom);
    wxPrintPreview* GetPrintPreview();

    void OnNext();
    void OnPrevious();
    void OnFirst();
    void OnLast();
    void OnGoto();
};



//---------------------------------------------------------------------------

MustHaveApp(wxPrintPreview);

class wxPrintPreview : public wxObject {
public:
    %nokwargs wxPrintPreview;
    wxPrintPreview(wxPyPrintout* printout,
                   wxPyPrintout* printoutForPrinting,
                   wxPrintDialogData *data=NULL);
    wxPrintPreview(wxPyPrintout* printout,
                  wxPyPrintout* printoutForPrinting,
                  wxPrintData* data);

    virtual bool SetCurrentPage(int pageNum);
    int GetCurrentPage();

    void SetPrintout(wxPyPrintout *printout);
    wxPyPrintout *GetPrintout();
    wxPyPrintout *GetPrintoutForPrinting();

    void SetFrame(wxFrame *frame);
    void SetCanvas(wxPreviewCanvas *canvas);

    virtual wxFrame *GetFrame();
    virtual wxPreviewCanvas *GetCanvas();

    // The preview canvas should call this from OnPaint
    virtual bool PaintPage(wxPreviewCanvas *canvas, wxDC& dc);

    // This draws a blank page onto the preview canvas
    virtual bool DrawBlankPage(wxPreviewCanvas *canvas, wxDC& dc);

    // This is called by wxPrintPreview to render a page into a wxMemoryDC.
    virtual bool RenderPage(int pageNum);

    // Adjusts the scrollbars for the current scale
    virtual void AdjustScrollbars(wxPreviewCanvas *canvas);

    wxPrintDialogData& GetPrintDialogData();

    virtual void SetZoom(int percent);
    int GetZoom();

    int GetMaxPage();
    int GetMinPage();

    bool Ok();
    void SetOk(bool ok);

    virtual bool Print(bool interactive);
    virtual void DetermineScaling();

    %pythoncode { def __nonzero__(self): return self.Ok() }
};



//---------------------------------------------------------------------------

// Python-derivable versions of the above preview classes

%{

#define DEC_PYCALLBACK_BOOL_PREWINDC(CBNAME)                                            \
    bool CBNAME(wxPreviewCanvas* a, wxDC& b);                                           \
    bool base_##CBNAME(wxPreviewCanvas* a, wxDC& b)


#define IMP_PYCALLBACK_BOOL_PREWINDC(CLASS, PCLASS, CBNAME)                             \
    bool CLASS::CBNAME(wxPreviewCanvas* a, wxDC& b) {                                   \
        bool rval=false;                                                                \
        bool found;                                                                     \
        bool blocked = wxPyBeginBlockThreads();                                         \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                        \
            PyObject* win = wxPyMake_wxObject(a,false);                                 \
            PyObject* dc  = wxPyMake_wxObject(&b,false);                                \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(OO)", win, dc));      \
            Py_DECREF(win);                                                             \
            Py_DECREF(dc);                                                              \
        }                                                                               \
        wxPyEndBlockThreads(blocked);                                                   \
        if (! found)                                                                    \
            rval = PCLASS::CBNAME(a, b);                                                \
        return rval;                                                                    \
    }                                                                                   \
    bool CLASS::base_##CBNAME(wxPreviewCanvas* a, wxDC& b) {                            \
        return PCLASS::CBNAME(a, b);                                                    \
    }




class wxPyPrintPreview : public wxPrintPreview
{
    DECLARE_CLASS(wxPyPrintPreview)
public:
    wxPyPrintPreview(wxPyPrintout* printout,
                     wxPyPrintout* printoutForPrinting,
                     wxPrintDialogData* data=NULL)
        : wxPrintPreview(printout, printoutForPrinting, data)
    {}
    wxPyPrintPreview(wxPyPrintout* printout,
                     wxPyPrintout* printoutForPrinting,
                     wxPrintData* data=NULL)
        : wxPrintPreview(printout, printoutForPrinting, data)
    {}

    DEC_PYCALLBACK_BOOL_INT(SetCurrentPage);
    DEC_PYCALLBACK_BOOL_PREWINDC(PaintPage);
    DEC_PYCALLBACK_BOOL_PREWINDC(DrawBlankPage);
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
IMPLEMENT_CLASS( wxPyPrintPreview, wxPostScriptPrintPreview );
#endif

IMP_PYCALLBACK_BOOL_INT     (wxPyPrintPreview, wxPrintPreview, SetCurrentPage);
IMP_PYCALLBACK_BOOL_PREWINDC(wxPyPrintPreview, wxPrintPreview, PaintPage);
IMP_PYCALLBACK_BOOL_PREWINDC(wxPyPrintPreview, wxPrintPreview, DrawBlankPage);
IMP_PYCALLBACK_BOOL_INT     (wxPyPrintPreview, wxPrintPreview, RenderPage);
IMP_PYCALLBACK_VOID_INT     (wxPyPrintPreview, wxPrintPreview, SetZoom);
IMP_PYCALLBACK_BOOL_BOOL    (wxPyPrintPreview, wxPrintPreview, Print);
IMP_PYCALLBACK_VOID_        (wxPyPrintPreview, wxPrintPreview, DetermineScaling);
%}


MustHaveApp(wxPyPrintPreview);

class wxPyPrintPreview : public wxPrintPreview
{
public:
    %pythonAppend wxPyPrintPreview   "self._setCallbackInfo(self, PyPrintPreview)"
    %nokwargs wxPyPrintPreview;
    wxPyPrintPreview(wxPyPrintout* printout,
                     wxPyPrintout* printoutForPrinting,
                     wxPrintDialogData* data=NULL);
    wxPyPrintPreview(wxPyPrintout* printout,
                     wxPyPrintout* printoutForPrinting,
                     wxPrintData* data);

    void _setCallbackInfo(PyObject* self, PyObject* _class);
    
    bool base_SetCurrentPage(int pageNum);
    bool base_PaintPage(wxPreviewCanvas *canvas, wxDC& dc);
    bool base_DrawBlankPage(wxPreviewCanvas *canvas, wxDC& dc);
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

    void SetPreviewCanvas(wxPreviewCanvas* canvas) { m_previewCanvas = canvas; }
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


MustHaveApp(wxPyPreviewFrame);

class wxPyPreviewFrame : public wxPreviewFrame
{
public:
    %pythonAppend wxPyPreviewFrame "self._setCallbackInfo(self, PyPreviewFrame); self._setOORInfo(self)"

    wxPyPreviewFrame(wxPrintPreview* preview, wxFrame* parent,
                     const wxString& title,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize&  size = wxDefaultSize,
                     long style = wxDEFAULT_FRAME_STYLE,
                     const wxString& name = wxPyFrameNameStr);

    void _setCallbackInfo(PyObject* self, PyObject* _class);

    void SetPreviewCanvas(wxPreviewCanvas* canvas);
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


MustHaveApp(wxPyPreviewControlBar);

class wxPyPreviewControlBar : public wxPreviewControlBar
{
public:
    %pythonAppend wxPyPreviewControlBar   "self._setCallbackInfo(self, PyPreviewControlBar); self._setOORInfo(self)"

    wxPyPreviewControlBar(wxPrintPreview *preview,
                          long buttons,
                          wxWindow *parent,
                          const wxPoint& pos = wxDefaultPosition,
                          const wxSize& size = wxDefaultSize,
                          long style = 0,
                          const wxString& name = wxPyPanelNameStr);

    void _setCallbackInfo(PyObject* self, PyObject* _class);

    void SetPrintPreview(wxPrintPreview* preview);

    void base_CreateButtons();
    void base_SetZoomControl(int zoom);
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
%init %{
    wxPyPtrTypeMap_Add("wxPrintout", "wxPyPrintout");
%}
//---------------------------------------------------------------------------
