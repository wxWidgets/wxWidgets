/////////////////////////////////////////////////////////////////////////////
// Name:        prntdlgg.cpp
// Purpose:     Generic print dialogs
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "prntdlgg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/defs.h"

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/dc.h"
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/stattext.h"
    #include "wx/statbox.h"
    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/textctrl.h"
    #include "wx/radiobox.h"
    #include "wx/filedlg.h"
    #include "wx/choice.h"
    #include <wx/intl.h>
#endif

#include "wx/generic/prntdlgg.h"

#if wxUSE_POSTSCRIPT
    #include "wx/generic/dcpsg.h"
#endif

#include "wx/printdlg.h"
#include "wx/paper.h"

// For print paper things
#include "wx/prntbase.h"

#include <stdlib.h>
#include <string.h>

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

#if wxUSE_POSTSCRIPT

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
#endif // USE_SHARED_LIBRARY

// ----------------------------------------------------------------------------
// global vars
// ----------------------------------------------------------------------------

extern wxPrintPaperDatabase *wxThePrintPaperDatabase;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// Generic print dialog for non-Windows printing use.
// ----------------------------------------------------------------------------

wxGenericPrintDialog::wxGenericPrintDialog(wxWindow *parent,
                                           wxPrintDialogData* data)
                    : wxDialog(parent, -1, _("Print"),
                               wxPoint(0, 0), wxSize(600, 600),
                               wxDEFAULT_DIALOG_STYLE |
                               wxDIALOG_MODAL |
                               wxTAB_TRAVERSAL)
{
    if ( data )
        m_printDialogData = *data;

    Init(parent);
}

wxGenericPrintDialog::wxGenericPrintDialog(wxWindow *parent,
                                           wxPrintData* data)
{
    if ( data )
        m_printDialogData = *data;

    Init(parent);
}

void wxGenericPrintDialog::Init(wxWindow *parent)
{
    wxDialog::Create(parent, -1, _("Print"), wxPoint(0, 0), wxSize(600, 600),
                     wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL | wxTAB_TRAVERSAL);

    (void)new wxStaticBox( this, -1, _( "Printer options" ), wxPoint( 5, 5), wxSize( 300, 60 ) );

    m_printToFileCheckBox = new wxCheckBox(this, wxPRINTID_PRINTTOFILE, _("Print to File"), wxPoint(20, 25) );

    m_setupButton = new wxButton(this, wxPRINTID_SETUP, _("Setup..."), wxPoint(160, 25), wxSize(100, -1));

    wxString *choices = new wxString[2];
    choices[0] = _("All");
    choices[1] = _("Pages");

    m_fromText = (wxTextCtrl*)NULL;

    if (m_printDialogData.GetFromPage() != 0)
    {
        m_rangeRadioBox = new wxRadioBox(this, wxPRINTID_RANGE, _("Print Range"),
                                         wxPoint(5, 80), wxSize(-1, -1),
                                         2, choices,
                                         1, wxRA_VERTICAL);
        m_rangeRadioBox->SetSelection(1);
    }

    if(m_printDialogData.GetFromPage() != 0)
    {
        (void) new wxStaticText(this, wxPRINTID_STATIC, _("From:"), wxPoint(5, 135));

        m_fromText = new wxTextCtrl(this, wxPRINTID_FROM, "", wxPoint(45, 130), wxSize(40, -1));

        (void) new wxStaticText(this, wxPRINTID_STATIC, _("To:"), wxPoint(100, 135));

        m_toText = new wxTextCtrl(this, wxPRINTID_TO, "", wxPoint(133, 130), wxSize(40, -1));
    }

    (void) new wxStaticText(this, wxPRINTID_STATIC, _("Copies:"), wxPoint(200, 135));

    m_noCopiesText = new wxTextCtrl(this, wxPRINTID_COPIES, "", wxPoint(252, 130), wxSize(40, -1));

    wxButton *okButton = new wxButton(this, wxID_OK, _("OK"), wxPoint(40, 180), wxSize(100, -1));
    (void) new wxButton(this, wxID_CANCEL, _("Cancel"), wxPoint(180, 180), wxSize(100, -1));

    okButton->SetDefault();
    okButton->SetFocus();
    Fit();
    Centre(wxBOTH);

    // Calls wxWindow::OnInitDialog and then wxGenericPrintDialog::TransferDataToWindow
    InitDialog();
    delete[] choices;
}

int wxGenericPrintDialog::ShowModal()
{
    if ( m_printDialogData.GetSetupDialog() )
    {
        // Make sure wxPrintData object reflects the settings now, in case the setup dialog
        // changes it. In fact there aren't any common settings at
        // present, but there might be in future.
        // TransferDataFromWindow();

        wxGenericPrintSetupDialog *genericPrintSetupDialog =
            new wxGenericPrintSetupDialog(this, & m_printDialogData.GetPrintData());
        int ret = genericPrintSetupDialog->ShowModal();
        if ( ret != wxID_CANCEL )
        {
            // Transfer settings to the global object (for compatibility) and to
            // the print dialog's print data.
            *wxThePrintSetupData = genericPrintSetupDialog->GetPrintData();
            m_printDialogData.GetPrintData() = genericPrintSetupDialog->GetPrintData();
        }
        genericPrintSetupDialog->Destroy();

        // Restore the wxPrintData settings again (uncomment if any settings become common
        // to both dialogs)
        // TransferDataToWindow();

        return ret;
    }
    else
    {
        return wxDialog::ShowModal();
    }
}

wxGenericPrintDialog::~wxGenericPrintDialog()
{
}

void wxGenericPrintDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
    TransferDataFromWindow();

    // There are some interactions between the global setup data
    // and the standard print dialog. The global printing 'mode'
    // is determined by whether the user checks Print to file
    // or not.
    if (m_printDialogData.GetPrintToFile())
    {
        m_printDialogData.GetPrintData().SetPrintMode(wxPRINT_MODE_FILE);
        wxThePrintSetupData->SetPrinterMode(wxPRINT_MODE_FILE);

        wxString f = wxFileSelector(_("PostScript file"),
            wxPathOnly(wxThePrintSetupData->GetPrinterFile()),
            wxFileNameFromPath(wxThePrintSetupData->GetPrinterFile()),
            "ps", "*.ps", 0, this);
        if ( f.IsEmpty() )
            return;

        m_printDialogData.GetPrintData().SetFilename(f);
        wxThePrintSetupData->SetPrinterFile(f);
    }
    else
        wxThePrintSetupData->SetPrinterMode(wxPRINT_MODE_PRINTER);

    EndModal(wxID_OK);
}

void wxGenericPrintDialog::OnRange(wxCommandEvent& event)
{
    if (!m_fromText) return;

    if (event.GetInt() == 0)
    {
        m_fromText->Enable(FALSE);
        m_toText->Enable(FALSE);
    }
    else if (event.GetInt() == 1)
    {
        m_fromText->Enable(TRUE);
        m_toText->Enable(TRUE);
    }
}

void wxGenericPrintDialog::OnSetup(wxCommandEvent& WXUNUSED(event))
{
    wxGenericPrintSetupDialog *genericPrintSetupDialog =
        new wxGenericPrintSetupDialog(this, wxThePrintSetupData);
    int ret = genericPrintSetupDialog->ShowModal();
    if ( ret != wxID_CANCEL )
    {
        *wxThePrintSetupData = genericPrintSetupDialog->GetPrintData();
        m_printDialogData = genericPrintSetupDialog->GetPrintData();
    }

    genericPrintSetupDialog->Close(TRUE);
}

bool wxGenericPrintDialog::TransferDataToWindow()
{
    char buf[10];

    if(m_printDialogData.GetFromPage() != 0)
    {
        if (m_printDialogData.GetEnablePageNumbers())
        {
            m_fromText->Enable(TRUE);
            m_toText->Enable(TRUE);

            sprintf(buf, "%d", m_printDialogData.GetFromPage());
            m_fromText->SetValue(buf);
            sprintf(buf, "%d", m_printDialogData.GetToPage());
            m_toText->SetValue(buf);

            if (m_printDialogData.GetAllPages())
                m_rangeRadioBox->SetSelection(0);
            else
                m_rangeRadioBox->SetSelection(1);
        }
        else
        {
            m_fromText->Enable(FALSE);
            m_toText->Enable(FALSE);
            m_rangeRadioBox->SetSelection(0);
            m_rangeRadioBox->wxRadioBox::Enable(1, FALSE);
        }
    }
    sprintf(buf, "%d", m_printDialogData.GetNoCopies());
    m_noCopiesText->SetValue(buf);

    m_printToFileCheckBox->SetValue(m_printDialogData.GetPrintToFile());
    m_printToFileCheckBox->Enable(m_printDialogData.GetEnablePrintToFile());
    return TRUE;
}

bool wxGenericPrintDialog::TransferDataFromWindow()
{
    if(m_printDialogData.GetFromPage() != -1)
    {
        if (m_printDialogData.GetEnablePageNumbers())
        {
            m_printDialogData.SetFromPage(atoi(m_fromText->GetValue()));
            m_printDialogData.SetToPage(atoi(m_toText->GetValue()));
        }
        if (m_rangeRadioBox->GetSelection() == 0)
            m_printDialogData.SetAllPages(TRUE);
        else
            m_printDialogData.SetAllPages(FALSE);
    }
    else
    { // continuous printing
        m_printDialogData.SetFromPage(1);
        m_printDialogData.SetToPage(32000);
    }
    m_printDialogData.SetNoCopies(atoi(m_noCopiesText->GetValue()));
    m_printDialogData.SetPrintToFile(m_printToFileCheckBox->GetValue());

    return TRUE;
}

/*
TODO: collate and noCopies should be duplicated across dialog data and print data objects
(slightly different semantics on Windows but let's ignore this for a bit).
*/

wxDC *wxGenericPrintDialog::GetPrintDC()
{
    return new wxPostScriptDC(wxThePrintSetupData->GetPrinterFile(), FALSE, (wxWindow *) NULL);
}

// ----------------------------------------------------------------------------
// Generic print setup dialog
// ----------------------------------------------------------------------------

wxGenericPrintSetupDialog::wxGenericPrintSetupDialog(wxWindow *parent, wxPrintData* data):
wxDialog(parent, -1, _("Print Setup"), wxPoint(0, 0), wxSize(600, 600), wxDEFAULT_DIALOG_STYLE|wxDIALOG_MODAL|wxTAB_TRAVERSAL)
{
    Init(data);
}

// Convert wxPrintSetupData to standard wxPrintData object
wxGenericPrintSetupDialog::wxGenericPrintSetupDialog(wxWindow *parent, wxPrintSetupData* data):
wxDialog(parent, -1, _("Print Setup"), wxPoint(0, 0), wxSize(600, 600), wxDEFAULT_DIALOG_STYLE|wxDIALOG_MODAL|wxTAB_TRAVERSAL)
{
    wxPrintData printData;
    if (data)
        printData = * data;
    else
        printData = * wxThePrintSetupData;

    Init(& printData);
}

void wxGenericPrintSetupDialog::Init(wxPrintData* data)
{
    if ( data )
        m_printData = *data;

    int staticBoxWidth = 300;

    (void) new wxStaticBox(this, wxPRINTID_STATIC, _("Paper size"), wxPoint(10, 10), wxSize(staticBoxWidth, 60) );

    int xPos = 20;
    int yPos = 30;
    m_paperTypeChoice = CreatePaperTypeChoice(&xPos, &yPos);

    wxString *choices =  new wxString[2];
    choices[0] = _("Portrait");
    choices[1] = _("Landscape");

    m_orientationRadioBox = new wxRadioBox(this, wxPRINTID_ORIENTATION, _("Orientation"),
        wxPoint(10, 80), wxSize(-1, -1), 2, choices, 1, wxRA_VERTICAL );
    m_orientationRadioBox->SetSelection(0);

    (void) new wxStaticBox(this, wxPRINTID_STATIC, _("Options"), wxPoint(10, 130), wxSize(staticBoxWidth, 50) );

    int colourYPos = 145;

#ifdef __WXMOTIF__
    colourYPos = 150;
#endif

    m_colourCheckBox = new wxCheckBox(this, wxPRINTID_PRINTCOLOUR, _("Print in colour"), wxPoint(15, colourYPos));

    (void) new wxStaticBox(this, wxPRINTID_STATIC, _("Print spooling"), wxPoint(330, 10), wxSize(200,170) );

    (void) new wxStaticText(this, wxPRINTID_STATIC, _("Printer command:"), wxPoint(340, 30));

    m_printerCommandText = new wxTextCtrl(this, wxPRINTID_COMMAND, "", wxPoint(360, 55), wxSize(150, -1));

    (void) new wxStaticText(this, wxPRINTID_STATIC, _("Printer options:"), wxPoint(340, 110));

    m_printerOptionsText = new wxTextCtrl(this, wxPRINTID_OPTIONS, "", wxPoint(360, 135), wxSize(150, -1));

    wxButton *okButton = new wxButton(this, wxID_OK, _("OK"), wxPoint(130, 200), wxSize(100, -1));
    (void) new wxButton(this, wxID_CANCEL, _("Cancel"), wxPoint(320, 200), wxSize(100, -1));

    okButton->SetDefault();
    okButton->SetFocus();

    Fit();
    Centre(wxBOTH);

    InitDialog();
    delete[] choices;
}

wxGenericPrintSetupDialog::~wxGenericPrintSetupDialog()
{
}

bool wxGenericPrintSetupDialog::TransferDataToWindow()
{
    if (m_printerCommandText && m_printData.GetPrinterCommand())
        m_printerCommandText->SetValue(m_printData.GetPrinterCommand());
    if (m_printerOptionsText && m_printData.GetPrinterOptions())
        m_printerOptionsText->SetValue(m_printData.GetPrinterOptions());
    if (m_colourCheckBox)
        m_colourCheckBox->SetValue(m_printData.GetColour());

    if (m_orientationRadioBox)
    {
        if (m_printData.GetOrientation() == wxPORTRAIT)
            m_orientationRadioBox->SetSelection(0);
        else
            m_orientationRadioBox->SetSelection(1);
    }
    return TRUE;
}

bool wxGenericPrintSetupDialog::TransferDataFromWindow()
{
    if (m_printerCommandText)
        m_printData.SetPrinterCommand(m_printerCommandText->GetValue());
    if (m_printerOptionsText)
        m_printData.SetPrinterOptions(m_printerOptionsText->GetValue());
    if (m_colourCheckBox)
        m_printData.SetColour(m_colourCheckBox->GetValue());
    if (m_orientationRadioBox)
    {
        int sel = m_orientationRadioBox->GetSelection();
        if (sel == 0)
            m_printData.SetOrientation(wxPORTRAIT);
        else
            m_printData.SetOrientation(wxLANDSCAPE);
    }
    if (m_paperTypeChoice)
    {
        wxString val(m_paperTypeChoice->GetStringSelection());
        if (!val.IsNull() && val != "")
            m_printData.SetPaperId(wxThePrintPaperDatabase->ConvertNameToId(val));
    }

    // This is for backward compatibility only
    *wxThePrintSetupData = GetPrintData();
    return TRUE;
}

wxChoice *wxGenericPrintSetupDialog::CreatePaperTypeChoice(int *x, int *y)
{
/* Should not be necessary
    if (!wxThePrintPaperDatabase)
    {
        wxThePrintPaperDatabase = new wxPrintPaperDatabase;
        wxThePrintPaperDatabase->CreateDatabase();
    }
*/
    int n = wxThePrintPaperDatabase->Number();
    wxString *choices = new wxString [n];
    int sel = 0;
    int i;
    for (i = 0; i < n; i++)
    {
        wxPrintPaperType *paper = (wxPrintPaperType *)wxThePrintPaperDatabase->Nth(i)->Data();
        choices[i] = paper->GetName();
        if (m_printData.GetPaperId() == paper->GetId())
            sel = i;
    }

    int width = 250;

    wxChoice *choice = new wxChoice(this, wxPRINTID_PAPERSIZE, wxPoint(*x, *y), wxSize(width, -1), n,
        choices);

    //    SetFont(thisFont);

    delete[] choices;

    choice->SetSelection(sel);
    return choice;
}
#endif // wxUSE_POSTSCRIPT

// ----------------------------------------------------------------------------
// Generic page setup dialog
// ----------------------------------------------------------------------------

void wxGenericPageSetupDialog::OnPrinter(wxCommandEvent& WXUNUSED(event))
{
    // We no longer query GetPrintMode, so we can eliminate the need
    // to call SetPrintMode.
    // This has the limitation that we can't explicitly call the PostScript
    // print setup dialog from the generic Page Setup dialog under Windows,
    // but since this choice would only happen when trying to do PostScript
    // printing under Windows (and only in 16-bit Windows which
    // doesn't have a Windows-specific page setup dialog) it's worth it.

    // First save the current settings, so the wxPrintData object is up to date.
    TransferDataFromWindow();

    // Transfer the current print settings from this dialog to the page setup dialog.
    wxPrintDialogData data;
    data = GetPageSetupData().GetPrintData();
    data.SetSetupDialog(TRUE);
    wxPrintDialog *printDialog = new wxPrintDialog(this, & data);
    printDialog->ShowModal();

    // Transfer the page setup print settings from the page dialog to this dialog again, in case
    // the page setup dialog changed something.
    GetPageSetupData().GetPrintData() = printDialog->GetPrintDialogData().GetPrintData();
    GetPageSetupData().CalculatePaperSizeFromId(); // Make sure page size reflects the id in wxPrintData

    printDialog->Destroy();

    // Now update the dialog in case the page setup dialog changed some of our settings.
    TransferDataToWindow();
}

wxGenericPageSetupDialog::wxGenericPageSetupDialog(wxWindow *parent, wxPageSetupData* data):
wxDialog(parent, -1, _("Page Setup"), wxPoint(0, 0), wxSize(600, 600), wxDIALOG_MODAL|wxDEFAULT_DIALOG_STYLE|wxTAB_TRAVERSAL)
{
    if ( data )
        m_pageData = *data;

    int buttonWidth = 75;
    int buttonHeight = 25;
    int spacing = 5;
#ifdef __WXMOTIF__
    spacing = 15;
#endif

    int yPos = 5;
    int xPos = 5;

    wxButton *okButton = new wxButton(this, wxID_OK, _("OK"), wxPoint(5, yPos), wxSize(buttonWidth, buttonHeight));
    (void) new wxButton(this, wxID_CANCEL, _("Cancel"), wxPoint(buttonWidth + 5 + spacing, yPos), wxSize(buttonWidth, buttonHeight));

    m_printerButton = new wxButton(this, wxPRINTID_SETUP, _("Printer..."), wxPoint(buttonWidth*2 + 5 + 2*spacing, yPos), wxSize(buttonWidth, buttonHeight));

    if ( !m_pageData.GetEnablePrinter() )
        m_printerButton->Enable(FALSE);

    //  if (m_printData.GetEnableHelp())
    //  wxButton *helpButton = new wxButton(this, (wxFunction)wxGenericPageSetupHelpProc, _("Help"), -1, -1, buttonWidth, buttonHeight);

    okButton->SetDefault();
    okButton->SetFocus();

    xPos = 5;
    yPos += 35;

#ifdef __WXMOTIF__
    yPos += 10;
#endif

    m_paperTypeChoice = CreatePaperTypeChoice(&xPos, &yPos);

    xPos = 5;

    wxString *choices = new wxString[2];
    choices[0] = _("Portrait");
    choices[1] = _("Landscape");
    m_orientationRadioBox = new wxRadioBox(this, wxPRINTID_ORIENTATION, _("Orientation"),
        wxPoint(xPos, yPos), wxSize(-1, -1), 2, choices, 2);
    m_orientationRadioBox->SetSelection(0);

    xPos = 5;
    yPos += 60;

    int staticWidth = 110;
#ifdef __WXMOTIF__
    staticWidth += 20;
#endif

    int textWidth = 60;
    spacing = 10;

    (void) new wxStaticText(this, wxPRINTID_STATIC, _("Left margin (mm):"), wxPoint(xPos, yPos));
    xPos += staticWidth;

    m_marginLeftText = new wxTextCtrl(this, wxPRINTID_LEFTMARGIN, "", wxPoint(xPos, yPos), wxSize(textWidth, -1));
    xPos += textWidth + spacing;

    (void) new wxStaticText(this, wxPRINTID_STATIC, _("Right margin (mm):"), wxPoint(xPos, yPos));
    xPos += staticWidth;

    m_marginRightText = new wxTextCtrl(this, wxPRINTID_RIGHTMARGIN, "", wxPoint(xPos, yPos), wxSize(textWidth, -1));
    xPos += textWidth + spacing;

    yPos += 35;
    xPos = 5;

    (void) new wxStaticText(this, wxPRINTID_STATIC, _("Top margin (mm):"), wxPoint(xPos, yPos));
    xPos += staticWidth;

    m_marginTopText = new wxTextCtrl(this, wxPRINTID_TOPMARGIN, "", wxPoint(xPos, yPos), wxSize(textWidth, -1));
    xPos += textWidth + spacing;

    (void) new wxStaticText(this, wxPRINTID_STATIC, _("Bottom margin (mm):"), wxPoint(xPos, yPos));
    xPos += staticWidth;

    m_marginBottomText = new wxTextCtrl(this, wxPRINTID_BOTTOMMARGIN, "", wxPoint(xPos, yPos), wxSize(textWidth, -1));

    Fit();
    Centre(wxBOTH);

    InitDialog();
    delete [] choices;
}

wxGenericPageSetupDialog::~wxGenericPageSetupDialog()
{
}

bool wxGenericPageSetupDialog::TransferDataToWindow()
{
    if (m_marginLeftText)
        m_marginLeftText->SetValue(IntToString((int) m_pageData.GetMarginTopLeft().x));
    if (m_marginTopText)
        m_marginTopText->SetValue(IntToString((int) m_pageData.GetMarginTopLeft().y));
    if (m_marginRightText)
        m_marginRightText->SetValue(IntToString((int) m_pageData.GetMarginBottomRight().x));
    if (m_marginBottomText)
        m_marginBottomText->SetValue(IntToString((int) m_pageData.GetMarginBottomRight().y));

    if (m_orientationRadioBox)
    {
        if (m_pageData.GetPrintData().GetOrientation() == wxPORTRAIT)
            m_orientationRadioBox->SetSelection(0);
        else
            m_orientationRadioBox->SetSelection(1);
    }

    // Find the paper type from either the current paper size in the wxPageSetupDialogData, or
    // failing that, the id in the wxPrintData object.

    wxPrintPaperType* type = wxThePrintPaperDatabase->FindPaperType(
             wxSize(m_pageData.GetPaperSize().x * 10, m_pageData.GetPaperSize().y * 10));

    if (!type && m_pageData.GetPrintData().GetPaperId() != wxPAPER_NONE)
        type = wxThePrintPaperDatabase->FindPaperType(m_pageData.GetPrintData().GetPaperId());

    if (type)
    {
        m_paperTypeChoice->SetStringSelection(type->GetName());
    }

    return TRUE;
}

bool wxGenericPageSetupDialog::TransferDataFromWindow()
{
    if (m_marginLeftText && m_marginTopText)
        m_pageData.SetMarginTopLeft(wxPoint(atoi((const char *)m_marginLeftText->GetValue()),atoi((const char *)m_marginTopText->GetValue())));
    if (m_marginRightText && m_marginBottomText)
        m_pageData.SetMarginBottomRight(wxPoint(atoi((const char *)m_marginRightText->GetValue()),atoi((const char *)m_marginBottomText->GetValue())));

    if (m_orientationRadioBox)
    {
        int sel = m_orientationRadioBox->GetSelection();
        if (sel == 0)
        {
#if wxUSE_POSTSCRIPT
            wxThePrintSetupData->SetPrinterOrientation(wxPORTRAIT);
#endif
            m_pageData.GetPrintData().SetOrientation(wxPORTRAIT);
        }
        else
        {
#if wxUSE_POSTSCRIPT
            wxThePrintSetupData->SetPrinterOrientation(wxLANDSCAPE);
#endif
            m_pageData.GetPrintData().SetOrientation(wxLANDSCAPE);
        }
    }
    if (m_paperTypeChoice)
    {
        wxString val(m_paperTypeChoice->GetStringSelection());
        if (!val.IsNull() && val != "")
        {
            wxPrintPaperType* paper = wxThePrintPaperDatabase->FindPaperType(val);
            if ( paper )
            {
                m_pageData.SetPaperSize(wxSize(paper->GetWidth()/10, paper->GetHeight()/10));
                m_pageData.GetPrintData().SetPaperId(paper->GetId());
            }
        }
    }

    return TRUE;
}

wxChoice *wxGenericPageSetupDialog::CreatePaperTypeChoice(int *x, int *y)
{
/*
    if (!wxThePrintPaperDatabase)
    {
        wxThePrintPaperDatabase = new wxPrintPaperDatabase;
        wxThePrintPaperDatabase->CreateDatabase();
    }
*/

    int n = wxThePrintPaperDatabase->Number();
    wxString *choices = new wxString [n];
    int i;
    for (i = 0; i < n; i++)
    {
        wxPrintPaperType *paper = (wxPrintPaperType *)wxThePrintPaperDatabase->Nth(i)->Data();
        choices[i] = paper->GetName();
    }

    (void) new wxStaticText(this, wxPRINTID_STATIC, _("Paper size"), wxPoint(*x, *y));
    *y += 25;

    wxChoice *choice = new wxChoice(this, wxPRINTID_PAPERSIZE, wxPoint(*x, *y), wxSize(300, -1), n,
        choices);
    *y += 35;
    delete[] choices;

//    choice->SetSelection(sel);
    return choice;
}

