/////////////////////////////////////////////////////////////////////////////
// Name:        cmndata.cpp
// Purpose:     Common GDI data
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
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
    #pragma implementation "cmndata.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <stdio.h>
    #include "wx/string.h"
    #include "wx/utils.h"
    #include "wx/app.h"
#endif

#include "wx/gdicmn.h"
#include "wx/cmndata.h"
#include "wx/log.h"
#include "wx/prntbase.h"
#include "wx/printdlg.h"

#if wxUSE_FONTDLG
    #include "wx/fontdlg.h"
#endif // wxUSE_FONTDLG

#if wxUSE_PRINTING_ARCHITECTURE
    #include "wx/paper.h"
#endif // wxUSE_PRINTING_ARCHITECTURE

#if defined(__WXMSW__) && !defined(__PALMOS__)
    #include <windowsx.h>
    #include "wx/msw/private.h"

    #ifndef __SMARTPHONE__ /* of WinCE */
        #include <commdlg.h>
    #endif

    #if defined(__WATCOMC__) || defined(__SYMANTEC__) || defined(__SALFORDC__)
        #include <windowsx.h>
        #include <commdlg.h>
    #endif
#endif // MSW

#ifdef __WXMAC__
    #include "wx/mac/private/print.h"
#endif

    #if wxUSE_PRINTING_ARCHITECTURE
        IMPLEMENT_DYNAMIC_CLASS(wxPrintData, wxObject)
        IMPLEMENT_DYNAMIC_CLASS(wxPrintDialogData, wxObject)
        IMPLEMENT_DYNAMIC_CLASS(wxPageSetupDialogData, wxObject)
    #endif // wxUSE_PRINTING_ARCHITECTURE

    IMPLEMENT_DYNAMIC_CLASS(wxFontData, wxObject)
    IMPLEMENT_DYNAMIC_CLASS(wxColourData, wxObject)

#ifndef DMPAPER_USER
     #define DMPAPER_USER                256
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxColourData
// ----------------------------------------------------------------------------

wxColourData::wxColourData()
{
    m_chooseFull = false;
    m_dataColour.Set(0,0,0);
    // m_custColours are wxNullColours initially
}

wxColourData::wxColourData(const wxColourData& data)
    : wxObject()
{
    (*this) = data;
}

wxColourData::~wxColourData()
{
}

void wxColourData::SetCustomColour(int i, const wxColour& colour)
{
    wxCHECK_RET( (i >= 0 && i < 16), _T("custom colour index out of range") );

    m_custColours[i] = colour;
}

wxColour wxColourData::GetCustomColour(int i)
{
    wxCHECK_MSG( (i >= 0 && i < 16), wxColour(0,0,0),
                 _T("custom colour index out of range") );

    return m_custColours[i];
}

void wxColourData::operator=(const wxColourData& data)
{
    int i;
    for (i = 0; i < 16; i++)
        m_custColours[i] = data.m_custColours[i];

    m_dataColour = (wxColour&)data.m_dataColour;
    m_chooseFull = data.m_chooseFull;
}

// ----------------------------------------------------------------------------
// Font data
// ----------------------------------------------------------------------------

wxFontData::wxFontData()
{
    // Intialize colour to black.
    m_fontColour = wxNullColour;

    m_showHelp = false;
    m_allowSymbols = true;
    m_enableEffects = true;
    m_minSize = 0;
    m_maxSize = 0;

    m_encoding = wxFONTENCODING_SYSTEM;
}

wxFontData::~wxFontData()
{
}

#if wxUSE_FONTDLG

wxFontDialogBase::~wxFontDialogBase()
{
}

#endif // wxUSE_FONTDLG

#if wxUSE_PRINTING_ARCHITECTURE
// ----------------------------------------------------------------------------
// Print data
// ----------------------------------------------------------------------------

wxPrintData::wxPrintData()
{
#ifdef __WXMAC__
    m_nativePrintData = wxNativePrintData::Create() ;
#endif
    m_bin = wxPRINTBIN_DEFAULT;
    m_printMode = wxPRINT_MODE_PRINTER;
    m_printOrientation = wxPORTRAIT;
    m_printNoCopies = 1;
    m_printCollate = false;

    // New, 24/3/99
    m_printerName = wxT("");
    m_colour = true;
    m_duplexMode = wxDUPLEX_SIMPLEX;
    m_printQuality = wxPRINT_QUALITY_HIGH;
    m_paperId = wxPAPER_A4;
    m_paperSize = wxSize(210, 297);

    m_nativeData = wxPrintFactory::GetFactory()->CreatePrintNativeData();
}

wxPrintData::wxPrintData(const wxPrintData& printData)
    : wxObject()
{
    (*this) = printData;
}

wxPrintData::~wxPrintData()
{
    m_nativeData->m_ref--;
    if (m_nativeData->m_ref == 0) 
        delete m_nativeData;
        
#ifdef __WXMAC__
    delete m_nativePrintData ;
#endif
}


void wxPrintData::ConvertToNative()
{
#ifdef __WXMAC__
    m_nativePrintData->TransferFrom( this ) ;
#else
    m_nativeData->TransferFrom( *this ) ;
#endif
}

void wxPrintData::ConvertFromNative()
{
#ifdef __WXMAC__
    m_nativePrintData->TransferTo( this ) ;
#else
    m_nativeData->TransferTo( *this ) ;
#endif
}

void wxPrintData::operator=(const wxPrintData& data)
{
    m_printNoCopies = data.m_printNoCopies;
    m_printCollate = data.m_printCollate;
    m_printOrientation = data.m_printOrientation;
    m_printerName = data.m_printerName;
    m_colour = data.m_colour;
    m_duplexMode = data.m_duplexMode;
    m_printQuality = data.m_printQuality;
    m_paperId = data.m_paperId;
    m_paperSize = data.m_paperSize;
    m_bin = data.m_bin;
    m_printMode = data.m_printMode;
    m_filename = data.m_filename;   
    
    m_nativeData = data.GetNativeData();
    m_nativeData->m_ref++;
    
#ifdef __WXMAC__
    m_nativePrintData->CopyFrom( data.m_nativePrintData ) ;
#endif
}

// Is this data OK for showing the print dialog?
bool wxPrintData::Ok() const
{
    m_nativeData->TransferFrom( *this );

    return m_nativeData->Ok();
}

// ----------------------------------------------------------------------------
// Print dialog data
// ----------------------------------------------------------------------------

wxPrintDialogData::wxPrintDialogData()
{
#ifdef __WXMSW__
    m_printDlgData = NULL;
#endif
    m_printFromPage = 0;
    m_printToPage = 0;
    m_printMinPage = 0;
    m_printMaxPage = 0;
    m_printNoCopies = 1;
    m_printAllPages = false;
    m_printCollate = false;
    m_printToFile = false;
    m_printSelection = false;
    m_printEnableSelection = false;
    m_printEnablePageNumbers = true;
    
    wxPrintFactory* factory = wxPrintFactory::GetFactory();
    m_printEnablePrintToFile = ! factory->HasOwnPrintToFile();
    
    m_printEnableHelp = false;
    m_printSetupDialog = false;
}

wxPrintDialogData::wxPrintDialogData(const wxPrintDialogData& dialogData)
    : wxObject()
{
#ifdef __WXMSW__
    m_printDlgData = NULL;
#endif
    (*this) = dialogData;
}

wxPrintDialogData::wxPrintDialogData(const wxPrintData& printData)
{
#ifdef __WXMSW__
    m_printDlgData = NULL;
#endif
    m_printFromPage = 1;
    m_printToPage = 0;
    m_printMinPage = 1;
    m_printMaxPage = 9999;
    m_printNoCopies = 1;
    m_printAllPages = false;
    m_printCollate = false;
    m_printToFile = false;
    m_printSelection = false;
    m_printEnableSelection = false;
    m_printEnablePageNumbers = true;
    m_printEnablePrintToFile = true;
    m_printEnableHelp = false;
    m_printSetupDialog = false;

    m_printData = printData;
}

wxPrintDialogData::~wxPrintDialogData()
{
#ifdef __WXMSW__
    PRINTDLG *pd = (PRINTDLG *) m_printDlgData;
    if ( pd && pd->hDevMode )
        GlobalFree(pd->hDevMode);
    if ( pd )
        delete pd;
#endif
}

#ifdef __WXMSW__
void wxPrintDialogData::ConvertToNative()
{
    wxWindowsPrintNativeData *data =
        (wxWindowsPrintNativeData *) m_printData.GetNativeData();
    data->TransferFrom( m_printData );

    PRINTDLG *pd = (PRINTDLG*) m_printDlgData;

    if (!pd)
    {
        pd = new PRINTDLG;
        memset( pd, 0, sizeof(PRINTDLG) );
        m_printDlgData = (void*) pd;

        // GNU-WIN32 has the wrong size PRINTDLG - can't work out why.
#ifdef __GNUWIN32__
        pd->lStructSize    = 66;
#else
        pd->lStructSize    = sizeof(PRINTDLG);
#endif
        pd->hwndOwner      = (HWND)NULL;
        pd->hDevMode       = NULL; // Will be created by PrintDlg
        pd->hDevNames      = NULL; // Ditto

        pd->Flags          = PD_RETURNDEFAULT;
        pd->nCopies        = 1;
    }

    // Pass the devmode data to the PRINTDLG structure, since it'll
    // be needed when PrintDlg is called.
    if (pd->hDevMode)
    {
        GlobalFree(pd->hDevMode);
    }

    // Pass the devnames data to the PRINTDLG structure, since it'll
    // be needed when PrintDlg is called.
    if (pd->hDevNames)
    {
        GlobalFree(pd->hDevNames);
    }

    pd->hDevMode = (HGLOBAL)(DWORD) data->GetDevMode();

    data->SetDevMode( (void*) NULL);

    // Shouldn't assert; we should be able to test Ok-ness at a higher level
    //wxASSERT_MSG( (pd->hDevMode), wxT("hDevMode must be non-NULL in ConvertToNative!"));

    pd->hDevNames = (HGLOBAL)(DWORD) data->GetDevNames();

    data->SetDevNames( (void*) NULL);

    pd->hDC = (HDC) NULL;
    pd->nFromPage = (WORD)m_printFromPage;
    pd->nToPage = (WORD)m_printToPage;
    pd->nMinPage = (WORD)m_printMinPage;
    pd->nMaxPage = (WORD)m_printMaxPage;
    pd->nCopies = (WORD)m_printNoCopies;

    pd->Flags = PD_RETURNDC;

#ifdef __GNUWIN32__
    pd->lStructSize = 66;
#else
    pd->lStructSize = sizeof( PRINTDLG );
#endif

    pd->hwndOwner=(HWND)NULL;
//    pd->hDevNames=(HANDLE)NULL;
    pd->hInstance=(HINSTANCE)NULL;
    pd->lCustData = (LPARAM) NULL;
    pd->lpfnPrintHook = NULL;
    pd->lpfnSetupHook = NULL;
    pd->lpPrintTemplateName = NULL;
    pd->lpSetupTemplateName = NULL;
    pd->hPrintTemplate = (HGLOBAL) NULL;
    pd->hSetupTemplate = (HGLOBAL) NULL;

    if ( m_printAllPages )
        pd->Flags |= PD_ALLPAGES;
    if ( m_printSelection )
        pd->Flags |= PD_SELECTION;
    if ( m_printCollate )
        pd->Flags |= PD_COLLATE;
    if ( m_printToFile )
        pd->Flags |= PD_PRINTTOFILE;
    if ( !m_printEnablePrintToFile )
        pd->Flags |= PD_DISABLEPRINTTOFILE;
    if ( !m_printEnableSelection )
        pd->Flags |= PD_NOSELECTION;
    if ( !m_printEnablePageNumbers )
        pd->Flags |= PD_NOPAGENUMS;
    else if ( (!m_printAllPages) && (!m_printSelection) && (m_printFromPage != 0) && (m_printToPage != 0))
        pd->Flags |= PD_PAGENUMS;
    if ( m_printEnableHelp )
        pd->Flags |= PD_SHOWHELP;
    if ( m_printSetupDialog )
        pd->Flags |= PD_PRINTSETUP;
}

void wxPrintDialogData::ConvertFromNative()
{
    PRINTDLG *pd = (PRINTDLG*) m_printDlgData;
    if ( pd == NULL )
        return;

    wxWindowsPrintNativeData *data =
        (wxWindowsPrintNativeData *) m_printData.GetNativeData();
    
    // Pass the devmode data back to the wxPrintData structure where it really belongs.
    if (pd->hDevMode)
    {
        if (data->GetDevMode())
        {
            // Make sure we don't leak memory
            GlobalFree( (HGLOBAL)(DWORD) data->GetDevMode() );
        }
        data->SetDevMode( (void*)(long) pd->hDevMode );
        pd->hDevMode = NULL;
    }

    // Pass the devnames data back to the wxPrintData structure where it really belongs.
    if (pd->hDevNames)
    {
        if (data->GetDevNames())
        {
            // Make sure we don't leak memory
            GlobalFree((HGLOBAL)(DWORD) data->GetDevNames());
        }
        data->SetDevNames((void*)(long) pd->hDevNames);
        pd->hDevNames = NULL;
    }

    // Now convert the DEVMODE object, passed down from the PRINTDLG object,
    // into wxWidgets form.
    data->TransferTo( m_printData );

    m_printFromPage = pd->nFromPage;
    m_printToPage = pd->nToPage;
    m_printMinPage = pd->nMinPage;
    m_printMaxPage = pd->nMaxPage;
    m_printNoCopies = pd->nCopies;

    m_printAllPages = (((pd->Flags & PD_PAGENUMS) != PD_PAGENUMS) && ((pd->Flags & PD_SELECTION) != PD_SELECTION));
    m_printSelection = ((pd->Flags & PD_SELECTION) == PD_SELECTION);
    m_printCollate = ((pd->Flags & PD_COLLATE) == PD_COLLATE);
    m_printToFile = ((pd->Flags & PD_PRINTTOFILE) == PD_PRINTTOFILE);
    m_printEnablePrintToFile = ((pd->Flags & PD_DISABLEPRINTTOFILE) != PD_DISABLEPRINTTOFILE);
    m_printEnableSelection = ((pd->Flags & PD_NOSELECTION) != PD_NOSELECTION);
    m_printEnablePageNumbers = ((pd->Flags & PD_NOPAGENUMS) != PD_NOPAGENUMS);
    m_printEnableHelp = ((pd->Flags & PD_SHOWHELP) == PD_SHOWHELP);
    m_printSetupDialog = ((pd->Flags & PD_PRINTSETUP) == PD_PRINTSETUP);

/* port is obsolete in WIN32
    // Get the port name
    if (pd->hDevNames)
    {
        LPDEVNAMES lpDevNames = (LPDEVNAMES)GlobalLock(pd->hDevNames);
        if (lpDevNames) {
            m_printData.SetPortName((LPSTR)lpDevNames + lpDevNames->wDriverOffset);
            wxString devName = (LPSTR)lpDevNames + lpDevNames->wDeviceOffset;
            GlobalUnlock(pd->hDevNames);

//            wxASSERT_MSG( (m_printerName == "" || (devName == m_printerName)), "Printer name obtained from DEVMODE and DEVNAMES were different!");
        }
    }
*/
}

void wxPrintDialogData::SetOwnerWindow(wxWindow* win)
{
    if ( m_printDlgData == NULL )
        ConvertToNative();

    if ( m_printDlgData != NULL && win != NULL)
    {
        PRINTDLG *pd = (PRINTDLG *) m_printDlgData;
        pd->hwndOwner=(HWND) win->GetHWND();
    }
}
#endif // MSW

#ifdef __WXMAC__

void wxPrintDialogData::ConvertToNative()
{
    m_printData.ConvertToNative();
    m_printData.m_nativePrintData->TransferFrom( this ) ;
}

void wxPrintDialogData::ConvertFromNative()
{
    m_printData.ConvertFromNative();
    m_printData.m_nativePrintData->TransferTo( this ) ;
}

#endif


void wxPrintDialogData::operator=(const wxPrintDialogData& data)
{
    m_printFromPage = data.m_printFromPage;
    m_printToPage = data.m_printToPage;
    m_printMinPage = data.m_printMinPage;
    m_printMaxPage = data.m_printMaxPage;
    m_printNoCopies = data.m_printNoCopies;
    m_printAllPages = data.m_printAllPages;
    m_printCollate = data.m_printCollate;
    m_printToFile = data.m_printToFile;
    m_printSelection = data.m_printSelection;
    m_printEnableSelection = data.m_printEnableSelection;
    m_printEnablePageNumbers = data.m_printEnablePageNumbers;
    m_printEnableHelp = data.m_printEnableHelp;
    m_printEnablePrintToFile = data.m_printEnablePrintToFile;
    m_printSetupDialog = data.m_printSetupDialog;

    m_printData = data.m_printData;
}

void wxPrintDialogData::operator=(const wxPrintData& data)
{
    m_printData = data;
}

// ----------------------------------------------------------------------------
// wxPageSetupDialogData
// ----------------------------------------------------------------------------

wxPageSetupDialogData::wxPageSetupDialogData()
{
#if defined(__WIN95__)
    m_pageSetupData = NULL;
#endif
    m_paperSize = wxSize(0, 0);

    CalculatePaperSizeFromId();

    m_minMarginTopLeft = wxPoint(0, 0);
    m_minMarginBottomRight = wxPoint(0, 0);
    m_marginTopLeft = wxPoint(0, 0);
    m_marginBottomRight = wxPoint(0, 0);

    // Flags
    m_defaultMinMargins = false;
    m_enableMargins = true;
    m_enableOrientation = true;
    m_enablePaper = true;
    m_enablePrinter = true;
    m_enableHelp = false;
    m_getDefaultInfo = false;
}

wxPageSetupDialogData::wxPageSetupDialogData(const wxPageSetupDialogData& dialogData)
    : wxObject()
{
#if defined(__WIN95__)
    m_pageSetupData = NULL;
#endif
    (*this) = dialogData;
}

wxPageSetupDialogData::wxPageSetupDialogData(const wxPrintData& printData)
{
#if defined(__WIN95__)
    m_pageSetupData = NULL;
#endif
    m_paperSize = wxSize(0, 0);
    m_minMarginTopLeft = wxPoint(0, 0);
    m_minMarginBottomRight = wxPoint(0, 0);
    m_marginTopLeft = wxPoint(0, 0);
    m_marginBottomRight = wxPoint(0, 0);

    // Flags
    m_defaultMinMargins = false;
    m_enableMargins = true;
    m_enableOrientation = true;
    m_enablePaper = true;
    m_enablePrinter = true;
    m_enableHelp = false;
    m_getDefaultInfo = false;

    m_printData = printData;

    // The wxPrintData paper size overrides these values, unless the size cannot
    // be found.
    CalculatePaperSizeFromId();
}

wxPageSetupDialogData::~wxPageSetupDialogData()
{
#if defined(__WIN95__) && defined(__WXMSW__)
    PAGESETUPDLG *pd = (PAGESETUPDLG *)m_pageSetupData;
    if ( pd && pd->hDevMode )
        GlobalFree(pd->hDevMode);
    if ( pd && pd->hDevNames )
        GlobalFree(pd->hDevNames);
    if ( pd )
        delete pd;
#endif
}

wxPageSetupDialogData& wxPageSetupDialogData::operator=(const wxPageSetupDialogData& data)
{
    m_paperSize = data.m_paperSize;
    m_minMarginTopLeft = data.m_minMarginTopLeft;
    m_minMarginBottomRight = data.m_minMarginBottomRight;
    m_marginTopLeft = data.m_marginTopLeft;
    m_marginBottomRight = data.m_marginBottomRight;
    m_defaultMinMargins = data.m_defaultMinMargins;
    m_enableMargins = data.m_enableMargins;
    m_enableOrientation = data.m_enableOrientation;
    m_enablePaper = data.m_enablePaper;
    m_enablePrinter = data.m_enablePrinter;
    m_getDefaultInfo = data.m_getDefaultInfo;;
    m_enableHelp = data.m_enableHelp;

    m_printData = data.m_printData;

    return *this;
}

wxPageSetupDialogData& wxPageSetupDialogData::operator=(const wxPrintData& data)
{
    m_printData = data;

    return *this;
}

#if defined(__WIN95__)
void wxPageSetupDialogData::ConvertToNative()
{
    wxWindowsPrintNativeData *data =
        (wxWindowsPrintNativeData *) m_printData.GetNativeData();
    data->TransferFrom( m_printData );

    PAGESETUPDLG *pd = (PAGESETUPDLG*) m_pageSetupData;

    if ( m_pageSetupData == NULL )
    {
        pd = new PAGESETUPDLG;
        pd->hDevMode = NULL;
        pd->hDevNames = NULL;
        m_pageSetupData = (void *)pd;
    }

    // Pass the devmode data (created in m_printData.ConvertToNative)
    // to the PRINTDLG structure, since it'll
    // be needed when PrintDlg is called.

    if (pd->hDevMode)
    {
        GlobalFree(pd->hDevMode);
        pd->hDevMode = NULL;
    }

    pd->hDevMode = (HGLOBAL) data->GetDevMode();

    data->SetDevMode( (void*) NULL );

    // Shouldn't assert; we should be able to test Ok-ness at a higher level
    //wxASSERT_MSG( (pd->hDevMode), wxT("hDevMode must be non-NULL in ConvertToNative!"));

    // Pass the devnames data (created in m_printData.ConvertToNative)
    // to the PRINTDLG structure, since it'll
    // be needed when PrintDlg is called.

    if (pd->hDevNames)
    {
        GlobalFree(pd->hDevNames);
        pd->hDevNames = NULL;
    }

    pd->hDevNames = (HGLOBAL) data->GetDevNames();

    data->SetDevNames((void*) NULL);

//        pd->hDevMode = GlobalAlloc(GMEM_MOVEABLE, sizeof(DEVMODE));

    pd->Flags = PSD_MARGINS|PSD_MINMARGINS;

    if ( m_defaultMinMargins )
        pd->Flags |= PSD_DEFAULTMINMARGINS;
    if ( !m_enableMargins )
        pd->Flags |= PSD_DISABLEMARGINS;
    if ( !m_enableOrientation )
        pd->Flags |= PSD_DISABLEORIENTATION;
    if ( !m_enablePaper )
        pd->Flags |= PSD_DISABLEPAPER;
    if ( !m_enablePrinter )
        pd->Flags |= PSD_DISABLEPRINTER;
    if ( m_getDefaultInfo )
        pd->Flags |= PSD_RETURNDEFAULT;
    if ( m_enableHelp )
        pd->Flags |= PSD_SHOWHELP;

    // We want the units to be in hundredths of a millimetre
    pd->Flags |= PSD_INHUNDREDTHSOFMILLIMETERS;

    pd->lStructSize = sizeof( PAGESETUPDLG );
    pd->hwndOwner=(HWND)NULL;
//    pd->hDevNames=(HWND)NULL;
    pd->hInstance=(HINSTANCE)NULL;
    //   PAGESETUPDLG is in hundreds of a mm
    pd->ptPaperSize.x = m_paperSize.x * 100;
    pd->ptPaperSize.y = m_paperSize.y * 100;

    pd->rtMinMargin.left = m_minMarginTopLeft.x * 100;
    pd->rtMinMargin.top = m_minMarginTopLeft.y * 100;
    pd->rtMinMargin.right = m_minMarginBottomRight.x * 100;
    pd->rtMinMargin.bottom = m_minMarginBottomRight.y * 100;

    pd->rtMargin.left = m_marginTopLeft.x * 100;
    pd->rtMargin.top = m_marginTopLeft.y * 100;
    pd->rtMargin.right = m_marginBottomRight.x * 100;
    pd->rtMargin.bottom = m_marginBottomRight.y * 100;

    pd->lCustData = 0;
    pd->lpfnPageSetupHook = NULL;
    pd->lpfnPagePaintHook = NULL;
    pd->hPageSetupTemplate = NULL;
    pd->lpPageSetupTemplateName = NULL;

/*
    if ( pd->hDevMode )
    {
        DEVMODE *devMode = (DEVMODE*) GlobalLock(pd->hDevMode);
        memset(devMode, 0, sizeof(DEVMODE));
        devMode->dmSize = sizeof(DEVMODE);
        devMode->dmOrientation = m_orientation;
        devMode->dmFields = DM_ORIENTATION;
        GlobalUnlock(pd->hDevMode);
    }
*/
}

void wxPageSetupDialogData::ConvertFromNative()
{
    PAGESETUPDLG *pd = (PAGESETUPDLG *) m_pageSetupData;
    if ( !pd )
        return;

    wxWindowsPrintNativeData *data =
        (wxWindowsPrintNativeData *) m_printData.GetNativeData();
        
    // Pass the devmode data back to the wxPrintData structure where it really belongs.
    if (pd->hDevMode)
    {
        if (data->GetDevMode())
        {
            // Make sure we don't leak memory
            GlobalFree((HGLOBAL) data->GetDevMode());
        }
        data->SetDevMode( (void*) pd->hDevMode );
        pd->hDevMode = NULL;
    }

    data->TransferTo( m_printData );

    // Pass the devnames data back to the wxPrintData structure where it really belongs.
    if (pd->hDevNames)
    {
        if (data->GetDevNames())
        {
            // Make sure we don't leak memory
            GlobalFree((HGLOBAL) data->GetDevNames());
        }
        data->SetDevNames((void*) pd->hDevNames);
        pd->hDevNames = NULL;
    }

    data->TransferTo( m_printData );

    pd->Flags = PSD_MARGINS|PSD_MINMARGINS;

    m_defaultMinMargins = ((pd->Flags & PSD_DEFAULTMINMARGINS) == PSD_DEFAULTMINMARGINS);
    m_enableMargins = ((pd->Flags & PSD_DISABLEMARGINS) != PSD_DISABLEMARGINS);
    m_enableOrientation = ((pd->Flags & PSD_DISABLEORIENTATION) != PSD_DISABLEORIENTATION);
    m_enablePaper = ((pd->Flags & PSD_DISABLEPAPER) != PSD_DISABLEPAPER);
    m_enablePrinter = ((pd->Flags & PSD_DISABLEPRINTER) != PSD_DISABLEPRINTER);
    m_getDefaultInfo = ((pd->Flags & PSD_RETURNDEFAULT) == PSD_RETURNDEFAULT);
    m_enableHelp = ((pd->Flags & PSD_SHOWHELP) == PSD_SHOWHELP);

    //   PAGESETUPDLG is in hundreds of a mm
    m_paperSize.x = pd->ptPaperSize.x / 100;
    m_paperSize.y = pd->ptPaperSize.y / 100;

    m_minMarginTopLeft.x = pd->rtMinMargin.left / 100;
    m_minMarginTopLeft.y = pd->rtMinMargin.top / 100;
    m_minMarginBottomRight.x = pd->rtMinMargin.right / 100;
    m_minMarginBottomRight.y = pd->rtMinMargin.bottom / 100;

    m_marginTopLeft.x = pd->rtMargin.left / 100;
    m_marginTopLeft.y = pd->rtMargin.top / 100;
    m_marginBottomRight.x = pd->rtMargin.right / 100;
    m_marginBottomRight.y = pd->rtMargin.bottom / 100;
}

void wxPageSetupDialogData::SetOwnerWindow(wxWindow* win)
{
    if ( m_pageSetupData == NULL )
        ConvertToNative();

    if ( m_pageSetupData != NULL && win != NULL)
    {
        PAGESETUPDLG *pd = (PAGESETUPDLG *) m_pageSetupData;
        pd->hwndOwner=(HWND) win->GetHWND();
    }
}
#endif // Win95

#ifdef __WXMAC__
void wxPageSetupDialogData::ConvertToNative()
{
    m_printData.ConvertToNative();
    m_printData.m_nativePrintData->TransferFrom( this ) ;
}

void wxPageSetupDialogData::ConvertFromNative()
{
    m_printData.ConvertFromNative ();
    m_paperSize = m_printData.GetPaperSize() ;
    CalculateIdFromPaperSize();
    m_printData.m_nativePrintData->TransferTo( this ) ;
    // adjust minimal values

    if ( m_marginTopLeft.x < m_minMarginTopLeft.x )
        m_marginTopLeft.x = m_minMarginTopLeft.x;

    if ( m_marginBottomRight.x < m_minMarginBottomRight.x )
        m_marginBottomRight.x = m_minMarginBottomRight.x;

    if ( m_marginTopLeft.y < m_minMarginTopLeft.y )
        m_marginTopLeft.y = m_minMarginTopLeft.y;

    if ( m_marginBottomRight.y < m_minMarginBottomRight.y )
        m_marginBottomRight.y = m_minMarginBottomRight.y;
}
#endif


// If a corresponding paper type is found in the paper database, will set the m_printData
// paper size id member as well.
void wxPageSetupDialogData::SetPaperSize(const wxSize& sz)
{
    m_paperSize = sz;

    CalculateIdFromPaperSize();
}

// Sets the wxPrintData id, plus the paper width/height if found in the paper database.
void wxPageSetupDialogData::SetPaperSize(wxPaperSize id)
{
    m_printData.SetPaperId(id);

    CalculatePaperSizeFromId();
}

// Use paper size defined in this object to set the wxPrintData
// paper id
void wxPageSetupDialogData::CalculateIdFromPaperSize()
{
    wxASSERT_MSG( (wxThePrintPaperDatabase != (wxPrintPaperDatabase*) NULL),
                  wxT("wxThePrintPaperDatabase should not be NULL. Do not create global print dialog data objects.") );

    wxSize sz = GetPaperSize();

    wxPaperSize id = wxThePrintPaperDatabase->GetSize(wxSize(sz.x* 10, sz.y * 10));
    if (id != wxPAPER_NONE)
    {
        m_printData.SetPaperId(id);
    }
}

// Use paper id in wxPrintData to set this object's paper size
void wxPageSetupDialogData::CalculatePaperSizeFromId()
{
    wxASSERT_MSG( (wxThePrintPaperDatabase != (wxPrintPaperDatabase*) NULL),
                  wxT("wxThePrintPaperDatabase should not be NULL. Do not create global print dialog data objects.") );

    wxSize sz = wxThePrintPaperDatabase->GetSize(m_printData.GetPaperId());

    // sz is in 10ths of a mm, while paper size is in mm
    m_paperSize.x = sz.x / 10;
    m_paperSize.y = sz.y / 10;
}

#endif // wxUSE_PRINTING_ARCHITECTURE
