/////////////////////////////////////////////////////////////////////////////
// Name:        printdlg.cpp
// Purpose:     wxPrintDialog, wxPageSetupDialog
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "printdlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// Don't use the Windows print dialog if we're in wxUniv mode and using
// the PostScript architecture
#if wxUSE_PRINTING_ARCHITECTURE && (!defined(__WXUNIVERSAL__) || !wxUSE_POSTSCRIPT_ARCHITECTURE_IN_MSW)

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif

#include "wx/cmndata.h"
#include "wx/printdlg.h"
#include "wx/dcprint.h"
#include "wx/paper.h"

#include <stdlib.h>

#include "wx/msw/private.h"

#include <commdlg.h>

#ifndef __WIN32__
    #include <print.h>
#endif

//----------------------------------------------------------------------------
// wxWindowsPrintNativeData
//----------------------------------------------------------------------------

#ifdef __WXDEBUG__
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

IMPLEMENT_CLASS(wxWindowsPrintNativeData, wxPrintNativeDataBase)

wxWindowsPrintNativeData::wxWindowsPrintNativeData()
{
    m_devMode = (void*) NULL;
    m_devNames = (void*) NULL;
}

wxWindowsPrintNativeData::~wxWindowsPrintNativeData()
{
    HGLOBAL hDevMode = (HGLOBAL)(DWORD) m_devMode;
    if ( hDevMode )
        GlobalFree(hDevMode);
    HGLOBAL hDevNames = (HGLOBAL)(DWORD) m_devNames;
    if ( hDevNames )
        GlobalFree(hDevNames);
}

bool wxWindowsPrintNativeData::Ok() const
{
    return (m_devMode != NULL) ;
}

bool wxWindowsPrintNativeData::ConvertTo( wxPrintData &data )
{
    HGLOBAL hDevMode = (HGLOBAL)(DWORD) m_devMode;
    HGLOBAL hDevNames = (HGLOBAL)(DWORD) m_devNames;

    if (!hDevMode)
        return false;

    if ( hDevMode )
    {
        LPDEVMODE devMode = (LPDEVMODE)GlobalLock(hDevMode);

        //// Orientation
        if (devMode->dmFields & DM_ORIENTATION)
            data.SetOrientation( devMode->dmOrientation );

        //// Collation
        if (devMode->dmFields & DM_COLLATE)
        {
            if (devMode->dmCollate == DMCOLLATE_TRUE)
                data.SetCollate( true );
            else
                data.SetCollate( false );
        }

        //// Number of copies
        if (devMode->dmFields & DM_COPIES)
            data.SetNoCopies( devMode->dmCopies );

        if (devMode->dmFields & DM_DEFAULTSOURCE)
            data.SetBin( (wxPrintBin)devMode->dmDefaultSource );

        //// Printer name
        if (devMode->dmDeviceName[0] != 0)
            data.SetPrinterName( devMode->dmDeviceName );

        //// Colour
        if (devMode->dmFields & DM_COLOR)
        {
            if (devMode->dmColor == DMCOLOR_COLOR)
                data.SetColour( true );
            else
                data.SetColour( false );
        }
        else
            data.SetColour( true );

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
                    data.SetPaperId( paper->GetId() );
                    data.SetPaperSize( wxSize(paper->GetWidth() / 10,paper->GetHeight() / 10) );
                }
                else
                {
                    // Shouldn't really get here
                    wxFAIL_MSG(wxT("Couldn't find paper size in paper database."));
                    data.SetPaperId( wxPAPER_NONE );
                    data.SetPaperSize( wxSize(0,0) );
                }
            }
            else
            {
                // Shouldn't really get here
                wxFAIL_MSG(wxT("Paper database wasn't initialized in wxPrintData::ConvertFromNative."));
                data.SetPaperId( wxPAPER_NONE );
                data.SetPaperSize( wxSize(0,0) );
            }
        }
        else if ((devMode->dmFields & DM_PAPERWIDTH) && (devMode->dmFields & DM_PAPERLENGTH))
        {
            // DEVMODE is in tenths of a milimeter
            data.SetPaperId( wxPAPER_NONE );
            data.SetPaperSize( wxSize(devMode->dmPaperWidth / 10, devMode->dmPaperLength / 10) );
        }
        else
        {
            // Shouldn't really get here
            wxFAIL_MSG(wxT("Couldn't find paper size from DEVMODE."));
            data.SetPaperId( wxPAPER_NONE );
            data.SetPaperSize( wxSize(0,0) );
        }

        //// Duplex

        if (devMode->dmFields & DM_DUPLEX)
        {
            switch (devMode->dmDuplex)
            {
                case DMDUP_HORIZONTAL:   data.SetDuplex( wxDUPLEX_HORIZONTAL ); break;
                case DMDUP_VERTICAL:     data.SetDuplex( wxDUPLEX_VERTICAL ); break;
                default:    
                case DMDUP_SIMPLEX:      data.SetDuplex( wxDUPLEX_SIMPLEX ); break;
            }
        }
        else
            data.SetDuplex( wxDUPLEX_SIMPLEX );

        //// Quality

        if (devMode->dmFields & DM_PRINTQUALITY)
        {
            switch (devMode->dmPrintQuality)
            {
                case DMRES_MEDIUM:  data.SetQuality( wxPRINT_QUALITY_MEDIUM ); break;
                case DMRES_LOW:     data.SetQuality( wxPRINT_QUALITY_LOW ); break;
                case DMRES_DRAFT:   data.SetQuality( wxPRINT_QUALITY_DRAFT ); break;
                case DMRES_HIGH:    data.SetQuality( wxPRINT_QUALITY_HIGH ); break;
                default:
                {
                    // TODO: if the printer fills in the resolution in DPI, how
                    // will the application know if it's high, low, draft etc.??
                    //                    wxFAIL_MSG("Warning: DM_PRINTQUALITY was not one of the standard values.");
                    data.SetQuality( devMode->dmPrintQuality );
                    break;

                }
            }
        }
        else
            data.SetQuality( wxPRINT_QUALITY_HIGH );

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
                data.SetPrinterName( printerName );

            GlobalUnlock(hDevNames);
        }
    }
    
    return true;
}

bool wxWindowsPrintNativeData::ConvertFrom( const wxPrintData &data )
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
        devMode->dmOrientation = (short)data.GetOrientation();

        //// Collation
        devMode->dmCollate = (data.GetCollate() ? DMCOLLATE_TRUE : DMCOLLATE_FALSE);
        devMode->dmFields |= DM_COLLATE;

        //// Number of copies
        devMode->dmCopies = (short)data.GetNoCopies();
        devMode->dmFields |= DM_COPIES;

        //// Printer name
        wxString name = data.GetPrinterName();
        if (name != wxT(""))
        {
            //int len = wxMin(31, m_printerName.Len());
            wxStrncpy((wxChar*)devMode->dmDeviceName,name.c_str(),31);
            devMode->dmDeviceName[31] = wxT('\0');
        }

        //// Colour
        if (data.GetColour())
            devMode->dmColor = DMCOLOR_COLOR;
        else
            devMode->dmColor = DMCOLOR_MONOCHROME;
        devMode->dmFields |= DM_COLOR;

        //// Paper size
        if (data.GetPaperId() == wxPAPER_NONE)
        {
            // DEVMODE is in tenths of a milimeter
            devMode->dmPaperWidth = (short)(data.GetPaperSize().x * 10);
            devMode->dmPaperLength = (short)(data.GetPaperSize().y * 10);
            devMode->dmPaperSize = DMPAPER_USER;
            devMode->dmFields |= DM_PAPERWIDTH;
            devMode->dmFields |= DM_PAPERLENGTH;
        }
        else
        {
            if (wxThePrintPaperDatabase)
            {
                wxPrintPaperType* paper = wxThePrintPaperDatabase->FindPaperType( data.GetPaperId() );
                if (paper)
                {
                    devMode->dmPaperSize = (short)paper->GetPlatformId();
                    devMode->dmFields |= DM_PAPERSIZE;
                }
            }
        }

        //// Duplex
        short duplex;
        switch (data.GetDuplex())
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
        switch (data.GetQuality())
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
                quality = (short)data.GetQuality();
                break;
        }
        devMode->dmPrintQuality = quality;
        devMode->dmFields |= DM_PRINTQUALITY;

        if (data.GetBin() != wxPRINTBIN_DEFAULT)
        {
            switch (data.GetBin())
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
                    devMode->dmDefaultSource = (short)(DMBIN_USER + data.GetBin() - wxPRINTBIN_USER);
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
    m_devNames = (void*) (long) wxCreateDevNames(wxT(""), data.GetPrinterName(), wxT(""));
    
    return true;
}
    
// ---------------------------------------------------------------------------
// wxPrintDialog
// ---------------------------------------------------------------------------

IMPLEMENT_CLASS(wxWindowsPrintDialog, wxPrintDialogBase)

wxWindowsPrintDialog::wxWindowsPrintDialog(wxWindow *p, wxPrintDialogData* data)
{
    Create(p, data);
}

wxWindowsPrintDialog::wxWindowsPrintDialog(wxWindow *p, wxPrintData* data)
{
    wxPrintDialogData data2;
    if ( data )
        data2 = *data;

    Create(p, &data2);
}

bool wxWindowsPrintDialog::Create(wxWindow *p, wxPrintDialogData* data)
{
    m_dialogParent = p;
    m_printerDC = NULL;
    m_destroyDC = true;

    if ( data )
        m_printDialogData = *data;

    m_printDialogData.SetOwnerWindow(p);

    return true;
}

wxWindowsPrintDialog::~wxWindowsPrintDialog()
{
    if (m_destroyDC && m_printerDC)
        delete m_printerDC;
}

int wxWindowsPrintDialog::ShowModal()
{
    m_printDialogData.ConvertToNative();

    PRINTDLG* p = (PRINTDLG *)m_printDialogData.GetNativeData() ;
    if (m_dialogParent)
        p->hwndOwner = (HWND) m_dialogParent->GetHWND();
    else if (wxTheApp->GetTopWindow())
        p->hwndOwner = (HWND) wxTheApp->GetTopWindow()->GetHWND();
    else
        p->hwndOwner = 0;

    bool ret = (PrintDlg( p ) != 0);

    p->hwndOwner = 0;

    if ( ret != false && ((PRINTDLG *)m_printDialogData.GetNativeData())->hDC)
    {
        wxPrinterDC *pdc = new wxPrinterDC((WXHDC) ((PRINTDLG *)m_printDialogData.GetNativeData())->hDC);
        m_printerDC = pdc;
        m_printDialogData.ConvertFromNative();
        return wxID_OK;
    }
    else
    {
        return wxID_CANCEL;
    }
}

wxDC *wxWindowsPrintDialog::GetPrintDC()
{
    if (m_printerDC)
    {
        m_destroyDC = false;
        return m_printerDC;
    }
    else
        return (wxDC*) NULL;
}

// ---------------------------------------------------------------------------
// wxPageSetupDialog
// ---------------------------------------------------------------------------

IMPLEMENT_CLASS(wxPageSetupDialog, wxDialog)

wxPageSetupDialog::wxPageSetupDialog()
{
    m_dialogParent = NULL;
}

wxPageSetupDialog::wxPageSetupDialog(wxWindow *p, wxPageSetupData *data)
{
    Create(p, data);
}

bool wxPageSetupDialog::Create(wxWindow *p, wxPageSetupData *data)
{
    m_dialogParent = p;

    if (data)
        m_pageSetupData = (*data);

#if defined(__WIN95__)
    m_pageSetupData.SetOwnerWindow(p);
#endif
    return true;
}

wxPageSetupDialog::~wxPageSetupDialog()
{
}

int wxPageSetupDialog::ShowModal()
{
#ifdef __WIN95__
    m_pageSetupData.ConvertToNative();
    PAGESETUPDLG *p = (PAGESETUPDLG *)m_pageSetupData.GetNativeData();
    if (m_dialogParent)
        p->hwndOwner = (HWND) m_dialogParent->GetHWND();
    else if (wxTheApp->GetTopWindow())
        p->hwndOwner = (HWND) wxTheApp->GetTopWindow()->GetHWND();
    else
        p->hwndOwner = 0;
    BOOL retVal = PageSetupDlg( p ) ;
    p->hwndOwner = 0;
    if (retVal)
    {
        m_pageSetupData.ConvertFromNative();
        return wxID_OK;
    }
    else
        return wxID_CANCEL;
#else
    wxGenericPageSetupDialog *genericPageSetupDialog = new wxGenericPageSetupDialog(GetParent(), & m_pageSetupData);
    int ret = genericPageSetupDialog->ShowModal();
    m_pageSetupData = genericPageSetupDialog->GetPageSetupData();
    genericPageSetupDialog->Close(true);
    return ret;
#endif
}

#endif
    // wxUSE_PRINTING_ARCHITECTURE
