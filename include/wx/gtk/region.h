/////////////////////////////////////////////////////////////////////////////
// Name:        region.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __REGIONH__
#define __REGIONH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/list.h"
#include "wx/gdiobj.h"
#include "wx/gdicmn.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxRegion;

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

enum wxRegionContain 
{
  wxOutRegion = 0, wxPartRegion = 1, wxInRegion = 2
};

// So far, for internal use only
enum wxRegionOp {
wxRGN_AND,          // Creates the intersection of the two combined regions.
wxRGN_COPY,         // Creates a copy of the region identified by hrgnSrc1.
wxRGN_DIFF,         // Combines the parts of hrgnSrc1 that are not part of hrgnSrc2.
wxRGN_OR,           // Creates the union of two combined regions.
wxRGN_XOR           // Creates the union of two combined regions except for any overlapping areas.
};

//-----------------------------------------------------------------------------
// wxRegion
//-----------------------------------------------------------------------------

class wxRegion : public wxGDIObject 
{
  DECLARE_DYNAMIC_CLASS(wxRegion);
  
  public:
  
    wxRegion( long x, long y, long w, long h );
    wxRegion( const wxPoint& topLeft, const wxPoint& bottomRight );
    wxRegion( const wxRect& rect );
    wxRegion(void);
    ~wxRegion(void);

    inline wxRegion( const wxRegion& r ) 
      { Ref(r); }
    inline wxRegion& operator = ( const wxRegion& r )
      { Ref(r); return (*this); }

    void Clear(void);

    bool Union( long x, long y, long width, long height );
    bool Union( const wxRect& rect );
    bool Union( const wxRegion& region );

    bool Intersect( long x, long y, long width, long height );
    bool Intersect( const wxRect& rect );
    bool Intersect( const wxRegion& region );

    bool Subtract( long x, long y, long width, long height );
    bool Subtract( const wxRect& rect );
    bool Subtract( const wxRegion& region );

    bool Xor( long x, long y, long width, long height );
    bool Xor( const wxRect& rect );
    bool Xor( const wxRegion& region );

    void GetBox( long& x, long& y, long&w, long &h ) const;
    wxRect GetBox(void) const ;

    bool Empty(void) const;

    wxRegionContain Contains( long x, long y ) const;
    wxRegionContain Contains( long x, long y, long w, long h ) const;
    
  public:
    
    GdkRegion *GetRegion(void) const;
};

#endif
	// __REGIONH__
