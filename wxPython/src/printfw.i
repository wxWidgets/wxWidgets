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

    DECLARE_DEF_STRING(FrameNameStr);
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

class wxPrintPreview : public wxObject {
public:
    wxPrintPreview(wxPyPrintout* printout, wxPyPrintout* printoutForPrinting, wxPrintData* data=NULL);
//    ~wxPrintPreview();   **** ????

    wxWindow* GetCanvas();
    int GetCurrentPage();
    wxFrame * GetFrame();
    int GetMaxPage();
    int GetMinPage();
    wxPrintDialogData& GetPrintDialogData();
    wxPyPrintout * GetPrintout();
    wxPyPrintout * GetPrintoutForPrinting();
    int GetZoom();
    bool Ok();
    bool Print(bool prompt);
    void SetCanvas(wxWindow* window);
    void SetCurrentPage(int pageNum);
    void SetFrame(wxFrame *frame);
    void SetPrintout(wxPyPrintout *printout);
    void SetZoom(int percent);

    %pragma(python) addtoclass = "def __nonzero__(self): return self.Ok()"
};

//----------------------------------------------------------------------

class wxPreviewFrame : public wxFrame {
public:
    wxPreviewFrame(wxPrintPreview* preview, wxFrame* parent, const wxString& title,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize&  size = wxDefaultSize,
                   long style = wxDEFAULT_FRAME_STYLE,
                   const wxString& name = wxPyFrameNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

    void Initialize();

    // ****  need to use derived class so these can be properly overridden:
    //void CreateControlBar()
    //void CreateCanvas()

};

//----------------------------------------------------------------------

%init %{
    wxPyPtrTypeMap_Add("wxPrintout", "wxPyPrintout");
%}

//----------------------------------------------------------------------
//----------------------------------------------------------------------




