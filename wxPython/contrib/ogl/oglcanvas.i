/////////////////////////////////////////////////////////////////////////////
// Name:        oglcanvas.i
// Purpose:     SWIG definitions for the wxWindows Object Graphics Library
//
// Author:      Robin Dunn
//
// Created:     7-Sept-1999
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


%module oglcanvas

%{
#include "export.h"
#include "oglhelpers.h"
%}

//---------------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

%extern wx.i
%import windows.i
%extern _defs.i
%extern misc.i
%extern gdi.i

%include _ogldefs.i

%extern oglbasic.i


%pragma(python) code = "import wx"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class wxDiagram {
public:
    wxDiagram();
    ~wxDiagram();

    void AddShape(wxPyShape*shape, wxPyShape *addAfter = NULL);
    void Clear(wxDC& dc);
    void DeleteAllShapes();
    void DrawOutline(wxDC& dc, double x1, double y1, double x2, double y2);
    wxPyShape* FindShape(long id);
    wxPyShapeCanvas* GetCanvas();
    int GetCount();
    double GetGridSpacing();
    int GetMouseTolerance();

    // wxList* GetShapeList();
    %addmethods {
        PyObject* GetShapeList() {
            wxList* list = self->GetShapeList();
            return wxPy_ConvertList(list, "wxPyShape");
        }
    }

    bool GetQuickEditMode();
    bool GetSnapToGrid();
    void InsertShape(wxPyShape *shape);
    bool LoadFile(const wxString& filename);

    // **** Have to deal with wxExpr and wxExprDatabase first...
    //void OnDatabaseLoad(wxExprDatabase& database);
    //void OnDatabaseSave(wxExprDatabase& database);
    //bool OnHeaderLoad(wxExprDatabase& database, wxExpr& expr);
    //bool OnHeaderSave(wxExprDatabase& database, wxExpr& expr);
    //bool OnShapeLoad(wxExprDatabase& database, wxPyShape& shape, wxExpr& expr);
    //bool OnShapeSave(wxExprDatabase& database, wxPyShape& shape, wxExpr& expr);


    void ReadContainerGeometry(wxExprDatabase& database);
    void ReadLines(wxExprDatabase& database);
    void ReadNodes(wxExprDatabase& database);
    void RecentreAll(wxDC& dc);
    void Redraw(wxDC& dc);
    void RemoveAllShapes();
    void RemoveShape(wxPyShape* shape);
    bool SaveFile(const wxString& filename);
    void SetCanvas(wxPyShapeCanvas* canvas);
    void SetGridSpacing(double spacing);
    void SetMouseTolerance(int tolerance);
    void SetQuickEditMode(bool mode);
    void SetSnapToGrid(bool snap);
    void ShowAll(bool show);
    void Snap(double *OUTPUT, double *OUTPUT);

};

//---------------------------------------------------------------------------

%{
    IMP_PYCALLBACK__2DBLINT(wxPyShapeCanvas, wxShapeCanvas, OnBeginDragLeft);
    IMP_PYCALLBACK__2DBLINT(wxPyShapeCanvas, wxShapeCanvas, OnBeginDragRight);
    IMP_PYCALLBACK__2DBLINT(wxPyShapeCanvas, wxShapeCanvas, OnEndDragLeft);
    IMP_PYCALLBACK__2DBLINT(wxPyShapeCanvas, wxShapeCanvas, OnEndDragRight);
    IMP_PYCALLBACK__BOOL2DBLINT(wxPyShapeCanvas, wxShapeCanvas, OnDragLeft);
    IMP_PYCALLBACK__BOOL2DBLINT(wxPyShapeCanvas, wxShapeCanvas, OnDragRight);
    IMP_PYCALLBACK__2DBLINT(wxPyShapeCanvas, wxShapeCanvas, OnLeftClick);
    IMP_PYCALLBACK__2DBLINT(wxPyShapeCanvas, wxShapeCanvas, OnRightClick);
%}


class wxPyShapeCanvas : public wxScrolledWindow {
public:
    wxPyShapeCanvas(wxWindow* parent = NULL, wxWindowID id = -1,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxBORDER);

    void _setSelf(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setSelf(self, wxPyShapeCanvas)"

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"
    %pragma(python) addtomethod = "__init__:#wx._StdOnScrollCallbacks(self)"

    void AddShape(wxPyShape *shape, wxPyShape *addAfter = NULL);

    // ****  Need a typemap for wxClassInfo, or implement wxObject, etc.....
    wxPyShape* FindShape(double x1, double y, int *OUTPUT,
                       wxClassInfo *info = NULL, wxPyShape* notImage = NULL);

    wxPyShape * FindFirstSensitiveShape(double x1, double y, int *OUTPUT, int op);
    wxDiagram* GetDiagram();

    bool GetQuickEditMode();
    void InsertShape(wxPyShape* shape);


    void base_OnBeginDragLeft(double x, double y, int keys = 0);
    void base_OnBeginDragRight(double x, double y, int keys = 0);
    void base_OnEndDragLeft(double x, double y, int keys = 0);
    void base_OnEndDragRight(double x, double y, int keys = 0);
    void base_OnDragLeft(bool draw, double x, double y, int keys = 0);
    void base_OnDragRight(bool draw, double x, double y, int keys = 0);
    void base_OnLeftClick(double x, double y, int keys = 0);
    void base_OnRightClick(double x, double y, int keys = 0);

    void Redraw(wxDC& dc);
    void RemoveShape(wxPyShape *shape);
    void SetDiagram(wxDiagram *diagram);
    void Snap(double *OUTPUT, double *OUTPUT);

};


//---------------------------------------------------------------------------


