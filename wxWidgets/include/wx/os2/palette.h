/////////////////////////////////////////////////////////////////////////////
// Name:        wx/os2/palette.h
// Purpose:     wxPalette class
// Author:      David Webster
// Modified by:
// Created:     10/12/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PALETTE_H_
#define _WX_PALETTE_H_

#include "wx/gdiobj.h"
#include "wx/os2/private.h"

class WXDLLEXPORT wxPalette;

class WXDLLEXPORT wxPaletteRefData: public wxGDIRefData
{
    friend class WXDLLEXPORT wxPalette;
public:
    wxPaletteRefData();
    virtual ~wxPaletteRefData();
// protected:
    WXHPALETTE                      m_hPalette;
    HPS                             m_hPS;
}; // end of CLASS wxPaletteRefData

#define M_PALETTEDATA ((wxPaletteRefData *)m_refData)

class WXDLLEXPORT wxPalette: public wxPaletteBase
{
    DECLARE_DYNAMIC_CLASS(wxPalette)

public:
    wxPalette();

    wxPalette( int                  n
              ,const unsigned char* pRed
              ,const unsigned char* pGreen
              ,const unsigned char* pBlue
             );
    virtual ~wxPalette();

    bool Create( int                  n
                ,const unsigned char* pRed
                ,const unsigned char* pGreen
                ,const unsigned char* pBlue
               );
    int  GetPixel( unsigned char cRed
                  ,unsigned char cGreen
                  ,unsigned char cBlue
                 ) const;
    bool GetRGB( int            nPixel
                ,unsigned char* pRed
                ,unsigned char* pGreen
                ,unsigned char* pBlue
               ) const;

    virtual bool Ok() const { return IsOk(); }
    virtual bool IsOk(void) const { return (m_refData != NULL) ; }

    virtual bool FreeResource(bool bForce = false);

    inline WXHPALETTE GetHPALETTE(void) const { return (M_PALETTEDATA ? M_PALETTEDATA->m_hPalette : 0); }
    void              SetHPALETTE(WXHPALETTE hPalette);
    void              SetPS(HPS hPS);
}; // end of CLASS wxPalette

#endif
    // _WX_PALETTE_H_
