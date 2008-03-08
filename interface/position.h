/////////////////////////////////////////////////////////////////////////////
// Name:        position.h
// Purpose:     documentation for wxPosition class
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
    
    @seealso
    wxPoint, wxSize
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
    int GetCol();

    /**
        Get the current row value.
    */
    int GetColumn();

    /**
        Get the current row value.
    */
    int GetRow();

    //@{
    /**
        
    */
    bool operator ==(const wxPosition& p);
        bool operator !=(const wxPosition& p);
        wxPosition operator +=(const wxPosition& p);
        wxPosition operator -=(const wxPosition& p);
        wxPosition operator +=(const wxSize& s);
        wxPosition operator -=(const wxSize& s);
        wxPosition operator +(const wxPosition& p);
        wxPosition operator -(const wxPosition& p);
        wxPosition operator +(const wxSize& s);
        wxPosition operator -(const wxSize& s);
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
