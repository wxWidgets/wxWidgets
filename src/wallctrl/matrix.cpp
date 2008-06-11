// TODO: Eleminate vectors

#include "wx/Wallctrl/Matrix.h"
#include <cmath>

VectorType MatrixLib::CrossProduct( VectorType V1, VectorType V2 )
{
	VectorType Out(3,0);
	Out[0] = V1[1]*V2[2] - V1[2]*V2[1];
	Out[1] = -(V1[0]*V2[2] - V1[2]*V2[0]);
	Out[2] = V1[0]*V2[1] - V1[1]*V2[0];

	return Out;
}

MatrixType MatrixLib::GetRotationMatrix( float p_Theta, VectorType V )
{
	float X = V[0];
	float Y = V[1];
	float Z = V[2];

	MatrixType M(3, VectorType(3,0));

	M[0][0]=cos(p_Theta) + (1-cos(p_Theta))*(X*X) +sin(p_Theta)*(0);
	M[0][1]=cos(p_Theta)*0 + (1-cos(p_Theta))*(X*Y) +sin(p_Theta)*(-Z);
	M[0][2]=cos(p_Theta)*0 + (1-cos(p_Theta))*(X*Z) +sin(p_Theta)*(Y);
	M[1][0]=cos(p_Theta)*0 + (1-cos(p_Theta))*(X*Y) +sin(p_Theta)*(Z);
	M[1][1]=cos(p_Theta) + (1-cos(p_Theta))*(Y*Y) +sin(p_Theta)*(0);
	M[1][2]=cos(p_Theta)*0 + (1-cos(p_Theta))*(Y*Z) +sin(p_Theta)*(-X);
	M[2][0]=cos(p_Theta)*0 + (1-cos(p_Theta))*(X*Z) +sin(p_Theta)*(-Y);
	M[2][1]=cos(p_Theta)*0 + (1-cos(p_Theta))*(Y*Z) +sin(p_Theta)*(X);
	M[2][2]=cos(p_Theta) + (1-cos(p_Theta))*(Z*Z) +sin(p_Theta)*(0);

	return M;
}

VectorType MatrixLib::ApplyMatrix( MatrixType M, VectorType V )
{
	VectorType Out(3,0);
	for (int i=0; i < 3; ++i)
	{
		Out[i] = V[0]*M[i][0] + V[1]*M[i][1] + V[2]*M[i][2];
	}
	return Out;
}

void MatrixLib::Normalize( VectorType &V )
{
	float Base = sqrt(V[0]*V[0] + V[1]*V[1] + V[2]*V[2]);
	if (Base == 0) return;
	V[0]/= Base;
	V[1]/=Base;
	V[2]/=Base;
}
