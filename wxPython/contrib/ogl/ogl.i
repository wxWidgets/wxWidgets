/////////////////////////////////////////////////////////////////////////////
// Name:        ogl.i
// Purpose:     SWIG definitions for the wxWindows Object Graphics Library
//
// Author:      Robin Dunn
//
// Created:     30-June-1999
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


%module ogl

%{
#include "wxPython.h"
#include "oglhelpers.h"
%}

//---------------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

%extern wx.i
%import windows.i
%extern _defs.i
%extern misc.i
%extern gdi.i

%include _ogldefs.i

%import oglbasic.i
%import oglshapes.i
%import oglshapes2.i
%import oglcanvas.i


%pragma(python) code = "import wx"

//---------------------------------------------------------------------------


enum {
    KEY_SHIFT,
    KEY_CTRL,
    ARROW_NONE,
    ARROW_END,
    ARROW_BOTH,
    ARROW_MIDDLE,
    ARROW_START,
    ARROW_HOLLOW_CIRCLE,
    ARROW_FILLED_CIRCLE,
    ARROW_ARROW,
    ARROW_SINGLE_OBLIQUE,
    ARROW_DOUBLE_OBLIQUE,
    ARROW_METAFILE,
    ARROW_POSITION_END,
    ARROW_POSITION_START,
    CONTROL_POINT_VERTICAL,
    CONTROL_POINT_HORIZONTAL,
    CONTROL_POINT_DIAGONAL,
    CONTROL_POINT_ENDPOINT_TO,
    CONTROL_POINT_ENDPOINT_FROM,
    CONTROL_POINT_LINE,
    FORMAT_NONE,
    FORMAT_CENTRE_HORIZ,
    FORMAT_CENTRE_VERT,
    FORMAT_SIZE_TO_CONTENTS,
    LINE_ALIGNMENT_HORIZ,
    LINE_ALIGNMENT_VERT,
    LINE_ALIGNMENT_TO_NEXT_HANDLE,
    LINE_ALIGNMENT_NONE,
    SHADOW_NONE,
    SHADOW_LEFT,
    SHADOW_RIGHT,
//      SHAPE_BASIC,
//      SHAPE_RECTANGLE,
//      SHAPE_ELLIPSE,
//      SHAPE_POLYGON,
//      SHAPE_CIRCLE,
//      SHAPE_LINE,
//      SHAPE_DIVIDED_RECTANGLE,
//      SHAPE_COMPOSITE,
//      SHAPE_CONTROL_POINT,
//      SHAPE_DRAWN,
//      SHAPE_DIVISION,
//      SHAPE_LABEL_OBJECT,
//      SHAPE_BITMAP,
//      SHAPE_DIVIDED_OBJECT_CONTROL_POINT,
//      OBJECT_REGION,
    OP_CLICK_LEFT,
    OP_CLICK_RIGHT,
    OP_DRAG_LEFT,
    OP_DRAG_RIGHT,
    OP_ALL,
    ATTACHMENT_MODE_NONE,
    ATTACHMENT_MODE_EDGE,
    ATTACHMENT_MODE_BRANCHING,
    BRANCHING_ATTACHMENT_NORMAL,
    BRANCHING_ATTACHMENT_BLOB,

    gyCONSTRAINT_CENTRED_VERTICALLY,
    gyCONSTRAINT_CENTRED_HORIZONTALLY,
    gyCONSTRAINT_CENTRED_BOTH,
    gyCONSTRAINT_LEFT_OF,
    gyCONSTRAINT_RIGHT_OF,
    gyCONSTRAINT_ABOVE,
    gyCONSTRAINT_BELOW,
    gyCONSTRAINT_ALIGNED_TOP,
    gyCONSTRAINT_ALIGNED_BOTTOM,
    gyCONSTRAINT_ALIGNED_LEFT,
    gyCONSTRAINT_ALIGNED_RIGHT,
    gyCONSTRAINT_MIDALIGNED_TOP,
    gyCONSTRAINT_MIDALIGNED_BOTTOM,
    gyCONSTRAINT_MIDALIGNED_LEFT,
    gyCONSTRAINT_MIDALIGNED_RIGHT,

    DIVISION_SIDE_NONE,
    DIVISION_SIDE_LEFT,
    DIVISION_SIDE_TOP,
    DIVISION_SIDE_RIGHT,
    DIVISION_SIDE_BOTTOM,

};



//---------------------------------------------------------------------------

void wxOGLInitialize();
void wxOGLCleanUp();


%{
//---------------------------------------------------------------------------
// This one will work for any class for the VERY generic cases, but beyond that
// the helper needs to know more about the type.

wxList* wxPy_wxListHelper(PyObject* pyList, char* className) {
    wxPyBeginBlockThreads();
    if (!PyList_Check(pyList)) {
        PyErr_SetString(PyExc_TypeError, "Expected a list object.");
        wxPyEndBlockThreads();
        return NULL;
    }
    int count = PyList_Size(pyList);
    wxList* list = new wxList;
    if (! list) {
        PyErr_SetString(PyExc_MemoryError, "Unable to allocate wxList object");
        wxPyEndBlockThreads();
        return NULL;
    }
    for (int x=0; x<count; x++) {
        PyObject* pyo = PyList_GetItem(pyList, x);
        wxObject* wxo = NULL;

        if (SWIG_GetPtrObj(pyo, (void **)&wxo, className)) {
            char errmsg[1024];
            sprintf(errmsg, "Type error, expected list of %s objects", className);
            PyErr_SetString(PyExc_TypeError, errmsg);
            wxPyEndBlockThreads();
            return NULL;
        }
        list->Append(wxo);
    }
    wxPyEndBlockThreads();
    return list;
}

//---------------------------------------------------------------------------

wxList* wxPy_wxRealPoint_ListHelper(PyObject* pyList) {
    wxPyBeginBlockThreads();
    if (!PyList_Check(pyList)) {
        PyErr_SetString(PyExc_TypeError, "Expected a list object.");
        wxPyEndBlockThreads();
        return NULL;
    }
    int count = PyList_Size(pyList);
    wxList* list = new wxList;
    if (! list) {
        PyErr_SetString(PyExc_MemoryError, "Unable to allocate wxList object");
        wxPyEndBlockThreads();
        return NULL;
    }
    for (int x=0; x<count; x++) {
        PyObject* pyo = PyList_GetItem(pyList, x);

        if (PyTuple_Check(pyo)) {
            PyObject* o1 = PyNumber_Float(PyTuple_GetItem(pyo, 0));
            PyObject* o2 = PyNumber_Float(PyTuple_GetItem(pyo, 1));

            double val1 = (o1 ? PyFloat_AsDouble(o1) : 0.0);
            double val2 = (o2 ? PyFloat_AsDouble(o2) : 0.0);

            list->Append((wxObject*) new wxRealPoint(val1, val2));

        } else {
            wxRealPoint* wxo = NULL;
            if (SWIG_GetPtrObj(pyo, (void **)&wxo, "_wxRealPoint_p")) {
                PyErr_SetString(PyExc_TypeError, "Type error, expected list of wxRealPoint objects or 2-tuples");
                wxPyEndBlockThreads();
                return NULL;
            }
            list->Append((wxObject*) new wxRealPoint(*wxo));
        }
    }
    wxPyEndBlockThreads();
    return list;
}

//---------------------------------------------------------------------------

PyObject*  wxPyMake_wxShapeEvtHandler(wxShapeEvtHandler* source) {
    PyObject* target = NULL;

    if (source && wxIsKindOf(source, wxShapeEvtHandler)) {
        // If it's derived from wxShapeEvtHandler then there may
        // already be a pointer to a Python object that we can use
        // in the OOR data.
        wxShapeEvtHandler* seh = (wxShapeEvtHandler*)source;
        wxPyOORClientData* data = (wxPyOORClientData*)seh->GetClientObject();
        if (data) {
            target = data->m_obj;
            Py_INCREF(target);
        }
    }
    if (! target) {
        target = wxPyMake_wxObject2(source, FALSE);
        if (target != Py_None)
            ((wxShapeEvtHandler*)source)->SetClientObject(new wxPyOORClientData(target));
    }
    return target;
}

//---------------------------------------------------------------------------

PyObject* wxPy_ConvertShapeList(wxListBase* list, const char* className) {
    PyObject*   pyList;
    PyObject*   pyObj;
    wxObject*   wxObj;
    wxNode*     node = list->First();

    wxPyBeginBlockThreads();
    pyList = PyList_New(0);
    while (node) {
        wxObj = node->Data();
        pyObj = wxPyMake_wxShapeEvtHandler((wxShapeEvtHandler*)wxObj);
        PyList_Append(pyList, pyObj);
        node = node->Next();
    }
    wxPyEndBlockThreads();
    return pyList;
}


//---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxPyShapeCanvas, wxShapeCanvas);
IMPLEMENT_DYNAMIC_CLASS(wxPyShapeEvtHandler, wxShapeEvtHandler);
IMPLEMENT_ABSTRACT_CLASS(wxPyShape, wxShape);
IMPLEMENT_DYNAMIC_CLASS(wxPyRectangleShape, wxRectangleShape);
IMPLEMENT_DYNAMIC_CLASS(wxPyControlPoint, wxControlPoint);
IMPLEMENT_DYNAMIC_CLASS(wxPyBitmapShape, wxBitmapShape);
IMPLEMENT_DYNAMIC_CLASS(wxPyDrawnShape, wxDrawnShape);
IMPLEMENT_DYNAMIC_CLASS(wxPyCompositeShape, wxCompositeShape);
IMPLEMENT_DYNAMIC_CLASS(wxPyDividedShape, wxDividedShape);
IMPLEMENT_DYNAMIC_CLASS(wxPyDivisionShape, wxDivisionShape);
IMPLEMENT_DYNAMIC_CLASS(wxPyEllipseShape, wxEllipseShape);
IMPLEMENT_DYNAMIC_CLASS(wxPyCircleShape, wxCircleShape);
IMPLEMENT_DYNAMIC_CLASS(wxPyLineShape, wxLineShape);
IMPLEMENT_DYNAMIC_CLASS(wxPyPolygonShape, wxPolygonShape);
IMPLEMENT_DYNAMIC_CLASS(wxPyTextShape, wxTextShape);

//---------------------------------------------------------------------------

extern "C" SWIGEXPORT(void) initoglbasicc();
extern "C" SWIGEXPORT(void) initoglshapesc();
extern "C" SWIGEXPORT(void) initoglshapes2c();
extern "C" SWIGEXPORT(void) initoglcanvasc();
%}


%init %{

    initoglbasicc();
    initoglshapesc();
    initoglshapes2c();
    initoglcanvasc();


    wxClassInfo::CleanUpClasses();
    wxClassInfo::InitializeClasses();

    wxPyPtrTypeMap_Add("wxControlPoint", "wxPyControlPoint");
    wxPyPtrTypeMap_Add("wxShapeCanvas", "wxPyShapeCanvas");
    wxPyPtrTypeMap_Add("wxShapeEvtHandler", "wxPyShapeEvtHandler");
    wxPyPtrTypeMap_Add("wxShape", "wxPyShape");
    wxPyPtrTypeMap_Add("wxRectangleShape", "wxPyRectangleShape");
    wxPyPtrTypeMap_Add("wxDrawnShape", "wxPyDrawnShape");
    wxPyPtrTypeMap_Add("wxCompositeShape", "wxPyCompositeShape");
    wxPyPtrTypeMap_Add("wxDividedShape", "wxPyDividedShape");
    wxPyPtrTypeMap_Add("wxDivisionShape", "wxPyDivisionShape");
    wxPyPtrTypeMap_Add("wxEllipseShape", "wxPyEllipseShape");
    wxPyPtrTypeMap_Add("wxCircleShape", "wxPyCircleShape");
    wxPyPtrTypeMap_Add("wxLineShape", "wxPyLineShape");
    wxPyPtrTypeMap_Add("wxPolygonShape", "wxPyPolygonShape");
    wxPyPtrTypeMap_Add("wxTextShape", "wxPyTextShape");

%}

//----------------------------------------------------------------------
// And this gets appended to the shadow class file.
//----------------------------------------------------------------------

%pragma(python) include="_oglextras.py";

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
