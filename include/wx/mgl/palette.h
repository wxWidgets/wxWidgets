/////////////////////////////////////////////////////////////////////////////
// Name:        palette.h
// Purpose:
// Author:      Vaclav Slavik
// Created:     2001/03/11
// Id:          $Id$
// Copyright:   (c) 2001 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __WX_PALETTE_H__
#define __WX_PALETTE_H__

#ifdef __GNUG__
#pragma interface "palette.h"
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/gdiobj.h"
#include "wx/gdicmn.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxPalette;

//-----------------------------------------------------------------------------
// wxPalette
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxPalette: public wxGDIObject
{
  DECLARE_DYNAMIC_CLASS(wxPalette)

  public:

    wxPalette();
    wxPalette(int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue);
    wxPalette(const wxPalette& palette);
    ~wxPalette();
    wxPalette& operator = (const wxPalette& palette);
    bool operator == (const wxPalette& palette) const;
    bool operator != (const wxPalette& palette) const;
    bool Ok() const;

    bool Create(int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue);
    int GetPixel(const unsigned char red, const unsigned char green, const unsigned char blue) const;
    bool GetRGB(int pixel, unsigned char *red, unsigned char *green, unsigned char *blue) const;

    // implementation
    int GetColoursCount() const;   
    // FIXME_MGL -- make this method standard part of wx API
    // FIXME_MGL -- create wxXXXBase classes for all GDI stuff (Vadim wants that)
    
    void *GetMGLpalette_t() const;
};

#define wxColorMap wxPalette
#define wxColourMap wxPalette

#endif // __WX_PALETTE_H__
