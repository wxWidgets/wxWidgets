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

#if wxUSE_FONTDLG
    #include "wx/fontdlg.h"
#endif // wxUSE_FONTDLG

#if wxUSE_PRINTING_ARCHITECTURE
    #include "wx/paper.h"
#endif // wxUSE_PRINTING_ARCHITECTURE

#ifdef __WXMSW__
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
#ifdef __WXMSW__
    m_devMode = (void*) NULL;
    m_devNames = (void*) NULL;
#elif defined( __WXMAC__ )
    m_nativePrintData = wxNativePrintData::Create() ;
#endif
    m_bin = wxPRINTBIN_DEFAULT;
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

    // PostScript-specific data
    m_previewCommand = wxT("");
    m_filename = wxT("");
#ifdef __VMS__
    m_printerCommand = wxT("print");
    m_printerOptions = wxT("/nonotify/queue=psqueue");
    m_afmPath = wxT("sys$ps_font_metrics:");
#endif

#ifdef __WXMSW__
    m_printerCommand = wxT("print");
    m_printerOptions = wxT("");
    m_afmPath = wxT("c:\\windows\\system\\");
#endif

#if !defined(__VMS__) && !defined(__WXMSW__)
    m_printerCommand = wxT("lpr");
    m_printerOptions = wxT("");
    m_afmPath = wxT("");
#endif

    m_printerScaleX = 1.0;
    m_printerScaleY = 1.0;
    m_printerTranslateX = 0;
    m_printerTranslateY = 0;
    m_printMode = wxPRINT_MODE_FILE;

#ifdef wxUSE_STREAMS
    m_outputstream = NULL;
#endif
}

wxPrintData::wxPrintData(const wxPrintData& printData)
    : wxObject()
{
#ifdef __WXMSW__
    m_devMode = (void*) NULL;
    m_devNames = (void*) NULL;
#elif defined( __WXMAC__ )
    m_nativePrintData = wxNativePrintData::Create() ;
#endif
    (*this) = printData;
}

wxPrintData::~wxPrintData()
{
#ifdef __WXMSW__
    HGLOBAL hDevMode = (HGLOBAL)(DWORD) m_devMode;
    if ( hDevMode )
        GlobalFree(hDevMode);
    HGLOBAL hDevNames = (HGLOBAL)(DWORD) m_devNames;
    if ( hDevNames )
        GlobalFree(hDevNames);
#elif defined(__WXMAC__)
    delete m_nativePrintData ;
#endif
}

#if defined(__WXMSW__) // && defined(__WIN32__)

#if defined(__WXDEBUG__) && defined(__WIN32__)
static wxString wxGetPrintDlgError()
{
    DWORD err = CommDlgExtendedError();
    wxString msg = wxT("Unknown");
    switch (err)
    {
        case CDERR_FINDRESFAILURE: msg = wxT("CDERR_FINDRESFAILURE"); break;
        case CDERR_INITIALIZATION: msg = wxT("CDERR_INITIALIZATION"); break;
        case CDERR_LOADRESFAILURE: msg = wxT("CDERR_LOADRESFAILURE"); break;
        case CDERR_LOADSTRFAILURE: msg = wxT("CDERR_LOADSTRFAILURE"); break;
        case CDERR_LOCKRESFAILURE: msg = wxT("CDERR_LOCKRESFAILURE"); break;
        case CDERR_MEMALLOCFAILURE: msg = wxT("CDERR_MEMALLOCFAILURE"); break;
        case CDERR_MEMLOCKFAILURE: msg = wxT("CDERR_MEMLOCKFAILURE"); break;
        case CDERR_NOHINSTANCE: msg = wxT("CDERR_NOHINSTANCE"); break;
        case CDERR_NOHOOK: msg = wxT("CDERR_NOHOOK"); break;
        case CDERR_NOTEMPLATE: msg = wxT("CDERR_NOTEMPLATE"); break;
        case CDERR_STRUCTSIZE: msg = wxT("CDERR_STRUCTSIZE"); break;
        case  PDERR_RETDEFFAILURE: msg = wxT("PDERR_RETDEFFAILURE"); break;
        case  PDERR_PRINTERNOTFOUND: msg = wxT("PDERR_PRINTERNOTFOUND"); break;
        case  PDERR_PARSEFAILURE: msg = wxT("PDERR_PARSEFAILURE"); break;
        case  PDERR_NODEVICES: msg = wxT("PDERR_NODEVICES"); break;
        case  PDERR_NODEFAULTPRN: msg = wxT("PDERR_NODEFAULTPRN"); break;
        case  PDERR_LOADDRVFAILURE: msg = wxT("PDERR_LOADDRVFAILURE"); break;
        case  PDERR_INITFAILURE: msg = wxT("PDERR_INITFAILURE"); break;
        case  PDERR_GETDEVMODEFAIL: msg = wxT("PDERR_GETDEVMODEFAIL"); break;
        case  PDERR_DNDMMISMATCH: msg = wxT("PDERR_DNDMMISMATCH"); break;
        case  PDERR_DEFAULTDIFFERENT: msg = wxT("PDERR_DEFAULTDIFFERENT"); break;
        case  PDERR_CREATEICFAILURE: msg = wxT("PDERR_CREATEICFAILURE"); break;
        default: break;
    }
    return msg;
}
#endif

static HGLOBAL wxCreateDevNames(const wxString& driverName, const wxString& printerName, const wxString& portName)
{
    HGLOBAL hDev = NULL;
    // if (!driverName.IsEmpty() && !printerName.IsEmpty() && !portName.IsEmpty())
    if (driverName.IsEmpty() && printerName.IsEmpty() && portName.IsEmpty())
    {
    }
    else
    {
        hDev = GlobalAlloc(GPTR, 4*sizeof(WORD)+
                           ( driverName.Length() + 1 +
            printerName.Length() + 1 +
                             portName.Length()+1 ) * sizeof(wxChar) );
        LPDEVNAMES lpDev = (LPDEVNAMES)GlobalLock(hDev);
        lpDev->wDriverOffset = sizeof(WORD) * 4 / sizeof(wxChar);
        wxStrcpy((wxChar*)lpDev + lpDev->wDriverOffset, driverName);

        lpDev->wDeviceOffset = (WORD)( lpDev->wDriverOffset +
                                       driverName.Length() + 1 );
        wxStrcpy((wxChar*)lpDev + lpDev->wDeviceOffset, printerName);

        lpDev->wOutputOffset = (WORD)( lpDev->wDeviceOffset +
                                       printerName.Length() + 1 );
        wxStrcpy((wxChar*)lpDev + lpDev->wOutputOffset, portName);

        lpDev->wDefault = 0;

        GlobalUnlock(hDev);
    }

    return hDev;
}

void wxPrintData::ConvertToNative()
{
    HGLOBAL hDevMode = (HGLOBAL)(DWORD) m_devMode;
    HGLOBAL hDevNames = (HGLOBAL)(DWORD) m_devNames;
    if (!hDevMode)
    {
        // Use PRINTDLG as a way of creating a DEVMODE object
        PRINTDLG pd;

        // GNU-WIN32 has the wrong size PRINTDLG - can't work out why.
#ifdef __GNUWIN32__
        memset(&pd, 0, 66);
        pd.lStructSize    = 66;
#else
        memset(&pd, 0, sizeof(PRINTDLG));
#ifdef __WXWINCE__
        pd.cbStruct    = sizeof(PRINTDLG);
#else
        pd.lStructSize    = sizeof(PRINTDLG);
#endif
#endif

        pd.hwndOwner      = (HWND)NULL;
        pd.hDevMode       = NULL; // Will be created by PrintDlg
        pd.hDevNames      = NULL; // Ditto
        //pd.hInstance      = (HINSTANCE) wxGetInstance();

        pd.Flags          = PD_RETURNDEFAULT;
        pd.nCopies        = 1;

        // Fill out the DEVMODE structure
        // so we can use it as input in the 'real' PrintDlg
        if (!PrintDlg(&pd))
        {
            if ( pd.hDevMode )
                GlobalFree(pd.hDevMode);
            if ( pd.hDevNames )
                GlobalFree(pd.hDevNames);
            pd.hDevMode = NULL;
            pd.hDevNames = NULL;

#if defined(__WXDEBUG__) && defined(__WIN32__)
            wxString str(wxT("Printing error: "));
            str += wxGetPrintDlgError();
            wxLogDebug(str);
#endif
        }
        else
        {
            hDevMode = pd.hDevMode;
            m_devMode = (void*)(long) hDevMode;
            pd.hDevMode = NULL;

            // We'll create a new DEVNAMEs structure below.
            if ( pd.hDevNames )
                GlobalFree(pd.hDevNames);
            pd.hDevNames = NULL;

            // hDevNames = pd->hDevNames;
            // m_devNames = (void*)(long) hDevNames;
            // pd->hDevnames = NULL;

        }
    }

    if ( hDevMode )
    {
        LPDEVMODE devMode = (LPDEVMODE) GlobalLock(hDevMode);

        //// Orientation

        devMode->dmOrientation = (short)m_printOrientation;

        //// Collation

        devMode->dmCollate = (m_printCollate ? DMCOLLATE_TRUE : DMCOLLATE_FALSE);
        devMode->dmFields |= DM_COLLATE;

        //// Number of copies

        devMode->dmCopies = (short)m_printNoCopies;
        devMode->dmFields |= DM_COPIES;

        //// Printer name

        if (m_printerName != wxT(""))
        {
            //int len = wxMin(31, m_printerName.Len());
            wxStrncpy((wxChar*)devMode->dmDeviceName,m_printerName.c_str(),31);
            devMode->dmDeviceName[31] = wxT('\0');
        }

        //// Colour

        if (m_colour)
            devMode->dmColor = DMCOLOR_COLOR;
        else
            devMode->dmColor = DMCOLOR_MONOCHROME;

        devMode->dmFields |= DM_COLOR;

        //// Paper size

        if (m_paperId == wxPAPER_NONE)
        {
            // DEVMODE is in tenths of a milimeter
            devMode->dmPaperWidth = (short)(m_paperSize.x * 10);
            devMode->dmPaperLength = (short)(m_paperSize.y * 10);
            devMode->dmPaperSize = DMPAPER_USER;
            devMode->dmFields |= DM_PAPERWIDTH;
            devMode->dmFields |= DM_PAPERLENGTH;
        }
        else
        {
            if (wxThePrintPaperDatabase)
            {
                wxPrintPaperType* paper = wxThePrintPaperDatabase->FindPaperType(m_paperId);
                if (paper)
                {
                    devMode->dmPaperSize = (short)paper->GetPlatformId();
                    devMode->dmFields |= DM_PAPERSIZE;
                }
            }
        }

        //// Duplex

        short duplex;
        switch (m_duplexMode)
        {
            case wxDUPLEX_HORIZONTAL:
                duplex = DMDUP_HORIZONTAL;
                break;
            case wxDUPLEX_VERTICAL:
                duplex = DMDUP_VERTICAL;
                break;
            default:
            // in fact case wxDUPLEX_SIMPLEX:
                duplex = DMDUP_SIMPLEX;
                break;
        }
        devMode->dmDuplex = duplex;
        devMode->dmFields |= DM_DUPLEX;

        //// Quality

        short quality;
        switch (m_printQuality)
        {
            case wxPRINT_QUALITY_MEDIUM:
                quality = DMRES_MEDIUM;
                break;
            case wxPRINT_QUALITY_LOW:
                quality = DMRES_LOW;
                break;
            case wxPRINT_QUALITY_DRAFT:
                quality = DMRES_DRAFT;
                break;
            case wxPRINT_QUALITY_HIGH:
                quality = DMRES_HIGH;
                break;
            default:
                quality = (short)m_printQuality;
                break;
        }
        devMode->dmPrintQuality = quality;
        devMode->dmFields |= DM_PRINTQUALITY;

        if ( m_bin != wxPRINTBIN_DEFAULT )
        {
            switch ( m_bin )
            {
                case wxPRINTBIN_ONLYONE:        devMode->dmDefaultSource = DMBIN_ONLYONE;       break;
                case wxPRINTBIN_LOWER:          devMode->dmDefaultSource = DMBIN_LOWER;         break;
                case wxPRINTBIN_MIDDLE:         devMode->dmDefaultSource = DMBIN_MIDDLE;        break;
                case wxPRINTBIN_MANUAL:         devMode->dmDefaultSource = DMBIN_MANUAL;        break;
                case wxPRINTBIN_ENVELOPE:       devMode->dmDefaultSource = DMBIN_ENVELOPE;      break;
                case wxPRINTBIN_ENVMANUAL:      devMode->dmDefaultSource = DMBIN_ENVMANUAL;     break;
                case wxPRINTBIN_AUTO:           devMode->dmDefaultSource = DMBIN_AUTO;          break;
                case wxPRINTBIN_TRACTOR:        devMode->dmDefaultSource = DMBIN_TRACTOR;       break;
                case wxPRINTBIN_SMALLFMT:       devMode->dmDefaultSource = DMBIN_SMALLFMT;      break;
                case wxPRINTBIN_LARGEFMT:       devMode->dmDefaultSource = DMBIN_LARGEFMT;      break;
                case wxPRINTBIN_LARGECAPACITY:  devMode->dmDefaultSource = DMBIN_LARGECAPACITY; break;
                case wxPRINTBIN_CASSETTE:       devMode->dmDefaultSource = DMBIN_CASSETTE;      break;
                case wxPRINTBIN_FORMSOURCE:     devMode->dmDefaultSource = DMBIN_FORMSOURCE;    break;

                default:
                    devMode->dmDefaultSource = (short)(DMBIN_USER + m_bin - wxPRINTBIN_USER);
                    break;
            }

            devMode->dmFields |= DM_DEFAULTSOURCE;
        }

        GlobalUnlock(hDevMode);
    }

    if ( hDevNames )
    {
        GlobalFree(hDevNames);
    }

    // TODO: I hope it's OK to pass some empty strings to DEVNAMES.
    m_devNames = (void*) (long) wxCreateDevNames(wxT(""), m_printerName, wxT(""));
}

void wxPrintData::ConvertFromNative()
{
    HGLOBAL hDevMode = (HGLOBAL)(DWORD) m_devMode;
    HGLOBAL hDevNames = (HGLOBAL)(DWORD) m_devNames;

    if (!hDevMode)
        return;

    if ( hDevMode )
    {
        LPDEVMODE devMode = (LPDEVMODE)GlobalLock(hDevMode);

        //// Orientation

        if (devMode->dmFields & DM_ORIENTATION)
            m_printOrientation = devMode->dmOrientation;

        //// Collation

        if (devMode->dmFields & DM_COLLATE)
        {
            if (devMode->dmCollate == DMCOLLATE_TRUE)
                m_printCollate = true;
            else
                m_printCollate = false;
        }

        //// Number of copies

        if (devMode->dmFields & DM_COPIES)
        {
            m_printNoCopies = devMode->dmCopies;
        }

        if (devMode->dmFields & DM_DEFAULTSOURCE)
        {
            m_bin = (wxPrintBin)devMode->dmDefaultSource;
        }

        //// Printer name

        if (devMode->dmDeviceName[0] != 0)
        {
            m_printerName = devMode->dmDeviceName;
        }

        //// Colour

        if (devMode->dmFields & DM_COLOR)
        {
            if (devMode->dmColor == DMCOLOR_COLOR)
                m_colour = true;
            else
                m_colour = false;
        }
        else
            m_colour = true;

        //// Paper size

        // We don't know size of user defined paper and some buggy drivers
        // set both DM_PAPERSIZE and DM_PAPERWIDTH & DM_PAPERLENGTH. Since
        // dmPaperSize >= DMPAPER_USER wouldn't be in wxWin's database, this
        // code wouldn't set m_paperSize correctly.
        if ((devMode->dmFields & DM_PAPERSIZE) && (devMode->dmPaperSize < DMPAPER_USER))
        {
            if (wxThePrintPaperDatabase)
            {
                wxPrintPaperType* paper = wxThePrintPaperDatabase->FindPaperTypeByPlatformId(devMode->dmPaperSize);
                if (paper)
                {
                    m_paperId = paper->GetId();
                    m_paperSize.x = paper->GetWidth() / 10;
                    m_paperSize.y = paper->GetHeight() / 10;
                }
                else
                {
                    // Shouldn't really get here
                    wxFAIL_MSG(wxT("Couldn't find paper size in paper database."));

                    m_paperId = wxPAPER_NONE;
                    m_paperSize.x = 0;
                    m_paperSize.y = 0;
                }
            }
            else
            {
                // Shouldn't really get here
                wxFAIL_MSG(wxT("Paper database wasn't initialized in wxPrintData::ConvertFromNative."));

                m_paperId = wxPAPER_NONE;
                m_paperSize.x = 0;
                m_paperSize.y = 0;
            }
        }
        else if ((devMode->dmFields & DM_PAPERWIDTH) && (devMode->dmFields & DM_PAPERLENGTH))
        {
            // DEVMODE is in tenths of a milimeter
            m_paperSize.x = devMode->dmPaperWidth / 10;
            m_paperSize.y = devMode->dmPaperLength / 10;
            m_paperId = wxPAPER_NONE;
        }
        else
        {
            // Shouldn't really get here
            wxFAIL_MSG(wxT("Couldn't find paper size from DEVMODE."));

            m_paperSize.x = 0;
            m_paperSize.y = 0;
            m_paperId = wxPAPER_NONE;
        }

        //// Duplex

        if (devMode->dmFields & DM_DUPLEX)
        {
            switch (devMode->dmDuplex)
            {
            case DMDUP_HORIZONTAL: {
                m_duplexMode = wxDUPLEX_HORIZONTAL; break;
                                   }
            case DMDUP_VERTICAL: {
                m_duplexMode = wxDUPLEX_VERTICAL; break;
                                 }
            default:
            case DMDUP_SIMPLEX: {
                m_duplexMode = wxDUPLEX_SIMPLEX; break;
                                }
            }
        }
        else
            m_duplexMode = wxDUPLEX_SIMPLEX;

        //// Quality

        if (devMode->dmFields & DM_PRINTQUALITY)
        {
            switch (devMode->dmPrintQuality)
            {
            case DMRES_MEDIUM: {
                m_printQuality = wxPRINT_QUALITY_MEDIUM; break;
                               }
            case DMRES_LOW: {
                m_printQuality = wxPRINT_QUALITY_LOW; break;
                            }
            case DMRES_DRAFT: {
                m_printQuality = wxPRINT_QUALITY_DRAFT; break;
                              }
            case DMRES_HIGH: {
                m_printQuality = wxPRINT_QUALITY_HIGH; break;
                             }
            default:
                {
                    // TODO: if the printer fills in the resolution in DPI, how
                    // will the application know if it's high, low, draft etc.??
                    //                    wxFAIL_MSG("Warning: DM_PRINTQUALITY was not one of the standard values.");
                    m_printQuality = devMode->dmPrintQuality; break;

                }
            }
        }
        else
            m_printQuality = wxPRINT_QUALITY_HIGH;

        GlobalUnlock(hDevMode);
    }

    if (hDevNames)
    {
        LPDEVNAMES lpDevNames = (LPDEVNAMES)GlobalLock(hDevNames);
        if (lpDevNames)
        {
            // TODO: Unicode-ification

            // Get the port name
            // port is obsolete in WIN32
            // m_printData.SetPortName((LPSTR)lpDevNames + lpDevNames->wDriverOffset);

            // Get the printer name
            wxString printerName = (LPTSTR)lpDevNames + lpDevNames->wDeviceOffset;

            // Not sure if we should check for this mismatch
//            wxASSERT_MSG( (m_printerName == "" || (devName == m_printerName)), "Printer name obtained from DEVMODE and DEVNAMES were different!");

            if (printerName != wxT(""))
                m_printerName = printerName;

            GlobalUnlock(hDevNames);
        }
    }
}

#endif

#ifdef __WXMAC__

void wxPrintData::ConvertToNative()
{
    m_nativePrintData->TransferFrom( this ) ;
}

void wxPrintData::ConvertFromNative()
{
    m_nativePrintData->TransferTo( this ) ;
}

#endif

void wxPrintData::operator=(const wxPrintData& data)
{
#ifdef __WXMAC__
    m_nativePrintData->CopyFrom( data.m_nativePrintData ) ;
#endif
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
#ifdef wxUSE_STREAMS
    m_outputstream = data.m_outputstream;
#endif

    // PostScript-specific data
    m_printerCommand = data.m_printerCommand;
    m_previewCommand = data.m_previewCommand;
    m_printerOptions = data.m_printerOptions;
    m_filename = data.m_filename;
    m_afmPath = data.m_afmPath;
    m_printerScaleX = data.m_printerScaleX;
    m_printerScaleY = data.m_printerScaleY;
    m_printerTranslateX = data.m_printerTranslateX;
    m_printerTranslateY = data.m_printerTranslateY;
    m_printMode = data.m_printMode;
}

// Is this data OK for showing the print dialog?
bool wxPrintData::Ok() const
{
#ifdef __WXMSW__
    ((wxPrintData*)this)->ConvertToNative();
    return (m_devMode != NULL) ;
#else
    return true;
#endif
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
    m_printEnablePrintToFile = true;
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
    m_printData.ConvertToNative();

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

    pd->hDevMode = (HGLOBAL)(DWORD) m_printData.GetNativeData();

    m_printData.SetNativeData((void*) NULL);

    // Shouldn't assert; we should be able to test Ok-ness at a higher level
    //wxASSERT_MSG( (pd->hDevMode), wxT("hDevMode must be non-NULL in ConvertToNative!"));

    pd->hDevNames = (HGLOBAL)(DWORD) m_printData.GetNativeDataDevNames();

    m_printData.SetNativeDataDevNames((void*) NULL);

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

    // Pass the devmode data back to the wxPrintData structure where it really belongs.
    if (pd->hDevMode)
    {
        if (m_printData.GetNativeData())
        {
            // Make sure we don't leak memory
            GlobalFree((HGLOBAL)(DWORD) m_printData.GetNativeData());
        }
        m_printData.SetNativeData((void*)(long) pd->hDevMode);
        pd->hDevMode = NULL;
    }

    // Pass the devnames data back to the wxPrintData structure where it really belongs.
    if (pd->hDevNames)
    {
        if (m_printData.GetNativeDataDevNames())
        {
            // Make sure we don't leak memory
            GlobalFree((HGLOBAL)(DWORD) m_printData.GetNativeDataDevNames());
        }
        m_printData.SetNativeDataDevNames((void*)(long) pd->hDevNames);
        pd->hDevNames = NULL;
    }

    // Now convert the DEVMODE object, passed down from the PRINTDLG object,
    // into wxWidgets form.
    m_printData.ConvertFromNative();

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
    m_printData.ConvertToNative();

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

    pd->hDevMode = (HGLOBAL) m_printData.GetNativeData();

    m_printData.SetNativeData((void*) NULL);

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

    pd->hDevNames = (HGLOBAL) m_printData.GetNativeDataDevNames();

    m_printData.SetNativeDataDevNames((void*) NULL);

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

    // Pass the devmode data back to the wxPrintData structure where it really belongs.
    if (pd->hDevMode)
    {
        if (m_printData.GetNativeData())
        {
            // Make sure we don't leak memory
            GlobalFree((HGLOBAL) m_printData.GetNativeData());
        }
        m_printData.SetNativeData((void*) pd->hDevMode);
        pd->hDevMode = NULL;
    }

    m_printData.ConvertFromNative();

    // Pass the devnames data back to the wxPrintData structure where it really belongs.
    if (pd->hDevNames)
    {
        if (m_printData.GetNativeDataDevNames())
        {
            // Make sure we don't leak memory
            GlobalFree((HGLOBAL) m_printData.GetNativeDataDevNames());
        }
        m_printData.SetNativeDataDevNames((void*) pd->hDevNames);
        pd->hDevNames = NULL;
    }

    m_printData.ConvertFromNative();

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
