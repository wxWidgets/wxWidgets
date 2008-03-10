/////////////////////////////////////////////////////////////////////////////
// Name:        brush.h
// Purpose:     interface of wxBrush
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxBrush
    @wxheader{brush.h}

    A brush is a drawing tool for filling in areas. It is used for painting
    the background of rectangles, ellipses, etc. It has a colour and a
    style.

    @library{wxcore}
    @category{gdi}

    @stdobjects
    ::Objects:, ::wxNullBrush, ::Pointers:, ::wxBLUE_BRUSH, ::wxGREEN_BRUSH,
    ::wxWHITE_BRUSH, ::wxBLACK_BRUSH, ::wxGREY_BRUSH, ::wxMEDIUM_GREY_BRUSH, ::wxLIGHT_GREY_BRUSH, ::wxTRANSPARENT_BRUSH, ::wxCYAN_BRUSH, ::wxRED_BRUSH,

    @see wxBrushList, wxDC, wxDC::SetBrush
*/
class wxBrush : public wxGDIObject
{
public:
    //@{
    /**
        Copy constructor, uses @ref overview_trefcount "reference counting".
        
        @param colour
            Colour object.
        @param colourName
            Colour name. The name will be looked up in the colour database.
        @param style
            One of:
        
        
        
        
        
        
            wxTRANSPARENT
        
        
        
        
            Transparent (no fill).
        
        
        
        
        
            wxSOLID
        
        
        
        
            Solid.
        
        
        
        
        
            wxSTIPPLE
        
        
        
        
            Uses a bitmap as a stipple.
        
        
        
        
        
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
        @param brush
            Pointer or reference to a brush to copy.
        @param stippleBitmap
            A bitmap to use for stippling.
        
        @remarks If a stipple brush is created, the brush style will be set to
                 wxSTIPPLE.
        
        @see wxBrushList, wxColour, wxColourDatabase
    */
    wxBrush();
    wxBrush(const wxColour& colour, int style = wxSOLID);
    wxBrush(const wxString& colourName, int style);
    wxBrush(const wxBitmap& stippleBitmap);
    wxBrush(const wxBrush& brush);
    //@}

    /**
        Destructor.
        See @ref overview_refcountdestruct "reference-counted object destruction" for
        more info.
        
        @remarks Although all remaining brushes are deleted when the application
                 exits, the application should try to clean up all
                 brushes itself. This is because wxWidgets cannot know
                 if a pointer to the brush object is stored in an
                 application data structure, and there is a risk of
                 double deletion.
    */
    ~wxBrush();

    /**
        Returns a reference to the brush colour.
        
        @see SetColour()
    */
    wxColour GetColour() const;

    /**
        Gets a pointer to the stipple bitmap. If the brush does not have a wxSTIPPLE
        style,
        this bitmap may be non-@NULL but uninitialised (@ref wxBitmap::isok
        wxBitmap:IsOk returns @false).
        
        @see SetStipple()
    */
    wxBitmap* GetStipple() const;

    /**
        Returns the brush style, one of:
        
        @b wxTRANSPARENT
        
        Transparent (no fill).
        
        @b wxSOLID
        
        Solid.
        
        @b wxBDIAGONAL_HATCH
        
        Backward diagonal hatch.
        
        @b wxCROSSDIAG_HATCH
        
        Cross-diagonal hatch.
        
        @b wxFDIAGONAL_HATCH
        
        Forward diagonal hatch.
        
        @b wxCROSS_HATCH
        
        Cross hatch.
        
        @b wxHORIZONTAL_HATCH
        
        Horizontal hatch.
        
        @b wxVERTICAL_HATCH
        
        Vertical hatch.
        
        @b wxSTIPPLE
        
        Stippled using a bitmap.
        
        @b wxSTIPPLE_MASK_OPAQUE
        
        Stippled using a bitmap's mask.
        
        @see SetStyle(), SetColour(), SetStipple()
    */
    int GetStyle() const;

    /**
        Returns @true if the style of the brush is any of hatched fills.
        
        @see GetStyle()
    */
    bool IsHatch() const;

    /**
        Returns @true if the brush is initialised. It will return @false if the default
        constructor has been used (for example, the brush is a member of a class, or
        @NULL has been assigned to it).
    */
    bool IsOk() const;

    //@{
    /**
        Sets the brush colour using red, green and blue values.
        
        @see GetColour()
    */
    void SetColour(wxColour& colour);
    void SetColour(const wxString& colourName);
    void SetColour(unsigned char red, unsigned char green,
                   unsigned char blue);
    //@}

    /**
        Sets the stipple bitmap.
        
        @param bitmap
            The bitmap to use for stippling.
        
        @remarks The style will be set to wxSTIPPLE, unless the bitmap has a mask
                 associated to it, in which case the style will be set
                 to wxSTIPPLE_MASK_OPAQUE.
        
        @see wxBitmap
    */
    void SetStipple(const wxBitmap& bitmap);

    /**
        Sets the brush style.
        
        @param style
            One of:
        
        
        
        
        
        
            wxTRANSPARENT
        
        
        
        
            Transparent (no fill).
        
        
        
        
        
            wxSOLID
        
        
        
        
            Solid.
        
        
        
        
        
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
        
        
        
        
        
            wxSTIPPLE
        
        
        
        
            Stippled using a bitmap.
        
        
        
        
        
            wxSTIPPLE_MASK_OPAQUE
        
        
        
        
            Stippled using a bitmap's mask.
        
        @see GetStyle()
    */
    void SetStyle(int style);

    /**
        Inequality operator.
        See @ref overview_refcountequality "reference-counted object comparison" for
        more info.
    */
    bool operator !=(const wxBrush& brush);

    /**
        Assignment operator, using @ref overview_trefcount "reference counting".
    */
    wxBrush operator =(const wxBrush& brush);

    /**
        Equality operator.
        See @ref overview_refcountequality "reference-counted object comparison" for
        more info.
    */
    bool operator ==(const wxBrush& brush);
};


/**
    FIXME
*/
wxBrush Objects:
;

/**
    FIXME
*/
wxBrush wxNullBrush;

/**
    FIXME
*/
wxBrush Pointers:
;

/**
    FIXME
*/
wxBrush wxBLUE_BRUSH;

/**
    FIXME
*/
wxBrush wxGREEN_BRUSH;

/**
    FIXME
*/
wxBrush wxWHITE_BRUSH;

/**
    FIXME
*/
wxBrush wxBLACK_BRUSH;

/**
    FIXME
*/
wxBrush wxGREY_BRUSH;

/**
    FIXME
*/
wxBrush wxMEDIUM_GREY_BRUSH;

/**
    FIXME
*/
wxBrush wxLIGHT_GREY_BRUSH;

/**
    FIXME
*/
wxBrush wxTRANSPARENT_BRUSH;

/**
    FIXME
*/
wxBrush wxCYAN_BRUSH;

/**
    FIXME
*/
wxBrush wxRED_BRUSH;


