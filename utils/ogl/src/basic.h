/////////////////////////////////////////////////////////////////////////////
// Name:        basic.h
// Purpose:     Basic OGL classes and definitions
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _OGL_BASIC_H_
#define _OGL_BASIC_H_

#ifdef __GNUG__
#pragma interface "basic.h"
#endif

#define OGL_VERSION     2.0

#ifndef DEFAULT_MOUSE_TOLERANCE
#define DEFAULT_MOUSE_TOLERANCE 3
#endif

// Edit these lines if you positively don't want PROLOGIO support
#ifndef PROLOGIO
#define PROLOGIO
#endif

// Key identifiers
#define KEY_SHIFT 1
#define KEY_CTRL  2

// Arrow styles

#define ARROW_NONE         0
#define ARROW_END          1
#define ARROW_BOTH         2
#define ARROW_MIDDLE       3
#define ARROW_START        4

// Control point types
// Rectangle and most other shapes
#define CONTROL_POINT_VERTICAL   1
#define CONTROL_POINT_HORIZONTAL 2
#define CONTROL_POINT_DIAGONAL   3

// Line
#define CONTROL_POINT_ENDPOINT_TO 4
#define CONTROL_POINT_ENDPOINT_FROM 5
#define CONTROL_POINT_LINE       6

// Types of formatting: can be combined in a bit list
#define FORMAT_NONE           0
                                // Left justification
#define FORMAT_CENTRE_HORIZ   1
                                // Centre horizontally
#define FORMAT_CENTRE_VERT    2
                                // Centre vertically
#define FORMAT_SIZE_TO_CONTENTS 4
                                // Resize shape to contents

// Shadow mode
#define SHADOW_NONE           0
#define SHADOW_LEFT           1
#define SHADOW_RIGHT          2

/*
 * Declare types
 *
 */

#define SHAPE_BASIC           wxTYPE_USER + 1
#define SHAPE_RECTANGLE       wxTYPE_USER + 2
#define SHAPE_ELLIPSE         wxTYPE_USER + 3
#define SHAPE_POLYGON         wxTYPE_USER + 4
#define SHAPE_CIRCLE          wxTYPE_USER + 5
#define SHAPE_LINE            wxTYPE_USER + 6
#define SHAPE_DIVIDED_RECTANGLE wxTYPE_USER + 8
#define SHAPE_COMPOSITE       wxTYPE_USER + 9
#define SHAPE_CONTROL_POINT   wxTYPE_USER + 10
#define SHAPE_DRAWN           wxTYPE_USER + 11
#define SHAPE_DIVISION        wxTYPE_USER + 12
#define SHAPE_LABEL_OBJECT    wxTYPE_USER + 13
#define SHAPE_BITMAP          wxTYPE_USER + 14
#define SHAPE_DIVIDED_OBJECT_CONTROL_POINT   wxTYPE_USER + 15

#define OBJECT_REGION         wxTYPE_USER + 20

#define OP_CLICK_LEFT  1
#define OP_CLICK_RIGHT 2
#define OP_DRAG_LEFT 4
#define OP_DRAG_RIGHT 8

#define OP_ALL (OP_CLICK_LEFT | OP_CLICK_RIGHT | OP_DRAG_LEFT | OP_DRAG_RIGHT)

// Attachment modes
#define ATTACHMENT_MODE_NONE        0
#define ATTACHMENT_MODE_EDGE        1
#define ATTACHMENT_MODE_BRANCHING   2

// Sub-modes for branching attachment mode
#define BRANCHING_ATTACHMENT_NORMAL 1
#define BRANCHING_ATTACHMENT_BLOB   2

class wxShapeTextLine;
class wxShapeCanvas;
class wxLineShape;
class wxControlPoint;
class wxShapeRegion;
class wxShape;

#ifdef PROLOGIO
class WXDLLEXPORT wxExpr;
class WXDLLEXPORT wxExprDatabase;
#endif

// Round up
#define WXROUND(x) ( (long) (x + 0.5) )

class wxShapeEvtHandler: public wxObject
{
 DECLARE_DYNAMIC_CLASS(wxShapeEvtHandler)

 public:
  wxShapeEvtHandler(wxShapeEvtHandler *prev = NULL, wxShape *shape = NULL);
  virtual ~wxShapeEvtHandler();

  inline void SetShape(wxShape *sh) { m_handlerShape = sh; }
  inline wxShape *GetShape() const { return m_handlerShape; }

  inline void SetPreviousHandler(wxShapeEvtHandler* handler) { m_previousHandler = handler; }
  inline wxShapeEvtHandler* GetPreviousHandler() const { return m_previousHandler; }

  // This is called when the _shape_ is deleted.
  virtual void OnDelete();
  virtual void OnDraw(wxDC& dc);
  virtual void OnDrawContents(wxDC& dc);
  virtual void OnDrawBranches(wxDC& dc, bool erase = FALSE);
  virtual void OnMoveLinks(wxDC& dc);
  virtual void OnErase(wxDC& dc);
  virtual void OnEraseContents(wxDC& dc);
  virtual void OnHighlight(wxDC& dc);
  virtual void OnLeftClick(double x, double y, int keys = 0, int attachment = 0);
  virtual void OnLeftDoubleClick(double x, double y, int keys = 0, int attachment = 0);
  virtual void OnRightClick(double x, double y, int keys = 0, int attachment = 0);
  virtual void OnSize(double x, double y);
  virtual bool OnMovePre(wxDC& dc, double x, double y, double old_x, double old_y, bool display = TRUE);
  virtual void OnMovePost(wxDC& dc, double x, double y, double old_x, double old_y, bool display = TRUE);

  virtual void OnDragLeft(bool draw, double x, double y, int keys=0, int attachment = 0); // Erase if draw false
  virtual void OnBeginDragLeft(double x, double y, int keys=0, int attachment = 0);
  virtual void OnEndDragLeft(double x, double y, int keys=0, int attachment = 0);
  virtual void OnDragRight(bool draw, double x, double y, int keys=0, int attachment = 0); // Erase if draw false
  virtual void OnBeginDragRight(double x, double y, int keys=0, int attachment = 0);
  virtual void OnEndDragRight(double x, double y, int keys=0, int attachment = 0);
  virtual void OnDrawOutline(wxDC& dc, double x, double y, double w, double h);
  virtual void OnDrawControlPoints(wxDC& dc);
  virtual void OnEraseControlPoints(wxDC& dc);
  virtual void OnMoveLink(wxDC& dc, bool moveControlPoints = TRUE);

  // Control points ('handles') redirect control to the actual shape, to make it easier
  // to override sizing behaviour.
  virtual void OnSizingDragLeft(wxControlPoint* pt, bool draw, double x, double y, int keys=0, int attachment = 0); // Erase if draw false
  virtual void OnSizingBeginDragLeft(wxControlPoint* pt, double x, double y, int keys=0, int attachment = 0);
  virtual void OnSizingEndDragLeft(wxControlPoint* pt, double x, double y, int keys=0, int attachment = 0);

  virtual void OnBeginSize(double WXUNUSED(w), double WXUNUSED(h)) { }
  virtual void OnEndSize(double WXUNUSED(w), double WXUNUSED(h)) { }

  // Can override this to prevent or intercept line reordering.
  virtual void OnChangeAttachment(int attachment, wxLineShape* line, wxList& ordering);

  // Creates a copy of this event handler.
  wxShapeEvtHandler *CreateNewCopy();

  // Does the copy - override for new event handlers which might store
  // app-specific data.
  virtual void CopyData(wxShapeEvtHandler& copy) {};

 private:
  wxShapeEvtHandler*    m_previousHandler;
  wxShape*              m_handlerShape;
};

class wxShape: public wxShapeEvtHandler
{
 DECLARE_ABSTRACT_CLASS(wxShape)

 public:

  wxShape(wxShapeCanvas *can = NULL);
  virtual ~wxShape();
  virtual void GetBoundingBoxMax(double *width, double *height);
  virtual void GetBoundingBoxMin(double *width, double *height) = 0;
  virtual bool GetPerimeterPoint(double x1, double y1,
                                 double x2, double y2,
                                 double *x3, double *y3);
  inline wxShapeCanvas *GetCanvas() { return m_canvas; }
  void SetCanvas(wxShapeCanvas *the_canvas);
  virtual void AddToCanvas(wxShapeCanvas *the_canvas, wxShape *addAfter = NULL);
  virtual void InsertInCanvas(wxShapeCanvas *the_canvas);

  virtual void RemoveFromCanvas(wxShapeCanvas *the_canvas);
  inline double GetX() const { return m_xpos; }
  inline double GetY() const { return m_ypos; }
  inline void SetX(double x) { m_xpos = x; }
  inline void SetY(double y) { m_ypos = y; }

  inline wxShape *GetParent() const { return m_parent; }
  inline void SetParent(wxShape *p) { m_parent = p; }
  wxShape *GetTopAncestor();
  inline wxList& GetChildren() { return m_children; }

  virtual void OnDraw(wxDC& dc);
  virtual void OnDrawContents(wxDC& dc);
  virtual void OnMoveLinks(wxDC& dc);
  virtual void Unlink() { };
  void SetDrawHandles(bool drawH);
  inline bool GetDrawHandles() { return m_drawHandles; }
  virtual void OnErase(wxDC& dc);
  virtual void OnEraseContents(wxDC& dc);
  virtual void OnHighlight(wxDC& dc);
  virtual void OnLeftClick(double x, double y, int keys = 0, int attachment = 0);
  virtual void OnLeftDoubleClick(double x, double y, int keys = 0, int attachment = 0) {}
  virtual void OnRightClick(double x, double y, int keys = 0, int attachment = 0);
  virtual void OnSize(double x, double y);
  virtual bool OnMovePre(wxDC& dc, double x, double y, double old_x, double old_y, bool display = TRUE);
  virtual void OnMovePost(wxDC& dc, double x, double y, double old_x, double old_y, bool display = TRUE);

  virtual void OnDragLeft(bool draw, double x, double y, int keys=0, int attachment = 0); // Erase if draw false
  virtual void OnBeginDragLeft(double x, double y, int keys=0, int attachment = 0);
  virtual void OnEndDragLeft(double x, double y, int keys=0, int attachment = 0);
  virtual void OnDragRight(bool draw, double x, double y, int keys=0, int attachment = 0); // Erase if draw false
  virtual void OnBeginDragRight(double x, double y, int keys=0, int attachment = 0);
  virtual void OnEndDragRight(double x, double y, int keys=0, int attachment = 0);
  virtual void OnDrawOutline(wxDC& dc, double x, double y, double w, double h);
  virtual void OnDrawControlPoints(wxDC& dc);
  virtual void OnEraseControlPoints(wxDC& dc);

  virtual void OnBeginSize(double WXUNUSED(w), double WXUNUSED(h)) { }
  virtual void OnEndSize(double WXUNUSED(w), double WXUNUSED(h)) { }

  // Control points ('handles') redirect control to the actual shape, to make it easier
  // to override sizing behaviour.
  virtual void OnSizingDragLeft(wxControlPoint* pt, bool draw, double x, double y, int keys=0, int attachment = 0); // Erase if draw false
  virtual void OnSizingBeginDragLeft(wxControlPoint* pt, double x, double y, int keys=0, int attachment = 0);
  virtual void OnSizingEndDragLeft(wxControlPoint* pt, double x, double y, int keys=0, int attachment = 0);

  virtual void MakeControlPoints();
  virtual void DeleteControlPoints(wxDC *dc = NULL);
  virtual void ResetControlPoints();

  inline wxShapeEvtHandler *GetEventHandler() { return m_eventHandler; }
  inline void SetEventHandler(wxShapeEvtHandler *handler) { m_eventHandler = handler; }

  // Mandatory control points, e.g. the divided line moving handles
  // should appear even if a child of the 'selected' image
  virtual void MakeMandatoryControlPoints();
  virtual void ResetMandatoryControlPoints();

  inline virtual bool Recompute() { return TRUE; };
  // Calculate size recursively, if size changes. Size might depend on children.
  inline virtual void CalculateSize() { };
  virtual void Select(bool select = TRUE, wxDC* dc = NULL);
  virtual void SetHighlight(bool hi = TRUE, bool recurse = FALSE);
  inline virtual bool IsHighlighted() const { return m_highlighted; };
  virtual bool Selected() const;
  virtual bool AncestorSelected() const;
  void SetSensitivityFilter(int sens = OP_ALL, bool recursive = FALSE);
  int GetSensitivityFilter() const { return m_sensitivity; }
  void SetDraggable(bool drag, bool recursive = FALSE);
  inline  void SetFixedSize(bool x, bool y) { m_fixedWidth = x; m_fixedHeight = y; };
  inline  void GetFixedSize(bool *x, bool *y) const { *x = m_fixedWidth; *y = m_fixedHeight; };
  inline  bool GetFixedWidth() const { return m_fixedWidth; }
  inline  bool GetFixedHeight() const { return m_fixedHeight; }
  inline  void SetSpaceAttachments(bool sp) { m_spaceAttachments = sp; };
  inline  bool GetSpaceAttachments() const { return m_spaceAttachments; };
  void SetShadowMode(int mode, bool redraw = FALSE);
  inline int GetShadowMode() const { return m_shadowMode; }
  virtual bool HitTest(double x, double y, int *attachment, double *distance);
  inline void SetCentreResize(bool cr) { m_centreResize = cr; }
  inline bool GetCentreResize() const { return m_centreResize; }
  inline void SetMaintainAspectRatio(bool ar) { m_maintainAspectRatio = ar; }
  inline bool GetMaintainAspectRatio() const { return m_maintainAspectRatio; }
  inline wxList& GetLines() const { return (wxList&) m_lines; }
  inline void SetDisableLabel(bool flag) { m_disableLabel = flag; }
  inline bool GetDisableLabel() const { return m_disableLabel; }
  inline void SetAttachmentMode(int mode) { m_attachmentMode = mode; }
  inline int GetAttachmentMode() const { return m_attachmentMode; }
  inline void SetId(long i) { m_id = i; }
  inline long GetId() const { return m_id; }

  void SetPen(wxPen *pen);
  void SetBrush(wxBrush *brush);
  inline void SetClientData(wxObject *client_data) { m_clientData = client_data; };
  inline wxObject *GetClientData() const { return m_clientData; };

  virtual void Show(bool show);
  virtual bool IsShown() const { return m_visible; }
  virtual void Move(wxDC& dc, double x1, double y1, bool display = TRUE);
  virtual void Erase(wxDC& dc);
  virtual void EraseContents(wxDC& dc);
  virtual void Draw(wxDC& dc);
  virtual void Flash();
  virtual void MoveLinks(wxDC& dc);
  virtual void DrawContents(wxDC& dc);  // E.g. for drawing text label
  virtual void SetSize(double x, double y, bool recursive = TRUE);
  virtual void SetAttachmentSize(double x, double y);
  void Attach(wxShapeCanvas *can);
  void Detach();

  inline virtual bool Constrain() { return FALSE; } ;

  void AddLine(wxLineShape *line, wxShape *other,
               int attachFrom = 0, int attachTo = 0,
               // The line ordering
               int positionFrom = -1, int positionTo = -1);

  // Return the zero-based position in m_lines of line.
  int GetLinePosition(wxLineShape* line);

  void AddText(const wxString& string);

  inline wxPen *GetPen() const { return m_pen; }
  inline wxBrush *GetBrush() const { return m_brush; }

  /*
   * Region-specific functions (defaults to the default region
   * for simple objects
   */

  // Set the default, single region size to be consistent
  // with the object size
  void SetDefaultRegionSize();
  virtual void FormatText(wxDC& dc, const wxString& s, int regionId = 0);
  virtual void SetFormatMode(int mode, int regionId = 0);
  virtual int GetFormatMode(int regionId = 0) const;
  virtual void SetFont(wxFont *font, int regionId = 0);
  virtual wxFont *GetFont(int regionId = 0) const;
  virtual void SetTextColour(const wxString& colour, int regionId = 0);
  virtual wxString GetTextColour(int regionId = 0) const;
  virtual inline int GetNumberOfTextRegions() const { return m_regions.Number(); }
  virtual void SetRegionName(const wxString& name, int regionId = 0);

  // Get the name representing the region for this image alone.
  // I.e. this image's region ids go from 0 to N-1.
  // But the names might be "0.2.0", "0.2.1" etc. depending on position in composite.
  // So the last digit represents the region Id, the others represent positions
  // in composites.
  virtual wxString GetRegionName(int regionId);

  // Gets the region corresponding to the name, or -1 if not found.
  virtual int GetRegionId(const wxString& name);

  // Construct names for regions, unique even for children of a composite.
  virtual void NameRegions(const wxString& parentName = "");

  // Get list of regions
  inline wxList& GetRegions() const { return (wxList&) m_regions; }

  virtual void AddRegion(wxShapeRegion *region);

  virtual void ClearRegions();

  // Assign new ids to this image and children (if composite)
  void AssignNewIds();

  // Returns actual image (same as 'this' if non-composite) and region id
  // for given region name.
  virtual wxShape *FindRegion(const wxString& regionName, int *regionId);

  // Finds all region names for this image (composite or simple).
  // Supply empty string list.
  virtual void FindRegionNames(wxStringList& list);

  virtual void ClearText(int regionId = 0);
  void RemoveLine(wxLineShape *line);

#ifdef PROLOGIO
  // I/O
  virtual void WriteAttributes(wxExpr *clause);
  virtual void ReadAttributes(wxExpr *clause);

  // In case the object has constraints it needs to read in in a different pass
  inline virtual void ReadConstraints(wxExpr *WXUNUSED(clause), wxExprDatabase *WXUNUSED(database)) { };
  virtual void WriteRegions(wxExpr *clause);
  virtual void ReadRegions(wxExpr *clause);
#endif

  // Attachment code
  virtual bool GetAttachmentPosition(int attachment, double *x, double *y,
                                     int nth = 0, int no_arcs = 1, wxLineShape *line = NULL);
  virtual int GetNumberOfAttachments() const;
  virtual bool AttachmentIsValid(int attachment) const;

  // Only get the attachment position at the _edge_ of the shape, ignoring
  // branching mode. This is used e.g. to indicate the edge of interest, not the point
  // on the attachment branch.
  virtual bool GetAttachmentPositionEdge(int attachment, double *x, double *y,
                                     int nth = 0, int no_arcs = 1, wxLineShape *line = NULL);

  // Assuming the attachment lies along a vertical or horizontal line,
  // calculate the position on that point.
  virtual wxRealPoint CalcSimpleAttachment(const wxRealPoint& pt1, const wxRealPoint& pt2,
    int nth, int noArcs, wxLineShape* line);

  // Returns TRUE if pt1 <= pt2 in the sense that one point comes before another on an
  // edge of the shape.
  // attachmentPoint is the attachment point (= side) in question.
  virtual bool AttachmentSortTest(int attachmentPoint, const wxRealPoint& pt1, const wxRealPoint& pt2);

  virtual void EraseLinks(wxDC& dc, int attachment = -1, bool recurse = FALSE);
  virtual void DrawLinks(wxDC& dc, int attachment = -1, bool recurse = FALSE);

  virtual bool MoveLineToNewAttachment(wxDC& dc, wxLineShape *to_move,
                                       double x, double y);

  // Reorders the lines coming into the node image at this attachment
  // position, in the order in which they appear in linesToSort.
  virtual void SortLines(int attachment, wxList& linesToSort);

  // Apply an attachment ordering change
  void ApplyAttachmentOrdering(wxList& ordering);

  // Can override this to prevent or intercept line reordering.
  virtual void OnChangeAttachment(int attachment, wxLineShape* line, wxList& ordering);

  //// New banching attachment code, 24/9/98

  //
  //             |________|
  //                 | <- root
  //                 | <- neck
  // shoulder1 ->---------<- shoulder2
  //             | | | | |<- stem
  //                      <- branching attachment point N-1

  // This function gets the root point at the given attachment.
  virtual wxRealPoint GetBranchingAttachmentRoot(int attachment);

  // This function gets information about where branching connections go (calls GetBranchingAttachmentRoot)
  virtual bool GetBranchingAttachmentInfo(int attachment, wxRealPoint& root, wxRealPoint& neck,
    wxRealPoint& shoulder1, wxRealPoint& shoulder2);

  // n is the number of the adjoining line, from 0 to N-1 where N is the number of lines
  // at this attachment point.
  // attachmentPoint is where the arc meets the stem, and stemPoint is where the stem meets the
  // shoulder.
  virtual bool GetBranchingAttachmentPoint(int attachment, int n, wxRealPoint& attachmentPoint,
    wxRealPoint& stemPoint);

  // Get the number of lines at this attachment position.
  virtual int GetAttachmentLineCount(int attachment) const;

  // Draw the branches (not the actual arcs though)
  virtual void OnDrawBranches(wxDC& dc, int attachment, bool erase = FALSE);
  virtual void OnDrawBranches(wxDC& dc, bool erase = FALSE);

  // Branching attachment settings
  inline void SetBranchNeckLength(int len) { m_branchNeckLength = len; }
  inline int GetBranchNeckLength() const { return m_branchNeckLength; }

  inline void SetBranchStemLength(int len) { m_branchStemLength = len; }
  inline int GetBranchStemLength() const { return m_branchStemLength; }

  inline void SetBranchSpacing(int len) { m_branchSpacing = len; }
  inline int GetBranchSpacing() const { return m_branchSpacing; }

  // Further detail on branching style, e.g. blobs on interconnections
  inline void SetBranchStyle(long style) { m_branchStyle = style; }
  inline long GetBranchStyle() const { return m_branchStyle; }

  // Rotate the standard attachment point from physical (0 is always North)
  // to logical (0 -> 1 if rotated by 90 degrees)
  virtual int PhysicalToLogicalAttachment(int physicalAttachment) const;

  // Rotate the standard attachment point from logical
  // to physical (0 is always North)
  virtual int LogicalToPhysicalAttachment(int logicalAttachment) const;

  // This is really to distinguish between lines and other images.
  // For lines, want to pass drag to canvas, since lines tend to prevent
  // dragging on a canvas (they get in the way.)
  virtual bool Draggable() const { return TRUE; }

  // Returns TRUE if image is a descendant of this image
  bool HasDescendant(wxShape *image);

  // Creates a copy of this shape.
  wxShape *CreateNewCopy(bool resetMapping = TRUE, bool recompute = TRUE);

  // Does the copying for this object
  virtual void Copy(wxShape& copy);

  // Does the copying for this object, including copying event
  // handler data if any. Calls the virtual Copy function.
  void CopyWithHandler(wxShape& copy);

  // Rotate about the given axis by the given amount in radians.
  virtual void Rotate(double x, double y, double theta);
  virtual inline double GetRotation() const { return m_rotation; }

  void ClearAttachments();

  // Recentres all the text regions for this object
  void Recentre(wxDC& dc);

  // Clears points from a list of wxRealPoints
  void ClearPointList(wxList& list);

 private:
  wxObject*             m_clientData;

 protected:
  wxShapeEvtHandler*    m_eventHandler;
  bool                  m_formatted;
  double                m_xpos, m_ypos;
  wxPen*                m_pen;
  wxBrush*              m_brush;
  wxFont*               m_font;
  wxColour*             m_textColour;
  wxString              m_textColourName;
  wxShapeCanvas*        m_canvas;
  wxList                m_lines;
  wxList                m_text;
  wxList                m_controlPoints;
  wxList                m_regions;
  wxList                m_attachmentPoints;
  bool                  m_visible;
  bool                  m_disableLabel;
  long                  m_id;
  bool                  m_selected;
  bool                  m_highlighted;      // Different from selected: user-defined highlighting,
                                            // e.g. thick border.
  double                m_rotation;
  int                   m_sensitivity;
  bool                  m_draggable;
  int                   m_attachmentMode;   // 0 for no attachments, 1 if using normal attachments,
                                            // 2 for branching attachments
  bool                  m_spaceAttachments; // TRUE if lines at one side should be spaced
  bool                  m_fixedWidth;
  bool                  m_fixedHeight;
  bool                  m_centreResize;    // Default is to resize keeping the centre constant (TRUE)
  bool                  m_drawHandles;     // Don't draw handles if FALSE, usually TRUE
  wxList                m_children;      // In case it's composite
  wxShape*              m_parent;      // In case it's a child
  int                   m_formatMode;
  int                   m_shadowMode;
  wxBrush*              m_shadowBrush;
  int                   m_shadowOffsetX;
  int                   m_shadowOffsetY;
  int                   m_textMarginX;    // Gap between text and border
  int                   m_textMarginY;
  wxString              m_regionName;
  bool                  m_maintainAspectRatio;
  int                   m_branchNeckLength;
  int                   m_branchStemLength;
  int                   m_branchSpacing;
  long                  m_branchStyle;
};

class wxPolygonShape: public wxShape
{
 DECLARE_DYNAMIC_CLASS(wxPolygonShape)
 public:
  wxPolygonShape();
  ~wxPolygonShape();

  // Takes a list of wxRealPoints; each point is an OFFSET from the centre.
  // Deletes user's points in destructor.
  virtual void Create(wxList *points);
  virtual void ClearPoints();

  void GetBoundingBoxMin(double *w, double *h);
  void CalculateBoundingBox();
  bool GetPerimeterPoint(double x1, double y1,
                                 double x2, double y2,
                                 double *x3, double *y3);
  bool HitTest(double x, double y, int *attachment, double *distance);
  void SetSize(double x, double y, bool recursive = TRUE);
  void OnDraw(wxDC& dc);
  void OnDrawOutline(wxDC& dc, double x, double y, double w, double h);

  // Control points ('handles') redirect control to the actual shape, to make it easier
  // to override sizing behaviour.
  virtual void OnSizingDragLeft(wxControlPoint* pt, bool draw, double x, double y, int keys=0, int attachment = 0);
  virtual void OnSizingBeginDragLeft(wxControlPoint* pt, double x, double y, int keys=0, int attachment = 0);
  virtual void OnSizingEndDragLeft(wxControlPoint* pt, double x, double y, int keys=0, int attachment = 0);

  // A polygon should have a control point at each vertex,
  // with the option of moving the control points individually
  // to change the shape.
  void MakeControlPoints();
  void ResetControlPoints();

  // If we've changed the shape, must make the original
  // points match the working points
  void UpdateOriginalPoints();

  // Add a control point after the given point
  virtual void AddPolygonPoint(int pos = 0);

  // Delete a control point
  virtual void DeletePolygonPoint(int pos = 0);

  // Recalculates the centre of the polygon
  virtual void CalculatePolygonCentre();

#ifdef PROLOGIO
  void WriteAttributes(wxExpr *clause);
  void ReadAttributes(wxExpr *clause);
#endif

  int GetNumberOfAttachments() const;
  bool GetAttachmentPosition(int attachment, double *x, double *y,
                                     int nth = 0, int no_arcs = 1, wxLineShape *line = NULL);
  bool AttachmentIsValid(int attachment);
  // Does the copying for this object
  void Copy(wxShape& copy);

  inline wxList *GetPoints() { return m_points; }

  // Rotate about the given axis by the given amount in radians
  virtual void Rotate(double x, double y, double theta);

 private:
  wxList*       m_points;
  wxList*       m_originalPoints;
  double        m_boundWidth;
  double        m_boundHeight;
  double        m_originalWidth;
  double        m_originalHeight;
};

class wxRectangleShape: public wxShape
{
 DECLARE_DYNAMIC_CLASS(wxRectangleShape)
 public:
  wxRectangleShape(double w = 0.0, double h = 0.0);
  void GetBoundingBoxMin(double *w, double *h);
  bool GetPerimeterPoint(double x1, double y1,
                                 double x2, double y2,
                                 double *x3, double *y3);
  void OnDraw(wxDC& dc);
  void SetSize(double x, double y, bool recursive = TRUE);
  void SetCornerRadius(double rad); // If > 0, rounded corners

#ifdef PROLOGIO
  void WriteAttributes(wxExpr *clause);
  void ReadAttributes(wxExpr *clause);
#endif

  int GetNumberOfAttachments() const;
  bool GetAttachmentPosition(int attachment, double *x, double *y,
                                     int nth = 0, int no_arcs = 1, wxLineShape *line = NULL);
  // Does the copying for this object
  void Copy(wxShape& copy);

  inline double GetWidth() const { return m_width; }
  inline double GetHeight() const { return m_height; }
  inline void SetWidth(double w) { m_width = w; }
  inline void SetHeight(double h) { m_height = h; }

protected:
  double m_width;
  double m_height;
  double m_cornerRadius;
};

class wxTextShape: public wxRectangleShape
{
 DECLARE_DYNAMIC_CLASS(wxTextShape)
 public:
  wxTextShape(double width = 0.0, double height = 0.0);

  void OnDraw(wxDC& dc);

#ifdef PROLOGIO
  void WriteAttributes(wxExpr *clause);
#endif

  // Does the copying for this object
  void Copy(wxShape& copy);
};

class wxEllipseShape: public wxShape
{
 DECLARE_DYNAMIC_CLASS(wxEllipseShape)
 public:
  wxEllipseShape(double w = 0.0, double h = 0.0);

  void GetBoundingBoxMin(double *w, double *h);
  bool GetPerimeterPoint(double x1, double y1,
                                 double x2, double y2,
                                 double *x3, double *y3);

  void OnDraw(wxDC& dc);
  void SetSize(double x, double y, bool recursive = TRUE);

#ifdef PROLOGIO
  void WriteAttributes(wxExpr *clause);
  void ReadAttributes(wxExpr *clause);
#endif

  int GetNumberOfAttachments() const;
  bool GetAttachmentPosition(int attachment, double *x, double *y,
                                     int nth = 0, int no_arcs = 1, wxLineShape *line = NULL);

  // Does the copying for this object
  void Copy(wxShape& copy);

  inline double GetWidth() const { return m_width; }
  inline double GetHeight() const { return m_height; }

  inline void SetWidth(double w) { m_width = w; }
  inline void SetHeight(double h) { m_height = h; }

protected:
  double m_width;
  double m_height;
};

class wxCircleShape: public wxEllipseShape
{
 DECLARE_DYNAMIC_CLASS(wxCircleShape)
 public:
  wxCircleShape(double w = 0.0);

  bool GetPerimeterPoint(double x1, double y1,
                                 double x2, double y2,
                                 double *x3, double *y3);
  // Does the copying for this object
  void Copy(wxShape& copy);
};

#endif
 // _OGL_BASIC_H_
