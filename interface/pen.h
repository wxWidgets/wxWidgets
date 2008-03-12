/////////////////////////////////////////////////////////////////////////////
// Name:        pen.h
// Purpose:     interface of wxPen
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxPen
    @ingroup group_class_gdi
    @wxheader{pen.h}

    A pen is a drawing tool for drawing outlines. It is used for drawing
    lines and painting the outline of rectangles, ellipses, etc. It has a
    colour, a width and a style.

    @library{wxcore}
    @category{gdi}

    @stdobjects
    ::Objects:, ::wxNullPen, ::Pointers:, ::wxRED_PEN, ::wxCYAN_PEN, ::wxGREEN_PEN,
    ::wxBLACK_PEN, ::wxWHITE_PEN, ::wxTRANSPARENT_PEN, ::wxBLACK_DASHED_PEN, ::wxGREY_PEN, ::wxMEDIUM_GREY_PEN, ::wxLIGHT_GREY_PEN,

    @see wxPenList, wxDC, wxDC::SetPen
*/
class wxPen : public wxGDIObject
{
public:
    //@{
    /**
        Copy constructor, uses @ref overview_trefcount "reference counting".
        
        @param colour
            A colour object.
        @param colourName
            A colour name.
        @param width
            Pen width. Under Windows, the pen width cannot be greater than 1 if
            the style is wxDOT, wxLONG_DASH, wxSHORT_DASH, wxDOT_DASH, or wxUSER_DASH.
        @param stipple
            A stipple bitmap.
        @param pen
            A pointer or reference to a pen to copy.
        @param style
            The style may be one of the following:
        
        
        
        
        
        
            wxSOLID
        
        
        
        
            Solid style.
        
        
        
        
        
            wxTRANSPARENT
        
        
        
        
            No pen is used.
        
        
        
        
        
            wxDOT
        
        
        
        
            Dotted style.
        
        
        
        
        
            wxLONG_DASH
        
        
        
        
            Long dashed style.
        
        
        
        
        
            wxSHORT_DASH
        
        
        
        
            Short dashed style.
        
        
        
        
        
            wxDOT_DASH
        
        
        
        
            Dot and dash style.
        
        
        
        
        
            wxSTIPPLE
        
        
        
        
            Use the stipple bitmap.
        
        
        
        
        
            wxUSER_DASH
        
        
        
        
            Use the user dashes: see SetDashes().
        
        
        
        
        
            wxBDIAGONAL_HATCH
        
        
        
        
            Backward diagonal hatch.
        
        
        
        
        
            wxCROSSDIAG_HATCH
        
        
        
        
            Cross-diagonal hatch.
        
        
        
        
        
            wxFDIAGONAL_HATCH
        
        
        
        
            Forward diagonal hatch.
        
        
        
        
        
            wxCROSS_HATCH
        
        
        
        
            Cross hatch.
        
        
        
        
        
            wxHORIZONTAL_HATCH
        
        
        
        
            Horizontal hatch.
        
        
        
        
        
            wxVERTICAL_HATCH
        
        
        
        
            Vertical hatch.
        
        @remarks Different versions of Windows and different versions of other
                 platforms support very different subsets of the styles
                 above - there is no similarity even between Windows95
                 and Windows98 - so handle with care.
        
        @see SetStyle(), SetColour(), SetWidth(), SetStipple()
    */
    wxPen();
    wxPen(const wxColour& colour, int width = 1,
          int style = wxSOLID);
    wxPen(const wxString& colourName, int width, int style);
    wxPen(const wxBitmap& stipple, int width);
    wxPen(const wxPen& pen);
    //@}

    /**
        Destructor.
        See @ref overview_refcountdestruct "reference-counted object destruction" for
        more info.
        
        @remarks Although all remaining pens are deleted when the application
                 exits, the application should try to clean up all pens
                 itself. This is because wxWidgets cannot know if a
                 pointer to the pen object is stored in an application
                 data structure, and there is a risk of double deletion.
    */
    ~wxPen();

    /**
        Returns the pen cap style, which may be one of @b wxCAP_ROUND, @b
        wxCAP_PROJECTING and
        @b wxCAP_BUTT. The default is @b wxCAP_ROUND.
        
        @see SetCap()
    */
    int GetCap() const;

    /**
        Returns a reference to the pen colour.
        
        @see SetColour()
    */
    wxColour GetColour() const;

    /**
        Gets an array of dashes (defined as char in X, DWORD under Windows).
        @a dashes is a pointer to the internal array. Do not deallocate or store this
        pointer.
        The function returns the number of dashes associated with this pen.
        
        @see SetDashes()
    */
    int GetDashes(wxDash** dashes) const;

    /**
        Returns the pen join style, which may be one of @b wxJOIN_BEVEL, @b
        wxJOIN_ROUND and
        @b wxJOIN_MITER. The default is @b wxJOIN_ROUND.
        
        @see SetJoin()
    */
    int GetJoin() const;

    /**
        Gets a pointer to the stipple bitmap.
        
        @see SetStipple()
    */
    wxBitmap* GetStipple() const;

    /**
        Returns the pen style.
        
        @see wxPen(), SetStyle()
    */
    int GetStyle() const;

    /**
        Returns the pen width.
        
        @see SetWidth()
    */
    int GetWidth() const;

    /**
        Returns @true if the pen is initialised.
    */
    bool IsOk() const;

    /**
        Sets the pen cap style, which may be one of @b wxCAP_ROUND, @b wxCAP_PROJECTING
        and
        @b wxCAP_BUTT. The default is @b wxCAP_ROUND.
        
        @see GetCap()
    */
    void SetCap(int capStyle);

    //@{
    /**
        The pen's colour is changed to the given colour.
        
        @see GetColour()
    */
    void SetColour(wxColour& colour);
    void SetColour(const wxString& colourName);
    void SetColour(unsigned char red, unsigned char green,
                   unsigned char blue);
    //@}

    /**
        Associates an array of pointers to dashes (defined as char in X, DWORD under
        Windows)
        with the pen. The array is not deallocated by wxPen, but neither must it be
        deallocated by the calling application until the pen is deleted or this
        function is called with a @NULL array.
        
        @see GetDashes()
    */
    void SetDashes(int n, wxDash* dashes);

    /**
        Sets the pen join style, which may be one of @b wxJOIN_BEVEL, @b wxJOIN_ROUND
        and
        @b wxJOIN_MITER. The default is @b wxJOIN_ROUND.
        
        @see GetJoin()
    */
    void SetJoin(int join_style);

    /**
        Sets the bitmap for stippling.
        
        @see GetStipple()
    */
    void SetStipple(wxBitmap* stipple);

    /**
        Set the pen style.
        
        @see wxPen()
    */
    void SetStyle(int style);

    /**
        Sets the pen width.
        
        @see GetWidth()
    */
    void SetWidth(int width);

    /**
        Inequality operator.
        See @ref overview_refcountequality "reference-counted object comparison" for
        more info.
    */
    bool operator !=(const wxPen& pen);

    /**
        Assignment operator, using @ref overview_trefcount "reference counting".
    */
    wxPen operator =(const wxPen& pen);

    /**
        Equality operator.
        See @ref overview_refcountequality "reference-counted object comparison" for
        more info.
    */
    bool operator ==(const wxPen& pen);
};


/**
    FIXME
*/
wxPen Objects:
;

/**
    FIXME
*/
wxPen wxNullPen;

/**
    FIXME
*/
wxPen Pointers:
;

/**
    FIXME
*/
wxPen wxRED_PEN;

/**
    FIXME
*/
wxPen wxCYAN_PEN;

/**
    FIXME
*/
wxPen wxGREEN_PEN;

/**
    FIXME
*/
wxPen wxBLACK_PEN;

/**
    FIXME
*/
wxPen wxWHITE_PEN;

/**
    FIXME
*/
wxPen wxTRANSPARENT_PEN;

/**
    FIXME
*/
wxPen wxBLACK_DASHED_PEN;

/**
    FIXME
*/
wxPen wxGREY_PEN;

/**
    FIXME
*/
wxPen wxMEDIUM_GREY_PEN;

/**
    FIXME
*/
wxPen wxLIGHT_GREY_PEN;


