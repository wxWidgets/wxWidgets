/////////////////////////////////////////////////////////////////////////////
// Name:        liner.h
// Author:      Klaas Holwerda
// Created:     1/10/2000
// Copyright:   2000 (c) Klaas Holwerda
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////
#ifndef __WXLINER_H
#define __WXLINER_H

#ifdef __GNUG__
    #pragma interface "liner.cpp"
#endif


#include "wx/geometry.h"

enum OUTPRODUCT {R_IS_LEFT,R_IS_ON,R_IS_RIGHT};

// Status of a point to a wxLine
enum R_PointStatus {R_LEFT_SIDE, R_RIGHT_SIDE, R_ON_AREA, R_IN_AREA};

class wxLine
{
public:
    // constructors and destructor
    wxLine( double x1, double y1, double x2, double y2 );
    wxLine( const wxPoint2DDouble& a, const wxPoint2DDouble& b);
    ~wxLine();

    wxPoint2DDouble GetBeginPoint();                               // Get the beginpoint from a wxLine
    wxPoint2DDouble GetEndPoint();                                 // Get the endpoint from a wxLine
    bool            CheckIntersect( wxLine&, double Marge); // Check if two wxLines intersects
    int             Intersect( wxLine&,  wxPoint2DDouble& bp ,wxPoint2DDouble& ep ,double Marge) ;   // Intersects two wxLines
    bool            Intersect( wxLine& lijn, wxPoint2DDouble& crossing); //intersect two (infinit) lines
    R_PointStatus   PointOnLine( const wxPoint2DDouble& a_Point, double& Distance, double Marge ); //For an infinite wxLine
    R_PointStatus   PointInLine( const wxPoint2DDouble& a_Point, double& Distance, double Marge ); //For a non-infinite wxLine
    OUTPRODUCT      OutProduct( const wxLine& two, double accur);              // outproduct of two wxLines
    double          Calculate_Y( double X);                                 // Caclulate Y if X is known
    void            Virtual_Point( wxPoint2DDouble& a_point, double distance) const;
    wxLine&         operator=( const wxLine&);                              // assignment operator
    void            CalculateLineParameters();                                  // Calculate the parameters if nessecary
    void            OffsetContour( const wxLine& nextline, double factor,wxPoint2DDouble& offsetpoint) const;

private:

    int   ActionOnTable1(R_PointStatus,R_PointStatus);                          // Function needed for Intersect
    int   ActionOnTable2(R_PointStatus,R_PointStatus);                          // Function needed for Intersect

    double      m_AA;
    double      m_BB;
    double      m_CC;
    wxPoint2DDouble m_a;
    wxPoint2DDouble m_b;
    bool        m_valid_parameters;
};

#endif
