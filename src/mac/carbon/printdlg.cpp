/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/printdlg.cpp
// Purpose:     wxPrintDialog, wxPageSetupDialog
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_PRINTING_ARCHITECTURE

#include "wx/printdlg.h"

#ifndef WX_PRECOMP
    #include "wx/object.h"
    #include "wx/dcprint.h"
    #include "wx/msgdlg.h"
    #include "wx/textctrl.h"
    #include "wx/sizer.h"
    #include "wx/stattext.h"
#endif

#include "wx/mac/printdlg.h"
#include "wx/mac/private/print.h"
#include "wx/statline.h"


// Use generic page setup dialog: use your own native one if one exists.

IMPLEMENT_DYNAMIC_CLASS(wxMacPrintDialog, wxPrintDialogBase)


wxMacPrintDialog::wxMacPrintDialog()
{
    m_dialogParent = NULL;
    m_printerDC = NULL;
    m_destroyDC = true;
}

wxMacPrintDialog::wxMacPrintDialog( wxWindow *p, wxPrintDialogData *data )
{
    Create( p, data );
}

wxMacPrintDialog::wxMacPrintDialog( wxWindow *p, wxPrintData *data )
{
    wxPrintDialogData data2;
    if (data != NULL)
        data2 = *data;

    Create( p, &data2 );
}

bool wxMacPrintDialog::Create( wxWindow *p, wxPrintDialogData *data )
{
    m_dialogParent = p;
    m_printerDC = NULL;
    m_destroyDC = true;

    if (data != NULL)
        m_printDialogData = *data;

    return true;
}

wxMacPrintDialog::~wxMacPrintDialog()
{
    if (m_destroyDC && m_printerDC)
    {
        delete m_printerDC;
        m_printerDC = NULL;
    }
}

int wxMacPrintDialog::ShowModal()
{
    m_printDialogData.GetPrintData().ConvertToNative();
    ((wxMacCarbonPrintData*)m_printDialogData.GetPrintData().GetNativeData())->TransferFrom( &m_printDialogData );

    int result = wxID_CANCEL;
    OSErr err = noErr;
    Boolean accepted;

    err = PMSessionPrintDialog(
        ((wxMacCarbonPrintData*)m_printDialogData.GetPrintData().GetNativeData())->m_macPrintSession,
        ((wxMacCarbonPrintData*)m_printDialogData.GetPrintData().GetNativeData())->m_macPrintSettings,
        ((wxMacCarbonPrintData*)m_printDialogData.GetPrintData().GetNativeData())->m_macPageFormat,
        &accepted );

    if ((err == noErr) && !accepted)
    {
        // user clicked Cancel button
        err = kPMCancel;
    }

    if (err == noErr)
    {
        result = wxID_OK;
    }

    if ((err != noErr) && (err != kPMCancel))
    {
        wxString message;

        message.Printf( wxT("Print Error %d"), err );
        wxMessageDialog dialog( NULL, message, wxEmptyString, wxICON_HAND | wxOK );
        dialog.ShowModal();
    }

    if (result == wxID_OK)
    {
        m_printDialogData.GetPrintData().ConvertFromNative();
        ((wxMacCarbonPrintData*)m_printDialogData.GetPrintData().GetNativeData())->TransferTo( &m_printDialogData );
    }

    return result;
}

wxDC *wxMacPrintDialog::GetPrintDC()
{
    return new wxPrinterDC( m_printDialogData.GetPrintData() );
}

IMPLEMENT_CLASS(wxMacPageSetupDialog, wxPageSetupDialogBase)

wxMacPageSetupDialog::wxMacPageSetupDialog( wxWindow *p, wxPageSetupData *data )
    : wxPageSetupDialogBase()
{
    Create( p, data );
}

bool wxMacPageSetupDialog::Create( wxWindow *p, wxPageSetupData *data )
{
    m_dialogParent = p;

    if (data != NULL)
        m_pageSetupData = (*data);

    return true;
}

wxMacPageSetupDialog::~wxMacPageSetupDialog()
{
}

wxPageSetupData& wxMacPageSetupDialog::GetPageSetupDialogData()
{
    return m_pageSetupData;
}

int wxMacPageSetupDialog::ShowModal()
{
    m_pageSetupData.GetPrintData().ConvertToNative();
    ((wxMacCarbonPrintData*)m_pageSetupData.GetPrintData().GetNativeData())->TransferFrom( &m_pageSetupData );

    int result = wxID_CANCEL;
    OSErr err = noErr;
    Boolean accepted;

    err = PMSessionPageSetupDialog(
        ((wxMacCarbonPrintData*)m_pageSetupData.GetPrintData().GetNativeData())->m_macPrintSession,
        ((wxMacCarbonPrintData*)m_pageSetupData.GetPrintData().GetNativeData())->m_macPageFormat,
        &accepted );

    if ((err == noErr) && !accepted)
    {
        // user clicked Cancel button
        err = kPMCancel;
    }

    // If the user did not cancel, flatten and save the PageFormat object
    // with our document.
    if (err == noErr)
    {
        result = wxID_OK;
    }

    if ((err != noErr) && (err != kPMCancel))
    {
        wxString message;

        message.Printf( wxT("Print Error %d"), err );
        wxMessageDialog dialog( NULL, message, wxEmptyString, wxICON_HAND | wxOK );
        dialog.ShowModal();
    }

    if (result == wxID_OK)
    {
        m_pageSetupData.GetPrintData().ConvertFromNative();
        m_pageSetupData.SetPaperSize( m_pageSetupData.GetPrintData().GetPaperSize() );
        ((wxMacCarbonPrintData*)m_pageSetupData.GetPrintData().GetNativeData())->TransferTo( &m_pageSetupData );
    }

    return result;
}


IMPLEMENT_CLASS(wxMacPageMarginsDialog, wxDialog)

wxMacPageMarginsDialog::wxMacPageMarginsDialog(wxFrame *parent, wxPageSetupData *data) :
  wxDialog(parent, wxID_ANY, wxString(wxT("Page Margins"))),
  m_pageSetupDialogData(data)
  {
  GetMinMargins();
  wxBoxSizer *colSizer = new wxBoxSizer(wxVERTICAL);
  wxFlexGridSizer *gridSizer = new wxFlexGridSizer(4, 5, 5);
  colSizer->Add(gridSizer, wxSizerFlags().Border(wxALL, 5));
  gridSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Left (mm):")), wxSizerFlags().Right());
  gridSizer->Add(m_LeftMargin = new wxTextCtrl(this, wxID_ANY), wxSizerFlags().Left());
  gridSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Top (mm):")), wxSizerFlags().Right());
  gridSizer->Add(m_TopMargin = new wxTextCtrl(this, wxID_ANY), wxSizerFlags().Left());
  gridSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Right (mm):")), wxSizerFlags().Right());
  gridSizer->Add(m_RightMargin = new wxTextCtrl(this, wxID_ANY), wxSizerFlags().Left());
  gridSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Bottom (mm):")), wxSizerFlags().Right());
  gridSizer->Add(m_BottomMargin = new wxTextCtrl(this, wxID_ANY), wxSizerFlags().Left());
  colSizer->Add(new wxStaticLine(this), wxSizerFlags().Expand().Border(wxTOP|wxBOTTOM, 5));
  colSizer->Add(CreateButtonSizer(wxOK | wxCANCEL), wxSizerFlags().Expand().Border(wxALL, 5));
  TransferToWindow();
  SetSizerAndFit(colSizer);
  Center(wxBOTH);
  }

bool wxMacPageMarginsDialog::TransferToWindow()
  {
  wxASSERT(m_pageSetupDialogData);
  wxPoint topLeft = m_pageSetupDialogData->GetMarginTopLeft();
  wxPoint bottomRight = m_pageSetupDialogData->GetMarginBottomRight();
  wxPoint minTopLeft = m_pageSetupDialogData->GetMinMarginTopLeft();
  wxPoint minBottomRight = m_pageSetupDialogData->GetMinMarginBottomRight();
  m_LeftMargin->SetValue(wxString::Format(wxT("%d"), wxMax(topLeft.x, minTopLeft.x)));
  m_LeftMargin->SetSelection(-1, -1);
  m_TopMargin->SetValue(wxString::Format(wxT("%d"), wxMax(topLeft.y, minTopLeft.y)));
  m_TopMargin->SetSelection(-1, -1);
  m_RightMargin->SetValue(wxString::Format(wxT("%d"), wxMax(bottomRight.x, minBottomRight.x)));
  m_RightMargin->SetSelection(-1, -1);
  m_BottomMargin->SetValue(wxString::Format(wxT("%d"), wxMax(bottomRight.y, minBottomRight.y)));
  m_BottomMargin->SetSelection(-1, -1);
  m_LeftMargin->SetFocus();
  return true;
  }
  
bool wxMacPageMarginsDialog::TransferDataFromWindow()
  {
  wxPoint topLeft, bottomRight;
  if (!CheckValue(m_LeftMargin, &topLeft.x, m_MinMarginTopLeft.x, wxT("left margin"))) return false;
  if (!CheckValue(m_TopMargin, &topLeft.y, m_MinMarginTopLeft.y, wxT("top margin"))) return false;
  if (!CheckValue(m_RightMargin, &bottomRight.x, m_MinMarginBottomRight.x, wxT("right margin"))) return false;
  if (!CheckValue(m_BottomMargin, &bottomRight.y, m_MinMarginBottomRight.y, wxT("bottom margin"))) return false;
  m_pageSetupDialogData->SetMarginTopLeft(topLeft);
  m_pageSetupDialogData->SetMarginBottomRight(bottomRight);
  return true;
  }
  
bool wxMacPageMarginsDialog::CheckValue(wxTextCtrl* textCtrl, int *value, int minValue, const wxString& name)
  {
  long lvalue;
  if (!textCtrl->GetValue().ToLong(&lvalue))
    {
    wxMessageBox(wxString::Format(wxT("Sorry, \"%s\" is not a valid numerical value for the %s"), textCtrl->GetValue().c_str(), name.c_str()), wxT("Page Margin Error"));
    return false;
    }
  if (lvalue < minValue)
    {
    wxMessageBox(wxString::Format(wxT("Sorry, \"%s\" is not a valid value for the %s, which must be >= %d"), textCtrl->GetValue().c_str(), name.c_str(), minValue), wxT("Page Margin Error"));
    textCtrl->SetValue(wxString::Format(wxT("%d"), minValue));
    textCtrl->SetSelection(-1, -1);
    textCtrl->SetFocus();
    return false;
    }
  *value = int(lvalue);
  return true;
  }

void wxMacPageMarginsDialog::GetMinMargins()
  {
  m_MinMarginTopLeft = m_pageSetupDialogData->GetMinMarginTopLeft();
  m_MinMarginBottomRight = m_pageSetupDialogData->GetMinMarginBottomRight();
  }



#endif
