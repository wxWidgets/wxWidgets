/////////////////////////////////////////////////////////////////////////////
// Name:        composit.cpp
// Purpose:     Composite OGL class
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "composit.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#ifdef PROLOGIO
#include <wx/wxexpr.h>
#endif

#include "basic.h"
#include "basicp.h"
#include "constrnt.h"
#include "composit.h"
#include "misc.h"
#include "canvas.h"

// Sometimes, objects need to access the whole database to
// construct themselves.
wxExprDatabase *GlobalwxExprDatabase = NULL;

// Popup menu for editing divisions
wxMenu *oglPopupDivisionMenu = NULL;

/*
 * Division control point
 */
 
class wxDivisionControlPoint: public wxControlPoint
{
 DECLARE_DYNAMIC_CLASS(wxDivisionControlPoint)
 public:
  wxDivisionControlPoint() {}
  wxDivisionControlPoint(wxShapeCanvas *the_canvas, wxShape *object, float size, float the_xoffset, float the_yoffset, int the_type);
  ~wxDivisionControlPoint();

  void OnDragLeft(bool draw, float x, float y, int keys=0, int attachment = 0);
  void OnBeginDragLeft(float x, float y, int keys=0, int attachment = 0);
  void OnEndDragLeft(float x, float y, int keys=0, int attachment = 0);
};

IMPLEMENT_DYNAMIC_CLASS(wxDivisionControlPoint, wxControlPoint)

/*
 * Composite object
 *
 */

IMPLEMENT_DYNAMIC_CLASS(wxCompositeShape, wxRectangleShape)

wxCompositeShape::wxCompositeShape(): wxRectangleShape(10.0, 10.0)
{
//  selectable = FALSE;
  m_oldX = m_xpos;
  m_oldY = m_ypos;
}

wxCompositeShape::~wxCompositeShape()
{
  wxNode *node = m_constraints.First();
  while (node)
  {
    OGLConstraint *constraint = (OGLConstraint *)node->Data();
    delete constraint;
    node = node->Next();
  }
  node = m_children.First();
  while (node)
  {
    wxShape *object = (wxShape *)node->Data();
    wxNode *next = node->Next();
    object->Unlink();
    delete object;
    node = next;
  }
}

void wxCompositeShape::OnDraw(wxDC& dc)
{
  float x1 = (float)(m_xpos - m_width/2.0);
  float y1 = (float)(m_ypos - m_height/2.0);

  if (m_shadowMode != SHADOW_NONE)
  {
    if (m_shadowBrush)
      dc.SetBrush(m_shadowBrush);
    dc.SetPen(transparent_pen);

    if (m_cornerRadius != 0.0)
      dc.DrawRoundedRectangle(x1 + m_shadowOffsetX, y1 + m_shadowOffsetY,
                               m_width, m_height, m_cornerRadius);
    else
      dc.DrawRectangle(x1 + m_shadowOffsetX, y1 + m_shadowOffsetY, m_width, m_height);
  }
}

void wxCompositeShape::OnDrawContents(wxDC& dc)
{
  wxNode *node = m_children.First();
  while (node)
  {
    wxShape *object = (wxShape *)node->Data();
    object->Draw(dc);
    object->DrawLinks(dc);
    node = node->Next();
  }
  wxShape::OnDrawContents(dc);
}

bool wxCompositeShape::OnMovePre(wxDC& dc, float x, float y, float oldx, float oldy, bool display)
{
  float diffX = x - oldx;
  float diffY = y - oldy;
  wxNode *node = m_children.First();
  while (node)
  {
    wxShape *object = (wxShape *)node->Data();

    object->Erase(dc);
    object->Move(dc, object->GetX() + diffX, object->GetY() + diffY, display);

    node = node->Next();
  }
  return TRUE;
}

void wxCompositeShape::OnErase(wxDC& dc)
{
  wxRectangleShape::OnErase(dc);
  wxNode *node = m_children.First();
  while (node)
  {
    wxShape *object = (wxShape *)node->Data();
    object->Erase(dc);
    node = node->Next();
  }
}

static float objectStartX = 0.0;
static float objectStartY = 0.0;

void wxCompositeShape::OnDragLeft(bool draw, float x, float y, int keys, int attachment)
{
  float xx = x;
  float yy = y;
  m_canvas->Snap(&xx, &yy);
  float offsetX = xx - objectStartX;
  float offsetY = yy - objectStartY;

  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  dc.SetLogicalFunction(wxXOR);
  wxPen dottedPen(wxColour(0, 0, 0), 1, wxDOT);
  dc.SetPen(dottedPen);
  dc.SetBrush((* wxTRANSPARENT_BRUSH));

  GetEventHandler()->OnDrawOutline(dc, GetX() + offsetX, GetY() + offsetY, GetWidth(), GetHeight());
//  wxShape::OnDragLeft(draw, x, y, keys, attachment);
}

void wxCompositeShape::OnBeginDragLeft(float x, float y, int keys, int attachment)
{
  objectStartX = x;
  objectStartY = y;

  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  Erase(dc);

  dc.SetLogicalFunction(wxXOR);

  wxPen dottedPen(wxColour(0, 0, 0), 1, wxDOT);
  dc.SetPen(dottedPen);
  dc.SetBrush((* wxTRANSPARENT_BRUSH));
  m_canvas->CaptureMouse();

  float xx = x;
  float yy = y;
  m_canvas->Snap(&xx, &yy);
  float offsetX = xx - objectStartX;
  float offsetY = yy - objectStartY;

  GetEventHandler()->OnDrawOutline(dc, GetX() + offsetX, GetY() + offsetY, GetWidth(), GetHeight());

//  wxShape::OnBeginDragLeft(x, y, keys, attachment);
}

void wxCompositeShape::OnEndDragLeft(float x, float y, int keys, int attachment)
{
//  wxShape::OnEndDragLeft(x, y, keys, attachment);

  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  m_canvas->ReleaseMouse();

  if (!m_draggable)
  {
    if (m_parent) m_parent->GetEventHandler()->OnEndDragLeft(x, y, keys, 0);
    return;
  }

  dc.SetLogicalFunction(wxCOPY);
  float xx = x;
  float yy = y;
  m_canvas->Snap(&xx, &yy);
  float offsetX = xx - objectStartX;
  float offsetY = yy - objectStartY;

  Move(dc, GetX() + offsetX, GetY() + offsetY);
  
  if (m_canvas && !m_canvas->GetQuickEditMode()) m_canvas->Redraw(dc);
}

void wxCompositeShape::OnRightClick(float x, float y, int keys, int attachment)
{
  // If we get a ctrl-right click, this means send the message to
  // the division, so we can invoke a user interface for dealing with regions.
  if (keys & KEY_CTRL)
  {
    wxNode *node = m_divisions.First();
    while (node)
    {
      wxDivisionShape *division = (wxDivisionShape *)node->Data();
      wxNode *next = node->Next();
      int attach = 0;
      float dist = 0.0;
      if (division->HitTest(x, y, &attach, &dist))
      {
        division->GetEventHandler()->OnRightClick(x, y, keys, attach);
        node = NULL;
      }
      if (node)
        node = next;
    }
  }
}

void wxCompositeShape::SetSize(float w, float h, bool recursive)
{
  SetAttachmentSize(w, h);

  float xScale = (float)(w/(wxMax(1.0, GetWidth())));
  float yScale = (float)(h/(wxMax(1.0, GetHeight())));

  m_width = w;
  m_height = h;

  if (!recursive) return;

  wxNode *node = m_children.First();

  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  float xBound, yBound;
  while (node)
  {
    wxShape *object = (wxShape *)node->Data();

    // Scale the position first
    float newX = (float)(((object->GetX() - GetX())*xScale) + GetX());
    float newY = (float)(((object->GetY() - GetY())*yScale) + GetY());
    object->Show(FALSE);
    object->Move(dc, newX, newY);
    object->Show(TRUE);
    
    // Now set the scaled size
    object->GetBoundingBoxMin(&xBound, &yBound);
    object->SetSize(object->GetFixedWidth() ? xBound : xScale*xBound,
                    object->GetFixedHeight() ? yBound : yScale*yBound);

    node = node->Next();
  }
  SetDefaultRegionSize();
}

void wxCompositeShape::AddChild(wxShape *child, wxShape *addAfter)
{
  m_children.Append(child);
  child->SetParent(this);
  if (m_canvas)
  {
    // Ensure we add at the right position
    if (addAfter)
      child->RemoveFromCanvas(m_canvas);
    child->AddToCanvas(m_canvas, addAfter);
  }
}

void wxCompositeShape::RemoveChild(wxShape *child)
{
  m_children.DeleteObject(child);
  m_divisions.DeleteObject(child);
  RemoveChildFromConstraints(child);
  child->SetParent(NULL);
}

void wxCompositeShape::DeleteConstraintsInvolvingChild(wxShape *child)
{
  wxNode *node = m_constraints.First();
  while (node)
  {
    OGLConstraint *constraint = (OGLConstraint *)node->Data();
    wxNode *nextNode = node->Next();

    if ((constraint->m_constrainingObject == child) ||
        constraint->m_constrainedObjects.Member(child))
    {
      delete constraint;
      delete node;
    }
    node = nextNode;
  }
}

void wxCompositeShape::RemoveChildFromConstraints(wxShape *child)
{
  wxNode *node = m_constraints.First();
  while (node)
  {
    OGLConstraint *constraint = (OGLConstraint *)node->Data();
    wxNode *nextNode = node->Next();

    if (constraint->m_constrainedObjects.Member(child))
      constraint->m_constrainedObjects.DeleteObject(child);
    if (constraint->m_constrainingObject == child)
      constraint->m_constrainingObject = NULL;

    // Delete the constraint if no participants left
    if (!constraint->m_constrainingObject)
    {
      delete constraint;
      delete node;
    }

    node = nextNode;
  }
}

void wxCompositeShape::Copy(wxShape& copy)
{
  wxRectangleShape::Copy(copy);

  wxASSERT( copy.IsKindOf(CLASSINFO(wxCompositeShape)) ) ;

  wxCompositeShape& compositeCopy = (wxCompositeShape&) copy;

  // Associate old and new copies for compositeCopying constraints and division geometry
  wxObjectCopyMapping.Append((long)this, &compositeCopy);

  // Copy the children
  wxNode *node = m_children.First();
  while (node)
  {
    wxShape *object = (wxShape *)node->Data();
    wxShape *newObject = object->CreateNewCopy(FALSE, FALSE);
    if (newObject->GetId() == 0)
      newObject->SetId(NewId());
      
    newObject->SetParent(&compositeCopy);
    compositeCopy.m_children.Append(newObject);

    // Some m_children may be divisions
    if (m_divisions.Member(object))
      compositeCopy.m_divisions.Append(newObject);

    wxObjectCopyMapping.Append((long)object, newObject);

    node = node->Next();
  }

  // Copy the constraints
  node = m_constraints.First();
  while (node)
  {
    OGLConstraint *constraint = (OGLConstraint *)node->Data();

    wxShape *newConstraining = (wxShape *)(wxObjectCopyMapping.Find((long)constraint->m_constrainingObject)->Data());

    wxList newConstrainedList;
    wxNode *node2 = constraint->m_constrainedObjects.First();
    while (node2)
    {
      wxShape *constrainedObject = (wxShape *)node2->Data();
      wxShape *newConstrained = (wxShape *)(wxObjectCopyMapping.Find((long)constrainedObject)->Data());
      newConstrainedList.Append(newConstrained);
      node2 = node2->Next(); 
    }

    OGLConstraint *newConstraint = new OGLConstraint(constraint->m_constraintType, newConstraining,
                                            newConstrainedList);
    newConstraint->m_constraintId = constraint->m_constraintId;
    if (constraint->m_constraintName)
    {
      newConstraint->m_constraintName = constraint->m_constraintName;
    }
    newConstraint->SetSpacing(constraint->m_xSpacing, constraint->m_ySpacing);
    compositeCopy.m_constraints.Append(newConstraint);

    node = node->Next();
  }

  // Now compositeCopy the division geometry
  node = m_divisions.First();
  while (node)
  {
    wxDivisionShape *division = (wxDivisionShape *)node->Data();
    wxNode *node1 = wxObjectCopyMapping.Find((long)division);
    wxNode *leftNode = NULL;
    wxNode *topNode = NULL;
    wxNode *rightNode = NULL;
    wxNode *bottomNode = NULL;
    if (division->GetLeftSide())
      leftNode = wxObjectCopyMapping.Find((long)division->GetLeftSide());
    if (division->GetTopSide())
      topNode = wxObjectCopyMapping.Find((long)division->GetTopSide());
    if (division->GetRightSide())
      rightNode = wxObjectCopyMapping.Find((long)division->GetRightSide());
    if (division->GetBottomSide())
      bottomNode = wxObjectCopyMapping.Find((long)division->GetBottomSide());
    if (node1)
    {
      wxDivisionShape *newDivision = (wxDivisionShape *)node1->Data();
      if (leftNode)
        newDivision->SetLeftSide((wxDivisionShape *)leftNode->Data());
      if (topNode)
        newDivision->SetTopSide((wxDivisionShape *)topNode->Data());
      if (rightNode)
        newDivision->SetRightSide((wxDivisionShape *)rightNode->Data());
      if (bottomNode)
        newDivision->SetBottomSide((wxDivisionShape *)bottomNode->Data());
    }
    node = node->Next();
  }
}

OGLConstraint *wxCompositeShape::AddConstraint(OGLConstraint *constraint)
{
  m_constraints.Append(constraint);
  if (constraint->m_constraintId == 0)
    constraint->m_constraintId = NewId();
  return constraint;
}

OGLConstraint *wxCompositeShape::AddConstraint(int type, wxShape *constraining, wxList& constrained)
{
  OGLConstraint *constraint = new OGLConstraint(type, constraining, constrained);
  if (constraint->m_constraintId == 0)
    constraint->m_constraintId = NewId();
  m_constraints.Append(constraint);
  return constraint;
}

OGLConstraint *wxCompositeShape::AddConstraint(int type, wxShape *constraining, wxShape *constrained)
{
  wxList l;
  l.Append(constrained);
  OGLConstraint *constraint = new OGLConstraint(type, constraining, l);
  if (constraint->m_constraintId == 0)
    constraint->m_constraintId = NewId();
  m_constraints.Append(constraint);
  return constraint;
}

OGLConstraint *wxCompositeShape::FindConstraint(long cId, wxCompositeShape **actualComposite)
{
  wxNode *node = m_constraints.First();
  while (node)
  {
    OGLConstraint *constraint = (OGLConstraint *)node->Data();
    if (constraint->m_constraintId == cId)
    {
      if (actualComposite)
        *actualComposite = this;
      return constraint;
    }
    node = node->Next();
  }
  // If not found, try children.
  node = m_children.First();
  while (node)
  {
    wxShape *child = (wxShape *)node->Data();
    if (child->IsKindOf(CLASSINFO(wxCompositeShape)))
    {
      OGLConstraint *constraint = ((wxCompositeShape *)child)->FindConstraint(cId, actualComposite);
      if (constraint)
      {
        if (actualComposite)
          *actualComposite = (wxCompositeShape *)child;
        return constraint;
      }
    }
    node = node->Next();
  }
  return NULL;
}

void wxCompositeShape::DeleteConstraint(OGLConstraint *constraint)
{
  m_constraints.DeleteObject(constraint);
  delete constraint;
}

void wxCompositeShape::CalculateSize()
{
  float maxX = (float) -999999.9;
  float maxY = (float) -999999.9;
  float minX = (float)  999999.9;
  float minY = (float)  999999.9;

  float w, h;
  wxNode *node = m_children.First();
  while (node)
  {
    wxShape *object = (wxShape *)node->Data();

    // Recalculate size of composite objects because may not conform
    // to size it was set to - depends on the children.
    object->CalculateSize();

    object->GetBoundingBoxMax(&w, &h);
    if ((object->GetX() + (w/2.0)) > maxX)
      maxX = (float)(object->GetX() + (w/2.0));
    if ((object->GetX() - (w/2.0)) < minX)
      minX = (float)(object->GetX() - (w/2.0));
    if ((object->GetY() + (h/2.0)) > maxY)
      maxY = (float)(object->GetY() + (h/2.0));
    if ((object->GetY() - (h/2.0)) < minY)
      minY = (float)(object->GetY() - (h/2.0));

    node = node->Next();
  }
  m_width = maxX - minX;
  m_height = maxY - minY;
  m_xpos = (float)(m_width/2.0 + minX);
  m_ypos = (float)(m_height/2.0 + minY);
}

bool wxCompositeShape::Recompute()
{
  int noIterations = 0;
  bool changed = TRUE;
  while (changed && (noIterations < 500))
  {
    changed = Constrain();
    noIterations ++;
  }
/*
#ifdef wx_x
  if (changed)
    cerr << "Warning: constraint algorithm failed after 500 iterations.\n";
#endif
*/
  return (!changed);
}

bool wxCompositeShape::Constrain()
{
  CalculateSize();

  bool changed = FALSE;
  wxNode *node = m_children.First();
  while (node)
  {
    wxShape *object = (wxShape *)node->Data();
    if (object->Constrain())
      changed = TRUE;
    node = node->Next();
  }

  node = m_constraints.First();
  while (node)
  {
    OGLConstraint *constraint = (OGLConstraint *)node->Data();
    if (constraint->Evaluate()) changed = TRUE;
    node = node->Next();
  }
  return changed;
}

#ifdef PROLOGIO
void wxCompositeShape::WritePrologAttributes(wxExpr *clause)
{
  wxRectangleShape::WritePrologAttributes(clause);

//  clause->AddAttributeValue("selectable", (long)selectable);

  // Output constraints as constraint1 = (...), constraint2 = (...), etc.
  int constraintNo = 1;
  char m_constraintNameBuf[20];
  wxNode *node = m_constraints.First();
  while (node)
  {
    OGLConstraint *constraint = (OGLConstraint *)node->Data();
    sprintf(m_constraintNameBuf, "constraint%d", constraintNo);

    // Each constraint is stored in the form
    // (type name id xspacing yspacing m_constrainingObjectId constrainedObjectIdList)
    wxExpr *constraintExpr = new wxExpr(PrologList);
    constraintExpr->Append(new wxExpr((long)constraint->m_constraintType));
    constraintExpr->Append(new wxExpr(PrologString, constraint->m_constraintName));
    constraintExpr->Append(new wxExpr(constraint->m_constraintId));
    constraintExpr->Append(new wxExpr(constraint->m_xSpacing));
    constraintExpr->Append(new wxExpr(constraint->m_ySpacing));
    constraintExpr->Append(new wxExpr(constraint->m_constrainingObject->GetId()));

    wxExpr *objectList = new wxExpr(PrologList);
    wxNode *node1 = constraint->m_constrainedObjects.First();
    while (node1)
    {
      wxShape *obj = (wxShape *)node1->Data();
      objectList->Append(new wxExpr(obj->GetId()));
      node1 = node1->Next();
    }
    constraintExpr->Append(objectList);

    clause->AddAttributeValue(m_constraintNameBuf, constraintExpr);

    node = node->Next();
    constraintNo ++;
  }

  // Write the ids of all the child images
  wxExpr *childrenExpr = new wxExpr(PrologList);
  node = m_children.First();
  while (node)
  {
    wxShape *child = (wxShape *)node->Data();
    childrenExpr->Append(new wxExpr(child->GetId()));
    node = node->Next();
  }
  clause->AddAttributeValue("children", childrenExpr);

  // Write the ids of all the division images
  if (m_divisions.Number() > 0)
  {
    wxExpr *divisionsExpr = new wxExpr(PrologList);
    node = m_divisions.First();
    while (node)
    {
      wxShape *child = (wxShape *)node->Data();
      divisionsExpr->Append(new wxExpr(child->GetId()));
      node = node->Next();
    }
    clause->AddAttributeValue("divisions", divisionsExpr);
  }
}

// Problem. Child images are always written AFTER the parent
// so as to be able to link up to parent. So we may not be able
// to find the constraint participants until we've read everything
// in. Need to have another pass for composites.
void wxCompositeShape::ReadPrologAttributes(wxExpr *clause)
{
  wxRectangleShape::ReadPrologAttributes(clause);

//  clause->GetAttributeValue("selectable", selectable);
}

void wxCompositeShape::ReadConstraints(wxExpr *clause, wxExprDatabase *database)
{
  // Constraints are output as constraint1 = (...), constraint2 = (...), etc.
  int constraintNo = 1;
  char m_constraintNameBuf[20];
  bool haveConstraints = TRUE;

  while (haveConstraints)
  {
    sprintf(m_constraintNameBuf, "constraint%d", constraintNo);
    wxExpr *constraintExpr = NULL;
    clause->GetAttributeValue(m_constraintNameBuf, &constraintExpr);
    if (!constraintExpr)
    {
      haveConstraints = FALSE;
      break;
    }
    int cType = 0;
    float cXSpacing = 0.0;
    float cYSpacing = 0.0;
    wxString cName("");
    long cId = 0;
    wxShape *m_constrainingObject = NULL;
    wxList m_constrainedObjects;

    // Each constraint is stored in the form
    // (type name id xspacing yspacing m_constrainingObjectId constrainedObjectIdList)

    wxExpr *typeExpr = constraintExpr->Nth(0);
    wxExpr *nameExpr = constraintExpr->Nth(1);
    wxExpr *idExpr = constraintExpr->Nth(2);
    wxExpr *xExpr = constraintExpr->Nth(3);
    wxExpr *yExpr = constraintExpr->Nth(4);
    wxExpr *constrainingExpr = constraintExpr->Nth(5);
    wxExpr *constrainedExpr = constraintExpr->Nth(6);

    cType = (int)typeExpr->IntegerValue();
    cXSpacing = xExpr->RealValue();
    cYSpacing = yExpr->RealValue();
    cName = nameExpr->StringValue();
    cId = idExpr->IntegerValue();

    wxExpr *objExpr1 = database->HashFind("node_image", constrainingExpr->IntegerValue());
    if (objExpr1 && objExpr1->GetClientData())
      m_constrainingObject = (wxShape *)objExpr1->GetClientData();
    else
      wxFatalError("Couldn't find constraining image of composite.", "Object graphics error");
 
    int i = 0;
    wxExpr *currentIdExpr = constrainedExpr->Nth(i);
    while (currentIdExpr)
    {
      long currentId = currentIdExpr->IntegerValue();
      wxExpr *objExpr2 = database->HashFind("node_image", currentId);
      if (objExpr2 && objExpr2->GetClientData())
      {
        m_constrainedObjects.Append((wxShape *)objExpr2->GetClientData());
      }
      else
      {
        wxFatalError("Couldn't find constrained image of composite.", "Object graphics error");
      }

      i ++;
      currentIdExpr = constrainedExpr->Nth(i);
    }
    OGLConstraint *newConstraint = AddConstraint(cType, m_constrainingObject, m_constrainedObjects);
    newConstraint->SetSpacing(cXSpacing, cYSpacing);
    newConstraint->m_constraintId = cId;
    newConstraint->m_constraintName = (const char*) cName;
    constraintNo ++;
  }
}
#endif

// Make this composite into a container by creating one wxDivisionShape
void wxCompositeShape::MakeContainer()
{
  wxDivisionShape *division = OnCreateDivision();
  m_divisions.Append(division);
  AddChild(division);

  division->SetSize(m_width, m_height);

  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  division->Move(dc, GetX(), GetY());
  Recompute();
  division->Show(TRUE);
}

wxDivisionShape *wxCompositeShape::OnCreateDivision()
{
  return new wxDivisionShape;
}

wxShape *wxCompositeShape::FindContainerImage()
{
  wxNode *node = m_children.First();
  while (node)
  {
    wxShape *child = (wxShape *)node->Data();
    if (!m_divisions.Member(child))
      return child;
    node = node->Next();
  }
  return NULL;
}

// Returns TRUE if division is a descendant of this container
bool wxCompositeShape::ContainsDivision(wxDivisionShape *division)
{
  if (m_divisions.Member(division))
    return TRUE;
  wxNode *node = m_children.First();
  while (node)
  {
    wxShape *child = (wxShape *)node->Data();
    if (child->IsKindOf(CLASSINFO(wxCompositeShape)))
    {
      bool ans = ((wxCompositeShape *)child)->ContainsDivision(division);
      if (ans)
        return TRUE;
    }
    node = node->Next();
  }
  return FALSE;
}

/*
 * Division object
 *
 */

IMPLEMENT_DYNAMIC_CLASS(wxDivisionShape, wxCompositeShape)

wxDivisionShape::wxDivisionShape()
{
  SetSensitivityFilter(OP_CLICK_LEFT | OP_CLICK_RIGHT | OP_DRAG_RIGHT);
  SetCentreResize(FALSE);
  SetAttachmentMode(TRUE);
  m_leftSide = NULL;
  m_rightSide = NULL;
  m_topSide = NULL;
  m_bottomSide = NULL;
  m_handleSide = DIVISION_SIDE_NONE;
  m_leftSidePen = wxBLACK_PEN;
  m_topSidePen = wxBLACK_PEN;
  m_leftSideColour = "BLACK";
  m_topSideColour = "BLACK";
  m_leftSideStyle = "Solid";
  m_topSideStyle = "Solid";
  ClearRegions();
}

wxDivisionShape::~wxDivisionShape()
{
}

void wxDivisionShape::OnDraw(wxDC& dc)
{
    dc.SetBrush(wxTRANSPARENT_BRUSH);
    dc.SetBackgroundMode(wxTRANSPARENT);

    float x1 = (float)(GetX() - (GetWidth()/2.0));
    float y1 = (float)(GetY() - (GetHeight()/2.0));
    float x2 = (float)(GetX() + (GetWidth()/2.0));
    float y2 = (float)(GetY() + (GetHeight()/2.0));

    // Should subtract 1 pixel if drawing under Windows
#ifdef __WXMSW__
    y2 -= (float)1.0;
#endif

    if (m_leftSide)
    {
      dc.SetPen(m_leftSidePen);
      dc.DrawLine(x1, y2, x1, y1);
    }
    if (m_topSide)
    {
      dc.SetPen(m_topSidePen);
      dc.DrawLine(x1, y1, x2, y1);
    }

    // For testing purposes, draw a rectangle so we know
    // how big the division is.
//    SetBrush(wxCYAN_BRUSH);
//    wxRectangleShape::OnDraw(dc);
}

void wxDivisionShape::OnDrawContents(wxDC& dc)
{
  wxCompositeShape::OnDrawContents(dc);
}

bool wxDivisionShape::OnMovePre(wxDC& dc, float x, float y, float oldx, float oldy, bool display)
{
  float diffX = x - oldx;
  float diffY = y - oldy;
  wxNode *node = m_children.First();
  while (node)
  {
    wxShape *object = (wxShape *)node->Data();
    object->Erase(dc);
    object->Move(dc, object->GetX() + diffX, object->GetY() + diffY, display);
    node = node->Next();
  }
  return TRUE;
}

void wxDivisionShape::OnDragLeft(bool draw, float x, float y, int keys, int attachment)
{
  if ((m_sensitivity & OP_DRAG_LEFT) != OP_DRAG_LEFT)
  {
    attachment = 0;
    float dist;
    if (m_parent)
    {
      m_parent->HitTest(x, y, &attachment, &dist);
      m_parent->GetEventHandler()->OnDragLeft(draw, x, y, keys, attachment);
    }
    return;
  }
  wxShape::OnDragLeft(draw, x, y, keys, attachment);
}

void wxDivisionShape::OnBeginDragLeft(float x, float y, int keys, int attachment)
{
  if ((m_sensitivity & OP_DRAG_LEFT) != OP_DRAG_LEFT)
  {
    attachment = 0;
    float dist;
    if (m_parent)
    {
      m_parent->HitTest(x, y, &attachment, &dist);
      m_parent->GetEventHandler()->OnBeginDragLeft(x, y, keys, attachment);
    }
    return;
  }

  wxShape::OnBeginDragLeft(x, y, keys, attachment);
}

void wxDivisionShape::OnEndDragLeft(float x, float y, int keys, int attachment)
{
  m_canvas->ReleaseMouse();
  if ((m_sensitivity & OP_DRAG_LEFT) != OP_DRAG_LEFT)
  {
    attachment = 0;
    float dist;
    if (m_parent)
    {
      m_parent->HitTest(x, y, &attachment, &dist);
      m_parent->GetEventHandler()->OnEndDragLeft(x, y, keys, attachment);
    }
    return;
  }
    
  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  dc.SetLogicalFunction(wxCOPY);

  m_canvas->Snap(&m_xpos, &m_ypos);
  GetEventHandler()->OnMovePre(dc, x, y, m_oldX, m_oldY);

  ResetControlPoints();
  Draw(dc);
  MoveLinks(dc);
  GetEventHandler()->OnDrawControlPoints(dc);

  if (m_canvas && !m_canvas->GetQuickEditMode()) m_canvas->Redraw(dc);
}

void wxDivisionShape::SetSize(float w, float h, bool recursive)
{
  m_width = w;
  m_height = h;
  wxRectangleShape::SetSize(w, h, recursive);
}

void wxDivisionShape::CalculateSize()
{
}

void wxDivisionShape::Copy(wxShape& copy)
{
  wxCompositeShape::Copy(copy);

  wxASSERT( copy.IsKindOf(CLASSINFO(wxDivisionShape)) ) ;

  wxDivisionShape& divisionCopy = (wxDivisionShape&) copy;

  divisionCopy.m_leftSideStyle = m_leftSideStyle;
  divisionCopy.m_topSideStyle = m_topSideStyle;
  divisionCopy.m_leftSideColour = m_leftSideColour;
  divisionCopy.m_topSideColour = m_topSideColour;

  divisionCopy.m_leftSidePen = m_leftSidePen;
  divisionCopy.m_topSidePen = m_topSidePen;
  divisionCopy.m_handleSide = m_handleSide;

  // Division geometry copying is handled at the wxCompositeShape level.
}

#ifdef PROLOGIO
void wxDivisionShape::WritePrologAttributes(wxExpr *clause)
{
  wxCompositeShape::WritePrologAttributes(clause);

  if (m_leftSide)
    clause->AddAttributeValue("left_side", (long)m_leftSide->GetId());
  if (m_topSide)
    clause->AddAttributeValue("top_side", (long)m_topSide->GetId());
  if (m_rightSide)
    clause->AddAttributeValue("right_side", (long)m_rightSide->GetId());
  if (m_bottomSide)
    clause->AddAttributeValue("bottom_side", (long)m_bottomSide->GetId());

  clause->AddAttributeValue("handle_side", (long)m_handleSide);
  clause->AddAttributeValueString("left_colour", m_leftSideColour);
  clause->AddAttributeValueString("top_colour", m_topSideColour);
  clause->AddAttributeValueString("left_style", m_leftSideStyle);
  clause->AddAttributeValueString("top_style", m_topSideStyle);
}

void wxDivisionShape::ReadPrologAttributes(wxExpr *clause)
{
  wxCompositeShape::ReadPrologAttributes(clause);

  clause->GetAttributeValue("handle_side", m_handleSide);
  clause->GetAttributeValue("left_colour", m_leftSideColour);
  clause->GetAttributeValue("top_colour", m_topSideColour);
  clause->GetAttributeValue("left_style", m_leftSideStyle);
  clause->GetAttributeValue("top_style", m_topSideStyle);
}
#endif

// Experimental
void wxDivisionShape::OnRightClick(float x, float y, int keys, int attachment)
{
  if (keys & KEY_CTRL)
  {
    PopupMenu(x, y);
  }
/*
  else if (keys & KEY_SHIFT)
  {
    if (m_leftSide || m_topSide || m_rightSide || m_bottomSide)
    {
      if (Selected())
      {
        Select(FALSE);
        GetParent()->Draw(dc);
      }
      else
        Select(TRUE);
    }
  }
*/
  else
  {
    attachment = 0;
    float dist;
    if (m_parent)
    {
      m_parent->HitTest(x, y, &attachment, &dist);
      m_parent->GetEventHandler()->OnRightClick(x, y, keys, attachment);
    }
    return;
  }
}


// Divide wxHORIZONTALly or wxVERTICALly
bool wxDivisionShape::Divide(int direction)
{
  // Calculate existing top-left, bottom-right
  float x1 = (float)(GetX() - (GetWidth()/2.0));
  float y1 = (float)(GetY() - (GetHeight()/2.0));
  wxCompositeShape *compositeParent = (wxCompositeShape *)GetParent();
  float oldWidth = GetWidth();
  float oldHeight = GetHeight();
  if (Selected())
    Select(FALSE);

  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  if (direction == wxVERTICAL)
  {
    // Dividing vertically means notionally putting a horizontal line through it.
    // Break existing piece into two.
    float newXPos1 = GetX();
    float newYPos1 = (float)(y1 + (GetHeight()/4.0));
    float newXPos2 = GetX();
    float newYPos2 = (float)(y1 + (3.0*GetHeight()/4.0));
    wxDivisionShape *newDivision = compositeParent->OnCreateDivision();
    newDivision->Show(TRUE);
    
    Erase(dc);

    // Anything adjoining the bottom of this division now adjoins the
    // bottom of the new division.
    wxNode *node = compositeParent->GetDivisions().First();
    while (node)
    {
      wxDivisionShape *obj = (wxDivisionShape *)node->Data();
      if (obj->GetTopSide() == this)
        obj->SetTopSide(newDivision);
      node = node->Next();
    }
    newDivision->SetTopSide(this);
    newDivision->SetBottomSide(m_bottomSide);
    newDivision->SetLeftSide(m_leftSide);
    newDivision->SetRightSide(m_rightSide);
    m_bottomSide = newDivision;

    compositeParent->GetDivisions().Append(newDivision);

    // CHANGE: Need to insert this division at start of divisions in the object
    // list, because e.g.:
    // 1) Add division
    // 2) Add contained object
    // 3) Add division
    // Division is now receiving mouse events _before_ the contained object,
    // because it was added last (on top of all others)

    // Add after the image that visualizes the container    
    compositeParent->AddChild(newDivision, compositeParent->FindContainerImage());

    m_handleSide = DIVISION_SIDE_BOTTOM;
    newDivision->SetHandleSide(DIVISION_SIDE_TOP);

    SetSize(oldWidth, (float)(oldHeight/2.0));
    Move(dc, newXPos1, newYPos1);

    newDivision->SetSize(oldWidth, (float)(oldHeight/2.0));
    newDivision->Move(dc, newXPos2, newYPos2);
  }
  else
  {
    // Dividing horizontally means notionally putting a vertical line through it.
    // Break existing piece into two.
    float newXPos1 = (float)(x1 + (GetWidth()/4.0));
    float newYPos1 = GetY();
    float newXPos2 = (float)(x1 + (3.0*GetWidth()/4.0));
    float newYPos2 = GetY();
    wxDivisionShape *newDivision = compositeParent->OnCreateDivision();
    newDivision->Show(TRUE);
    
    Erase(dc);

    // Anything adjoining the left of this division now adjoins the
    // left of the new division.
    wxNode *node = compositeParent->GetDivisions().First();
    while (node)
    {
      wxDivisionShape *obj = (wxDivisionShape *)node->Data();
      if (obj->GetLeftSide() == this)
        obj->SetLeftSide(newDivision);
      node = node->Next();
    }
    newDivision->SetTopSide(m_topSide);
    newDivision->SetBottomSide(m_bottomSide);
    newDivision->SetLeftSide(this);
    newDivision->SetRightSide(m_rightSide);
    m_rightSide = newDivision;

    compositeParent->GetDivisions().Append(newDivision);
    compositeParent->AddChild(newDivision, compositeParent->FindContainerImage());

    m_handleSide = DIVISION_SIDE_RIGHT;
    newDivision->SetHandleSide(DIVISION_SIDE_LEFT);

    SetSize((float)(oldWidth/2.0), oldHeight);
    Move(dc, newXPos1, newYPos1);

    newDivision->SetSize((float)(oldWidth/2.0), oldHeight);
    newDivision->Move(dc, newXPos2, newYPos2);
  }
  if (compositeParent->Selected())
  {
    compositeParent->DeleteControlPoints(& dc);
    compositeParent->MakeControlPoints();
    compositeParent->MakeMandatoryControlPoints();
  }
  compositeParent->Draw(dc);
  return TRUE;
}

// Make one control point for every visible line
void wxDivisionShape::MakeControlPoints()
{
  MakeMandatoryControlPoints();
}

void wxDivisionShape::MakeMandatoryControlPoints()
{
  float maxX, maxY;

  GetBoundingBoxMax(&maxX, &maxY);
  float x, y;
  int direction;
/*
  if (m_leftSide)
  {
    x = (float)(-maxX/2.0);
    y = 0.0;
    wxDivisionControlPoint *control = new wxDivisionControlPoint(m_canvas, this, CONTROL_POINT_SIZE, x, y,
                                             CONTROL_POINT_HORIZONTAL);
    m_canvas->AddShape(control);
    m_controlPoints.Append(control);
  }
  if (m_topSide)
  {
    x = 0.0;
    y = (float)(-maxY/2.0);
    wxDivisionControlPoint *control = new wxDivisionControlPoint(m_canvas, this, CONTROL_POINT_SIZE, x, y,
                                             CONTROL_POINT_VERTICAL);
    m_canvas->AddShape(control);
    m_controlPoints.Append(control);
  }
*/
  switch (m_handleSide)
  {
    case DIVISION_SIDE_LEFT:
    {
      x = (float)(-maxX/2.0);
      y = 0.0;
      direction = CONTROL_POINT_HORIZONTAL;
      break;
    }
    case DIVISION_SIDE_TOP:
    {
      x = 0.0;
      y = (float)(-maxY/2.0);
      direction = CONTROL_POINT_VERTICAL;
      break;
    }
    case DIVISION_SIDE_RIGHT:
    {
      x = (float)(maxX/2.0);
      y = 0.0;
      direction = CONTROL_POINT_HORIZONTAL;
      break;
    }
    case DIVISION_SIDE_BOTTOM:
    {
      x = 0.0;
      y = (float)(maxY/2.0);
      direction = CONTROL_POINT_VERTICAL;
      break;
    }
    default:
      break;
  }
  if (m_handleSide != DIVISION_SIDE_NONE)
  {
    wxDivisionControlPoint *control = new wxDivisionControlPoint(m_canvas, this, CONTROL_POINT_SIZE, x, y,
                                             direction);
    m_canvas->AddShape(control);
    m_controlPoints.Append(control);
  }
}

void wxDivisionShape::ResetControlPoints()
{
  ResetMandatoryControlPoints();
}

void wxDivisionShape::ResetMandatoryControlPoints()
{
  if (m_controlPoints.Number() < 1)
    return;

  float maxX, maxY;

  GetBoundingBoxMax(&maxX, &maxY);
/*
  wxNode *node = m_controlPoints.First();
  while (node)
  {
    wxDivisionControlPoint *control = (wxDivisionControlPoint *)node->Data();
    if (control->type == CONTROL_POINT_HORIZONTAL)
    {
      control->xoffset = (float)(-maxX/2.0); control->m_yoffset = 0.0;
    }
    else if (control->type == CONTROL_POINT_VERTICAL)
    {
      control->xoffset = 0.0; control->m_yoffset = (float)(-maxY/2.0);
    }
    node = node->Next();
  }
*/
  wxNode *node = m_controlPoints.First();
  if ((m_handleSide == DIVISION_SIDE_LEFT) && node)
  {
    wxDivisionControlPoint *control = (wxDivisionControlPoint *)node->Data();
    control->m_xoffset = (float)(-maxX/2.0); control->m_yoffset = 0.0;
  }

  if ((m_handleSide == DIVISION_SIDE_TOP) && node)
  {
    wxDivisionControlPoint *control = (wxDivisionControlPoint *)node->Data();
    control->m_xoffset = 0.0; control->m_yoffset = (float)(-maxY/2.0);
  }

  if ((m_handleSide == DIVISION_SIDE_RIGHT) && node)
  {
    wxDivisionControlPoint *control = (wxDivisionControlPoint *)node->Data();
    control->m_xoffset = (float)(maxX/2.0); control->m_yoffset = 0.0;
  }

  if ((m_handleSide == DIVISION_SIDE_BOTTOM) && node)
  {
    wxDivisionControlPoint *control = (wxDivisionControlPoint *)node->Data();
    control->m_xoffset = 0.0; control->m_yoffset = (float)(maxY/2.0);
  }
}

// Adjust a side, returning FALSE if it's not physically possible.
bool wxDivisionShape::AdjustLeft(float left, bool test)
{
  float x2 = (float)(GetX() + (GetWidth()/2.0));

  if (left >= x2)
    return FALSE;
  if (test)
    return TRUE;

  float newW = x2 - left;
  float newX = (float)(left + newW/2.0);
  SetSize(newW, GetHeight());

  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  Move(dc, newX, GetY());
    
  return TRUE;
}

bool wxDivisionShape::AdjustTop(float top, bool test)
{
  float y2 = (float)(GetY() + (GetHeight()/2.0));

  if (top >= y2)
    return FALSE;
  if (test)
    return TRUE;

  float newH = y2 - top;
  float newY = (float)(top + newH/2.0);
  SetSize(GetWidth(), newH);

  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  Move(dc, GetX(), newY);
    
  return TRUE;
}

bool wxDivisionShape::AdjustRight(float right, bool test)
{
  float x1 = (float)(GetX() - (GetWidth()/2.0));

  if (right <= x1)
    return FALSE;
  if (test)
    return TRUE;

  float newW = right - x1;
  float newX = (float)(x1 + newW/2.0);
  SetSize(newW, GetHeight());

  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  Move(dc, newX, GetY());
    
  return TRUE;
}

bool wxDivisionShape::AdjustBottom(float bottom, bool test)
{
  float y1 = (float)(GetY() - (GetHeight()/2.0));

  if (bottom <= y1)
    return FALSE;
  if (test)
    return TRUE;

  float newH = bottom - y1;
  float newY = (float)(y1 + newH/2.0);
  SetSize(GetWidth(), newH);

  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  Move(dc, GetX(), newY);
    
  return TRUE;
}

wxDivisionControlPoint::wxDivisionControlPoint(wxShapeCanvas *the_canvas, wxShape *object, float size, float the_xoffset, float the_yoffset, int the_type):
  wxControlPoint(the_canvas, object, size, the_xoffset, the_yoffset, the_type)
{
  SetEraseObject(FALSE);
}

wxDivisionControlPoint::~wxDivisionControlPoint()
{
}

static float originalX = 0.0;
static float originalY = 0.0;
static float originalW = 0.0;
static float originalH = 0.0;

// Implement resizing of canvas object
void wxDivisionControlPoint::OnDragLeft(bool draw, float x, float y, int keys, int attachment)
{
  wxControlPoint::OnDragLeft(draw, x, y, keys, attachment);
}

void wxDivisionControlPoint::OnBeginDragLeft(float x, float y, int keys, int attachment)
{
  wxDivisionShape *division = (wxDivisionShape *)m_shape;
  originalX = division->GetX();
  originalY = division->GetY();
  originalW = division->GetWidth();
  originalH = division->GetHeight();
  
  wxControlPoint::OnBeginDragLeft(x, y, keys, attachment);
}

void wxDivisionControlPoint::OnEndDragLeft(float x, float y, int keys, int attachment)
{
  wxControlPoint::OnEndDragLeft(x, y, keys, attachment);

  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  wxDivisionShape *division = (wxDivisionShape *)m_shape;
  wxCompositeShape *divisionParent = (wxCompositeShape *)division->GetParent();

  // Need to check it's within the bounds of the parent composite.
  float x1 = (float)(divisionParent->GetX() - (divisionParent->GetWidth()/2.0));
  float y1 = (float)(divisionParent->GetY() - (divisionParent->GetHeight()/2.0));
  float x2 = (float)(divisionParent->GetX() + (divisionParent->GetWidth()/2.0));
  float y2 = (float)(divisionParent->GetY() + (divisionParent->GetHeight()/2.0));

  // Need to check it has not made the division zero or negative width/height
  float dx1 = (float)(division->GetX() - (division->GetWidth()/2.0));
  float dy1 = (float)(division->GetY() - (division->GetHeight()/2.0));
  float dx2 = (float)(division->GetX() + (division->GetWidth()/2.0));
  float dy2 = (float)(division->GetY() + (division->GetHeight()/2.0));

  bool success = TRUE;
  switch (division->GetHandleSide())
  {
    case DIVISION_SIDE_LEFT:
    {
      if ((x <= x1) || (x >= x2) || (x >= dx2))
        success = FALSE;
      // Try it out first...
      else if (!division->ResizeAdjoining(DIVISION_SIDE_LEFT, x, TRUE))
        success = FALSE;
      else
        division->ResizeAdjoining(DIVISION_SIDE_LEFT, x, FALSE);
      
      break;
    }
    case DIVISION_SIDE_TOP:
    {
      if ((y <= y1) || (y >= y2) || (y >= dy2))
        success = FALSE;
      else if (!division->ResizeAdjoining(DIVISION_SIDE_TOP, y, TRUE))
        success = FALSE;
      else
        division->ResizeAdjoining(DIVISION_SIDE_TOP, y, FALSE);
      
      break;
    }
    case DIVISION_SIDE_RIGHT:
    {
      if ((x <= x1) || (x >= x2) || (x <= dx1))
        success = FALSE;
      else if (!division->ResizeAdjoining(DIVISION_SIDE_RIGHT, x, TRUE))
        success = FALSE;
      else
        division->ResizeAdjoining(DIVISION_SIDE_RIGHT, x, FALSE);
      
      break;
    }
    case DIVISION_SIDE_BOTTOM:
    {
      if ((y <= y1) || (y >= y2) || (y <= dy1))
        success = FALSE;
      else if (!division->ResizeAdjoining(DIVISION_SIDE_BOTTOM, y, TRUE))
        success = FALSE;
      else
        division->ResizeAdjoining(DIVISION_SIDE_BOTTOM, y, FALSE);
      
      break;
    }
  }
  if (!success)
  {
    division->SetSize(originalW, originalH);
    division->Move(dc, originalX, originalY);
  }
  divisionParent->Draw(dc);
  division->GetEventHandler()->OnDrawControlPoints(dc);
}

/* Resize adjoining divisions.
 *
   Behaviour should be as follows:
   If right edge moves, find all objects whose left edge
   adjoins this object, and move left edge accordingly.
   If left..., move ... right.
   If top..., move ... bottom.
   If bottom..., move top.
   If size goes to zero or end position is other side of start position,
   resize to original size and return.
 */
bool wxDivisionShape::ResizeAdjoining(int side, float newPos, bool test)
{
  wxCompositeShape *divisionParent = (wxCompositeShape *)GetParent();
  wxNode *node = divisionParent->GetDivisions().First();
  while (node)
  {
    wxDivisionShape *division = (wxDivisionShape *)node->Data();
    switch (side)
    {
      case DIVISION_SIDE_LEFT:
      {
        if (division->m_rightSide == this)
        {
          bool success = division->AdjustRight(newPos, test);
          if (!success && test)
            return FALSE;
        }
        break;
      }
      case DIVISION_SIDE_TOP:
      {
        if (division->m_bottomSide == this)
        {
          bool success = division->AdjustBottom(newPos, test);
          if (!success && test)
            return FALSE;
        }
        break;
      }
      case DIVISION_SIDE_RIGHT:
      {
        if (division->m_leftSide == this)
        {
          bool success = division->AdjustLeft(newPos, test);
          if (!success && test)
            return FALSE;
        }
        break;
      }
      case DIVISION_SIDE_BOTTOM:
      {
        if (division->m_topSide == this)
        {
          bool success = division->AdjustTop(newPos, test);
          if (!success && test)
            return FALSE;
        }
        break;
      }
      default:
        break;
    }
    node = node->Next();
  }
  
  return TRUE;
}

/*
 * Popup menu for editing divisions
 *
 */
 
void oglGraphicsDivisionMenuProc(wxMenu& menu, wxCommandEvent& event)
{
  wxDivisionShape *division = (wxDivisionShape *)menu.GetClientData();
  switch (event.GetInt())
  {
    case DIVISION_MENU_SPLIT_HORIZONTALLY:
    {
      division->Divide(wxHORIZONTAL);
      break;
    }
    case DIVISION_MENU_SPLIT_VERTICALLY:
    {
      division->Divide(wxVERTICAL);
      break;
    }
    case DIVISION_MENU_EDIT_LEFT_EDGE:
    {
      division->EditEdge(DIVISION_SIDE_LEFT);
      break;
    }
    case DIVISION_MENU_EDIT_TOP_EDGE:
    {
      division->EditEdge(DIVISION_SIDE_TOP);
      break;
    }
    default:
      break;
  }
}

void wxDivisionShape::EditEdge(int side)
{
  wxMessageBox("EditEdge() not implemented", "OGL", wxOK);

#if 0
  wxBeginBusyCursor();

  wxPen *currentPen = NULL;
  char **pColour = NULL;
  char **pStyle = NULL;
  if (side == DIVISION_SIDE_LEFT)
  {
    currentPen = m_leftSidePen;
    pColour = &m_leftSideColour;
    pStyle = &m_leftSideStyle;
  }
  else
  {
    currentPen = m_topSidePen;
    pColour = &m_topSideColour;
    pStyle = &m_topSideStyle;
  }

  GraphicsForm *form = new GraphicsForm("Containers");
  int lineWidth = currentPen->GetWidth();
  
  form->Add(wxMakeFormShort("Width", &lineWidth, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               150));
  form->Add(wxMakeFormString("Colour", pColour, wxFORM_CHOICE,
            new wxList(wxMakeConstraintStrings(
  "BLACK"            ,
  "BLUE"             ,
  "BROWN"            ,
  "CORAL"            ,
  "CYAN"             ,
  "DARK GREY"        ,
  "DARK GREEN"       ,
  "DIM GREY"         ,
  "GREY"             ,
  "GREEN"            ,
  "LIGHT BLUE"       ,
  "LIGHT GREY"       ,
  "MAGENTA"          ,
  "MAROON"           ,
  "NAVY"             ,
  "ORANGE"           ,
  "PURPLE"           ,
  "RED"              ,
  "TURQUOISE"        ,
  "VIOLET"           ,
  "WHITE"            ,
  "YELLOW"           ,
  NULL),
  NULL), NULL, wxVERTICAL, 150));
  form->Add(wxMakeFormString("Style", pStyle, wxFORM_CHOICE,
            new wxList(wxMakeConstraintStrings(
  "Solid"            ,
  "Short Dash"       ,
  "Long Dash"        ,
  "Dot"              ,
  "Dot Dash"         ,
  NULL),
  NULL), NULL, wxVERTICAL, 100));

  wxDialogBox *dialog = new wxDialogBox(m_canvas->GetParent(), "Division properties", 10, 10, 500, 500);
  if (GraphicsLabelFont)
    dialog->SetLabelFont(GraphicsLabelFont);
  if (GraphicsButtonFont)
    dialog->SetButtonFont(GraphicsButtonFont);

  form->AssociatePanel(dialog);
  form->dialog = dialog;

  dialog->Fit();
  dialog->Centre(wxBOTH);

  wxEndBusyCursor();
  dialog->Show(TRUE);

  int lineStyle = wxSOLID;
  if (*pStyle)
  {
    if (strcmp(*pStyle, "Solid") == 0)
      lineStyle = wxSOLID;
    else if (strcmp(*pStyle, "Dot") == 0)
      lineStyle = wxDOT;
    else if (strcmp(*pStyle, "Short Dash") == 0)
      lineStyle = wxSHORT_DASH;
    else if (strcmp(*pStyle, "Long Dash") == 0)
      lineStyle = wxLONG_DASH;
    else if (strcmp(*pStyle, "Dot Dash") == 0)
      lineStyle = wxDOT_DASH;
  }
  
  wxPen *newPen = wxThePenList->FindOrCreatePen(*pColour, lineWidth, lineStyle);
  if (!pen)
    pen = wxBLACK_PEN;
  if (side == DIVISION_SIDE_LEFT)
    m_leftSidePen = newPen;
  else
    m_topSidePen = newPen;

  // Need to draw whole image again
  wxCompositeShape *compositeParent = (wxCompositeShape *)GetParent();
  compositeParent->Draw(dc);
#endif
}

// Popup menu
void wxDivisionShape::PopupMenu(float x, float y)
{
  oglPopupDivisionMenu->SetClientData((char *)this);
  if (m_leftSide)
    oglPopupDivisionMenu->Enable(DIVISION_MENU_EDIT_LEFT_EDGE, TRUE);
  else
    oglPopupDivisionMenu->Enable(DIVISION_MENU_EDIT_LEFT_EDGE, FALSE);
  if (m_topSide)
    oglPopupDivisionMenu->Enable(DIVISION_MENU_EDIT_TOP_EDGE, TRUE);
  else
    oglPopupDivisionMenu->Enable(DIVISION_MENU_EDIT_TOP_EDGE, FALSE);
      
  int x1, y1;
  m_canvas->ViewStart(&x1, &y1);

  int unit_x, unit_y;
  m_canvas->GetScrollPixelsPerUnit(&unit_x, &unit_y);

  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  int mouse_x = (int)(dc.LogicalToDeviceX(x - x1*unit_x));
  int mouse_y = (int)(dc.LogicalToDeviceY(y - y1*unit_y));

  m_canvas->PopupMenu(oglPopupDivisionMenu, mouse_x, mouse_y);
}

void wxDivisionShape::SetLeftSideColour(const wxString& colour)
{
  m_leftSideColour = colour;
}

void wxDivisionShape::SetTopSideColour(const wxString& colour)
{
  m_topSideColour = colour;
}

void wxDivisionShape::SetLeftSideStyle(const wxString& style)
{
  m_leftSideStyle = style;
}

void wxDivisionShape::SetTopSideStyle(const wxString& style)
{
  m_topSideStyle = style;
}

