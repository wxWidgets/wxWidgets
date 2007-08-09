/////////////////////////////////////////////////////////////////////////////
// Name:        _ogldefs.i
// Purpose:     SWIG definitions for the wxWindows Object Graphics Library
//
// Author:      Robin Dunn
//
// Created:     27-Aug-1999
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------------------
// Typemaps just for OGL


// OOR Support
%typemap(out) wxPyShape*                { $result = wxPyMake_wxShapeEvtHandler($1, $owner); }
%typemap(out) wxPyShapeEvtHandler*      { $result = wxPyMake_wxShapeEvtHandler($1, $owner); }
%typemap(out) wxPyDivisionShape*        { $result = wxPyMake_wxShapeEvtHandler($1, $owner); }

%typemap(out) wxPyShapeCanvas*          { $result = wxPyMake_wxObject($1, $owner); }
%typemap(out) wxDiagram*                { $result = wxPyMake_wxObject($1, $owner); }
%typemap(out) wxOGLConstraint*          { $result = wxPyMake_wxObject($1, $owner); }
%typemap(out) wxPseudoMetaFile*         { $result = wxPyMake_wxObject($1, $owner); }
%typemap(out) wxArrowHead*              { $result = wxPyMake_wxObject($1, $owner); }




// wxOGL doesn't use a ref-counted copy of pens and brushes, so we'll
// use the pen and brush lists to simulate that...

%typemap(in) wxPen* {
    wxPen* temp;
    if ($input) {
        if ($input == Py_None) { temp = NULL; }
        else if (! wxPyConvertSwigPtr($input, (void **) &temp, wxT("wxPen"))) {
            PyErr_SetString(PyExc_TypeError, "Type error, expected wxPen.");
        return NULL;
        }
    }
    if (temp)
        $1 = wxThePenList->FindOrCreatePen(temp->GetColour(),
                                           temp->GetWidth(),
                                           temp->GetStyle());
    else
        $1 = NULL;
}

%typemap(in) wxBrush* {
    wxBrush* temp;
    if ($input) {
        if ($input == Py_None) { temp = NULL; }
        else if (! wxPyConvertSwigPtr($input, (void **) &temp, wxT("wxBrush"))) {
            PyErr_SetString(PyExc_TypeError, "Type error, expected wxBrush.");
        return NULL;
        }
    }
    if (temp)
        $1 = wxTheBrushList->FindOrCreateBrush(temp->GetColour(), temp->GetStyle());
    else
        $1 = NULL;
}


%typemap(in) wxFont* {
    wxFont* temp;
    if ($input) {
        if ($input == Py_None) { temp = NULL; }
        else if (! wxPyConvertSwigPtr($input, (void **) &temp, wxT("wxFont"))) {
            PyErr_SetString(PyExc_TypeError, "Type error, expected wxFont.");
        return NULL;
        }
    }
    if (temp)
        $1 = wxTheFontList->FindOrCreateFont(temp->GetPointSize(),
                                             temp->GetFamily(),
                                             temp->GetStyle(),
                                             temp->GetWeight(),
                                             temp->GetUnderlined(),
                                             temp->GetFaceName(),
                                             temp->GetEncoding());
    else
        $1 = NULL;
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------





