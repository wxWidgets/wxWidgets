/////////////////////////////////////////////////////////////////////////////
// Name:        my_typemaps.i
// Purpose:     Special typemaps specifically for wxPython.
//
// Author:      Robin Dunn
//
// Created:     7/3/97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------------------
// Tell SWIG to wrap all the wrappers with Python's thread macros

%except(python) {
    wxPy_BEGIN_ALLOW_THREADS;
    $function
    wxPy_END_ALLOW_THREADS;
}

//----------------------------------------------------------------------
// LCOUNT and choices go together.  They allow a single Python list to be
// converted to an integer count and an array count items long.

%typemap(python,build) int LCOUNT {
    if (_in_choices) {
        $target = PyList_Size(_in_choices);
    }
    else {
        $target = 0;
    }
}



%typemap(python,in) byte* choices  {
    $target = byte_LIST_helper($source);
    if ($target == NULL) {
        return NULL;
    }
}
%typemap(python,freearg) byte* choices {
    delete [] $source;
}


%typemap(python,in) int* choices  {
    $target = int_LIST_helper($source);
    if ($target == NULL) {
        return NULL;
    }
}
%typemap(python,freearg) int* choices {
    delete [] $source;
}


%typemap(python,in) long* choices {
    $target = long_LIST_helper($source);
    if ($target == NULL) {
        return NULL;
    }
}
%typemap(python,freearg) long* choices {
    delete [] $source;
}


%typemap(python,in) unsigned long* choices {
    $target = (unsigned long*)long_LIST_helper($source);
    if ($target == NULL) {
        return NULL;
    }
}
%typemap(python,freearg) unsigned long* choices {
    delete [] $source;
}


#ifdef __WXMSW__
%typemap(python,in) wxDash* choices = unsigned long* choices;
%typemap(python,freearg) wxDash* choices = unsigned long* choices;
#else
%typemap(python,in) wxDash* choices = byte* choices;
%typemap(python,freearg) wxDash* choices = byte* choices;
#endif


%typemap(python,in) char** choices {
    $target = string_LIST_helper($source);
    if ($target == NULL) {
        return NULL;
    }
}
%typemap(python,freearg) char** choices {
    delete [] $source;
}


%typemap(python,in) wxBitmap** choices  {
    $target = wxBitmap_LIST_helper($source);
    if ($target == NULL) {
        return NULL;
    }
}
%typemap(python,freearg) wxBitmap** choices {
    delete [] $source;
}

%typemap(python,in) wxString* choices  {
    $target = wxString_LIST_helper($source);
    if ($target == NULL) {
        return NULL;
    }
}
%typemap(python,freearg) wxString* choices {
    delete [] $source;
}

%typemap(python,in) wxAcceleratorEntry* choices  {
    $target = wxAcceleratorEntry_LIST_helper($source);
    if ($target == NULL) {
        return NULL;
    }
}
%typemap(python,freearg) wxAcceleratorEntry* choices {
    delete [] $source;
}





%typemap(python,build) int PCOUNT {
    if (_in_points) {
        $target = PyList_Size(_in_points);
    }
    else {
        $target = 0;
    }
}

%typemap(python,in) wxPoint* points  {
    $target = wxPoint_LIST_helper($source);
    if ($target == NULL) {
        return NULL;
    }
}
%typemap(python,freearg) wxPoint* points {
    delete [] $source;
}



//---------------------------------------------------------------------------

%{
static char* wxStringErrorMsg = "string type is required for parameter";
%}

%typemap(python, in) wxString& {
    if (!PyString_Check($source)) {
        PyErr_SetString(PyExc_TypeError, wxStringErrorMsg);
        return NULL;
    }
    $target = new wxString(PyString_AsString($source), PyString_Size($source));
}
%typemap(python, freearg) wxString& {
    if ($target)
        delete $source;
}



%typemap(python, out) wxString {
    $target = PyString_FromStringAndSize($source->c_str(), $source->Len());
}
%typemap(python, ret) wxString {
    delete $source;
}


%typemap(python, out) wxString* {
    $target = PyString_FromStringAndSize($source->c_str(), $source->Len());
}





//---------------------------------------------------------------------------
// Typemaps to convert Python sequence objects (2-tuples, etc.) to
// wxSize, wxPoint, wxRealPoint, and wxRect.

%typemap(python,in) wxSize& (wxSize temp) {
    $target = &temp;
    if (! wxSize_helper($source, &$target))
        return NULL;
}

%typemap(python,in) wxPoint& (wxPoint temp) {
    $target = &temp;
    if (! wxPoint_helper($source, &$target))
        return NULL;
}

%typemap(python,in) wxRealPoint& (wxRealPoint temp) {
    $target = &temp;
    if (! wxRealPoint_helper($source, &$target))
        return NULL;
}

%typemap(python,in) wxRect& (wxRect temp) {
    $target = &temp;
    if (! wxRect_helper($source, &$target))
        return NULL;
}


//---------------------------------------------------------------------------
// Map T_OUTPUTs for floats to return ints.


%typemap(python,ignore) float          *T_OUTPUT_TOINT(float temp),
                        double         *T_OUTPUT_TOINT(double temp)
{
  $target = &temp;
}


%typemap(python,argout) float          *T_OUTPUT_TOINT,
                        double         *T_OUTPUT_TOINT
{
    PyObject *o;
    o = PyInt_FromLong((long) (*$source));
    $target = t_output_helper($target, o);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

