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

        int __cmp__(const wxSize* sz) {
            if (! sz) return 1;
            if (*self == *sz) return 0;
            return -1;
        }
    }

    %pragma(python) addtoclass = "
    def __str__(self):                   return str(self.asTuple())
    def __repr__(self):                  return str(self.asTuple())
    def __len__(self):                   return len(self.asTuple())
    def __getitem__(self, index):        return self.asTuple()[index]
    def __setitem__(self, index, val):
        if index == 0: self.width = val
        elif index == 1: self.height = val
        else: raise IndexError
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

        wxRealPoint __add__(const wxRealPoint* p) {
            if (! p) return *self;
            return *self + *p;
        }

        wxRealPoint __sub__(const wxRealPoint* p) {
            if (! p) return *self;
            return *self - *p;
        }

        int __cmp__(const wxRealPoint* p) {
            if (! p) return 1;
            if (*self == *p) return 0;
            return -1;
        }
    }
    %pragma(python) addtoclass = "
    def __str__(self):                   return str(self.asTuple())
    def __repr__(self):                  return str(self.asTuple())
    def __len__(self):                   return len(self.asTuple())
    def __getitem__(self, index):        return self.asTuple()[index]
    def __setitem__(self, index, val):
        if index == 0: self.width = val
        elif index == 1: self.height = val
        else: raise IndexError
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

        wxPoint __add__(const wxPoint* p) {
            if (! p) return *self;
            return *self + *p;
        }

        wxPoint __sub__(const wxPoint* p) {
            if (! p) return *self;
            return *self - *p;
        }

        int __cmp__(const wxPoint* p) {
            if (! p) return 1;
            if (*self == *p) return 0;
            return -1;
        }
    }
    %pragma(python) addtoclass = "
    def __str__(self):                   return str(self.asTuple())
    def __repr__(self):                  return str(self.asTuple())
    def __len__(self):                   return len(self.asTuple())
    def __getitem__(self, index):        return self.asTuple()[index]
    def __setitem__(self, index, val):
        if index == 0: self.x = val
        elif index == 1: self.y = val
        else: raise IndexError
"
};

//---------------------------------------------------------------------------

class wxRect {
public:
    wxRect(int x=0, int y=0, int w=0, int h=0);
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

    int  GetLeft();
    int  GetTop();
    int  GetBottom();
    int  GetRight();

    void SetLeft(int left);
    void SetRight(int right);
    void SetTop(int top);
    void SetBottom(int bottom);

    void Inflate(int dx, int dy);
    bool Inside(int cx, int cy);

    int x, y, width, height;

    %addmethods {
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

        wxRect __add__(const wxRect* rect) {
            if (! rect) return *self;
            return *self + *rect;
        }

        int __cmp__(const wxRect* rect) {
            if (! rect) return 1;
            if (*self == *rect) return 0;
            return -1;
        }
    }

    %pragma(python) addtoclass = "
    def __str__(self):                   return str(self.asTuple())
    def __repr__(self):                  return str(self.asTuple())
    def __len__(self):                   return len(self.asTuple())
    def __getitem__(self, index):        return self.asTuple()[index]
    def __setitem__(self, index, val):
        if index == 0: self.x = val
        elif index == 1: self.y = val
        elif index == 2: self.width = val
        elif index == 3: self.height = val
        else: raise IndexError

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
            obj = wxPyConstructObject((void*)newRect, "wxRect");
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
// Miscellaneous functions

long wxNewId();
void wxRegisterId(long id);
%name(NewId) long wxNewId();
%name(RegisterId) void wxRegisterId(long id);
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

%inline %{
    wxString wxGetResource(const wxString& section, const wxString& entry,
                           const wxString& file = wxPyEmptyString) {
        wxChar * retval;
        wxGetResource(section, entry, &retval, file);
        return retval;
    }
%}

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



