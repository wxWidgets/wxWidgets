// TODO: Eleminate STL vectors
#include <vector>

#include "wx/wx.h"

// WX_DEFINE_ARRAY_PTR( , );

//WX_DEFINE_ARRAY(float, VectorType);
//WX_DEFINE_ARRAY_FLOAT(VectorType);
//WX_DECLARE_OBJARRAY(float, VectorType);

// TODO: This is a work around until wxVector is available or STL vectors are used
// This only has 3 fixed elements, do not attempt to resize
class VectorType
{
public:
	VectorType(int size=3, float defval=0)
	{
		// This actually must always be 3 in this workaround
		m_size = size;

		for (int i=0; i < m_size; ++i)
		{
			m_vector[i] = defval;
		}
	}
	VectorType (VectorType &other)
	{
		m_size = other.size();
		if (m_size > 0)
		{
			for (int i=0; i < m_size; ++i)
			{
				m_vector[i] = other[i];
			}
		}
	}
	~VectorType()
	{
	}

	float & operator [](unsigned index)
	{
		return m_vector[index];
	}

	unsigned size() const
	{
		return m_size;
	}

	void resize(unsigned newSize, float defval=0)
	{
		return;
	}
private:
	// The vector
	float m_vector [3];
	unsigned m_size;
};

//typedef double[3] VectorType;
//#define VectorType float[3];
//typedef std::vector <float> VectorType;

//WX_DEFINE_ARRAY(float, VectorType);
//typedef std::vector <VectorType> MatrixType;
//WX_DECLARE_OBJARRAY(VectorType, MatrixType);

/*
class MatrixLib
{
	static VectorType CrossProduct( VectorType V1, VectorType V2 );
	static MatrixType GetRotationMatrix( float p_Theta, VectorType V );
	static VectorType ApplyMatrix( MatrixType M, VectorType V );
	static void Normalize( VectorType &V );
};
*/