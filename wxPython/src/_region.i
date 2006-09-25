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


// // these constants are used with wxRegion::Combine() in the ports which have
// // this method
// enum wxRegionOp
// {
//     // Creates the intersection of the two combined regions.
//     wxRGN_AND,

//     // Creates a copy of the region
//     wxRGN_COPY,

//     // Combines the parts of first region that are not in the second one
//     wxRGN_DIFF,

//     // Creates the union of two combined regions.
//     wxRGN_OR,

//     // Creates the union of two regions except for any overlapping areas.
//     wxRGN_XOR
// };



MustHaveApp(wxRegion);

class wxRegion : public wxGDIObject {
public:
    wxRegion(wxCoord x=0, wxCoord y=0, wxCoord width=0, wxCoord height=0);
    %RenameCtor(RegionFromBitmap, wxRegion(const wxBitmap& bmp));
    %RenameCtor(RegionFromBitmapColour, wxRegion(const wxBitmap& bmp,
                                                 const wxColour& transColour,
                                                 int   tolerance = 0));
    %RenameCtor(RegionFromPoints, wxRegion(int points, wxPoint* points_array,
                                           int fillStyle = wxWINDING_RULE));

    ~wxRegion();


    void Clear();
    bool Offset(wxCoord x, wxCoord y);

    wxRegionContain Contains(wxCoord x, wxCoord y);
    %Rename(ContainsPoint, wxRegionContain, Contains(const wxPoint& pt));
    %Rename(ContainsRect, wxRegionContain, Contains(const wxRect& rect));
    %Rename(ContainsRectDim, wxRegionContain, Contains(wxCoord x, wxCoord y, wxCoord w, wxCoord h));

    wxRect GetBox();

    bool Intersect(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    %Rename(IntersectRect, bool, Intersect(const wxRect& rect));
    %Rename(IntersectRegion, bool, Intersect(const wxRegion& region));

    bool IsEmpty();

    // Is region equal (i.e. covers the same area as another one)?
    bool IsEqual(const wxRegion& region) const;

    bool Union(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    %Rename(UnionRect, bool, Union(const wxRect& rect));
    %Rename(UnionRegion, bool, Union(const wxRegion& region));

    bool Subtract(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    %Rename(SubtractRect, bool, Subtract(const wxRect& rect));
    %Rename(SubtractRegion, bool, Subtract(const wxRegion& region));

    bool Xor(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    %Rename(XorRect, bool, Xor(const wxRect& rect));
    %Rename(XorRegion, bool, Xor(const wxRegion& region));

    // Convert the region to a B&W bitmap with the white pixels being inside
    // the region.
    wxBitmap ConvertToBitmap();

    // Use the non-transparent pixels of a wxBitmap for the region to combine
    // with this region.  If the bitmap has a mask then it will be used,
    // otherwise the colour to be treated as transparent may be specified,
    // along with an optional tolerance value.
    %Rename(UnionBitmap, bool, Union(const wxBitmap& bmp));
    %Rename(UnionBitmapColour, bool, Union(const wxBitmap& bmp,
                                           const wxColour& transColour,
                                           int   tolerance = 0));


//     bool Combine(wxCoord x, wxCoord y, wxCoord w, wxCoord h, wxRegionOp op);
//     %Rename(CombineRect, bool , Combine(const wxRect& rect, wxRegionOp op));
//     %Rename(CombineRegion, bool , Combine(const wxRegion& region, wxRegionOp op));
    
    %property(Box, GetBox, doc="See `GetBox`");    
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

    %property(H, GetH, doc="See `GetH`");
    %property(Height, GetHeight, doc="See `GetHeight`");
    %property(Rect, GetRect, doc="See `GetRect`");
    %property(W, GetW, doc="See `GetW`");
    %property(Width, GetWidth, doc="See `GetWidth`");
    %property(X, GetX, doc="See `GetX`");
    %property(Y, GetY, doc="See `GetY`");
};



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
