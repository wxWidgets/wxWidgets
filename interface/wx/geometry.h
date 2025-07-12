/////////////////////////////////////////////////////////////////////////////
// Name:        geometry.h
// Purpose:     interface of geometry classes
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


/**
    Represents where a point is in relation to a rectangle.
 */
enum wxOutCode
{
    /// The point is inside the rectangle.
    wxInside = 0x00 ,
    /// The point is outside of the rectangle, to its left.
    wxOutLeft = 0x01 ,
    /// The point is outside of the rectangle, to its right.
    wxOutRight = 0x02 ,
    /// The point is outside of the rectangle, above it.
    wxOutTop = 0x08 ,
    /// The point is outside of the rectangle, below it.
    wxOutBottom = 0x04
};

class wxPoint2DInt
{
public :
    wxPoint2DInt();
    wxPoint2DInt( wxInt32 x , wxInt32 y );
    wxPoint2DInt( const wxPoint2DInt &pt );
    wxPoint2DInt( const wxPoint &pt );

    // noops for this class, just return the coords
    void GetFloor( wxInt32 *x , wxInt32 *y ) const;
    void GetRounded( wxInt32 *x , wxInt32 *y ) const;

    wxDouble GetVectorLength() const;
           wxDouble GetVectorAngle() const;
    void SetVectorLength( wxDouble length );
           void SetVectorAngle( wxDouble degrees );
    // set the vector length to 1.0, preserving the angle
    void Normalize();

    wxDouble GetDistance( const wxPoint2DInt &pt ) const;
    wxDouble GetDistanceSquare( const wxPoint2DInt &pt ) const;
    wxInt32 GetDotProduct( const wxPoint2DInt &vec ) const;
    wxInt32 GetCrossProduct( const wxPoint2DInt &vec ) const;

    // the reflection of this point
    wxPoint2DInt operator-() const;

    wxPoint2DInt& operator=(const wxPoint2DInt& pt);
    wxPoint2DInt& operator+=(const wxPoint2DInt& pt);
    wxPoint2DInt& operator-=(const wxPoint2DInt& pt);
    wxPoint2DInt& operator*=(wxDouble n);
    wxPoint2DInt& operator*=(wxInt32 n);
    wxPoint2DInt& operator/=(wxDouble n);
    wxPoint2DInt& operator/=(wxInt32 n);
    operator wxPoint() const;
    bool operator==(const wxPoint2DInt& pt) const;
    bool operator!=(const wxPoint2DInt& pt) const;

    wxInt32 m_x;
    wxInt32 m_y;
};

wxPoint2DInt operator+(const wxPoint2DInt& pt1 , const wxPoint2DInt& pt2);
wxPoint2DInt operator-(const wxPoint2DInt& pt1 , const wxPoint2DInt& pt2);
wxPoint2DInt operator*(wxDouble n , const wxPoint2DInt& pt);
wxPoint2DInt operator*(wxInt32 n , const wxPoint2DInt& pt);
wxPoint2DInt operator*(const wxPoint2DInt& pt , wxDouble n);
wxPoint2DInt operator*(const wxPoint2DInt& pt , wxInt32 n);
wxPoint2DInt operator/(const wxPoint2DInt& pt , wxDouble n);
wxPoint2DInt operator/(const wxPoint2DInt& pt , wxInt32 n);


/**
    wxPoint2DDouble represents a point in a 2D (Cartesian) coordinate system,
    with additional vector operations available.
 */
class wxPoint2DDouble
{
public :
    /**
       Initializes to zero the x and y members.
    */
    wxPoint2DDouble();
    /**
       Initializes the point with the given coordinates.
    */
    wxPoint2DDouble( wxDouble x , wxDouble y );
    /**
       Initializes the point from another point.
    */
    wxPoint2DDouble( const wxPoint2DDouble &pt );
    /**
       Initializes the point from another point.
    */
    wxPoint2DDouble( const wxPoint2DInt &pt );
    /**
       Initializes the point from another point.
    */
    wxPoint2DDouble( const wxPoint &pt );

    /**
       Returns the floored value of the point's coordinates.

       @param[out] x The value to write the floored X coordinate to.
       @param[out] y The value to write the floored Y coordinate to.
    */
    void GetFloor(wxInt32* x, wxInt32* y) const;
    /**
       @overload
    */
    wxPoint GetFloor() const;
    /**
       Returns the rounded value of the point's coordinates.

       @param[out] x The value to write the rounded X coordinate to.
       @param[out] y The value to write the rounded Y coordinate to.
    */
    void GetRounded(wxInt32* x, wxInt32* y) const;
    /**
        @overload
    */
    wxPoint GetRounded() const;

    /**
        Returns the hypotenuse, where the X and Y coordinates of the point
        represent the lengths of the base and height sides of a right triangle.
     */
    wxDouble GetVectorLength() const;
    /**
       Sets the vector length to @c length, preserving the right angle and altering
       the X and Y values (which represent the base and height sides
       of a right triangle).
    */
    void SetVectorLength(wxDouble length);

    /**
       Returns the principal value of the arc tangent of the Y and X values,
       expressed in degrees.
    */
    wxDouble GetVectorAngle() const ;

    /**
       Repositions the X and Y coordinates based on the provided angle's degrees.
    */
    void SetVectorAngle( wxDouble degrees );

    /**
        Sets the vector length to 1.0, preserving the right angle and altering
        the X and Y values (which represent the base and height sides
        of a right triangle).
    */
    void Normalize();

    /**
        Returns the distance between this point and @c pt.
    */
    wxDouble GetDistance( const wxPoint2DDouble &pt ) const;
    /**
        Returns the squared distance between this point and @c pt.
    */
    wxDouble GetDistanceSquare( const wxPoint2DDouble &pt ) const;

    /**
        Returns the dot (i.e., scalar) product, where the products of the
        X and Y values of this point and @c are added.
    */
    wxDouble GetDotProduct( const wxPoint2DDouble &vec ) const;
    /**
        Returns the cross product, where the products of the Y
        values of this point and @c are subtracted from the X products.
        This represents another vector that is at right angles to both points.
    */
    wxDouble GetCrossProduct( const wxPoint2DDouble &vec ) const;

    /**
        @name Miscellaneous operators

        Note that binary operators are defined as friend functions inside this
        class, making them accessible via argument-dependent lookup, but hidden
        otherwise.
    */
    ///@{
    /**
        Returns the reflection (i.e., negation) of this point.
        For example, (2, 4) on a Cartesian coordinate system will become (-2, -4).
    */
    wxPoint2DDouble operator-() const;

    wxPoint2DDouble& operator=(const wxPoint2DDouble& pt);
    wxPoint2DDouble& operator+=(const wxPoint2DDouble& pt);
    wxPoint2DDouble& operator-=(const wxPoint2DDouble& pt);
    wxPoint2DDouble& operator*=(wxDouble n);
    wxPoint2DDouble& operator*=(wxInt32 n);
    wxPoint2DDouble& operator/=(wxDouble n);
    wxPoint2DDouble& operator/=(wxInt32 n);

    bool operator==(const wxPoint2DDouble& pt) const;
    bool operator!=(const wxPoint2DDouble& pt) const;

    friend wxPoint2DDouble operator+(const wxPoint2DDouble& pt1, const wxPoint2DDouble& pt2);
    friend wxPoint2DDouble operator-(const wxPoint2DDouble& pt1, const wxPoint2DDouble& pt2);
    friend wxPoint2DDouble operator*(wxDouble n, const wxPoint2DDouble& pt);
    friend wxPoint2DDouble operator*(wxInt32 n, const wxPoint2DDouble& pt);
    friend wxPoint2DDouble operator*(const wxPoint2DDouble& pt, wxDouble n);
    friend wxPoint2DDouble operator*(const wxPoint2DDouble& pt, wxInt32 n);
    friend wxPoint2DDouble operator/(const wxPoint2DDouble& pt, wxDouble n);
    friend wxPoint2DDouble operator/(const wxPoint2DDouble& pt, wxInt32 n);
    ///@}

    /**
        X coordinate of this point.
    */
    wxDouble m_x;
    /**
        Y coordinate of this point.
    */
    wxDouble m_y;
};

/**
    wxRect2DDouble is an axis-aligned rectangle;
    each side of the rect is parallel to the X or Y axis.

    The rectangle is either defined by the top left and bottom right corner,
    or by the top left corner and size.

    A point is contained within the rectangle if left <= @c @c m_x < right
    and top <= @c m_y < bottom; thus, it is a half open interval.

    @note wxRect2DDouble has subtle differences from wxRect in how its edge
     and corner functions work. With wxRect2DDouble, there are two counterparts:
    - `SetXXX` functions, which keep the other corners at their position whenever sensible
    - `MoveXXX` functions, which keep the size of the rectangle and move the other
       corners appropriately
 */
class wxRect2DDouble
{
public:
    /**
        Default constructor.
        Initializes to zero the internal @a m_x, @a m_y, @a m_width and @a m_height members.
    */
    wxRect2DDouble();
    /**
        Creates a wxRect2DDouble object from @a x, @a y, @a width and @a height values.
    */
    wxRect2DDouble(wxDouble x, wxDouble y, wxDouble width, wxDouble height);

    /**
        Constructs a wxRect2DDouble from a wxRect.

        @since 3.3.0
    */
    explicit wxRect2DDouble(const wxRect& rect);

    /**
        Returns the position.
    */
    wxPoint2DDouble GetPosition() const;
    /**
        Returns the size.
   */
    wxSize GetSize() const;

    /**
        Returns the left position of the rectangle.

        @since 3.3.0
    */
    wxDouble GetX() const;

    /**
        Returns the top position of the rect.

        @since 3.3.0
    */
    wxDouble GetY() const;

    /**
        Returns the width.

        @since 3.3.0
    */
    wxDouble GetWidth() const;

    /**
        Sets the width.

        @since 3.3.0
    */
    void SetWidth(wxDouble w);

    /**
        Returns the height.

        @since 3.3.0
    */
    wxDouble GetHeight() const;

    /**
        Sets the height.

        @since 3.3.0
    */
    void SetHeight(wxDouble h) { m_height = h; }

    /**
        Returns the left point of the rectangle (the same as GetX()).
    */
    wxDouble GetLeft() const;
    /**
       Set the left side of the rectangle.

       @note This will preserve the width of the rectangle.
       Use MoveLeftTo() to change the left position
       of the rectangle, adjusting its width accordingly.
    */
    void SetLeft( wxDouble n );
    /**
        Sets the left position, which may adjust the width of the rectangle.
    */
    void MoveLeftTo( wxDouble n );
    /**
        Returns the top point of the rectangle (the same as GetY()).
    */
    wxDouble GetTop() const;
    /**
        Set the top edge of the rectangle.

        @note This will alter the height of the rectangle.
        Use MoveTopTo() to only move the top.
    */
    void SetTop( wxDouble n );
    /**
        Set the top edge of the rectangle, preserving the height.
    */
    void MoveTopTo( wxDouble n );
    /**
        Returns the bottom point of the rectangle.
    */
    wxDouble GetBottom() const;
    /**
       Set the bottom edge of the rectangle.

       @note This will preserve the top position and
        alter the height of the rectangle.
        Use MoveBottomTo() to only move the bottom.
     */
    void SetBottom( wxDouble n );
    /**
       Set the bottom edge of the rectangle, preserving the height.
     */
    void MoveBottomTo( wxDouble n );
    /**
        Returns the right point of the rectangle.
    */
    wxDouble GetRight() const;
    /**
       Set the right side of the rectangle.

       @note This will preserve the left position and
        alter the width of the rectangle.
        Use MoveRightTo() to only move the bottom.
     */
    void SetRight( wxDouble n );
    /**
       Set the right side of the rectangle, preserving the width.
     */
    void MoveRightTo( wxDouble n );

    /**
        Returns the position of the top left corner of the rectangle, same as
        GetPosition().
    */
    wxPoint2DDouble GetLeftTop() const;
    /**
       Set the top-left point of the rectangle.

       @note This will alter the height of the rectangle.
       Use MoveLeftTopTo() to only move the top.
     */
    void SetLeftTop( const wxPoint2DDouble &pt );
    /**
       Set the top-left point of the rectangle, while preserving the
       width and height of the rectangle.
     */
    void MoveLeftTopTo( const wxPoint2DDouble &pt );
    /**
        Returns the position of the bottom left corner.
    */
    wxPoint2DDouble GetLeftBottom() const;
    /**
       Set the bottom-left point of the rectangle.

       @note This will alter the width and height of the rectangle.
        Use MoveLeftBottomTo() to only move the left bottom corner.
     */
    void SetLeftBottom( const wxPoint2DDouble &pt );
    /**
       Set the bottom-left point of the rectangle, while preserving the
       width and height of the rectangle.
     */
    void MoveLeftBottomTo( const wxPoint2DDouble &pt );
    /**
        Returns the position of the top right corner.
    */
    wxPoint2DDouble GetRightTop() const;
    /**
       Set the top-right point of the rectangle.

       @note This will alter the width and height of the rectangle.
       Use MoveRightTopTo() to only move the right top corner.
     */
    void SetRightTop( const wxPoint2DDouble &pt );
    /**
       Set the top-right point of the rectangle, while preserving the
       width and height of the rectangle.
     */
    void MoveRightTopTo( const wxPoint2DDouble &pt );
    /**
        Returns the position of the bottom right corner.
    */
    wxPoint2DDouble GetRightBottom() const;
    /**
       Set the bottom-right point of the rectangle.

       @note This will alter the width and height of the rectangle.
       Use MoveRightBottomTo() to only move the right bottom corner.
     */
    void SetRightBottom( const wxPoint2DDouble &pt );
    /**
       Set the bottom-right point of the rectangle, while preserving the
       width and height of the rectangle.
     */
    void MoveRightBottomTo( const wxPoint2DDouble &pt );
    /**
       Returns the centre point of the rectangle.
     */
    wxPoint2DDouble GetCentre() const;
    /**
       Recenters (i.e., moves) the rectangle to the given point.
     */
    void SetCentre( const wxPoint2DDouble &pt );
    /**
       An alias for MoveCentreTo().
     */
    void MoveCentreTo( const wxPoint2DDouble &pt );
    /**
       Returns the relative location of a point to the rectangle
       (e.g., inside or to the left of it).
     */
    wxOutCode GetOutCode( const wxPoint2DDouble &pt ) const;
    /**
        Returns @true if the given point is inside the rectangle (or on its
        boundary) and @false otherwise.
    */
    bool Contains( const wxPoint2DDouble &pt ) const;
    /**
        Returns @true if the given rectangle is completely inside this
        rectangle (or touches its boundary) and @false otherwise.
    */
    bool Contains( const wxRect2DDouble &rect ) const;
    /**
        Returns @true if this rectangle has a width or height less than or
        equal to 0 and @false otherwise.
    */
    bool IsEmpty() const;
    /**
        Returns @true if another rectangle has the same width and height.
    */
    bool HaveEqualSize( const wxRect2DDouble &rect ) const;

    /**
        Offsets the rectangle by @c x and @c y, but maintains the bottom right corner.

        @note This will affect the width and height of the rectangle.
    */
    void Inset( wxDouble x , wxDouble y );
    void Inset( wxDouble left , wxDouble top ,wxDouble right , wxDouble bottom  );
    /**
        Moves the rectangle by the specified offset. If X of @a pt is positive, the
        rectangle is moved to the right, if Y of @a pt is positive, it is moved to the
        bottom, otherwise it is moved to the left or top respectively.
    */
    void Offset( const wxPoint2DDouble &pt );
    /**
        @overload
        @since 3.3.0
    */
    void Offset( wxDouble dx, wxDouble dy );

    /**
        Resizes the rectangle to fit within the dimensions of another rectangle.
    */
    void ConstrainTo( const wxRect2DDouble &rect );

    wxPoint2DDouble Interpolate( wxInt32 widthfactor, wxInt32 heightfactor ) const;

    /**
        Returns the intersecting rectangle of two rectangles.
    */
    static void Intersect( const wxRect2DDouble &src1 , const wxRect2DDouble &src2 , wxRect2DDouble *dest );
    /**
        Constrains the rectangle to the intersection of another rectangle.
    */
    void Intersect( const wxRect2DDouble &otherRect );
    /**
        Returns the intersecting rectangle of this rectangle with another one.
    */
    wxRect2DDouble CreateIntersection( const wxRect2DDouble &otherRect ) const;
    /**
        Returns @true if this rectangle has a non-empty intersection with the
        rectangle @a rect and @false otherwise.
    */
    bool Intersects( const wxRect2DDouble &rect ) const;

    /**
        Returns the union rectangle of two rectangles.
    */
    static void Union( const wxRect2DDouble &src1 , const wxRect2DDouble &src2 , wxRect2DDouble *dest );
    /**
        Expands the rectangle to the union with another rectangle.
    */
    void Union( const wxRect2DDouble &otherRect );
    /**
        Expands the rectangle to include the point at @c pt.
    */
    void Union( const wxPoint2DDouble &pt );
    /**
        Returns the union of this rectangle with another one.
    */
    wxRect2DDouble CreateUnion( const wxRect2DDouble &otherRect ) const;

    void Scale( wxDouble f );
    void Scale( wxInt32 num , wxInt32 denum );

    ///@{
    /**
        Increases the size of the rectangle.

        The left border is moved farther left and the right border is moved
        farther right by @a dx. The upper border is moved farther up and the
        bottom border is moved farther down by @a dy. (Note that the width and
        height of the rectangle thus change by 2*dx and 2*dy, respectively.) If
        one or both of @a dx and @a dy are negative, the opposite happens: the
        rectangle size decreases in the respective direction.

        Inflating and deflating behaves "naturally". Defined more precisely,
        that means:
        -# "Real" inflates (that is, @a dx and/or @a dy = 0) are not
           constrained. Thus inflating a rectangle can cause its upper left
           corner to move into the negative numbers. (2.5.4 and older forced
           the top left coordinate to not fall below (0, 0), which implied a
           forced move of the rectangle.)
        -# Deflates are clamped to not reduce the width or height of the
           rectangle below zero. In such cases, the top-left corner is
           nonetheless handled properly. For example, a rectangle at (10, 10)
           with size (20, 40) that is inflated by (-15, -15) will become
           located at (20, 25) at size (0, 10). Finally, observe that the width
           and height are treated independently. In the above example, the
           width is reduced by 20, whereas the height is reduced by the full 30
           (rather than also stopping at 20, when the width reached zero).

        @see Deflate()
        @since 3.3.0
    */
    wxRect2DDouble& Inflate(wxDouble dx, wxDouble dy);
    /// @overload
    wxRect2DDouble& Inflate(const wxSize& d);
    /// @overload
    wxRect2DDouble& Inflate(wxDouble d);
    /// @overload
    wxRect2DDouble Inflate(wxDouble dx, wxDouble dy) const;
    ///@}

    ///@{
    /**
        Decrease the rectangle size.

        This method is the opposite from Inflate(): Deflate(a, b) is equivalent
        to Inflate(-a, -b). Please refer to Inflate() for full description.

        @since 3.3.0
    */
    wxRect2DDouble& Deflate(wxDouble dx, wxDouble dy);
    /// @overload
    wxRect2DDouble& Deflate(const wxSize& d);
    /// @overload
    wxRect2DDouble& Deflate(wxDouble d);
    /// @overload
    wxRect2DDouble Deflate(wxDouble dx, wxDouble dy) const;
    ///@}

    /**
        Returns the rectangle as a wxRect.

        @since 3.3.0
    */
    wxRect ToRect() const;

    /**
        @name Miscellaneous operators
    */
    ///@{
    /**
        Assignment operator.
    */
    wxRect2DDouble& operator = (const wxRect2DDouble& rect);
    /**
        Equality operator.
    */
    bool operator == (const wxRect2DDouble& rect) const;
    /**
        Inequality operator.
    */
    bool operator != (const wxRect2DDouble& rect) const;
    ///@}

    wxDouble  m_x;
    wxDouble  m_y;
    wxDouble  m_width;
    wxDouble m_height;
};




// wxRect2Ds are a axis-aligned rectangles, each side of the rect is parallel to the x- or m_y- axis. The rectangle is either defined by the
// top left and bottom right corner, or by the top left corner and size. A point is contained within the rectangle if
// left <= x < right  and top <= m_y < bottom , thus it is a half open interval.

class wxRect2DInt
{
public:
    wxRect2DInt();
    wxRect2DInt( const wxRect& r );
    wxRect2DInt(wxInt32 x, wxInt32 y, wxInt32 w, wxInt32 h);
    wxRect2DInt(const wxPoint2DInt& topLeft, const wxPoint2DInt& bottomRight);
    wxRect2DInt(const wxPoint2DInt& pos, const wxSize& size);
    wxRect2DInt(const wxRect2DInt& rect);

        // single attribute accessors

    wxPoint2DInt GetPosition() const;
    wxSize GetSize() const;

        // for the edge and corner accessors there are two setters counterparts, the Set.. functions keep the other corners at their
        // position whenever sensible, the Move.. functions keep the size of the rect and move the other corners appropriately

    wxInt32 GetLeft() const;
    void SetLeft( wxInt32 n );
    void MoveLeftTo( wxInt32 n );
    wxInt32 GetTop() const;
    void SetTop( wxInt32 n );
    void MoveTopTo( wxInt32 n );
    wxInt32 GetBottom() const;
    void SetBottom( wxInt32 n );
    void MoveBottomTo( wxInt32 n );
    wxInt32 GetRight() const;
    void SetRight( wxInt32 n );
    void MoveRightTo( wxInt32 n );

    wxPoint2DInt GetLeftTop() const;
    void SetLeftTop( const wxPoint2DInt &pt ) ;
    void MoveLeftTopTo( const wxPoint2DInt &pt ) ;
    wxPoint2DInt GetLeftBottom() const ;
    void SetLeftBottom( const wxPoint2DInt &pt ) ;
    void MoveLeftBottomTo( const wxPoint2DInt &pt ) ;
    wxPoint2DInt GetRightTop() const ;
    void SetRightTop( const wxPoint2DInt &pt ) ;
    void MoveRightTopTo( const wxPoint2DInt &pt ) ;
    wxPoint2DInt GetRightBottom() const ;
    void SetRightBottom( const wxPoint2DInt &pt ) ;
    void MoveRightBottomTo( const wxPoint2DInt &pt ) ;
    wxPoint2DInt GetCentre() const ;
    void SetCentre( const wxPoint2DInt &pt ) ;
    void MoveCentreTo( const wxPoint2DInt &pt ) ;
    wxOutCode GetOutCode( const wxPoint2DInt &pt ) const;
    wxOutCode GetOutcode( const wxPoint2DInt &pt ) const;
    bool Contains( const wxPoint2DInt &pt ) const;
    bool Contains( const wxRect2DInt &rect ) const;
    bool IsEmpty() const;
    bool HaveEqualSize( const wxRect2DInt &rect ) const;

    void Inset( wxInt32 x , wxInt32 y );
    void Inset( wxInt32 left , wxInt32 top ,wxInt32 right , wxInt32 bottom  );
    void Offset( const wxPoint2DInt &pt );
    void ConstrainTo( const wxRect2DInt &rect );
    wxPoint2DInt Interpolate( wxInt32 widthfactor, wxInt32 heightfactor ) const;

    static void Intersect( const wxRect2DInt &src1 , const wxRect2DInt &src2 , wxRect2DInt *dest );
    void Intersect( const wxRect2DInt &otherRect );
    wxRect2DInt CreateIntersection( const wxRect2DInt &otherRect ) const;
    bool Intersects( const wxRect2DInt &rect ) const;

    static void Union( const wxRect2DInt &src1 , const wxRect2DInt &src2 , wxRect2DInt *dest );
    void Union( const wxRect2DInt &otherRect );
    void Union( const wxPoint2DInt &pt );
    wxRect2DInt CreateUnion( const wxRect2DInt &otherRect ) const;

    void Scale( wxInt32 f );
    void Scale( wxInt32 num , wxInt32 denum );

    wxRect2DInt& operator = (const wxRect2DInt& rect);
    bool operator == (const wxRect2DInt& rect) const;
    bool operator != (const wxRect2DInt& rect) const;


    wxInt32 m_x;
    wxInt32 m_y;
    wxInt32 m_width;
    wxInt32 m_height;
};





class wxTransform2D
{
public :
    virtual ~wxTransform2D();
    virtual void                    Transform( wxPoint2DInt* pt )const  = 0;
    virtual void                    Transform( wxRect2DInt* r ) const;
    virtual wxPoint2DInt    Transform( const wxPoint2DInt &pt ) const;
    virtual wxRect2DInt        Transform( const wxRect2DInt &r ) const ;

    virtual void                    InverseTransform( wxPoint2DInt* pt ) const  = 0;
    virtual void                    InverseTransform( wxRect2DInt* r ) const ;
    virtual wxPoint2DInt    InverseTransform( const wxPoint2DInt &pt ) const ;
    virtual wxRect2DInt        InverseTransform( const wxRect2DInt &r ) const ;
};


