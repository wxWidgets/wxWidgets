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

    wxColour();
    wxColour( char red, char green, char blue );
    wxColour( const wxString &colourName ) { InitFromName(colourName); }
//    wxColour( const char *colourName ) { InitFromName(colourName); }
    wxColour( const wxColour& col );
    wxColour( const wxColour* col );
    ~wxColour();
    wxColour& operator = ( const wxColour& col );
    wxColour& operator = ( const wxString& colourName );
    bool operator == ( const wxColour& col );
    bool operator != ( const wxColour& col );
    void Set( const unsigned char red, const unsigned char green, const unsigned char blue );
    unsigned char Red() const;
    unsigned char Green() const;
    unsigned char Blue() const;
    bool Ok() const;

  private:
  public:

    friend wxDC;
    friend wxPaintDC;
    friend wxBitmap;
    friend wxWindow;

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
