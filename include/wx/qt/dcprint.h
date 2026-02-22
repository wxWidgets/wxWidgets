/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/dcprint.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_DCPRINT_H_
#define _WX_QT_DCPRINT_H_

#include "wx/dcprint.h"
#include "wx/cmndata.h"
#include "wx/qt/dc.h"

class QPrinter;

class WXDLLIMPEXP_CORE wxPrinterDCImpl : public wxQtDCImpl
{
public:
    wxPrinterDCImpl(wxPrinterDC* owner, const wxPrintData& data);
    ~wxPrinterDCImpl();

    // override some base class virtuals
    virtual bool StartDoc(const wxString& message) override;
    virtual void EndDoc() override;
    virtual void StartPage() override;
    virtual void EndPage() override;

    virtual wxRect GetPaperRect() const override;

    virtual int GetResolution() const override;

    virtual wxSize GetPPI() const override;

    virtual void DoGetSize(int* width, int* height) const override;
    virtual void DoGetSizeMM(int* width, int* height) const override;

    QPrinter* GetQtPrinter() const { return m_qtPrinter; }

protected:
    wxPrintData m_printData;
    QPrinter* m_qtPrinter = nullptr; // This pointer is obtained from m_printData
                                     // and stored here for convenience.

private:
    wxDECLARE_CLASS(wxPrinterDCImpl);
    wxDECLARE_NO_COPY_CLASS(wxPrinterDCImpl);
};

#endif // _WX_QT_DCPRINT_H_
