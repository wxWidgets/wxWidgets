/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/dcprint.cpp
// Purpose:     wxPrinterDC class
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id: 
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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

#if wxUSE_PRINTING_ARCHITECTURE

#include "wx/palmos/private.h"

#if wxUSE_WXDIB
#include "wx/palmos/dib.h"
#endif

#include "wx/dcprint.h"
#include "math.h"

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
wxPrinterDC::wxPrinterDC(const wxString& driver_name,
                         const wxString& device_name,
                         const wxString& file,
                         bool interactive,
                         int orientation)
{
}

wxPrinterDC::wxPrinterDC(const wxPrintData& printData)
{
}


wxPrinterDC::wxPrinterDC(WXHDC dc)
{
}

void wxPrinterDC::Init()
{
}

// ----------------------------------------------------------------------------
// wxPrinterDC {Start/End}{Page/Doc} methods
// ----------------------------------------------------------------------------

bool wxPrinterDC::StartDoc(const wxString& message)
{
    return false;
}

void wxPrinterDC::EndDoc()
{
}

void wxPrinterDC::StartPage()
{
}

void wxPrinterDC::EndPage()
{
}

// Returns default device and port names
static bool wxGetDefaultDeviceName(wxString& deviceName, wxString& portName)
{
    return false;
}

// Gets an HDC for the specified printer configuration
WXHDC WXDLLEXPORT wxGetPrinterDC(const wxPrintData& printDataConst)
{
    return (WXHDC) 0;
}

// ----------------------------------------------------------------------------
// wxPrinterDC bit blitting/bitmap drawing
// ----------------------------------------------------------------------------

// helper of DoDrawBitmap() and DoBlit()
static
bool DrawBitmapUsingStretchDIBits(HDC hdc,
                                  const wxBitmap& bmp,
                                  wxCoord x, wxCoord y)
{
    return false;
}

void wxPrinterDC::DoDrawBitmap(const wxBitmap& bmp,
                               wxCoord x, wxCoord y,
                               bool useMask)
{
}

bool wxPrinterDC::DoBlit(wxCoord xdest, wxCoord ydest,
                         wxCoord width, wxCoord height,
                         wxDC *source,
                         wxCoord WXUNUSED(xsrc), wxCoord WXUNUSED(ysrc),
                         int WXUNUSED(rop), bool useMask,
                         wxCoord WXUNUSED(xsrcMask), wxCoord WXUNUSED(ysrcMask))
{
    return false;
}

#endif
    // wxUSE_PRINTING_ARCHITECTURE
