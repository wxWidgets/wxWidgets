/////////////////////////////////////////////////////////////////////////////
// Name:        dcprint.cpp
// Purpose:     wxPrinterDC class
// Author:      David Webster
// Modified by:
// Created:     10/14/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define INCL_DEV
#define INCL_GPI
#define INCL_PM
#include<os2.h>

#ifndef WX_PRECOMP
#endif

#include "wx/string.h"
#include "wx/log.h"
#include "wx/window.h"
#include "wx/os2/private.h"
#include "wx/dcprint.h"
#include "math.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_CLASS(wxPrinterDC, wxDC)
#endif


// This form is deprecated
wxPrinterDC::wxPrinterDC(const wxString& driver_name, const wxString& device_name, const wxString& file, bool interactive, int orientation)
{
    LONG            lType;
    HAB             hab;
    DEVOPENSTRUC    devOpen = { (char*)device_name.c_str()
                               ,(char*)driver_name.c_str()
                               ,NULL
                               ,NULL
                               ,NULL
                               ,NULL
                               ,NULL
                               ,NULL
                               ,NULL
                              };

    m_isInteractive = interactive;

    if (!file.IsNull() && file != wxT(""))
        m_printData.SetFilename(file);

/*
    Implement PM's version of this
#if wxUSE_COMMON_DIALOGS
    if (interactive)
    {
        PRINTDLG pd;

        pd.lStructSize = sizeof( PRINTDLG );
        pd.hwndOwner=(HWND) NULL;
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
*/
        if ((!driver_name.IsNull() && driver_name != wxT("")) &&
            (!device_name.IsNull() && device_name != wxT("")) &&
            (!file.IsNull() && file != wxT("")))
        {
            m_hDC = (WXHDC) ::DevOpenDC( hab
                                        ,OD_QUEUED
                                        ,"*"
                                        ,5L
                                        ,(PDEVOPENDATA)&devOpen
                                        ,NULLHANDLE
                                       );
            m_ok = m_hDC ? TRUE: FALSE;
        }
        else
        {
            wxPrintData printData;
            printData.SetOrientation(orientation);
            m_hDC = wxGetPrinterDC(printData);
            m_ok = m_hDC ? TRUE: FALSE;
        }

        if (m_hDC)
        {
            //     int width = GetDeviceCaps(m_hDC, VERTRES);
            //     int height = GetDeviceCaps(m_hDC, HORZRES);
            SetMapMode(wxMM_TEXT);
        }
        SetBrush(*wxBLACK_BRUSH);
        SetPen(*wxBLACK_PEN);
}

wxPrinterDC::wxPrinterDC(const wxPrintData& printData)
{
    m_printData = printData;

    m_isInteractive = FALSE;

    m_hDC = wxGetPrinterDC(printData);
    m_ok = (m_hDC != 0);

    if (m_hDC)
        SetMapMode(wxMM_TEXT);

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
        SetMapMode(wxMM_TEXT);
    }
    SetBrush(*wxBLACK_BRUSH);
    SetPen(*wxBLACK_PEN);
}

wxPrinterDC::~wxPrinterDC(void)
{
}

bool wxPrinterDC::StartDoc(const wxString& message)
{
/* TODO:  PM's implementation
   DOCINFO docinfo;
    docinfo.cbSize = sizeof(DOCINFO);
    docinfo.lpszDocName = (const wxChar*)message;

    wxString filename(m_printData.GetFilename());

    if (filename.IsEmpty())
        docinfo.lpszOutput = NULL;
    else
        docinfo.lpszOutput = (const wxChar *) filename;

#if defined(__WIN95__)
    docinfo.lpszDatatype = NULL;
    docinfo.fwType = 0;
#endif

    if (!m_hDC)
        return FALSE;

    int ret =
#ifndef __WIN32__
        ::StartDoc((HDC) m_hDC, &docinfo);
#else
#ifdef UNICODE
    ::StartDocW((HDC) m_hDC, &docinfo);
#else
#ifdef __TWIN32__
    ::StartDoc((HDC) m_hDC, &docinfo);
#else
    ::StartDocA((HDC) m_hDC, &docinfo);
#endif
#endif
#endif

#ifndef __WIN16__
    if (ret <= 0)
    {
        DWORD lastError = GetLastError();
        wxLogDebug(wxT("wxDC::StartDoc failed with error: %d\n"), lastError);
    }
#endif
    return (ret > 0);
*/
    return(TRUE);
}

void wxPrinterDC::EndDoc(void)
{
//    if (m_hDC) ::EndDoc((HDC) m_hDC);
}

void wxPrinterDC::StartPage(void)
{
//    if (m_hDC)
//        ::StartPage((HDC) m_hDC);
}

void wxPrinterDC::EndPage(void)
{
//    if (m_hDC)
//        ::EndPage((HDC) m_hDC);
}

// Returns default device and port names
static bool wxGetDefaultDeviceName(wxString& deviceName, wxString& portName)
{
    deviceName = "";
/*
    LPDEVNAMES  lpDevNames;
    LPSTR       lpszDriverName;
    LPSTR       lpszDeviceName;
    LPSTR       lpszPortName;

    PRINTDLG    pd;

    // Cygwin has trouble believing PRINTDLG is 66 bytes - thinks it is 68
#ifdef __GNUWIN32__
    pd.lStructSize    = 66; // sizeof(PRINTDLG);
#else
    pd.lStructSize    = sizeof(PRINTDLG);
#endif

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

        return FALSE;
    }

    if (pd.hDevNames)
    {
        lpDevNames = (LPDEVNAMES)GlobalLock(pd.hDevNames);
        lpszDriverName = (LPSTR)lpDevNames + lpDevNames->wDriverOffset;
        lpszDeviceName = (LPSTR)lpDevNames + lpDevNames->wDeviceOffset;
        lpszPortName   = (LPSTR)lpDevNames + lpDevNames->wOutputOffset;
        GlobalUnlock(pd.hDevNames);
        GlobalFree(pd.hDevNames);
        pd.hDevNames=NULL;

        deviceName = lpszDeviceName;
        portName = lpszPortName;
    }

    if (pd.hDevMode)
    {
        GlobalFree(pd.hDevMode);
        pd.hDevMode=NULL;
    }
    return ( deviceName != wxT("") );
*/
    return(TRUE);
}

// Gets an HDC for the specified printer configuration
WXHDC WXDLLEXPORT wxGetPrinterDC(const wxPrintData& printDataConst)
{
    HDC   hDC;
/*
    wxPrintData printData = printDataConst;
    printData.ConvertToNative();

    wxChar* driverName = (wxChar*) NULL;

    wxString devNameStr = printData.GetPrinterName();
    wxChar* deviceName;
    wxChar* portName = (wxChar*) NULL; // Obsolete in WIN32

    if (devNameStr == wxT(""))
        deviceName = (wxChar*) NULL;
    else
        deviceName = WXSTRINGCAST devNameStr;

    LPDEVMODE lpDevMode = (LPDEVMODE) NULL;

    HGLOBAL hDevMode = (HGLOBAL)(DWORD) printData.GetNativeData();

    if ( hDevMode )
        lpDevMode = (DEVMODE*) GlobalLock(hDevMode);

    if (devNameStr == wxT(""))
    {
        // Retrieve the default device name
        wxString portName;
        bool ret = wxGetDefaultDeviceName(devNameStr, portName);

        wxASSERT_MSG( ret, wxT("Could not get default device name.") );

        deviceName = WXSTRINGCAST devNameStr;
    }

#ifdef __WIN32__
    HDC hDC = CreateDC(driverName, deviceName, portName, (DEVMODE *) lpDevMode);
#else
    HDC hDC = CreateDC(driverName, deviceName, portName, (LPSTR) lpDevMode);
#endif

    if (hDevMode && lpDevMode)
        GlobalUnlock(hDevMode);
*/
    return (WXHDC) hDC;
}

