/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/printqt.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_PRINTQT_H_
#define _WX_QT_PRINTQT_H_

#include "wx/prntbase.h"

class WXDLLIMPEXP_CORE wxQtPrinter : public wxPrinterBase
{
public:
    wxQtPrinter( wxPrintDialogData *data = NULL );

    virtual bool Setup(wxWindow *parent) wxOVERRIDE;
    virtual bool Print(wxWindow *parent, wxPrintout *printout, bool prompt = true) wxOVERRIDE;
    virtual wxDC* PrintDialog(wxWindow *parent) wxOVERRIDE;
private:
};



class WXDLLIMPEXP_CORE wxQtPrintPreview : public wxPrintPreviewBase
{
public:
    wxQtPrintPreview(wxPrintout *printout,
                          wxPrintout *printoutForPrinting = NULL,
                          wxPrintDialogData *data = NULL);
    wxQtPrintPreview(wxPrintout *printout,
                          wxPrintout *printoutForPrinting,
                          wxPrintData *data);

    virtual bool Print(bool interactive) wxOVERRIDE;
    virtual void DetermineScaling() wxOVERRIDE;

protected:
};

#endif // _WX_QT_PRINTQT_H_
