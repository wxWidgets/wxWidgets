/////////////////////////////////////////////////////////////////////////////
// Name:        wx/os2/dcprint.h
// Purpose:     wxPrinterDC class
// Author:      David Webster
// Modified by:
// Created:     09/12/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCPRINT_H_
#define _WX_DCPRINT_H_

#if wxUSE_PRINTING_ARCHITECTURE

#include "wx/dc.h"
#include "wx/cmndata.h"

class WXDLLEXPORT wxPrinterDC: public wxDC
{
 public:
    // Create a printer DC [obsolete]
    wxPrinterDC( const wxString& rsDriver
                ,const wxString& rsDevice
                ,const wxString& rsOutput
                ,bool            bInteractive = TRUE
                ,int             nOrientation = wxPORTRAIT
               );

    // Create from print data
    wxPrinterDC(const wxPrintData& rData);
    wxPrinterDC(WXHDC hTheDC);

    bool StartDoc(const wxString& rsMessage);
    void EndDoc(void);
    void StartPage(void);
    void EndPage(void);

    wxRect GetPaperRect();

protected:
    virtual void DoDrawBitmap( const wxBitmap& rBmp
                              ,wxCoord         vX
                              ,wxCoord         vY
                              ,bool            bUseMask = FALSE
                             );
    virtual bool DoBlit( wxCoord vXdest
                        ,wxCoord vYdest
                        ,wxCoord vWidth
                        ,wxCoord vHeight
                        ,wxDC*   pSource
                        ,wxCoord vXsrc
                        ,wxCoord vYsrc
                        ,int     nRop = wxCOPY
                        ,bool    bUseMask = FALSE
                        ,wxCoord vXsrcMask = -1
                        ,wxCoord vYsrcMask = -1
                       );

    // init the dc
    void Init(void);

    wxPrintData                     m_printData;
private:
    DECLARE_CLASS(wxPrinterDC)
}; // end of CLASS wxPrinterDC

// Gets an HDC for the specified printer configuration
WXHDC WXDLLEXPORT wxGetPrinterDC(const wxPrintData& rData);

#endif // wxUSE_PRINTING_ARCHITECTURE

#endif
    // _WX_DCPRINT_H_

