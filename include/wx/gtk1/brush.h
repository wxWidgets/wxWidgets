/////////////////////////////////////////////////////////////////////////////
// Name:        brush.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKBRUSHH__
#define __GTKBRUSHH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/gdiobj.h"
#include "wx/bitmap.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxBrush;

//-----------------------------------------------------------------------------
// wxBrush
//-----------------------------------------------------------------------------

class wxBrush: public wxGDIObject
{
  DECLARE_DYNAMIC_CLASS(wxBrush)

  public:

    wxBrush(void);
    wxBrush( const wxColour &colour, const int style );
    wxBrush( const wxString &colourName, const int style );
    wxBrush( const wxBitmap &stippleBitmap );
    wxBrush( const wxBrush &brush );
    wxBrush( const wxBrush *brush );
    ~wxBrush(void);
    wxBrush& operator = ( const wxBrush& brush );
    bool operator == ( const wxBrush& brush );
    bool operator != ( const wxBrush& brush );
    bool Ok(void) const;

    int GetStyle(void) const;
    wxColour &GetColour(void) const;
    wxBitmap *GetStipple(void) const;
    
    // no data :-)
};

#endif // __GTKBRUSHH__
