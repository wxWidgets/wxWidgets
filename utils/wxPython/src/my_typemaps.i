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
// LCOUNT and LIST go together.  They allow a single Python list to be
// converted to an integer count and an array count items long.

%typemap(python,build) int LCOUNT {
    if (_in_LIST) {
        $target = PyList_Size(_in_LIST);
    }
    else {
        $target = 0;
    }
}



%typemap(python,in) byte* LIST  {
    $target = byte_LIST_helper($source);
    if ($target == NULL) {
        return NULL;
    }
}
%typemap(python,freearg) byte* LIST {
    delete [] $source;
}


%typemap(python,in) int* LIST  {
    $target = int_LIST_helper($source);
    if ($target == NULL) {
        return NULL;
    }
}
%typemap(python,freearg) int* LIST {
    delete [] $source;
}


%typemap(python,in) long* LIST {
    $target = long_LIST_helper($source);
    if ($target == NULL) {
        return NULL;
    }
}
%typemap(python,freearg) long* LIST {
    delete [] $source;
}


%typemap(python,in) unsigned long* LIST {
    $target = (unsigned long*)long_LIST_helper($source);
    if ($target == NULL) {
        return NULL;
    }
}
%typemap(python,freearg) unsigned long* LIST {
    delete [] $source;
}


#ifdef __WXMSW__
%typemap(python,in) wxDash* LIST = unsigned long* LIST;
%typemap(python,freearg) wxDash* LIST = unsigned long* LIST;
#else
%typemap(python,in) wxDash* LIST = byte* LIST;
%typemap(python,freearg) wxDash* LIST = byte* LIST;
#endif


%typemap(python,in) char** LIST {
    $target = string_LIST_helper($source);
    if ($target == NULL) {
        return NULL;
    }
}
%typemap(python,freearg) char** LIST {
    delete [] $source;
}



%typemap(python,in) wxPoint* LIST  {
    $target = wxPoint_LIST_helper($source);
    if ($target == NULL) {
        return NULL;
    }
}
%typemap(python,freearg) wxPoint* LIST {
    delete [] $source;
}

%typemap(python,in) wxBitmap** LIST  {
    $target = wxBitmap_LIST_helper($source);
    if ($target == NULL) {
        return NULL;
    }
}
%typemap(python,freearg) wxBitmap** LIST {
    delete [] $source;
}

%typemap(python,in) wxString* LIST  {
    $target = wxString_LIST_helper($source);
    if ($target == NULL) {
        return NULL;
    }
}
%typemap(python,freearg) wxString* LIST {
    delete [] $source;
}

%typemap(python,in) wxAcceleratorEntry* LIST  {
    $target = wxAcceleratorEntry_LIST_helper($source);
    if ($target == NULL) {
        return NULL;
    }
}
%typemap(python,freearg) wxAcceleratorEntry* LIST {
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
    $target = PyString_FromString(WXSTRINGCAST *($source));
}
%typemap(python, ret) wxString {
    delete $source;
}


%typemap(python, out) wxString* {
    $target = PyString_FromString(WXSTRINGCAST (*$source));
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

