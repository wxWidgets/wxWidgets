/////////////////////////////////////////////////////////////////////////////
// Name:        pen.h
// Purpose:     interface of wxPen* classes
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    The possible styles for a wxPen.
*/
enum wxPenStyle
{
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

    wxPENSTYLE_STIPPLE_MASK_OPAQUE,
        /**< @todo WHAT's this? */

    wxPENSTYLE_STIPPLE_MASK,
        /**< @todo WHAT's this? */

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

    wxPENSTYLE_FIRST_HATCH = wxPENSTYLE_BDIAGONAL_HATCH,
    wxPENSTYLE_LAST_HATCH = wxPENSTYLE_VERTICAL_HATCH,
    wxPENSTYLE_MAX
};

/**
    The possible join values of a wxPen.

    @todo use wxPENJOIN_ prefix
*/
enum wxPenJoin
{
    wxJOIN_INVALID = -1,

    wxJOIN_BEVEL = 120,
    wxJOIN_MITER,
    wxJOIN_ROUND,
};


/**
    The possible cap values of a wxPen.

    @todo use wxPENCAP_ prefix
*/
enum wxPenCap
{
    wxCAP_INVALID = -1,

    wxCAP_ROUND = 130,
    wxCAP_PROJECTING,
    wxCAP_BUTT
};



/**
    @class wxPen
    @wxheader{pen.h}

    A pen is a drawing tool for drawing outlines. It is used for drawing
    lines and painting the outline of rectangles, ellipses, etc.
    It has a colour, a width and a style.

    On a monochrome display, wxWidgets shows all non-white pens as black.

    Do not initialize objects on the stack before the program commences,
    since other required structures may not have been set up yet.
    Instead, define global pointers to objects and create them in wxApp::OnInit
    or when required.

    An application may wish to dynamically create pens with different characteristics,
    and there is the consequent danger that a large number of duplicate pens will
    be created. Therefore an application may wish to get a pointer to a pen by using
    the global list of pens wxThePenList, and calling the member function
    wxPenList::FindOrCreatePen().
    See the entry for wxPenList.

    This class uses reference counting and copy-on-write internally so that
    assignments between two instances of this class are very cheap.
    You can therefore use actual objects instead of pointers without efficiency problems.
    If an instance of this class is changed it will create its own data internally
    so that other instances, which previously shared the data using the reference
    counting, are not affected.

    @library{wxcore}
    @category{gdi}

    @stdobjects
    ::wxNullPen, ::wxRED_PEN, ::wxCYAN_PEN, ::wxGREEN_PEN, ::wxBLACK_PEN,
    ::wxWHITE_PEN, ::wxTRANSPARENT_PEN, ::wxBLACK_DASHED_PEN, ::wxGREY_PEN,
    ::wxMEDIUM_GREY_PEN, ::wxLIGHT_GREY_PEN

    @see wxPenList, wxDC, wxDC::SetPen
*/
class wxPen : public wxGDIObject
{
public:
    /**
        Default constructor. The pen will be uninitialised, and wxPen:IsOk will return @false.
    */
    wxPen();

    /**
        Constructs a pen from a colour object, pen width and style.
    */
    wxPen(const wxColour& colour, int width = 1, wxPenStyle style = wxSOLID);

    /**
        Constructs a pen from a colour name, pen width and style.
    */
    wxPen(const wxString& colourName, int width, wxPenStyle style);

    /**
        Constructs a stippled pen from a stipple bitmap and a width.
    */
    wxPen(const wxBitmap& stipple, int width);

    /**
        Copy constructor, uses @ref overview_refcount "reference counting".

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
            The style may be one of the ::wxPenStyle values.

        @remarks Different versions of Windows and different versions of other
                 platforms support very different subsets of the styles
                 above - there is no similarity even between Windows95
                 and Windows98 - so handle with care.

        @see SetStyle(), SetColour(), SetWidth(), SetStipple()
    */
    wxPen(const wxPen& pen);

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
    wxPenCap GetCap() const;

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
    wxPenJoin GetJoin() const;

    /**
        Gets a pointer to the stipple bitmap.

        @see SetStipple()
    */
    wxBitmap* GetStipple() const;

    /**
        Returns the pen style.

        @see wxPen(), SetStyle()
    */
    wxPenStyle GetStyle() const;

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
    void SetCap(wxPenCap capStyle);

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
    void SetJoin(wxPenJoin join_style);

    /**
        Sets the bitmap for stippling.

        @see GetStipple()
    */
    void SetStipple(wxBitmap* stipple);

    /**
        Set the pen style.

        @see wxPen()
    */
    void SetStyle(wxPenStyle style);

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
wxPen wxNullPen;

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



/**
    @class wxPenList
    @wxheader{gdicmn.h}

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
    it for you. For example, you can set a pen in a device context, and then
    immediately delete the pen you passed, because the pen is 'copied'.

    So you may find it easier to ignore the pen list, and instead create
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
        Constructor. The application should not construct its own pen list:
        use the object pointer ::wxThePenList.
    */
    wxPenList();

    /**
        Finds a pen with the specified attributes and returns it, else creates a
        new pen, adds it to the pen list, and returns it.

        @param colour
            Colour object.
        @param width
            Width of pen.
        @param style
            Pen style. See ::wxPenStyle for a list of styles.
    */
    wxPen* FindOrCreatePen(const wxColour& colour, int width, wxPenStyle style);
};

/**
    The global list of wxPen objects ready to be re-used (for better performances).
*/
wxPenList* wxThePenList;

