/*
Program wxLine.CPP
Purpose Mainly used for calculating crossings
Last Update 05-12-1995
*/

#ifdef __GNUG__
#pragma implementation "liner.cpp"
#endif

#include <math.h>

#include <stdlib.h>

#include "wx/canvas/liner.h"

wxLine::wxLine( double x1, double y1, double x2, double y2 )
{
    m_AA = 0.0;
    m_BB = 0.0;
    m_CC = 0.0;

    m_a=wxPoint2DDouble(x1,y1);
    m_b=wxPoint2DDouble(x2,y2);
    if (m_a==m_b)
        assert(0);

    m_valid_parameters = FALSE;
}

wxLine::~wxLine()
{
}

wxLine::wxLine(const wxPoint2DDouble& a,const wxPoint2DDouble& b)
{
    if (a==b)
        assert(0);

    m_a=a;
    m_b=b;
    m_valid_parameters = FALSE;
}



// ActionOnTable1
// This function decide which action must be taken, after PointInLine
// has given the results of two points in relation to a wxLine. See table 1 in the report
//
// input Result_beginPoint:
//          Result_endPoint :
//       The results can be R_R_LEFT_SIDE, R_R_RIGHT_SIDE, R_R_ON_AREA, R_R_IN_AREA
//
// return -1: Illegal combination
//         0: No action, no crosspoints
//         1: Investigate results points in relation to the other wxLine
//         2: endPoint is a crosspoint, no further investigation
//         3: beginPoint is a crosspoint, no further investigation
//            4: beginPoint and endPoint are crosspoints, no further investigation
//         5: beginPoint is a crosspoint, need further investigation
//         6: endPoint is a crosspoint, need further investigation
int wxLine::ActionOnTable1(R_PointStatus Result_beginPoint, R_PointStatus Result_endPoint)
{
    // beginPoint and endPoint are crosspoints
    if (
         (Result_beginPoint == R_IN_AREA)
         &&
         (Result_endPoint == R_IN_AREA)
        )
        return 4;
   // there are no crosspoints, no action
    if (
         (
          (Result_beginPoint == R_LEFT_SIDE)
          &&
          (Result_endPoint == R_LEFT_SIDE)
         )
         ||
         (
          (Result_beginPoint == R_RIGHT_SIDE)
          &&
          (Result_endPoint == R_RIGHT_SIDE)
         )
        )
        return 0;
    // maybe there is a crosspoint, further investigation needed
    if (
         (
          (Result_beginPoint == R_LEFT_SIDE)
          &&
          (
            (Result_endPoint == R_RIGHT_SIDE)
            ||
            (Result_endPoint == R_ON_AREA)
          )
         )
         ||
         (
          (Result_beginPoint == R_RIGHT_SIDE)
          &&
          (
            (Result_endPoint == R_LEFT_SIDE)
            ||
            (Result_endPoint == R_ON_AREA)
          )
         )
         ||
         (
          (Result_beginPoint == R_ON_AREA)
          &&
          (
            (Result_endPoint == R_LEFT_SIDE)
            ||
            (Result_endPoint == R_RIGHT_SIDE)
            ||
            (Result_endPoint == R_ON_AREA)
          )
         )
        )
        return 1;
    //there is a crosspoint
    if (
         (
          (Result_beginPoint == R_LEFT_SIDE)
          ||
          (Result_beginPoint == R_RIGHT_SIDE)
         )
         &&
         (Result_endPoint == R_IN_AREA)
        )
        return 2;
    // there is a crosspoint
    if (
         (Result_beginPoint == R_IN_AREA)
         &&
         (
          (Result_endPoint == R_LEFT_SIDE)
          ||
          (Result_endPoint == R_RIGHT_SIDE)
         )
        )
        return 3;
    // beginPoint is a crosspoint, further investigation needed
    if (
         (Result_beginPoint == R_IN_AREA)
         &&
         (Result_endPoint == R_ON_AREA)
        )
        return 5;
    // endPoint is a crosspoint, further investigation needed
    if (
         (Result_beginPoint == R_ON_AREA)
         &&
         (Result_endPoint == R_IN_AREA)
        )
        return 6;
    // All other combinations are illegal
    return -1;
}


// ActionOnTable2
// This function decide which action must be taken, after PointInLine
// has given the results of two points in relation to a wxLine. It can only give a
// correct decision if first the relation of the points from the wxLine
// are investigated in relation to the wxLine wich can be constucted from the points.
//
// input Result_beginPoint:
//          Result_endPoint :
//       The results can be R_LEFT_SIDE, R_RIGHT_SIDE, R_ON_AREA, R_IN_AREA
//
// return -1: Illegal combination
//         0: No action, no crosspoints
//         1: Calculate crosspoint
//         2: endPoint is a crosspoint
//         3: beginPoint is a crosspoint
//         4: beginPoint and endPoint are crosspoints
int wxLine::ActionOnTable2(R_PointStatus Result_beginPoint, R_PointStatus Result_endPoint)
{
    // beginPoint and eindpoint are crosspoints
    if (
         (Result_beginPoint == R_IN_AREA)
         &&
         (Result_endPoint == R_IN_AREA)
        )
        return 4;
    // there are no crosspoints
    if (
         (
          (Result_beginPoint == R_LEFT_SIDE)
          &&
          (
            (Result_endPoint == R_LEFT_SIDE)
            ||
            (Result_endPoint == R_ON_AREA)
          )
         )
         ||
         (
          (Result_beginPoint == R_RIGHT_SIDE)
          &&
          (
            (Result_endPoint == R_RIGHT_SIDE)
            ||
            (Result_endPoint == R_ON_AREA)
          )
         )
         ||
         (
          (Result_beginPoint == R_ON_AREA)
          &&
          (
            (Result_endPoint == R_LEFT_SIDE)
            ||
            (Result_endPoint == R_RIGHT_SIDE)
            ||
            (Result_endPoint == R_ON_AREA)
          )
         )
        )
        return 0;
    // there is a real intersection, which must be calculated
    if (
         (
          (Result_beginPoint == R_LEFT_SIDE)
          &&
          (Result_endPoint == R_RIGHT_SIDE)
         )
         ||
         (
          (Result_beginPoint == R_RIGHT_SIDE)
          &&
          (Result_endPoint == R_LEFT_SIDE)
         )
        )
        return 1;
    // endPoint is a crosspoint
    if (
         (
          (Result_beginPoint == R_LEFT_SIDE)
          ||
          (Result_beginPoint == R_RIGHT_SIDE)
          ||
          (Result_beginPoint == R_ON_AREA)
         )
         &&
         (Result_endPoint == R_IN_AREA)
        )
        return 2;
    // beginPoint is a crosspoint
    if (
         (Result_beginPoint == R_IN_AREA)
         &&
         (
          (Result_endPoint == R_LEFT_SIDE)
          ||
          (Result_endPoint == R_RIGHT_SIDE)
          ||
          (Result_endPoint == R_ON_AREA)
         )
        )
        return 3;
    // All other combinations are illegal
    return -1;
}

// Calculate the Y when the X is given
double wxLine::Calculate_Y(double X)
{
    CalculateLineParameters();
    if (m_AA != 0)
        return -(m_AA * X + m_CC) / m_BB;
    else
        // horizontal wxLine
        return m_a.m_y;
}

void wxLine::Virtual_Point(wxPoint2DDouble& a_point,double distance)  const
{
    assert(m_valid_parameters);

    //calculate the distance using the slope of the wxLine
   //and rotate 90 degrees

    a_point.m_y=a_point.m_y + (distance * -m_BB);
    a_point.m_x=a_point.m_x - (distance * m_AA );
}



//
// Calculate the lineparameters for the wxLine if nessecary
//
void wxLine::CalculateLineParameters()
{
    // if not valid_parameters calculate the parameters
    if (!m_valid_parameters)
    {
        double length;

        // bp AND ep may not be the same
        if (m_a == m_b)
            assert (0);

        m_AA = (m_b.m_y - m_a.m_y); // A = (Y2-Y1)
        m_BB = (m_a.m_x - m_b.m_x); // B = (X1-X2)

        // the parameters A end B can now be normalized
        length = sqrt(m_AA*m_AA + m_BB*m_BB);

        assert(length !=0);

        m_AA = (m_AA / length);
        m_BB = (m_BB / length);

        m_CC = -((m_AA * m_a.m_x) + (m_a.m_y * m_BB));

        m_valid_parameters = TRUE;
    }
}


// Checks if a wxLine intersect with another wxLine
// inout    wxLine : another wxLine
//          Marge: optional, standard on MARGE (declared in MISC.CPP)
//
// return   true : wxLines are crossing
//          false: wxLines are not crossing
//
bool wxLine::CheckIntersect (wxLine& lijn, double Marge)
{
    double distance=0;

   // bp AND ep may not be the same
   if (m_a == m_b)
      assert (0);

    int Take_Action1, Take_Action2;
    bool Total_Result=FALSE;
    R_PointStatus Result_beginPoint,Result_endPoint;

    Result_beginPoint = PointInLine(lijn.m_a,distance,Marge);
    Result_endPoint   = PointInLine(lijn.m_b,distance,Marge);
    Take_Action1 = ActionOnTable1(Result_beginPoint,Result_endPoint);
    switch (Take_Action1)
    {
        case 0: Total_Result = FALSE ; break;
        case 1: {
                        Result_beginPoint = lijn.PointInLine(m_a,distance,Marge);
                        Result_endPoint   = lijn.PointInLine(m_b,distance,Marge);
                        Take_Action2 = ActionOnTable2(Result_beginPoint,Result_endPoint);
                        switch (Take_Action2)
                        {
                            case 0: Total_Result = FALSE; break;
                            case 1: case 2: case 3: case 4: Total_Result = TRUE; break;
                        }
                  }; break; // This break belongs to the switch(Take_Action1)
        case 2: case 3: case 4: case 5: case 6: Total_Result = TRUE; break;
    }
    return Total_Result; //This is the final decision
}


//
// Get the beginPoint from the wxLine
// usage: Point aPoint = a_line.GetBeginPoint()
//
wxPoint2DDouble wxLine::GetBeginPoint()
{
  return m_a;
}


//
// Get the endPoint from the wxLine
// usage: Point aPoint = a_line.GetEndPoint()
//
wxPoint2DDouble wxLine::GetEndPoint()
{
    return m_b;
}

// Intersects two wxLines
// input    wxLine : another wxLine
//          Marge: optional, standard on MARGE
//
// return   0: If there are no crossings
//          1: If there is one crossing
//          2: If there are two crossings
int wxLine::Intersect(wxLine& lijn, wxPoint2DDouble& c1 ,wxPoint2DDouble& c2 , double Marge)
{
    double distance=0;

    // bp AND ep may not be the same
    if (m_a == m_b)
        assert (0);

    R_PointStatus Result_beginPoint,Result_endPoint;
    int Take_Action1, Take_Action2, Number_of_Crossings = 0;

    Result_beginPoint = PointInLine(lijn.m_a,distance,Marge);
    Result_endPoint   = PointInLine(lijn.m_b,distance,Marge);

    Take_Action1 = ActionOnTable1(Result_beginPoint,Result_endPoint);
// 0: No action, no crosspoints
// 1: Investigate results points in relation to the other wxLine
// 2: endPoint is a crosspoint, no further investigation
// 3: beginPoint is a crosspoint, no further investigation
// 4: beginPoint and endPoint are crosspoints, no further investigation
// 5: beginPoint is a crosspoint, need further investigation
// 6: endPoint is a crosspoint, need further investigation

    // The first switch will insert a crosspoint immediatly
    switch (Take_Action1)
    {
        case 2: case 6: c1=lijn.m_b;
                        Number_of_Crossings = 1;
                        break;
        case 3: case 5: c1=lijn.m_a;
                        Number_of_Crossings = 1;
                        break;
        case 4:         c1=lijn.m_a;
                        c2=lijn.m_b;
                        Number_of_Crossings = 2;
                        break;
        default:
                        break;
    }

    // This switch wil investigate the points of this wxLine in relation to lijn
    // 1: Investigate results points in relation to the other wxLine
    // 5: beginPoint is a crosspoint, need further investigation
    // 6: endPoint is a crosspoint, need further investigation
    switch (Take_Action1)
    {
        case 1: case 5: case 6:
        {
            Result_beginPoint = lijn.PointInLine(m_a,distance,Marge);
            Result_endPoint   = lijn.PointInLine(m_b,distance,Marge);
                Take_Action2 = ActionOnTable2(Result_beginPoint,Result_endPoint);
            // return -1: Illegal combination
            //         0: No action, no crosspoints
            //         1: Calculate crosspoint
            //         2: endPoint is a crosspoint
            //         3: beginPoint is a crosspoint
            //         4: beginPoint and endPoint are crosspoints
            switch (Take_Action2)
            {
                // for the cases see the returnvalue of ActionTable2
                case 1: {   // begin of scope to calculate the intersection
                            double X, Y, Denominator;
                            CalculateLineParameters();
                            Denominator  = (m_AA * lijn.m_BB) - (lijn.m_AA * m_BB);
                            // Denominator may not be 0
                            assert(Denominator != 0.0);
                            // Calculate intersection of both linesegments
                            X = ((m_BB * lijn.m_CC) - (lijn.m_BB * m_CC)) / Denominator;
                            Y = ((lijn.m_AA * m_CC) - (m_AA * lijn.m_CC)) / Denominator;

                            c1.m_x=X;
                            c1.m_y=Y;
                         }
                         Number_of_Crossings++;
                         break;
                case 2:  c2=m_a;
                         Number_of_Crossings++;
                         break;
                case 3:  c2=m_b;
                         Number_of_Crossings++;
                         break;
                case 4:  c1=m_a;
                         c2=m_b;
                         Number_of_Crossings = 2;
                         break;
            }
        };
        break;
        default:
             break;
    }
    return Number_of_Crossings; //This is de final number of crossings
}

//
// test if a point lies in the linesegment. If the point isn't on the wxLine
// the function returns a value that indicates on which side of the
// wxLine the point is (in linedirection from first point to second point
//
// returns R_LEFT_SIDE, when point lies on the left side of the wxLine
//         R_RIGHT_SIDE, when point lies on the right side of the wxLine
//         R_ON_AREA, when point lies on the infinite wxLine within a range
//         R_IN_AREA, when point lies in the area of the linesegment
//        the returnvalues are declared in (wxLine.H)
R_PointStatus wxLine::PointInLine(const wxPoint2DDouble& a_Point, double& Distance,double Marge)
{
    Distance=0;

    // Point may not be the same
    assert(m_a != m_b);

    int Result_ofm_BBox=FALSE;
    R_PointStatus Result_of_Online;

    //quick test if point is begin or endpoint
    if (a_Point == m_a || a_Point == m_b)
        return R_IN_AREA;

    // Checking if point is in bounding-box with marge
    double xmin=wxMin(m_a.m_x,m_b.m_x);
    double xmax=wxMax(m_a.m_x,m_b.m_x);
    double ymin=wxMin(m_a.m_y,m_b.m_y);
    double ymax=wxMax(m_a.m_y,m_b.m_y);

    if (  a_Point.m_x >= (xmin - Marge) && a_Point.m_x <= (xmax + Marge) &&
          a_Point.m_y >= (ymin - Marge) && a_Point.m_y <= (ymax + Marge) )
        Result_ofm_BBox=TRUE;

    // Checking if point is on the infinite wxLine
    Result_of_Online = PointOnLine(a_Point, Distance, Marge);

    // point in boundingbox of the wxLine and is on the wxLine then the point is R_IN_AREA
    if ((Result_ofm_BBox) && (Result_of_Online == R_ON_AREA))
        return R_IN_AREA;
    else
        return Result_of_Online;
}


//
// test if a point lies on the wxLine. If the point isn't on the wxLine
// the function returns a value that indicates on which side of the
// wxLine the point is (in linedirection from first point to second point
//
// returns R_LEFT_SIDE, when point lies on the left side of the wxLine
//         R_ON_AREA, when point lies on the infinite wxLine within a range
//         R_RIGHT_SIDE, when point lies on the right side of the wxLine
//        R_LEFT_SIDE , R_RIGHT_SIDE , R_ON_AREA
R_PointStatus wxLine::PointOnLine(const wxPoint2DDouble& a_Point, double& Distance, double Marge)
{
    Distance=0;
    // Point may not be queal
    assert(m_a!=m_b);

    //quick test if point is begin or endpoint
    if (a_Point == m_a || a_Point == m_b)
        return R_ON_AREA;

    CalculateLineParameters();
    // calculate the distance of a_Point in relation to the wxLine
    Distance = (m_AA * a_Point.m_x)+(m_BB * a_Point.m_y) + m_CC;

    if (Distance < -Marge)
        return R_LEFT_SIDE;
    else
    {
        if (Distance > Marge)
            return R_RIGHT_SIDE;
        else
            return R_ON_AREA;
    }
}

// makes a wxLine same as these
// usage : wxLine1 = wxLine2;
wxLine& wxLine::operator=(const wxLine& a_line)
{
    m_AA = a_line.m_AA;
    m_BB = a_line.m_BB;
    m_CC = a_line.m_CC;

    m_a= a_line.m_a;
    m_b= a_line.m_b;
    m_valid_parameters = a_line.m_valid_parameters;
    return *this;
}

void wxLine::OffsetContour(const wxLine& nextline,double factor,wxPoint2DDouble& offsetpoint)  const
{
    wxPoint2DDouble offs_begin(m_a);
    wxPoint2DDouble offs_end(m_b);

    wxPoint2DDouble offs_bgn_next(nextline.m_a);
    wxPoint2DDouble offs_end_next(nextline.m_b);
    // make a wxPoint2DDouble from this point

    Virtual_Point(offs_begin,factor);
    Virtual_Point(offs_end,factor);
    wxLine  offs_currentline(offs_begin,offs_end);

    nextline.Virtual_Point(offs_bgn_next,factor);
    nextline.Virtual_Point(offs_end_next,factor);
    wxLine  offs_nextline(offs_bgn_next, offs_end_next);

    offs_nextline.CalculateLineParameters();
    offs_currentline.CalculateLineParameters();
    offs_currentline.Intersect(offs_nextline,offsetpoint);
}

// Return the position of the second wxLine compared to this wxLine
// Result = IS_ON | IS_LEFT | IS_RIGHT
// Here Left and Right is defined as being left or right from
// the this wxLine towards the center (common) node
// direction of vetors taken as begin to endpoint with end of this at
// begin of wxLine two
OUTPRODUCT wxLine::OutProduct(const wxLine& two,double accur)
{
    R_PointStatus uitp;
    double distance;
    if (two.m_a==two.m_b)
        assert(0);
    if (m_a==m_b)
        assert(0);

    uitp=PointOnLine(two.m_b, distance, accur);


    /*double uitp=  (_x - first._x) * (third._y - _y) -
                    (_y - first._y) * (third._x - _x);
    if (uitp>0) return IS_LEFT;
    if (uitp<0) return IS_RIGHT;
    return IS_ON;*/

    //depending on direction of this link (going to or coming from centre)
    if (uitp==R_LEFT_SIDE)
        return R_IS_LEFT;
    if (uitp==R_RIGHT_SIDE)
        return R_IS_RIGHT;
    return R_IS_ON;
}

// Intersects two lines if a crossing return TRUE
// else FALSE
bool wxLine::Intersect(wxLine& lijn,wxPoint2DDouble& crossing)
{
    // lijn must exist
    assert(m_valid_parameters);
    assert(lijn.m_valid_parameters);

    double X, Y, Denominator;
    Denominator  = (m_AA * lijn.m_BB) - (lijn.m_AA * m_BB);
    // Denominator may not be 0
    if (Denominator == 0.0)
        return FALSE;
    // Calculate intersection of both linesegments
    X = ((m_BB * lijn.m_CC) - (lijn.m_BB * m_CC)) / Denominator;
    Y = ((lijn.m_AA * m_CC) - (m_AA * lijn.m_CC)) / Denominator;

    crossing.m_x=X;
    crossing.m_y=Y;
    return TRUE;
}

