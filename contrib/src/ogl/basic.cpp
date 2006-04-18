/////////////////////////////////////////////////////////////////////////////
// Name:        basic.cpp
// Purpose:     Basic OGL classes
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if wxUSE_PROLOGIO
#include "wx/deprecated/wxexpr.h"
#endif

#ifdef new
#undef new
#endif

#include <stdio.h>
#include <ctype.h>

#include "wx/ogl/ogl.h"


// Control point types
// Rectangle and most other shapes
#define CONTROL_POINT_VERTICAL   1
#define CONTROL_POINT_HORIZONTAL 2
#define CONTROL_POINT_DIAGONAL   3

// Line
#define CONTROL_POINT_ENDPOINT_TO 4
#define CONTROL_POINT_ENDPOINT_FROM 5
#define CONTROL_POINT_LINE       6

IMPLEMENT_DYNAMIC_CLASS(wxShapeTextLine, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxAttachmentPoint, wxObject)

wxShapeTextLine::wxShapeTextLine(double the_x, double the_y, const wxString& the_line)
{
  m_x = the_x; m_y = the_y; m_line = the_line;
}

wxShapeTextLine::~wxShapeTextLine()
{
}

IMPLEMENT_ABSTRACT_CLASS(wxShapeEvtHandler, wxObject)

wxShapeEvtHandler::wxShapeEvtHandler(wxShapeEvtHandler *prev, wxShape *shape)
{
  m_previousHandler = prev;
  m_handlerShape = shape;
}

wxShapeEvtHandler::~wxShapeEvtHandler()
{
}

// Creates a copy of this event handler.
wxShapeEvtHandler* wxShapeEvtHandler::CreateNewCopy()
{
  wxShapeEvtHandler* newObject = (wxShapeEvtHandler*) GetClassInfo()->CreateObject();

  wxASSERT( (newObject != NULL) );
  wxASSERT( (newObject->IsKindOf(CLASSINFO(wxShapeEvtHandler))) );

  newObject->m_previousHandler = newObject;

  CopyData(*newObject);

  return newObject;
}


void wxShapeEvtHandler::OnDelete()
{
  if (this != GetShape())
    delete this;
}

void wxShapeEvtHandler::OnDraw(wxDC& dc)
{
  if (m_previousHandler)
    m_previousHandler->OnDraw(dc);
}

void wxShapeEvtHandler::OnMoveLinks(wxDC& dc)
{
  if (m_previousHandler)
    m_previousHandler->OnMoveLinks(dc);
}

void wxShapeEvtHandler::OnMoveLink(wxDC& dc, bool moveControlPoints)
{
  if (m_previousHandler)
    m_previousHandler->OnMoveLink(dc, moveControlPoints);
}

void wxShapeEvtHandler::OnDrawContents(wxDC& dc)
{
  if (m_previousHandler)
    m_previousHandler->OnDrawContents(dc);
}

void wxShapeEvtHandler::OnDrawBranches(wxDC& dc, bool erase)
{
  if (m_previousHandler)
    m_previousHandler->OnDrawBranches(dc, erase);
}

void wxShapeEvtHandler::OnSize(double x, double y)
{
  if (m_previousHandler)
    m_previousHandler->OnSize(x, y);
}

bool wxShapeEvtHandler::OnMovePre(wxDC& dc, double x, double y, double old_x, double old_y, bool display)
{
  if (m_previousHandler)
    return m_previousHandler->OnMovePre(dc, x, y, old_x, old_y, display);
  else
    return true;
}

void wxShapeEvtHandler::OnMovePost(wxDC& dc, double x, double y, double old_x, double old_y, bool display)
{
  if (m_previousHandler)
    m_previousHandler->OnMovePost(dc, x, y, old_x, old_y, display);
}

void wxShapeEvtHandler::OnErase(wxDC& dc)
{
  if (m_previousHandler)
    m_previousHandler->OnErase(dc);
}

void wxShapeEvtHandler::OnEraseContents(wxDC& dc)
{
  if (m_previousHandler)
    m_previousHandler->OnEraseContents(dc);
}

void wxShapeEvtHandler::OnHighlight(wxDC& dc)
{
  if (m_previousHandler)
    m_previousHandler->OnHighlight(dc);
}

void wxShapeEvtHandler::OnLeftClick(double x, double y, int keys, int attachment)
{
  if (m_previousHandler)
    m_previousHandler->OnLeftClick(x, y, keys, attachment);
}

void wxShapeEvtHandler::OnLeftDoubleClick(double x, double y, int keys, int attachment)
{
  if (m_previousHandler)
    m_previousHandler->OnLeftDoubleClick(x, y, keys, attachment);
}

void wxShapeEvtHandler::OnRightClick(double x, double y, int keys, int attachment)
{
  if (m_previousHandler)
    m_previousHandler->OnRightClick(x, y, keys, attachment);
}

void wxShapeEvtHandler::OnDragLeft(bool draw, double x, double y, int keys, int attachment)
{
  if (m_previousHandler)
    m_previousHandler->OnDragLeft(draw, x, y, keys, attachment);
}

void wxShapeEvtHandler::OnBeginDragLeft(double x, double y, int keys, int attachment)
{
  if (m_previousHandler)
    m_previousHandler->OnBeginDragLeft(x, y, keys, attachment);
}

void wxShapeEvtHandler::OnEndDragLeft(double x, double y, int keys, int attachment)
{
  if (m_previousHandler)
    m_previousHandler->OnEndDragLeft(x, y, keys, attachment);
}

void wxShapeEvtHandler::OnDragRight(bool draw, double x, double y, int keys, int attachment)
{
  if (m_previousHandler)
    m_previousHandler->OnDragRight(draw, x, y, keys, attachment);
}

void wxShapeEvtHandler::OnBeginDragRight(double x, double y, int keys, int attachment)
{
  if (m_previousHandler)
    m_previousHandler->OnBeginDragRight(x, y, keys, attachment);
}

void wxShapeEvtHandler::OnEndDragRight(double x, double y, int keys, int attachment)
{
  if (m_previousHandler)
    m_previousHandler->OnEndDragRight(x, y, keys, attachment);
}

// Control points ('handles') redirect control to the actual shape, to make it easier
// to override sizing behaviour.
void wxShapeEvtHandler::OnSizingDragLeft(wxControlPoint* pt, bool draw, double x, double y, int keys, int attachment)
{
  if (m_previousHandler)
    m_previousHandler->OnSizingDragLeft(pt, draw, x, y, keys, attachment);
}

void wxShapeEvtHandler::OnSizingBeginDragLeft(wxControlPoint* pt, double x, double y, int keys, int attachment)
{
  if (m_previousHandler)
    m_previousHandler->OnSizingBeginDragLeft(pt, x, y, keys, attachment);
}

void wxShapeEvtHandler::OnSizingEndDragLeft(wxControlPoint* pt, double x, double y, int keys, int attachment)
{
  if (m_previousHandler)
    m_previousHandler->OnSizingEndDragLeft(pt, x, y, keys, attachment);
}

void wxShapeEvtHandler::OnDrawOutline(wxDC& dc, double x, double y, double w, double h)
{
  if (m_previousHandler)
    m_previousHandler->OnDrawOutline(dc, x, y, w, h);
}

void wxShapeEvtHandler::OnDrawControlPoints(wxDC& dc)
{
  if (m_previousHandler)
    m_previousHandler->OnDrawControlPoints(dc);
}

void wxShapeEvtHandler::OnEraseControlPoints(wxDC& dc)
{
  if (m_previousHandler)
    m_previousHandler->OnEraseControlPoints(dc);
}

// Can override this to prevent or intercept line reordering.
void wxShapeEvtHandler::OnChangeAttachment(int attachment, wxLineShape* line, wxList& ordering)
{
  if (m_previousHandler)
    m_previousHandler->OnChangeAttachment(attachment, line, ordering);
}

IMPLEMENT_ABSTRACT_CLASS(wxShape, wxShapeEvtHandler)

wxShape::wxShape(wxShapeCanvas *can)
{
  m_eventHandler = this;
  SetShape(this);
  m_id = 0;
  m_formatted = false;
  m_canvas = can;
  m_xpos = 0.0; m_ypos = 0.0;
  m_pen = g_oglBlackPen;
  m_brush = wxWHITE_BRUSH;
  m_font = g_oglNormalFont;
  m_textColour = wxT("BLACK");
  m_textColourName = wxT("BLACK");
  m_visible = false;
  m_selected = false;
  m_attachmentMode = ATTACHMENT_MODE_NONE;
  m_spaceAttachments = true;
  m_disableLabel = false;
  m_fixedWidth = false;
  m_fixedHeight = false;
  m_drawHandles = true;
  m_sensitivity = OP_ALL;
  m_draggable = true;
  m_parent = NULL;
  m_formatMode = FORMAT_CENTRE_HORIZ | FORMAT_CENTRE_VERT;
  m_shadowMode = SHADOW_NONE;
  m_shadowOffsetX = 6;
  m_shadowOffsetY = 6;
  m_shadowBrush = wxBLACK_BRUSH;
  m_textMarginX = 5;
  m_textMarginY = 5;
  m_regionName = wxT("0");
  m_centreResize = true;
  m_maintainAspectRatio = false;
  m_highlighted = false;
  m_rotation = 0.0;
  m_branchNeckLength = 10;
  m_branchStemLength = 10;
  m_branchSpacing = 10;
  m_branchStyle = BRANCHING_ATTACHMENT_NORMAL;

  // Set up a default region. Much of the above will be put into
  // the region eventually (the duplication is for compatibility)
  wxShapeRegion *region = new wxShapeRegion;
  m_regions.Append(region);
  region->SetName(wxT("0"));
  region->SetFont(g_oglNormalFont);
  region->SetFormatMode(FORMAT_CENTRE_HORIZ | FORMAT_CENTRE_VERT);
  region->SetColour(wxT("BLACK"));
}

wxShape::~wxShape()
{
  if (m_parent)
    m_parent->GetChildren().DeleteObject(this);

  ClearText();
  ClearRegions();
  ClearAttachments();

  if (m_canvas)
    m_canvas->RemoveShape(this);

  GetEventHandler()->OnDelete();
}

void wxShape::SetHighlight(bool hi, bool recurse)
{
  m_highlighted = hi;
  if (recurse)
  {
    wxNode *node = m_children.GetFirst();
    while (node)
    {
      wxShape *child = (wxShape *)node->GetData();
      child->SetHighlight(hi, recurse);
      node = node->GetNext();
    }
  }
}

void wxShape::SetSensitivityFilter(int sens, bool recursive)
{
  if (sens & OP_DRAG_LEFT)
    m_draggable = true;
  else
    m_draggable = false;

  m_sensitivity = sens;
  if (recursive)
  {
    wxNode *node = m_children.GetFirst();
    while (node)
    {
      wxShape *obj = (wxShape *)node->GetData();
      obj->SetSensitivityFilter(sens, true);
      node = node->GetNext();
    }
  }
}

void wxShape::SetDraggable(bool drag, bool recursive)
{
  m_draggable = drag;
  if (m_draggable)
    m_sensitivity |= OP_DRAG_LEFT;
  else
    if (m_sensitivity & OP_DRAG_LEFT)
      m_sensitivity = m_sensitivity - OP_DRAG_LEFT;

  if (recursive)
  {
    wxNode *node = m_children.GetFirst();
    while (node)
    {
      wxShape *obj = (wxShape *)node->GetData();
      obj->SetDraggable(drag, true);
      node = node->GetNext();
    }
  }
}

void wxShape::SetDrawHandles(bool drawH)
{
  m_drawHandles = drawH;
  wxNode *node = m_children.GetFirst();
  while (node)
  {
    wxShape *obj = (wxShape *)node->GetData();
    obj->SetDrawHandles(drawH);
    node = node->GetNext();
  }
}

void wxShape::SetShadowMode(int mode, bool redraw)
{
  if (redraw && GetCanvas())
  {
    wxClientDC dc(GetCanvas());
    GetCanvas()->PrepareDC(dc);
    Erase(dc);

    m_shadowMode = mode;

    Draw(dc);
  }
  else
  {
    m_shadowMode = mode;
  }
}

void wxShape::SetCanvas(wxShapeCanvas *theCanvas)
{
  m_canvas = theCanvas;
  wxNode *node = m_children.GetFirst();
  while (node)
  {
    wxShape *child = (wxShape *)node->GetData();
    child->SetCanvas(theCanvas);
    node = node->GetNext();
  }
}

void wxShape::AddToCanvas(wxShapeCanvas *theCanvas, wxShape *addAfter)
{
  theCanvas->AddShape(this, addAfter);
  wxNode *node = m_children.GetFirst();
  wxShape *lastImage = this;
  while (node)
  {
    wxShape *object = (wxShape *)node->GetData();
    object->AddToCanvas(theCanvas, lastImage);
    lastImage = object;

    node = node->GetNext();
  }
}

// Insert at front of canvas
void wxShape::InsertInCanvas(wxShapeCanvas *theCanvas)
{
  theCanvas->InsertShape(this);
  wxNode *node = m_children.GetFirst();
  wxShape *lastImage = this;
  while (node)
  {
    wxShape *object = (wxShape *)node->GetData();
    object->AddToCanvas(theCanvas, lastImage);
    lastImage = object;

    node = node->GetNext();
  }
}

void wxShape::RemoveFromCanvas(wxShapeCanvas *theCanvas)
{
  if (Selected())
    Select(false);
  theCanvas->RemoveShape(this);
  wxNode *node = m_children.GetFirst();
  while (node)
  {
    wxShape *object = (wxShape *)node->GetData();
    object->RemoveFromCanvas(theCanvas);

    node = node->GetNext();
  }
}

void wxShape::ClearAttachments()
{
  wxNode *node = m_attachmentPoints.GetFirst();
  while (node)
  {
    wxAttachmentPoint *point = (wxAttachmentPoint *)node->GetData();
    delete point;
    node = node->GetNext();
  }
  m_attachmentPoints.Clear();
}

void wxShape::ClearText(int regionId)
{
  if (regionId == 0)
  {
    m_text.DeleteContents(true);
    m_text.Clear();
    m_text.DeleteContents(false);
  }
  wxNode *node = m_regions.Item(regionId);
  if (!node)
    return;
  wxShapeRegion *region = (wxShapeRegion *)node->GetData();
  region->ClearText();
}

void wxShape::ClearRegions()
{
  wxNode *node = m_regions.GetFirst();
  while (node)
  {
    wxShapeRegion *region = (wxShapeRegion *)node->GetData();
    wxNode *next = node->GetNext();
    delete region;
    delete node;
    node = next;
  }
}

void wxShape::AddRegion(wxShapeRegion *region)
{
  m_regions.Append(region);
}

void wxShape::SetDefaultRegionSize()
{
  wxNode *node = m_regions.GetFirst();
  if (!node) return;
  wxShapeRegion *region = (wxShapeRegion *)node->GetData();
  double w, h;
  GetBoundingBoxMin(&w, &h);
  region->SetSize(w, h);
}

bool wxShape::HitTest(double x, double y, int *attachment, double *distance)
{
//  if (!sensitive)
//    return false;

  double width = 0.0, height = 0.0;
  GetBoundingBoxMin(&width, &height);
  if (fabs(width) < 4.0) width = 4.0;
  if (fabs(height) < 4.0) height = 4.0;

  width += (double)4.0; height += (double)4.0; // Allowance for inaccurate mousing

  double left = (double)(m_xpos - (width/2.0));
  double top = (double)(m_ypos - (height/2.0));
  double right = (double)(m_xpos + (width/2.0));
  double bottom = (double)(m_ypos + (height/2.0));

  int nearest_attachment = 0;

  // If within the bounding box, check the attachment points
  // within the object.

  if (x >= left && x <= right && y >= top && y <= bottom)
  {
    int n = GetNumberOfAttachments();
    double nearest = 999999.0;

    // GetAttachmentPosition[Edge] takes a logical attachment position,
    // i.e. if it's rotated through 90%, position 0 is East-facing.

    for (int i = 0; i < n; i++)
    {
      double xp, yp;
      if (GetAttachmentPositionEdge(i, &xp, &yp))
      {
        double l = (double)sqrt(((xp - x) * (xp - x)) +
                   ((yp - y) * (yp - y)));

        if (l < nearest)
        {
          nearest = l;
          nearest_attachment = i;
        }
      }
    }
    *attachment = nearest_attachment;
    *distance = nearest;
    return true;
  }
  else return false;
}

// Format a text string according to the region size, adding
// strings with positions to region text list

static bool GraphicsInSizeToContents = false; // Infinite recursion elimination
void wxShape::FormatText(wxDC& dc, const wxString& s, int i)
{
  double w, h;
  ClearText(i);

  if (m_regions.GetCount() < 1)
    return;
  wxNode *node = m_regions.Item(i);
  if (!node)
    return;

  wxShapeRegion *region = (wxShapeRegion *)node->GetData();
  // region->SetText(s);  // don't set the formatted text yet, it will be done below
  region->m_regionText = s;
  dc.SetFont(* region->GetFont());

  region->GetSize(&w, &h);

  wxStringList *stringList = oglFormatText(dc, s, (w-2*m_textMarginX), (h-2*m_textMarginY), region->GetFormatMode());
  node = (wxNode*)stringList->GetFirst();
  while (node)
  {
    wxChar *s = (wxChar *)node->GetData();
    wxShapeTextLine *line = new wxShapeTextLine(0.0, 0.0, s);
    region->GetFormattedText().Append((wxObject *)line);
    node = node->GetNext();
  }
  delete stringList;
  double actualW = w;
  double actualH = h;
  // Don't try to resize an object with more than one image (this case should be dealt
  // with by overriden handlers)
  if ((region->GetFormatMode() & FORMAT_SIZE_TO_CONTENTS) &&
      (region->GetFormattedText().GetCount() > 0) &&
      (m_regions.GetCount() == 1) && !GraphicsInSizeToContents)
  {
    oglGetCentredTextExtent(dc, &(region->GetFormattedText()), m_xpos, m_ypos, w, h, &actualW, &actualH);
    if ((actualW+2*m_textMarginX != w ) || (actualH+2*m_textMarginY != h))
    {
      // If we are a descendant of a composite, must make sure the composite gets
      // resized properly
      wxShape *topAncestor = GetTopAncestor();

      if (topAncestor != this)
      {
        // Make sure we don't recurse infinitely
        GraphicsInSizeToContents = true;

        wxCompositeShape *composite = (wxCompositeShape *)topAncestor;
        composite->Erase(dc);
        SetSize(actualW+2*m_textMarginX, actualH+2*m_textMarginY);
        Move(dc, m_xpos, m_ypos);
        composite->CalculateSize();
        if (composite->Selected())
        {
          composite->DeleteControlPoints(& dc);
          composite->MakeControlPoints();
          composite->MakeMandatoryControlPoints();
        }
        // Where infinite recursion might happen if we didn't stop it
        composite->Draw(dc);

        GraphicsInSizeToContents = false;
      }
      else
      {
        Erase(dc);
        SetSize(actualW+2*m_textMarginX, actualH+2*m_textMarginY);
        Move(dc, m_xpos, m_ypos);
      }
      SetSize(actualW+2*m_textMarginX, actualH+2*m_textMarginY);
      Move(dc, m_xpos, m_ypos);
      EraseContents(dc);
    }
  }
  oglCentreText(dc, &(region->GetFormattedText()), m_xpos, m_ypos, actualW-2*m_textMarginX, actualH-2*m_textMarginY, region->GetFormatMode());
  m_formatted = true;
}

void wxShape::Recentre(wxDC& dc)
{
  double w, h;
  GetBoundingBoxMin(&w, &h);

  int noRegions = m_regions.GetCount();
  for (int i = 0; i < noRegions; i++)
  {
    wxNode *node = m_regions.Item(i);
    if (node)
    {
      wxShapeRegion *region = (wxShapeRegion *)node->GetData();
      oglCentreText(dc, &(region->GetFormattedText()), m_xpos, m_ypos, w-2*m_textMarginX, h-2*m_textMarginY, region->GetFormatMode());
    }
  }
}

bool wxShape::GetPerimeterPoint(double WXUNUSED(x1), double WXUNUSED(y1),
                                     double WXUNUSED(x2), double WXUNUSED(y2),
                                     double *WXUNUSED(x3), double *WXUNUSED(y3))
{
  return false;
}

void wxShape::SetPen(const wxPen *the_pen)
{
  m_pen = the_pen;
}

void wxShape::SetBrush(const wxBrush *the_brush)
{
  m_brush = the_brush;
}

// Get the top-most (non-division) ancestor, or self
wxShape *wxShape::GetTopAncestor()
{
  if (!GetParent())
    return this;

  if (GetParent()->IsKindOf(CLASSINFO(wxDivisionShape)))
    return this;
  else return GetParent()->GetTopAncestor();
}

/*
 * Region functions
 *
 */
void wxShape::SetFont(wxFont *the_font, int regionId)
{
  m_font = the_font;
  wxNode *node = m_regions.Item(regionId);
  if (!node)
    return;
  wxShapeRegion *region = (wxShapeRegion *)node->GetData();
  region->SetFont(the_font);
}

wxFont *wxShape::GetFont(int n) const
{
  wxNode *node = m_regions.Item(n);
  if (!node)
    return NULL;
  wxShapeRegion *region = (wxShapeRegion *)node->GetData();
  return region->GetFont();
}

void wxShape::SetFormatMode(int mode, int regionId)
{
  wxNode *node = m_regions.Item(regionId);
  if (!node)
    return;
  wxShapeRegion *region = (wxShapeRegion *)node->GetData();
  region->SetFormatMode(mode);
}

int wxShape::GetFormatMode(int regionId) const
{
  wxNode *node = m_regions.Item(regionId);
  if (!node)
    return 0;
  wxShapeRegion *region = (wxShapeRegion *)node->GetData();
  return region->GetFormatMode();
}

void wxShape::SetTextColour(const wxString& the_colour, int regionId)
{
  m_textColour = wxTheColourDatabase->Find(the_colour);
  m_textColourName = the_colour;

  wxNode *node = m_regions.Item(regionId);
  if (!node)
    return;
  wxShapeRegion *region = (wxShapeRegion *)node->GetData();
  region->SetColour(the_colour);
}

wxString wxShape::GetTextColour(int regionId) const
{
  wxNode *node = m_regions.Item(regionId);
  if (!node)
    return wxEmptyString;
  wxShapeRegion *region = (wxShapeRegion *)node->GetData();
  return region->GetColour();
}

void wxShape::SetRegionName(const wxString& name, int regionId)
{
  wxNode *node = m_regions.Item(regionId);
  if (!node)
    return;
  wxShapeRegion *region = (wxShapeRegion *)node->GetData();
  region->SetName(name);
}

wxString wxShape::GetRegionName(int regionId)
{
  wxNode *node = m_regions.Item(regionId);
  if (!node)
    return wxEmptyString;
  wxShapeRegion *region = (wxShapeRegion *)node->GetData();
  return region->GetName();
}

int wxShape::GetRegionId(const wxString& name)
{
  wxNode *node = m_regions.GetFirst();
  int i = 0;
  while (node)
  {
    wxShapeRegion *region = (wxShapeRegion *)node->GetData();
    if (region->GetName() == name)
      return i;
    node = node->GetNext();
    i ++;
  }
  return -1;
}

// Name all m_regions in all subimages recursively.
void wxShape::NameRegions(const wxString& parentName)
{
  int n = GetNumberOfTextRegions();
  wxString buff;
  for (int i = 0; i < n; i++)
  {
    if (parentName.Length() > 0)
      buff << parentName << wxT(".") << i;
    else
      buff << i;
    SetRegionName(buff, i);
  }
  wxNode *node = m_children.GetFirst();
  int j = 0;
  while (node)
  {
    buff.Empty();
    wxShape *child = (wxShape *)node->GetData();
    if (parentName.Length() > 0)
      buff << parentName << wxT(".") << j;
    else
      buff << j;
    child->NameRegions(buff);
    node = node->GetNext();
    j ++;
  }
}

// Get a region by name, possibly looking recursively into composites.
wxShape *wxShape::FindRegion(const wxString& name, int *regionId)
{
  int id = GetRegionId(name);
  if (id > -1)
  {
    *regionId = id;
    return this;
  }

  wxNode *node = m_children.GetFirst();
  while (node)
  {
    wxShape *child = (wxShape *)node->GetData();
    wxShape *actualImage = child->FindRegion(name, regionId);
    if (actualImage)
      return actualImage;
    node = node->GetNext();
  }
  return NULL;
}

// Finds all region names for this image (composite or simple).
// Supply empty string list.
void wxShape::FindRegionNames(wxStringList& list)
{
  int n = GetNumberOfTextRegions();
  for (int i = 0; i < n; i++)
  {
    wxString name(GetRegionName(i));
    list.Add(name);
  }

  wxNode *node = m_children.GetFirst();
  while (node)
  {
    wxShape *child = (wxShape *)node->GetData();
    child->FindRegionNames(list);
    node = node->GetNext();
  }
}

void wxShape::AssignNewIds()
{
//  if (m_id == 0)
  m_id = wxNewId();
  wxNode *node = m_children.GetFirst();
  while (node)
  {
    wxShape *child = (wxShape *)node->GetData();
    child->AssignNewIds();
    node = node->GetNext();
  }
}

void wxShape::OnDraw(wxDC& WXUNUSED(dc))
{
}

void wxShape::OnMoveLinks(wxDC& dc)
{
  // Want to set the ends of all attached links
  // to point to/from this object

  wxNode *current = m_lines.GetFirst();
  while (current)
  {
    wxLineShape *line = (wxLineShape *)current->GetData();
    line->GetEventHandler()->OnMoveLink(dc);
    current = current->GetNext();
  }
}


void wxShape::OnDrawContents(wxDC& dc)
{
  double bound_x, bound_y;
  GetBoundingBoxMin(&bound_x, &bound_y);
    if (m_regions.GetCount() < 1) return;

    if (m_pen) dc.SetPen(* m_pen);

    wxShapeRegion *region = (wxShapeRegion *)m_regions.GetFirst()->GetData();
    if (region->GetFont()) dc.SetFont(* region->GetFont());

    dc.SetTextForeground(region->GetActualColourObject());
    dc.SetBackgroundMode(wxTRANSPARENT);
    if (!m_formatted)
    {
      oglCentreText(dc, &(region->GetFormattedText()), m_xpos, m_ypos, bound_x-2*m_textMarginX, bound_y-2*m_textMarginY, region->GetFormatMode());
      m_formatted = true;
    }
    if (!GetDisableLabel())
    {
      oglDrawFormattedText(dc, &(region->GetFormattedText()), m_xpos, m_ypos, bound_x-2*m_textMarginX, bound_y-2*m_textMarginY, region->GetFormatMode());
    }
}

void wxShape::DrawContents(wxDC& dc)
{
  GetEventHandler()->OnDrawContents(dc);
}

void wxShape::OnSize(double WXUNUSED(x), double WXUNUSED(y))
{
}

bool wxShape::OnMovePre(wxDC& WXUNUSED(dc), double WXUNUSED(x), double WXUNUSED(y), double WXUNUSED(old_x), double WXUNUSED(old_y), bool WXUNUSED(display))
{
  return true;
}

void wxShape::OnMovePost(wxDC& WXUNUSED(dc), double WXUNUSED(x), double WXUNUSED(y), double WXUNUSED(old_x), double WXUNUSED(old_y), bool WXUNUSED(display))
{
}

void wxShape::OnErase(wxDC& dc)
{
  if (!m_visible)
    return;

  // Erase links
  wxNode *current = m_lines.GetFirst();
  while (current)
  {
    wxLineShape *line = (wxLineShape *)current->GetData();
    line->GetEventHandler()->OnErase(dc);
    current = current->GetNext();
  }
  GetEventHandler()->OnEraseContents(dc);
}

void wxShape::OnEraseContents(wxDC& dc)
{
  if (!m_visible)
    return;

  double maxX, maxY, minX, minY;
  double xp = GetX();
  double yp = GetY();
  GetBoundingBoxMin(&minX, &minY);
  GetBoundingBoxMax(&maxX, &maxY);
  double topLeftX = (double)(xp - (maxX / 2.0) - 2.0);
  double topLeftY = (double)(yp - (maxY / 2.0) - 2.0);

    int penWidth = 0;
    if (m_pen)
      penWidth = m_pen->GetWidth();

    dc.SetPen(GetBackgroundPen());
    dc.SetBrush(GetBackgroundBrush());

    dc.DrawRectangle(WXROUND(topLeftX - penWidth), WXROUND(topLeftY - penWidth),
                      WXROUND(maxX + penWidth*2.0 + 4.0), WXROUND(maxY + penWidth*2.0 + 4.0));
}

void wxShape::EraseLinks(wxDC& dc, int attachment, bool recurse)
{
  if (!m_visible)
    return;

  wxNode *current = m_lines.GetFirst();
  while (current)
  {
    wxLineShape *line = (wxLineShape *)current->GetData();
    if (attachment == -1 || ((line->GetTo() == this && line->GetAttachmentTo() == attachment) ||
                             (line->GetFrom() == this && line->GetAttachmentFrom() == attachment)))
      line->GetEventHandler()->OnErase(dc);
    current = current->GetNext();
  }
  if (recurse)
  {
    wxNode *node = m_children.GetFirst();
    while (node)
    {
      wxShape *child = (wxShape *)node->GetData();
      child->EraseLinks(dc, attachment, recurse);
      node = node->GetNext();
    }
  }
}

void wxShape::DrawLinks(wxDC& dc, int attachment, bool recurse)
{
  if (!m_visible)
    return;

  wxNode *current = m_lines.GetFirst();
  while (current)
  {
    wxLineShape *line = (wxLineShape *)current->GetData();
    if (attachment == -1 ||
        (line->GetTo() == this && line->GetAttachmentTo() == attachment) ||
        (line->GetFrom() == this && line->GetAttachmentFrom() == attachment))
      line->Draw(dc);
    current = current->GetNext();
  }
  if (recurse)
  {
    wxNode *node = m_children.GetFirst();
    while (node)
    {
      wxShape *child = (wxShape *)node->GetData();
      child->DrawLinks(dc, attachment, recurse);
      node = node->GetNext();
    }
  }
}

// Returns true if pt1 <= pt2 in the sense that one point comes before another on an
// edge of the shape.
// attachmentPoint is the attachment point (= side) in question.

// This is the default, rectangular implementation.
bool wxShape::AttachmentSortTest(int attachmentPoint, const wxRealPoint& pt1, const wxRealPoint& pt2)
{
    int physicalAttachment = LogicalToPhysicalAttachment(attachmentPoint);
    switch (physicalAttachment)
    {
        case 0:
        case 2:
        {
          return (pt1.x <= pt2.x) ;
        }
        case 1:
        case 3:
        {
          return (pt1.y <= pt2.y) ;
        }
    }

    return false;
}

bool wxShape::MoveLineToNewAttachment(wxDC& dc, wxLineShape *to_move,
                                       double x, double y)
{
  if (GetAttachmentMode() == ATTACHMENT_MODE_NONE)
      return false;

  int newAttachment, oldAttachment;
  double distance;

  // Is (x, y) on this object? If so, find the new attachment point
  // the user has moved the point to
  bool hit = HitTest(x, y, &newAttachment, &distance);
  if (!hit)
    return false;

  EraseLinks(dc);

  if (to_move->GetTo() == this)
    oldAttachment = to_move->GetAttachmentTo();
  else
    oldAttachment = to_move->GetAttachmentFrom();

  // The links in a new ordering.
  wxList newOrdering;

  // First, add all links to the new list.
  wxNode *node = m_lines.GetFirst();
  while (node)
  {
    newOrdering.Append(node->GetData());
    node = node->GetNext();
  }

  // Delete the line object from the list of links; we're going to move
  // it to another position in the list
  newOrdering.DeleteObject(to_move);

  double old_x = (double) -99999.9;
  double old_y = (double) -99999.9;

  node = newOrdering.GetFirst();
  bool found = false;

  while (!found && node)
  {
    wxLineShape *line = (wxLineShape *)node->GetData();
    if ((line->GetTo() == this && oldAttachment == line->GetAttachmentTo()) ||
        (line->GetFrom() == this && oldAttachment == line->GetAttachmentFrom()))
    {
      double startX, startY, endX, endY;
      double xp, yp;
      line->GetEnds(&startX, &startY, &endX, &endY);
      if (line->GetTo() == this)
      {
        xp = endX;
        yp = endY;
      } else
      {
        xp = startX;
        yp = startY;
      }

      wxRealPoint thisPoint(xp, yp);
      wxRealPoint lastPoint(old_x, old_y);
      wxRealPoint newPoint(x, y);

      if (AttachmentSortTest(newAttachment, newPoint, thisPoint) && AttachmentSortTest(newAttachment, lastPoint, newPoint))
      {
        found = true;
        newOrdering.Insert(node, to_move);
      }

      old_x = xp;
      old_y = yp;
    }
    node = node->GetNext();
  }

  if (!found)
    newOrdering.Append(to_move);

  GetEventHandler()->OnChangeAttachment(newAttachment, to_move, newOrdering);

  return true;
}

void wxShape::OnChangeAttachment(int attachment, wxLineShape* line, wxList& ordering)
{
    if (line->GetTo() == this)
        line->SetAttachmentTo(attachment);
    else
        line->SetAttachmentFrom(attachment);

    ApplyAttachmentOrdering(ordering);

    wxClientDC dc(GetCanvas());
    GetCanvas()->PrepareDC(dc);

    MoveLinks(dc);

    if (!GetCanvas()->GetQuickEditMode()) GetCanvas()->Redraw(dc);
}

// Reorders the lines according to the given list.
void wxShape::ApplyAttachmentOrdering(wxList& linesToSort)
{
  // This is a temporary store of all the lines.
  wxList linesStore;

  wxNode *node = m_lines.GetFirst();
  while (node)
  {
    wxLineShape *line = (wxLineShape *)node->GetData();
    linesStore.Append(line);
    node = node->GetNext();;
  }

  m_lines.Clear();

  node = linesToSort.GetFirst();
  while (node)
  {
    wxLineShape *line = (wxLineShape *)node->GetData();
    if (linesStore.Member(line))
    {
      // Done this one
      linesStore.DeleteObject(line);
      m_lines.Append(line);
    }
    node = node->GetNext();
  }

  // Now add any lines that haven't been listed in linesToSort.
  node = linesStore.GetFirst();
  while (node)
  {
    wxLineShape *line = (wxLineShape *)node->GetData();
    m_lines.Append(line);
    node = node->GetNext();
  }
}

// Reorders the lines coming into the node image at this attachment
// position, in the order in which they appear in linesToSort.
// Any remaining lines not in the list will be added to the end.
void wxShape::SortLines(int attachment, wxList& linesToSort)
{
  // This is a temporary store of all the lines at this attachment
  // point. We'll tick them off as we've processed them.
  wxList linesAtThisAttachment;

  wxNode *node = m_lines.GetFirst();
  while (node)
  {
    wxLineShape *line = (wxLineShape *)node->GetData();
    wxNode *next = node->GetNext();
    if ((line->GetTo() == this && line->GetAttachmentTo() == attachment) ||
        (line->GetFrom() == this && line->GetAttachmentFrom() == attachment))
    {
      linesAtThisAttachment.Append(line);
      delete node;
      node = next;
    }
    else node = node->GetNext();
  }

  node = linesToSort.GetFirst();
  while (node)
  {
    wxLineShape *line = (wxLineShape *)node->GetData();
    if (linesAtThisAttachment.Member(line))
    {
      // Done this one
      linesAtThisAttachment.DeleteObject(line);
      m_lines.Append(line);
    }
    node = node->GetNext();
  }

  // Now add any lines that haven't been listed in linesToSort.
  node = linesAtThisAttachment.GetFirst();
  while (node)
  {
    wxLineShape *line = (wxLineShape *)node->GetData();
    m_lines.Append(line);
    node = node->GetNext();
  }
}

void wxShape::OnHighlight(wxDC& WXUNUSED(dc))
{
}

void wxShape::OnLeftClick(double x, double y, int keys, int attachment)
{
  if ((m_sensitivity & OP_CLICK_LEFT) != OP_CLICK_LEFT)
  {
    attachment = 0;
    double dist;
    if (m_parent)
    {
      m_parent->HitTest(x, y, &attachment, &dist);
      m_parent->GetEventHandler()->OnLeftClick(x, y, keys, attachment);
    }
    return;
  }
}

void wxShape::OnRightClick(double x, double y, int keys, int attachment)
{
  if ((m_sensitivity & OP_CLICK_RIGHT) != OP_CLICK_RIGHT)
  {
    attachment = 0;
    double dist;
    if (m_parent)
    {
      m_parent->HitTest(x, y, &attachment, &dist);
      m_parent->GetEventHandler()->OnRightClick(x, y, keys, attachment);
    }
    return;
  }
}

double DragOffsetX = 0.0;
double DragOffsetY = 0.0;

void wxShape::OnDragLeft(bool draw, double x, double y, int keys, int attachment)
{
  if ((m_sensitivity & OP_DRAG_LEFT) != OP_DRAG_LEFT)
  {
    attachment = 0;
    double dist;
    if (m_parent)
    {
      m_parent->HitTest(x, y, &attachment, &dist);
      m_parent->GetEventHandler()->OnDragLeft(draw, x, y, keys, attachment);
    }
    return;
  }

  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  dc.SetLogicalFunction(OGLRBLF);

  wxPen dottedPen(*wxBLACK, 1, wxDOT);
  dc.SetPen(dottedPen);
  dc.SetBrush(* wxTRANSPARENT_BRUSH);

  double xx, yy;
  xx = x + DragOffsetX;
  yy = y + DragOffsetY;

  m_canvas->Snap(&xx, &yy);
//  m_xpos = xx; m_ypos = yy;
  double w, h;
  GetBoundingBoxMax(&w, &h);
  GetEventHandler()->OnDrawOutline(dc, xx, yy, w, h);
}

void wxShape::OnBeginDragLeft(double x, double y, int keys, int attachment)
{
  if ((m_sensitivity & OP_DRAG_LEFT) != OP_DRAG_LEFT)
  {
    attachment = 0;
    double dist;
    if (m_parent)
    {
      m_parent->HitTest(x, y, &attachment, &dist);
      m_parent->GetEventHandler()->OnBeginDragLeft(x, y, keys, attachment);
    }
    return;
  }

  DragOffsetX = m_xpos - x;
  DragOffsetY = m_ypos - y;

  wxClientDC dc(GetCanvas());
  GetCanvas()->PrepareDC(dc);

  // New policy: don't erase shape until end of drag.
//  Erase(dc);

  double xx, yy;
  xx = x + DragOffsetX;
  yy = y + DragOffsetY;
  m_canvas->Snap(&xx, &yy);
//  m_xpos = xx; m_ypos = yy;
  dc.SetLogicalFunction(OGLRBLF);

  wxPen dottedPen(*wxBLACK, 1, wxDOT);
  dc.SetPen(dottedPen);
  dc.SetBrush((* wxTRANSPARENT_BRUSH));

  double w, h;
  GetBoundingBoxMax(&w, &h);
  GetEventHandler()->OnDrawOutline(dc, xx, yy, w, h);
  m_canvas->CaptureMouse();
}

void wxShape::OnEndDragLeft(double x, double y, int keys, int attachment)
{
  if (!m_draggable)
    return;
  m_canvas->ReleaseMouse();
  if ((m_sensitivity & OP_DRAG_LEFT) != OP_DRAG_LEFT)
  {
    attachment = 0;
    double dist;
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

  double xx = x + DragOffsetX;
  double yy = y + DragOffsetY;
  m_canvas->Snap(&xx, &yy);
//  canvas->Snap(&m_xpos, &m_ypos);

  // New policy: erase shape at end of drag.
  Erase(dc);

  Move(dc, xx, yy);
  if (m_canvas && !m_canvas->GetQuickEditMode()) m_canvas->Redraw(dc);
}

void wxShape::OnDragRight(bool draw, double x, double y, int keys, int attachment)
{
  if ((m_sensitivity & OP_DRAG_RIGHT) != OP_DRAG_RIGHT)
  {
    attachment = 0;
    double dist;
    if (m_parent)
    {
      m_parent->HitTest(x, y, &attachment, &dist);
      m_parent->GetEventHandler()->OnDragRight(draw, x, y, keys, attachment);
    }
    return;
  }
}

void wxShape::OnBeginDragRight(double x, double y, int keys, int attachment)
{
  if ((m_sensitivity & OP_DRAG_RIGHT) != OP_DRAG_RIGHT)
  {
    attachment = 0;
    double dist;
    if (m_parent)
    {
      m_parent->HitTest(x, y, &attachment, &dist);
      m_parent->GetEventHandler()->OnBeginDragRight(x, y, keys, attachment);
    }
    return;
  }
}

void wxShape::OnEndDragRight(double x, double y, int keys, int attachment)
{
  if ((m_sensitivity & OP_DRAG_RIGHT) != OP_DRAG_RIGHT)
  {
    attachment = 0;
    double dist;
    if (m_parent)
    {
      m_parent->HitTest(x, y, &attachment, &dist);
      m_parent->GetEventHandler()->OnEndDragRight(x, y, keys, attachment);
    }
    return;
  }
}

void wxShape::OnDrawOutline(wxDC& dc, double x, double y, double w, double h)
{
  double top_left_x = (double)(x - w/2.0);
  double top_left_y = (double)(y - h/2.0);
  double top_right_x = (double)(top_left_x + w);
  double top_right_y = (double)top_left_y;
  double bottom_left_x = (double)top_left_x;
  double bottom_left_y = (double)(top_left_y + h);
  double bottom_right_x = (double)top_right_x;
  double bottom_right_y = (double)bottom_left_y;

  wxPoint points[5];
  points[0].x = WXROUND(top_left_x); points[0].y = WXROUND(top_left_y);
  points[1].x = WXROUND(top_right_x); points[1].y = WXROUND(top_right_y);
  points[2].x = WXROUND(bottom_right_x); points[2].y = WXROUND(bottom_right_y);
  points[3].x = WXROUND(bottom_left_x); points[3].y = WXROUND(bottom_left_y);
  points[4].x = WXROUND(top_left_x); points[4].y = WXROUND(top_left_y);

  dc.DrawLines(5, points);
}

void wxShape::Attach(wxShapeCanvas *can)
{
  m_canvas = can;
}

void wxShape::Detach()
{
  m_canvas = NULL;
}

void wxShape::Move(wxDC& dc, double x, double y, bool display)
{
  double old_x = m_xpos;
  double old_y = m_ypos;

  if (!GetEventHandler()->OnMovePre(dc, x, y, old_x, old_y, display))
  {
//    m_xpos = old_x;
//    m_ypos = old_y;
    return;
  }

  m_xpos = x; m_ypos = y;

  ResetControlPoints();

  if (display)
    Draw(dc);

  MoveLinks(dc);

  GetEventHandler()->OnMovePost(dc, x, y, old_x, old_y, display);
}

void wxShape::MoveLinks(wxDC& dc)
{
  GetEventHandler()->OnMoveLinks(dc);
}


void wxShape::Draw(wxDC& dc)
{
  if (m_visible)
  {
    GetEventHandler()->OnDraw(dc);
    GetEventHandler()->OnDrawContents(dc);
    GetEventHandler()->OnDrawControlPoints(dc);
    GetEventHandler()->OnDrawBranches(dc);
  }
}

void wxShape::Flash()
{
    if (GetCanvas())
    {
        wxClientDC dc(GetCanvas());
        GetCanvas()->PrepareDC(dc);

        dc.SetLogicalFunction(OGLRBLF);
        Draw(dc);
        dc.SetLogicalFunction(wxCOPY);
        Draw(dc);
    }
}

void wxShape::Show(bool show)
{
  m_visible = show;
  wxNode *node = m_children.GetFirst();
  while (node)
  {
    wxShape *image = (wxShape *)node->GetData();
    image->Show(show);
    node = node->GetNext();
  }
}

void wxShape::Erase(wxDC& dc)
{
  GetEventHandler()->OnErase(dc);
  GetEventHandler()->OnEraseControlPoints(dc);
  GetEventHandler()->OnDrawBranches(dc, true);
}

void wxShape::EraseContents(wxDC& dc)
{
  GetEventHandler()->OnEraseContents(dc);
}

void wxShape::AddText(const wxString& string)
{
  wxNode *node = m_regions.GetFirst();
  if (!node)
    return;
  wxShapeRegion *region = (wxShapeRegion *)node->GetData();
  region->ClearText();
  wxShapeTextLine *new_line =
      new wxShapeTextLine(0.0, 0.0, string);
  region->GetFormattedText().Append(new_line);

  m_formatted = false;
}

void wxShape::SetSize(double x, double y, bool WXUNUSED(recursive))
{
  SetAttachmentSize(x, y);
  SetDefaultRegionSize();
}

void wxShape::SetAttachmentSize(double w, double h)
{
  double scaleX;
  double scaleY;
  double width, height;
  GetBoundingBoxMin(&width, &height);
  if (width == 0.0)
    scaleX = 1.0;
  else scaleX = w/width;
  if (height == 0.0)
    scaleY = 1.0;
  else scaleY = h/height;

  wxNode *node = m_attachmentPoints.GetFirst();
  while (node)
  {
    wxAttachmentPoint *point = (wxAttachmentPoint *)node->GetData();
    point->m_x = (double)(point->m_x * scaleX);
    point->m_y = (double)(point->m_y * scaleY);
    node = node->GetNext();
  }
}

// Add line FROM this object
void wxShape::AddLine(wxLineShape *line, wxShape *other,
                            int attachFrom, int attachTo,
                            // The line ordering
                            int positionFrom, int positionTo)
{
    if (positionFrom == -1)
    {
        if (!m_lines.Member(line))
            m_lines.Append(line);
    }
    else
    {
        // Don't preserve old ordering if we have new ordering instructions
        m_lines.DeleteObject(line);
        if (positionFrom < (int) m_lines.GetCount())
        {
            wxNode* node = m_lines.Item(positionFrom);
            m_lines.Insert(node, line);
        }
        else
            m_lines.Append(line);
    }

    if (positionTo == -1)
    {
        if (!other->m_lines.Member(line))
            other->m_lines.Append(line);
    }
    else
    {
        // Don't preserve old ordering if we have new ordering instructions
        other->m_lines.DeleteObject(line);
        if (positionTo < (int) other->m_lines.GetCount())
        {
            wxNode* node = other->m_lines.Item(positionTo);
            other->m_lines.Insert(node, line);
        }
        else
            other->m_lines.Append(line);
    }
#if 0
    // Wrong: doesn't preserve ordering of shape already linked
    m_lines.DeleteObject(line);
    other->m_lines.DeleteObject(line);

    if (positionFrom == -1)
        m_lines.Append(line);
    else
    {
        if (positionFrom < m_lines.GetCount())
        {
            wxNode* node = m_lines.Item(positionFrom);
            m_lines.Insert(node, line);
        }
        else
            m_lines.Append(line);
    }

    if (positionTo == -1)
        other->m_lines.Append(line);
    else
    {
        if (positionTo < other->m_lines.GetCount())
        {
            wxNode* node = other->m_lines.Item(positionTo);
            other->m_lines.Insert(node, line);
        }
        else
            other->m_lines.Append(line);
    }
#endif

    line->SetFrom(this);
    line->SetTo(other);
    line->SetAttachments(attachFrom, attachTo);
}

void wxShape::RemoveLine(wxLineShape *line)
{
  if (line->GetFrom() == this)
    line->GetTo()->m_lines.DeleteObject(line);
  else
   line->GetFrom()->m_lines.DeleteObject(line);

  m_lines.DeleteObject(line);
}

#if wxUSE_PROLOGIO
void wxShape::WriteAttributes(wxExpr *clause)
{
  clause->AddAttributeValueString(_T("type"), GetClassInfo()->GetClassName());
  clause->AddAttributeValue(_T("id"), m_id);

  if (m_pen)
  {
    int penWidth = m_pen->GetWidth();
    int penStyle = m_pen->GetStyle();
    if (penWidth != 1)
      clause->AddAttributeValue(_T("pen_width"), (long)penWidth);
    if (penStyle != wxSOLID)
      clause->AddAttributeValue(_T("pen_style"), (long)penStyle);

    wxString penColour = wxTheColourDatabase->FindName(m_pen->GetColour());
    if (penColour == wxEmptyString)
    {
      wxString hex(oglColourToHex(m_pen->GetColour()));
      hex = wxString(_T("#")) + hex;
      clause->AddAttributeValueString(_T("pen_colour"), hex);
    }
    else if (penColour != _T("BLACK"))
      clause->AddAttributeValueString(_T("pen_colour"), penColour);
  }

  if (m_brush)
  {
    wxString brushColour = wxTheColourDatabase->FindName(m_brush->GetColour());

    if (brushColour == wxEmptyString)
    {
      wxString hex(oglColourToHex(m_brush->GetColour()));
      hex = wxString(_T("#")) + hex;
      clause->AddAttributeValueString(_T("brush_colour"), hex);
    }
    else if (brushColour != _T("WHITE"))
      clause->AddAttributeValueString(_T("brush_colour"), brushColour);

    if (m_brush->GetStyle() != wxSOLID)
      clause->AddAttributeValue(_T("brush_style"), (long)m_brush->GetStyle());
  }

  // Output line ids

  int n_lines = m_lines.GetCount();
  if (n_lines > 0)
  {
    wxExpr *list = new wxExpr(wxExprList);
    wxNode *node = m_lines.GetFirst();
    while (node)
    {
      wxShape *line = (wxShape *)node->GetData();
      wxExpr *id_expr = new wxExpr(line->GetId());
      list->Append(id_expr);
      node = node->GetNext();
    }
    clause->AddAttributeValue(_T("arcs"), list);
  }

  // Miscellaneous members
  if (m_attachmentMode != 0)
    clause->AddAttributeValue(_T("use_attachments"), (long)m_attachmentMode);
  if (m_sensitivity != OP_ALL)
    clause->AddAttributeValue(_T("sensitivity"), (long)m_sensitivity);
  if (!m_spaceAttachments)
    clause->AddAttributeValue(_T("space_attachments"), (long)m_spaceAttachments);
  if (m_fixedWidth)
    clause->AddAttributeValue(_T("fixed_width"), (long)m_fixedWidth);
  if (m_fixedHeight)
    clause->AddAttributeValue(_T("fixed_height"), (long)m_fixedHeight);
  if (m_shadowMode != SHADOW_NONE)
    clause->AddAttributeValue(_T("shadow_mode"), (long)m_shadowMode);
  if (m_centreResize != true)
    clause->AddAttributeValue(_T("centre_resize"), (long)0);
  clause->AddAttributeValue(_T("maintain_aspect_ratio"), (long) m_maintainAspectRatio);
  if (m_highlighted != false)
    clause->AddAttributeValue(_T("hilite"), (long)m_highlighted);

  if (m_parent) // For composite objects
    clause->AddAttributeValue(_T("parent"), (long)m_parent->GetId());

  if (m_rotation != 0.0)
    clause->AddAttributeValue(_T("rotation"), m_rotation);

  if (!this->IsKindOf(CLASSINFO(wxLineShape)))
  {
    clause->AddAttributeValue(_T("neck_length"), (long) m_branchNeckLength);
    clause->AddAttributeValue(_T("stem_length"), (long) m_branchStemLength);
    clause->AddAttributeValue(_T("branch_spacing"), (long) m_branchSpacing);
    clause->AddAttributeValue(_T("branch_style"), (long) m_branchStyle);
  }

  // Write user-defined attachment points, if any
  if (m_attachmentPoints.GetCount() > 0)
  {
    wxExpr *attachmentList = new wxExpr(wxExprList);
    wxNode *node = m_attachmentPoints.GetFirst();
    while (node)
    {
      wxAttachmentPoint *point = (wxAttachmentPoint *)node->GetData();
      wxExpr *pointExpr = new wxExpr(wxExprList);
      pointExpr->Append(new wxExpr((long)point->m_id));
      pointExpr->Append(new wxExpr(point->m_x));
      pointExpr->Append(new wxExpr(point->m_y));
      attachmentList->Append(pointExpr);
      node = node->GetNext();
    }
    clause->AddAttributeValue(_T("user_attachments"), attachmentList);
  }

  // Write text regions
  WriteRegions(clause);
}

void wxShape::WriteRegions(wxExpr *clause)
{
  // Output regions as region1 = (...), region2 = (...), etc
  // and formatted text as text1 = (...), text2 = (...) etc.
  int regionNo = 1;
  wxChar regionNameBuf[20];
  wxChar textNameBuf[20];
  wxNode *node = m_regions.GetFirst();
  while (node)
  {
    wxShapeRegion *region = (wxShapeRegion *)node->GetData();
    wxSprintf(regionNameBuf, _T("region%d"), regionNo);
    wxSprintf(textNameBuf, _T("text%d"), regionNo);

    // Original text and region attributes:
    // region1 = (regionName regionText x y width height minWidth minHeight proportionX proportionY
    //            formatMode fontSize fontFamily fontStyle fontWeight textColour)
    wxExpr *regionExpr = new wxExpr(wxExprList);
    regionExpr->Append(new wxExpr(wxExprString, region->m_regionName));
    regionExpr->Append(new wxExpr(wxExprString, region->m_regionText));

    regionExpr->Append(new wxExpr(region->m_x));
    regionExpr->Append(new wxExpr(region->m_y));
    regionExpr->Append(new wxExpr(region->GetWidth()));
    regionExpr->Append(new wxExpr(region->GetHeight()));

    regionExpr->Append(new wxExpr(region->m_minWidth));
    regionExpr->Append(new wxExpr(region->m_minHeight));
    regionExpr->Append(new wxExpr(region->m_regionProportionX));
    regionExpr->Append(new wxExpr(region->m_regionProportionY));

    regionExpr->Append(new wxExpr((long)region->m_formatMode));

    regionExpr->Append(new wxExpr((long)(region->m_font ? region->m_font->GetPointSize() : 10)));
    regionExpr->Append(new wxExpr((long)(region->m_font ? region->m_font->GetFamily() : wxDEFAULT)));
    regionExpr->Append(new wxExpr((long)(region->m_font ? region->m_font->GetStyle() : wxDEFAULT)));
    regionExpr->Append(new wxExpr((long)(region->m_font ? region->m_font->GetWeight() : wxNORMAL)));
    regionExpr->Append(new wxExpr(wxExprString, region->m_textColour));

    // New members for pen colour/style
    regionExpr->Append(new wxExpr(wxExprString, region->m_penColour));
    regionExpr->Append(new wxExpr((long)region->m_penStyle));

    // Formatted text:
    // text1 = ((x y string) (x y string) ...)
    wxExpr *textExpr = new wxExpr(wxExprList);

    wxNode *textNode = region->m_formattedText.GetFirst();
    while (textNode)
    {
      wxShapeTextLine *line = (wxShapeTextLine *)textNode->GetData();
      wxExpr *list2 = new wxExpr(wxExprList);
      list2->Append(new wxExpr(line->GetX()));
      list2->Append(new wxExpr(line->GetY()));
      list2->Append(new wxExpr(wxExprString, line->GetText()));
      textExpr->Append(list2);
      textNode = textNode->GetNext();
    }

    // Now add both attributes to the clause
    clause->AddAttributeValue(regionNameBuf, regionExpr);
    clause->AddAttributeValue(textNameBuf, textExpr);

    node = node->GetNext();
    regionNo ++;
  }
}

void wxShape::ReadAttributes(wxExpr *clause)
{
  clause->GetAttributeValue(_T("id"), m_id);
  wxRegisterId(m_id);

  clause->GetAttributeValue(_T("x"), m_xpos);
  clause->GetAttributeValue(_T("y"), m_ypos);

  // Input text strings (FOR COMPATIBILITY WITH OLD FILES ONLY. SEE REGION CODE BELOW.)
  ClearText();
  wxExpr *strings = clause->AttributeValue(_T("text"));
  if (strings && strings->Type() == wxExprList)
  {
    m_formatted = true;  // Assume text is formatted unless we prove otherwise
    wxExpr *node = strings->value.first;
    while (node)
    {
      wxExpr *string_expr = node;
      double the_x = 0.0;
      double the_y = 0.0;
      wxString the_string = wxEmptyString;

      // string_expr can either be a string, or a list of
      // 3 elements: x, y, and string.
      if (string_expr->Type() == wxExprString)
      {
        the_string = string_expr->StringValue();
        m_formatted = false;
      }
      else if (string_expr->Type() == wxExprList)
      {
        wxExpr *first = string_expr->value.first;
        wxExpr *second = first ? first->next : (wxExpr*) NULL;
        wxExpr *third = second ? second->next : (wxExpr*) NULL;

        if (first && second && third &&
            (first->Type() == wxExprReal || first->Type() == wxExprInteger) &&
            (second->Type() == wxExprReal || second->Type() == wxExprInteger) &&
            third->Type() == wxExprString)
          {
            if (first->Type() == wxExprReal)
              the_x = first->RealValue();
            else the_x = (double)first->IntegerValue();

            if (second->Type() == wxExprReal)
              the_y = second->RealValue();
            else the_y = (double)second->IntegerValue();

            the_string = third->StringValue();
          }
      }
      wxShapeTextLine *line =
            new wxShapeTextLine(the_x, the_y, the_string);
      m_text.Append(line);

      node = node->next;
    }
  }

  wxString pen_string = wxEmptyString;
  wxString brush_string = wxEmptyString;
  int pen_width = 1;
  int pen_style = wxSOLID;
  int brush_style = wxSOLID;
  m_attachmentMode = ATTACHMENT_MODE_NONE;

  clause->GetAttributeValue(_T("pen_colour"), pen_string);
  clause->GetAttributeValue(_T("text_colour"), m_textColourName);

  SetTextColour(m_textColourName);

  clause->GetAttributeValue(_T("region_name"), m_regionName);

  clause->GetAttributeValue(_T("brush_colour"), brush_string);
  clause->GetAttributeValue(_T("pen_width"), pen_width);
  clause->GetAttributeValue(_T("pen_style"), pen_style);
  clause->GetAttributeValue(_T("brush_style"), brush_style);

  int iVal = (int) m_attachmentMode;
  clause->GetAttributeValue(_T("use_attachments"), iVal);
  m_attachmentMode = iVal;

  clause->GetAttributeValue(_T("sensitivity"), m_sensitivity);

  iVal = (int) m_spaceAttachments;
  clause->GetAttributeValue(_T("space_attachments"), iVal);
  m_spaceAttachments = (iVal != 0);

  iVal = (int) m_fixedWidth;
  clause->GetAttributeValue(_T("fixed_width"), iVal);
  m_fixedWidth = (iVal != 0);

  iVal = (int) m_fixedHeight;
  clause->GetAttributeValue(_T("fixed_height"), iVal);
  m_fixedHeight = (iVal != 0);

  clause->GetAttributeValue(_T("format_mode"), m_formatMode);
  clause->GetAttributeValue(_T("shadow_mode"), m_shadowMode);

  iVal = m_branchNeckLength;
  clause->GetAttributeValue(_T("neck_length"), iVal);
  m_branchNeckLength = iVal;

  iVal = m_branchStemLength;
  clause->GetAttributeValue(_T("stem_length"), iVal);
  m_branchStemLength = iVal;

  iVal = m_branchSpacing;
  clause->GetAttributeValue(_T("branch_spacing"), iVal);
  m_branchSpacing = iVal;

  clause->GetAttributeValue(_T("branch_style"), m_branchStyle);

  iVal = (int) m_centreResize;
  clause->GetAttributeValue(_T("centre_resize"), iVal);
  m_centreResize = (iVal != 0);

  iVal = (int) m_maintainAspectRatio;
  clause->GetAttributeValue(_T("maintain_aspect_ratio"), iVal);
  m_maintainAspectRatio = (iVal != 0);

  iVal = (int) m_highlighted;
  clause->GetAttributeValue(_T("hilite"), iVal);
  m_highlighted = (iVal != 0);

  clause->GetAttributeValue(_T("rotation"), m_rotation);

  if (pen_string == wxEmptyString)
    pen_string = _T("BLACK");
  if (brush_string == wxEmptyString)
    brush_string = _T("WHITE");

  if (pen_string.GetChar(0) == '#')
  {
    wxColour col(oglHexToColour(pen_string.After('#')));
    m_pen = wxThePenList->FindOrCreatePen(col, pen_width, pen_style);
  }
  else
    m_pen = wxThePenList->FindOrCreatePen(pen_string, pen_width, pen_style);

  if (!m_pen)
    m_pen = wxBLACK_PEN;

  if (brush_string.GetChar(0) == '#')
  {
    wxColour col(oglHexToColour(brush_string.After('#')));
    m_brush = wxTheBrushList->FindOrCreateBrush(col, brush_style);
  }
  else
    m_brush = wxTheBrushList->FindOrCreateBrush(brush_string, brush_style);

  if (!m_brush)
    m_brush = wxWHITE_BRUSH;

  int point_size = 10;
  clause->GetAttributeValue(_T("point_size"), point_size);
  SetFont(oglMatchFont(point_size));

  // Read user-defined attachment points, if any
  wxExpr *attachmentList = clause->AttributeValue(_T("user_attachments"));
  if (attachmentList)
  {
    wxExpr *pointExpr = attachmentList->GetFirst();
    while (pointExpr)
    {
      wxExpr *idExpr = pointExpr->Nth(0);
      wxExpr *xExpr = pointExpr->Nth(1);
      wxExpr *yExpr = pointExpr->Nth(2);
      if (idExpr && xExpr && yExpr)
      {
        wxAttachmentPoint *point = new wxAttachmentPoint;
        point->m_id = (int)idExpr->IntegerValue();
        point->m_x = xExpr->RealValue();
        point->m_y = yExpr->RealValue();
        m_attachmentPoints.Append((wxObject *)point);
      }
      pointExpr = pointExpr->GetNext();
    }
  }

  // Read text regions
  ReadRegions(clause);
}

void wxShape::ReadRegions(wxExpr *clause)
{
  ClearRegions();

  // region1 = (regionName regionText x y width height minWidth minHeight proportionX proportionY
  //            formatMode fontSize fontFamily fontStyle fontWeight textColour)
  int regionNo = 1;
  wxChar regionNameBuf[20];
  wxChar textNameBuf[20];

  wxExpr *regionExpr;
  wxExpr *textExpr = NULL;
  wxSprintf(regionNameBuf, _T("region%d"), regionNo);
  wxSprintf(textNameBuf, _T("text%d"), regionNo);

  m_formatted = true;  // Assume text is formatted unless we prove otherwise

  while ((regionExpr = clause->AttributeValue(regionNameBuf)) != NULL)
  {
    /*
     * Get the region information
     *
     */

    wxString regionName = wxEmptyString;
    wxString regionText = wxEmptyString;
    double x = 0.0;
    double y = 0.0;
    double width = 0.0;
    double height = 0.0;
    double minWidth = 5.0;
    double minHeight = 5.0;
    double m_regionProportionX = -1.0;
    double m_regionProportionY = -1.0;
    int formatMode = FORMAT_NONE;
    int fontSize = 10;
    int fontFamily = wxSWISS;
    int fontStyle = wxNORMAL;
    int fontWeight = wxNORMAL;
    wxString regionTextColour = wxEmptyString;
    wxString penColour = wxEmptyString;
    int penStyle = wxSOLID;

    if (regionExpr->Type() == wxExprList)
    {
      wxExpr *nameExpr = regionExpr->Nth(0);
      wxExpr *textExpr = regionExpr->Nth(1);
      wxExpr *xExpr = regionExpr->Nth(2);
      wxExpr *yExpr = regionExpr->Nth(3);
      wxExpr *widthExpr = regionExpr->Nth(4);
      wxExpr *heightExpr = regionExpr->Nth(5);
      wxExpr *minWidthExpr = regionExpr->Nth(6);
      wxExpr *minHeightExpr = regionExpr->Nth(7);
      wxExpr *propXExpr = regionExpr->Nth(8);
      wxExpr *propYExpr = regionExpr->Nth(9);
      wxExpr *formatExpr = regionExpr->Nth(10);
      wxExpr *sizeExpr = regionExpr->Nth(11);
      wxExpr *familyExpr = regionExpr->Nth(12);
      wxExpr *styleExpr = regionExpr->Nth(13);
      wxExpr *weightExpr = regionExpr->Nth(14);
      wxExpr *colourExpr = regionExpr->Nth(15);
      wxExpr *penColourExpr = regionExpr->Nth(16);
      wxExpr *penStyleExpr = regionExpr->Nth(17);

      regionName = nameExpr->StringValue();
      regionText = textExpr->StringValue();

      x = xExpr->RealValue();
      y = yExpr->RealValue();

      width = widthExpr->RealValue();
      height = heightExpr->RealValue();

      minWidth = minWidthExpr->RealValue();
      minHeight = minHeightExpr->RealValue();

      m_regionProportionX = propXExpr->RealValue();
      m_regionProportionY = propYExpr->RealValue();

      formatMode = (int) formatExpr->IntegerValue();
      fontSize = (int)sizeExpr->IntegerValue();
      fontFamily = (int)familyExpr->IntegerValue();
      fontStyle = (int)styleExpr->IntegerValue();
      fontWeight = (int)weightExpr->IntegerValue();

      if (colourExpr)
      {
        regionTextColour = colourExpr->StringValue();
      }
      else
        regionTextColour = _T("BLACK");

      if (penColourExpr)
        penColour = penColourExpr->StringValue();
      if (penStyleExpr)
        penStyle = (int)penStyleExpr->IntegerValue();
    }
    wxFont *font = wxTheFontList->FindOrCreateFont(fontSize, fontFamily, fontStyle, fontWeight);

    wxShapeRegion *region = new wxShapeRegion;
    region->SetProportions(m_regionProportionX, m_regionProportionY);
    region->SetFont(font);
    region->SetSize(width, height);
    region->SetPosition(x, y);
    region->SetMinSize(minWidth, minHeight);
    region->SetFormatMode(formatMode);
    region->SetPenStyle(penStyle);
    if (penColour != wxEmptyString)
      region->SetPenColour(penColour);

    region->m_textColour = regionTextColour;
    region->m_regionText = regionText;
    region->m_regionName = regionName;

    m_regions.Append(region);

    /*
     * Get the formatted text strings
     *
     */
    textExpr = clause->AttributeValue(textNameBuf);
    if (textExpr && (textExpr->Type() == wxExprList))
    {
      wxExpr *node = textExpr->value.first;
      while (node)
      {
        wxExpr *string_expr = node;
        double the_x = 0.0;
        double the_y = 0.0;
        wxString the_string = wxEmptyString;

        // string_expr can either be a string, or a list of
        // 3 elements: x, y, and string.
        if (string_expr->Type() == wxExprString)
        {
          the_string = string_expr->StringValue();
          m_formatted = false;
        }
        else if (string_expr->Type() == wxExprList)
        {
          wxExpr *first = string_expr->value.first;
          wxExpr *second = first ? first->next : (wxExpr*) NULL;
          wxExpr *third = second ? second->next : (wxExpr*) NULL;

          if (first && second && third &&
              (first->Type() == wxExprReal || first->Type() == wxExprInteger) &&
              (second->Type() == wxExprReal || second->Type() == wxExprInteger) &&
              third->Type() == wxExprString)
          {
            if (first->Type() == wxExprReal)
              the_x = first->RealValue();
            else the_x = (double)first->IntegerValue();

            if (second->Type() == wxExprReal)
              the_y = second->RealValue();
            else the_y = (double)second->IntegerValue();

            the_string = third->StringValue();
          }
        }
        if (the_string)
        {
          wxShapeTextLine *line =
              new wxShapeTextLine(the_x, the_y, the_string);
          region->m_formattedText.Append(line);
        }
        node = node->next;
      }
    }

    regionNo ++;
    wxSprintf(regionNameBuf, _T("region%d"), regionNo);
    wxSprintf(textNameBuf, _T("text%d"), regionNo);
  }

  // Compatibility: check for no regions (old file).
  // Lines and divided rectangles must deal with this compatibility
  // theirselves. Composites _may_ not have any regions anyway.
  if ((m_regions.GetCount() == 0) &&
      !this->IsKindOf(CLASSINFO(wxLineShape)) && !this->IsKindOf(CLASSINFO(wxDividedShape)) &&
      !this->IsKindOf(CLASSINFO(wxCompositeShape)))
  {
    wxShapeRegion *newRegion = new wxShapeRegion;
    newRegion->SetName(_T("0"));
    m_regions.Append((wxObject *)newRegion);
    if (m_text.GetCount() > 0)
    {
      newRegion->ClearText();
      wxNode *node = m_text.GetFirst();
      while (node)
      {
        wxShapeTextLine *textLine = (wxShapeTextLine *)node->GetData();
        wxNode *next = node->GetNext();
        newRegion->GetFormattedText().Append((wxObject *)textLine);
        delete node;
        node = next;
      }
    }
  }
}

#endif

void wxShape::Copy(wxShape& copy)
{
  copy.m_id = m_id;
  copy.m_xpos = m_xpos;
  copy.m_ypos = m_ypos;
  copy.m_pen = m_pen;
  copy.m_brush = m_brush;
  copy.m_textColour = m_textColour;
  copy.m_centreResize = m_centreResize;
  copy.m_maintainAspectRatio = m_maintainAspectRatio;
  copy.m_attachmentMode = m_attachmentMode;
  copy.m_spaceAttachments = m_spaceAttachments;
  copy.m_highlighted = m_highlighted;
  copy.m_rotation = m_rotation;
  copy.m_textColourName = m_textColourName;
  copy.m_regionName = m_regionName;

  copy.m_sensitivity = m_sensitivity;
  copy.m_draggable = m_draggable;
  copy.m_fixedWidth = m_fixedWidth;
  copy.m_fixedHeight = m_fixedHeight;
  copy.m_formatMode = m_formatMode;
  copy.m_drawHandles = m_drawHandles;

  copy.m_visible = m_visible;
  copy.m_shadowMode = m_shadowMode;
  copy.m_shadowOffsetX = m_shadowOffsetX;
  copy.m_shadowOffsetY = m_shadowOffsetY;
  copy.m_shadowBrush = m_shadowBrush;

  copy.m_branchNeckLength = m_branchNeckLength;
  copy.m_branchStemLength = m_branchStemLength;
  copy.m_branchSpacing = m_branchSpacing;

  // Copy text regions
  copy.ClearRegions();
  wxNode *node = m_regions.GetFirst();
  while (node)
  {
    wxShapeRegion *region = (wxShapeRegion *)node->GetData();
    wxShapeRegion *newRegion = new wxShapeRegion(*region);
    copy.m_regions.Append(newRegion);
    node = node->GetNext();
  }

  // Copy attachments
  copy.ClearAttachments();
  node = m_attachmentPoints.GetFirst();
  while (node)
  {
    wxAttachmentPoint *point = (wxAttachmentPoint *)node->GetData();
    wxAttachmentPoint *newPoint = new wxAttachmentPoint;
    newPoint->m_id = point->m_id;
    newPoint->m_x = point->m_x;
    newPoint->m_y = point->m_y;
    copy.m_attachmentPoints.Append((wxObject *)newPoint);
    node = node->GetNext();
  }

  // Copy lines
  copy.m_lines.Clear();
  node = m_lines.GetFirst();
  while (node)
  {
    wxLineShape* line = (wxLineShape*) node->GetData();
    copy.m_lines.Append(line);
    node = node->GetNext();
  }
}

// Create and return a new, fully copied object.
wxShape *wxShape::CreateNewCopy(bool resetMapping, bool recompute)
{
  if (resetMapping)
    oglObjectCopyMapping.Clear();

  wxShape* newObject = (wxShape*) GetClassInfo()->CreateObject();

  wxASSERT( (newObject != NULL) );
  wxASSERT( (newObject->IsKindOf(CLASSINFO(wxShape))) );

  Copy(*newObject);

  if (GetEventHandler() != this)
  {
    wxShapeEvtHandler* newHandler = GetEventHandler()->CreateNewCopy();
    newObject->SetEventHandler(newHandler);
    newObject->SetPreviousHandler(NULL);
    newHandler->SetPreviousHandler(newObject);
    newHandler->SetShape(newObject);
  }

  if (recompute)
    newObject->Recompute();
  return newObject;
}

// Does the copying for this object, including copying event
// handler data if any. Calls the virtual Copy function.
void wxShape::CopyWithHandler(wxShape& copy)
{
    Copy(copy);

    if (GetEventHandler() != this)
    {
        wxASSERT( copy.GetEventHandler() != NULL );
        wxASSERT( copy.GetEventHandler() != (&copy) );
        wxASSERT( GetEventHandler()->GetClassInfo() == copy.GetEventHandler()->GetClassInfo() );
        GetEventHandler()->CopyData(* (copy.GetEventHandler()));
    }
}


// Default - make 6 control points
void wxShape::MakeControlPoints()
{
  double maxX, maxY, minX, minY;

  GetBoundingBoxMax(&maxX, &maxY);
  GetBoundingBoxMin(&minX, &minY);

  double widthMin = (double)(minX + CONTROL_POINT_SIZE + 2);
  double heightMin = (double)(minY + CONTROL_POINT_SIZE + 2);

  // Offsets from main object
  double top = (double)(- (heightMin / 2.0));
  double bottom = (double)(heightMin / 2.0 + (maxY - minY));
  double left = (double)(- (widthMin / 2.0));
  double right = (double)(widthMin / 2.0 + (maxX - minX));

  wxControlPoint *control = new wxControlPoint(m_canvas, this, CONTROL_POINT_SIZE, left, top,
                                           CONTROL_POINT_DIAGONAL);
  m_canvas->AddShape(control);
  m_controlPoints.Append(control);

  control = new wxControlPoint(m_canvas, this, CONTROL_POINT_SIZE, 0, top,
                                           CONTROL_POINT_VERTICAL);
  m_canvas->AddShape(control);
  m_controlPoints.Append(control);

  control = new wxControlPoint(m_canvas, this, CONTROL_POINT_SIZE, right, top,
                                           CONTROL_POINT_DIAGONAL);
  m_canvas->AddShape(control);
  m_controlPoints.Append(control);

  control = new wxControlPoint(m_canvas, this, CONTROL_POINT_SIZE, right, 0,
                                           CONTROL_POINT_HORIZONTAL);
  m_canvas->AddShape(control);
  m_controlPoints.Append(control);

  control = new wxControlPoint(m_canvas, this, CONTROL_POINT_SIZE, right, bottom,
                                           CONTROL_POINT_DIAGONAL);
  m_canvas->AddShape(control);
  m_controlPoints.Append(control);

  control = new wxControlPoint(m_canvas, this, CONTROL_POINT_SIZE, 0, bottom,
                                           CONTROL_POINT_VERTICAL);
  m_canvas->AddShape(control);
  m_controlPoints.Append(control);

  control = new wxControlPoint(m_canvas, this, CONTROL_POINT_SIZE, left, bottom,
                                           CONTROL_POINT_DIAGONAL);
  m_canvas->AddShape(control);
  m_controlPoints.Append(control);

  control = new wxControlPoint(m_canvas, this, CONTROL_POINT_SIZE, left, 0,
                                           CONTROL_POINT_HORIZONTAL);
  m_canvas->AddShape(control);
  m_controlPoints.Append(control);

}

void wxShape::MakeMandatoryControlPoints()
{
  wxNode *node = m_children.GetFirst();
  while (node)
  {
    wxShape *child = (wxShape *)node->GetData();
    child->MakeMandatoryControlPoints();
    node = node->GetNext();
  }
}

void wxShape::ResetMandatoryControlPoints()
{
  wxNode *node = m_children.GetFirst();
  while (node)
  {
    wxShape *child = (wxShape *)node->GetData();
    child->ResetMandatoryControlPoints();
    node = node->GetNext();
  }
}

void wxShape::ResetControlPoints()
{
  ResetMandatoryControlPoints();

  if (m_controlPoints.GetCount() < 1)
    return;

  double maxX, maxY, minX, minY;

  GetBoundingBoxMax(&maxX, &maxY);
  GetBoundingBoxMin(&minX, &minY);

  double widthMin = (double)(minX + CONTROL_POINT_SIZE + 2);
  double heightMin = (double)(minY + CONTROL_POINT_SIZE + 2);

  // Offsets from main object
  double top = (double)(- (heightMin / 2.0));
  double bottom = (double)(heightMin / 2.0 + (maxY - minY));
  double left = (double)(- (widthMin / 2.0));
  double right = (double)(widthMin / 2.0 + (maxX - minX));

  wxNode *node = m_controlPoints.GetFirst();
  wxControlPoint *control = (wxControlPoint *)node->GetData();
  control->m_xoffset = left; control->m_yoffset = top;

  node = node->GetNext(); control = (wxControlPoint *)node->GetData();
  control->m_xoffset = 0; control->m_yoffset = top;

  node = node->GetNext(); control = (wxControlPoint *)node->GetData();
  control->m_xoffset = right; control->m_yoffset = top;

  node = node->GetNext(); control = (wxControlPoint *)node->GetData();
  control->m_xoffset = right; control->m_yoffset = 0;

  node = node->GetNext(); control = (wxControlPoint *)node->GetData();
  control->m_xoffset = right; control->m_yoffset = bottom;

  node = node->GetNext(); control = (wxControlPoint *)node->GetData();
  control->m_xoffset = 0; control->m_yoffset = bottom;

  node = node->GetNext(); control = (wxControlPoint *)node->GetData();
  control->m_xoffset = left; control->m_yoffset = bottom;

  node = node->GetNext(); control = (wxControlPoint *)node->GetData();
  control->m_xoffset = left; control->m_yoffset = 0;
}

void wxShape::DeleteControlPoints(wxDC *dc)
{
  wxNode *node = m_controlPoints.GetFirst();
  while (node)
  {
    wxControlPoint *control = (wxControlPoint *)node->GetData();
    if (dc)
        control->GetEventHandler()->OnErase(*dc);
    m_canvas->RemoveShape(control);
    delete control;
    delete node;
    node = m_controlPoints.GetFirst();
  }
  // Children of divisions are contained objects,
  // so stop here
  if (!IsKindOf(CLASSINFO(wxDivisionShape)))
  {
    node = m_children.GetFirst();
    while (node)
    {
      wxShape *child = (wxShape *)node->GetData();
      child->DeleteControlPoints(dc);
      node = node->GetNext();
    }
  }
}

void wxShape::OnDrawControlPoints(wxDC& dc)
{
  if (!m_drawHandles)
    return;

  dc.SetBrush(* wxBLACK_BRUSH);
  dc.SetPen(* wxBLACK_PEN);

  wxNode *node = m_controlPoints.GetFirst();
  while (node)
  {
    wxControlPoint *control = (wxControlPoint *)node->GetData();
    control->Draw(dc);
    node = node->GetNext();
  }
  // Children of divisions are contained objects,
  // so stop here.
  // This test bypasses the type facility for speed
  // (critical when drawing)
  if (!IsKindOf(CLASSINFO(wxDivisionShape)))
  {
    node = m_children.GetFirst();
    while (node)
    {
      wxShape *child = (wxShape *)node->GetData();
      child->GetEventHandler()->OnDrawControlPoints(dc);
      node = node->GetNext();
    }
  }
}

void wxShape::OnEraseControlPoints(wxDC& dc)
{
  wxNode *node = m_controlPoints.GetFirst();
  while (node)
  {
    wxControlPoint *control = (wxControlPoint *)node->GetData();
    control->Erase(dc);
    node = node->GetNext();
  }
  if (!IsKindOf(CLASSINFO(wxDivisionShape)))
  {
    node = m_children.GetFirst();
    while (node)
    {
      wxShape *child = (wxShape *)node->GetData();
      child->GetEventHandler()->OnEraseControlPoints(dc);
      node = node->GetNext();
    }
  }
}

void wxShape::Select(bool select, wxDC* dc)
{
  m_selected = select;
  if (select)
  {
    MakeControlPoints();
    // Children of divisions are contained objects,
    // so stop here
    if (!IsKindOf(CLASSINFO(wxDivisionShape)))
    {
      wxNode *node = m_children.GetFirst();
      while (node)
      {
        wxShape *child = (wxShape *)node->GetData();
        child->MakeMandatoryControlPoints();
        node = node->GetNext();
      }
    }
    if (dc)
        GetEventHandler()->OnDrawControlPoints(*dc);
  }
  if (!select)
  {
    DeleteControlPoints(dc);
    if (!IsKindOf(CLASSINFO(wxDivisionShape)))
    {
      wxNode *node = m_children.GetFirst();
      while (node)
      {
        wxShape *child = (wxShape *)node->GetData();
        child->DeleteControlPoints(dc);
        node = node->GetNext();
      }
    }
  }
}

bool wxShape::Selected() const
{
  return m_selected;
}

bool wxShape::AncestorSelected() const
{
  if (m_selected) return true;
  if (!GetParent())
    return false;
  else
    return GetParent()->AncestorSelected();
}

int wxShape::GetNumberOfAttachments() const
{
  // Should return the MAXIMUM attachment point id here,
  // so higher-level functions can iterate through all attachments,
  // even if they're not contiguous.
  if (m_attachmentPoints.GetCount() == 0)
    return 4;
  else
  {
    int maxN = 3;
    wxNode *node = m_attachmentPoints.GetFirst();
    while (node)
    {
      wxAttachmentPoint *point = (wxAttachmentPoint *)node->GetData();
      if (point->m_id > maxN)
        maxN = point->m_id;
      node = node->GetNext();
    }
    return maxN+1;;
  }
}

bool wxShape::AttachmentIsValid(int attachment) const
{
  if (m_attachmentPoints.GetCount() == 0)
  {
    return ((attachment >= 0) && (attachment < 4)) ;
  }

  wxNode *node = m_attachmentPoints.GetFirst();
  while (node)
  {
    wxAttachmentPoint *point = (wxAttachmentPoint *)node->GetData();
    if (point->m_id == attachment)
      return true;
    node = node->GetNext();
  }
  return false;
}

bool wxShape::GetAttachmentPosition(int attachment, double *x, double *y,
                                         int nth, int no_arcs, wxLineShape *line)
{
    if (m_attachmentMode == ATTACHMENT_MODE_NONE)
    {
        *x = m_xpos; *y = m_ypos;
        return true;
    }
    else if (m_attachmentMode == ATTACHMENT_MODE_BRANCHING)
    {
        wxRealPoint pt, stemPt;
        GetBranchingAttachmentPoint(attachment, nth, pt, stemPt);
        *x = pt.x;
        *y = pt.y;
        return true;
    }
    else if (m_attachmentMode == ATTACHMENT_MODE_EDGE)
    {
        if (m_attachmentPoints.GetCount() > 0)
        {
            wxNode *node = m_attachmentPoints.GetFirst();
            while (node)
            {
                wxAttachmentPoint *point = (wxAttachmentPoint *)node->GetData();
                if (point->m_id == attachment)
                {
                    *x = (double)(m_xpos + point->m_x);
                    *y = (double)(m_ypos + point->m_y);
                    return true;
                }
                node = node->GetNext();
            }
            *x = m_xpos; *y = m_ypos;
            return false;
        }
        else
        {
            // Assume is rectangular
            double w, h;
            GetBoundingBoxMax(&w, &h);
            double top = (double)(m_ypos + h/2.0);
            double bottom = (double)(m_ypos - h/2.0);
            double left = (double)(m_xpos - w/2.0);
            double right = (double)(m_xpos + w/2.0);

#if 0
            /* bool isEnd = */ (line && line->IsEnd(this));
#endif

            int physicalAttachment = LogicalToPhysicalAttachment(attachment);

            // Simplified code
            switch (physicalAttachment)
            {
                case 0:
                {
                    wxRealPoint pt = CalcSimpleAttachment(wxRealPoint(left, bottom), wxRealPoint(right, bottom),
                            nth, no_arcs, line);

                    *x = pt.x; *y = pt.y;
                    break;
                }
                case 1:
                {
                    wxRealPoint pt = CalcSimpleAttachment(wxRealPoint(right, bottom), wxRealPoint(right, top),
                            nth, no_arcs, line);

                    *x = pt.x; *y = pt.y;
                    break;
                }
                case 2:
                {
                    wxRealPoint pt = CalcSimpleAttachment(wxRealPoint(left, top), wxRealPoint(right, top),
                            nth, no_arcs, line);

                    *x = pt.x; *y = pt.y;
                    break;
                }
                case 3:
                {
                    wxRealPoint pt = CalcSimpleAttachment(wxRealPoint(left, bottom), wxRealPoint(left, top),
                            nth, no_arcs, line);

                    *x = pt.x; *y = pt.y;
                    break;
                }
                default:
                {
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}

void wxShape::GetBoundingBoxMax(double *w, double *h)
{
  double ww, hh;
  GetBoundingBoxMin(&ww, &hh);
  if (m_shadowMode != SHADOW_NONE)
  {
    ww += m_shadowOffsetX;
    hh += m_shadowOffsetY;
  }
  *w = ww;
  *h = hh;
}

// Returns true if image is a descendant of this composite
bool wxShape::HasDescendant(wxShape *image)
{
  if (image == this)
    return true;
  wxNode *node = m_children.GetFirst();
  while (node)
  {
    wxShape *child = (wxShape *)node->GetData();
    bool ans = child->HasDescendant(image);
    if (ans)
      return true;
    node = node->GetNext();
  }
  return false;
}

// Clears points from a list of wxRealPoints, and clears list
void wxShape::ClearPointList(wxList& list)
{
    wxNode* node = list.GetFirst();
    while (node)
    {
        wxRealPoint* pt = (wxRealPoint*) node->GetData();
        delete pt;

        node = node->GetNext();
    }
    list.Clear();
}

// Assuming the attachment lies along a vertical or horizontal line,
// calculate the position on that point.
wxRealPoint wxShape::CalcSimpleAttachment(const wxRealPoint& pt1, const wxRealPoint& pt2,
    int nth, int noArcs, wxLineShape* line)
{
    bool isEnd = (line && line->IsEnd(this));

    // Are we horizontal or vertical?
    bool isHorizontal = (oglRoughlyEqual(pt1.y, pt2.y) == true);

    double x, y;

    if (isHorizontal)
    {
        wxRealPoint firstPoint, secondPoint;
        if (pt1.x > pt2.x)
        {
            firstPoint = pt2;
            secondPoint = pt1;
        }
        else
        {
            firstPoint = pt1;
            secondPoint = pt2;
        }

        if (m_spaceAttachments)
        {
          if (line && (line->GetAlignmentType(isEnd) == LINE_ALIGNMENT_TO_NEXT_HANDLE))
          {
            // Align line according to the next handle along
            wxRealPoint *point = line->GetNextControlPoint(this);
            if (point->x < firstPoint.x)
              x = firstPoint.x;
            else if (point->x > secondPoint.x)
              x = secondPoint.x;
            else
              x = point->x;
          }
          else
            x = firstPoint.x + (nth + 1)*(secondPoint.x - firstPoint.x)/(noArcs + 1);
        }
        else x = (secondPoint.x - firstPoint.x)/2.0; // Midpoint

        y = pt1.y;
    }
    else
    {
        wxASSERT( oglRoughlyEqual(pt1.x, pt2.x) == true );

        wxRealPoint firstPoint, secondPoint;
        if (pt1.y > pt2.y)
        {
            firstPoint = pt2;
            secondPoint = pt1;
        }
        else
        {
            firstPoint = pt1;
            secondPoint = pt2;
        }

        if (m_spaceAttachments)
        {
          if (line && (line->GetAlignmentType(isEnd) == LINE_ALIGNMENT_TO_NEXT_HANDLE))
          {
            // Align line according to the next handle along
            wxRealPoint *point = line->GetNextControlPoint(this);
            if (point->y < firstPoint.y)
              y = firstPoint.y;
            else if (point->y > secondPoint.y)
              y = secondPoint.y;
            else
              y = point->y;
          }
          else
            y = firstPoint.y + (nth + 1)*(secondPoint.y - firstPoint.y)/(noArcs + 1);
        }
        else y = (secondPoint.y - firstPoint.y)/2.0; // Midpoint

        x = pt1.x;
    }

    return wxRealPoint(x, y);
}

// Return the zero-based position in m_lines of line.
int wxShape::GetLinePosition(wxLineShape* line)
{
    for (size_t i = 0; i < m_lines.GetCount(); i++)
        if ((wxLineShape*) (m_lines.Item(i)->GetData()) == line)
            return i;

    return 0;
}

//
//             |________|
//                 | <- root
//                 | <- neck
// shoulder1 ->---------<- shoulder2
//             | | | | |
//                      <- branching attachment point N-1

// This function gets information about where branching connections go.
// Returns false if there are no lines at this attachment.
bool wxShape::GetBranchingAttachmentInfo(int attachment, wxRealPoint& root, wxRealPoint& neck,
    wxRealPoint& shoulder1, wxRealPoint& shoulder2)
{
    int physicalAttachment = LogicalToPhysicalAttachment(attachment);

    // Number of lines at this attachment.
    int lineCount = GetAttachmentLineCount(attachment);

    if (lineCount == 0)
        return false;

    int totalBranchLength = m_branchSpacing * (lineCount - 1);

    root = GetBranchingAttachmentRoot(attachment);

    // Assume that we have attachment points 0 to 3: top, right, bottom, left.
    switch (physicalAttachment)
    {
        case 0:
        {
            neck.x = GetX();
            neck.y = root.y - m_branchNeckLength;

            shoulder1.x = root.x - (totalBranchLength/2.0) ;
            shoulder2.x = root.x + (totalBranchLength/2.0) ;

            shoulder1.y = neck.y;
            shoulder2.y = neck.y;
            break;
        }
        case 1:
        {
            neck.x = root.x + m_branchNeckLength;
            neck.y = root.y;

            shoulder1.x = neck.x ;
            shoulder2.x = neck.x ;

            shoulder1.y = neck.y - (totalBranchLength/2.0) ;
            shoulder2.y = neck.y + (totalBranchLength/2.0) ;
            break;
        }
        case 2:
        {
            neck.x = GetX();
            neck.y = root.y + m_branchNeckLength;

            shoulder1.x = root.x - (totalBranchLength/2.0) ;
            shoulder2.x = root.x + (totalBranchLength/2.0) ;

            shoulder1.y = neck.y;
            shoulder2.y = neck.y;
            break;
        }
        case 3:
        {
            neck.x = root.x - m_branchNeckLength;
            neck.y = root.y ;

            shoulder1.x = neck.x ;
            shoulder2.x = neck.x ;

            shoulder1.y = neck.y - (totalBranchLength/2.0) ;
            shoulder2.y = neck.y + (totalBranchLength/2.0) ;
            break;
        }
        default:
        {
            wxFAIL_MSG( wxT("Unrecognised attachment point in GetBranchingAttachmentInfo.") );
            break;
        }
    }
    return true;
}

// n is the number of the adjoining line, from 0 to N-1 where N is the number of lines
// at this attachment point.
// Get the attachment point where the arc joins the stem, and also the point where the
// the stem meets the shoulder.
bool wxShape::GetBranchingAttachmentPoint(int attachment, int n, wxRealPoint& pt, wxRealPoint& stemPt)
{
    int physicalAttachment = LogicalToPhysicalAttachment(attachment);

    wxRealPoint root, neck, shoulder1, shoulder2;
    GetBranchingAttachmentInfo(attachment, root, neck, shoulder1, shoulder2);

    // Assume that we have attachment points 0 to 3: top, right, bottom, left.
    switch (physicalAttachment)
    {
        case 0:
        {
            pt.y = neck.y - m_branchStemLength;
            pt.x = shoulder1.x + n*m_branchSpacing;

            stemPt.x = pt.x;
            stemPt.y = neck.y;
            break;
        }
        case 2:
        {
            pt.y = neck.y + m_branchStemLength;
            pt.x = shoulder1.x + n*m_branchSpacing;

            stemPt.x = pt.x;
            stemPt.y = neck.y;
            break;
        }
        case 1:
        {
            pt.x = neck.x + m_branchStemLength;
            pt.y = shoulder1.y + n*m_branchSpacing;

            stemPt.x = neck.x;
            stemPt.y = pt.y;
            break;
        }
        case 3:
        {
            pt.x = neck.x - m_branchStemLength;
            pt.y = shoulder1.y + n*m_branchSpacing;

            stemPt.x = neck.x;
            stemPt.y = pt.y;
            break;
        }
        default:
        {
            wxFAIL_MSG( wxT("Unrecognised attachment point in GetBranchingAttachmentPoint.") );
            break;
        }
    }

    return true;
}

// Get the number of lines at this attachment position.
int wxShape::GetAttachmentLineCount(int attachment) const
{
    int count = 0;
    wxNode* node = m_lines.GetFirst();
    while (node)
    {
        wxLineShape* lineShape = (wxLineShape*) node->GetData();
        if ((lineShape->GetFrom() == this) && (lineShape->GetAttachmentFrom() == attachment))
            count ++;
        else if ((lineShape->GetTo() == this) && (lineShape->GetAttachmentTo() == attachment))
            count ++;

        node = node->GetNext();
    }
    return count;
}

// This function gets the root point at the given attachment.
wxRealPoint wxShape::GetBranchingAttachmentRoot(int attachment)
{
    int physicalAttachment = LogicalToPhysicalAttachment(attachment);

    wxRealPoint root;

    double width, height;
    GetBoundingBoxMax(& width, & height);

    // Assume that we have attachment points 0 to 3: top, right, bottom, left.
    switch (physicalAttachment)
    {
        case 0:
        {
            root.x = GetX() ;
            root.y = GetY() - height/2.0;
            break;
        }
        case 1:
        {
            root.x = GetX() + width/2.0;
            root.y = GetY() ;
            break;
        }
        case 2:
        {
            root.x = GetX() ;
            root.y = GetY() + height/2.0;
            break;
        }
        case 3:
        {
            root.x = GetX() - width/2.0;
            root.y = GetY() ;
            break;
        }
        default:
        {
            wxFAIL_MSG( wxT("Unrecognised attachment point in GetBranchingAttachmentRoot.") );
            break;
        }
    }
    return root;
}

// Draw or erase the branches (not the actual arcs though)
void wxShape::OnDrawBranches(wxDC& dc, int attachment, bool erase)
{
    int count = GetAttachmentLineCount(attachment);
    if (count == 0)
        return;

    wxRealPoint root, neck, shoulder1, shoulder2;
    GetBranchingAttachmentInfo(attachment, root, neck, shoulder1, shoulder2);

    if (erase)
    {
        dc.SetPen(*wxWHITE_PEN);
        dc.SetBrush(*wxWHITE_BRUSH);
    }
    else
    {
        dc.SetPen(*wxBLACK_PEN);
        dc.SetBrush(*wxBLACK_BRUSH);
    }

    // Draw neck
    dc.DrawLine((long) root.x, (long) root.y, (long) neck.x, (long) neck.y);

    if (count > 1)
    {
        // Draw shoulder-to-shoulder line
        dc.DrawLine((long) shoulder1.x, (long) shoulder1.y, (long) shoulder2.x, (long) shoulder2.y);
    }
    // Draw all the little branches
    int i;
    for (i = 0; i < count; i++)
    {
        wxRealPoint pt, stemPt;
        GetBranchingAttachmentPoint(attachment, i, pt, stemPt);
        dc.DrawLine((long) stemPt.x, (long) stemPt.y, (long) pt.x, (long) pt.y);

        if ((GetBranchStyle() & BRANCHING_ATTACHMENT_BLOB) && (count > 1))
        {
            long blobSize=6;
//            dc.DrawEllipse((long) (stemPt.x + 0.5 - (blobSize/2.0)), (long) (stemPt.y + 0.5 - (blobSize/2.0)), blobSize, blobSize);
            dc.DrawEllipse((long) (stemPt.x - (blobSize/2.0)), (long) (stemPt.y - (blobSize/2.0)), blobSize, blobSize);
        }
    }
}

// Draw or erase the branches (not the actual arcs though)
void wxShape::OnDrawBranches(wxDC& dc, bool erase)
{
    if (m_attachmentMode != ATTACHMENT_MODE_BRANCHING)
        return;

    int count = GetNumberOfAttachments();
    int i;
    for (i = 0; i < count; i++)
        OnDrawBranches(dc, i, erase);
}

// Only get the attachment position at the _edge_ of the shape, ignoring
// branching mode. This is used e.g. to indicate the edge of interest, not the point
// on the attachment branch.
bool wxShape::GetAttachmentPositionEdge(int attachment, double *x, double *y,
                                     int nth, int no_arcs, wxLineShape *line)
{
    int oldMode = m_attachmentMode;

    // Calculate as if to edge, not branch
    if (m_attachmentMode == ATTACHMENT_MODE_BRANCHING)
        m_attachmentMode = ATTACHMENT_MODE_EDGE;
    bool success = GetAttachmentPosition(attachment, x, y, nth, no_arcs, line);
    m_attachmentMode = oldMode;

    return success;
}

// Rotate the standard attachment point from physical (0 is always North)
// to logical (0 -> 1 if rotated by 90 degrees)
int wxShape::PhysicalToLogicalAttachment(int physicalAttachment) const
{
    const double pi = M_PI ;
    int i;
    if (oglRoughlyEqual(GetRotation(), 0.0))
    {
        i = physicalAttachment;
    }
    else if (oglRoughlyEqual(GetRotation(), (pi/2.0)))
    {
        i = physicalAttachment - 1;
    }
    else if (oglRoughlyEqual(GetRotation(), pi))
    {
        i = physicalAttachment - 2;
    }
    else if (oglRoughlyEqual(GetRotation(), (3.0*pi/2.0)))
    {
        i = physicalAttachment - 3;
    }
    else
        // Can't handle -- assume the same.
        return physicalAttachment;

    if (i < 0)
      i += 4;

    return i;
}

// Rotate the standard attachment point from logical
// to physical (0 is always North)
int wxShape::LogicalToPhysicalAttachment(int logicalAttachment) const
{
    const double pi = M_PI ;
    int i;
    if (oglRoughlyEqual(GetRotation(), 0.0))
    {
        i = logicalAttachment;
    }
    else if (oglRoughlyEqual(GetRotation(), (pi/2.0)))
    {
        i = logicalAttachment + 1;
    }
    else if (oglRoughlyEqual(GetRotation(), pi))
    {
        i = logicalAttachment + 2;
    }
    else if (oglRoughlyEqual(GetRotation(), (3.0*pi/2.0)))
    {
        i = logicalAttachment + 3;
    }
    else
        // Can't handle -- assume the same.
        return logicalAttachment;

    if (i > 3)
      i -= 4;

    return i;
}

void wxShape::Rotate(double WXUNUSED(x), double WXUNUSED(y), double theta)
{
    const double pi = M_PI ;
    m_rotation = theta;
    if (m_rotation < 0.0)
    {
        m_rotation += 2*pi;
    }
    else if (m_rotation > 2*pi)
    {
        m_rotation -= 2*pi;
    }
}


wxPen wxShape::GetBackgroundPen()
{
    if (GetCanvas())
    {
        wxColour c = GetCanvas()->GetBackgroundColour();
        return wxPen(c, 1, wxSOLID);
    }
    return * g_oglWhiteBackgroundPen;
}


wxBrush wxShape::GetBackgroundBrush()
{
    if (GetCanvas())
    {
        wxColour c = GetCanvas()->GetBackgroundColour();
        return wxBrush(c, wxSOLID);
    }
    return * g_oglWhiteBackgroundBrush;
}
