/////////////////////////////////////////////////////////////////////////////
// Name:        polygon.cpp
// Author:      Klaas Holwerda
// Created:     XX/XX/XX
// Copyright:   2000 (c) Klaas Holwerda
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "polygon.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/canvas/polygon.h"
#include "wx/canvas/liner.h"

void ConvertSplinedPolygon(wxList* list, double Aber);
void ConvertSplinedPolyline(wxList* list,double Aber);
void ConvertSplinedPolygon(int& n, wxPoint2DDouble* points[], double Aber);
static void GetLRO(const wxPoint2DDouble& P, const wxPoint2DDouble& p1, const wxPoint2DDouble& p2, int &LRO1, int &LRO2,const double marge);

//----------------------------------------------------------------------------
// wxCanvasPolyline
//----------------------------------------------------------------------------

wxCanvasPolyline::wxCanvasPolyline( int n,  wxPoint2DDouble points[])
   : wxCanvasObject()
{
    m_n = n;
    m_points = points;
    m_pen = *wxBLACK_PEN;

    CalcBoundingBox();
}

wxCanvasPolyline::~wxCanvasPolyline()
{
    delete m_points;
}

void wxCanvasPolyline::SetPosXY( double x, double y)
{
    double xo=m_points[0].m_x;
    double yo=m_points[0].m_y;
    int i;
    for (i=0; i < m_n;i++)
    {
        m_points[i].m_x += (x-xo);
        m_points[i].m_y += (y-yo);
    }
    CalcBoundingBox();
}

void wxCanvasPolyline::TransLate( double x, double y )
{
    int i;
    for (i=0; i < m_n;i++)
    {
        m_points[i].m_x += x;
        m_points[i].m_y += y;
    }
    CalcBoundingBox();
}

void wxCanvasPolyline::CalcBoundingBox()
{
    m_bbox.SetValid(FALSE);

    int i;
    for (i=0; i < m_n;i++)
    {
        m_bbox.Expand( m_points[i].m_x,m_points[i].m_y);
    }

    //include the pen width also
    m_bbox.EnLarge(m_pen.GetWidth());
}

void wxCanvasPolyline::Render(wxTransformMatrix* cworld, int clip_x, int clip_y, int clip_width, int clip_height )
{
    if (!m_visible) return;

    int start_y = clip_y;
    int end_y = clip_y+clip_height;

    int start_x = clip_x;
    int end_x = clip_x+clip_width;

#if IMAGE_CANVAS
#else
    wxPoint *cpoints = new wxPoint[m_n];
    int i;
    for (i = 0; i < m_n; i++)
    {
        double x1;
        double y1;
        //transform to absolute
        cworld->TransformPoint( m_points[i].m_x, m_points[i].m_y, x1, y1 );
        //transform to device
        cpoints[i].x = m_admin->LogicalToDeviceX(x1);
        cpoints[i].y = m_admin->LogicalToDeviceY(y1);
    }
    wxDC *dc = m_admin->GetActive()->GetDC();
    dc->SetClippingRegion(start_x,start_y,end_x-start_x,end_y-start_y);
    int pw=m_pen.GetWidth();
    m_pen.SetWidth(m_admin->LogicalToDeviceXRel(pw));
    dc->SetPen(m_pen);
    dc->DrawLines(m_n, cpoints, 0,0);
    delete [] cpoints;
    dc->SetPen(wxNullPen);
    dc->DestroyClippingRegion();
    m_pen.SetWidth(pw);
#endif
}

void wxCanvasPolyline::WriteSVG( wxTextOutputStream &stream )
{
}

wxCanvasObject* wxCanvasPolyline::IsHitWorld( double x, double y, double margin )
{
    if ((x >= m_bbox.GetMinX()-margin) &&
        (x <= m_bbox.GetMaxX()+margin) &&
        (y >= m_bbox.GetMinY()-margin) &&
        (y <= m_bbox.GetMaxY()+margin)
       )
    {
        wxPoint2DDouble P=wxPoint2DDouble(x,y);
        if (PointOnPolyline(P,m_pen.GetWidth()/2+margin))
            return this;
        else
            return (wxCanvasObject*) NULL;
    }
    return (wxCanvasObject*) NULL;
}

bool wxCanvasPolyline::PointOnPolyline(const wxPoint2DDouble& P, double margin)
{
    bool    result = FALSE;
    double  distance;
    wxPoint2DDouble p1,p2;

    p2=m_points[0];
    int i;
    for (i = 0; i < m_n-1; i++)
    {
        p1=p2;
        p2=m_points[i+1];
        if (margin > sqrt(pow(p1.m_x-P.m_x,2)+pow(p1.m_y-P.m_y,2)))
        {
            result=TRUE;
            break;
        }
        else if (!((p1.m_x == p2.m_x) && (p1.m_y == p2.m_y)))
        {
            wxLine line1(p1,p2);
            if (line1.PointInLine(P,distance,margin) == R_IN_AREA)
            {
                result=TRUE;
                break;
            }
        }
    }

    return result;
}

//----------------------------------------------------------------------------
// wxCanvasPolygon
//----------------------------------------------------------------------------

wxCanvasPolygon::wxCanvasPolygon( int n, wxPoint2DDouble points[],bool splined)
   : wxCanvasObject()
{
    m_n = n;
    m_points = points;
    m_brush = *wxBLACK_BRUSH;
    m_pen = *wxTRANSPARENT_PEN;
    m_textfg=*wxBLACK;
    m_textbg=*wxWHITE;
    m_transp=FALSE;
    m_gpen=*wxBLACK_PEN;
    m_gdistance=0;
    m_gradient=FALSE;
    m_spline = splined;

    if (m_spline)
    {
        ConvertSplinedPolygon(m_n, &m_points, 10 );
    }

    CalcBoundingBox();
}

wxCanvasPolygon::~wxCanvasPolygon()
{
    delete m_points;
}

void wxCanvasPolygon::SetPosXY( double x, double y)
{
    double xo=m_points[0].m_x;
    double yo=m_points[0].m_y;
    int i;
    for (i=0; i < m_n;i++)
    {
        m_points[i].m_x += (x-xo);
        m_points[i].m_y += (y-yo);
    }
    CalcBoundingBox();
}

void wxCanvasPolygon::TransLate( double x, double y )
{
    int i;
    for (i=0; i < m_n;i++)
    {
        m_points[i].m_x += x;
        m_points[i].m_y += y;
    }
    CalcBoundingBox();
}

void wxCanvasPolygon::CalcBoundingBox()
{

    m_bbox.SetValid(FALSE);

    int i;
    for (i=0; i < m_n;i++)
    {
        m_bbox.Expand( m_points[i].m_x,m_points[i].m_y);
    }

    //include the pen width also
    m_bbox.EnLarge(m_pen.GetWidth());
}

void wxCanvasPolygon::Render(wxTransformMatrix* cworld, int clip_x, int clip_y, int clip_width, int clip_height )
{
    if (!m_visible) return;

    int start_y = clip_y;
    int end_y = clip_y+clip_height;

    int start_x = clip_x;
    int end_x = clip_x+clip_width;

#if IMAGE_CANVAS
#else
    //one extra for drawlines in some cases
    wxPoint *cpoints = new wxPoint[m_n+1];
    int i;
    for (i = 0; i < m_n; i++)
    {
        double x1;
        double y1;
        cworld->TransformPoint( m_points[i].m_x, m_points[i].m_y, x1, y1 );
        cpoints[i].x = m_admin->LogicalToDeviceX(x1);
        cpoints[i].y = m_admin->LogicalToDeviceY(y1);
    }
    double x1;
    double y1;
    cworld->TransformPoint( m_points[0].m_x, m_points[0].m_y, x1, y1 );
    cpoints[m_n].x = m_admin->LogicalToDeviceX(x1);
    cpoints[m_n].y = m_admin->LogicalToDeviceY(y1);

    wxDC *dc = m_admin->GetActive()->GetDC();
    dc->SetClippingRegion(start_x,start_y,end_x-start_x,end_y-start_y);
    dc->SetBrush(m_brush);
    int pw=m_pen.GetWidth();
    m_pen.SetWidth(m_admin->LogicalToDeviceXRel(pw));
    if ( m_brush.GetStyle() == wxSTIPPLE_MASK_OPAQUE && m_transp)
    {
        //draw a transparent polygon
        //leaf the pen not transparent, which i prefer
        dc->SetPen( wxPen( *wxWHITE,m_admin->LogicalToDeviceXRel(pw), wxSOLID) );
        dc->SetTextForeground(*wxBLACK);
        dc->SetTextBackground(*wxWHITE);
        dc->SetLogicalFunction(wxAND_INVERT);
        // BLACK OUT the opaque pixels and leave the rest as is
        dc->DrawPolygon(m_n, cpoints, 0,0,wxWINDING_RULE);
        // Set background and foreground colors for fill pattern
        //the previous blacked out pixels are now merged with the layer color
        //while the non blacked out pixels stay as they are.
        dc->SetTextForeground(*wxBLACK);
        //now define what will be the color of the fillpattern parts that are not transparent
        dc->SetTextBackground(m_textfg);
        dc->SetLogicalFunction(wxOR);
        //don't understand how but the outline is also depending on logicalfunction
        dc->SetPen(m_pen);
        dc->DrawPolygon(m_n, cpoints, 0,0,wxWINDING_RULE);
    }
    else if (m_gradient)
    {
        int pw2=m_gpen.GetWidth();
        m_gpen.SetWidth(m_admin->LogicalToDeviceYRel(pw2));
        FillPolygon(cworld,clip_x,clip_y,clip_width,clip_height );
        if (m_pen.GetStyle() != wxTRANSPARENT)
        {
            dc->SetPen(m_pen);
            dc->DrawLines(m_n+1, cpoints, 0,0);
        }
        m_gpen.SetWidth(pw2);
    }
    else
    {
        dc->SetPen(m_pen);
        dc->DrawPolygon(m_n, cpoints, 0,0,wxWINDING_RULE);
    }
    delete [] cpoints;
    dc->SetBrush(wxNullBrush);
    dc->SetPen(wxNullPen);
    dc->DestroyClippingRegion();
    m_pen.SetWidth(pw);
#endif
}

void wxCanvasPolygon::WriteSVG( wxTextOutputStream &stream )
{
}

wxCanvasObject* wxCanvasPolygon::IsHitWorld( double x, double y, double margin )
{
    if ((x >= m_bbox.GetMinX()-margin) &&
        (x <= m_bbox.GetMaxX()+margin) &&
        (y >= m_bbox.GetMinY()-margin) &&
        (y <= m_bbox.GetMaxY()+margin)
       )
    {
        wxPoint2DDouble P=wxPoint2DDouble(x,y);
        INOUTPOLY io=PointInPolygon(P, m_pen.GetWidth()/2+margin);
        if (io == OUTSIDE_POLY)
            return (wxCanvasObject*) NULL;
        else
            return this;
    }
    return (wxCanvasObject*) NULL;
}

INOUTPOLY wxCanvasPolygon::PointInPolygon(const wxPoint2DDouble& P, double marge)
{
    int     R_tot = 0, L_tot = 0;
    int     p1_LRO, p2_LRO;
    double  px = P.m_x, py = P.m_y;
    double  Y_intersect;
    wxPoint2DDouble p1,p2;

    //iterate across points until we are sure that the given point is in or out
    int i;
    for (i = 0; i < m_n; i++)
    {
        p1 = m_points[i];
        if (i == m_n-1)
            p2 = m_points[0];
        else
            p2 = m_points[i+1];

        //more accurate
        GetLRO(P,p1,p2,p1_LRO,p2_LRO,marge/10);
        if (p1_LRO != p2_LRO)
        {
            int L = 0, R = 0;
            if (p2_LRO == -1) { R = -p1_LRO; L = 1; }
            if (p2_LRO == 0) if (p1_LRO == 1) R = -1; else L = -1;
            if (p2_LRO == 1) { R = 1; L = p1_LRO; }

            // calculate intersection point with line for px
            if (p1_LRO == 0)
            {
                if ((p1.m_y < (py + marge)) && (p1.m_y > (py - marge)))
                    return ON_POLY;
                else
                    Y_intersect = p1.m_y;
            }
            else if (p2_LRO == 0)
            {
                if ((p2.m_y < (py + marge)) && (p2.m_y > (py - marge)))
                    return ON_POLY;
                else
                    Y_intersect = p2.m_y;
            }
            else //both p2_LRO and p1_LRO not 0
            {
                if ((p1.m_y > (py + marge)) && (p2.m_y > (py + marge)))
                    Y_intersect = p1.m_y; //a save value to check later
                else if ((p1.m_y < (py- marge)) && (p2.m_y < (py - marge)))
                    Y_intersect = p1.m_y; //a save value to check later
                else //need to calculate intersection
                {
                    if (!((p1.m_x == p2.m_x) && (p1.m_y == p2.m_y)))
                    {
                        wxLine line1(p1,p2);
                        line1.CalculateLineParameters();
                        Y_intersect = line1.Calculate_Y(px);
                    }
                    else
                         continue;
                }
            }
            if (Y_intersect > (py + marge))
            {
                R_tot += R;
                L_tot += L;
            }
            else if ((Y_intersect <= (py + marge)) && (Y_intersect >= (py - marge)))
            {
               return ON_POLY;
            }
        }
    }

    // geef het juiste resultaat terug
    if (R_tot == 0)
        if (L_tot == 0) return OUTSIDE_POLY;
        else return ON_POLY;
    else
        if (L_tot == 0) return ON_POLY;
        else return INSIDE_POLY;
}

//----------------------------------------------------------------------------
// wxCanvasPolylineL
//----------------------------------------------------------------------------

wxCanvasPolylineL::wxCanvasPolylineL( wxList* points, bool spline )
   : wxCanvasObject()
{
    m_lpoints = points;
    m_pen = *wxBLACK_PEN;
    m_spline=spline;
    if (m_spline)
        ConvertSplinedPolyline(m_lpoints, 10);
    CalcBoundingBox();
}

wxCanvasPolylineL::~wxCanvasPolylineL()
{
    m_lpoints->DeleteContents(TRUE);
    delete m_lpoints;
}

double wxCanvasPolylineL::GetPosX()
{
    wxNode *node = m_lpoints->GetFirst();
    wxPoint2DDouble* point = (wxPoint2DDouble*)node->Data();
    return point->m_x;
}

double wxCanvasPolylineL::GetPosY()
{
    wxNode *node = m_lpoints->GetFirst();
    wxPoint2DDouble* point = (wxPoint2DDouble*)node->Data();
    return point->m_y;
}

void wxCanvasPolylineL::SetPosXY( double x, double y )
{
    wxNode *node = m_lpoints->GetFirst();
    wxPoint2DDouble* point = (wxPoint2DDouble*)node->Data();
    double xo=point->m_x;
    double yo=point->m_y;
    while (node)
    {
        point = (wxPoint2DDouble*)node->Data();
        point->m_x = point->m_x + x-xo;
        point->m_y = point->m_y + y-yo;
        node = node->GetNext();
    }
    CalcBoundingBox();
}

void wxCanvasPolylineL::TransLate( double x, double y )
{
    wxNode *node = m_lpoints->GetFirst();
    while (node)
    {
        wxPoint2DDouble* point = (wxPoint2DDouble*)node->Data();
        point->m_x += x;
        point->m_y += y;
        node = node->GetNext();
    }
    CalcBoundingBox();
}

void wxCanvasPolylineL::CalcBoundingBox()
{
    m_bbox.SetValid(FALSE);

    wxNode *node = m_lpoints->GetFirst();
    while (node)
    {
        wxPoint2DDouble* point = (wxPoint2DDouble*)node->Data();
        m_bbox.Expand( point->m_x,point->m_y);
        node = node->GetNext();
    }

    //include the pen width also
    m_bbox.EnLarge(m_pen.GetWidth());
}

void wxCanvasPolylineL::Render(wxTransformMatrix* cworld, int clip_x, int clip_y, int clip_width, int clip_height )
{
    if (!m_visible) return;

    int start_y = clip_y;
    int end_y = clip_y+clip_height;

    int start_x = clip_x;
    int end_x = clip_x+clip_width;

#if IMAGE_CANVAS
#else
    int n=m_lpoints->GetCount();
    wxPoint *cpoints = new wxPoint[n];

    wxNode *node = m_lpoints->GetFirst();
    int i=0;
    while (node)
    {
        wxPoint2DDouble* point = (wxPoint2DDouble*)node->Data();
        double x1;
        double y1;
        //transform to absolute
        cworld->TransformPoint( point->m_x, point->m_y, x1, y1 );
        //transform to device
        cpoints[i].x = m_admin->LogicalToDeviceX(x1);
        cpoints[i].y = m_admin->LogicalToDeviceY(y1);

        node = node->GetNext();
        i++;
    }

    wxDC *dc = m_admin->GetActive()->GetDC();
    dc->SetClippingRegion(start_x,start_y,end_x-start_x,end_y-start_y);
    int pw=m_pen.GetWidth();
    m_pen.SetWidth(m_admin->LogicalToDeviceXRel(pw));
    dc->SetPen(m_pen);
    dc->DrawLines(n, cpoints, 0,0);
    delete [] cpoints;
    dc->SetPen(wxNullPen);
    dc->DestroyClippingRegion();
    m_pen.SetWidth(pw);
#endif
}

void wxCanvasPolylineL::WriteSVG( wxTextOutputStream &stream )
{
}

wxCanvasObject* wxCanvasPolylineL::IsHitWorld( double x, double y, double margin )
{
    if ((x >= m_bbox.GetMinX()-margin) &&
        (x <= m_bbox.GetMaxX()+margin) &&
        (y >= m_bbox.GetMinY()-margin) &&
        (y <= m_bbox.GetMaxY()+margin)
       )
    {
        wxPoint2DDouble P=wxPoint2DDouble(x,y);
        if (PointOnPolyline(P,m_pen.GetWidth()/2+margin))
            return this;
        else
            return (wxCanvasObject*) NULL;
    }
    return (wxCanvasObject*) NULL;
}

bool wxCanvasPolylineL::PointOnPolyline(const wxPoint2DDouble& P, double margin)
{
    bool    result = FALSE;
    double  distance;
    wxPoint2DDouble p1,p2;

    wxNode *node = m_lpoints->GetFirst();
    p2 = *(wxPoint2DDouble*)node->Data();
    while (node && !result)
    {
        p1=p2;
        node=node->GetNext();
        if (!node) break;
        p2 = *(wxPoint2DDouble*)node->Data();

        if (margin > sqrt(pow(p1.m_x-P.m_x,2)+pow(p1.m_y-P.m_y,2)))
            result=TRUE;
        else if (!((p1.m_x == p2.m_x) && (p1.m_y == p2.m_y)))
        {
            wxLine line1(p1,p2);
            if (line1.PointInLine(P,distance,margin) == R_IN_AREA)
            result=TRUE;
        }
    }

    return result;
}

//----------------------------------------------------------------------------
// wxCanvasPolygon
//----------------------------------------------------------------------------

wxCanvasPolygonL::wxCanvasPolygonL( wxList* points, bool spline )
   : wxCanvasObject()
{
    m_lpoints = points;
    m_brush = *wxBLACK_BRUSH;
    m_pen = *wxTRANSPARENT_PEN;
    m_spline=spline;
    m_textfg=*wxBLACK;
    m_textbg=*wxWHITE;
    m_transp=FALSE;

    if (m_spline)
        ConvertSplinedPolyline(m_lpoints, 10);
    CalcBoundingBox();
}

wxCanvasPolygonL::~wxCanvasPolygonL()
{
    m_lpoints->DeleteContents(TRUE);
    delete m_lpoints;
}

double wxCanvasPolygonL::GetPosX()
{
    wxNode *node = m_lpoints->GetFirst();
    wxPoint2DDouble* point = (wxPoint2DDouble*)node->Data();
    return point->m_x;
}

double wxCanvasPolygonL::GetPosY()
{
    wxNode *node = m_lpoints->GetFirst();
    wxPoint2DDouble* point = (wxPoint2DDouble*)node->Data();
    return point->m_y;
}

void wxCanvasPolygonL::SetPosXY( double x, double y )
{
    wxNode *node = m_lpoints->GetFirst();
    wxPoint2DDouble* point = (wxPoint2DDouble*)node->Data();
    double xo=point->m_x;
    double yo=point->m_y;
    while (node)
    {
        point = (wxPoint2DDouble*)node->Data();
        point->m_x = point->m_x + x-xo;
        point->m_y = point->m_y + y-yo;
        node = node->GetNext();
    }
    CalcBoundingBox();
}

void wxCanvasPolygonL::TransLate( double x, double y )
{
    wxNode *node = m_lpoints->GetFirst();
    while (node)
    {
        wxPoint2DDouble* point = (wxPoint2DDouble*)node->Data();
        point->m_x += x;
        point->m_y += y;
        node = node->GetNext();
    }
    CalcBoundingBox();
}

void wxCanvasPolygonL::CalcBoundingBox()
{

    m_bbox.SetValid(FALSE);

    wxNode *node = m_lpoints->GetFirst();
    while (node)
    {
        wxPoint2DDouble* point = (wxPoint2DDouble*)node->Data();
        m_bbox.Expand( point->m_x,point->m_y);
        node = node->GetNext();
    }

    //include the pen width also
    m_bbox.EnLarge(m_pen.GetWidth());
}

void wxCanvasPolygonL::Render(wxTransformMatrix* cworld, int clip_x, int clip_y, int clip_width, int clip_height )
{
    if (!m_visible) return;

    int start_y = clip_y;
    int end_y = clip_y+clip_height;

    int start_x = clip_x;
    int end_x = clip_x+clip_width;

#if IMAGE_CANVAS
#else
    int n=m_lpoints->GetCount();
    wxPoint *cpoints = new wxPoint[n];

    wxNode *node = m_lpoints->GetFirst();
    int i=0;
    while (node)
    {
        wxPoint2DDouble* point = (wxPoint2DDouble*)node->Data();
        double x1;
        double y1;
        //transform to absolute
        cworld->TransformPoint( point->m_x, point->m_y, x1, y1 );
        //transform to device
        cpoints[i].x = m_admin->LogicalToDeviceX(x1);
        cpoints[i].y = m_admin->LogicalToDeviceY(y1);

        node = node->GetNext();
        i++;
    }
    wxDC *dc = m_admin->GetActive()->GetDC();
    dc->SetClippingRegion(start_x,start_y,end_x-start_x,end_y-start_y);
    dc->SetBrush(m_brush);
    int pw=m_pen.GetWidth();
    m_pen.SetWidth(m_admin->LogicalToDeviceXRel(pw));
    if ( m_brush.GetStyle() == wxSTIPPLE_MASK_OPAQUE && m_transp)
    {
        //draw a transparent polygon
        //leaf the pen not transparent, which i prefer
        dc->SetPen( wxPen( *wxWHITE,m_admin->LogicalToDeviceXRel(pw), wxSOLID) );
        dc->SetTextForeground(*wxBLACK);
        dc->SetTextBackground(*wxWHITE);
        dc->SetLogicalFunction(wxAND_INVERT);
        // BLACK OUT the opaque pixels and leave the rest as is
        dc->DrawPolygon(n, cpoints, 0,0,wxWINDING_RULE);
        // Set background and foreground colors for fill pattern
        //the previous blacked out pixels are now merged with the layer color
        //while the non blacked out pixels stay as they are.
        dc->SetTextForeground(*wxBLACK);
        //now define what will be the color of the fillpattern parts that are not transparent
        dc->SetTextBackground(m_textfg);
        dc->SetLogicalFunction(wxOR);
        //don't understand how but the outline is also depending on logicalfunction
        dc->SetPen(m_pen);
        dc->DrawPolygon(n, cpoints, 0,0,wxWINDING_RULE);
        dc->SetLogicalFunction(wxCOPY);
    }
    else
    {
        dc->SetPen(m_pen);
        dc->SetTextForeground(m_textfg);
        dc->SetTextBackground(m_textbg);
        dc->DrawPolygon(n, cpoints, 0,0,wxWINDING_RULE);
    }
    delete [] cpoints;
    dc->SetBrush(wxNullBrush);
    dc->SetPen(wxNullPen);
    dc->DestroyClippingRegion();
    m_pen.SetWidth(pw);
#endif
}

void wxCanvasPolygonL::WriteSVG( wxTextOutputStream &stream )
{
}

static void GetLRO(const wxPoint2DDouble& P, const wxPoint2DDouble& p1, const wxPoint2DDouble& p2, int &LRO1, int &LRO2,const double marge)
{
    if (p1.m_x > (P.m_x + marge)) LRO1 = -1;        // beginnode is right of P
    else
        if (p1.m_x < (P.m_x - marge)) LRO1 = 1;     // beginnode is left of P
        else LRO1 = 0;                              // beginnode is on vertical line through P

    if (p2.m_x > (P.m_x + marge)) LRO2 = -1;        // endnode is right of P
    else
        if (p2.m_x < (P.m_x - marge)) LRO2 = 1;     // endnode is left of P
        else LRO2 = 0;                              // endnode is on vertical line through P
}

wxCanvasObject* wxCanvasPolygonL::IsHitWorld( double x, double y, double margin )
{
    if ((x >= m_bbox.GetMinX()-margin) &&
        (x <= m_bbox.GetMaxX()+margin) &&
        (y >= m_bbox.GetMinY()-margin) &&
        (y <= m_bbox.GetMaxY()+margin)
       )
    {
        wxPoint2DDouble P=wxPoint2DDouble(x,y);
        INOUTPOLY io=PointInPolygon(P,m_pen.GetWidth()/2 + margin);
        if (io == OUTSIDE_POLY)
            return (wxCanvasObject*) NULL;
        else
            return this;
    }
    return (wxCanvasObject*) NULL;
}

INOUTPOLY wxCanvasPolygonL::PointInPolygon(const wxPoint2DDouble& P, double marge)
{
    int     R_tot = 0, L_tot = 0;
    int     p1_LRO, p2_LRO;
    double  px = P.m_x, py = P.m_y;
    double  Y_intersect;
    wxPoint2DDouble p1,p2;

    //iterate across points until we are sure that the given point is in or out
    wxNode *node = m_lpoints->GetFirst();

    while (node)
    {
        p1 = *(wxPoint2DDouble*)node->Data();
        if (m_lpoints->GetLast() == node)
        {
            p2 = *(wxPoint2DDouble*)m_lpoints->GetFirst();
        }
        else
        {
            p2 = *(wxPoint2DDouble*)node->GetNext()->Data();
        }

        //more accurate
        GetLRO(P,p1,p2,p1_LRO,p2_LRO,marge/10);
        if (p1_LRO != p2_LRO)
        {
            int L = 0, R = 0;
            if (p2_LRO == -1) { R = -p1_LRO; L = 1; }
            if (p2_LRO == 0) if (p1_LRO == 1) R = -1; else L = -1;
            if (p2_LRO == 1) { R = 1; L = p1_LRO; }

            // calculate intersection point with line for px
            if (p1_LRO == 0)
            {
                if ((p1.m_y < (py + marge)) && (p1.m_y > (py - marge)))
                    return ON_POLY;
                else
                    Y_intersect = p1.m_y;
            }
            else if (p2_LRO == 0)
            {
                if ((p2.m_y < (py + marge)) && (p2.m_y > (py - marge)))
                    return ON_POLY;
                else
                    Y_intersect = p2.m_y;
            }
            else //both p2_LRO and p1_LRO not 0
            {
                if ((p1.m_y > (py + marge)) && (p2.m_y > (py + marge)))
                    Y_intersect = p1.m_y; //a save value to check later
                else if ((p1.m_y < (py- marge)) && (p2.m_y < (py - marge)))
                    Y_intersect = p1.m_y; //a save value to check later
                else //need to calculate intersection
                {
                    if (!((p1.m_x == p2.m_x) && (p1.m_y == p2.m_y)))
                    {
                        wxLine line1(p1,p2);
                        line1.CalculateLineParameters();
                        Y_intersect = line1.Calculate_Y(px);
                    }
                    else
                         continue;
                }
            }
            if (Y_intersect > (py + marge))
            {
                R_tot += R;
                L_tot += L;
            }
            else if ((Y_intersect <= (py + marge)) && (Y_intersect >= (py - marge)))
            {
               return ON_POLY;
            }
        }
        node=node->Next();
    }

    // geef het juiste resultaat terug
    if (R_tot == 0)
        if (L_tot == 0) return OUTSIDE_POLY;
        else return ON_POLY;
    else
        if (L_tot == 0) return ON_POLY;
        else return INSIDE_POLY;
}

// ---------------------------------------------------------------------------
// spline drawing code
// ---------------------------------------------------------------------------

static void gds_quadratic_spline(wxList *org,double a1, double b1, double a2, double b2,
                         double a3, double b3, double a4, double b4,double aber);
static void gds_clear_stack();
static int gds_spline_pop(double *x1, double *y1, double *x2, double *y2, double *x3,
                  double *y3, double *x4, double *y4);
static void gds_spline_push(double x1, double y1, double x2, double y2, double x3, double y3,
                    double x4, double y4);

void ConvertSplinedPolygon(int& n, wxPoint2DDouble* points[], double Aber)
{
    wxList h;
    int i;
    for (i = 0; i < n; i++)
    {
        h.Append((wxObject*) new wxPoint2DDouble((*points)[i].m_x, (*points)[i].m_y));
    }
    delete *points;

    ConvertSplinedPolygon(&h, Aber);

    n=h.GetCount();
    *points = new wxPoint2DDouble[n];
    wxNode* node=h.GetFirst();
    for (i = 0; i < n; i++)
    {
        wxNode* hh= node;
        node = node->GetNext();
        (*points)[i].m_x=((wxPoint2DDouble*) hh->GetData())->m_x;
        (*points)[i].m_y=((wxPoint2DDouble*) hh->GetData())->m_y;
        delete (wxPoint2DDouble*) hh->GetData();
        h.DeleteNode(hh);
    }
}

void ConvertSplinedPolygon(wxList* list, double Aber)
{
    wxPoint2DDouble* point;
    double           cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4;
    double           x1, y1, x2, y2;

    if (list->GetCount() <2)
        return;

    wxNode* iter=list->GetLast();
    x1 = ((wxPoint2DDouble*)iter->Data())->m_x;
    y1 = ((wxPoint2DDouble*)iter->Data())->m_y;

    iter=list->GetFirst();
    x2 = ((wxPoint2DDouble*)iter->Data())->m_x;
    y2 = ((wxPoint2DDouble*)iter->Data())->m_y;

    point = new wxPoint2DDouble(x2,y2);
    list->Append((wxObject*)point);

    cx1 = (x1 + x2) / 2.0;
    cy1 = (y1 + y2) / 2.0;
    cx2 = (cx1 + x2) / 2.0;
    cy2 = (cy1 + y2) / 2.0;

    delete (wxPoint2DDouble*) iter->Data();
    delete iter;
    iter=list->GetFirst();
    x1 = ((wxPoint2DDouble*)iter->Data())->m_x;
    y1 = ((wxPoint2DDouble*)iter->Data())->m_y;
    point = new wxPoint2DDouble(x1,y1);
    list->Append((wxObject*)point);

    int i=1;
    int count=list->GetCount();
    while (i < count)
    {
        x1 = x2;
        y1 = y2;
        x2 = ((wxPoint2DDouble*)iter->Data())->m_x;
        y2 = ((wxPoint2DDouble*)iter->Data())->m_y;
        cx4 = (x1 + x2) / 2.0;
        cy4 = (y1 + y2) / 2.0;
        cx3 = (x1 + cx4) / 2.0;
        cy3 = (y1 + cy4) / 2.0;

        gds_quadratic_spline(list,cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4,Aber);

        cx1 = cx4;
        cy1 = cy4;
        cx2 = (cx1 + x2) / 2.0;
        cy2 = (cy1 + y2) / 2.0;
        delete (wxPoint2DDouble*)iter->Data();
        delete iter;
        iter=list->GetFirst();
        i++;
    }

    iter=list->GetFirst();
    delete (wxPoint2DDouble*)iter->Data();
    delete iter;
}

void ConvertSplinedPolyline(wxList* list,double Aber)
{
    double           cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4;
    double           x1, y1, x2, y2;


    if (list->GetCount() <2)
        return;



    wxNode* iter=list->GetFirst();

    x1 = ((wxPoint2DDouble*)iter->Data())->m_x;
    y1 = ((wxPoint2DDouble*)iter->Data())->m_y;

    delete (wxPoint2DDouble*)iter->Data();
    delete iter;
    iter=list->GetFirst();
    x2 = ((wxPoint2DDouble*)iter->Data())->m_x;
    y2 = ((wxPoint2DDouble*)iter->Data())->m_y;
    cx1 = (x1 + x2) / 2.0;
    cy1 = (y1 + y2) / 2.0;
    cx2 = (cx1 + x2) / 2.0;
    cy2 = (cy1 + y2) / 2.0;

    wxPoint2DDouble* point = new wxPoint2DDouble(x1,y1);
    list->Append((wxObject*)point);

    delete (wxPoint2DDouble*)iter->Data();
    delete iter;
    iter=list->GetFirst();

    int i=1;
    int count=list->GetCount();
    while (i < count)
    {
        x1 = x2;
        y1 = y2;
        x2 = ((wxPoint2DDouble*)iter->Data())->m_x;
        y2 = ((wxPoint2DDouble*)iter->Data())->m_y;
        cx4 = (x1 + x2) / 2.0;
        cy4 = (y1 + y2) / 2.0;
        cx3 = (x1 + cx4) / 2.0;
        cy3 = (y1 + cy4) / 2.0;

        gds_quadratic_spline(list,cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4,Aber);

        cx1 = cx4;
        cy1 = cy4;
        cx2 = (cx1 + x2) / 2.0;
        cy2 = (cy1 + y2) / 2.0;
        delete (wxPoint2DDouble*)iter->Data();
        delete iter;
        iter=list->GetFirst();
        i++;
    }

    point = new wxPoint2DDouble(cx1,cy1);
    list->Append((wxObject*)point);

    point = new wxPoint2DDouble(x2,y2);
    list->Append((wxObject*)point);
}

/********************* CURVES FOR SPLINES *****************************

  The following spline drawing routine is from

    "An Algorithm for High-Speed Curve Generation"
    by George Merrill Chaikin,
    Computer Graphics and Image Processing, 3, Academic Press,
    1974, 346-349.

      and

        "On Chaikin's Algorithm" by R. F. Riesenfeld,
        Computer Graphics and Image Processing, 4, Academic Press,
        1975, 304-310.

***********************************************************************/

#define     half(z1, z2)    ((z1+z2)/2.0)
#define     THRESHOLD   5

/* iterative version */

void gds_quadratic_spline(wxList *org,double a1, double b1, double a2, double b2, double a3, double b3, double a4,
                         double b4,double Aber)
{
    register double  xmid, ymid;
    double           x1, y1, x2, y2, x3, y3, x4, y4;
    wxPoint2DDouble* point;

    gds_clear_stack();
    gds_spline_push(a1, b1, a2, b2, a3, b3, a4, b4);

    while (gds_spline_pop(&x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4))
    {
        xmid = half(x2, x3);
        ymid = half(y2, y3);
        if (fabs(x1 - xmid) < Aber && fabs(y1 - ymid) < Aber &&
            fabs(xmid - x4) < Aber && fabs(ymid - y4) < Aber)
        {
            point = new wxPoint2DDouble(x1,y1);
            org->Append((wxObject*)point);
            point = new wxPoint2DDouble(xmid,ymid);
            org->Append((wxObject*)point);
        } else {
            gds_spline_push(xmid, ymid, half(xmid, x3), half(ymid, y3),
                half(x3, x4), half(y3, y4), x4, y4);
            gds_spline_push(x1, y1, half(x1, x2), half(y1, y2),
                half(x2, xmid), half(y2, ymid), xmid, ymid);
        }
    }
}


/* utilities used by spline drawing routines */


typedef struct gds_spline_stack_struct {
    double           x1, y1, x2, y2, x3, y3, x4, y4;
}
Stack;

#define         SPLINE_STACK_DEPTH             20
static Stack    gds_spline_stack[SPLINE_STACK_DEPTH];
static Stack   *gds_stack_top;
static int      gds_stack_count;

static void gds_clear_stack()
{
    gds_stack_top = gds_spline_stack;
    gds_stack_count = 0;
}

static void gds_spline_push(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{
    gds_stack_top->x1 = x1;
    gds_stack_top->y1 = y1;
    gds_stack_top->x2 = x2;
    gds_stack_top->y2 = y2;
    gds_stack_top->x3 = x3;
    gds_stack_top->y3 = y3;
    gds_stack_top->x4 = x4;
    gds_stack_top->y4 = y4;
    gds_stack_top++;
    gds_stack_count++;
}

int gds_spline_pop(double *x1, double *y1, double *x2, double *y2,
                  double *x3, double *y3, double *x4, double *y4)
{
    if (gds_stack_count == 0)
        return (0);
    gds_stack_top--;
    gds_stack_count--;
    *x1 = gds_stack_top->x1;
    *y1 = gds_stack_top->y1;
    *x2 = gds_stack_top->x2;
    *y2 = gds_stack_top->y2;
    *x3 = gds_stack_top->x3;
    *y3 = gds_stack_top->y3;
    *x4 = gds_stack_top->x4;
    *y4 = gds_stack_top->y4;
    return (1);
}

void wxAET::CalculateLineParameters( const wxPoint2DDouble& p1 , const wxPoint2DDouble& p2 )
{
    double A = p2.m_y - p1.m_y; //A (y2-y1)
    if (A == 0)
    {
        m_horizontal=TRUE;
        m_BdivA=0;
        m_CdivA=0;
    }
    else
    {
        m_horizontal=FALSE;
        m_BdivA= (p1.m_x - p2.m_x)/A; //B (x1-x2)
        //normalize
        m_CdivA= ((p2.m_x*p1.m_y) - (p1.m_x*p2.m_y)) /A ;
    }
}

void wxAET::CalculateXs( double y )
{
    m_xs= -m_BdivA * y - m_CdivA;
}

//use by polygon filling
//moves the scanline up
//index is the index of the point where the search begins
//direction is +1 or -1 and indicates if the segment ascends or decends
bool wxCanvasPolygon::MoveUp( double horline, int& index, int direction)
{
    int walk = (index + direction + m_n) % m_n;
    while ( m_points[walk].m_y < horline )
    {
        if (m_points[walk].m_y < m_points[index].m_y )
            return FALSE;
        else
        {
            //modify index
            index=walk;
            walk = (index + direction + m_n) % m_n;
        }
    }
    return TRUE;
}

//a crictical point is a point between a decending and a ascending segment
//collect those points for filling later
void wxCanvasPolygon::DetectCriticalPoints()
{
    //candidate for critical point
    //true if Y is getting lower, unchanged i Y is unchanged
    //and if Y becomes higher and candidate was true: it is a critical point
    bool candidate = FALSE;
    int i,j;

    for ( i=0; i < m_n; i++)
    {
        //j next point
        j= (i+1) % m_n;

        //check if Y is smaller
        if (m_points[i].m_y > m_points[j].m_y)
            //we have a candidate
            candidate=TRUE;
        else if ( (m_points[i].m_y < m_points[j].m_y) && candidate)
        {   //this is a critical point put in list
            bool inserted=FALSE;
            wxNode *node = m_CRlist.GetFirst();
            while (node)
            {
                //sorted on smallest Y value
                int* ind=(int*) node->GetData();
                if (m_points[*ind].m_y > m_points[i].m_y)
                {
                    m_CRlist.Insert(node,(wxObject*) new int(i));
                    inserted = TRUE;
                    break;
                }
                node = node->GetNext();
            }
            if (!inserted)
                m_CRlist.Append((wxObject*) new int(i));
            candidate = FALSE;
        }
    }
    if (candidate)
    {
        for ( i=0; i < m_n; i++)
        {
            //j next point
            j= (i+1) % m_n;

            //check if Y is smaller
            if (m_points[i].m_y > m_points[j].m_y)
                //we have a candidate
                candidate=TRUE;
            else if ( (m_points[i].m_y < m_points[j].m_y) && candidate)
            {   //this is a critical point put in list
                bool inserted=FALSE;
                wxNode *node = m_CRlist.GetFirst();
                while (node)
                {
                    //sorted on smallest Y value
                    int* ind=(int*) node->GetData();
                    if (m_points[*ind].m_y > m_points[i].m_y)
                    {
                        m_CRlist.Insert(node,(wxObject*) new int(i));
                        inserted = TRUE;
                        break;
                    }
                    node = node->GetNext();
                }
                if (!inserted)
                    m_CRlist.Append((wxObject*) new int(i));
                candidate = FALSE;
            }
        }
    }
}


void wxCanvasPolygon::FillPolygon(wxTransformMatrix* cworld, int clip_x, int clip_y, int clip_width, int clip_height )
{
#if IMAGE_CANVAS
#else
    int index;
    //how much is on pixel in world coordinates

    double scalefactor;
    if (m_gdistance)
        scalefactor=m_gdistance;
    else
        //abs here needed if yaxis is going up (always scan in world coordinates UP)
        scalefactor=fabs(m_admin->DeviceToLogicalYRel(1)); //1 pixel height

    wxDC *dc = m_admin->GetActive()->GetDC();
    dc->SetClippingRegion( clip_x, clip_y, clip_width, clip_height );
    wxPen  gradientpen=m_gpen;

    int dred = m_textbg.Red()-m_textfg.Red();
    int dgreen = m_textbg.Green()-m_textfg.Green();
    int dblue = m_textbg.Blue()-m_textfg.Blue();

    //total number of lines to go from m_textbg to m_textfg
    //gives the following number of steps for the gradient color
    int stepcol = (int) (m_bbox.GetHeight()/scalefactor);

    DetectCriticalPoints();

    double min;
    double max;
    if (cworld->IsIdentity())
    {
        //TODO do something with clipping region (inverse transform?)
        //the next does not work, i don't know why
        //min = wxMin (m_admin->DeviceToLogicalY(clip_y),m_admin->DeviceToLogicalY(clip_y+clip_height));
        //max = wxMax (m_admin->DeviceToLogicalY(clip_y),m_admin->DeviceToLogicalY(clip_y+clip_height));
        min= m_bbox.GetMinY();
        max= m_bbox.GetMaxY();
    }
    else
    {
        min= m_bbox.GetMinY();
        max= m_bbox.GetMaxY();
    }

    int curcol = (int)( (min - m_bbox.GetMinY())/scalefactor );

    double i;
    for ( i = min; i < max; i+=scalefactor)
    {
        wxNode *node = m_AETlist.GetFirst();
        int count= m_AETlist.GetCount();
        while (count > 0)
        {
            wxAET* ele = ((wxAET*)node->GetData());
            index= ele->m_index;
            int direction = ele->m_direction;
            if (!MoveUp(i,index,direction))
            {
                wxNode* h = node;
                //remove this node
                node = node->GetNext();
                m_AETlist.DeleteNode(h);
            }
            else
            {
                if (ele->m_index != index)
                {
                   ele->m_index=index;
                   int h = (index + direction + m_n) % m_n;
                   ele->CalculateLineParameters(m_points[h],m_points[index]);
                }
                if (ele->m_horizontal)
                   ele->m_xs=m_points[index].m_x;
                else
                   ele->CalculateXs(i);
                node = node->GetNext();
            }
            count--;
        }

        node = m_CRlist.GetFirst();
        while (m_CRlist.GetCount() && m_points[*((int*)node->GetData())].m_y <=i )
        {
            int DI;
            for ( DI = -1; DI <=1 ; DI+=2)
            {
                index=*((int*)node->GetData());
                if (MoveUp(i,index,DI))
                {
                    wxAET* ele = new wxAET();
                    ele->m_index=index;
                    ele->m_direction=DI;
                    int h = (index + DI + m_n) % m_n;
                    ele->CalculateLineParameters(m_points[h],m_points[index]);
                    if (ele->m_horizontal)
                        ele->m_xs=m_points[index].m_x;
                    else
                        ele->CalculateXs(i);

                    //insert in sorted order od m_xs
                    bool inserted=FALSE;
                    wxNode *node2 = m_AETlist.GetFirst();
                    while (node2)
                    {
                        //sorted on smallest xs value
                        if (ele->m_xs < ((wxAET*)node2->GetData())->m_xs)
                        {
                            m_AETlist.Insert(node2,(wxObject*) ele);
                            inserted = TRUE;
                            break;
                        }
                        node2 = node2->GetNext();
                    }
                    if (!inserted)
                        m_AETlist.Append((wxObject*)ele);
                }
            }

            wxNode* h= node;
            node = node->GetNext();
            m_CRlist.DeleteNode(h);
        }

        curcol++;
        wxColour gradcol(m_textbg.Red()+dred*curcol/stepcol,
                         m_textbg.Green()+dgreen*curcol/stepcol,
                         m_textbg.Blue()+dblue*curcol/stepcol);
        gradientpen.SetColour(gradcol);

        //m_AETlist must be sorted in m_xs at this moment
        //now draw all the line parts on one horizontal scanline (Winding Rule)
        int out= 0;
        node = m_AETlist.GetFirst();
        while (node)
        {
            wxAET* ele = ((wxAET*)node->GetData());
            out+=ele->m_direction;
            if (out != 0)
            {
                double x1=ele->m_xs;
                node = node->GetNext();
                ele = ((wxAET*)node->GetData());
                double x2=ele->m_xs;
                dc->SetPen( gradientpen );
                double wx1,wy1,wx2,wy2;
                cworld->TransformPoint( x1, i, wx1, wy1 );
                cworld->TransformPoint( x2, i, wx2, wy2 );
                int dx1,dy1,dx2,dy2;
                dx1 = m_admin->LogicalToDeviceX( wx1 );
                dy1 = m_admin->LogicalToDeviceY( wy1 );
                dx2 = m_admin->LogicalToDeviceX( wx2 );
                dy2 = m_admin->LogicalToDeviceY( wy2 );

                //TODO KKK need real line clipping here since line can be rotated.
                if (0 && cworld->IsIdentity())
                {
                    if (dx1 < clip_x) dx1=clip_x;
                    if (dx2 > clip_x + clip_width) dx2=clip_x + clip_width;
                    if ((dy1 >  clip_y) && dy1 < clip_y + clip_height)
                        dc->DrawLine( dx1, dy1, dx2, dy2 );
                }
                else
                {
                    dc->DrawLine( dx1, dy1, dx2, dy2 );
                }

            }
            else
                node = node->GetNext();
        }
    }

    dc->DestroyClippingRegion();
#endif
}





