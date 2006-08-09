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


//----------------------------------------------------------------------
// Typemaps to convert a list of items to an int (size) and an array

%define MAKE_INT_ARRAY_TYPEMAPS(NAME, ARR_NAME)
    %typemap(in) (int NAME, int* ARR_NAME) {
        $1 = PyList_Size($input);
        $2 =  int_LIST_helper($input);
        if ($2 == NULL) SWIG_fail;
    }

    %typemap(freearg) (int NAME, int* ARR_NAME) {
        if ($2) delete [] $2;
    }
%enddef

MAKE_INT_ARRAY_TYPEMAPS(widths, widths_field)
MAKE_INT_ARRAY_TYPEMAPS(styles, styles_field)

    

// Same things for a wxString
%typemap(in) (int choices, wxString* choices_array ) {
    $1 = PyList_Size($input);
    $2 = wxString_LIST_helper($input);
    if ($2 == NULL) SWIG_fail;
}
%typemap(freearg) (int choices, wxString* choices_array ) {
    if ($2) delete [] $2;
}



//---------------------------------------------------------------------------
// wxString typemaps


%typemap(in) wxString& (bool temp=false) {
    $1 = wxString_in_helper($input);
    if ($1 == NULL) SWIG_fail;
    temp = true;
}
%typemap(freearg) wxString& {
    if (temp$argnum)
        delete $1;
}

%typemap(out) wxString& {
%#if wxUSE_UNICODE
    $result = PyUnicode_FromWideChar($1->c_str(), $1->Len());
%#else
    $result = PyString_FromStringAndSize($1->c_str(), $1->Len());
%#endif
}


%apply wxString& { wxString* }



%typemap(out) wxString {
%#if wxUSE_UNICODE
    $result = PyUnicode_FromWideChar($1.c_str(), $1.Len());
%#else
    $result = PyString_FromStringAndSize($1.c_str(), $1.Len());
%#endif
}

%typemap(varout) wxString {
%#if wxUSE_UNICODE
    $result = PyUnicode_FromWideChar($1.c_str(), $1.Len());
%#else
    $result = PyString_FromStringAndSize($1.c_str(), $1.Len());
%#endif
}


%typemap(in) wxString {
    wxString* sptr = wxString_in_helper($input);
    if (sptr == NULL) SWIG_fail;
    $1 = *sptr;
    delete sptr;
}

%typemap(typecheck, precedence=SWIG_TYPECHECK_POINTER) wxString& {
    $1 = PyString_Check($input) || PyUnicode_Check($input);
}

//---------------------------------------------------------------------------
// wxMemoryBuffer  (needed for wxSTC)

%typemap(in) wxMemoryBuffer& (bool temp=false) {
    if (!PyString_Check($input)) {
        PyErr_SetString(PyExc_TypeError, "String buffer expected");
        SWIG_fail;
    }
    char* str = PyString_AS_STRING($input);
    int   len = PyString_GET_SIZE($input);
    $1 = new wxMemoryBuffer(len);
    temp = true;
    memcpy($1->GetData(), str, len);
    $1->SetDataLen(len);
}

%typemap(freearg) wxMemoryBuffer& {
    if (temp$argnum) delete $1;
}

%typemap(out) wxMemoryBuffer {
    $result = PyString_FromStringAndSize((char*)$1.GetData(), $1.GetDataLen());
}



%typemap(out) wxCharBuffer {
    $result = PyString_FromString((char*)$1.data());
}


//---------------------------------------------------------------------------
// Typemaps to convert Python sequence objects (tuples, etc.) to
// wxSize, wxPoint, wxRealPoint, and wxRect.


%typemap(in) wxSize& (wxSize temp) {
    $1 = &temp;
    if ( ! wxSize_helper($input, &$1)) SWIG_fail;
}
%typemap(typecheck, precedence=SWIG_TYPECHECK_POINTER) wxSize& {
    $1 = wxPySimple_typecheck($input, wxT("wxSize"), 2);
}


%typemap(in) wxPoint& (wxPoint temp) {
    $1 = &temp;
    if ( ! wxPoint_helper($input, &$1)) SWIG_fail;
}
%typemap(typecheck, precedence=SWIG_TYPECHECK_POINTER) wxPoint& {
    $1 = wxPySimple_typecheck($input, wxT("wxPoint"), 2);
}


%typemap(in) wxRealPoint& (wxRealPoint temp) {
    $1 = &temp;
    if ( ! wxRealPoint_helper($input, &$1)) SWIG_fail;
}
%typemap(typecheck, precedence=SWIG_TYPECHECK_POINTER) wxRealPoint& {
    $1 = wxPySimple_typecheck($input, wxT("wxRealPoint"), 2);
}


%typemap(in) wxRect& (wxRect temp) {
    $1 = &temp;
    if ( ! wxRect_helper($input, &$1)) SWIG_fail;
}
%typemap(typecheck, precedence=SWIG_TYPECHECK_POINTER) wxRect& {
    $1 = wxPySimple_typecheck($input, wxT("wxRect"), 4);
}


%typemap(in) wxPoint2D& (wxPoint2D temp) {
    $1 = &temp;
    if ( ! wxPoint2D_helper($input, &$1)) SWIG_fail;
}
%typemap(typecheck, precedence=SWIG_TYPECHECK_POINTER) wxPoint2D& {
    $1 = wxPySimple_typecheck($input, wxT("wxPoint2D"), 2);
}


//---------------------------------------------------------------------------
// Typemap to convert strings to wxColour.  Two string formats are accepted,
// either a colour name, or a hex colour spec like "#RRGGBB"

%typemap(in) wxColour& (wxColour temp) {
    $1 = &temp;
    if ( ! wxColour_helper($input, &$1)) SWIG_fail;
}
%typemap(typecheck, precedence=SWIG_TYPECHECK_POINTER) wxColour& {
    $1 = wxColour_typecheck($input);
}

%typemap(in) wxColour* (wxColour temp) {
    $1 = &temp;
    if ( ! wxColour_helper($input, &$1)) SWIG_fail;
}
%typemap(typecheck, precedence=SWIG_TYPECHECK_POINTER) wxColour* {
    $1 = wxColour_typecheck($input);
}


//---------------------------------------------------------------------------
// Typemap for wxArrayString from Python sequence objects

%typemap(in) wxArrayString& (bool temp=false) {
    if (! PySequence_Check($input)) {
        PyErr_SetString(PyExc_TypeError, "Sequence of strings expected.");
        SWIG_fail;
    }
    $1 = new wxArrayString;
    temp = true;
    int i, len=PySequence_Length($input);
    for (i=0; i<len; i++) {
        PyObject* item = PySequence_GetItem($input, i);
        wxString* s = wxString_in_helper(item);
        if (PyErr_Occurred())  SWIG_fail;
        $1->Add(*s);
        delete s;
        Py_DECREF(item);
    }
}

%typemap(freearg) wxArrayString& {
    if (temp$argnum) delete $1;
}

//---------------------------------------------------------------------------
// Typemap for wxArrayInt from Python sequence objects

%typemap(in) wxArrayInt& (bool temp=false) {
    if (! PySequence_Check($input)) {
        PyErr_SetString(PyExc_TypeError, "Sequence of integers expected.");
        SWIG_fail;
    }
    $1 = new wxArrayInt;
    temp = true;
    int i, len=PySequence_Length($input);
    for (i=0; i<len; i++) {
        PyObject* item = PySequence_GetItem($input, i);
        PyObject* number  = PyNumber_Int(item);
        $1->Add(PyInt_AS_LONG(number));
        Py_DECREF(item);
        Py_DECREF(number);
    }
}

%typemap(freearg) wxArrayInt& {
    if (temp$argnum) delete $1;
}


//---------------------------------------------------------------------------
// Typemaps to convert an array of ints to a list for return values
%typemap(out) wxArrayInt& {
    $result = PyList_New(0);
    size_t idx;
    for (idx = 0; idx < $1->GetCount(); idx += 1) {
        PyObject* val = PyInt_FromLong( $1->Item(idx) );
        PyList_Append($result, val);
        Py_DECREF(val);
    }
}

%typemap(out) wxArrayInt {
    $result = PyList_New(0);
    size_t idx;
    for (idx = 0; idx < $1.GetCount(); idx += 1) {
        PyObject* val = PyInt_FromLong( $1.Item(idx) );
        PyList_Append($result, val);
        Py_DECREF(val);
    }
}



// Typemaps to convert an array of strings to a list for return values
%typemap(out) wxArrayString& {
    $result = wxArrayString2PyList_helper(*$1);
}

%typemap(out) wxArrayString {
    $result = wxArrayString2PyList_helper($1);
}


//---------------------------------------------------------------------------

%typemap(out) bool {
    $result = $1 ? Py_True : Py_False; Py_INCREF($result);
}


//---------------------------------------------------------------------------
// wxFileOffset, can be a 32-bit or a 64-bit integer

%typemap(in) wxFileOffset {
    if (sizeof(wxFileOffset) > sizeof(long))
        $1 = PyLong_AsLongLong($input);
    else
        $1 = PyInt_AsLong($input);
}

%typemap(out) wxFileOffset {
    if (sizeof(wxFileOffset) > sizeof(long))
        $result = PyLong_FromLongLong($1);
    else
        $result = PyInt_FromLong($1);
}


//---------------------------------------------------------------------------
// Typemap for when GDI objects are returned by reference.  This will cause a
// copy to be made instead of returning a reference to the same instance.  The
// GDI object's internal refcounting scheme will do a copy-on-write of the
// internal data as needed.

// These too?
//, wxRegion&, wxPalette&

%typemap(out) wxBrush&, wxPen&, wxFont&, wxBitmap&, wxIcon&, wxCursor& {
    $*1_ltype* resultptr = new $*1_ltype(*$1);
    $result = SWIG_NewPointerObj((void*)(resultptr), $1_descriptor, 1);
}


//---------------------------------------------------------------------------
// Typemaps to convert return values that are base class pointers
// to the real derived type, if possible.  See wxPyMake_wxObject in
// helpers.cpp

// NOTE: For those classes that also call _setOORInfo these typemaps should be
// disabled for the constructor.

%typemap(out) wxEvtHandler*             { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxMenu*                   { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxValidator*              { $result = wxPyMake_wxObject($1, (bool)$owner); }

%typemap(out) wxApp*                    { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxPyApp*                  { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxDC*                     { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxFSFile*                 { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxFileSystem*             { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxImageList*              { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxImage*                  { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxListItem*               { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxMenuItem*               { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxMouseEvent*             { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxObject*                 { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxPyPrintout*             { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxToolBarToolBase*        { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxToolTip*                { $result = wxPyMake_wxObject($1, (bool)$owner); }


%typemap(out) wxBitmapButton*           { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxButton*                 { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxControl*                { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxFrame*                  { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxGrid*                   { $result = wxPyMake_wxObject($1, (bool)$owner); }
//%typemap(out) wxListCtrl*               { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxMDIChildFrame*          { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxMDIClientWindow*        { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxMenuBar*                { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxNotebook*               { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxStaticBox*              { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxStatusBar*              { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxTextCtrl*               { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxToolBar*                { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxToolBarBase*            { $result = wxPyMake_wxObject($1, (bool)$owner); }
//%typemap(out) wxTreeCtrl*               { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxPyTreeCtrl*             { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxWindow*                 { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxPyHtmlWindow*           { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxWizardPage*             { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxPyWizardPage*           { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxPanel*                  { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxDialog*                 { $result = wxPyMake_wxObject($1, (bool)$owner); }
%typemap(out) wxScrolledWindow*         { $result = wxPyMake_wxObject($1, (bool)$owner); }

%typemap(out) wxSizer*                  { $result = wxPyMake_wxObject($1, (bool)$owner); }


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------






