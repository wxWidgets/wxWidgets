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
something.  It simply contians integer width and height proprtites.
In most places in wxPython where a wx.Size is expected a
(width,height) tuple can be used instead.", "");

class wxSize
{
public:
    %name(width) int x;
    %name(height)int y;
    %pythoncode { x = width; y = height }
    
    DocCtorStr(
        wxSize(int w=0, int h=0),
        "Creates a size object.", "");

    ~wxSize();

// None/NULL is now handled properly by the typemap, so these are not needed.
//     %extend {
//         bool __eq__(const wxSize* other) { return other ? (*self == *other) : false; }
//         bool __ne__(const wxSize* other) { return other ? (*self != *other) : true;  }
//     }

    DocDeclStr(
        bool, operator==(const wxSize& sz),
        "Test for equality of wx.Size objects.", "");

    DocDeclStr(
        bool, operator!=(const wxSize& sz),
        "Test for inequality.", "");

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
            bool blocked = wxPyBeginBlockThreads();
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

    DocDeclStr(
        bool, operator==(const wxRealPoint& pt),
        "Test for equality of wx.RealPoint objects.", "");

    DocDeclStr(
        bool, operator!=(const wxRealPoint& pt),
        "Test for inequality of wx.RealPoint objects.", "");

    
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
            bool blocked = wxPyBeginBlockThreads();
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

    
    DocDeclStr(
        bool, operator==(const wxPoint& pt),
        "Test for equality of wx.Point objects.", "");

    DocDeclStr(
        bool, operator!=(const wxPoint& pt),
        "Test for inequality of wx.Point objects.", "");



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
            bool blocked = wxPyBeginBlockThreads();
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

    wxPoint GetTopLeft() const;
    void SetTopLeft(const wxPoint &p);
    wxPoint GetBottomRight() const;
    void SetBottomRight(const wxPoint &p);

//    wxPoint GetLeftTop() const;
//    void SetLeftTop(const wxPoint &p);
//    wxPoint GetRightBottom() const;
//    void SetRightBottom(const wxPoint &p);

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
        "Increase the rectangle size by dx in x direction and dy in y
direction. Both or one of) parameters may be negative to decrease the
rectangle size.", "");

    DocDeclStr(
        wxRect&, Deflate(wxCoord dx, wxCoord dy),
        "Decrease the rectangle size by dx in x direction and dy in y
direction. Both or one of) parameters may be negative to increase the
rectngle size. This method is the opposite of Inflate.", "");

    DocDeclStrName(
        void, Offset(wxCoord dx, wxCoord dy),
        "Moves the rectangle by the specified offset. If dx is positive, the
rectangle is moved to the right, if dy is positive, it is moved to the
bottom, otherwise it is moved to the left or top respectively.", "",
        OffsetXY);
    
    DocDeclStr(
        void, Offset(const wxPoint& pt),
        "Same as OffsetXY but uses dx,dy from Point", "");

    DocDeclStr(
        wxRect&, Intersect(const wxRect& rect),
        "Return the intersectsion of this rectangle and rect.", "");

    DocDeclStr(
        wxRect, operator+(const wxRect& rect) const,
        "Add the properties of rect to this rectangle and return the result.", "");

    DocDeclStr(
        wxRect&, operator+=(const wxRect& rect),
        "Add the properties of rect to this rectangle, updating this rectangle.", "");

    DocDeclStr(
        bool, operator==(const wxRect& rect) const,
        "Test for equality.", "");

    DocDeclStr(
        bool, operator!=(const wxRect& rect) const,
        "Test for inequality.", "");

    
    DocStr( Inside, "Return True if the point is (not strcitly) inside the rect.", "");
    %name(InsideXY)bool Inside(int x, int y) const;
    bool Inside(const wxPoint& pt) const;

    DocDeclStr(    
        bool, Intersects(const wxRect& rect) const,
        "Returns True if the rectangles have a non empty intersection.", "");

    
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
            bool blocked = wxPyBeginBlockThreads();
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
            bool blocked = wxPyBeginBlockThreads();
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
    %name(Point2DCopy) wxPoint2D( const wxPoint2D &pt );
    %name(Point2DFromPoint) wxPoint2D( const wxPoint &pt );

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

    DocDeclStr(
        bool, operator==(const wxPoint2D& pt) const,
        "Test for equality", "");
    
    DocDeclStr(
        bool, operator!=(const wxPoint2D& pt) const,
        "Test for inequality", "");

    %name(x)double m_x;
    %name(y)double m_y;

    %extend {
        void Set( double x=0 , double y=0 ) {
            self->m_x = x;
            self->m_y = y;
        }

        DocAStr(Get,
               "Get() -> (x,y)",
               "Return x and y properties as a tuple.", "");               
        PyObject* Get() {
            bool blocked = wxPyBeginBlockThreads();
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
};


//---------------------------------------------------------------------------

%immutable;
const wxPoint     wxDefaultPosition;
const wxSize      wxDefaultSize;
%mutable;

//---------------------------------------------------------------------------
