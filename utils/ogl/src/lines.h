/////////////////////////////////////////////////////////////////////////////
// Name:        lines.h
// Purpose:     wxLineShape
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _OGL_LINES_H_
#define _OGL_LINES_H_

#ifdef __GNUG__
#pragma interface "lines.h"
#endif

class wxLabelShape;
class wxPseudoMetaFile;
class wxLineControlPoint;
/*
 * Arcs with multiple arrowheads
 *
 */

// Types of arrowhead
// (i) Built-in
#define ARROW_HOLLOW_CIRCLE   1
#define ARROW_FILLED_CIRCLE   2
#define ARROW_ARROW           3
#define ARROW_SINGLE_OBLIQUE  4
#define ARROW_DOUBLE_OBLIQUE  5
// (ii) Custom
#define ARROW_METAFILE        20

// Position of arrow on line
#define ARROW_POSITION_START  0
#define ARROW_POSITION_END    1
#define ARROW_POSITION_MIDDLE 2

// Line alignment flags
// Vertical by default
#define LINE_ALIGNMENT_HORIZ              1
#define LINE_ALIGNMENT_VERT               0
#define LINE_ALIGNMENT_TO_NEXT_HANDLE     2
#define LINE_ALIGNMENT_NONE               0

class wxArrowHead: public wxObject
{
 DECLARE_DYNAMIC_CLASS(wxArrowHead)

 public:
  wxArrowHead(WXTYPE type = 0, int end = 0, double size = 0.0, double dist = 0.0, const wxString& name = "", wxPseudoMetaFile *mf = NULL,
            long arrowId = -1);
  ~wxArrowHead();
  wxArrowHead(wxArrowHead& toCopy);

  inline WXTYPE _GetType() const { return m_arrowType; }
  inline int GetPosition() const { return m_arrowEnd; }
  inline void SetPosition(int pos) { m_arrowEnd = pos; }
  inline double GetXOffset() const { return m_xOffset; }
  inline double GetYOffset() const { return m_yOffset; }
  inline double GetSpacing() const { return m_spacing; }
  inline double GetSize() const { return m_arrowSize; }
  inline wxString GetName() const { return m_arrowName; }
  inline void SetXOffset(double x) { m_xOffset = x; }
  inline void SetYOffset(double y) { m_yOffset = y; }
  inline wxPseudoMetaFile *GetMetaFile() const { return m_metaFile; }
  inline long GetId() const { return m_id; }
  inline int GetArrowEnd() const { return m_arrowEnd; }
  inline double GetArrowSize() const { return m_arrowSize; }
  void SetSize(double size);
  inline void SetSpacing(double sp) { m_spacing = sp; }

 protected:
  WXTYPE            m_arrowType;
  int               m_arrowEnd;         // Position on line
  double             m_xOffset;          // Distance from arc start or end, w.r.t. point on arrowhead
                                        // nearest start or end. If zero, use default spacing.
  double             m_yOffset;          // vertical offset (w.r.t. a horizontal line). Normally zero.
  double             m_spacing;          // Spacing from the last arrowhead
  double             m_arrowSize;        // Length of arrowhead
  wxString          m_arrowName;        // Name of arrow
  bool              m_saveToFile;       // TRUE if we want to save custom arrowheads to file.
  wxPseudoMetaFile* m_metaFile;         // Pseudo metafile if this is a custom arrowhead
  long              m_id;               // identifier
};

// Line object
class wxLineShape: public wxShape
{
 DECLARE_DYNAMIC_CLASS(wxLineShape)

 public:
  wxLineShape();
  ~wxLineShape();

  // Called when a connected object has moved, to move the link to
  // correct position
  // moveControlPoints must be disabled when a control point is being
  // dragged.
  void OnMoveLink(wxDC& dc, bool moveControlPoints = TRUE);
  bool OnMovePre(wxDC& dc, double x, double y, double old_x, double old_y, bool display = TRUE);
  void OnDraw(wxDC& dc);
  void OnDrawContents(wxDC& dc);
  void OnDrawControlPoints(wxDC& dc);
  void OnEraseControlPoints(wxDC& dc);
  void OnErase(wxDC& dc);
  virtual inline void OnMoveControlPoint(int WXUNUSED(which), double WXUNUSED(x), double WXUNUSED(y)) {}
  void OnDrawOutline(wxDC& dc, double x, double y, double w, double h);
  void GetBoundingBoxMin(double *w, double *h);
  void FormatText(wxDC& dc, const wxString& s, int regionId = 0);
  virtual void SetEnds(double x1, double y1, double x2, double y2);
  virtual void GetEnds(double *x1, double *y1, double *x2, double *y2);
  inline virtual wxShape *GetFrom() { return m_from; }
  inline virtual wxShape *GetTo() { return m_to; }
  inline virtual int GetAttachmentFrom() { return m_attachmentFrom; }
  inline virtual int GetAttachmentTo() { return m_attachmentTo; }

  virtual void SetFrom(wxShape *object);
  virtual void SetTo(wxShape *object);
  virtual void DrawArrows(wxDC& dc);

  // Finds the x, y points at the two ends of the line.
  // This function can be used by e.g. line-routing routines to
  // get the actual points on the two node images where the lines will be drawn
  // to/from.
  void FindLineEndPoints(double *fromX, double *fromY, double *toX, double *toY);

  // Format one region at this position
  void DrawRegion(wxDC& dc, wxShapeRegion *region, double x, double y);

  // Erase one region at this position
  void EraseRegion(wxDC& dc, wxShapeRegion *region, double x, double y);

  // Get the reference point for a label. Region x and y
  // are offsets from this.
  // position is 0 (middle), 1 (start), 2 (end)
  void GetLabelPosition(int position, double *x, double *y);

  // Straighten verticals and horizontals
  virtual void Straighten(wxDC& dc);

  // Not implemented
  inline void SetMaintainStraightLines(bool flag) { m_maintainStraightLines = flag; }
  inline bool GetMaintainStraightLines() const { return m_maintainStraightLines; }

  // Make handle control points
  void MakeControlPoints();
  void ResetControlPoints();

  // Make a given number of control points
  virtual void MakeLineControlPoints(int n);
  virtual wxNode *InsertLineControlPoint(wxDC* dc);
  virtual bool DeleteLineControlPoint();
  virtual void Initialise();
  inline wxList *GetLineControlPoints() { return m_lineControlPoints; }

  // Override dragging behaviour - don't want to be able to drag lines!
  void OnDragLeft(bool draw, double x, double y, int keys=0, int attachment = 0);
  void OnBeginDragLeft(double x, double y, int keys=0, int attachment = 0);
  void OnEndDragLeft(double x, double y, int keys=0, int attachment = 0);

  // Control points ('handles') redirect control to the actual shape, to make it easier
  // to override sizing behaviour.
  virtual void OnSizingDragLeft(wxControlPoint* pt, bool draw, double x, double y, int keys=0, int attachment = 0);
  virtual void OnSizingBeginDragLeft(wxControlPoint* pt, double x, double y, int keys=0, int attachment = 0);
  virtual void OnSizingEndDragLeft(wxControlPoint* pt, double x, double y, int keys=0, int attachment = 0);

  // Override select, to create/delete temporary label-moving objects
  void Select(bool select = TRUE, wxDC* dc = NULL);

  // Set to spline (TRUE) or line (FALSE)
  inline void SetSpline(bool spl) { m_isSpline = spl; }
  inline bool IsSpline() const { return m_isSpline; }

  void Unlink();
  void SetAttachments(int from_attach, int to_attach);
  inline void SetAttachmentFrom(int attach) { m_attachmentFrom = attach; }
  inline void SetAttachmentTo(int attach) { m_attachmentTo = attach; }

  bool HitTest(double x, double y, int *attachment, double *distance);

#ifdef PROLOGIO
  // Prolog database stuff
  virtual char *GetFunctor();
  virtual void WritePrologAttributes(wxExpr *clause);
  virtual void ReadPrologAttributes(wxExpr *clause);
#endif

  virtual void FindNth(wxShape *image, int *nth, int *no_arcs, bool incoming);

  // Find which position we're talking about at this (x, y).
  // Returns ARROW_POSITION_START, ARROW_POSITION_MIDDLE, ARROW_POSITION_END
  int FindLinePosition(double x, double y);

  // This is really to distinguish between lines and other images.
  // For lines, want to pass drag to canvas, since lines tend to prevent
  // dragging on a canvas (they get in the way.)
  virtual bool Draggable() const { return FALSE; }

  // Does the copying for this object
  void Copy(wxShape& copy);

  // Add an arrowhead.
  wxArrowHead *AddArrow(WXTYPE type, int end = ARROW_POSITION_END,
                double arrowSize = 10.0, double xOffset = 0.0, const wxString& name = "",
                wxPseudoMetaFile *mf = NULL, long arrowId = -1);

  // Add an arrowhead in the position indicated by the reference
  // list of arrowheads, which contains all legal arrowheads for this
  // line, in the correct order.
  // E.g. reference list:      a b c d e
  //      Current line list:   a d
  // Add c, then line list is: a c d
  // If no legal arrowhead position, return FALSE.
  // Assume reference list is for one end only, since it potentially defines
  // the ordering for any one of the 3 positions. So we don't check
  // the reference list for arrowhead position.
  bool AddArrowOrdered(wxArrowHead *arrow, wxList& referenceList, int end);

  // Delete arrowhead(s)
  void ClearArrowsAtPosition(int end = -1);
  bool ClearArrow(const wxString& name);
  wxArrowHead *FindArrowHead(int position, const wxString& name);
  wxArrowHead *FindArrowHead(long arrowId);
  bool DeleteArrowHead(int position, const wxString& name);
  bool DeleteArrowHead(long arrowId);
  void DrawArrow(wxDC& dc, wxArrowHead *arrow, double xOffset, bool proportionalOffset);
  inline void SetIgnoreOffsets(bool ignore) { m_ignoreArrowOffsets = ignore; }
  inline wxList& GetArrows() const { return (wxList&) m_arcArrows; }

  // Find horizontal width for drawing a line with
  // arrows in minimum space. Assume arrows at
  // END only
  double FindMinimumWidth();

  // Set alignment flags. ALIGNMENT NOT IMPLEMENTED.
  void SetAlignmentOrientation(bool isEnd, bool isHoriz);
  void SetAlignmentType(bool isEnd, int alignType);
  bool GetAlignmentOrientation(bool isEnd);
  int GetAlignmentType(bool isEnd);

  // Find next control point in line after the start/end point
  // (depending on whether the node object is at start or end)
  wxRealPoint *GetNextControlPoint(wxShape *nodeObject);
  inline bool IsEnd(wxShape *nodeObject) const { return (m_to == nodeObject); }

private:
  bool              m_erasing;              // flag to say whether we're erasing or drawing
                                            // this line (really so metafiles can draw a
                                            // blank rectangle)
  bool              m_ignoreArrowOffsets;   // Don't always want to draw arrowhead offsets
                                            // because they may not work on tool palettes (for example)
  bool              m_isSpline;
  bool              m_maintainStraightLines;

protected:
  // Temporary list of line segment orientations
  // so we know what direction the line is supposed to be dog-legging
  // in. The values are integer: 0 for vertical, 1 for horizontal.
  wxList            m_lineOrientations;

  // Temporary pointers for start, middle and end label editing objects
  // (active only when the line is selected)
  wxLabelShape*     m_labelObjects[3];

  // These define the segmented line - not to be confused with temporary control
  // points which appear when object is selected (although in this case they'll
  // probably be the same)
  wxList*           m_lineControlPoints;

  double             m_arrowSpacing; // Separation between adjacent arrows

  wxShape*          m_to;
  wxShape*          m_from;

/*
  double             m_actualTextWidth;  // Space the text takes up
  double             m_actualTextHeight; // (depends on text content unlike nodes)
*/
  int               m_attachmentTo;   // Attachment point at one end
  int               m_attachmentFrom; // Attachment point at other end

  // Alignment flags
  int               m_alignmentStart;
  int               m_alignmentEnd;

  wxList            m_arcArrows;

};

#endif
    // _OGL_LINES_H_
