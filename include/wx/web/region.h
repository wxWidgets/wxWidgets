#ifndef __WX_REGION_H__
#define __WX_REGION_H__

class WXDLLIMPEXP_CORE wxRegion : public wxRegionBase {
public:
    wxRegion();

    wxRegion(wxCoord x, wxCoord y, wxCoord w, wxCoord h);
    wxRegion(const wxPoint& topLeft, const wxPoint& bottomRight);
    wxRegion(const wxRect& rect );
    wxRegion(size_t n, const wxPoint *points, int fillStyle = wxODDEVEN_RULE);
    wxRegion(const wxBitmap& bmp);
    wxRegion(const wxBitmap& bmp,
             const wxColour& transColour, int tolerance = 0);
    virtual ~wxRegion();

    virtual void Clear();
    virtual bool IsEmpty() const;

protected:
    virtual bool DoIsEqual(const wxRegion& region) const;
    virtual bool DoGetBox(wxCoord& x, wxCoord& y, wxCoord& w, wxCoord& h) const;
    virtual wxRegionContain DoContainsPoint(wxCoord x, wxCoord y) const;
    virtual wxRegionContain DoContainsRect(const wxRect& rect) const;

    virtual bool DoOffset(wxCoord x, wxCoord y);
    virtual bool DoUnionWithRect(const wxRect& rect);
    virtual bool DoUnionWithRegion(const wxRegion& region);
    virtual bool DoIntersect(const wxRegion& region);
    virtual bool DoSubtract(const wxRegion& region);
    virtual bool DoXor(const wxRegion& region);

    void InitRect(wxCoord x, wxCoord y, wxCoord w, wxCoord h);

private:
    DECLARE_DYNAMIC_CLASS(wxRegion)
};

class WXDLLIMPEXP_CORE wxRegionIterator: public wxObject {
public:
    wxRegionIterator();
    wxRegionIterator(const wxRegion& region);
    wxRegionIterator(const wxRegionIterator& ri);
    ~wxRegionIterator();

    wxRegionIterator& operator=(const wxRegionIterator& ri);

    void Reset();
    void Reset(const wxRegion& region);

    bool HaveRects() const;
    operator bool () const;

    wxRegionIterator& operator ++ ();
    wxRegionIterator operator ++ (int);

    wxCoord GetX() const;
    wxCoord GetY() const;
    wxCoord GetW() const;
    wxCoord GetWidth() const;
    wxCoord GetH() const;
    wxCoord GetHeight() const;
    wxRect GetRect() const;

private:
    DECLARE_DYNAMIC_CLASS(wxRegionIterator)
};


#endif // __WX_REGION_H__
