/////////////////////////////////////////////////////////////////////////////
// Name:        pen.h
// Purpose:     wxPen class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __PENH__
#define __PENH__

#ifdef __GNUG__
#pragma interface "pen.h"
#endif

#include "wx/gdiobj.h"

typedef    WXDWORD  wxDash ;

class WXDLLEXPORT wxPen;

class WXDLLEXPORT wxPenRefData: public wxGDIRefData
{
    friend class WXDLLEXPORT wxPen;
public:
    wxPenRefData(void);
    ~wxPenRefData(void);

protected:
  int           m_width;
  int           m_style;
  int           m_join ;
  int           m_cap ;
  wxBitmap      m_stipple ;
  int           m_nbDash ;
  wxDash *      m_dash ;
  wxColour      m_colour;
  WXHPEN        m_hPen;
};

#define M_PENDATA ((wxPenRefData *)m_refData)

// Pen
class WXDLLEXPORT wxPen: public wxGDIObject
{
  DECLARE_DYNAMIC_CLASS(wxPen)
public:
  wxPen(void);
  wxPen(const wxColour& col, const int width, const int style);
  wxPen(const wxString& col, const int width, const int style);
  wxPen(const wxBitmap& stipple, const int width);
  inline wxPen(const wxPen& pen) { Ref(pen); }
  inline wxPen(const wxPen* pen) { /* UnRef(); */ if (pen) Ref(*pen); }
  ~wxPen(void);

  inline wxPen& operator = (const wxPen& pen) { if (*this == pen) return (*this); Ref(pen); return *this; }
  inline bool operator == (const wxPen& pen) { return m_refData == pen.m_refData; }
  inline bool operator != (const wxPen& pen) { return m_refData != pen.m_refData; }

  virtual bool Ok(void) const { return (m_refData != NULL) ; }

  // Override in order to recreate the pen
  void SetColour(const wxColour& col) ;
  void SetColour(const wxString& col)  ;
  void SetColour(const unsigned char r, const unsigned char g, const unsigned char b)  ;

  void SetWidth(const int width)  ;
  void SetStyle(const int style)  ;
  void SetStipple(const wxBitmap& stipple)  ;
  void SetDashes(const int nb_dashes, const wxDash *dash)  ;
  void SetJoin(const int join)  ;
  void SetCap(const int cap)  ;

  inline wxColour& GetColour(void) const { return (M_PENDATA ? M_PENDATA->m_colour : wxNullColour); };
  inline int GetWidth(void) const { return (M_PENDATA ? M_PENDATA->m_width : 0); };
  inline int GetStyle(void) const { return (M_PENDATA ? M_PENDATA->m_style : 0); };
  inline int GetJoin(void) const { return (M_PENDATA ? M_PENDATA->m_join : 0); };
  inline int GetCap(void) const { return (M_PENDATA ? M_PENDATA->m_cap : 0); };
  inline int GetDashes(wxDash **ptr) const {
     *ptr = (M_PENDATA ? M_PENDATA->m_dash : NULL); return (M_PENDATA ? M_PENDATA->m_nbDash : 0);
  }

  inline wxBitmap *GetStipple(void) const { return (M_PENDATA ? (& M_PENDATA->m_stipple) : NULL); };

  // Internal
  bool RealizeResource(void);
  bool FreeResource(bool force = FALSE);
  WXHANDLE GetResourceHandle(void) ;
  bool IsFree(void);
};

int wx2msPenStyle(int wx_style);

#endif
    // __PENH__
