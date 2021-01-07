/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/region.h
// Purpose:     header for wxRegion
// Author:      Peter Most, Javier Torres
// Copyright:   (c) Peter Most, Javier Torres
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_REGION_H_
#define _WX_QT_REGION_H_

class QRegion;
class QRect;
template<class T> class QVector;

class WXDLLIMPEXP_CORE wxRegion : public wxRegionBase
{
public:
    wxRegion();
    wxRegion(wxCoord x, wxCoord y, wxCoord w, wxCoord h);
    wxRegion(const wxPoint& topLeft, const wxPoint& bottomRight);
    wxRegion(const wxRect& rect);
    wxRegion(size_t n, const wxPoint *points, wxPolygonFillMode fillStyle = wxODDEVEN_RULE);
    wxRegion(const wxBitmap& bmp);
    wxRegion(const wxBitmap& bmp, const wxColour& transp, int tolerance = 0);

    virtual bool IsEmpty() const wxOVERRIDE;
    virtual void Clear() wxOVERRIDE;

    virtual const QRegion &GetHandle() const;
    virtual void QtSetRegion(QRegion region); // Hangs on to this region

protected:
    virtual wxGDIRefData *CreateGDIRefData() const wxOVERRIDE;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const wxOVERRIDE;

    virtual bool DoIsEqual(const wxRegion& region) const wxOVERRIDE;
    virtual bool DoGetBox(wxCoord& x, wxCoord& y, wxCoord& w, wxCoord& h) const wxOVERRIDE;
    virtual wxRegionContain DoContainsPoint(wxCoord x, wxCoord y) const wxOVERRIDE;
    virtual wxRegionContain DoContainsRect(const wxRect& rect) const wxOVERRIDE;

    virtual bool DoOffset(wxCoord x, wxCoord y) wxOVERRIDE;

    virtual bool DoUnionWithRect(const wxRect& rect) wxOVERRIDE;
    virtual bool DoUnionWithRegion(const wxRegion& region) wxOVERRIDE;

    virtual bool DoIntersect(const wxRegion& region) wxOVERRIDE;
    virtual bool DoSubtract(const wxRegion& region) wxOVERRIDE;
    virtual bool DoXor(const wxRegion& region) wxOVERRIDE;

    virtual bool DoCombine(const wxRegion& rgn, wxRegionOp op);

private:
    wxDECLARE_DYNAMIC_CLASS(wxRegion);
};



class WXDLLIMPEXP_CORE wxRegionIterator: public wxObject
{
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
    QVector < QRect > *m_qtRects;
    int m_pos;

    wxDECLARE_DYNAMIC_CLASS(wxRegionIterator);
};

#endif // _WX_QT_REGION_H_
