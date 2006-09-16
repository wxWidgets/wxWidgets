/////////////////////////////////////////////////////////////////////////////
// Name:        _gdicmn.i
// Purpose:     SWIG interface for common GDI stuff and misc classes
//
// Author:      Robin Dunn
//
// Created:     13-Sept-2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
%newgroup


enum wxBitmapType
{
    wxBITMAP_TYPE_INVALID,          // should be == 0 for compatibility!
    wxBITMAP_TYPE_BMP,
    wxBITMAP_TYPE_ICO,
    wxBITMAP_TYPE_CUR,
    wxBITMAP_TYPE_XBM,
    wxBITMAP_TYPE_XBM_DATA,
    wxBITMAP_TYPE_XPM,
    wxBITMAP_TYPE_XPM_DATA,
    wxBITMAP_TYPE_TIF,
    wxBITMAP_TYPE_GIF,
    wxBITMAP_TYPE_PNG,
    wxBITMAP_TYPE_JPEG,
    wxBITMAP_TYPE_PNM,
    wxBITMAP_TYPE_PCX,
    wxBITMAP_TYPE_PICT,
    wxBITMAP_TYPE_ICON,
    wxBITMAP_TYPE_ANI,
    wxBITMAP_TYPE_IFF,
    wxBITMAP_TYPE_MACCURSOR,

//    wxBITMAP_TYPE_BMP_RESOURCE,
//    wxBITMAP_TYPE_RESOURCE = wxBITMAP_TYPE_BMP_RESOURCE,
//    wxBITMAP_TYPE_ICO_RESOURCE,
//    wxBITMAP_TYPE_CUR_RESOURCE,
//    wxBITMAP_TYPE_TIF_RESOURCE,
//    wxBITMAP_TYPE_GIF_RESOURCE,
//    wxBITMAP_TYPE_PNG_RESOURCE,
//    wxBITMAP_TYPE_JPEG_RESOURCE,
//    wxBITMAP_TYPE_PNM_RESOURCE,
//    wxBITMAP_TYPE_PCX_RESOURCE,
//    wxBITMAP_TYPE_PICT_RESOURCE,
//    wxBITMAP_TYPE_ICON_RESOURCE,
//    wxBITMAP_TYPE_MACCURSOR_RESOURCE,

    wxBITMAP_TYPE_ANY = 50
};

// Standard cursors
enum wxStockCursor
{
    wxCURSOR_NONE,          // should be 0
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
    wxCURSOR_DEFAULT, // standard X11 cursor
    wxCURSOR_COPY_ARROW , // MacOS Theme Plus arrow
    
// #ifdef __X__
//     // Not yet implemented for Windows
//     wxCURSOR_CROSS_REVERSE,
//     wxCURSOR_DOUBLE_ARROW,
//     wxCURSOR_BASED_ARROW_UP,
//     wxCURSOR_BASED_ARROW_DOWN,
// #endif // X11

    wxCURSOR_ARROWWAIT,

    wxCURSOR_MAX
};

%{
#ifndef __WXMAC__
#define wxCURSOR_COPY_ARROW wxCURSOR_ARROW
#endif
%}

//---------------------------------------------------------------------------
%newgroup

DocStr( wxSize,
"wx.Size is a useful data structure used to represent the size of
something.  It simply contains integer width and height
properties.  In most places in wxPython where a wx.Size is
expected a (width, height) tuple can be used instead.", "");

class wxSize
{
public:
    %Rename(width, int,  x);
    %Rename(height,int,  y);
    %pythoncode { x = width; y = height }
    
    DocCtorStr(
        wxSize(int w=0, int h=0),
        "Creates a size object.", "");

    ~wxSize();

    
    %extend {
        KeepGIL(__eq__);
        DocStr(__eq__, "Test for equality of wx.Size objects.", "");
        bool __eq__(PyObject* other) {
            wxSize  temp, *obj = &temp;
            if ( other == Py_None ) return false;
            if ( ! wxSize_helper(other, &obj) ) {
                PyErr_Clear();
                return false;
            }
            return self->operator==(*obj);
        }

        
        KeepGIL(__ne__);
        DocStr(__ne__, "Test for inequality of wx.Size objects.", "");
        bool __ne__(PyObject* other) {
            wxSize  temp, *obj = &temp;
            if ( other == Py_None ) return true;
            if ( ! wxSize_helper(other, &obj)) {
                PyErr_Clear();
                return true;
            }
            return self->operator!=(*obj);
        }
    }

    DocDeclStr(
        wxSize, operator+(const wxSize& sz),
        "Add sz's proprties to this and return the result.", "");

    DocDeclStr(
        wxSize, operator-(const wxSize& sz),
        "Subtract sz's properties from this and return the result.", "");

    DocDeclStr(
        void, IncTo(const wxSize& sz),
        "Increments this object so that both of its dimensions are not less
than the corresponding dimensions of the size.", "");

    DocDeclStr(
        void, DecTo(const wxSize& sz),
        "Decrements this object so that both of its dimensions are not greater
than the corresponding dimensions of the size.", "");

    
    DocDeclStr(
        void , Scale(float xscale, float yscale),
        "Scales the dimensions of this object by the given factors.", "");

    DocDeclStr(
        void, Set(int w, int h),
        "Set both width and height.", "");
    
    void SetWidth(int w);
    void SetHeight(int h);
    int GetWidth() const;
    int GetHeight() const;

    
    DocDeclStr(
        bool , IsFullySpecified() const,
        "Returns True if both components of the size are non-default values.", "");
    

    DocDeclStr(
        void , SetDefaults(const wxSize& size),
        "Combine this size with the other one replacing the default components
of this object (i.e. equal to -1) with those of the other.", "");
    
   
    //int GetX() const;
    //int GetY() const;

    %extend {
        DocAStr(Get,
               "Get() -> (width,height)",
               "Returns the width and height properties as a tuple.", "");
        PyObject* Get() {
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, PyInt_FromLong(self->x));
            PyTuple_SET_ITEM(tup, 1, PyInt_FromLong(self->y));
            wxPyEndBlockThreads(blocked);
            return tup;
        }
    }
    %pythoncode {
    asTuple = wx._deprecated(Get, "asTuple is deprecated, use `Get` instead")
    def __str__(self):                   return str(self.Get())
    def __repr__(self):                  return 'wx.Size'+str(self.Get())
    def __len__(self):                   return len(self.Get())
    def __getitem__(self, index):        return self.Get()[index]
    def __setitem__(self, index, val):
        if index == 0: self.width = val
        elif index == 1: self.height = val
        else: raise IndexError
    def __nonzero__(self):               return self.Get() != (0,0)
    __safe_for_unpickling__ = True
    def __reduce__(self):                return (wx.Size, self.Get())
    }

};

//---------------------------------------------------------------------------
%newgroup

DocStr( wxRealPoint,
"A data structure for representing a point or position with floating
point x and y properties.  In wxPython most places that expect a
wx.RealPoint can also accept a (x,y) tuple.", "");
class wxRealPoint
{
public:
    double x;
    double y;

    DocCtorStr(
        wxRealPoint(double x=0.0, double y=0.0),
        "Create a wx.RealPoint object", "");
    
    ~wxRealPoint();

    %extend {
        KeepGIL(__eq__);
        DocStr(__eq__, "Test for equality of wx.RealPoint objects.", "");
        bool __eq__(PyObject* other) {
            wxRealPoint  temp, *obj = &temp;
            if ( other == Py_None ) return false;
            if ( ! wxRealPoint_helper(other, &obj) ) {
                PyErr_Clear();
                return false;
            }
            return self->operator==(*obj);
        }

        
        KeepGIL(__ne__);
        DocStr(__ne__, "Test for inequality of wx.RealPoint objects.", "");
        bool __ne__(PyObject* other) {
            wxRealPoint  temp, *obj = &temp;
            if ( other == Py_None ) return true;
            if ( ! wxRealPoint_helper(other, &obj)) {
                PyErr_Clear();
                return true;
            }
            return self->operator!=(*obj);
        }
    }

    
    DocDeclStr(
        wxRealPoint, operator+(const wxRealPoint& pt),
        "Add pt's proprties to this and return the result.", "");

    DocDeclStr(
        wxRealPoint, operator-(const wxRealPoint& pt),
        "Subtract pt's proprties from this and return the result", "");


    %extend {
        DocStr(Set, "Set both the x and y properties", "");
        void Set(double x, double y) {
            self->x = x;
            self->y = y;
        }

        DocAStr(Get,
               "Get() -> (x,y)",
               "Return the x and y properties as a tuple. ", "");
        PyObject* Get() {
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, PyFloat_FromDouble(self->x));
            PyTuple_SET_ITEM(tup, 1, PyFloat_FromDouble(self->y));
            wxPyEndBlockThreads(blocked);
            return tup;
        }
    }

    %pythoncode {
    asTuple = wx._deprecated(Get, "asTuple is deprecated, use `Get` instead")
    def __str__(self):                   return str(self.Get())
    def __repr__(self):                  return 'wx.RealPoint'+str(self.Get())
    def __len__(self):                   return len(self.Get())
    def __getitem__(self, index):        return self.Get()[index]
    def __setitem__(self, index, val):
        if index == 0: self.x = val
        elif index == 1: self.y = val
        else: raise IndexError
    def __nonzero__(self):               return self.Get() != (0.0, 0.0)
    __safe_for_unpickling__ = True
    def __reduce__(self):                return (wx.RealPoint, self.Get())
    }
};


//---------------------------------------------------------------------------
%newgroup


DocStr(wxPoint,
"A data structure for representing a point or position with integer x
and y properties.  Most places in wxPython that expect a wx.Point can
also accept a (x,y) tuple.", "");

class wxPoint
{
public:
    int x, y;

    DocCtorStr(
        wxPoint(int x=0, int y=0),
        "Create a wx.Point object", "");

    ~wxPoint();

    
    %extend {
        KeepGIL(__eq__);
        DocStr(__eq__, "Test for equality of wx.Point objects.", "");
        bool __eq__(PyObject* other) {
            wxPoint  temp, *obj = &temp;
            if ( other == Py_None ) return false;
            if ( ! wxPoint_helper(other, &obj) ) {
                PyErr_Clear();
                return false;
            }
            return self->operator==(*obj);
        }

        
        KeepGIL(__ne__);
        DocStr(__ne__, "Test for inequality of wx.Point objects.", "");
        bool __ne__(PyObject* other) {
            wxPoint  temp, *obj = &temp;
            if ( other == Py_None ) return true;
            if ( ! wxPoint_helper(other, &obj)) {
                PyErr_Clear();
                return true;
            }
            return self->operator!=(*obj);
        }
    }


//     %nokwargs operator+;
//     %nokwargs operator-;
//     %nokwargs operator+=;
//     %nokwargs operator-=;
    
    DocDeclStr(
        wxPoint, operator+(const wxPoint& pt),
        "Add pt's proprties to this and return the result.", "");

   
    DocDeclStr(
        wxPoint, operator-(const wxPoint& pt),
        "Subtract pt's proprties from this and return the result", "");


    DocDeclStr(
        wxPoint&, operator+=(const wxPoint& pt),
        "Add pt to this object.", "");

    DocDeclStr(
        wxPoint&, operator-=(const wxPoint& pt),
        "Subtract pt from this object.", "");


    
//     DocDeclStr(
//         wxPoint, operator+(const wxSize& sz),
//         "Add sz to this Point and return the result.", "");

//     DocDeclStr(
//         wxPoint, operator-(const wxSize& sz),
//         "Subtract sz from this Point and return the result", "");


//     DocDeclStr(
//         wxPoint&, operator+=(const wxSize& sz),
//         "Add sz to this object.", "");

//     DocDeclStr(
//         wxPoint&, operator-=(const wxSize& sz),
//         "Subtract sz from this object.", "");


    
    
    %extend {
        DocStr(Set, "Set both the x and y properties", "");
        void Set(long x, long y) {
            self->x = x;
            self->y = y;
        }

        DocAStr(Get,
               "Get() -> (x,y)",
               "Return the x and y properties as a tuple. ", "");
        PyObject* Get() {
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, PyInt_FromLong(self->x));
            PyTuple_SET_ITEM(tup, 1, PyInt_FromLong(self->y));
            wxPyEndBlockThreads(blocked);
            return tup;
        }
    }

    %pythoncode {
    asTuple = wx._deprecated(Get, "asTuple is deprecated, use `Get` instead")
    def __str__(self):                   return str(self.Get())
    def __repr__(self):                  return 'wx.Point'+str(self.Get())
    def __len__(self):                   return len(self.Get())
    def __getitem__(self, index):        return self.Get()[index]
    def __setitem__(self, index, val):
        if index == 0: self.x = val
        elif index == 1: self.y = val
        else: raise IndexError
    def __nonzero__(self):               return self.Get() != (0,0)
    __safe_for_unpickling__ = True
    def __reduce__(self):                return (wx.Point, self.Get())
    }
};

//---------------------------------------------------------------------------
%newgroup


DocStr(wxRect,
"A class for representing and manipulating rectangles.  It has x, y,
width and height properties.  In wxPython most palces that expect a
wx.Rect can also accept a (x,y,width,height) tuple.", "");

class wxRect
{
public:
    DocCtorStr(
        wxRect(int x=0, int y=0, int width=0, int height=0),
        "Create a new Rect object.", "");          

    DocCtorStrName(
        wxRect(const wxPoint& topLeft, const wxPoint& bottomRight),
        "Create a new Rect object from Points representing two corners.", "",
        RectPP);

    DocCtorStrName(
        wxRect(const wxPoint& pos, const wxSize& size),
        "Create a new Rect from a position and size.", "",
         RectPS);

    DocCtorStrName(
        wxRect(const wxSize& size),
        "Create a new Rect from a size only.", "",
         RectS);

    ~wxRect();

    int GetX() const;
    void SetX(int x);

    int GetY();
    void SetY(int y);

    int GetWidth() const;
    void SetWidth(int w);

    int GetHeight() const;
    void SetHeight(int h);

    wxPoint GetPosition() const;
    void SetPosition( const wxPoint &p );

    wxSize GetSize() const;
    void SetSize( const wxSize &s );

    bool IsEmpty() const;
    
    wxPoint GetTopLeft() const;
    void SetTopLeft(const wxPoint &p);
    wxPoint GetBottomRight() const;
    void SetBottomRight(const wxPoint &p);

    wxPoint GetTopRight() const;
    void SetTopRight(const wxPoint &p);
    wxPoint GetBottomLeft() const;
    void SetBottomLeft(const wxPoint &p);
    
//    wxPoint GetLeftTop() const;
//    void SetLeftTop(const wxPoint &p);
//    wxPoint GetRightBottom() const;
//    void SetRightBottom(const wxPoint &p);
//    wxPoint GetRightTop() const;
//    void SetRightTop(const wxPoint &p);
//    wxPoint GetLeftBottom() const;
//    void SetLeftBottom(const wxPoint &p);

    int GetLeft()   const;
    int GetTop()    const;
    int GetBottom() const;
    int GetRight()  const;

    void SetLeft(int left);
    void SetRight(int right);
    void SetTop(int top);
    void SetBottom(int bottom);

    %pythoncode {
        position = property(GetPosition, SetPosition)
        size = property(GetSize, SetSize)
        left = property(GetLeft, SetLeft)
        right = property(GetRight, SetRight)
        top = property(GetTop, SetTop)
        bottom = property(GetBottom, SetBottom)
    }

    DocDeclStr(
        wxRect&, Inflate(wxCoord dx, wxCoord dy),
        "Increases the size of the rectangle.

The left border is moved farther left and the right border is moved
farther right by ``dx``. The upper border is moved farther up and the
bottom border is moved farther down by ``dy``. (Note the the width and
height of the rectangle thus change by ``2*dx`` and ``2*dy``,
respectively.) If one or both of ``dx`` and ``dy`` are negative, the
opposite happens: the rectangle size decreases in the respective
direction.

The change is made to the rectangle inplace, if instead you need a
copy that is inflated, preserving the original then make the copy
first::

    copy = wx.Rect(*original)
    copy.Inflate(10,15)

", "
Inflating and deflating behaves *naturally*. Defined more precisely,
that means:

    * Real inflates (that is, ``dx`` and/or ``dy`` >= 0) are not
      constrained. Thus inflating a rectangle can cause its upper left
      corner to move into the negative numbers. (The versions prior to
      2.5.4 forced the top left coordinate to not fall below (0, 0),
      which implied a forced move of the rectangle.)

    * Deflates are clamped to not reduce the width or height of the
      rectangle below zero. In such cases, the top-left corner is
      nonetheless handled properly. For example, a rectangle at (10,
      10) with size (20, 40) that is inflated by (-15, -15) will
      become located at (20, 25) at size (0, 10). Finally, observe
      that the width and height are treated independently. In the
      above example, the width is reduced by 20, whereas the height is
      reduced by the full 30 (rather than also stopping at 20, when
      the width reached zero).

:see: `Deflate`
");

    DocDeclStr(
        wxRect&, Deflate(wxCoord dx, wxCoord dy),
        "Decrease the rectangle size. This method is the opposite of `Inflate`
in that Deflate(a,b) is equivalent to Inflate(-a,-b).  Please refer to
`Inflate` for a full description.", "");

    DocDeclStrName(
        void, Offset(wxCoord dx, wxCoord dy),
        "Moves the rectangle by the specified offset. If dx is positive, the
rectangle is moved to the right, if dy is positive, it is moved to the
bottom, otherwise it is moved to the left or top respectively.", "",
        OffsetXY);
    
    DocDeclStr(
        void, Offset(const wxPoint& pt),
        "Same as `OffsetXY` but uses dx,dy from Point", "");

    DocDeclStr(
        wxRect, Intersect(const wxRect& rect),
        "Returns the intersectsion of this rectangle and rect.", "");

    DocDeclStr(
        wxRect , Union(const wxRect& rect),
        "Returns the union of this rectangle and rect.", "");
    
    
    DocDeclStr(
        wxRect, operator+(const wxRect& rect) const,
        "Add the properties of rect to this rectangle and return the result.", "");

    DocDeclStr(
        wxRect&, operator+=(const wxRect& rect),
        "Add the properties of rect to this rectangle, updating this rectangle.", "");

    %extend {
        KeepGIL(__eq__);
        DocStr(__eq__, "Test for equality of wx.Rect objects.", "");
        bool __eq__(PyObject* other) {
            wxRect  temp, *obj = &temp;
            if ( other == Py_None ) return false;
            if ( ! wxRect_helper(other, &obj) ) {
                PyErr_Clear();
                return false;
            }
            return self->operator==(*obj);
        }

        
        KeepGIL(__ne__);
        DocStr(__ne__, "Test for inequality of wx.Rect objects.", "");
        bool __ne__(PyObject* other) {
            wxRect  temp, *obj = &temp;
            if ( other == Py_None ) return true;
            if ( ! wxRect_helper(other, &obj)) {
                PyErr_Clear();
                return true;
            }
            return self->operator!=(*obj);
        }
    }

    
    DocStr( Contains, "Return True if the point is inside the rect.", "");
    %Rename(ContainsXY, bool, Contains(int x, int y) const);
    bool Contains(const wxPoint& pt) const;

    DocDeclStrName(
        bool, Contains(const wxRect& rect) const,
        "Returns ``True`` if the given rectangle is completely inside this
rectangle or touches its boundary.", "",
        ContainsRect);
    %pythoncode {
        Inside = wx._deprecated(Contains, "Use `Contains` instead.")
        InsideXY = wx._deprecated(ContainsXY, "Use `ContainsXY` instead.")
        InsideRect = wx._deprecated(ContainsRect, "Use `ContainsRect` instead.")
    }
    
    DocDeclStr(    
        bool, Intersects(const wxRect& rect) const,
        "Returns True if the rectangles have a non empty intersection.", "");

    DocDeclStr(
        wxRect, CenterIn(const wxRect& r, int dir = wxBOTH),
        "Center this rectangle within the one passed to the method, which is
usually, but not necessarily, the larger one.", "");
    %pythoncode { CentreIn = CenterIn }
    
    
    int x, y, width, height;


    %extend {
       DocStr(Set, "Set all rectangle properties.", "");
        void Set(int x=0, int y=0, int width=0, int height=0) {
            self->x = x;
            self->y = y;
            self->width = width;
            self->height = height;
        }

        DocAStr(Get,
               "Get() -> (x,y,width,height)",
               "Return the rectangle properties as a tuple.", "");
        PyObject* Get() {
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            PyObject* tup = PyTuple_New(4);
            PyTuple_SET_ITEM(tup, 0, PyInt_FromLong(self->x));
            PyTuple_SET_ITEM(tup, 1, PyInt_FromLong(self->y));
            PyTuple_SET_ITEM(tup, 2, PyInt_FromLong(self->width));
            PyTuple_SET_ITEM(tup, 3, PyInt_FromLong(self->height));
            wxPyEndBlockThreads(blocked);
            return tup;
        }
    }

    %pythoncode {
    asTuple = wx._deprecated(Get, "asTuple is deprecated, use `Get` instead")
    def __str__(self):                   return str(self.Get())
    def __repr__(self):                  return 'wx.Rect'+str(self.Get())
    def __len__(self):                   return len(self.Get())
    def __getitem__(self, index):        return self.Get()[index]
    def __setitem__(self, index, val):
        if index == 0: self.x = val
        elif index == 1: self.y = val
        elif index == 2: self.width = val
        elif index == 3: self.height = val
        else: raise IndexError
    def __nonzero__(self):               return self.Get() != (0,0,0,0)
    __safe_for_unpickling__ = True
    def __reduce__(self):                return (wx.Rect, self.Get())
    }

    
    %property(Bottom, GetBottom, SetBottom, doc="See `GetBottom` and `SetBottom`");
    %property(BottomRight, GetBottomRight, SetBottomRight, doc="See `GetBottomRight` and `SetBottomRight`");
    %property(BottomLeft, GetBottomLeft, SetBottomLeft, doc="See `GetBottomLeft` and `SetBottomLeft`");
    %property(Height, GetHeight, SetHeight, doc="See `GetHeight` and `SetHeight`");
    %property(Left, GetLeft, SetLeft, doc="See `GetLeft` and `SetLeft`");
    %property(Position, GetPosition, SetPosition, doc="See `GetPosition` and `SetPosition`");
    %property(Right, GetRight, SetRight, doc="See `GetRight` and `SetRight`");
    %property(Size, GetSize, SetSize, doc="See `GetSize` and `SetSize`");
    %property(Top, GetTop, SetTop, doc="See `GetTop` and `SetTop`");
    %property(TopLeft, GetTopLeft, SetTopLeft, doc="See `GetTopLeft` and `SetTopLeft`");
    %property(TopRight, GetTopRight, SetTopRight, doc="See `GetTopRight` and `SetTopRight`");
    %property(Width, GetWidth, SetWidth, doc="See `GetWidth` and `SetWidth`");
    %property(X, GetX, SetX, doc="See `GetX` and `SetX`");
    %property(Y, GetY, SetY, doc="See `GetY` and `SetY`");

    %property(Empty, IsEmpty, doc="See `IsEmpty`");
};


MustHaveApp(wxIntersectRect);

DocAStr(wxIntersectRect,
       "IntersectRect(Rect r1, Rect r2) -> Rect",
       "Calculate and return the intersection of r1 and r2.", "");
%inline %{
    PyObject* wxIntersectRect(wxRect* r1, wxRect* r2) {
        wxRegion  reg1(*r1);
        wxRegion  reg2(*r2);
        wxRect    dest(0,0,0,0);
        PyObject* obj;

        reg1.Intersect(reg2);
        dest = reg1.GetBox();

        if (dest != wxRect(0,0,0,0)) {
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            wxRect* newRect = new wxRect(dest);
            obj = wxPyConstructObject((void*)newRect, wxT("wxRect"), true);
            wxPyEndBlockThreads(blocked);
            return obj;
        }
        Py_INCREF(Py_None);
        return Py_None;
    }
%}

//---------------------------------------------------------------------------
%newgroup


DocStr(wxPoint2D,
      "wx.Point2Ds represent a point or a vector in a 2d coordinate system
with floating point values.", "");

class wxPoint2D
{
public:
    DocStr(wxPoint2D, "Create a w.Point2D object.", "");
    wxPoint2D( double x=0.0 , double y=0.0 );
    %RenameCtor(Point2DCopy, wxPoint2D( const wxPoint2D &pt ));
    %RenameCtor(Point2DFromPoint, wxPoint2D( const wxPoint &pt ));

    DocDeclAStr(
        void, GetFloor( int *OUTPUT , int *OUTPUT ) const,
        "GetFloor() -> (x,y)",
        "Convert to integer", "");
    
    DocDeclAStr(
        void, GetRounded( int *OUTPUT , int *OUTPUT ) const,
        "GetRounded() -> (x,y)",
        "Convert to integer", "");

    double GetVectorLength() const;
    double GetVectorAngle() const ;
    void SetVectorLength( double length );
    void SetVectorAngle( double degrees );

    // LinkError: void SetPolarCoordinates( double angle , double length );
    // LinkError: void Normalize();
    %pythoncode {
    def SetPolarCoordinates(self, angle, length):
        self.SetVectorLength(length)
        self.SetVectorAngle(angle)
    def Normalize(self):
        self.SetVectorLength(1.0)
    }

    double GetDistance( const wxPoint2D &pt ) const;
    double GetDistanceSquare( const wxPoint2D &pt ) const;
    double GetDotProduct( const wxPoint2D &vec ) const;
    double GetCrossProduct( const wxPoint2D &vec ) const;

    DocDeclStr(
        wxPoint2D, operator-(),
        "the reflection of this point", "");

    wxPoint2D& operator+=(const wxPoint2D& pt);
    wxPoint2D& operator-=(const wxPoint2D& pt);

    wxPoint2D& operator*=(const wxPoint2D& pt);
    wxPoint2D& operator/=(const wxPoint2D& pt);

    %extend {
        KeepGIL(__eq__);
        DocStr(__eq__, "Test for equality of wx.Point2D objects.", "");
        bool __eq__(PyObject* other) {
            wxPoint2D  temp, *obj = &temp;
            if ( other == Py_None ) return false;
            if ( ! wxPoint2D_helper(other, &obj) ) {
                PyErr_Clear();
                return false;
            }
            return self->operator==(*obj);
        }

        
        KeepGIL(__ne__);
        DocStr(__ne__, "Test for inequality of wx.Point2D objects.", "");
        bool __ne__(PyObject* other) {
            wxPoint2D  temp, *obj = &temp;
            if ( other == Py_None ) return true;
            if ( ! wxPoint2D_helper(other, &obj)) {
                PyErr_Clear();
                return true;
            }
            return self->operator!=(*obj);
        }
    }

    %Rename(x, double,  m_x);
    %Rename(y, double,  m_y);

    %extend {
        void Set( double x=0 , double y=0 ) {
            self->m_x = x;
            self->m_y = y;
        }

        DocAStr(Get,
               "Get() -> (x,y)",
               "Return x and y properties as a tuple.", "");               
        PyObject* Get() {
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, PyFloat_FromDouble(self->m_x));
            PyTuple_SET_ITEM(tup, 1, PyFloat_FromDouble(self->m_y));
            wxPyEndBlockThreads(blocked);
            return tup;
        }
    }

    %pythoncode {
    asTuple = wx._deprecated(Get, "asTuple is deprecated, use `Get` instead")
    def __str__(self):                   return str(self.Get())
    def __repr__(self):                  return 'wx.Point2D'+str(self.Get())
    def __len__(self):                   return len(self.Get())
    def __getitem__(self, index):        return self.Get()[index]
    def __setitem__(self, index, val):
        if index == 0: self.x = val
        elif index == 1: self.y = val
        else: raise IndexError
    def __nonzero__(self):               return self.Get() != (0.0, 0.0)
    __safe_for_unpickling__ = True
    def __reduce__(self):                return (wx.Point2D, self.Get())
    }

    %property(Floor, GetFloor, doc="See `GetFloor`");
    %property(Rounded, GetRounded, doc="See `GetRounded`");
    %property(VectorAngle, GetVectorAngle, SetVectorAngle, doc="See `GetVectorAngle` and `SetVectorAngle`");
    %property(VectorLength, GetVectorLength, SetVectorLength, doc="See `GetVectorLength` and `SetVectorLength`");
    
};


//---------------------------------------------------------------------------

%immutable;
const wxPoint     wxDefaultPosition;
const wxSize      wxDefaultSize;
%mutable;

//---------------------------------------------------------------------------
