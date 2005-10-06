/////////////////////////////////////////////////////////////////////////////
// Name:        divided.cpp
// Purpose:     wxDividedShape class
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

#include "wx/ogl/ogl.h"


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
  double defaultProportion = (double)(GetRegions().GetCount() > 0 ? (1.0/((double)(GetRegions().GetCount()))) : 0.0);
  double currentY = (double)(m_ypos - (m_height / 2.0));
  double maxY = (double)(m_ypos + (m_height / 2.0));

  double leftX = (double)(m_xpos - (m_width / 2.0));
  double rightX = (double)(m_xpos + (m_width / 2.0));

  if (m_pen) dc.SetPen(* m_pen);

  dc.SetTextForeground(m_textColour);

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
    formatted = true;
  }
*/
  if (GetDisableLabel()) return;

  double xMargin = 2;
  double yMargin = 2;
  dc.SetBackgroundMode(wxTRANSPARENT);

  wxObjectList::compatibility_iterator node = GetRegions().GetFirst();
  while (node)
  {
    wxShapeRegion *region = (wxShapeRegion *)node->GetData();
    dc.SetFont(* region->GetFont());
    dc.SetTextForeground(region->GetActualColourObject());

    double proportion =
      region->m_regionProportionY < 0.0 ? defaultProportion : region->m_regionProportionY;

    double y = currentY + m_height*proportion;
    double actualY = maxY < y ? maxY : y;

    double centreX = m_xpos;
    double centreY = (double)(currentY + (actualY - currentY)/2.0);

    oglDrawFormattedText(dc, &region->m_formattedText,
             (double)(centreX), (double)(centreY), (double)(m_width-2*xMargin), (double)(actualY - currentY - 2*yMargin),
             region->m_formatMode);
    if ((y <= maxY) && (node->GetNext()))
    {
      wxPen *regionPen = region->GetActualPen();
      if (regionPen)
      {
        dc.SetPen(* regionPen);
        dc.DrawLine(WXROUND(leftX), WXROUND(y), WXROUND(rightX), WXROUND(y));
      }
    }

    currentY = actualY;

    node = node->GetNext();
  }
}

void wxDividedShape::SetSize(double w, double h, bool WXUNUSED(recursive))
{
  SetAttachmentSize(w, h);
  m_width = w;
  m_height = h;
  SetRegionSizes();
}

void wxDividedShape::SetRegionSizes()
{
  if (GetRegions().GetCount() == 0)
    return;

  double defaultProportion = (double)(GetRegions().GetCount() > 0 ? (1.0/((double)(GetRegions().GetCount()))) : 0.0);
  double currentY = (double)(m_ypos - (m_height / 2.0));
  double maxY = (double)(m_ypos + (m_height / 2.0));

//  double leftX = (double)(m_xpos - (m_width / 2.0));
//  double rightX = (double)(m_xpos + (m_width / 2.0));

  wxObjectList::compatibility_iterator node = GetRegions().GetFirst();
  while (node)
  {
    wxShapeRegion *region = (wxShapeRegion *)node->GetData();
    double proportion =
      region->m_regionProportionY <= 0.0 ? defaultProportion : region->m_regionProportionY;

    double sizeY = (double)proportion*m_height;
    double y = currentY + sizeY;
    double actualY = maxY < y ? maxY : y;

    double centreY = (double)(currentY + (actualY - currentY)/2.0);

    region->SetSize(m_width, sizeY);
    region->SetPosition(0.0, (double)(centreY - m_ypos));
    currentY = actualY;
    node = node->GetNext();
  }
}

// Attachment points correspond to regions in the divided box
bool wxDividedShape::GetAttachmentPosition(int attachment, double *x, double *y, int nth, int no_arcs,
  wxLineShape *line)
{
  int totalNumberAttachments = (GetRegions().GetCount() * 2) + 2;
  if ((GetAttachmentMode() == ATTACHMENT_MODE_NONE) || (attachment >= totalNumberAttachments))
  {
    return wxShape::GetAttachmentPosition(attachment, x, y, nth, no_arcs);
  }

  int n = GetRegions().GetCount();
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
    bool isLeft = !(attachment < (n+1));
    int i = (isLeft) ? (totalNumberAttachments - attachment - 1) : (attachment-1);
    wxObjectList::compatibility_iterator node = GetRegions().Item(i);
    if (node)
    {
      wxShapeRegion *region = (wxShapeRegion *)node->GetData();

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
      return false;
    }
  }
  return true;
}

int wxDividedShape::GetNumberOfAttachments() const
{
  // There are two attachments for each region (left and right),
  // plus one on the top and one on the bottom.
  int n = (GetRegions().GetCount() * 2) + 2;

  int maxN = n - 1;
  wxObjectList::compatibility_iterator node = m_attachmentPoints.GetFirst();
  while (node)
  {
    wxAttachmentPoint *point = (wxAttachmentPoint *)node->GetData();
    if (point->m_id > maxN)
      maxN = point->m_id;
    node = node->GetNext();
  }
  return maxN + 1;
}

bool wxDividedShape::AttachmentIsValid(int attachment) const
{
  int totalNumberAttachments = (GetRegions().GetCount() * 2) + 2;
  if (attachment >= totalNumberAttachments)
  {
    return wxShape::AttachmentIsValid(attachment);
  }
  else if (attachment >= 0)
    return true;
  else
    return false;
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

  wxObjectList::compatibility_iterator node = GetRegions().GetFirst();
  int i = 0;
  while (node)
  {
    wxShapeRegion *region = (wxShapeRegion *)node->GetData();

    double proportion = region->m_regionProportionY;

    double y = currentY + m_height*proportion;
    double actualY = (double)(maxY < y ? maxY : y);

    if (node->GetNext())
    {
      wxDividedShapeControlPoint *controlPoint =
        new wxDividedShapeControlPoint(m_canvas, this, i, CONTROL_POINT_SIZE, 0.0, (double)(actualY - GetY()), 0);
      m_canvas->AddShape(controlPoint);
      m_controlPoints.Append(controlPoint);
    }
    currentY = actualY;
    i ++;
    node = node->GetNext();
  }
}

void wxDividedShape::ResetControlPoints()
{
  // May only have the region handles, (n - 1) of them.
  if (m_controlPoints.GetCount() > (GetRegions().GetCount() - 1))
    wxRectangleShape::ResetControlPoints();

  ResetMandatoryControlPoints();
}

void wxDividedShape::ResetMandatoryControlPoints()
{
  double currentY = (double)(GetY() - (m_height / 2.0));
  double maxY = (double)(GetY() + (m_height / 2.0));

  wxObjectList::compatibility_iterator node = m_controlPoints.GetFirst();
  int i = 0;
  while (node)
  {
    wxControlPoint *controlPoint = (wxControlPoint *)node->GetData();
    if (controlPoint->IsKindOf(CLASSINFO(wxDividedShapeControlPoint)))
    {
      wxObjectList::compatibility_iterator node1 = GetRegions().Item(i);
      wxShapeRegion *region = (wxShapeRegion *)node1->GetData();

      double proportion = region->m_regionProportionY;

      double y = currentY + m_height*proportion;
      double actualY = (double)(maxY < y ? maxY : y);

      controlPoint->m_xoffset = 0.0;
      controlPoint->m_yoffset = (double)(actualY - GetY());
      currentY = actualY;
      i ++;
    }
    node = node->GetNext();
  }
}

#if wxUSE_PROLOGIO
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
  wxMessageBox(wxT("EditRegions() is unimplemented."), wxT("OGL"), wxOK);

  // TODO
#if 0
  if (GetRegions().GetCount() < 2)
    return;

  wxBeginBusyCursor();

  GraphicsForm *form = new GraphicsForm("Divided nodes");
  // Need an array to store all the style strings,
  // since they need to be converted to integers
  char **styleStrings = new char *[GetRegions().GetCount()];
  for (int j = 0; j < GetRegions().GetCount(); j++)
    styleStrings[j] = NULL;

  int i = 0;
  wxNode *node = GetRegions().GetFirst();
  while (node && node->GetNext())
  {
    wxShapeRegion *region = (wxShapeRegion *)node->GetData();
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
    node = node->GetNext();
    i ++;
    if (node && node->GetNext())
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

  dialog->Show(true);

  node = GetRegions().GetFirst();
  i = 0;
  while (node)
  {
    wxShapeRegion *region = (wxShapeRegion *)node->GetData();

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
    node = node->GetNext();
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
void wxDividedShapeControlPoint::OnDragLeft(bool WXUNUSED(draw), double WXUNUSED(x), double y, int WXUNUSED(keys), int WXUNUSED(attachment))
{
    wxClientDC dc(GetCanvas());
    GetCanvas()->PrepareDC(dc);

    dc.SetLogicalFunction(OGLRBLF);
    wxPen dottedPen(*wxBLACK, 1, wxDOT);
    dc.SetPen(dottedPen);
    dc.SetBrush((* wxTRANSPARENT_BRUSH));

    wxDividedShape *dividedObject = (wxDividedShape *)m_shape;
    double x1 = (double)(dividedObject->GetX() - (dividedObject->GetWidth()/2.0));
    double y1 = y;
    double x2 = (double)(dividedObject->GetX() + (dividedObject->GetWidth()/2.0));
    double y2 = y;
    dc.DrawLine(WXROUND(x1), WXROUND(y1), WXROUND(x2), WXROUND(y2));
}

void wxDividedShapeControlPoint::OnBeginDragLeft(double WXUNUSED(x), double y, int WXUNUSED(keys), int WXUNUSED(attachment))
{
    wxClientDC dc(GetCanvas());
    GetCanvas()->PrepareDC(dc);

    wxDividedShape *dividedObject = (wxDividedShape *)m_shape;
    dc.SetLogicalFunction(OGLRBLF);
    wxPen dottedPen(*wxBLACK, 1, wxDOT);
    dc.SetPen(dottedPen);
    dc.SetBrush((* wxTRANSPARENT_BRUSH));

    double x1 = (double)(dividedObject->GetX() - (dividedObject->GetWidth()/2.0));
    double y1 = y;
    double x2 = (double)(dividedObject->GetX() + (dividedObject->GetWidth()/2.0));
    double y2 = y;
    dc.DrawLine(WXROUND(x1), WXROUND(y1), WXROUND(x2), WXROUND(y2));
    m_canvas->CaptureMouse();
}

void wxDividedShapeControlPoint::OnEndDragLeft(double WXUNUSED(x), double y, int WXUNUSED(keys), int WXUNUSED(attachment))
{
    wxClientDC dc(GetCanvas());
    GetCanvas()->PrepareDC(dc);

    wxDividedShape *dividedObject = (wxDividedShape *)m_shape;
    wxObjectList::compatibility_iterator node = dividedObject->GetRegions().Item(regionId);
    if (!node)
    return;

    wxShapeRegion *thisRegion = (wxShapeRegion *)node->GetData();
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
    double nextRegionBottom = 0.0;

    node = dividedObject->GetRegions().GetFirst();
    while (node)
    {
      wxShapeRegion *region = (wxShapeRegion *)node->GetData();

      double proportion = region->m_regionProportionY;
      double yy = currentY + (dividedObject->GetHeight()*proportion);
      double actualY = (double)(maxY < yy ? maxY : yy);

      if (region == thisRegion)
      {
        thisRegionTop = currentY;
        if (node->GetNext())
          nextRegion = (wxShapeRegion *)node->GetNext()->GetData();
      }
      if (region == nextRegion)
      {
        nextRegionBottom = actualY;
      }

      currentY = actualY;
      node = node->GetNext();
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
    node = dividedObject->GetRegions().GetFirst();
    while (node)
    {
        wxShapeRegion *region = (wxShapeRegion *)node->GetData();
        if (region->GetText())
        {
        wxString s(region->GetText());
        dividedObject->FormatText(dc, s.c_str(), i);
        }
        node = node->GetNext();
        i++;
    }
    dividedObject->SetRegionSizes();
    dividedObject->Draw(dc);
    dividedObject->GetEventHandler()->OnMoveLinks(dc);
}

