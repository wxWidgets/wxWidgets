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



class wxPrintData {
public:
    wxPrintData();
    ~wxPrintData();

    int GetNoCopies();
    bool GetCollate();
    int  GetOrientation();

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

//    // PostScript-specific data
//      const wxString& GetPrinterCommand();
//      const wxString& GetPrinterOptions();
//      const wxString& GetPreviewCommand();
//      const wxString& GetFilename();
//      const wxString& GetFontMetricPath();
//      double GetPrinterScaleX();
//      double GetPrinterScaleY();
//      long GetPrinterTranslateX();
//      long GetPrinterTranslateY();
//      wxPrintMode GetPrintMode();

//      void SetPrinterCommand(const wxString& command);
//      void SetPrinterOptions(const wxString& options);
//      void SetPreviewCommand(const wxString& command);
//      void SetFilename(const wxString& filename);
//      void SetFontMetricPath(const wxString& path);
//      void SetPrinterScaleX(double x);
//      void SetPrinterScaleY(double y);
//      void SetPrinterScaling(double x, double y);
//      void SetPrinterTranslateX(long x);
//      void SetPrinterTranslateY(long y);
//      void SetPrinterTranslation(long x, long y);
//      void SetPrintMode(wxPrintMode printMode);

};

//----------------------------------------------------------------------

class wxPageSetupDialogData {
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
    void SetDefaultInfo(bool flag);
    void SetDefaultMinMargins(bool flag);
    void SetMarginTopLeft(const wxPoint& pt);
    void SetMarginBottomRight(const wxPoint& pt);
    void SetMinMarginTopLeft(const wxPoint& pt);
    void SetMinMarginBottomRight(const wxPoint& pt);
    void SetPaperId(wxPaperSize& id);
    void SetPaperSize(const wxSize& size);
    void SetPrintData(const wxPrintData& printData);
};


class wxPageSetupDialog : public wxDialog {
public:
    wxPageSetupDialog(wxWindow* parent, wxPageSetupDialogData* data = NULL);

    %pragma(python) addtomethod = "__init__:wx._StdDialogCallbacks(self)"

    wxPageSetupDialogData& GetPageSetupData();
    int ShowModal();
};

//----------------------------------------------------------------------


class wxPrintDialogData {
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
    %addmethods {
        %new wxPrintData* GetPrintData() {
            return new wxPrintData(self->GetPrintData());  // force a copy
        }
    }
    bool GetPrintToFile();
    int GetToPage();
    void SetCollate(bool flag);
    void SetFromPage(int page);
    void SetMaxPage(int page);
    void SetMinPage(int page);
    void SetNoCopies(int n);
    void SetPrintData(const wxPrintData& printData);
    void SetPrintToFile(bool flag);
    void SetSetupDialog(bool flag);
    void SetToPage(int page);
};


class wxPrintDialog : public wxDialog {
public:
    wxPrintDialog(wxWindow* parent, wxPrintDialogData* data = NULL);

    %pragma(python) addtomethod = "__init__:wx._StdDialogCallbacks(self)"

    wxPrintDialogData& GetPrintDialogData();
    %new wxDC* GetPrintDC();
    int ShowModal();
};

//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Custom wxPrintout class that knows how to call python
%{
class wxPyPrintout : public wxPrintout {
public:
    wxPyPrintout(const wxString& title) : wxPrintout(title) {}

    DEC_PYCALLBACK_BOOL_INTINT(OnBeginDocument);
    DEC_PYCALLBACK__(OnEndDocument);
    DEC_PYCALLBACK__(OnBeginPrinting);
    DEC_PYCALLBACK__(OnEndPrinting);
    DEC_PYCALLBACK__(OnPreparePrinting);
    DEC_PYCALLBACK_BOOL_INT_pure(OnPrintPage);
    DEC_PYCALLBACK_BOOL_INT(HasPage);


    // Since this one would be tough and ugly to do with the Macros...
    void GetPageInfo(int *minPage, int *maxPage, int *pageFrom, int *pageTo) {
        bool hadErr = false;

        bool doSave = wxPyRestoreThread();
        if (m_myInst.findCallback("GetPageInfo")) {
            PyObject* result = m_myInst.callCallbackObj(Py_BuildValue("()"));
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
        else
            wxPrintout::GetPageInfo(minPage, maxPage, pageFrom, pageTo);

        wxPySaveThread(doSave);
    }

    void base_GetPageInfo(int *minPage, int *maxPage, int *pageFrom, int *pageTo) {
        wxPrintout::GetPageInfo(minPage, maxPage, pageFrom, pageTo);
    }

    PYPRIVATE;
};

IMP_PYCALLBACK_BOOL_INTINT(wxPyPrintout, wxPrintout, OnBeginDocument);
IMP_PYCALLBACK__(wxPyPrintout, wxPrintout, OnEndDocument);
IMP_PYCALLBACK__(wxPyPrintout, wxPrintout, OnBeginPrinting);
IMP_PYCALLBACK__(wxPyPrintout, wxPrintout, OnEndPrinting);
IMP_PYCALLBACK__(wxPyPrintout, wxPrintout, OnPreparePrinting);
IMP_PYCALLBACK_BOOL_INT_pure(wxPyPrintout, wxPrintout, OnPrintPage);
IMP_PYCALLBACK_BOOL_INT(wxPyPrintout, wxPrintout, HasPage);


%}


// Now define the custom class for SWIGging
%name(wxPrintout) class wxPyPrintout {
public:
    wxPyPrintout(const char* title = "Printout");

    void _setSelf(PyObject* self);
    %pragma(python) addtomethod = "__init__:self._setSelf(self)"

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

class wxPrinter {
public:
    wxPrinter(wxPrintDialogData* data = NULL);
    ~wxPrinter();

//    bool Abort();
    void CreateAbortWindow(wxWindow* parent, wxPyPrintout* printout);
    wxPrintDialogData& GetPrintDialogData();
    bool Print(wxWindow *parent, wxPyPrintout *printout, int prompt=TRUE);
    wxDC* PrintDialog(wxWindow *parent);
    void ReportError(wxWindow *parent, wxPyPrintout *printout, char* message);
    bool Setup(wxWindow *parent);
};

//----------------------------------------------------------------------

class wxPrintPreview {
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
};

//----------------------------------------------------------------------

class wxPreviewFrame : public wxFrame {
public:
    wxPreviewFrame(wxPrintPreview* preview, wxFrame* parent, const wxString& title,
                   const wxPoint& pos = wxPyDefaultPosition,
                   const wxSize&  size = wxPyDefaultSize,
                   long style = wxDEFAULT_FRAME_STYLE,
                   char* name = "frame");

    %pragma(python) addtomethod = "__init__:wx._StdFrameCallbacks(self)"

    void Initialize();

    // ****  need to use derived class so these can be properly overridden:
    //void CreateControlBar()
    //void CreateCanvas()

};

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------




