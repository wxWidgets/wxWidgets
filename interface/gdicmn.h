/////////////////////////////////////////////////////////////////////////////
// Name:        gdicmn.h
// Purpose:     interface of wxRealPoint
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


/**
    Bitmap flags.
*/
enum wxBitmapType
{
    wxBITMAP_TYPE_INVALID,
    wxBITMAP_TYPE_BMP,
    wxBITMAP_TYPE_BMP_RESOURCE,
    wxBITMAP_TYPE_RESOURCE = wxBITMAP_TYPE_BMP_RESOURCE,
    wxBITMAP_TYPE_ICO,
    wxBITMAP_TYPE_ICO_RESOURCE,
    wxBITMAP_TYPE_CUR,
    wxBITMAP_TYPE_CUR_RESOURCE,
    wxBITMAP_TYPE_XBM,
    wxBITMAP_TYPE_XBM_DATA,
    wxBITMAP_TYPE_XPM,
    wxBITMAP_TYPE_XPM_DATA,
    wxBITMAP_TYPE_TIF,
    wxBITMAP_TYPE_TIF_RESOURCE,
    wxBITMAP_TYPE_GIF,
    wxBITMAP_TYPE_GIF_RESOURCE,
    wxBITMAP_TYPE_PNG,
    wxBITMAP_TYPE_PNG_RESOURCE,
    wxBITMAP_TYPE_JPEG,
    wxBITMAP_TYPE_JPEG_RESOURCE,
    wxBITMAP_TYPE_PNM,
    wxBITMAP_TYPE_PNM_RESOURCE,
    wxBITMAP_TYPE_PCX,
    wxBITMAP_TYPE_PCX_RESOURCE,
    wxBITMAP_TYPE_PICT,
    wxBITMAP_TYPE_PICT_RESOURCE,
    wxBITMAP_TYPE_ICON,
    wxBITMAP_TYPE_ICON_RESOURCE,
    wxBITMAP_TYPE_ANI,
    wxBITMAP_TYPE_IFF,
    wxBITMAP_TYPE_TGA,
    wxBITMAP_TYPE_MACCURSOR,
    wxBITMAP_TYPE_MACCURSOR_RESOURCE,
    wxBITMAP_TYPE_ANY = 50
};

/**
    Standard cursors.
*/
enum wxStockCursor
{
    wxCURSOR_NONE,
    wxCURSOR_ARROW,
    wxCURSOR_RIGHT_ARROW,
    wxCURSOR_BULLSEYE,
    wxCURSOR_CHAR,
    wxCURSOR_CROSS,
    wxCURSOR_HAND,
    wxCURSOR_IBEAM,
    wxCURSOR_LEFT_BUTTON,
    wxCURSOR_MAGNIFIER,
    wxCURSOR_MIDDLE_BUTTON,
    wxCURSOR_NO_ENTRY,
    wxCURSOR_PAINT_BRUSH,
    wxCURSOR_PENCIL,
    wxCURSOR_POINT_LEFT,
    wxCURSOR_POINT_RIGHT,
    wxCURSOR_QUESTION_ARROW,
    wxCURSOR_RIGHT_BUTTON,
    wxCURSOR_SIZENESW,
    wxCURSOR_SIZENS,
    wxCURSOR_SIZENWSE,
    wxCURSOR_SIZEWE,
    wxCURSOR_SIZING,
    wxCURSOR_SPRAYCAN,
    wxCURSOR_WAIT,
    wxCURSOR_WATCH,
    wxCURSOR_BLANK,
    wxCURSOR_DEFAULT,       //!< standard X11 cursor
    wxCURSOR_COPY_ARROW ,   //!< MacOS Theme Plus arrow
    // Not yet implemented for Windows
    wxCURSOR_CROSS_REVERSE,
    wxCURSOR_DOUBLE_ARROW,
    wxCURSOR_BASED_ARROW_UP,
    wxCURSOR_BASED_ARROW_DOWN,
    wxCURSOR_ARROWWAIT,
    wxCURSOR_MAX
};



/**
    @class wxRealPoint
    @wxheader{gdicmn.h}

    A @b wxRealPoint is a useful data structure for graphics operations.
    It contains floating point @e x and @e y members.
    See also wxPoint for an integer version.

    @library{wxcore}
    @category{data}

    @see wxPoint
*/
class wxRealPoint
{
public:
    //@{
    /**
        Create a point.
        double  x
        double  y
        Members of the @b wxRealPoint object.
    */
    wxRealPoint();
    wxRealPoint(double x, double y);
    //@}
};



/**
    @class wxRect
    @wxheader{gdicmn.h}

    A class for manipulating rectangles.

    @library{wxcore}
    @category{data}

    @see wxPoint, wxSize
*/
class wxRect
{
public:
    //@{
    /**
        Creates a wxRect object from size values at the origin.
    */
    wxRect();
    wxRect(int x, int y, int width, int height);
    wxRect(const wxPoint& topLeft, const wxPoint& bottomRight);
    wxRect(const wxPoint& pos, const wxSize& size);
    wxRect(const wxSize& size);
    //@}

    //@{
    /**
        Returns the rectangle having the same size as this one but centered relatively
        to the given rectangle @e r. By default, rectangle is centred in both
        directions but if @a dir includes only @c wxVERTICAL or only
        @c wxHORIZONTAL flag, then it is only centered in this direction while
        the other component of its position remains unchanged.
    */
    wxRect CentreIn(const wxRect& r, int dir = wxBOTH) const;
    const wxRect  CenterIn(const wxRect& r, int dir = wxBOTH) const;
    //@}

    //@{
    /**
        Returns @true if the given rectangle is completely inside this rectangle
        (or touches its boundary) and @false otherwise.
    */
    bool Contains(int x, int y) const;
    const bool Contains(const wxPoint& pt) const;
    const bool Contains(const wxRect& rect) const;
    //@}

    //@{
    /**
        Decrease the rectangle size.
        This method is the opposite from Inflate():
        Deflate(a, b) is equivalent to Inflate(-a, -b).
        Please refer to Inflate() for full description.

        @see Inflate()
    */
    void Deflate(wxCoord dx, wxCoord dy) const;
    void Deflate(const wxSize& diff) const;
    void Deflate(wxCoord diff) const;
    wxRect Deflate(wxCoord dx, wxCoord dy) const;
    //@}

    /**
        Gets the bottom point of the rectangle.
    */
    int GetBottom() const;

    /**
        Gets the position of the bottom left corner.
    */
    wxPoint GetBottomLeft() const;

    /**
        Gets the position of the bottom right corner.
    */
    wxPoint GetBottomRight() const;

    /**
        Gets the height member.
    */
    int GetHeight() const;

    /**
        Gets the left point of the rectangle (the same as wxRect::GetX).
    */
    int GetLeft() const;

    /**
        Gets the position.
    */
    wxPoint GetPosition() const;

    /**
        Gets the right point of the rectangle.
    */
    int GetRight() const;

    /**
        Gets the size.

        @see SetSize()
    */
    wxSize GetSize() const;

    /**
        Gets the top point of the rectangle (the same as wxRect::GetY).
    */
    int GetTop() const;

    /**
        Gets the position of the top left corner of the rectangle, same as
        GetPosition().
    */
    wxPoint GetTopLeft() const;

    /**
        Gets the position of the top right corner.
    */
    wxPoint GetTopRight() const;

    /**
        Gets the width member.
    */
    int GetWidth() const;

    /**
        Gets the x member.
    */
    int GetX() const;

    /**
        Gets the y member.
    */
    int GetY() const;

    //@{
    /**
        Increases the size of the rectangle.
        The second form uses the same @a diff for both @a dx and @e dy.
        The first two versions modify the rectangle in place, the last one returns a
        new rectangle leaving this one unchanged.
        The left border is moved farther left and the right border is moved farther
        right by @e dx. The upper border is moved farther up and the bottom border
        is moved farther down by @e dy. (Note the the width and height of the
        rectangle thus change by 2*@a dx and 2*@e dy, respectively.) If one or
        both of @a dx and @a dy are negative, the opposite happens: the rectangle
        size decreases in the respective direction.
        Inflating and deflating behaves "naturally''. Defined more precisely, that
        means:
             "Real'' inflates (that is, @a dx and/or @a dy = 0) are not
                constrained. Thus inflating a rectangle can cause its upper left corner
                to move into the negative numbers. (the versions prior to 2.5.4 forced
                the top left coordinate to not fall below (0, 0), which implied a
                forced move of the rectangle.)
             Deflates are clamped to not reduce the width or height of the
                rectangle below zero. In such cases, the top-left corner is nonetheless
                handled properly. For example, a rectangle at (10, 10) with size (20,
                40) that is inflated by (-15, -15) will become located at (20, 25) at
                size (0, 10). Finally, observe that the width and height are treated
                independently. In the above example, the width is reduced by 20,
                whereas the height is reduced by the full 30 (rather than also stopping
                at 20, when the width reached zero).

        @see Deflate()
    */
    void Inflate(wxCoord dx, wxCoord dy) const;
    void Inflate(const wxSize& diff) const;
    void Inflate(wxCoord diff) const;
    wxRect Inflate(wxCoord dx, wxCoord dy) const;
    //@}

    //@{
    /**
        Modifies the rectangle to contain the overlapping box of this rectangle and the
        one passed in as parameter. The const version returns the new rectangle, the
        other one modifies this rectangle in place.
    */
    wxRect Intersect(const wxRect& rect);
    const wxRect&  Intersect(const wxRect& rect);
    //@}

    /**
        Returns @true if this rectangle has a non-empty intersection with the
        rectangle @a rect and @false otherwise.
    */
    bool Intersects(const wxRect& rect) const;

    /**
        Returns @true if this rectangle has a width or height less than or equal to
        0 and @false otherwise.
    */
    bool IsEmpty() const;

    //@{
    /**
        Moves the rectangle by the specified offset. If @a dx is positive, the
        rectangle is moved to the right, if @a dy is positive, it is moved to the
        bottom, otherwise it is moved to the left or top respectively.
    */
    void Offset(wxCoord dx, wxCoord dy);
    void Offset(const wxPoint& pt);
    //@}

    /**
        Sets the height.
    */
    void SetHeight(int height);

    /**
        Sets the size.

        @see GetSize()
    */
    void SetSize(const wxSize& s);

    /**
        Sets the width.
    */
    void SetWidth(int width);

    /**
        Sets the x position.
    */
    void SetX(int x);

    /**
        Sets the y position.
    */
    void SetY(int y);

    //@{
    /**
        Modifies the rectangle to contain the bounding box of this rectangle and the
        one passed in as parameter. The const version returns the new rectangle, the
        other one modifies this rectangle in place.
    */
    wxRect Union(const wxRect& rect);
    const wxRect&  Union(const wxRect& rect);
    //@}

    /**
        int height
        Height member.
    */


    //@{
    /**
        Returns the intersection of two rectangles (which may be empty).
    */
    bool operator !=(const wxRect& r1, const wxRect& r2);
    wxRect operator +(const wxRect& r1, const wxRect& r2);
    wxRect operator +=(const wxRect& r);
    See also wxRect  operator *(const wxRect& r1,
                                const wxRect& r2);
    wxRect operator *=(const wxRect& r);
    //@}

    /**
        Assignment operator.
    */
    void operator =(const wxRect& rect);

    /**
        Equality operator.
    */
    bool operator ==(const wxRect& r1, const wxRect& r2);

    /**
        int width
        Width member.
    */


    /**
        int x
        x coordinate of the top-level corner of the rectangle.
    */


    /**
        int y
        y coordinate of the top-level corner of the rectangle.
    */
};



/**
    @class wxPoint
    @wxheader{gdicmn.h}

    A @b wxPoint is a useful data structure for graphics operations.
    It simply contains integer @e x and @e y members.

    See also wxRealPoint for a floating point version.

    @library{wxcore}
    @category{data}

    @see wxRealPoint
*/
class wxPoint
{
public:
    //@{
    /**
        Create a point.
    */
    wxPoint();
    wxPoint(int x, int y);
    //@}

    //@{
    /**
        Operators for sum and subtraction between a wxPoint object and a
        wxSize object.
    */
    void operator =(const wxPoint& pt);
    bool operator ==(const wxPoint& p1, const wxPoint& p2);
    bool operator !=(const wxPoint& p1, const wxPoint& p2);
    wxPoint operator +(const wxPoint& p1, const wxPoint& p2);
    wxPoint operator -(const wxPoint& p1, const wxPoint& p2);
    wxPoint operator +=(const wxPoint& pt);
    wxPoint operator -=(const wxPoint& pt);
    wxPoint operator +(const wxPoint& pt, const wxSize& sz);
    wxPoint operator -(const wxPoint& pt, const wxSize& sz);
    wxPoint operator +(const wxSize& sz, const wxPoint& pt);
    wxPoint operator -(const wxSize& sz, const wxPoint& pt);
    wxPoint operator +=(const wxSize& sz);
    wxPoint operator -=(const wxSize& sz);
    //@}

    /**
        int x
        x member.
    */


    /**
        int  y
        y member.
    */
};



/**
    @class wxColourDatabase
    @wxheader{gdicmn.h}

    wxWidgets maintains a database of standard RGB colours for a predefined
    set of named colours (such as "BLACK'', "LIGHT GREY''). The
    application may add to this set if desired by using
    wxColourDatabase::AddColour and may use it to look up
    colours by names using wxColourDatabase::Find or find the names
    for the standard colour suing wxColourDatabase::FindName.

    There is one predefined instance of this class called
    @b wxTheColourDatabase.

    @library{wxcore}
    @category{FIXME}

    @see wxColour
*/
class wxColourDatabase
{
public:
    /**
        Constructs the colour database. It will be initialized at the first use.
    */
    wxColourDatabase();

    //@{
    /**
        Adds a colour to the database. If a colour with the same name already exists,
        it is replaced.
        Please note that the overload taking a pointer is deprecated and will be
        removed in the next wxWidgets version, please don't use it.
    */
    void AddColour(const wxString& colourName,
                   const wxColour& colour);
    void AddColour(const wxString& colourName, wxColour* colour);
    //@}

    /**
        Finds a colour given the name. Returns an invalid colour object (that is, such
        that its @ref wxColour::isok Ok method returns @false) if the colour wasn't
        found in the database.
    */
    wxColour Find(const wxString& colourName);

    /**
        Finds a colour name given the colour. Returns an empty string if the colour is
        not found in the database.
    */
    wxString FindName(const wxColour& colour) const;
};



/**
    @class wxFontList
    @wxheader{gdicmn.h}

    A font list is a list containing all fonts which have been created. There
    is only one instance of this class: @b wxTheFontList.  Use this object to search
    for a previously created font of the desired type and create it if not already
    found.
    In some windowing systems, the font may be a scarce resource, so it is best to
    reuse old resources if possible.  When an application finishes, all fonts will
    be
    deleted and their resources freed, eliminating the possibility of 'memory
    leaks'.

    @library{wxcore}
    @category{gdi}

    @see wxFont
*/
class wxFontList : public wxList
{
public:
    /**
        Constructor. The application should not construct its own font list:
        use the object pointer @b wxTheFontList.
    */
    wxFontList();

    /**
        Finds a font of the given specification, or creates one and adds it to the
        list. See the @ref wxFont::ctor "wxFont constructor" for
        details of the arguments.
    */
    wxFont* FindOrCreateFont(int point_size, int family, int style,
                             int weight,
                             bool underline = false,
                             const wxString& facename = NULL,
                             wxFontEncoding encoding = wxFONTENCODING_DEFAULT);
};



/**
    @class wxSize
    @wxheader{gdicmn.h}

    A @b wxSize is a useful data structure for graphics operations.
    It simply contains integer @e width and @e height members.

    wxSize is used throughout wxWidgets as well as wxPoint which, although almost
    equivalent to wxSize, has a different meaning: wxPoint represents a position
    while wxSize - the size.

    @b wxPython note: wxPython defines aliases for the @c x and @c y members
    named @c width and @c height since it makes much more sense for
    sizes.


    @library{wxcore}
    @category{data}

    @see wxPoint, wxRealPoint
*/
class wxSize
{
public:
    //@{
    /**
        Creates a size object.
    */
    wxSize();
    wxSize(int width, int height);
    //@}

    //@{
    /**
        Decreases the size in x- and y- directions
             By @e size.x and @e size.y for the first overload
             By @a dx and @a dy for the second one
             By @a d and @a d for the third one

        @see IncBy()
    */
    void DecBy(const wxSize& size);
    void DecBy(int dx, int dy);
    void DecBy(int d);
    //@}

    /**
        Decrements this object so that both of its dimensions are not greater than the
        corresponding dimensions of the @e size.

        @see IncTo()
    */
    void DecTo(const wxSize& size);

    /**
        Gets the height member.
    */
    int GetHeight() const;

    /**
        Gets the width member.
    */
    int GetWidth() const;

    //@{
    /**
        Increases the size in x- and y- directions
             By @e size.x and @e size.y for the first overload
             By @a dx and @a dy for the second one
             By @a d and @a d for the third one

        @see DecBy()
    */
    void IncBy(const wxSize& size);
    void IncBy(int dx, int dy);
    void IncBy(int d);
    //@}

    /**
        Increments this object so that both of its dimensions are not less than the
        corresponding dimensions of the @e size.

        @see DecTo()
    */
    void IncTo(const wxSize& size);

    /**
        Returns @true if neither of the size object components is equal to -1, which
        is used as default for the size values in wxWidgets (hence the predefined
        @c wxDefaultSize has both of its components equal to -1).
        This method is typically used before calling
        SetDefaults().
    */
    bool IsFullySpecified() const;

    //@{
    /**
        Operators for division and multiplication between a wxSize object and an
        integer.
    */
    void operator =(const wxSize& sz);
    bool operator ==(const wxSize& s1, const wxSize& s2);
    bool operator !=(const wxSize& s1, const wxSize& s2);
    wxSize operator +(const wxSize& s1, const wxSize& s2);
    wxSize operator -(const wxSize& s1, const wxSize& s2);
    wxSize operator +=(const wxSize& sz);
    wxSize operator -=(const wxSize& sz);
    wxSize operator /(const wxSize& sz, int factor);
    wxSize operator *(const wxSize& sz, int factor);
    wxSize operator *(int factor, const wxSize& sz);
    wxSize operator /=(int factor);
    wxSize operator *=(int factor);
    //@}

    /**
        Scales the dimensions of this object by the given factors.
        If you want to scale both dimensions by the same factor you can also use
        the @ref operators() "operator *="
        Returns a reference to this object (so that you can concatenate other
        operations in the same line).
    */
    wxSize Scale(float xscale, float yscale);

    /**
        Sets the width and height members.
    */
    void Set(int width, int height);

    /**
        Combine this size object with another one replacing the default (i.e. equal
        to -1) components of this object with those of the other. It is typically
        used like this:

        @see IsFullySpecified()
    */
    void SetDefaults(const wxSize& sizeDefault);

    /**
        Sets the height.
    */
    void SetHeight(int height);

    /**
        Sets the width.
    */
    void SetWidth(int width);
};





// ============================================================================
// Global functions/macros
// ============================================================================

/** @ingroup group_funcmacro_gdi */
//@{

/**
    This macro loads a bitmap from either application resources (on the
    platforms for which they exist, i.e. Windows and OS2) or from an XPM file.
    This can help to avoid using @ifdef_ when creating bitmaps.

    @see @ref overview_bitmap, wxICON()

    @header{wx/gdicmn.h}
*/
#define wxBITMAP(bitmapName)

/**
    This macro loads an icon from either application resources (on the
    platforms for which they exist, i.e. Windows and OS2) or from an XPM file.
    This can help to avoid using @ifdef_ when creating icons.

    @see @ref overview_bitmap, wxBITMAP()

    @header{wx/gdicmn.h}
*/
wxICON();

/**
    Returns @true if the display is colour, @false otherwise.

    @header{wx/gdicmn.h}
*/
bool wxColourDisplay();

/**
    Returns the depth of the display (a value of 1 denotes a monochrome
    display).

    @header{wx/gdicmn.h}
*/
int wxDisplayDepth();

/**
    Globally sets the cursor; only has an effect on Windows, Mac and GTK+. You
    should call this function with wxNullCursor to restore the system cursor.

    @see wxCursor, wxWindow::SetCursor()

    @header{wx/gdicmn.h}
*/
void wxSetCursor(const wxCursor& cursor);

//@}

/** @ingroup group_funcmacro_gdi */
//@{
/**
    Returns the dimensions of the work area on the display. On Windows this
    means the area not covered by the taskbar, etc. Other platforms are
    currently defaulting to the whole display until a way is found to provide
    this info for all window managers, etc.

    @header{wx/gdicmn.h}
*/
void wxClientDisplayRect(int* x, int* y, int* width, int* height);
wxRect wxGetClientDisplayRect();
//@}

/** @ingroup group_funcmacro_gdi */
//@{
/**
    Returns the display size in pixels.

    @header{wx/gdicmn.h}
*/
void wxDisplaySize(int* width, int* height);
wxSize wxGetDisplaySize();
//@}

/** @ingroup group_funcmacro_gdi */
//@{
/**
    Returns the display size in millimeters.

    @header{wx/gdicmn.h}
*/
void wxDisplaySizeMM(int* width, int* height);
wxSize wxGetDisplaySizeMM();
//@}

