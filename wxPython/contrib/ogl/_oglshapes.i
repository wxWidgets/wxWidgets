/////////////////////////////////////////////////////////////////////////////
// Name:        oglshapes.i
// Purpose:     SWIG definitions for the wxWindows Object Graphics Library
//
// Author:      Robin Dunn
//
// Created:     3-Sept-1999
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


// not a %module


//---------------------------------------------------------------------------

enum {
    oglMETAFLAGS_OUTLINE,
    oglMETAFLAGS_ATTACHMENTS
};

//---------------------------------------------------------------------------

%typemap(in) (int points, wxPoint* points_array ) {
    $2 = wxPoint_LIST_helper($input, &$1);
    if ($2 == NULL) SWIG_fail;
}
%typemap(freearg) (int points, wxPoint* points_array ) {
    if ($2) delete [] $2;
}

//---------------------------------------------------------------------------

class wxPseudoMetaFile : public wxObject {
public:
  wxPseudoMetaFile();
  ~wxPseudoMetaFile();

    void Draw(wxDC& dc, double xoffset, double yoffset);
#ifdef wxUSE_PROLOGIO
    void WriteAttributes(wxExpr *clause, int whichAngle);
    void ReadAttributes(wxExpr *clause, int whichAngle);
#endif
    void Clear();
    void Copy(wxPseudoMetaFile& copy);
    void Scale(double sx, double sy);
    void ScaleTo(double w, double h);
    void Translate(double x, double y);
    void Rotate(double x, double y, double theta);
    bool LoadFromMetaFile(const wxString& filename, double *width, double *height);
    void GetBounds(double *minX, double *minY, double *maxX, double *maxY);
    void CalculateSize(wxPyDrawnShape* shape);

    // ****  fix these...  is it even possible?  these are lists of various GDI opperations (not the objects...)
    // wxList& GetOutlineColours();
    // wxList& GetFillColours();
    // wxList& GetOps();

    void SetRotateable(bool rot);
    bool GetRotateable();
    void SetSize(double w, double h);
    void SetFillBrush(wxBrush* brush);
    wxBrush* GetFillBrush();
    void SetOutlinePen(wxPen* pen);
    wxPen* GetOutlinePen();
    void SetOutlineOp(int op);
    int GetOutlineOp();


    bool IsValid();
    void DrawLine(const wxPoint& pt1, const wxPoint& pt2);
    void DrawRectangle(const wxRect& rect);
    void DrawRoundedRectangle(const wxRect& rect, double radius);
    void DrawArc(const wxPoint& centrePt, const wxPoint& startPt, const wxPoint& endPt);
    void DrawEllipticArc(const wxRect& rect, double startAngle, double endAngle);
    void DrawEllipse(const wxRect& rect);
    void DrawPoint(const wxPoint& pt);
    void DrawText(const wxString& text, const wxPoint& pt);
    void DrawLines(int points, wxPoint* points_array);
    void DrawPolygon(int points, wxPoint* points_array, int flags = 0);
    void DrawSpline(int points, wxPoint* points_array);
    void SetClippingRect(const wxRect& rect);
    void DestroyClippingRect();
    void SetPen(wxPen* pen, bool isOutline = FALSE);
    void SetBrush(wxBrush* brush, bool isFill = FALSE);
    void SetFont(wxFont* font);
    void SetTextColour(const wxColour& colour);
    void SetBackgroundColour(const wxColour& colour);
    void SetBackgroundMode(int mode);
};


//---------------------------------------------------------------------------

%{
// Using this macro can sometimes provoke an Internal Compiler Error in MSVC
// 6, so instead I'l just expand it out by hand...
//    WXSHAPE_IMP_CALLBACKS(wxPyRectangleShape, wxRectangleShape);

    IMP_PYCALLBACK__                (wxPyRectangleShape, wxRectangleShape,  OnDelete);
    IMP_PYCALLBACK__DC              (wxPyRectangleShape, wxRectangleShape,  OnDraw);
    IMP_PYCALLBACK__DC              (wxPyRectangleShape, wxRectangleShape,  OnDrawContents);
    IMP_PYCALLBACK__DCBOOL          (wxPyRectangleShape, wxRectangleShape,  OnDrawBranches);
    IMP_PYCALLBACK__DC              (wxPyRectangleShape, wxRectangleShape,  OnMoveLinks);
    IMP_PYCALLBACK__DC              (wxPyRectangleShape, wxRectangleShape,  OnErase);
    IMP_PYCALLBACK__DC              (wxPyRectangleShape, wxRectangleShape,  OnEraseContents);
    IMP_PYCALLBACK__DC              (wxPyRectangleShape, wxRectangleShape,  OnHighlight);
    IMP_PYCALLBACK__2DBL2INT        (wxPyRectangleShape, wxRectangleShape,  OnLeftClick);
    IMP_PYCALLBACK__2DBL2INT        (wxPyRectangleShape, wxRectangleShape,  OnLeftDoubleClick);
    IMP_PYCALLBACK__2DBL2INT        (wxPyRectangleShape, wxRectangleShape,  OnRightClick);
    IMP_PYCALLBACK__2DBL            (wxPyRectangleShape, wxRectangleShape,  OnSize);
    IMP_PYCALLBACK_BOOL_DC4DBLBOOL  (wxPyRectangleShape, wxRectangleShape,  OnMovePre);
    IMP_PYCALLBACK__DC4DBLBOOL      (wxPyRectangleShape, wxRectangleShape,  OnMovePost);
    IMP_PYCALLBACK__BOOL2DBL2INT    (wxPyRectangleShape, wxRectangleShape,  OnDragLeft);
    IMP_PYCALLBACK__2DBL2INT        (wxPyRectangleShape, wxRectangleShape,  OnBeginDragLeft);
    IMP_PYCALLBACK__2DBL2INT        (wxPyRectangleShape, wxRectangleShape,  OnEndDragLeft);
    IMP_PYCALLBACK__BOOL2DBL2INT    (wxPyRectangleShape, wxRectangleShape,  OnDragRight);
    IMP_PYCALLBACK__2DBL2INT        (wxPyRectangleShape, wxRectangleShape,  OnBeginDragRight);
    IMP_PYCALLBACK__2DBL2INT        (wxPyRectangleShape, wxRectangleShape,  OnEndDragRight);
    IMP_PYCALLBACK__DC4DBL          (wxPyRectangleShape, wxRectangleShape,  OnDrawOutline);
    IMP_PYCALLBACK__DC              (wxPyRectangleShape, wxRectangleShape,  OnDrawControlPoints);
    IMP_PYCALLBACK__DC              (wxPyRectangleShape, wxRectangleShape,  OnEraseControlPoints);
    IMP_PYCALLBACK__DCBOOL          (wxPyRectangleShape, wxRectangleShape,  OnMoveLink);
    IMP_PYCALLBACK__WXCPBOOL2DBL2INT(wxPyRectangleShape, wxRectangleShape,  OnSizingDragLeft);
    IMP_PYCALLBACK__WXCP2DBL2INT    (wxPyRectangleShape, wxRectangleShape,  OnSizingBeginDragLeft);
    IMP_PYCALLBACK__WXCP2DBL2INT    (wxPyRectangleShape, wxRectangleShape,  OnSizingEndDragLeft);
    IMP_PYCALLBACK__2DBL            (wxPyRectangleShape, wxRectangleShape,  OnBeginSize);
    IMP_PYCALLBACK__2DBL            (wxPyRectangleShape, wxRectangleShape,  OnEndSize)
        
%}

class wxPyRectangleShape : public wxPyShape {
public:
    %pythonAppend wxPyRectangleShape "self._setOORandCallbackInfo(PyRectangleShape)"
    
    wxPyRectangleShape(double width = 0.0, double height = 0.0);
    void _setCallbackInfo(PyObject* self, PyObject* _class);

    void SetCornerRadius(double radius);
    double GetCornerRadius();
    
    void base_OnDelete();
    void base_OnDraw(wxDC& dc);
    void base_OnDrawContents(wxDC& dc);
    void base_OnDrawBranches(wxDC& dc, bool erase = FALSE);
    void base_OnMoveLinks(wxDC& dc);
    void base_OnErase(wxDC& dc);
    void base_OnEraseContents(wxDC& dc);
    void base_OnHighlight(wxDC& dc);
    void base_OnLeftClick(double x, double y, int keys = 0, int attachment = 0);
    void base_OnLeftDoubleClick(double x, double y, int keys = 0, int attachment = 0);
    void base_OnRightClick(double x, double y, int keys = 0, int attachment = 0);
    void base_OnSize(double x, double y);
    bool base_OnMovePre(wxDC& dc, double x, double y, double old_x, double old_y, bool display = true);
    void base_OnMovePost(wxDC& dc, double x, double y, double old_x, double old_y, bool display = true);
    void base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnDrawOutline(wxDC& dc, double x, double y, double w, double h);
    void base_OnDrawControlPoints(wxDC& dc);
    void base_OnEraseControlPoints(wxDC& dc);
    void base_OnMoveLink(wxDC& dc, bool moveControlPoints = true);
    void base_OnSizingDragLeft(wxPyControlPoint* pt, bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnSizingBeginDragLeft(wxPyControlPoint* pt, double x, double y, int keys=0, int attachment = 0);
    void base_OnSizingEndDragLeft(wxPyControlPoint* pt, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginSize(double w, double h);
    void base_OnEndSize(double w, double h);
//    void base_OnChangeAttachment(int attachment, wxPyLineShape* line, wxList& ordering);
};

//---------------------------------------------------------------------------

%{
    WXSHAPE_IMP_CALLBACKS(wxPyControlPoint, wxControlPoint);
%}

class wxPyControlPoint : public wxPyRectangleShape {
public:
    %pythonAppend wxPyControlPoint "self._setOORandCallbackInfo(PyControlPoint)"
    
    wxPyControlPoint(wxPyShapeCanvas *the_canvas = NULL,
                     wxPyShape *object = NULL,
                     double size = 0.0, double the_xoffset = 0.0,
                     double the_yoffset = 0.0, int the_type = 0);

    void _setCallbackInfo(PyObject* self, PyObject* _class);

    void SetCornerRadius(double radius);

    void base_OnDelete();
    void base_OnDraw(wxDC& dc);
    void base_OnDrawContents(wxDC& dc);
    void base_OnDrawBranches(wxDC& dc, bool erase = FALSE);
    void base_OnMoveLinks(wxDC& dc);
    void base_OnErase(wxDC& dc);
    void base_OnEraseContents(wxDC& dc);
    void base_OnHighlight(wxDC& dc);
    void base_OnLeftClick(double x, double y, int keys = 0, int attachment = 0);
    void base_OnLeftDoubleClick(double x, double y, int keys = 0, int attachment = 0);
    void base_OnRightClick(double x, double y, int keys = 0, int attachment = 0);
    void base_OnSize(double x, double y);
    bool base_OnMovePre(wxDC& dc, double x, double y, double old_x, double old_y, bool display = true);
    void base_OnMovePost(wxDC& dc, double x, double y, double old_x, double old_y, bool display = true);
    void base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnDrawOutline(wxDC& dc, double x, double y, double w, double h);
    void base_OnDrawControlPoints(wxDC& dc);
    void base_OnEraseControlPoints(wxDC& dc);
    void base_OnMoveLink(wxDC& dc, bool moveControlPoints = true);
    void base_OnSizingDragLeft(wxPyControlPoint* pt, bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnSizingBeginDragLeft(wxPyControlPoint* pt, double x, double y, int keys=0, int attachment = 0);
    void base_OnSizingEndDragLeft(wxPyControlPoint* pt, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginSize(double w, double h);
    void base_OnEndSize(double w, double h);
//    void base_OnChangeAttachment(int attachment, wxPyLineShape* line, wxList& ordering);
};

//---------------------------------------------------------------------------
%{
    WXSHAPE_IMP_CALLBACKS(wxPyBitmapShape, wxBitmapShape);
%}


class wxPyBitmapShape : public wxPyRectangleShape {
public:
    %pythonAppend wxPyBitmapShape "self._setOORandCallbackInfo(PyBitmapShape)"
    
    wxPyBitmapShape();

    void _setCallbackInfo(PyObject* self, PyObject* _class);

    wxBitmap& GetBitmap();
    wxString GetFilename();
    void SetBitmap(const wxBitmap& bitmap);
    void SetFilename(const wxString& filename);

    void base_OnDelete();
    void base_OnDraw(wxDC& dc);
    void base_OnDrawContents(wxDC& dc);
    void base_OnDrawBranches(wxDC& dc, bool erase = FALSE);
    void base_OnMoveLinks(wxDC& dc);
    void base_OnErase(wxDC& dc);
    void base_OnEraseContents(wxDC& dc);
    void base_OnHighlight(wxDC& dc);
    void base_OnLeftClick(double x, double y, int keys = 0, int attachment = 0);
    void base_OnLeftDoubleClick(double x, double y, int keys = 0, int attachment = 0);
    void base_OnRightClick(double x, double y, int keys = 0, int attachment = 0);
    void base_OnSize(double x, double y);
    bool base_OnMovePre(wxDC& dc, double x, double y, double old_x, double old_y, bool display = true);
    void base_OnMovePost(wxDC& dc, double x, double y, double old_x, double old_y, bool display = true);
    void base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnDrawOutline(wxDC& dc, double x, double y, double w, double h);
    void base_OnDrawControlPoints(wxDC& dc);
    void base_OnEraseControlPoints(wxDC& dc);
    void base_OnMoveLink(wxDC& dc, bool moveControlPoints = true);
    void base_OnSizingDragLeft(wxPyControlPoint* pt, bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnSizingBeginDragLeft(wxPyControlPoint* pt, double x, double y, int keys=0, int attachment = 0);
    void base_OnSizingEndDragLeft(wxPyControlPoint* pt, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginSize(double w, double h);
    void base_OnEndSize(double w, double h);
//    void base_OnChangeAttachment(int attachment, wxPyLineShape* line, wxList& ordering);
};

//---------------------------------------------------------------------------
%{
    WXSHAPE_IMP_CALLBACKS(wxPyDrawnShape, wxDrawnShape);
%}

class wxPyDrawnShape : public wxPyRectangleShape {
public:
    %pythonAppend wxPyDrawnShape "self._setOORandCallbackInfo(PyDrawnShape)"
    
    wxPyDrawnShape();

    void _setCallbackInfo(PyObject* self, PyObject* _class);

    void CalculateSize();
    void DestroyClippingRect();
    void DrawArc(const wxPoint& centrePoint, const wxPoint& startPoint,
                 const wxPoint& endPoint);
    void DrawAtAngle(int angle);
    void DrawEllipticArc(const wxRect& rect, double startAngle, double endAngle);
    void DrawLine(const wxPoint& point1, const wxPoint& point2);
    void DrawLines(int points, wxPoint* points_array);
    void DrawPoint(const wxPoint& point);
    
    void DrawPolygon(int points, wxPoint* points_array, int flags = 0);
    
    void DrawRectangle(const wxRect& rect);
    void DrawRoundedRectangle(const wxRect& rect, double radius);
    void DrawSpline(int points, wxPoint* points_array);
    void DrawText(const wxString& text, const wxPoint& point);
    int GetAngle();

    wxPseudoMetaFile& GetMetaFile();

    double GetRotation();
    bool LoadFromMetaFile(const wxString& filename);
    void Rotate(double x, double y, double theta);
    void SetClippingRect(const wxRect& rect);
    void SetDrawnBackgroundColour(const wxColour& colour);
    void SetDrawnBackgroundMode(int mode);
    void SetDrawnBrush(wxBrush* pen, bool isOutline = FALSE);
    void SetDrawnFont(wxFont* font);
    void SetDrawnPen(wxPen* pen, bool isOutline = FALSE);
    void SetDrawnTextColour(const wxColour& colour);
    void Scale(double sx, double sy);
    void SetSaveToFile(bool save);
    void Translate(double x, double y);


    void base_OnDelete();
    void base_OnDraw(wxDC& dc);
    void base_OnDrawContents(wxDC& dc);
    void base_OnDrawBranches(wxDC& dc, bool erase = FALSE);
    void base_OnMoveLinks(wxDC& dc);
    void base_OnErase(wxDC& dc);
    void base_OnEraseContents(wxDC& dc);
    void base_OnHighlight(wxDC& dc);
    void base_OnLeftClick(double x, double y, int keys = 0, int attachment = 0);
    void base_OnLeftDoubleClick(double x, double y, int keys = 0, int attachment = 0);
    void base_OnRightClick(double x, double y, int keys = 0, int attachment = 0);
    void base_OnSize(double x, double y);
    bool base_OnMovePre(wxDC& dc, double x, double y, double old_x, double old_y, bool display = true);
    void base_OnMovePost(wxDC& dc, double x, double y, double old_x, double old_y, bool display = true);
    void base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnDrawOutline(wxDC& dc, double x, double y, double w, double h);
    void base_OnDrawControlPoints(wxDC& dc);
    void base_OnEraseControlPoints(wxDC& dc);
    void base_OnMoveLink(wxDC& dc, bool moveControlPoints = true);
    void base_OnSizingDragLeft(wxPyControlPoint* pt, bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnSizingBeginDragLeft(wxPyControlPoint* pt, double x, double y, int keys=0, int attachment = 0);
    void base_OnSizingEndDragLeft(wxPyControlPoint* pt, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginSize(double w, double h);
    void base_OnEndSize(double w, double h);
//    void base_OnChangeAttachment(int attachment, wxPyLineShape* line, wxList& ordering);

};


//---------------------------------------------------------------------------

class wxOGLConstraint : public wxObject {
public:
    //wxOGLConstraint(int type, wxPyShape *constraining, wxList& constrained);
    %extend {
        wxOGLConstraint(int type, wxPyShape *constraining, PyObject* constrained) {
            wxList* list = wxPy_wxListHelper(constrained, wxT("wxPyShape"));
            wxOGLConstraint* rv = new wxOGLConstraint(type, constraining, *list);
            delete list;
            return rv;
        }
    }

    //~wxOGLConstraint();  The wxCompositShape takes ownership of the constraint

    bool Evaluate();
    void SetSpacing(double x, double y);
    bool Equals(double a, double b);

};



//---------------------------------------------------------------------------

%{
    WXSHAPE_IMP_CALLBACKS(wxPyCompositeShape, wxCompositeShape);
%}

class wxPyCompositeShape : public wxPyRectangleShape {
public:
    %pythonAppend wxPyCompositeShape "self._setOORandCallbackInfo(PyCompositeShape)"
    
    wxPyCompositeShape();

    void _setCallbackInfo(PyObject* self, PyObject* _class);
    
    void AddChild(wxPyShape *child, wxPyShape *addAfter = NULL);
    wxOGLConstraint * AddConstraint(wxOGLConstraint *constraint);


    //wxOGLConstraint * AddConstraint(int type,
    //                                wxPyShape *constraining,
    //                                wxList& constrained);
    %extend {
        wxOGLConstraint * AddConstrainedShapes(int type, wxPyShape *constraining,
                                               PyObject* constrained) {
            wxList* list = wxPy_wxListHelper(constrained, wxT("wxPyShape"));
            wxOGLConstraint* rv = self->AddConstraint(type, constraining, *list);
            delete list;
            return rv;
        }
    }

    %name(AddSimpleConstraint)wxOGLConstraint* AddConstraint(int type,
                                                             wxPyShape *constraining,
                                                             wxPyShape *constrained);

    void CalculateSize();
    bool ContainsDivision(wxPyDivisionShape *division);
    void DeleteConstraint(wxOGLConstraint *constraint);
    void DeleteConstraintsInvolvingChild(wxPyShape *child);

    // ****  Needs an output typemap
    //wxOGLConstraint * FindConstraint(long id, wxPyCompositeShape **actualComposite);

    wxPyShape * FindContainerImage();

    // wxList& GetConstraints();
    // wxList& GetDivisions();
    %extend {
        PyObject* GetConstraints() {
            wxList& list = self->GetConstraints();
            return wxPy_ConvertList(&list);
        }

        PyObject* GetDivisions() {
            wxList& list = self->GetDivisions();
            return wxPy_ConvertShapeList(&list);
        }
    }

    void MakeContainer();
    bool Recompute();
    void RemoveChild(wxPyShape *child);


    void base_OnDelete();
    void base_OnDraw(wxDC& dc);
    void base_OnDrawContents(wxDC& dc);
    void base_OnDrawBranches(wxDC& dc, bool erase = FALSE);
    void base_OnMoveLinks(wxDC& dc);
    void base_OnErase(wxDC& dc);
    void base_OnEraseContents(wxDC& dc);
    void base_OnHighlight(wxDC& dc);
    void base_OnLeftClick(double x, double y, int keys = 0, int attachment = 0);
    void base_OnLeftDoubleClick(double x, double y, int keys = 0, int attachment = 0);
    void base_OnRightClick(double x, double y, int keys = 0, int attachment = 0);
    void base_OnSize(double x, double y);
    bool base_OnMovePre(wxDC& dc, double x, double y, double old_x, double old_y, bool display = true);
    void base_OnMovePost(wxDC& dc, double x, double y, double old_x, double old_y, bool display = true);
    void base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnDrawOutline(wxDC& dc, double x, double y, double w, double h);
    void base_OnDrawControlPoints(wxDC& dc);
    void base_OnEraseControlPoints(wxDC& dc);
    void base_OnMoveLink(wxDC& dc, bool moveControlPoints = true);
    void base_OnSizingDragLeft(wxPyControlPoint* pt, bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnSizingBeginDragLeft(wxPyControlPoint* pt, double x, double y, int keys=0, int attachment = 0);
    void base_OnSizingEndDragLeft(wxPyControlPoint* pt, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginSize(double w, double h);
    void base_OnEndSize(double w, double h);
//    void base_OnChangeAttachment(int attachment, wxPyLineShape* line, wxList& ordering);

};


//---------------------------------------------------------------------------

%{
    WXSHAPE_IMP_CALLBACKS(wxPyDividedShape, wxDividedShape);
%}

class wxPyDividedShape : public wxPyRectangleShape {
public:
    %pythonAppend wxPyDividedShape "self._setOORandCallbackInfo(PyDividedShape)"
    
    wxPyDividedShape(double width = 0.0, double height = 0.0);

    void _setCallbackInfo(PyObject* self, PyObject* _class);

    void EditRegions();
    void SetRegionSizes();

    void base_OnDelete();
    void base_OnDraw(wxDC& dc);
    void base_OnDrawContents(wxDC& dc);
    void base_OnDrawBranches(wxDC& dc, bool erase = FALSE);
    void base_OnMoveLinks(wxDC& dc);
    void base_OnErase(wxDC& dc);
    void base_OnEraseContents(wxDC& dc);
    void base_OnHighlight(wxDC& dc);
    void base_OnLeftClick(double x, double y, int keys = 0, int attachment = 0);
    void base_OnLeftDoubleClick(double x, double y, int keys = 0, int attachment = 0);
    void base_OnRightClick(double x, double y, int keys = 0, int attachment = 0);
    void base_OnSize(double x, double y);
    bool base_OnMovePre(wxDC& dc, double x, double y, double old_x, double old_y, bool display = true);
    void base_OnMovePost(wxDC& dc, double x, double y, double old_x, double old_y, bool display = true);
    void base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnDrawOutline(wxDC& dc, double x, double y, double w, double h);
    void base_OnDrawControlPoints(wxDC& dc);
    void base_OnEraseControlPoints(wxDC& dc);
    void base_OnMoveLink(wxDC& dc, bool moveControlPoints = true);
    void base_OnSizingDragLeft(wxPyControlPoint* pt, bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnSizingBeginDragLeft(wxPyControlPoint* pt, double x, double y, int keys=0, int attachment = 0);
    void base_OnSizingEndDragLeft(wxPyControlPoint* pt, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginSize(double w, double h);
    void base_OnEndSize(double w, double h);
//    void base_OnChangeAttachment(int attachment, wxPyLineShape* line, wxList& ordering);

};


//---------------------------------------------------------------------------
%{
    WXSHAPE_IMP_CALLBACKS(wxPyDivisionShape, wxDivisionShape);
%}

class wxPyDivisionShape : public wxPyCompositeShape {
public:
    %pythonAppend wxPyDivisionShape "self._setOORandCallbackInfo(PyDivisionShape)"
    %typemap(out) wxPyDivisionShape*;    // turn off this typemap
    
    wxPyDivisionShape();

    %typemap(out) wxPyDivisionShape*        { $result = wxPyMake_wxShapeEvtHandler($1, $owner); }

    void _setCallbackInfo(PyObject* self, PyObject* _class);

    void AdjustBottom(double bottom, bool test);
    void AdjustLeft(double left, bool test);
    void AdjustRight(double right, bool test);
    void AdjustTop(double top, bool test);
    void Divide(int direction);
    void EditEdge(int side);
    wxPyDivisionShape * GetBottomSide();
    int GetHandleSide();
    wxPyDivisionShape * GetLeftSide();
    wxString GetLeftSideColour();
    wxPen * GetLeftSidePen();
    wxPyDivisionShape * GetRightSide();
    wxPyDivisionShape * GetTopSide();
    wxPen * GetTopSidePen();
    void ResizeAdjoining(int side, double newPos, bool test);
    void PopupMenu(double x, double y);
    void SetBottomSide(wxPyDivisionShape *shape);
    void SetHandleSide(int side);
    void SetLeftSide(wxPyDivisionShape *shape);
    void SetLeftSideColour(const wxString& colour);
    void SetLeftSidePen(wxPen *pen);
    void SetRightSide(wxPyDivisionShape *shape);
    void SetTopSide(wxPyDivisionShape *shape);
    void SetTopSideColour(const wxString& colour);
    void SetTopSidePen(wxPen *pen);



    void base_OnDelete();
    void base_OnDraw(wxDC& dc);
    void base_OnDrawContents(wxDC& dc);
    void base_OnDrawBranches(wxDC& dc, bool erase = FALSE);
    void base_OnMoveLinks(wxDC& dc);
    void base_OnErase(wxDC& dc);
    void base_OnEraseContents(wxDC& dc);
    void base_OnHighlight(wxDC& dc);
    void base_OnLeftClick(double x, double y, int keys = 0, int attachment = 0);
    void base_OnLeftDoubleClick(double x, double y, int keys = 0, int attachment = 0);
    void base_OnRightClick(double x, double y, int keys = 0, int attachment = 0);
    void base_OnSize(double x, double y);
    bool base_OnMovePre(wxDC& dc, double x, double y, double old_x, double old_y, bool display = true);
    void base_OnMovePost(wxDC& dc, double x, double y, double old_x, double old_y, bool display = true);
    void base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnDrawOutline(wxDC& dc, double x, double y, double w, double h);
    void base_OnDrawControlPoints(wxDC& dc);
    void base_OnEraseControlPoints(wxDC& dc);
    void base_OnMoveLink(wxDC& dc, bool moveControlPoints = true);
    void base_OnSizingDragLeft(wxPyControlPoint* pt, bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnSizingBeginDragLeft(wxPyControlPoint* pt, double x, double y, int keys=0, int attachment = 0);
    void base_OnSizingEndDragLeft(wxPyControlPoint* pt, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginSize(double w, double h);
    void base_OnEndSize(double w, double h);
//    void base_OnChangeAttachment(int attachment, wxPyLineShape* line, wxList& ordering);

};



//---------------------------------------------------------------------------

