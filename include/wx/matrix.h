/////////////////////////////////////////////////////////////////////////////
// Name:        matrix.h
// Purpose:     wxTransformMatrix class. NOT YET USED
// Author:      Chris Breeze, Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __MATRIXH__
#define __MATRIXH__

#ifdef __GNUG__
#pragma interface "matrix.h"
#endif

#include "wx/object.h"

// A simple 3x3 matrix. This may be replaced by a more general matrix
// class some day.
//
// Note: this is intended to be used in wxDC at some point to replace
// the current system of scaling/translation. It is not yet used.

class WXDLLEXPORT wxTransformMatrix: public wxObject
{
public:
	wxTransformMatrix(void);
	wxTransformMatrix(const wxTransformMatrix& mat);

	double GetValue(int row, int col) const;
	void SetValue(int row, int col, double value);

	void operator = (const wxTransformMatrix& mat);
	bool operator == (const wxTransformMatrix& mat);
	bool operator != (const wxTransformMatrix& mat);

	double& operator()(int row, int col);
	double operator()(int row, int col) const;

	// Invert matrix
	bool Invert(void);

	// Make into identity matrix
	bool Identity(void);

	// Is the matrix the identity matrix?
	// Only returns a flag, which is set whenever an operation
	// is done.
	inline bool IsIdentity(void) const { return m_isIdentity; };

	// This does an actual check.
	inline bool IsIdentity1(void) const ;

	// Isotropic scaling
	bool Scale(double scale);

	// Translate
	bool Translate(double x, double y);

	// Rotate
	bool Rotate(double angle);

	// Transform X value from logical to device
	inline double TransformX(double x) const;

	// Transform Y value from logical to device
	inline double TransformY(double y) const;

	// Transform a point from logical to device coordinates
	bool TransformPoint(double x, double y, double& tx, double& ty) const;

	// Transform a point from device to logical coordinates.

	// Example of use:
	//   wxTransformMatrix mat = dc.GetTransformation();
	//   mat.Invert();
	//   mat.InverseTransformPoint(x, y, x1, y1);
	// OR (shorthand:)
	//   dc.LogicalToDevice(x, y, x1, y1);
	// The latter is slightly less efficient if we're doing several
	// conversions, since the matrix is inverted several times.

	// N.B. 'this' matrix is the inverse at this point

	bool InverseTransformPoint(double x, double y, double& tx, double& ty) const;

public:
    double 	m_matrix[3][3];
	bool	m_isIdentity;
/*
	double m11, m21, m31;
	double m12, m22, m32;
	double m13, m23, m33;
*/
};


/*
The code is wrong and doesn't compile. Chris Breeze als reported, that
some functions of wxTransformMatrix cannot work because it is not
known if he matrix has been inverted. Be careful when using it.

// Transform X value from logical to device
inline double wxTransformMatrix::TransformX(double x) const
{
	return (m_isIdentity ? x : (x * m_matrix[0][0] + y * m_matrix[1][0] + m_matrix[2][0]));
}

// Transform Y value from logical to device
inline double wxTransformMatrix::TransformY(double y) const
{
	return (m_isIdentity ? y : (x * m_matrix[0][1] + y * m_matrix[1][1] + m_matrix[2][1]));
}
*/

// Is the matrix the identity matrix?
// Perhaps there's some kind of optimization we can do to make this
// a faster operation. E.g. each operation (scale, translate etc.)
// checks whether it's still the identity matrix and sets a flag.
inline bool wxTransformMatrix::IsIdentity1(void) const
{
	return
	 (m_matrix[0][0] == 1.0 &&
	  m_matrix[1][1] == 1.0 &&
	  m_matrix[2][2] == 1.0 &&
	  m_matrix[1][0] == 0.0 &&
	  m_matrix[2][0] == 0.0 &&
	  m_matrix[0][1] == 0.0 &&
	  m_matrix[2][1] == 0.0 &&
	  m_matrix[0][2] == 0.0 &&
	  m_matrix[1][2] == 0.0) ;
}

// Calculates the determinant of a 2 x 2 matrix
inline double wxCalculateDet(double a11, double a21, double a12, double a22)
{
	return a11 * a22 - a12 * a21;
}

#endif
	// __MATRIXH__
