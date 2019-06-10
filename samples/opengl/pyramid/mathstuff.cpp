/////////////////////////////////////////////////////////////////////////////
// Name:        mathstuff.cpp
// Purpose:     Some maths used for pyramid sample
// Author:      Manuel Martin
// Created:     2015/01/31
// Copyright:   (c) 2015 Manuel Martin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include <cmath>

#include "mathstuff.h"

// Overload of "-" operator
myVec3 operator- (const myVec3& v1, const myVec3& v2)
{
    return myVec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

// Vector normalization
myVec3 MyNormalize(const myVec3& v)
{
    double mo = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if ( mo > 1E-20 )
        return myVec3(v.x / mo, v.y / mo, v.z / mo);
    else
        return myVec3();
}

// Dot product
double MyDot(const myVec3& v1, const myVec3& v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z ;
}

// Cross product
myVec3 MyCross(const myVec3& v1, const myVec3& v2)
{
    return myVec3( v1.y * v2.z - v2.y * v1.z,
                   v1.z * v2.x - v2.z * v1.x,
                   v1.x * v2.y - v2.x * v1.y );
}

// Distance between two points
double MyDistance(const myVec3& v1, const myVec3& v2)
{
    double rx = v1.x -v2.x;
    double ry = v1.y -v2.y;
    double rz = v1.z -v2.z;

    return sqrt(rx*rx + ry*ry + rz*rz);
}

// Angle between two normalized vectors, in radians
double AngleBetween(const myVec3& v1, const myVec3& v2)
{
    double angle = MyDot(v1, v2);
    // Prevent issues due to numerical precision
    if (angle > 1.0)
        angle = 1.0;
    if (angle < -1.0)
        angle = -1.0;

    return acos(angle);
}

// Matrix 4x4 by 4x1 multiplication
// Attention: No bounds check!
myVec4 MyMatMul4x1(const double *m1, const myVec4& v)
{
    myVec4 mmv;
    mmv.x = m1[0] * v.x + m1[4] * v.y +  m1[8] * v.z + m1[12] * v.w ;
    mmv.y = m1[1] * v.x + m1[5] * v.y +  m1[9] * v.z + m1[13] * v.w ;
    mmv.z = m1[2] * v.x + m1[6] * v.y + m1[10] * v.z + m1[14] * v.w ;
    mmv.w = m1[3] * v.x + m1[7] * v.y + m1[11] * v.z + m1[15] * v.w ;

    return mmv;
}

// Matrix 4x4 multiplication
// Attention: No bounds check!
void MyMatMul4x4(const double *m1, const double *m2, double* mm)
{
     mm[0] = m1[0] *  m2[0] + m1[4] *  m2[1] +  m1[8] *  m2[2] + m1[12] *  m2[3] ;
     mm[1] = m1[1] *  m2[0] + m1[5] *  m2[1] +  m1[9] *  m2[2] + m1[13] *  m2[3] ;
     mm[2] = m1[2] *  m2[0] + m1[6] *  m2[1] + m1[10] *  m2[2] + m1[14] *  m2[3] ;
     mm[3] = m1[3] *  m2[0] + m1[7] *  m2[1] + m1[11] *  m2[2] + m1[15] *  m2[3] ;
     mm[4] = m1[0] *  m2[4] + m1[4] *  m2[5] +  m1[8] *  m2[6] + m1[12] *  m2[7] ;
     mm[5] = m1[1] *  m2[4] + m1[5] *  m2[5] +  m1[9] *  m2[6] + m1[13] *  m2[7] ;
     mm[6] = m1[2] *  m2[4] + m1[6] *  m2[5] + m1[10] *  m2[6] + m1[14] *  m2[7] ;
     mm[7] = m1[3] *  m2[4] + m1[7] *  m2[5] + m1[11] *  m2[6] + m1[15] *  m2[7] ;
     mm[8] = m1[0] *  m2[8] + m1[4] *  m2[9] +  m1[8] * m2[10] + m1[12] * m2[11] ;
     mm[9] = m1[1] *  m2[8] + m1[5] *  m2[9] +  m1[9] * m2[10] + m1[13] * m2[11] ;
    mm[10] = m1[2] *  m2[8] + m1[6] *  m2[9] + m1[10] * m2[10] + m1[14] * m2[11] ;
    mm[11] = m1[3] *  m2[8] + m1[7] *  m2[9] + m1[11] * m2[10] + m1[15] * m2[11] ;
    mm[12] = m1[0] * m2[12] + m1[4] * m2[13] +  m1[8] * m2[14] + m1[12] * m2[15] ;
    mm[13] = m1[1] * m2[12] + m1[5] * m2[13] +  m1[9] * m2[14] + m1[13] * m2[15] ;
    mm[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15] ;
    mm[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15] ;
}

// Matrix 4x4 inverse. Returns the determinant.
// Attention: No bounds check!
// Method used is "adjugate matrix" with "cofactors".
// A faster method, such as "LU decomposition", isn't much faster than this code.
double MyMatInverse(const double *m, double *minv)
{
    double det;
    double cof[16], sdt[19];

    // The 2x2 determinants used for cofactors
    sdt[0]  = m[10] * m[15] - m[14] * m[11] ;
    sdt[1]  =  m[9] * m[15] - m[13] * m[11] ;
    sdt[2]  =  m[9] * m[14] - m[13] * m[10] ;
    sdt[3]  =  m[8] * m[15] - m[12] * m[11] ;
    sdt[4]  =  m[8] * m[14] - m[12] * m[10] ;
    sdt[5]  =  m[8] * m[13] - m[12] *  m[9] ;
    sdt[6]  =  m[6] * m[15] - m[14] *  m[7] ;
    sdt[7]  =  m[5] * m[15] - m[13] *  m[7] ;
    sdt[8]  =  m[5] * m[14] - m[13] *  m[6] ;
    sdt[9]  =  m[4] * m[15] - m[12] *  m[7] ;
    sdt[10] =  m[4] * m[14] - m[12] *  m[6] ;
    sdt[11] =  m[5] * m[15] - m[13] *  m[7] ;
    sdt[12] =  m[4] * m[13] - m[12] *  m[5] ;
    sdt[13] =  m[6] * m[11] - m[10] *  m[7] ;
    sdt[14] =  m[5] * m[11] -  m[9] *  m[7] ;
    sdt[15] =  m[5] * m[10] -  m[9] *  m[6] ;
    sdt[16] =  m[4] * m[11] -  m[8] *  m[7] ;
    sdt[17] =  m[4] * m[10] -  m[8] *  m[6] ;
    sdt[18] =  m[4] *  m[9] -  m[8] *  m[5] ;
    // The cofactors, transposed
    cof[0]  =   m[5] *  sdt[0] - m[6] *  sdt[1] + m[7] *  sdt[2] ;
    cof[1]  = - m[1] *  sdt[0] + m[2] *  sdt[1] - m[3] *  sdt[2] ;
    cof[2]  =   m[1] *  sdt[6] - m[2] *  sdt[7] + m[3] *  sdt[8] ;
    cof[3]  = - m[1] * sdt[13] + m[2] * sdt[14] - m[3] * sdt[15] ;
    cof[4]  = - m[4] *  sdt[0] + m[6] *  sdt[3] - m[7] *  sdt[4] ;
    cof[5]  =   m[0] *  sdt[0] - m[2] *  sdt[3] + m[3] *  sdt[4] ;
    cof[6]  = - m[0] *  sdt[6] + m[2] *  sdt[9] - m[3] * sdt[10] ;
    cof[7]  =   m[0] * sdt[13] - m[2] * sdt[16] + m[3] * sdt[17] ;
    cof[8]  =   m[4] *  sdt[1] - m[5] *  sdt[3] + m[7] *  sdt[5] ;
    cof[9]  = - m[0] *  sdt[1] + m[1] *  sdt[3] - m[3] *  sdt[5] ;
    cof[10] =   m[0] * sdt[11] - m[1] *  sdt[9] + m[3] * sdt[12] ;
    cof[11] = - m[0] * sdt[14] + m[1] * sdt[16] - m[3] * sdt[18] ;
    cof[12] = - m[4] *  sdt[2] + m[5] *  sdt[4] - m[6] *  sdt[5] ;
    cof[13] =   m[0] *  sdt[2] - m[1] *  sdt[4] + m[2] *  sdt[5] ;
    cof[14] = - m[0] *  sdt[8] + m[1] * sdt[10] - m[2] * sdt[12] ;
    cof[15] =   m[0] * sdt[15] - m[1] * sdt[17] + m[2] * sdt[18] ;

    det = m[0] * cof[0] + m[1] * cof[4] + m[2] * cof[8] + m[3] * cof[12] ;

    if ( fabs(det) > 10E-9 ) // Some precision value
    {
        double invdet = 1.0 / det;
        for (int i = 0; i < 16; ++i)
            minv[i] = cof[i] * invdet;
    }
    else
    {
        // Enable comparison with 0
        det = 0.0;
    }

    return det;
}

// Matrix of rotation around an axis in the origin.
// angle is positive if follows axis (right-hand rule)
// Attention: No bounds check!
void MyRotate(const myVec3& axis, double angle, double *mrot)
{
    double c = cos(angle);
    double s = sin(angle);
    double t = 1.0 - c;

    // Normalize the axis vector
    myVec3 uv = MyNormalize(axis);

    // Store the matrix in column order
    mrot[0]  = t * uv.x * uv.x + c ;
    mrot[1]  = t * uv.x * uv.y + s * uv.z ;
    mrot[2]  = t * uv.x * uv.z - s * uv.y ;
    mrot[3]  = 0.0 ;
    mrot[4]  = t * uv.y * uv.x - s * uv.z ;
    mrot[5]  = t * uv.y * uv.y + c ;
    mrot[6]  = t * uv.y * uv.z + s * uv.x ;
    mrot[7]  = 0.0 ;
    mrot[8]  = t * uv.z * uv.x + s * uv.y ;
    mrot[9]  = t * uv.z * uv.y - s * uv.x ;
    mrot[10] = t * uv.z * uv.z + c ;
    mrot[11] = 0.0 ;
    mrot[12] = mrot[13] = mrot[14] = 0.0 ;
    mrot[15] = 1.0 ;
}

// Matrix for defining the viewing transformation
// Attention: No bounds check!
// Unchecked conditions:
//   camPos != targ  && camUp != {0,0,0}
//   camUo can't be parallel to camPos - targ
void MyLookAt(const myVec3& camPos, const myVec3& camUp, const myVec3& targ, double *mt)
{
    myVec3 tc = MyNormalize(targ - camPos);
    myVec3 up = MyNormalize(camUp);
    // Normalize tc x up for the case where up is not perpendicular to tc
    myVec3  s = MyNormalize(MyCross(tc, up));
    myVec3  u = MyNormalize(MyCross(s, tc)); //Normalize to improve accuracy

    // Store the matrix in column order
    mt[0]  = s.x ;
    mt[1]  = u.x ;
    mt[2]  = - tc.x ;
    mt[3]  = 0.0 ;
    mt[4]  = s.y ;
    mt[5]  = u.y ;
    mt[6]  = - tc.y ;
    mt[7]  = 0.0 ;
    mt[8]  = s.z ;
    mt[9]  = u.z ;
    mt[10] = - tc.z ;
    mt[11] = 0.0 ;
    mt[12] = - MyDot(s, camPos) ;
    mt[13] = - MyDot(u, camPos) ;
    mt[14] =   MyDot(tc, camPos) ;
    mt[15] = 1.0 ;
}

// Matrix for defining the perspective projection with symmetric frustum
// From camera coordinates to canonical (2x2x2 cube) coordinates.
// Attention: No bounds check!
// Unchecked conditions: fov > 0 && zNear > 0 && zFar > zNear && aspect > 0
void MyPerspective(double fov, double aspect, double zNear, double zFar, double *mp)
{
    double f = 1.0 / tan(fov / 2.0);

    // Store the matrix in column order
    mp[0]  = f / aspect ;
    mp[1]  = mp[2] = mp[3]  = 0.0 ;
    mp[4]  = 0.0 ;
    mp[5]  = f ;
    mp[6]  = mp[7] = 0.0 ;
    mp[8]  = mp[9] = 0.0 ;
    mp[10] = (zNear + zFar) / (zNear - zFar) ;
    mp[11] = -1.0 ;
    mp[12] = mp[13] = 0.0 ;
    mp[14] = 2.0 * zNear * zFar / (zNear - zFar) ;
    mp[15] = 0.0 ;
}

// Matrix for defining the orthogonal projection with symmetric frustum
// From camera coordinates to canonical (2x2x2 cube) coordinates.
// Attention: No bounds check!
// Unchecked conditions: left != right && bottom != top && zNear != zFar
void MyOrtho(double left, double right, double bottom, double top,
             double zNear, double zFar, double *mo)
{
    // Store the matrix in column order
    mo[0]  = 2.0 / (right - left) ;
    mo[1]  = mo[2] = mo[3] = mo[4] = 0.0 ;
    mo[5]  = 2.0 / (top - bottom) ;
    mo[6]  = mo[7] = mo[8]  = mo[9] = 0.0 ;
    mo[10] = 2.0 / (zNear - zFar) ;
    mo[11] = 0.0 ;
    mo[12] = -(right + left) / (right - left) ;
    mo[13] = -(top + bottom) / (top - bottom) ;
    mo[14] = (zNear + zFar) / (zNear - zFar) ;
    mo[15] = 1.0 ;
}

