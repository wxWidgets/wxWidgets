/////////////////////////////////////////////////////////////////////////////
// Name:        colour.h
// Purpose:     wxColour class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __COLOURH__
#define __COLOURH__

#ifdef __GNUG__
#pragma interface "colour.h"
#endif

// Colour
class WXDLLEXPORT wxColour: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxColour)
public:
  wxColour(void);
  wxColour(const unsigned char r, const unsigned char g, const unsigned char b);
  wxColour(unsigned long colRGB) { Set(colRGB); }
  wxColour(const wxColour& col);
  wxColour(const wxString& col);
  ~wxColour(void) ;
  wxColour& operator =(const wxColour& src) ;
  wxColour& operator =(const wxString& src) ;
  inline int Ok(void) const { return (m_isInit) ; }

  void Set(unsigned char r, unsigned char g, unsigned char b);
  void Set(unsigned long colRGB) 
  { 
    // we don't need to know sizeof(long) here because we assume that the three
    // least significant bytes contain the R, G and B values
    Set((unsigned char)colRGB, 
        (unsigned char)(colRGB >> 8),
        (unsigned char)(colRGB >> 16)); 
  }

  // Let's remove this inelegant function
#if WXWIN_COMPATIBILITY
  void Get(unsigned char *r, unsigned char *g, unsigned char *b) const;
#endif

  inline unsigned char Red(void) const { return m_red; }
  inline unsigned char Green(void) const { return m_green; }
  inline unsigned char Blue(void) const { return m_blue; }

  inline bool operator == (const wxColour& colour) { return (m_red == colour.m_red && m_green == colour.m_green && m_blue == colour.m_blue); }

  inline bool operator != (const wxColour& colour) { return (!(m_red == colour.m_red && m_green == colour.m_green && m_blue == colour.m_blue)); }

  WXCOLORREF GetPixel(void) const { return m_pixel; };

 private:
  bool 			m_isInit;
  unsigned char m_red;
  unsigned char m_blue;
  unsigned char m_green;
 public:
  WXCOLORREF m_pixel ;
};

#define wxColor wxColour

#endif
	// __COLOURH__
