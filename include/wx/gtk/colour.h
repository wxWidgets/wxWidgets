/////////////////////////////////////////////////////////////////////////////
// Name:        colour.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
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
  DECLARE_DYNAMIC_CLASS(wxColour)

  public:

    wxColour(void);
    wxColour( char red, char green, char blue );
    wxColour( const wxString &colourName );
    wxColour( const wxColour& col );
    wxColour( const wxColour* col );
    ~wxColour(void);
    wxColour& operator = ( const wxColour& col );
    wxColour& operator = ( const wxString& colourName );
    bool operator == ( const wxColour& col );
    bool operator != ( const wxColour& col );
    void Set( const unsigned char red, const unsigned char green, const unsigned char blue );
    unsigned char Red(void) const;
    unsigned char Green(void) const;
    unsigned char Blue(void) const;
    bool Ok(void) const;

  private:
  public:
  
    friend wxDC;
    friend wxPaintDC;
    friend wxBitmap;
    friend wxWindow;
        
    void CalcPixel( GdkColormap *cmap );
    int GetPixel(void);
    GdkColor *GetColor(void);
    
    // no data :-)
};
  
#endif // __GTKCOLOURH__
