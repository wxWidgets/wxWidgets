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
    
    virtual bool TransferTo( wxPrintData &data );
    virtual bool TransferFrom( const wxPrintData &data );

    virtual bool IsOk() const;
    
};

class WXDLLIMPEXP_CORE wxQtPrintDialog : public wxPrintDialogBase
{
public:
    wxQtPrintDialog(wxWindow *parent, wxPrintDialogData *data);
    wxQtPrintDialog(wxWindow *parent, wxPrintData *data);

    virtual wxPrintDialogData& GetPrintDialogData();
    virtual wxPrintData& GetPrintData();
    virtual wxDC *GetPrintDC();

protected:

private:
};



class WXDLLIMPEXP_CORE wxQtPageSetupDialog: public wxPageSetupDialogBase
{
public:
    wxQtPageSetupDialog();
    wxQtPageSetupDialog(wxWindow *parent, wxPageSetupDialogData *data = NULL);

    bool Create(wxWindow *parent, wxPageSetupDialogData *data = NULL);

    virtual wxPageSetupDialogData& GetPageSetupDialogData();

private:
};

#endif // _WX_QT_PRINTDLG_H_
