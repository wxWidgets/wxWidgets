%module misc
%{
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


#include "helpers.h"
#include <wx/resource.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i

//---------------------------------------------------------------------------



class wxSize {
public:
    %name(width) long x;
    %name(height)long y;

    wxSize(long w=0, long h=0);
    ~wxSize();
    void Set(long w, long h);
    %name(GetWidth) long GetX();
    %name(GetHeight)long GetY();

    %addmethods {
        PyObject* __str__() {
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, PyInt_FromLong(self->x));
            PyTuple_SET_ITEM(tup, 1, PyInt_FromLong(self->y));
            return tup;
        }
    }
};

//---------------------------------------------------------------------------

class wxRealPoint {
public:
    double x;
    double y;
    wxRealPoint(double x=0.0, double y=0.0);
    ~wxRealPoint();
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
        PyObject* __str__() {
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, PyInt_FromLong(self->x));
            PyTuple_SET_ITEM(tup, 1, PyInt_FromLong(self->y));
            return tup;
        }
    }
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
};



//---------------------------------------------------------------------------
// Dialog Functions

char* wxFileSelector(char* message,
                        char* default_path = NULL,
                        char* default_filename = NULL,
                        char* default_extension = NULL,
                        char* wildcard = "*.*",
                        int flags = 0,
                        wxWindow *parent = NULL,
                        int x = -1, int y = -1);

wxString wxGetTextFromUser(const wxString& message,
                           const wxString& caption = wxPyEmptyStr,
                           const wxString& default_value = wxPyEmptyStr,
                           wxWindow *parent = NULL,
                           int x = -1, int y = -1,
                           bool centre = TRUE);

// TODO: Need to custom wrap this one...
// int wxGetMultipleChoice(char* message, char* caption,
//                         int LCOUNT, char** LIST,
//                         int nsel, int *selection,
//                         wxWindow *parent = NULL, int x = -1, int y = -1,
//                         bool centre = TRUE, int width=150, int height=200);


wxString wxGetSingleChoice(const wxString& message, const wxString& caption,
                           int LCOUNT, wxString* LIST,
                           wxWindow *parent = NULL,
                           int x = -1, int y = -1,
                           bool centre = TRUE,
                           int width=150, int height=200);

int wxGetSingleChoiceIndex(const wxString& message, const wxString& caption,
                           int LCOUNT, wxString* LIST,
                           wxWindow *parent = NULL,
                           int x = -1, int y = -1,
                           bool centre = TRUE,
                           int width=150, int height=200);


int wxMessageBox(const wxString& message,
                 const wxString& caption = wxPyEmptyStr,
                 int style = wxOK | wxCENTRE,
                 wxWindow *parent = NULL,
                 int x = -1, int y = -1);

//---------------------------------------------------------------------------
// GDI Functions

bool wxColourDisplay();
int wxDisplayDepth();
void wxSetCursor(wxCursor *cursor);

//---------------------------------------------------------------------------
// Miscellaneous functions

long NewId();
void RegisterId(long id);
void wxBeginBusyCursor(wxCursor *cursor = wxHOURGLASS_CURSOR);
void wxBell();
void wxDisplaySize(int *OUTPUT, int *OUTPUT);
void wxEndBusyCursor();
long wxExecute(const wxString& command, bool sync = FALSE);
wxWindow * wxFindWindowByLabel(const wxString& label, wxWindow *parent=NULL);
wxWindow * wxFindWindowByName(const wxString& name, wxWindow *parent=NULL);
wxWindow * wxGetActiveWindow();
long wxGetElapsedTime(bool resetTimer = TRUE);
long wxGetFreeMemory();
void wxGetMousePosition(int* OUTPUT, int* OUTPUT);
bool wxIsBusy();
wxString wxNow();
bool wxShell(const wxString& command = wxPyEmptyStr);
void wxStartTimer();
bool wxYield();

int wxGetOsVersion(int *OUTPUT, int *OUTPUT);
%inline %{
    char* wxGetResource(char *section, char *entry, char *file = NULL) {
        char * retval;
        wxGetResource(section, entry, &retval, file);
        return retval;
    }
%}

//---------------------------------------------------------------------------
// Resource System

bool wxResourceAddIdentifier(char *name, int value);
void wxResourceClear(void);
wxBitmap * wxResourceCreateBitmap(char *resource);
wxIcon * wxResourceCreateIcon(char *resource);
wxMenuBar * wxResourceCreateMenuBar(char *resource);
int wxResourceGetIdentifier(char *name);
bool wxResourceParseData(char *resource, wxResourceTable *table = NULL);
bool wxResourceParseFile(char *filename, wxResourceTable *table = NULL);
bool wxResourceParseString(char *resource, wxResourceTable *table = NULL);



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
    void AsIs(void);
    void Below(wxWindow *otherWin, int margin=0);
    void Unconstrained(void);
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
// Accelerator Entry and Table

class wxAcceleratorEntry {
public:
    wxAcceleratorEntry(int flags = 0, int keyCode = 0, int cmd = 0);
    //~wxAcceleratorEntry();  *** ?

    void Set(int flags, int keyCode, int Cmd);
    int GetFlags();
    int GetKeyCode();
    int GetCommand();
};


class wxAcceleratorTable {
public:
    // Can also accept a list of 3-tuples
    wxAcceleratorTable(int LCOUNT, wxAcceleratorEntry* LIST);
    // ~wxAcceleratorEntry(); *** ?

};

//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
//
// $Log$
// Revision 1.1  1998/08/09 08:25:51  RD
// Initial version
//
//


