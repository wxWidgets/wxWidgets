/////////////////////////////////////////////////////////////////////////////
// Name:        mathstuff.h
// Purpose:     Some maths used for pyramid sample
// Author:      Manuel Martin
// Created:     2015/01/31
// Copyright:   (c) 2015 Manuel Martin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef MATHSTUFF_H
  #define MATHSTUFF_H

//NOTE:
// glm library is great for handling matrices and vectors in an OpenGL style, see
// http://glm.g-truc.net/
// But it's too large for this simple sample. I coded on my own the maths needed.

// A vector with 3 components
class myVec3
{
public:
    myVec3() { x = y = z = 0.0 ;}
    myVec3(double xd, double yd, double zd) : x(xd), y(yd), z(zd) {}
    myVec3(float xd, float yd, float zd) : x(double(xd)), y(double(yd)), z(double(zd)) {}
    ~myVec3() {}

    double x, y, z;
};

// A vector with 4 components
class myVec4
{
public:
    myVec4() { x = y = z = w = 0.0 ;}
    myVec4(double xd, double yd, double zd, double wd)
            : x(xd), y(yd), z(zd), w(wd) {}
    myVec4(const myVec3& v3, double wd = 0.0)
            : x(v3.x), y(v3.y), z(v3.z), w(wd) {}
    ~myVec4() {}

    double x, y, z, w;
};

// Overload of "-" operator
myVec3 operator- (const myVec3& v1, const myVec3& v2);

// Vector normalization
myVec3 MyNormalize(const myVec3& v);

// Dot product
double MyDot(const myVec3& v1, const myVec3& v2);

// Cross product
myVec3 MyCross(const myVec3& v1, const myVec3& v2);

// Distance between two points
double MyDistance(const myVec3& v1, const myVec3& v2);

// Angle between two normalized vectors, in radians
double AngleBetween(const myVec3& v1, const myVec3& v2);

// Matrix 4x4 by 4x1 multiplication
myVec4 MyMatMul4x1(const double *m1, const myVec4& v);

// Matrix 4x4 by 4x4 multiplication
void MyMatMul4x4(const double *m1, const double *m2, double* mm);

// Matrix inverse. Returns the determinant
double MyMatInverse(const double *m, double *minv);

// Matrix of rotation around an axis in the origin
void MyRotate(const myVec3& axis, double angle, double *mrot);

// Matrix for defining the viewing transformation
void MyLookAt(const myVec3& camPos, const myVec3& camUp, const myVec3& targ, double *mt);

// Matrix for defining the perspective projection with symmetric frustum
void MyPerspective(double fov, double aspect, double zNear, double zFar, double *mp);

// Matrix for defining the orthogonal projection
void MyOrtho(double left, double right, double bottom, double top,
             double zNear, double zFar, double *mo);

#endif // MATHSTUFF_H
