/////////////////////////////////////////////////////////////////////////////
// Name:        oglhelpers.cpp
// Purpose:     Some Helper functions to help in data conversions in OGL
//
// Author:      Robin Dunn
//
// Created:     3-Sept-1999
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include <Python.h>
#include "helpers.h"

//---------------------------------------------------------------------------
// This one will work for any class for the VERY generic cases, but beyond that
// the helper needs to know more about the type.

wxList* wxPy_wxListHelper(PyObject* pyList, char* className) {
    bool doSave = wxPyRestoreThread();
    if (!PyList_Check(pyList)) {
        PyErr_SetString(PyExc_TypeError, "Expected a list object.");
        wxPySaveThread(doSave);
        return NULL;
    }
    int count = PyList_Size(pyList);
    wxList* list = new wxList;
    if (! list) {
        PyErr_SetString(PyExc_MemoryError, "Unable to allocate wxList object");
        wxPySaveThread(doSave);
        return NULL;
    }
    for (int x=0; x<count; x++) {
        PyObject* pyo = PyList_GetItem(pyList, x);
        wxObject* wxo = NULL;

        if (SWIG_GetPtrObj(pyo, (void **)&wxo, className)) {
            char errmsg[1024];
            sprintf(errmsg, "Type error, expected list of %s objects", className);
            PyErr_SetString(PyExc_TypeError, errmsg);
            wxPySaveThread(doSave);
            return NULL;
        }
        list->Append(wxo);
    }
    wxPySaveThread(doSave);
    return list;
}

//---------------------------------------------------------------------------

wxList* wxPy_wxRealPoint_ListHelper(PyObject* pyList) {
    bool doSave = wxPyRestoreThread();
    if (!PyList_Check(pyList)) {
        PyErr_SetString(PyExc_TypeError, "Expected a list object.");
        wxPySaveThread(doSave);
        return NULL;
    }
    int count = PyList_Size(pyList);
    wxList* list = new wxList;
    if (! list) {
        PyErr_SetString(PyExc_MemoryError, "Unable to allocate wxList object");
        wxPySaveThread(doSave);
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
                wxPySaveThread(doSave);
                return NULL;
            }
            list->Append((wxObject*) new wxRealPoint(*wxo));
        }
    }
    wxPySaveThread(doSave);
    return list;
}


//---------------------------------------------------------------------------









