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
// Tell SWIG to wrap all the wrappers with our thread protection

%except(python) {
    PyThreadState* __tstate = wxPyBeginAllowThreads();
$function
    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
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

// wxDash is a signed char
%typemap(python,in) wxDash* choices  {
    $target = (wxDash*)byte_LIST_helper($source);
    if ($target == NULL) {
        return NULL;
    }
}
%typemap(python,freearg) wxDash* choices {
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
	$target = NPOINTS;
}

%typemap(python,in) wxPoint* points (int NPOINTS) {
    $target = wxPoint_LIST_helper($source, &NPOINTS);
	if ($target == NULL) {
		return NULL;
	}
}
%typemap(python,freearg) wxPoint* points {
    delete [] $source;
}




//---------------------------------------------------------------------------


%typemap(python, in) wxString& {
    $target = wxString_in_helper($source);
    if ($target == NULL)
        return NULL;
}


%typemap(python, freearg) wxString& {
    if ($target)
        delete $source;
}



%typemap(python, out) wxString {
#if wxUSE_UNICODE
    $target = PyUnicode_FromWideChar($source->c_str(), $source->Len());
#else
    $target = PyString_FromStringAndSize($source->c_str(), $source->Len());
#endif
}
%typemap(python, ret) wxString {
    delete $source;
}


%typemap(python, out) wxString* {
#if wxUSE_UNICODE
    $target = PyUnicode_FromWideChar($source->c_str(), $source->Len());
#else
    $target = PyString_FromStringAndSize($source->c_str(), $source->Len());
#endif
}



//---------------------------------------------------------------------------


%typemap(python, in) wxMemoryBuffer& {
    if (!PyString_Check($source)) {
        PyErr_SetString(PyExc_TypeError, "String buffer expected");
        return NULL;
    }
    char* str = PyString_AS_STRING($source);
    int   len = PyString_GET_SIZE($source);
    $target = new wxMemoryBuffer(len);
    memcpy($target->GetData(), str, len);
}

%typemap(python, freearg) wxMemoryBuffer& {
    if ($target)
        delete $source;
}

%typemap(python, out) wxMemoryBuffer {
    $target = PyString_FromStringAndSize((char*)$source->GetData(), $source->GetDataLen());
}

%typemap(python, ret) wxMemoryBuffer {
    delete $source;
}


//---------------------------------------------------------------------------
// Typemaps to convert Python sequence objects (tuples, etc.) to
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

%typemap(python,in) wxPoint2DDouble& (wxPoint2DDouble temp) {
    $target = &temp;
    if (! wxPoint2DDouble_helper($source, &$target))
        return NULL;
}

//---------------------------------------------------------------------------
// Typemap to convert strings to wxColour.  Two string formats are accepted,
// either a colour name, or a hex colour spec like "#RRGGBB"

%typemap(python,in) wxColour& (wxColour temp) {
    $target = &temp;
    if (! wxColour_helper($source, &$target))
        return NULL;
}

//---------------------------------------------------------------------------
// Typemap for wxArrayString from Python sequence objects

%typemap(python,in) wxArrayString& {
    if (! PySequence_Check($source)) {
        PyErr_SetString(PyExc_TypeError, "Sequence of strings expected.");
        return NULL;
    }
    $target = new wxArrayString;
    int i, len=PySequence_Length($source);
    for (i=0; i<len; i++) {
        PyObject* item = PySequence_GetItem($source, i);
#if wxUSE_UNICODE
        PyObject* str  = PyObject_Unicode(item);
#else
        PyObject* str  = PyObject_Str(item);
#endif
        $target->Add(Py2wxString(str));
        Py_DECREF(item);
        Py_DECREF(str);
    }
}

%typemap(python, freearg) wxArrayString& {
    if ($target)
        delete $source;
}

//---------------------------------------------------------------------------
// Typemap for wxArrayInt from Python sequence objects

%typemap(python,in) wxArrayInt& {
    if (! PySequence_Check($source)) {
        PyErr_SetString(PyExc_TypeError, "Sequence of integers expected.");
        return NULL;
    }
    $target = new wxArrayInt;
    int i, len=PySequence_Length($source);
    for (i=0; i<len; i++) {
        PyObject* item = PySequence_GetItem($source, i);
        PyObject* number  = PyNumber_Int(item);
        $target->Add(PyInt_AS_LONG(number));
        Py_DECREF(item);
        Py_DECREF(number);
    }
}

%typemap(python, freearg) wxArrayInt& {
    if ($target)
        delete $source;
}


// Typemaps to convert an array of ints to a list
%typemap(python, out) wxArrayInt& {
    $target = PyList_New(0);
    size_t idx;
    for (idx = 0; idx < $source->GetCount(); idx += 1) {
        PyObject* val = PyInt_FromLong($source->Item(idx));
        PyList_Append($target, val);
        Py_DECREF(val);
    }
}

%typemap(python, out) wxArrayInt {
    $target = PyList_New(0);
    size_t idx;
    for (idx = 0; idx < $source->GetCount(); idx += 1) {
        PyObject* val = PyInt_FromLong($source->Item(idx));
        PyList_Append($target, val);
        Py_DECREF(val);
    }
    delete $source;
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


%typemap(python,ignore) bool *T_OUTPUT(int temp)
{
  $target = (bool*)&temp;
}

%typemap(python,argout) bool *T_OUTPUT
{
    PyObject *o;
    o = PyInt_FromLong((long) (*$source));
    $target = t_output_helper($target, o);
}

%typemap(python,ignore) bool  *OUTPUT = bool *T_OUTPUT;
%typemap(python,argout) bool  *OUTPUT = bool *T_OUTPUT;



%typemap(python,ignore) byte *T_OUTPUT(int temp)
{
  $target = (byte*)&temp;
}

%typemap(python,argout) byte *T_OUTPUT
{
    PyObject *o;
    o = PyInt_FromLong((long) (*$source));
    $target = t_output_helper($target, o);
}

%typemap(python,ignore) byte  *OUTPUT = byte *T_OUTPUT;
%typemap(python,argout) byte  *OUTPUT = byte *T_OUTPUT;


%typemap(python,ignore) wxCoord *OUTPUT = int *OUTPUT;
%typemap(python,argout) wxCoord *OUTPUT = int *OUTPUT;


//---------------------------------------------------------------------------
// Typemaps to convert return values that are base class pointers
// to the real derived type, if possible.  See wxPyMake_wxObject in
// helpers.cpp

%typemap(python, out) wxEvtHandler*             { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxMenu*                   { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxValidator*              { $target = wxPyMake_wxObject($source); }

%typemap(python, out) wxApp*                    { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxPyApp*                  { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxDC*                     { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxFSFile*                 { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxFileSystem*             { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxGridTableBase*          { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxImageList*              { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxListItem*               { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxMenuItem*               { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxMouseEvent*             { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxObject*                 { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxPyPrintout*             { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxToolBarToolBase*        { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxToolTip*                { $target = wxPyMake_wxObject($source); }


%typemap(python, out) wxBitmapButton*           { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxButton*                 { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxControl*                { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxFrame*                  { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxGrid*                   { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxListCtrl*               { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxMDIChildFrame*          { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxMDIClientWindow*        { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxMenuBar*                { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxNotebook*               { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxStaticBox*              { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxStatusBar*              { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxTextCtrl*               { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxToolBar*                { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxToolBarBase*            { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxTreeCtrl*               { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxWindow*                 { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxHtmlWindow*             { $target = wxPyMake_wxObject($source); }
%typemap(python, out) wxWizardPage*             { $target = wxPyMake_wxObject($source); }

%typemap(python, out) wxSizer*                  { $target = wxPyMake_wxSizer($source); }


//%typemap(python, out) wxHtmlCell*               { $target = wxPyMake_wxObject($source); }
//%typemap(python, out) wxHtmlContainerCell*      { $target = wxPyMake_wxObject($source); }
//%typemap(python, out) wxHtmlParser*             { $target = wxPyMake_wxObject($source); }
//%typemap(python, out) wxHtmlWinParser*          { $target = wxPyMake_wxObject($source); }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------






