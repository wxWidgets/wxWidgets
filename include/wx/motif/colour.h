/////////////////////////////////////////////////////////////////////////////
// Name:        colour.h
// Purpose:     wxColour class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COLOUR_H_
#define _WX_COLOUR_H_

#ifdef __GNUG__
#pragma interface "colour.h"
#endif

#include "wx/object.h"
#include "wx/string.h"

// Colour
class WXDLLEXPORT wxColour: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxColour)
public:
  wxColour();
  wxColour(unsigned char r, unsigned char g, unsigned char b);
  wxColour(unsigned long colRGB) { Set(colRGB); }
  wxColour(const wxColour& col);
  wxColour(const wxString& col);
  ~wxColour() ;
  wxColour& operator =(const wxColour& src) ;
  wxColour& operator =(const wxString& src) ;
  inline int Ok() const { return (m_isInit) ; }

  void Set(unsigned char r, unsigned char g, unsigned char b);
  void Set(unsigned long colRGB) 
  { 
    // we don't need to know sizeof(long) here because we assume that the three
    // least significant bytes contain the R, G and B values
    Set((unsigned char)colRGB, 
        (unsigned char)(colRGB >> 8),
        (unsigned char)(colRGB >> 16)); 
  }

  inline unsigned char Red() const { return m_red; }
  inline unsigned char Green() const { return m_green; }
  inline unsigned char Blue() const { return m_blue; }

  inline bool operator == (const wxColour& colour) { return (m_red == colour.m_red && m_green == colour.m_green && m_blue == colour.m_blue); }

  inline bool operator != (const wxColour& colour) { return (!(m_red == colour.m_red && m_green == colour.m_green && m_blue == colour.m_blue)); }

  inline int GetPixel() const { return m_pixel; };
  inline void SetPixel(int pixel) { m_pixel = pixel; };

  // Allocate a colour, or nearest colour, using the given display.
  // If realloc is TRUE, ignore the existing pixel, otherwise just return
  // the existing one.
  // Returns FALSE if an exact match was not found, TRUE otherwise.

  // TODO: can this handle mono displays? If not, we should have an extra
  // flag to specify whether this should be black or white by default.

  bool AllocColour(WXDisplay* display, bool realloc);

 private:
  bool          m_isInit;
  unsigned char m_red;
  unsigned char m_blue;
  unsigned char m_green;
 public:
  int           m_pixel;

};

#define wxColor wxColour

#endif
	// _WX_COLOUR_H_
