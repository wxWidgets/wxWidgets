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


%module oglshapes

%{
#include "helpers.h"
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

%import oglbasic.i


%pragma(python) code = "import wx"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class wxPseudoMetaFile {
public:
  wxPseudoMetaFile();
  ~wxPseudoMetaFile();

    void Draw(wxDC& dc, double xoffset, double yoffset);

    void WriteAttributes(wxExpr *clause, int whichAngle);
    void ReadAttributes(wxExpr *clause, int whichAngle);
    void Clear();
    void Copy(wxPseudoMetaFile& copy);
    void Scale(double sx, double sy);
    void ScaleTo(double w, double h);
    void Translate(double x, double y);
    void Rotate(double x, double y, double theta);
    bool LoadFromMetaFile(char* filename, double *width, double *height);
    void GetBounds(double *minX, double *minY, double *maxX, double *maxY);
    void CalculateSize(wxDrawnShape* shape);

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
    void DrawLines(int PCOUNT, wxPoint* points);
    void DrawPolygon(int PCOUNT, wxPoint* points, int flags = 0);
    void DrawSpline(int PCOUNT, wxPoint* points);
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
    WXSHAPE_IMP_CALLBACKS(wxPyRectangleShape, wxRectangleShape);
%}

class wxPyRectangleShape : public wxPyShape {
public:
    wxPyRectangleShape(double width = 0.0, double height = 0.0);

    void _setSelf(PyObject* self);
    %pragma(python) addtomethod = "__init__:self._setSelf(self)"

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
    bool base_OnMovePre(wxDC& dc, double x, double y, double old_x, double old_y, bool display = TRUE);
    void base_OnMovePost(wxDC& dc, double x, double y, double old_x, double old_y, bool display = TRUE);
    void base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnDrawOutline(wxDC& dc, double x, double y, double w, double h);
    void base_OnDrawControlPoints(wxDC& dc);
    void base_OnEraseControlPoints(wxDC& dc);
    void base_OnMoveLink(wxDC& dc, bool moveControlPoints = TRUE);
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
    wxPyControlPoint(wxPyShapeCanvas *the_canvas = NULL,
                     wxPyShape *object = NULL,
                     double size = 0.0, double the_xoffset = 0.0,
                     double the_yoffset = 0.0, int the_type = 0);

    void _setSelf(PyObject* self);
    %pragma(python) addtomethod = "__init__:self._setSelf(self)"

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
    bool base_OnMovePre(wxDC& dc, double x, double y, double old_x, double old_y, bool display = TRUE);
    void base_OnMovePost(wxDC& dc, double x, double y, double old_x, double old_y, bool display = TRUE);
    void base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnDrawOutline(wxDC& dc, double x, double y, double w, double h);
    void base_OnDrawControlPoints(wxDC& dc);
    void base_OnEraseControlPoints(wxDC& dc);
    void base_OnMoveLink(wxDC& dc, bool moveControlPoints = TRUE);
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
    wxPyBitmapShape();

    void _setSelf(PyObject* self);
    %pragma(python) addtomethod = "__init__:self._setSelf(self)"

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
    bool base_OnMovePre(wxDC& dc, double x, double y, double old_x, double old_y, bool display = TRUE);
    void base_OnMovePost(wxDC& dc, double x, double y, double old_x, double old_y, bool display = TRUE);
    void base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnDrawOutline(wxDC& dc, double x, double y, double w, double h);
    void base_OnDrawControlPoints(wxDC& dc);
    void base_OnEraseControlPoints(wxDC& dc);
    void base_OnMoveLink(wxDC& dc, bool moveControlPoints = TRUE);
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
    wxPyDrawnShape();

    void _setSelf(PyObject* self);
    %pragma(python) addtomethod = "__init__:self._setSelf(self)"

    void CalculateSize();
    void DestroyClippingRect();
    void DrawArc(const wxPoint& centrePoint, const wxPoint& startPoint,
                 const wxPoint& endPoint);
    void DrawAtAngle(int angle);
    void DrawEllipticArc(const wxRect& rect, double startAngle, double endAngle);
    void DrawLine(const wxPoint& point1, const wxPoint& point2);
    void DrawLines(int PCOUNT, wxPoint* points);
    void DrawPoint(const wxPoint& point);
    void DrawPolygon(int PCOUNT, wxPoint* points, int flags = 0);
    void DrawRectangle(const wxRect& rect);
    void DrawRoundedRectangle(const wxRect& rect, double radius);
    void DrawSpline(int PCOUNT, wxPoint* points);
    void DrawText(const wxString& text, const wxPoint& point);
    int GetAngle();

    wxPseudoMetaFile& GetMetaFile();

    double GetRotation();
    bool LoadFromMetaFile(char * filename);
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
    bool base_OnMovePre(wxDC& dc, double x, double y, double old_x, double old_y, bool display = TRUE);
    void base_OnMovePost(wxDC& dc, double x, double y, double old_x, double old_y, bool display = TRUE);
    void base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnDrawOutline(wxDC& dc, double x, double y, double w, double h);
    void base_OnDrawControlPoints(wxDC& dc);
    void base_OnEraseControlPoints(wxDC& dc);
    void base_OnMoveLink(wxDC& dc, bool moveControlPoints = TRUE);
    void base_OnSizingDragLeft(wxPyControlPoint* pt, bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnSizingBeginDragLeft(wxPyControlPoint* pt, double x, double y, int keys=0, int attachment = 0);
    void base_OnSizingEndDragLeft(wxPyControlPoint* pt, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginSize(double w, double h);
    void base_OnEndSize(double w, double h);
//    void base_OnChangeAttachment(int attachment, wxPyLineShape* line, wxList& ordering);

};


//---------------------------------------------------------------------------

class wxOGLConstraint  {
public:
    //wxOGLConstraint(int type, wxPyShape *constraining, wxList& constrained);
    %addmethods {
        wxOGLConstraint(int type, wxPyShape *constraining, PyObject* constrained) {
            wxList* list = wxPy_wxListHelper(constrained, "_wxPyShape_p");
            wxOGLConstraint* rv = new wxOGLConstraint(type, constraining, *list);
            delete list;
            return rv;
        }
    }
    ~wxOGLConstraint();

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
    wxPyCompositeShape();

    void _setSelf(PyObject* self);
    %pragma(python) addtomethod = "__init__:self._setSelf(self)"

    void AddChild(wxPyShape *child, wxPyShape *addAfter = NULL);

    wxOGLConstraint * AddConstraint(wxOGLConstraint *constraint);


    //wxOGLConstraint * AddConstraint(int type,
    //                                wxPyShape *constraining,
    //                                wxList& constrained);
    %addmethods {
        wxOGLConstraint * AddConstrainedShapes(int type, wxPyShape *constraining,
                                               PyObject* constrained) {
            wxList* list = wxPy_wxListHelper(constrained, "_wxPyShape_p");
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
    %addmethods {
        PyObject* GetConstraints() {
            wxList& list = self->GetConstraints();
            return wxPy_ConvertList(&list, "wxOGLConstraint");
        }

        PyObject* GetDivisions() {
            wxList& list = self->GetDivisions();
            return wxPy_ConvertList(&list, "wxPyDivisionShape");
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
    bool base_OnMovePre(wxDC& dc, double x, double y, double old_x, double old_y, bool display = TRUE);
    void base_OnMovePost(wxDC& dc, double x, double y, double old_x, double old_y, bool display = TRUE);
    void base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnDrawOutline(wxDC& dc, double x, double y, double w, double h);
    void base_OnDrawControlPoints(wxDC& dc);
    void base_OnEraseControlPoints(wxDC& dc);
    void base_OnMoveLink(wxDC& dc, bool moveControlPoints = TRUE);
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
    wxPyDividedShape(double width = 0.0, double height = 0.0);

    void _setSelf(PyObject* self);
    %pragma(python) addtomethod = "__init__:self._setSelf(self)"

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
    bool base_OnMovePre(wxDC& dc, double x, double y, double old_x, double old_y, bool display = TRUE);
    void base_OnMovePost(wxDC& dc, double x, double y, double old_x, double old_y, bool display = TRUE);
    void base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnDrawOutline(wxDC& dc, double x, double y, double w, double h);
    void base_OnDrawControlPoints(wxDC& dc);
    void base_OnEraseControlPoints(wxDC& dc);
    void base_OnMoveLink(wxDC& dc, bool moveControlPoints = TRUE);
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
    wxPyDivisionShape();

    void _setSelf(PyObject* self);
    %pragma(python) addtomethod = "__init__:self._setSelf(self)"

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
    bool base_OnMovePre(wxDC& dc, double x, double y, double old_x, double old_y, bool display = TRUE);
    void base_OnMovePost(wxDC& dc, double x, double y, double old_x, double old_y, bool display = TRUE);
    void base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragLeft(double x, double y, int keys=0, int attachment = 0);
    void base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnEndDragRight(double x, double y, int keys=0, int attachment = 0);
    void base_OnDrawOutline(wxDC& dc, double x, double y, double w, double h);
    void base_OnDrawControlPoints(wxDC& dc);
    void base_OnEraseControlPoints(wxDC& dc);
    void base_OnMoveLink(wxDC& dc, bool moveControlPoints = TRUE);
    void base_OnSizingDragLeft(wxPyControlPoint* pt, bool draw, double x, double y, int keys=0, int attachment = 0);
    void base_OnSizingBeginDragLeft(wxPyControlPoint* pt, double x, double y, int keys=0, int attachment = 0);
    void base_OnSizingEndDragLeft(wxPyControlPoint* pt, double x, double y, int keys=0, int attachment = 0);
    void base_OnBeginSize(double w, double h);
    void base_OnEndSize(double w, double h);
//    void base_OnChangeAttachment(int attachment, wxPyLineShape* line, wxList& ordering);

};



//---------------------------------------------------------------------------

