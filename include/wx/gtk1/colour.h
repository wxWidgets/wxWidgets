/////////////////////////////////////////////////////////////////////////////
// Name:        colour.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKCOLOURH__
#define __GTKCOLOURH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/gdiobj.h"
#include "wx/palette.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxDC;
class wxPaintDC;
class wxBitmap;
class wxWindow;

class wxColour;

//-----------------------------------------------------------------------------
// wxColour
//-----------------------------------------------------------------------------

class wxColour: public wxGDIObject
{
public:
  // ctors
    // default
  wxColour();
    // from RGB
  wxColour( unsigned char red, unsigned char green, unsigned char blue );
  wxColour( unsigned long colRGB ) { Set(colRGB); }

    // implicit conversion from the colour name
  wxColour( const wxString &colourName ) { InitFromName(colourName); }
  wxColour( const char *colourName ) { InitFromName(colourName); }

    // copy ctors and assignment operators
  wxColour( const wxColour& col );
  wxColour& operator = ( const wxColour& col );

    // dtor
  ~wxColour();

  // comparison
  bool operator == ( const wxColour& col ) const;
  bool operator != ( const wxColour& col ) const;

  // accessors
  void Set( unsigned char red, unsigned char green, unsigned char blue );
  void Set( unsigned long colRGB )
  {
    // we don't need to know sizeof(long) here because we assume that the three
    // least significant bytes contain the R, G and B values
    Set((unsigned char)colRGB,
        (unsigned char)(colRGB >> 8),
        (unsigned char)(colRGB >> 16));
  }

  unsigned char Red() const;
  unsigned char Green() const;
  unsigned char Blue() const;
  bool Ok() const;

  // implementation
  void CalcPixel( GdkColormap *cmap );
  int GetPixel() const;
  GdkColor *GetColor() const;

protected:
  // helper functions
  void InitFromName(const wxString& colourName);

private:
  DECLARE_DYNAMIC_CLASS(wxColour)
};

#endif // __GTKCOLOURH__
