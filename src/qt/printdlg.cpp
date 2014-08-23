/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/printdlg.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/qt/printdlg.h"

wxQtPrintNativeData::wxQtPrintNativeData()
{
}

bool wxQtPrintNativeData::TransferTo( wxPrintData &WXUNUSED(data) )
{
    return false;
}

bool wxQtPrintNativeData::TransferFrom( const wxPrintData &WXUNUSED(data) )
{
    return false;
}

bool wxQtPrintNativeData::IsOk() const
{
    return false;
}

//##############################################################################

wxQtPrintDialog::wxQtPrintDialog(wxWindow *WXUNUSED(parent), wxPrintDialogData *WXUNUSED(data))
{
}

wxQtPrintDialog::wxQtPrintDialog(wxWindow *WXUNUSED(parent), wxPrintData *WXUNUSED(data))
{
}


wxPrintDialogData& wxQtPrintDialog::GetPrintDialogData()
{
    static wxPrintDialogData s_data;

    return s_data;
}

wxPrintData& wxQtPrintDialog::GetPrintData()
{
    static wxPrintData s_data;

    return s_data;
}

wxDC *wxQtPrintDialog::GetPrintDC()
{
    return NULL;
}

//##############################################################################

wxQtPageSetupDialog::wxQtPageSetupDialog()
{
}

wxQtPageSetupDialog::wxQtPageSetupDialog(wxWindow *WXUNUSED(parent), wxPageSetupDialogData *WXUNUSED(data))
{
}

bool wxQtPageSetupDialog::Create(wxWindow *WXUNUSED(parent), wxPageSetupDialogData *WXUNUSED(data))
{
    return false;
}

wxPageSetupDialogData& wxQtPageSetupDialog::GetPageSetupDialogData()
{
    static wxPageSetupDialogData s_data;

    return s_data;
}

