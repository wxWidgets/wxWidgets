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

#include <wx/notebook.h>
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

//---------------------------------------------------------------------------


class wxSizerItem : public wxObject {
public:
    // No need to ever create one directly in Python...

    //wxSizerItem( int width, int height, int option, int flag, int border, wxObject* userData);
    //wxSizerItem( wxWindow *window, int option, int flag, int border, wxObject* userData );
    //wxSizerItem( wxSizer *sizer, int option, int flag, int border, wxObject* userData );

    void DeleteWindows();

    wxPoint GetPosition();
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
    bool IsShown();

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
    void Show(bool show);

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

class wxSizer : public wxObject {
public:
    // wxSizer();      ****  abstract, can't instantiate
    // ~wxSizer();

    %addmethods {
        void _setOORInfo(PyObject* _self) {
            self->SetClientObject(new wxPyOORClientData(_self));
        }
    }

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
    def Add(self, *args, **kw):
        if type(args[0]) == type(1):
            self.AddSpacer(*args, **kw)
        elif isinstance(args[0], wxSizerPtr):
            self.AddSizer(*args, **kw)
        elif isinstance(args[0], wxWindowPtr):
            self.AddWindow(*args, **kw)
        else:
            raise TypeError, 'Expected int, wxSizer or wxWindow parameter'

    def Insert(self, *args, **kw):
        if type(args[1]) == type(1):
            self.InsertSpacer(*args, **kw)
        elif isinstance(args[1], wxSizerPtr):
            self.InsertSizer(*args, **kw)
        elif isinstance(args[1], wxWindowPtr):
            self.InsertWindow(*args, **kw)
        else:
            raise TypeError, 'Expected int, wxSizer or wxWindow parameter'

    def Prepend(self, *args, **kw):
        if type(args[0]) == type(1):
            self.PrependSpacer(*args, **kw)
        elif isinstance(args[0], wxSizerPtr):
            self.PrependSizer(*args, **kw)
        elif isinstance(args[0], wxWindowPtr):
            self.PrependWindow(*args, **kw)
        else:
            raise TypeError, 'Expected int, wxSizer or wxWindow parameter'

    def Remove(self, *args, **kw):
        if type(args[0]) == type(1):
            return self.RemovePos(*args, **kw)
        elif isinstance(args[0], wxSizerPtr):
            return self.RemoveSizer(*args, **kw)
        elif isinstance(args[0], wxWindowPtr):
            return self.RemoveWindow(*args, **kw)
        else:
            raise TypeError, 'Expected int, wxSizer or wxWindow parameter'

    def AddMany(self, widgets):
        for childinfo in widgets:
            if type(childinfo) != type(()):
                childinfo = (childinfo, )
            self.Add(*childinfo)
"


    void SetDimension( int x, int y, int width, int height );
    void SetMinSize(wxSize size);

    %name(SetItemMinSizeWindow) void SetItemMinSize(wxWindow* window, int width, int height);
    %name(SetItemMinSizeSizer) void SetItemMinSize(wxSizer* sizer, int width, int height);
    %name(SetItemMinSizePos) void SetItemMinSize(int pos, int width, int height);

    %pragma(python) addtoclass = "
    def SetItemMinSize(self, *args):
        if type(args[0]) == type(1):
            self.SetItemMinSizePos(*args)
        elif isinstance(args[0], wxSizerPtr):
            self.SetItemMinSizeSizer(*args)
        elif isinstance(args[0], wxWindowPtr):
            self.SetItemMinSizeWindow(*args)
        else:
            raise TypeError, 'Expected int, wxSizer or wxWindow parameter'
     "

    wxSize GetSize();
    wxPoint GetPosition();
    wxSize GetMinSize();

    %pragma(python) addtoclass = "
    def GetSizeTuple(self):
        return self.GetSize().asTuple()
    def GetPositionTuple(self):
        return self.GetPosition().asTuple()
    def GetMinSizeTuple(self):
        return self.GetMinSize().asTuple()
    "

    // void RecalcSizes() = 0;
    // wxSize CalcMin() = 0;

    void Layout();

    wxSize Fit( wxWindow *window );
    void FitInside( wxWindow *window );

    void SetSizeHints( wxWindow *window );
    void SetVirtualSizeHints( wxWindow *window );

    void Clear( bool delete_windows=FALSE );
    void DeleteWindows();


    // wxList& GetChildren();
    %addmethods {
        PyObject* GetChildren() {
            wxList& list = self->GetChildren();
            return wxPy_ConvertList(&list, "wxSizerItem");
        }
    }


    // Manage whether individual windows or sub-sizers are considered
    // in the layout calculations or not.
    %name(ShowWindow)void Show( wxWindow *window, bool show = TRUE );
    %name(HideWindow)void Hide( wxWindow *window );
    %name(ShowSizer)void Show( wxSizer *sizer, bool show = TRUE );
    %name(HideSizer)void Hide( wxSizer *sizer );
    %name(IsShownWindow)bool IsShown( wxWindow *window );
    %name(IsShownSizer)bool IsShown( wxSizer *sizer );

    %pragma(python) addtoclass = "
    def Show(self, *args):
        if isinstance(args[0], wxSizerPtr):
            self.ShowSizer(*args)
        elif isinstance(args[0], wxWindowPtr):
            self.ShowWindow(*args)
        else:
            raise TypeError, 'Expected wxSizer or wxWindow parameter'

    def Hide(self, *args):
        if isinstance(args[0], wxSizerPtr):
            self.HideSizer(*args)
        elif isinstance(args[0], wxWindowPtr):
            self.HideWindow(*args)
        else:
            raise TypeError, 'Expected wxSizer or wxWindow parameter'

    def IsShown(self, *args):
        if isinstance(args[0], wxSizerPtr):
            return self.IsShownSizer(*args)
        elif isinstance(args[0], wxWindowPtr):
            return self.IsShownWindow(*args)
        else:
            raise TypeError, 'Expected wxSizer or wxWindow parameter'
"

    // Recursively call wxWindow::Show() on all sizer items.
    void ShowItems(bool show);

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
    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxPySizer)"
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
};


//---------------------------------------------------------------------------

class  wxBoxSizer : public wxSizer {
public:
    wxBoxSizer(int orient = wxHORIZONTAL);
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    int GetOrientation();
    void SetOrientation(int orient);
    void RecalcSizes();
    wxSize CalcMin();
};

//---------------------------------------------------------------------------

class  wxStaticBoxSizer : public wxBoxSizer {
public:
    wxStaticBoxSizer(wxStaticBox *box, int orient = wxHORIZONTAL);
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    wxStaticBox *GetStaticBox();
    void RecalcSizes();
    wxSize CalcMin();
};

//---------------------------------------------------------------------------

class wxNotebookSizer: public wxSizer {
public:
    wxNotebookSizer( wxNotebook *nb );
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    void RecalcSizes();
    wxSize CalcMin();
    wxNotebook *GetNotebook();
};

//---------------------------------------------------------------------------

class wxGridSizer: public wxSizer
{
public:
    wxGridSizer( int rows=1, int cols=0, int vgap=0, int hgap=0 );
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

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
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

    void RecalcSizes();
    wxSize CalcMin();

    void AddGrowableRow( size_t idx );
    void RemoveGrowableRow( size_t idx );
    void AddGrowableCol( size_t idx );
    void RemoveGrowableCol( size_t idx );

};

//---------------------------------------------------------------------------
