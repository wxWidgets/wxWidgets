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

    bool IsWindow();
    bool IsSizer();
    bool IsSpacer();

    wxWindow *GetWindow();
    wxSizer *GetSizer();
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
    void _setSelf(PyObject* self);
    %pragma(python) addtomethod = "__init__:self._setSelf(self)"
};


//---------------------------------------------------------------------------

class  wxBoxSizer : public wxSizer {
public:
    wxBoxSizer(int orient = wxHORIZONTAL);
    int GetOrientation();
};

//---------------------------------------------------------------------------

class  wxStaticBoxSizer : public wxBoxSizer {
public:
    wxStaticBoxSizer(wxStaticBox *box, int orient = wxHORIZONTAL);
    wxStaticBox *GetStaticBox();
};

//---------------------------------------------------------------------------
