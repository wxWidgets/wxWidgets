/////////////////////////////////////////////////////////////////////////////
// Name:        grid.i
// Purpose:     SWIG definitions for the new wxGrid and related classes
//
// Author:      Robin Dunn
//
// Created:     17-March-2000
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module grid

#ifndef OLD_GRID

%{
#include "helpers.h"
#include <wx/grid.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import misc.i
%import gdi.i
%import windows.i
%import controls.i
%import events.i

%pragma(python) code = "import wx"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Macros, similar to what's in helpers.h, to aid in the creation of
// virtual methods that are able to make callbacks to Python.  Many of these
// are specific to wxGrid and so are kept here to reduce the mess in helpers.h
// a bit.


%{
#define PYCALLBACK_GCA_INTINT(PCLASS, CBNAME)                           \
    wxGridCellAttr* CBNAME(int a, int b) {                              \
        wxGridCellAttr* rval = NULL;                                    \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME)) {                           \
            PyObject* ro;                                               \
            wxGridCellAttr* ptr;                                        \
            ro = m_myInst.callCallbackObj(Py_BuildValue("(ii)", a, b)); \
            if (ro) {                                                   \
                if (!SWIG_GetPtrObj(ro, (void **)&ptr, "_wxGridCellAttr_p"))    \
                    rval = ptr;                                         \
                Py_DECREF(ro);                                          \
            }                                                           \
        }                                                               \
        else                                                            \
            rval = PCLASS::CBNAME(a, b);                                \
        wxPySaveThread(doSave);                                         \
        return rval;                                                    \
    }                                                                   \
    wxGridCellAttr *base_##CBNAME(int a, int b) {                       \
        return PCLASS::CBNAME(a, b);                                    \
    }



#define PYCALLBACK__GCAINTINT(PCLASS, CBNAME)                           \
    void CBNAME(wxGridCellAttr *attr, int a, int b) {                   \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME))                             \
            m_myInst.callCallback(                                      \
                Py_BuildValue("(Oii)",                                  \
                              wxPyConstructObject((void*)attr, "_wxGridCellAttr_p"),    \
                              a, b));                                   \
        else                                                            \
            PCLASS::CBNAME(attr, a, b);                                 \
        wxPySaveThread(doSave);                                         \
    }                                                                   \
    void base_##CBNAME(wxGridCellAttr *attr, int a, int b) {            \
        PCLASS::CBNAME(attr, a, b);                                     \
    }



#define PYCALLBACK__GCAINT(PCLASS, CBNAME)                              \
    void CBNAME(wxGridCellAttr *attr, int val) {                        \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME))                             \
            m_myInst.callCallback(                                      \
                Py_BuildValue("(Oi)",                                   \
                              wxPyConstructObject((void*)attr, "_wxGridCellAttr_p"),    \
                              val));                                    \
        else                                                            \
            PCLASS::CBNAME(attr, val);                                  \
        wxPySaveThread(doSave);                                         \
    }                                                                   \
    void base_##CBNAME(wxGridCellAttr *attr, int val) {                 \
        PCLASS::CBNAME(attr, val);                                      \
    }



#define PYCALLBACK_INT__pure(CBNAME)                                    \
    int  CBNAME() {                                                     \
        bool doSave = wxPyRestoreThread();                              \
        int rval = 0;                                                   \
        if (m_myInst.findCallback(#CBNAME))                             \
            rval = m_myInst.callCallback(Py_BuildValue("()"));          \
        wxPySaveThread(doSave);                                         \
        return rval;                                                    \
    }



#define PYCALLBACK_BOOL_INTINT_pure(CBNAME)                             \
    bool CBNAME(int a, int b) {                                         \
        bool doSave = wxPyRestoreThread();                              \
        bool rval = 0;                                                  \
        if (m_myInst.findCallback(#CBNAME))                             \
            rval = m_myInst.callCallback(Py_BuildValue("(ii)",a,b));    \
        wxPySaveThread(doSave);                                         \
        return rval;                                                    \
    }



#define PYCALLBACK_STRING_INTINT_pure(CBNAME)                           \
    wxString CBNAME(int a, int b) {                                     \
        bool doSave = wxPyRestoreThread();                              \
        wxString rval;                                                  \
        if (m_myInst.findCallback(#CBNAME)) {                           \
            PyObject* ro;                                               \
            ro = m_myInst.callCallbackObj(Py_BuildValue("(ii)",a,b));   \
            if (ro) {                                                   \
                rval = PyString_AsString(PyObject_Str(ro));             \
                Py_DECREF(ro);                                          \
            }                                                           \
        }                                                               \
        wxPySaveThread(doSave);                                         \
        return rval;                                                    \
    }



#define PYCALLBACK__INTINTSTRING_pure(CBNAME)                           \
    void CBNAME(int a, int b, const wxString& c) {                      \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME))                             \
            m_myInst.callCallback(Py_BuildValue("(iis)",a,b,c.c_str()));    \
        wxPySaveThread(doSave);                                         \
    }


#define PYCALLBACK_STRING_INTINT(PCLASS, CBNAME)                        \
    wxString CBNAME(int a, int b) {                                     \
        bool doSave = wxPyRestoreThread();                              \
        wxString rval;                                                  \
        if (m_myInst.findCallback(#CBNAME)) {                           \
            PyObject* ro;                                               \
            ro = m_myInst.callCallbackObj(Py_BuildValue("(ii)",a,b));   \
            if (ro) {                                                   \
                rval = PyString_AsString(PyObject_Str(ro));             \
                Py_DECREF(ro);                                          \
            }                                                           \
        } else                                                          \
            rval = PCLASS::CBNAME(a, b);                                \
        wxPySaveThread(doSave);                                         \
        return rval;                                                    \
    }                                                                   \
    wxString base_##CBNAME(int a, int b) {                              \
        return PCLASS::CBNAME(a, b);                                    \
    }



#define PYCALLBACK_BOOL_INTINTSTRING(PCLASS, CBNAME)                    \
    bool CBNAME(int a, int b, const wxString& c)  {                     \
        bool rval;                                                      \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME))                             \
            rval = m_myInst.callCallback(Py_BuildValue("(iis)", a,b,c.c_str()));   \
        else                                                            \
            rval = PCLASS::CBNAME(a,b,c);                               \
        wxPySaveThread(doSave);                                         \
        return rval;                                                    \
    }                                                                   \
    bool base_##CBNAME(int a, int b, const wxString& c) {               \
        return PCLASS::CBNAME(a,b,c);                                   \
    }




#define PYCALLBACK_LONG_INTINT(PCLASS, CBNAME)                          \
    long CBNAME(int a, int b)  {                                        \
        long rval;                                                      \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME))                             \
            rval = m_myInst.callCallback(Py_BuildValue("(ii)", a,b));   \
        else                                                            \
            rval = PCLASS::CBNAME(a,b);                                 \
        wxPySaveThread(doSave);                                         \
        return rval;                                                    \
    }                                                                   \
    long base_##CBNAME(int a, int b) {                                  \
        return PCLASS::CBNAME(a,b);                                     \
    }



#define PYCALLBACK_BOOL_INTINT(PCLASS, CBNAME)                          \
    bool CBNAME(int a, int b)  {                                        \
        bool rval;                                                      \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME))                             \
            rval = m_myInst.callCallback(Py_BuildValue("(ii)", a,b));   \
        else                                                            \
            rval = PCLASS::CBNAME(a,b);                                 \
        wxPySaveThread(doSave);                                         \
        return rval;                                                    \
    }                                                                   \
    bool base_##CBNAME(int a, int b) {                                  \
        return PCLASS::CBNAME(a,b);                                     \
    }



#define PYCALLBACK_DOUBLE_INTINT(PCLASS, CBNAME)                        \
    double CBNAME(int a, int b) {                                       \
        bool doSave = wxPyRestoreThread();                              \
        double rval;                                                    \
        if (m_myInst.findCallback(#CBNAME)) {                           \
            PyObject* ro;                                               \
            ro = m_myInst.callCallbackObj(Py_BuildValue("(ii)",a,b));   \
            if (ro) {                                                   \
                rval = PyFloat_AsDouble(PyObject_Str(ro));              \
                Py_DECREF(ro);                                          \
            }                                                           \
        } else                                                          \
            rval = PCLASS::CBNAME(a, b);                                \
        wxPySaveThread(doSave);                                         \
        return rval;                                                    \
    }                                                                   \
    double base_##CBNAME(int a, int b) {                                \
        return PCLASS::CBNAME(a, b);                                    \
    }



#define PYCALLBACK__(PCLASS, CBNAME)                                    \
    void CBNAME()  {                                                    \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME))                             \
            m_myInst.callCallback(Py_BuildValue("()"));                 \
        else                                                            \
            PCLASS::CBNAME();                                           \
        wxPySaveThread(doSave);                                         \
    }                                                                   \
    void base_##CBNAME() {                                              \
        PCLASS::CBNAME();                                               \
    }




#define PYCALLBACK_BOOL_SIZETSIZET(PCLASS, CBNAME)                      \
    bool CBNAME(size_t a, size_t b)  {                                  \
        bool rval;                                                      \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME))                             \
            rval = m_myInst.callCallback(Py_BuildValue("(ii)", a,b));   \
        else                                                            \
            rval = PCLASS::CBNAME(a,b);                                 \
        wxPySaveThread(doSave);                                         \
        return rval;                                                    \
    }                                                                   \
    bool base_##CBNAME(size_t a, size_t b) {                            \
        return PCLASS::CBNAME(a,b);                                     \
    }



#define PYCALLBACK_BOOL_SIZET(PCLASS, CBNAME)                           \
    bool CBNAME(size_t a)  {                                            \
        bool rval;                                                      \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME))                             \
            rval = m_myInst.callCallback(Py_BuildValue("(i)", a));      \
        else                                                            \
            rval = PCLASS::CBNAME(a);                                   \
        wxPySaveThread(doSave);                                         \
        return rval;                                                    \
    }                                                                   \
    bool base_##CBNAME(size_t a) {                                      \
        return PCLASS::CBNAME(a);                                       \
    }



#define PYCALLBACK_STRING_INT(PCLASS, CBNAME)                           \
    wxString CBNAME(int a) {                                            \
        bool doSave = wxPyRestoreThread();                              \
        wxString rval;                                                  \
        if (m_myInst.findCallback(#CBNAME)) {                           \
            PyObject* ro;                                               \
            ro = m_myInst.callCallbackObj(Py_BuildValue("(i)",a));      \
            if (ro) {                                                   \
                rval = PyString_AsString(PyObject_Str(ro));             \
                Py_DECREF(ro);                                          \
            }                                                           \
        } else                                                          \
            rval = PCLASS::CBNAME(a);                                   \
        wxPySaveThread(doSave);                                         \
        return rval;                                                    \
    }                                                                   \
    wxString base_##CBNAME(int a) {                                     \
        return PCLASS::CBNAME(a);                                       \
    }



#define PYCALLBACK__INTSTRING(PCLASS, CBNAME)                           \
    void CBNAME(int a, const wxString& c)  {                            \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME))                             \
            m_myInst.callCallback(Py_BuildValue("(is)", a,c.c_str()));  \
        else                                                            \
            PCLASS::CBNAME(a,c);                                        \
        wxPySaveThread(doSave);                                         \
    }                                                                   \
    void base_##CBNAME(int a, const wxString& c) {                      \
        PCLASS::CBNAME(a,c);                                            \
    }




#define PYCALLBACK_BOOL_(PCLASS, CBNAME)                                \
    bool CBNAME()  {                                                    \
        bool rval;                                                      \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME))                             \
            rval = m_myInst.callCallback(Py_BuildValue("()"));          \
        else                                                            \
            rval = PCLASS::CBNAME();                                    \
        wxPySaveThread(doSave);                                         \
        return rval;                                                    \
    }                                                                   \
    bool base_##CBNAME() {                                              \
        return PCLASS::CBNAME();                                        \
    }



#define PYCALLBACK__SIZETINT(PCLASS, CBNAME)                            \
    void CBNAME(size_t a, int b)  {                                     \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME))                             \
            m_myInst.callCallback(Py_BuildValue("(ii)", a,b));          \
        else                                                            \
            PCLASS::CBNAME(a,b);                                        \
        wxPySaveThread(doSave);                                         \
    }                                                                   \
    void base_##CBNAME(size_t a, int b) {                               \
        PCLASS::CBNAME(a,b);                                            \
    }




#define PYCALLBACK__INTINTLONG(PCLASS, CBNAME)                          \
    void CBNAME(int a, int b, long c)  {                                \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME))                             \
            m_myInst.callCallback(Py_BuildValue("(iii)", a,b,c));       \
        else                                                            \
            PCLASS::CBNAME(a,b,c);                                      \
        wxPySaveThread(doSave);                                         \
    }                                                                   \
    void base_##CBNAME(int a, int b, long c) {                          \
        PCLASS::CBNAME(a,b,c);                                          \
    }




#define PYCALLBACK__INTINTDOUBLE(PCLASS, CBNAME)                        \
    void CBNAME(int a, int b, double c)  {                              \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME))                             \
            m_myInst.callCallback(Py_BuildValue("(iif)", a,b,c));       \
        else                                                            \
            PCLASS::CBNAME(a,b,c);                                      \
        wxPySaveThread(doSave);                                         \
    }                                                                   \
    void base_##CBNAME(int a, int b, double c) {                        \
        PCLASS::CBNAME(a,b,c);                                          \
    }



#define PYCALLBACK__INTINTBOOL(PCLASS, CBNAME)                          \
    void CBNAME(int a, int b, bool c)  {                                \
        bool doSave = wxPyRestoreThread();                              \
        if (m_myInst.findCallback(#CBNAME))                             \
            m_myInst.callCallback(Py_BuildValue("(iii)", a,b,c));       \
        else                                                            \
            PCLASS::CBNAME(a,b,c);                                      \
        wxPySaveThread(doSave);                                         \
    }                                                                   \
    void base_##CBNAME(int a, int b, bool c) {                          \
        PCLASS::CBNAME(a,b,c);                                          \
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

%readonly
wxGridCellCoords wxGridNoCellCoords;
wxRect           wxGridNoCellRect;
%readwrite


//---------------------------------------------------------------------------
// wxGridCellRenderer is an ABC, and several derived classes are available.
// Classes implemented in Python should be derived from wxPyGridCellRenderer.


class wxGridCellRenderer
{
public:
    void SetParameters(const wxString& params);

    virtual void Draw(wxGrid& grid,
                      wxGridCellAttr& attr,
                      wxDC& dc,
                      const wxRect& rect,
                      int row, int col,
                      bool isSelected) = 0;
    virtual wxSize GetBestSize(wxGrid& grid,
                               wxGridCellAttr& attr,
                               wxDC& dc,
                               int row, int col) = 0;
    virtual wxGridCellRenderer *Clone() const = 0;
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
        bool doSave = wxPyRestoreThread();
        if (m_myInst.findCallback("Draw")) {
            m_myInst.callCallback(
                Py_BuildValue("(OOOOiii)",
                              wxPyConstructObject((void*)&grid, "wxGrid"),
                              wxPyConstructObject((void*)&attr, "wxGridCellAttr"),
                              wxPyConstructObject((void*)&dc,   "wxDC"),
                              wxPyConstructObject((void*)&rect, "wxRect"),
                              row, col, isSelected));
        }
        wxPySaveThread(doSave);
    }

    wxSize GetBestSize(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc,
                       int row, int col) {
        wxSize rval;
        bool doSave = wxPyRestoreThread();
        if (m_myInst.findCallback("GetBestSize")) {
            PyObject* ro;
            wxSize*   ptr;
            ro = m_myInst.callCallbackObj(
                Py_BuildValue("(OOOii)",
                              wxPyConstructObject((void*)&grid, "wxGrid"),
                              wxPyConstructObject((void*)&attr, "wxGridCellAttr"),
                              wxPyConstructObject((void*)&dc,   "wxDC"),
                              row, col));
            if (ro) {
                if (!SWIG_GetPtrObj(ro, (void **)&ptr, "_wxSize_p"))
                    rval = *ptr;
                Py_DECREF(ro);
            }
        }
        wxPySaveThread(doSave);
        return rval;
    }


    wxGridCellRenderer *Clone() const {
        wxGridCellRenderer* rval = NULL;
        bool doSave = wxPyRestoreThread();
        if (m_myInst.findCallback("Clone")) {
            PyObject* ro;
            wxGridCellRenderer* ptr;
            ro = m_myInst.callCallbackObj(Py_BuildValue("()"));
            if (ro) {
                if (!SWIG_GetPtrObj(ro, (void **)&ptr, "_wxGridCellRenderer_p"))
                    rval = ptr;
                Py_DECREF(ro);
            }
        }
        wxPySaveThread(doSave);
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
    wxPyGridCellRenderer();
    void _setSelf(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setSelf(self, wxPyGridCellRenderer)"

    void base_SetParameters(const wxString& params);
};

//---------------------------------------------------------------------------
//  Predefined Renderers

class wxGridCellStringRenderer : public wxGridCellRenderer
{
public:
    wxGridCellStringRenderer();
};


class  wxGridCellNumberRenderer : public wxGridCellStringRenderer
{
public:
    wxGridCellNumberRenderer();
};


class  wxGridCellFloatRenderer : public wxGridCellStringRenderer
{
public:
    wxGridCellFloatRenderer(int width = -1, int precision = -1);

    int GetWidth() const;
    void SetWidth(int width);
    int GetPrecision() const;
    void SetPrecision(int precision);
};


class  wxGridCellBoolRenderer : public wxGridCellRenderer
{
public:
    wxGridCellBoolRenderer();
};



//---------------------------------------------------------------------------
// wxGridCellEditor is an ABC, and several derived classes are available.
// Classes implemented in Python should be derived from wxPyGridCellEditor.

class  wxGridCellEditor
{
public:
    bool IsCreated();
    wxControl* GetControl();
    void SetControl(wxControl* control);

    void SetParameters(const wxString& params);

    virtual void Create(wxWindow* parent,
                        wxWindowID id,
                        wxEvtHandler* evtHandler) = 0;
    virtual void BeginEdit(int row, int col, wxGrid* grid) = 0;
    virtual bool EndEdit(int row, int col, wxGrid* grid) = 0;
    virtual void Reset() = 0;
    virtual wxGridCellEditor *Clone() const = 0;

    virtual void SetSize(const wxRect& rect);
    virtual void Show(bool show, wxGridCellAttr *attr = NULL);
    virtual void PaintBackground(const wxRect& rectCell, wxGridCellAttr *attr);
    virtual void StartingKey(wxKeyEvent& event);
    virtual void StartingClick();
    virtual void HandleReturn(wxKeyEvent& event);
    virtual void Destroy();

};


// The C++ version of wxPyGridCellEditor
%{
class wxPyGridCellEditor : public wxGridCellEditor
{
public:
    wxPyGridCellEditor() : wxGridCellEditor() {}

    void Create(wxWindow* parent, wxWindowID id, wxEvtHandler* evtHandler) {
        bool doSave = wxPyRestoreThread();
        if (m_myInst.findCallback("Create")) {
            m_myInst.callCallback(
                Py_BuildValue("(OiO)",
                              wxPyConstructObject((void*)parent, "wxWindow"),
                              id,
                              wxPyConstructObject((void*)evtHandler, "wxEvtHandler")));
        }
        wxPySaveThread(doSave);
    }


    void BeginEdit(int row, int col, wxGrid* grid) {
        bool doSave = wxPyRestoreThread();
        if (m_myInst.findCallback("BeginEdit")) {
            m_myInst.callCallback(
                Py_BuildValue("(iiO)", row, col,
                              wxPyConstructObject((void*)grid, "wxGrid")));
        }
        wxPySaveThread(doSave);
    }


    bool EndEdit(int row, int col, wxGrid* grid) {
        bool rv = FALSE;
        bool doSave = wxPyRestoreThread();
        if (m_myInst.findCallback("EndEdit")) {
            rv = m_myInst.callCallback(
                Py_BuildValue("(iiO)", row, col,
                              wxPyConstructObject((void*)grid, "wxGrid")));
        }
        wxPySaveThread(doSave);
        return rv;
    }


    wxGridCellEditor *Clone() const {
        wxGridCellEditor* rval = NULL;
        bool doSave = wxPyRestoreThread();
        if (m_myInst.findCallback("Clone")) {
            PyObject* ro;
            wxGridCellEditor* ptr;
            ro = m_myInst.callCallbackObj(Py_BuildValue("()"));
            if (ro) {
                if (!SWIG_GetPtrObj(ro, (void **)&ptr, "_wxGridCellEditor_p"))
                    rval = ptr;
                Py_DECREF(ro);
            }
        }
        wxPySaveThread(doSave);
        return rval;
    }


    void Show(bool show, wxGridCellAttr *attr) {
        bool doSave = wxPyRestoreThread();
        if (m_myInst.findCallback("Show"))
            m_myInst.callCallback(
                Py_BuildValue("(iO)", show,
                              wxPyConstructObject((void*)attr, "_wxGridCellAttr_p")));
        else
            wxGridCellEditor::Show(show, attr);
        wxPySaveThread(doSave);
    }
    void base_Show(bool show, wxGridCellAttr *attr) {
        wxGridCellEditor::Show(show, attr);
    }


    void PaintBackground(const wxRect& rectCell, wxGridCellAttr *attr) {
        bool doSave = wxPyRestoreThread();
        if (m_myInst.findCallback("PaintBackground"))
            m_myInst.callCallback(
                Py_BuildValue("(OO)",
                              wxPyConstructObject((void*)&rectCell, "_wxRect_p"),
                              wxPyConstructObject((void*)attr, "_wxGridCellAttr_p")));
        else
            wxGridCellEditor::PaintBackground(rectCell, attr);
        wxPySaveThread(doSave);
    }
    void base_PaintBackground(const wxRect& rectCell, wxGridCellAttr *attr) {
        wxGridCellEditor::PaintBackground(rectCell, attr);
    }


    DEC_PYCALLBACK___pure(Reset);
    DEC_PYCALLBACK__constany(SetSize, wxRect);
    DEC_PYCALLBACK__any(StartingKey, wxKeyEvent);
    DEC_PYCALLBACK__any(HandleReturn, wxKeyEvent);
    DEC_PYCALLBACK__(StartingClick);
    DEC_PYCALLBACK__(Destroy);
    DEC_PYCALLBACK__STRING(SetParameters);

    PYPRIVATE;
};


IMP_PYCALLBACK__STRING( wxPyGridCellEditor, wxGridCellEditor, SetParameters);
IMP_PYCALLBACK___pure(wxPyGridCellEditor, wxGridCellEditor, Reset);
IMP_PYCALLBACK__constany(wxPyGridCellEditor, wxGridCellEditor, SetSize, wxRect);
IMP_PYCALLBACK__any(wxPyGridCellEditor, wxGridCellEditor, StartingKey, wxKeyEvent);
IMP_PYCALLBACK__any(wxPyGridCellEditor, wxGridCellEditor, HandleReturn, wxKeyEvent);
IMP_PYCALLBACK__(wxPyGridCellEditor, wxGridCellEditor, StartingClick);
IMP_PYCALLBACK__(wxPyGridCellEditor, wxGridCellEditor, Destroy);

%}


// Let SWIG know about it so it can create the Python version
class wxPyGridCellEditor : public wxGridCellEditor {
public:
    wxPyGridCellEditor();
    void _setSelf(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setSelf(self, wxPyGridCellEditor)"

    void base_SetSize(const wxRect& rect);
    void base_Show(bool show, wxGridCellAttr *attr = NULL);
    void base_PaintBackground(const wxRect& rectCell, wxGridCellAttr *attr);
    void base_StartingKey(wxKeyEvent& event);
    void base_StartingClick();
    void base_HandleReturn(wxKeyEvent& event);
    void base_Destroy();
    void base_SetParameters(const wxString& params);
};

//---------------------------------------------------------------------------
//  Predefined Editors

class wxGridCellTextEditor : public wxGridCellEditor
{
public:
    wxGridCellTextEditor();
};


class wxGridCellNumberEditor : public wxGridCellTextEditor
{
public:
    wxGridCellNumberEditor(int min = -1, int max = -1);
};


class wxGridCellFloatEditor : public wxGridCellTextEditor
{
public:
    wxGridCellFloatEditor();
};


class wxGridCellBoolEditor : public wxGridCellEditor
{
public:
    wxGridCellBoolEditor();
};


class wxGridCellChoiceEditor : public wxGridCellEditor
{
public:
    wxGridCellChoiceEditor(int LCOUNT = 0,
                           const wxString* choices = NULL,
                           bool allowOthers = FALSE);
};


//---------------------------------------------------------------------------


class wxGridCellAttr
{
public:
    wxGridCellAttr();

    wxGridCellAttr *Clone() const;
    void IncRef();
    void DecRef();
    void SetTextColour(const wxColour& colText);
    void SetBackgroundColour(const wxColour& colBack);
    void SetFont(const wxFont& font);
    void SetAlignment(int hAlign, int vAlign);
    void SetReadOnly(bool isReadOnly = TRUE);

    void SetRenderer(wxGridCellRenderer *renderer);
    void SetEditor(wxGridCellEditor* editor);

    bool HasTextColour() const;
    bool HasBackgroundColour() const;
    bool HasFont() const;
    bool HasAlignment() const;
    bool HasRenderer() const;
    bool HasEditor() const;

    const wxColour& GetTextColour() const;
    const wxColour& GetBackgroundColour() const;
    const wxFont& GetFont() const;
    void GetAlignment(int *OUTPUT, int *OUTPUT) const;
    wxGridCellRenderer *GetRenderer(wxGrid* grid, int row, int col) const;
    wxGridCellEditor *GetEditor(wxGrid* grid, int row, int col) const;

    bool IsReadOnly() const;
    void SetDefAttr(wxGridCellAttr* defAttr);
};

//---------------------------------------------------------------------------

class wxGridCellAttrProvider
{
public:
    wxGridCellAttrProvider();
    // ???? virtual ~wxGridCellAttrProvider();

    wxGridCellAttr *GetAttr(int row, int col) const;
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

    PYCALLBACK_GCA_INTINT(wxGridCellAttrProvider, GetAttr);
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
    wxPyGridCellAttrProvider();
    void _setSelf(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setSelf(self, wxPyGridCellAttrProvider)"

    wxGridCellAttr *base_GetAttr(int row, int col);
    void base_SetAttr(wxGridCellAttr *attr, int row, int col);
    void base_SetRowAttr(wxGridCellAttr *attr, int row);
    void base_SetColAttr(wxGridCellAttr *attr, int col);
};


//---------------------------------------------------------------------------
// Grid Table Base class and Python aware version



class wxGridTableBase
{
public:
    // wxGridTableBase();   This is an ABC
    //~wxGridTableBase();

    void SetAttrProvider(wxGridCellAttrProvider *attrProvider);
    wxGridCellAttrProvider *GetAttrProvider() const;
    void SetView( wxGrid *grid );
    wxGrid * GetView() const;


    // pure virtuals
    virtual int GetNumberRows() = 0;
    virtual int GetNumberCols() = 0;
    virtual bool IsEmptyCell( int row, int col ) = 0;
    virtual wxString GetValue( int row, int col ) = 0;
    virtual void SetValue( int row, int col, const wxString& value ) = 0;

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

    virtual wxGridCellAttr *GetAttr( int row, int col );
    virtual void SetAttr(wxGridCellAttr* attr, int row, int col);
    virtual void SetRowAttr(wxGridCellAttr *attr, int row);
    virtual void SetColAttr(wxGridCellAttr *attr, int col);

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
    PYCALLBACK_GCA_INTINT(wxGridTableBase, GetAttr);
    PYCALLBACK__GCAINTINT(wxGridTableBase, SetAttr);
    PYCALLBACK__GCAINT(wxGridTableBase, SetRowAttr);
    PYCALLBACK__GCAINT(wxGridTableBase, SetColAttr);



    wxString GetValue(int row, int col) {
        bool doSave = wxPyRestoreThread();
        wxString rval;
        if (m_myInst.findCallback("GetValue")) {
            PyObject* ro;
            ro = m_myInst.callCallbackObj(Py_BuildValue("(ii)",row,col));
            if (ro) {
                rval = PyString_AsString(PyObject_Str(ro));
                Py_DECREF(ro);
            }
        }
        wxPySaveThread(doSave);
        return rval;
    }

    void SetValue(int row, int col, const wxString& val) {
        bool doSave = wxPyRestoreThread();
        if (m_myInst.findCallback("SetValue"))
            m_myInst.callCallback(Py_BuildValue("(iis)",row,col,val.c_str()));
        wxPySaveThread(doSave);
    }


    // Map the Get/Set methods for the standard non-string types to
    // the GetValue and SetValue python methods.
    long GetValueAsLong( int row, int col ) {
        long rval = 0;
        bool doSave = wxPyRestoreThread();
        if (m_myInst.findCallback("GetValue")) {
            PyObject* ro;
            PyObject* num;
            ro = m_myInst.callCallbackObj(Py_BuildValue("(ii)", row, col));
            if (ro && PyNumber_Check(ro)) {
                num = PyNumber_Int(ro);
                if (num) {
                    rval = PyInt_AsLong(num);
                    Py_DECREF(num);
                }
                Py_DECREF(ro);
            }
        }
        wxPySaveThread(doSave);
        return rval;
    }

    double GetValueAsDouble( int row, int col ) {
        double rval = 0.0;
        bool doSave = wxPyRestoreThread();
        if (m_myInst.findCallback("GetValue")) {
            PyObject* ro;
            PyObject* num;
            ro = m_myInst.callCallbackObj(Py_BuildValue("(ii)", row, col));
            if (ro && PyNumber_Check(ro)) {
                num = PyNumber_Float(ro);
                if (num) {
                    rval = PyFloat_AsDouble(num);
                    Py_DECREF(num);
                }
                Py_DECREF(ro);
            }
        }
        wxPySaveThread(doSave);
        return rval;
    }

    bool GetValueAsBool( int row, int col ) {
        return (bool)GetValueAsLong(row, col);
    }

    void SetValueAsLong( int row, int col, long value ) {
        bool doSave = wxPyRestoreThread();
        if (m_myInst.findCallback("SetValue")) {
            m_myInst.callCallback(Py_BuildValue("(iii)", row, col, value));
        }
        wxPySaveThread(doSave);
    }

    void SetValueAsDouble( int row, int col, double value ) {
        bool doSave = wxPyRestoreThread();
        if (m_myInst.findCallback("SetValue")) {
            m_myInst.callCallback(Py_BuildValue("(iid)", row, col, value));
        }
        wxPySaveThread(doSave);
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
    wxPyGridTableBase();
    void _setSelf(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setSelf(self, wxPyGridTableBase)"

    %addmethods { void Destroy() { delete self; } }

    wxString base_GetTypeName( int row, int col );
    bool base_CanGetValueAs( int row, int col, const wxString& typeName );
    bool base_CanSetValueAs( int row, int col, const wxString& typeName );
    void base_Clear();
    bool base_InsertRows( size_t pos = 0, size_t numRows = 1 );
    bool base_AppendRows( size_t numRows = 1 );
    bool base_DeleteRows( size_t pos = 0, size_t numRows = 1 );
    bool base_InsertCols( size_t pos = 0, size_t numCols = 1 );
    bool base_AppendCols( size_t numCols = 1 );
    bool base_DeleteCols( size_t pos = 0, size_t numCols = 1 );
    wxString base_GetRowLabelValue( int row );
    wxString base_GetColLabelValue( int col );
    void base_SetRowLabelValue( int row, const wxString& value );
    void base_SetColLabelValue( int col, const wxString& value );
    bool base_CanHaveAttributes();
    wxGridCellAttr *base_GetAttr( int row, int col );
    void base_SetAttr(wxGridCellAttr* attr, int row, int col);
    void base_SetRowAttr(wxGridCellAttr *attr, int row);
    void base_SetColAttr(wxGridCellAttr *attr, int col);
};


//---------------------------------------------------------------------------
// Predefined Tables

class  wxGridStringTable : public wxGridTableBase
{
public:
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
};


//---------------------------------------------------------------------------

class wxGridCellCoords
{
public:
    wxGridCellCoords( int r=-1, int c=-1 );
    ~wxGridCellCoords();

    int GetRow() const { return m_row; }
    void SetRow( int n ) { m_row = n; }
    int GetCol() const { return m_col; }
    void SetCol( int n ) { m_col = n; }
    void Set( int row, int col ) { m_row = row; m_col = col; }

    %addmethods {
        PyObject* asTuple() {
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, PyInt_FromLong(self->GetRow()));
            PyTuple_SET_ITEM(tup, 1, PyInt_FromLong(self->GetCol()));
            return tup;
        }

        int __cmp__( const wxGridCellCoords& other ) {
            return *self != other;
        }
    }
    %pragma(python) addtoclass = "def __str__(self): return str(self.asTuple())"
    %pragma(python) addtoclass = "def __repr__(self): return str(self.asTuple())"
};

// Typemap to allow conversion of sequence objects to wxGridCellCoords...
%typemap(python,in) wxGridCellCoords& (wxGridCellCoords temp) {
    $target = &temp;
    if (! wxGridCellCoords_helper($source, &$target))
        return NULL;
}

// ...and here is the associated helper.
%{
bool wxGridCellCoords_helper(PyObject* source, wxGridCellCoords** obj) {

    // If source is an object instance then it may already be the right type
    if (PyInstance_Check(source)) {
        wxGridCellCoords* ptr;
        if (SWIG_GetPtrObj(source, (void **)&ptr, "_wxGridCellCoords_p"))
            goto error;
        *obj = ptr;
        return TRUE;
    }
    // otherwise a 2-tuple of integers is expected
    else if (PySequence_Check(source) && PyObject_Length(source) == 2) {
        PyObject* o1 = PySequence_GetItem(source, 0);
        PyObject* o2 = PySequence_GetItem(source, 1);
        **obj = wxGridCellCoords(PyInt_AsLong(o1), PyInt_AsLong(o2));
        return TRUE;
    }

 error:
    PyErr_SetString(PyExc_TypeError, "Expected a 2-tuple of integers or a wxGridCellCoords object.");
    return FALSE;
}
%}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// The grid itself


// Fool SWIG into treating this enum as an int
typedef int WXGRIDSELECTIONMODES;

// but let the C++ code know what it really is.
%{
typedef wxGrid::wxGridSelectionModes WXGRIDSELECTIONMODES;
%}



class wxGrid : public wxScrolledWindow
{
public:
    wxGrid( wxWindow *parent,
            wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxWANTS_CHARS,
            const char* name = wxPanelNameStr );

    //~wxGrid();

    enum wxGridSelectionModes {wxGridSelectCells,
                               wxGridSelectRows,
                               wxGridSelectColumns};

    bool CreateGrid( int numRows, int numCols,
                     WXGRIDSELECTIONMODES selmode = wxGrid::wxGridSelectCells );
    void SetSelectionMode(WXGRIDSELECTIONMODES selmode);


    // ------ grid dimensions
    //
    int      GetNumberRows();
    int      GetNumberCols();


    // ------ display update functions
    //
    void CalcRowLabelsExposed( wxRegion& reg );

    void CalcColLabelsExposed( wxRegion& reg );
    void CalcCellsExposed( wxRegion& reg );


#ifdef NOTNEEDED // ????
    // ------ event handlers
    //
    void ProcessRowLabelMouseEvent( wxMouseEvent& event );
    void ProcessColLabelMouseEvent( wxMouseEvent& event );
    void ProcessCornerLabelMouseEvent( wxMouseEvent& event );
    void ProcessGridCellMouseEvent( wxMouseEvent& event );
    void DoEndDragResizeRow();
    void DoEndDragResizeCol();
#endif

    bool ProcessTableMessage( wxGridTableMessage& );


    wxGridTableBase * GetTable() const;
    bool SetTable( wxGridTableBase *table, bool takeOwnership=FALSE,
                   WXGRIDSELECTIONMODES selmode =
                   wxGrid::wxGridSelectCells );

    void ClearGrid();
    bool InsertRows( int pos = 0, int numRows = 1, bool updateLabels=TRUE );
    bool AppendRows( int numRows = 1, bool updateLabels=TRUE );
    bool DeleteRows( int pos = 0, int numRows = 1, bool updateLabels=TRUE );
    bool InsertCols( int pos = 0, int numCols = 1, bool updateLabels=TRUE );
    bool AppendCols( int numCols = 1, bool updateLabels=TRUE );
    bool DeleteCols( int pos = 0, int numCols = 1, bool updateLabels=TRUE );

#ifdef NOTNEEDED // ????
    void DrawGridCellArea( wxDC& dc );
    void DrawGridSpace( wxDC& dc );
    void DrawCellBorder( wxDC& dc, const wxGridCellCoords& );
    void DrawAllGridLines( wxDC& dc, const wxRegion & reg );
    void DrawCell( wxDC& dc, const wxGridCellCoords& );
    void DrawHighlight(wxDC& dc);
#endif

    // this function is called when the current cell highlight must be redrawn
    // and may be overridden by the user
    virtual void DrawCellHighlight( wxDC& dc, const wxGridCellAttr *attr );

#ifdef NOTNEEDED // ????
    void DrawRowLabels( wxDC& dc );
    void DrawRowLabel( wxDC& dc, int row );

    void DrawColLabels( wxDC& dc );
    void DrawColLabel( wxDC& dc, int col );
#endif

    // ------ Cell text drawing functions
    //
    void DrawTextRectangle( wxDC& dc, const wxString&, const wxRect&,
                            int horizontalAlignment = wxLEFT,
                            int verticalAlignment = wxTOP );

    // Split a string containing newline chararcters into an array of
    // strings and return the number of lines
    //
    void StringToLines( const wxString& value, wxArrayString& lines );

    void GetTextBoxSize( wxDC& dc,
                         wxArrayString& lines,
                         long *width, long *height );


    // ------
    // Code that does a lot of grid modification can be enclosed
    // between BeginBatch() and EndBatch() calls to avoid screen
    // flicker
    //
    void     BeginBatch();
    void     EndBatch();
    int      GetBatchCount();


    // ------ edit control functions
    //
    bool IsEditable() { return m_editable; }
    void EnableEditing( bool edit );

    void EnableCellEditControl( bool enable = TRUE );
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
    %addmethods {
        %new wxGridCellCoords* XYToCell(int x, int y) {
            wxGridCellCoords rv;
            self->XYToCell(x, y, rv);
            return new wxGridCellCoords(rv);
        }
    }

    int  YToRow( int y );
    int  XToCol( int x );

    int  YToEdgeOfRow( int y );
    int  XToEdgeOfCol( int x );

    wxRect CellToRect( int row, int col );
    // TODO: ??? wxRect CellToRect( const wxGridCellCoords& coords );


    int  GetGridCursorRow();
    int  GetGridCursorCol();

    // check to see if a cell is either wholly visible (the default arg) or
    // at least partially visible in the grid window
    //
    bool IsVisible( int row, int col, bool wholeCellVisible = TRUE );
    // TODO: ??? bool IsVisible( const wxGridCellCoords& coords, bool wholeCellVisible = TRUE );
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
    void     GetRowLabelAlignment( int *OUTPUT, int *OUTPUT );
    void     GetColLabelAlignment( int *OUTPUT, int *OUTPUT );
    wxString GetRowLabelValue( int row );
    wxString GetColLabelValue( int col );
    wxColour GetGridLineColour();

    void     SetRowLabelSize( int width );
    void     SetColLabelSize( int height );
    void     SetLabelBackgroundColour( const wxColour& );
    void     SetLabelTextColour( const wxColour& );
    void     SetLabelFont( const wxFont& );
    void     SetRowLabelAlignment( int horiz, int vert );
    void     SetColLabelAlignment( int horiz, int vert );
    void     SetRowLabelValue( int row, const wxString& );
    void     SetColLabelValue( int col, const wxString& );
    void     SetGridLineColour( const wxColour& );

    void     EnableDragRowSize( bool enable = TRUE );
    void     DisableDragRowSize();
    bool     CanDragRowSize();
    void     EnableDragColSize( bool enable = TRUE );
    void     DisableDragColSize();
    bool     CanDragColSize();
    void     EnableDragGridSize(bool enable = TRUE);
    void     DisableDragGridSize();
    bool     CanDragGridSize();

    // this sets the specified attribute for all cells in this row/col
    void     SetRowAttr(int row, wxGridCellAttr *attr);
    void     SetColAttr(int col, wxGridCellAttr *attr);

    // shortcuts for setting the column parameters

    // set the format for the data in the column: default is string
    void     SetColFormatBool(int col);
    void     SetColFormatNumber(int col);
    void     SetColFormatFloat(int col, int width = -1, int precision = -1);
    void     SetColFormatCustom(int col, const wxString& typeName);

    void     EnableGridLines( bool enable = TRUE );
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
    void     GetDefaultCellAlignment( int *horiz, int *vert );
    void     GetCellAlignment( int row, int col, int *horiz, int *vert );

    void     SetDefaultRowSize( int height, bool resizeExistingRows = FALSE );
    void     SetRowSize( int row, int height );
    void     SetDefaultColSize( int width, bool resizeExistingCols = FALSE );

    void     SetColSize( int col, int width );

    // automatically size the column or row to fit to its contents, if
    // setAsMin is TRUE, this optimal width will also be set as minimal width
    // for this column
    void     AutoSizeColumn( int col, bool setAsMin = TRUE );
    void     AutoSizeRow( int row, bool setAsMin = TRUE );


    // auto size all columns (very ineffective for big grids!)
    void     AutoSizeColumns( bool setAsMin = TRUE );
    void     AutoSizeRows( bool setAsMin = TRUE );

    // auto size the grid, that is make the columns/rows of the "right" size
    // and also set the grid size to just fit its contents
    void     AutoSize();

    // column won't be resized to be lesser width - this must be called during
    // the grid creation because it won't resize the column if it's already
    // narrower than the minimal width
    void     SetColMinimalWidth( int col, int width );
    void     SetRowMinimalHeight( int row, int width );

    void     SetDefaultCellBackgroundColour( const wxColour& );
    void     SetCellBackgroundColour( int row, int col, const wxColour& );
    void     SetDefaultCellTextColour( const wxColour& );

    void     SetCellTextColour( int row, int col, const wxColour& );
    void     SetDefaultCellFont( const wxFont& );
    void     SetCellFont( int row, int col, const wxFont& );
    void     SetDefaultCellAlignment( int horiz, int vert );
    void     SetCellAlignment( int row, int col, int horiz, int vert );

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

    // returns TRUE if the cell can't be edited
    bool IsReadOnly(int row, int col) const;

    // make the cell editable/readonly
    void SetReadOnly(int row, int col, bool isReadOnly = TRUE);

    // ------ selections of blocks of cells
    //
    void SelectRow( int row, bool addToSelected = FALSE );
    void SelectCol( int col, bool addToSelected = FALSE );

    void SelectBlock( int topRow, int leftCol, int bottomRow, int rightCol );
    // TODO: ??? void SelectBlock( const wxGridCellCoords& topLeft,
    // TODO: ???                   const wxGridCellCoords& bottomRight )

    void SelectAll();
    bool IsSelection();
    void ClearSelection();
    bool IsInSelection( int row, int col );
    // TODO: ??? bool IsInSelection( const wxGridCellCoords& coords )


    // This function returns the rectangle that encloses the block of cells
    // limited by TopLeft and BottomRight cell in device coords and clipped
    //  to the client size of the grid window.
    //
    wxRect BlockToDeviceRect( const wxGridCellCoords & topLeft,
                              const wxGridCellCoords & bottomRight );

    // This function returns the rectangle that encloses the selected cells
    // in device coords and clipped to the client size of the grid window.
    //
    wxRect SelectionToDeviceRect();

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
};


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Grid events and stuff



class wxGridEvent : public wxNotifyEvent
{
public:
    wxGridEvent(int id, wxEventType type, wxGrid* obj,
                int row=-1, int col=-1, int x=-1, int y=-1, bool sel = TRUE,
                bool control=FALSE, bool shift=FALSE, bool alt=FALSE, bool meta=FALSE);

    virtual int GetRow();
    virtual int GetCol();
    wxPoint     GetPosition();
    bool        Selecting();
    bool        ControlDown();
    bool        MetaDown();
    bool        ShiftDown();
    bool        AltDown();

};


class  wxGridSizeEvent : public wxNotifyEvent
{
public:
    wxGridSizeEvent(int id, wxEventType type, wxGrid* obj,
                int rowOrCol=-1, int x=-1, int y=-1,
                bool control=FALSE, bool shift=FALSE, bool alt=FALSE, bool meta=FALSE);

    int         GetRowOrCol();
    wxPoint     GetPosition();
    bool        ControlDown();
    bool        MetaDown();
    bool        ShiftDown();
    bool        AltDown();

};


class wxGridRangeSelectEvent : public wxNotifyEvent
{
public:
    wxGridRangeSelectEvent(int id, wxEventType type, wxGrid* obj,
                           const wxGridCellCoords& topLeft,
                           const wxGridCellCoords& bottomRight,
                           bool sel = TRUE,
                           bool control=FALSE, bool shift=FALSE,
                           bool alt=FALSE, bool meta=FALSE);

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
};

enum {
    wxEVT_GRID_CELL_LEFT_CLICK,
    wxEVT_GRID_CELL_RIGHT_CLICK,
    wxEVT_GRID_CELL_LEFT_DCLICK,
    wxEVT_GRID_CELL_RIGHT_DCLICK,
    wxEVT_GRID_LABEL_LEFT_CLICK,
    wxEVT_GRID_LABEL_RIGHT_CLICK,
    wxEVT_GRID_LABEL_LEFT_DCLICK,
    wxEVT_GRID_LABEL_RIGHT_DCLICK,
    wxEVT_GRID_ROW_SIZE,
    wxEVT_GRID_COL_SIZE,
    wxEVT_GRID_RANGE_SELECT,
    wxEVT_GRID_CELL_CHANGE,
    wxEVT_GRID_SELECT_CELL,
    wxEVT_GRID_EDITOR_SHOWN,
    wxEVT_GRID_EDITOR_HIDDEN,
};



%pragma(python) code = "
def EVT_GRID_CELL_LEFT_CLICK(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_CELL_LEFT_CLICK, fn)

def EVT_GRID_CELL_RIGHT_CLICK(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_CELL_RIGHT_CLICK, fn)

def EVT_GRID_CELL_LEFT_DCLICK(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_CELL_LEFT_DCLICK, fn)

def EVT_GRID_CELL_RIGHT_DCLICK(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_CELL_RIGHT_DCLICK, fn)

def EVT_GRID_LABEL_LEFT_CLICK(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_LABEL_LEFT_CLICK, fn)

def EVT_GRID_LABEL_RIGHT_CLICK(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_LABEL_RIGHT_CLICK, fn)

def EVT_GRID_LABEL_LEFT_DCLICK(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_LABEL_LEFT_DCLICK, fn)

def EVT_GRID_LABEL_RIGHT_DCLICK(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_LABEL_RIGHT_DCLICK, fn)

def EVT_GRID_ROW_SIZE(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_ROW_SIZE, fn)

def EVT_GRID_COL_SIZE(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_COL_SIZE, fn)

def EVT_GRID_RANGE_SELECT(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_RANGE_SELECT, fn)

def EVT_GRID_CELL_CHANGE(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_CELL_CHANGE, fn)

def EVT_GRID_SELECT_CELL(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_SELECT_CELL, fn)

def EVT_GRID_EDITOR_SHOWN(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_EDITOR_SHOWN, fn)

def EVT_GRID_EDITOR_HIDDEN(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_EDITOR_HIDDEN, fn)

"

//---------------------------------------------------------------------------

%pragma(python) include="_gridextras.py";

//---------------------------------------------------------------------------


#endif
