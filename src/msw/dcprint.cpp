/////////////////////////////////////////////////////////////////////////////
// Name:        dcprint.cpp
// Purpose:     wxPrinterDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#pragma implementation "dcprint.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif

#include "wx/dcprint.h"
#include "math.h"
#include "fstream.h"

#include <windows.h>

#if wxUSE_COMMON_DIALOGS
#include <commdlg.h>
#endif

#ifndef __WIN32__
#include <print.h>
#endif

#ifdef DrawText
#undef DrawText
#endif

#ifdef GetCharWidth
#undef GetCharWidth
#endif

#ifdef StartDoc
#undef StartDoc
#endif

#if !USE_SHARED_LIBRARY
IMPLEMENT_CLASS(wxPrinterDC, wxDC)
#endif

wxPrinterDC::wxPrinterDC(const wxString& driver_name, const wxString& device_name, const wxString& file, bool interactive, int orientation)
{
  m_isInteractive = interactive;

  if (!file.IsNull() && file != "")
    m_filename = file;

#if wxUSE_COMMON_DIALOGS
   if (interactive)
   {
     PRINTDLG pd;
	
     pd.lStructSize = sizeof( PRINTDLG );
     pd.hwndOwner=NULL;
     pd.hDevMode=(HANDLE)NULL;
     pd.hDevNames=(HANDLE)NULL;
     pd.Flags=PD_RETURNDC | PD_NOSELECTION | PD_NOPAGENUMS;
     pd.nFromPage=0;
     pd.nToPage=0;
     pd.nMinPage=0;
     pd.nMaxPage=0;
     pd.nCopies=1;
     pd.hInstance=(HINSTANCE)NULL;

     if ( PrintDlg( &pd ) != 0 )
     {
       m_hDC = (WXHDC) pd.hDC;
       m_ok = TRUE;
     }
     else
     {
       m_ok = FALSE;
       return;
     }

//     m_dontDelete = TRUE;
   }
   else
#endif
   if ((!driver_name.IsNull() && driver_name != "") &&
       (!device_name.IsNull() && device_name != "") &&
       (!file.IsNull() && file != ""))
   {
     m_hDC = (WXHDC) CreateDC((char *) (const char *) driver_name, (char *) (const char *) device_name, (char *) (const char *) file, NULL);
     m_ok = m_hDC ? TRUE: FALSE;
   }
   else
   {
     m_hDC = wxGetPrinterDC(orientation);
     m_ok = m_hDC ? TRUE: FALSE;
   }
   
   if (m_hDC)
   {
//     int width = GetDeviceCaps(m_hDC, VERTRES);
//     int height = GetDeviceCaps(m_hDC, HORZRES);
     SetMapMode(MM_TEXT);
   }
  SetBrush(*wxBLACK_BRUSH);
  SetPen(*wxBLACK_PEN);
}

wxPrinterDC::wxPrinterDC(WXHDC theDC)
{
  m_isInteractive = FALSE;

  m_hDC = theDC;
  m_ok = TRUE;
  if (m_hDC)
 {
//     int width = GetDeviceCaps(m_hDC, VERTRES);
//     int height = GetDeviceCaps(m_hDC, HORZRES);
    SetMapMode(MM_TEXT);
  }
  SetBrush(*wxBLACK_BRUSH);
  SetPen(*wxBLACK_PEN);
}

wxPrinterDC::~wxPrinterDC(void)
{
}

WXHDC wxGetPrinterDC(int orientation)
{
    HDC         hDC;
    LPDEVMODE   lpDevMode = NULL;
    LPDEVNAMES  lpDevNames;
    LPSTR       lpszDriverName;
    LPSTR       lpszDeviceName;
    LPSTR       lpszPortName;

    PRINTDLG    pd;
    // __GNUWIN32__ has trouble believing PRINTDLG is 66 bytes - thinks it is 68
    pd.lStructSize    = 66; // sizeof(PRINTDLG);
    pd.hwndOwner      = (HWND)NULL;
    pd.hDevMode       = NULL; // Will be created by PrintDlg
    pd.hDevNames      = NULL; // Ditto
    pd.Flags          = PD_RETURNDEFAULT;
    pd.nCopies        = 1;

    if (!PrintDlg((LPPRINTDLG)&pd))
    {
        if ( pd.hDevMode )
            GlobalFree(pd.hDevMode);
        if (pd.hDevNames)
            GlobalFree(pd.hDevNames);

        return(0);
    }

    if (!pd.hDevNames)
    {
        if ( pd.hDevMode )
            GlobalFree(pd.hDevMode);
    }

    lpDevNames = (LPDEVNAMES)GlobalLock(pd.hDevNames);
    lpszDriverName = (LPSTR)lpDevNames + lpDevNames->wDriverOffset;
    lpszDeviceName = (LPSTR)lpDevNames + lpDevNames->wDeviceOffset;
    lpszPortName   = (LPSTR)lpDevNames + lpDevNames->wOutputOffset;
    GlobalUnlock(pd.hDevNames);

    if ( pd.hDevMode )
    {
        lpDevMode = (DEVMODE*) GlobalLock(pd.hDevMode);
        lpDevMode->dmOrientation = orientation;
        lpDevMode->dmFields |= DM_ORIENTATION;
    }

#ifdef __WIN32__
    hDC = CreateDC(lpszDriverName, lpszDeviceName, lpszPortName, (DEVMODE *)lpDevMode);
#else
    hDC = CreateDC(lpszDriverName, lpszDeviceName, lpszPortName, (LPSTR)lpDevMode);
#endif

    if (pd.hDevMode && lpDevMode)
        GlobalUnlock(pd.hDevMode);

    if (pd.hDevNames)
    {
	    GlobalFree(pd.hDevNames);
	    pd.hDevNames=NULL;
    }
    if (pd.hDevMode)
    {
        GlobalFree(pd.hDevMode);
        pd.hDevMode=NULL;
    }
    return (WXHDC) hDC;
}


