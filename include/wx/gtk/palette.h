/////////////////////////////////////////////////////////////////////////////
// Name:        palette.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKPALETTEH__
#define __GTKPALETTEH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/gdiobj.h"
#include "wx/gdicmn.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxPalette;

//-----------------------------------------------------------------------------
// wxPalette
//-----------------------------------------------------------------------------

class wxPalette: public wxGDIObject
{
  DECLARE_DYNAMIC_CLASS(wxPalette)

  public:
  
    wxPalette(void);
    wxPalette( const int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue );
    wxPalette( const wxPalette& palette );
    wxPalette( const wxPalette* palette );
    ~wxPalette(void);
    wxPalette& operator = ( const wxPalette& palette );
    bool operator == ( const wxPalette& palette );
    bool operator != ( const wxPalette& palette );
    bool Ok(void) const;
    
    bool Create( const int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue);
    int GetPixel( const unsigned char red, const unsigned char green, const unsigned char blue ) const;
    bool GetRGB( const int pixel, unsigned char *red, unsigned char *green, unsigned char *blue ) const;
    
    // no data
};

#define wxColorMap wxPalette
#define wxColourMap wxPalette

#endif // __GTKPALETTEH__
