/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/printwin.h
// Purpose:     wxWindowsPrinter, wxWindowsPrintPreview classes
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRINTWIN_H_
#define _WX_PRINTWIN_H_

#include "wx/prntbase.h"

// Bricsys change: cache wxEnhMetaFile that the preview generates, such that it does not have to be
// recreated on zoom. Please follow BS_CACHE_PREVIEW_METAFILE defines to find all related code.
//#define BS_CACHE_PREVIEW_METAFILE 0
#define BS_CACHE_PREVIEW_METAFILE wxUSE_ENH_METAFILE

#if BS_CACHE_PREVIEW_METAFILE
#include <memory>
class wxEnhMetaFile;
#endif // BS_CACHE_PREVIEW_METAFILE

// ---------------------------------------------------------------------------
// Represents the printer: manages printing a wxPrintout object
// ---------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxWindowsPrinter : public wxPrinterBase
{
    wxDECLARE_DYNAMIC_CLASS(wxWindowsPrinter);

public:
    wxWindowsPrinter(wxPrintDialogData *data = NULL);

    virtual bool Print(wxWindow *parent,
                       wxPrintout *printout,
                       bool prompt = true,
                       bool batchMode = false) wxOVERRIDE;

    virtual wxDC *PrintDialog(wxWindow *parent) wxOVERRIDE;
    virtual bool Setup(wxWindow *parent) wxOVERRIDE;

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
                          wxPrintout *printoutForPrinting = NULL,
                          wxPrintDialogData *data = NULL);
    wxWindowsPrintPreview(wxPrintout *printout,
                          wxPrintout *printoutForPrinting,
                          wxPrintData *data);
    virtual ~wxWindowsPrintPreview();

    virtual bool Print(bool interactive) wxOVERRIDE;
    virtual void DetermineScaling() wxOVERRIDE;

protected:
#if wxUSE_ENH_METAFILE
    virtual bool RenderPageIntoBitmap(wxBitmap& bmp, int pageNum) wxOVERRIDE;

#if BS_CACHE_PREVIEW_METAFILE
    std::unique_ptr<wxEnhMetaFile> m_metafile;
    int m_metafilePageNum;
#endif // BS_CACHE_PREVIEW_METAFILE

#endif // wxUSE_ENH_METAFILE

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxWindowsPrintPreview);
};

#endif
// _WX_PRINTWIN_H_
