/////////////////////////////////////////////////////////////////////////////
// Name:        sizers.i
// Purpose:     SWIG definitions of the wxSizer family of classes
//
// Author:      Robin Dunn
//
// Created:     18-Sept-1999
// RCS-ID:      $Id$
// Copyright:   (c) 1999 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module sizers

%{
#include "helpers.h"
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import misc.i
%import windows.i
%import controls.i

%pragma(python) code = "import wx"
%pragma(python) code = "import string"

//---------------------------------------------------------------------------


class wxSizerItem {
public:
    // No need to ever create one directly in Python...

    //wxSizerItem( int width, int height, int option, int flag, int border, wxObject* userData);
    //wxSizerItem( wxWindow *window, int option, int flag, int border, wxObject* userData );
    //wxSizerItem( wxSizer *sizer, int option, int flag, int border, wxObject* userData );

    wxSize GetSize();
    wxSize CalcMin();
    void SetDimension( wxPoint pos, wxSize size );
    %name(SetRatioWH) void SetRatio( int width, int height );
    %name(SetRatioSize) void SetRatio( wxSize size );
    void SetRatio( float ratio );
    float GetRatio();

    bool IsWindow();
    bool IsSizer();
    bool IsSpacer();

    wxWindow *GetWindow();
    void SetWindow( wxWindow *window );
    wxSizer *GetSizer();
    void SetSizer( wxSizer *sizer );
    int GetOption();
    int GetFlag();
    int GetBorder();

    void SetInitSize( int x, int y );
    void SetOption( int option );
    void SetFlag( int flag );
    void SetBorder( int border );

    // wxObject* GetUserData();
    %addmethods {
        // Assume that the user data is a wxPyUserData object and return the contents
        PyObject* GetUserData() {
            wxPyUserData* data = (wxPyUserData*)self->GetUserData();
            if (data) {
                Py_INCREF(data->m_obj);
                return data->m_obj;
            } else {
                Py_INCREF(Py_None);
                return Py_None;
            }
        }
    }
};


//---------------------------------------------------------------------------

class wxSizer {
public:
    // wxSizer();      ****  abstract, can't instantiate
    // ~wxSizer();

    %addmethods {
        void Destroy() { delete self; }

        void AddWindow(wxWindow *window, int option=0, int flag=0, int border=0,
                       PyObject* userData=NULL) {
            wxPyUserData* data = NULL;
            if (userData) data = new wxPyUserData(userData);
            self->Add(window, option, flag, border, data);
        }
        void AddSizer(wxSizer *sizer, int option=0, int flag=0, int border=0,
                      PyObject* userData=NULL) {
            wxPyUserData* data = NULL;
            if (userData) data = new wxPyUserData(userData);
            self->Add(sizer, option, flag, border, data);
        }
        void AddSpacer(int width, int height, int option=0, int flag=0,
                       int border=0, PyObject* userData=NULL) {
            wxPyUserData* data = NULL;
            if (userData) data = new wxPyUserData(userData);
            self->Add(width, height, option, flag, border, data);
        }


        void InsertWindow(int before, wxWindow *window, int option=0, int flag=0,
                          int border=0, PyObject* userData=NULL) {
            wxPyUserData* data = NULL;
            if (userData) data = new wxPyUserData(userData);
            self->Insert(before, window, option, flag, border, data);
        }
        void InsertSizer(int before, wxSizer *sizer, int option=0, int flag=0,
                         int border=0, PyObject* userData=NULL) {
            wxPyUserData* data = NULL;
            if (userData) data = new wxPyUserData(userData);
            self->Insert(before, sizer, option, flag, border, data);
        }
        void InsertSpacer(int before, int width, int height, int option=0, int flag=0,
                          int border=0, PyObject* userData=NULL) {
            wxPyUserData* data = NULL;
            if (userData) data = new wxPyUserData(userData);
            self->Insert(before, width, height, option, flag, border, data);
        }


        void PrependWindow(wxWindow *window, int option=0, int flag=0, int border=0,
                           PyObject* userData=NULL) {
            wxPyUserData* data = NULL;
            if (userData) data = new wxPyUserData(userData);
            self->Prepend(window, option, flag, border, data);
        }
        void PrependSizer(wxSizer *sizer, int option=0, int flag=0, int border=0,
                          PyObject* userData=NULL) {
            wxPyUserData* data = NULL;
            if (userData) data = new wxPyUserData(userData);
            self->Prepend(sizer, option, flag, border, data);
        }
        void PrependSpacer(int width, int height, int option=0, int flag=0,
                           int border=0, PyObject* userData=NULL) {
            wxPyUserData* data = NULL;
            if (userData) data = new wxPyUserData(userData);
            self->Prepend(width, height, option, flag, border, data);
        }
    }

    %name(RemoveWindow)bool Remove( wxWindow *window );
    %name(RemoveSizer)bool Remove( wxSizer *sizer );
    %name(RemovePos)bool Remove( int pos );


    %pragma(python) addtoclass = "
    def Add(self, *args):
        if type(args[0]) == type(1):
            apply(self.AddSpacer, args)
        elif string.find(args[0].this, 'Sizer') != -1:
            apply(self.AddSizer, args)
        else:
            apply(self.AddWindow, args)

    def Insert(self, *args):
        if type(args[0]) == type(1):
            apply(self.InsertSpacer, args)
        elif string.find(args[0].this, 'Sizer') != -1:
            apply(self.InsertSizer, args)
        else:
            apply(self.InsertWindow, args)

    def Prepend(self, *args):
        if type(args[0]) == type(1):
            apply(self.PrependSpacer, args)
        elif string.find(args[0].this, 'Sizer') != -1:
            apply(self.PrependSizer, args)
        else:
            apply(self.PrependWindow, args)

    def Remove(self, *args):
        if type(args[0]) == type(1):
            apply(self.RemovePos, args)
        elif string.find(args[0].this, 'Sizer') != -1:
            apply(self.RemoveSizer, args)
        else:
            apply(self.RemoveWindow, args)

    def AddMany(self, widgets):
        for childinfo in widgets:
            if type(childinfo) != type(()):
                childinfo = (childinfo, )
            apply(self.Add, childinfo)
"


    void SetDimension( int x, int y, int width, int height );
    void SetMinSize(wxSize size);

    %name(SetItemMinSizeWindow) void SetItemMinSize(wxWindow* window, int width, int height);
    %name(SetItemMinSizeSizer) void SetItemMinSize(wxSizer* sizer, int width, int height);
    %name(SetItemMinSizePos) void SetItemMinSize(int pos, int width, int height);

    %pragma(python) addtoclass = "
    def SetItemMinSize(self, *args):
        if type(args[0]) == type(1):
            apply(self.SetItemMinSizePos, args)
        elif string.find(args[0].this, 'Sizer') != -1:
            apply(self.SetItemMinSizeSizer, args)
        else:
            apply(self.SetItemMinSizeWindow, args)
     "

    wxSize GetSize();
    wxPoint GetPosition();
    wxSize GetMinSize();

    // void RecalcSizes() = 0;
    // wxSize CalcMin() = 0;

    void Layout();

    void Fit( wxWindow *window );
    void SetSizeHints( wxWindow *window );

    // wxList& GetChildren();
    %addmethods {
        PyObject* GetChildren() {
            wxList& list = self->GetChildren();
            return wxPy_ConvertList(&list, "wxSizerItem");
        }
    }
};


//---------------------------------------------------------------------------
// Use this one for deriving Python classes from
%{
class wxPySizer : public wxSizer {
    DECLARE_DYNAMIC_CLASS(wxPySizer);
public:
    wxPySizer() : wxSizer() {};

    DEC_PYCALLBACK___pure(RecalcSizes);
    DEC_PYCALLBACK_wxSize__pure(CalcMin);
    PYPRIVATE;
};


IMP_PYCALLBACK___pure(wxPySizer, wxSizer, RecalcSizes);
IMP_PYCALLBACK_wxSize__pure(wxPySizer, wxSizer, CalcMin);

IMPLEMENT_DYNAMIC_CLASS(wxPySizer, wxSizer);
%}



class wxPySizer : public wxSizer {
public:
    wxPySizer();
    void _setSelf(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setSelf(self, wxPySizer)"
};


//---------------------------------------------------------------------------

class  wxBoxSizer : public wxSizer {
public:
    wxBoxSizer(int orient = wxHORIZONTAL);
    int GetOrientation();
    void RecalcSizes();
    wxSize CalcMin();
};

//---------------------------------------------------------------------------

class  wxStaticBoxSizer : public wxBoxSizer {
public:
    wxStaticBoxSizer(wxStaticBox *box, int orient = wxHORIZONTAL);
    wxStaticBox *GetStaticBox();
    void RecalcSizes();
    wxSize CalcMin();
};

//---------------------------------------------------------------------------

class wxNotebookSizer: public wxSizer {
public:
    wxNotebookSizer( wxNotebook *nb );

    void RecalcSizes();
    wxSize CalcMin();

    wxNotebook *GetNotebook();
};

//---------------------------------------------------------------------------

class wxGridSizer: public wxSizer
{
public:
    wxGridSizer( int rows=1, int cols=0, int vgap=0, int hgap=0 );

    void RecalcSizes();
    wxSize CalcMin();

    void SetCols( int cols );
    void SetRows( int rows );
    void SetVGap( int gap );
    void SetHGap( int gap );
    int GetCols();
    int GetRows();
    int GetVGap();
    int GetHGap();
};

//---------------------------------------------------------------------------

class wxFlexGridSizer: public wxGridSizer
{
public:
    wxFlexGridSizer( int rows=1, int cols=0, int vgap=0, int hgap=0 );

    void RecalcSizes();
    wxSize CalcMin();

    void AddGrowableRow( size_t idx );
    void RemoveGrowableRow( size_t idx );
    void AddGrowableCol( size_t idx );
    void RemoveGrowableCol( size_t idx );

};

//---------------------------------------------------------------------------
