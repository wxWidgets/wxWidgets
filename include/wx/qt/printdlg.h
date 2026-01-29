/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/printdlg.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_PRINTDLG_H_
#define _WX_QT_PRINTDLG_H_

#include "wx/prntbase.h"
#include "wx/printdlg.h"

#include <memory>

class QPrinter;

class WXDLLIMPEXP_CORE wxQtPrintNativeData: public wxPrintNativeDataBase
{
public:
    wxQtPrintNativeData();
    ~wxQtPrintNativeData();

    virtual bool TransferTo(wxPrintData& data) override;
    virtual bool TransferFrom(const wxPrintData& data) override;

    virtual bool IsOk() const override;

    QPrinter* GetQtPrinter() const { return m_qtPrinter.get(); }

private:
    std::unique_ptr<QPrinter> m_qtPrinter;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxQtPrintNativeData);
};

class WXDLLIMPEXP_CORE wxQtPrintDialog: public wxPrintDialogBase
{
public:
    wxQtPrintDialog() = default;
    explicit wxQtPrintDialog(wxWindow* parent, wxPrintDialogData* data = nullptr);
    wxQtPrintDialog(wxWindow* parent, wxPrintData* data);

    virtual int ShowModal() override;

    virtual wxPrintDialogData& GetPrintDialogData() override { return m_printDialogData; }
    virtual wxPrintData& GetPrintData() override { return m_printDialogData.GetPrintData(); }

    virtual wxDC* GetPrintDC() override;

private:
    wxWindow*         m_dialogParent = nullptr;
    wxPrinterDC*      m_printerDC = nullptr;
    wxPrintDialogData m_printDialogData;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxQtPrintDialog);
};


class WXDLLIMPEXP_CORE wxQtPageSetupDialog: public wxPageSetupDialogBase
{
public:
    wxQtPageSetupDialog() = default;
    explicit wxQtPageSetupDialog(wxWindow* parent, wxPageSetupDialogData* data = nullptr);

    virtual int ShowModal() override;

    virtual wxPageSetupDialogData& GetPageSetupDialogData() override { return m_pageSetupData; }

private:
    wxWindow*               m_dialogParent = nullptr;
    wxPageSetupDialogData   m_pageSetupData;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxQtPageSetupDialog);
};

#endif // _WX_QT_PRINTDLG_H_
