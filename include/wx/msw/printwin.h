/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/printwin.h
// Purpose:     wxWindowsPrinter, wxWindowsPrintPreview classes
// Author:      Julian Smart
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRINTWIN_H_
#define _WX_PRINTWIN_H_

#include "wx/prntbase.h"

// ---------------------------------------------------------------------------
// Represents the printer: manages printing a wxPrintout object
// ---------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxWindowsPrinter : public wxPrinterBase
{
    wxDECLARE_DYNAMIC_CLASS(wxWindowsPrinter);

public:
    wxWindowsPrinter(wxPrintDialogData *data = nullptr);

    virtual bool Print(wxWindow *parent,
                       wxPrintout *printout,
                       bool prompt = true) override;

    virtual wxDC *PrintDialog(wxWindow *parent) override;
    virtual bool Setup(wxWindow *parent) override;

private:
    wxDECLARE_NO_COPY_CLASS(wxWindowsPrinter);
};

// ---------------------------------------------------------------------------
// wxPrintPreview: programmer creates an object of this class to preview a
// wxPrintout.
// ---------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxWindowsPrintPreview : public wxPrintPreviewBase
{
public:
    wxWindowsPrintPreview(wxPrintout *printout,
                          wxPrintout *printoutForPrinting = nullptr,
                          wxPrintDialogData *data = nullptr);
    wxWindowsPrintPreview(wxPrintout *printout,
                          wxPrintout *printoutForPrinting,
                          wxPrintData *data);
    virtual ~wxWindowsPrintPreview();

    virtual bool Print(bool interactive) override;
    virtual void DetermineScaling() override;

protected:
#if wxUSE_ENH_METAFILE
    virtual bool RenderPageIntoBitmap(wxBitmap& bmp, int pageNum) override;
#endif

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxWindowsPrintPreview);
};

#endif
// _WX_PRINTWIN_H_
