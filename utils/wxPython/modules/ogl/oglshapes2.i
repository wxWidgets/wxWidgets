/////////////////////////////////////////////////////////////////////////////
// Name:        oglshapes2.i
// Purpose:     SWIG definitions for the wxWindows Object Graphics Library
//
// Author:      Robin Dunn
//
// Created:     3-Sept-1999
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


%module oglshapes2

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
%import oglshapes.i


%pragma(python) code = "import wx"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
%{
    WXSHAPE_IMP_CALLBACKS(wxPyEllipseShape, wxEllipseShape);
%}

class wxPyEllipseShape : public wxPyShape {
public:
    wxPyEllipseShape(double width = 0.0, double height = 0.0);

    void _setSelf(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setSelf(self, wxPyEllipseShape)"

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
    WXSHAPE_IMP_CALLBACKS(wxPyCircleShape, wxCircleShape);
%}

class wxPyCircleShape : public wxPyEllipseShape {
public:
    wxPyCircleShape(double width = 0.0);

    void _setSelf(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setSelf(self, wxPyCircleShape)"

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
//---------------------------------------------------------------------------
class wxArrowHead {
public:
    wxArrowHead(int type = 0,
                int end = 0,
                double size = 0.0,
                double dist = 0.0,
                const char * name = "",
                wxPseudoMetaFile *mf = NULL,
                long arrowId = -1);
    ~wxArrowHead();

    int _GetType();
    int GetPosition();
    void SetPosition(int pos);
    double GetXOffset();
    double GetYOffset();
    double GetSpacing();
    double GetSize();
    wxString GetName();
    void SetXOffset(double x);
    void SetYOffset(double y);
    wxPseudoMetaFile *GetMetaFile();
    long GetId();
    int GetArrowEnd();
    double GetArrowSize();
    void SetSize(double size);
    void SetSpacing(double sp);
};
//---------------------------------------------------------------------------
%{
    WXSHAPE_IMP_CALLBACKS(wxPyLineShape, wxLineShape);
%}

class wxPyLineShape : public wxPyShape {
public:
    wxPyLineShape();

    void _setSelf(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setSelf(self, wxPyLineShape)"

    void AddArrow(int type, int end = ARROW_POSITION_END,
                  double arrowSize = 10.0, double xOffset = 0.0,
                  char* name = "", wxPseudoMetaFile *mf = NULL,
                  long arrowId = -1);

    //void AddArrowOrdered(wxArrowHead *arrow, wxList& referenceList, int end);
    %addmethods {
        void AddArrowOrdered(wxArrowHead *arrow, PyObject* referenceList, int end) {
            wxList* list = wxPy_wxListHelper(referenceList, "_wxArrowHead_p");
            self->AddArrowOrdered(arrow, *list, end);
            delete list;
        }
    }
    bool ClearArrow(const wxString& name);
    void ClearArrowsAtPosition(int position = -1);
    void DrawArrow(wxDC& dc, wxArrowHead *arrow, double xOffset, bool proportionalOffset);
    %name(DeleteArrowHeadId)bool DeleteArrowHead(long arrowId);
    bool DeleteArrowHead(int position, const wxString& name);
    bool DeleteLineControlPoint();
    void DrawArrows(wxDC& dc);
    void DrawRegion(wxDC& dc, wxShapeRegion *region, double x, double y);
    void EraseRegion(wxDC& dc, wxShapeRegion *region, double x, double y);
    %name(FindArrowHeadId)wxArrowHead * FindArrowHead(long arrowId);
    wxArrowHead * FindArrowHead(int position, const wxString& name);
    void FindLineEndPoints(double *OUTPUT, double *OUTPUT,
                           double *OUTPUT, double *OUTPUT);
    int FindLinePosition(double x, double y);
    double FindMinimumWidth();
    void FindNth(wxPyShape *image, int *OUTPUT, int *OUTPUT, bool incoming);
    int GetAttachmentFrom();
    int GetAttachmentTo();
    void GetEnds(double *OUTPUT, double *OUTPUT, double *OUTPUT, double *OUTPUT);
    wxPyShape * GetFrom();
    void GetLabelPosition(int position, double *OUTPUT, double *OUTPUT);
    wxPoint * GetNextControlPoint(wxPyShape *shape);
    wxPyShape * GetTo();
    void Initialise();
    void InsertLineControlPoint(wxDC* dc);
    bool IsEnd(wxPyShape *shape);
    bool IsSpline();
    void MakeLineControlPoints(int n);

    void SetAttachmentFrom(int fromAttach);
    void SetAttachments(int fromAttach, int toAttach);
    void SetAttachmentTo(int toAttach);
    void SetEnds(double x1, double y1, double x2, double y2);
    void SetFrom(wxPyShape *object);
    void SetIgnoreOffsets(bool ignore);
    void SetSpline(bool spline);
    void SetTo(wxPyShape *object);
    void Straighten(wxDC* dc = NULL);
    void Unlink();


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
    WXSHAPE_IMP_CALLBACKS(wxPyPolygonShape, wxPolygonShape);
%}

class wxPyPolygonShape : public wxPyShape {
public:
    wxPyPolygonShape();

    void _setSelf(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setSelf(self, wxPyPolygonShape)"


    // void Create(wxList* points);
    %addmethods {
        PyObject* Create(PyObject* points) {
            wxList* list = wxPy_wxRealPoint_ListHelper(points);
            if (list) {
                self->Create(list);
                Py_INCREF(Py_None);
                return Py_None;
            }
            else return NULL;
        }
    }

    void AddPolygonPoint(int pos = 0);
    void CalculatePolygonCentre();
    void DeletePolygonPoint(int pos = 0);

    // wxList* GetPoints();
    %addmethods {
        PyObject* GetPoints() {
            wxList* list = self->GetPoints();
            return wxPy_ConvertList(list, "wxRealPoint");
        }
    }

    void UpdateOriginalPoints();


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
    WXSHAPE_IMP_CALLBACKS(wxPyTextShape, wxTextShape);
%}

class wxPyTextShape : public wxPyRectangleShape {
public:
    wxPyTextShape(double width = 0.0, double height = 0.0);

    void _setSelf(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setSelf(self, wxPyTextShape)"


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
