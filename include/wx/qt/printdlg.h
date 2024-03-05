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

class WXDLLIMPEXP_CORE wxQtPrintNativeData: public wxPrintNativeDataBase
{
public:
    wxQtPrintNativeData();

    virtual bool TransferTo( wxPrintData &data ) override;
    virtual bool TransferFrom( const wxPrintData &data ) override;

    virtual bool IsOk() const override;

};

class WXDLLIMPEXP_CORE wxQtPrintDialog : public wxPrintDialogBase
{
public:
    wxQtPrintDialog(wxWindow *parent, wxPrintDialogData *data);
    wxQtPrintDialog(wxWindow *parent, wxPrintData *data);

    virtual wxPrintDialogData& GetPrintDialogData() override;
    virtual wxPrintData& GetPrintData() override;
    virtual wxDC *GetPrintDC() override;

protected:

private:
};



class WXDLLIMPEXP_CORE wxQtPageSetupDialog: public wxPageSetupDialogBase
{
public:
    wxQtPageSetupDialog();
    wxQtPageSetupDialog(wxWindow *parent, wxPageSetupDialogData *data = nullptr);

    bool Create(wxWindow *parent, wxPageSetupDialogData *data = nullptr);

    virtual wxPageSetupDialogData& GetPageSetupDialogData() override;

private:
};

#endif // _WX_QT_PRINTDLG_H_
