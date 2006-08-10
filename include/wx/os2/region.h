/////////////////////////////////////////////////////////////////////////////
// Name:        region.h
// Purpose:     wxRegion class
// Author:      David Webster
// Modified by:
// Created:     10/15/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_REGION_H_
#define _WX_REGION_H_

#include "wx/list.h"
#include "wx/gdiobj.h"
#include "wx/gdicmn.h"
#include "wx/bitmap.h"
#include "wx/os2/private.h"

class WXDLLEXPORT wxRect;
class WXDLLEXPORT wxPoint;

// So far, for internal use only
enum wxRegionOp { wxRGN_AND         // Creates the intersection of the two combined regions.
                 ,wxRGN_COPY         // Creates a copy of the region identified by hrgnSrc1.
                 ,wxRGN_DIFF         // Combines the parts of hrgnSrc1 that are not part of hrgnSrc2.
                 ,wxRGN_OR           // Creates the union of two combined regions.
                 ,wxRGN_XOR          // Creates the union of two combined regions except for any overlapping areas.
                };

class WXDLLEXPORT wxRegion : public wxGDIObject
{
public:
    wxRegion( wxCoord x
             ,wxCoord y
             ,wxCoord vWidth
             ,wxCoord vHeight
            );
    wxRegion( const wxPoint& rTopLeft
             ,const wxPoint& rBottomRight
            );
    wxRegion(const wxRect& rRect);
    wxRegion(WXHRGN hRegion, WXHDC hPS); // Hangs on to this region
    wxRegion( const wxBitmap& bmp)
    {
        Union(bmp);
    }
    wxRegion( const wxBitmap& bmp,
              const wxColour& transColour, int tolerance = 0)
    {
        Union(bmp, transColour, tolerance);
    }

    wxRegion();
    ~wxRegion();

    //
    // Modify region
    //

    //
    // Clear current region
    //
    void Clear(void);

    bool Offset( wxCoord x
                ,wxCoord y
               );

    //
    // Union rectangle or region with this.
    //
    inline bool Union( wxCoord x
                      ,wxCoord y
                      ,wxCoord vWidth
                      ,wxCoord vHeight
                     )
    {
        return Combine( x
                       ,y
                       ,vWidth
                       ,vHeight
                       ,wxRGN_OR
                      );
    }
    inline bool Union( const wxRect& rRect) { return Combine(rRect, wxRGN_OR); }
    inline bool Union(const wxRegion& rRegion) { return Combine(rRegion, wxRGN_OR); }

    //
    // Intersect rectangle or region with this.
    //
    inline bool Intersect( wxCoord x
                          ,wxCoord y
                          ,wxCoord vWidth
                          ,wxCoord vHeight
                         )
    {
        return Combine( x
                       ,y
                       ,vWidth
                       ,vHeight
                       ,wxRGN_AND
                      );
    }
    inline bool Intersect(const wxRect& rRect)  { return Combine(rRect, wxRGN_AND); }
    inline bool Intersect(const wxRegion& rRegion)  { return Combine(rRegion, wxRGN_AND); }

    //
    // Subtract rectangle or region from this:
    // Combines the parts of 'this' that are not part of the second region.
    //
    inline bool Subtract( wxCoord x
                         ,wxCoord y
                         ,wxCoord vWidth
                         ,wxCoord vHeight
                        )
    {
        return Combine( x
                       ,y
                       ,vWidth
                       ,vHeight
                       ,wxRGN_DIFF
                      );
    }
    inline bool Subtract(const wxRect& rRect)  { return Combine(rRect, wxRGN_DIFF); }
    inline bool Subtract(const wxRegion& rRegion)  { return Combine(rRegion, wxRGN_DIFF); }

    //
    // XOR: the union of two combined regions except for any overlapping areas.
    //
    inline bool Xor( wxCoord x
                    ,wxCoord y
                    ,wxCoord vWidth
                    ,wxCoord vHeight
                   )
    {
        return Combine( x
                       ,y
                       ,vWidth
                       ,vHeight
                       ,wxRGN_XOR
                      );
    }
    inline bool Xor(const wxRect& rRect)  { return Combine(rRect, wxRGN_XOR); }
    inline bool Xor(const wxRegion& rRegion)  { return Combine(rRegion, wxRGN_XOR); }

    //
    // Information on region
    // Outer bounds of region
    //
    void   GetBox( wxCoord& rX
                  ,wxCoord& rY
                  ,wxCoord& rWidth
                  ,wxCoord& rHeight
                 ) const;
    wxRect GetBox(void) const;

    //
    // Is region empty?
    //
    bool        Empty(void) const;
    inline bool IsEmpty() const { return Empty(); }

    //
    // Tests
    // Does the region contain the point (x,y)?
    //
    inline wxRegionContain Contains( wxCoord lX, wxCoord lY ) const{
        return Contains( wxPoint( lX, lY ) );
    }
    //
    // Convert the region to a B&W bitmap with the black pixels being inside
    // the region.
    //
    wxBitmap        ConvertToBitmap(void) const;

    // Use the non-transparent pixels of a wxBitmap for the region to combine
    // with this region.  First version takes transparency from bitmap's mask,
    // second lets the user specify the colour to be treated as transparent
    // along with an optional tolerance value.
    // NOTE: implemented in common/rgncmn.cpp
    bool Union(const wxBitmap& bmp);
    bool Union(const wxBitmap& bmp,
               const wxColour& transColour, int tolerance = 0);

    //
    // Does the region contain the point pt?
    //
    wxRegionContain Contains(const wxPoint& rPoint) const;

    //
    // Does the region contain the rectangle (x, y, w, h)?
    //
    wxRegionContain Contains( wxCoord x
                             ,wxCoord y
                             ,wxCoord lWidth
                             ,wxCoord lHeight
                            ) const;

    //
    // Does the region contain the rectangle rect?
    //
    inline wxRegionContain Contains(const wxRect& rRect) const{
         return Contains( rRect.x, rRect.y,
                          rRect.GetWidth(), rRect.GetHeight());
    }

    //
    // Internal
    //
    bool Combine( wxCoord    x
                 ,wxCoord    y
                 ,wxCoord    vWidth
                 ,wxCoord    vHeight
                 ,wxRegionOp eOp
                );
    bool Combine( const wxRegion& rRegion
                 ,wxRegionOp      eOp
                );
    bool Combine( const wxRect& rRect
                 ,wxRegionOp    eOp
                );

    //
    // Get internal region handle
    //
    WXHRGN GetHRGN(void) const;
    void   SetPS(HPS hPS);

protected:
    virtual wxObjectRefData* CreateData(void) const;
    virtual wxObjectRefData* CloneData(const wxObjectRefData* pData) const;

    friend class WXDLLEXPORT wxRegionIterator;
    DECLARE_DYNAMIC_CLASS(wxRegion);

}; // end of CLASS wxRegion

class WXDLLEXPORT wxRegionIterator : public wxObject
{
DECLARE_DYNAMIC_CLASS(wxRegionIterator);
public:
    wxRegionIterator();
    wxRegionIterator(const wxRegion& rRegion);
    ~wxRegionIterator();

    void Reset(void) { m_lCurrent = 0; }
    void Reset(const wxRegion& rRegion);

    operator bool (void) const { return m_lCurrent < m_lNumRects; }
    bool HaveRects(void) const { return m_lCurrent < m_lNumRects; }

    void operator ++ (void);
    void operator ++ (int);

    wxCoord GetX(void) const;
    wxCoord GetY(void) const;
    wxCoord GetW(void) const;
    wxCoord GetWidth(void) const { return GetW(); }
    wxCoord GetH(void) const;
    wxCoord GetHeight(void) const { return GetH(); }
    wxRect  GetRect(void) const { return wxRect(GetX(), GetY(), GetWidth(), GetHeight()); }

private:
    long                            m_lCurrent;
    long                            m_lNumRects;
    wxRegion                        m_vRegion;
    wxRect*                         m_pRects;
}; // end of wxRegionIterator

#endif
    // _WX_REGION_H_
