/////////////////////////////////////////////////////////////////////////////
// Name:        oglbasic.i
// Purpose:     SWIG definitions for the wxWindows Object Graphics Library
//
// Author:      Robin Dunn
//
// Created:     3-Sept-1999
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


%module oglbasic

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

//%extern oglcanvas.i


%pragma(python) code = "import wx"
%pragma(python) code = "from oglcanvas import wxPyShapeCanvasPtr"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


class wxShapeRegion {
public:
    wxShapeRegion();
    //~wxShapeRegion();

    void SetText(const wxString& s);
    void SetFont(wxFont *f);
    void SetMinSize(double w, double h);
    void SetSize(double w, double h);
    void SetPosition(double x, double y);
    void SetProportions(double x, double y);
    void SetFormatMode(int mode);
    void SetName(const wxString& s);
    void SetColour(const wxString& col);

    wxString GetText();
    wxFont *GetFont();
    void GetMinSize(double *x, double *y);
    void GetProportion(double *x, double *y);
    void GetSize(double *x, double *y);
    void GetPosition(double *xp, double *yp);
    int GetFormatMode();
    wxString GetName();
    wxString GetColour();
    wxColour *GetActualColourObject();
    wxList& GetFormattedText();
    wxString GetPenColour();
    int GetPenStyle();
    void SetPenStyle(int style);
    void SetPenColour(const wxString& col);
    wxPen *GetActualPen();
    double GetWidth();
    double GetHeight();

    void ClearText();
};


//---------------------------------------------------------------------------

%{
    WXSHAPE_IMP_CALLBACKS(wxPyShapeEvtHandler,wxShapeEvtHandler);
%}


class wxPyShapeEvtHandler {
public:
    wxPyShapeEvtHandler(wxPyShapeEvtHandler *prev = NULL,
                        wxPyShape *shape = NULL);

    void _setSelf(PyObject* self);
    %pragma(python) addtomethod = "__init__:self._setSelf(self)"

    %addmethods {
        void Destroy() { delete self; }
    }

    void SetShape(wxPyShape *sh);
    wxPyShape *GetShape();
    void SetPreviousHandler(wxPyShapeEvtHandler* handler);
    wxPyShapeEvtHandler* GetPreviousHandler();
    wxPyShapeEvtHandler* CreateNewCopy();

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
    WXSHAPE_IMP_CALLBACKS(wxPyShape, wxShape);
%}


class wxPyShape : public wxPyShapeEvtHandler {
public:
    // wxPyShape(wxPyShapeCanvas *can = NULL);     abstract base class...

    void _setSelf(PyObject* self);
    %pragma(python) addtomethod = "__init__:self._setSelf(self)"

    %addmethods {
        void Destroy() { delete self; }
    }

    void GetBoundingBoxMax(double *OUTPUT, double *OUTPUT);
    void GetBoundingBoxMin(double *OUTPUT, double *OUTPUT);
    bool GetPerimeterPoint(double x1, double y1,
                           double x2, double y2,
                           double *OUTPUT, double *OUTPUT);
    wxPyShapeCanvas *GetCanvas();
    void SetCanvas(wxPyShapeCanvas *the_canvas);
    void AddToCanvas(wxPyShapeCanvas *the_canvas, wxPyShape *addAfter = NULL);
    void InsertInCanvas(wxPyShapeCanvas *the_canvas);
    void RemoveFromCanvas(wxPyShapeCanvas *the_canvas);
    double GetX();
    double GetY();
    void SetX(double x);
    void SetY(double y);
    wxPyShape *GetParent();
    void SetParent(wxPyShape *p);
    wxPyShape *GetTopAncestor();


    // wxList& GetChildren();
    %addmethods {
        PyObject* GetChildren() {
            wxList& list = self->GetChildren();
            return wxPy_ConvertList(&list, "wxPyShape");
        }
    }


    void Unlink();
    void SetDrawHandles(bool drawH);
    bool GetDrawHandles();
    void MakeControlPoints();
    void DeleteControlPoints(wxDC *dc = NULL);
    void ResetControlPoints();
    wxPyShapeEvtHandler *GetEventHandler();
    void SetEventHandler(wxPyShapeEvtHandler *handler);
    void MakeMandatoryControlPoints();
    void ResetMandatoryControlPoints();
    bool Recompute();
    void CalculateSize();
    void Select(bool select = TRUE, wxDC* dc = NULL);
    void SetHighlight(bool hi = TRUE, bool recurse = FALSE);
    bool IsHighlighted() ;
    bool Selected();
    bool AncestorSelected();
    void SetSensitivityFilter(int sens = OP_ALL, bool recursive = FALSE);
    int GetSensitivityFilter();
    void SetDraggable(bool drag, bool recursive = FALSE);
    void SetFixedSize(bool x, bool y);
    void GetFixedSize(bool *OUTPUT, bool *OUTPUT) ;
    bool GetFixedWidth();
    bool GetFixedHeight();
    void SetSpaceAttachments(bool sp);
    bool GetSpaceAttachments() ;
    void SetShadowMode(int mode, bool redraw = FALSE);
    int GetShadowMode();
    bool HitTest(double x, double y, int *OUTPUT, double *OUTPUT);
    void SetCentreResize(bool cr);
    bool GetCentreResize();
    void SetMaintainAspectRatio(bool ar);
    bool GetMaintainAspectRatio();


    // wxList& GetLines();
    %addmethods {
        PyObject* GetLines() {
            wxList& list = self->GetLines();
            return wxPy_ConvertList(&list, "wxPyLineShape");
        }
    }

    void SetDisableLabel(bool flag);
    bool GetDisableLabel();
    void SetAttachmentMode(int mode);
    int GetAttachmentMode();
    void SetId(long i);
    long GetId();
    void SetPen(wxPen *pen);
    void SetBrush(wxBrush *brush);

    // void SetClientData(wxObject *client_data);
    // wxObject *GetClientData();
    %addmethods {
        void SetClientData(PyObject* userData) {
            wxPyUserData* data = NULL;
            if (userData)
                data = new wxPyUserData(userData);
            self->SetClientData(data);
        }

        PyObject* GetClientData() {
            wxPyUserData* data = (wxPyUserData*)self->GetClientData();
            if (data) {
                Py_INCREF(data->m_obj);
                return data->m_obj;
            } else {
                Py_INCREF(Py_None);
                return Py_None;
            }
        }
    }

    void Show(bool show);
    bool IsShown();
    void Move(wxDC& dc, double x1, double y1, bool display = TRUE);
    void Erase(wxDC& dc);
    void EraseContents(wxDC& dc);
    void Draw(wxDC& dc);
    void Flash();
    void MoveLinks(wxDC& dc);
    void DrawContents(wxDC& dc);
    void SetSize(double x, double y, bool recursive = TRUE);
    void SetAttachmentSize(double x, double y);
    void Attach(wxPyShapeCanvas *can);
    void Detach();
    bool Constrain();
    void AddLine(wxPyLineShape *line, wxPyShape *other,
                 int attachFrom = 0, int attachTo = 0,
                 int positionFrom = -1, int positionTo = -1);
    int GetLinePosition(wxPyLineShape* line);
    void AddText(const wxString& string);
    wxPen *GetPen();
    wxBrush *GetBrush();
    void SetDefaultRegionSize();
    void FormatText(wxDC& dc, const wxString& s, int regionId = 0);
    void SetFormatMode(int mode, int regionId = 0);
    int GetFormatMode(int regionId = 0);
    void SetFont(wxFont *font, int regionId = 0);
    wxFont *GetFont(int regionId = 0);
    void SetTextColour(const wxString& colour, int regionId = 0);
    wxString GetTextColour(int regionId = 0);
    int GetNumberOfTextRegions();
    void SetRegionName(const wxString& name, int regionId = 0);
    wxString GetRegionName(int regionId);
    int GetRegionId(const wxString& name);
    void NameRegions(const wxString& parentName = "");

    // wxList& GetRegions();
    %addmethods {
        PyObject* GetRegions() {
            wxList& list = self->GetRegions();
            return wxPy_ConvertList(&list, "wxShapeRegion");
        }
    }

    void AddRegion(wxShapeRegion *region);
    void ClearRegions();
    void AssignNewIds();
    wxPyShape *FindRegion(const wxString& regionName, int *regionId);
    void FindRegionNames(wxStringList& list);
    void ClearText(int regionId = 0);
    void RemoveLine(wxPyLineShape *line);

#ifdef PROLOGIO
    void WriteAttributes(wxExpr *clause);
    void ReadAttributes(wxExpr *clause);
    void ReadConstraints(wxExpr *clause, wxExprDatabase *database);
    void WriteRegions(wxExpr *clause);
    void ReadRegions(wxExpr *clause);
#endif

    bool GetAttachmentPosition(int attachment, double *OUTPUT, double *OUTPUT,
                               int nth = 0, int no_arcs = 1, wxPyLineShape *line = NULL);
    int GetNumberOfAttachments();
    bool AttachmentIsValid(int attachment);
    bool GetAttachmentPositionEdge(int attachment, double *OUTPUT, double *OUTPUT,
                                   int nth = 0, int no_arcs = 1, wxPyLineShape *line = NULL);
    wxRealPoint CalcSimpleAttachment(const wxRealPoint& pt1, const wxRealPoint& pt2,
                                     int nth, int noArcs, wxPyLineShape* line);
    bool AttachmentSortTest(int attachmentPoint, const wxRealPoint& pt1, const wxRealPoint& pt2);
    void EraseLinks(wxDC& dc, int attachment = -1, bool recurse = FALSE);
    void DrawLinks(wxDC& dc, int attachment = -1, bool recurse = FALSE);
    bool MoveLineToNewAttachment(wxDC& dc, wxPyLineShape *to_move,
                                 double x, double y);

    //void ApplyAttachmentOrdering(wxList& linesToSort);
    %addmethods {
        void ApplyAttachmentOrdering(PyObject* linesToSort) {
            wxList* list = wxPy_wxListHelper(linesToSort, "_wxPyLineShape_p");
            self->ApplyAttachmentOrdering(*list);
            delete list;
        }
    }

    wxRealPoint GetBranchingAttachmentRoot(int attachment);
    bool GetBranchingAttachmentInfo(int attachment, wxRealPoint& root, wxRealPoint& neck,
                                    wxRealPoint& shoulder1, wxRealPoint& shoulder2);
    bool GetBranchingAttachmentPoint(int attachment, int n, wxRealPoint& attachmentPoint,
                                     wxRealPoint& stemPoint);
    int GetAttachmentLineCount(int attachment);
    void SetBranchNeckLength(int len);
    int GetBranchNeckLength();
    void SetBranchStemLength(int len);
    int GetBranchStemLength();
    void SetBranchSpacing(int len);
    int GetBranchSpacing();
    void SetBranchStyle(long style);
    long GetBranchStyle();
    int PhysicalToLogicalAttachment(int physicalAttachment);
    int LogicalToPhysicalAttachment(int logicalAttachment);
    bool Draggable();
    bool HasDescendant(wxPyShape *image);
    wxPyShape *CreateNewCopy(bool resetMapping = TRUE, bool recompute = TRUE);
    void Copy(wxPyShape& copy);
    void CopyWithHandler(wxPyShape& copy);
    void Rotate(double x, double y, double theta);
    double GetRotation();
    void ClearAttachments();
    void Recentre(wxDC& dc);
    void ClearPointList(wxList& list);

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

