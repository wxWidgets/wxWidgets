/////////////////////////////////////////////////////////////////////////////
// Name:        brush.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
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
public:
    wxBrush();
    wxBrush( const wxColour &colour, int style );
    wxBrush( const wxBitmap &stippleBitmap );
    wxBrush( const wxBrush &brush );
    ~wxBrush();
    wxBrush& operator = ( const wxBrush& brush );
    bool operator == ( const wxBrush& brush );
    bool operator != ( const wxBrush& brush );
    bool Ok() const;

    int GetStyle() const;
    wxColour &GetColour() const;
    wxBitmap *GetStipple() const;

    void SetColour( const wxColour& col );
    void SetColour( unsigned char r, unsigned char g, unsigned char b );
    void SetStyle( int style );
    void SetStipple( const wxBitmap& stipple );

    void Unshare();

private:
    DECLARE_DYNAMIC_CLASS(wxBrush)
};

#endif // __GTKBRUSHH__
