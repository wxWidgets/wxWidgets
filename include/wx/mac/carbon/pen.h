/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/carbon/pen.h
// Purpose:     wxPen class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PEN_H_
#define _WX_PEN_H_

#include "wx/gdiobj.h"
#include "wx/colour.h"
#include "wx/bitmap.h"

class WXDLLIMPEXP_FWD_CORE wxPen;

// Pen
class WXDLLEXPORT wxPen: public wxGDIObject
{
  DECLARE_DYNAMIC_CLASS(wxPen)
public:
  wxPen();
  wxPen(const wxColour& col, int width = 1, int style = wxSOLID);
  wxPen(const wxBitmap& stipple, int width);
  virtual ~wxPen();

  bool operator == (const wxPen& pen) const;
  bool operator != (const wxPen& pen) const { return !(*this == pen); }

  // Override in order to recreate the pen
  void SetColour(const wxColour& col) ;
  void SetColour(unsigned char r, unsigned char g, unsigned char b) ;

  void SetWidth(int width)  ;
  void SetStyle(int style)  ;
  void SetStipple(const wxBitmap& stipple)  ;
  void SetDashes(int nb_dashes, const wxDash *dash)  ;
  void SetJoin(int join)  ;
  void SetCap(int cap)  ;

  wxColour& GetColour() const ;
  int GetWidth() const;
  int GetStyle() const;
  int GetJoin() const;
  int GetCap() const;
  int GetDashes(wxDash **ptr) const;

  wxBitmap *GetStipple() const ;

// Implementation

  // Useful helper: create the brush resource
  bool RealizeResource();

private:
    void Unshare();
};

#endif
    // _WX_PEN_H_
