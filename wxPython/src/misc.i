/////////////////////////////////////////////////////////////////////////////
// Name:        misc.i
// Purpose:     Definitions of miscelaneous functions and classes
//
// Author:      Robin Dunn
//
// Created:     7/3/97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module misc

%{
#include "helpers.h"
#include <wx/resource.h>
#include <wx/tooltip.h>
#include <wx/busyinfo.h>
#include <wx/geometry.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i


//---------------------------------------------------------------------------
%{
    // Put some wx default wxChar* values into wxStrings.
    static const wxString wxPyEmptyString(wxT(""));
%}
//---------------------------------------------------------------------------


class wxObject {
public:

    %addmethods {
        wxString GetClassName() {
            return self->GetClassInfo()->GetClassName();
        }

        void Destroy() {
            delete self;
        }
    }
};

//---------------------------------------------------------------------------

class wxSize {
public:
    long x;
    long y;
    %name(width) long x;
    %name(height)long y;

    wxSize(long w=0, long h=0);
    ~wxSize();
    void Set(long w, long h);
    long GetX();
    long GetY();
    long GetWidth();
    long GetHeight();
    void SetWidth(long w);
    void SetHeight(long h);

    %addmethods {
        PyObject* asTuple() {
            wxPyBeginBlockThreads();
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, PyInt_FromLong(self->x));
            PyTuple_SET_ITEM(tup, 1, PyInt_FromLong(self->y));
            wxPyEndBlockThreads();
            return tup;
        }

        bool __eq__(PyObject* obj) {
            wxSize  tmp;
            wxSize* ptr = &tmp;
            if (obj == Py_None)    return FALSE;
            wxPyBLOCK_THREADS(bool success = wxSize_helper(obj, &ptr); PyErr_Clear());
            if (! success)         return FALSE;
            return *self == *ptr;
        }
        bool __ne__(PyObject* obj) {
            wxSize  tmp;
            wxSize* ptr = &tmp;
            if (obj == Py_None)    return TRUE;
            wxPyBLOCK_THREADS(bool success = wxSize_helper(obj, &ptr); PyErr_Clear());
            if (! success)         return TRUE;
            return *self != *ptr;
        }

    }

    %pragma(python) addtoclass = "
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
"

};

//---------------------------------------------------------------------------

class wxRealPoint {
public:
    double x;
    double y;
    wxRealPoint(double x=0.0, double y=0.0);
    ~wxRealPoint();

    %addmethods {
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

        wxRealPoint __add__(const wxRealPoint& p) {
            return *self + p;
        }

        wxRealPoint __sub__(const wxRealPoint& p) {
            return *self - p;
        }

        bool __eq__(PyObject* obj) {
            wxRealPoint  tmp;
            wxRealPoint* ptr = &tmp;
            if (obj == Py_None)    return FALSE;
            wxPyBLOCK_THREADS(bool success = wxRealPoint_helper(obj, &ptr); PyErr_Clear());
            if (! success)         return FALSE;
            return *self == *ptr;
        }
        bool __ne__(PyObject* obj) {
            wxRealPoint  tmp;
            wxRealPoint* ptr = &tmp;
            if (obj == Py_None)    return TRUE;
            wxPyBLOCK_THREADS(bool success = wxRealPoint_helper(obj, &ptr); PyErr_Clear());
            if (! success)         return TRUE;
            return *self != *ptr;
        }

    }

    %pragma(python) addtoclass = "
    def __str__(self):                   return str(self.asTuple())
    def __repr__(self):                  return 'wxRealPoint'+str(self.asTuple())
    def __len__(self):                   return len(self.asTuple())
    def __getitem__(self, index):        return self.asTuple()[index]
    def __setitem__(self, index, val):
        if index == 0: self.width = val
        elif index == 1: self.height = val
        else: raise IndexError
    def __nonzero__(self):      return self.asTuple() != (0.0, 0.0)
    def __getinitargs__(self):           return ()
    def __getstate__(self):              return self.asTuple()
    def __setstate__(self, state):       self.Set(*state)
"
};


class wxPoint {
public:
    long x;
    long y;
    wxPoint(long x=0, long y=0);
    ~wxPoint();

    %addmethods {
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

        wxPoint __add__(const wxPoint& p) {
            return *self + p;
        }

        wxPoint __sub__(const wxPoint& p) {
            return *self - p;
        }

        bool __eq__(PyObject* obj) {
            wxPoint  tmp;
            wxPoint* ptr = &tmp;
            if (obj == Py_None)    return FALSE;
            wxPyBLOCK_THREADS(bool success = wxPoint_helper(obj, &ptr); PyErr_Clear());
            if (! success)         return FALSE;
            return *self == *ptr;
        }
        bool __ne__(PyObject* obj) {
            wxPoint  tmp;
            wxPoint* ptr = &tmp;
            if (obj == Py_None)    return TRUE;
            wxPyBLOCK_THREADS(bool success = wxPoint_helper(obj, &ptr); PyErr_Clear());
            if (! success)         return TRUE;
            return *self != *ptr;
        }

    }

    %pragma(python) addtoclass = "
    def __str__(self):                   return str(self.asTuple())
    def __repr__(self):                  return 'wxPoint'+str(self.asTuple())
    def __len__(self):                   return len(self.asTuple())
    def __getitem__(self, index):        return self.asTuple()[index]
    def __setitem__(self, index, val):
        if index == 0: self.x = val
        elif index == 1: self.y = val
        else: raise IndexError
    def __nonzero__(self):      return self.asTuple() != (0,0)
    def __getinitargs__(self):           return ()
    def __getstate__(self):              return self.asTuple()
    def __setstate__(self, state):       self.Set(*state)
"
};

//---------------------------------------------------------------------------

class wxRect {
public:
    wxRect(int x=0, int y=0, int width=0, int height=0);
    // TODO: do this one too... wxRect(const wxPoint& pos, const wxSize& size);
    ~wxRect();

    int  GetX();
    void SetX(int X);
    int  GetY();
    void SetY(int Y);
    int  GetWidth();
    void SetWidth(int w);
    int  GetHeight();
    void SetHeight(int h);


    wxPoint GetPosition();
    wxSize GetSize();
    void SetPosition( const wxPoint &p );
    void SetSize( const wxSize &s );

    int  GetLeft();
    int  GetTop();
    int  GetBottom();
    int  GetRight();

    void SetLeft(int left);
    void SetRight(int right);
    void SetTop(int top);
    void SetBottom(int bottom);

    void Deflate(int dx, int dy);
    void Inflate(int dx, int dy);
    %name(InsideXY)bool Inside(int cx, int cy);
    bool Inside(const wxPoint& pt);
    bool Intersects(const wxRect& rect);
    %name(OffsetXY) void Offset(int dx, int dy);
    void Offset(const wxPoint& pt);

    int x, y, width, height;

    %addmethods {
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

        wxRect __add__(const wxRect& rect) {
            return *self + rect;
        }

        bool __eq__(PyObject* obj) {
            wxRect  tmp;
            wxRect* ptr = &tmp;
            if (obj == Py_None)    return FALSE;
            wxPyBLOCK_THREADS(bool success = wxRect_helper(obj, &ptr); PyErr_Clear());
            if (! success)         return FALSE;
            return *self == *ptr;
        }
        bool __ne__(PyObject* obj) {
            wxRect  tmp;
            wxRect* ptr = &tmp;
            if (obj == Py_None)    return TRUE;
            wxPyBLOCK_THREADS(bool success = wxRect_helper(obj, &ptr); PyErr_Clear());
            if (! success)         return TRUE;
            return *self != *ptr;
        }

    }

    %pragma(python) addtoclass = "
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

    # override the __getattr__ made by SWIG
    def __getattr__(self, name):
        d = {
            'x' : miscc.wxRect_x_get,
            'y' : miscc.wxRect_y_get,
            'width' : miscc.wxRect_width_get,
            'height' : miscc.wxRect_height_get,
            'top' : miscc.wxRect_GetTop,
            'bottom' : miscc.wxRect_GetBottom,
            'left' : miscc.wxRect_GetLeft,
            'right' : miscc.wxRect_GetRight,
            }
        try:
            func = d[name]
        except KeyError:
            raise AttributeError,name
        return func(self)

    # and also the __setattr__
    def __setattr__(self, name, value):
        d = {
            'x' : miscc.wxRect_x_set,
            'y' : miscc.wxRect_y_set,
            'width' : miscc.wxRect_width_set,
            'height' : miscc.wxRect_height_set,
            'top' : miscc.wxRect_SetTop,
            'bottom' : miscc.wxRect_SetBottom,
            'left' : miscc.wxRect_SetLeft,
            'right' : miscc.wxRect_SetRight,
            }
        try:
            func = d[name]
        except KeyError:
            self.__dict__[name] = value
            return
        func(self, value)
    "

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
            obj = wxPyConstructObject((void*)newRect, wxT("wxRect"));
            PyObject* one = PyInt_FromLong(1);
            PyObject_SetAttrString(obj, "thisown", one);
            Py_DECREF(one);
            wxPyEndBlockThreads();
            return obj;
        }
        Py_INCREF(Py_None);
        return Py_None;
    }
%}



//---------------------------------------------------------------------------
// wxPoint2Ds represent a point or a vector in a 2d coordinate system

class wxPoint2DDouble
{
public:
    double m_x;
    double m_y;

    %name(x)double m_x;
    %name(y)double m_y;

    wxPoint2DDouble( double x=0 , double y=0 );
    %name(wxPoint2DDoubleCopy)wxPoint2DDouble( const wxPoint2DDouble &pt );
    %name(wxPoint2DDoubleFromPoint)wxPoint2DDouble( const wxPoint &pt );

    // two different conversions to integers, floor and rounding
    void GetFloor( int* OUTPUT , int* OUTPUT ) const;
    void GetRounded( int* OUTPUT , int* OUTPUT ) const;

    double GetVectorLength() const;
    double GetVectorAngle() const ;
    void SetVectorLength( double length );
    void SetVectorAngle( double degrees );
    // LinkError: void SetPolarCoordinates( double angle , double length );
    // LinkError: void Normalize();
    %pragma(python) addtoclass = "
    def SetPolarCoordinates(self, angle, length):
        self.SetVectorLength(length)
        self.SetVectorAngle(angle)
    def Normalize(self):
        self.SetVectorLength(1.0)
    "

    double GetDistance( const wxPoint2DDouble &pt ) const;
    double GetDistanceSquare( const wxPoint2DDouble &pt ) const;
    double GetDotProduct( const wxPoint2DDouble &vec ) const;
    double GetCrossProduct( const wxPoint2DDouble &vec ) const;

    %addmethods {
        void Set( double x=0 , double y=0 ) {
            self->m_x = x;
            self->m_y = y;
        }

        // the reflection of this point
        wxPoint2DDouble __neg__() { return -(*self); }

        wxPoint2DDouble& __iadd__(const wxPoint2DDouble& pt) { return (*self) += pt; }
        wxPoint2DDouble& __isub__(const wxPoint2DDouble& pt) { return (*self) -= pt; }
        wxPoint2DDouble& __imul__(const wxPoint2DDouble& pt) { return (*self) *= pt; }
        wxPoint2DDouble& __idiv__(const wxPoint2DDouble& pt) { return (*self) /= pt; }

        // TODO:
        //wxPoint2DDouble& operator*=(double n);
        //wxPoint2DDouble& operator*=(int n);
        //wxPoint2DDouble& operator/=(double n);
        //wxPoint2DDouble& operator/=(int n);

        bool __eq__(PyObject* obj) {
            wxPoint2DDouble  tmp;
            wxPoint2DDouble* ptr = &tmp;
            if (obj == Py_None)    return FALSE;
            wxPyBLOCK_THREADS(bool success = wxPoint2DDouble_helper(obj, &ptr); PyErr_Clear());
            if (! success)         return FALSE;
            return *self == *ptr;
        }
        bool __ne__(PyObject* obj) {
            wxPoint2DDouble  tmp;
            wxPoint2DDouble* ptr = &tmp;
            if (obj == Py_None)    return TRUE;
            wxPyBLOCK_THREADS(bool success = wxPoint2DDouble_helper(obj, &ptr); PyErr_Clear());
            if (! success)         return TRUE;
            return *self != *ptr;
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

    %pragma(python) addtoclass = "
    def __str__(self):                   return str(self.asTuple())
    def __repr__(self):                  return 'wxPoint2DDouble'+str(self.asTuple())
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
"
};


//---------------------------------------------------------------------------
// Miscellaneous functions

long wxNewId();
void wxRegisterId(long id);
long wxGetCurrentId();

void wxBell();
void wxEndBusyCursor();

long wxGetElapsedTime(bool resetTimer = TRUE);
#ifdef __WXMSW__
long wxGetFreeMemory();
#endif
void wxGetMousePosition(int* OUTPUT, int* OUTPUT);
bool wxIsBusy();
wxString wxNow();
bool wxShell(const wxString& command = wxPyEmptyString);
void wxStartTimer();
int wxGetOsVersion(int *OUTPUT, int *OUTPUT);
wxString wxGetOsDescription();

enum wxShutdownFlags
{
    wxSHUTDOWN_POWEROFF,    // power off the computer
    wxSHUTDOWN_REBOOT       // shutdown and reboot
};

// Shutdown or reboot the PC
bool wxShutdown(wxShutdownFlags wFlags);


void wxSleep(int secs);
void wxUsleep(unsigned long milliseconds);
bool wxYield();
bool wxYieldIfNeeded();
void wxEnableTopLevelWindows(bool enable);

#ifdef wxUSE_RESOURCES
inline %{
    wxString wxGetResource(const wxString& section, const wxString& entry,
                           const wxString& file = wxPyEmptyString) {
        wxChar* retval;
        wxGetResource(section, entry, &retval, file);
        return retval;
    }
%}
#endif

wxString wxStripMenuCodes(const wxString& in);


wxString wxGetEmailAddress();
wxString wxGetHostName();
wxString wxGetFullHostName();
wxString wxGetUserId();
wxString wxGetUserName();
wxString wxGetHomeDir();
wxString wxGetUserHome(const wxString& user = wxPyEmptyString);

unsigned long wxGetProcessId();

// When wxApp gets the virtual method magic then enable this.
// bool wxHandleFatalExceptions(bool doIt = TRUE);

//----------------------------------------------------------------------

enum wxEdge { wxLeft, wxTop, wxRight, wxBottom, wxWidth, wxHeight,
              wxCentre, wxCenter = wxCentre, wxCentreX, wxCentreY };
enum wxRelationship { wxUnconstrained = 0,
                      wxAsIs,
                      wxPercentOf,
                      wxAbove,
                      wxBelow,
                      wxLeftOf,
                      wxRightOf,
                      wxSameAs,
                      wxAbsolute };


class wxIndividualLayoutConstraint : public wxObject {
public:
//    wxIndividualLayoutConstraint();
//    ~wxIndividualLayoutConstraint();

    void Above(wxWindow *otherWin, int margin=0);
    void Absolute(int value);
    void AsIs();
    void Below(wxWindow *otherWin, int margin=0);
    void Unconstrained();
    void LeftOf(wxWindow *otherWin, int margin=0);
    void PercentOf(wxWindow *otherWin, wxEdge edge, int percent);
    void RightOf(wxWindow *otherWin, int margin=0);
    void SameAs(wxWindow *otherWin, wxEdge edge, int margin=0);
    void Set(wxRelationship rel, wxWindow *otherWin, wxEdge otherEdge, int value=0, int margin=0);
};


class wxLayoutConstraints : public wxObject {
public:
    wxLayoutConstraints();

%readonly
    wxIndividualLayoutConstraint bottom;
    wxIndividualLayoutConstraint centreX;
    wxIndividualLayoutConstraint centreY;
    wxIndividualLayoutConstraint height;
    wxIndividualLayoutConstraint left;
    wxIndividualLayoutConstraint right;
    wxIndividualLayoutConstraint top;
    wxIndividualLayoutConstraint width;
%readwrite
}



//---------------------------------------------------------------------------
// Accelerator Entry and Table

class wxAcceleratorEntry {
public:
    wxAcceleratorEntry(int flags = 0, int keyCode = 0, int cmd = 0);
    ~wxAcceleratorEntry();

    void Set(int flags, int keyCode, int Cmd);
    int GetFlags();
    int GetKeyCode();
    int GetCommand();
};


class wxAcceleratorTable : public wxObject {
public:
    // Can also accept a list of 3-tuples
    wxAcceleratorTable(int LCOUNT, wxAcceleratorEntry* choices);
    ~wxAcceleratorTable();

};

wxAcceleratorEntry *wxGetAccelFromString(const wxString& label);

%readonly
%{
#if 0  // we want to use the definition from the header, not the
       // one SWIG will generate.
%}
extern wxAcceleratorTable wxNullAcceleratorTable;
%{
#endif
%}
%readwrite

//---------------------------------------------------------------------------

class wxBusyInfo : public wxObject {
public:
    wxBusyInfo(const wxString& message);
    ~wxBusyInfo();
};


//---------------------------------------------------------------------------



