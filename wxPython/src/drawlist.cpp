/////////////////////////////////////////////////////////////////////////////
// Name:        drawlist.cpp
// Purpose:     Helper functions for optimized list drawing on a wxDC
//
// Author:      Robin Dunn,  Chris Barker
//
// Created:
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


#undef DEBUG
#include <Python.h>
#include "wx/wxPython/wxPython.h"
#include "wx/wxPython/pydrawxxx.h"


//---------------------------------------------------------------------------


// Called from _gdiinit so we can do the API import while the GIL is held
void wxPyDrawList_SetAPIPtr()
{
    wxPyCoreAPI_IMPORT();
}


PyObject* wxPyDrawXXXList(wxDC& dc, wxPyDrawListOp_t doDraw,
                          PyObject* pyCoords, PyObject* pyPens, PyObject* pyBrushes)
{
    bool blocked = wxPyBeginBlockThreads(); 

    bool      isFastSeq  = PyList_Check(pyCoords) || PyTuple_Check(pyCoords);
    bool      isFastPens = PyList_Check(pyPens) || PyTuple_Check(pyPens);
    bool      isFastBrushes = PyList_Check(pyBrushes) || PyTuple_Check(pyBrushes);
    int       numObjs = 0;
    int       numPens = 0;
    int       numBrushes = 0;
    wxPen*    pen;
    wxBrush*  brush;
    PyObject* obj;
    PyObject* coords;
    int       i = 0;
    PyObject* retval;

    if (!PySequence_Check(pyCoords)) {
        goto err0;
    }
    if (!PySequence_Check(pyPens)) {
        goto err1;
    }
    if (!PySequence_Check(pyBrushes)) {
        goto err2;
    }
    numObjs = PySequence_Length(pyCoords);
    numPens = PySequence_Length(pyPens);
    numBrushes = PySequence_Length(pyBrushes);
    for (i = 0; i < numObjs; i++) {
        // Use a new pen?
        if (i < numPens) {
            if (isFastPens) {
                obj = PySequence_Fast_GET_ITEM(pyPens, i);
            }
            else {
                obj = PySequence_GetItem(pyPens, i);
            }
            if (! wxPyConvertSwigPtr(obj, (void **) &pen, wxT("wxPen"))) {
                if (!isFastPens)
                    Py_DECREF(obj);
                goto err1;
            }

            dc.SetPen(*pen);
            if (!isFastPens)
                Py_DECREF(obj);
        }
        // Use a new brush?
        if (i < numBrushes) {
            if (isFastBrushes) {
                obj = PySequence_Fast_GET_ITEM(pyBrushes, i);
            }
            else {
                obj = PySequence_GetItem(pyBrushes, i);
            }
            if (!wxPyConvertSwigPtr(obj, (void **) &brush, wxT("wxBrush"))) {
                if (!isFastBrushes)
                    Py_DECREF(obj);
                goto err2;
            }

            dc.SetBrush(*brush);
            if (!isFastBrushes)
                Py_DECREF(obj);
        }

        // Get the Coordinates
        if (isFastSeq) {
            coords = PySequence_Fast_GET_ITEM(pyCoords, i);
        }
        else {
            coords = PySequence_GetItem(pyCoords, i);
        }


        // call the drawOp
        bool success = doDraw(dc, coords);
        if (!isFastSeq)
            Py_DECREF(coords);

        if (! success) {
            retval = NULL;
            goto exit;
        }

    } // end of main for loop

    Py_INCREF(Py_None);
    retval = Py_None;
    goto exit;


 err0:
    PyErr_SetString(PyExc_TypeError, "Expected a sequence of coordinates");
    retval = NULL;
    goto exit;

 err1:
    PyErr_SetString(PyExc_TypeError, "Expected a sequence of wxPens");
    retval = NULL;
    goto exit;

 err2:
    PyErr_SetString(PyExc_TypeError, "Expected a sequence of wxBrushes");
    retval = NULL;
    goto exit;


 exit:
    wxPyEndBlockThreads(blocked);
    return retval;
}



bool wxPyDrawXXXPoint(wxDC& dc, PyObject* coords)
{
    int x, y;

    if (! wxPy2int_seq_helper(coords, &x, &y)) {
        PyErr_SetString(PyExc_TypeError, "Expected a sequence of (x,y) sequences.");
        return false;
    }
    dc.DrawPoint(x, y);
    return true;
}

bool wxPyDrawXXXLine(wxDC& dc, PyObject* coords)
{
    int x1, y1, x2, y2;

    if (! wxPy4int_seq_helper(coords, &x1, &y1, &x2, &y2)) {
        PyErr_SetString(PyExc_TypeError, "Expected a sequence of (x1,y1, x1,y2) sequences.");
        return false;
    }
    dc.DrawLine(x1,y1, x2,y2);
    return true;
}

bool wxPyDrawXXXRectangle(wxDC& dc, PyObject* coords)
{
    int x, y, w, h;

    if (! wxPy4int_seq_helper(coords, &x, &y, &w, &h)) {
        PyErr_SetString(PyExc_TypeError, "Expected a sequence of (x,y, w,h) sequences.");
        return false;
    }
    dc.DrawRectangle(x, y, w, h);
    return true;
}

bool wxPyDrawXXXEllipse(wxDC& dc, PyObject* coords)
{
    int x, y, w, h;

    if (! wxPy4int_seq_helper(coords, &x, &y, &w, &h)) {
        PyErr_SetString(PyExc_TypeError, "Expected a sequence of (x,y, w,h) sequences.");
        return false;
    }
    dc.DrawEllipse(x, y, w, h);
    return true;
}


bool wxPyDrawXXXPolygon(wxDC& dc, PyObject* coords)
{
    wxPoint* points;
    int numPoints;

    points = wxPoint_LIST_helper(coords, &numPoints);
    if (! points) {
        PyErr_SetString(PyExc_TypeError, "Expected a sequence of sequences of (x,y) sequences.");
        return false;
    }
    dc.DrawPolygon(numPoints, points);
    delete [] points;
    return true;
}


//---------------------------------------------------------------------------



PyObject* wxPyDrawTextList(wxDC& dc, PyObject* textList, PyObject* pyPoints, PyObject* foregroundList, PyObject* backgroundList)
{
    bool blocked = wxPyBeginBlockThreads();

    bool      isFastSeq  = PyList_Check(pyPoints) || PyTuple_Check(pyPoints);
    bool      isFastText = PyList_Check(textList) || PyTuple_Check(textList);
    bool      isFastForeground = PyList_Check(foregroundList) || PyTuple_Check(foregroundList);
    bool      isFastBackground = PyList_Check(backgroundList) || PyTuple_Check(backgroundList);
    int       numText = 0;
    int       numPoints = 0;
    int       numForeground = 0;
    int       numBackground = 0;
    PyObject* obj;
    int       x1, y1;
    int       i = 0;
    wxColor*    color;
    PyObject* retval;
    wxString  string;

    if (!PySequence_Check(pyPoints)) {
        goto err0;
    }
    if (!PySequence_Check(textList)) {
        goto err1;
    }
    if (!PySequence_Check(foregroundList)) {
        goto err2;
    }
    if (!PySequence_Check(backgroundList)) {
        goto err3;
    }
    numPoints = PySequence_Length(pyPoints);
    numText = PySequence_Length(textList);
    numForeground = PySequence_Length(foregroundList);
    numBackground = PySequence_Length(backgroundList);

    for (i = 0; i < numPoints; i++) {
        // Use a new string ?
        if (i < numText) {
            if ( isFastText ) {
                obj = PySequence_Fast_GET_ITEM(textList, i);
            }
            else {
                obj = PySequence_GetItem(textList, i);
            }
            if (! PyString_Check(obj) && !PyUnicode_Check(obj) ) {
                Py_DECREF(obj);
                goto err1;
            }
            string = Py2wxString(obj);
            if ( !isFastText )
                Py_DECREF(obj);
        }

        if (i < numForeground) {
            // Use a new foreground ?
            if ( isFastForeground ) {
                obj = PySequence_Fast_GET_ITEM(foregroundList, i);
            }
            else {
                obj = PySequence_GetItem(foregroundList, i);
            }
            if (! wxPyConvertSwigPtr(obj, (void **) &color, wxT("wxColour"))) {
                if (!isFastForeground)
                    Py_DECREF(obj);
                goto err2;
            }
            dc.SetTextForeground(*color);
            if ( !isFastForeground )
                Py_DECREF(obj);
        }

        if (i < numBackground) {
            // Use a new background ?
            if ( isFastBackground ) {
                obj = PySequence_Fast_GET_ITEM(backgroundList, i);
            }
            else {
                obj = PySequence_GetItem(backgroundList, i);
            }
            if (! wxPyConvertSwigPtr(obj, (void **) &color, wxT("wxColour"))) {
                if (!isFastBackground)
                    Py_DECREF(obj);
                goto err3;
            }
            dc.SetTextBackground(*color);
            if ( !isFastBackground )
                Py_DECREF(obj);
        }

        // Get the point coordinates
        if (isFastSeq) {
            obj = PySequence_Fast_GET_ITEM(pyPoints, i);
        }
        else {
            obj = PySequence_GetItem(pyPoints, i);
        }
        if (! wxPy2int_seq_helper(obj, &x1, &y1)) {
            if (! isFastSeq)
                Py_DECREF(obj);
            goto err0;
        }
        if (PyErr_Occurred()) {
            retval = NULL;
            if (!isFastSeq)
                Py_DECREF(obj);
            goto exit;
        }


        // Now draw the text
        dc.DrawText(string, x1, y1);

        if (!isFastText)
            Py_DECREF(obj);
    }

    Py_INCREF(Py_None);
    retval = Py_None;
    goto exit;

 err0:
    PyErr_SetString(PyExc_TypeError, "Expected a sequence of (x,y) sequences.");
    retval = NULL;
    goto exit;
 err1:
    PyErr_SetString(PyExc_TypeError, "Expected a sequence of strings");
    retval = NULL;
    goto exit;

 err2:
    PyErr_SetString(PyExc_TypeError, "Expected a sequence of wxColours for foregrounds");
    retval = NULL;
    goto exit;

 err3:
    PyErr_SetString(PyExc_TypeError, "Expected a sequence of wxColours for backgrounds");
    retval = NULL;
    goto exit;

 exit:
    wxPyEndBlockThreads(blocked);
    return retval;
}



//---------------------------------------------------------------------------
