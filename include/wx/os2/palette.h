/////////////////////////////////////////////////////////////////////////////
// Name:        palette.h
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

class WXDLLEXPORT wxPalette;

class WXDLLEXPORT wxPaletteRefData: public wxGDIRefData
{
    friend class WXDLLEXPORT wxPalette;
public:
    wxPaletteRefData();
    ~wxPaletteRefData();
protected:
    WXHPALETTE                      m_hPalette;
    HPS                             m_hPS;
}; // end of CLASS wxPaletteRefData

#define M_PALETTEDATA ((wxPaletteRefData *)m_refData)

class WXDLLEXPORT wxPalette: public wxGDIObject
{
    DECLARE_DYNAMIC_CLASS(wxPalette)

public:
    wxPalette();
    inline wxPalette(const wxPalette& rPalette) { Ref(rPalette); }

    wxPalette( int                  n
              ,const unsigned char* pRed
              ,const unsigned char* pGreen
              ,const unsigned char* pBlue
             );
    ~wxPalette();

    bool Create( int                  n
                ,const unsigned char* pRed
                ,const unsigned char* pGreen
                ,const unsigned char* pBlue
               );
    int  GetPixel( const unsigned char cRed
                  ,const unsigned char cGreen
                  ,const unsigned char cBlue
                 ) const;
    bool GetRGB( int            nPixel
                ,unsigned char* pRed
                ,unsigned char* pGreen
                ,unsigned char* pBlue
               ) const;

    virtual bool Ok(void) const { return (m_refData != NULL) ; }

    inline wxPalette& operator = (const wxPalette& rPalette) { if (*this == rPalette) return (*this); Ref(rPalette); return *this; }
    inline bool       operator == (const wxPalette& rPalette) { return m_refData == rPalette.m_refData; }
    inline bool       operator != (const wxPalette& rPalette) { return m_refData != rPalette.m_refData; }

    virtual bool FreeResource(bool bForce = FALSE);

    inline WXHPALETTE GetHPALETTE(void) const { return (M_PALETTEDATA ? M_PALETTEDATA->m_hPalette : 0); }
    void              SetHPALETTE(WXHPALETTE hPalette);
    void              SetPS(HPS hPS);
}; // end of CLASS wxPalette

#define wxColorMap wxPalette
#define wxColourMap wxPalette

#endif
    // _WX_PALETTE_H_

