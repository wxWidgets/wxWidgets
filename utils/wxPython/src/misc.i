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
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, PyInt_FromLong(self->x));
            PyTuple_SET_ITEM(tup, 1, PyInt_FromLong(self->y));
            return tup;
        }
    }
    %pragma(python) addtoclass = "def __str__(self): return str(self.asTuple())"
    %pragma(python) addtoclass = "def __repr__(self): return str(self.asTuple())"

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
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, PyFloat_FromDouble(self->x));
            PyTuple_SET_ITEM(tup, 1, PyFloat_FromDouble(self->y));
            return tup;
        }
    }
    %pragma(python) addtoclass = "def __str__(self): return str(self.asTuple())"
    %pragma(python) addtoclass = "def __repr__(self): return str(self.asTuple())"
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
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, PyInt_FromLong(self->x));
            PyTuple_SET_ITEM(tup, 1, PyInt_FromLong(self->y));
            return tup;
        }
    }
    %pragma(python) addtoclass = "def __str__(self): return str(self.asTuple())"
    %pragma(python) addtoclass = "def __repr__(self): return str(self.asTuple())"
};

//---------------------------------------------------------------------------

class wxRect {
public:
   wxRect(long x=0, long y=0, long w=0, long h=0);
   // TODO: do this one too... wxRect(const wxPoint& pos, const wxSize& size);
   ~wxRect();

   long  GetX();
   void SetX(long X);
   long  GetY();
   void SetY(long Y);
   long  GetWidth();
   void SetWidth(long w);
   long  GetHeight();
   void SetHeight(long h);


   wxPoint GetPosition();
   wxSize GetSize();

   long  GetLeft();
   long  GetTop();
   long  GetBottom();
   long  GetRight();

   long x, y, width, height;

    %addmethods {
        PyObject* asTuple() {
            PyObject* tup = PyTuple_New(4);
            PyTuple_SET_ITEM(tup, 0, PyInt_FromLong(self->x));
            PyTuple_SET_ITEM(tup, 1, PyInt_FromLong(self->y));
            PyTuple_SET_ITEM(tup, 2, PyInt_FromLong(self->width));
            PyTuple_SET_ITEM(tup, 3, PyInt_FromLong(self->height));
            return tup;
        }
    }
    %pragma(python) addtoclass = "def __str__(self): return str(self.asTuple())"
    %pragma(python) addtoclass = "def __repr__(self): return str(self.asTuple())"
};




//---------------------------------------------------------------------------
// Miscellaneous functions

long wxNewId();
void wxRegisterId(long id);
%name(NewId) long wxNewId();
%name(RegisterId) void wxRegisterId(long id);

void wxBell();
void wxDisplaySize(int *OUTPUT, int *OUTPUT);
void wxEndBusyCursor();
long wxExecute(const wxString& command, bool sync = FALSE);
long wxGetElapsedTime(bool resetTimer = TRUE);
#ifdef __WXMSW__
long wxGetFreeMemory();
#endif
void wxGetMousePosition(int* OUTPUT, int* OUTPUT);
bool wxIsBusy();
wxString wxNow();
bool wxShell(const wxString& command = wxPyEmptyStr);
void wxStartTimer();
int wxGetOsVersion(int *OUTPUT, int *OUTPUT);

void wxSleep(int secs);
bool wxYield();
bool wxSafeYield();
void wxEnableTopLevelWindows(bool enable);

%inline %{
    char* wxGetResource(char *section, char *entry, char *file = NULL) {
        char * retval;
        wxGetResource(section, entry, &retval, file);
        return retval;
    }
%}


//----------------------------------------------------------------------

class wxPyTimer {
public:
    wxPyTimer(PyObject* notify);
    ~wxPyTimer();
    int Interval();
    void Start(int milliseconds=-1, int oneShot=FALSE);
    void Stop();
};

//---------------------------------------------------------------------------

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


class wxIndividualLayoutConstraint {
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


class wxLayoutConstraints {
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
// Regions, etc.

enum wxRegionContain {
	wxOutRegion, wxPartRegion, wxInRegion
};


class wxRegion {
public:
    wxRegion();
    ~wxRegion();

    void Clear();
    wxRegionContain Contains(long x, long y);
    %name(ContainsPoint)wxRegionContain Contains(const wxPoint& pt);
    %name(ContainsRect)wxRegionContain Contains(const wxRect& rect);

    wxRect GetBox();
    bool Intersect(const wxRect& rect);
    bool IsEmpty();
    bool Subtract(const wxRect& rect);
    bool Union(const wxRect& rect);
    bool Xor(const wxRect& rect);
};



class wxRegionIterator {
public:
    wxRegionIterator(const wxRegion& region);
    ~wxRegionIterator();

    long GetX();
    long GetY();
    long GetW();
    long GetWidth();
    long GetH();
    long GetHeight();
    wxRect GetRect();
    bool HaveRects();
    void Reset();

    %addmethods {
        void Next() {
            (*self) ++;
        }
    };
};



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


class wxAcceleratorTable {
public:
    // Can also accept a list of 3-tuples
    wxAcceleratorTable(int LCOUNT, wxAcceleratorEntry* LIST);
    ~wxAcceleratorTable();

};

//---------------------------------------------------------------------------

class wxBusyInfo {
public:
    wxBusyInfo(const wxString& message);
    ~wxBusyInfo();
};



//---------------------------------------------------------------------------


