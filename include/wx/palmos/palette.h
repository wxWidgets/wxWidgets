/////////////////////////////////////////////////////////////////////////////
// Name:        palette.h
// Purpose:     wxPalette class
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id:
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PALETTE_H_
#define _WX_PALETTE_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "palette.h"
#endif

#include "wx/gdiobj.h"

class WXDLLEXPORT wxPalette;

class WXDLLEXPORT wxPaletteRefData: public wxGDIRefData
{
    friend class WXDLLEXPORT wxPalette;
public:
    wxPaletteRefData(void);
    ~wxPaletteRefData(void);
protected:
 WXHPALETTE m_hPalette;
};

#define M_PALETTEDATA ((wxPaletteRefData *)m_refData)

class WXDLLEXPORT wxPalette: public wxPaletteBase
{
  DECLARE_DYNAMIC_CLASS(wxPalette)

public:
  wxPalette(void);
  inline wxPalette(const wxPalette& palette) { Ref(palette); }

  wxPalette(int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue);
  ~wxPalette(void);
  bool Create(int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue);
  int GetPixel(const unsigned char red, const unsigned char green, const unsigned char blue) const;
  bool GetRGB(int pixel, unsigned char *red, unsigned char *green, unsigned char *blue) const;

  virtual bool Ok(void) const { return (m_refData != NULL) ; }

  inline wxPalette& operator = (const wxPalette& palette) { if (*this == palette) return (*this); Ref(palette); return *this; }
  inline bool operator == (const wxPalette& palette) const { return m_refData == palette.m_refData; }
  inline bool operator != (const wxPalette& palette) const { return m_refData != palette.m_refData; }

  virtual bool FreeResource(bool force = false);

  inline WXHPALETTE GetHPALETTE(void) const { return (M_PALETTEDATA ? M_PALETTEDATA->m_hPalette : 0); }
  void SetHPALETTE(WXHPALETTE pal);
};

#endif
    // _WX_PALETTE_H_
