/////////////////////////////////////////////////////////////////////////////
// Name:        brush.h
// Purpose:     wxBrush class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BRUSH_H_
#define _WX_BRUSH_H_

#ifdef __GNUG__
#pragma interface "brush.h"
#endif

#include "wx/gdicmn.h"
#include "wx/gdiobj.h"
#include "wx/bitmap.h"

class WXDLLEXPORT wxBrush;

class WXDLLEXPORT wxBrushRefData: public wxGDIRefData
{
    friend class WXDLLEXPORT wxBrush;
public:
    wxBrushRefData(void);
    wxBrushRefData(const wxBrushRefData& data);
    ~wxBrushRefData(void);

protected:
  int           m_style;
  wxBitmap      m_stipple ;
  wxColour      m_colour;
  WXHBRUSH      m_hBrush;
};

#define M_BRUSHDATA ((wxBrushRefData *)m_refData)

// Brush
class WXDLLEXPORT wxBrush: public wxGDIObject
{
  DECLARE_DYNAMIC_CLASS(wxBrush)

public:
  wxBrush(void);
  wxBrush(const wxColour& col, int style);
  wxBrush(const wxBitmap& stipple);
  inline wxBrush(const wxBrush& brush) { Ref(brush); }
  ~wxBrush(void);

  virtual void SetColour(const wxColour& col)  ;
  virtual void SetColour(unsigned char r, unsigned char g, unsigned char b)  ;
  virtual void SetStyle(int style)  ;
  virtual void SetStipple(const wxBitmap& stipple)  ;

  inline wxBrush& operator = (const wxBrush& brush) { if (*this == brush) return (*this); Ref(brush); return *this; }
  inline bool operator == (const wxBrush& brush) { return m_refData == brush.m_refData; }
  inline bool operator != (const wxBrush& brush) { return m_refData != brush.m_refData; }

  inline wxColour& GetColour(void) const { return (M_BRUSHDATA ? M_BRUSHDATA->m_colour : wxNullColour); };
  inline int GetStyle(void) const { return (M_BRUSHDATA ? M_BRUSHDATA->m_style : 0); };
  inline wxBitmap *GetStipple(void) const { return (M_BRUSHDATA ? & M_BRUSHDATA->m_stipple : 0); };

  virtual bool Ok(void) const { return (m_refData != NULL) ; }

  // Internal
  bool RealizeResource(void);
  WXHANDLE GetResourceHandle(void) ;
  bool FreeResource(bool force = FALSE);
  bool IsFree(void);
  void Unshare();
};

#endif
    // _WX_BRUSH_H_
