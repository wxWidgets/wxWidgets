/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/printdlg.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/qt/printdlg.h"

wxQtPrintNativeData::wxQtPrintNativeData()
{
}

bool wxQtPrintNativeData::TransferTo( wxPrintData &data )
{
    return false;
}

bool wxQtPrintNativeData::TransferFrom( const wxPrintData &data )
{
    return false;
}

bool wxQtPrintNativeData::IsOk() const
{
    return false;
}

//##############################################################################

wxQtPrintDialog::wxQtPrintDialog(wxWindow *parent, wxPrintDialogData *data)
{
}

wxQtPrintDialog::wxQtPrintDialog(wxWindow *parent, wxPrintData *data)
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

wxQtPageSetupDialog::wxQtPageSetupDialog(wxWindow *parent, wxPageSetupDialogData *data)
{
}

bool wxQtPageSetupDialog::Create(wxWindow *parent, wxPageSetupDialogData *data)
{
    return false;
}

wxPageSetupDialogData& wxQtPageSetupDialog::GetPageSetupDialogData()
{
    static wxPageSetupDialogData s_data;

    return s_data;
}

