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

    bool IsOk();
    %pythoncode { Ok = IsOk }

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

    %pythoncode { def __nonzero__(self): return self.IsOk() }

    //char* GetPrivData() const;
    //int GetPrivDataLen() const;
    //void SetPrivData( char *privData, int len );

    %extend {
        PyObject* GetPrivData() {
            PyObject* data;
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            data = PyString_FromStringAndSize(self->GetPrivData(),
                                              self->GetPrivDataLen());
            wxPyEndBlockThreads(blocked);
            return data;
        }

        void SetPrivData(PyObject* data) {
            if (! PyString_Check(data)) {
                wxPyBLOCK_THREADS(PyErr_SetString(PyExc_TypeError,
                                                  "Expected string object"));
                return /* NULL */ ;
            }

            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            self->SetPrivData(PyString_AS_STRING(data), PyString_GET_SIZE(data));
            wxPyEndBlockThreads(blocked);
        }
    }
    

    // NOTE: These are now inside of #if WXWIN_COMPATIBILITY_2_4, so be
    //       prepared to remove them...
    
    // PostScript-specific data
// WXWIN_COMPATIBILITY_2_4
#if 0
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
#endif

    %property(Bin, GetBin, SetBin, doc="See `GetBin` and `SetBin`");
    %property(Collate, GetCollate, SetCollate, doc="See `GetCollate` and `SetCollate`");
    %property(Colour, GetColour, SetColour, doc="See `GetColour` and `SetColour`");
    %property(Duplex, GetDuplex, SetDuplex, doc="See `GetDuplex` and `SetDuplex`");
    %property(Filename, GetFilename, SetFilename, doc="See `GetFilename` and `SetFilename`");
    %property(NoCopies, GetNoCopies, SetNoCopies, doc="See `GetNoCopies` and `SetNoCopies`");
    %property(Orientation, GetOrientation, SetOrientation, doc="See `GetOrientation` and `SetOrientation`");
    %property(PaperId, GetPaperId, SetPaperId, doc="See `GetPaperId` and `SetPaperId`");
    %property(PaperSize, GetPaperSize, SetPaperSize, doc="See `GetPaperSize` and `SetPaperSize`");
    %property(PrintMode, GetPrintMode, SetPrintMode, doc="See `GetPrintMode` and `SetPrintMode`");
    %property(PrinterName, GetPrinterName, SetPrinterName, doc="See `GetPrinterName` and `SetPrinterName`");
    %property(PrivData, GetPrivData, SetPrivData, doc="See `GetPrivData` and `SetPrivData`");
    %property(Quality, GetQuality, SetQuality, doc="See `GetQuality` and `SetQuality`");   
};

//---------------------------------------------------------------------------

class wxPageSetupDialogData : public wxObject {
public:
    %nokwargs wxPageSetupDialogData;
    wxPageSetupDialogData();
    wxPageSetupDialogData(const wxPageSetupDialogData& data);  // for making copies
    wxPageSetupDialogData(const wxPrintData& data); 
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

    bool IsOk();
    %pythoncode { Ok = IsOk }

    void SetDefaultInfo(bool flag);
    void SetDefaultMinMargins(bool flag);
    void SetMarginTopLeft(const wxPoint& pt);
    void SetMarginBottomRight(const wxPoint& pt);
    void SetMinMarginTopLeft(const wxPoint& pt);
    void SetMinMarginBottomRight(const wxPoint& pt);
    void SetPaperId(wxPaperSize id);
    void SetPaperSize(const wxSize& size);
    
    void SetPrintData(const wxPrintData& printData);

    // Use paper size defined in this object to set the wxPrintData
    // paper id
    void CalculateIdFromPaperSize();

    // Use paper id in wxPrintData to set this object's paper size
    void CalculatePaperSizeFromId();

    %pythoncode { def __nonzero__(self): return self.IsOk() }

    %property(DefaultInfo, GetDefaultInfo, SetDefaultInfo, doc="See `GetDefaultInfo` and `SetDefaultInfo`");
    %property(DefaultMinMargins, GetDefaultMinMargins, SetDefaultMinMargins, doc="See `GetDefaultMinMargins` and `SetDefaultMinMargins`");

//     %property(EnableHelp, GetEnableHelp, doc="See `GetEnableHelp`");
//     %property(EnableMargins, GetEnableMargins, doc="See `GetEnableMargins`");
//     %property(EnableOrientation, GetEnableOrientation, doc="See `GetEnableOrientation`");
//     %property(EnablePaper, GetEnablePaper, doc="See `GetEnablePaper`");
//     %property(EnablePrinter, GetEnablePrinter, doc="See `GetEnablePrinter`");

    %property(MarginBottomRight, GetMarginBottomRight, SetMarginBottomRight, doc="See `GetMarginBottomRight` and `SetMarginBottomRight`");
    %property(MarginTopLeft, GetMarginTopLeft, SetMarginTopLeft, doc="See `GetMarginTopLeft` and `SetMarginTopLeft`");
    %property(MinMarginBottomRight, GetMinMarginBottomRight, SetMinMarginBottomRight, doc="See `GetMinMarginBottomRight` and `SetMinMarginBottomRight`");
    %property(MinMarginTopLeft, GetMinMarginTopLeft, SetMinMarginTopLeft, doc="See `GetMinMarginTopLeft` and `SetMinMarginTopLeft`");
    %property(PaperId, GetPaperId, SetPaperId, doc="See `GetPaperId` and `SetPaperId`");
    %property(PaperSize, GetPaperSize, SetPaperSize, doc="See `GetPaperSize` and `SetPaperSize`");
    %property(PrintData, GetPrintData, SetPrintData, doc="See `GetPrintData` and `SetPrintData`");
    
};



// NOTE: Contrary to it's name, this class doesn't derive from wxDialog.  It
// is a facade in front of a platform-specific (native dialog) provided by the
// wxPrintFactory.

MustHaveApp(wxPageSetupDialog);

class wxPageSetupDialog : public wxObject
{
public:
    wxPageSetupDialog(wxWindow* parent, wxPageSetupDialogData* data = NULL);
    ~wxPageSetupDialog();
    
    wxPageSetupDialogData& GetPageSetupData();
    wxPageSetupDialogData& GetPageSetupDialogData();
    int ShowModal();

    %pythoncode { def Destroy(self): pass }

    %property(PageSetupData, GetPageSetupData, doc="See `GetPageSetupData`");
    %property(PageSetupDialogData, GetPageSetupDialogData, doc="See `GetPageSetupDialogData`");
    
};

//---------------------------------------------------------------------------


class wxPrintDialogData : public wxObject {
public:
    %nokwargs wxPrintDialogData;
    wxPrintDialogData();
    wxPrintDialogData(const wxPrintData& printData);
    wxPrintDialogData(const wxPrintDialogData& printData);  // for making copies
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
#if 0
    bool GetSetupDialog() const;
    void SetSetupDialog(bool flag);
#endif
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
    bool IsOk() const;
    %pythoncode { Ok = IsOk }

    
    wxPrintData& GetPrintData();
    void SetPrintData(const wxPrintData& printData);

    %pythoncode { def __nonzero__(self): return self.IsOk() }
    
    %property(AllPages, GetAllPages, SetAllPages, doc="See `GetAllPages` and `SetAllPages`");
    %property(Collate, GetCollate, SetCollate, doc="See `GetCollate` and `SetCollate`");
//     %property(EnableHelp, GetEnableHelp, doc="See `GetEnableHelp`");
//     %property(EnablePageNumbers, GetEnablePageNumbers, doc="See `GetEnablePageNumbers`");
//     %property(EnablePrintToFile, GetEnablePrintToFile, doc="See `GetEnablePrintToFile`");
//     %property(EnableSelection, GetEnableSelection, doc="See `GetEnableSelection`");
    %property(FromPage, GetFromPage, SetFromPage, doc="See `GetFromPage` and `SetFromPage`");
    %property(MaxPage, GetMaxPage, SetMaxPage, doc="See `GetMaxPage` and `SetMaxPage`");
    %property(MinPage, GetMinPage, SetMinPage, doc="See `GetMinPage` and `SetMinPage`");
    %property(NoCopies, GetNoCopies, SetNoCopies, doc="See `GetNoCopies` and `SetNoCopies`");
    %property(PrintData, GetPrintData, SetPrintData, doc="See `GetPrintData` and `SetPrintData`");
    %property(PrintToFile, GetPrintToFile, SetPrintToFile, doc="See `GetPrintToFile` and `SetPrintToFile`");
    %property(Selection, GetSelection, SetSelection, doc="See `GetSelection` and `SetSelection`");
    %property(ToPage, GetToPage, SetToPage, doc="See `GetToPage` and `SetToPage`");
};



MustHaveApp(wxPrintDialog);


// NOTE: Contrary to it's name, this class doesn't derive from wxDialog.  It
// is a facade in front of a platform-specific (native dialog) provided by the
// wxPrintFactory.

class wxPrintDialog : public wxObject {
public:
    wxPrintDialog(wxWindow* parent, wxPrintDialogData* data = NULL);

    // TODO?: wxPrintDialog(wxWindow *parent, wxPrintData* data);

    ~wxPrintDialog();
    
    virtual int ShowModal();

    virtual wxPrintDialogData& GetPrintDialogData();
    virtual wxPrintData& GetPrintData();

    %newobject GetPrintDC;
    virtual wxDC *GetPrintDC();

    %pythoncode { def Destroy(self): pass }
    
    %property(PrintDC, GetPrintDC, doc="See `GetPrintDC`");
    %property(PrintData, GetPrintData, doc="See `GetPrintData`");
    %property(PrintDialogData, GetPrintDialogData, doc="See `GetPrintDialogData`");
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

    %property(Abort, GetAbort, doc="See `GetAbort`");
    %property(PrintDialogData, GetPrintDialogData, doc="See `GetPrintDialogData`");
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

    wxPyBlock_t blocked = wxPyBeginBlockThreads();
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
%rename(Printout) wxPyPrintout;
class wxPyPrintout  : public wxObject {
public:
    %pythonAppend wxPyPrintout   "self._setCallbackInfo(self, Printout)"
    %typemap(out) wxPyPrintout*;    // turn off this typemap

    wxPyPrintout(const wxString& title = wxPyPrintoutTitleStr);
    ~wxPyPrintout();      
    
    // Turn it back on again
    %typemap(out) wxPyPrintout* { $result = wxPyMake_wxObject($1, $owner); }

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

    
    bool OnBeginDocument(int startPage, int endPage);
    void OnEndDocument();
    void OnBeginPrinting();
    void OnEndPrinting();
    void OnPreparePrinting();
    bool HasPage(int page);
    DocDeclA(
        void, GetPageInfo(int *OUTPUT, int *OUTPUT, int *OUTPUT, int *OUTPUT),
        "GetPageInfo() -> (minPage, maxPage, pageFrom, pageTo)");
    
    %MAKE_BASE_FUNC(Printout, OnBeginDocument);
    %MAKE_BASE_FUNC(Printout, OnEndDocument);
    %MAKE_BASE_FUNC(Printout, OnBeginPrinting);
    %MAKE_BASE_FUNC(Printout, OnEndPrinting);
    %MAKE_BASE_FUNC(Printout, OnPreparePrinting);
    %MAKE_BASE_FUNC(Printout, GetPageInfo);

    
    %property(DC, GetDC, SetDC, doc="See `GetDC` and `SetDC`");
    %property(PPIPrinter, GetPPIPrinter, SetPPIPrinter, doc="See `GetPPIPrinter` and `SetPPIPrinter`");
    %property(PPIScreen, GetPPIScreen, SetPPIScreen, doc="See `GetPPIScreen` and `SetPPIScreen`");
    %property(PageSizeMM, GetPageSizeMM, SetPageSizeMM, doc="See `GetPageSizeMM` and `SetPageSizeMM`");
    %property(PageSizePixels, GetPageSizePixels, SetPageSizePixels, doc="See `GetPageSizePixels` and `SetPageSizePixels`");
    %property(Title, GetTitle, doc="See `GetTitle`");
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
    %disownarg(wxPrintPreview*);
    
    %pythonAppend wxPreviewFrame   "self._setOORInfo(self)"
    wxPreviewFrame(wxPrintPreview* preview, wxFrame* parent, const wxString& title,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize&  size = wxDefaultSize,
                   long style = wxDEFAULT_FRAME_STYLE,
                   const wxString& name = wxPyFrameNameStr);

    %cleardisown(wxPrintPreview*);

    void Initialize();
    void CreateControlBar();
    void CreateCanvas();

    wxPreviewControlBar* GetControlBar() const;

    %property(ControlBar, GetControlBar, doc="See `GetControlBar`");
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

    %property(PrintPreview, GetPrintPreview, doc="See `GetPrintPreview`");
    %property(ZoomControl, GetZoomControl, SetZoomControl, doc="See `GetZoomControl` and `SetZoomControl`");    
};



//---------------------------------------------------------------------------

MustHaveApp(wxPrintPreview);

class wxPrintPreview : public wxObject {
public:
    %disownarg(wxPyPrintout*);
    
    %nokwargs wxPrintPreview;
    wxPrintPreview(wxPyPrintout* printout,
                   wxPyPrintout* printoutForPrinting,
                   wxPrintDialogData *data=NULL);
    wxPrintPreview(wxPyPrintout* printout,
                  wxPyPrintout* printoutForPrinting,
                  wxPrintData* data);

    ~wxPrintPreview();
    
    virtual bool SetCurrentPage(int pageNum);
    int GetCurrentPage();

    void SetPrintout(wxPyPrintout *printout);
    wxPyPrintout *GetPrintout();
    wxPyPrintout *GetPrintoutForPrinting();

    %cleardisown(wxPyPrintout*);
    
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

    bool IsOk();
    %pythoncode { Ok = IsOk }
    void SetOk(bool ok);

    virtual bool Print(bool interactive);
    virtual void DetermineScaling();

    %pythoncode { def __nonzero__(self): return self.IsOk() }
    
    %property(Canvas, GetCanvas, SetCanvas, doc="See `GetCanvas` and `SetCanvas`");
    %property(CurrentPage, GetCurrentPage, SetCurrentPage, doc="See `GetCurrentPage` and `SetCurrentPage`");
    %property(Frame, GetFrame, SetFrame, doc="See `GetFrame` and `SetFrame`");
    %property(MaxPage, GetMaxPage, doc="See `GetMaxPage`");
    %property(MinPage, GetMinPage, doc="See `GetMinPage`");
    %property(PrintDialogData, GetPrintDialogData, doc="See `GetPrintDialogData`");
    %property(Printout, GetPrintout, SetPrintout, doc="See `GetPrintout` and `SetPrintout`");
    %property(PrintoutForPrinting, GetPrintoutForPrinting, doc="See `GetPrintoutForPrinting`");
    %property(Zoom, GetZoom, SetZoom, doc="See `GetZoom` and `SetZoom`");
};



//---------------------------------------------------------------------------

// Python-derivable versions of the above preview classes

%{

#define DEC_PYCALLBACK_BOOL_PREWINDC(CBNAME)                                            \
    bool CBNAME(wxPreviewCanvas* a, wxDC& b)


#define IMP_PYCALLBACK_BOOL_PREWINDC(CLASS, PCLASS, CBNAME)                             \
    bool CLASS::CBNAME(wxPreviewCanvas* a, wxDC& b) {                                   \
        bool rval=false;                                                                \
        bool found;                                                                     \
        wxPyBlock_t blocked = wxPyBeginBlockThreads();                                  \
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
                     wxPrintData* data)
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
    %disownarg(wxPyPrintout*);

    %pythonAppend wxPyPrintPreview   "self._setCallbackInfo(self, PyPrintPreview)"
    %nokwargs wxPyPrintPreview;
    wxPyPrintPreview(wxPyPrintout* printout,
                     wxPyPrintout* printoutForPrinting,
                     wxPrintDialogData* data=NULL);
    wxPyPrintPreview(wxPyPrintout* printout,
                     wxPyPrintout* printoutForPrinting,
                     wxPrintData* data);

    %cleardisown(wxPyPrintout*);
    
    void _setCallbackInfo(PyObject* self, PyObject* _class);
    
    bool SetCurrentPage(int pageNum);
    bool PaintPage(wxPreviewCanvas *canvas, wxDC& dc);
    bool DrawBlankPage(wxPreviewCanvas *canvas, wxDC& dc);
    bool RenderPage(int pageNum);
    void SetZoom(int percent);
    bool Print(bool interactive);
    void DetermineScaling();

    %MAKE_BASE_FUNC(PyPrintPreview, SetCurrentPage);
    %MAKE_BASE_FUNC(PyPrintPreview, PaintPage);
    %MAKE_BASE_FUNC(PyPrintPreview, DrawBlankPage);
    %MAKE_BASE_FUNC(PyPrintPreview, RenderPage);
    %MAKE_BASE_FUNC(PyPrintPreview, SetZoom);
    %MAKE_BASE_FUNC(PyPrintPreview, Print);
    %MAKE_BASE_FUNC(PyPrintPreview, DetermineScaling);
};




%{
class wxPyPreviewFrame : public wxPreviewFrame
{
    DECLARE_CLASS(wxPyPreviewFrame)
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

    void Initialize();
    void CreateCanvas();
    void CreateControlBar();

    %MAKE_BASE_FUNC(PyPreviewFrame, Initialize);
    %MAKE_BASE_FUNC(PyPreviewFrame, CreateCanvas);
    %MAKE_BASE_FUNC(PyPreviewFrame, CreateControlBar);
};




%{
class wxPyPreviewControlBar : public wxPreviewControlBar
{
    DECLARE_CLASS(wxPyPreviewControlBar)
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

    void CreateButtons();
    void SetZoomControl(int zoom);

    %MAKE_BASE_FUNC(PreviewControlBar, CreateButtons);
    %MAKE_BASE_FUNC(PreviewControlBar, SetZoomControl);
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
%init %{
    wxPyPtrTypeMap_Add("wxPrintout", "wxPyPrintout");
%}
//---------------------------------------------------------------------------
