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

IMPLEMENT_DYNAMIC_CLASS(wxDrawnShape, wxShape)

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
}

void wxDrawnShape::Translate(float x, float y)
{
  m_metafile.Translate(x, y);
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
  gdiIndex = theGdiIndex;
  image = theImage;
  mode = theMode;
}

void wxOpSetGDI::Do(wxDC& dc, float xoffset, float yoffset)
{
  switch (op)
  {
    case DRAWOP_SET_PEN:
    {
      // Check for overriding this operation for outline
      // colour
      if (image->m_outlineColours.Member((wxObject *)gdiIndex))
      {
        if (image->m_outlinePen)
          dc.SetPen(image->m_outlinePen);
      }
      else
      {
        wxNode *node = image->m_gdiObjects.Nth(gdiIndex);
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
      if (image->m_outlineColours.Member((wxObject *)gdiIndex))
      {
        // Need to construct a brush to match the outline pen's colour
        if (image->m_outlinePen)
        {
          wxBrush *br = wxTheBrushList->FindOrCreateBrush(image->m_outlinePen->GetColour(), wxSOLID);
          if (br)
            dc.SetBrush(br);
        }
      }
      else if (image->m_fillColours.Member((wxObject *)gdiIndex))
      {
        if (image->m_fillBrush)
        {
          dc.SetBrush(image->m_fillBrush);
        }
      }
      else
      {
        wxNode *node = image->m_gdiObjects.Nth(gdiIndex);
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
      wxNode *node = image->m_gdiObjects.Nth(gdiIndex);
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
      wxColour col(r,g,b);
      dc.SetTextForeground(col);
      break;
    }
    case DRAWOP_SET_BK_COLOUR:
    {
      wxColour col(r,g,b);
      dc.SetTextBackground(col);
      break;
    }
    case DRAWOP_SET_BK_MODE:
    {
      dc.SetBackgroundMode(mode);
      break;
    }
    default:
      break;
  }
}

wxDrawOp *wxOpSetGDI::Copy(wxPseudoMetaFile *newImage)
{
  wxOpSetGDI *newOp = new wxOpSetGDI(op, newImage, gdiIndex, mode);
  newOp->r = r;
  newOp->g = g;
  newOp->b = b;
  return newOp;
}

wxExpr *wxOpSetGDI::WritewxExpr(wxPseudoMetaFile *image)
{
  wxExpr *expr = new wxExpr(PrologList);
  expr->Append(new wxExpr((long)op));
  switch (op)
  {
    case DRAWOP_SET_PEN:
    case DRAWOP_SET_BRUSH:
    case DRAWOP_SET_FONT:
    {
      expr->Append(new wxExpr((long)gdiIndex));
      break;
    }
    case DRAWOP_SET_TEXT_COLOUR:
    case DRAWOP_SET_BK_COLOUR:
    {
      expr->Append(new wxExpr((long)r));
      expr->Append(new wxExpr((long)g));
      expr->Append(new wxExpr((long)b));
      break;
    }
    case DRAWOP_SET_BK_MODE:
    {
      expr->Append(new wxExpr((long)mode));
      break;
    }
    default:
      break;
  }
  return expr;
}

void wxOpSetGDI::ReadwxExpr(wxPseudoMetaFile *image, wxExpr *expr)
{
  switch (op)
  {
    case DRAWOP_SET_PEN:
    case DRAWOP_SET_BRUSH:
    case DRAWOP_SET_FONT:
    {
      gdiIndex = (int)expr->Nth(1)->IntegerValue();
      break;
    }
    case DRAWOP_SET_TEXT_COLOUR:
    case DRAWOP_SET_BK_COLOUR:
    {
      r = (unsigned char)expr->Nth(1)->IntegerValue();
      g = (unsigned char)expr->Nth(2)->IntegerValue();
      b = (unsigned char)expr->Nth(3)->IntegerValue();
      break;
    }
    case DRAWOP_SET_BK_MODE:
    {
      mode = (int)expr->Nth(1)->IntegerValue();
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
  x1 = theX1;
  y1 = theY1;
  x2 = theX2;
  y2 = theY2;
}

wxDrawOp *wxOpSetClipping::Copy(wxPseudoMetaFile *newImage)
{
  wxOpSetClipping *newOp = new wxOpSetClipping(op, x1, y1, x2, y2);
  return newOp;
}
    
void wxOpSetClipping::Do(wxDC& dc, float xoffset, float yoffset)
{
  switch (op)
  {
    case DRAWOP_SET_CLIPPING_RECT:
    {
      dc.SetClippingRegion(x1 + xoffset, y1 + yoffset, x2 + xoffset, y2 + yoffset);
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
  x1 *= xScale;
  y1 *= yScale;
  x2 *= xScale;
  y2 *= yScale;
}

void wxOpSetClipping::Translate(float x, float y)
{
  x1 += x;
  y1 += y;
}

wxExpr *wxOpSetClipping::WritewxExpr(wxPseudoMetaFile *image)
{
  wxExpr *expr = new wxExpr(PrologList);
  expr->Append(new wxExpr((long)op));
  switch (op)
  {
    case DRAWOP_SET_CLIPPING_RECT:
    {
      expr->Append(new wxExpr(x1));
      expr->Append(new wxExpr(y1));
      expr->Append(new wxExpr(x2));
      expr->Append(new wxExpr(y2));
      break;
    }
    default:
      break;
  }
  return expr;
}

void wxOpSetClipping::ReadwxExpr(wxPseudoMetaFile *image, wxExpr *expr)
{
  switch (op)
  {
    case DRAWOP_SET_CLIPPING_RECT:
    {
      x1 = expr->Nth(1)->RealValue();
      y1 = expr->Nth(2)->RealValue();
      x2 = expr->Nth(3)->RealValue();
      y2 = expr->Nth(4)->RealValue();
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
  x1 = theX1;
  y1 = theY1;
  x2 = theX2;
  y2 = theY2;
  radius = theRadius;
  if (s) textString = copystring(s);
  else textString = NULL;
}

wxOpDraw::~wxOpDraw()
{
  if (textString) delete[] textString;
}

wxDrawOp *wxOpDraw::Copy(wxPseudoMetaFile *newImage)
{
  wxOpDraw *newOp = new wxOpDraw(op, x1, y1, x2, y2, radius, textString);
  return newOp;
}

void wxOpDraw::Do(wxDC& dc, float xoffset, float yoffset)
{
  switch (op)
  {
    case DRAWOP_DRAW_LINE:
    {
      dc.DrawLine(x1+xoffset, y1+yoffset, x2+xoffset, y2+yoffset);
      break;
    }
    case DRAWOP_DRAW_RECT:
    {
      dc.DrawRectangle(x1+xoffset, y1+yoffset, x2+xoffset, y2+yoffset);
      break;
    }
    case DRAWOP_DRAW_ROUNDED_RECT:
    {
      dc.DrawRoundedRectangle(x1+xoffset, y1+yoffset, x2+xoffset, y2+yoffset, radius);
      break;
    }
    case DRAWOP_DRAW_ELLIPSE:
    {
      dc.DrawEllipse(x1+xoffset, y1+yoffset, x2+xoffset, y2+yoffset);
      break;
    }
    case DRAWOP_DRAW_POINT:
    {
      dc.DrawPoint(x1+xoffset, y1+yoffset);
      break;
    }
    case DRAWOP_DRAW_TEXT:
    {
      dc.DrawText(textString, x1+xoffset, y1+yoffset);
      break;
    }
    default:
      break;
  }
}

void wxOpDraw::Scale(float scaleX, float scaleY)
{
  x1 *= scaleX;
  y1 *= scaleY;
  x2 *= scaleX;
  y2 *= scaleY;
  radius *= scaleX;
}

void wxOpDraw::Translate(float x, float y)
{
  x1 += x;
  y1 += y;
}

void wxOpDraw::Rotate(float x, float y, float sinTheta, float cosTheta)
{
  x1 = x1*cosTheta - y1*sinTheta + x*(1 - cosTheta) + y*sinTheta;
  y1 = x1*sinTheta + y1*cosTheta + y*(1 - cosTheta) + x*sinTheta;
  
  switch (op)
  {
    case DRAWOP_DRAW_LINE:
    {
      x2 = x2*cosTheta - y2*sinTheta + x*(1 - cosTheta) + y*sinTheta;
      y2 = x2*sinTheta + y2*cosTheta + y*(1 - cosTheta) + x*sinTheta;
      break;
    }
    default:
      break;
  }
}

wxExpr *wxOpDraw::WritewxExpr(wxPseudoMetaFile *image)
{
  wxExpr *expr = new wxExpr(PrologList);
  expr->Append(new wxExpr((long)op));
  switch (op)
  {
    case DRAWOP_DRAW_LINE:
    case DRAWOP_DRAW_RECT:
    case DRAWOP_DRAW_ELLIPSE:
    {
      expr->Append(new wxExpr(x1));
      expr->Append(new wxExpr(y1));
      expr->Append(new wxExpr(x2));
      expr->Append(new wxExpr(y2));
      break;
    }
    case DRAWOP_DRAW_ROUNDED_RECT:
    {
      expr->Append(new wxExpr(x1));
      expr->Append(new wxExpr(y1));
      expr->Append(new wxExpr(x2));
      expr->Append(new wxExpr(y2));
      expr->Append(new wxExpr(radius));
      break;
    }
    case DRAWOP_DRAW_POINT:
    {
      expr->Append(new wxExpr(x1));
      expr->Append(new wxExpr(y1));
      break;
    }
    case DRAWOP_DRAW_TEXT:
    {
      expr->Append(new wxExpr(x1));
      expr->Append(new wxExpr(y1));
      expr->Append(new wxExpr(PrologString, textString));
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

void wxOpDraw::ReadwxExpr(wxPseudoMetaFile *image, wxExpr *expr)
{
  switch (op)
  {
    case DRAWOP_DRAW_LINE:
    case DRAWOP_DRAW_RECT:
    case DRAWOP_DRAW_ELLIPSE:
    {
      x1 = expr->Nth(1)->RealValue();
      y1 = expr->Nth(2)->RealValue();
      x2 = expr->Nth(3)->RealValue();
      y2 = expr->Nth(4)->RealValue();
      break;
    }
    case DRAWOP_DRAW_ROUNDED_RECT:
    {
      x1 = expr->Nth(1)->RealValue();
      y1 = expr->Nth(2)->RealValue();
      x2 = expr->Nth(3)->RealValue();
      y2 = expr->Nth(4)->RealValue();
      radius = expr->Nth(5)->RealValue();
      break;
    }
    case DRAWOP_DRAW_POINT:
    {
      x1 = expr->Nth(1)->RealValue();
      y1 = expr->Nth(2)->RealValue();
      break;
    }
    case DRAWOP_DRAW_TEXT:
    {
      x1 = expr->Nth(1)->RealValue();
      y1 = expr->Nth(2)->RealValue();
      textString = copystring(expr->Nth(3)->StringValue());
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
  noPoints = n;
  points = thePoints;
}

wxOpPolyDraw::~wxOpPolyDraw()
{
  delete[] points;
}

wxDrawOp *wxOpPolyDraw::Copy(wxPseudoMetaFile *newImage)
{
  wxRealPoint *newPoints = new wxRealPoint[noPoints];
  for (int i = 0; i < noPoints; i++)
  {
    newPoints[i].x = points[i].x;
    newPoints[i].y = points[i].y;
  }
  wxOpPolyDraw *newOp = new wxOpPolyDraw(op, noPoints, newPoints);
  return newOp;
}

void wxOpPolyDraw::Do(wxDC& dc, float xoffset, float yoffset)
{
  switch (op)
  {
    case DRAWOP_DRAW_POLYLINE:
    {
        wxPoint *actualPoints = new wxPoint[noPoints];
        int i;
        for (i = 0; i < noPoints; i++)
        {
            actualPoints[i].x = (long) points[i].x;
            actualPoints[i].y = (long) points[i].y;
        }

        dc.DrawLines(noPoints, actualPoints, xoffset, yoffset);

        delete[] actualPoints;
        break;
    }
    case DRAWOP_DRAW_POLYGON:
    {
        wxPoint *actualPoints = new wxPoint[noPoints];
        int i;
        for (i = 0; i < noPoints; i++)
        {
            actualPoints[i].x = (long) points[i].x;
            actualPoints[i].y = (long) points[i].y;
        }

        dc.DrawPolygon(noPoints, actualPoints, xoffset, yoffset);

        delete[] actualPoints;
        break;
    }
    case DRAWOP_DRAW_SPLINE:
    {
//      dc.DrawSpline(noPoints, points, xoffset, yoffset);
      break;
    }
    default:
      break;
  }
}

void wxOpPolyDraw::Scale(float scaleX, float scaleY)
{
  for (int i = 0; i < noPoints; i++)
  {
    points[i].x *= scaleX;
    points[i].y *= scaleY;
  }
}

void wxOpPolyDraw::Translate(float x, float y)
{
  for (int i = 0; i < noPoints; i++)
  {
    points[i].x += x;
    points[i].y += y;
  }
}

void wxOpPolyDraw::Rotate(float x, float y, float sinTheta, float cosTheta)
{
  for (int i = 0; i < noPoints; i++)
  {
    float x1 = points[i].x;
    float y1 = points[i].y;
    points[i].x = x1*cosTheta - y1*sinTheta + x*(1 - cosTheta) + y*sinTheta;
    points[i].y = x1*sinTheta + y1*cosTheta + y*(1 - cosTheta) + x*sinTheta;
  }
}

wxExpr *wxOpPolyDraw::WritewxExpr(wxPseudoMetaFile *image)
{
  wxExpr *expr = new wxExpr(PrologList);
  expr->Append(new wxExpr((long)op));
  expr->Append(new wxExpr((long)noPoints));

//  char buf1[9];
  char buf2[5];
  char buf3[5];

  GraphicsBuffer[0] = 0;
  
  /*
   * Store each coordinate pair in a hex string to save space.
   * E.g. "1B9080CD". 4 hex digits per coordinate pair.
   *
   */
   
  for (int i = 0; i < noPoints; i++)
  {
    long signedX = (long)(points[i].x*100.0);
    long signedY = (long)(points[i].y*100.0);

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

void wxOpPolyDraw::ReadwxExpr(wxPseudoMetaFile *image, wxExpr *expr)
{
  noPoints = (int)expr->Nth(1)->IntegerValue();

  char buf1[5];
  char buf2[5];

  points = new wxRealPoint[noPoints];
  int i = 0;
  int bufPtr = 0;
  wxString hexString = expr->Nth(2)->StringValue();
  while (i < noPoints)
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

    points[i].x = (float)(signedX / 100.0);
    points[i].y = (float)(signedY / 100.0);

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
    wxExpr *expr = op->WritewxExpr(this);
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
          theOp->ReadwxExpr(this, expr);
          m_ops.Append(theOp);
          break;
        }
        
        case DRAWOP_SET_CLIPPING_RECT:
        case DRAWOP_DESTROY_CLIPPING_RECT:
        {
          wxOpSetClipping *theOp = new wxOpSetClipping(opId, 0.0, 0.0, 0.0, 0.0);
          theOp->ReadwxExpr(this, expr);
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
          theOp->ReadwxExpr(this, expr);
          m_ops.Append(theOp);
          break;
        }
        case DRAWOP_DRAW_SPLINE:
        case DRAWOP_DRAW_POLYLINE:
        case DRAWOP_DRAW_POLYGON:
        {
          wxOpPolyDraw *theOp = new wxOpPolyDraw(opId, 0, NULL);
          theOp->ReadwxExpr(this, expr);
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
  copy.m_outlineColours.Clear();
  node = m_outlineColours.First();
  while (node)
  {
    copy.m_outlineColours.Append((wxObject *)node->Data());
    node = node->Next();
  }
  copy.m_fillColours.Clear();
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
        op->r = (unsigned char)record->param1;
        op->g = (unsigned char)record->param2;
        op->b = (unsigned char)record->param3;
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
        op->r = (unsigned char)record->param1;
        op->g = (unsigned char)record->param2;
        op->b = (unsigned char)record->param3;
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
        rec->param4 = getshort(handle); // y2
        rec->param3 = getshort(handle); // x2
        rec->param2 = getshort(handle); // y1
        rec->param1 = getshort(handle); // x1
*/
        break;
      }
      case META_INTERSECTCLIPRECT:
      {
/*
        rec->param4 = getshort(handle); // y2
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
    switch (op->op)
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
        if (opDraw->x1 < minX) minX = opDraw->x1;
        if (opDraw->x1 > maxX) maxX = opDraw->x1;
        if (opDraw->y1 < minY) minY = opDraw->y1;
        if (opDraw->y1 > maxY) maxY = opDraw->y1;
        if (op->op == DRAWOP_DRAW_LINE)
        {
          if (opDraw->x2 < minX) minX = opDraw->x2;
          if (opDraw->x2 > maxX) maxX = opDraw->x2;
          if (opDraw->y2 < minY) minY = opDraw->y2;
          if (opDraw->y2 > maxY) maxY = opDraw->y2;
        }
        else if (op->op == DRAWOP_DRAW_RECT ||
                 op->op == DRAWOP_DRAW_ROUNDED_RECT ||
                 op->op == DRAWOP_DRAW_ELLIPSE)
        {
          if ((opDraw->x1 + opDraw->x2) < minX) minX = (opDraw->x1 + opDraw->x2);
          if ((opDraw->x1 + opDraw->x2) > maxX) maxX = (opDraw->x1 + opDraw->x2);
          if ((opDraw->y1 + opDraw->y2) < minY) minY = (opDraw->y1 + opDraw->y2);
          if ((opDraw->y1 + opDraw->y2) > maxY) maxY = (opDraw->y1 + opDraw->y2);
        }
        break;
      }
      case DRAWOP_DRAW_POLYLINE:
      case DRAWOP_DRAW_POLYGON:
      case DRAWOP_DRAW_SPLINE:
      {
        wxOpPolyDraw *poly = (wxOpPolyDraw *)op;
        for (int i = 0; i < poly->noPoints; i++)
        {
          if (poly->points[i].x < minX) minX = poly->points[i].x;
          if (poly->points[i].x > maxX) maxX = poly->points[i].x;
          if (poly->points[i].y < minY) minY = poly->points[i].y;
          if (poly->points[i].y > maxY) maxY = poly->points[i].y;
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


