/////////////////////////////////////////////////////////////////////////////
// Name:        cmndata.cpp
// Purpose:     Common GDI data
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "cmndata.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/utils.h"
#include "wx/app.h"
#endif

#include "wx/gdicmn.h"
#include "wx/cmndata.h"

#ifdef __WINDOWS__
#include <windows.h>

#ifndef __WIN32__
#include <print.h>
#include <commdlg.h>
#endif
#endif

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxPrintData, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxPageSetupData, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxFontData, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxColourData, wxObject)
#endif

/*
 * wxColourData
 */

wxColourData::wxColourData(void)
{
  int i;
  for (i = 0; i < 16; i++)
    custColours[i].Set(255, 255, 255);

  chooseFull = FALSE;
  dataColour.Set(0,0,0);
} 

wxColourData::~wxColourData(void)
{
}

void wxColourData::SetCustomColour(int i, wxColour& colour)
{
  if (i > 15 || i < 0)
    return;

  custColours[i] = colour;
}

wxColour wxColourData::GetCustomColour(int i)
{
  if (i > 15 || i < 0)
    return wxColour(0,0,0);

  return custColours[i];
}

void wxColourData::operator=(const wxColourData& data)
{
  int i;
  for (i = 0; i < 16; i++)
    custColours[i] = data.custColours[i];

  dataColour = (wxColour&)data.dataColour;
  chooseFull = data.chooseFull;
}

/*
 * Font data
 */

wxFontData::wxFontData(void)
{
  // Intialize colour to black.
  fontColour.Set(0, 0, 0);

  showHelp = FALSE;
  allowSymbols = TRUE;
  enableEffects = TRUE;
  minSize = 0;
  maxSize = 0;
}

wxFontData::~wxFontData(void)
{
}

void wxFontData::operator=(const wxFontData& data)
{
   fontColour = data.fontColour;
   showHelp = data.showHelp;
   allowSymbols = data.allowSymbols;
   enableEffects = data.enableEffects;
   initialFont = data.initialFont;
   chosenFont = data.chosenFont;
   minSize = data.minSize;
   maxSize = data.maxSize;
}

/*
 * Print data
 */

wxPrintData::wxPrintData(void)
{
#ifdef __WINDOWS__
    printData = NULL;
#endif
    printOrientation = wxPORTRAIT;
    printFromPage = 0;
    printToPage = 0;
    printMinPage = 0;
    printMaxPage = 0;
    printNoCopies = 1;
    printAllPages = FALSE;
    printCollate = FALSE;
    printToFile = FALSE;
    printEnableSelection = FALSE;
    printEnablePageNumbers = TRUE;
    printEnablePrintToFile = TRUE;
    printEnableHelp = FALSE;
    printSetupDialog = FALSE;
}

wxPrintData::~wxPrintData(void)
{
#ifdef __WINDOWS__
    PRINTDLG *pd = (PRINTDLG *)printData;
    if ( pd && pd->hDevMode )
        GlobalFree(pd->hDevMode);
    if ( pd )
        delete pd;
#endif
}

#ifdef __WINDOWS__
void wxPrintData::ConvertToNative(void)
{
    PRINTDLG *pd = (PRINTDLG*) printData;
    if ( pd == NULL )
    {
        pd = new PRINTDLG;
        printData = (void*) pd;

        // GNU-WIN32 has the wrong size PRINTDLG - can't work out why.
//        pd->lStructSize    = sizeof(PRINTDLG);
        pd->lStructSize    = 66 ;
        pd->hwndOwner      = (HWND)NULL;
        pd->hDevMode       = NULL; // Will be created by PrintDlg
        pd->hDevNames      = NULL; // Ditto

        // Why had I put this #ifdef in?? Seems fine to me.
#if 1
        pd->Flags          = PD_RETURNDEFAULT;
        pd->nCopies        = 1;

        // Fill out the DEVMODE structure
        // so we can use it as input in the 'real' PrintDlg
        if (!PrintDlg(pd))
        {
            if ( pd->hDevMode )
                GlobalFree(pd->hDevMode);
            if ( pd->hDevNames )
                GlobalFree(pd->hDevNames);
            pd->hDevMode = NULL;
            pd->hDevNames = NULL;
        }
        else
        {
            if ( pd->hDevNames )
                GlobalFree(pd->hDevNames);
            pd->hDevNames = NULL;
        }
#endif
    }

    if ( pd->hDevMode )
    {
        DEVMODE *devMode = (DEVMODE*) GlobalLock(pd->hDevMode);
        devMode->dmOrientation = printOrientation;
        devMode->dmFields = DM_ORIENTATION;
        GlobalUnlock(pd->hDevMode);
    }
    pd->hDC = (HDC) NULL;
    pd->nFromPage = (UINT)printFromPage;
    pd->nToPage = (UINT)printToPage;
    pd->nMinPage = (UINT)printMinPage;
    pd->nMaxPage = (UINT)printMaxPage;
    pd->nCopies = (UINT)printNoCopies;

    pd->Flags = PD_RETURNDC ;
//    pd->lStructSize = sizeof( PRINTDLG );
    pd->lStructSize = 66 ;
    pd->hwndOwner=(HANDLE)NULL;
    pd->hDevNames=(HANDLE)NULL;
    pd->hInstance=(HINSTANCE)NULL;
    pd->lCustData = (LPARAM) NULL;
    pd->lpfnPrintHook = NULL;
    pd->lpfnSetupHook = NULL;
    pd->lpPrintTemplateName = NULL;
    pd->lpSetupTemplateName = NULL;
    pd->hPrintTemplate = (HGLOBAL) NULL;
    pd->hSetupTemplate = (HGLOBAL) NULL;

    if ( printAllPages )
        pd->Flags |= PD_ALLPAGES;
    if ( printCollate )
        pd->Flags |= PD_COLLATE;
    if ( printToFile )
        pd->Flags |= PD_PRINTTOFILE;
    if ( !printEnablePrintToFile )
        pd->Flags |= PD_DISABLEPRINTTOFILE;
    if ( !printEnableSelection )
      pd->Flags |= PD_NOSELECTION;
    if ( !printEnablePageNumbers )
      pd->Flags |= PD_NOPAGENUMS;
    if ( printEnableHelp )
      pd->Flags |= PD_SHOWHELP;
    if ( printSetupDialog )
      pd->Flags |= PD_PRINTSETUP;
}

void wxPrintData::ConvertFromNative(void)
{
    PRINTDLG *pd = (PRINTDLG*) printData;
    if ( pd == NULL )
        return;

    if ( pd->hDevMode )
    {
        DEVMODE *devMode = (DEVMODE*) GlobalLock(pd->hDevMode);
        printOrientation = devMode->dmOrientation;
        GlobalUnlock(pd->hDevMode);
    }
    printFromPage = pd->nFromPage ;
    printToPage = pd->nToPage ;
    printMinPage = pd->nMinPage ;
    printMaxPage = pd->nMaxPage ;
    printNoCopies = pd->nCopies ;

    printAllPages = ((pd->Flags & PD_ALLPAGES) == PD_ALLPAGES);
    printCollate = ((pd->Flags & PD_COLLATE) == PD_COLLATE);
    printToFile = ((pd->Flags & PD_PRINTTOFILE) == PD_PRINTTOFILE);
    printEnablePrintToFile = ((pd->Flags & PD_DISABLEPRINTTOFILE) != PD_DISABLEPRINTTOFILE);
    printEnableSelection = ((pd->Flags & PD_NOSELECTION) != PD_NOSELECTION);
    printEnablePageNumbers = ((pd->Flags & PD_NOPAGENUMS) != PD_NOPAGENUMS);
    printEnableHelp = ((pd->Flags & PD_SHOWHELP) == PD_SHOWHELP);
    printSetupDialog = ((pd->Flags & PD_PRINTSETUP) == PD_PRINTSETUP);
}

void wxPrintData::SetOwnerWindow(wxWindow* win)
{
    if ( printData == NULL )
        ConvertToNative();

    if ( printData != NULL && win != NULL)
    {
      PRINTDLG *pd = (PRINTDLG *) printData ;
      pd->hwndOwner=(HWND) win->GetHWND();
    }
}
#endif

void wxPrintData::operator=(const wxPrintData& data)
{
    printFromPage = data.printFromPage;
    printToPage = data.printToPage;
    printMinPage = data.printMinPage;
    printMaxPage = data.printMaxPage;
    printNoCopies = data.printNoCopies;
    printAllPages = data.printAllPages;
    printCollate = data.printCollate;
    printToFile = data.printToFile;
    printEnableSelection = data.printEnableSelection;
    printEnablePageNumbers = data.printEnablePageNumbers;
    printEnableHelp = data.printEnableHelp;
    printEnablePrintToFile = data.printEnablePrintToFile;
    printSetupDialog = data.printSetupDialog;
    printOrientation = data.printOrientation;
}

/*
 * wxPageSetupData
 */

wxPageSetupData::wxPageSetupData(void)
{
#if defined(__WIN95__)
  m_pageSetupData = NULL;
#endif
  m_paperSize = wxPoint(0, 0);
  m_minMarginTopLeft = wxPoint(0, 0);
  m_minMarginBottomRight = wxPoint(0, 0);
  m_marginTopLeft = wxPoint(0, 0);
  m_marginBottomRight = wxPoint(0, 0);
  m_orientation = wxPORTRAIT;

  // Flags
  m_defaultMinMargins = FALSE;
  m_enableMargins = TRUE;
  m_enableOrientation = TRUE;
  m_enablePaper = TRUE;
  m_enablePrinter = TRUE;
  m_enableHelp = FALSE;
  m_getDefaultInfo = FALSE;
}

wxPageSetupData::~wxPageSetupData(void)
{
#if defined(__WIN95__)
    PAGESETUPDLG *pd = (PAGESETUPDLG *)m_pageSetupData;
    if ( pd && pd->hDevMode )
        GlobalFree(pd->hDevMode);
    if ( pd )
        delete pd;
#endif
}

void wxPageSetupData::operator=(const wxPageSetupData& data)
{
  m_paperSize = data.m_paperSize;
  m_minMarginTopLeft = data.m_minMarginTopLeft;
  m_minMarginBottomRight = data.m_minMarginBottomRight;
  m_marginTopLeft = data.m_marginTopLeft;
  m_marginBottomRight = data.m_marginBottomRight;
  m_orientation = data.m_orientation;

  m_defaultMinMargins = data.m_defaultMinMargins;
  m_enableMargins = data.m_enableMargins;
  m_enableOrientation = data.m_enableOrientation;
  m_enablePaper = data.m_enablePaper;
  m_enablePrinter = data.m_enablePrinter;
  m_getDefaultInfo = data.m_getDefaultInfo;;
  m_enableHelp = data.m_enableHelp;
}

#if defined(__WIN95__)
void wxPageSetupData::ConvertToNative(void)
{
    PAGESETUPDLG *pd = (PAGESETUPDLG*) m_pageSetupData;
    if ( m_pageSetupData == NULL )
    {
      pd = new PAGESETUPDLG;
      pd->hDevMode = GlobalAlloc(GMEM_MOVEABLE, sizeof(DEVMODE));
      m_pageSetupData = (void *)pd;
    }
  
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

    pd->lStructSize = sizeof( PAGESETUPDLG );
    pd->hwndOwner=(HANDLE)NULL;
    pd->hDevNames=(HWND)NULL;
    pd->hInstance=(HINSTANCE)NULL;

    pd->ptPaperSize.x = m_paperSize.x;
    pd->ptPaperSize.y = m_paperSize.y;

    pd->rtMinMargin.left = m_minMarginTopLeft.x;
    pd->rtMinMargin.top = m_minMarginTopLeft.y;
    pd->rtMinMargin.right = m_minMarginBottomRight.x;
    pd->rtMinMargin.bottom = m_minMarginBottomRight.y;

    pd->rtMargin.left = m_marginTopLeft.x;
    pd->rtMargin.top = m_marginTopLeft.y;
    pd->rtMargin.right = m_marginBottomRight.x;
    pd->rtMargin.bottom = m_marginBottomRight.y;

    pd->lCustData = 0;
    pd->lpfnPageSetupHook = NULL;
    pd->lpfnPagePaintHook = NULL;
    pd->hPageSetupTemplate = NULL;
    pd->lpPageSetupTemplateName = NULL;

    if ( pd->hDevMode )
    {
        DEVMODE *devMode = (DEVMODE*) GlobalLock(pd->hDevMode);
        memset(devMode, 0, sizeof(DEVMODE));
        devMode->dmSize = sizeof(DEVMODE);
        devMode->dmOrientation = m_orientation;
        devMode->dmFields = DM_ORIENTATION;
        GlobalUnlock(pd->hDevMode);
    }
}

void wxPageSetupData::ConvertFromNative(void)
{
    PAGESETUPDLG *pd = (PAGESETUPDLG *) m_pageSetupData ;
    if ( !pd )
        return;

    pd->Flags = PSD_MARGINS|PSD_MINMARGINS;

    m_defaultMinMargins = ((pd->Flags & PSD_DEFAULTMINMARGINS) == PSD_DEFAULTMINMARGINS);
    m_enableMargins = ((pd->Flags & PSD_DISABLEMARGINS) != PSD_DISABLEMARGINS);
    m_enableOrientation = ((pd->Flags & PSD_DISABLEORIENTATION) != PSD_DISABLEORIENTATION);
    m_enablePaper = ((pd->Flags & PSD_DISABLEPAPER) != PSD_DISABLEPAPER);
    m_enablePrinter = ((pd->Flags & PSD_DISABLEPRINTER) != PSD_DISABLEPRINTER);
    m_getDefaultInfo = ((pd->Flags & PSD_RETURNDEFAULT) == PSD_RETURNDEFAULT);
    m_enableHelp = ((pd->Flags & PSD_SHOWHELP) == PSD_SHOWHELP);

    m_paperSize.x = pd->ptPaperSize.x ;
    m_paperSize.y = pd->ptPaperSize.y ;

    m_minMarginTopLeft.x = pd->rtMinMargin.left ;
    m_minMarginTopLeft.y = pd->rtMinMargin.top ;
    m_minMarginBottomRight.x = pd->rtMinMargin.right ;
    m_minMarginBottomRight.y = pd->rtMinMargin.bottom ;

    m_marginTopLeft.x = pd->rtMargin.left ;
    m_marginTopLeft.y = pd->rtMargin.top ;
    m_marginBottomRight.x = pd->rtMargin.right ;
    m_marginBottomRight.y = pd->rtMargin.bottom ;

    if ( pd->hDevMode )
    {
        DEVMODE *devMode = (DEVMODE*) GlobalLock(pd->hDevMode);
        m_orientation = devMode->dmOrientation ;
        GlobalUnlock(pd->hDevMode);
    }
}

void wxPageSetupData::SetOwnerWindow(wxWindow* win)
{
    if ( m_pageSetupData == NULL )
        ConvertToNative();

    if ( m_pageSetupData != NULL && win != NULL)
    {
      PAGESETUPDLG *pd = (PAGESETUPDLG *) m_pageSetupData ;
      pd->hwndOwner=(HWND) win->GetHWND();
    }
}
#endif

