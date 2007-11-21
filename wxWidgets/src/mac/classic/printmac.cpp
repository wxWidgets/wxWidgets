/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/printwin.cpp
// Purpose:     wxMacPrinter framework
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/dc.h"
    #include "wx/app.h"
    #include "wx/msgdlg.h"
    #include "wx/dcprint.h"
#endif

#include "wx/mac/uma.h"

#include "wx/mac/printmac.h"
#include "wx/mac/private/print.h"

#define mm2pt            2.83464566929
#define pt2mm            0.352777777778

#include "wx/printdlg.h"

#include <stdlib.h>

IMPLEMENT_DYNAMIC_CLASS(wxMacPrinter, wxPrinterBase)
IMPLEMENT_CLASS(wxMacPrintPreview, wxPrintPreviewBase)

#if TARGET_CARBON

wxNativePrintData* wxNativePrintData::Create()
{
    return new wxMacCarbonPrintData() ;
}

wxMacCarbonPrintData::wxMacCarbonPrintData()
{
    m_macPageFormat = kPMNoPageFormat;
    m_macPrintSettings = kPMNoPrintSettings;
    m_macPrintSession = kPMNoReference ;
    ValidateOrCreate() ;
}

wxMacCarbonPrintData::~wxMacCarbonPrintData()
{
    if (m_macPageFormat != kPMNoPageFormat)
    {
        (void)PMRelease(m_macPageFormat);
        m_macPageFormat = kPMNoPageFormat;
    }

    if (m_macPrintSettings != kPMNoPrintSettings)
    {
        (void)PMRelease(m_macPrintSettings);
        m_macPrintSettings = kPMNoPrintSettings;
    }

    if ( m_macPrintSession != kPMNoReference )
    {
        (void)PMRelease(m_macPrintSession);
        m_macPrintSession = kPMNoReference;
    }
}

void wxMacCarbonPrintData::ValidateOrCreate()
{
    OSStatus err = noErr ;
    if ( m_macPrintSession == kPMNoReference )
    {
        err = PMCreateSession( (PMPrintSession *) &m_macPrintSession ) ;
    }
    //  Set up a valid PageFormat object.
    if ( m_macPageFormat == kPMNoPageFormat)
    {
        err = PMCreatePageFormat((PMPageFormat *) &m_macPageFormat);

        //  Note that PMPageFormat is not session-specific, but calling
        //  PMSessionDefaultPageFormat assigns values specific to the printer
        //  associated with the current printing session.
        if ((err == noErr) &&
            ( m_macPageFormat != kPMNoPageFormat))
        {
            err = PMSessionDefaultPageFormat((PMPrintSession) m_macPrintSession,
                (PMPageFormat) m_macPageFormat);
        }
    }
    else
    {
        err = PMSessionValidatePageFormat((PMPrintSession) m_macPrintSession,
            (PMPageFormat) m_macPageFormat,
            kPMDontWantBoolean);
    }

    //  Set up a valid PrintSettings object.
    if ( m_macPrintSettings == kPMNoPrintSettings)
    {
        err = PMCreatePrintSettings((PMPrintSettings *) &m_macPrintSettings);

        //  Note that PMPrintSettings is not session-specific, but calling
        //  PMSessionDefaultPrintSettings assigns values specific to the printer
        //  associated with the current printing session.
        if ((err == noErr) &&
            ( m_macPrintSettings != kPMNoPrintSettings))
        {
            err = PMSessionDefaultPrintSettings((PMPrintSession) m_macPrintSession,
                (PMPrintSettings) m_macPrintSettings);
        }
    }
    else
    {
        err = PMSessionValidatePrintSettings((PMPrintSession) m_macPrintSession,
            (PMPrintSettings) m_macPrintSettings,
            kPMDontWantBoolean);
    }
}

void wxMacCarbonPrintData::TransferFrom( wxPrintData* data )
{
    ValidateOrCreate() ;
    PMSetCopies( (PMPrintSettings) m_macPrintSettings , data->GetNoCopies() , false ) ;
    PMSetOrientation( (PMPageFormat) m_macPageFormat , ( data->GetOrientation() == wxLANDSCAPE ) ?
        kPMLandscape : kPMPortrait , false ) ;
    // collate cannot be set
#if 0 // not yet tested
    if ( !m_printerName.empty() )
        PMSessionSetCurrentPrinter( (PMPrintSession) m_macPrintSession , wxMacCFStringHolder( m_printerName , wxFont::GetDefaultEncoding() ) ) ;
#endif
    PMColorMode color ;
    PMGetColorMode(  (PMPrintSettings) m_macPrintSettings, &color ) ;
    if ( data->GetColour() )
    {
        if ( color == kPMBlackAndWhite )
            PMSetColorMode( (PMPrintSettings) m_macPrintSettings, kPMColor ) ;
    }
    else
        PMSetColorMode( (PMPrintSettings) m_macPrintSettings, kPMBlackAndWhite ) ;

    // PMDuplexMode not yet accessible via API
    // PMQualityMode not yet accessible via API
    // todo paperSize
}

void wxMacCarbonPrintData::TransferTo( wxPrintData* data )
{
    OSStatus err = noErr ;

    UInt32 copies ;
    err = PMGetCopies( m_macPrintSettings , &copies ) ;
    if ( err == noErr )
        data->SetNoCopies( copies ) ;

    PMOrientation orientation ;
    err = PMGetOrientation(  m_macPageFormat , &orientation ) ;
    if ( err == noErr )
    {
        if ( orientation == kPMPortrait || orientation == kPMReversePortrait )
            data->SetOrientation( wxPORTRAIT  );
        else
            data->SetOrientation( wxLANDSCAPE );
    }

    // collate cannot be set
#if 0
    {
        wxMacCFStringHolder name ;
        PMPrinter printer ;
        PMSessionGetCurrentPrinter( m_macPrintSession ,
            &printer ) ;
        m_printerName = name.AsString() ;
    }
#endif

    PMColorMode color ;
    err = PMGetColorMode( m_macPrintSettings, &color ) ;
    if ( err == noErr )
        data->SetColour( !(color == kPMBlackAndWhite) ) ;

    // PMDuplexMode not yet accessible via API
    // PMQualityMode not yet accessible via API
    // todo paperSize
    PMRect rPaper;
    err = PMGetUnadjustedPaperRect( m_macPageFormat, &rPaper);
    if ( err == noErr )
    {
        data->SetPaperSize( wxSize (
            (int)(( rPaper.right - rPaper.left ) * pt2mm + 0.5 ) ,
             (int)(( rPaper.bottom - rPaper.top ) * pt2mm + 0.5 ) ) );
    }
}

void wxMacCarbonPrintData::TransferFrom( wxPageSetupData *data )
{
    // should we setup the page rect here ?
    // since MacOS sometimes has two same paper rects with different
    // page rects we could make it roundtrip safe perhaps
#if TARGET_CARBON
#else
#endif
}

void wxMacCarbonPrintData::TransferTo( wxPageSetupData* data )
{
    PMRect rPaper;
    OSStatus err = PMGetUnadjustedPaperRect(m_macPageFormat, &rPaper);
    if ( err == noErr )
    {
        PMRect rPage ;
        err = PMGetUnadjustedPageRect(m_macPageFormat , &rPage ) ;
        if ( err == noErr )
        {
            data->SetMinMarginTopLeft( wxPoint (
                (int)(((double) rPage.left - rPaper.left ) * pt2mm) ,
                (int)(((double) rPage.top - rPaper.top ) * pt2mm) ) ) ;

            data->SetMinMarginBottomRight( wxPoint (
                (wxCoord)(((double) rPaper.right - rPage.right ) * pt2mm),
                (wxCoord)(((double) rPaper.bottom - rPage.bottom ) * pt2mm)) ) ;
        }
    }
}

void wxMacCarbonPrintData::TransferTo( wxPrintDialogData* data )
{
    UInt32 minPage , maxPage ;
    PMGetPageRange( m_macPrintSettings , &minPage , &maxPage ) ;
    data->SetMinPage( minPage ) ;
    data->SetMaxPage( maxPage ) ;
    UInt32 copies ;
    PMGetCopies( m_macPrintSettings , &copies ) ;
    data->SetNoCopies( copies ) ;
    UInt32 from , to ;
    PMGetFirstPage( m_macPrintSettings , &from ) ;
    PMGetLastPage( m_macPrintSettings , &to ) ;
    data->SetFromPage( from ) ;
    data->SetToPage( to ) ;
}

void wxMacCarbonPrintData::TransferFrom( wxPrintDialogData* data )
{
    PMSetPageRange( m_macPrintSettings , data->GetMinPage() , data->GetMaxPage() ) ;
    PMSetCopies( m_macPrintSettings , data->GetNoCopies() , false ) ;
    PMSetFirstPage( m_macPrintSettings , data->GetFromPage() , false ) ;

    int toPage = data->GetToPage();
    if (toPage < 1)
        toPage = data->GetFromPage();
    PMSetLastPage( m_macPrintSettings , toPage , false ) ;
}

void wxMacCarbonPrintData::CopyFrom( wxNativePrintData* d )
{
    wxMacCarbonPrintData *data = (wxMacCarbonPrintData*) d ;
    if ( data->m_macPrintSession != kPMNoReference )
        PMRetain( data->m_macPrintSession ) ;
    if ( m_macPrintSession != kPMNoReference )
    {
        PMRelease( m_macPrintSession ) ;
        m_macPrintSession = kPMNoReference ;
    }
    if ( data->m_macPrintSession != kPMNoReference )
        m_macPrintSession = data->m_macPrintSession ;

    if ( data->m_macPrintSettings != kPMNoPrintSettings )
        PMRetain( data->m_macPrintSettings ) ;
    if ( m_macPrintSettings != kPMNoPrintSettings )
    {
        PMRelease( m_macPrintSettings ) ;
        m_macPrintSettings = kPMNoPrintSettings ;
    }
    if ( data->m_macPrintSettings != kPMNoPrintSettings )
        m_macPrintSettings = data->m_macPrintSettings ;

    if ( data->m_macPageFormat != kPMNoPageFormat )
        PMRetain( data->m_macPageFormat ) ;
    if ( m_macPageFormat != kPMNoPageFormat )
    {
        PMRelease( m_macPageFormat ) ;
        m_macPageFormat = kPMNoPageFormat ;
    }
    if ( data->m_macPageFormat != kPMNoPageFormat )
        m_macPageFormat = data->m_macPageFormat ;
}

int wxMacCarbonPrintData::ShowPrintDialog()
{
    int result = wxID_CANCEL ;
    OSErr err = noErr ;
    wxString message ;

    Boolean        accepted;

    {
        //  Display the Print dialog.
        if (err == noErr)
        {
            err = PMSessionPrintDialog( m_macPrintSession,
                m_macPrintSettings,
                m_macPageFormat,
                &accepted);
            if ((err == noErr) && !accepted)
            {
                err = kPMCancel; // user clicked Cancel button
            }
        }
        if  ( err == noErr )
        {
            result = wxID_OK ;
        }
    }
    if ((err != noErr) && (err != kPMCancel))
    {
        message.Printf( wxT("Print Error %d"), err ) ;
        wxMessageDialog dialog( NULL , message  , wxEmptyString, wxICON_HAND | wxOK) ;
        dialog.ShowModal();
    }

    return result ;
}

int wxMacCarbonPrintData::ShowPageSetupDialog()
{
    int      result = wxID_CANCEL ;
    OSErr    err = noErr ;
    wxString message ;

    Boolean        accepted;
    {
        //  Display the Page Setup dialog.
        if (err == noErr)
        {
            err = PMSessionPageSetupDialog( m_macPrintSession,
                m_macPageFormat,
                &accepted);
            if ((err == noErr) && !accepted)
            {
                err = kPMCancel; // user clicked Cancel button
            }
        }

        //  If the user did not cancel, flatten and save the PageFormat object
        //  with our document.
        if (err == noErr) {
            result = wxID_OK ;
        }
    }
    if ((err != noErr) && (err != kPMCancel))
    {
        message.Printf( wxT("Print Error %d"), err ) ;
        wxMessageDialog dialog( NULL , message , wxEmptyString, wxICON_HAND | wxOK) ;
        dialog.ShowModal();
    }

    return result ;
}

#else

wxNativePrintData* wxNativePrintData::Create()
{
    return new wxMacClassicPrintData() ;
}

wxMacClassicPrintData::wxMacClassicPrintData()
{
    m_macPrintSettings = NULL ;
    ValidateOrCreate() ;
}

wxMacClassicPrintData::~wxMacClassicPrintData()
{
    wxASSERT( m_macPrintSettings );
    DisposeHandle( (Handle) m_macPrintSettings ) ;
}

void wxMacClassicPrintData::ValidateOrCreate()
{
    if ( m_macPrintSettings == NULL )
    {
        m_macPrintSettings = (THPrint) NewHandleClear( sizeof( TPrint ) );
        (**m_macPrintSettings).iPrVersion = 0;                    // something invalid

        (**m_macPrintSettings).prInfo.iHRes = 72;
        (**m_macPrintSettings).prInfo.iVRes = 72;
        Rect r1 = { 0, 0, 8*72 - 2 * 18, 11*72 - 2 * 36 };
        (**m_macPrintSettings).prInfo.rPage = r1;// must have its top left & (0,0)

        Rect r2 = { -18, -36, 8*72 - 18, 11*72 - 36  };
        (**m_macPrintSettings).rPaper = r2;
        (**m_macPrintSettings).prStl.iPageV = 11 * 120 ;                // 11 inches in 120th of an inch
        (**m_macPrintSettings).prStl.iPageH = 8 * 120 ;                // 8 inches in 120th of an inch
    }
    else
    {
    }
}

void wxMacClassicPrintData::TransferFrom( wxPrintData* data )
{
    ValidateOrCreate() ;
    (**m_macPrintSettings).prJob.iCopies = data->GetNoCopies() ;
    // on mac the paper rect has a negative top left corner, because the page rect (printable area) is at 0,0
    // if all printing data is consolidated in on structure we will be able to set additional infos about pages
}

void wxMacClassicPrintData::TransferTo( wxPrintData* data )
{
    data->SetNoCopies( (**m_macPrintSettings).prJob.iCopies );
    data->SetPaperSize( wxSize(
        ((double) (**m_macPrintSettings).rPaper.right - (**m_macPrintSettings).rPaper.left ) * pt2mm ,
        ((double) (**m_macPrintSettings).rPaper.bottom - (**m_macPrintSettings).rPaper.top ) * pt2mm ) ) ;
}

void wxMacClassicPrintData::TransferFrom( wxPageSetupData *data )
{
}

void wxMacClassicPrintData::TransferTo( wxPageSetupData* data )
{
    data->SetMinMarginTopLeft( wxPoint(
        ((double) (**m_macPrintSettings).prInfo.rPage.left -(**m_macPrintSettings).rPaper.left ) * pt2mm ,
        ((double) (**m_macPrintSettings).prInfo.rPage.top -(**m_macPrintSettings).rPaper.top ) * pt2mm ) ) ;
    data->SetMinMarginBottomRight( wxPoint(
        ((double) (**m_macPrintSettings).rPaper.right - (**m_macPrintSettings).prInfo.rPage.right ) * pt2mm ,
        ((double)(**m_macPrintSettings).rPaper.bottom - (**m_macPrintSettings).prInfo.rPage.bottom ) * pt2mm ) ) ;
}

void wxMacClassicPrintData::TransferFrom( wxPrintDialogData* data )
{
    int toPage = data->GetToPage();
    if (toPage < 1)
        toPage = data->GetFromPage();
    (**m_macPrintSettings).prJob.iFstPage = data->GetFromPage() ;
    (**m_macPrintSettings).prJob.iLstPage = toPage;
}

void wxMacClassicPrintData::TransferTo( wxPrintDialogData* data )
{
    data->SetFromPage( (**m_macPrintSettings).prJob.iFstPage ) ;
    data->SetToPage( (**m_macPrintSettings).prJob.iLstPage ) ;
}

void wxMacClassicPrintData::CopyFrom( wxNativePrintData* data )
{
    DisposeHandle( (Handle) m_macPrintSettings ) ;
    m_macPrintSettings = ((wxMacClassicPrintData*)data)->m_macPrintSettings;
    HandToHand( (Handle*) &m_macPrintSettings );
}

int wxMacClassicPrintData::ShowPrintDialog()
{
    int result = wxID_CANCEL ;
    OSErr err = noErr ;
    wxString message ;

    err = ::UMAPrOpen() ;
    if ( err == noErr )
    {
        if  ( ::PrJobDialog( m_macPrintSettings ) )
        {
            result = wxID_OK ;
        }

    }
    else
    {
        message.Printf( wxT("Print Error %d"), err ) ;
        wxMessageDialog dialog( NULL , message  , wxEmptyString, wxICON_HAND | wxOK) ;
        dialog.ShowModal();
    }
    ::UMAPrClose() ;

    return result ;
}

int wxMacClassicPrintData::ShowPageSetupDialog()
{
    int      result = wxID_CANCEL ;
    OSErr    err = noErr ;
    wxString message ;

    err = ::UMAPrOpen() ;
    if ( err == noErr )
    {
        if  ( ::PrStlDialog( m_macPrintSettings ) )
        {
            result = wxID_OK ;
        }

    }
    else
    {
        message.Printf( wxT("Print Error %d"), err ) ;
        wxMessageDialog dialog( NULL , message , wxEmptyString , wxICON_HAND | wxOK) ;
        dialog.ShowModal();
    }
    ::UMAPrClose() ;
    return result ;
}

#endif

/*
* Printer
*/

wxMacPrinter::wxMacPrinter(wxPrintDialogData *data):
wxPrinterBase(data)
{
}

wxMacPrinter::~wxMacPrinter(void)
{
}

bool wxMacPrinter::Print(wxWindow *parent, wxPrintout *printout, bool prompt)
{
    sm_abortIt = false;
    sm_abortWindow = NULL;

    if (!printout)
        return false;

    printout->SetIsPreview(false);
    if (m_printDialogData.GetMinPage() < 1)
        m_printDialogData.SetMinPage(1);
    if (m_printDialogData.GetMaxPage() < 1)
        m_printDialogData.SetMaxPage(9999);

    // Create a suitable device context
    wxPrinterDC *dc = NULL;
    if (prompt)
    {
        wxPrintDialog dialog(parent, & m_printDialogData);
        if (dialog.ShowModal() == wxID_OK)
        {
            dc = dialog.GetPrintDC();
            m_printDialogData = dialog.GetPrintDialogData();
        }
    }
    else
    {
        dc = new wxPrinterDC( m_printDialogData.GetPrintData() ) ;
    }

    // May have pressed cancel.
    if (!dc || !dc->Ok())
    {
        if (dc) delete dc;
        return false;
    }

    // on the mac we have always pixels as addressing mode with 72 dpi
    printout->SetPPIScreen(72, 72);
    printout->SetPPIPrinter(72, 72);

    // Set printout parameters
    printout->SetDC(dc);

    int w, h;
    wxCoord ww, hh;
    dc->GetSize(&w, &h);
    printout->SetPageSizePixels((int)w, (int)h);
    printout->SetPaperRectPixels(dc->GetPaperRect());
    dc->GetSizeMM(&ww, &hh);
    printout->SetPageSizeMM((int)ww, (int)hh);

    // Create an abort window
    wxBeginBusyCursor();

    printout->OnPreparePrinting();

    // Get some parameters from the printout, if defined
    int fromPage, toPage;
    int minPage, maxPage;
    printout->GetPageInfo(&minPage, &maxPage, &fromPage, &toPage);

    if (maxPage == 0)
    {
        wxEndBusyCursor();
        return false;
    }

    // Only set min and max, because from and to have been
    // set by the user
    m_printDialogData.SetMinPage(minPage);
    m_printDialogData.SetMaxPage(maxPage);

    wxWindow *win = CreateAbortWindow(parent, printout);
    wxSafeYield(win,true);

    if (!win)
    {
        wxEndBusyCursor();
        wxMessageBox(wxT("Sorry, could not create an abort dialog."), wxT("Print Error"), wxOK, parent);
        delete dc;
        return false;
    }
    sm_abortWindow = win;
    sm_abortWindow->Show(true);
    wxSafeYield(win,true);

    printout->OnBeginPrinting();

    bool keepGoing = true;

    int copyCount;
    for (copyCount = 1; copyCount <= m_printDialogData.GetNoCopies(); copyCount ++)
    {
        if (!printout->OnBeginDocument(m_printDialogData.GetFromPage(), m_printDialogData.GetToPage()))
        {
            wxEndBusyCursor();
            wxMessageBox(wxT("Could not start printing."), wxT("Print Error"), wxOK, parent);
            break;
        }
        if (sm_abortIt)
            break;

        int pn;
        for (pn = m_printDialogData.GetFromPage(); keepGoing && (pn <= m_printDialogData.GetToPage()) && printout->HasPage(pn);
        pn++)
        {
            if (sm_abortIt)
            {
                keepGoing = false;
                break;
            }
            else
            {
#if TARGET_CARBON
                if ( UMAGetSystemVersion() >= 0x1000 )
#endif
                {
                    GrafPtr thePort ;
                    GetPort( &thePort ) ;
                    wxSafeYield(win,true);
                    SetPort( thePort ) ;
                }
                dc->StartPage();
                keepGoing = printout->OnPrintPage(pn);
                dc->EndPage();
            }
        }
        printout->OnEndDocument();
    }

    printout->OnEndPrinting();

    if (sm_abortWindow)
    {
        sm_abortWindow->Show(false);
        delete sm_abortWindow;
        sm_abortWindow = NULL;
    }

    wxEndBusyCursor();

    delete dc;

    return true;
}

wxDC* wxMacPrinter::PrintDialog(wxWindow *parent)
{
    wxDC* dc = (wxDC*) NULL;

    wxPrintDialog dialog(parent, & m_printDialogData);
    int ret = dialog.ShowModal();

    if (ret == wxID_OK)
    {
        dc = dialog.GetPrintDC();
        m_printDialogData = dialog.GetPrintDialogData();
    }

    return dc;
}

bool wxMacPrinter::Setup(wxWindow *parent)
{
#if 0
    wxPrintDialog dialog(parent, & m_printDialogData);
    dialog.GetPrintDialogData().SetSetupDialog(true);

    int ret = dialog.ShowModal();

    if (ret == wxID_OK)
    {
        m_printDialogData = dialog.GetPrintDialogData();
    }

    return (ret == wxID_OK);
#endif
    return wxID_CANCEL;
}

/*
* Print preview
*/

wxMacPrintPreview::wxMacPrintPreview(wxPrintout *printout,
                                     wxPrintout *printoutForPrinting,
                                     wxPrintDialogData *data)
                                     : wxPrintPreviewBase(printout, printoutForPrinting, data)
{
    DetermineScaling();
}

wxMacPrintPreview::wxMacPrintPreview(wxPrintout *printout, wxPrintout *printoutForPrinting, wxPrintData *data):
wxPrintPreviewBase(printout, printoutForPrinting, data)
{
    DetermineScaling();
}

wxMacPrintPreview::~wxMacPrintPreview(void)
{
}

bool wxMacPrintPreview::Print(bool interactive)
{
    if (!m_printPrintout)
        return false;
    wxMacPrinter printer(&m_printDialogData);
    return printer.Print(m_previewFrame, m_printPrintout, interactive);
}

void wxMacPrintPreview::DetermineScaling(void)
{
    int screenWidth , screenHeight ;
    wxDisplaySize( &screenWidth , &screenHeight ) ;

    wxSize ppiScreen( 72 , 72 ) ;
    wxSize ppiPrinter( 72 , 72 ) ;

    m_previewPrintout->SetPPIScreen( ppiScreen.x , ppiScreen.y ) ;

    wxCoord w , h ;
    wxCoord ww, hh;
    wxRect paperRect;

    // Get a device context for the currently selected printer
    wxPrinterDC printerDC(m_printDialogData.GetPrintData());
    if (printerDC.Ok())
    {
        printerDC.GetSizeMM(&ww, &hh);
        printerDC.GetSize( &w , &h ) ;
        ppiPrinter = printerDC.GetPPI() ;
        paperRect = printerDC.GetPaperRect();
        m_isOk = true ;
    }
    else
    {
        // use some defaults
        w = 8 * 72 ;
        h = 11 * 72 ;
        ww = (wxCoord) (w * 25.4 / ppiPrinter.x) ;
        hh = (wxCoord) (h * 25.4 / ppiPrinter.y) ;
        paperRect = wxRect(0, 0, w, h);
        m_isOk = false ;
    }
    m_previewPrintout->SetPageSizeMM(ww, hh);
    m_previewPrintout->SetPageSizePixels(w , h) ;
    m_previewPrintout->SetPaperRectPixels(paperRect);
    m_pageWidth = w;
    m_pageHeight = h;
    m_previewPrintout->SetPPIPrinter( ppiPrinter.x , ppiPrinter.y ) ;

    m_previewScaleX = float(ppiScreen.x) / ppiPrinter.x;
    m_previewScaleY = float(ppiScreen.y) / ppiPrinter.y;
}
