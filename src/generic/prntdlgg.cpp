/////////////////////////////////////////////////////////////////////////////
// Name:        prntdlgg.cpp
// Purpose:     Generic print dialogs
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "prntdlgg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_PRINTING_ARCHITECTURE && (!defined(__WXMSW__) || wxUSE_POSTSCRIPT_ARCHITECTURE_IN_MSW)

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/dc.h"
    #include "wx/stattext.h"
    #include "wx/statbox.h"
    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/textctrl.h"
    #include "wx/radiobox.h"
    #include "wx/filedlg.h"
    #include "wx/combobox.h"
    #include "wx/intl.h"
    #include "wx/sizer.h"
    #include "wx/cmndata.h"
#endif

#if wxUSE_STATLINE
  #include "wx/statline.h"
#endif

#include "wx/generic/prntdlgg.h"

#if wxUSE_POSTSCRIPT
    #include "wx/generic/dcpsg.h"
#endif

#include "wx/printdlg.h"
#include "wx/paper.h"
#include "wx/filename.h"

// For print paper things
#include "wx/prntbase.h"

#include <stdlib.h>
#include <string.h>

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------


#if wxUSE_POSTSCRIPT

IMPLEMENT_CLASS(wxGenericPrintSetupDialog, wxDialog)

#endif // wxUSE_POSTSCRIPT

IMPLEMENT_CLASS(wxGenericPageSetupDialog, wxDialog)

BEGIN_EVENT_TABLE(wxGenericPageSetupDialog, wxDialog)
    EVT_BUTTON(wxPRINTID_SETUP, wxGenericPageSetupDialog::OnPrinter)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// global vars
// ----------------------------------------------------------------------------

extern wxPrintPaperDatabase *wxThePrintPaperDatabase;

#if wxUSE_POSTSCRIPT

// ----------------------------------------------------------------------------
// Generic print dialog for non-Windows printing use.
// ----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGenericPrintDialog, wxPrintDialogBase)

BEGIN_EVENT_TABLE(wxGenericPrintDialog, wxPrintDialogBase)
    EVT_BUTTON(wxID_OK, wxGenericPrintDialog::OnOK)
    EVT_BUTTON(wxPRINTID_SETUP, wxGenericPrintDialog::OnSetup)
    EVT_RADIOBOX(wxPRINTID_RANGE, wxGenericPrintDialog::OnRange)
END_EVENT_TABLE()

wxGenericPrintDialog::wxGenericPrintDialog(wxWindow *parent,
                                           wxPrintDialogData* data)
                    : wxPrintDialogBase(parent, wxID_ANY, _("Print"),
                               wxPoint(0, 0), wxSize(600, 600),
                               wxDEFAULT_DIALOG_STYLE |
                               wxTAB_TRAVERSAL)
{
    if ( data )
        m_printDialogData = *data;

    Init(parent);
}

wxGenericPrintDialog::wxGenericPrintDialog(wxWindow *parent,
                                           wxPrintData* data)
                    : wxPrintDialogBase(parent, wxID_ANY, _("Print"),
                               wxPoint(0, 0), wxSize(600, 600),
                               wxDEFAULT_DIALOG_STYLE |
                               wxTAB_TRAVERSAL)
{
    if ( data )
        m_printDialogData = *data;

    Init(parent);
}

void wxGenericPrintDialog::Init(wxWindow * WXUNUSED(parent))
{
  //    wxDialog::Create(parent, wxID_ANY, _("Print"), wxPoint(0, 0), wxSize(600, 600),
  //                     wxDEFAULT_DIALOG_STYLE | wxTAB_TRAVERSAL);

    wxBoxSizer *mainsizer = new wxBoxSizer( wxVERTICAL );

    // 1) top row

    wxStaticBoxSizer *topsizer = new wxStaticBoxSizer(
        new wxStaticBox( this, wxID_ANY, _( "Printer options" ) ), wxHORIZONTAL );
    m_printToFileCheckBox = new wxCheckBox( this, wxPRINTID_PRINTTOFILE, _("Print to File") );
    topsizer->Add( m_printToFileCheckBox, 0, wxCENTER|wxALL, 5 );

    topsizer->Add( 60,2,1 );

    m_setupButton = new wxButton(this, wxPRINTID_SETUP, _("Setup...") );
    topsizer->Add( m_setupButton, 0, wxCENTER|wxALL, 5 );

    mainsizer->Add( topsizer, 0, wxLEFT|wxTOP|wxRIGHT, 10 );

    // 2) middle row with radio box

    wxString *choices = new wxString[2];
    choices[0] = _("All");
    choices[1] = _("Pages");

    m_fromText = (wxTextCtrl*)NULL;
    m_toText = (wxTextCtrl*)NULL;
    m_rangeRadioBox = (wxRadioBox *)NULL;

    if (m_printDialogData.GetFromPage() != 0)
    {
        m_rangeRadioBox = new wxRadioBox(this, wxPRINTID_RANGE, _("Print Range"),
                                         wxDefaultPosition, wxDefaultSize,
                                         2, choices,
                                         1, wxRA_VERTICAL);
        m_rangeRadioBox->SetSelection(1);

        mainsizer->Add( m_rangeRadioBox, 0, wxLEFT|wxTOP|wxRIGHT, 10 );
    }

    // 3) bottom row

    wxBoxSizer *bottomsizer = new wxBoxSizer( wxHORIZONTAL );

    if (m_printDialogData.GetFromPage() != 0)
    {
        bottomsizer->Add( new wxStaticText(this, wxPRINTID_STATIC, _("From:") ), 0, wxCENTER|wxALL, 5 );
        m_fromText = new wxTextCtrl(this, wxPRINTID_FROM, wxEmptyString, wxDefaultPosition, wxSize(40, wxDefaultCoord));
        bottomsizer->Add( m_fromText, 1, wxCENTER|wxRIGHT, 10 );

        bottomsizer->Add( new wxStaticText(this, wxPRINTID_STATIC, _("To:") ), 0, wxCENTER|wxALL, 5);
        m_toText = new wxTextCtrl(this, wxPRINTID_TO, wxEmptyString, wxDefaultPosition, wxSize(40, wxDefaultCoord));
        bottomsizer->Add( m_toText, 1, wxCENTER|wxRIGHT, 10 );
    }

    bottomsizer->Add( new wxStaticText(this, wxPRINTID_STATIC, _("Copies:") ), 0, wxCENTER|wxALL, 5 );
    m_noCopiesText = new wxTextCtrl(this, wxPRINTID_COPIES, wxEmptyString, wxPoint(252, 130), wxSize(40, wxDefaultCoord));
    bottomsizer->Add( m_noCopiesText, 1, wxCENTER|wxRIGHT, 10 );

    mainsizer->Add( bottomsizer, 0, wxTOP|wxLEFT|wxRIGHT, 12 );

#if wxUSE_STATLINE
    // 4) static line
    mainsizer->Add( new wxStaticLine( this, wxID_ANY ), 0, wxEXPAND | wxLEFT|wxRIGHT|wxTOP, 10 );
#endif

    // 5) buttons

    mainsizer->Add( CreateButtonSizer( wxOK|wxCANCEL), 0, wxCENTER|wxALL, 10 );

    SetAutoLayout( true );
    SetSizer( mainsizer );

    mainsizer->Fit( this );
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
            // Transfer settings to  the print dialog's print data.
            m_printDialogData.GetPrintData() = genericPrintSetupDialog->GetPrintData();
        }
        // genericPrintSetupDialog->Destroy();

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

    // An empty 'to' field signals printing just the
    // 'from' page.
    if (m_printDialogData.GetToPage() < 1)
        m_printDialogData.SetToPage(m_printDialogData.GetFromPage());

    // There are some interactions between the global setup data
    // and the standard print dialog. The global printing 'mode'
    // is determined by whether the user checks Print to file
    // or not.
    if (m_printDialogData.GetPrintToFile())
    {
        m_printDialogData.GetPrintData().SetPrintMode(wxPRINT_MODE_FILE);

        wxFileName fname( m_printDialogData.GetPrintData().GetFilename() );

        wxFileDialog dialog( this, _("PostScript file"),
            fname.GetPath(), fname.GetFullName(), wxT("*.ps"), wxOPEN | wxOVERWRITE_PROMPT );
        if (dialog.ShowModal() != wxID_OK) return;

        m_printDialogData.GetPrintData().SetFilename( dialog.GetPath() );
    }
    else
    {
        m_printDialogData.GetPrintData().SetPrintMode(wxPRINT_MODE_PRINTER);
    }

    EndModal(wxID_OK);
}

void wxGenericPrintDialog::OnRange(wxCommandEvent& event)
{
    if (!m_fromText) return;

    if (event.GetInt() == 0)
    {
        m_fromText->Enable(false);
        m_toText->Enable(false);
    }
    else if (event.GetInt() == 1)
    {
        m_fromText->Enable(true);
        m_toText->Enable(true);
    }
}

void wxGenericPrintDialog::OnSetup(wxCommandEvent& WXUNUSED(event))
{
    wxGenericPrintSetupDialog dialog( this, &m_printDialogData.GetPrintData() );
    if (dialog.ShowModal() != wxID_CANCEL)
    {
        m_printDialogData = dialog.GetPrintData();
    }
}

bool wxGenericPrintDialog::TransferDataToWindow()
{
    if(m_printDialogData.GetFromPage() != 0)
    {
       if(m_fromText)
       {
          if (m_printDialogData.GetEnablePageNumbers())
          {
             m_fromText->Enable(true);
             m_toText->Enable(true);
             if (m_printDialogData.GetFromPage() > 0)
                m_fromText->SetValue(wxString::Format(_T("%d"), m_printDialogData.GetFromPage()));
             if (m_printDialogData.GetToPage() > 0)
                m_toText->SetValue(wxString::Format(_T("%d"), m_printDialogData.GetToPage()));
             if(m_rangeRadioBox)
                if (m_printDialogData.GetAllPages() || m_printDialogData.GetFromPage() == 0)
                   m_rangeRadioBox->SetSelection(0);
                else
                   m_rangeRadioBox->SetSelection(1);
          }
          else
          {
             m_fromText->Enable(false);
             m_toText->Enable(false);
             if(m_rangeRadioBox)
             {
                m_rangeRadioBox->SetSelection(0);
                m_rangeRadioBox->wxRadioBox::Enable(1, false);
             }
          }
       }
    }
    m_noCopiesText->SetValue(
        wxString::Format(_T("%d"), m_printDialogData.GetNoCopies()));

    m_printToFileCheckBox->SetValue(m_printDialogData.GetPrintToFile());
    m_printToFileCheckBox->Enable(m_printDialogData.GetEnablePrintToFile());
    return true;
}

bool wxGenericPrintDialog::TransferDataFromWindow()
{
    long res = 0;
    if(m_printDialogData.GetFromPage() != -1)
    {
        if (m_printDialogData.GetEnablePageNumbers())
        {
            if(m_fromText)
            {
                wxString value = m_fromText->GetValue();
                if (value.ToLong( &res ))
                    m_printDialogData.SetFromPage( res );
            }
            if(m_toText)
            {
                wxString value = m_toText->GetValue();
                if (value.ToLong( &res ))
                    m_printDialogData.SetToPage( res );
            }
        }
        if(m_rangeRadioBox)
        {
           if (m_rangeRadioBox->GetSelection() == 0)
              m_printDialogData.SetAllPages(true);
           else
              m_printDialogData.SetAllPages(false);
        }
    }
    else
    { // continuous printing
        m_printDialogData.SetFromPage(1);
        m_printDialogData.SetToPage(32000);
    }

    wxString value = m_noCopiesText->GetValue();
    if (value.ToLong( &res ))
        m_printDialogData.SetNoCopies( res );

    m_printDialogData.SetPrintToFile(m_printToFileCheckBox->GetValue());

    return true;
}

/*
TODO: collate and noCopies should be duplicated across dialog data and print data objects
(slightly different semantics on Windows but let's ignore this for a bit).
*/

wxDC *wxGenericPrintDialog::GetPrintDC()
{
  //    return new wxPostScriptDC(wxThePrintSetupData->GetPrinterFile(), false, (wxWindow *) NULL);
  return new wxPostScriptDC(GetPrintDialogData().GetPrintData());
}

// ----------------------------------------------------------------------------
// Generic print setup dialog
// ----------------------------------------------------------------------------

wxGenericPrintSetupDialog::wxGenericPrintSetupDialog(wxWindow *parent, wxPrintData* data):
wxDialog(parent, wxID_ANY, _("Print Setup"), wxPoint(0, 0), wxSize(600, 600), wxDEFAULT_DIALOG_STYLE|wxTAB_TRAVERSAL)
{
    Init(data);
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
        wxPoint(10, 80), wxDefaultSize, 2, choices, 1, wxRA_VERTICAL );
    m_orientationRadioBox->SetSelection(0);

    (void) new wxStaticBox(this, wxPRINTID_STATIC, _("Options"), wxPoint(10, 130), wxSize(staticBoxWidth, 50) );

    int colourYPos = 145;

#ifdef __WXMOTIF__
    colourYPos = 150;
#endif

    m_colourCheckBox = new wxCheckBox(this, wxPRINTID_PRINTCOLOUR, _("Print in colour"), wxPoint(15, colourYPos));

    (void) new wxStaticBox(this, wxPRINTID_STATIC, _("Print spooling"), wxPoint(330, 10), wxSize(200,170) );

    (void) new wxStaticText(this, wxPRINTID_STATIC, _("Printer command:"), wxPoint(340, 30));

    m_printerCommandText = new wxTextCtrl(this, wxPRINTID_COMMAND, wxEmptyString, wxPoint(360, 55), wxSize(150, wxDefaultCoord));

    (void) new wxStaticText(this, wxPRINTID_STATIC, _("Printer options:"), wxPoint(340, 110));

    m_printerOptionsText = new wxTextCtrl(this, wxPRINTID_OPTIONS, wxEmptyString, wxPoint(360, 135), wxSize(150, wxDefaultCoord));

    wxButton *okButton = new wxButton(this, wxID_OK, _("OK"), wxPoint(130, 200), wxSize(80, wxDefaultCoord));
    (void) new wxButton(this, wxID_CANCEL, _("Cancel"), wxPoint(320, 200), wxSize(80, wxDefaultCoord));

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
    return true;
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
        int selectedItem = m_paperTypeChoice->GetSelection();
        if (selectedItem != -1)
        {
            wxPrintPaperType *paper = wxThePrintPaperDatabase->Item(selectedItem);
            if (paper != NULL)
              m_printData.SetPaperId( paper->GetId());
        }
    }

    return true;
}

wxComboBox *wxGenericPrintSetupDialog::CreatePaperTypeChoice(int *x, int *y)
{
/* Should not be necessary
    if (!wxThePrintPaperDatabase)
    {
        wxThePrintPaperDatabase = new wxPrintPaperDatabase;
        wxThePrintPaperDatabase->CreateDatabase();
    }
*/
    size_t      n = wxThePrintPaperDatabase->GetCount();
    wxString   *choices = new wxString [n];
    size_t      sel = 0;

    for (size_t i = 0; i < n; i++)
    {
        wxPrintPaperType *paper = wxThePrintPaperDatabase->Item(i);
        choices[i] = paper->GetName();
        if (m_printData.GetPaperId() == paper->GetId())
            sel = i;
    }

    int width = 250;

    wxComboBox *choice = new wxComboBox( this,
                                         wxPRINTID_PAPERSIZE,
                                         _("Paper Size"),
                                         wxPoint(*x, *y),
                                         wxSize(width, wxDefaultCoord),
                                         n, choices );

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
    data.SetSetupDialog(true);
    wxPrintDialog printDialog(this, & data);
    printDialog.ShowModal();

    // Transfer the page setup print settings from the page dialog to this dialog again, in case
    // the page setup dialog changed something.
    GetPageSetupData().GetPrintData() = printDialog.GetPrintDialogData().GetPrintData();
    GetPageSetupData().CalculatePaperSizeFromId(); // Make sure page size reflects the id in wxPrintData

    // Now update the dialog in case the page setup dialog changed some of our settings.
    TransferDataToWindow();
}

wxGenericPageSetupDialog::wxGenericPageSetupDialog( wxWindow *parent,
                                                    wxPageSetupData* data)
    : wxDialog( parent,
                wxID_ANY,
                _("Page Setup"),
                wxPoint(0, 0),
                wxSize(600, 600),
                wxDEFAULT_DIALOG_STYLE|wxTAB_TRAVERSAL )
{
    if (data)
        m_pageData = *data;

    int textWidth = 80;

    wxBoxSizer *mainsizer = new wxBoxSizer( wxVERTICAL );

    // 1) top
    wxStaticBoxSizer *topsizer = new wxStaticBoxSizer(
      new wxStaticBox(this,wxPRINTID_STATIC, _("Paper size")), wxHORIZONTAL );

    size_t      n = wxThePrintPaperDatabase->GetCount();
    wxString   *choices = new wxString [n];

    for (size_t i = 0; i < n; i++)
    {
        wxPrintPaperType *paper = wxThePrintPaperDatabase->Item(i);
        choices[i] = paper->GetName();
    }

    m_paperTypeChoice = new wxComboBox( this,
                                        wxPRINTID_PAPERSIZE,
                                        _("Paper Size"),
                                        wxDefaultPosition,
                                        wxSize(300, wxDefaultCoord),
                                        n, choices );
    topsizer->Add( m_paperTypeChoice, 1, wxEXPAND|wxALL, 5 );
//  m_paperTypeChoice->SetSelection(sel);

    mainsizer->Add( topsizer, 0, wxTOP|wxLEFT|wxRIGHT | wxEXPAND, 10 );

    // 2) middle sizer with radio box

    wxString *choices2 = new wxString[2];
    choices2[0] = _("Portrait");
    choices2[1] = _("Landscape");
    m_orientationRadioBox = new wxRadioBox(this, wxPRINTID_ORIENTATION, _("Orientation"),
        wxDefaultPosition, wxDefaultSize, 2, choices2, 2);
    m_orientationRadioBox->SetSelection(0);

    mainsizer->Add( m_orientationRadioBox, 0, wxTOP|wxLEFT|wxRIGHT, 10 );

    // 3) margins

    wxBoxSizer *table = new wxBoxSizer( wxHORIZONTAL );

    wxBoxSizer *column1 = new wxBoxSizer( wxVERTICAL );
    column1->Add( new wxStaticText(this, wxPRINTID_STATIC, _("Left margin (mm):")),1,wxALL|wxALIGN_RIGHT,5 );
    column1->Add( new wxStaticText(this, wxPRINTID_STATIC, _("Top margin (mm):")),1,wxALL|wxALIGN_RIGHT,5 );
    table->Add( column1, 0, wxALL | wxEXPAND, 5 );

    wxBoxSizer *column2 = new wxBoxSizer( wxVERTICAL );
    m_marginLeftText = new wxTextCtrl(this, wxPRINTID_LEFTMARGIN, wxEmptyString, wxDefaultPosition, wxSize(textWidth, wxDefaultCoord));
    m_marginTopText = new wxTextCtrl(this, wxPRINTID_TOPMARGIN, wxEmptyString, wxDefaultPosition, wxSize(textWidth, wxDefaultCoord));
    column2->Add( m_marginLeftText, 1, wxALL, 5 );
    column2->Add( m_marginTopText, 1, wxALL, 5 );
    table->Add( column2, 0, wxRIGHT|wxTOP|wxBOTTOM | wxEXPAND, 5 );

    wxBoxSizer *column3 = new wxBoxSizer( wxVERTICAL );
    column3->Add( new wxStaticText(this, wxPRINTID_STATIC, _("Right margin (mm):")),1,wxALL|wxALIGN_RIGHT,5 );
    column3->Add( new wxStaticText(this, wxPRINTID_STATIC, _("Bottom margin (mm):")),1,wxALL|wxALIGN_RIGHT,5 );
    table->Add( column3, 0, wxALL | wxEXPAND, 5 );

    wxBoxSizer *column4 = new wxBoxSizer( wxVERTICAL );
    m_marginRightText = new wxTextCtrl(this, wxPRINTID_RIGHTMARGIN, wxEmptyString, wxDefaultPosition, wxSize(textWidth, wxDefaultCoord));
    m_marginBottomText = new wxTextCtrl(this, wxPRINTID_BOTTOMMARGIN, wxEmptyString, wxDefaultPosition, wxSize(textWidth, wxDefaultCoord));
    column4->Add( m_marginRightText, 1, wxALL, 5 );
    column4->Add( m_marginBottomText, 1, wxALL, 5 );
    table->Add( column4, 0, wxRIGHT|wxTOP|wxBOTTOM | wxEXPAND, 5 );

    mainsizer->Add( table, 0 );

#if wxUSE_STATLINE
    // 5) static line
    mainsizer->Add( new wxStaticLine( this, wxID_ANY ), 0, wxEXPAND | wxLEFT|wxRIGHT|wxTOP, 10 );
#endif

    // 6) buttons

    wxSizer* buttonsizer = CreateButtonSizer( wxOK|wxCANCEL);
    m_printerButton = new wxButton(this, wxPRINTID_SETUP, _("Printer...") );
    buttonsizer->Add( m_printerButton, 0, wxLEFT|wxRIGHT, 10 );
    if ( !m_pageData.GetEnablePrinter() )
        m_printerButton->Enable(false);
    //  if (m_printData.GetEnableHelp())
    //  wxButton *helpButton = new wxButton(this, (wxFunction)wxGenericPageSetupHelpProc, _("Help"), wxDefaultCoord, wxDefaultCoord, buttonWidth, buttonHeight);
    mainsizer->Add( buttonsizer, 0, wxCENTER|wxALL, 10 );


    SetAutoLayout( true );
    SetSizer( mainsizer );

    mainsizer->Fit( this );
    Centre(wxBOTH);

    InitDialog();

    delete[] choices;
    delete [] choices2;
}

wxGenericPageSetupDialog::~wxGenericPageSetupDialog()
{
}

bool wxGenericPageSetupDialog::TransferDataToWindow()
{
    if (m_marginLeftText)
        m_marginLeftText->SetValue(wxString::Format(wxT("%d"), m_pageData.GetMarginTopLeft().x));
    if (m_marginTopText)
        m_marginTopText->SetValue(wxString::Format(wxT("%d"), m_pageData.GetMarginTopLeft().y));
    if (m_marginRightText)
        m_marginRightText->SetValue(wxString::Format(wxT("%d"), m_pageData.GetMarginBottomRight().x));
    if (m_marginBottomText)
        m_marginBottomText->SetValue(wxString::Format(wxT("%d"), m_pageData.GetMarginBottomRight().y));

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

    return true;
}

bool wxGenericPageSetupDialog::TransferDataFromWindow()
{
    if (m_marginLeftText && m_marginTopText)
    {
        int left = wxAtoi( m_marginLeftText->GetValue().c_str() );
        int top = wxAtoi( m_marginTopText->GetValue().c_str() );
        m_pageData.SetMarginTopLeft( wxPoint(left,top) );
    }
    if (m_marginRightText && m_marginBottomText)
    {
        int right = wxAtoi( m_marginRightText->GetValue().c_str() );
        int bottom = wxAtoi( m_marginBottomText->GetValue().c_str() );
        m_pageData.SetMarginBottomRight( wxPoint(right,bottom) );
    }

    if (m_orientationRadioBox)
    {
        int sel = m_orientationRadioBox->GetSelection();
        if (sel == 0)
        {
            m_pageData.GetPrintData().SetOrientation(wxPORTRAIT);
        }
        else
        {
            m_pageData.GetPrintData().SetOrientation(wxLANDSCAPE);
        }
    }

    if (m_paperTypeChoice)
    {
        int selectedItem = m_paperTypeChoice->GetSelection();
        if (selectedItem != -1)
        {
            wxPrintPaperType *paper = wxThePrintPaperDatabase->Item(selectedItem);
            if ( paper )
            {
                m_pageData.SetPaperSize(wxSize(paper->GetWidth()/10, paper->GetHeight()/10));
                m_pageData.GetPrintData().SetPaperId(paper->GetId());
            }
        }
    }

    return true;
}

wxComboBox *wxGenericPageSetupDialog::CreatePaperTypeChoice(int *x, int *y)
{
/*
    if (!wxThePrintPaperDatabase)
    {
        wxThePrintPaperDatabase = new wxPrintPaperDatabase;
        wxThePrintPaperDatabase->CreateDatabase();
    }
*/

    size_t      n = wxThePrintPaperDatabase->GetCount();
    wxString   *choices = new wxString [n];

    for (size_t i = 0; i < n; i++)
    {
        wxPrintPaperType *paper = wxThePrintPaperDatabase->Item(i);
        choices[i] = paper->GetName();
    }

    (void) new wxStaticText(this, wxPRINTID_STATIC, _("Paper size"), wxPoint(*x, *y));
    *y += 25;

    wxComboBox *choice = new wxComboBox( this,
                                         wxPRINTID_PAPERSIZE,
                                         _("Paper Size"),
                                         wxPoint(*x, *y),
                                         wxSize(300, wxDefaultCoord),
                                         n, choices );
    *y += 35;
    delete[] choices;

//    choice->SetSelection(sel);
    return choice;
}

#endif

