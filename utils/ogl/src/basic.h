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
  virtual void OnMoveLinks(wxDC& dc);
  virtual void OnErase(wxDC& dc);
  virtual void OnEraseContents(wxDC& dc);
  virtual void OnHighlight(wxDC& dc);
  virtual void OnLeftClick(float x, float y, int keys = 0, int attachment = 0);
  virtual void OnRightClick(float x, float y, int keys = 0, int attachment = 0);
  virtual void OnSize(float x, float y);
  virtual bool OnMovePre(wxDC& dc, float x, float y, float old_x, float old_y, bool display = TRUE);
  virtual void OnMovePost(wxDC& dc, float x, float y, float old_x, float old_y, bool display = TRUE);

  virtual void OnDragLeft(bool draw, float x, float y, int keys=0, int attachment = 0); // Erase if draw false
  virtual void OnBeginDragLeft(float x, float y, int keys=0, int attachment = 0);
  virtual void OnEndDragLeft(float x, float y, int keys=0, int attachment = 0);
  virtual void OnDragRight(bool draw, float x, float y, int keys=0, int attachment = 0); // Erase if draw false
  virtual void OnBeginDragRight(float x, float y, int keys=0, int attachment = 0);
  virtual void OnEndDragRight(float x, float y, int keys=0, int attachment = 0);
  virtual void OnDrawOutline(wxDC& dc, float x, float y, float w, float h);
  virtual void OnDrawControlPoints(wxDC& dc);
  virtual void OnEraseControlPoints(wxDC& dc);
  virtual void OnMoveLink(wxDC& dc, bool moveControlPoints = TRUE);

  // Control points ('handles') redirect control to the actual shape, to make it easier
  // to override sizing behaviour.
  virtual void OnSizingDragLeft(wxControlPoint* pt, bool draw, float x, float y, int keys=0, int attachment = 0); // Erase if draw false
  virtual void OnSizingBeginDragLeft(wxControlPoint* pt, float x, float y, int keys=0, int attachment = 0);
  virtual void OnSizingEndDragLeft(wxControlPoint* pt, float x, float y, int keys=0, int attachment = 0);

  virtual void OnBeginSize(float WXUNUSED(w), float WXUNUSED(h)) { }
  virtual void OnEndSize(float WXUNUSED(w), float WXUNUSED(h)) { }

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
  virtual void GetBoundingBoxMax(float *width, float *height);
  virtual void GetBoundingBoxMin(float *width, float *height) = 0;
  virtual bool GetPerimeterPoint(float x1, float y1,
                                 float x2, float y2,
                                 float *x3, float *y3);
  inline wxShapeCanvas *GetCanvas() { return m_canvas; }
  void SetCanvas(wxShapeCanvas *the_canvas);
  virtual void AddToCanvas(wxShapeCanvas *the_canvas, wxShape *addAfter = NULL);
  virtual void InsertInCanvas(wxShapeCanvas *the_canvas);

  virtual void RemoveFromCanvas(wxShapeCanvas *the_canvas);
  inline float GetX() const { return m_xpos; }
  inline float GetY() const { return m_ypos; }
  inline void SetX(float x) { m_xpos = x; }
  inline void SetY(float y) { m_ypos = y; }

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
  virtual void OnLeftClick(float x, float y, int keys = 0, int attachment = 0);
  virtual void OnRightClick(float x, float y, int keys = 0, int attachment = 0);
  virtual void OnSize(float x, float y);
  virtual bool OnMovePre(wxDC& dc, float x, float y, float old_x, float old_y, bool display = TRUE);
  virtual void OnMovePost(wxDC& dc, float x, float y, float old_x, float old_y, bool display = TRUE);

  virtual void OnDragLeft(bool draw, float x, float y, int keys=0, int attachment = 0); // Erase if draw false
  virtual void OnBeginDragLeft(float x, float y, int keys=0, int attachment = 0);
  virtual void OnEndDragLeft(float x, float y, int keys=0, int attachment = 0);
  virtual void OnDragRight(bool draw, float x, float y, int keys=0, int attachment = 0); // Erase if draw false
  virtual void OnBeginDragRight(float x, float y, int keys=0, int attachment = 0);
  virtual void OnEndDragRight(float x, float y, int keys=0, int attachment = 0);
  virtual void OnDrawOutline(wxDC& dc, float x, float y, float w, float h);
  virtual void OnDrawControlPoints(wxDC& dc);
  virtual void OnEraseControlPoints(wxDC& dc);

  virtual void OnBeginSize(float WXUNUSED(w), float WXUNUSED(h)) { }
  virtual void OnEndSize(float WXUNUSED(w), float WXUNUSED(h)) { }

  // Control points ('handles') redirect control to the actual shape, to make it easier
  // to override sizing behaviour.
  virtual void OnSizingDragLeft(wxControlPoint* pt, bool draw, float x, float y, int keys=0, int attachment = 0); // Erase if draw false
  virtual void OnSizingBeginDragLeft(wxControlPoint* pt, float x, float y, int keys=0, int attachment = 0);
  virtual void OnSizingEndDragLeft(wxControlPoint* pt, float x, float y, int keys=0, int attachment = 0);

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
  virtual bool HitTest(float x, float y, int *attachment, float *distance);
  inline void SetCentreResize(bool cr) { m_centreResize = cr; }
  inline bool GetCentreResize() const { return m_centreResize; }
  inline wxList& GetLines() { return m_lines; }
  inline void SetDisableLabel(bool flag) { m_disableLabel = flag; }
  inline bool GetDisableLabel() const { return m_disableLabel; }
  inline void SetAttachmentMode(bool flag) { m_attachmentMode = flag; }
  inline bool GetAttachmentMode() const { return m_attachmentMode; }
  inline void SetId(long i) { m_id = i; }
  inline long GetId() const { return m_id; }

  void SetPen(wxPen *pen);
  void SetBrush(wxBrush *brush);
  inline void SetClientData(wxObject *client_data) { m_clientData = client_data; };
  inline wxObject *GetClientData() const { return m_clientData; };

  virtual void Show(bool show);
  virtual bool IsShown() const { return m_visible; }
  virtual void Move(wxDC& dc, float x1, float y1, bool display = TRUE);
  virtual void Erase(wxDC& dc);
  virtual void EraseContents(wxDC& dc);
  virtual void Draw(wxDC& dc);
  virtual void Flash();
  virtual void MoveLinks(wxDC& dc);
  virtual void DrawContents(wxDC& dc);  // E.g. for drawing text label
  virtual void SetSize(float x, float y, bool recursive = TRUE);
  virtual void SetAttachmentSize(float x, float y);
  void Attach(wxShapeCanvas *can);
  void Detach();

  inline virtual bool Constrain() { return FALSE; } ;

  void AddLine(wxLineShape *line, wxShape *other,
               int attachFrom = 0, int attachTo = 0);
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
  // Prolog database stuff
  virtual char *GetFunctor();
  virtual void WritePrologAttributes(wxExpr *clause);
  virtual void ReadPrologAttributes(wxExpr *clause);

  // In case the object has constraints it needs to read in in a different pass
  inline virtual void ReadConstraints(wxExpr *WXUNUSED(clause), wxExprDatabase *WXUNUSED(database)) { };
  virtual void WriteRegions(wxExpr *clause);
  virtual void ReadRegions(wxExpr *clause);
#endif

  // Attachment code
  virtual bool GetAttachmentPosition(int attachment, float *x, float *y,
                                     int nth = 0, int no_arcs = 1, wxLineShape *line = NULL);
  virtual int GetNumberOfAttachments();
  virtual bool AttachmentIsValid(int attachment);

  virtual void EraseLinks(wxDC& dc, int attachment = -1, bool recurse = FALSE);
  virtual void DrawLinks(wxDC& dc, int attachment = -1, bool recurse = FALSE);

  virtual void MoveLineToNewAttachment(wxDC& dc, wxLineShape *to_move,
                                       float x, float y);

  // Reorders the lines coming into the node image at this attachment
  // position, in the order in which they appear in linesToSort.
  virtual void SortLines(int attachment, wxList& linesToSort);

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

  // Rotate about the given axis by the given amount in radians
  // (does nothing for most objects)
  // But even non-rotating objects should record their notional
  // rotation in case it's important (e.g. in dog-leg code).
  virtual inline void Rotate(float WXUNUSED(x), float WXUNUSED(y), float theta) { m_rotation = theta; }
  virtual inline float GetRotation() const { return m_rotation; }

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
  float                 m_xpos, m_ypos;
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
  float                 m_rotation;
  int                   m_sensitivity;
  bool                  m_draggable;
  bool                  m_attachmentMode;   // TRUE if using attachments, FALSE otherwise
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

  void GetBoundingBoxMin(float *w, float *h);
  void CalculateBoundingBox();
  bool GetPerimeterPoint(float x1, float y1,
                                 float x2, float y2,
                                 float *x3, float *y3);
  bool HitTest(float x, float y, int *attachment, float *distance);
  void SetSize(float x, float y, bool recursive = TRUE);
  void OnDraw(wxDC& dc);
  void OnDrawOutline(wxDC& dc, float x, float y, float w, float h);

  // Control points ('handles') redirect control to the actual shape, to make it easier
  // to override sizing behaviour.
  virtual void OnSizingDragLeft(wxControlPoint* pt, bool draw, float x, float y, int keys=0, int attachment = 0);
  virtual void OnSizingBeginDragLeft(wxControlPoint* pt, float x, float y, int keys=0, int attachment = 0);
  virtual void OnSizingEndDragLeft(wxControlPoint* pt, float x, float y, int keys=0, int attachment = 0);

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
  // Prolog database stuff
  void WritePrologAttributes(wxExpr *clause);
  void ReadPrologAttributes(wxExpr *clause);
#endif

  int GetNumberOfAttachments();
  bool GetAttachmentPosition(int attachment, float *x, float *y,
                                     int nth = 0, int no_arcs = 1, wxLineShape *line = NULL);
  bool AttachmentIsValid(int attachment);
  // Does the copying for this object
  void Copy(wxShape& copy);

  inline wxList *GetPoints() { return m_points; }

 private:
  wxList*       m_points;
  wxList*       m_originalPoints;
  float         m_boundWidth;
  float         m_boundHeight;
  float         m_originalWidth;
  float         m_originalHeight;
};

class wxRectangleShape: public wxShape
{
 DECLARE_DYNAMIC_CLASS(wxRectangleShape)
 public:
  wxRectangleShape(float w = 0.0, float h = 0.0);
  void GetBoundingBoxMin(float *w, float *h);
  bool GetPerimeterPoint(float x1, float y1,
                                 float x2, float y2,
                                 float *x3, float *y3);
  void OnDraw(wxDC& dc);
  void SetSize(float x, float y, bool recursive = TRUE);
  void SetCornerRadius(float rad); // If > 0, rounded corners

#ifdef PROLOGIO
  // Prolog database stuff
  void WritePrologAttributes(wxExpr *clause);
  void ReadPrologAttributes(wxExpr *clause);
#endif

  int GetNumberOfAttachments();
  bool GetAttachmentPosition(int attachment, float *x, float *y,
                                     int nth = 0, int no_arcs = 1, wxLineShape *line = NULL);
  // Does the copying for this object
  void Copy(wxShape& copy);

  inline float GetWidth() const { return m_width; }
  inline float GetHeight() const { return m_height; }

protected:
  float m_width;
  float m_height;
  float m_cornerRadius;
};

class wxTextShape: public wxRectangleShape
{
 DECLARE_DYNAMIC_CLASS(wxTextShape)
 public:
  wxTextShape(float width = 0.0, float height = 0.0);

  void OnDraw(wxDC& dc);

#ifdef PROLOGIO
  void WritePrologAttributes(wxExpr *clause);
#endif

  // Does the copying for this object
  void Copy(wxShape& copy);
};

class wxEllipseShape: public wxShape
{
 DECLARE_DYNAMIC_CLASS(wxEllipseShape)
 public:
  wxEllipseShape(float w = 0.0, float h = 0.0);

  void GetBoundingBoxMin(float *w, float *h);
  bool GetPerimeterPoint(float x1, float y1,
                                 float x2, float y2,
                                 float *x3, float *y3);

  void OnDraw(wxDC& dc);
  void SetSize(float x, float y, bool recursive = TRUE);

#ifdef PROLOGIO
  // Prolog database stuff
  void WritePrologAttributes(wxExpr *clause);
  void ReadPrologAttributes(wxExpr *clause);
#endif

  int GetNumberOfAttachments();
  bool GetAttachmentPosition(int attachment, float *x, float *y,
                                     int nth = 0, int no_arcs = 1, wxLineShape *line = NULL);

  // Does the copying for this object
  void Copy(wxShape& copy);

  inline float GetWidth() const { return m_width; }
  inline float GetHeight() const { return m_height; }

protected:
  float m_width;
  float m_height;
};

class wxCircleShape: public wxEllipseShape
{
 DECLARE_DYNAMIC_CLASS(wxCircleShape)
 public:
  wxCircleShape(float w = 0.0);

  bool GetPerimeterPoint(float x1, float y1,
                                 float x2, float y2,
                                 float *x3, float *y3);
  // Does the copying for this object
  void Copy(wxShape& copy);
};

#endif
 // _OGL_BASIC_H_
