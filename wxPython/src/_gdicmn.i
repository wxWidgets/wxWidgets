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
    wxBITMAP_TYPE_MACCURSOR,
    wxBITMAP_TYPE_MACCURSOR_RESOURCE,
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

class wxSize
{
public:
    %name(width) int x;
    %name(height)int y;

    wxSize(int w=0, int h=0);
    ~wxSize();

    bool operator==(const wxSize& sz) const;
    bool operator!=(const wxSize& sz) const;

    wxSize operator+(const wxSize& sz);
    wxSize operator-(const wxSize& sz);

    void IncTo(const wxSize& sz);
    void DecTo(const wxSize& sz);

    void Set(int xx, int yy);
    void SetWidth(int w);
    void SetHeight(int h);
    int GetWidth() const;
    int GetHeight() const;

    int GetX() const;
    int GetY() const;

    %extend {
        PyObject* asTuple() {
            wxPyBeginBlockThreads();
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, PyInt_FromLong(self->x));
            PyTuple_SET_ITEM(tup, 1, PyInt_FromLong(self->y));
            wxPyEndBlockThreads();
            return tup;
        }
    }
    %pythoncode {
    def __str__(self):                   return str(self.asTuple())
    def __repr__(self):                  return 'wxSize'+str(self.asTuple())
    def __len__(self):                   return len(self.asTuple())
    def __getitem__(self, index):        return self.asTuple()[index]
    def __setitem__(self, index, val):
        if index == 0: self.width = val
        elif index == 1: self.height = val
        else: raise IndexError
    def __nonzero__(self):               return self.asTuple() != (0,0)
    def __getinitargs__(self):           return ()
    def __getstate__(self):              return self.asTuple()
    def __setstate__(self, state):       self.Set(*state)
    }

};

//---------------------------------------------------------------------------
%newgroup


class wxRealPoint
{
public:
    double x;
    double y;

    wxRealPoint(double x=0.0, double y=0.0);
    ~wxRealPoint();

    wxRealPoint operator+(const wxRealPoint& pt) const;
    wxRealPoint operator-(const wxRealPoint& pt) const;

    bool operator==(const wxRealPoint& pt) const;
    bool operator!=(const wxRealPoint& pt) const;

    %extend {
        void Set(double x, double y) {
            self->x = x;
            self->y = y;
        }
        PyObject* asTuple() {
            wxPyBeginBlockThreads();
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, PyFloat_FromDouble(self->x));
            PyTuple_SET_ITEM(tup, 1, PyFloat_FromDouble(self->y));
            wxPyEndBlockThreads();
            return tup;
        }
    }

    %pythoncode {
    def __str__(self):                   return str(self.asTuple())
    def __repr__(self):                  return 'wxRealPoint'+str(self.asTuple())
    def __len__(self):                   return len(self.asTuple())
    def __getitem__(self, index):        return self.asTuple()[index]
    def __setitem__(self, index, val):
        if index == 0: self.width = val
        elif index == 1: self.height = val
        else: raise IndexError
    def __nonzero__(self):               return self.asTuple() != (0.0, 0.0)
    def __getinitargs__(self):           return ()
    def __getstate__(self):              return self.asTuple()
    def __setstate__(self, state):       self.Set(*state)
    }
};

//---------------------------------------------------------------------------
%newgroup

class wxPoint
{
public:
    int x, y;

    wxPoint(int x=0, int y=0);
    ~wxPoint();

    bool operator==(const wxPoint& p) const;
    bool operator!=(const wxPoint& p) const;

    wxPoint operator+(const wxPoint& p) const;
    wxPoint operator-(const wxPoint& p) const;

    wxPoint& operator+=(const wxPoint& p);
    wxPoint& operator-=(const wxPoint& p);

    %extend {
        void Set(long x, long y) {
            self->x = x;
            self->y = y;
        }
        PyObject* asTuple() {
            wxPyBeginBlockThreads();
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, PyInt_FromLong(self->x));
            PyTuple_SET_ITEM(tup, 1, PyInt_FromLong(self->y));
            wxPyEndBlockThreads();
            return tup;
        }
    }

    %pythoncode {
    def __str__(self):                   return str(self.asTuple())
    def __repr__(self):                  return 'wxPoint'+str(self.asTuple())
    def __len__(self):                   return len(self.asTuple())
    def __getitem__(self, index):        return self.asTuple()[index]
    def __setitem__(self, index, val):
        if index == 0: self.x = val
        elif index == 1: self.y = val
        else: raise IndexError
    def __nonzero__(self):               return self.asTuple() != (0,0)
    def __getinitargs__(self):           return ()
    def __getstate__(self):              return self.asTuple()
    def __setstate__(self, state):       self.Set(*state)
    }
};

//---------------------------------------------------------------------------
%newgroup

class wxRect
{
public:
    wxRect(int x=0, int y=0, int width=0, int height=0);
    %name(RectPP) wxRect(const wxPoint& topLeft, const wxPoint& bottomRight);
    %name(RectPS) wxRect(const wxPoint& pos, const wxSize& size);
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

    int GetLeft()   const;
    int GetTop()    const;
    int GetBottom() const;
    int GetRight()  const;

    void SetLeft(int left);
    void SetRight(int right);
    void SetTop(int top);
    void SetBottom(int bottom);

    wxRect& Inflate(wxCoord dx, wxCoord dy);
    wxRect& Deflate(wxCoord dx, wxCoord dy);

    %name(OffsetXY)void Offset(wxCoord dx, wxCoord dy);
    void Offset(const wxPoint& pt);

    wxRect& Intersect(const wxRect& rect);

    wxRect operator+(const wxRect& rect) const;
    wxRect& operator+=(const wxRect& rect);

    bool operator==(const wxRect& rect) const;
    bool operator!=(const wxRect& rect) const { return !(*this == rect); }

    // return TRUE if the point is (not strcitly) inside the rect
    %name(InsideXY)bool Inside(int x, int y) const;
    bool Inside(const wxPoint& pt) const;

    // return TRUE if the rectangles have a non empty intersection
    bool Intersects(const wxRect& rect) const;

    int x, y, width, height;


    %extend {
         void Set(int x=0, int y=0, int width=0, int height=0) {
            self->x = x;
            self->y = y;
            self->width = width;
            self->height = height;
        }

        PyObject* asTuple() {
            wxPyBeginBlockThreads();
            PyObject* tup = PyTuple_New(4);
            PyTuple_SET_ITEM(tup, 0, PyInt_FromLong(self->x));
            PyTuple_SET_ITEM(tup, 1, PyInt_FromLong(self->y));
            PyTuple_SET_ITEM(tup, 2, PyInt_FromLong(self->width));
            PyTuple_SET_ITEM(tup, 3, PyInt_FromLong(self->height));
            wxPyEndBlockThreads();
            return tup;
        }
    }

    %pythoncode {
    def __str__(self):                   return str(self.asTuple())
    def __repr__(self):                  return 'wxRect'+str(self.asTuple())
    def __len__(self):                   return len(self.asTuple())
    def __getitem__(self, index):        return self.asTuple()[index]
    def __setitem__(self, index, val):
        if index == 0: self.x = val
        elif index == 1: self.y = val
        elif index == 2: self.width = val
        elif index == 3: self.height = val
        else: raise IndexError
    def __nonzero__(self):               return self.asTuple() != (0,0,0,0)
    def __getinitargs__(self):           return ()
    def __getstate__(self):              return self.asTuple()
    def __setstate__(self, state):       self.Set(*state)
    }
};


%inline %{
    PyObject* wxIntersectRect(wxRect* r1, wxRect* r2) {
        wxRegion  reg1(*r1);
        wxRegion  reg2(*r2);
        wxRect    dest(0,0,0,0);
        PyObject* obj;

        reg1.Intersect(reg2);
        dest = reg1.GetBox();

        if (dest != wxRect(0,0,0,0)) {
            wxPyBeginBlockThreads();
            wxRect* newRect = new wxRect(dest);
            obj = wxPyConstructObject((void*)newRect, wxT("wxRect"), true);
            wxPyEndBlockThreads();
            return obj;
        }
        Py_INCREF(Py_None);
        return Py_None;
    }
%}

//---------------------------------------------------------------------------
%newgroup

%{
%}


// wxPoint2Ds represent a point or a vector in a 2d coordinate system

class wxPoint2D
{
public :
    wxPoint2D( double x=0.0 , double y=0.0 );
    %name(Point2DCopy) wxPoint2D( const wxPoint2D &pt );
    %name(Point2DFromPoint) wxPoint2D( const wxPoint &pt );

    // two different conversions to integers, floor and rounding
    void GetFloor( int *OUTPUT , int *OUTPUT ) const;
    void GetRounded( int *OUTPUT , int *OUTPUT ) const;

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

    // the reflection of this point
    wxPoint2D operator-();

    wxPoint2D& operator+=(const wxPoint2D& pt);
    wxPoint2D& operator-=(const wxPoint2D& pt);

    wxPoint2D& operator*=(const wxPoint2D& pt);
    wxPoint2D& operator/=(const wxPoint2D& pt);

    bool operator==(const wxPoint2D& pt) const;
    bool operator!=(const wxPoint2D& pt) const;

    double m_x;
    double m_y;
    %name(x)double m_x;
    %name(y)double m_y;

    %extend {
        void Set( double x=0 , double y=0 ) {
            self->m_x = x;
            self->m_y = y;
        }
        PyObject* asTuple() {
            wxPyBeginBlockThreads();
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, PyFloat_FromDouble(self->m_x));
            PyTuple_SET_ITEM(tup, 1, PyFloat_FromDouble(self->m_y));
            wxPyEndBlockThreads();
            return tup;
        }
    }

    %pythoncode {
    def __str__(self):                   return str(self.asTuple())
    def __repr__(self):                  return 'wxPoint2D'+str(self.asTuple())
    def __len__(self):                   return len(self.asTuple())
    def __getitem__(self, index):        return self.asTuple()[index]
    def __setitem__(self, index, val):
        if index == 0: self.m_x = val
        elif index == 1: self.m_yt = val
        else: raise IndexError
    def __nonzero__(self):               return self.asTuple() != (0.0, 0.0)
    def __getinitargs__(self):           return ()
    def __getstate__(self):              return self.asTuple()
    def __setstate__(self, state):       self.Set(*state)

    }
};


//---------------------------------------------------------------------------

%immutable;
const wxPoint     wxDefaultPosition;
const wxSize      wxDefaultSize;
%mutable;

//---------------------------------------------------------------------------
