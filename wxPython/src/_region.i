/////////////////////////////////////////////////////////////////////////////
// Name:        _imaglist.i
// Purpose:     SWIG interface defs for wxImageList and releated classes
//
// Author:      Robin Dunn
//
// Created:     7-July-1997
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

%{
%}

//---------------------------------------------------------------------------

%typemap(in) (int points, wxPoint* points_array ) {
    $2 = wxPoint_LIST_helper($input, &$1);
    if ($2 == NULL) SWIG_fail;
}
%typemap(freearg) (int points, wxPoint* points_array ) {
    if ($2) delete [] $2;
}

//---------------------------------------------------------------------------
%newgroup


enum wxRegionContain
{
    wxOutRegion = 0,
    wxPartRegion = 1,
    wxInRegion = 2
};



MustHaveApp(wxRegion);

class wxRegion : public wxGDIObject {
public:
    wxRegion(wxCoord x=0, wxCoord y=0, wxCoord width=0, wxCoord height=0);
    %name(RegionFromBitmap)wxRegion(const wxBitmap& bmp);
    %name(RegionFromBitmapColour)wxRegion(const wxBitmap& bmp,
                                          const wxColour& transColour,
                                          int   tolerance = 0);
    %name(RegionFromPoints)wxRegion(int points, wxPoint* points_array,
                                      int fillStyle = wxWINDING_RULE);

    ~wxRegion();


    void Clear();
    bool Offset(wxCoord x, wxCoord y);

    wxRegionContain Contains(wxCoord x, wxCoord y);
    %name(ContainsPoint)wxRegionContain Contains(const wxPoint& pt);
    %name(ContainsRect)wxRegionContain Contains(const wxRect& rect);
    %name(ContainsRectDim)wxRegionContain Contains(wxCoord x, wxCoord y, wxCoord w, wxCoord h);

    wxRect GetBox();

    bool Intersect(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    %name(IntersectRect)bool Intersect(const wxRect& rect);
    %name(IntersectRegion)bool Intersect(const wxRegion& region);

    bool IsEmpty();

    bool Union(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    %name(UnionRect)bool Union(const wxRect& rect);
    %name(UnionRegion)bool Union(const wxRegion& region);

    bool Subtract(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    %name(SubtractRect)bool Subtract(const wxRect& rect);
    %name(SubtractRegion)bool Subtract(const wxRegion& region);

    bool Xor(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    %name(XorRect)bool Xor(const wxRect& rect);
    %name(XorRegion)bool Xor(const wxRegion& region);

    // Convert the region to a B&W bitmap with the white pixels being inside
    // the region.
    wxBitmap ConvertToBitmap();

    // Use the non-transparent pixels of a wxBitmap for the region to combine
    // with this region.  If the bitmap has a mask then it will be used,
    // otherwise the colour to be treated as transparent may be specified,
    // along with an optional tolerance value.
    %name(UnionBitmap)bool Union(const wxBitmap& bmp);
    %name(UnionBitmapColour)bool Union(const wxBitmap& bmp,
                                       const wxColour& transColour,
                                       int   tolerance = 0);
};



MustHaveApp(wxRegionIterator);

class wxRegionIterator : public wxObject {
public:
    wxRegionIterator(const wxRegion& region);
    ~wxRegionIterator();

    wxCoord GetX();
    wxCoord GetY();
    wxCoord GetW();
    wxCoord GetWidth();
    wxCoord GetH();
    wxCoord GetHeight();
    wxRect GetRect();
    bool HaveRects();
    void Reset();

    %extend {
        void Next() {
            (*self) ++;
        }

        bool __nonzero__() {
            return self->operator bool();
        }
    };
};



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
