// TODO: Eleminate vectors
#include <vector>

// WX_DEFINE_ARRAY_PTR( , );
typedef std::vector <std::vector <float> > MatrixType;
typedef std::vector <float> VectorType;

class MatrixLib
{
	static VectorType CrossProduct( VectorType V1, VectorType V2 );
	static MatrixType GetRotationMatrix( float p_Theta, VectorType V );
	static VectorType ApplyMatrix( MatrixType M, VectorType V );
	static void Normalize( VectorType &V );
};
