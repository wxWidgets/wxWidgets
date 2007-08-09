////////////////////////////////////////////////////////////////////////////
// Name:        twoitem.h
// Purpose:     A template function to help with converting a python object
//              to some wx class that takes two integer value parameters.
//              Factored out of wxPython_int.h               
//
// Author:      Robin Dunn
//
// Created:     25-April-2006
// RCS-ID:      $Id$
// Copyright:   (c) 2006 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __twoitem_h__
#define __twoitem_h__

template<class T>
bool wxPyTwoIntItem_helper(PyObject* source, T** obj, const wxChar* name)
{
    // If source is an object instance then it may already be the right type
    if (wxPySwigInstance_Check(source)) {
        T* ptr;
        if (! wxPyConvertSwigPtr(source, (void **)&ptr, name))
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
        **obj = T(PyInt_AsLong(o1), PyInt_AsLong(o2));
        Py_DECREF(o1);
        Py_DECREF(o2);
        return true;
    }

 error:
    wxString msg;
    msg.Printf(wxT("Expected a 2-tuple of integers or a %s object."), name);
    PyErr_SetString(PyExc_TypeError, msg.mb_str());
    return false;
}


#endif
