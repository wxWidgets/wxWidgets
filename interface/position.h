/////////////////////////////////////////////////////////////////////////////
// Name:        position.h
// Purpose:     interface of wxPosition
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxPosition
    @wxheader{position.h}

    This class represents the position of an item in any kind of grid of rows and
    columns such as wxGridBagSizer, or
    wxHVScrolledWindow.

    @library{wxbase}
    @category{FIXME}

    @see wxPoint, wxSize
*/
class wxPosition
{
public:
    //@{
    /**
        Construct a new wxPosition, optionally setting the row and column. The
        default value is (0, 0).
    */
    wxPosition();
    wxPosition(int row, int col);
    //@}

    /**
        A synonym for GetColumn().
    */
    int GetCol() const;

    /**
        Get the current row value.
    */
    int GetColumn() const;

    /**
        Get the current row value.
    */
    int GetRow() const;

    //@{
    /**
        
    */
    bool operator ==(const wxPosition& p) const;
    const bool operator !=(const wxPosition& p) const;
    const wxPosition&  operator +=(const wxPosition& p) const;
    wxPosition operator -=(const wxPosition& p) const;
    wxPosition operator +=(const wxSize& s) const;
    wxPosition operator -=(const wxSize& s) const;
    wxPosition operator +(const wxPosition& p) const;
    const wxPosition  operator -(const wxPosition& p) const;
    const wxPosition  operator +(const wxSize& s) const;
    const wxPosition  operator -(const wxSize& s) const;
    //@}

    /**
        A synonym for SetColumn().
    */
    void SetCol(int column);

    /**
        Set a new column value.
    */
    void SetColumn(int column);

    /**
        Set a new row value.
    */
    void SetRow(int row);
};

