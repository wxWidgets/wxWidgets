/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/printqt.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_PRINTQT_H_
#define _WX_QT_PRINTQT_H_

#include "wx/prntbase.h"

// ---------------------------------------------------------------------------
// Represents the printer: manages printing a wxPrintout object
// ---------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxQtPrinter : public wxPrinterBase
{
public:
    explicit wxQtPrinter(wxPrintDialogData* data = nullptr);

    virtual bool Setup(wxWindow* parent) override;
    virtual bool Print(wxWindow* parent, wxPrintout* printout, bool prompt = true) override;
    virtual wxDC* PrintDialog(wxWindow* parent) override;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxQtPrinter);
};


// ---------------------------------------------------------------------------
// wxPrintPreview: programmer creates an object of this class to preview a
// wxPrintout.
// ---------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxQtPrintPreview : public wxPrintPreviewBase
{
public:
    wxQtPrintPreview(wxPrintout* printout,
                     wxPrintout* printoutForPrinting = nullptr,
                     wxPrintDialogData* data = nullptr);
    wxQtPrintPreview(wxPrintout* printout,
                     wxPrintout* printoutForPrinting,
                     wxPrintData* data);

    virtual bool Print(bool interactive) override;
    virtual void DetermineScaling() override;

protected:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxQtPrintPreview);
};

#endif // _WX_QT_PRINTQT_H_
