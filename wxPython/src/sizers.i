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

    //wxSizerItem( int width, int height, int proportion, int flag, int border, wxObject* userData);
    //wxSizerItem( wxWindow *window, int proportion, int flag, int border, wxObject* userData );
    //wxSizerItem( wxSizer *sizer, int proportion, int flag, int border, wxObject* userData );

    void DeleteWindows();
    void DetachSizer();

    wxSize GetSize();
    wxSize CalcMin();
    void SetDimension( wxPoint pos, wxSize size );

    wxSize GetMinSize();
    void SetInitSize( int x, int y );

    %name(SetRatioWH) void SetRatio( int width, int height );
    %name(SetRatioSize) void SetRatio( wxSize size );
    void SetRatio( float ratio );
    float GetRatio();

    bool IsWindow();
    bool IsSizer();
    bool IsSpacer();

    void SetProportion( int proportion );
    int GetProportion();
    %pragma(python) addtoclass = "SetOption = SetProportion"
    %pragma(python) addtoclass = "GetOption = GetProportion"
    void SetFlag( int flag );
    int GetFlag();
    void SetBorder( int border );
    int GetBorder();

    wxWindow *GetWindow();
    void SetWindow( wxWindow *window );
    wxSizer *GetSizer();
    void SetSizer( wxSizer *sizer );
    const wxSize& GetSpacer();
    void SetSpacer( const wxSize &size );

    void Show( bool show );
    bool IsShown();

    wxPoint GetPosition();

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


        void _Add(PyObject* item, int proportion=0, int flag=0, int border=0,
                  PyObject* userData=NULL, int option=-1) {
            // The option parameter is only for backwards compatibility
            // with keyword args, all new code should use "proportion"
            // instead.  This can be removed eventually.
            if (option != -1) proportion = option;

            wxWindow* window;
            wxSizer*  sizer;
            wxSize    size;
            wxSize*   sizePtr = &size;
            wxPyUserData* data = NULL;
            if (userData) data = new wxPyUserData(userData);

            // Find out what type the item is and call the real Add method
            if (! SWIG_GetPtrObj(item, (void**)&window, "_wxWindow_p"))
                self->Add(window, proportion, flag, border, data);

            else if (!SWIG_GetPtrObj(item, (void**)&sizer, "_wxSizer_p"))
                self->Add(sizer, proportion, flag, border, data);

            else if (wxSize_helper(item, &sizePtr))
                self->Add(sizePtr->GetWidth(), sizePtr->GetHeight(),
                          proportion, flag, border, data);
            else {
                if (data) delete data;
                PyErr_SetString(PyExc_TypeError,
                                "wxWindow, wxSizer, wxSize, or (w,h) expected for item");
            }
        }


        void _Insert(int before, PyObject* item, int proportion=0, int flag=0,
                     int border=0, PyObject* userData=NULL, int option=-1) {
            // The option parameter is only for backwards compatibility
            // with keyword args, all new code should use "proportion"
            // instead.  This can be removed eventually.
            if (option != -1) proportion = option;

            wxWindow* window;
            wxSizer*  sizer;
            wxSize    size;
            wxSize*   sizePtr = &size;
            wxPyUserData* data = NULL;
            if (userData) data = new wxPyUserData(userData);

            // Find out what type the item is and call the real Insert method
            if (! SWIG_GetPtrObj(item, (void**)&window, "_wxWindow_p"))
                self->Insert(before, window, proportion, flag, border, data);

            else if (!SWIG_GetPtrObj(item, (void**)&sizer, "_wxSizer_p"))
                self->Insert(before, sizer, proportion, flag, border, data);

            else if (wxSize_helper(item, &sizePtr))
                self->Insert(before, sizePtr->GetWidth(), sizePtr->GetHeight(),
                          proportion, flag, border, data);
            else {
                if (data) delete data;
                PyErr_SetString(PyExc_TypeError,
                                "wxWindow, wxSizer, wxSize, or (w,h) expected for item");
            }
        }



        void _Prepend(PyObject* item, int proportion=0, int flag=0, int border=0,
                           PyObject* userData=NULL, int option=-1) {
            // The option parameter is only for backwards compatibility
            // with keyword args, all new code should use "proportion"
            // instead.  This can be removed eventually.
            if (option != -1) proportion = option;

            wxWindow* window;
            wxSizer*  sizer;
            wxSize    size;
            wxSize*   sizePtr = &size;
            wxPyUserData* data = NULL;
            if (userData) data = new wxPyUserData(userData);

            // Find out what type the item is and call the real Prepend method
            if (! SWIG_GetPtrObj(item, (void**)&window, "_wxWindow_p"))
                self->Prepend(window, proportion, flag, border, data);

            else if (!SWIG_GetPtrObj(item, (void**)&sizer, "_wxSizer_p"))
                self->Prepend(sizer, proportion, flag, border, data);

            else if (wxSize_helper(item, &sizePtr))
                self->Prepend(sizePtr->GetWidth(), sizePtr->GetHeight(),
                              proportion, flag, border, data);
            else {
                if (data) delete data;
                PyErr_SetString(PyExc_TypeError,
                                "wxWindow, wxSizer, wxSize, or (w,h) expected for item");
            }
        }

        bool Remove(PyObject* item) {
            wxWindow* window;
            wxSizer*  sizer;

            // Find out what type the item is and call the real Remove method
            if (! SWIG_GetPtrObj(item, (void**)&window, "_wxWindow_p"))
                return self->Remove(window);

            else if (!SWIG_GetPtrObj(item, (void**)&sizer, "_wxSizer_p"))
                return self->Remove(sizer);

            else if (PyInt_Check(item)) {
                int pos = PyInt_AsLong(item);
                return self->Remove(pos);
            }
            else {
                PyErr_SetString(PyExc_TypeError,
                                "wxWindow, wxSizer or int (position) expected.");
                return FALSE;
            }
        }

        void _SetItemMinSize(PyObject* item, wxSize size) {
            wxWindow* window;
            wxSizer*  sizer;

            // Find out what type the item is and call the real Remove method
            if (! SWIG_GetPtrObj(item, (void**)&window, "_wxWindow_p"))
                self->SetItemMinSize(window, size);

            else if (!SWIG_GetPtrObj(item, (void**)&sizer, "_wxSizer_p"))
                self->SetItemMinSize(sizer, size);

            else if (PyInt_Check(item)) {
                int pos = PyInt_AsLong(item);
                self->SetItemMinSize(pos, size);
            }
            else
                PyErr_SetString(PyExc_TypeError,
                                "wxWindow, wxSizer or int (position) expected.");
        }

    }


    %pragma(python) addtoclass = "
    def Add(self, item, *args, **kw):
        if type(item) == type(1):
            item = (item, args[0])  # backwards compatibility, args are width, height
            args = args[1:]
        self._Add(item, *args, **kw)

    def AddMany(self, widgets):
        for childinfo in widgets:
            if type(childinfo) != type(()):
                childinfo = (childinfo, )
            self.Add(*childinfo)

    def Prepend(self, item, *args, **kw):
        if type(item) == type(1):
            item = (item, args[0])  # backwards compatibility, args are width, height
            args = args[1:]
        self._Prepend(item, *args, **kw)

    def Insert(self, before, item, *args, **kw):
        if type(item) == type(1):
            item = (item, args[0])  # backwards compatibility, args are width, height
            args = args[1:]
        self._Insert(before, item, *args, **kw)


    # for backwards compatibility only
    AddWindow = AddSizer = AddSpacer = Add
    PrependWindow = PrependSizer = PrependSpacer = Prepend
    InsertWindow = InsertSizer = InsertSpacer = Insert
    RemoveWindow = RemoveSizer = RemovePos = Remove


    def SetItemMinSize(self, item, *args):
        if len(args) == 2:
            return self._SetItemMinSize(item, args)
        else:
            return self._SetItemMinSize(item, args[0])

"


    void SetDimension( int x, int y, int width, int height );
    void SetMinSize(wxSize size);

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

    %addmethods {
        void Show(PyObject* item, bool show = TRUE) {
            wxWindow* window;
            wxSizer*  sizer;
            // Find out what type the item is and call the real method
            if (! SWIG_GetPtrObj(item, (void**)&window, "_wxWindow_p"))
                self->Show(window, show);
            else if (!SWIG_GetPtrObj(item, (void**)&sizer, "_wxSizer_p"))
                self->Show(sizer, show);
            else
                PyErr_SetString(PyExc_TypeError, "wxWindow or wxSizer expected.");
        }

        void Hide(PyObject* item) {
            wxWindow* window;
            wxSizer*  sizer;
            // Find out what type the item is and call the real method
            if (! SWIG_GetPtrObj(item, (void**)&window, "_wxWindow_p"))
                self->Hide(window);
            else if (!SWIG_GetPtrObj(item, (void**)&sizer, "_wxSizer_p"))
                self->Hide(sizer);
            else
                PyErr_SetString(PyExc_TypeError, "wxWindow or wxSizer expected.");
        }

        bool IsShown(PyObject* item) {
            wxWindow* window;
            wxSizer*  sizer;
            // Find out what type the item is and call the real method
            if (! SWIG_GetPtrObj(item, (void**)&window, "_wxWindow_p"))
                return self->IsShown(window);
            else if (!SWIG_GetPtrObj(item, (void**)&sizer, "_wxSizer_p"))
                return self->IsShown(sizer);
            else {
                PyErr_SetString(PyExc_TypeError, "wxWindow or wxSizer expected.");
                return FALSE;
            }
        }
    }


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

enum wxFlexSizerGrowMode
{
    // don't resize the cells in non-flexible direction at all
    wxFLEX_GROWMODE_NONE,

    // uniformly resize only the specified ones (default)
    wxFLEX_GROWMODE_SPECIFIED,

    // uniformly resize all cells
    wxFLEX_GROWMODE_ALL
};


class wxFlexGridSizer: public wxGridSizer
{
public:
    wxFlexGridSizer( int rows=1, int cols=0, int vgap=0, int hgap=0 );
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

    void RecalcSizes();
    wxSize CalcMin();

    void AddGrowableRow( size_t idx, int proportion = 0  );
    void RemoveGrowableRow( size_t idx );
    void AddGrowableCol( size_t idx, int proportion = 0  );
    void RemoveGrowableCol( size_t idx );

    // the sizer cells may grow in both directions, not grow at all or only
    // grow in one direction but not the other

    // the direction may be wxVERTICAL, wxHORIZONTAL or wxBOTH (default)
    void SetFlexibleDirection(int direction);
    int GetFlexibleDirection();

    // note that the grow mode only applies to the direction which is not
    // flexible
    void SetNonFlexibleGrowMode(wxFlexSizerGrowMode mode);
    wxFlexSizerGrowMode GetNonFlexibleGrowMode();
};

//---------------------------------------------------------------------------
