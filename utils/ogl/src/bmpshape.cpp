/////////////////////////////////////////////////////////////////////////////
// Name:        bmpshape.cpp
// Purpose:     Bitmap shape class
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "bmpshape.h"
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
#include "bmpshape.h"
#include "misc.h"

/*
 * Bitmap object
 *
 */

IMPLEMENT_DYNAMIC_CLASS(wxBitmapShape, wxShape)

wxBitmapShape::wxBitmapShape():wxRectangleShape(100.0, 50.0)
{
  m_filename = "";
}

wxBitmapShape::~wxBitmapShape()
{
}

void wxBitmapShape::OnDraw(wxDC& dc)
{
  if (!m_bitmap.Ok())
    return;
    
  wxMemoryDC tempDC;
  tempDC.SelectObject(m_bitmap);
  double x, y;
  x = WXROUND(m_xpos - m_bitmap.GetWidth() / 2.0);
  y = WXROUND(m_ypos - m_bitmap.GetHeight() / 2.0);
  dc.Blit(x, y, m_bitmap.GetWidth(), m_bitmap.GetHeight(), &tempDC, 0, 0);
}

void wxBitmapShape::SetSize(double w, double h, bool recursive)
{
  if (m_bitmap.Ok())
  {
    w = m_bitmap.GetWidth();
    h = m_bitmap.GetHeight();
  }

  SetAttachmentSize(w, h);

  m_width = w;
  m_height = h;
  SetDefaultRegionSize();
}

#ifdef PROLOGIO
void wxBitmapShape::WriteAttributes(wxExpr *clause)
{
  // Can't really save the bitmap; so instantiate the bitmap
  // at a higher level in the application, from a symbol library.
  wxRectangleShape::WriteAttributes(clause);
  clause->AddAttributeValueString("filename", m_filename);
}

void wxBitmapShape::ReadAttributes(wxExpr *clause)
{
  wxRectangleShape::ReadAttributes(clause);
  clause->GetAttributeValue("filename", m_filename);
}
#endif

// Does the copying for this object
void wxBitmapShape::Copy(wxShape& copy)
{
  wxRectangleShape::Copy(copy);

  wxASSERT( copy.IsKindOf(CLASSINFO(wxBitmapShape)) ) ;

  wxBitmapShape& bitmapCopy = (wxBitmapShape&) copy;

  bitmapCopy.m_bitmap = m_bitmap;
  bitmapCopy.SetFilename(m_filename);
}

void wxBitmapShape::SetBitmap(const wxBitmap& bm)
{
  m_bitmap = bm;
  if (m_bitmap.Ok())
    SetSize(m_bitmap.GetWidth(), m_bitmap.GetHeight());
}


