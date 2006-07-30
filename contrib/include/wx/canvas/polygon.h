/////////////////////////////////////////////////////////////////////////////
// Name:        polygon.h
// Author:      Klaas Holwerda
// Created:     XX/XX/XX
// Copyright:   2000 (c) Klaas Holwerda
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WXPOLYGON_H__
#define __WXPOLYGON_H__

#ifdef __GNUG__
    #pragma interface "polygon.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/matrix.h"
#include "wx/geometry.h"
#include "bbox.h"
#include "canvas.h"

enum INOUTPOLY {OUTSIDE_POLY,INSIDE_POLY,ON_POLY};

//----------------------------------------------------------------------------
// wxCanvasPolygon
//----------------------------------------------------------------------------

class wxCanvasPolygon: public wxCanvasObject
{
public:
    wxCanvasPolygon( int n, wxPoint2DDouble points[],  bool spline = FALSE );
    ~wxCanvasPolygon();
    void SetBrush( const wxBrush& brush)  { m_brush = brush; }
    void SetPen( const wxPen& pen)        { m_pen = pen; CalcBoundingBox(); }

    //set colour 1
    //being the background color if filling with a monochrome bitmap
    //or in case of gradient filling the starting colour for the fill
    void SetColour1( const wxColour& fg) { m_textfg=fg;}
    //set colour 1
    //being the foreground color if filling with a monochrome bitmap
    //or in case of gradient filling the ending colour for the fill
    void SetColour2( const wxColour& bg) { m_textbg=bg;}
    //transparent filling when bitmapbrush is monochrome
    void SetTransParent(bool transp) { m_transp=transp;}
    //gradient filling using lines chnging in colour from colour1 to colour2
    void SetGradient(bool gradient, const wxPen& gpen, double distance)
    { m_gradient=gradient;
      m_gpen=gpen;
      m_gdistance=distance;
    }

    double  GetPosX() { return m_points[0].m_x; }
    double  GetPosY() { return m_points[0].m_y; }
    void    SetPosXY( double x, double y);

    void TransLate( double x, double y );
    void CalcBoundingBox();

    virtual void Render(wxTransformMatrix* cworld, int clip_x, int clip_y, int clip_width, int clip_height );
    virtual void WriteSVG( wxTextOutputStream &stream );

    wxCanvasObject* IsHitWorld( double x, double y, double margin = 0 );

    INOUTPOLY PointInPolygon(const wxPoint2DDouble& P, double marge);

private:

    bool MoveUp(double horline, int& index, int direction);
    void DetectCriticalPoints();
    void FillPolygon(wxTransformMatrix* cworld, int clip_x, int clip_y, int clip_width, int clip_height );
    wxList m_CRlist;
    wxList m_AETlist;


    wxBrush       m_brush;
    wxPen         m_pen;
    wxColour      m_textbg;
    wxColour      m_textfg;

    //if brush is of type bitmap with a mask fill with mask transparent
    bool          m_transp;
    bool          m_gradient;
    wxPen         m_gpen;
    double        m_gdistance;

    bool          m_spline;

    int           m_n;
    wxPoint2DDouble* m_points;
};

//----------------------------------------------------------------------------
// wxCanvasPolyline
//----------------------------------------------------------------------------

class wxCanvasPolyline: public wxCanvasObject
{
public:
    wxCanvasPolyline(int n, wxPoint2DDouble points[]);
    ~wxCanvasPolyline();
    void SetPen( const wxPen& pen)    { m_pen = pen; CalcBoundingBox(); }

    double  GetPosX() { return m_points[0].m_x; }
    double  GetPosY() { return m_points[0].m_y; }
    void    SetPosXY( double x, double y);

    void TransLate( double x, double y );
    void CalcBoundingBox();

    virtual void Render(wxTransformMatrix* cworld, int clip_x, int clip_y, int clip_width, int clip_height );
    virtual void WriteSVG( wxTextOutputStream &stream );

    wxCanvasObject* IsHitWorld( double x, double y, double margin = 0 );

    bool PointOnPolyline(const wxPoint2DDouble& P, double marge);

private:

    wxPen         m_pen;

    int           m_n;
    wxPoint2DDouble* m_points;
};

//----------------------------------------------------------------------------
// wxCanvasPolygon
//----------------------------------------------------------------------------

class wxCanvasPolygonL: public wxCanvasObject
{
public:
    wxCanvasPolygonL(wxList* points, bool spline = FALSE);
    ~wxCanvasPolygonL();
    void SetBrush( const wxBrush& brush)  { m_brush = brush; }
    void SetPen( const wxPen& pen)        { m_pen = pen; CalcBoundingBox(); }
    void SetColour1( const wxColour& fg) { m_textfg=fg;}
    void SetColour2( const wxColour& bg) { m_textbg=bg;}
    void SetTransParent(bool transp) { m_transp=transp;}

    double  GetPosX();
    double  GetPosY();
    void    SetPosXY( double x, double y);
    void TransLate( double x, double y );
    void CalcBoundingBox();

    virtual void Render(wxTransformMatrix* cworld, int clip_x, int clip_y, int clip_width, int clip_height );
    virtual void WriteSVG( wxTextOutputStream &stream );

    wxCanvasObject* IsHitWorld( double x, double y, double margin = 0 );

    INOUTPOLY PointInPolygon(const wxPoint2DDouble& P, double marge);

private:

    wxBrush       m_brush;
    wxPen         m_pen;
    bool          m_spline;
    wxColour      m_textbg;
    wxColour      m_textfg;
    //if brush is of type bitmap with a mask fill with mask transparent
    bool          m_transp;

    wxList*       m_lpoints;
};

//----------------------------------------------------------------------------
// wxCanvasPolyline
//----------------------------------------------------------------------------

class wxCanvasPolylineL: public wxCanvasObject
{
public:
    wxCanvasPolylineL(wxList* points, bool spline );
    ~wxCanvasPolylineL();
    void SetPen( const wxPen& pen)    { m_pen = pen; CalcBoundingBox(); }

    double  GetPosX();
    double  GetPosY();
    void    SetPosXY( double x, double y);

    void TransLate( double x, double y );
    void CalcBoundingBox();

    virtual void Render(wxTransformMatrix* cworld, int clip_x, int clip_y, int clip_width, int clip_height );
    virtual void WriteSVG( wxTextOutputStream &stream );

    wxCanvasObject* IsHitWorld( double x, double y, double margin = 0 );

    bool PointOnPolyline(const wxPoint2DDouble& P, double marge);

private:
    wxPen         m_pen;
    bool          m_spline;

    wxList*       m_lpoints;
};

//active edge table
class wxAET
{
public:
    inline void CalculateLineParameters( const wxPoint2DDouble& p1 , const wxPoint2DDouble& p2 );
    inline void CalculateXs( double y );

    //line paramters
    bool m_horizontal;
    double m_BdivA;
    double m_CdivA;
    int m_index;
    int m_direction;
    //intersection point with scanline;
    double m_xs;
};

#endif

