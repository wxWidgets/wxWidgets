/////////////////////////////////////////////////////////////////////////////
// Name:        bitmap.h
// Purpose:     wxBitmapShape
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _OGL_BITMAP_H_
#define _OGL_BITMAP_H_

#ifdef __GNUG__
#pragma interface "bitmap.h"
#endif

#include "basic.h"

class wxBitmapShape: public wxRectangleShape
{
 DECLARE_DYNAMIC_CLASS(wxBitmapShape)
 public:
  wxBitmapShape();
  ~wxBitmapShape();

  void OnDraw(wxDC& dc);

#ifdef PROLOGIO
  // Prolog database stuff
  char *GetFunctor();
  void WritePrologAttributes(wxExpr *clause);
  void ReadPrologAttributes(wxExpr *clause);
#endif

  // Does the copying for this object
  void Copy(wxBitmapShape& copy);

  // Returns a new instance, and does the copy for this class. Define for each class.
  wxShape *PrivateCopy();

  void SetSize(float w, float h, bool recursive = TRUE);
  inline wxBitmap& GetBitmap() const { return (wxBitmap&) m_bitmap; }
  void SetBitmap(const wxBitmap& bm);
  inline void SetFilename(const wxString& f) { m_filename = f; };
  inline wxString GetFilename() const { return m_filename; }

private:
  wxBitmap      m_bitmap;
  wxString      m_filename;
};

#endif
  // _OGL_BITMAP_H_


