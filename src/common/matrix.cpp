/////////////////////////////////////////////////////////////////////////////
// Name:        matrix.cpp
// Purpose:     wxTransformMatrix class
// Author:      Chris Breeze, Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "matrix.h"
#endif

// Note: this is intended to be used in wxDC at some point to replace
// the current system of scaling/translation. It is not yet used.

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/defs.h"
#endif

#include "wx/matrix.h"
#include <math.h>

const double pi = 3.1415926535;

wxTransformMatrix::wxTransformMatrix(void)
{
	m_isIdentity = FALSE;

	Identity();
}

wxTransformMatrix::wxTransformMatrix(const wxTransformMatrix& mat)
{
	(*this) = mat;
}

double wxTransformMatrix::GetValue(int row, int col) const
{
	if (row < 0 || row > 2 || col < 0 || col > 2)
		return 0.0;

	return m_matrix[row][col];
}

void wxTransformMatrix::SetValue(int row, int col, double value)
{
	if (row < 0 || row > 2 || col < 0 || col > 2)
		return;

	m_matrix[row][col] = value;
}

void wxTransformMatrix::operator = (const wxTransformMatrix& mat)
{
	int i, j;
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			m_matrix[i][j] = mat.m_matrix[i][j];
		}
	}
	m_isIdentity = mat.m_isIdentity;
}

bool wxTransformMatrix::operator == (const wxTransformMatrix& mat)
{
	int i, j;
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			if (m_matrix[i][j] != mat.m_matrix[i][j])
				return FALSE;
		}
	}
	return TRUE;
}

bool wxTransformMatrix::operator != (const wxTransformMatrix& mat)
{
	return (! ((*this) == mat));
}

double& wxTransformMatrix::operator()(int row, int col)
{
	if (row < 0 || row > 2 || col < 0 || col > 2)
		return m_matrix[0][0];

	return m_matrix[row][col];
}

double wxTransformMatrix::operator()(int row, int col) const
{
	if (row < 0 || row > 2 || col < 0 || col > 2)
		return 0.0;

	return m_matrix[row][col];
}

// Invert matrix
bool wxTransformMatrix::Invert(void)
{
	double inverseMatrix[3][3];

	// calculate the adjoint
	inverseMatrix[0][0] =  wxCalculateDet(m_matrix[1][1],m_matrix[2][1],m_matrix[1][2],m_matrix[2][2]);
	inverseMatrix[0][1] = -wxCalculateDet(m_matrix[0][1],m_matrix[2][1],m_matrix[0][2],m_matrix[2][2]);
	inverseMatrix[0][2] =  wxCalculateDet(m_matrix[0][1],m_matrix[1][1],m_matrix[0][2],m_matrix[1][2]);

	inverseMatrix[1][0] = -wxCalculateDet(m_matrix[1][0],m_matrix[2][0],m_matrix[1][2],m_matrix[2][2]);
	inverseMatrix[1][1] =  wxCalculateDet(m_matrix[0][0],m_matrix[2][0],m_matrix[0][2],m_matrix[2][2]);
	inverseMatrix[1][2] = -wxCalculateDet(m_matrix[0][0],m_matrix[1][0],m_matrix[0][2],m_matrix[1][2]);

	inverseMatrix[2][0] =  wxCalculateDet(m_matrix[1][0],m_matrix[2][0],m_matrix[1][1],m_matrix[2][1]);
	inverseMatrix[2][1] = -wxCalculateDet(m_matrix[0][0],m_matrix[2][0],m_matrix[0][1],m_matrix[2][1]);
	inverseMatrix[2][2] =  wxCalculateDet(m_matrix[0][0],m_matrix[1][0],m_matrix[0][1],m_matrix[1][1]);

	// now divide by the determinant
	double det = m_matrix[0][0] * inverseMatrix[0][0] + m_matrix[0][1] * inverseMatrix[1][0] + m_matrix[0][2] * inverseMatrix[2][0];
	if (det != 0.0)
	{
		inverseMatrix[0][0] /= det; inverseMatrix[1][0] /= det; inverseMatrix[2][0] /= det;
		inverseMatrix[0][1] /= det; inverseMatrix[1][1] /= det; inverseMatrix[2][1] /= det;
		inverseMatrix[0][2] /= det; inverseMatrix[1][2] /= det; inverseMatrix[2][2] /= det;

		int i, j;
		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				m_matrix[i][j] = inverseMatrix[i][j];
			}
		}
		m_isIdentity = IsIdentity1();
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// Make into identity matrix
bool wxTransformMatrix::Identity(void)
{
	m_matrix[0][0] = m_matrix[1][1] = m_matrix[2][2] = 1.0;
	m_matrix[1][0] = m_matrix[2][0] = m_matrix[0][1] = m_matrix[2][1] = m_matrix[0][2] = m_matrix[1][2] = 0.0;
	m_isIdentity = TRUE;

	return TRUE;
}

// Scale by scale (isotropic scaling i.e. the same in x and y):
//           | scale  0      0      |
// matrix' = |  0     scale  0      | x matrix
//           |  0     0      scale  |
//
bool wxTransformMatrix::Scale(double scale)
{
	int i, j;
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			m_matrix[i][j] *= scale;
		}
	}
	m_isIdentity = IsIdentity1();

	return TRUE;
}

// Translate by dx, dy:
//           | 1  0 dx |
// matrix' = | 0  1 dy | x matrix
//           | 0  0  1 |
//
bool wxTransformMatrix::Translate(double dx, double dy)
{
	int i;
	for (i = 0; i < 3; i++)
		m_matrix[i][0] += dx * m_matrix[i][2];
	for (i = 0; i < 3; i++)
		m_matrix[i][1] += dy * m_matrix[i][2];

	m_isIdentity = IsIdentity1();

	return TRUE;
}

// Rotate by the given number of degrees:
//           |  cos sin 0 |
// matrix' = | -sin cos 0 | x matrix
//           |   0   0  1 |
//
bool wxTransformMatrix::Rotate(double degrees)
{
	double angle = degrees * pi / 180.0;
	double s = sin(angle);
	double c = cos(angle);

	m_matrix[0][0] = c * m_matrix[0][0] + s * m_matrix[0][1];
	m_matrix[1][0] = c * m_matrix[1][0] + s * m_matrix[1][1];
	m_matrix[2][0] = c * m_matrix[2][0] + s * m_matrix[2][1];
	m_matrix[0][2] = c * m_matrix[0][1] - s * m_matrix[0][0];
	m_matrix[1][2] = c * m_matrix[1][1] - s * m_matrix[1][0];
	m_matrix[2][2] = c * m_matrix[2][1] - s * m_matrix[2][0];

	m_isIdentity = IsIdentity1();

	return TRUE;
}

// Transform a point from logical to device coordinates
bool wxTransformMatrix::TransformPoint(double x, double y, double& tx, double& ty) const
{
	if (IsIdentity())
	{
		tx = x;	ty = y;	return TRUE;
	}

	tx = x * m_matrix[0][0] + y * m_matrix[1][0] + m_matrix[2][0];
	ty = x * m_matrix[0][1] + y * m_matrix[1][1] + m_matrix[2][1];

	return TRUE;
}

// Transform a point from device to logical coordinates.

// Example of use:
//   wxTransformMatrix mat = dc.GetTransformation();
//   mat.Invert();
//   mat.InverseTransformPoint(x, y, x1, y1);
// OR (shorthand:)
//   dc.LogicalToDevice(x, y, x1, y1);
// The latter is slightly less efficient if we're doing several
// conversions, since the matrix is inverted several times.

bool wxTransformMatrix::InverseTransformPoint(double x, double y, double& tx, double& ty) const
{
	if (IsIdentity())
	{
		tx = x;	ty = y;	return TRUE;
	}

	double z = (1.0 - m_matrix[0][2] * x - m_matrix[1][2] * y) / m_matrix[2][2];
	if (z == 0.0)
	{
//		z = 0.0000001;
		return FALSE;
	}
	tx = x * m_matrix[0][0] + y * m_matrix[1][0] + z * m_matrix[2][0];
	ty = x * m_matrix[0][1] + y * m_matrix[1][1] + z * m_matrix[2][1];
	return TRUE;
}

