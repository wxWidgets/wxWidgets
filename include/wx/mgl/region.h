/////////////////////////////////////////////////////////////////////////////
// Name:        region.h
// Purpose:     wxRegion class
// Author:      Vaclav Slavik
// Created:     2001/03/12
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vaclav Slavik
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_REGION_H_
#define _WX_REGION_H_

#ifdef __GNUG__
#pragma interface "region.h"
#endif

#include "wx/list.h"
#include "wx/gdiobj.h"
#include "wx/gdicmn.h"
#include "wx/list.h"

class WXDLLEXPORT wxRect;
class WXDLLEXPORT wxPoint;
class MGLRegion;

enum wxRegionContain 
{
	wxOutRegion = 0, 
    wxPartRegion = 1, 
    wxInRegion = 2
};

class WXDLLEXPORT wxRegion : public wxGDIObject 
{
    DECLARE_DYNAMIC_CLASS(wxRegion);
	friend class WXDLLEXPORT wxRegionIterator;

public:
    wxRegion(wxCoord x, wxCoord y, wxCoord w, wxCoord h);
    wxRegion(const wxPoint& topLeft, const wxPoint& bottomRight);
    wxRegion(const wxRect& rect);
    wxRegion(const MGLRegion& region);

	wxRegion();
	~wxRegion();

	//# Copying
	inline wxRegion(const wxRegion& r)
		{ Ref(r); }
	inline wxRegion& operator = (const wxRegion& r)
		{ Ref(r); return (*this); }

	//# Modify region
	// Clear current region
	void Clear(void);

	// Union rectangle or region with this.
	bool Union(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
	bool Union(const wxRect& rect) { return Union(rect.x, rect.y, rect.width, rect.height); }
	bool Union(const wxRegion& region);

	// Intersect rectangle or region with this.
	bool Intersect(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
	bool Intersect(const wxRect& rect) { return Intersect(rect.x, rect.y, rect.width, rect.height); }
	bool Intersect(const wxRegion& region);

	// Subtract rectangle or region from this:
    // Combines the parts of 'this' that are not part of the second region.
	bool Subtract(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
	bool Subtract(const wxRect& rect) { return Subtract(rect.x, rect.y, rect.width, rect.height); }
	bool Subtract(const wxRegion& region);

	// XOR: the union of two combined regions except for any overlapping areas.
	bool Xor(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
	bool Xor(const wxRect& rect) { return Xor(rect.x, rect.y, rect.width, rect.height); }
	bool Xor(const wxRegion& region);

	//# Information on region
	// Outer bounds of region
	void GetBox(wxCoord& x, wxCoord& y, wxCoord&w, wxCoord &h) const;
	wxRect GetBox(void) const ;

	// Is region empty?
	bool Empty(void) const;
    inline bool IsEmpty(void) const { return Empty(); }

	//# Tests
	// Does the region contain the point (x,y)?
	wxRegionContain Contains(wxCoord x, wxCoord y) const;
	// Does the region contain the point pt?
	wxRegionContain Contains(const wxPoint& pt) const;
	// Does the region contain the rectangle (x, y, w, h)?
	wxRegionContain Contains(wxCoord x, wxCoord y, wxCoord w, wxCoord h) const;
	// Does the region contain the rectangle rect?
	wxRegionContain Contains(const wxRect& rect) const;
    
    // implementation from now on:
    const MGLRegion& GetMGLRegion() const;
    
private:
    void Unshare();
};


WX_DECLARE_EXPORTED_LIST(wxRect, wxRegionRectList);

class WXDLLEXPORT wxRegionIterator : public wxObject 
{
    DECLARE_DYNAMIC_CLASS(wxRegionIterator);
public:
	wxRegionIterator(void);
	wxRegionIterator(const wxRegion& region);
	~wxRegionIterator(void);

	void Reset(void) { m_currentNode = NULL; }
	void Reset(const wxRegion& region);

#ifndef __SALFORDC__
	operator bool (void) const { return (m_currentNode != NULL); }
#endif

	bool HaveRects(void) const { return (m_currentNode != NULL); }

	void operator ++ (void);
	void operator ++ (int);

	wxCoord GetX(void) const;
	wxCoord GetY(void) const;
	wxCoord GetW(void) const;
	wxCoord GetWidth(void) const { return GetW(); }
	wxCoord GetH(void) const;
	wxCoord GetHeight(void) const { return GetH(); }
    wxRect GetRect() const { return wxRect(GetX(), GetY(), GetWidth(), GetHeight()); }

private:
    wxRegionRectList         m_rects;
    wxRegionRectList::Node  *m_currentNode;
};

#endif
	// _WX_REGION_H_
