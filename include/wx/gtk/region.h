/////////////////////////////////////////////////////////////////////////////
// Name:        region.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
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
    wxRegion();
    ~wxRegion();

    inline wxRegion( const wxRegion& r ): wxGDIObject()
      { Ref(r); }
    inline wxRegion& operator = ( const wxRegion& r )
      { Ref(r); return (*this); }

    bool operator == ( const wxRegion& region );
    bool operator != ( const wxRegion& region );
    
    void Clear();

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
    wxRect GetBox() const ;

    bool Empty() const;
    bool IsEmpty() const { return Empty(); }

    wxRegionContain Contains( long x, long y ) const;
    wxRegionContain Contains( long x, long y, long w, long h ) const;
    wxRegionContain Contains(const wxPoint& pt) const;
    wxRegionContain Contains(const wxRect& rect) const;
    
  public:
  
    wxList    *GetRectList() const;    
    GdkRegion *GetRegion() const;
};

class wxRegionIterator: public wxObject 
{
public:
    wxRegionIterator();
    wxRegionIterator(const wxRegion& region);

    void Reset() { m_current = 0; }
    void Reset(const wxRegion& region);

    operator bool () const;
    bool HaveRects() const;

    void operator ++ ();
    void operator ++ (int);

    long GetX() const;
    long GetY() const;
    long GetW() const;
    long GetWidth() const { return GetW(); }
    long GetH() const;
    long GetHeight() const { return GetH(); }
    wxRect GetRect() const { return wxRect(GetX(), GetY(), GetWidth(), GetHeight()); }

private:
    long       m_current;
    wxRegion   m_region;

private:
    DECLARE_DYNAMIC_CLASS(wxRegionIterator);
};


#endif
	// __REGIONH__
