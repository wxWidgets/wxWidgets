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


class wxOGLConstraint;
class wxBitmapShape;
class wxDiagram;
class wxDrawnShape;
class wxCircleShape;
class wxCompositeShape;
class wxDividedShape;
class wxDivisionShape;
class wxEllipseShape;
class wxLineShape;
class wxPolygonShape;
class wxRectangleShape;
class wxPseudoMetaFile;
class wxShape;
class wxShapeCanvas;
class wxShapeEvtHandler;
class wxTextShape;
class wxControlPoint;

class wxPyOGLConstraint;
class wxPyBitmapShape;
class wxPyDiagram;
class wxPyDrawnShape;
class wxPyCircleShape;
class wxPyCompositeShape;
class wxPyDividedShape;
class wxPyDivisionShape;
class wxPyEllipseShape;
class wxPyLineShape;
class wxPyPolygonShape;
class wxPyRectangleShape;
class wxPyPseudoMetaFile;
class wxPyShape;
class wxPyShapeCanvas;
class wxPyShapeEvtHandler;
class wxPyTextShape;
class wxPyControlPoint;


//---------------------------------------------------------------------------
// Typemaps just for OGL


// wxOGL doesn't use a ref-counted copy of pens and brushes, so we'll
// use the pen and brush lists to simulate that...


%typemap(python, in) wxPen* {
    wxPen* temp;
    if ($source) {
        if ($source == Py_None) { temp = NULL; }
        else if (SWIG_GetPtrObj($source, (void **) &temp,"_wxPen_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error, expected _wxPen_p.");
        return NULL;
        }
    }
    if (temp)
        $target = wxThePenList->FindOrCreatePen(temp->GetColour(),
                                                temp->GetWidth(),
                                                temp->GetStyle());
    else
        $target = NULL;
}

%typemap(python, in) wxBrush* {
    wxBrush* temp;
    if ($source) {
        if ($source == Py_None) { temp = NULL; }
        else if (SWIG_GetPtrObj($source, (void **) &temp,"_wxBrush_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error, expected _wxBrush_p.");
        return NULL;
        }
    }
    if (temp)
        $target = wxTheBrushList->FindOrCreateBrush(temp->GetColour(), temp->GetStyle());
    else
        $target = NULL;
}


%typemap(python, in) wxFont* {
    wxFont* temp;
    if ($source) {
        if ($source == Py_None) { temp = NULL; }
        else if (SWIG_GetPtrObj($source, (void **) &temp,"_wxFont_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error, expected _wxFont_p.");
        return NULL;
        }
    }
    if (temp)
        $target = wxTheFontList->FindOrCreateFont(temp->GetPointSize(),
                                                  temp->GetFamily(),
                                                  temp->GetStyle(),
                                                  temp->GetWeight(),
                                                  temp->GetUnderlined(),
                                                  temp->GetFaceName(),
                                                  temp->GetEncoding());
    else
        $target = NULL;
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------





