/////////////////////////////////////////////////////////////////////////////
// Name:        prntdlgg.cpp
// Purpose:     Generic print dialogs
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "prntdlgg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/defs.h"

#define WINDOWS_PRINTING    (wxTheApp->GetPrintMode() == wxPRINT_WINDOWS)

#ifndef WX_PRECOMP
#include "wx/utils.h"
#include "wx/dc.h"
#include "wx/app.h"
#include "wx/frame.h"
#include "wx/stattext.h"
#include "wx/button.h"
#include "wx/checkbox.h"
#include "wx/textctrl.h"
#include "wx/radiobox.h"
#include "wx/filedlg.h"
#include "wx/choice.h"
#include <wx/intl.h>
#endif

#include "wx/generic/prntdlgg.h"
#include "wx/printdlg.h"

#include <stdlib.h>
#include <string.h>

#if !USE_SHARED_LIBRARY
IMPLEMENT_CLASS(wxGenericPrintDialog, wxDialog)
IMPLEMENT_CLASS(wxGenericPrintSetupDialog, wxDialog)
IMPLEMENT_CLASS(wxGenericPageSetupDialog, wxDialog)

BEGIN_EVENT_TABLE(wxGenericPrintDialog, wxDialog)
    EVT_BUTTON(wxID_OK, wxGenericPrintDialog::OnOK)
    EVT_BUTTON(wxPRINTID_SETUP, wxGenericPrintDialog::OnSetup)
    EVT_RADIOBOX(wxPRINTID_RANGE, wxGenericPrintDialog::OnRange)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxGenericPageSetupDialog, wxDialog)
    EVT_BUTTON(wxPRINTID_SETUP, wxGenericPageSetupDialog::OnPrinter)
END_EVENT_TABLE()
#endif

extern wxPrintPaperDatabase *wxThePrintPaperDatabase;

/*
 * Generic print dialog for non-Windows printing use.
 */


wxGenericPrintDialog::wxGenericPrintDialog(wxWindow *parent, wxPrintData* data):
  wxDialog(parent, -1, _("Print"), wxPoint(0, 0), wxSize(600, 600), wxDEFAULT_DIALOG_STYLE|wxDIALOG_MODAL)
{
  if ( data )
    printData = *data;
  
  int buttonWidth = 65;
  int buttonHeight = 25;
  int spacing = 5;
  int yPos = 5;
  int xPos = 5;

  wxButton *okButton = new wxButton(this, wxID_OK, _("OK"), wxPoint(5, yPos), wxSize(buttonWidth, buttonHeight));
  (void) new wxButton(this, wxID_CANCEL, _("Cancel"), wxPoint(buttonWidth + 5 + spacing, yPos), wxSize(buttonWidth, buttonHeight));

  setupButton = new wxButton(this, wxPRINTID_SETUP, _("Setup..."), wxPoint(buttonWidth*2 + 5 + 2*spacing, yPos), wxSize(buttonWidth, buttonHeight));

  okButton->SetDefault();
  okButton->SetFocus();

  yPos += 35;

  wxString choices[2];
  choices[0] = _("All");
  choices[1] = _("Pages");
  
  fromText = (wxTextCtrl*)NULL;
  
  if(printData.GetFromPage() != 0)
  {
     rangeRadioBox = new wxRadioBox(this, wxPRINTID_RANGE, _("Print Range"),
                                    wxPoint(5, yPos), wxSize(-1, -1), 2, choices, 2);
     rangeRadioBox->SetSelection(1);
  }
  
  yPos += 60;
  xPos = 5;
  int staticWidth = 45;
  int textWidth = 40;
  spacing = 10;

  if(printData.GetFromPage() != 0)
  {
     (void) new wxStaticText(this, wxPRINTID_STATIC, _("From:"), wxPoint(xPos, yPos));
     xPos += staticWidth;

     fromText = new wxTextCtrl(this, wxPRINTID_FROM, "", wxPoint(xPos, yPos), wxSize(textWidth, -1));
     xPos += spacing + textWidth;
     
     (void) new wxStaticText(this, wxPRINTID_STATIC, _("To:"), wxPoint(xPos, yPos));
     xPos += staticWidth;
     
     toText = new wxTextCtrl(this, wxPRINTID_TO, "", wxPoint(xPos, yPos), wxSize(textWidth, -1));
     xPos += spacing + textWidth;
  }
  
  (void) new wxStaticText(this, wxPRINTID_STATIC, _("Copies:"), wxPoint(xPos, yPos));
  xPos += spacing + staticWidth;

  noCopiesText = new wxTextCtrl(this, wxPRINTID_COPIES, "", wxPoint(xPos, yPos), wxSize(textWidth, -1));

  yPos += 30;
  xPos = 5;

  printToFileCheckBox = new wxCheckBox(this, wxPRINTID_PRINTTOFILE, _("Print to File"), wxPoint(xPos, yPos));

  Fit();
  Centre(wxBOTH);

  // Calls wxWindow::OnInitDialog and then wxGenericPrintDialog::TransferDataToWindow
  InitDialog();
}

int wxGenericPrintDialog::ShowModal(void)
{
    if ( printData.GetSetupDialog() )
    {
        wxGenericPrintSetupDialog *genericPrintSetupDialog =
            new wxGenericPrintSetupDialog(GetParent(), wxThePrintSetupData);
        int ret = genericPrintSetupDialog->ShowModal();
        if ( ret != wxID_CANCEL )
        {
            *wxThePrintSetupData = genericPrintSetupDialog->printData;
        }
        genericPrintSetupDialog->Close(TRUE);
        return ret;
    }
    else
    {
        return wxDialog::ShowModal();
    }
}

wxGenericPrintDialog::~wxGenericPrintDialog(void)
{
}

void wxGenericPrintDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
  TransferDataFromWindow();

  // There are some interactions between the global setup data
  // and the standard print dialog. The global printing 'mode'
  // is determined by whether the user checks Print to file
  // or not.
  if (printData.GetPrintToFile())
  {
    wxThePrintSetupData->SetPrinterMode(PS_FILE);

    char *f = wxFileSelector(_("PostScript file"),
        wxPathOnly(wxThePrintSetupData->GetPrinterFile()),
        wxFileNameFromPath(wxThePrintSetupData->GetPrinterFile()),
        "ps", "*.ps", 0, this);
    if (f)
      wxThePrintSetupData->SetPrinterFile(f);
    else
      return;
  }
  else
    wxThePrintSetupData->SetPrinterMode(PS_PRINTER);
  
  EndModal(wxID_OK);
}

void wxGenericPrintDialog::OnRange(wxCommandEvent& event)
{
  if (!fromText) return;

  if (event.GetInt() == 1)
  {
    fromText->Enable(FALSE);
    toText->Enable(FALSE);
  }
  else if (event.GetInt() == 0)
  {
    fromText->Enable(TRUE);
    toText->Enable(TRUE);
  }
}

void wxGenericPrintDialog::OnSetup(wxCommandEvent& WXUNUSED(event))
{
  wxGenericPrintSetupDialog *genericPrintSetupDialog =
          new wxGenericPrintSetupDialog(this, wxThePrintSetupData);
  int ret = genericPrintSetupDialog->ShowModal();
  if ( ret != wxID_CANCEL )
  {
    *wxThePrintSetupData = genericPrintSetupDialog->printData;
    printData.SetOrientation(wxThePrintSetupData->GetPrinterOrientation());
  }

  genericPrintSetupDialog->Close(TRUE);
}

bool wxGenericPrintDialog::TransferDataToWindow(void)
{
   char buf[10];

   if(printData.GetFromPage() != 0)
   {
      if (printData.GetEnablePageNumbers())
      {
         fromText->Enable(TRUE);
         toText->Enable(TRUE);
         
         sprintf(buf, "%d", printData.GetFromPage());
         fromText->SetValue(buf);
         sprintf(buf, "%d", printData.GetFromPage());
         toText->SetValue(buf);
         
         if (printData.GetAllPages())
            rangeRadioBox->SetSelection(0);
         else
            rangeRadioBox->SetSelection(1);
      }
      else
      {
         fromText->Enable(FALSE);
         toText->Enable(FALSE);
         rangeRadioBox->SetSelection(0);
         rangeRadioBox->wxRadioBox::Enable(1, FALSE);
      }
   }
  sprintf(buf, "%d", printData.GetNoCopies());
  noCopiesText->SetValue(buf);

  printToFileCheckBox->SetValue(printData.GetPrintToFile());
  printToFileCheckBox->Enable(printData.GetEnablePrintToFile());
  return TRUE;
}

bool wxGenericPrintDialog::TransferDataFromWindow(void)
{
   if(printData.GetFromPage() != -1)
   {
      if (printData.GetEnablePageNumbers())
      {
         printData.SetFromPage(atoi(fromText->GetValue()));
         printData.SetToPage(atoi(toText->GetValue()));
      }
      if (rangeRadioBox->GetSelection() == 0)
         printData.SetAllPages(TRUE);
      else
         printData.SetAllPages(FALSE);
   }
   else
   { // continuous printing
      printData.SetFromPage(1);
      printData.SetToPage(32000);
   }
   printData.SetNoCopies(atoi(noCopiesText->GetValue()));
   printData.SetPrintToFile(printToFileCheckBox->GetValue());

  return TRUE;
}

wxDC *wxGenericPrintDialog::GetPrintDC(void)
{
  return new wxPostScriptDC(wxThePrintSetupData->GetPrinterFile(), FALSE, NULL);
}

/*
 * Generic print setup dialog
 */

wxGenericPrintSetupDialog::wxGenericPrintSetupDialog(wxWindow *parent, wxPrintSetupData* data):
  wxDialog(parent, -1, _("Print Setup"), wxPoint(0, 0), wxSize(600, 600), wxDEFAULT_DIALOG_STYLE|wxDIALOG_MODAL)
{
  if ( data )
    printData = *data;

  int buttonWidth = 65;
  int buttonHeight = 25;
  int spacing = 5;
  int yPos = 5;
  int xPos = 5;

  wxButton *okButton = new wxButton(this, wxID_OK, _("OK"), wxPoint(xPos, yPos), wxSize(buttonWidth, buttonHeight));
  xPos += buttonWidth + spacing;
  (void) new wxButton(this, wxID_CANCEL, _("Cancel"), wxPoint(xPos, yPos), wxSize(buttonWidth, buttonHeight));

  okButton->SetDefault();
  okButton->SetFocus();

  yPos += 35;
  xPos = 5;

  paperTypeChoice = CreatePaperTypeChoice(&xPos, &yPos);

  wxString choices[2];
  choices[0] = _("Portrait");
  choices[1] = _("Landscape");

  orientationRadioBox = new wxRadioBox(this, wxPRINTID_ORIENTATION, _("Orientation"),
    wxPoint(xPos, yPos), wxSize(-1, -1), 2, choices, 2);
  orientationRadioBox->SetSelection(0);

  xPos += 200;

  colourCheckBox = new wxCheckBox(this, wxPRINTID_PRINTCOLOUR, _("Print in colour"), wxPoint(xPos, yPos));

  xPos = 5;
  yPos += 60;

  int staticWidth = 100;
  int textWidth = 120;
  spacing = 10;

  (void) new wxStaticText(this, wxPRINTID_STATIC, _("Printer command:"), wxPoint(xPos, yPos));
  xPos += staticWidth;

  printerCommandText = new wxTextCtrl(this, wxPRINTID_COMMAND, "", wxPoint(xPos, yPos), wxSize(textWidth, -1));
  xPos += textWidth + spacing;

  (void) new wxStaticText(this, wxPRINTID_STATIC, _("Printer options:"), wxPoint(xPos, yPos));
  xPos += staticWidth;

  printerOptionsText = new wxTextCtrl(this, wxPRINTID_OPTIONS, "", wxPoint(xPos, yPos), wxSize(textWidth, -1));

  Fit();
  Centre(wxBOTH);

  InitDialog();
}

wxGenericPrintSetupDialog::~wxGenericPrintSetupDialog(void)
{
}

bool wxGenericPrintSetupDialog::TransferDataToWindow(void)
{
  if (printerCommandText && printData.GetPrinterCommand())
    printerCommandText->SetValue(printData.GetPrinterCommand());
  if (printerOptionsText && printData.GetPrinterOptions())
    printerOptionsText->SetValue(printData.GetPrinterOptions());
  if (colourCheckBox)
    colourCheckBox->SetValue(printData.GetColour());

  if (orientationRadioBox)
  {
    if (printData.GetPrinterOrientation() == PS_PORTRAIT)
      orientationRadioBox->SetSelection(0);
    else
      orientationRadioBox->SetSelection(1);
  }
  return TRUE;
}

bool wxGenericPrintSetupDialog::TransferDataFromWindow(void)
{
  if (printerCommandText)
    printData.SetPrinterCommand(WXSTRINGCAST printerCommandText->GetValue());
  if (printerOptionsText)
    printData.SetPrinterOptions(WXSTRINGCAST printerOptionsText->GetValue());
  if (colourCheckBox)
    printData.SetColour(colourCheckBox->GetValue());
  if (orientationRadioBox)
  {
    int sel = orientationRadioBox->GetSelection();
    if (sel == 0)
      printData.SetPrinterOrientation(PS_PORTRAIT);
    else
      printData.SetPrinterOrientation(PS_LANDSCAPE);
  }
  if (paperTypeChoice)
  {
    wxString val(paperTypeChoice->GetStringSelection());
    if (!val.IsNull() && val != "")
      printData.SetPaperName((char *)(const char *)val);
  }
  return TRUE;
}

wxChoice *wxGenericPrintSetupDialog::CreatePaperTypeChoice(int *x, int *y)
{
  if (!wxThePrintPaperDatabase)
  {
    wxThePrintPaperDatabase = new wxPrintPaperDatabase;
    wxThePrintPaperDatabase->CreateDatabase();
  }
  int n = wxThePrintPaperDatabase->Number();
  wxString *choices = new wxString [n];
  int sel = 0;
  int i;
  for (i = 0; i < n; i++)
  {
    wxPrintPaperType *paper = (wxPrintPaperType *)wxThePrintPaperDatabase->Nth(i)->Data();
    choices[i] = paper->pageName;
    if (printData.GetPaperName() && choices[i] == printData.GetPaperName())
      sel = i;
  }

  (void) new wxStaticText(this, wxPRINTID_STATIC, _("Paper size"), wxPoint(*x, *y));
  *y += 25;

  wxChoice *choice = new wxChoice(this, wxPRINTID_PAPERSIZE, wxPoint(*x, *y), wxSize(300, -1), n,
    choices);
  *y += 35;
  delete[] choices;

  choice->SetSelection(sel);
  return choice;
}

/*
 * Generic page setup dialog
 */

void wxGenericPageSetupDialog::OnPrinter(wxCommandEvent& WXUNUSED(event))
{
  if (wxTheApp->GetPrintMode() == wxPRINT_POSTSCRIPT)
  {
    wxGenericPrintSetupDialog *genericPrintSetupDialog =
          new wxGenericPrintSetupDialog(this, wxThePrintSetupData);
    int ret = genericPrintSetupDialog->ShowModal();
    if (ret == wxID_OK)
      *wxThePrintSetupData = genericPrintSetupDialog->GetPrintData();

    genericPrintSetupDialog->Close(TRUE);
  }
#ifdef __WXMSW__
  else
  {
    wxPrintData data;
    data.SetSetupDialog(TRUE);
    wxPrintDialog printDialog(this, & data);
    printDialog.Show(TRUE);
  }
#endif
}

wxGenericPageSetupDialog::wxGenericPageSetupDialog(wxWindow *parent, wxPageSetupData* data):
  wxDialog(parent, -1, _("Page Setup"), wxPoint(0, 0), wxSize(600, 600), wxDIALOG_MODAL|wxDEFAULT_DIALOG_STYLE)
{
  if ( data )
    pageData = *data;

  int buttonWidth = 75;
  int buttonHeight = 25;
  int spacing = 5;
  int yPos = 5;
  int xPos = 5;

  wxButton *okButton = new wxButton(this, wxID_OK, _("OK"), wxPoint(5, yPos), wxSize(buttonWidth, buttonHeight));
  (void) new wxButton(this, wxID_CANCEL, _("Cancel"), wxPoint(buttonWidth + 5 + spacing, yPos), wxSize(buttonWidth, buttonHeight));

  printerButton = new wxButton(this, wxPRINTID_SETUP, _("Printer..."), wxPoint(buttonWidth*2 + 5 + 2*spacing, yPos), wxSize(buttonWidth, buttonHeight));

  if ( !pageData.GetEnablePrinter() )
    printerButton->Enable(FALSE);

//  if (printData.GetEnableHelp())
//  wxButton *helpButton = new wxButton(this, (wxFunction)wxGenericPageSetupHelpProc, _("Help"), -1, -1, buttonWidth, buttonHeight);

  okButton->SetDefault();
  okButton->SetFocus();

  xPos = 5;
  yPos += 35;

  paperTypeChoice = CreatePaperTypeChoice(&xPos, &yPos);

  xPos = 5;

  wxString choices[2];
  choices[0] = _("Portrait");
  choices[1] = _("Landscape");
  orientationRadioBox = new wxRadioBox(this, wxPRINTID_ORIENTATION, _("Orientation"),
    wxPoint(xPos, yPos), wxSize(-1, -1), 2, choices, 2);
  orientationRadioBox->SetSelection(0);

  xPos = 5;
  yPos += 60;

  int staticWidth = 110;
  int textWidth = 60;
  spacing = 10;

  (void) new wxStaticText(this, wxPRINTID_STATIC, _("Left margin (mm):"), wxPoint(xPos, yPos));
  xPos += staticWidth;

  marginLeftText = new wxTextCtrl(this, wxPRINTID_LEFTMARGIN, "", wxPoint(xPos, yPos), wxSize(textWidth, -1));
  xPos += textWidth + spacing;

  (void) new wxStaticText(this, wxPRINTID_STATIC, _("Right margin (mm):"), wxPoint(xPos, yPos));
  xPos += staticWidth;

  marginRightText = new wxTextCtrl(this, wxPRINTID_RIGHTMARGIN, "", wxPoint(xPos, yPos), wxSize(textWidth, -1));
  xPos += textWidth + spacing;

  yPos += 35;
  xPos = 5;

  (void) new wxStaticText(this, wxPRINTID_STATIC, _("Top margin (mm):"), wxPoint(xPos, yPos));
  xPos += staticWidth;

  marginTopText = new wxTextCtrl(this, wxPRINTID_TOPMARGIN, "", wxPoint(xPos, yPos), wxSize(textWidth, -1));
  xPos += textWidth + spacing;

  (void) new wxStaticText(this, wxPRINTID_STATIC, _("Bottom margin (mm):"), wxPoint(xPos, yPos));
  xPos += staticWidth;

  marginBottomText = new wxTextCtrl(this, wxPRINTID_BOTTOMMARGIN, "", wxPoint(xPos, yPos), wxSize(textWidth, -1));

  Fit();
  Centre(wxBOTH);

  InitDialog();
}

wxGenericPageSetupDialog::~wxGenericPageSetupDialog(void)
{
}

bool wxGenericPageSetupDialog::TransferDataToWindow(void)
{
  if (marginLeftText)
    marginLeftText->SetValue(IntToString((int) pageData.GetMarginTopLeft().x));
  if (marginTopText)
    marginTopText->SetValue(IntToString((int) pageData.GetMarginTopLeft().y));
  if (marginRightText)
    marginRightText->SetValue(IntToString((int) pageData.GetMarginBottomRight().x));
  if (marginBottomText)
    marginBottomText->SetValue(IntToString((int) pageData.GetMarginBottomRight().y));

  if (orientationRadioBox)
  {
    if (pageData.GetOrientation() == wxPORTRAIT)
      orientationRadioBox->SetSelection(0);
    else
      orientationRadioBox->SetSelection(1);
  }
  return TRUE;
}

bool wxGenericPageSetupDialog::TransferDataFromWindow(void)
{
  if (marginLeftText && marginTopText)
    pageData.SetMarginTopLeft(wxPoint(atoi((const char *)marginLeftText->GetValue()),atoi((const char *)marginTopText->GetValue())));
  if (marginRightText && marginBottomText)
    pageData.SetMarginBottomRight(wxPoint(atoi((const char *)marginRightText->GetValue()),atoi((const char *)marginBottomText->GetValue())));

  if (orientationRadioBox)
  {
    int sel = orientationRadioBox->GetSelection();
    if (sel == 0)
    {
      wxThePrintSetupData->SetPrinterOrientation(wxPORTRAIT);
      pageData.SetOrientation(wxPORTRAIT);
    }
    else
    {
      wxThePrintSetupData->SetPrinterOrientation(wxLANDSCAPE);
      pageData.SetOrientation(wxLANDSCAPE);
    }
  }
  if (paperTypeChoice)
  {
    wxString val(paperTypeChoice->GetStringSelection());
    if (!val.IsNull() && val != "")
    {
        wxPrintPaperType* paper = wxThePrintPaperDatabase->FindPaperType((char*) (const char *)val);
        if ( paper )
        {
            pageData.SetPaperSize(wxPoint(paper->widthMM, paper->heightMM));
        }
    }
  }
  return TRUE;
}

wxChoice *wxGenericPageSetupDialog::CreatePaperTypeChoice(int *x, int *y)
{
  if (!wxThePrintPaperDatabase)
  {
    wxThePrintPaperDatabase = new wxPrintPaperDatabase;
    wxThePrintPaperDatabase->CreateDatabase();
  }
  int n = wxThePrintPaperDatabase->Number();
  wxString *choices = new wxString [n];
  int sel = 0;
  int i;
  for (i = 0; i < n; i++)
  {
    wxPrintPaperType *paper = (wxPrintPaperType *)wxThePrintPaperDatabase->Nth(i)->Data();
    choices[i] = paper->pageName;
    if (pageData.GetPaperSize().x == paper->widthMM && pageData.GetPaperSize().y == paper->heightMM)
      sel = i;
  }

  (void) new wxStaticText(this, wxPRINTID_STATIC, _("Paper size"), wxPoint(*x, *y));
  *y += 25;

  wxChoice *choice = new wxChoice(this, wxPRINTID_PAPERSIZE, wxPoint(*x, *y), wxSize(300, -1), n,
    choices);
  *y += 35;
  delete[] choices;

  choice->SetSelection(sel);
  return choice;
}


