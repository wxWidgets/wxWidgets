/////////////////////////////////////////////////////////////////////////////
// Name:        divided.cpp
// Purpose:     wxDividedShape class
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "divided.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/wxexpr.h>

#include "basic.h"
#include "basicp.h"
#include "canvas.h"
#include "divided.h"
#include "lines.h"
#include "misc.h"

class wxDividedShapeControlPoint: public wxControlPoint
{
 DECLARE_DYNAMIC_CLASS(wxDividedShapeControlPoint)
 private:
  int regionId;
 public:
  wxDividedShapeControlPoint() { regionId = 0; }
  wxDividedShapeControlPoint(wxShapeCanvas *the_canvas, wxShape *object, int region,
                            double size, double the_xoffset, double the_yoffset, int the_type);
  ~wxDividedShapeControlPoint();

  void OnDragLeft(bool draw, double x, double y, int keys=0, int attachment = 0);
  void OnBeginDragLeft(double x, double y, int keys=0, int attachment = 0);
  void OnEndDragLeft(double x, double y, int keys=0, int attachment = 0);
};

IMPLEMENT_DYNAMIC_CLASS(wxDividedShapeControlPoint, wxControlPoint)

/*
 * Divided object
 *
 */

IMPLEMENT_DYNAMIC_CLASS(wxDividedShape, wxRectangleShape)

wxDividedShape::wxDividedShape(double w, double h): wxRectangleShape(w, h)
{
  ClearRegions();
}

wxDividedShape::~wxDividedShape()
{
}

void wxDividedShape::OnDraw(wxDC& dc)
{
  wxRectangleShape::OnDraw(dc);
}

void wxDividedShape::OnDrawContents(wxDC& dc)
{
  double defaultProportion = (double)(GetRegions().Number() > 0 ? (1.0/((double)(GetRegions().Number()))) : 0.0);
  double currentY = (double)(m_ypos - (m_height / 2.0));
  double maxY = (double)(m_ypos + (m_height / 2.0));

  double leftX = (double)(m_xpos - (m_width / 2.0));
  double rightX = (double)(m_xpos + (m_width / 2.0));

  if (m_pen) dc.SetPen(m_pen);

  if (m_textColour) dc.SetTextForeground(* m_textColour);

#ifdef __WXMSW__
  // For efficiency, don't do this under X - doesn't make
  // any visible difference for our purposes.
  if (m_brush)
    dc.SetTextBackground(m_brush->GetColour());
#endif
/*
  if (!formatted)
  {
    FormatRegionText();
    formatted = TRUE;
  }
*/
  if (GetDisableLabel()) return;

  double xMargin = 2;
  double yMargin = 2;
  dc.SetBackgroundMode(wxTRANSPARENT);

  wxNode *node = GetRegions().First();
  while (node)
  {
    wxShapeRegion *region = (wxShapeRegion *)node->Data();
    dc.SetFont(region->GetFont());
    dc.SetTextForeground(* region->GetActualColourObject());

    double proportion =
      region->m_regionProportionY < 0.0 ? defaultProportion : region->m_regionProportionY;

    double y = currentY + m_height*proportion;
    double actualY = maxY < y ? maxY : y;

    double centreX = m_xpos;
    double centreY = (double)(currentY + (actualY - currentY)/2.0);

    oglDrawFormattedText(dc, &region->m_formattedText,
             (double)(centreX), (double)(centreY), (double)(m_width-2*xMargin), (double)(actualY - currentY - 2*yMargin),
             region->m_formatMode);
    if ((y <= maxY) && (node->Next()))
    {
      wxPen *regionPen = region->GetActualPen();
      if (regionPen)
      {
        dc.SetPen(regionPen);
        dc.DrawLine(WXROUND(leftX), WXROUND(y), WXROUND(rightX), WXROUND(y));
      }
    }

    currentY = actualY;

    node = node->Next();
  }
}

void wxDividedShape::SetSize(double w, double h, bool recursive)
{
  SetAttachmentSize(w, h);
  m_width = w;
  m_height = h;
  SetRegionSizes();
}

void wxDividedShape::SetRegionSizes()
{
  if (GetRegions().Number() == 0)
    return;
    
  double defaultProportion = (double)(GetRegions().Number() > 0 ? (1.0/((double)(GetRegions().Number()))) : 0.0);
  double currentY = (double)(m_ypos - (m_height / 2.0));
  double maxY = (double)(m_ypos + (m_height / 2.0));

//  double leftX = (double)(m_xpos - (m_width / 2.0));
//  double rightX = (double)(m_xpos + (m_width / 2.0));

  wxNode *node = GetRegions().First();
  while (node)
  {
    wxShapeRegion *region = (wxShapeRegion *)node->Data();
    double proportion =
      region->m_regionProportionY <= 0.0 ? defaultProportion : region->m_regionProportionY;

    double sizeY = (double)proportion*m_height;
    double y = currentY + sizeY;
    double actualY = maxY < y ? maxY : y;

    double centreY = (double)(currentY + (actualY - currentY)/2.0);

    region->SetSize(m_width, sizeY);
    region->SetPosition(0.0, (double)(centreY - m_ypos));
    currentY = actualY;
    node = node->Next();
  }
}

// Attachment points correspond to regions in the divided box
bool wxDividedShape::GetAttachmentPosition(int attachment, double *x, double *y, int nth, int no_arcs,
  wxLineShape *line)
{
  int totalNumberAttachments = (GetRegions().Number() * 2) + 2;
  if ((GetAttachmentMode() == ATTACHMENT_MODE_NONE) || (attachment >= totalNumberAttachments))
  {
    return wxShape::GetAttachmentPosition(attachment, x, y, nth, no_arcs);
  }

  int n = GetRegions().Number();
  bool isEnd = (line && line->IsEnd(this));

  double left = (double)(m_xpos - m_width/2.0);
  double right = (double)(m_xpos + m_width/2.0);
  double top = (double)(m_ypos - m_height/2.0);
  double bottom = (double)(m_ypos + m_height/2.0);

  // Zero is top, n+1 is bottom.
  if (attachment == 0)
  {
    *y = top;
    if (m_spaceAttachments)
    {
      if (line && (line->GetAlignmentType(isEnd) == LINE_ALIGNMENT_TO_NEXT_HANDLE))
      {
        // Align line according to the next handle along
        wxRealPoint *point = line->GetNextControlPoint(this);
        if (point->x < left)
          *x = left;
        else if (point->x > right)
          *x = right;
        else
          *x = point->x;
      }
      else
        *x = left + (nth + 1)*m_width/(no_arcs + 1);
    }
    else
      *x = m_xpos;
  }
  else if (attachment == (n+1))
  {
    *y = bottom;
    if (m_spaceAttachments)
    {
      if (line && (line->GetAlignmentType(isEnd) == LINE_ALIGNMENT_TO_NEXT_HANDLE))
      {
        // Align line according to the next handle along
        wxRealPoint *point = line->GetNextControlPoint(this);
        if (point->x < left)
          *x = left;
        else if (point->x > right)
          *x = right;
        else
          *x = point->x;
      }
      else
        *x = left + (nth + 1)*m_width/(no_arcs + 1);
    }
    else
      *x = m_xpos;
  }
  // Left or right.
  else
  {
    int i = 0;
    bool isLeft = FALSE;
    if (attachment < (n+1))
    {
      i = attachment-1;
      isLeft = FALSE;
    }
    else
    {
      i = (totalNumberAttachments - attachment - 1);
      isLeft = TRUE;
    }
    wxNode *node = GetRegions().Nth(i);
    if (node)
    {
      wxShapeRegion *region = (wxShapeRegion *)node->Data();

      if (isLeft)
        *x = left;
      else
        *x = right;

      // Calculate top and bottom of region
      top = (double)((m_ypos + region->m_y) - (region->m_height/2.0));
      bottom = (double)((m_ypos + region->m_y) + (region->m_height/2.0));

      // Assuming we can trust the absolute size and
      // position of these regions...
      if (m_spaceAttachments)
      {
        if (line && (line->GetAlignmentType(isEnd) == LINE_ALIGNMENT_TO_NEXT_HANDLE))
        {
          // Align line according to the next handle along
          wxRealPoint *point = line->GetNextControlPoint(this);
          if (point->y < bottom)
            *y = bottom;
          else if (point->y > top)
            *y = top;
          else
            *y = point->y;
        }
        else
//          *y = (double)(((m_ypos + region->m_y) - (region->m_height/2.0)) + (nth + 1)*region->m_height/(no_arcs+1));
          *y = (double)(top + (nth + 1)*region->m_height/(no_arcs+1));
      }
      else
        *y = (double)(m_ypos + region->m_y);
    }
    else
    {
      *x = m_xpos;
      *y = m_ypos;
      return FALSE;
    }
  }
  return TRUE;
}

int wxDividedShape::GetNumberOfAttachments() const
{
  // There are two attachments for each region (left and right),
  // plus one on the top and one on the bottom.
  int n = (GetRegions().Number() * 2) + 2;

  int maxN = n - 1;
  wxNode *node = m_attachmentPoints.First();
  while (node)
  {
    wxAttachmentPoint *point = (wxAttachmentPoint *)node->Data();
    if (point->m_id > maxN)
      maxN = point->m_id;
    node = node->Next();
  }
  return maxN + 1;
}

bool wxDividedShape::AttachmentIsValid(int attachment)
{
  int totalNumberAttachments = (GetRegions().Number() * 2) + 2;
  if (attachment >= totalNumberAttachments)
  {
    return wxShape::AttachmentIsValid(attachment);
  }
  else if (attachment >= 0)
    return TRUE;
  else
    return FALSE;
}

void wxDividedShape::Copy(wxShape& copy)
{
  wxRectangleShape::Copy(copy);
}

// Region operations

void wxDividedShape::MakeControlPoints()
{
  wxRectangleShape::MakeControlPoints();

  MakeMandatoryControlPoints();
}

void wxDividedShape::MakeMandatoryControlPoints()
{
  double currentY = (double)(GetY() - (m_height / 2.0));
  double maxY = (double)(GetY() + (m_height / 2.0));

  wxNode *node = GetRegions().First();
  int i = 0;
  while (node)
  {
    wxShapeRegion *region = (wxShapeRegion *)node->Data();

    double proportion = region->m_regionProportionY;

    double y = currentY + m_height*proportion;
    double actualY = (double)(maxY < y ? maxY : y);

    if (node->Next())
    {
      wxDividedShapeControlPoint *controlPoint =
        new wxDividedShapeControlPoint(m_canvas, this, i, CONTROL_POINT_SIZE, 0.0, (double)(actualY - GetY()), 0);
      m_canvas->AddShape(controlPoint);
      m_controlPoints.Append(controlPoint);
    }
    currentY = actualY;
    i ++;
    node = node->Next();
  }
}

void wxDividedShape::ResetControlPoints()
{
  // May only have the region handles, (n - 1) of them.
  if (m_controlPoints.Number() > (GetRegions().Number() - 1))
    wxRectangleShape::ResetControlPoints();

  ResetMandatoryControlPoints();
}

void wxDividedShape::ResetMandatoryControlPoints()
{
  double currentY = (double)(GetY() - (m_height / 2.0));
  double maxY = (double)(GetY() + (m_height / 2.0));

  wxNode *node = m_controlPoints.First();
  int i = 0;
  while (node)
  {
    wxControlPoint *controlPoint = (wxControlPoint *)node->Data();
    if (controlPoint->IsKindOf(CLASSINFO(wxDividedShapeControlPoint)))
    {
      wxNode *node1 = GetRegions().Nth(i);
      wxShapeRegion *region = (wxShapeRegion *)node1->Data();

      double proportion = region->m_regionProportionY;

      double y = currentY + m_height*proportion;
      double actualY = (double)(maxY < y ? maxY : y);

      controlPoint->m_xoffset = 0.0;
      controlPoint->m_yoffset = (double)(actualY - GetY());
      currentY = actualY;
      i ++;
    }
    node = node->Next();
  }
}

#ifdef PROLOGIO
void wxDividedShape::WriteAttributes(wxExpr *clause)
{
  wxRectangleShape::WriteAttributes(clause);
}

void wxDividedShape::ReadAttributes(wxExpr *clause)
{
  wxRectangleShape::ReadAttributes(clause);
}
#endif

/*
 * Edit the division colour/style
 *
 */

void wxDividedShape::EditRegions()
{
  wxMessageBox("EditRegions() is unimplemented.", "OGL", wxOK);

  // TODO
#if 0
  if (GetRegions().Number() < 2)
    return;
    
  wxBeginBusyCursor();

  GraphicsForm *form = new GraphicsForm("Divided nodes");
  // Need an array to store all the style strings,
  // since they need to be converted to integers
  char **styleStrings = new char *[GetRegions().Number()];
  for (int j = 0; j < GetRegions().Number(); j++)
    styleStrings[j] = NULL;

  int i = 0;
  wxNode *node = GetRegions().First();
  while (node && node->Next())
  {
    wxShapeRegion *region = (wxShapeRegion *)node->Data();
    char buf[50];
    sprintf(buf, "Region %d", (i+1));
    form->Add(wxMakeFormMessage(buf));
    form->Add(wxMakeFormNewLine());

    form->Add(wxMakeFormString("Colour", &region->penColour, wxFORM_CHOICE,
              new wxList(wxMakeConstraintStrings(
    "Invisible"        ,
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

    char *styleString = NULL;
    switch (region->penStyle)
    {
      case wxSHORT_DASH:
        styleString = "Short Dash";
        break;
      case wxLONG_DASH:
        styleString = "Long Dash";
        break;
      case wxDOT:
        styleString = "Dot";
        break;
      case wxDOT_DASH:
        styleString = "Dot Dash";
        break;
      case wxSOLID:
      default:
        styleString = "Solid";
        break;
    }
    styleStrings[i] = copystring(styleString);
    form->Add(wxMakeFormString("Style", &(styleStrings[i]), wxFORM_CHOICE,
              new wxList(wxMakeConstraintStrings(
    "Solid"            ,
    "Short Dash"       ,
    "Long Dash"        ,
    "Dot"              ,
    "Dot Dash"         ,
    NULL),
    NULL), NULL, wxVERTICAL, 100));
    node = node->Next();
    i ++;
    if (node && node->Next())
      form->Add(wxMakeFormNewLine());
  }
  wxDialogBox *dialog = new wxDialogBox(m_canvas->GetParent(), "Divided object properties", 10, 10, 500, 500);
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

  node = GetRegions().First();
  i = 0;
  while (node)
  {
    wxShapeRegion *region = (wxShapeRegion *)node->Data();

    if (styleStrings[i])
    {
      if (strcmp(styleStrings[i], "Solid") == 0)
        region->penStyle = wxSOLID;
      else if (strcmp(styleStrings[i], "Dot") == 0)
        region->penStyle = wxDOT;
      else if (strcmp(styleStrings[i], "Short Dash") == 0)
        region->penStyle = wxSHORT_DASH;
      else if (strcmp(styleStrings[i], "Long Dash") == 0)
        region->penStyle = wxLONG_DASH;
      else if (strcmp(styleStrings[i], "Dot Dash") == 0)
        region->penStyle = wxDOT_DASH;
      delete[] styleStrings[i];
    }
    region->m_actualPenObject = NULL;
    node = node->Next();
    i ++;
  }
  delete[] styleStrings;
  Draw(dc);
#endif
}

void wxDividedShape::OnRightClick(double x, double y, int keys, int attachment)
{
  if (keys & KEY_CTRL)
  {
    EditRegions();
  }
  else
  {
    wxRectangleShape::OnRightClick(x, y, keys, attachment);
  }
}

wxDividedShapeControlPoint::wxDividedShapeControlPoint(wxShapeCanvas *the_canvas, wxShape *object,
  int region, double size, double the_m_xoffset, double the_m_yoffset, int the_type):
    wxControlPoint(the_canvas, object, size, the_m_xoffset, the_m_yoffset, the_type)
{
  regionId = region;
}

wxDividedShapeControlPoint::~wxDividedShapeControlPoint()
{
}

// Implement resizing of divided object division
void wxDividedShapeControlPoint::OnDragLeft(bool draw, double x, double y, int keys, int attachment)
{
    wxClientDC dc(GetCanvas());
    GetCanvas()->PrepareDC(dc);

    dc.SetLogicalFunction(wxXOR);
    wxPen dottedPen(wxColour(0, 0, 0), 1, wxDOT);
    dc.SetPen(dottedPen);
    dc.SetBrush((* wxTRANSPARENT_BRUSH));

    wxDividedShape *dividedObject = (wxDividedShape *)m_shape;
    double x1 = (double)(dividedObject->GetX() - (dividedObject->GetWidth()/2.0));
    double y1 = y;
    double x2 = (double)(dividedObject->GetX() + (dividedObject->GetWidth()/2.0));
    double y2 = y;
    dc.DrawLine(WXROUND(x1), WXROUND(y1), WXROUND(x2), WXROUND(y2));
}

void wxDividedShapeControlPoint::OnBeginDragLeft(double x, double y, int keys, int attachment)
{
    wxClientDC dc(GetCanvas());
    GetCanvas()->PrepareDC(dc);

    wxDividedShape *dividedObject = (wxDividedShape *)m_shape;
    dc.SetLogicalFunction(wxXOR);
    wxPen dottedPen(wxColour(0, 0, 0), 1, wxDOT);
    dc.SetPen(dottedPen);
    dc.SetBrush((* wxTRANSPARENT_BRUSH));

    double x1 = (double)(dividedObject->GetX() - (dividedObject->GetWidth()/2.0));
    double y1 = y;
    double x2 = (double)(dividedObject->GetX() + (dividedObject->GetWidth()/2.0));
    double y2 = y;
    dc.DrawLine(WXROUND(x1), WXROUND(y1), WXROUND(x2), WXROUND(y2));
    m_canvas->CaptureMouse();
}

void wxDividedShapeControlPoint::OnEndDragLeft(double x, double y, int keys, int attachment)
{
    wxClientDC dc(GetCanvas());
    GetCanvas()->PrepareDC(dc);

    wxDividedShape *dividedObject = (wxDividedShape *)m_shape;
    wxNode *node = dividedObject->GetRegions().Nth(regionId);
    if (!node)
    return;
    
    wxShapeRegion *thisRegion = (wxShapeRegion *)node->Data();
    wxShapeRegion *nextRegion = NULL; // Region below this one

    dc.SetLogicalFunction(wxCOPY);

    m_canvas->ReleaseMouse();

    // Find the old top and bottom of this region,
    // and calculate the new proportion for this region
    // if legal.
  
    double currentY = (double)(dividedObject->GetY() - (dividedObject->GetHeight() / 2.0));
    double maxY = (double)(dividedObject->GetY() + (dividedObject->GetHeight() / 2.0));

    // Save values
    double thisRegionTop = 0.0;
    double thisRegionBottom = 0.0;
    double nextRegionBottom = 0.0;

    node = dividedObject->GetRegions().First();
    while (node)
    {
      wxShapeRegion *region = (wxShapeRegion *)node->Data();

      double proportion = region->m_regionProportionY;
      double yy = currentY + (dividedObject->GetHeight()*proportion);
      double actualY = (double)(maxY < yy ? maxY : yy);

      if (region == thisRegion)
      {
        thisRegionTop = currentY;
        thisRegionBottom = actualY;
        if (node->Next())
          nextRegion = (wxShapeRegion *)node->Next()->Data();
      }
      if (region == nextRegion)
      {
        nextRegionBottom = actualY;
      }

      currentY = actualY;
      node = node->Next();
    }
    if (!nextRegion)
      return;

    // Check that we haven't gone above this region or below
    // next region.
    if ((y <= thisRegionTop) || (y >= nextRegionBottom))
    return;

    dividedObject->EraseLinks(dc);

    // Now calculate the new proportions of this region and the next region.
    double thisProportion = (double)((y - thisRegionTop)/dividedObject->GetHeight());
    double nextProportion = (double)((nextRegionBottom - y)/dividedObject->GetHeight());
    thisRegion->SetProportions(0.0, thisProportion);
    nextRegion->SetProportions(0.0, nextProportion);
    m_yoffset = (double)(y - dividedObject->GetY());

    // Now reformat text
    int i = 0;
    node = dividedObject->GetRegions().First();
    while (node)
    {
        wxShapeRegion *region = (wxShapeRegion *)node->Data();
        if (region->GetText())
        {
        char *s = copystring(region->GetText());
        dividedObject->FormatText(dc, s, i);
        delete[] s;
        }
        node = node->Next();
        i++;
    }
    dividedObject->SetRegionSizes();
    dividedObject->Draw(dc);
    dividedObject->GetEventHandler()->OnMoveLinks(dc);
}

