/////////////////////////////////////////////////////////////////////////////
// Name:        oglhelpers.h
// Purpose:     Some Helper functions to help in data conversions in OGL
//
// Author:      Robin Dunn
//
// Created:     9-Sept-1999
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __wxp_ogl_helpers__
#define __wxp_ogl_helpers__


#include <wx/ogl/ogl.h>
#include <wx/ogl/basicp.h>
#include <wx/ogl/constrnt.h>
#include <wx/ogl/bmpshape.h>
#include <wx/ogl/drawn.h>


//---------------------------------------------------------------------------
// Define a macro that will be used in the class definintions below

#define WXSHAPE_DEC_CALLBACKS()       \
    DEC_PYCALLBACK__                (OnDelete);             \
    DEC_PYCALLBACK__DC              (OnDraw);               \
    DEC_PYCALLBACK__DC              (OnDrawContents);       \
    DEC_PYCALLBACK__DCBOOL          (OnDrawBranches);       \
    DEC_PYCALLBACK__DC              (OnMoveLinks);          \
    DEC_PYCALLBACK__DC              (OnErase);              \
    DEC_PYCALLBACK__DC              (OnEraseContents);      \
    DEC_PYCALLBACK__DC              (OnHighlight);          \
    DEC_PYCALLBACK__2DBL2INT        (OnLeftClick);          \
    DEC_PYCALLBACK__2DBL2INT        (OnLeftDoubleClick);    \
    DEC_PYCALLBACK__2DBL2INT        (OnRightClick);         \
    DEC_PYCALLBACK__2DBL            (OnSize);               \
    DEC_PYCALLBACK_BOOL_DC4DBLBOOL  (OnMovePre);            \
    DEC_PYCALLBACK__DC4DBLBOOL      (OnMovePost);           \
    DEC_PYCALLBACK__BOOL2DBL2INT    (OnDragLeft);           \
    DEC_PYCALLBACK__2DBL2INT        (OnBeginDragLeft);      \
    DEC_PYCALLBACK__2DBL2INT        (OnEndDragLeft);        \
    DEC_PYCALLBACK__BOOL2DBL2INT    (OnDragRight);          \
    DEC_PYCALLBACK__2DBL2INT        (OnBeginDragRight);     \
    DEC_PYCALLBACK__2DBL2INT        (OnEndDragRight);       \
    DEC_PYCALLBACK__DC4DBL          (OnDrawOutline);        \
    DEC_PYCALLBACK__DC              (OnDrawControlPoints);  \
    DEC_PYCALLBACK__DC              (OnEraseControlPoints); \
    DEC_PYCALLBACK__DCBOOL          (OnMoveLink);           \
    DEC_PYCALLBACK__WXCPBOOL2DBL2INT(OnSizingDragLeft);     \
    DEC_PYCALLBACK__WXCP2DBL2INT    (OnSizingBeginDragLeft);\
    DEC_PYCALLBACK__WXCP2DBL2INT    (OnSizingEndDragLeft);  \
    DEC_PYCALLBACK__2DBL            (OnBeginSize);          \
    DEC_PYCALLBACK__2DBL            (OnEndSize);            \
    PYPRIVATE


#define WXSHAPE_IMP_CALLBACKS(CLASS, PARENT)       \
    IMP_PYCALLBACK__                (CLASS, PARENT,  OnDelete);             \
    IMP_PYCALLBACK__DC              (CLASS, PARENT,  OnDraw);               \
    IMP_PYCALLBACK__DC              (CLASS, PARENT,  OnDrawContents);       \
    IMP_PYCALLBACK__DCBOOL          (CLASS, PARENT,  OnDrawBranches);       \
    IMP_PYCALLBACK__DC              (CLASS, PARENT,  OnMoveLinks);          \
    IMP_PYCALLBACK__DC              (CLASS, PARENT,  OnErase);              \
    IMP_PYCALLBACK__DC              (CLASS, PARENT,  OnEraseContents);      \
    IMP_PYCALLBACK__DC              (CLASS, PARENT,  OnHighlight);          \
    IMP_PYCALLBACK__2DBL2INT        (CLASS, PARENT,  OnLeftClick);          \
    IMP_PYCALLBACK__2DBL2INT        (CLASS, PARENT,  OnLeftDoubleClick);    \
    IMP_PYCALLBACK__2DBL2INT        (CLASS, PARENT,  OnRightClick);         \
    IMP_PYCALLBACK__2DBL            (CLASS, PARENT,  OnSize);               \
    IMP_PYCALLBACK_BOOL_DC4DBLBOOL  (CLASS, PARENT,  OnMovePre);            \
    IMP_PYCALLBACK__DC4DBLBOOL      (CLASS, PARENT,  OnMovePost);           \
    IMP_PYCALLBACK__BOOL2DBL2INT    (CLASS, PARENT,  OnDragLeft);           \
    IMP_PYCALLBACK__2DBL2INT        (CLASS, PARENT,  OnBeginDragLeft);      \
    IMP_PYCALLBACK__2DBL2INT        (CLASS, PARENT,  OnEndDragLeft);        \
    IMP_PYCALLBACK__BOOL2DBL2INT    (CLASS, PARENT,  OnDragRight);          \
    IMP_PYCALLBACK__2DBL2INT        (CLASS, PARENT,  OnBeginDragRight);     \
    IMP_PYCALLBACK__2DBL2INT        (CLASS, PARENT,  OnEndDragRight);       \
    IMP_PYCALLBACK__DC4DBL          (CLASS, PARENT,  OnDrawOutline);        \
    IMP_PYCALLBACK__DC              (CLASS, PARENT,  OnDrawControlPoints);  \
    IMP_PYCALLBACK__DC              (CLASS, PARENT,  OnEraseControlPoints); \
    IMP_PYCALLBACK__DCBOOL          (CLASS, PARENT,  OnMoveLink);           \
    IMP_PYCALLBACK__WXCPBOOL2DBL2INT(CLASS, PARENT,  OnSizingDragLeft);     \
    IMP_PYCALLBACK__WXCP2DBL2INT    (CLASS, PARENT,  OnSizingBeginDragLeft);\
    IMP_PYCALLBACK__WXCP2DBL2INT    (CLASS, PARENT,  OnSizingEndDragLeft);  \
    IMP_PYCALLBACK__2DBL            (CLASS, PARENT,  OnBeginSize);          \
    IMP_PYCALLBACK__2DBL            (CLASS, PARENT,  OnEndSize)


    //  This one may be difficult...
    //PYCALLBACK__???????       (PARENT,  OnChangeAttachment);



//---------------------------------------------------------------------------
// These are prototypes of some helper functions found in ogl.i

wxList* wxPy_wxListHelper(PyObject* pyList, const wxChar* className);
wxList* wxPy_wxRealPoint_ListHelper(PyObject* pyList);
PyObject* wxPyMake_wxShapeEvtHandler(wxShapeEvtHandler* source);
PyObject* wxPy_ConvertShapeList(wxListBase* list);
PyObject* wxPy_ConvertRealPointList(wxListBase* list);


//---------------------------------------------------------------------------
// Classes that derive from the shapes and such, but which know how to turn
// virtual callbacks into Python callbacks.

class wxPyShapeCanvas : public wxShapeCanvas {
    DECLARE_DYNAMIC_CLASS(wxPyShapeCanvas);
public:
    wxPyShapeCanvas(wxWindow* parent = NULL, wxWindowID id = -1,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxBORDER,
                    const wxString& name = wxT("shapeCanvas"))
        : wxShapeCanvas(parent, id, pos, size, style, name) {}

    DEC_PYCALLBACK__2DBLINT(OnBeginDragLeft);
    DEC_PYCALLBACK__2DBLINT(OnBeginDragRight);
    DEC_PYCALLBACK__2DBLINT(OnEndDragLeft);
    DEC_PYCALLBACK__2DBLINT(OnEndDragRight);
    DEC_PYCALLBACK__BOOL2DBLINT(OnDragLeft);
    DEC_PYCALLBACK__BOOL2DBLINT(OnDragRight);
    DEC_PYCALLBACK__2DBLINT(OnLeftClick);
    DEC_PYCALLBACK__2DBLINT(OnRightClick);

    PYPRIVATE;
};



class wxPyShapeEvtHandler : public wxShapeEvtHandler {
    DECLARE_DYNAMIC_CLASS(wxPyShapeEvtHandler);
public:
    wxPyShapeEvtHandler(wxShapeEvtHandler *prev = NULL, wxShape *shape = NULL)
        : wxShapeEvtHandler(prev, shape) {}

    WXSHAPE_DEC_CALLBACKS();
};


class wxPyShape : public wxShape {
    DECLARE_ABSTRACT_CLASS(wxPyShape);
public:
    wxPyShape(wxPyShapeCanvas *can = NULL)
        : wxShape(can) {}

    virtual void GetBoundingBoxMin(double *width, double *height)
    {
        if (width) *width = 0.0;
        if (height) *height = 0.0;
    }

    WXSHAPE_DEC_CALLBACKS();

};


class wxPyRectangleShape : public wxRectangleShape {
    DECLARE_DYNAMIC_CLASS(wxPyRectangleShape);
public:
    wxPyRectangleShape(double width = 0.0, double height = 0.0)
        : wxRectangleShape(width, height) {}

    WXSHAPE_DEC_CALLBACKS();
};

class wxPyControlPoint : public wxControlPoint {
    DECLARE_DYNAMIC_CLASS(wxPyControlPoint);
public:
    wxPyControlPoint(wxPyShapeCanvas *the_canvas = NULL,
                     wxPyShape *object = NULL,
                     double size = 0.0, double the_xoffset = 0.0,
                     double the_yoffset = 0.0, int the_type = 0)
        : wxControlPoint(the_canvas, object, size,
                         the_xoffset, the_yoffset, the_type) {}

    WXSHAPE_DEC_CALLBACKS();
};



class wxPyBitmapShape : public wxBitmapShape {
    DECLARE_DYNAMIC_CLASS(wxPyBitmapShape);
public:
    wxPyBitmapShape() : wxBitmapShape() {}

    WXSHAPE_DEC_CALLBACKS();
};



class wxPyDrawnShape : public wxDrawnShape {
    DECLARE_DYNAMIC_CLASS(wxPyDrawnShape);
public:
    wxPyDrawnShape() : wxDrawnShape() {}

    WXSHAPE_DEC_CALLBACKS();
};


class wxPyCompositeShape : public wxCompositeShape {
    DECLARE_DYNAMIC_CLASS(wxPyCompositeShape);
public:
    wxPyCompositeShape() : wxCompositeShape() {}

    WXSHAPE_DEC_CALLBACKS();
};


class wxPyDividedShape : public wxDividedShape {
    DECLARE_DYNAMIC_CLASS(wxPyDividedShape);
public:
    wxPyDividedShape(double width = 0.0, double height = 0.0)
        : wxDividedShape(width, height) {}

    WXSHAPE_DEC_CALLBACKS();
};


class wxPyDivisionShape : public wxDivisionShape {
    DECLARE_DYNAMIC_CLASS(wxPyDivisionShape);
public:
    wxPyDivisionShape() : wxDivisionShape() {}

    WXSHAPE_DEC_CALLBACKS();
};


class wxPyEllipseShape : public wxEllipseShape {
    DECLARE_DYNAMIC_CLASS(wxPyEllipseShape);
public:
    wxPyEllipseShape(double width = 0.0, double height = 0.0)
        : wxEllipseShape(width, height) {}

    WXSHAPE_DEC_CALLBACKS();
};


class wxPyCircleShape : public wxCircleShape {
    DECLARE_DYNAMIC_CLASS(wxPyCircleShape);
public:
    wxPyCircleShape(double width = 0.0)
        : wxCircleShape(width) {}

    WXSHAPE_DEC_CALLBACKS();
};


class wxPyLineShape : public wxLineShape {
    DECLARE_DYNAMIC_CLASS(wxPyLineShape);
public:
    wxPyLineShape() : wxLineShape() {}

    WXSHAPE_DEC_CALLBACKS();
};


class wxPyPolygonShape : public wxPolygonShape {
    DECLARE_DYNAMIC_CLASS(wxPyPolygonShape);
public:
    wxPyPolygonShape() : wxPolygonShape() {}

    WXSHAPE_DEC_CALLBACKS();
};


class wxPyTextShape : public wxTextShape {
    DECLARE_DYNAMIC_CLASS(wxPyTextShape);
public:
    wxPyTextShape(double width = 0.0, double height = 0.0)
        : wxTextShape(width, height) {}

    WXSHAPE_DEC_CALLBACKS();
};



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif


