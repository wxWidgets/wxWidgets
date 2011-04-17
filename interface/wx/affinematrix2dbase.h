/////////////////////////////////////////////////////////////////////////////
// Name:        affinematrix2dbase.h
// Purpose:     wxMatrix2D documentation
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxMatrix2D

    A simple container for 2x2 matrix.

    This simple structure is used with wxAffineMatrix2D.

    @library{wxcore}
    @category{misc}

    @since 2.9.2
 */
struct wxMatrix2D
{
    /**
        Default constructor.

        Initializes the matrix elements to the identity.
     */
    wxMatrix2D(wxDouble v11 = 1,
               wxDouble v12 = 0,
               wxDouble v21 = 0,
               wxDouble v22 = 1);

    /// The matrix elements in the usual mathematical notation.
    wxDouble m_11, m_12, m_21, m_22;
};
