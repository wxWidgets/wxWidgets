////////////////////////////////////////////////////////////////////////////
// Name:        grid.i
// Purpose:     SWIG definitions for the new wxGrid and related classes
//
// Author:      Robin Dunn
//
// Created:     17-March-2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%define DOCSTRING
"Classes for implementing a spreadsheet-like control."
%enddef

%module(package="wx", docstring=DOCSTRING) grid


%{
#include "wx/wxPython/wxPython.h"
#include "wx/wxPython/pyclasses.h"
#include "wx/wxPython/printfw.h"

#include <wx/grid.h>
#include <wx/generic/gridctrl.h>

%}


//---------------------------------------------------------------------------

%import windows.i
%pythoncode { wx = _core }
%pythoncode { __docfilter__ = wx.__DocFilter(globals()) }


MAKE_CONST_WXSTRING_NOSWIG(EmptyString);
MAKE_CONST_WXSTRING_NOSWIG(GridNameStr);
MAKE_CONST_WXSTRING_NOSWIG(DefaultDateTimeFormat);

//---------------------------------------------------------------------------
// OOR related typemaps and helper functions

%typemap(out) wxGridCellRenderer*     { $result = wxPyMake_wxGridCellRenderer($1, (bool)$owner); }
%typemap(out) wxGridCellEditor*       { $result = wxPyMake_wxGridCellEditor($1,   (bool)$owner); }
%typemap(out) wxGridCellAttr*         { $result = wxPyMake_wxGridCellAttr($1,     (bool)$owner); }
%typemap(out) wxGridCellAttrProvider* { $result = wxPyMake_wxGridCellAttrProvider($1, (bool)$owner); }
%typemap(out) wxGridTableBase*        { $result = wxPyMake_wxGridTableBase($1,    (bool)$owner); }


%{

#define wxPyMake_TEMPLATE(TYPE) \
PyObject* wxPyMake_##TYPE(TYPE* source, bool setThisOwn) { \
    PyObject* target = NULL; \
    if (source) { \
        /* Check if there is already a pointer to a Python object in the \
           OOR data that we can use. */ \
        wxPyOORClientData* data = (wxPyOORClientData*)source->GetClientObject(); \
        if (data) { \
            target = data->m_obj; \
            if (target) \
                Py_INCREF(target); \
        } \
        /* Otherwise make a new wrapper for it the old fashioned way and \
           give it the OOR treatment */ \
        if (! target) { \
            target = wxPyConstructObject(source, wxT(#TYPE), setThisOwn); \
            if (target) \
                source->SetClientObject(new wxPyOORClientData(target)); \
        } \
    } else {  /* source was NULL so return None. */ \
        Py_INCREF(Py_None); target = Py_None; \
    } \
    return target; \
} \


wxPyMake_TEMPLATE(wxGridCellRenderer)
wxPyMake_TEMPLATE(wxGridCellEditor)
wxPyMake_TEMPLATE(wxGridCellAttr)
wxPyMake_TEMPLATE(wxGridCellAttrProvider)
wxPyMake_TEMPLATE(wxGridTableBase)

%}

//---------------------------------------------------------------------------
// Macros, similar to what's in helpers.h, to aid in the creation of
// virtual methods that are able to make callbacks to Python.  Many of these
// are specific to wxGrid and so are kept here to reduce the mess in helpers.h
// a bit.


%{
#define PYCALLBACK_GCA_INTINTKIND(PCLASS, CBNAME)                               \
    wxGridCellAttr* CBNAME(int a, int b, wxGridCellAttr::wxAttrKind c) {        \
        wxGridCellAttr* rval = NULL;                                            \
        bool found;                                                             \
        wxPyBlock_t blocked = wxPyBeginBlockThreads();                          \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* ro;                                                       \
            wxGridCellAttr* ptr;                                                \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(iii)", a, b, c)); \
            if (ro) {                                                           \
                if (wxPyConvertSwigPtr(ro, (void **)&ptr, wxT("wxGridCellAttr"))) \
                    rval = ptr;                                                 \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            rval = PCLASS::CBNAME(a, b, c);                                     \
        return rval;                                                            \
    }          


#define PYCALLBACK__GCAINTINT(PCLASS, CBNAME)                                   \
    void CBNAME(wxGridCellAttr *attr, int a, int b) {                           \
        wxPyBlock_t blocked = wxPyBeginBlockThreads();                          \
        bool found;                                                             \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyMake_wxGridCellAttr(attr,false);                \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Oii)", obj, a, b));  \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            PCLASS::CBNAME(attr, a, b);                                         \
    }             



#define PYCALLBACK__GCAINT(PCLASS, CBNAME)                                      \
    void CBNAME(wxGridCellAttr *attr, int val) {                                \
        wxPyBlock_t blocked = wxPyBeginBlockThreads();                          \
        bool found;                                                             \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* obj = wxPyMake_wxGridCellAttr(attr,false);                \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(Oi)", obj, val));    \
            Py_DECREF(obj);                                                     \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            PCLASS::CBNAME(attr, val);                                          \
    }      



#define PYCALLBACK_INT__pure(CBNAME)                                            \
    int  CBNAME() {                                                             \
        wxPyBlock_t blocked = wxPyBeginBlockThreads();                          \
        int rval = 0;                                                           \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                            \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("()"));         \
        wxPyEndBlockThreads(blocked);                                           \
        return rval;                                                            \
    }



#define PYCALLBACK_BOOL_INTINT_pure(CBNAME)                                     \
    bool CBNAME(int a, int b) {                                                 \
        wxPyBlock_t blocked = wxPyBeginBlockThreads();                          \
        bool rval = 0;                                                          \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME))                            \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(ii)",a,b));   \
        wxPyEndBlockThreads(blocked);                                           \
        return rval;                                                            \
    }


#define PYCALLBACK_STRING_INTINT_pure(CBNAME)                                   \
    wxString CBNAME(int a, int b) {                                             \
        wxPyBlock_t blocked = wxPyBeginBlockThreads();                          \
        wxString rval;                                                          \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(ii)",a,b));  \
            if (ro) {                                                           \
                rval = Py2wxString(ro);                                         \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        return rval;                                                            \
    }


#define PYCALLBACK__INTINTSTRING_pure(CBNAME)                                   \
    void CBNAME(int a, int b, const wxString& c) {                              \
        wxPyBlock_t blocked = wxPyBeginBlockThreads();                          \
        if (wxPyCBH_findCallback(m_myInst, #CBNAME)) {                          \
            PyObject* s = wx2PyString(c);                                       \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iiO)",a,b,s));\
            Py_DECREF(s);                                                       \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
    }


#define PYCALLBACK_STRING_INTINT(PCLASS, CBNAME)                                \
    wxString CBNAME(int a, int b) {                                             \
        bool found;                                                             \
        wxPyBlock_t blocked = wxPyBeginBlockThreads();                          \
        wxString rval;                                                          \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(ii)",a,b));  \
            if (ro) {                                                           \
                rval = Py2wxString(ro);                                         \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            rval = PCLASS::CBNAME(a, b);                                        \
        return rval;                                                            \
    }               


#define PYCALLBACK_BOOL_INTINTSTRING(PCLASS, CBNAME)                            \
    bool CBNAME(int a, int b, const wxString& c)  {                             \
        bool rval = 0;                                                          \
        bool found;                                                             \
        wxPyBlock_t blocked = wxPyBeginBlockThreads();                          \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* s = wx2PyString(c);                                       \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iiO)",a,b,s));\
            Py_DECREF(s);                                                       \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            rval = PCLASS::CBNAME(a,b,c);                                       \
        return rval;                                                            \
    }              




#define PYCALLBACK_LONG_INTINT(PCLASS, CBNAME)                                  \
    long CBNAME(int a, int b)  {                                                \
        long rval;                                                              \
        bool found;                                                             \
        wxPyBlock_t blocked = wxPyBeginBlockThreads();                          \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(ii)", a,b));  \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            rval = PCLASS::CBNAME(a,b);                                         \
        return rval;                                                            \
    }               


#define PYCALLBACK_BOOL_INTINT(PCLASS, CBNAME)                                  \
    bool CBNAME(int a, int b)  {                                                \
        bool rval = 0;                                                          \
        bool found;                                                             \
        wxPyBlock_t blocked = wxPyBeginBlockThreads();                          \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(ii)", a,b));  \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            rval = PCLASS::CBNAME(a,b);                                         \
        return rval;                                                            \
    }              



#define PYCALLBACK_DOUBLE_INTINT(PCLASS, CBNAME)                                \
    double CBNAME(int a, int b) {                                               \
        bool found;                                                             \
        wxPyBlock_t blocked = wxPyBeginBlockThreads();                          \
        double rval;                                                            \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(ii)",a,b));  \
            if (ro) {                                                           \
                PyObject* str = PyObject_Str(ro);                               \
                rval = PyFloat_AsDouble(str);                                   \
                Py_DECREF(ro);   Py_DECREF(str);                                \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            rval = PCLASS::CBNAME(a, b);                                        \
        return rval;                                                            \
    }              



#define PYCALLBACK__(PCLASS, CBNAME)                                            \
    void CBNAME()  {                                                            \
        bool found;                                                             \
        wxPyBlock_t blocked = wxPyBeginBlockThreads();                          \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("()"));                \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            PCLASS::CBNAME();                                                   \
    }               



#define PYCALLBACK_BOOL_SIZETSIZET(PCLASS, CBNAME)                              \
    bool CBNAME(size_t a, size_t b)  {                                          \
        bool rval = 0;                                                          \
        bool found;                                                             \
        wxPyBlock_t blocked = wxPyBeginBlockThreads();                          \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(ii)", a,b));  \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            rval = PCLASS::CBNAME(a,b);                                         \
        return rval;                                                            \
    }          



#define PYCALLBACK_BOOL_SIZET(PCLASS, CBNAME)                                   \
    bool CBNAME(size_t a)  {                                                    \
        bool rval = 0;                                                          \
        bool found;                                                             \
        wxPyBlock_t blocked = wxPyBeginBlockThreads();                          \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(i)", a));     \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            rval = PCLASS::CBNAME(a);                                           \
        return rval;                                                            \
    }           


#define PYCALLBACK_STRING_INT(PCLASS, CBNAME)                                   \
    wxString CBNAME(int a) {                                                    \
        bool found;                                                             \
        wxPyBlock_t blocked = wxPyBeginBlockThreads();                          \
        wxString rval;                                                          \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* ro;                                                       \
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(i)",a));     \
            if (ro) {                                                           \
                rval = Py2wxString(ro);                                         \
                Py_DECREF(ro);                                                  \
            }                                                                   \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            rval = PCLASS::CBNAME(a);                                           \
        return rval;                                                            \
    }          


#define PYCALLBACK__INTSTRING(PCLASS, CBNAME)                                   \
    void CBNAME(int a, const wxString& c)  {                                    \
        bool found;                                                             \
        wxPyBlock_t blocked = wxPyBeginBlockThreads();                          \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME))) {                \
            PyObject* s = wx2PyString(c);                                       \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iO)",a,s));          \
            Py_DECREF(s);                                                       \
        }                                                                       \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            PCLASS::CBNAME(a,c);                                                \
    }           




#define PYCALLBACK_BOOL_(PCLASS, CBNAME)                                        \
    bool CBNAME()  {                                                            \
        bool rval = 0;                                                          \
        bool found;                                                             \
        wxPyBlock_t blocked = wxPyBeginBlockThreads();                          \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("()"));         \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            rval = PCLASS::CBNAME();                                            \
        return rval;                                                            \
    }         



#define PYCALLBACK__SIZETINT(PCLASS, CBNAME)                                    \
    void CBNAME(size_t a, int b)  {                                             \
        bool found;                                                             \
        wxPyBlock_t blocked = wxPyBeginBlockThreads();                          \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(ii)", a,b));         \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            PCLASS::CBNAME(a,b);                                                \
    }        




#define PYCALLBACK__INTINTLONG(PCLASS, CBNAME)                                  \
    void CBNAME(int a, int b, long c)  {                                        \
        bool found;                                                             \
        wxPyBlock_t blocked = wxPyBeginBlockThreads();                          \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iii)", a,b,c));      \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            PCLASS::CBNAME(a,b,c);                                              \
    }          




#define PYCALLBACK__INTINTDOUBLE(PCLASS, CBNAME)                                \
    void CBNAME(int a, int b, double c)  {                                      \
        bool found;                                                             \
        wxPyBlock_t blocked = wxPyBeginBlockThreads();                          \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iif)", a,b,c));      \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            PCLASS::CBNAME(a,b,c);                                              \
    }       



#define PYCALLBACK__INTINTBOOL(PCLASS, CBNAME)                                  \
    void CBNAME(int a, int b, bool c)  {                                        \
        bool found;                                                             \
        wxPyBlock_t blocked = wxPyBeginBlockThreads();                          \
        if ((found = wxPyCBH_findCallback(m_myInst, #CBNAME)))                  \
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iii)", a,b,c));      \
        wxPyEndBlockThreads(blocked);                                           \
        if (! found)                                                            \
            PCLASS::CBNAME(a,b,c);                                              \
    }         



%}

//---------------------------------------------------------------------------

class wxGridCellCoords;
class wxGridCellAttr;

#define wxGRID_VALUE_STRING     "string"
#define wxGRID_VALUE_BOOL       "bool"
#define wxGRID_VALUE_NUMBER     "long"
#define wxGRID_VALUE_FLOAT      "double"
#define wxGRID_VALUE_CHOICE     "choice"
#define wxGRID_VALUE_TEXT       "string"
#define wxGRID_VALUE_LONG       "long"
#define wxGRID_VALUE_CHOICEINT  "choiceint"
#define wxGRID_VALUE_DATETIME   "datetime"


%immutable;
const wxGridCellCoords wxGridNoCellCoords;
const wxRect           wxGridNoCellRect;
%mutable;


%{
#define wxGRID_DEFAULT_NUMBER_ROWS        WXGRID_DEFAULT_NUMBER_ROWS
#define wxGRID_DEFAULT_NUMBER_COLS        WXGRID_DEFAULT_NUMBER_COLS
#define wxGRID_DEFAULT_ROW_HEIGHT         WXGRID_DEFAULT_ROW_HEIGHT
#define wxGRID_DEFAULT_COL_WIDTH          WXGRID_DEFAULT_COL_WIDTH
#define wxGRID_DEFAULT_COL_LABEL_HEIGHT   WXGRID_DEFAULT_COL_LABEL_HEIGHT
#define wxGRID_DEFAULT_ROW_LABEL_WIDTH    WXGRID_DEFAULT_ROW_LABEL_WIDTH
#define wxGRID_LABEL_EDGE_ZONE            WXGRID_LABEL_EDGE_ZONE
#define wxGRID_MIN_ROW_HEIGHT             WXGRID_MIN_ROW_HEIGHT
#define wxGRID_MIN_COL_WIDTH              WXGRID_MIN_COL_WIDTH
#define wxGRID_DEFAULT_SCROLLBAR_WIDTH    WXGRID_DEFAULT_SCROLLBAR_WIDTH
%}

enum {
    wxGRID_DEFAULT_NUMBER_ROWS,
    wxGRID_DEFAULT_NUMBER_COLS,
    wxGRID_DEFAULT_ROW_HEIGHT,
    wxGRID_DEFAULT_COL_WIDTH,
    wxGRID_DEFAULT_COL_LABEL_HEIGHT,
    wxGRID_DEFAULT_ROW_LABEL_WIDTH,
    wxGRID_LABEL_EDGE_ZONE,
    wxGRID_MIN_ROW_HEIGHT,
    wxGRID_MIN_COL_WIDTH,
    wxGRID_DEFAULT_SCROLLBAR_WIDTH
};


//---------------------------------------------------------------------------

// TODO: Use these to have SWIG automatically handle the IncRef/DecRef calls:
// 
//        %ref   wxGridCellWorker "$this->IncRef();";
//        %unref wxGridCellWorker "$this->DecRef();";
//

class  wxGridCellWorker
{
public:
    %extend {
        void _setOORInfo(PyObject* _self) {
            if (!self->GetClientObject())
                self->SetClientObject(new wxPyOORClientData(_self));
        }

        // A dummy dtor to shut up SWIG.  (The real one is protected and can
        // only be called by DecRef)
        ~wxGridCellWorker() {
        }
    }

    void SetParameters(const wxString& params);
    void IncRef();
    void DecRef();
};



// wxGridCellRenderer is an ABC, and several derived classes are available.
// Classes implemented in Python should be derived from wxPyGridCellRenderer.

class wxGridCellRenderer : public wxGridCellWorker
{
public:
    virtual void Draw(wxGrid& grid,
                      wxGridCellAttr& attr,
                      wxDC& dc,
                      const wxRect& rect,
                      int row, int col,
                      bool isSelected);
    
    virtual wxSize GetBestSize(wxGrid& grid,
                               wxGridCellAttr& attr,
                               wxDC& dc,
                               int row, int col);
    
    virtual wxGridCellRenderer *Clone() const;
};



// The C++ version of wxPyGridCellRenderer
%{
class wxPyGridCellRenderer : public wxGridCellRenderer
{
public:
    wxPyGridCellRenderer() : wxGridCellRenderer() {};

    // Implement Python callback aware virtual methods
    void Draw(wxGrid& grid, wxGridCellAttr& attr,
              wxDC& dc, const wxRect& rect,
              int row, int col, bool isSelected) {
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if (wxPyCBH_findCallback(m_myInst, "Draw")) {
            PyObject* go = wxPyMake_wxObject(&grid,false);
            PyObject* dco = wxPyMake_wxObject(&dc,false);
            PyObject* ao = wxPyMake_wxGridCellAttr(&attr,false);
            PyObject* ro = wxPyConstructObject((void*)&rect, wxT("wxRect"), 0);

            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(OOOOiii)", go, ao, dco, ro,
                                                         row, col, isSelected));
            Py_DECREF(go);
            Py_DECREF(ao);
            Py_DECREF(dco);
            Py_DECREF(ro);
        }
        wxPyEndBlockThreads(blocked);
    }

    wxSize GetBestSize(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc,
                       int row, int col) {
        wxSize rval;
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if (wxPyCBH_findCallback(m_myInst, "GetBestSize")) {
            PyObject* ro;
            wxSize*   ptr;
            PyObject* go = wxPyMake_wxObject(&grid,false);
            PyObject* dco = wxPyMake_wxObject(&dc,false);
            PyObject* ao = wxPyMake_wxGridCellAttr(&attr,false);

            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(OOOii)",
                                                                 go, ao, dco,
                                                                 row, col));
            Py_DECREF(go);
            Py_DECREF(ao);
            Py_DECREF(dco);

            if (ro) {
                const char* errmsg = "GetBestSize should return a 2-tuple of integers or a wxSize object.";
                if (wxPyConvertSwigPtr(ro, (void **)&ptr, wxT("wxSize"))) {
                    rval = *ptr;
                }
                else if (PySequence_Check(ro) && PyObject_Length(ro) == 2) {
                    PyObject* o1 = PySequence_GetItem(ro, 0);
                    PyObject* o2 = PySequence_GetItem(ro, 1);
                    if (PyNumber_Check(o1) && PyNumber_Check(o2))
                        rval = wxSize(PyInt_AsLong(o1), PyInt_AsLong(o2));
                    else
                        PyErr_SetString(PyExc_TypeError, errmsg);
                    Py_DECREF(o1);
                    Py_DECREF(o2);
                }
                else {
                    PyErr_SetString(PyExc_TypeError, errmsg);
                }
                Py_DECREF(ro);
            }
        }
        wxPyEndBlockThreads(blocked);
        return rval;
    }


    wxGridCellRenderer *Clone() const {
        wxGridCellRenderer* rval = NULL;
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if (wxPyCBH_findCallback(m_myInst, "Clone")) {
            PyObject* ro;
            wxGridCellRenderer* ptr;
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));
            if (ro) {
                if (wxPyConvertSwigPtr(ro, (void **)&ptr, wxT("wxGridCellRenderer")))
                    rval = ptr;
                Py_DECREF(ro);
            }
        }
        wxPyEndBlockThreads(blocked);
        return rval;
    }

    DEC_PYCALLBACK__STRING(SetParameters);

    PYPRIVATE;
};

IMP_PYCALLBACK__STRING( wxPyGridCellRenderer, wxGridCellRenderer, SetParameters);

%}


// Let SWIG know about it so it can create the Python version
class wxPyGridCellRenderer : public wxGridCellRenderer {
public:
    %pythonAppend wxPyGridCellRenderer  "self._setCallbackInfo(self, PyGridCellRenderer);self._setOORInfo(self)"

    wxPyGridCellRenderer();
    void _setCallbackInfo(PyObject* self, PyObject* _class);

    void SetParameters(const wxString& params);
    %MAKE_BASE_FUNC(PyGridCellRenderer, SetParameters);
};

//---------------------------------------------------------------------------
//  Predefined Renderers

class wxGridCellStringRenderer : public wxGridCellRenderer
{
public:
    %pythonAppend wxGridCellStringRenderer "self._setOORInfo(self)"
    wxGridCellStringRenderer();
};


class  wxGridCellNumberRenderer : public wxGridCellStringRenderer
{
public:
    %pythonAppend wxGridCellNumberRenderer "self._setOORInfo(self)"
    wxGridCellNumberRenderer();
};


class  wxGridCellFloatRenderer : public wxGridCellStringRenderer
{
public:
    %pythonAppend wxGridCellFloatRenderer "self._setOORInfo(self)"
    wxGridCellFloatRenderer(int width = -1, int precision = -1);

    int GetWidth() const;
    void SetWidth(int width);
    int GetPrecision() const;
    void SetPrecision(int precision);

    %property(Precision, GetPrecision, SetPrecision, doc="See `GetPrecision` and `SetPrecision`");
    %property(Width, GetWidth, SetWidth, doc="See `GetWidth` and `SetWidth`");
};


class  wxGridCellBoolRenderer : public wxGridCellRenderer
{
public:
    %pythonAppend wxGridCellBoolRenderer "self._setOORInfo(self)"
    wxGridCellBoolRenderer();
};


class wxGridCellDateTimeRenderer : public wxGridCellStringRenderer
{
public:
    %pythonAppend wxGridCellDateTimeRenderer "self._setOORInfo(self)"
    wxGridCellDateTimeRenderer(wxString outformat = wxPyDefaultDateTimeFormat,
                               wxString informat =  wxPyDefaultDateTimeFormat);
};


class wxGridCellEnumRenderer : public wxGridCellStringRenderer
{
public:
    %pythonAppend wxGridCellEnumRenderer "self._setOORInfo(self)"
    wxGridCellEnumRenderer( const wxString& choices = wxPyEmptyString );
};


class wxGridCellAutoWrapStringRenderer : public wxGridCellStringRenderer
{
public:
    %pythonAppend wxGridCellAutoWrapStringRenderer "self._setOORInfo(self)"
    wxGridCellAutoWrapStringRenderer();
};


//---------------------------------------------------------------------------
// wxGridCellEditor is an ABC, and several derived classes are available.
// Classes implemented in Python should be derived from wxPyGridCellEditor.

class wxGridCellEditor : public wxGridCellWorker
{
public:
    bool IsCreated();
    wxControl* GetControl();
    void SetControl(wxControl* control);

    wxGridCellAttr* GetCellAttr();
    void SetCellAttr(wxGridCellAttr* attr);

    virtual void Create(wxWindow* parent,
                        wxWindowID id,
                        wxEvtHandler* evtHandler);
    virtual void BeginEdit(int row, int col, wxGrid* grid);
    virtual bool EndEdit(int row, int col, wxGrid* grid);
    virtual void Reset();
    virtual wxGridCellEditor *Clone() const;

    virtual void SetSize(const wxRect& rect);
    virtual void Show(bool show, wxGridCellAttr *attr = NULL);
    virtual void PaintBackground(const wxRect& rectCell, wxGridCellAttr *attr);
    virtual bool IsAcceptedKey(wxKeyEvent& event);
    virtual void StartingKey(wxKeyEvent& event);
    virtual void StartingClick();
    virtual void HandleReturn(wxKeyEvent& event);

    %pythonPrepend Destroy "args[0].this.own(False)"
    virtual void Destroy();

    %property(CellAttr, GetCellAttr, SetCellAttr, doc="See `GetCellAttr` and `SetCellAttr`");
    %property(Control, GetControl, SetControl, doc="See `GetControl` and `SetControl`");
};


// The C++ version of wxPyGridCellEditor
%{
class wxPyGridCellEditor : public wxGridCellEditor
{
public:
    wxPyGridCellEditor() : wxGridCellEditor() {}

    void Create(wxWindow* parent, wxWindowID id, wxEvtHandler* evtHandler) {
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if (wxPyCBH_findCallback(m_myInst, "Create")) {
            PyObject* po = wxPyMake_wxObject(parent,false);
            PyObject* eo = wxPyMake_wxObject(evtHandler,false);

            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(OiO)", po, id, eo));
            Py_DECREF(po);
            Py_DECREF(eo);
        }
        wxPyEndBlockThreads(blocked);
    }


    void BeginEdit(int row, int col, wxGrid* grid) {
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if (wxPyCBH_findCallback(m_myInst, "BeginEdit")) {
            PyObject* go = wxPyMake_wxObject(grid,false);
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iiO)", row, col, go));
            Py_DECREF(go);
        }
        wxPyEndBlockThreads(blocked);
    }


    bool EndEdit(int row, int col, wxGrid* grid) {
        bool rv = false;
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if (wxPyCBH_findCallback(m_myInst, "EndEdit")) {
            PyObject* go = wxPyMake_wxObject(grid,false);
            rv = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iiO)", row, col, go));
            Py_DECREF(go);
        }
        wxPyEndBlockThreads(blocked);
        return rv;
    }


    wxGridCellEditor* Clone() const {
        wxGridCellEditor* rval = NULL;
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if (wxPyCBH_findCallback(m_myInst, "Clone")) {
            PyObject* ro;
            wxGridCellEditor* ptr;
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("()"));
            if (ro) {
                if (wxPyConvertSwigPtr(ro, (void **)&ptr, wxT("wxGridCellEditor")))
                    rval = ptr;
                Py_DECREF(ro);
            }
        }
        wxPyEndBlockThreads(blocked);
        return rval;
    }


    void Show(bool show, wxGridCellAttr *attr) {
        bool found;
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "Show"))) {
            PyObject* ao = wxPyMake_wxGridCellAttr(attr,false);
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iO)", show, ao));
            Py_DECREF(ao);
        }
        wxPyEndBlockThreads(blocked);
        if (! found)
            wxGridCellEditor::Show(show, attr);
    }


    void PaintBackground(const wxRect& rectCell, wxGridCellAttr *attr) {
        bool found;
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "PaintBackground)"))) {
            PyObject* ao = wxPyMake_wxGridCellAttr(attr,false);
            PyObject* ro = wxPyConstructObject((void*)&rectCell, wxT("wxRect"), 0);

            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(OO)", ro, ao));

            Py_DECREF(ro);
            Py_DECREF(ao);
        }
        wxPyEndBlockThreads(blocked);
        if (! found)
            wxGridCellEditor::PaintBackground(rectCell, attr);
    }


    DEC_PYCALLBACK___pure(Reset);
    DEC_PYCALLBACK__constany(SetSize, wxRect);
    DEC_PYCALLBACK_bool_any(IsAcceptedKey, wxKeyEvent);
    DEC_PYCALLBACK__any(StartingKey, wxKeyEvent);
    DEC_PYCALLBACK__any(HandleReturn, wxKeyEvent);
    DEC_PYCALLBACK__(StartingClick);
    DEC_PYCALLBACK__(Destroy);
    DEC_PYCALLBACK__STRING(SetParameters);
    DEC_PYCALLBACK_STRING__constpure(GetValue);

    PYPRIVATE;
};


IMP_PYCALLBACK__STRING( wxPyGridCellEditor, wxGridCellEditor, SetParameters);
IMP_PYCALLBACK___pure(wxPyGridCellEditor, wxGridCellEditor, Reset);
IMP_PYCALLBACK__constany(wxPyGridCellEditor, wxGridCellEditor, SetSize, wxRect);
IMP_PYCALLBACK_bool_any(wxPyGridCellEditor, wxGridCellEditor, IsAcceptedKey, wxKeyEvent);
IMP_PYCALLBACK__any(wxPyGridCellEditor, wxGridCellEditor, StartingKey, wxKeyEvent);
IMP_PYCALLBACK__any(wxPyGridCellEditor, wxGridCellEditor, HandleReturn, wxKeyEvent);
IMP_PYCALLBACK__(wxPyGridCellEditor, wxGridCellEditor, StartingClick);
IMP_PYCALLBACK__(wxPyGridCellEditor, wxGridCellEditor, Destroy);
IMP_PYCALLBACK_STRING__constpure(wxPyGridCellEditor, wxGridCellEditor, GetValue);

%}


// Let SWIG know about it so it can create the Python version
class wxPyGridCellEditor : public wxGridCellEditor {
public:
    %pythonAppend wxPyGridCellEditor  "self._setCallbackInfo(self, PyGridCellEditor);self._setOORInfo(self)"

    wxPyGridCellEditor();
    void _setCallbackInfo(PyObject* self, PyObject* _class);

    void SetSize(const wxRect& rect);
    void Show(bool show, wxGridCellAttr *attr = NULL);
    void PaintBackground(const wxRect& rectCell, wxGridCellAttr *attr);
    bool IsAcceptedKey(wxKeyEvent& event);
    void StartingKey(wxKeyEvent& event);
    void StartingClick();
    void HandleReturn(wxKeyEvent& event);
    void Destroy();
    void SetParameters(const wxString& params);

    %MAKE_BASE_FUNC(PyGridCellEditor, SetSize);
    %MAKE_BASE_FUNC(PyGridCellEditor, Show);
    %MAKE_BASE_FUNC(PyGridCellEditor, PaintBackground);
    %MAKE_BASE_FUNC(PyGridCellEditor, IsAcceptedKey);
    %MAKE_BASE_FUNC(PyGridCellEditor, StartingKey);
    %MAKE_BASE_FUNC(PyGridCellEditor, StartingClick);
    %MAKE_BASE_FUNC(PyGridCellEditor, HandleReturn);
    %MAKE_BASE_FUNC(PyGridCellEditor, Destroy);
    %MAKE_BASE_FUNC(PyGridCellEditor, SetParameters);
};

//---------------------------------------------------------------------------
//  Predefined Editors

class wxGridCellTextEditor : public wxGridCellEditor
{
public:
    %pythonAppend wxGridCellTextEditor  "self._setOORInfo(self)"
    wxGridCellTextEditor();
    virtual wxString GetValue();

    %property(Value, GetValue, doc="See `GetValue`");
};


class wxGridCellNumberEditor : public wxGridCellTextEditor
{
public:
    %pythonAppend wxGridCellNumberEditor  "self._setOORInfo(self)"
    wxGridCellNumberEditor(int min = -1, int max = -1);
};


class wxGridCellFloatEditor : public wxGridCellTextEditor
{
public:
    %pythonAppend wxGridCellFloatEditor  "self._setOORInfo(self)"
    wxGridCellFloatEditor(int width = -1, int precision = -1);
};


MAKE_CONST_WXSTRING2(OneString, _T("1"));
class wxGridCellBoolEditor : public wxGridCellEditor
{
public:
    %pythonAppend wxGridCellBoolEditor  "self._setOORInfo(self)"
    wxGridCellBoolEditor();

    // set the string values returned by GetValue() for the true and false
    // states, respectively
    static void UseStringValues(const wxString& valueTrue = wxPyOneString,
                                const wxString& valueFalse = wxPyEmptyString);

    // return true if the given string is equal to the string representation of
    // true value which we currently use
    static bool IsTrueValue(const wxString& value);

};

class wxGridCellChoiceEditor : public wxGridCellEditor
{
public:
    %pythonAppend wxGridCellChoiceEditor  "self._setOORInfo(self)"
    wxGridCellChoiceEditor(int choices = 0,
                           const wxString* choices_array = NULL,
                           bool allowOthers = false);
};


class wxGridCellEnumEditor : public wxGridCellChoiceEditor
{
public:
    %pythonAppend wxGridCellEnumEditor  "self._setOORInfo(self)"
    wxGridCellEnumEditor( const wxString& choices = wxPyEmptyString );
};


class wxGridCellAutoWrapStringEditor : public wxGridCellTextEditor
{
public:
    %pythonAppend wxGridCellAutoWrapStringEditor  "self._setOORInfo(self)"
    wxGridCellAutoWrapStringEditor();
};



//---------------------------------------------------------------------------


class wxGridCellAttr
{
public:
    enum wxAttrKind
    {
        Any,
        Default,
        Cell,
        Row,
        Col,
        Merged
    };

    %extend {
        void _setOORInfo(PyObject* _self) {
            if (!self->GetClientObject())
                self->SetClientObject(new wxPyOORClientData(_self));
        }
    }

    %pythonAppend wxGridCellAttr  "self._setOORInfo(self)"

    wxGridCellAttr(wxGridCellAttr *attrDefault = NULL);

    %extend {
        // A dummy dtor to shut up SWIG.  (The real one is protected and can
        // only be called by DecRef)
        ~wxGridCellAttr() {
        }
    }
    
    wxGridCellAttr *Clone() const;
    void MergeWith(wxGridCellAttr *mergefrom);
    
    void IncRef();
    void DecRef();
    
    void SetTextColour(const wxColour& colText);
    void SetBackgroundColour(const wxColour& colBack);
    void SetFont(const wxFont& font);
    void SetAlignment(int hAlign, int vAlign);
    void SetSize(int num_rows, int num_cols);
    void SetOverflow( bool allow = true );
    void SetReadOnly(bool isReadOnly = true);

    void SetRenderer(wxGridCellRenderer *renderer);
    void SetEditor(wxGridCellEditor* editor);
    void SetKind(wxAttrKind kind);

    bool HasTextColour() const;
    bool HasBackgroundColour() const;
    bool HasFont() const;
    bool HasAlignment() const;
    bool HasRenderer() const;
    bool HasEditor() const;
    bool HasReadWriteMode() const;
    bool HasOverflowMode() const;

    wxColour GetTextColour() const;
    wxColour GetBackgroundColour() const;
    wxFont GetFont() const;

    DocDeclA(
        void, GetAlignment(int *OUTPUT, int *OUTPUT) const,
        "GetAlignment() -> (hAlign, vAlign)");

    DocDeclA(
        void, GetSize(int *OUTPUT, int *OUTPUT) const,
        "GetSize() -> (num_rows, num_cols)");

    bool GetOverflow() const;
    wxGridCellRenderer *GetRenderer(wxGrid* grid, int row, int col) const;
    wxGridCellEditor *GetEditor(wxGrid* grid, int row, int col) const;

    bool IsReadOnly() const;
    wxAttrKind GetKind();
    void SetDefAttr(wxGridCellAttr* defAttr);
    
    %property(Alignment, GetAlignment, SetAlignment, doc="See `GetAlignment` and `SetAlignment`");
    %property(BackgroundColour, GetBackgroundColour, SetBackgroundColour, doc="See `GetBackgroundColour` and `SetBackgroundColour`");
    %property(Font, GetFont, SetFont, doc="See `GetFont` and `SetFont`");
    %property(Kind, GetKind, SetKind, doc="See `GetKind` and `SetKind`");
    %property(Overflow, GetOverflow, SetOverflow, doc="See `GetOverflow` and `SetOverflow`");
    %property(Size, GetSize, SetSize, doc="See `GetSize` and `SetSize`");
    %property(TextColour, GetTextColour, SetTextColour, doc="See `GetTextColour` and `SetTextColour`");
};

//---------------------------------------------------------------------------

class wxGridCellAttrProvider
{
public:
    %pythonAppend wxGridCellAttrProvider "self._setOORInfo(self)"
    wxGridCellAttrProvider();
    // ???? virtual ~wxGridCellAttrProvider();

    %extend {
        void _setOORInfo(PyObject* _self) {
            if (!self->GetClientObject())
                self->SetClientObject(new wxPyOORClientData(_self));
        }
    }

    wxGridCellAttr *GetAttr(int row, int col,
                            wxGridCellAttr::wxAttrKind  kind) const;
    void SetAttr(wxGridCellAttr *attr, int row, int col);
    void SetRowAttr(wxGridCellAttr *attr, int row);
    void SetColAttr(wxGridCellAttr *attr, int col);

    void UpdateAttrRows( size_t pos, int numRows );
    void UpdateAttrCols( size_t pos, int numCols );

};


// A Python-aware version
%{
class wxPyGridCellAttrProvider : public wxGridCellAttrProvider
{
public:
    wxPyGridCellAttrProvider() : wxGridCellAttrProvider() {};

    PYCALLBACK_GCA_INTINTKIND(wxGridCellAttrProvider, GetAttr);
    PYCALLBACK__GCAINTINT(wxGridCellAttrProvider, SetAttr);
    PYCALLBACK__GCAINT(wxGridCellAttrProvider, SetRowAttr);
    PYCALLBACK__GCAINT(wxGridCellAttrProvider, SetColAttr);

    PYPRIVATE;
};
%}


// The python-aware version get's SWIGified
class wxPyGridCellAttrProvider : public wxGridCellAttrProvider
{
public:
    %pythonAppend wxPyGridCellAttrProvider  "self._setCallbackInfo(self, PyGridCellAttrProvider)"
    wxPyGridCellAttrProvider();
    void _setCallbackInfo(PyObject* self, PyObject* _class);

    wxGridCellAttr *GetAttr(int row, int col,
                            wxGridCellAttr::wxAttrKind kind);
    void SetAttr(wxGridCellAttr *attr, int row, int col);
    void SetRowAttr(wxGridCellAttr *attr, int row);
    void SetColAttr(wxGridCellAttr *attr, int col);

    %MAKE_BASE_FUNC(PyGridCellAttrProvider, GetAttr);
    %MAKE_BASE_FUNC(PyGridCellAttrProvider, SetAttr);
    %MAKE_BASE_FUNC(PyGridCellAttrProvider, SetRowAttr);
    %MAKE_BASE_FUNC(PyGridCellAttrProvider, SetColAttr);
};


//---------------------------------------------------------------------------
// Grid Table Base class and Python aware version


class wxGridTableBase : public wxObject
{
public:
    // wxGridTableBase();   This is an ABC
    ~wxGridTableBase();

    %extend {
        void _setOORInfo(PyObject* _self) {
            if (!self->GetClientObject())
                self->SetClientObject(new wxPyOORClientData(_self));
        }
    }

    void SetAttrProvider(wxGridCellAttrProvider *attrProvider);
    wxGridCellAttrProvider *GetAttrProvider() const;
    void SetView( wxGrid *grid );
    wxGrid * GetView() const;


    // pure virtuals
    virtual int GetNumberRows();
    virtual int GetNumberCols();
    virtual bool IsEmptyCell( int row, int col );
    virtual wxString GetValue( int row, int col );
    virtual void SetValue( int row, int col, const wxString& value );

    // virtuals overridable in wxPyGridTableBase
    virtual wxString GetTypeName( int row, int col );
    virtual bool CanGetValueAs( int row, int col, const wxString& typeName );
    virtual bool CanSetValueAs( int row, int col, const wxString& typeName );
    virtual long GetValueAsLong( int row, int col );
    virtual double GetValueAsDouble( int row, int col );
    virtual bool GetValueAsBool( int row, int col );
    virtual void SetValueAsLong( int row, int col, long value );
    virtual void SetValueAsDouble( int row, int col, double value );
    virtual void SetValueAsBool( int row, int col, bool value );

    //virtual void* GetValueAsCustom( int row, int col, const wxString& typeName );
    //virtual void  SetValueAsCustom( int row, int col, const wxString& typeName, void* value );


    virtual void Clear();
    virtual bool InsertRows( size_t pos = 0, size_t numRows = 1 );
    virtual bool AppendRows( size_t numRows = 1 );
    virtual bool DeleteRows( size_t pos = 0, size_t numRows = 1 );
    virtual bool InsertCols( size_t pos = 0, size_t numCols = 1 );
    virtual bool AppendCols( size_t numCols = 1 );
    virtual bool DeleteCols( size_t pos = 0, size_t numCols = 1 );

    virtual wxString GetRowLabelValue( int row );
    virtual wxString GetColLabelValue( int col );
    virtual void SetRowLabelValue( int row, const wxString& value );
    virtual void SetColLabelValue( int col, const wxString& value );

    virtual bool CanHaveAttributes();

    virtual wxGridCellAttr *GetAttr( int row, int col,
                                     wxGridCellAttr::wxAttrKind  kind);
    virtual void SetAttr(wxGridCellAttr* attr, int row, int col);
    virtual void SetRowAttr(wxGridCellAttr *attr, int row);
    virtual void SetColAttr(wxGridCellAttr *attr, int col);

    %property(AttrProvider, GetAttrProvider, SetAttrProvider, doc="See `GetAttrProvider` and `SetAttrProvider`");
    %property(NumberCols, GetNumberCols, doc="See `GetNumberCols`");
    %property(NumberRows, GetNumberRows, doc="See `GetNumberRows`");
    %property(View, GetView, SetView, doc="See `GetView` and `SetView`");
};



// Python-aware version
%{
class wxPyGridTableBase : public wxGridTableBase
{
public:
    wxPyGridTableBase() : wxGridTableBase() {}

    PYCALLBACK_INT__pure(GetNumberRows);
    PYCALLBACK_INT__pure(GetNumberCols);
    PYCALLBACK_BOOL_INTINT_pure(IsEmptyCell);
    PYCALLBACK_STRING_INTINT(wxGridTableBase, GetTypeName);
    PYCALLBACK_BOOL_INTINTSTRING(wxGridTableBase, CanGetValueAs);
    PYCALLBACK_BOOL_INTINTSTRING(wxGridTableBase, CanSetValueAs);
    PYCALLBACK__(wxGridTableBase, Clear);
    PYCALLBACK_BOOL_SIZETSIZET(wxGridTableBase, InsertRows);
    PYCALLBACK_BOOL_SIZETSIZET(wxGridTableBase, DeleteRows);
    PYCALLBACK_BOOL_SIZETSIZET(wxGridTableBase, InsertCols);
    PYCALLBACK_BOOL_SIZETSIZET(wxGridTableBase, DeleteCols);
    PYCALLBACK_BOOL_SIZET(wxGridTableBase, AppendRows);
    PYCALLBACK_BOOL_SIZET(wxGridTableBase, AppendCols);
    PYCALLBACK_STRING_INT(wxGridTableBase, GetRowLabelValue);
    PYCALLBACK_STRING_INT(wxGridTableBase, GetColLabelValue);
    PYCALLBACK__INTSTRING(wxGridTableBase, SetRowLabelValue);
    PYCALLBACK__INTSTRING(wxGridTableBase, SetColLabelValue);
    PYCALLBACK_BOOL_(wxGridTableBase, CanHaveAttributes);
    PYCALLBACK_GCA_INTINTKIND(wxGridTableBase, GetAttr);
    PYCALLBACK__GCAINTINT(wxGridTableBase, SetAttr);
    PYCALLBACK__GCAINT(wxGridTableBase, SetRowAttr);
    PYCALLBACK__GCAINT(wxGridTableBase, SetColAttr);


    wxString GetValue(int row, int col) {
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        wxString rval;
        if (wxPyCBH_findCallback(m_myInst, "GetValue")) {
            PyObject* ro;
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(ii)",row,col));
            if (ro) {
                if (!PyString_Check(ro) && !PyUnicode_Check(ro)) {
                    PyObject* old = ro;
                    ro = PyObject_Str(ro);
                    Py_DECREF(old);
                }
                rval = Py2wxString(ro);
                Py_DECREF(ro);
            }
        }
        wxPyEndBlockThreads(blocked);
        return rval;
    }

    void SetValue(int row, int col, const wxString& val) {
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if (wxPyCBH_findCallback(m_myInst, "SetValue")) {
            PyObject* s = wx2PyString(val);
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iiO)",row,col,s));
            Py_DECREF(s);
	}
        wxPyEndBlockThreads(blocked);
    }


    // Map the Get/Set methods for the standard non-string types to
    // the GetValue and SetValue python methods.
    long GetValueAsLong( int row, int col ) {
        long rval = 0;
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if (wxPyCBH_findCallback(m_myInst, "GetValue")) {
            PyObject* ro;
            PyObject* num;
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(ii)", row, col));
            if (ro && PyNumber_Check(ro)) {
                num = PyNumber_Int(ro);
                if (num) {
                    rval = PyInt_AsLong(num);
                    Py_DECREF(num);
                }
                Py_DECREF(ro);
            }
        }
        wxPyEndBlockThreads(blocked);
        return rval;
    }

    double GetValueAsDouble( int row, int col ) {
        double rval = 0.0;
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if (wxPyCBH_findCallback(m_myInst, "GetValue")) {
            PyObject* ro;
            PyObject* num;
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(ii)", row, col));
            if (ro && PyNumber_Check(ro)) {
                num = PyNumber_Float(ro);
                if (num) {
                    rval = PyFloat_AsDouble(num);
                    Py_DECREF(num);
                }
                Py_DECREF(ro);
            }
        }
        wxPyEndBlockThreads(blocked);
        return rval;
    }

    bool GetValueAsBool( int row, int col ) {
        return (bool)GetValueAsLong(row, col);
    }

    void SetValueAsLong( int row, int col, long value ) {
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if (wxPyCBH_findCallback(m_myInst, "SetValue")) {
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iii)", row, col, value));
        }
        wxPyEndBlockThreads(blocked);
    }

    void SetValueAsDouble( int row, int col, double value ) {
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if (wxPyCBH_findCallback(m_myInst, "SetValue")) {
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iid)", row, col, value));
        }
        wxPyEndBlockThreads(blocked);
    }

    void SetValueAsBool( int row, int col, bool value ) {
        SetValueAsLong( row, col, (long)value );
    }


    PYPRIVATE;
};
%}


// The python-aware version get's SWIGified
class wxPyGridTableBase : public wxGridTableBase
{
public:
    %pythonAppend wxPyGridTableBase "self._setCallbackInfo(self, PyGridTableBase);self._setOORInfo(self)"
    wxPyGridTableBase();
    void _setCallbackInfo(PyObject* self, PyObject* _class);

    %pythonPrepend Destroy "args[0].this.own(False)"
    %extend { void Destroy() { delete self; } }

    wxString GetTypeName( int row, int col );
    bool CanGetValueAs( int row, int col, const wxString& typeName );
    bool CanSetValueAs( int row, int col, const wxString& typeName );
    void Clear();
    bool InsertRows( size_t pos = 0, size_t numRows = 1 );
    bool AppendRows( size_t numRows = 1 );
    bool DeleteRows( size_t pos = 0, size_t numRows = 1 );
    bool InsertCols( size_t pos = 0, size_t numCols = 1 );
    bool AppendCols( size_t numCols = 1 );
    bool DeleteCols( size_t pos = 0, size_t numCols = 1 );
    wxString GetRowLabelValue( int row );
    wxString GetColLabelValue( int col );
    void SetRowLabelValue( int row, const wxString& value );
    void SetColLabelValue( int col, const wxString& value );
    bool CanHaveAttributes();
    wxGridCellAttr *GetAttr( int row, int col,
                                  wxGridCellAttr::wxAttrKind kind );
    void SetAttr(wxGridCellAttr* attr, int row, int col);
    void SetRowAttr(wxGridCellAttr *attr, int row);
    void SetColAttr(wxGridCellAttr *attr, int col);

    %MAKE_BASE_FUNC(PyGridTableBase, GetTypeName);
    %MAKE_BASE_FUNC(PyGridTableBase, CanGetValueAs);
    %MAKE_BASE_FUNC(PyGridTableBase, CanSetValueAs);
    %MAKE_BASE_FUNC(PyGridTableBase, Clear);
    %MAKE_BASE_FUNC(PyGridTableBase, InsertRows);
    %MAKE_BASE_FUNC(PyGridTableBase, AppendRows);
    %MAKE_BASE_FUNC(PyGridTableBase, DeleteRows);
    %MAKE_BASE_FUNC(PyGridTableBase, InsertCols);
    %MAKE_BASE_FUNC(PyGridTableBase, AppendCols);
    %MAKE_BASE_FUNC(PyGridTableBase, DeleteCols);
    %MAKE_BASE_FUNC(PyGridTableBase, GetRowLabelValue);
    %MAKE_BASE_FUNC(PyGridTableBase, GetColLabelValue);
    %MAKE_BASE_FUNC(PyGridTableBase, SetRowLabelValue);
    %MAKE_BASE_FUNC(PyGridTableBase, SetColLabelValue);
    %MAKE_BASE_FUNC(PyGridTableBase, CanHaveAttributes);
    %MAKE_BASE_FUNC(PyGridTableBase, GetAttr);
    %MAKE_BASE_FUNC(PyGridTableBase, SetAttr);
    %MAKE_BASE_FUNC(PyGridTableBase, SetRowAttr);
    %MAKE_BASE_FUNC(PyGridTableBase, SetColAttr);
};


//---------------------------------------------------------------------------
// Predefined Tables

class  wxGridStringTable : public wxGridTableBase
{
public:
    %pythonAppend wxGridStringTable "self._setOORInfo(self)"
    wxGridStringTable( int numRows=0, int numCols=0 );
};

//---------------------------------------------------------------------------
// The Table can pass messages to the grid to tell it to update itself if
// something has changed.

enum wxGridTableRequest
{
    wxGRIDTABLE_REQUEST_VIEW_GET_VALUES = 2000,
    wxGRIDTABLE_REQUEST_VIEW_SEND_VALUES,
    wxGRIDTABLE_NOTIFY_ROWS_INSERTED,
    wxGRIDTABLE_NOTIFY_ROWS_APPENDED,
    wxGRIDTABLE_NOTIFY_ROWS_DELETED,
    wxGRIDTABLE_NOTIFY_COLS_INSERTED,
    wxGRIDTABLE_NOTIFY_COLS_APPENDED,
    wxGRIDTABLE_NOTIFY_COLS_DELETED
};


class wxGridTableMessage
{
public:
    wxGridTableMessage( wxGridTableBase *table, int id,
                        int comInt1 = -1,
                        int comInt2 = -1 );
    ~wxGridTableMessage();

    void SetTableObject( wxGridTableBase *table );
    wxGridTableBase * GetTableObject() const;
    void SetId( int id );
    int  GetId();
    void SetCommandInt( int comInt1 );
    int  GetCommandInt();
    void SetCommandInt2( int comInt2 );
    int  GetCommandInt2();

    %property(CommandInt, GetCommandInt, SetCommandInt, doc="See `GetCommandInt` and `SetCommandInt`");
    %property(CommandInt2, GetCommandInt2, SetCommandInt2, doc="See `GetCommandInt2` and `SetCommandInt2`");
    %property(Id, GetId, SetId, doc="See `GetId` and `SetId`");
    %property(TableObject, GetTableObject, SetTableObject, doc="See `GetTableObject` and `SetTableObject`");
};


//---------------------------------------------------------------------------


// Typemap to allow conversion of sequence objects to wxGridCellCoords...
%typemap(in) wxGridCellCoords& (wxGridCellCoords temp) {
    $1 = &temp;
    if (! wxGridCellCoords_helper($input, &$1)) SWIG_fail;
}
%typemap(typecheck, precedence=SWIG_TYPECHECK_POINTER) wxGridCellCoords& {
    $1 = wxGridCellCoords_typecheck($input);
}


// ...and here is the associated helper.
%{
bool wxGridCellCoords_helper(PyObject* source, wxGridCellCoords** obj) {

    if (source == Py_None) {
        **obj = wxGridCellCoords(-1,-1);
        return true;
    }

    // If source is an object instance then it may already be the right type
    if (wxPySwigInstance_Check(source)) {
        wxGridCellCoords* ptr;
        if (! wxPyConvertSwigPtr(source, (void **)&ptr, wxT("wxGridCellCoords")))
            goto error;
        *obj = ptr;
        return true;
    }
    // otherwise a 2-tuple of integers is expected
    else if (PySequence_Check(source) && PyObject_Length(source) == 2) {
        PyObject* o1 = PySequence_GetItem(source, 0);
        PyObject* o2 = PySequence_GetItem(source, 1);
        if (!PyNumber_Check(o1) || !PyNumber_Check(o2)) {
            Py_DECREF(o1);
            Py_DECREF(o2);
            goto error;
        }
        **obj = wxGridCellCoords(PyInt_AsLong(o1), PyInt_AsLong(o2));
        Py_DECREF(o1);
        Py_DECREF(o2);
        return true;
    }

 error:
    PyErr_SetString(PyExc_TypeError, "Expected a 2-tuple of integers or a wxGridCellCoords object.");
    return false;
}


bool wxGridCellCoords_typecheck(PyObject* source) {
    void* ptr;

    if (wxPySwigInstance_Check(source) &&
        wxPyConvertSwigPtr(source, (void **)&ptr, wxT("wxGridCellCoords")))
        return true;

    PyErr_Clear();
    if (PySequence_Check(source) && PySequence_Length(source) == 2)
        return true;

    return false;
}
%}


// Typemap to convert an array of cells coords to a list of tuples...
%typemap(out) wxGridCellCoordsArray {
    $result = wxGridCellCoordsArray_helper($1);
}

// %typemap(ret) wxGridCellCoordsArray {
//     delete $1;
// }


// ...and the helper function for the above typemap.
%{
PyObject* wxGridCellCoordsArray_helper(const wxGridCellCoordsArray& source)
{
    PyObject* list = PyList_New(0);
    size_t idx;
    for (idx = 0; idx < source.GetCount(); idx += 1) {
        wxGridCellCoords& coord = source.Item(idx);
        PyObject* tup = PyTuple_New(2);
        PyTuple_SET_ITEM(tup, 0, PyInt_FromLong(coord.GetRow()));
        PyTuple_SET_ITEM(tup, 1, PyInt_FromLong(coord.GetCol()));
        PyList_Append(list, tup);
        Py_DECREF(tup);
    }
    return list;
}
%}





class wxGridCellCoords
{
public:
    wxGridCellCoords( int r=-1, int c=-1 );
    ~wxGridCellCoords();

    int GetRow() const;
    void SetRow( int n );
    int GetCol() const;
    void SetCol( int n );
    void Set( int row, int col );

    %extend {
        KeepGIL(__eq__);
        DocStr(__eq__, "Test for equality of GridCellCoords objects.", "");
        bool __eq__(PyObject* other) {
            wxGridCellCoords  temp, *obj = &temp;
            if ( other == Py_None ) return false;
            if ( ! wxGridCellCoords_helper(other, &obj) ) {
                PyErr_Clear();
                return false;
            }
            return self->operator==(*obj);
        }

        
        KeepGIL(__ne__);
        DocStr(__ne__, "Test for inequality of GridCellCoords objects.", "");
        bool __ne__(PyObject* other) {
            wxGridCellCoords  temp, *obj = &temp;
            if ( other == Py_None ) return true;
            if ( ! wxGridCellCoords_helper(other, &obj)) {
                PyErr_Clear();
                return true;
            }
            return self->operator!=(*obj);
        }
    }


    %extend {
        PyObject* Get() {
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, PyInt_FromLong(self->GetRow()));
            PyTuple_SET_ITEM(tup, 1, PyInt_FromLong(self->GetCol()));
            return tup;
        }
    }
    %pythoncode {
    asTuple = wx._deprecated(Get, "asTuple is deprecated, use `Get` instead")
    def __str__(self):                   return str(self.Get())
    def __repr__(self):                  return 'wxGridCellCoords'+str(self.Get())
    def __len__(self):                   return len(self.Get())
    def __getitem__(self, index):        return self.asTuple()[index]
    def __setitem__(self, index, val):
        if index == 0: self.SetRow(val)
        elif index == 1: self.SetCol(val)
        else: raise IndexError
    }

    %property(Col, GetCol, SetCol, doc="See `GetCol` and `SetCol`");
    %property(Row, GetRow, SetRow, doc="See `GetRow` and `SetRow`");
};


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// The grid itself


// Fool SWIG into treating this enum as an int
typedef int WXGRIDSELECTIONMODES;

// but let the C++ code know what it really is.
%{
typedef wxGrid::wxGridSelectionModes WXGRIDSELECTIONMODES;
%}



MustHaveApp(wxGrid);

class wxGrid : public wxScrolledWindow
{
public:
    %pythonAppend wxGrid "self._setOORInfo(self)"
    %pythonAppend wxGrid() ""
    
    %typemap(out) wxGrid*;    // turn off this typemap

    wxGrid( wxWindow *parent,
            wxWindowID id=-1,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxWANTS_CHARS,
            const wxString& name = wxPyGridNameStr);

    %RenameCtor(PreGrid,  wxGrid());

    
    // Turn it back on again
    %typemap(out) wxGrid* { $result = wxPyMake_wxObject($1, $owner); }


    bool Create( wxWindow *parent,
                 wxWindowID id=-1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxWANTS_CHARS,
                 const wxString& name = wxPyGridNameStr );

    // Override the global renamer to leave these as they are, for backwards
    // compatibility
    %rename(wxGridSelectCells)   wxGridSelectCells;
    %rename(wxGridSelectRows)    wxGridSelectRows;
    %rename(wxGridSelectColumns) wxGridSelectColumns;
    
    enum wxGridSelectionModes {
        wxGridSelectCells,
        wxGridSelectRows,
        wxGridSelectColumns
    };
    %pythoncode {
        SelectCells =   wxGridSelectCells
        SelectRows =    wxGridSelectRows
        SelectColumns = wxGridSelectColumns
    }

    bool CreateGrid( int numRows, int numCols,
                     WXGRIDSELECTIONMODES selmode = wxGrid::wxGridSelectCells );
    void SetSelectionMode(WXGRIDSELECTIONMODES selmode);
    WXGRIDSELECTIONMODES GetSelectionMode();


    // ------ grid dimensions
    //
    int      GetNumberRows();
    int      GetNumberCols();


    bool ProcessTableMessage( wxGridTableMessage& );


    wxGridTableBase * GetTable() const;

    %disownarg(wxGridTableBase *);
    bool SetTable( wxGridTableBase *table, bool takeOwnership=false,
                   WXGRIDSELECTIONMODES selmode =
                   wxGrid::wxGridSelectCells );
    %cleardisown(wxGridTableBase *);

    void ClearGrid();
    bool InsertRows( int pos = 0, int numRows = 1, bool updateLabels=true );
    bool AppendRows( int numRows = 1, bool updateLabels=true );
    bool DeleteRows( int pos = 0, int numRows = 1, bool updateLabels=true );
    bool InsertCols( int pos = 0, int numCols = 1, bool updateLabels=true );
    bool AppendCols( int numCols = 1, bool updateLabels=true );
    bool DeleteCols( int pos = 0, int numCols = 1, bool updateLabels=true );


    // this function is called when the current cell highlight must be redrawn
    // and may be overridden by the user
    virtual void DrawCellHighlight( wxDC& dc, const wxGridCellAttr *attr );


    // ------ Cell text drawing functions
    //
    void DrawTextRectangle( wxDC& dc, const wxString&, const wxRect&,
                            int horizontalAlignment = wxLEFT,
                            int verticalAlignment = wxTOP,
                            int textOrientation = wxHORIZONTAL );

//      // Split a string containing newline chararcters into an array of
//      // strings and return the number of lines
//      //
//      void StringToLines( const wxString& value, wxArrayString& lines );

    DocDeclA(
        void, GetTextBoxSize( wxDC& dc, wxArrayString& lines,
                              long *OUTPUT, long *OUTPUT ),
        "GetTextBoxSize(DC dc, list lines) -> (width, height)");


    // ------
    // Code that does a lot of grid modification can be enclosed
    // between BeginBatch() and EndBatch() calls to avoid screen
    // flicker
    //
    void     BeginBatch();
    void     EndBatch();
    int      GetBatchCount();
    void     ForceRefresh();


    // ------ edit control functions
    //
    bool IsEditable();
    void EnableEditing( bool edit );

    void EnableCellEditControl( bool enable = true );
    void DisableCellEditControl();
    bool CanEnableCellControl() const;
    bool IsCellEditControlEnabled() const;
    bool IsCellEditControlShown() const;

    bool IsCurrentCellReadOnly() const;

    void ShowCellEditControl();
    void HideCellEditControl();
    void SaveEditControlValue();


    // ------ grid location functions
    //  Note that all of these functions work with the logical coordinates of
    //  grid cells and labels so you will need to convert from device
    //  coordinates for mouse events etc.
    //

    //void XYToCell( int x, int y, wxGridCellCoords& );
    %extend {
        wxGridCellCoords XYToCell(int x, int y) {
            wxGridCellCoords rv;
            self->XYToCell(x, y, rv);
            return rv;
        }
    }

    int  YToRow( int y );
    int  XToCol( int x, bool clipToMinMax = false );

    int  YToEdgeOfRow( int y );
    int  XToEdgeOfCol( int x );

    wxRect CellToRect( int row, int col );
    // TODO: ??? wxRect CellToRect( const wxGridCellCoords& coords );


    int  GetGridCursorRow();
    int  GetGridCursorCol();

    // check to see if a cell is either wholly visible (the default arg) or
    // at least partially visible in the grid window
    //
    bool IsVisible( int row, int col, bool wholeCellVisible = true );
    // TODO: ??? bool IsVisible( const wxGridCellCoords& coords, bool wholeCellVisible = true );
    void MakeCellVisible( int row, int col );
    // TODO: ??? void MakeCellVisible( const wxGridCellCoords& coords );


    // ------ grid cursor movement functions
    //
    void SetGridCursor( int row, int col );
    bool MoveCursorUp( bool expandSelection );
    bool MoveCursorDown( bool expandSelection );
    bool MoveCursorLeft( bool expandSelection );
    bool MoveCursorRight( bool expandSelection );
    bool MovePageDown();
    bool MovePageUp();
    bool MoveCursorUpBlock( bool expandSelection );
    bool MoveCursorDownBlock( bool expandSelection );
    bool MoveCursorLeftBlock( bool expandSelection );
    bool MoveCursorRightBlock( bool expandSelection );


    // ------ label and gridline formatting
    //
    int      GetDefaultRowLabelSize();
    int      GetRowLabelSize();
    int      GetDefaultColLabelSize();
    int      GetColLabelSize();
    wxColour GetLabelBackgroundColour();
    wxColour GetLabelTextColour();
    wxFont   GetLabelFont();

    DocDeclA(
        void, GetRowLabelAlignment( int *OUTPUT, int *OUTPUT ),
        "GetRowLabelAlignment() -> (horiz, vert)");

    DocDeclA(
        void, GetColLabelAlignment( int *OUTPUT, int *OUTPUT ),
        "GetColLabelAlignment() -> (horiz, vert)");

    int      GetColLabelTextOrientation();
    wxString GetRowLabelValue( int row );
    wxString GetColLabelValue( int col );
    wxColour GetGridLineColour();

    virtual wxPen GetDefaultGridLinePen();
    virtual wxPen GetRowGridLinePen(int row);
    virtual wxPen GetColGridLinePen(int col);
    
    
    wxColour GetCellHighlightColour();
    int      GetCellHighlightPenWidth();
    int      GetCellHighlightROPenWidth();

    void     SetRowLabelSize( int width );
    void     SetColLabelSize( int height );
    void     SetLabelBackgroundColour( const wxColour& );
    void     SetLabelTextColour( const wxColour& );
    void     SetLabelFont( const wxFont& );
    void     SetRowLabelAlignment( int horiz, int vert );
    void     SetColLabelAlignment( int horiz, int vert );
    void     SetColLabelTextOrientation( int textOrientation );
    void     SetRowLabelValue( int row, const wxString& );
    void     SetColLabelValue( int col, const wxString& );
    void     SetGridLineColour( const wxColour& );
    void     SetCellHighlightColour( const wxColour& );
    void     SetCellHighlightPenWidth(int width);
    void     SetCellHighlightROPenWidth(int width);

    void     EnableDragRowSize( bool enable = true );
    void     DisableDragRowSize();
    bool     CanDragRowSize();
    void     EnableDragColSize( bool enable = true );
    void     DisableDragColSize();
    bool     CanDragColSize();
    void     EnableDragColMove( bool enable = true );
    void     DisableDragColMove() { EnableDragColMove( false ); }
    bool     CanDragColMove() { return m_canDragColMove; }
    void     EnableDragGridSize(bool enable = true);
    void     DisableDragGridSize();
    bool     CanDragGridSize();

    void     EnableDragCell( bool enable = true );
    void     DisableDragCell();
    bool     CanDragCell();

    // this sets the specified attribute for all cells in this row/col
    void     SetAttr(int row, int col, wxGridCellAttr *attr);
    void     SetRowAttr(int row, wxGridCellAttr *attr);
    void     SetColAttr(int col, wxGridCellAttr *attr);

    // returns the attribute we may modify in place: a new one if this cell
    // doesn't have any yet or the existing one if it does
    //
    // DecRef() must be called on the returned pointer, as usual
    wxGridCellAttr *GetOrCreateCellAttr(int row, int col) const;

    
    // shortcuts for setting the column parameters

    // set the format for the data in the column: default is string
    void     SetColFormatBool(int col);
    void     SetColFormatNumber(int col);
    void     SetColFormatFloat(int col, int width = -1, int precision = -1);
    void     SetColFormatCustom(int col, const wxString& typeName);

    void     EnableGridLines( bool enable = true );
    bool     GridLinesEnabled();

    // ------ row and col formatting
    //
    int      GetDefaultRowSize();
    int      GetRowSize( int row );
    int      GetDefaultColSize();
    int      GetColSize( int col );
    wxColour GetDefaultCellBackgroundColour();
    wxColour GetCellBackgroundColour( int row, int col );
    wxColour GetDefaultCellTextColour();
    wxColour GetCellTextColour( int row, int col );
    wxFont   GetDefaultCellFont();
    wxFont   GetCellFont( int row, int col );

    DocDeclA(
        void, GetDefaultCellAlignment( int *OUTPUT, int *OUTPUT ),
        "GetDefaultCellAlignment() -> (horiz, vert)");

    DocDeclA(
        void, GetCellAlignment( int row, int col, int *OUTPUT, int *OUTPUT ),
        "GetCellAlignment(int row, int col) -> (horiz, vert)");

    bool     GetDefaultCellOverflow();
    bool     GetCellOverflow( int row, int col );

    DocDeclA(
        void, GetCellSize( int row, int col, int *OUTPUT, int *OUTPUT ),
        "GetCellSize(int row, int col) -> (num_rows, num_cols)");

    void     SetDefaultRowSize( int height, bool resizeExistingRows = false );
    void     SetRowSize( int row, int height );
    void     SetDefaultColSize( int width, bool resizeExistingCols = false );

    void     SetColSize( int col, int width );

    int GetColAt( int colPos ) const;
    void SetColPos( int colID, int newPos );
    int GetColPos( int colID ) const;
    
    // automatically size the column or row to fit to its contents, if
    // setAsMin is True, this optimal width will also be set as minimal width
    // for this column
    void     AutoSizeColumn( int col, bool setAsMin = true );
    void     AutoSizeRow( int row, bool setAsMin = true );


    // auto size all columns (very ineffective for big grids!)
    void     AutoSizeColumns( bool setAsMin = true );
    void     AutoSizeRows( bool setAsMin = true );

    // auto size the grid, that is make the columns/rows of the "right" size
    // and also set the grid size to just fit its contents
    void     AutoSize();

    // autosize row height depending on label text
    void     AutoSizeRowLabelSize( int row );

    // autosize column width depending on label text
    void     AutoSizeColLabelSize( int col );


    // column won't be resized to be lesser width - this must be called during
    // the grid creation because it won't resize the column if it's already
    // narrower than the minimal width
    void     SetColMinimalWidth( int col, int width );
    void     SetRowMinimalHeight( int row, int width );

    void     SetColMinimalAcceptableWidth( int width );
    void     SetRowMinimalAcceptableHeight( int width );
    int      GetColMinimalAcceptableWidth() const;
    int      GetRowMinimalAcceptableHeight() const;

    void     SetDefaultCellBackgroundColour( const wxColour& );
    void     SetCellBackgroundColour( int row, int col, const wxColour& );
    void     SetDefaultCellTextColour( const wxColour& );

    void     SetCellTextColour( int row, int col, const wxColour& );
    void     SetDefaultCellFont( const wxFont& );
    void     SetCellFont( int row, int col, const wxFont& );
    void     SetDefaultCellAlignment( int horiz, int vert );
    void     SetCellAlignment( int row, int col, int horiz, int vert );
    void     SetDefaultCellOverflow( bool allow );
    void     SetCellOverflow( int row, int col, bool allow );
    void     SetCellSize( int row, int col, int num_rows, int num_cols );

    // takes ownership of the pointer
    void SetDefaultRenderer(wxGridCellRenderer *renderer);
    void SetCellRenderer(int row, int col, wxGridCellRenderer *renderer);
    wxGridCellRenderer *GetDefaultRenderer() const;
    wxGridCellRenderer* GetCellRenderer(int row, int col);

    // takes ownership of the pointer
    void SetDefaultEditor(wxGridCellEditor *editor);
    void SetCellEditor(int row, int col, wxGridCellEditor *editor);
    wxGridCellEditor *GetDefaultEditor() const;
    wxGridCellEditor* GetCellEditor(int row, int col);



    // ------ cell value accessors
    //
    wxString GetCellValue( int row, int col );
    // TODO: ??? wxString GetCellValue( const wxGridCellCoords& coords )

    void SetCellValue( int row, int col, const wxString& s );
    // TODO: ??? void SetCellValue( const wxGridCellCoords& coords, const wxString& s )

    // returns True if the cell can't be edited
    bool IsReadOnly(int row, int col) const;

    // make the cell editable/readonly
    void SetReadOnly(int row, int col, bool isReadOnly = true);

    // ------ selections of blocks of cells
    //
    void SelectRow( int row, bool addToSelected = false );
    void SelectCol( int col, bool addToSelected = false );

    void SelectBlock( int topRow, int leftCol, int bottomRow, int rightCol,
                      bool addToSelected = false );
    // TODO: ??? void SelectBlock( const wxGridCellCoords& topLeft,
    // TODO: ???                   const wxGridCellCoords& bottomRight )

    void SelectAll();
    bool IsSelection();
    void ClearSelection();
    bool IsInSelection( int row, int col );
    // TODO: ??? bool IsInSelection( const wxGridCellCoords& coords )

    const wxGridCellCoordsArray GetSelectedCells() const;
    const wxGridCellCoordsArray GetSelectionBlockTopLeft() const;
    const wxGridCellCoordsArray GetSelectionBlockBottomRight() const;
    const wxArrayInt GetSelectedRows() const;
    const wxArrayInt GetSelectedCols() const;

    void DeselectRow( int row );
    void DeselectCol( int col );
    void DeselectCell( int row, int col );


    // This function returns the rectangle that encloses the block of cells
    // limited by TopLeft and BottomRight cell in device coords and clipped
    //  to the client size of the grid window.
    //
    wxRect BlockToDeviceRect( const wxGridCellCoords & topLeft,
                              const wxGridCellCoords & bottomRight );


    // Access or update the selection fore/back colours
    wxColour GetSelectionBackground() const;
    wxColour GetSelectionForeground() const;

    void SetSelectionBackground(const wxColour& c);
    void SetSelectionForeground(const wxColour& c);


    // Methods for a registry for mapping data types to Renderers/Editors
    void RegisterDataType(const wxString& typeName,
                          wxGridCellRenderer* renderer,
                          wxGridCellEditor* editor);
    wxGridCellEditor* GetDefaultEditorForCell(int row, int col) const;
    // TODO: ??? wxGridCellEditor* GetDefaultEditorForCell(const wxGridCellCoords& c) const
    wxGridCellRenderer* GetDefaultRendererForCell(int row, int col) const;
    wxGridCellEditor* GetDefaultEditorForType(const wxString& typeName) const;
    wxGridCellRenderer* GetDefaultRendererForType(const wxString& typeName) const;

    // grid may occupy more space than needed for its rows/columns, this
    // function allows to set how big this extra space is
    void SetMargins(int extraWidth, int extraHeight);


    // Accessors for component windows
    wxWindow* GetGridWindow();
    wxWindow* GetGridRowLabelWindow();
    wxWindow* GetGridColLabelWindow();
    wxWindow* GetGridCornerLabelWindow();

    // Allow adjustment of scroll increment. The default is (15, 15).
    void SetScrollLineX(int x);
    void SetScrollLineY(int y);
    int GetScrollLineX() const;
    int GetScrollLineY() const;

    int GetScrollX(int x) const;
    int GetScrollY(int y) const;

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    %property(BatchCount, GetBatchCount, doc="See `GetBatchCount`");
    %property(CellHighlightColour, GetCellHighlightColour, SetCellHighlightColour, doc="See `GetCellHighlightColour` and `SetCellHighlightColour`");
    %property(CellHighlightPenWidth, GetCellHighlightPenWidth, SetCellHighlightPenWidth, doc="See `GetCellHighlightPenWidth` and `SetCellHighlightPenWidth`");
    %property(CellHighlightROPenWidth, GetCellHighlightROPenWidth, SetCellHighlightROPenWidth, doc="See `GetCellHighlightROPenWidth` and `SetCellHighlightROPenWidth`");
    %property(CellSize, GetCellSize, SetCellSize, doc="See `GetCellSize` and `SetCellSize`");
    %property(ColLabelAlignment, GetColLabelAlignment, SetColLabelAlignment, doc="See `GetColLabelAlignment` and `SetColLabelAlignment`");
    %property(ColLabelSize, GetColLabelSize, SetColLabelSize, doc="See `GetColLabelSize` and `SetColLabelSize`");
    %property(ColLabelTextOrientation, GetColLabelTextOrientation, SetColLabelTextOrientation, doc="See `GetColLabelTextOrientation` and `SetColLabelTextOrientation`");
    %property(ColMinimalAcceptableWidth, GetColMinimalAcceptableWidth, SetColMinimalAcceptableWidth, doc="See `GetColMinimalAcceptableWidth` and `SetColMinimalAcceptableWidth`");
    %property(DefaultCellAlignment, GetDefaultCellAlignment, SetDefaultCellAlignment, doc="See `GetDefaultCellAlignment` and `SetDefaultCellAlignment`");
    %property(DefaultCellBackgroundColour, GetDefaultCellBackgroundColour, SetDefaultCellBackgroundColour, doc="See `GetDefaultCellBackgroundColour` and `SetDefaultCellBackgroundColour`");
    %property(DefaultCellFont, GetDefaultCellFont, SetDefaultCellFont, doc="See `GetDefaultCellFont` and `SetDefaultCellFont`");
    %property(DefaultCellOverflow, GetDefaultCellOverflow, SetDefaultCellOverflow, doc="See `GetDefaultCellOverflow` and `SetDefaultCellOverflow`");
    %property(DefaultCellTextColour, GetDefaultCellTextColour, SetDefaultCellTextColour, doc="See `GetDefaultCellTextColour` and `SetDefaultCellTextColour`");
    %property(DefaultColLabelSize, GetDefaultColLabelSize, doc="See `GetDefaultColLabelSize`");
    %property(DefaultColSize, GetDefaultColSize, SetDefaultColSize, doc="See `GetDefaultColSize` and `SetDefaultColSize`");
    %property(DefaultEditor, GetDefaultEditor, SetDefaultEditor, doc="See `GetDefaultEditor` and `SetDefaultEditor`");
    %property(DefaultGridLinePen, GetDefaultGridLinePen, doc="See `GetDefaultGridLinePen`");
    %property(DefaultRenderer, GetDefaultRenderer, SetDefaultRenderer, doc="See `GetDefaultRenderer` and `SetDefaultRenderer`");
    %property(DefaultRowLabelSize, GetDefaultRowLabelSize, doc="See `GetDefaultRowLabelSize`");
    %property(DefaultRowSize, GetDefaultRowSize, SetDefaultRowSize, doc="See `GetDefaultRowSize` and `SetDefaultRowSize`");
    %property(GridColLabelWindow, GetGridColLabelWindow, doc="See `GetGridColLabelWindow`");
    %property(GridCornerLabelWindow, GetGridCornerLabelWindow, doc="See `GetGridCornerLabelWindow`");
    %property(GridCursorCol, GetGridCursorCol, doc="See `GetGridCursorCol`");
    %property(GridCursorRow, GetGridCursorRow, doc="See `GetGridCursorRow`");
    %property(GridLineColour, GetGridLineColour, SetGridLineColour, doc="See `GetGridLineColour` and `SetGridLineColour`");
    %property(GridRowLabelWindow, GetGridRowLabelWindow, doc="See `GetGridRowLabelWindow`");
    %property(GridWindow, GetGridWindow, doc="See `GetGridWindow`");
    %property(LabelBackgroundColour, GetLabelBackgroundColour, SetLabelBackgroundColour, doc="See `GetLabelBackgroundColour` and `SetLabelBackgroundColour`");
    %property(LabelFont, GetLabelFont, SetLabelFont, doc="See `GetLabelFont` and `SetLabelFont`");
    %property(LabelTextColour, GetLabelTextColour, SetLabelTextColour, doc="See `GetLabelTextColour` and `SetLabelTextColour`");
    %property(NumberCols, GetNumberCols, doc="See `GetNumberCols`");
    %property(NumberRows, GetNumberRows, doc="See `GetNumberRows`");
    %property(RowLabelAlignment, GetRowLabelAlignment, SetRowLabelAlignment, doc="See `GetRowLabelAlignment` and `SetRowLabelAlignment`");
    %property(RowLabelSize, GetRowLabelSize, SetRowLabelSize, doc="See `GetRowLabelSize` and `SetRowLabelSize`");
    %property(RowMinimalAcceptableHeight, GetRowMinimalAcceptableHeight, SetRowMinimalAcceptableHeight, doc="See `GetRowMinimalAcceptableHeight` and `SetRowMinimalAcceptableHeight`");
    %property(ScrollLineX, GetScrollLineX, SetScrollLineX, doc="See `GetScrollLineX` and `SetScrollLineX`");
    %property(ScrollLineY, GetScrollLineY, SetScrollLineY, doc="See `GetScrollLineY` and `SetScrollLineY`");
    %property(SelectedCells, GetSelectedCells, doc="See `GetSelectedCells`");
    %property(SelectedCols, GetSelectedCols, doc="See `GetSelectedCols`");
    %property(SelectedRows, GetSelectedRows, doc="See `GetSelectedRows`");
    %property(SelectionBackground, GetSelectionBackground, SetSelectionBackground, doc="See `GetSelectionBackground` and `SetSelectionBackground`");
    %property(SelectionBlockBottomRight, GetSelectionBlockBottomRight, doc="See `GetSelectionBlockBottomRight`");
    %property(SelectionBlockTopLeft, GetSelectionBlockTopLeft, doc="See `GetSelectionBlockTopLeft`");
    %property(SelectionForeground, GetSelectionForeground, SetSelectionForeground, doc="See `GetSelectionForeground` and `SetSelectionForeground`");
    %property(SelectionMode, GetSelectionMode, SetSelectionMode, doc="See `GetSelectionMode` and `SetSelectionMode`");
    %property(Table, GetTable, SetTable, doc="See `GetTable` and `SetTable`");
    
};


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Grid events and stuff



class wxGridEvent : public wxNotifyEvent
{
public:
    wxGridEvent(int id, wxEventType type, wxGrid* obj,
                int row=-1, int col=-1, int x=-1, int y=-1, bool sel = true,
                bool control=false, bool shift=false, bool alt=false, bool meta=false);

    virtual int GetRow();
    virtual int GetCol();
    wxPoint     GetPosition();
    bool        Selecting();
    bool        ControlDown();
    bool        MetaDown();
    bool        ShiftDown();
    bool        AltDown();
    bool        CmdDown();

    %property(Col, GetCol, doc="See `GetCol`");
    %property(Position, GetPosition, doc="See `GetPosition`");
    %property(Row, GetRow, doc="See `GetRow`");
};


class  wxGridSizeEvent : public wxNotifyEvent
{
public:
    wxGridSizeEvent(int id, wxEventType type, wxGrid* obj,
                int rowOrCol=-1, int x=-1, int y=-1,
                bool control=false, bool shift=false, bool alt=false, bool meta=false);

    int         GetRowOrCol();
    wxPoint     GetPosition();
    bool        ControlDown();
    bool        MetaDown();
    bool        ShiftDown();
    bool        AltDown();
    bool        CmdDown();

    %property(Position, GetPosition, doc="See `GetPosition`");
    %property(RowOrCol, GetRowOrCol, doc="See `GetRowOrCol`");
};


class wxGridRangeSelectEvent : public wxNotifyEvent
{
public:
    wxGridRangeSelectEvent(int id, wxEventType type, wxGrid* obj,
                           const wxGridCellCoords& topLeft,
                           const wxGridCellCoords& bottomRight,
                           bool sel = true,
                           bool control=false, bool shift=false,
                           bool alt=false, bool meta=false);

    wxGridCellCoords GetTopLeftCoords();
    wxGridCellCoords GetBottomRightCoords();
    int         GetTopRow();
    int         GetBottomRow();
    int         GetLeftCol();
    int         GetRightCol();
    bool        Selecting();
    bool        ControlDown();
    bool        MetaDown();
    bool        ShiftDown();
    bool        AltDown();
    bool        CmdDown();

    %property(BottomRightCoords, GetBottomRightCoords, doc="See `GetBottomRightCoords`");
    %property(BottomRow, GetBottomRow, doc="See `GetBottomRow`");
    %property(LeftCol, GetLeftCol, doc="See `GetLeftCol`");
    %property(RightCol, GetRightCol, doc="See `GetRightCol`");
    %property(TopLeftCoords, GetTopLeftCoords, doc="See `GetTopLeftCoords`");
    %property(TopRow, GetTopRow, doc="See `GetTopRow`");
};


class wxGridEditorCreatedEvent : public wxCommandEvent {
public:
    wxGridEditorCreatedEvent(int id, wxEventType type, wxObject* obj,
                             int row, int col, wxControl* ctrl);

    int GetRow();
    int GetCol();
    wxControl* GetControl();
    void SetRow(int row);
    void SetCol(int col);
    void SetControl(wxControl* ctrl);

    %property(Col, GetCol, SetCol, doc="See `GetCol` and `SetCol`");
    %property(Control, GetControl, SetControl, doc="See `GetControl` and `SetControl`");
    %property(Row, GetRow, SetRow, doc="See `GetRow` and `SetRow`");
};



%constant wxEventType wxEVT_GRID_CELL_LEFT_CLICK;
%constant wxEventType wxEVT_GRID_CELL_RIGHT_CLICK;
%constant wxEventType wxEVT_GRID_CELL_LEFT_DCLICK;
%constant wxEventType wxEVT_GRID_CELL_RIGHT_DCLICK;
%constant wxEventType wxEVT_GRID_LABEL_LEFT_CLICK;
%constant wxEventType wxEVT_GRID_LABEL_RIGHT_CLICK;
%constant wxEventType wxEVT_GRID_LABEL_LEFT_DCLICK;
%constant wxEventType wxEVT_GRID_LABEL_RIGHT_DCLICK;
%constant wxEventType wxEVT_GRID_ROW_SIZE;
%constant wxEventType wxEVT_GRID_COL_SIZE;
%constant wxEventType wxEVT_GRID_RANGE_SELECT;
%constant wxEventType wxEVT_GRID_CELL_CHANGE;
%constant wxEventType wxEVT_GRID_SELECT_CELL;
%constant wxEventType wxEVT_GRID_EDITOR_SHOWN;
%constant wxEventType wxEVT_GRID_EDITOR_HIDDEN;
%constant wxEventType wxEVT_GRID_EDITOR_CREATED;
%constant wxEventType wxEVT_GRID_CELL_BEGIN_DRAG;



%pythoncode {
EVT_GRID_CELL_LEFT_CLICK = wx.PyEventBinder( wxEVT_GRID_CELL_LEFT_CLICK )
EVT_GRID_CELL_RIGHT_CLICK = wx.PyEventBinder( wxEVT_GRID_CELL_RIGHT_CLICK )
EVT_GRID_CELL_LEFT_DCLICK = wx.PyEventBinder( wxEVT_GRID_CELL_LEFT_DCLICK )
EVT_GRID_CELL_RIGHT_DCLICK = wx.PyEventBinder( wxEVT_GRID_CELL_RIGHT_DCLICK )
EVT_GRID_LABEL_LEFT_CLICK = wx.PyEventBinder( wxEVT_GRID_LABEL_LEFT_CLICK )
EVT_GRID_LABEL_RIGHT_CLICK = wx.PyEventBinder( wxEVT_GRID_LABEL_RIGHT_CLICK )
EVT_GRID_LABEL_LEFT_DCLICK = wx.PyEventBinder( wxEVT_GRID_LABEL_LEFT_DCLICK )
EVT_GRID_LABEL_RIGHT_DCLICK = wx.PyEventBinder( wxEVT_GRID_LABEL_RIGHT_DCLICK )
EVT_GRID_ROW_SIZE = wx.PyEventBinder( wxEVT_GRID_ROW_SIZE )
EVT_GRID_COL_SIZE = wx.PyEventBinder( wxEVT_GRID_COL_SIZE )
EVT_GRID_RANGE_SELECT = wx.PyEventBinder( wxEVT_GRID_RANGE_SELECT )
EVT_GRID_CELL_CHANGE = wx.PyEventBinder( wxEVT_GRID_CELL_CHANGE )
EVT_GRID_SELECT_CELL = wx.PyEventBinder( wxEVT_GRID_SELECT_CELL )
EVT_GRID_EDITOR_SHOWN = wx.PyEventBinder( wxEVT_GRID_EDITOR_SHOWN )
EVT_GRID_EDITOR_HIDDEN = wx.PyEventBinder( wxEVT_GRID_EDITOR_HIDDEN )
EVT_GRID_EDITOR_CREATED = wx.PyEventBinder( wxEVT_GRID_EDITOR_CREATED )
EVT_GRID_CELL_BEGIN_DRAG = wx.PyEventBinder( wxEVT_GRID_CELL_BEGIN_DRAG )


%# The same as above but with the ability to specify an identifier
EVT_GRID_CMD_CELL_LEFT_CLICK =     wx.PyEventBinder( wxEVT_GRID_CELL_LEFT_CLICK,    1 )
EVT_GRID_CMD_CELL_RIGHT_CLICK =    wx.PyEventBinder( wxEVT_GRID_CELL_RIGHT_CLICK,   1 )
EVT_GRID_CMD_CELL_LEFT_DCLICK =    wx.PyEventBinder( wxEVT_GRID_CELL_LEFT_DCLICK,   1 )
EVT_GRID_CMD_CELL_RIGHT_DCLICK =   wx.PyEventBinder( wxEVT_GRID_CELL_RIGHT_DCLICK,  1 )
EVT_GRID_CMD_LABEL_LEFT_CLICK =    wx.PyEventBinder( wxEVT_GRID_LABEL_LEFT_CLICK,   1 )
EVT_GRID_CMD_LABEL_RIGHT_CLICK =   wx.PyEventBinder( wxEVT_GRID_LABEL_RIGHT_CLICK,  1 )
EVT_GRID_CMD_LABEL_LEFT_DCLICK =   wx.PyEventBinder( wxEVT_GRID_LABEL_LEFT_DCLICK,  1 )
EVT_GRID_CMD_LABEL_RIGHT_DCLICK =  wx.PyEventBinder( wxEVT_GRID_LABEL_RIGHT_DCLICK, 1 )
EVT_GRID_CMD_ROW_SIZE =            wx.PyEventBinder( wxEVT_GRID_ROW_SIZE,           1 )
EVT_GRID_CMD_COL_SIZE =            wx.PyEventBinder( wxEVT_GRID_COL_SIZE,           1 )
EVT_GRID_CMD_RANGE_SELECT =        wx.PyEventBinder( wxEVT_GRID_RANGE_SELECT,       1 )
EVT_GRID_CMD_CELL_CHANGE =         wx.PyEventBinder( wxEVT_GRID_CELL_CHANGE,        1 )
EVT_GRID_CMD_SELECT_CELL =         wx.PyEventBinder( wxEVT_GRID_SELECT_CELL,        1 )
EVT_GRID_CMD_EDITOR_SHOWN =        wx.PyEventBinder( wxEVT_GRID_EDITOR_SHOWN,       1 )
EVT_GRID_CMD_EDITOR_HIDDEN =       wx.PyEventBinder( wxEVT_GRID_EDITOR_HIDDEN,      1 )
EVT_GRID_CMD_EDITOR_CREATED =      wx.PyEventBinder( wxEVT_GRID_EDITOR_CREATED,     1 )
EVT_GRID_CMD_CELL_BEGIN_DRAG =     wx.PyEventBinder( wxEVT_GRID_CELL_BEGIN_DRAG,    1 )
    
}

//---------------------------------------------------------------------------

%init %{
%}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

