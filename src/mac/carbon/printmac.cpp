/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/printwin.cpp
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

#if wxUSE_PRINTING_ARCHITECTURE

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/dc.h"
    #include "wx/app.h"
    #include "wx/msgdlg.h"
    #include "wx/dcprint.h"
    #include "wx/math.h"
#endif

#include "wx/mac/uma.h"

#include "wx/mac/printmac.h"
#include "wx/mac/private/print.h"

#include "wx/printdlg.h"
#include "wx/mac/printdlg.h"

#include <stdlib.h>

IMPLEMENT_DYNAMIC_CLASS(wxMacCarbonPrintData, wxPrintNativeDataBase)
IMPLEMENT_DYNAMIC_CLASS(wxMacPrinter, wxPrinterBase)
IMPLEMENT_CLASS(wxMacPrintPreview, wxPrintPreviewBase)

bool wxMacCarbonPrintData::IsOk() const
{
    return (m_macPageFormat != kPMNoPageFormat) && (m_macPrintSettings != kPMNoPrintSettings) && (m_macPrintSession != kPMNoReference);
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

bool wxMacCarbonPrintData::TransferFrom( const wxPrintData &data )
{
    ValidateOrCreate() ;
    PMSetCopies( (PMPrintSettings) m_macPrintSettings , data.GetNoCopies() , false ) ;
    PMSetOrientation( (PMPageFormat) m_macPageFormat , ( data.GetOrientation() == wxLANDSCAPE ) ?
        kPMLandscape : kPMPortrait , false ) ;
    // collate cannot be set
#if 0 // not yet tested
    if ( !m_printerName.empty() )
        PMSessionSetCurrentPrinter( (PMPrintSession) m_macPrintSession , wxMacCFStringHolder( m_printerName , wxFont::GetDefaultEncoding() ) ) ;
#endif
    PMColorMode color ;
    PMGetColorMode(  (PMPrintSettings) m_macPrintSettings, &color ) ;
    if ( data.GetColour() )
    {
        if ( color == kPMBlackAndWhite )
            PMSetColorMode( (PMPrintSettings) m_macPrintSettings, kPMColor ) ;
    }
    else
        PMSetColorMode( (PMPrintSettings) m_macPrintSettings, kPMBlackAndWhite ) ;

    // PMDuplexMode not yet accessible via API
    // PMQualityMode not yet accessible via API
    // todo paperSize
    PMResolution res;
    PMPrinter printer;
    PMTag tag = kPMMaxSquareResolution;
    PMSessionGetCurrentPrinter(m_macPrintSession, &printer);
    PMPrinterGetPrinterResolution(printer, tag, &res);
    PMSetResolution((PMPageFormat) m_macPageFormat, &res);
    // after setting the new resolution the format has to be updated, otherwise the page rect remains 
    // at the 'old' scaling
    PMSessionValidatePageFormat((PMPrintSession) m_macPrintSession,
            (PMPageFormat) m_macPageFormat,
            kPMDontWantBoolean) ;

    return true ;
}

bool wxMacCarbonPrintData::TransferTo( wxPrintData &data )
{
    OSStatus err = noErr ;

    UInt32 copies ;
    err = PMGetCopies( m_macPrintSettings , &copies ) ;
    if ( err == noErr )
        data.SetNoCopies( copies ) ;

    PMOrientation orientation ;
    err = PMGetOrientation(  m_macPageFormat , &orientation ) ;
    if ( err == noErr )
    {
        if ( orientation == kPMPortrait || orientation == kPMReversePortrait )
            data.SetOrientation( wxPORTRAIT  );
        else
            data.SetOrientation( wxLANDSCAPE );
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
        data.SetColour( !(color == kPMBlackAndWhite) ) ;

    // PMDuplexMode not yet accessible via API
    // PMQualityMode not yet accessible via API
    // todo paperSize
    PMRect rPaper;
    err = PMGetUnadjustedPaperRect( m_macPageFormat, &rPaper);
    if ( err == noErr )
    {
        data.SetPaperSize( wxSize (
            (int)(( rPaper.right - rPaper.left ) * pt2mm + 0.5 ) ,
             (int)(( rPaper.bottom - rPaper.top ) * pt2mm + 0.5 ) ) );
    }
    return true ;
}

void wxMacCarbonPrintData::TransferFrom( wxPageSetupData *data )
{
    // should we setup the page rect here ?
    // since MacOS sometimes has two same paper rects with different
    // page rects we could make it roundtrip safe perhaps
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

            if ( data->GetMarginTopLeft().x < data->GetMinMarginTopLeft().x )
                data->SetMarginTopLeft( wxPoint( data->GetMinMarginTopLeft().x ,
                    data->GetMarginTopLeft().y ) ) ;

            if ( data->GetMarginBottomRight().x < data->GetMinMarginBottomRight().x )
                data->SetMarginBottomRight( wxPoint( data->GetMinMarginBottomRight().x ,
                    data->GetMarginBottomRight().y ) );

            if ( data->GetMarginTopLeft().y < data->GetMinMarginTopLeft().y )
                data->SetMarginTopLeft( wxPoint( data->GetMarginTopLeft().x , data->GetMinMarginTopLeft().y ) );

            if ( data->GetMarginBottomRight().y < data->GetMinMarginBottomRight().y )
                data->SetMarginBottomRight( wxPoint( data->GetMarginBottomRight().x ,
                    data->GetMinMarginBottomRight().y) );
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
    if ( to >= 0x7FFFFFFF ) //  due to an OS Bug we don't get back kPMPrintAllPages
    {
        data->SetAllPages( true ) ;
        // This means all pages, more or less
        data->SetFromPage(1);
        data->SetToPage(32000);
    }
    else
    {
        data->SetFromPage( from ) ;
        data->SetToPage( to ) ;
        data->SetAllPages( false );
    }
}

void wxMacCarbonPrintData::TransferFrom( wxPrintDialogData* data )
{
    PMSetPageRange( m_macPrintSettings , data->GetMinPage() , data->GetMaxPage() ) ;
    PMSetCopies( m_macPrintSettings , data->GetNoCopies() , false ) ;
    PMSetFirstPage( m_macPrintSettings , data->GetFromPage() , false ) ;

    if (data->GetAllPages() || data->GetFromPage() == 0)
        PMSetLastPage( m_macPrintSettings , (UInt32) kPMPrintAllPages, true ) ;
    else
        PMSetLastPage( m_macPrintSettings , (UInt32) data->GetToPage() , false ) ;
}

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
    wxDC *dc = NULL;
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
        if (dc)
            delete dc;
        return false;
    }

    // on the mac we have always pixels as addressing mode with 72 dpi
    printout->SetPPIScreen(72, 72);
    PMResolution res;
    wxMacCarbonPrintData* nativeData = (wxMacCarbonPrintData*)
          (m_printDialogData.GetPrintData().GetNativeData());
    PMGetResolution((PMPageFormat) (nativeData->m_macPageFormat), &res);
    printout->SetPPIPrinter(int(res.hRes), int(res.vRes));

    // Set printout parameters
    printout->SetDC(dc);

    int w, h;
    wxCoord ww, hh;
    dc->GetSize(&w, &h);
    printout->SetPageSizePixels((int)w, (int)h);
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
        for (pn = m_printDialogData.GetFromPage();
        keepGoing && (pn <= m_printDialogData.GetToPage()) && printout->HasPage(pn);
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
        m_printDialogData = dialog.GetPrintDialogData();

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
    
    int x , y ;
    wxCoord ww, hh;

    // Get a device context for the currently selected printer
    wxPrinterDC printerDC(m_printDialogData.GetPrintData());
    if (printerDC.Ok())
    {
        printerDC.GetSizeMM(&ww, &hh);
        printerDC.GetSize( &x , &y ) ;
        ppiPrinter = printerDC.GetPPI() ;
        m_isOk = true ;
    }
    else
    {
        // use some defaults
        x = 8 * 72 ;
        y = 11 * 72 ;
        ww = (int) (x * 25.4 / ppiPrinter.x) ;
        hh = (int) (y * 25.4 / ppiPrinter.y) ;
        m_isOk = false ;
    }
    m_previewPrintout->SetPageSizeMM((int)ww, (int)hh);
    m_previewPrintout->SetPageSizePixels( x , y) ;
    m_pageWidth = x ;
    m_pageHeight = y ;
    m_previewPrintout->SetPPIPrinter( ppiPrinter.x , ppiPrinter.y ) ;

    m_previewScale = (float)((float)ppiScreen.x/(float)ppiPrinter.y);
}

#endif
