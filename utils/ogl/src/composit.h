/////////////////////////////////////////////////////////////////////////////
// Name:        composit.h
// Purpose:     wxCompositeShape
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _OGL_COMPOSIT_H_
#define _OGL_COMPOSIT_H_

#ifdef __GNUG__
#pragma interface "composit.h"
#endif

class wxDivisionShape;
class OGLConstraint;

/*
 * A composite object is an invisible rectangle surrounding all children
 *
 */

class wxCompositeShape: public wxRectangleShape
{
 DECLARE_DYNAMIC_CLASS(wxCompositeShape)
public:

  wxCompositeShape();
  ~wxCompositeShape();

  void OnDraw(wxDC& dc);
  void OnDrawContents(wxDC& dc);
  void OnErase(wxDC& dc);
  bool OnMovePre(wxDC& dc, float x, float y, float oldX, float oldY, bool display = TRUE);
  void OnDragLeft(bool draw, float x, float y, int keys, int attachment = 0);
  void OnBeginDragLeft(float x, float y, int keys, int attachment = 0);
  void OnEndDragLeft(float x, float y, int keys, int attachment = 0);

  void OnRightClick(float x, float y, int keys, int attachment = 0);

  void SetSize(float w, float h, bool recursive = TRUE);

  // Returns TRUE if it settled down
  bool Recompute();

  // New members
  void AddChild(wxShape *child, wxShape *addAfter = NULL);
  void RemoveChild(wxShape *child);

  OGLConstraint *AddConstraint(OGLConstraint *constraint);
  OGLConstraint *AddConstraint(int type, wxShape *constraining, wxList& constrained);
  OGLConstraint *AddConstraint(int type, wxShape *constraining, wxShape *constrained);

  void DeleteConstraint(OGLConstraint *constraint);

  // Delete constraints that involve this child.
  void DeleteConstraintsInvolvingChild(wxShape *child);

  // Remove the image from any constraints involving it, but DON'T
  // remove any constraints.
  void RemoveChildFromConstraints(wxShape *child);

  // Find constraint, also returning actual composite the constraint was in,
  // in case it had to find it recursively.
  OGLConstraint *FindConstraint(long id, wxCompositeShape **actualComposite = NULL);

  // Returns TRUE if something changed
  bool Constrain();

  // Make this composite into a container by creating one wxDivisionShape
  void MakeContainer();

  // Calculates size and position of composite object based on children
  void CalculateSize();

#ifdef PROLOGIO
  // Prolog database stuff
  void WritePrologAttributes(wxExpr *clause);
  void ReadPrologAttributes(wxExpr *clause);
  // In case the object has constraints it needs to read in in a different pass
  void ReadConstraints(wxExpr *clause, wxExprDatabase *database);
#endif
  // Does the copying for this object
  void Copy(wxCompositeShape& copy);
  wxShape *PrivateCopy();

  virtual wxDivisionShape *OnCreateDivision();

  // Finds the image used to visualize a container. This is any child
  // of the composite that is not in the divisions list.
  wxShape *FindContainerImage();

  // Returns TRUE if division is a descendant of this container
  bool ContainsDivision(wxDivisionShape *division);

  inline wxList& GetDivisions() const { return (wxList&) m_divisions; }
  inline wxList& GetConstraints() const { return (wxList&) m_constraints; }

protected:
  float             m_oldX;
  float             m_oldY;
  wxList            m_constraints;
  wxList            m_divisions; // In case it's a container
};

/*
 * A division object is a composite with special properties,
 * to be used for containment. It's a subdivision of a container.
 * A containing node image consists of a composite with a main child shape
 * such as rounded rectangle, plus a list of division objects.
 * It needs to be a composite because a division contains pieces
 * of diagram.
 * NOTE a container has at least one wxDivisionShape for consistency.
 * This can be subdivided, so it turns into two objects, then each of
 * these can be subdivided, etc.
 */
#define DIVISION_SIDE_NONE      0
#define DIVISION_SIDE_LEFT      1
#define DIVISION_SIDE_TOP       2
#define DIVISION_SIDE_RIGHT     3
#define DIVISION_SIDE_BOTTOM    4

class wxDivisionShape: public wxCompositeShape
{
 DECLARE_DYNAMIC_CLASS(wxDivisionShape)
 public:

  wxDivisionShape();
  ~wxDivisionShape();

  void OnDraw(wxDC& dc);
  void OnDrawContents(wxDC& dc);
  bool OnMovePre(wxDC& dc, float x, float y, float oldX, float oldY, bool display = TRUE);
  void OnDragLeft(bool draw, float x, float y, int keys, int attachment = 0);
  void OnBeginDragLeft(float x, float y, int keys, int attachment = 0);
  void OnEndDragLeft(float x, float y, int keys, int attachment = 0);

  void OnRightClick(float x, float y, int keys = 0, int attachment = 0);

  // Don't want this kind of composite to resize its subdiagrams, so
  // override composite's SetSize.
  void SetSize(float w, float h, bool recursive = TRUE);

  // Similarly for calculating size: it's fixed at whatever SetSize
  // set it to, not in terms of children.
  void CalculateSize();

  void MakeControlPoints();
  void ResetControlPoints();
  void MakeMandatoryControlPoints();
  void ResetMandatoryControlPoints();

#ifdef PROLOGIO
  // Prolog database stuff
  void WritePrologAttributes(wxExpr *clause);
  void ReadPrologAttributes(wxExpr *clause);
#endif
  // Does the copying for this object
  void Copy(wxDivisionShape& copy);
  wxShape *PrivateCopy();

  // Divide horizontally (wxHORIZONTAL) or vertically (wxVERTICAL)
  bool Divide(int direction);

  // Resize adjoining divisions at the given side. If test is TRUE,
  // just see whether it's possible for each adjoining region,
  // returning FALSE if it's not.
  bool ResizeAdjoining(int side, float newPos, bool test);

  // Adjust a side, returning FALSE if it's not physically possible.
  bool AdjustLeft(float left, bool test);
  bool AdjustTop(float top, bool test);
  bool AdjustRight(float right, bool test);
  bool AdjustBottom(float bottom, bool test);

  // Edit style of left or top side
  void EditEdge(int side);

  // Popup menu
  void PopupMenu(float x, float y);
  
  inline void SetLeftSide(wxDivisionShape *shape) { m_leftSide = shape; }
  inline void SetTopSide(wxDivisionShape *shape) { m_topSide = shape; }
  inline void SetRightSide(wxDivisionShape *shape) { m_rightSide = shape; }
  inline void SetBottomSide(wxDivisionShape *shape) { m_bottomSide = shape; }
  inline wxDivisionShape *GetLeftSide() const { return m_leftSide; }
  inline wxDivisionShape *GetTopSide() const { return m_topSide; }
  inline wxDivisionShape *GetRightSide() const { return m_rightSide; }
  inline wxDivisionShape *GetBottomSide() const { return m_bottomSide; }

  inline void SetHandleSide(int side) { m_handleSide = side; }
  inline int GetHandleSide() const { return m_handleSide; }

  inline void SetLeftSidePen(wxPen *pen) { m_leftSidePen = pen; }
  inline wxPen *GetLeftSidePen() const { return m_leftSidePen; }
  inline void SetTopSidePen(wxPen *pen) { m_topSidePen = pen; }
  inline wxPen *GetTopSidePen() const { return m_topSidePen; }

  void SetLeftSideColour(const wxString& colour);
  void SetTopSideColour(const wxString& colour);
  void SetLeftSideStyle(const wxString& style);
  void SetTopSideStyle(const wxString& style);

  inline wxString GetLeftSideColour() const { return m_leftSideColour; }
  inline wxString GetTopSideColour() const { return m_topSideColour; }
  inline wxString GetLeftSideStyle() const { return m_leftSideStyle; }
  inline wxString GetTopSideStyle() const { return m_topSideStyle; }

 protected:
  // Adjoining divisions. NULL indicates edge
  // of container, and that side shouldn't be
  // drawn.
  wxDivisionShape*      m_leftSide;
  wxDivisionShape*      m_rightSide;
  wxDivisionShape*      m_topSide;
  wxDivisionShape*      m_bottomSide;

  int                   m_handleSide;       // Side at which handle is legal

  wxPen*                m_leftSidePen;
  wxPen*                m_topSidePen;
  wxString              m_leftSideColour;
  wxString              m_topSideColour;
  wxString              m_leftSideStyle;
  wxString              m_topSideStyle;
};

extern wxMenu *oglPopupDivisionMenu;
extern void oglGraphicsDivisionMenuProc(wxMenu& menu, wxCommandEvent& event);

#define DIVISION_MENU_SPLIT_HORIZONTALLY    1
#define DIVISION_MENU_SPLIT_VERTICALLY      2
#define DIVISION_MENU_EDIT_LEFT_EDGE        3
#define DIVISION_MENU_EDIT_TOP_EDGE         4
#define DIVISION_MENU_EDIT_RIGHT_EDGE       5
#define DIVISION_MENU_EDIT_BOTTOM_EDGE      6
#define DIVISION_MENU_DELETE_ALL            7

#endif
 // _OGL_COMPOSIT_H_
