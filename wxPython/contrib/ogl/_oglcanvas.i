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


// not a %module


//---------------------------------------------------------------------------
%{
    // Put some wx default wxChar* values into wxStrings.
    DECLARE_DEF_STRING(ShapeCanvasNameStr);
%}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class wxDiagram : public wxObject {
public:
    wxDiagram();
    //~wxDiagram();

    void AddShape(wxPyShape* shape, wxPyShape *addAfter = NULL);
    void Clear(wxDC& dc);
    void DeleteAllShapes();
    void DrawOutline(wxDC& dc, double x1, double y1, double x2, double y2);
    wxPyShape* FindShape(long id);
    wxPyShapeCanvas* GetCanvas();
    int GetCount();
    double GetGridSpacing();
    int GetMouseTolerance();

    // wxList* GetShapeList();
    %extend {
        PyObject* GetShapeList() {
            wxList* list = self->GetShapeList();
            return wxPy_ConvertShapeList(list);
        }
    }

    bool GetQuickEditMode();
    bool GetSnapToGrid();
    void InsertShape(wxPyShape *shape);

#ifdef wxUSE_PROLOGIO
    bool LoadFile(const wxString& filename);
    bool SaveFile(const wxString& filename);
#endif

#ifdef wxUSE_PROLOGIO
    // **** Have to deal with wxExpr and wxExprDatabase first...
    //void OnDatabaseLoad(wxExprDatabase& database);
    //void OnDatabaseSave(wxExprDatabase& database);
    //bool OnHeaderLoad(wxExprDatabase& database, wxExpr& expr);
    //bool OnHeaderSave(wxExprDatabase& database, wxExpr& expr);
    //bool OnShapeLoad(wxExprDatabase& database, wxPyShape& shape, wxExpr& expr);
    //bool OnShapeSave(wxExprDatabase& database, wxPyShape& shape, wxExpr& expr);
#endif

#ifdef wxUSE_PROLOGIO
    void ReadContainerGeometry(wxExprDatabase& database);
    void ReadLines(wxExprDatabase& database);
    void ReadNodes(wxExprDatabase& database);
#endif

    void RecentreAll(wxDC& dc);
    void Redraw(wxDC& dc);
    void RemoveAllShapes();
    void RemoveShape(wxPyShape* shape);
    void SetCanvas(wxPyShapeCanvas* canvas);
    void SetGridSpacing(double spacing);
    void SetMouseTolerance(int tolerance);
    void SetQuickEditMode(bool mode);
    void SetSnapToGrid(bool snap);
    void ShowAll(bool show);
    void Snap(double *INOUT, double *INOUT);

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


MustHaveApp(wxPyShapeCanvas);

class wxPyShapeCanvas : public wxScrolledWindow {
public:
    %pythonAppend wxPyShapeCanvas "self._setOORandCallbackInfo(PyShapeCanvas)"
    %typemap(out) wxPyShapeCanvas*;    // turn off this typemap

    wxPyShapeCanvas(wxWindow* parent = NULL, wxWindowID id = -1,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxBORDER,
                    const wxString& name = wxPyShapeCanvasNameStr);

    %typemap(out) wxPyShapeCanvas*          { $result = wxPyMake_wxObject($1, $owner); }
    
    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pythoncode {
    def _setOORandCallbackInfo(self, _class):
        self._setOORInfo(self)
        self._setCallbackInfo(self, _class)
    }

    void AddShape(wxPyShape *shape, wxPyShape *addAfter = NULL);

    // ****  Need a typemap for wxClassInfo, or fully implement wxObject, etc.....
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
    void Snap(double *INOUT, double *INOUT);


    %pythoncode {
    def GetShapeList(self):
        return self.GetDiagram().GetShapeList()
    }
};


//---------------------------------------------------------------------------


