/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/dcprint.cpp
// Purpose:     wxPrinterDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "dcprint.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/window.h"
    #include "wx/dcmemory.h"
#endif

#include "wx/msw/private.h"
#include "wx/dcprint.h"
#include "math.h"

#if wxUSE_COMMON_DIALOGS || defined(__WXWINE__)
    #include <commdlg.h>
#endif

#ifndef __WIN32__
    #include <print.h>
#endif

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------
IMPLEMENT_CLASS(wxPrinterDC, wxDC)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxPrinterDC construction
// ----------------------------------------------------------------------------

// This form is deprecated
wxPrinterDC::wxPrinterDC(const wxString& driver_name, const wxString& device_name, const wxString& file, bool interactive, int orientation)
{
    m_isInteractive = interactive;

    if ( !!file )
        m_printData.SetFilename(file);

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
#endif // wxUSE_COMMON_DIALOGS
        if ((!driver_name.IsNull() && driver_name != wxT("")) &&
            (!device_name.IsNull() && device_name != wxT("")) &&
            (!file.IsNull() && file != wxT("")))
        {
            m_hDC = (WXHDC) CreateDC(WXSTRINGCAST driver_name, WXSTRINGCAST device_name, WXSTRINGCAST file, NULL);
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

wxPrinterDC::~wxPrinterDC()
{
}

// ----------------------------------------------------------------------------
// wxPrinterDC {Start/End}{Page/Doc} methods
// ----------------------------------------------------------------------------

bool wxPrinterDC::StartDoc(const wxString& message)
{
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

    int ret = ::StartDoc(GetHdc(), &docinfo);

#ifndef __WIN16__
    if (ret <= 0)
    {
        DWORD lastError = GetLastError();
        wxLogDebug(wxT("wxDC::StartDoc failed with error: %d\n"), lastError);
    }
#endif

    return (ret > 0);
}

void wxPrinterDC::EndDoc()
{
    if (m_hDC) ::EndDoc((HDC) m_hDC);
}

void wxPrinterDC::StartPage()
{
    if (m_hDC)
        ::StartPage((HDC) m_hDC);
}

void wxPrinterDC::EndPage()
{
    if (m_hDC)
        ::EndPage((HDC) m_hDC);
}

// Returns default device and port names
static bool wxGetDefaultDeviceName(wxString& deviceName, wxString& portName)
{
    deviceName = "";

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

        deviceName = lpszDeviceName;
        portName = lpszPortName;

        GlobalUnlock(pd.hDevNames);
        GlobalFree(pd.hDevNames);
        pd.hDevNames=NULL;
    }

    if (pd.hDevMode)
    {
        GlobalFree(pd.hDevMode);
        pd.hDevMode=NULL;
    }
    return ( deviceName != wxT("") );
}

#if 0
// This uses defaults, except for orientation, so we should eliminate this function
// and use the 2nd form (passing wxPrintData) instead.
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
#endif

// Gets an HDC for the specified printer configuration
WXHDC WXDLLEXPORT wxGetPrinterDC(const wxPrintData& printDataConst)
{
    wxPrintData printData = printDataConst;
    printData.ConvertToNative();

    wxChar* driverName = (wxChar*) NULL;

    wxString devNameStr = printData.GetPrinterName();
    wxChar* portName = (wxChar*) NULL; // Obsolete in WIN32

    const wxChar* deviceName;
    if ( !devNameStr )
        deviceName = (wxChar*) NULL;
    else
        deviceName = devNameStr.c_str();

    LPDEVMODE lpDevMode = (LPDEVMODE) NULL;

    HGLOBAL hDevMode = (HGLOBAL)(DWORD) printData.GetNativeData();

    if ( hDevMode )
        lpDevMode = (DEVMODE*) GlobalLock(hDevMode);

    if ( !devNameStr )
    {
        // Retrieve the default device name
        wxString portName;
#ifdef  __WXDEBUG__
        bool ret =
#else   // !Debug
        (void)
#endif // Debug/Release
        wxGetDefaultDeviceName(devNameStr, portName);

        wxASSERT_MSG( ret, wxT("Could not get default device name.") );

        deviceName = devNameStr.c_str();
    }

#ifdef __WIN32__
    HDC hDC = CreateDC(driverName, deviceName, portName, (DEVMODE *) lpDevMode);
#else
    HDC hDC = CreateDC(driverName, deviceName, portName, (LPSTR) lpDevMode);
#endif

    if (hDevMode && lpDevMode)
        GlobalUnlock(hDevMode);

    return (WXHDC) hDC;
}

// ----------------------------------------------------------------------------
// wxPrinterDC bit blitting/bitmap drawing
// ----------------------------------------------------------------------------

void wxPrinterDC::DoDrawBitmap(const wxBitmap &bmp,
                               wxCoord x, wxCoord y,
                               bool useMask)
{
    wxCHECK_RET( bmp.Ok(), _T("invalid bitmap in wxPrinterDC::DrawBitmap") );

    int width = bmp.GetWidth(),
        height = bmp.GetHeight();

    if ( ::GetDeviceCaps(GetHdc(), RASTERCAPS) & RC_STRETCHDIB )
    {
        BITMAPINFO *info = (BITMAPINFO *) malloc( sizeof( BITMAPINFOHEADER ) + 256 * sizeof(RGBQUAD ) );
        memset( info, 0, sizeof( BITMAPINFOHEADER ) );

        int iBitsSize = ((width + 3 ) & ~3 ) * height;

        void* bits = malloc( iBitsSize );

        info->bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
        info->bmiHeader.biWidth = width;
        info->bmiHeader.biHeight = height;
        info->bmiHeader.biPlanes = 1;
        info->bmiHeader.biBitCount = 8;
        info->bmiHeader.biCompression = BI_RGB;

        ScreenHDC display;
        if ( GetDIBits(display, GetHbitmapOf(bmp), 0,
                       bmp.GetHeight(), bits, info,
                       DIB_RGB_COLORS) )
        {
            if ( ::StretchDIBits(GetHdc(), x, y,
                                 width, height,
                                 0 , 0, width, height,
                                 bits, info,
                                 DIB_RGB_COLORS, SRCCOPY) == GDI_ERROR )
            {
                wxLogLastError("StretchDIBits");
            }
        }

        free(bits);
        free(info);
    }
    else // no support for StretchDIBits()
    {
        wxMemoryDC memDC;
        memDC.SelectObject(bmp);

        Blit(x, y, width, height, &memDC, 0, 0, wxCOPY, useMask);

        memDC.SelectObject(wxNullBitmap);
    }
}

bool wxPrinterDC::DoBlit(wxCoord xdest, wxCoord ydest,
                         wxCoord width, wxCoord height,
                         wxDC *source,
                         wxCoord xsrc, wxCoord ysrc,
                         int rop, bool useMask)
{
    bool success = TRUE;

    if ( useMask )
    {
        // If we are printing source colours are screen colours
        // not printer colours and so we need copy the bitmap
        // pixel by pixel.
        RECT rect;
        HDC dc_src = GetHdcOf(*source);
        HDC dc_mask = ::CreateCompatibleDC(dc_src);

        ::SelectObject(dc_mask, (HBITMAP) source->GetSelectedBitmap().GetMask()->GetMaskBitmap());
        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
            {
                COLORREF cref = ::GetPixel(dc_mask, x, y);
                if (cref)
                {
                    HBRUSH brush = ::CreateSolidBrush(::GetPixel(dc_src, x, y));
                    rect.left = xdest + x;
                    rect.right = rect.left + 1;
                    rect.top = ydest + y; 
                    rect.bottom = rect.top + 1;
                    ::FillRect(GetHdc(), &rect, brush);
                    ::DeleteObject(brush);
                }
            }
        }
        ::SelectObject(dc_mask, 0);
        ::DeleteDC(dc_mask);
    }
    else // no mask
    {
        if ( ::GetDeviceCaps(GetHdc(), RASTERCAPS) & RC_STRETCHDIB )
        {
            wxBitmap& bmp = source->GetSelectedBitmap();
            int width = bmp.GetWidth(),
                height = bmp.GetHeight();

            BITMAPINFO *info = (BITMAPINFO *) malloc( sizeof( BITMAPINFOHEADER ) + 256 * sizeof(RGBQUAD ) );
            int iBitsSize = ((width + 3 ) & ~3 ) * height;

            void* bits = malloc( iBitsSize );

            memset( info , 0 , sizeof( BITMAPINFOHEADER ) );

            info->bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
            info->bmiHeader.biWidth = width;
            info->bmiHeader.biHeight = height;
            info->bmiHeader.biPlanes = 1;
            info->bmiHeader.biBitCount = 8;
            info->bmiHeader.biCompression = BI_RGB;

            ScreenHDC display;
            if ( !::GetDIBits(display, GetHbitmapOf(bmp), 0,
                              height, bits, info, DIB_RGB_COLORS) )
            {
                wxLogLastError("GetDIBits");

                success = FALSE;
            }

            if ( success )
            {
                success = ::StretchDIBits(GetHdc(), xdest, ydest,
                                          width, height,
                                          xsrc, ysrc,
                                          width, height,
                                          bits, info ,
                                          DIB_RGB_COLORS,
                                          SRCCOPY) != GDI_ERROR;
                if ( !success )
                {
                    wxLogLastError("StretchDIBits");
                }
            }

            free(bits);
            free(info);
        }
        else // no support for StretchDIBits
        {
            // as we are printing, source colours are screen colours not printer
            // colours and so we need copy the bitmap pixel by pixel.
            HDC dc_src = GetHdcOf(*source);
            RECT rect;
            for (int y = 0; y < height; y++)
            {
                // This is Stefan Csomor's optimisation, where identical adjacent
                // pixels are drawn together.
                for (int x = 0; x < width; x++)
                {
                    COLORREF col = ::GetPixel(dc_src, x, y);
                    HBRUSH brush = ::CreateSolidBrush( col );

                    rect.left = xdest + x;
                    rect.top = ydest + y;
                    while( (x + 1 < width) && (::GetPixel(dc_src, x + 1, y) == col ) )
                    {
                        ++x;
                    }
                    rect.right = xdest + x + 1;
                    rect.bottom = rect.top + 1;
                    ::FillRect((HDC) m_hDC, &rect, brush);
                    ::DeleteObject(brush);
                }
            }
        }
    }

    return success;
}
