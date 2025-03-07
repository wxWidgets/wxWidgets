/////////////////////////////////////////////////////////////////////////////
// Name:        pen.h
// Purpose:     interface of wxPen* classes
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    The possible styles for a wxPen.

    Note that hatched pen styles are not supported by X11-based ports,
    including wxGTK.
*/
enum wxPenStyle
{
    wxPENSTYLE_INVALID = -1,

    wxPENSTYLE_SOLID,
        /**< Solid style. */

    wxPENSTYLE_DOT,
        /**< Dotted style. */

    wxPENSTYLE_LONG_DASH,
        /**< Long dashed style. */

    wxPENSTYLE_SHORT_DASH,
        /**< Short dashed style. */

    wxPENSTYLE_DOT_DASH,
        /**< Dot and dash style. */

    wxPENSTYLE_USER_DASH,
        /**< Use the user dashes: see wxPen::SetDashes. */

    wxPENSTYLE_TRANSPARENT,
        /**< No pen is used. */

    wxPENSTYLE_STIPPLE,
        /**< Use the stipple bitmap. */

    wxPENSTYLE_BDIAGONAL_HATCH,
        /**< Backward diagonal hatch. */

    wxPENSTYLE_CROSSDIAG_HATCH,
        /**< Cross-diagonal hatch. */

    wxPENSTYLE_FDIAGONAL_HATCH,
        /**< Forward diagonal hatch. */

    wxPENSTYLE_CROSS_HATCH,
        /**< Cross hatch. */

    wxPENSTYLE_HORIZONTAL_HATCH,
        /**< Horizontal hatch. */

    wxPENSTYLE_VERTICAL_HATCH,
        /**< Vertical hatch. */

    wxPENSTYLE_FIRST_HATCH,
        /**< First of the hatch styles (inclusive). */

    wxPENSTYLE_LAST_HATCH
        /**< Last of the hatch styles (inclusive). */
};

/**
    Possible values for pen quality.

    Pen quality is currently only used in wxMSW; the other ports ignore it and
    always use the same default pen quality.

    In wxMSW, the choice of quality affects whether "cosmetic" or "geometric"
    native pens are used in situations when both are usable. Notably, for
    dotted and dashed pens of width 1, high quality geometric pens are used by
    default since wxWidgets 3.1.4, while previous versions used lower quality
    (but much faster) cosmetic pens. If drawing performance is more important
    than the exact appearance of the lines drawn using this pen, low quality
    may be explicitly selected.

    See wxPenInfo::Quality() and wxPen::SetQuality().

    @since 3.1.5
 */
enum wxPenQuality
{
    wxPEN_QUALITY_DEFAULT,  ///< Select the appropriate quality automatically.
    wxPEN_QUALITY_LOW,      ///< Less good looking but faster.
    wxPEN_QUALITY_HIGH      ///< Best looking, at the expense of speed.
};

/**
    The possible join values of a wxPen.

    @todo use wxPENJOIN_ prefix
*/
enum wxPenJoin
{
    wxJOIN_INVALID = -1,

    wxJOIN_BEVEL = 120,
        /**< The intersection of two lines will have
             a diagonal edge "cut" into it. */
    wxJOIN_MITER,
        /**< The intersection of two lines will have
             a sharp corner. */
    wxJOIN_ROUND,
        /**< The intersection of two lines will have
             a rounded edge. */
};


/**
    The possible cap values of a wxPen.

    @todo use wxPENCAP_ prefix
*/
enum wxPenCap
{
    wxCAP_INVALID = -1,

    wxCAP_ROUND = 130,
        /**< The pen will have a rounded edge and will extend
             beyond the start and end of a line. */
    wxCAP_PROJECTING,
        /**< The pen's edge will be square and will extend
             beyond the start and end of a line. */
    wxCAP_BUTT
        /**< The pen's edge will be square and will not extend
             beyond the start and end of a line. */
};



/**
    @class wxPenInfo

    This class is a helper used for wxPen creation using named parameter
    idiom: it allows specifying various wxPen attributes using the chained
    calls to its clearly named methods instead of passing them in the fixed
    order to wxPen constructors.

    For instance, to create a dotted blue pen with the given join style you
    could do
    @code
    wxPen pen(wxPenInfo(*wxBLUE).Style(wxPENSTYLE_DOT).Join(wxJOIN_BEVEL));
    @endcode

    @since 3.1.1
 */
class wxPenInfo
{
public:
    /**
        Constructor, which can set the colour, width and style for the pen.
    */
    explicit wxPenInfo(const wxColour& colour = wxColour(),
                       int width = 1,
                       wxPenStyle style = wxPENSTYLE_SOLID);

    /**
        Sets the colour for the pen.
    */
    wxPenInfo& Colour(const wxColour& col);

    /**
        Sets the line width for the pen.
    */
    wxPenInfo& Width(int width);

    /**
        Sets the style for the pen.
    */
    wxPenInfo& Style(wxPenStyle style);

    /**
        Sets the bitmap used for stippling.

        When the pen is used, a repeating pattern of this bitmap will be drawn.
    */
    wxPenInfo& Stipple(const wxBitmap& stipple);

    /**
        Sets the dash pattern used to draw the pen's line.
    */
    wxPenInfo& Dashes(int nb_dashes, const wxDash *dash);

    /**
        Sets the join for the pen, which is the appearance of
        where two lines meet or overlap.
    */
    wxPenInfo& Join(wxPenJoin join);

    /**
        Sets the cap (i.e., the end point) for the pen.
    */
    wxPenInfo& Cap(wxPenCap cap);

    /**
        Set the pen quality.

        Using LowQuality() or HighQuality() is usually more convenient.

        @see wxPen::SetQuality()

        @since 3.1.5
     */
    wxPenInfo& Quality(wxPenQuality quality);

    /**
        Set low pen quality.

        This is the same as calling Quality() with ::wxPEN_QUALITY_LOW.

        @since 3.1.5
     */
    wxPenInfo& LowQuality();

    /**
        Set high pen quality.

        This is the same as calling Quality() with ::wxPEN_QUALITY_HIGH.

        @since 3.1.5
     */
    wxPenInfo& HighQuality();

    /**
        Returns the pen's colour.
    */
    wxColour GetColour() const;
    /**
        Returns the pen's stipple bitmap.
    */
    wxBitmap GetStipple() const;
    /**
        Returns the pen's style.
    */
    wxPenStyle GetStyle() const;
    /**
        Returns the pen's joining method.
    */
    wxPenJoin GetJoin() const;
    /**
        Returns the pen's cap (i.e., end-point style).
    */
    wxPenCap GetCap() const;
    /**
        Returns the pen's quality.
    */
    wxPenQuality GetQuality() const;
    /**
        Returns the pen's dash pattern and
        number of dashes in the pattern.
    */
    int GetDashes(wxDash **ptr);
    /**
        Returns the number of dashes in the
        pen's dash pattern.
    */
    int GetDashCount() const;
    /**
        Returns the pen's dash pattern.
    */
    wxDash* GetDash() const;
    /**
        Returns whether the pen is transparent.
    */
    bool IsTransparent() const;
    /**
        Returns the pen's line width.
    */
    int GetWidth() const;
};



/**
    @class wxPen

    A pen is a tool for drawing outlines. It is used for drawing
    lines and painting the outline of rectangles, ellipses, etc.
    It has a colour, a width and a style.

    @note On a monochrome display, wxWidgets shows all non-white pens as black.

    Do not initialize objects on the stack before the program commences,
    since other required structures may not have been set up yet.
    Instead, define global pointers to objects and create them in wxApp::OnInit()
    or when required.

    An application may wish to dynamically create pens with different characteristics,
    and there is the consequent danger that a large number of duplicate pens will
    be created. Therefore, an application may wish to get a pointer to a pen by using
    the global list of pens, ::wxThePenList, and calling the member function
    wxPenList::FindOrCreatePen().
    See wxPenList for more info.

    This class uses @ref overview_refcount "reference counting and copy-on-write" internally
    so that assignments between two instances of this class are very cheap.
    You can therefore use actual objects instead of pointers without efficiency problems.
    If an instance of this class is changed, it will create its own data internally
    so that other instances, which previously shared the data using the reference
    counting, are not affected.

    @library{wxcore}
    @category{gdi}

    @stdobjects
    @li ::wxNullPen
    @li ::wxBLACK_DASHED_PEN
    @li ::wxBLACK_PEN
    @li ::wxBLUE_PEN
    @li ::wxCYAN_PEN
    @li ::wxGREEN_PEN
    @li ::wxYELLOW_PEN
    @li ::wxGREY_PEN
    @li ::wxLIGHT_GREY_PEN
    @li ::wxMEDIUM_GREY_PEN
    @li ::wxRED_PEN
    @li ::wxTRANSPARENT_PEN
    @li ::wxWHITE_PEN

    @see wxPenList, wxDC, wxDC::SetPen()
*/
class wxPen : public wxGDIObject
{
public:
    /**
        Default constructor. The pen will be uninitialised, and IsOk() will return @false.
    */
    wxPen();

    /**
        Creates a pen object using the specified pen description.
    */
    wxPen(const wxPenInfo& info);

    /**
        Constructs a pen from a colour object, pen width and style.

        @param colour
            A colour object.
        @param width
            Pen width. Under Windows, the pen width cannot be greater than 1 if
            the style is @c wxPENSTYLE_DOT, @c wxPENSTYLE_LONG_DASH, @c wxPENSTYLE_SHORT_DASH,
            @c wxPENSTYLE_DOT_DASH, or @c wxPENSTYLE_USER_DASH.
        @param style
            The style may be one of the ::wxPenStyle values.

        @remarks Different versions of Windows and different versions of other
                 platforms support very different subsets of the styles above,
                 so handle with care.

        @see SetStyle(), SetColour(), SetWidth()
    */
    wxPen(const wxColour& colour, int width = 1, wxPenStyle style = wxPENSTYLE_SOLID);

    /**
        Constructs a stippled pen from a stipple bitmap and a width.

        @param width
            Pen width. Under Windows, the pen width cannot be greater than 1 if
            the style is @c wxPENSTYLE_DOT, @c wxPENSTYLE_LONG_DASH, @c wxPENSTYLE_SHORT_DASH,
            @c wxPENSTYLE_DOT_DASH, or @c wxPENSTYLE_USER_DASH.
        @param stipple
            A stipple bitmap.

        @onlyfor{wxmsw,wxosx}

        @see SetWidth(), SetStipple()
    */
    wxPen(const wxBitmap& stipple, int width);

    /**
        Copy constructor, uses @ref overview_refcount.

        @param pen
            A pointer or reference to a pen to copy.
    */
    wxPen(const wxPen& pen);

    /**
        Returns the pen cap style, which may be one of @c wxCAP_ROUND,
        @c wxCAP_PROJECTING or @c wxCAP_BUTT.

        The default is @c wxCAP_ROUND.

        @see SetCap()
    */
    virtual wxPenCap GetCap() const;

    /**
        Returns the pen quality.

        The default is ::wxPEN_QUALITY_DEFAULT.

        @see wxPenQuality, SetQuality()
     */
    wxPenQuality GetQuality() const;

    /**
        Returns a reference to the pen colour.

        @see SetColour()
    */
    virtual wxColour GetColour() const;

    /**
        Gets an array of dashes (defined as @c char in X, @c DWORD under Windows).
        @a dashes is a pointer to the internal array. Do not deallocate or store this
        pointer.

        @return The number of dashes associated with this pen.

        @see SetDashes()
    */
    virtual int GetDashes(wxDash** dashes) const;

    /**
        Returns the pen join style, which may be one of @c wxJOIN_BEVEL,
        @c wxJOIN_ROUND or @c wxJOIN_MITER.

        The default is @c wxJOIN_ROUND.

        @see SetJoin()
    */
    virtual wxPenJoin GetJoin() const;

    /**
        Gets a pointer to the stipple bitmap.

        @see SetStipple()
    */
    virtual wxBitmap* GetStipple() const;

    /**
        Returns the pen style.

        @see wxPen(), SetStyle()
    */
    virtual wxPenStyle GetStyle() const;

    /**
        Returns the pen width.

        @see SetWidth()
    */
    virtual int GetWidth() const;

    /**
        Returns @true if the pen is initialised.

        Notice that an uninitialized pen object can't be queried for any pen
        properties and all calls to the accessor methods on it will result in
        an assert failure.
    */
    virtual bool IsOk() const;

    /**
        Returns @true if the pen is a valid non-transparent pen.

        This method returns @true if the pen object is initialized and has a
        non-transparent style. Notice that this should be used instead of
        simply testing whether GetStyle() returns a style different from
        wxPENSTYLE_TRANSPARENT if the pen may be invalid as GetStyle() would
        assert in this case.

        @see IsTransparent()

        @since 2.9.2.
     */
    bool IsNonTransparent() const;

    /**
        Returns @true if the pen is transparent.

        A transparent pen is simply a pen with wxPENSTYLE_TRANSPARENT style.

        Notice that this function works even for non-initialized pens (for
        which it returns @false) unlike tests of the form <code>GetStyle() ==
        wxPENSTYLE_TRANSPARENT</code> which would assert if the pen is invalid.

        @see IsNonTransparent()

        @since 2.9.2.
     */
    bool IsTransparent() const;

    /**
        Sets the pen cap style, which may be one of @c wxCAP_ROUND, @c wxCAP_PROJECTING
        or @c wxCAP_BUTT. The default is @c wxCAP_ROUND.

        @see GetCap()
    */
    virtual void SetCap(wxPenCap capStyle);

    /**
        Sets the pen quality.

        Explicitly selecting low pen quality may be useful in wxMSW if drawing
        performance is more important than the exact appearance of the lines
        drawn with this pen.

        @see wxPenQuality

        @since 3.1.5
     */
    void SetQuality(wxPenQuality quality);

    ///@{
    /**
        The pen's colour is changed to the given colour.

        @see GetColour()
    */
    virtual void SetColour(wxColour& colour);
    virtual void SetColour(unsigned char red, unsigned char green, unsigned char blue);
    ///@}

    /**
        Associates an array of dash values (defined as @c char in X, @c DWORD under
        Windows) with the pen.

        The array is not deallocated by wxPen, but neither must it be deallocated by
        the calling application until the pen is deleted or this function is called
        with a @NULL array.

        @see GetDashes()
    */
    virtual void SetDashes(int n, const wxDash* dash);

    /**
        Sets the pen join style, which may be one of @c wxJOIN_BEVEL, @c wxJOIN_ROUND
        or @c wxJOIN_MITER.

        The default is @c wxJOIN_ROUND.

        @see GetJoin()
    */
    virtual void SetJoin(wxPenJoin join_style);

    /**
        Sets the bitmap for stippling.

        @see GetStipple()
    */
    virtual void SetStipple(const wxBitmap& stipple);

    /**
        Set the pen style.

        @see wxPen()
    */
    virtual void SetStyle(wxPenStyle style);

    /**
        Sets the pen width.

        @see GetWidth()
    */
    virtual void SetWidth(int width);

    /**
        Inequality operator.

        See @ref overview_refcount_equality "reference-counted object comparison" for
        more info.
    */
    bool operator!=(const wxPen& pen) const;

    /**
        Assignment operator, using @ref overview_refcount.
    */
    wxPen& operator=(const wxPen& pen);

    /**
        Equality operator.

        See @ref overview_refcount_equality "reference-counted object comparison" for
        more info.
    */
    bool operator==(const wxPen& pen) const;
};

/**
    An empty pen.
    wxPen::IsOk() always returns @false for this object.
*/
wxPen wxNullPen;

/**
    Red pen.
    Except for the color it has all standard attributes
    (1-pixel width, @c wxPENSTYLE_SOLID and @c wxCAP_ROUND styles, etc...).
*/
wxPen* wxRED_PEN;

/**
    Blue pen.
    Except for the color it has all standard attributes
    (1-pixel width, @c wxPENSTYLE_SOLID and @c wxCAP_ROUND styles, etc...).
*/
wxPen* wxBLUE_PEN;

/**
    Cyan pen.
    Except for the color it has all standard attributes
    (1-pixel width, @c wxPENSTYLE_SOLID and @c wxCAP_ROUND styles, etc...).
*/
wxPen* wxCYAN_PEN;

/**
    Green pen.
    Except for the color it has all standard attributes
    (1-pixel width, @c wxPENSTYLE_SOLID and @c wxCAP_ROUND styles, etc...).
*/
wxPen* wxGREEN_PEN;

/**
    Yellow pen.
    Except for the color it has all standard attributes
    (1-pixel width, @c wxPENSTYLE_SOLID and @c wxCAP_ROUND styles, etc...).
*/
wxPen* wxYELLOW_PEN;

/**
    Black pen.
    Except for the color it has all standard attributes
    (1-pixel width, @c wxPENSTYLE_SOLID and @c wxCAP_ROUND styles, etc...).
*/
wxPen* wxBLACK_PEN;

/**
    White pen.
    Except for the color it has all standard attributes
    (1-pixel width, @c wxPENSTYLE_SOLID and @c wxCAP_ROUND styles, etc...).
*/
wxPen* wxWHITE_PEN;

/**
    Transparent pen.
    Except for the color it has all standard attributes
    (1-pixel width, @c wxPENSTYLE_SOLID and @c wxCAP_ROUND styles, etc...).
*/
wxPen* wxTRANSPARENT_PEN;

/**
    Black dashed pen.
    Except for the color and for the @c wxPENSTYLE_SHORT_DASH it has all standard attributes
    (1-pixel width, @c wxCAP_ROUND style, etc...).
*/
wxPen* wxBLACK_DASHED_PEN;

/**
    Grey pen.
    Except for the color it has all standard attributes
    (1-pixel width, @c wxPENSTYLE_SOLID and @c wxCAP_ROUND styles, etc...).
*/
wxPen* wxGREY_PEN;

/**
    Medium-grey pen.
    Except for the color it has all standard attributes
    (1-pixel width, @c wxPENSTYLE_SOLID and @c wxCAP_ROUND styles, etc...).
*/
wxPen* wxMEDIUM_GREY_PEN;

/**
    Light-grey pen.
    Except for the color it has all standard attributes
    (1-pixel width, @c wxPENSTYLE_SOLID and @c wxCAP_ROUND styles, etc...).
*/
wxPen* wxLIGHT_GREY_PEN;



/**
    @class wxPenList

    There is only one instance of this class: ::wxThePenList.
    Use this object to search for a previously created pen of the desired
    type and create it if not already found. In some windowing systems,
    the pen may be a scarce resource, so it can pay to reuse old
    resources if possible. When an application finishes, all pens will
    be deleted and their resources freed, eliminating the possibility of
    'memory leaks'. However, it is best not to rely on this automatic
    cleanup because it can lead to double deletion in some circumstances.

    There are two mechanisms in recent versions of wxWidgets which make the
    pen list less useful than it once was. Under Windows, scarce resources
    are cleaned up internally if they are not being used. Also, a referencing
    counting mechanism applied to all GDI objects means that some sharing
    of underlying resources is possible. You don't have to keep track of pointers,
    working out when it is safe delete a pen, because the referencing counting does
    it for you. For example, you can set a pen in a device context and then
    immediately delete the pen you passed because the pen is 'copied'.

    So you may find it easier to ignore the pen list and instead create
    and copy pens as you see fit. If your Windows resource meter suggests
    your application is using too many resources, you can resort to using
    GDI lists to share objects explicitly.

    The only compelling use for the pen list is for wxWidgets to keep
    track of pens in order to clean them up on exit. It is also kept for
    backward compatibility with earlier versions of wxWidgets.

    @library{wxcore}
    @category{gdi}

    @stdobjects
    ::wxThePenList

    @see wxPen
*/
class wxPenList
{
public:
    /**
        Constructor. The application should not construct its own pen list;
        use the object pointer ::wxThePenList.
    */
    wxPenList();

    /**
        Finds a pen with the specified attributes and returns it; otherwise,
        creates a new pen, adds it to the pen list, and returns it.

        @param colour
            Colour object.
        @param width
            Width of pen.
        @param style
            Pen style. See ::wxPenStyle for a list of styles.
    */
    wxPen* FindOrCreatePen(const wxColour& colour,
                           int width = 1,
                           wxPenStyle style = wxPENSTYLE_SOLID);
};

/**
    The global list of wxPen objects ready to be re-used (for better performances).
*/
wxPenList* wxThePenList;

