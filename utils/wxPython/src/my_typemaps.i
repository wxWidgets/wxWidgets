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
// Here are some to map (int LCOUNT, int* LIST), etc. from a python list

%{

HELPEREXPORT byte* byte_LIST_helper(PyObject* source);
HELPEREXPORT int* int_LIST_helper(PyObject* source);
HELPEREXPORT long* long_LIST_helper(PyObject* source);
HELPEREXPORT char** string_LIST_helper(PyObject* source);
HELPEREXPORT wxPoint* wxPoint_LIST_helper(PyObject* source);
HELPEREXPORT wxBitmap** wxBitmap_LIST_helper(PyObject* source);
HELPEREXPORT wxString* wxString_LIST_helper(PyObject* source);
HELPEREXPORT wxAcceleratorEntry* wxAcceleratorEntry_LIST_helper(PyObject* source);

%}

//----------------------------------------------------------------------

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



%typemap(python,in) wxDash* LIST = unsigned long* LIST;
%typemap(python,freearg) wxDash* LIST = unsigned long* LIST;


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


// --------------------------------------------------------------------
//---------------------------------------------------------------------------



// --------------------------------------------------------------------
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
/////////////////////////////////////////////////////////////////////////////
//
// $Log$
// Revision 1.6  1999/09/10 06:05:32  RD
// Lots more support for event-less callbacks
// Exported some symbols for the other modules to use
// etc.
//
// Revision 1.5  1999/04/30 03:29:19  RD
//
// wxPython 2.0b9, first phase (win32)
// Added gobs of stuff, see wxPython/README.txt for details
//
// Revision 1.4.4.2  1999/03/28 06:35:01  RD
//
// wxPython 2.0b8
//     Python thread support
//     various minor additions
//     various minor fixes
//
// Revision 1.4.4.1  1999/03/16 06:04:03  RD
//
// wxPython 2.0b7
//
// Revision 1.4  1998/11/25 08:45:27  RD
//
// Added wxPalette, wxRegion, wxRegionIterator, wxTaskbarIcon
// Added events for wxGrid
// Other various fixes and additions
//
// Revision 1.3  1998/11/15 23:03:47  RD
// Removing some ifdef's for wxGTK
//
// Revision 1.2  1998/08/14 23:36:39  RD
// Beginings of wxGTK compatibility
//
// Revision 1.1  1998/08/09 08:25:52  RD
// Initial version
//
//

