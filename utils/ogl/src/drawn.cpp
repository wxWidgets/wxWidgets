/////////////////////////////////////////////////////////////////////////////
// Name:        drawn.cpp
// Purpose:     wxDrawnShape
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "drawn.h"
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
#include "canvas.h"
#include "mfutils.h"
#include "drawn.h"
#include "drawnp.h"
#include "misc.h"

static void IntToHex(unsigned int dec, char *buf);
static unsigned long HexToInt(char *buf);
extern char *GraphicsBuffer;

#define gyTYPE_PEN   40
#define gyTYPE_BRUSH 41
#define gyTYPE_FONT  42

/*
 * Drawn object
 *
 */

IMPLEMENT_DYNAMIC_CLASS(wxDrawnShape, wxRectangleShape)

wxDrawnShape::wxDrawnShape():wxRectangleShape(100.0, 50.0)
{
  m_saveToFile = TRUE;
}

wxDrawnShape::~wxDrawnShape()
{
}

void wxDrawnShape::OnDraw(wxDC& dc)
{
  // Pass pen and brush in case we have force outline
  // and fill colours
  if (m_shadowMode != SHADOW_NONE)
  {
    if (m_shadowBrush)
      m_metafile.m_fillBrush = m_shadowBrush;
    m_metafile.m_outlinePen = transparent_pen;
    m_metafile.Draw(dc, m_xpos + m_shadowOffsetX, m_ypos + m_shadowOffsetY);
  }
    
  m_metafile.m_outlinePen = m_pen;
  m_metafile.m_fillBrush = m_brush;
  m_metafile.Draw(dc, m_xpos, m_ypos);
}

void wxDrawnShape::SetSize(float w, float h, bool recursive)
{
  SetAttachmentSize(w, h);

  float scaleX;
  float scaleY;
  if (GetWidth() == 0.0)
    scaleX = 1.0;
  else scaleX = w/GetWidth();
  if (GetHeight() == 0.0)
    scaleY = 1.0;
  else scaleY = h/GetHeight();

  m_metafile.Scale(scaleX, scaleY);
  m_width = w;
  m_height = h;
  SetDefaultRegionSize();
}

void wxDrawnShape::Scale(float sx, float sy)
{
  m_metafile.Scale(sx, sy);
  m_metafile.CalculateSize(this);
}

void wxDrawnShape::Translate(float x, float y)
{
  m_metafile.Translate(x, y);
  m_metafile.CalculateSize(this);
}

void wxDrawnShape::Rotate(float x, float y, float theta)
{
  if (!m_metafile.GetRotateable())
    return;
    
  float actualTheta = theta-m_metafile.m_currentRotation;

  // Rotate metafile
  m_metafile.Rotate(x, y, theta);

  // Rotate attachment points
  float sinTheta = (float)sin(actualTheta);
  float cosTheta = (float)cos(actualTheta);
  wxNode *node = m_attachmentPoints.First();
  while (node)
  {
    wxAttachmentPoint *point = (wxAttachmentPoint *)node->Data();
    float x1 = point->m_x;
    float y1 = point->m_y;
    point->m_x = x1*cosTheta - y1*sinTheta + x*(1 - cosTheta) + y*sinTheta;
    point->m_y = x1*sinTheta + y1*cosTheta + y*(1 - cosTheta) + x*sinTheta;
    node = node->Next();
  }
  m_rotation = theta;

  m_metafile.CalculateSize(this);
}

#ifdef PROLOGIO
// Prolog database stuff
char *wxDrawnShape::GetFunctor()
{
  return "node_image";
}

void wxDrawnShape::WritePrologAttributes(wxExpr *clause)
{
  wxRectangleShape::WritePrologAttributes(clause);

  clause->AddAttributeValue("save_metafile", (long)m_saveToFile);
  if (m_saveToFile)
    m_metafile.WritePrologAttributes(clause);
}

void wxDrawnShape::ReadPrologAttributes(wxExpr *clause)
{
  wxRectangleShape::ReadPrologAttributes(clause);

  int iVal = (int) m_saveToFile;
  clause->AssignAttributeValue("save_metafile", &iVal);
  m_saveToFile = (iVal != 0);

  if (m_saveToFile)
    m_metafile.ReadPrologAttributes(clause);
}
#endif

// Does the copying for this object
void wxDrawnShape::Copy(wxShape& copy)
{
  wxRectangleShape::Copy(copy);

  wxASSERT( copy.IsKindOf(CLASSINFO(wxDrawnShape)) ) ;

  wxDrawnShape& drawnCopy = (wxDrawnShape&) copy;

  m_metafile.Copy(drawnCopy.m_metafile);
  drawnCopy.m_saveToFile = m_saveToFile;
}

bool wxDrawnShape::LoadFromMetaFile(char *filename)
{
  return m_metafile.LoadFromMetaFile(filename, &m_width, &m_height);
}

// Set of functions for drawing into a pseudo metafile.
// They use integers, but doubles are used internally for accuracy
// when scaling.

void wxDrawnShape::DrawLine(const wxPoint& pt1, const wxPoint& pt2)
{
    m_metafile.DrawLine(pt1, pt2);
}

void wxDrawnShape::DrawRectangle(const wxRect& rect)
{
    m_metafile.DrawRectangle(rect);
}

void wxDrawnShape::DrawRoundedRectangle(const wxRect& rect, double radius)
{
    m_metafile.DrawRoundedRectangle(rect, radius);
}

void wxDrawnShape::DrawEllipse(const wxRect& rect)
{
    m_metafile.DrawEllipse(rect);
}

void wxDrawnShape::DrawPoint(const wxPoint& pt)
{
    m_metafile.DrawPoint(pt);
}

void wxDrawnShape::DrawText(const wxString& text, const wxPoint& pt)
{
    m_metafile.DrawText(text, pt);
}

void wxDrawnShape::DrawLines(int n, wxPoint pts[])
{
    m_metafile.DrawLines(n, pts);
}

void wxDrawnShape::DrawPolygon(int n, wxPoint pts[])
{
    m_metafile.DrawPolygon(n, pts);
}

void wxDrawnShape::DrawSpline(int n, wxPoint pts[])
{
    m_metafile.DrawSpline(n, pts);
}

void wxDrawnShape::SetClippingRect(const wxRect& rect)
{
    m_metafile.SetClippingRect(rect);
}

void wxDrawnShape::DestroyClippingRect()
{
    m_metafile.DestroyClippingRect();
}

void wxDrawnShape::SetPen(wxPen* pen, bool isOutline)
{
    m_metafile.SetPen(pen, isOutline);
}

void wxDrawnShape::SetBrush(wxBrush* brush, bool isFill)
{
    m_metafile.SetBrush(brush, isFill);
}

void wxDrawnShape::SetFont(wxFont* font)
{
    m_metafile.SetFont(font);
}

void wxDrawnShape::SetTextColour(const wxColour& colour)
{
    m_metafile.SetTextColour(colour);
}

void wxDrawnShape::SetBackgroundColour(const wxColour& colour)
{
    m_metafile.SetBackgroundColour(colour);
}

void wxDrawnShape::SetBackgroundMode(int mode)
{
    m_metafile.SetBackgroundMode(mode);
}


/*
 * Individual operations
 *
 */
 
/*
 * Set font, brush, text colour
 *
 */
 
wxOpSetGDI::wxOpSetGDI(int theOp, wxPseudoMetaFile *theImage, int theGdiIndex, int theMode):
  wxDrawOp(theOp)
{
  m_gdiIndex = theGdiIndex;
  m_image = theImage;
  m_mode = theMode;
}

void wxOpSetGDI::Do(wxDC& dc, float xoffset, float yoffset)
{
  switch (m_op)
  {
    case DRAWOP_SET_PEN:
    {
      // Check for overriding this operation for outline
      // colour
      if (m_image->m_outlineColours.Member((wxObject *)m_gdiIndex))
      {
        if (m_image->m_outlinePen)
          dc.SetPen(m_image->m_outlinePen);
      }
      else
      {
        wxNode *node = m_image->m_gdiObjects.Nth(m_gdiIndex);
        if (node)
        {
          wxPen *pen = (wxPen *)node->Data();
          if (pen)
            dc.SetPen(pen);
        }
      }
      break;
    }
    case DRAWOP_SET_BRUSH:
    {
      // Check for overriding this operation for outline or fill
      // colour
      if (m_image->m_outlineColours.Member((wxObject *)m_gdiIndex))
      {
        // Need to construct a brush to match the outline pen's colour
        if (m_image->m_outlinePen)
        {
          wxBrush *br = wxTheBrushList->FindOrCreateBrush(m_image->m_outlinePen->GetColour(), wxSOLID);
          if (br)
            dc.SetBrush(br);
        }
      }
      else if (m_image->m_fillColours.Member((wxObject *)m_gdiIndex))
      {
        if (m_image->m_fillBrush)
        {
          dc.SetBrush(m_image->m_fillBrush);
        }
      }
      else
      {
        wxNode *node = m_image->m_gdiObjects.Nth(m_gdiIndex);
        if (node)
        {
          wxBrush *brush = (wxBrush *)node->Data();
          if (brush)
            dc.SetBrush(brush);
        }
      }
      break;
    }
    case DRAWOP_SET_FONT:
    {
      wxNode *node = m_image->m_gdiObjects.Nth(m_gdiIndex);
      if (node)
      {
        wxFont *font = (wxFont *)node->Data();
        if (font)
          dc.SetFont(font);
      }
      break;
    }
    case DRAWOP_SET_TEXT_COLOUR:
    {
      wxColour col(m_r,m_g,m_b);
      dc.SetTextForeground(col);
      break;
    }
    case DRAWOP_SET_BK_COLOUR:
    {
      wxColour col(m_r,m_g,m_b);
      dc.SetTextBackground(col);
      break;
    }
    case DRAWOP_SET_BK_MODE:
    {
      dc.SetBackgroundMode(m_mode);
      break;
    }
    default:
      break;
  }
}

wxDrawOp *wxOpSetGDI::Copy(wxPseudoMetaFile *newImage)
{
  wxOpSetGDI *newOp = new wxOpSetGDI(m_op, newImage, m_gdiIndex, m_mode);
  newOp->m_r = m_r;
  newOp->m_g = m_g;
  newOp->m_b = m_b;
  return newOp;
}

wxExpr *wxOpSetGDI::WriteExpr(wxPseudoMetaFile *image)
{
  wxExpr *expr = new wxExpr(PrologList);
  expr->Append(new wxExpr((long)m_op));
  switch (m_op)
  {
    case DRAWOP_SET_PEN:
    case DRAWOP_SET_BRUSH:
    case DRAWOP_SET_FONT:
    {
      expr->Append(new wxExpr((long)m_gdiIndex));
      break;
    }
    case DRAWOP_SET_TEXT_COLOUR:
    case DRAWOP_SET_BK_COLOUR:
    {
      expr->Append(new wxExpr((long)m_r));
      expr->Append(new wxExpr((long)m_g));
      expr->Append(new wxExpr((long)m_b));
      break;
    }
    case DRAWOP_SET_BK_MODE:
    {
      expr->Append(new wxExpr((long)m_mode));
      break;
    }
    default:
      break;
  }
  return expr;
}

void wxOpSetGDI::ReadExpr(wxPseudoMetaFile *image, wxExpr *expr)
{
  switch (m_op)
  {
    case DRAWOP_SET_PEN:
    case DRAWOP_SET_BRUSH:
    case DRAWOP_SET_FONT:
    {
      m_gdiIndex = (int)expr->Nth(1)->IntegerValue();
      break;
    }
    case DRAWOP_SET_TEXT_COLOUR:
    case DRAWOP_SET_BK_COLOUR:
    {
      m_r = (unsigned char)expr->Nth(1)->IntegerValue();
      m_g = (unsigned char)expr->Nth(2)->IntegerValue();
      m_b = (unsigned char)expr->Nth(3)->IntegerValue();
      break;
    }
    case DRAWOP_SET_BK_MODE:
    {
      m_mode = (int)expr->Nth(1)->IntegerValue();
      break;
    }
    default:
      break;
  }
}

/*
 * Set/destroy clipping
 *
 */
 
wxOpSetClipping::wxOpSetClipping(int theOp, float theX1, float theY1,
    float theX2, float theY2):wxDrawOp(theOp)
{
  m_x1 = theX1;
  m_y1 = theY1;
  m_x2 = theX2;
  m_y2 = theY2;
}

wxDrawOp *wxOpSetClipping::Copy(wxPseudoMetaFile *newImage)
{
  wxOpSetClipping *newOp = new wxOpSetClipping(m_op, m_x1, m_y1, m_x2, m_y2);
  return newOp;
}
    
void wxOpSetClipping::Do(wxDC& dc, float xoffset, float yoffset)
{
  switch (m_op)
  {
    case DRAWOP_SET_CLIPPING_RECT:
    {
      dc.SetClippingRegion(m_x1 + xoffset, m_y1 + yoffset, m_x2 + xoffset, m_y2 + yoffset);
      break;
    }
    case DRAWOP_DESTROY_CLIPPING_RECT:
    {
      dc.DestroyClippingRegion();
      break;
    }
    default:
      break;
  }
}

void wxOpSetClipping::Scale(float xScale, float yScale)
{
  m_x1 *= xScale;
  m_y1 *= yScale;
  m_x2 *= xScale;
  m_y2 *= yScale;
}

void wxOpSetClipping::Translate(float x, float y)
{
  m_x1 += x;
  m_y1 += y;
}

wxExpr *wxOpSetClipping::WriteExpr(wxPseudoMetaFile *image)
{
  wxExpr *expr = new wxExpr(PrologList);
  expr->Append(new wxExpr((long)m_op));
  switch (m_op)
  {
    case DRAWOP_SET_CLIPPING_RECT:
    {
      expr->Append(new wxExpr(m_x1));
      expr->Append(new wxExpr(m_y1));
      expr->Append(new wxExpr(m_x2));
      expr->Append(new wxExpr(m_y2));
      break;
    }
    default:
      break;
  }
  return expr;
}

void wxOpSetClipping::ReadExpr(wxPseudoMetaFile *image, wxExpr *expr)
{
  switch (m_op)
  {
    case DRAWOP_SET_CLIPPING_RECT:
    {
      m_x1 = expr->Nth(1)->RealValue();
      m_y1 = expr->Nth(2)->RealValue();
      m_x2 = expr->Nth(3)->RealValue();
      m_y2 = expr->Nth(4)->RealValue();
      break;
    }
    default:
      break;
  }
}

/*
 * Draw line, rectangle, rounded rectangle, ellipse, point, arc, text
 *
 */
 
wxOpDraw::wxOpDraw(int theOp, float theX1, float theY1, float theX2, float theY2,
         float theRadius, char *s):wxDrawOp(theOp)
{
  m_x1 = theX1;
  m_y1 = theY1;
  m_x2 = theX2;
  m_y2 = theY2;
  m_radius = theRadius;
  if (s) m_textString = copystring(s);
  else m_textString = NULL;
}

wxOpDraw::~wxOpDraw()
{
  if (m_textString) delete[] m_textString;
}

wxDrawOp *wxOpDraw::Copy(wxPseudoMetaFile *newImage)
{
  wxOpDraw *newOp = new wxOpDraw(m_op, m_x1, m_y1, m_x2, m_y2, m_radius, m_textString);
  return newOp;
}

void wxOpDraw::Do(wxDC& dc, float xoffset, float yoffset)
{
  switch (m_op)
  {
    case DRAWOP_DRAW_LINE:
    {
      dc.DrawLine(m_x1+xoffset, m_y1+yoffset, m_x2+xoffset, m_y2+yoffset);
      break;
    }
    case DRAWOP_DRAW_RECT:
    {
      dc.DrawRectangle(m_x1+xoffset, m_y1+yoffset, m_x2, m_y2);
      break;
    }
    case DRAWOP_DRAW_ROUNDED_RECT:
    {
      dc.DrawRoundedRectangle(m_x1+xoffset, m_y1+yoffset, m_x2, m_y2, m_radius);
      break;
    }
    case DRAWOP_DRAW_ELLIPSE:
    {
      dc.DrawEllipse(m_x1+xoffset, m_y1+yoffset, m_x2, m_y2);
      break;
    }
    case DRAWOP_DRAW_POINT:
    {
      dc.DrawPoint(m_x1+xoffset, m_y1+yoffset);
      break;
    }
    case DRAWOP_DRAW_TEXT:
    {
      dc.DrawText(m_textString, m_x1+xoffset, m_y1+yoffset);
      break;
    }
    default:
      break;
  }
}

void wxOpDraw::Scale(float scaleX, float scaleY)
{
  m_x1 *= scaleX;
  m_y1 *= scaleY;
  m_x2 *= scaleX;
  m_y2 *= scaleY;
  m_radius *= scaleX;
}

void wxOpDraw::Translate(float x, float y)
{
  m_x1 += x;
  m_y1 += y;
}

void wxOpDraw::Rotate(float x, float y, float sinTheta, float cosTheta)
{
  m_x1 = m_x1*cosTheta - m_y1*sinTheta + x*(1 - cosTheta) + y*sinTheta;
  m_y1 = m_x1*sinTheta + m_y1*cosTheta + y*(1 - cosTheta) + x*sinTheta;

  switch (m_op)
  {
    case DRAWOP_DRAW_LINE:
    {
      m_x2 = m_x2*cosTheta - m_y2*sinTheta + x*(1 - cosTheta) + y*sinTheta;
      m_y2 = m_x2*sinTheta + m_y2*cosTheta + y*(1 - cosTheta) + x*sinTheta;
      break;
    }
    default:
      break;
  }
}

wxExpr *wxOpDraw::WriteExpr(wxPseudoMetaFile *image)
{
  wxExpr *expr = new wxExpr(PrologList);
  expr->Append(new wxExpr((long)m_op));
  switch (m_op)
  {
    case DRAWOP_DRAW_LINE:
    case DRAWOP_DRAW_RECT:
    case DRAWOP_DRAW_ELLIPSE:
    {
      expr->Append(new wxExpr(m_x1));
      expr->Append(new wxExpr(m_y1));
      expr->Append(new wxExpr(m_x2));
      expr->Append(new wxExpr(m_y2));
      break;
    }
    case DRAWOP_DRAW_ROUNDED_RECT:
    {
      expr->Append(new wxExpr(m_x1));
      expr->Append(new wxExpr(m_y1));
      expr->Append(new wxExpr(m_x2));
      expr->Append(new wxExpr(m_y2));
      expr->Append(new wxExpr(m_radius));
      break;
    }
    case DRAWOP_DRAW_POINT:
    {
      expr->Append(new wxExpr(m_x1));
      expr->Append(new wxExpr(m_y1));
      break;
    }
    case DRAWOP_DRAW_TEXT:
    {
      expr->Append(new wxExpr(m_x1));
      expr->Append(new wxExpr(m_y1));
      expr->Append(new wxExpr(PrologString, m_textString));
      break;
    }
    case DRAWOP_DRAW_ARC:
    default:
    {
      break;
    }
  }
  return expr;
}

void wxOpDraw::ReadExpr(wxPseudoMetaFile *image, wxExpr *expr)
{
  switch (m_op)
  {
    case DRAWOP_DRAW_LINE:
    case DRAWOP_DRAW_RECT:
    case DRAWOP_DRAW_ELLIPSE:
    {
      m_x1 = expr->Nth(1)->RealValue();
      m_y1 = expr->Nth(2)->RealValue();
      m_x2 = expr->Nth(3)->RealValue();
      m_y2 = expr->Nth(4)->RealValue();
      break;
    }
    case DRAWOP_DRAW_ROUNDED_RECT:
    {
      m_x1 = expr->Nth(1)->RealValue();
      m_y1 = expr->Nth(2)->RealValue();
      m_x2 = expr->Nth(3)->RealValue();
      m_y2 = expr->Nth(4)->RealValue();
      m_radius = expr->Nth(5)->RealValue();
      break;
    }
    case DRAWOP_DRAW_POINT:
    {
      m_x1 = expr->Nth(1)->RealValue();
      m_y1 = expr->Nth(2)->RealValue();
      break;
    }
    case DRAWOP_DRAW_TEXT:
    {
      m_x1 = expr->Nth(1)->RealValue();
      m_y1 = expr->Nth(2)->RealValue();
      m_textString = copystring(expr->Nth(3)->StringValue());
      break;
    }
    case DRAWOP_DRAW_ARC:
    default:
    {
      break;
    }
  }
}

/*
 * Draw polygon, polyline, spline
 *
 */

wxOpPolyDraw::wxOpPolyDraw(int theOp, int n, wxRealPoint *thePoints):wxDrawOp(theOp)
{
  m_noPoints = n;
  m_points = thePoints;
}

wxOpPolyDraw::~wxOpPolyDraw()
{
  delete[] m_points;
}

wxDrawOp *wxOpPolyDraw::Copy(wxPseudoMetaFile *newImage)
{
  wxRealPoint *newPoints = new wxRealPoint[m_noPoints];
  for (int i = 0; i < m_noPoints; i++)
  {
    newPoints[i].x = m_points[i].x;
    newPoints[i].y = m_points[i].y;
  }
  wxOpPolyDraw *newOp = new wxOpPolyDraw(m_op, m_noPoints, newPoints);
  return newOp;
}

void wxOpPolyDraw::Do(wxDC& dc, float xoffset, float yoffset)
{
  switch (m_op)
  {
    case DRAWOP_DRAW_POLYLINE:
    {
        wxPoint *actualPoints = new wxPoint[m_noPoints];
        int i;
        for (i = 0; i < m_noPoints; i++)
        {
            actualPoints[i].x = (long) m_points[i].x;
            actualPoints[i].y = (long) m_points[i].y;
        }

        dc.DrawLines(m_noPoints, actualPoints, xoffset, yoffset);

        delete[] actualPoints;
        break;
    }
    case DRAWOP_DRAW_POLYGON:
    {
        wxPoint *actualPoints = new wxPoint[m_noPoints];
        int i;
        for (i = 0; i < m_noPoints; i++)
        {
            actualPoints[i].x = (long) m_points[i].x;
            actualPoints[i].y = (long) m_points[i].y;
        }

        dc.DrawPolygon(m_noPoints, actualPoints, xoffset, yoffset);

        delete[] actualPoints;
        break;
    }
    case DRAWOP_DRAW_SPLINE:
    {
        wxPoint *actualPoints = new wxPoint[m_noPoints];
        int i;
        for (i = 0; i < m_noPoints; i++)
        {
            actualPoints[i].x = (long) m_points[i].x;
            actualPoints[i].y = (long) m_points[i].y;
        }

        dc.DrawSpline(m_noPoints, actualPoints); // no offsets in DrawSpline // , xoffset, yoffset);

        delete[] actualPoints;
        break;
      break;
    }
    default:
      break;
  }
}

void wxOpPolyDraw::Scale(float scaleX, float scaleY)
{
  for (int i = 0; i < m_noPoints; i++)
  {
    m_points[i].x *= scaleX;
    m_points[i].y *= scaleY;
  }
}

void wxOpPolyDraw::Translate(float x, float y)
{
  for (int i = 0; i < m_noPoints; i++)
  {
    m_points[i].x += x;
    m_points[i].y += y;
  }
}

void wxOpPolyDraw::Rotate(float x, float y, float sinTheta, float cosTheta)
{
  for (int i = 0; i < m_noPoints; i++)
  {
    float x1 = m_points[i].x;
    float y1 = m_points[i].y;
    m_points[i].x = x1*cosTheta - y1*sinTheta + x*(1 - cosTheta) + y*sinTheta;
    m_points[i].y = x1*sinTheta + y1*cosTheta + y*(1 - cosTheta) + x*sinTheta;
  }
}

wxExpr *wxOpPolyDraw::WriteExpr(wxPseudoMetaFile *image)
{
  wxExpr *expr = new wxExpr(PrologList);
  expr->Append(new wxExpr((long)m_op));
  expr->Append(new wxExpr((long)m_noPoints));

//  char buf1[9];
  char buf2[5];
  char buf3[5];

  GraphicsBuffer[0] = 0;
  
  /*
   * Store each coordinate pair in a hex string to save space.
   * E.g. "1B9080CD". 4 hex digits per coordinate pair.
   *
   */
   
  for (int i = 0; i < m_noPoints; i++)
  {
    long signedX = (long)(m_points[i].x*100.0);
    long signedY = (long)(m_points[i].y*100.0);

    // Scale to 0 -> 64K
    long unSignedX = (long)(signedX + 32767.0);
    long unSignedY = (long)(signedY + 32767.0);
    
//    IntToHex((unsigned int)signedX, buf2);
//    IntToHex((unsigned int)signedY, buf3);
    IntToHex((int)unSignedX, buf2);
    IntToHex((int)unSignedY, buf3);

    // Don't overrun the buffer
    if ((i*8) < 3000)
    {
      strcat(GraphicsBuffer, buf2);
      strcat(GraphicsBuffer, buf3);
    }
  }
  expr->Append(new wxExpr(PrologString, GraphicsBuffer));
  return expr;
}

void wxOpPolyDraw::ReadExpr(wxPseudoMetaFile *image, wxExpr *expr)
{
  m_noPoints = (int)expr->Nth(1)->IntegerValue();

  char buf1[5];
  char buf2[5];

  m_points = new wxRealPoint[m_noPoints];
  int i = 0;
  int bufPtr = 0;
  wxString hexString = expr->Nth(2)->StringValue();
  while (i < m_noPoints)
  {
    buf1[0] = hexString[bufPtr];
    buf1[1] = hexString[bufPtr + 1];
    buf1[2] = hexString[bufPtr + 2];
    buf1[3] = hexString[bufPtr + 3];
    buf1[4] = 0;
    
    buf2[0] = hexString[bufPtr + 4];
    buf2[1] = hexString[bufPtr + 5];
    buf2[2] = hexString[bufPtr + 6];
    buf2[3] = hexString[bufPtr + 7];
    buf2[4] = 0;

    bufPtr += 8;

//    int signedX = (signed int)HexToInt(buf1);
//    int signedY = (signed int)HexToInt(buf2);
    long unSignedX = HexToInt(buf1);
    long unSignedY = HexToInt(buf2);
    // Scale -32K -> +32K
    long signedX = unSignedX - 32767;
    long signedY = unSignedY - 32767;
#ifdef __WXMSW__
    int testX = (signed int)unSignedX;
    int testY = (signed int)unSignedY;
#endif

    m_points[i].x = (float)(signedX / 100.0);
    m_points[i].y = (float)(signedY / 100.0);

    i ++;
  }
}


/*
 * Utilities
 *
 */

static char hexArray[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B',
  'C', 'D', 'E', 'F' };

// Convert unsigned 16-bit integer to 4-character hex string
static void IntToHex(unsigned int dec, char *buf)
{
  int digit1 = (int)(dec/4096);
  int digit2 = (int)((dec - (digit1*4096))/256);
  int digit3 = (int)((dec - (digit1*4096) - (digit2*256))/16);
  int digit4 = dec - (digit1*4096 + digit2*256 + digit3*16);
  
  buf[0] = hexArray[digit1];
  buf[1] = hexArray[digit2];
  buf[2] = hexArray[digit3];
  buf[3] = hexArray[digit4];
  buf[4] = 0;
}

// One hex digit to decimal number
static int HexToInt1(char hex)
{
  switch (hex)
  {
    case '0':
      return 0;
    case '1':
      return 1;
    case '2':
      return 2;
    case '3':
      return 3;
    case '4':
      return 4;
    case '5':
      return 5;
    case '6':
      return 6;
    case '7':
      return 7;
    case '8':
      return 8;
    case '9':
      return 9;
    case 'A':
      return 10;
    case 'B':
      return 11;
    case 'C':
      return 12;
    case 'D':
      return 13;
    case 'E':
      return 14;
    case 'F':
      return 15;
    default:
      return 0;
  }
  return 0;
}

// 4-digit hex string to unsigned integer
static unsigned long HexToInt(char *buf)
{
  long d1 = (long)(HexToInt1(buf[0])*4096.0) ;
  long d2 = (long)(HexToInt1(buf[1])*256.0) ;
  long d3 = (long)(HexToInt1(buf[2])*16.0) ;
  long d4 = (long)(HexToInt1(buf[3])) ;
  unsigned long n = (long)(d1 + d2 + d3 + d4) ;
  return n;
}

/*
 * wxPseudo meta-file
 *
 */

IMPLEMENT_DYNAMIC_CLASS(wxPseudoMetaFile, wxObject)

wxPseudoMetaFile::wxPseudoMetaFile()
{
  m_currentRotation = 0;
  m_rotateable = TRUE;
  m_width = 0.0;
  m_height = 0.0;
  m_outlinePen = NULL;
  m_fillBrush = NULL;
}

wxPseudoMetaFile::wxPseudoMetaFile(wxPseudoMetaFile& mf)
{
  mf.Copy(*this);
}

wxPseudoMetaFile::~wxPseudoMetaFile()
{
  Clear();
}

void wxPseudoMetaFile::Clear()
{
  wxNode *node = m_ops.First();
  while (node)
  {
    wxDrawOp *op = (wxDrawOp *)node->Data();
    delete op;
    node = node->Next();
  }
  m_ops.Clear();
  m_gdiObjects.Clear();
  m_outlineColours.Clear();
  m_fillColours.Clear();
}

void wxPseudoMetaFile::Draw(wxDC& dc, float xoffset, float yoffset)
{
  wxNode *node = m_ops.First();
  while (node)
  {
    wxDrawOp *op = (wxDrawOp *)node->Data();
    op->Do(dc, xoffset, yoffset);
    node = node->Next();
  }
}

void wxPseudoMetaFile::Scale(float sx, float sy)
{
  wxNode *node = m_ops.First();
  while (node)
  {
    wxDrawOp *op = (wxDrawOp *)node->Data();
    op->Scale(sx, sy);
    node = node->Next();
  }
  m_width *= sx;
  m_height *= sy;
}

void wxPseudoMetaFile::Translate(float x, float y)
{
  wxNode *node = m_ops.First();
  while (node)
  {
    wxDrawOp *op = (wxDrawOp *)node->Data();
    op->Translate(x, y);
    node = node->Next();
  }
}

void wxPseudoMetaFile::Rotate(float x, float y, float theta)
{
  float theta1 = theta-m_currentRotation;
  if (theta1 == 0.0) return;
  float cosTheta = (float)cos(theta1);
  float sinTheta = (float)sin(theta1);

  wxNode *node = m_ops.First();
  while (node)
  {
    wxDrawOp *op = (wxDrawOp *)node->Data();
    op->Rotate(x, y, sinTheta, cosTheta);
    node = node->Next();
  }
  m_currentRotation = theta;
}

#ifdef PROLOGIO
void wxPseudoMetaFile::WritePrologAttributes(wxExpr *clause)
{
  // Write width and height
  clause->AddAttributeValue("meta_width", m_width);
  clause->AddAttributeValue("meta_height", m_height);
  clause->AddAttributeValue("meta_rotateable", (long)m_rotateable);

  // Write GDI objects
  char buf[50];
  int i = 1;
  wxNode *node = m_gdiObjects.First();
  while (node)
  {
    sprintf(buf, "gdi%d", i);
    wxObject *obj = (wxObject *)node->Data();
    wxExpr *expr = NULL;
    if (obj)
    {
      if (obj->IsKindOf(CLASSINFO(wxPen)))
      {
        wxPen *thePen = (wxPen *)obj;
        expr = new wxExpr(PrologList);
        expr->Append(new wxExpr((long)gyTYPE_PEN));
        expr->Append(new wxExpr((long)thePen->GetWidth()));
        expr->Append(new wxExpr((long)thePen->GetStyle()));
        expr->Append(new wxExpr((long)thePen->GetColour().Red()));
        expr->Append(new wxExpr((long)thePen->GetColour().Green()));
        expr->Append(new wxExpr((long)thePen->GetColour().Blue()));
      }
      else if (obj->IsKindOf(CLASSINFO(wxBrush)))
      {
        wxBrush *theBrush = (wxBrush *)obj;
        expr = new wxExpr(PrologList);
        expr->Append(new wxExpr((long)gyTYPE_BRUSH));
        expr->Append(new wxExpr((long)theBrush->GetStyle()));
        expr->Append(new wxExpr((long)theBrush->GetColour().Red()));
        expr->Append(new wxExpr((long)theBrush->GetColour().Green()));
        expr->Append(new wxExpr((long)theBrush->GetColour().Blue()));
      }
      else if (obj->IsKindOf(CLASSINFO(wxFont)))
      {
        wxFont *theFont = (wxFont *)obj;
        expr = new wxExpr(PrologList);
        expr->Append(new wxExpr((long)gyTYPE_FONT));
        expr->Append(new wxExpr((long)theFont->GetPointSize()));
        expr->Append(new wxExpr((long)theFont->GetFamily()));
        expr->Append(new wxExpr((long)theFont->GetStyle()));
        expr->Append(new wxExpr((long)theFont->GetWeight()));
        expr->Append(new wxExpr((long)theFont->GetUnderlined()));
      }
    }
    else
    {
      // If no recognised GDI object, append a place holder anyway.
      expr = new wxExpr(PrologList);
      expr->Append(new wxExpr((long)0));
    }

    if (expr)
    {
      clause->AddAttributeValue(buf, expr);
      i ++;
    }
    node = node->Next();
  }

  // Write drawing operations
  i = 1;
  node = m_ops.First();
  while (node)
  {
    sprintf(buf, "op%d", i);
    wxDrawOp *op = (wxDrawOp *)node->Data();
    wxExpr *expr = op->WriteExpr(this);
    if (expr)
    {
      clause->AddAttributeValue(buf, expr);
      i ++;
    }
    node = node->Next();
  }

  // Write outline and fill GDI op lists (if any)
  if (m_outlineColours.Number() > 0)
  {
    wxExpr *outlineExpr = new wxExpr(PrologList);
    node = m_outlineColours.First();
    while (node)
    {
      outlineExpr->Append(new wxExpr((long)node->Data()));
      node = node->Next();
    }
    clause->AddAttributeValue("outline_objects", outlineExpr);
  }
  if (m_fillColours.Number() > 0)
  {
    wxExpr *fillExpr = new wxExpr(PrologList);
    node = m_fillColours.First();
    while (node)
    {
      fillExpr->Append(new wxExpr((long)node->Data()));
      node = node->Next();
    }
    clause->AddAttributeValue("fill_objects", fillExpr);
  }
    
}

void wxPseudoMetaFile::ReadPrologAttributes(wxExpr *clause)
{
  clause->AssignAttributeValue("meta_width", &m_width);
  clause->AssignAttributeValue("meta_height", &m_height);

  int iVal = (int) m_rotateable;
  clause->AssignAttributeValue("meta_rotateable", &iVal);
  m_rotateable = (iVal != 0);

  // Read GDI objects
  char buf[50];
  int i = 1;
  bool keepGoing = TRUE;
  while (keepGoing)
  {
    sprintf(buf, "gdi%d", i);
    wxExpr *expr = NULL;
    clause->AssignAttributeValue(buf, &expr);
    if (!expr)
    {
      keepGoing = FALSE;
    }
    else
    {
      wxExpr *idExpr = expr->Nth(0);
      switch (idExpr->IntegerValue())
      {
        case gyTYPE_PEN:
        {
          int penWidth = (int)expr->Nth(1)->IntegerValue();
          int penStyle = (int)expr->Nth(2)->IntegerValue();
          int penRed = (int)expr->Nth(3)->IntegerValue();
          int penGreen = (int)expr->Nth(4)->IntegerValue();
          int penBlue = (int)expr->Nth(5)->IntegerValue();
          wxColour col(penRed, penGreen, penBlue);
          wxPen *p = wxThePenList->FindOrCreatePen(col, penWidth, penStyle);
          if (!p)
            p = wxBLACK_PEN;
          m_gdiObjects.Append(p);
          break;
        }
        case gyTYPE_BRUSH:
        {
          int brushStyle = (int)expr->Nth(1)->IntegerValue();
          int brushRed = (int)expr->Nth(2)->IntegerValue();
          int brushGreen = (int)expr->Nth(3)->IntegerValue();
          int brushBlue = (int)expr->Nth(4)->IntegerValue();
          wxColour col(brushRed, brushGreen, brushBlue);
          wxBrush *b = wxTheBrushList->FindOrCreateBrush(col, brushStyle);
          if (!b)
            b = wxWHITE_BRUSH;
          m_gdiObjects.Append(b);
          break;
        }
        case gyTYPE_FONT:
        {
          int fontPointSize = (int)expr->Nth(1)->IntegerValue();
          int fontFamily = (int)expr->Nth(2)->IntegerValue();
          int fontStyle = (int)expr->Nth(3)->IntegerValue();
          int fontWeight = (int)expr->Nth(4)->IntegerValue();
          int fontUnderlined = (int)expr->Nth(5)->IntegerValue();
          m_gdiObjects.Append(wxTheFontList->FindOrCreateFont(fontPointSize,
                           fontFamily, fontStyle, fontWeight, (fontUnderlined != 0)));
          break;
        }
        default:
        {
          // Place holder
          m_gdiObjects.Append(NULL);
          break;
        }
      }
      i ++;
    }
  }

  // Now read in the operations
  keepGoing = TRUE;
  i = 1;
  while (keepGoing)
  {
    sprintf(buf, "op%d", i);
    wxExpr *expr = NULL;
    clause->AssignAttributeValue(buf, &expr);
    if (!expr)
    {
      keepGoing = FALSE;
    }
    else
    {
      wxExpr *idExpr = expr->Nth(0);
      int opId = (int)idExpr->IntegerValue();
      switch (opId)
      {
        case DRAWOP_SET_PEN:
        case DRAWOP_SET_BRUSH:
        case DRAWOP_SET_FONT:
        case DRAWOP_SET_TEXT_COLOUR:
        case DRAWOP_SET_BK_COLOUR:
        case DRAWOP_SET_BK_MODE:
        {
          wxOpSetGDI *theOp = new wxOpSetGDI(opId, this, 0);
          theOp->ReadExpr(this, expr);
          m_ops.Append(theOp);
          break;
        }
        
        case DRAWOP_SET_CLIPPING_RECT:
        case DRAWOP_DESTROY_CLIPPING_RECT:
        {
          wxOpSetClipping *theOp = new wxOpSetClipping(opId, 0.0, 0.0, 0.0, 0.0);
          theOp->ReadExpr(this, expr);
          m_ops.Append(theOp);
          break;
        }

        case DRAWOP_DRAW_LINE:
        case DRAWOP_DRAW_RECT:
        case DRAWOP_DRAW_ROUNDED_RECT:
        case DRAWOP_DRAW_ELLIPSE:
        case DRAWOP_DRAW_POINT:
        case DRAWOP_DRAW_ARC:
        case DRAWOP_DRAW_TEXT:
        {
          wxOpDraw *theOp = new wxOpDraw(opId, 0.0, 0.0, 0.0, 0.0);
          theOp->ReadExpr(this, expr);
          m_ops.Append(theOp);
          break;
        }
        case DRAWOP_DRAW_SPLINE:
        case DRAWOP_DRAW_POLYLINE:
        case DRAWOP_DRAW_POLYGON:
        {
          wxOpPolyDraw *theOp = new wxOpPolyDraw(opId, 0, NULL);
          theOp->ReadExpr(this, expr);
          m_ops.Append(theOp);
          break;
        }
        default:
          break;
      }
    }
    i ++;
  }

  // Now read in the list of outline and fill operations, if any
  wxExpr *expr1 = clause->AttributeValue("outline_objects");
  if (expr1)
  {
    wxExpr *eachExpr = expr1->GetFirst();
    while (eachExpr)
    {
      m_outlineColours.Append((wxObject *)eachExpr->IntegerValue());
      eachExpr = eachExpr->GetNext();
    }
  }
  expr1 = clause->AttributeValue("fill_objects");
  if (expr1)
  {
    wxExpr *eachExpr = expr1->GetFirst();
    while (eachExpr)
    {
      m_fillColours.Append((wxObject *)eachExpr->IntegerValue());
      eachExpr = eachExpr->GetNext();
    }
  }
}
#endif

// Does the copying for this object
void wxPseudoMetaFile::Copy(wxPseudoMetaFile& copy)
{
  copy.m_currentRotation = m_currentRotation;
  copy.m_width = m_width;
  copy.m_height = m_height;
  copy.m_rotateable = m_rotateable;
  copy.m_fillBrush = m_fillBrush;
  copy.m_outlinePen = m_outlinePen;

  copy.Clear();

  // Copy the GDI objects
  wxNode *node = m_gdiObjects.First();
  while (node)
  {
    wxObject *obj = (wxObject *)node->Data();
    copy.m_gdiObjects.Append(obj);
    node = node->Next();
  }
  
  // Copy the operations
  node = m_ops.First();
  while (node)
  {
    wxDrawOp *op = (wxDrawOp *)node->Data();
    copy.m_ops.Append(op->Copy(&copy));
    node = node->Next();
  }

  // Copy the outline/fill operations
  node = m_outlineColours.First();
  while (node)
  {
    copy.m_outlineColours.Append((wxObject *)node->Data());
    node = node->Next();
  }
  node = m_fillColours.First();
  while (node)
  {
    copy.m_fillColours.Append((wxObject *)node->Data());
    node = node->Next();
  }
}

/*
 * Pass size of existing image; scale height to
 * fit width and return new width and height.
 *
 */
 
bool wxPseudoMetaFile::LoadFromMetaFile(char *filename, float *rwidth, float *rheight)
{
  if (!FileExists(filename))
    return NULL;
    
  wxXMetaFile *metaFile = new wxXMetaFile;
  
  if (!metaFile->ReadFile(filename))
  {
    delete metaFile;
    return FALSE;
  }

  float lastX = 0.0;
  float lastY = 0.0;

  // Convert from metafile records to wxDrawnShape records
  wxNode *node = metaFile->metaRecords.First();
  while (node)
  {
    wxMetaRecord *record = (wxMetaRecord *)node->Data();
    switch (record->metaFunction)
    {
      case META_SETBKCOLOR:
      {
        wxOpSetGDI *op = new wxOpSetGDI(DRAWOP_SET_BK_COLOUR, this, 0);
        op->m_r = (unsigned char)record->param1;
        op->m_g = (unsigned char)record->param2;
        op->m_b = (unsigned char)record->param3;
        m_ops.Append(op);
        break;
      }
      case META_SETBKMODE:
      {
        wxOpSetGDI *op = new wxOpSetGDI(DRAWOP_SET_BK_MODE, this, 0, (int)record->param1);
        m_ops.Append(op);
        break;
      }
      case META_SETMAPMODE:
      {
        break;
      }
//      case META_SETROP2:
//      case META_SETRELABS:
//      case META_SETPOLYFILLMODE:
//      case META_SETSTRETCHBLTMODE:
//      case META_SETTEXTCHAREXTRA:
      case META_SETTEXTCOLOR:
      {
        wxOpSetGDI *op = new wxOpSetGDI(DRAWOP_SET_TEXT_COLOUR, this, 0);
        op->m_r = (unsigned char)record->param1;
        op->m_g = (unsigned char)record->param2;
        op->m_b = (unsigned char)record->param3;
        m_ops.Append(op);
        break;
      }
//      case META_SETTEXTJUSTIFICATION:
//      case META_SETWINDOWORG:
//      case META_SETWINDOWEXT:
//      case META_SETVIEWPORTORG:
//      case META_SETVIEWPORTEXT:
//      case META_OFFSETWINDOWORG:
//      case META_SCALEWINDOWEXT:
//      case META_OFFSETVIEWPORTORG:
//      case META_SCALEVIEWPORTEXT:
      case META_LINETO:
      {
        wxOpDraw *op = new wxOpDraw(DRAWOP_DRAW_LINE, (float)lastX, (float)lastY,
                               (float)record->param1, (float)record->param2);
        m_ops.Append(op);
        break;
      }
      case META_MOVETO:
      {
        lastX = (float)record->param1;
        lastY = (float)record->param2;
        break;
      }
      case META_EXCLUDECLIPRECT:
      {
/*
        wxMetaRecord *rec = new wxMetaRecord(META_EXCLUDECLIPRECT);
        rec->param4 = getshort(handle); // m_y2
        rec->param3 = getshort(handle); // x2
        rec->param2 = getshort(handle); // y1
        rec->param1 = getshort(handle); // x1
*/
        break;
      }
      case META_INTERSECTCLIPRECT:
      {
/*
        rec->param4 = getshort(handle); // m_y2
        rec->param3 = getshort(handle); // x2
        rec->param2 = getshort(handle); // y1
        rec->param1 = getshort(handle); // x1
*/
        break;
      }
//      case META_ARC: // DO!!!
      case META_ELLIPSE:
      {
        wxOpDraw *op = new wxOpDraw(DRAWOP_DRAW_ELLIPSE,
                               (float)record->param1, (float)record->param2,
                               (float)(record->param3 - record->param1),
                               (float)(record->param4 - record->param2));
        m_ops.Append(op);
        break;
      }
//      case META_FLOODFILL:
//      case META_PIE: // DO!!!
      case META_RECTANGLE:
      {
        wxOpDraw *op = new wxOpDraw(DRAWOP_DRAW_RECT,
                               (float)record->param1, (float)record->param2,
                               (float)(record->param3 - record->param1),
                               (float)(record->param4 - record->param2));
        m_ops.Append(op);
        break;
      }
      case META_ROUNDRECT:
      {
        wxOpDraw *op = new wxOpDraw(DRAWOP_DRAW_ROUNDED_RECT,
              (float)record->param1, (float)record->param2,
              (float)(record->param3 - record->param1),
              (float)(record->param4 - record->param2), (float)record->param5);
        m_ops.Append(op);
        break;
      }
//      case META_PATBLT:
//      case META_SAVEDC:
      case META_SETPIXEL:
      {
        wxOpDraw *op = new wxOpDraw(DRAWOP_DRAW_POINT,
              (float)record->param1, (float)record->param2,
              0.0, 0.0);

//        SHOULD SET THE COLOUR - SET PEN?
//        rec->param3 = getint(handle);   // COLORREF
        m_ops.Append(op);
        break;
      }
//      case META_OFFSETCLIPRGN:
      case META_TEXTOUT:
      {
        wxOpDraw *op = new wxOpDraw(DRAWOP_DRAW_TEXT,
              (float)record->param1, (float)record->param2,
              0.0, 0.0, 0.0, record->stringParam);
        m_ops.Append(op);
        break;
      }
//      case META_BITBLT:
//      case META_STRETCHBLT:
      case META_POLYGON:
      {
        int n = (int)record->param1;
        wxRealPoint *newPoints = new wxRealPoint[n];
        for (int i = 0; i < n; i++)
        {
          newPoints[i].x = record->points[i].x;
          newPoints[i].y = record->points[i].y;
        }
        
        wxOpPolyDraw *op = new wxOpPolyDraw(DRAWOP_DRAW_POLYGON, n, newPoints);
        m_ops.Append(op);
        break;
      }
      case META_POLYLINE:
      {
        int n = (int)record->param1;
        wxRealPoint *newPoints = new wxRealPoint[n];
        for (int i = 0; i < n; i++)
        {
          newPoints[i].x = record->points[i].x;
          newPoints[i].y = record->points[i].y;
        }
        
        wxOpPolyDraw *op = new wxOpPolyDraw(DRAWOP_DRAW_POLYLINE, n, newPoints);
        m_ops.Append(op);
        break;
      }
//      case META_ESCAPE:
//      case META_RESTOREDC:
//      case META_FILLREGION:
//      case META_FRAMEREGION:
//      case META_INVERTREGION:
//      case META_PAINTREGION:
//      case META_SELECTCLIPREGION: // DO THIS!
      case META_SELECTOBJECT:
      {
        // The pen, brush etc. has already been created when the metafile
        // was read in, so we don't create it - we set it.
        wxNode *recNode = metaFile->gdiObjects.Nth((int)record->param2);
        if (recNode)
        {
          wxMetaRecord *gdiRec = (wxMetaRecord *)recNode->Data();
          if (gdiRec && (gdiRec->param1 != 0))
          {
            wxObject *obj = (wxObject *)gdiRec->param1;
            if (obj->IsKindOf(CLASSINFO(wxPen)))
            {
              wxOpSetGDI *op = new wxOpSetGDI(DRAWOP_SET_PEN, this, (int)record->param2);
              m_ops.Append(op);
            }
            else if (obj->IsKindOf(CLASSINFO(wxBrush)))
            {
              wxOpSetGDI *op = new wxOpSetGDI(DRAWOP_SET_BRUSH, this, (int)record->param2);
              m_ops.Append(op);
            }
            else if (obj->IsKindOf(CLASSINFO(wxFont)))
            {
              wxOpSetGDI *op = new wxOpSetGDI(DRAWOP_SET_FONT, this, (int)record->param2);
              m_ops.Append(op);
            }
          }
        }
        break;
      }
//      case META_SETTEXTALIGN:
//      case META_DRAWTEXT:
//      case META_CHORD:
//      case META_SETMAPPERFLAGS:
//      case META_EXTTEXTOUT:
//      case META_SETDIBTODEV:
//      case META_SELECTPALETTE:
//      case META_REALIZEPALETTE:
//      case META_ANIMATEPALETTE:
//      case META_SETPALENTRIES:
//      case META_POLYPOLYGON:
//      case META_RESIZEPALETTE:
//      case META_DIBBITBLT:
//      case META_DIBSTRETCHBLT:
      case META_DIBCREATEPATTERNBRUSH:
      {
        // Place holder
        m_gdiObjects.Append(NULL);
        break;
      }
//      case META_STRETCHDIB:
//      case META_EXTFLOODFILL:
//      case META_RESETDC:
//      case META_STARTDOC:
//      case META_STARTPAGE:
//      case META_ENDPAGE:
//      case META_ABORTDOC:
//      case META_ENDDOC:
//      case META_DELETEOBJECT: // DO!!
      case META_CREATEPALETTE:
      {
        // Place holder
        m_gdiObjects.Append(NULL);
        break;
      }
      case META_CREATEBRUSH:
      {
        // Place holder
        m_gdiObjects.Append(NULL);
        break;
      }
      case META_CREATEPATTERNBRUSH:
      {
        // Place holder
        m_gdiObjects.Append(NULL);
        break;
      }
      case META_CREATEPENINDIRECT:
      {
        // The pen is created when the metafile is read in.
        // We keep track of all the GDI objects needed for this
        // image so when reading the wxDrawnShape from file,
        // we can read in all the GDI objects, then refer
        // to them by an index starting from zero thereafter.
        m_gdiObjects.Append((wxObject *)record->param1);
        break;
      }
      case META_CREATEFONTINDIRECT:
      {
        m_gdiObjects.Append((wxObject *)record->param1);
        break;
      }
      case META_CREATEBRUSHINDIRECT:
      {
        // Don't have to do anything here: the pen is created
        // when the metafile is read in.
        m_gdiObjects.Append((wxObject *)record->param1);
        break;
      }
      case META_CREATEBITMAPINDIRECT:
      {
        // Place holder
        m_gdiObjects.Append(NULL);
        break;
      }
      case META_CREATEBITMAP:
      {
        // Place holder
        m_gdiObjects.Append(NULL);
        break;
      }
      case META_CREATEREGION:
      {
        // Place holder
        m_gdiObjects.Append(NULL);
        break;
      }
      default:
      {
        break;
      }
    }
    node = node->Next();
  }
  float actualWidth = (float)fabs(metaFile->right - metaFile->left);
  float actualHeight = (float)fabs(metaFile->bottom - metaFile->top);
  
  float initialScaleX = 1.0;
  float initialScaleY = 1.0;

  float xoffset, yoffset;

  // Translate so origin is at centre of rectangle
  if (metaFile->bottom > metaFile->top)
    yoffset = - (float)((metaFile->bottom - metaFile->top)/2.0);
  else
    yoffset = - (float)((metaFile->top - metaFile->bottom)/2.0);

  if (metaFile->right > metaFile->left)
    xoffset = - (float)((metaFile->right - metaFile->left)/2.0);
  else
    xoffset = - (float)((metaFile->left - metaFile->right)/2.0);

  Translate(xoffset, yoffset);

  // Scale to a reasonable size (take the width of this wxDrawnShape
  // as a guide)
  if (actualWidth != 0.0)
  {
    initialScaleX = (float)((*rwidth) / actualWidth);
    initialScaleY = initialScaleX;
    (*rheight) = initialScaleY*actualHeight;
  }
  Scale(initialScaleX, initialScaleY);

  m_width = (actualWidth*initialScaleX);
  m_height = *rheight;

  delete metaFile;
  return TRUE;
}

// Scale to fit size
void wxPseudoMetaFile::ScaleTo(float w, float h)
{
  float scaleX = (float)(w/m_width);
  float scaleY = (float)(h/m_height);

  // Do the scaling
  Scale(scaleX, scaleY);
}

void wxPseudoMetaFile::GetBounds(float *boundMinX, float *boundMinY, float *boundMaxX, float *boundMaxY)
{
  float maxX = (float) -99999.9;
  float maxY = (float) -99999.9;
  float minX = (float) 99999.9;
  float minY = (float) 99999.9;

  wxNode *node = m_ops.First();
  while (node)
  {
    wxDrawOp *op = (wxDrawOp *)node->Data();
    switch (op->GetOp())
    {
      case DRAWOP_DRAW_LINE:
      case DRAWOP_DRAW_RECT:
      case DRAWOP_DRAW_ROUNDED_RECT:
      case DRAWOP_DRAW_ELLIPSE:
      case DRAWOP_DRAW_POINT:
      case DRAWOP_DRAW_ARC:
      case DRAWOP_DRAW_TEXT:
      {
        wxOpDraw *opDraw = (wxOpDraw *)op;
        if (opDraw->m_x1 < minX) minX = opDraw->m_x1;
        if (opDraw->m_x1 > maxX) maxX = opDraw->m_x1;
        if (opDraw->m_y1 < minY) minY = opDraw->m_y1;
        if (opDraw->m_y1 > maxY) maxY = opDraw->m_y1;
        if (op->GetOp() == DRAWOP_DRAW_LINE)
        {
          if (opDraw->m_x2 < minX) minX = opDraw->m_x2;
          if (opDraw->m_x2 > maxX) maxX = opDraw->m_x2;
          if (opDraw->m_y2 < minY) minY = opDraw->m_y2;
          if (opDraw->m_y2 > maxY) maxY = opDraw->m_y2;
        }
        else if (op->GetOp() == DRAWOP_DRAW_RECT ||
                 op->GetOp() == DRAWOP_DRAW_ROUNDED_RECT ||
                 op->GetOp() == DRAWOP_DRAW_ELLIPSE)
        {
          if ((opDraw->m_x1 + opDraw->m_x2) < minX) minX = (opDraw->m_x1 + opDraw->m_x2);
          if ((opDraw->m_x1 + opDraw->m_x2) > maxX) maxX = (opDraw->m_x1 + opDraw->m_x2);
          if ((opDraw->m_y1 + opDraw->m_y2) < minY) minY = (opDraw->m_y1 + opDraw->m_y2);
          if ((opDraw->m_y1 + opDraw->m_y2) > maxY) maxY = (opDraw->m_y1 + opDraw->m_y2);
        }
        break;
      }
      case DRAWOP_DRAW_POLYLINE:
      case DRAWOP_DRAW_POLYGON:
      case DRAWOP_DRAW_SPLINE:
      {
        wxOpPolyDraw *poly = (wxOpPolyDraw *)op;
        for (int i = 0; i < poly->m_noPoints; i++)
        {
          if (poly->m_points[i].x < minX) minX = poly->m_points[i].x;
          if (poly->m_points[i].x > maxX) maxX = poly->m_points[i].x;
          if (poly->m_points[i].y < minY) minY = poly->m_points[i].y;
          if (poly->m_points[i].y > maxY) maxY = poly->m_points[i].y;
        }
        break;
      }
      default:
        break;
    }
    node = node->Next();
  }

  *boundMinX = minX;
  *boundMinY = minY;
  *boundMaxX = maxX;
  *boundMaxY = maxY;
/*
  *w = (float)fabs(maxX - minX);
  *h = (float)fabs(maxY - minY);
*/
}

// Calculate size from current operations
void wxPseudoMetaFile::CalculateSize(wxDrawnShape* shape)
{
  float boundMinX, boundMinY, boundMaxX, boundMaxY;

  GetBounds(& boundMinX, & boundMinY, & boundMaxX, & boundMaxY);

  SetSize(boundMaxX - boundMinX, boundMaxY - boundMinY);

  if (shape)
  {
    shape->SetWidth(m_width);
    shape->SetHeight(m_height);
  }
}

// Set of functions for drawing into a pseudo metafile.
// They use integers, but doubles are used internally for accuracy
// when scaling.

void wxPseudoMetaFile::DrawLine(const wxPoint& pt1, const wxPoint& pt2)
{
    wxOpDraw *theOp = new wxOpDraw(DRAWOP_DRAW_LINE,
          (double) pt1.x, (double) pt1.y, (double) pt2.x, (double) pt2.y);

    m_ops.Append(theOp);
}

void wxPseudoMetaFile::DrawRectangle(const wxRect& rect)
{
    wxOpDraw *theOp = new wxOpDraw(DRAWOP_DRAW_RECT,
          (double) rect.x, (double) rect.y, (double) rect.width, (double) rect.height);

    m_ops.Append(theOp);
}

void wxPseudoMetaFile::DrawRoundedRectangle(const wxRect& rect, double radius)
{
    wxOpDraw *theOp = new wxOpDraw(DRAWOP_DRAW_ROUNDED_RECT,
          (double) rect.x, (double) rect.y, (double) rect.width, (double) rect.height);

    theOp->m_radius = radius;

    m_ops.Append(theOp);
}

void wxPseudoMetaFile::DrawEllipse(const wxRect& rect)
{
    wxOpDraw *theOp = new wxOpDraw(DRAWOP_DRAW_ELLIPSE,
          (double) rect.x, (double) rect.y, (double) rect.width, (double) rect.height);

    m_ops.Append(theOp);
}

void wxPseudoMetaFile::DrawPoint(const wxPoint& pt)
{
    wxOpDraw *theOp = new wxOpDraw(DRAWOP_DRAW_POINT,
          (double) pt.x, (double) pt.y, 0.0, 0.0);

    m_ops.Append(theOp);
}

void wxPseudoMetaFile::DrawText(const wxString& text, const wxPoint& pt)
{
    wxOpDraw *theOp = new wxOpDraw(DRAWOP_DRAW_TEXT,
          (double) pt.x, (double) pt.y, 0.0, 0.0);

    theOp->m_textString = copystring(text);

    m_ops.Append(theOp);
}

void wxPseudoMetaFile::DrawLines(int n, wxPoint pts[])
{
    wxRealPoint* realPoints = new wxRealPoint[n];
    int i;
    for (i = 0; i < n; i++)
    {
        realPoints[i].x = pts[i].x;
        realPoints[i].y = pts[i].y;
    }
    wxOpPolyDraw* theOp = new wxOpPolyDraw(DRAWOP_DRAW_POLYLINE, n, realPoints);
    m_ops.Append(theOp);
}

void wxPseudoMetaFile::DrawPolygon(int n, wxPoint pts[])
{
    wxRealPoint* realPoints = new wxRealPoint[n];
    int i;
    for (i = 0; i < n; i++)
    {
        realPoints[i].x = pts[i].x;
        realPoints[i].y = pts[i].y;
    }
    wxOpPolyDraw* theOp = new wxOpPolyDraw(DRAWOP_DRAW_POLYGON, n, realPoints);
    m_ops.Append(theOp);
}

void wxPseudoMetaFile::DrawSpline(int n, wxPoint pts[])
{
    wxRealPoint* realPoints = new wxRealPoint[n];
    int i;
    for (i = 0; i < n; i++)
    {
        realPoints[i].x = pts[i].x;
        realPoints[i].y = pts[i].y;
    }
    wxOpPolyDraw* theOp = new wxOpPolyDraw(DRAWOP_DRAW_SPLINE, n, realPoints);
    m_ops.Append(theOp);
}

void wxPseudoMetaFile::SetClippingRect(const wxRect& rect)
{
    wxOpSetClipping* theOp = new wxOpSetClipping(DRAWOP_SET_CLIPPING_RECT,
        (double) rect.x, (double) rect.y, (double) rect.width, (double) rect.height);
}

void wxPseudoMetaFile::DestroyClippingRect()
{
    wxOpSetClipping* theOp = new wxOpSetClipping(DRAWOP_DESTROY_CLIPPING_RECT,
        0.0, 0.0, 0.0, 0.0);

    m_ops.Append(theOp);
}

void wxPseudoMetaFile::SetPen(wxPen* pen, bool isOutline)
{
    m_gdiObjects.Append(pen);
    int n = m_gdiObjects.Number();

    wxOpSetGDI* theOp = new wxOpSetGDI(DRAWOP_SET_PEN, this, n - 1);

    m_ops.Append(theOp);

    if (isOutline)
    {
        m_outlineColours.Append((wxObject*) (n - 1));
    }
}

void wxPseudoMetaFile::SetBrush(wxBrush* brush, bool isFill)
{
    m_gdiObjects.Append(brush);
    int n = m_gdiObjects.Number();

    wxOpSetGDI* theOp = new wxOpSetGDI(DRAWOP_SET_BRUSH, this, n - 1);

    m_ops.Append(theOp);

    if (isFill)
    {
        m_fillColours.Append((wxObject*) (n - 1));
    }
}

void wxPseudoMetaFile::SetFont(wxFont* font)
{
    m_gdiObjects.Append(font);
    int n = m_gdiObjects.Number();

    wxOpSetGDI* theOp = new wxOpSetGDI(DRAWOP_SET_FONT, this, n - 1);

    m_ops.Append(theOp);
}

void wxPseudoMetaFile::SetTextColour(const wxColour& colour)
{
   wxOpSetGDI* theOp = new wxOpSetGDI(DRAWOP_SET_TEXT_COLOUR, this, 0);
   theOp->m_r = colour.Red();
   theOp->m_g = colour.Green();
   theOp->m_b = colour.Blue();

   m_ops.Append(theOp);
}

void wxPseudoMetaFile::SetBackgroundColour(const wxColour& colour)
{
   wxOpSetGDI* theOp = new wxOpSetGDI(DRAWOP_SET_BK_COLOUR, this, 0);
   theOp->m_r = colour.Red();
   theOp->m_g = colour.Green();
   theOp->m_b = colour.Blue();

   m_ops.Append(theOp);
}

void wxPseudoMetaFile::SetBackgroundMode(int mode)
{
   wxOpSetGDI* theOp = new wxOpSetGDI(DRAWOP_SET_BK_MODE, this, 0, mode);

   m_ops.Append(theOp);
}

