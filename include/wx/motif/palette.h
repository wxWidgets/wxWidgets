/////////////////////////////////////////////////////////////////////////////
// Name:        palette.h
// Purpose:     wxPalette class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PALETTE_H_
#define _WX_PALETTE_H_

#ifdef __GNUG__
#pragma interface "palette.h"
#endif

#include "wx/gdiobj.h"
#include "wx/list.h"

class WXDLLEXPORT wxPalette;

// Palette for one display
class wxXPalette : public wxObject
{
DECLARE_DYNAMIC_CLASS(wxXPalette)

public:
    wxXPalette();

    WXDisplay*        m_display;
    int               m_pix_array_n;
    unsigned long*    m_pix_array;
    WXColormap        m_cmap;
    bool              m_destroyable;
};

class WXDLLEXPORT wxPaletteRefData: public wxGDIRefData
{
    friend class WXDLLEXPORT wxPalette;
public:
    wxPaletteRefData();
    ~wxPaletteRefData();

protected:
    wxList  m_palettes;
};

#define M_PALETTEDATA ((wxPaletteRefData *)m_refData)

class WXDLLEXPORT wxPalette: public wxGDIObject
{
  DECLARE_DYNAMIC_CLASS(wxPalette)

public:
  wxPalette();
  wxPalette(const wxPalette& palette) { Ref(palette); }

  wxPalette(int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue);
  ~wxPalette();
  bool Create(int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue);
  int GetPixel(const unsigned char red, const unsigned char green, const unsigned char blue) const;
  bool GetRGB(int pixel, unsigned char *red, unsigned char *green, unsigned char *blue) const;

  virtual bool Ok() const { return (m_refData != NULL) ; }

  wxPalette& operator = (const wxPalette& palette) { if (*this == palette) return (*this); Ref(palette); return *this; }
  bool operator == (const wxPalette& palette) { return m_refData == palette.m_refData; }
  bool operator != (const wxPalette& palette) { return m_refData != palette.m_refData; }

  // Motif-specific
  WXColormap GetXColormap(WXDisplay* display = NULL) const;
  bool TransferBitmap(void *data, int depth, int size);
  bool TransferBitmap8(unsigned char *data, unsigned long size, void *dest, unsigned int bpp);
  unsigned long *GetXPixArray(WXDisplay* display, int *pix_array_n);
  void PutXColormap(WXDisplay* display, WXColormap cmap, bool destroyable);
};

#endif
    // _WX_PALETTE_H_
