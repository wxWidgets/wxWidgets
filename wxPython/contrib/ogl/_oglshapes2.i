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


// not a %module


//---------------------------------------------------------------------------

%{
    WXSHAPE_IMP_CALLBACKS(wxPyEllipseShape, wxEllipseShape);
%}

class wxPyEllipseShape : public wxPyShape {
public:
    %pythonAppend wxPyEllipseShape "self._setOORandCallbackInfo(PyEllipseShape)"
    
    wxPyEllipseShape(double width = 0.0, double height = 0.0);

    void _setCallbackInfo(PyObject* self, PyObject* _class);

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
    WXSHAPE_IMP_CALLBACKS(wxPyCircleShape, wxCircleShape);
%}

class wxPyCircleShape : public wxPyEllipseShape {
public:
    %pythonAppend wxPyCircleShape "self._setOORandCallbackInfo(PyCircleShape)"
    
    wxPyCircleShape(double width = 0.0);

    void _setCallbackInfo(PyObject* self, PyObject* _class);

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
//---------------------------------------------------------------------------

class wxArrowHead : public wxObject {
public:
    wxArrowHead(int type = 0,
                int end = 0,
                double size = 0.0,
                double dist = 0.0,
                const wxString& name = wxPyEmptyString,
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
    %pythonAppend wxPyLineShape "self._setOORandCallbackInfo(PyLineShape)"
    
    wxPyLineShape();

    void _setCallbackInfo(PyObject* self, PyObject* _class);

    void AddArrow(int type, int end = ARROW_POSITION_END,
                  double arrowSize = 10.0, double xOffset = 0.0,
                  const wxString& name = wxPyEmptyString, wxPseudoMetaFile *mf = NULL,
                  long arrowId = -1);

    //void AddArrowOrdered(wxArrowHead *arrow, wxList& referenceList, int end);
    %extend {
        void AddArrowOrdered(wxArrowHead *arrow, PyObject* referenceList, int end) {
            wxList* list = wxPy_wxListHelper(referenceList, wxT("wxArrowHead"));
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
    wxRealPoint * GetNextControlPoint(wxPyShape *shape);
    wxPyShape * GetTo();
    void Initialise();
    void InsertLineControlPoint(wxDC* dc);
    bool IsEnd(wxPyShape *shape);
    bool IsSpline();
    void MakeLineControlPoints(int n);

    //   inline wxList *GetLineControlPoints() { return m_lineControlPoints; }
    %extend {
        PyObject* GetLineControlPoints() {
            wxList* list = self->GetLineControlPoints();
            return wxPy_ConvertRealPointList(list);
        }

        // part of Patch# 893337
        void SetLineControlPoints(PyObject* list) {
            wxList* real_point_list = wxPy_wxRealPoint_ListHelper(list);
            self->MakeLineControlPoints((int)(real_point_list->GetCount()));
            wxList* old_control_points = self->GetLineControlPoints();
            wxNode* old_node = old_control_points->GetFirst();
            wxNode* real_node = real_point_list->GetFirst();
            while(old_node)
            {
                wxRealPoint* old_point = (wxRealPoint*)old_node->GetData();
                wxRealPoint* new_point = (wxRealPoint*)real_node->GetData();
                old_point->x = new_point->x;
                old_point->y = new_point->y;
                old_node = old_node->GetNext();
                real_node = real_node->GetNext();
            }
            self->ClearPointList(*real_point_list);
            delete real_point_list;
        }
    }

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


    void SetAlignmentOrientation(bool isEnd, bool isHoriz);
    void SetAlignmentType(bool isEnd, int alignType);
    bool GetAlignmentOrientation(bool isEnd);
    int GetAlignmentType(bool isEnd);
    int GetAlignmentStart() const;
    int GetAlignmentEnd() const;

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
    WXSHAPE_IMP_CALLBACKS(wxPyPolygonShape, wxPolygonShape);
%}

class wxPyPolygonShape : public wxPyShape {
public:
    %pythonAppend wxPyPolygonShape "self._setOORandCallbackInfo(PyPolygonShape)"
    
    wxPyPolygonShape();

    void _setCallbackInfo(PyObject* self, PyObject* _class);


    // void Create(wxList* points);
    %extend {
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
    %extend {
        PyObject* GetPoints() {
            wxList* list = self->GetPoints();
            return wxPy_ConvertRealPointList(list);
        }

        PyObject* GetOriginalPoints() {
            wxList* list = self->GetOriginalPoints();
            PyObject*   pyList;
            PyObject*   pyObj;
            wxObject*   wxObj;
            wxNode*     node = list->GetFirst();

            bool blocked = wxPyBeginBlockThreads();
            pyList = PyList_New(0);
            while (node) {
                wxObj = node->GetData();
                pyObj = wxPyConstructObject(wxObj, wxT("wxRealPoint"), 0);
                PyList_Append(pyList, pyObj);
                node = node->GetNext();
            }
            wxPyEndBlockThreads(blocked);
            return pyList;
        }        
    }

    double GetOriginalWidth() const;
    double GetOriginalHeight() const;

    void SetOriginalWidth(double w);
    void SetOriginalHeight(double h);

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
    WXSHAPE_IMP_CALLBACKS(wxPyTextShape, wxTextShape);
%}

class wxPyTextShape : public wxPyRectangleShape {
public:
    %pythonAppend wxPyTextShape "self._setOORandCallbackInfo(PyTextShape)"
    
    wxPyTextShape(double width = 0.0, double height = 0.0);

    void _setCallbackInfo(PyObject* self, PyObject* _class);

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
