#ifndef __WXBOUNDINGBOX_H__
#define __WXBOUNDINGBOX_H__

#ifdef __GNUG__
    #pragma interface "bbox.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wxmatrix.h"
#include "wx/geometry.h"

enum OVERLAP {_IN,_ON,_OUT};

//Purpose   The wxBoundingBox class stores one wxBoundingBox.
//The wxBoundingBox is defined by two coordiates,
//a upperleft coordinate and a lowerright coordinate.
class wxBoundingBox
{
public:
    wxBoundingBox();
    wxBoundingBox(wxBoundingBox&);
    wxBoundingBox(const wxPoint2DDouble&);
    wxBoundingBox(double xmin, double ymin, double xmax, double ymax);

    bool And(wxBoundingBox*, double Marge = 0);

    void EnLarge(const double Marge);
    void Shrink(const double Marge);

    void Expand(const wxPoint2DDouble& , const wxPoint2DDouble&);
    void Expand(const wxPoint2DDouble&);
    void Expand(double x,double y);
    void Expand(const wxBoundingBox& bbox);

    OVERLAP Intersect( wxBoundingBox &, double Marge = 0);
    bool LineIntersect(const wxPoint2DDouble& begin, const wxPoint2DDouble& end );
    bool PointInBox( const wxPoint2DDouble&, double Marge = 0);
    bool PointInBox( double, double, double Marge = 0);

    void Reset();

    void Translate( wxPoint2DDouble& );
    void MapBbox( const wxTransformMatrix& matrix);

    double  GetWidth() {return m_maxx-m_minx;};
    double  GetHeight(){return m_maxy-m_miny;};
    bool    GetValid()  const;
    void    SetValid(bool);

    void    SetBoundingBox(const wxPoint2DDouble& a_point);

    void    SetMin(double, double);
    void    SetMax(double, double);
    inline  wxPoint2DDouble GetMin();
    inline  wxPoint2DDouble GetMax();
    inline  double GetMinX(){return m_minx;};
    inline  double GetMinY(){return m_miny;};
    inline  double GetMaxX(){return m_maxx;};
    inline  double GetMaxY(){return m_maxy;};

    wxBoundingBox&  operator+( wxBoundingBox& );
    wxBoundingBox&  operator=(  const wxBoundingBox& );

protected:
    //bounding box in world
    double        m_minx;
    double        m_miny;
    double        m_maxx;
    double        m_maxy;
    bool          m_validbbox;
};

#endif
