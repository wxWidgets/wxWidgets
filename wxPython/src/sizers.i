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

        void AddWindow(wxWindow *window, int proportion=0, int flag=0, int border=0,
                       PyObject* userData=NULL) {
            wxPyUserData* data = NULL;
            if (userData) data = new wxPyUserData(userData);
            self->Add(window, proportion, flag, border, data);
        }
        void AddSizer(wxSizer *sizer, int proportion=0, int flag=0, int border=0,
                      PyObject* userData=NULL) {
            wxPyUserData* data = NULL;
            if (userData) data = new wxPyUserData(userData);
            self->Add(sizer, proportion, flag, border, data);
        }
        void AddSpacer(int width, int height, int proportion=0, int flag=0,
                       int border=0, PyObject* userData=NULL) {
            wxPyUserData* data = NULL;
            if (userData) data = new wxPyUserData(userData);
            self->Add(width, height, proportion, flag, border, data);
        }

        void InsertWindow(int before, wxWindow *window, int proportion=0, int flag=0,
                          int border=0, PyObject* userData=NULL) {
            wxPyUserData* data = NULL;
            if (userData) data = new wxPyUserData(userData);
            self->Insert(before, window, proportion, flag, border, data);
        }
        void InsertSizer(int before, wxSizer *sizer, int proportion=0, int flag=0,
                         int border=0, PyObject* userData=NULL) {
            wxPyUserData* data = NULL;
            if (userData) data = new wxPyUserData(userData);
            self->Insert(before, sizer, proportion, flag, border, data);
        }
        void InsertSpacer(int before, int width, int height, int proportion=0, int flag=0,
                          int border=0, PyObject* userData=NULL) {
            wxPyUserData* data = NULL;
            if (userData) data = new wxPyUserData(userData);
            self->Insert(before, width, height, proportion, flag, border, data);
        }


        void PrependWindow(wxWindow *window, int proportion=0, int flag=0, int border=0,
                           PyObject* userData=NULL) {
            wxPyUserData* data = NULL;
            if (userData) data = new wxPyUserData(userData);
            self->Prepend(window, proportion, flag, border, data);
        }
        void PrependSizer(wxSizer *sizer, int proportion=0, int flag=0, int border=0,
                          PyObject* userData=NULL) {
            wxPyUserData* data = NULL;
            if (userData) data = new wxPyUserData(userData);
            self->Prepend(sizer, proportion, flag, border, data);
        }
        void PrependSpacer(int width, int height, int proportion=0, int flag=0,
                           int border=0, PyObject* userData=NULL) {
            wxPyUserData* data = NULL;
            if (userData) data = new wxPyUserData(userData);
            self->Prepend(width, height, proportion, flag, border, data);
        }

        // TODO:  AddItem, InsertItem, PrependItem

    }

    %name(RemoveWindow)bool Remove( wxWindow *window );  // TODO: This is DEPRECATED.  Should all be removed?
    %name(RemoveSizer)bool Remove( wxSizer *sizer );
    %name(RemovePos)bool Remove( int pos );

    %name(DetachWindow)bool Detach( wxWindow *window );
    %name(DetachSizer)bool Detach( wxSizer *sizer );
    %name(DetachPos)bool Detach( int pos );


    %pragma(python) addtoclass = "
    def Add(self, *args, **kw):
        if type(args[0]) == type(1):
            apply(self.AddSpacer, args, kw)
        elif isinstance(args[0], wxSizerPtr):
            apply(self.AddSizer, args, kw)
        elif isinstance(args[0], wxWindowPtr):
            apply(self.AddWindow, args, kw)
        else:
            raise TypeError, 'Expected int, wxSizer or wxWindow parameter'

    def Insert(self, *args, **kw):
        if type(args[1]) == type(1):
            apply(self.InsertSpacer, args, kw)
        elif isinstance(args[1], wxSizerPtr):
            apply(self.InsertSizer, args, kw)
        elif isinstance(args[1], wxWindowPtr):
            apply(self.InsertWindow, args, kw)
        else:
            raise TypeError, 'Expected int, wxSizer or wxWindow parameter'

    def Prepend(self, *args, **kw):
        if type(args[0]) == type(1):
            apply(self.PrependSpacer, args, kw)
        elif isinstance(args[0], wxSizerPtr):
            apply(self.PrependSizer, args, kw)
        elif isinstance(args[0], wxWindowPtr):
            apply(self.PrependWindow, args, kw)
        else:
            raise TypeError, 'Expected int, wxSizer or wxWindow parameter'

    def Remove(self, *args, **kw):
        if type(args[0]) == type(1):
            return apply(self.RemovePos, args, kw)
        elif isinstance(args[0], wxSizerPtr):
            return apply(self.RemoveSizer, args, kw)
        elif isinstance(args[0], wxWindowPtr):
            return apply(self.RemoveWindow, args, kw)
        else:
            raise TypeError, 'Expected int, wxSizer or wxWindow parameter'

    def Detach(self, *args, **kw):
        if type(args[0]) == type(1):
            return apply(self.DetachPos, args, kw)
        elif isinstance(args[0], wxSizerPtr):
            return apply(self.DetachSizer, args, kw)
        elif isinstance(args[0], wxWindowPtr):
            return apply(self.DetachWindow, args, kw)
        else:
            raise TypeError, 'Expected int, wxSizer or wxWindow parameter'

    def AddMany(self, widgets):
        for childinfo in widgets:
            if type(childinfo) != type(()):
                childinfo = (childinfo, )
            apply(self.Add, childinfo)
"

    void Clear( bool delete_windows = false );
    void DeleteWindows();

    void SetMinSize(wxSize size);

    %name(SetItemMinSizeWindow) void SetItemMinSize(wxWindow* window, wxSize size);
    %name(SetItemMinSizeSizer) void SetItemMinSize(wxSizer* sizer, wxSize size);
    %name(SetItemMinSizePos) void SetItemMinSize(int pos, wxSize size);
    %name(SetItemMinSizeWindowWH) void SetItemMinSize(wxWindow* window, int width, int height);
    %name(SetItemMinSizeSizerWH) void SetItemMinSize(wxSizer* sizer, int width, int height);
    %name(SetItemMinSizePosWH) void SetItemMinSize(int pos, int width, int height);

    %pragma(python) addtoclass = "
    def SetItemMinSize(self, *args):
        if type(args[0]) == type(1):
            apply(self.SetItemMinSizePos, args)
        elif isinstance(args[0], wxSizerPtr):
            apply(self.SetItemMinSizeSizer, args)
        elif isinstance(args[0], wxWindowPtr):
            apply(self.SetItemMinSizeWindow, args)
        else:
            raise TypeError, 'Expected int, wxSizer or wxWindow parameter'

    def SetItemMinSizeWH(self, *args):
        if type(args[0]) == type(1):
            apply(self.SetItemMinSizePosWH, args)
        elif isinstance(args[0], wxSizerPtr):
            apply(self.SetItemMinSizeSizerWH, args)
        elif isinstance(args[0], wxWindowPtr):
            apply(self.SetItemMinSizeWindowWH, args)
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


    // wxList& GetChildren();
    %addmethods {
        PyObject* GetChildren() {
            wxSizerItemList& list = self->GetChildren();
            return wxPy_ConvertList(&list, "wxSizerItem");
        }
    }

    void SetDimension( int x, int y, int width, int height );

    // Manage whether individual windows or sub-sizers are considered
    // in the layout calculations or not.
    %name(ShowWindow)void Show( wxWindow *window, bool show = TRUE );
    %name(ShowSizer)void Show( wxSizer *sizer, bool show = TRUE );
    %name(ShowPos)void Show( size_t index, bool show = TRUE );
    %name(HideWindow)void Hide( wxWindow *window );
    %name(HideSizer)void Hide( wxSizer *sizer );
    %name(HidePos)void Hide( size_t index );
    %name(IsShownWindow)bool IsShown( wxWindow *window );
    %name(IsShownSizer)bool IsShown( wxSizer *sizer );
    %name(IsShownPos)bool IsShown( size_t index );

    %pragma(python) addtoclass = "
    def Show(self, *args):
        if type(args[0]) == type(1):
            apply(self.ShowPos, args)
        elif isinstance(args[0], wxSizerPtr):
            apply(self.ShowSizer, args)
        elif isinstance(args[0], wxWindowPtr):
            apply(self.ShowWindow, args)
        else:
            raise TypeError, 'Expected int, wxSizer or wxWindow parameter'

    def Hide(self, *args):
        if type(args[0]) == type(1):
            apply(self.HidePos, args)
        elif isinstance(args[0], wxSizerPtr):
            apply(self.HideSizer, args)
        elif isinstance(args[0], wxWindowPtr):
            apply(self.HideWindow, args)
        else:
            raise TypeError, 'Expected int, wxSizer or wxWindow parameter'

    def IsShown(self, *args):
        if type(args[0]) == type(1):
            return apply(self.IsShownPos, args)
        elif isinstance(args[0], wxSizerPtr):
            return apply(self.IsShownSizer, args)
        elif isinstance(args[0], wxWindowPtr):
            return apply(self.IsShownWindow, args)
        else:
            raise TypeError, 'Expected int, wxSizer or wxWindow parameter'
"

    // Recursively call wxWindow::Show () on all sizer items.
    void ShowItems (bool show);

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
